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

#include "gui/model/repeated_primitive_model.hpp"

#include "error_macros.hpp"
#include "gui/model/message_model.hpp"

RepeatedPrimitiveModel::RepeatedPrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc,
                                               ProtoModel* parent_model, const int& index_in_parent)
    : PrimitiveModel(message_buffer, field_desc, parent_model, index_in_parent),
      m_message_buffer(message_buffer),
      m_field_desc(field_desc) {}

void RepeatedPrimitiveModel::build_sub_models() {
  CHECK_CONDITION_TRUE(!m_field_desc->is_repeated(), "Field " + m_field_desc->full_name() + " is not repeated.");
  CHECK_CONDITION_TRUE(m_field_desc->message_type() != nullptr,
                       "Field " + m_field_desc->full_name() + " is not a primitive field.");

  int size{rowCount()};
  for (int i{0}; i < size; i++) {
    SILENT_CONTINUE_IF_TRUE(i < (int)m_sub_models.size());
    append_row(i);
  }
}

void RepeatedPrimitiveModel::parent_data_changed() const {
  const ProtoModel* parent{get_parent_model()};
  CHECK_PARAM_NULLPTR(parent, "Parent of repeated primitive model is null.");

  QModelIndex index{this->parent(QModelIndex())};
  CHECK_CONDITION_TRUE(!index.isValid(), "Parent index is invalid.");

  ProtoModel* t{const_cast<ProtoModel*>(parent)};

  if (MessageModel * message_m{dynamic_cast<MessageModel*>(t)}) {
    Q_EMIT message_m->dataChanged(index, index);
    message_m->parent_data_changed();
    return;
  }

  ERROR_PRINT("Parent model is not a message model.");
}

QVariant RepeatedPrimitiveModel::data() const {
  FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from RepeatedPrimitiveModel.");
}

bool RepeatedPrimitiveModel::set_data([[maybe_unused]] const QVariant& value) {
  FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a RepeatedPrimitiveModel.");
}

PrimitiveModel* RepeatedPrimitiveModel::get_sub_model(const int& index) const {
  VALIDATE_INDEX_NON_VOID(
      index, rowCount(), nullptr,
      "Accessing out-of-range proto row " + std::to_string(index) + " of " + std::to_string(rowCount()));

  PrimitiveModel* m = dynamic_cast<PrimitiveModel*>(m_sub_models.at(index));
  CHECK_PARAM_NULLPTR_NON_VOID(m, nullptr, "Failed to cast sub model to PrimitiveModel.");

  return m;
}

const ProtoModel* RepeatedPrimitiveModel::get_sub_model(const FieldPath& path, const bool& for_set_data,
                                                        const bool& for_get_oneof) const {
  CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + m_field_desc->full_name());
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(path.is_empty(), this);

  int index{-1};

  CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_repeated_index(index), nullptr,
                                "Next component is not a repeated index.");
  VALIDATE_INDEX_NON_VOID(index, rowCount(), nullptr, "Index out of range.");

  CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr, "Failed to skip repeated index.");

  return m_sub_models.at(index)->get_sub_model(path, for_set_data, for_get_oneof);
}

QModelIndex RepeatedPrimitiveModel::parent([[maybe_unused]] const QModelIndex& child) const {
  const ProtoModel* parent{get_parent_model()};
  CHECK_PARAM_NULLPTR_NON_VOID(parent, QModelIndex(), "Parent of repeated primitive model is null.");

  ProtoModel* t{const_cast<ProtoModel*>(parent)};

  /*
        Here we need to return an index that represents the parent of the this message model.
        The index should contain the row and column of the parent model in its parent. 
        
        If the parent model is a MessageModel, then the row should be 0 and the column should 
        be the index in it.
        
        Note that the parent model can be a MessageModel only as primitive models cannot have 
        children except for repeated primitive models which are only allowed to have children 
        of type PrimitiveModel.
    */
  if (MessageModel * message_m{dynamic_cast<MessageModel*>(t)}) {
    return message_m->index(0, m_index_in_parent, message_m->parent(QModelIndex()));
  }

  FAIL_AND_RETURN_NON_VOID(QModelIndex(), "Parent model is not a message model.");
}

int RepeatedPrimitiveModel::rowCount([[maybe_unused]] const QModelIndex& parent) const {
  const Reflection* refl{m_message_buffer->GetReflection()};
  return refl->FieldSize(*m_message_buffer, m_field_desc);
}

QVariant RepeatedPrimitiveModel::data(const QModelIndex& index, int role) const {
  CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
  CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
  VALIDATE_INDEX_NON_VOID(
      index.row(), rowCount(), QVariant(),
      "Accessing out-of-range proto row " + std::to_string(index.row()) + " of " + std::to_string(rowCount()));
  CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, QVariant(), "A primitive model should have only one column.");

  return get_sub_model(index.row())->data(this->index(0, 0, index), role);
}

bool RepeatedPrimitiveModel::setData([[maybe_unused]] const QModelIndex& index, [[maybe_unused]] const QVariant& value,
                                     [[maybe_unused]] int role) {
  CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
  CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
  CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
  CHECK_CONDITION_TRUE_NON_VOID(value.isNull(), false, "Value is null.");
  VALIDATE_INDEX_NON_VOID(
      index.row(), rowCount(), false,
      "Accessing out-of-range proto row " + std::to_string(index.row()) + " of " + std::to_string(rowCount()));
  CHECK_CONDITION_TRUE_NON_VOID(index.column() > 0, false, "A primitive model should have only one column.");

  return get_sub_model(index.row())->setData(this->index(0, 0, index), value, role);
}

