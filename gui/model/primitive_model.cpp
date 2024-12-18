/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2024 Seif Kandil (k0T0z)                                       */
/*                                                                               */
/*  This file is a part of the ENIGMA Development Environment.                   */
/*                                                                               */
/*                                                                               */
/*  ENIGMA is free software: you can redistribute it and/or modify it under the  */
/*  terms of the GNU General Public License as published by the Free Software    */
/*  Foundation, version 3 of the license or any later version.                   */
/*                                                                               */
/*  This application and its source code is distributed AS-IS, WITHOUT ANY       */
/*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    */
/*  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        */
/*  details.                                                                     */
/*                                                                               */
/*  You should have recieved a copy of the GNU General Public License along      */
/*  with this code. If not, see <http://www.gnu.org/licenses/>                   */
/*                                                                               */
/*  ENIGMA is an environment designed to create games and other programs with a  */
/*  high-level, fully compilable language. Developers of ENIGMA or anything      */
/*  associated with ENIGMA are in no way responsible for its users or            */
/*  applications created by its users, or damages caused by the environment      */
/*  or programs made in the environment.                                         */
/*                                                                               */
/*********************************************************************************/

#include "gui/model/primitive_model.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/message_model.hpp"
#include "gui/model/oneof_model.hpp"
#include "gui/model/repeated_primitive_model.hpp"
#include "gui/model/utils/utils.hpp"

#include "error_macros.hpp"

using namespace google::protobuf;

PrimitiveModel::PrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model,
                               const int& index_in_parent)
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer), m_field_desc(field_desc) {}

void PrimitiveModel::build_sub_models() { FAIL_AND_RETURN("Primitive model does not have sub-models."); }

void PrimitiveModel::parent_data_changed() const {
  const ProtoModel* parent{get_parent_model()};
  CHECK_PARAM_NULLPTR(parent, "Parent of primitive model is null.");

  QModelIndex index{this->parent(QModelIndex())};
  CHECK_CONDITION_TRUE(!index.isValid(), "Parent index is invalid.");

  ProtoModel* t{const_cast<ProtoModel*>(parent)};

  if (RepeatedPrimitiveModel * repeated_m{dynamic_cast<RepeatedPrimitiveModel*>(t)}) {
    Q_EMIT repeated_m->dataChanged(index, index);
    repeated_m->parent_data_changed();
    return;
  } else if (MessageModel * message_m{dynamic_cast<MessageModel*>(t)}) {
    Q_EMIT message_m->dataChanged(index, index);
    message_m->parent_data_changed();
    return;
  } else if (OneofModel * oneof_m{dynamic_cast<OneofModel*>(t)}) {
    Q_EMIT oneof_m->dataChanged(index, index);
    oneof_m->parent_data_changed();
    return;
  }

  ERROR_PRINT("Parent model is not a repeated primitive model, message model, or oneof model.");
}

QVariant PrimitiveModel::data() const { return data(this->index(0, 0, this->parent(QModelIndex()))); }

bool PrimitiveModel::set_data(const QVariant& value) {
  return setData(this->index(0, 0, this->parent(QModelIndex())), value);
}

const ProtoModel* PrimitiveModel::get_sub_model([[maybe_unused]] const FieldPath& path, const bool& for_set_data,
                                                const bool& for_get_oneof) const {
  const Descriptor* desc{m_message_buffer->GetDescriptor()};
  CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + desc->full_name());
  CHECK_CONDITION_TRUE_NON_VOID(!path.is_empty(), nullptr, "Trying to get sub-model of a primitive model.");
  return this;
}

const FieldDescriptor* PrimitiveModel::get_column_descriptor([[maybe_unused]] const int& column) const {
  CHECK_CONDITION_TRUE_NON_VOID(column > 0, nullptr, "A primitive model should have only one column.");
  return m_field_desc;
}

