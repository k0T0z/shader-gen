#include "gui/model/repeated_message_model.hpp"

#include "error_macros.hpp"

RepeatedMessageModel::RepeatedMessageModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : MessageModel(message_buffer, parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {
    build_sub_models();
}

void RepeatedMessageModel::build_sub_models() {
    CHECK_CONDITION_TRUE(!m_field_desc->is_repeated(), "Field is not repeated.");
    CHECK_PARAM_NULLPTR(m_field_desc->message_type(), "Field does not have a message type.");

    const Reflection* refl {m_message_buffer->GetReflection()};
    int size {rowCount()};
    for (int i = 0; i < size; i++) {
        m_sub_models.emplace_back(new MessageModel(refl->MutableRepeatedMessage(m_message_buffer, m_field_desc, i), this, i));
    }
}

QVariant RepeatedMessageModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from RepeatedMessageModel.");
}

bool RepeatedMessageModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data to RepeatedMessageModel.");
}

const ProtoModel* RepeatedMessageModel::get_sub_model(const FieldPath& path) const {
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "");

    int index {-1};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr);
    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr); // Skip the repeated index

    return const_cast<const ProtoModel*>(m_sub_models.at(index)->get_sub_model(path));
}

MessageModel* RepeatedMessageModel::get_sub_model(const int& index) const {
    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    MessageModel* m = dynamic_cast<MessageModel*>(m_sub_models.at(index));
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m, nullptr);

    return m;
}

const FieldDescriptor* RepeatedMessageModel::get_column_descriptor([[maybe_unused]] const int& column) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    VALIDATE_INDEX_NON_VOID(column, columnCount(), nullptr, 
        "Requesting descriptor of invalid column (field index) " + std::to_string(column) + " of MessageModel " + desc->full_name());
    return desc->field(column);
}

int RepeatedMessageModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedMessageModel::data(const QModelIndex& index, int role) const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from a repeated message model.");
}

bool RepeatedMessageModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a repeated message model.");
}

int RepeatedMessageModel::append_row() {
    int row {rowCount()};
    beginInsertRows(QModelIndex(), row, row);

    const Reflection* refl {m_message_buffer->GetReflection()};
    Message* new_buffer {refl->AddMessage(m_message_buffer, m_field_desc)};
    m_sub_models.emplace_back(new MessageModel(new_buffer, this, row));

    bool result {insertRows(row, 1)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, -1);

    parent_data_changed();

    endInsertRows();

    return row;
}

bool RepeatedMessageModel::remove_row(const int& row) {
    SILENT_VALIDATE_INDEX_NON_VOID(row, rowCount(), false);

    beginRemoveRows(QModelIndex(), row, row);

    // We remove a row by swapping the last row with the row to be removed and then removing the last row.
    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection.RemoveLast.details
    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->SwapElements(m_message_buffer, m_field_desc, row, rowCount() - 1);
    refl->RemoveLast(m_message_buffer, m_field_desc);

    parent_data_changed();

    endRemoveRows();

    return true;
}

int RepeatedMessageModel::field_to_column(const int& fn) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, -1);

    return field->index();
}

bool RepeatedMessageModel::insertRows(int row, int count, const QModelIndex &parent) {
    return insertRow(row, parent);
}

bool RepeatedMessageModel::removeRows(int row, int count, const QModelIndex &parent) {
    return removeRow(row, parent);
}

void RepeatedMessageModel::clear_sub_models() {
    int size {rowCount()};

    for (int i = 0; i < size; ++i) {
        MessageModel* m {m_sub_models.at(i)};
        delete m;
    }

    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->ClearField(m_message_buffer, m_field_desc);
    m_sub_models.clear();
}
