#include "gui/model/repeated_primitive_model.hpp"

#include "error_macros.hpp"

RepeatedPrimitiveModel::RepeatedPrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : PrimitiveModel(message_buffer, m_field_desc, parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {
    build_sub_models();
}

void RepeatedPrimitiveModel::build_sub_models() {
    CHECK_CONDITION_TRUE(!m_field_desc->is_repeated(), "Field is not repeated.");
    CHECK_CONDITION_TRUE(m_field_desc->message_type() != nullptr, "Field does have a message type.");

    const Reflection* refl {m_message_buffer->GetReflection()};
    int size {rowCount()};
    for (int i = 0; i < size; i++) {
        m_sub_models.emplace_back(new PrimitiveModel(refl->MutableRepeatedMessage(m_message_buffer, m_field_desc, i), m_field_desc, this, i));
    }
}

QVariant RepeatedPrimitiveModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from RepeatedPrimitiveModel.");
}

bool RepeatedPrimitiveModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data to RepeatedPrimitiveModel.");
}

const ProtoModel* RepeatedPrimitiveModel::get_sub_model(const FieldPath& path) const {
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "");

    int index {-1};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr);
    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr); // Skip the repeated index

    return const_cast<const ProtoModel*>(m_sub_models.at(index)->get_sub_model(path));
}

PrimitiveModel* RepeatedPrimitiveModel::get_sub_model(const int& index) const {
    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    PrimitiveModel* m = dynamic_cast<PrimitiveModel*>(m_sub_models.at(index));
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m, nullptr);

    return m;
}

const FieldDescriptor* RepeatedPrimitiveModel::get_column_descriptor([[maybe_unused]] const int& column) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    VALIDATE_INDEX_NON_VOID(column, columnCount(), nullptr, 
        "Requesting descriptor of invalid column (field index) " + std::to_string(column) + " of MessageModel " + desc->full_name());
    return desc->field(column);
}

int RepeatedPrimitiveModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedPrimitiveModel::data(const QModelIndex& index, int role) const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from a primitive message model.");
}

bool RepeatedPrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a primitive message model.");
}

int RepeatedPrimitiveModel::append_row() {
    int row {rowCount()};
    // Q_EMIT rowsAboutToBeInserted(QModelIndex(), row, row, {});
    beginInsertRows(QModelIndex(), row, row);

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
            FAIL_AND_RETURN_NON_VOID(-1, "Trying to append an enum field.");
            break;
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
            break;
    }

    m_sub_models.emplace_back(new PrimitiveModel(m_message_buffer, m_field_desc, this, row));

    bool result {insertRows(row, 1)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, -1);

    parent_data_changed();

    endInsertRows();
    // Q_EMIT rowsInserted(QModelIndex(), row, row, {});

    return row;
}

bool RepeatedPrimitiveModel::remove_row(const int& row) {
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

int RepeatedPrimitiveModel::field_to_column(const int& fn) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, -1);

    return field->index();
}

bool RepeatedPrimitiveModel::insertRows(int row, int count, const QModelIndex &parent) {
    return insertRow(row, parent);
}

bool RepeatedPrimitiveModel::removeRows(int row, int count, const QModelIndex &parent) {
    return removeRow(row, parent);
}

void RepeatedPrimitiveModel::clear_sub_models() {
    int size {rowCount()};

    for (int i = 0; i < size; ++i) delete m_sub_models.at(i);

    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->ClearField(m_message_buffer, m_field_desc);
    m_sub_models.clear();
}
