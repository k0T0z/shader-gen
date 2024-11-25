#ifndef PRIMITIVE_MODEL_HPP
#define PRIMITIVE_MODEL_HPP

#include <vector>
#include <memory>
#include <google/protobuf/message.h>
#include "gui/model/proto_model.hpp"

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

class PrimitiveModel : public ProtoModel {
    Q_OBJECT

public:
    PrimitiveModel(const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& column_in_parent);

    QVariant data() const override;
    bool set_data(const QVariant& value) override;
    
    const ProtoModel* get_sub_model(const FieldPath& path) const override;

    const FieldDescriptor* get_column_descriptor(const int& column) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override { return 1; }
    int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 1; }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole) override;

    const FieldDescriptor* get_field_descriptor() const { return m_field_desc; }

private:
    const FieldDescriptor* m_field_desc;
};

#endif // PRIMITIVE_MODEL_HPP
