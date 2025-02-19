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

#ifndef MODEL_UTILS_HPP
#define MODEL_UTILS_HPP

#include <google/protobuf/descriptor.h>
#include "error_macros.hpp"
#include "gui/model/schema/visual_shader_nodes.pb.h"

#include "gui/controller/vs_proto_node.hpp"

using namespace gui::model::schema;

namespace shadergen_utils {
inline static bool is_inside_real_oneof(const google::protobuf::FieldDescriptor* field) noexcept {
  // if (!field->containing_oneof()) return false;

  // // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor.real_oneof_decl_count.details
  // int real_oneofs_count {field->containing_type()->real_oneof_decl_count()};

  // // Check if the field is in a real oneof, not a synthetic oneof
  // for (int i = 0; i < real_oneofs_count; ++i) {
  //     // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor.oneof_decl.details
  //     // Real oneofs always come first, so iterating up to real_oneof_decl_cout() will yield all real oneofs.
  //     if (field->containing_oneof() == field->containing_type()->oneof_decl(i)) {
  //         return true;
  //     }
  // }

  // return false;

  SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);
  return field->real_containing_oneof() != nullptr;
}

inline static bool is_valid_enum_index(const google::protobuf::EnumDescriptor* enum_descriptor,
                                       const int& index) noexcept {
  CHECK_PARAM_NULLPTR_NON_VOID(enum_descriptor, false, "Enum descriptor is null.");
  VALIDATE_INDEX_NON_VOID(index, enum_descriptor->value_count(), false, "Invalid enum value index");
  return true;
}

inline static int get_enum_value_by_enum_index(const google::protobuf::EnumDescriptor* enum_descriptor,
                                          const int& index) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_enum_index(enum_descriptor, index), -1, "Invalid enum value index");
  return enum_descriptor->value(index)->number();
}

inline static bool is_valid_enum_value(const google::protobuf::EnumDescriptor* enum_descriptor,
                                       const int& value) noexcept {
  CHECK_PARAM_NULLPTR_NON_VOID(enum_descriptor, false, "Enum descriptor is null.");
  int size {enum_descriptor->value_count()};
  for (int i {0}; i < size; ++i) {
    if (get_enum_value_by_enum_index(enum_descriptor, i) == value) {
      return true;
    }
  }
  return false;
}

inline static std::string get_enum_value_caption_by_value(const google::protobuf::EnumDescriptor* enum_descriptor,
                                                                const int& value) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_enum_value(enum_descriptor, value), "", "Invalid enum value");
  CHECK_CONDITION_TRUE_NON_VOID(
      !enum_descriptor->FindValueByNumber(value)->options().HasExtension(gui::model::schema::value_caption),
      "", "Enum value caption not set");
  return enum_descriptor->FindValueByNumber(value)->options().GetExtension(gui::model::schema::value_caption);
}

inline static std::string get_enum_value_caption_by_index(const google::protobuf::EnumDescriptor* enum_descriptor,
                                                                const int& index) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_enum_index(enum_descriptor, index), "", "Invalid enum value index");
  CHECK_CONDITION_TRUE_NON_VOID(
      !enum_descriptor->value(index)->options().HasExtension(gui::model::schema::value_caption),
      "", "Enum value caption not set");
  return enum_descriptor->value(index)->options().GetExtension(gui::model::schema::value_caption);
}

inline static VisualShaderNodePortType get_enum_value_port_type_by_value(const google::protobuf::EnumDescriptor* enum_descriptor,
                                                                const int& value) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_enum_value(enum_descriptor, value), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid enum value");
  CHECK_CONDITION_TRUE_NON_VOID(
      !enum_descriptor->FindValueByNumber(value)->options().HasExtension(gui::model::schema::value_port_type),
      VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Enum value port type not set");
  return enum_descriptor->FindValueByNumber(value)->options().GetExtension(gui::model::schema::value_port_type);
}

inline static VisualShaderNodePortType get_enum_value_port_type_by_index(const google::protobuf::EnumDescriptor* enum_descriptor,
                                                                const int& index) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_enum_index(enum_descriptor, index), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid enum value index");
  CHECK_CONDITION_TRUE_NON_VOID(
      !enum_descriptor->value(index)->options().HasExtension(gui::model::schema::value_port_type),
      VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Enum value port type not set");
  return enum_descriptor->value(index)->options().GetExtension(gui::model::schema::value_port_type);
}

inline static std::string get_enum_value_name_by_index(const google::protobuf::EnumDescriptor* enum_descriptor,
                                                                const int& index) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_enum_index(enum_descriptor, index), "", "Invalid enum value index");
  CHECK_CONDITION_TRUE_NON_VOID(
      !enum_descriptor->value(index)->options().HasExtension(gui::model::schema::value_name),
      "", "Enum value input name not set");
  return enum_descriptor->value(index)->options().GetExtension(gui::model::schema::value_name);
}

inline static std::shared_ptr<IVisualShaderProtoNode> get_proto_node_by_oneof_value_field_number(const int& oneof_value_field_number) noexcept {
  switch (oneof_value_field_number) {
    case VisualShader::VisualShaderNode::kInputFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeInput>>();
    }
    case VisualShader::VisualShaderNode::kOutputFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeOutput>>();
    }
    case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatConstant>>();
    }
    case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntConstant>>();
    }
    case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntConstant>>();
    }
    case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeBooleanConstant>>();
    }
    case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeColorConstant>>();
    }
    case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec2Constant>>();
    }
    case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec3Constant>>();
    }
    case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec4Constant>>();
    }
    case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatOp>>();
    }
    case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntOp>>();
    }
    case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntOp>>();
    }
    case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorOp>>();
    }
    case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatFunc>>();
    }
    case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntFunc>>();
    }
    case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntFunc>>();
    }
    case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorFunc>>();
    }
    case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeValueNoise>>();
    }
    case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodePerlinNoise>>();
    }
    case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVoronoiNoise>>();
    }
    case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeDotProduct>>();
    }
    case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorLen>>();
    }
    case VisualShader::VisualShaderNode::kClampFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeClamp>>();
    }
    case VisualShader::VisualShaderNode::kStepFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeStep>>();
    }
    case VisualShader::VisualShaderNode::kSmoothStepFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeSmoothStep>>();
    }
    case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorDistance>>();
    }
    case VisualShader::VisualShaderNode::kMixFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeMix>>();
    }
    case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorCompose>>();
    }
    case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorCompose>>();
    }
    case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorCompose>>();
    }
    case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorDecompose>>();
    }
    case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorDecompose>>();
    }
    case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorDecompose>>();
    }
    case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIf>>();
    }
    case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeSwitch>>();
    }
    case VisualShader::VisualShaderNode::kIsFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIs>>();
    }
    case VisualShader::VisualShaderNode::kCompareFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeCompare>>();
    }
    default:
      WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
      break;
  }
  return nullptr;
}
}  // namespace shadergen_utils

#endif  // MODEL_UTILS_HPP
