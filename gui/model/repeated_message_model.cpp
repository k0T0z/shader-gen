#include "gui/model/repeated_message_model.hpp"

#include "gui/model/utils/utils.hpp"
#include "error_macros.hpp"

using Message = google::protobuf::Message;
using OneofDescriptor = google::protobuf::OneofDescriptor;

RepeatedMessageModel::RepeatedMessageModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : MessageModel(message_buffer, parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {}

void RepeatedMessageModel::build_sub_models() {
    CHECK_CONDITION_TRUE(!m_field_desc->is_repeated(), "Field " + m_field_desc->full_name() + " is not repeated.");
    CHECK_PARAM_NULLPTR(m_field_desc->message_type(), "Field does not have a message type.");

    int size {rowCount()};
    for (int i {0}; i < size; i++) {
        append_row(i);
    }
}

QVariant RepeatedMessageModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from RepeatedMessageModel.");
}

bool RepeatedMessageModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a RepeatedMessageModel.");
}

MessageModel* RepeatedMessageModel::get_sub_model(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr, 
        "Accessing out-of-range proto row " + std::to_string(index) + " of " + std::to_string(rowCount()));

    MessageModel* m = dynamic_cast<MessageModel*>(m_sub_models.at(index));
    CHECK_PARAM_NULLPTR_NON_VOID(m, nullptr, "Failed to cast sub model to MessageModel.");

    return m;
}

const ProtoModel* RepeatedMessageModel::get_sub_model(const FieldPath& path) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(path.is_empty(), this);
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + m_field_desc->full_name());

    int index {-1};

    CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr, "Next component is not a repeated index.");
    VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr, "Index out of range.");

    CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr, "Failed to skip repeated index.");

    return m_sub_models.at(index)->get_sub_model(path);
}

QModelIndex RepeatedMessageModel::index(int row, int column, [[maybe_unused]] const QModelIndex& parent) const {
    Q_UNUSED(parent);
    Q_UNUSED(column);
    SILENT_VALIDATE_INDEX_NON_VOID(row, rowCount(), QModelIndex());
    return createIndex(row, column);
}

QModelIndex RepeatedMessageModel::parent([[maybe_unused]] const QModelIndex& child) const {
    Q_UNUSED(child);
    const ProtoModel* parent_model {get_parent_model()};

    const ProtoModel* root_model {get_root_model()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_model == root_model, QModelIndex());

    MessageModel* parent_message_model {dynamic_cast<MessageModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_message_model != nullptr, createIndex(0, parent_model->get_index_in_parent()));

    return QModelIndex();
}

int RepeatedMessageModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedMessageModel::data(const QModelIndex& index, int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), QVariant(), 
        "Accessing out-of-range proto row " + std::to_string(index.row()) + " of " + std::to_string(rowCount()));
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    return get_sub_model(index.row())->data(this->index(0, index.column(), index), role);
}

bool RepeatedMessageModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), false, 
        "Accessing out-of-range proto row " + std::to_string(index.row()) + " of " + std::to_string(rowCount()));
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), false, 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    return get_sub_model(index.row())->setData(this->index(0, index.column(), index), value, role);
}

int RepeatedMessageModel::append_row() {
    CHECK_CONDITION_TRUE_NON_VOID(m_field_desc->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE, -1, "Field is not a message.");

    int row {rowCount()};
    // Q_EMIT rowsAboutToBeInserted(QModelIndex(), row, row, {});
    beginInsertRows(QModelIndex(), row, row);

    const Reflection* refl {m_message_buffer->GetReflection()};
    Message* new_buffer {refl->AddMessage(m_message_buffer, m_field_desc)};
    m_sub_models.emplace_back(new MessageModel(new_buffer, this, row));

    bool result {insertRows(row, 1)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, -1);

    parent_data_changed();

    endInsertRows();
    // Q_EMIT rowsInserted(QModelIndex(), row, row, {});

    return row;
}

bool RepeatedMessageModel::remove_row(const int& row) {
    SILENT_VALIDATE_INDEX_NON_VOID(row, rowCount(), false);

    // Q_EMIT rowsAboutToBeRemoved(QModelIndex(), row, row, {});
    beginRemoveRows(QModelIndex(), row, row);

    // We remove a row by swapping the last row with the row to be removed and then removing the last row.
    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection.RemoveLast.details
    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->SwapElements(m_message_buffer, m_field_desc, row, rowCount() - 1);
    refl->RemoveLast(m_message_buffer, m_field_desc);

    parent_data_changed();

    endRemoveRows();
    // Q_EMIT rowsRemoved(QModelIndex(), row, row, {});

    return true;
}

int RepeatedMessageModel::field_to_column(const int& fn) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};

    CHECK_PARAM_NULLPTR_NON_VOID(field, -1, "Field not found.");

    return field->index();
}

bool RepeatedMessageModel::insertRows(int row, [[maybe_unused]] int count, const QModelIndex &parent) {
    return insertRow(row, parent);
}

bool RepeatedMessageModel::removeRows(int row, [[maybe_unused]] int count, const QModelIndex &parent) {
    return removeRow(row, parent);
}

void RepeatedMessageModel::clear_sub_models() {
    int size {rowCount()};

    for (int i = 0; i < size; ++i) delete m_sub_models.at(i);

    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->ClearField(m_message_buffer, m_field_desc);
    m_sub_models.clear();
}

void RepeatedMessageModel::append_row(const int& row) {
    CHECK_CONDITION_TRUE(m_field_desc->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE, "Field is not a message.");

    // Q_EMIT rowsAboutToBeInserted(QModelIndex(), row, row, {});
    beginInsertRows(QModelIndex(), row, row);

    const Reflection* refl {m_message_buffer->GetReflection()};
    m_sub_models.emplace_back(new MessageModel(refl->MutableRepeatedMessage(m_message_buffer, m_field_desc, row), this, row));

    bool result {insertRows(row, 1)};
    CHECK_CONDITION_TRUE(!result, "Failed to insert row.");

    parent_data_changed();

    endInsertRows();
    // Q_EMIT rowsInserted(QModelIndex(), row, row, {});
}
