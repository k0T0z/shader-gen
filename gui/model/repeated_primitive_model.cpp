#include "gui/model/repeated_primitive_model.hpp"

#include "gui/model/message_model.hpp"
#include "error_macros.hpp"

RepeatedPrimitiveModel::RepeatedPrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : PrimitiveModel(message_buffer, field_desc, parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {}

void RepeatedPrimitiveModel::build_sub_models() {
    CHECK_CONDITION_TRUE(!m_field_desc->is_repeated(), "Field " + m_field_desc->full_name() + " is not repeated.");
    CHECK_CONDITION_TRUE(m_field_desc->message_type() != nullptr, "Field " + m_field_desc->full_name() + " is not a primitive field.");

    int size {rowCount()};
    for (int i {0}; i < size; i++) {
        append_row(i);
    }
}

QVariant RepeatedPrimitiveModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from RepeatedPrimitiveModel.");
}

bool RepeatedPrimitiveModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a RepeatedPrimitiveModel.");
}

PrimitiveModel* RepeatedPrimitiveModel::get_sub_model(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr, 
        "Accessing out-of-range proto row " + std::to_string(index) + " of " + std::to_string(rowCount()));

    PrimitiveModel* m = dynamic_cast<PrimitiveModel*>(m_sub_models.at(index));
    CHECK_PARAM_NULLPTR_NON_VOID(m, nullptr, "Failed to cast sub model to PrimitiveModel.");

    return m;
}

const ProtoModel* RepeatedPrimitiveModel::get_sub_model(const FieldPath& path) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(path.is_empty(), this);
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + m_field_desc->full_name());

    int index {-1};

    CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr, "Next component is not a repeated index.");
    VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr, "Index out of range.");

    CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr, "Failed to skip repeated index.");

    return m_sub_models.at(index)->get_sub_model(path);
}

QModelIndex RepeatedPrimitiveModel::index(int row, int column, [[maybe_unused]] const QModelIndex& parent) const {
    Q_UNUSED(parent);
    Q_UNUSED(column);
    SILENT_VALIDATE_INDEX_NON_VOID(row, rowCount(), QModelIndex());
    return createIndex(row, column);
}

QModelIndex RepeatedPrimitiveModel::parent([[maybe_unused]] const QModelIndex& child) const {
    Q_UNUSED(child);
    const ProtoModel* parent_model {get_parent_model()};

    const ProtoModel* root_model {get_root_model()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_model == root_model, QModelIndex());

    MessageModel* parent_message_model {dynamic_cast<MessageModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_message_model != nullptr, createIndex(0, parent_model->get_index_in_parent()));

    return QModelIndex();
}

int RepeatedPrimitiveModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedPrimitiveModel::data(const QModelIndex& index, int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), QVariant(), 
        "Accessing out-of-range proto row " + std::to_string(index.row()) + " of " + std::to_string(rowCount()));
    CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, QVariant(), "A primitive model should have only one column.");

    return get_sub_model(index.row())->data(this->index(0, 0, index), role);
}

bool RepeatedPrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), false, 
        "Accessing out-of-range proto row " + std::to_string(index.row()) + " of " + std::to_string(rowCount()));
    CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, false, "A primitive model should have only one column.");

    return get_sub_model(index.row())->setData(this->index(0, 0, index), value, role);
}

int RepeatedPrimitiveModel::append_row() {
    int row {rowCount()};
    QModelIndex index {this->index(row, -1)};
    // Q_EMIT rowsAboutToBeInserted(index, row, row, {});
    beginInsertRows(index, row, row);

    bool result {insertRows(row, 1, index)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, -1);

    parent_data_changed();

    endInsertRows();
    // Q_EMIT rowsInserted(index, row, row, {});

    return row;
}

bool RepeatedPrimitiveModel::remove_row(const int& row) {
    VALIDATE_INDEX_NON_VOID(row, rowCount(), false, "Index out of range.");

    QModelIndex index {this->index(row, -1)};
    // Q_EMIT rowsAboutToBeRemoved(index, row, row, {});
    beginRemoveRows(index, row, row);

    bool result {removeRows(row, 1, index)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, false);

    parent_data_changed();

    endRemoveRows();
    // Q_EMIT rowsRemoved(index, row, row, {});

    return true;
}

