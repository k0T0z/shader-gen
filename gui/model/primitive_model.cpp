#include "gui/model/primitive_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

#include "error_macros.hpp"

using namespace google::protobuf;

PrimitiveModel::PrimitiveModel(FieldDescriptor* field_desc, ProtoModel* parent_model, const int& column_in_parent)
    : ProtoModel(parent_model, column_in_parent), m_field_desc(field_desc) {}

