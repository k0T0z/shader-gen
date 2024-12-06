#include "gui/model/primitive_model.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/message_model.hpp"
#include "gui/model/repeated_primitive_model.hpp"
#include "gui/model/oneof_model.hpp"

#include "error_macros.hpp"

using namespace google::protobuf;

PrimitiveModel::PrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent)
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {}

void PrimitiveModel::build_sub_models() {
    FAIL_AND_RETURN("Primitive model does not have sub-models.");
}

void PrimitiveModel::parent_data_changed() const {
    const ProtoModel* parent {get_parent_model()};
    CHECK_PARAM_NULLPTR(parent, "Parent of primitive model is null.");

    QModelIndex index {this->parent(QModelIndex())};
    CHECK_CONDITION_TRUE(!index.isValid(), "Parent index is invalid.");

    ProtoModel* t {const_cast<ProtoModel*>(parent)};

    if (RepeatedPrimitiveModel* repeated_m {dynamic_cast<RepeatedPrimitiveModel*>(t)}) {
        Q_EMIT repeated_m->dataChanged(index, index);
        repeated_m->parent_data_changed();
        return;
    } else if (MessageModel* message_m {dynamic_cast<MessageModel*>(t)}) {
        Q_EMIT message_m->dataChanged(index, index);
        message_m->parent_data_changed();
        return;
    } else if (OneofModel* oneof_m {dynamic_cast<OneofModel*>(t)}) {
        Q_EMIT oneof_m->dataChanged(index, index);
        oneof_m->parent_data_changed();
        return;
    }

    ERROR_PRINT("Parent model is not a repeated primitive model, message model, or oneof model.");
}

QVariant PrimitiveModel::data() const {
    return data(this->index(0, 0, this->parent(QModelIndex())));
}

bool PrimitiveModel::set_data(const QVariant& value) {
    return setData(this->index(0, 0, this->parent(QModelIndex())), value);
}

const ProtoModel* PrimitiveModel::get_sub_model([[maybe_unused]] const FieldPath& path, const bool& for_set_data) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + desc->full_name());
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_empty(), nullptr, "Trying to get sub-model of a primitive model.");
    return this;
}

const FieldDescriptor* PrimitiveModel::get_column_descriptor([[maybe_unused]] const int& column) const {
    CHECK_CONDITION_TRUE_NON_VOID(column > 0, nullptr, "A primitive model should have only one column.");
    return m_field_desc;
}

QModelIndex PrimitiveModel::parent([[maybe_unused]] const QModelIndex& child) const {
    const ProtoModel* parent {get_parent_model()};
    CHECK_PARAM_NULLPTR_NON_VOID(parent, QModelIndex(), "Parent of primitive model is null.");

    ProtoModel* t {const_cast<ProtoModel*>(parent)};

    /*
        Here we need to return an index that represents the parent of the this message model.
        The index should contain the row and column of the parent model in its parent. 
        
        If the parent model is a RepeatedPrimitiveModel, then the row should be the index in it.
        While the column will always be 0 as repeated primitive models has only one column.
        
        If the parent model is a MessageModel, then the row should be 0 and the column should 
        be the index in it. Same goes for OneofModel.
        
        Note that the parent model can be a RepeatedPrimitiveModel, MessageModel, or OneofModel 
        as primitive models cannot have children except for repeated primitive models which are
        only allowed to have children of type PrimitiveModel.
    */
    if (RepeatedPrimitiveModel* repeated_m {dynamic_cast<RepeatedPrimitiveModel*>(t)}) {
        return repeated_m->index(m_index_in_parent, 0, repeated_m->parent(QModelIndex()));
    } else if (MessageModel* message_m {dynamic_cast<MessageModel*>(t)}) {
        return message_m->index(0, m_index_in_parent, message_m->parent(QModelIndex()));
    } else if (OneofModel* oneof_m {dynamic_cast<OneofModel*>(t)}) {
        return oneof_m->index(0, m_index_in_parent, oneof_m->parent(QModelIndex()));
    }

    FAIL_AND_RETURN_NON_VOID(QModelIndex(), "Parent model is not a repeated primitive model, message model, or oneof model.");
}

