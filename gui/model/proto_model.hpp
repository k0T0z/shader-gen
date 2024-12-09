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

#ifndef PROTO_MODEL_HPP
#define PROTO_MODEL_HPP

#include <QAbstractItemModel>
#include <google/protobuf/message.h>
#include "gui/model/utils/field_path.hpp"
#include "error_macros.hpp"

using Message = google::protobuf::Message;
using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

// Check https://protobuf.dev/programming-guides/best-practices/ before adding any new features
// https://doc.qt.io/qt-5/model-view-programming.html#creating-new-models
class ProtoModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ProtoModel(ProtoModel* parent_model = nullptr, const int& index_in_parent = -1);
    virtual ~ProtoModel() override = default;

    virtual void build_sub_models() = 0;

    // JSON Serialization
    bool loadFromJson(const QString& filePath);
    bool saveToJson(const QString& filePath) const;

    virtual void parent_data_changed() const = 0;

    // Set and Get data using FieldPath
    QVariant data(const FieldPath& path) const;
    bool set_data(const FieldPath& path, const QVariant& value);

    virtual QVariant data() const = 0;
    virtual bool set_data(const QVariant& value) = 0;

    /**
     * @brief Get the sub model object
     * 
     * @param path 
     * @param for_set_data This is used for setting data in the model only
     *                     in the oneof model. For exmaple, if we want to set 
     *                     a specific field, to get to this field we need to 
     *                     pass on multiple oneofs, so what if one of the oneofs 
     *                     is not set? We need to set it and then move on, otherwise
     *                     this specific oneof will block our way to the desired field.
     * 
     * @param for_get_oneof Currently, the FieldPath class supports getting sub-models 
     *                      for all models except for the oneof model. For example, if we 
     *                      want to get a pointer to a specific repeated field to append 
     *                      a new row, we can use the FieldPath class to get to the repeated,
     *                      however, this won't work for the oneof model. This is because
     *                      if FieldPath::FieldNumber(3) is inside a oneof, I don't skip it, 
     *                      however, I leave it to be skipped inside the OneofModel itself
     *                      and the way I return the sub-models is to check if the FieldPath
     *                      is empty. If you noticed, I don't skip the oneof field number,
     *                      this means every time I reach a oneof model, I will have a non-empty
     *                      FieldPath. So, set this parameter to true if you want to get the
     *                      sub-model of a oneof model.
     * @return const ProtoModel* 
     */
    virtual const ProtoModel* get_sub_model(const FieldPath& path, const bool& for_set_data = false, const bool& for_get_oneof = false) const = 0; // A read-only version of ProtoModel

    virtual int get_oneof_value_field_number() const { return -1; }

    virtual const FieldDescriptor* get_column_descriptor(const int& column) const = 0;

    /**
     * @brief 
     * 
     * @note https://doc.qt.io/qt-5/model-view-programming.html#model-indexes
     * 
     * @param row 
     * @param column 
     * @param parent 
     * @return QModelIndex 
     */
    virtual QModelIndex index([[maybe_unused]] int row, [[maybe_unused]] int column, [[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent([[maybe_unused]] const QModelIndex &child) const override { return QModelIndex(); }
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    virtual bool hasChildren([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override { return false; }
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override = 0;
    virtual QVariant headerData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role = Qt::DisplayRole) const override { return QVariant(); }
    virtual bool setHeaderData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] const QVariant &value, [[maybe_unused]] int role = Qt::EditRole) override { return false; }

    virtual QMap<int, QVariant> itemData([[maybe_unused]] const QModelIndex &index) const override { return QMap<int, QVariant>(); }
    virtual bool setItemData([[maybe_unused]] const QModelIndex &index, [[maybe_unused]] const QMap<int, QVariant> &roles) override { return false; }

    virtual int append_row() { return -1; }
    virtual bool remove_row([[maybe_unused]] const int& row) { return false; }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override { return Qt::NoItemFlags; }

    const ProtoModel* get_root_model() const;
    virtual Message* get_message_buffer() const { return nullptr; }
    const ProtoModel* get_parent_model() const { return m_parent_model; }

    /**
     * @brief Set the index in parent object
     * 
     * @note Only used to update the index of repeated models' children
     *       when a row is removed.
     * 
     * @param index 
     */
    void set_index_in_parent(const int& index) { m_index_in_parent = index; }
    int get_index_in_parent() const { return m_index_in_parent; }

protected:
    const ProtoModel* m_parent_model;
    int m_index_in_parent;

    // Serialization helper
    QByteArray serializeToJson() const;
    bool deserializeFromJson(const QByteArray& jsonData);

    virtual bool insertRows(int row, [[maybe_unused]] int count, const QModelIndex &parent = QModelIndex()) override { return false; }
    virtual bool removeRows(int row, [[maybe_unused]] int count, const QModelIndex &parent = QModelIndex()) override { return false; }
    virtual void clear_sub_models() = 0;
};

#endif // PROTO_MODEL_HPP