int RepeatedPrimitiveModel::append_row() {
  /*
        Here we can't call this->index() because the new row is not added in the proto
        model yet. In the append_row(const int& row) function, the new row is added to the
        proto model however, we need to add it to QAbstractItemModel as well.
    */
  int row{rowCount()};
  QModelIndex parent_index{this->parent(QModelIndex())};

  beginInsertRows(parent_index, row, row);

  bool result{insertRows(row, 1, parent_index)};
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, -1);

  endInsertRows();

  return row;
}

bool RepeatedPrimitiveModel::remove_row(const int& row) {
  VALIDATE_INDEX_NON_VOID(row, rowCount(), false, "Index out of range.");

  QModelIndex parent_index{this->parent(QModelIndex())};

  beginRemoveRows(parent_index, row, row);

  bool result{removeRows(row, 1, parent_index)};
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(!result, false);

  endRemoveRows();

  return true;
}

int RepeatedPrimitiveModel::field_to_column(const int& fn) const {
  const Descriptor* desc{m_message_buffer->GetDescriptor()};

  const FieldDescriptor* field_desc{desc->FindFieldByNumber(fn)};

  CHECK_PARAM_NULLPTR_NON_VOID(field_desc, -1, "Field not found.");

  return field_desc->index();
}

bool RepeatedPrimitiveModel::insertRows(int row, int count, const QModelIndex& parent) {
  CHECK_CONDITION_TRUE_NON_VOID(!parent.isValid(), false, "Parent is not valid.");
  CHECK_CONDITION_TRUE_NON_VOID(row >= (rowCount() + 1), false, "You can only insert a row at the end of the model.");
  CHECK_CONDITION_TRUE_NON_VOID(
      count != 1, false,
      "Invalid number of rows: " + std::to_string(count) + ". Adding multiple rows at once is not supported yet.");

  const Reflection* refl{m_message_buffer->GetReflection()};

  // Create a new model for an existing primitve or a new primitive
  if (row >= rowCount()) {
    switch (m_field_desc->cpp_type()) {
      case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
        FAIL_AND_RETURN_NON_VOID(false, "Trying to append a message field.");
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT32:
        refl->AddInt32(m_message_buffer, m_field_desc, 0);
        break;
      case FieldDescriptor::CppType::CPPTYPE_INT64:
        refl->AddInt64(m_message_buffer, m_field_desc, 0);
        break;
      case FieldDescriptor::CppType::CPPTYPE_UINT32:
        refl->AddUInt32(m_message_buffer, m_field_desc, 0);
        break;
      case FieldDescriptor::CppType::CPPTYPE_UINT64:
        refl->AddUInt64(m_message_buffer, m_field_desc, 0);
        break;
      case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
        refl->AddDouble(m_message_buffer, m_field_desc, 0.0);
        break;
      case FieldDescriptor::CppType::CPPTYPE_FLOAT:
        refl->AddFloat(m_message_buffer, m_field_desc, 0.0f);
        break;
      case FieldDescriptor::CppType::CPPTYPE_BOOL:
        refl->AddBool(m_message_buffer, m_field_desc, false);
        break;
      case FieldDescriptor::CppType::CPPTYPE_STRING:
        refl->AddString(m_message_buffer, m_field_desc, "");
        break;
      case FieldDescriptor::CppType::CPPTYPE_ENUM:
        refl->AddEnumValue(m_message_buffer, m_field_desc, 0);
        break;
      default:
        WARN_PRINT("Unsupported field type: " + std::to_string(m_field_desc->cpp_type()));
        break;
    }
  }

  m_sub_models.emplace_back(new PrimitiveModel(m_message_buffer, m_field_desc, this, row));

  return true;
}

bool RepeatedPrimitiveModel::removeRows(int row, int count, const QModelIndex& parent) {
  CHECK_CONDITION_TRUE_NON_VOID(!parent.isValid(), false, "Parent is not valid.");
  VALIDATE_INDEX_NON_VOID(row, rowCount(), false, "Index out of range.");
  CHECK_CONDITION_TRUE_NON_VOID(
      count != 1, false,
      "Invalid number of rows: " + std::to_string(count) + ". Adding multiple rows at once is not supported yet.");

  int last_row{rowCount() - 1};

  if (row < last_row) std::swap(m_sub_models.at(row), m_sub_models.at(last_row));

  delete m_sub_models.at(last_row);
  m_sub_models.erase(m_sub_models.begin() + last_row);

  // We remove a row by swapping the last row with the row to be removed and then removing the last row.
  // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection.RemoveLast.details
  const Reflection* refl{m_message_buffer->GetReflection()};
  if (row < last_row) refl->SwapElements(m_message_buffer, m_field_desc, row, last_row);
  refl->RemoveLast(m_message_buffer, m_field_desc);

  // Must be done after removing the row
  for (int i{row}; i < rowCount(); i++) m_sub_models.at(i)->set_index_in_parent(i);  // Update the index in parent

  return true;
}

void RepeatedPrimitiveModel::clear_sub_models() {
  for (auto& sub_model : m_sub_models) delete sub_model;
  m_sub_models.clear();

  const Reflection* refl{m_message_buffer->GetReflection()};
  refl->ClearField(m_message_buffer, m_field_desc);
}

void RepeatedPrimitiveModel::append_row(const int& row) {
  VALIDATE_INDEX(row, rowCount(),
                 "You are allowed to append only a row between 0 and " + std::to_string(rowCount()) + "exclusive.");

  QModelIndex parent_index{this->parent(QModelIndex())};

  beginInsertRows(parent_index, row, row);
  
  bool result{insertRows(row, 1, parent_index)};
  CHECK_CONDITION_TRUE(!result, "Failed to insert row.");

  endInsertRows();
}
