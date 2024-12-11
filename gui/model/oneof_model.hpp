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

#ifndef ONEOF_MODEL_HPP
#define ONEOF_MODEL_HPP

#include <google/protobuf/message.h>
#include <memory>
#include <vector>
#include "gui/model/proto_model.hpp"

using Message = google::protobuf::Message;
using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using OneofDescriptor = google::protobuf::OneofDescriptor;

class OneofModel : public ProtoModel {
  Q_OBJECT

 public:
  OneofModel(Message* message_buffer, const OneofDescriptor* oneof_desc, ProtoModel* parent_model,
             const int& index_in_parent);
  ~OneofModel() override { clear_sub_model(); }

  void build_sub_models() override;

  void parent_data_changed() const override;

  QVariant data() const override;
  bool set_data(const QVariant& value) override;

  const ProtoModel* get_sub_model(const int& field_number) const;
  const ProtoModel* get_sub_model(const FieldPath& path, const bool& for_set_data = false,
                                  const bool& for_get_oneof = false) const override;

  int get_oneof_value_field_number() const override;

  const FieldDescriptor* get_column_descriptor(const int& column) const override;

  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount([[maybe_unused]] const QModelIndex& parent = QModelIndex()) const override { return 1; }
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  QVariant headerData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation,
                      [[maybe_unused]] int role = Qt::DisplayRole) const override;

  bool set_oneof(const int& field_number) const;

 private:
  Message* m_message_buffer;
  const OneofDescriptor* m_oneof_desc;

  mutable const FieldDescriptor* m_current_field_desc;
  mutable ProtoModel* m_sub_model;

  void clear_sub_models() override {}
  void clear_sub_model() const;
  bool is_set() const;
  bool set_oneof(const FieldDescriptor* field_desc) const;
};

#endif  // ONEOF_MODEL_HPP
