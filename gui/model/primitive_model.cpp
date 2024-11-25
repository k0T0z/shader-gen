#include "gui/model/primitive_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

#include "error_macros.hpp"

using namespace google::protobuf;

PrimitiveModel::PrimitiveModel(const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& column_in_parent)
    : ProtoModel(parent_model, column_in_parent), m_field_desc(field_desc) {}

const FieldDescriptor* PrimitiveModel::get_column_descriptor(const int& column) const {
    return m_parent_model->get_column_descriptor(m_column_in_parent);
}

bool PrimitiveModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    return m_parent_model->set_data_at_col(m_column_in_parent, value);
}

const ProtoModel* PrimitiveModel::get_sub_model(const FieldPath& path) const {
    FAIL_AND_RETURN_NON_VOID(nullptr, "Primitive model does not have sub-models.");
}

QVariant PrimitiveModel::data() const {
    return m_parent_model->data_at_col(m_column_in_parent);
}

bool PrimitiveModel::set_data(const QVariant& value) {
    return m_parent_model->set_data_at_col(m_column_in_parent, value);
}

QVariant PrimitiveModel::data(const QModelIndex& index, int role) const {
    return m_parent_model->data_at_col(m_column_in_parent);
}

QModelIndex PrimitiveModel::parent(const QModelIndex& child) const {
    return QModelIndex();
}

QModelIndex PrimitiveModel::index(int row, int column, const QModelIndex& parent) const {
    return this->createIndex(0, 0);
}
