#include "gui/model/repeated_message_model.hpp"

#include "error_macros.hpp"

RepeatedMessageModel::RepeatedMessageModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : MessageModel(message_buffer, parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {
    build_sub_models();
}

void RepeatedMessageModel::build_sub_models() {
    const Reflection* refl {m_message_buffer->GetReflection()};
    int size {rowCount()};
    for (int i = 0; i < size; i++) {
        m_sub_models.emplace_back(new MessageModel(refl->MutableRepeatedMessage(m_message_buffer, m_field_desc, i), this, i));
    }
}

QVariant RepeatedMessageModel::data() const {
    std::vector<QVariant> data;
    int size {(int)m_sub_models.size()};
    for (int i = 0; i < size; ++i) {
        data.emplace_back(m_sub_models.at(i)->data());
    }
    return QVariant::fromValue(data);
}

bool RepeatedMessageModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Method not implemented.");
}

const ProtoModel* RepeatedMessageModel::get_sub_model(const FieldPath& path) const {
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "");

    int index {-1};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr);
    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr); // Skip the repeated index

    return m_sub_models.at(index)->get_sub_model(path);
}

MessageModel* RepeatedMessageModel::get_sub_model(const int& index) const {
    // const Reflection* refl {m_message_buffer->GetReflection()};

    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    return m_sub_models.at(index);
}

const FieldDescriptor* RepeatedMessageModel::get_column_descriptor([[maybe_unused]] const int& column) const {
    return nullptr;
}

int RepeatedMessageModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedMessageModel::data(const QModelIndex& index, int role) const {
    VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), QVariant(), 
        "Row index " + std::to_string(index.row()) + " is out of bounds (" + std::to_string(rowCount()) + " rows total)");
    return m_sub_models.at(index.row())->data(m_sub_models.at(index.row())->index(index.column(), role));
}

bool RepeatedMessageModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a repeated message model.");
}

bool RepeatedMessageModel::insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) {
    return insertRow(row, parent);
}

bool RepeatedMessageModel::removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) {
    return removeRow(row, parent);
}

// bool RepeatedMessageModel::insertRow(int row, const QModelIndex &parent) {
//     SILENT_CHECK_CONDITION_TRUE_NON_VOID(row <= rowCount(), false);

//     beginInsertRows(parent, row, row);

//     const Reflection* refl {m_message_buffer->GetReflection()};
//     Message* new_buffer {refl->AddMessage(m_message_buffer, m_field_desc)};
//     m_sub_models.emplace_back(new MessageModel(new_buffer, this, rowCount()));

//     parent_data_changed();

//     endInsertRows();

//     return false;
// }

int RepeatedMessageModel::field_to_column(const int& fn) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, -1);

    return field->index();
}

void RepeatedMessageModel::clear_sub_models() {
    int size {(int)m_sub_models.size()};
    for (int i = 0; i < size; ++i) {
        MessageModel* m {m_sub_models.at(i)};
        delete m;
    }

    m_sub_models.clear();
}