int RepeatedPrimitiveModel::field_to_column(const int& fn) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};

    CHECK_PARAM_NULLPTR_NON_VOID(field, -1, "Field not found.");

    return field->index();
}

bool RepeatedPrimitiveModel::insertRows(int row, int count, const QModelIndex &parent) {
    CHECK_CONDITION_TRUE_NON_VOID(!parent.isValid(), false, "Parent is not valid.");
    VALIDATE_INDEX_NON_VOID(row, rowCount(), false, "Index out of range.");
    CHECK_CONDITION_TRUE_NON_VOID(count == 1, false, "Invalid number of rows: " + std::to_string(count) + ". Adding multiple rows at once is not supported yet.");

    const Reflection* refl {m_message_buffer->GetReflection()};
    
    switch (m_field_desc->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
            FAIL_AND_RETURN_NON_VOID(-1, "Trying to append a message field.");
            break;
        case FieldDescriptor::CppType::CPPTYPE_INT32: refl->AddInt32(m_message_buffer, m_field_desc, 0); break;
        case FieldDescriptor::CppType::CPPTYPE_INT64: refl->AddInt64(m_message_buffer, m_field_desc, 0); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT32: refl->AddUInt32(m_message_buffer, m_field_desc, 0); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT64: refl->AddUInt64(m_message_buffer, m_field_desc, 0); break;
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: refl->AddDouble(m_message_buffer, m_field_desc, 0.0); break;
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: refl->AddFloat(m_message_buffer, m_field_desc, 0.0f); break;
        case FieldDescriptor::CppType::CPPTYPE_BOOL: refl->AddBool(m_message_buffer, m_field_desc, false); break;
        case FieldDescriptor::CppType::CPPTYPE_STRING: refl->AddString(m_message_buffer, m_field_desc, ""); break;
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
            break;
    }

    m_sub_models.emplace_back(new PrimitiveModel(m_message_buffer, m_field_desc, this, row));

    return true;
}

bool RepeatedPrimitiveModel::removeRows(int row, int count, const QModelIndex &parent) {
    CHECK_CONDITION_TRUE_NON_VOID(!parent.isValid(), false, "Parent is not valid.");
    VALIDATE_INDEX_NON_VOID(row, rowCount(), false, "Index out of range.");
    CHECK_CONDITION_TRUE_NON_VOID(count == 1, false, "Invalid number of rows: " + std::to_string(count) + ". Adding multiple rows at once is not supported yet.");

    delete m_sub_models.at(row);
    m_sub_models.erase(m_sub_models.begin() + row);

    // We remove a row by swapping the last row with the row to be removed and then removing the last row.
    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection.RemoveLast.details
    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->SwapElements(m_message_buffer, m_field_desc, row, rowCount() - 1);
    refl->RemoveLast(m_message_buffer, m_field_desc);

    return true;
}

void RepeatedPrimitiveModel::clear_sub_models() {
    int size {rowCount()};

    for (int i = 0; i < size; ++i) delete m_sub_models.at(i);

    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->ClearField(m_message_buffer, m_field_desc);
    m_sub_models.clear();
}

void RepeatedPrimitiveModel::append_row(const int& row) {
    CHECK_CONDITION_TRUE(m_field_desc->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE, "Field is not a message.");

    QModelIndex index {this->index(row, -1)};
    // Q_EMIT rowsAboutToBeInserted(index, row, row, {});
    beginInsertRows(index, row, row);

    m_sub_models.emplace_back(new PrimitiveModel(m_message_buffer, m_field_desc, this, row));

    bool result {insertRows(row, 1, index)};
    CHECK_CONDITION_TRUE(!result, "Failed to insert row.");

    parent_data_changed();

    endInsertRows();
    // Q_EMIT rowsInserted(index, row, row, {});
}
