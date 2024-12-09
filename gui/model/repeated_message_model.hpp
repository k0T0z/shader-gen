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

#ifndef REPEATED_MESSAGE_MODEL_HPP
#define REPEATED_MESSAGE_MODEL_HPP

#include "gui/model/message_model.hpp"

/**
    RepeatedMessageModel represents a multiple Protobuf message. The structure of the model is as follows:

            field 1        field 2       field 3     field 4      ...
        ╭─────────────┬──────────────┬────────────┬────────────┬─────────╮
        │    0        │    ...       │   ...      │    ...     │   M     │
        ├─────────────┼──────────────┼────────────┼────────────┼─────────┤
        │    ...      │     ...      │   ...      │   ...      │  ...    │
        ├─────────────┼──────────────┼────────────┼────────────┼─────────┤
        │    N        │   ...        │   ...      │  ...       │  ...    │
        ╰─────────────┴──────────────┴────────────┴────────────┴─────────╯

    @brief The repeated message represents multiple rows in the model. It can have multiple fields.
           Each field represents a column in the model.
*/
class RepeatedMessageModel : public MessageModel {
    Q_OBJECT

public:
    RepeatedMessageModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent);
    ~RepeatedMessageModel() override {
        clear_sub_models();
    }

    void build_sub_models() override;

    void parent_data_changed() const override;

    QVariant data() const override;
    bool set_data(const QVariant& value) override;
    
    MessageModel* get_sub_model(const int& index) const;
    const ProtoModel* get_sub_model(const FieldPath& path, const bool& for_set_data = false, const bool& for_get_oneof = false) const override;

    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    int append_row() override;
    bool remove_row(const int& row) override;

    int field_to_column(const int& fn) const;

    Message* get_message_buffer() const { return m_message_buffer; }

private:
    Message* m_message_buffer;
    const FieldDescriptor* m_field_desc;
    std::vector<MessageModel*> m_sub_models;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void clear_sub_models() override;
    void append_row(const int& row);
};

#endif // REPEATED_MESSAGE_MODEL_HPP

   