QModelIndex PrimitiveModel::parent([[maybe_unused]] const QModelIndex& child) const {
  const ProtoModel* parent{get_parent_model()};
  CHECK_PARAM_NULLPTR_NON_VOID(parent, QModelIndex(), "Parent of primitive model is null.");

  ProtoModel* t{const_cast<ProtoModel*>(parent)};

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
  if (RepeatedPrimitiveModel * repeated_m{dynamic_cast<RepeatedPrimitiveModel*>(t)}) {
    return repeated_m->index(m_index_in_parent, 0, repeated_m->parent(QModelIndex()));
  } else if (MessageModel * message_m{dynamic_cast<MessageModel*>(t)}) {
    return message_m->index(0, m_index_in_parent, message_m->parent(QModelIndex()));
  } else if (OneofModel * oneof_m{dynamic_cast<OneofModel*>(t)}) {
    return oneof_m->index(0, m_index_in_parent, oneof_m->parent(QModelIndex()));
  }

  FAIL_AND_RETURN_NON_VOID(QModelIndex(),
                           "Parent model is not a repeated primitive model, message model, or oneof model.");
}

QVariant PrimitiveModel::data([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] int role) const {
  CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
  CHECK_PARAM_NULLPTR_NON_VOID(m_field_desc, QVariant(), "Field descriptor is null.");
  CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
  CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, QVariant(), "A primitive model should have only one row.");
  CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, QVariant(), "A primitive model should have only one column.");

  const Reflection* refl{m_message_buffer->GetReflection()};

  if (m_field_desc->is_repeated()) {
    if (index.parent().isValid()) {
      VALIDATE_INDEX_NON_VOID(index.parent().row(), get_parent_model()->rowCount(), QVariant(),
                              "Parent row index is out of range.");
    } else {
      QModelIndex parent_index{this->parent(QModelIndex())};
      CHECK_CONDITION_TRUE_NON_VOID(!parent_index.isValid(), QVariant(), "Parent index is invalid.");
      VALIDATE_INDEX_NON_VOID(parent_index.row(), get_parent_model()->rowCount(), QVariant(),
                              "Parent row index is out of range.");
    }

    switch (m_field_desc->cpp_type()) {
      case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        FAIL_AND_RETURN_NON_VOID(QVariant(), "Trying to get a message field.");
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT32:
        return refl->GetRepeatedInt32(*m_message_buffer, m_field_desc, index.parent().row());
      case FieldDescriptor::CppType::CPPTYPE_INT64:
        return QVariant::fromValue(refl->GetRepeatedInt64(*m_message_buffer, m_field_desc, index.parent().row()));
      case FieldDescriptor::CppType::CPPTYPE_UINT32:
        return refl->GetRepeatedUInt32(*m_message_buffer, m_field_desc, index.parent().row());
      case FieldDescriptor::CppType::CPPTYPE_UINT64:
        return QVariant::fromValue(refl->GetRepeatedUInt64(*m_message_buffer, m_field_desc, index.parent().row()));
      case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        return refl->GetRepeatedDouble(*m_message_buffer, m_field_desc, index.parent().row());
      case FieldDescriptor::CppType::CPPTYPE_FLOAT:
        return refl->GetRepeatedFloat(*m_message_buffer, m_field_desc, index.parent().row());
      case FieldDescriptor::CppType::CPPTYPE_BOOL:
        return refl->GetRepeatedBool(*m_message_buffer, m_field_desc, index.parent().row());
      case FieldDescriptor::CppType::CPPTYPE_STRING:
        return QString::fromStdString(refl->GetRepeatedString(*m_message_buffer, m_field_desc, index.parent().row()));
      case FieldDescriptor::CppType::CPPTYPE_ENUM:
        return refl->GetRepeatedEnumValue(*m_message_buffer, m_field_desc, index.parent().row());
      default:
        WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
        break;
    }
  } else {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!refl->HasField(*m_message_buffer, m_field_desc), QVariant());

    switch (m_field_desc->cpp_type()) {
      case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        FAIL_AND_RETURN_NON_VOID(QVariant(), "Trying to get a message field.");
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT32:
        return refl->GetInt32(*m_message_buffer, m_field_desc);
      case FieldDescriptor::CppType::CPPTYPE_INT64:
        return QVariant::fromValue(refl->GetInt64(*m_message_buffer, m_field_desc));
      case FieldDescriptor::CppType::CPPTYPE_UINT32:
        return refl->GetUInt32(*m_message_buffer, m_field_desc);
      case FieldDescriptor::CppType::CPPTYPE_UINT64:
        return QVariant::fromValue(refl->GetUInt64(*m_message_buffer, m_field_desc));
      case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        return refl->GetDouble(*m_message_buffer, m_field_desc);
      case FieldDescriptor::CppType::CPPTYPE_FLOAT:
        return refl->GetFloat(*m_message_buffer, m_field_desc);
      case FieldDescriptor::CppType::CPPTYPE_BOOL:
        return refl->GetBool(*m_message_buffer, m_field_desc);
      case FieldDescriptor::CppType::CPPTYPE_STRING:
        return QString::fromStdString(refl->GetString(*m_message_buffer, m_field_desc));
      case FieldDescriptor::CppType::CPPTYPE_ENUM:
        return refl->GetEnumValue(*m_message_buffer, m_field_desc);
      default:
        WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
        break;
    }
  }

  return QVariant();
}