QVariant PrimitiveModel::data([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_PARAM_NULLPTR_NON_VOID(m_field_desc, QVariant(), "Field descriptor is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, QVariant(), "A primitive model should have only one row.");
    CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, QVariant(), "A primitive model should have only one column.");

    const Reflection* refl {m_message_buffer->GetReflection()};

    if (m_field_desc->is_repeated()) {
        if (index.parent().isValid()) {
            VALIDATE_INDEX_NON_VOID(index.parent().row(), get_parent_model()->rowCount(), QVariant(), "Parent row index is out of range.");
        } else {
            QModelIndex parent_index {this->parent(QModelIndex())};
            CHECK_CONDITION_TRUE_NON_VOID(!parent_index.isValid(), QVariant(), "Parent index is invalid.");
            VALIDATE_INDEX_NON_VOID(parent_index.row(), get_parent_model()->rowCount(), QVariant(), "Parent row index is out of range.");
        }        

        switch (m_field_desc->cpp_type()) {
            case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
                FAIL_AND_RETURN_NON_VOID(QVariant(), "Trying to get a message field.");
                break;
            case FieldDescriptor::CppType::CPPTYPE_INT32: return refl->GetRepeatedInt32(*m_message_buffer, m_field_desc, index.parent().row());
            case FieldDescriptor::CppType::CPPTYPE_INT64: return QVariant::fromValue(refl->GetRepeatedInt64(*m_message_buffer, m_field_desc, index.parent().row()));
            case FieldDescriptor::CppType::CPPTYPE_UINT32: return refl->GetRepeatedUInt32(*m_message_buffer, m_field_desc, index.parent().row());
            case FieldDescriptor::CppType::CPPTYPE_UINT64: return QVariant::fromValue(refl->GetRepeatedUInt64(*m_message_buffer, m_field_desc, index.parent().row()));
            case FieldDescriptor::CppType::CPPTYPE_DOUBLE: return refl->GetRepeatedDouble(*m_message_buffer, m_field_desc, index.parent().row());
            case FieldDescriptor::CppType::CPPTYPE_FLOAT: return refl->GetRepeatedFloat(*m_message_buffer, m_field_desc, index.parent().row());
            case FieldDescriptor::CppType::CPPTYPE_BOOL: return refl->GetRepeatedBool(*m_message_buffer, m_field_desc, index.parent().row());
            case FieldDescriptor::CppType::CPPTYPE_STRING: return QString::fromStdString(refl->GetRepeatedString(*m_message_buffer, m_field_desc, index.parent().row()));
            case FieldDescriptor::CppType::CPPTYPE_ENUM: return refl->GetRepeatedEnumValue(*m_message_buffer, m_field_desc, index.parent().row());
            default:
                WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
                break;
        }
    } else {
        CHECK_CONDITION_TRUE_NON_VOID(!refl->HasField(*m_message_buffer, m_field_desc), QVariant(), "Field is not set.");

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
            case FieldDescriptor::CppType::CPPTYPE_ENUM: return refl->GetEnumValue(*m_message_buffer, m_field_desc);
            default:
                WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
                break;
        }
    }

    return QVariant();
}

bool PrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value, [[maybe_unused]] int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_PARAM_NULLPTR_NON_VOID(m_field_desc, false, "Field descriptor is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(value.isNull(), false, "Value is null.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, false, "A primitive model should have only one row.");
    CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, false, "A primitive model should have only one column.");

    const Reflection* refl {m_message_buffer->GetReflection()};
    
    if (m_field_desc->is_repeated()) {
        if (index.parent().isValid()) {
            VALIDATE_INDEX_NON_VOID(index.parent().row(), get_parent_model()->rowCount(), false, "Parent row index is out of range.");
        } else {
            QModelIndex parent_index {this->parent(QModelIndex())};
            CHECK_CONDITION_TRUE_NON_VOID(!parent_index.isValid(), false, "Parent index is invalid.");
            VALIDATE_INDEX_NON_VOID(parent_index.row(), get_parent_model()->rowCount(), false, "Parent row index is out of range.");
        }

        switch (m_field_desc->cpp_type()) {
            case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
                FAIL_AND_RETURN_NON_VOID(false, "Trying to set a message field.");
                break;
            case FieldDescriptor::CppType::CPPTYPE_INT32: refl->SetRepeatedInt32(m_message_buffer, m_field_desc, index.parent().row(), value.toInt()); break;
            case FieldDescriptor::CppType::CPPTYPE_INT64: refl->SetRepeatedInt64(m_message_buffer, m_field_desc, index.parent().row(), value.toLongLong()); break;
            case FieldDescriptor::CppType::CPPTYPE_UINT32: refl->SetRepeatedUInt32(m_message_buffer, m_field_desc, index.parent().row(), value.toUInt()); break;
            case FieldDescriptor::CppType::CPPTYPE_UINT64: refl->SetRepeatedUInt64(m_message_buffer, m_field_desc, index.parent().row(), value.toULongLong()); break;
            case FieldDescriptor::CppType::CPPTYPE_DOUBLE: refl->SetRepeatedDouble(m_message_buffer, m_field_desc, index.parent().row(), value.toDouble()); break;
            case FieldDescriptor::CppType::CPPTYPE_FLOAT: refl->SetRepeatedFloat(m_message_buffer, m_field_desc, index.parent().row(), value.toFloat()); break;
            case FieldDescriptor::CppType::CPPTYPE_BOOL: refl->SetRepeatedBool(m_message_buffer, m_field_desc, index.parent().row(), value.toBool()); break;
            case FieldDescriptor::CppType::CPPTYPE_STRING: refl->SetRepeatedString(m_message_buffer, m_field_desc, index.parent().row(), value.toString().toStdString()); break;
            case FieldDescriptor::CppType::CPPTYPE_ENUM: refl->SetRepeatedEnumValue(m_message_buffer, m_field_desc, index.parent().row(), value.toInt()); break;
            default:
                WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
                break;
        }
    } else {
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
            case FieldDescriptor::CppType::CPPTYPE_ENUM: refl->SetEnumValue(m_message_buffer, m_field_desc, value.toInt()); break;
            default:
                WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
                break;
        }
    }

    Q_EMIT dataChanged(index, index);
    parent_data_changed();

    return true;
}

QVariant PrimitiveModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(orientation != Qt::Orientation::Horizontal, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(role != Qt::DisplayRole, QVariant());
    CHECK_CONDITION_TRUE_NON_VOID(section > 0, QVariant(), "A primitive model should have only one column.");
    
    const FieldDescriptor* field_desc {get_column_descriptor(section)};
    CHECK_PARAM_NULLPTR_NON_VOID(field_desc, QVariant(), "Field is null.");

    return QString::fromStdString(field_desc->full_name());
}
