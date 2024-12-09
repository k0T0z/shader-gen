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

#ifndef MESSAGE_MODEL_HPP
#define MESSAGE_MODEL_HPP

#include <google/protobuf/message.h>
#include <memory>
#include <vector>
#include "gui/model/proto_model.hpp"

using Message = google::protobuf::Message;
using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

/**
    MessageModel represents a single Protobuf message. The structure of the model is as follows:

            field 1        field 2       field 3     field 4      ...
        ╭─────────────┬──────────────┬────────────┬────────────┬─────────╮
        │    ...      │    ...       │   ...      │    ...     │  ...    │
        ╰─────────────┴──────────────┴────────────┴────────────┴─────────╯

    @brief The message represents a single row in the model. It can have multiple fields.
           Each field represents a column in the model.

    @note Oneof fields are represented as a single column.

    @todo Make lazy allocation of sub-models' messages
*/
class MessageModel : public ProtoModel {
  Q_OBJECT

 public:
  MessageModel(Message* message_buffer, ProtoModel* parent_model = nullptr, const int& index_in_parent = -1);
  virtual ~MessageModel() override { clear_sub_models(); }

  virtual void build_sub_models() override;

  virtual void parent_data_changed() const override;

  QVariant data() const override;
  bool set_data(const QVariant& value) override;

  const ProtoModel* get_sub_model(const int& field_number) const;
  const ProtoModel* get_sub_model(const FieldPath& path, const bool& for_set_data = false,
                                  const bool& for_get_oneof = false) const override;

  const FieldDescriptor* get_column_descriptor(const int& column) const override;

  virtual QModelIndex parent(const QModelIndex& child) const override;
  virtual int rowCount([[maybe_unused]] const QModelIndex& parent = QModelIndex()) const override { return 1; }
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  virtual QVariant headerData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation,
                              [[maybe_unused]] int role = Qt::DisplayRole) const override;

  virtual Message* get_message_buffer() const override { return m_message_buffer; }

 private:
  Message* m_message_buffer;
  std::unordered_map<int, ProtoModel*> m_sub_models_by_field_number;
  std::unordered_map<std::string, ProtoModel*> m_sub_models_by_oneof_name;

  /*
        This last_accessed_field_index is only needed in one case: When I need to propagate a
        column index from a message to its parent which is a repeated message model.

        For example: Let's say we have the following message:

        message SomeMessage {
            int32 id = 1;
            string name = 2;
            string email = 3;
        }

        message MyMessage {
            repeated SomeMessage some_messages = 1;
        }

        Now, SomeMessage has 0 rows and 3 columns. How many columns should some_messages model
        have? It should have 3 columns and undefined number of rows.

        Now, let's say that some_messages has 5 rows and we want to set the field at row 0 and 
        column 2 or other words "the email field of the first row". This means the some_messages
        should emit a dataChanged signal with top-left and bottom-right indices of (0, 2) and 
        (0, 2). This means we need to save the 2 somewhere. This is my approach and if you found
        a better way, bing me!!

        Check MessageModel::parent_data_changed(), the RepeatedMessageModel if branch for more 
        information. This is the only branch we use the last_accessed_field_index for.
    */
  mutable int last_accessed_field_index;

  virtual void clear_sub_models() override;
  int map_to_oneof_index(const int& field_index) const;
};

#endif  // MESSAGE_MODEL_HPP
