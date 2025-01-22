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

#include "gui/model/proto_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/util/json_util.h>
#include <QDebug>
#include <QFile>

#include "error_macros.hpp"

using namespace google::protobuf::util;
using Message = google::protobuf::Message;

ProtoModel::ProtoModel(ProtoModel* parent_model, const int& index_in_parent)
    : QAbstractItemModel(parent_model), m_parent_model(parent_model), m_index_in_parent(index_in_parent) {
  if (m_parent_model == nullptr) {
    QObject::connect(this, &QAbstractItemModel::dataChanged,
                     [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
                       this->saveToJson("model.json");
                     });
  }
}

QByteArray ProtoModel::serializeToJson() const {
  const ProtoModel* root_model = get_root_model();
  CHECK_PARAM_NULLPTR_NON_VOID(root_model, QByteArray(), "Failed to get root model");
  Message* message_buffer = root_model->get_message_buffer();
  CHECK_PARAM_NULLPTR_NON_VOID(message_buffer, QByteArray(), "Failed to get message buffer");

  std::string jsonString;
  JsonOptions options;
  options.add_whitespace = true;  // Makes JSON human-readable
  absl::Status status = MessageToJsonString(*message_buffer, &jsonString, options);
  CHECK_CONDITION_TRUE_NON_VOID(!status.ok(), QByteArray(), "Failed to serialize JSON:" + status.ToString());
  return QByteArray::fromStdString(jsonString);
}

bool ProtoModel::deserializeFromJson(const QByteArray& jsonData) {
  const ProtoModel* root_model = get_root_model();
  CHECK_PARAM_NULLPTR_NON_VOID(root_model, false, "Failed to get root model");
  Message* message_buffer = root_model->get_message_buffer();
  CHECK_PARAM_NULLPTR_NON_VOID(message_buffer, false, "Failed to get message buffer");

  JsonParseOptions options;
  absl::Status status = JsonStringToMessage(jsonData.toStdString(), message_buffer, options);

  CHECK_CONDITION_TRUE_NON_VOID(!status.ok(), false, "Failed to deserialize JSON:" + status.ToString());

  return true;
}

bool ProtoModel::loadFromJson(const QString& filePath) {
  QFile file(filePath);
  CHECK_CONDITION_TRUE_NON_VOID(!file.open(QIODevice::ReadOnly), false,
                                "Failed to open file for reading: " + filePath.toStdString());

  QByteArray jsonData = file.readAll();
  file.close();
  return deserializeFromJson(jsonData);
}

bool ProtoModel::saveToJson(const QString& filePath) const {
  QFile file(filePath);
  CHECK_CONDITION_TRUE_NON_VOID(!file.open(QIODevice::WriteOnly), false,
                                "Failed to open file for writing: " + filePath.toStdString());

  QByteArray jsonData = serializeToJson();
  file.write(jsonData);
  file.close();
  return true;
}

QVariant ProtoModel::data(const FieldPath& path) const {
  const ProtoModel* model = get_sub_model(path);
  CHECK_PARAM_NULLPTR_NON_VOID(model, QVariant(), "Failed to get sub model " + path.to_string());
  return model->data();
}

bool ProtoModel::set_data(const FieldPath& path, const QVariant& value) {
  ProtoModel* model = const_cast<ProtoModel*>(get_sub_model(path, true));
  CHECK_PARAM_NULLPTR_NON_VOID(model, false, "Failed to get sub model " + path.to_string());
  return model->set_data(value);
}

QModelIndex ProtoModel::index(int row, int column, [[maybe_unused]] const QModelIndex& parent) const {
  CHECK_CONDITION_TRUE_NON_VOID(!hasIndex(row, column, parent), QModelIndex(), "Invalid index.");
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(!parent.isValid(), createIndex(row, column));
  return createIndex(row, column, parent.internalPointer());
}

const ProtoModel* ProtoModel::get_root_model() const {
  const ProtoModel* m{this};
  while (m->get_parent_model()) m = m->get_parent_model();
  return m;
}
