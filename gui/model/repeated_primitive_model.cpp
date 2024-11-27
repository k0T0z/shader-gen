#include "gui/model/repeated_primitive_model.hpp"

#include "error_macros.hpp"

RepeatedPrimitiveModel::RepeatedPrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : PrimitiveModel(message_buffer, m_field_desc, parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {
    build_sub_models();
}

void RepeatedPrimitiveModel::build_sub_models() {
    const Reflection* refl {m_message_buffer->GetReflection()};
    int size {rowCount()};
    for (int i = 0; i < size; i++) {
        m_sub_models.emplace_back(new PrimitiveModel(refl->MutableRepeatedMessage(m_message_buffer, m_field_desc, i), m_field_desc, this, i));
    }
}

QVariant RepeatedPrimitiveModel::data() const {
    std::vector<QVariant> data;
    int size {(int)m_sub_models.size()};
    for (int i = 0; i < size; ++i) {
        data.emplace_back(m_sub_models.at(i)->data());
    }
    return QVariant::fromValue(data);
}

bool RepeatedPrimitiveModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Method not implemented.");
}

const ProtoModel* RepeatedPrimitiveModel::get_sub_model(const FieldPath& path) const {
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "");

    int index {-1};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr);
    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr); // Skip the repeated index

    return m_sub_models.at(index)->get_sub_model(path);
}

PrimitiveModel* RepeatedPrimitiveModel::get_sub_model(const int& index) const {
    // const Reflection* refl {m_message_buffer->GetReflection()};

    SILENT_VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr);

    return m_sub_models.at(index);
}

const FieldDescriptor* RepeatedPrimitiveModel::get_column_descriptor([[maybe_unused]] const int& column) const {
    return nullptr;
}

int RepeatedPrimitiveModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedPrimitiveModel::data(const QModelIndex& index, int role) const {
    VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), QVariant(), 
        "Row index " + std::to_string(index.row()) + " is out of bounds (" + std::to_string(rowCount()) + " rows total)");
    
    const Reflection* refl {m_message_buffer->GetReflection()};

    switch (m_field_desc->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_INT32: return refl->GetRepeatedInt32(*m_message_buffer, m_field_desc, index.row()); break;
        case FieldDescriptor::CppType::CPPTYPE_INT64: return QVariant::fromValue(refl->GetRepeatedInt64(*m_message_buffer, m_field_desc, index.row())); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT32: return refl->GetRepeatedUInt32(*m_message_buffer, m_field_desc, index.row()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT64: return QVariant::fromValue(refl->GetRepeatedUInt64(*m_message_buffer, m_field_desc, index.row())); break;
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: return refl->GetRepeatedDouble(*m_message_buffer, m_field_desc, index.row()); break;
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: return refl->GetRepeatedFloat(*m_message_buffer, m_field_desc, index.row()); break;
        case FieldDescriptor::CppType::CPPTYPE_BOOL: return refl->GetRepeatedBool(*m_message_buffer, m_field_desc, index.row()); break;
        case FieldDescriptor::CppType::CPPTYPE_STRING: return QVariant::fromValue(refl->GetRepeatedString(*m_message_buffer, m_field_desc, index.row())); break;
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            FAIL_AND_RETURN_NON_VOID(false, "RepeatedMessageModel should contain only message fields.");
            break;
    }

    return QVariant();
}

bool RepeatedPrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    SILENT_VALIDATE_INDEX_NON_VOID(index.row(), rowCount(), false);

    const Reflection* refl {m_message_buffer->GetReflection()};

    switch (m_field_desc->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_INT32: refl->SetRepeatedInt32(m_message_buffer, m_field_desc, index.row(), value.toInt()); break;
        case FieldDescriptor::CppType::CPPTYPE_INT64: refl->SetRepeatedInt64(m_message_buffer, m_field_desc, index.row(), value.toLongLong()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT32: refl->SetRepeatedUInt32(m_message_buffer, m_field_desc, index.row(), value.toUInt()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT64: refl->SetRepeatedUInt64(m_message_buffer, m_field_desc, index.row(), value.toULongLong()); break;
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: refl->SetRepeatedDouble(m_message_buffer, m_field_desc, index.row(), value.toDouble()); break;
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: refl->SetRepeatedFloat(m_message_buffer, m_field_desc, index.row(), value.toFloat()); break;
        case FieldDescriptor::CppType::CPPTYPE_BOOL: refl->SetRepeatedBool(m_message_buffer, m_field_desc, index.row(), value.toBool()); break;
        case FieldDescriptor::CppType::CPPTYPE_STRING: refl->SetRepeatedString(m_message_buffer, m_field_desc, index.row(), value.toString().toStdString()); break;
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            FAIL_AND_RETURN_NON_VOID(false, "RepeatedMessageModel should contain only message fields.");
            break;
    }

    return true;
}

bool RepeatedPrimitiveModel::insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) {
    return insertRow(row, parent);
}

bool RepeatedPrimitiveModel::removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) {
    return removeRow(row, parent);
}

// bool RepeatedPrimitiveModel::insertRow(int row, const QModelIndex &parent) {
//     SILENT_CHECK_CONDITION_TRUE_NON_VOID(row <= rowCount(), false);

//     beginInsertRows(parent, row, row);

//     const Reflection* refl {m_message_buffer->GetReflection()};
//     Message* new_buffer {refl->AddMessage(m_message_buffer, m_field_desc)};
//     m_sub_models.emplace_back(new PrimitiveModel(new_buffer, m_field_desc, this, rowCount()));

//     parent_data_changed();

//     endInsertRows();

//     return false;
// }

int RepeatedPrimitiveModel::field_to_column(const int& fn) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, -1);

    return field->index();
}

void RepeatedPrimitiveModel::clear_sub_models() {
    int size {(int)m_sub_models.size()};
    for (int i = 0; i < size; ++i) {
        PrimitiveModel* m {m_sub_models.at(i)};
        delete m;
    }

    m_sub_models.clear();
}
