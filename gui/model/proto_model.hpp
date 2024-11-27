#ifndef PROTO_MODEL_HPP
#define PROTO_MODEL_HPP

#include <QAbstractItemModel>
#include <google/protobuf/message.h>
#include "gui/model/utils/field_path.hpp"
#include "error_macros.hpp"

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

class ProtoModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum {
        INDEX_INVALID = -1,
    };

    explicit ProtoModel(ProtoModel* parent_model = nullptr, const int& index_in_parent = INDEX_INVALID);
    virtual ~ProtoModel() override = default;

    virtual void build_sub_models() = 0;

    // JSON Serialization
    // bool loadFromJson(const QString& filePath);
    // bool saveToJson(const QString& filePath) const;

    void parent_data_changed() const;

    // Set and Get data using FieldPath
    QVariant data(const FieldPath& path) const;
    bool set_data(const FieldPath& path, const QVariant& value);

    virtual QVariant data() const = 0;
    virtual bool set_data(const QVariant& value) = 0;

    virtual ProtoModel* get_sub_model([[maybe_unused]] const FieldPath& path) { return nullptr; }
    virtual const ProtoModel* get_sub_model(const FieldPath& path) const = 0; // A read-only version of ProtoModel

    QVariant data_at_col(const int& col) const { return data(index(0, col, QModelIndex())); }
    bool set_data_at_col(const int& col, const QVariant& value) { return set_data_at_col(0, col, value); }
    bool set_data_at_col(const int& row, const int& col, const QVariant& value) {
        return setData(index(row, col, QModelIndex()), value);
    }

    virtual const FieldDescriptor* get_column_descriptor(const int& column) const = 0;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override { return QModelIndex(); }
    virtual QModelIndex parent(const QModelIndex &child) const override { return QModelIndex(); }
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override { return false; }
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override = 0;
    virtual QVariant headerData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role = Qt::DisplayRole) const override { return QVariant(); }
    virtual bool setHeaderData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] const QVariant &value, [[maybe_unused]] int role = Qt::EditRole) override { return false; }

    virtual QMap<int, QVariant> itemData(const QModelIndex &index) const override { return QMap<int, QVariant>(); }
    virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override { return false; }

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override { return insertRow(row, parent); }
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override { return removeRow(row, parent); }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override { 
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), Qt::NoItemFlags);
        auto flags = QAbstractItemModel::flags(index);
        flags |= Qt::ItemIsEditable;
        return flags;
    }

    ProtoModel* get_parent_model() const { return m_parent_model; }
    int get_index_in_parent() const { return m_index_in_parent; }

protected:
    ProtoModel* m_parent_model;
    int m_index_in_parent;

    // Serialization helper
    // QByteArray serializeToJson() const;
    // bool deserializeFromJson(const QByteArray& jsonData);

    // Helper methods for traversing fields
    // google::protobuf::Message* traverseToField(google::protobuf::Message* message, const FieldPath& path, int depth = 0) const;
    // const google::protobuf::Message* traverseToField(const google::protobuf::Message* message, const FieldPath& path, int depth = 0) const;
};

#endif // PROTO_MODEL_HPP