bool PrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value,
                             [[maybe_unused]] int role) {
  CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
  CHECK_PARAM_NULLPTR_NON_VOID(m_field_desc, false, "Field descriptor is null.");
  CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
  CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
  CHECK_CONDITION_TRUE_NON_VOID(value.isNull(), false, "Value is null.");
  CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, false, "A primitive model should have only one row.");
  CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, false, "A primitive model should have only one column.");

  const Reflection* refl{m_message_buffer->GetReflection()};

  if (m_field_desc->is_repeated()) {
    if (index.parent().isValid()) {
      VALIDATE_INDEX_NON_VOID(index.parent().row(), get_parent_model()->rowCount(), false,
                              "Parent row index is out of range.");
    } else {
      QModelIndex parent_index{this->parent(QModelIndex())};
      CHECK_CONDITION_TRUE_NON_VOID(!parent_index.isValid(), false, "Parent index is invalid.");
      VALIDATE_INDEX_NON_VOID(parent_index.row(), get_parent_model()->rowCount(), false,
                              "Parent row index is out of range.");
    }

    switch (m_field_desc->cpp_type()) {
      case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        FAIL_AND_RETURN_NON_VOID(false, "Trying to set a message field.");
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT32:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<int>(), false, "Value is not an int.");
        refl->SetRepeatedInt32(m_message_buffer, m_field_desc, index.parent().row(), value.toInt());
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT64:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<qlonglong>(), false, "Value is not a long long.");
        refl->SetRepeatedInt64(m_message_buffer, m_field_desc, index.parent().row(), value.toLongLong());
        break;
      case FieldDescriptor::CppType::CPPTYPE_UINT32:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<uint>(), false, "Value is not an unsigned int.");
        refl->SetRepeatedUInt32(m_message_buffer, m_field_desc, index.parent().row(), value.toUInt());
        break;
      case FieldDescriptor::CppType::CPPTYPE_UINT64:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<qulonglong>(), false, "Value is not an unsigned long long.");
        refl->SetRepeatedUInt64(m_message_buffer, m_field_desc, index.parent().row(), value.toULongLong());
        break;
      case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<double>(), false, "Value is not a double.");
        refl->SetRepeatedDouble(m_message_buffer, m_field_desc, index.parent().row(), value.toDouble());
        break;
      case FieldDescriptor::CppType::CPPTYPE_FLOAT:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<float>(), false, "Value is not a float.");
        refl->SetRepeatedFloat(m_message_buffer, m_field_desc, index.parent().row(), value.toFloat());
        break;
      case FieldDescriptor::CppType::CPPTYPE_BOOL:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<bool>(), false, "Value is not a bool.");
        refl->SetRepeatedBool(m_message_buffer, m_field_desc, index.parent().row(), value.toBool());
        break;
      case FieldDescriptor::CppType::CPPTYPE_STRING:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<QString>(), false, "Value is not a string.");
        refl->SetRepeatedString(m_message_buffer, m_field_desc, index.parent().row(), value.toString().toStdString());
        break;
      case FieldDescriptor::CppType::CPPTYPE_ENUM: {
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<int>(), false, "Value is not an Enum int.");
        CHECK_CONDITION_TRUE_NON_VOID(!shadergen_utils::is_valid_enum_value(m_field_desc->enum_type(), value.toInt()),
                                      false, "Enum value is not valid.");
        refl->SetRepeatedEnumValue(m_message_buffer, m_field_desc, index.parent().row(), value.toInt());
        break;
      }
      default:
        WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
        break;
    }
  } else {
    switch (m_field_desc->cpp_type()) {
      case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        FAIL_AND_RETURN_NON_VOID(false, "Trying to set a message field.");
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT32:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<int>(), false, "Value is not an int.");
        refl->SetInt32(m_message_buffer, m_field_desc, value.toInt());
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT64:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<qlonglong>(), false, "Value is not a long long.");
        refl->SetInt64(m_message_buffer, m_field_desc, value.toLongLong());
        break;
      case FieldDescriptor::CppType::CPPTYPE_UINT32:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<uint>(), false, "Value is not an unsigned int.");
        refl->SetUInt32(m_message_buffer, m_field_desc, value.toUInt());
        break;
      case FieldDescriptor::CppType::CPPTYPE_UINT64:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<qulonglong>(), false, "Value is not an unsigned long long.");
        refl->SetUInt64(m_message_buffer, m_field_desc, value.toULongLong());
        break;
      case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<double>(), false, "Value is not a double.");
        refl->SetDouble(m_message_buffer, m_field_desc, value.toDouble());
        break;
      case FieldDescriptor::CppType::CPPTYPE_FLOAT:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<float>(), false, "Value is not a float.");
        refl->SetFloat(m_message_buffer, m_field_desc, value.toFloat());
        break;
      case FieldDescriptor::CppType::CPPTYPE_BOOL:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<bool>(), false, "Value is not a bool.");
        refl->SetBool(m_message_buffer, m_field_desc, value.toBool());
        break;
      case FieldDescriptor::CppType::CPPTYPE_STRING:
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<QString>(), false, "Value is not a string.");
        refl->SetString(m_message_buffer, m_field_desc, value.toString().toStdString());
        break;
      case FieldDescriptor::CppType::CPPTYPE_ENUM: {
        CHECK_CONDITION_TRUE_NON_VOID(!value.canConvert<int>(), false, "Value is not an Enum int.");
        CHECK_CONDITION_TRUE_NON_VOID(!shadergen_utils::is_valid_enum_value(m_field_desc->enum_type(), value.toInt()),
                                      false, "Enum value is not valid.");
        refl->SetEnumValue(m_message_buffer, m_field_desc, value.toInt());
        break;
      }
      default:
        WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
        break;
    }
  }

  Q_EMIT dataChanged(index, index);
  parent_data_changed();

  return true;
}

QVariant PrimitiveModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation,
                                    [[maybe_unused]] int role) const {
  SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(orientation != Qt::Orientation::Horizontal, QVariant());
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(role != Qt::DisplayRole, QVariant());
  CHECK_CONDITION_TRUE_NON_VOID(section > 0, QVariant(), "A primitive model should have only one column.");

  const FieldDescriptor* field_desc{get_column_descriptor(section)};
  CHECK_PARAM_NULLPTR_NON_VOID(field_desc, QVariant(), "Field is null.");

  return QString::fromStdString(field_desc->full_name());
}
