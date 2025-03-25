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
#include "generator/visual_shader_node_port_type_generator.hpp"
#include "gui/model/utils/field_path.hpp"
#include "gui/model/message_model.hpp"
#include "ai-agent/utils/utils.hpp"

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
    case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorDistance>>();
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

inline static std::shared_ptr<VisualShaderNodePortTypeGenerator> get_port_type_generator(const MessageModel* node_model) noexcept {
  // I don't care about the field number, just send any field number inside the oneof model you want to get
  const ProtoModel* oneof_model{
      node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                false, true)};
  CHECK_PARAM_NULLPTR_NON_VOID(oneof_model, nullptr, "Oneof Model is nullptr.");
  const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

  const std::shared_ptr<IVisualShaderProtoNode> proto_node{get_proto_node_by_oneof_value_field_number(oneof_value_field_number)};

  switch (oneof_value_field_number) {
    case VisualShader::VisualShaderNode::kInputFieldNumber: {
      const ProtoModel* input_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)))};

      const ProtoModel* input_type_model{input_model->get_sub_model(FieldPath::Of<VisualShaderNodeInput>(
        FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)))};

      const VisualShaderNodeInput::VisualShaderNodeInputType input_type{input_type_model->data().toInt()};

      return std::make_shared<VisualShaderNodePortTypeGeneratorInput>(proto_node, input_type);
    }
    case VisualShader::VisualShaderNode::kOutputFieldNumber: {
      return std::make_shared<VisualShaderNodePortTypeGeneratorOutput>(proto_node);
    }
    case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorIntConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorBooleanConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorColorConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVec2Constant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVec3Constant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVec4Constant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
        const ProtoModel* float_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatOpFieldNumber)))};

        const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type {float_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatOp>(FieldPath::FieldNumber(VisualShaderNodeFloatOp::kOpTypeFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
        const ProtoModel* int_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntOpFieldNumber)))};

        const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type {int_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntOp>(FieldPath::FieldNumber(VisualShaderNodeIntOp::kOpTypeFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorIntOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
        const ProtoModel* uint_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintOpFieldNumber)))};

        const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type {uint_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntOp>(FieldPath::FieldNumber(VisualShaderNodeUIntOp::kOpTypeFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
        const ProtoModel* vector_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorOpFieldNumber)))};

        const ProtoModel* vector_type_model{vector_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorOp>(
          FieldPath::FieldNumber(VisualShaderNodeVectorOp::kVecTypeFieldNumber)))};

        const VisualShaderNodeVectorType type {vector_type_model->data().toInt()};
        const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type {vector_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorOp>(FieldPath::FieldNumber(VisualShaderNodeVectorOp::kOpTypeFieldNumber)))->data().toInt()};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorOp>(proto_node, type, op_type);
    }
    case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
        const ProtoModel* float_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatFuncFieldNumber)))};

        const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type {float_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatFunc>(FieldPath::FieldNumber(VisualShaderNodeFloatFunc::kFuncTypeFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
        const ProtoModel* int_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntFuncFieldNumber)))};

        const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type {int_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntFunc>(FieldPath::FieldNumber(VisualShaderNodeIntFunc::kFuncTypeFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorIntFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
        const ProtoModel* uint_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintFuncFieldNumber)))};

        const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type {uint_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntFunc>(FieldPath::FieldNumber(VisualShaderNodeUIntFunc::kFuncTypeFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
        const ProtoModel* vector_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorFuncFieldNumber)))};

        const ProtoModel* vector_type_model{vector_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(
          FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kVecTypeFieldNumber)))};

        const VisualShaderNodeVectorType type {vector_type_model->data().toInt()};
        const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type {vector_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kFuncTypeFieldNumber)))->data().toInt()};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorFunc>(proto_node, type, func_type);
    }
    case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorValueNoise>(proto_node);
    }
    case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorPerlinNoise>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVoronoiNoise>(proto_node);
    }
    case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorDotProduct>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
        const ProtoModel* vector_len_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorLenFieldNumber)))};

        const ProtoModel* vector_type_model{vector_len_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorLen>(
          FieldPath::FieldNumber(VisualShaderNodeVectorLen::kVecTypeFieldNumber)))};

        const VisualShaderNodeVectorType type {vector_type_model->data().toInt()};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorLen>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kClampFieldNumber: {
        const ProtoModel* clamp_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kClampFieldNumber)))};

        const ProtoModel* clamp_type_model{clamp_model->get_sub_model(FieldPath::Of<VisualShaderNodeClamp>(
          FieldPath::FieldNumber(VisualShaderNodeClamp::kTypeFieldNumber)))};

        const VisualShaderNodeClamp::VisualShaderNodeClampType type {clamp_type_model->data().toInt()};

        return std::make_shared<VisualShaderNodePortTypeGeneratorClamp>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
        const ProtoModel* vector_distance_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorDistanceFieldNumber)))};

        const ProtoModel* vector_distance_type_model{vector_distance_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorDistance>(
          FieldPath::FieldNumber(VisualShaderNodeVectorDistance::kVecTypeFieldNumber)))};

        const VisualShaderNodeVectorType type {vector_distance_type_model->data().toInt()};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDistance>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorCompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_2D);
    }
    case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorCompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_3D);
    }
    case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorCompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_4D);
    }
    case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDecompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_2D);
    }
    case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDecompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_3D);
    }
    case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDecompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_4D);
    }
    case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorIf>(proto_node);
    }
    case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
        const ProtoModel* switch_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber)))};

        const ProtoModel* switch_type_model{switch_model->get_sub_model(FieldPath::Of<VisualShaderNodeSwitch>(
          FieldPath::FieldNumber(VisualShaderNodeSwitch::kTypeFieldNumber)))};

        const VisualShaderNodeSwitch::VisualShaderNodeSwitchType type {switch_type_model->data().toInt()};

        return std::make_shared<VisualShaderNodePortTypeGeneratorSwitch>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kIsFieldNumber: {
        const ProtoModel* is_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIsFieldNumber)))};

        const VisualShaderNodeIs::VisualShaderNodeIsFunction func {is_model->get_sub_model(FieldPath::Of<VisualShaderNodeIs>(FieldPath::FieldNumber(VisualShaderNodeIs::kFuncFieldNumber)))->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorIs>(proto_node, func);
    }
    case VisualShader::VisualShaderNode::kCompareFieldNumber: {
        const ProtoModel* compare_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
          FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber)))};

        const ProtoModel* compare_type_model{compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(
          FieldPath::FieldNumber(VisualShaderNodeCompare::kTypeFieldNumber)))};

        const VisualShaderNodeCompare::VisualShaderNodeCompareType type {compare_type_model->data().toInt()};
        return std::make_shared<VisualShaderNodePortTypeGeneratorCompare>(proto_node, type);
    }
    default:
      WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
      break;
  }
  return nullptr;
}

