#ifndef PROTO_MODEL_HPP
#define PROTO_MODEL_HPP

#include <QAbstractItemModel>
#include <google/protobuf/message.h>
#include "gui/model/utils/field_path.hpp"
#include "error_macros.hpp"

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

// Check https://protobuf.dev/programming-guides/best-practices/ before adding any new features
// https://doc.qt.io/qt-5/model-view-programming.html#creating-new-models
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

    virtual const ProtoModel* get_sub_model(const FieldPath& path) const = 0; // A read-only version of ProtoModel

    virtual const FieldDescriptor* get_column_descriptor(const int& column) const = 0;

    virtual QModelIndex index([[maybe_unused]] int row, [[maybe_unused]] int column, [[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override { return QModelIndex(); }
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

    virtual bool insertRows(int row, [[maybe_unused]] int count, const QModelIndex &parent = QModelIndex()) override { return insertRow(row, parent); }
    virtual bool removeRows(int row, [[maybe_unused]] int count, const QModelIndex &parent = QModelIndex()) override { return removeRow(row, parent); }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override { 
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), Qt::NoItemFlags);
        auto flags = QAbstractItemModel::flags(index);
        flags |= Qt::ItemIsEditable;
        return flags;
    }

    const ProtoModel* get_root_model() const;
    const ProtoModel* get_parent_model() const { return m_parent_model; }
    int get_index_in_parent() const { return m_index_in_parent; }

protected:
    const ProtoModel* m_parent_model;
    int m_index_in_parent;

    // Serialization helper
    // QByteArray serializeToJson() const;
    // bool deserializeFromJson(const QByteArray& jsonData);

    // Helper methods for traversing fields
    // google::protobuf::Message* traverseToField(google::protobuf::Message* message, const FieldPath& path, int depth = 0) const;
    // const google::protobuf::Message* traverseToField(const google::protobuf::Message* message, const FieldPath& path, int depth = 0) const;
};

#endif // PROTO_MODEL_HPP
