#include "gui/model/primitive_model.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/message_model.hpp"
#include "gui/model/repeated_primitive_model.hpp"

#include "error_macros.hpp"

using namespace google::protobuf;

PrimitiveModel::PrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {}

void PrimitiveModel::build_sub_models() {
    FAIL_AND_RETURN("Primitive model does not have sub-models.");
}

QVariant PrimitiveModel::data() const {
    return data(index(0, 0));
}

bool PrimitiveModel::set_data(const QVariant& value) {
    return setData(index(0, 0), value);
}

const ProtoModel* PrimitiveModel::get_sub_model([[maybe_unused]] const FieldPath& path) const {
    FAIL_AND_RETURN_NON_VOID(nullptr, "Primitive model does not have sub-models.");
}

const FieldDescriptor* PrimitiveModel::get_column_descriptor([[maybe_unused]] const int& column) const {
    return m_field_desc;
}

QModelIndex PrimitiveModel::index([[maybe_unused]] int row, [[maybe_unused]] int column, [[maybe_unused]] const QModelIndex& parent) const {
    Q_UNUSED(parent);
    CHECK_CONDITION_TRUE_NON_VOID(row == 0, QModelIndex(), "A primitive model should have only one row.");
    CHECK_CONDITION_TRUE_NON_VOID(column == 0, QModelIndex(), "A primitive model should have only one column.");
    return this->createIndex(row, column);
}

QModelIndex PrimitiveModel::parent([[maybe_unused]] const QModelIndex& child) const {
    Q_UNUSED(child);
    const ProtoModel* parent_model {get_parent_model()};

    const ProtoModel* root_model {get_root_model()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_model == root_model, QModelIndex());

    /*
        Here we need to return an index that represents the parent of the child parameter.
        The index should contain the row and column of the parent model in its parent. If 
        the parent model is a RepeatedPrimitiveModel, then the get_index_in_parent() should be
        the row and column is always 0. If the parent model is a MessageModel, then the 
        get_index_in_parent() should be the column because a MessageModel is allowed to 
        have only one row. Note that the parent model can be a RepeatedPrimitiveModel or a 
        MessageModel only as primitive models are not allowed to have children.
    */
    RepeatedPrimitiveModel* parent_repeat_model {dynamic_cast<RepeatedPrimitiveModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_repeat_model != nullptr, createIndex(parent_model->get_index_in_parent(), 0)); // RepeatedPrimitiveModel have only one column

    MessageModel* parent_message_model {dynamic_cast<MessageModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_message_model != nullptr, createIndex(0, parent_model->get_index_in_parent()));

    return QModelIndex();
}

QVariant PrimitiveModel::data([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_PARAM_NULLPTR_NON_VOID(m_field_desc, QVariant(), "Field descriptor is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() == 0, QVariant(), "A primitive model should have only one row.");
    CHECK_CONDITION_TRUE_NON_VOID(index.column() == 0, QVariant(), "A primitive model should have only one column.");

    const Reflection* refl {m_message_buffer->GetReflection()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!refl->HasField(*m_message_buffer, m_field_desc), QVariant());

    switch (m_field_desc->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
            FAIL_AND_RETURN_NON_VOID(QVariant(), "Trying to get a message field.");
            break;
        case FieldDescriptor::CppType::CPPTYPE_INT32: return refl->GetInt32(*m_message_buffer, m_field_desc);
        case FieldDescriptor::CppType::CPPTYPE_INT64: return QVariant::fromValue(refl->GetInt64(*m_message_buffer, m_field_desc));
        case FieldDescriptor::CppType::CPPTYPE_UINT32: return refl->GetUInt32(*m_message_buffer, m_field_desc);
        case FieldDescriptor::CppType::CPPTYPE_UINT64: return QVariant::fromValue(refl->GetUInt64(*m_message_buffer, m_field_desc));
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: return refl->GetDouble(*m_message_buffer, m_field_desc);
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: return refl->GetFloat(*m_message_buffer, m_field_desc);
        case FieldDescriptor::CppType::CPPTYPE_BOOL: return refl->GetBool(*m_message_buffer, m_field_desc);
        case FieldDescriptor::CppType::CPPTYPE_STRING: return QString::fromStdString(refl->GetString(*m_message_buffer, m_field_desc));
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
            break;
    }

    return QVariant();
}

bool PrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_PARAM_NULLPTR_NON_VOID(m_field_desc, false, "Field descriptor is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() == 0, false, "A primitive model should have only one row.");
    CHECK_CONDITION_TRUE_NON_VOID(index.column() == 0, false, "A primitive model should have only one column.");

    const Reflection* refl {m_message_buffer->GetReflection()};

    switch (m_field_desc->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
            FAIL_AND_RETURN_NON_VOID(false, "Trying to set a message field.");
            break;
        case FieldDescriptor::CppType::CPPTYPE_INT32: refl->SetInt32(m_message_buffer, m_field_desc, value.toInt()); break;
        case FieldDescriptor::CppType::CPPTYPE_INT64: refl->SetInt64(m_message_buffer, m_field_desc, value.toLongLong()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT32: refl->SetUInt32(m_message_buffer, m_field_desc, value.toUInt()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT64: refl->SetUInt64(m_message_buffer, m_field_desc, value.toULongLong()); break;
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: refl->SetDouble(m_message_buffer, m_field_desc, value.toDouble()); break;
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: refl->SetFloat(m_message_buffer, m_field_desc, value.toFloat()); break;
        case FieldDescriptor::CppType::CPPTYPE_BOOL: refl->SetBool(m_message_buffer, m_field_desc, value.toBool()); break;
        case FieldDescriptor::CppType::CPPTYPE_STRING: refl->SetString(m_message_buffer, m_field_desc, value.toString().toStdString()); break;
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
            break;
    }

    emit dataChanged(index, index);
    ProtoModel::parent_data_changed();

    return true;
}