inline static std::shared_ptr<VisualShaderNodePortTypeGenerator> get_port_type_generator(const std::string& node_entity) noexcept {
  const std::vector<std::string> entity_tokens{ai_agent_utils::split_string(node_entity, ';')};
  const int entity_type = std::stoi(ai_agent_utils::get_entity_type(entity_tokens));
  CHECK_CONDITION_TRUE_NON_VOID(entity_type != 0, nullptr, "Wrong entity type.");
  const int oneof_value_field_number = std::stoi(ai_agent_utils::get_node_entity_oneof_value_field_number(entity_tokens));
  const std::vector<std::string> parameters{ai_agent_utils::get_node_entity_parameters(entity_tokens)};
  
  const std::shared_ptr<IVisualShaderProtoNode> proto_node{get_proto_node_by_oneof_value_field_number(oneof_value_field_number)};

  switch (oneof_value_field_number) {
    case VisualShader::VisualShaderNode::kInputFieldNumber: {
      const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
      const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
      CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeInput::kTypeFieldNumber, nullptr, "Wrong field number.");
      const VisualShaderNodeInput::VisualShaderNodeInputType input_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

      return std::make_shared<VisualShaderNodePortTypeGeneratorInput>(proto_node, input_type);
    }
    case VisualShader::VisualShaderNode::kOutputFieldNumber: {
      return std::make_shared<VisualShaderNodePortTypeGeneratorOutput>(proto_node);
    }
    case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorIntConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorBooleanConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorColorConstant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVec2Constant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVec3Constant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVec4Constant>(proto_node);
    }
    case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeFloatOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeIntOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorIntOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeUIntOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorOp::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        const std::vector<std::string> parameter_tokens1{ai_agent_utils::split_string(parameters.at(1), '=')};
        const int field_number1 = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens1));
        CHECK_CONDITION_TRUE_NON_VOID(field_number1 != VisualShaderNodeVectorOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens1))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorOp>(proto_node, type, op_type);
    }
    case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeFloatFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeIntFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorIntFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeUIntFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorFunc::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        const std::vector<std::string> parameter_tokens1{ai_agent_utils::split_string(parameters.at(1), '=')};
        const int field_number1 = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens1));
        CHECK_CONDITION_TRUE_NON_VOID(field_number1 != VisualShaderNodeVectorFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens1))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorFunc>(proto_node, type, func_type);
    }
    case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorValueNoise>(proto_node);
    }
    case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorPerlinNoise>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVoronoiNoise>(proto_node);
    }
    case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorDotProduct>(proto_node);
    }
    case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorLen::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorLen>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kClampFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeClamp::kTypeFieldNumber, nullptr, "Wrong field number."); 
        const VisualShaderNodeClamp::VisualShaderNodeClampType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorClamp>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorDistance::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDistance>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorCompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_2D);
    }
    case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorCompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_3D);
    }
    case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorCompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_4D);
    }
    case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDecompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_2D);
    }
    case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDecompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_3D);
    }
    case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorDecompose>(proto_node, VisualShaderNodeVectorType::TYPE_VECTOR_4D);
    }
    case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
        return std::make_shared<VisualShaderNodePortTypeGeneratorIf>(proto_node);
    }
    case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeSwitch::kTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeSwitch::VisualShaderNodeSwitchType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorSwitch>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kIsFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeIs::kFuncFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeIs::VisualShaderNodeIsFunction func{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorIs>(proto_node, func);
    }
    case VisualShader::VisualShaderNode::kCompareFieldNumber: {
        const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeCompare::kTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeCompare::VisualShaderNodeCompareType type{std::stoi(ai_agent_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorCompare>(proto_node, type);
    }
    default:
      WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
      break;
  }
  return nullptr;
}
}  // namespace shadergen_utils

#endif  // MODEL_UTILS_HPP
