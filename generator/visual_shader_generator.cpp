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

#include "generator/visual_shader_generator.hpp"

#include <algorithm>
#include <iomanip>
#include <memory>
#include <sstream>

#include "error_macros.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "generator/vs_node_noise_generators.hpp"
#include "gui/model/utils/utils.hpp"

namespace shadergen_visual_shader_generator {
std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>> to_proto_nodes(const ProtoModel* nodes) noexcept {
  int size{nodes->rowCount()};
  std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>> proto_nodes;

  // Cast to ReapeatedMessageModel
  const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
  CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, proto_nodes, "Nodes is not a repeated message model.");

  for (int i{0}; i < size; ++i) {
    const MessageModel* node_model{repeated_nodes->get_sub_model(i)};

    const int n_id{node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
        FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)))->data().toInt()};

    if (proto_nodes.find(n_id) != proto_nodes.end()) {
      FAIL_AND_RETURN_NON_VOID(proto_nodes, "Node id already exists.");
    }

    // I don't care about the field number, just send any field number inside the oneof model you want to get
    const ProtoModel* oneof_model{
        node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                      FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                  false, true)};
    FAIL_AND_RETURN_NON_VOID(proto_nodes, "Oneof Model is nullptr.");
    const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

    switch (oneof_value_field_number) {
      case VisualShader::VisualShaderNode::kInputFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeInput>>();
        break;
      }
      case VisualShader::VisualShaderNode::kOutputFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeOutput>>();
        break;
      }
      case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatConstant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntConstant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntConstant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeBooleanConstant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeColorConstant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec2Constant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec3Constant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec4Constant>>();
        break;
      }
      case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatOp>>();
        break;
      }
      case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntOp>>();
        break;
      }
      case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntOp>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorOp>>();
        break;
      }
      case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatFunc>>();
        break;
      }
      case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntFunc>>();
        break;
      }
      case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntFunc>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorFunc>>();
        break;
      }
      case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeValueNoise>>();
        break;
      }
      case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodePerlinNoise>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVoronoiNoise>>();
        break;
      }
      case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeDotProduct>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorLen>>();
        break;
      }
      case VisualShader::VisualShaderNode::kClampFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeClamp>>();
        break;
      }
      case VisualShader::VisualShaderNode::kStepFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeStep>>();
        break;
      }
      case VisualShader::VisualShaderNode::kSmoothStepFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeSmoothStep>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorDistance>>();
        break;
      }
      case VisualShader::VisualShaderNode::kMixFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeMix>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorCompose>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorCompose>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorCompose>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorDecompose>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorDecompose>>();
        break;
      }
      case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorDecompose>>();
        break;
      }
      case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeIf>>();
        break;
      }
      case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeSwitch>>();
        break;
      }
      case VisualShader::VisualShaderNode::kIsFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeIs>>();
        break;
      }
      case VisualShader::VisualShaderNode::kCompareFieldNumber: {
        proto_nodes[n_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeCompare>>();
        break;
      }
      default:
        WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
        break;
    }
  }

  return proto_nodes;
}

std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>> to_generators(const ProtoModel* nodes) noexcept {
  int size{nodes->rowCount()};
  std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>> generators;

  // Cast to ReapeatedMessageModel
  const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
  CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, generators, "Nodes is not a repeated message model.");

  for (int i{0}; i < size; ++i) {
    const MessageModel* node_model{repeated_nodes->get_sub_model(i)};

    const int n_id{node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
        FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)))->data().toInt()};

    if (generators.find(n_id) != generators.end()) {
      FAIL_AND_RETURN_NON_VOID(generators, "Node ID already exists in the generators map.");
    }

    // I don't care about the field number, just send any field number inside the oneof model you want to get
    const ProtoModel* oneof_model{
        node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                      FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                  false, true)};
    FAIL_AND_RETURN_NON_VOID(generators, "Oneof Model is nullptr.");
    const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

    switch (oneof_value_field_number) {
      case VisualShader::VisualShaderNode::kInputFieldNumber: {
        const ProtoModel* input_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)))};

        const VisualShaderNodeInputType input_type{input_model->get_sub_model(FieldPath::Of<VisualShaderNodeInput>(
            FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)))->data().toInt()};
        generators[n_id] = std::make_shared<VisualShaderNodeGeneratorInput>(input_type);
        break;
      }
      case VisualShader::VisualShaderNode::kOutputFieldNumber: {
        generators[n_id] = std::make_shared<VisualShaderNodeGeneratorOutput>();
        break;
      }
      case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
          const ProtoModel* float_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatConstantFieldNumber)))};

          const float value {float_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatConstant>(FieldPath::FieldNumber(VisualShaderNodeFloatConstant::kValueFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorFloatConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
          const ProtoModel* int_model_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntConstantFieldNumber)))};

          const int value {int_model_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntConstant>(FieldPath::FieldNumber(VisualShaderNodeIntConstant::kValueFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
          const ProtoModel* uint_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintConstantFieldNumber)))};

          const unsigned int value {uint_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntConstant>(FieldPath::FieldNumber(VisualShaderNodeUIntConstant::kValueFieldNumber)))->data().toUInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorUIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
          const ProtoModel* bool_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber)))};

          const bool value {bool_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeBooleanConstant>(FieldPath::FieldNumber(VisualShaderNodeBooleanConstant::kValueFieldNumber)))->data().toBool()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorBoolConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
          const ProtoModel* color_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber)))};

          const float r {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kRFieldNumber)))->data().toFloat()};
          const float g {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kGFieldNumber)))->data().toFloat()};
          const float b {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kBFieldNumber)))->data().toFloat()};
          const float a {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kAFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorColorConstant>(r, g, b, a);
          break;
      }
      case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
          const ProtoModel* vec2_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber)))};

          const float x {vec2_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec2Constant>(FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kXFieldNumber)))->data().toFloat()};
          const float y {vec2_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec2Constant>(FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kYFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVec2Constant>(x, y);
          break;
      }
      case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
          const ProtoModel* vec3_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber)))};

          const float x {vec3_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kXFieldNumber)))->data().toFloat()};
          const float y {vec3_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kYFieldNumber)))->data().toFloat()};
          const float z {vec3_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kZFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVec3Constant>(x, y, z);
          break;
      }
      case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
          const ProtoModel* vec4_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber)))};

          const float x {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kXFieldNumber)))->data().toFloat()};
          const float y {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kYFieldNumber)))->data().toFloat()};
          const float z {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kZFieldNumber)))->data().toFloat()};
          const float w {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kWFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVec4Constant>(x, y, z, w);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
          const ProtoModel* float_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatOpFieldNumber)))};

          const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type {float_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatOp>(FieldPath::FieldNumber(VisualShaderNodeFloatOp::kOpFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorFloatOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
          const ProtoModel* int_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntOpFieldNumber)))};

          const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type {int_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntOp>(FieldPath::FieldNumber(VisualShaderNodeIntOp::kOpFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIntOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
          const ProtoModel* uint_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintOpFieldNumber)))};

          const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type {uint_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntOp>(FieldPath::FieldNumber(VisualShaderNodeUIntOp::kOpFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorUIntOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
          const ProtoModel* vector_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorOpFieldNumber)))};

          const VisualShaderNodeVectorType type {vector_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorOp>(FieldPath::FieldNumber(VisualShaderNodeVectorOp::kTypeFieldNumber)))->data().toInt()};
          const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type {vector_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorOp>(FieldPath::FieldNumber(VisualShaderNodeVectorOp::kOpFieldNumber)))->data().toInt()};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorOp>(type, op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
          const ProtoModel* float_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatFuncFieldNumber)))};

          const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type {float_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatFunc>(FieldPath::FieldNumber(VisualShaderNodeFloatFunc::kFuncFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorFloatFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
          const ProtoModel* int_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntFuncFieldNumber)))};

          const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type {int_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntFunc>(FieldPath::FieldNumber(VisualShaderNodeIntFunc::kFuncFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIntFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
          const ProtoModel* uint_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintFuncFieldNumber)))};

          const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type {uint_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntFunc>(FieldPath::FieldNumber(VisualShaderNodeUIntFunc::kFuncFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorUIntFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
          const ProtoModel* vector_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorFuncFieldNumber)))};

          const VisualShaderNodeVectorType type {vector_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kTypeFieldNumber)))->data().toInt()};
          const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type {vector_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kFuncFieldNumber)))->data().toInt()};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorFunc>(type, func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
          const ProtoModel* value_noise_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kValueNoiseFieldNumber)))};

          const float scale {value_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodeValueNoise>(FieldPath::FieldNumber(VisualShaderNodeValueNoise::kScaleFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorValueNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
            const ProtoModel* perlin_noise_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                FieldPath::FieldNumber(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber)))};

            const float scale {perlin_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodePerlinNoise>(FieldPath::FieldNumber(VisualShaderNodePerlinNoise::kScaleFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorPerlinNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
          const ProtoModel* voronoi_noise_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber)))};

          const float angle_offset {voronoi_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodeVoronoiNoise>(FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber)))->data().toFloat()};
          const float cell_density {voronoi_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodeVoronoiNoise>(FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber)))->data().toFloat()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVoronoiNoise>(angle_offset, cell_density);
          break;
      }
      case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorDotProduct>();
          break;
      }
      case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorLen>();
          break;
      }
      case VisualShader::VisualShaderNode::kClampFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorClamp>();
          break;
      }
      case VisualShader::VisualShaderNode::kStepFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorStep>();
          break;
      }
      case VisualShader::VisualShaderNode::kSmoothStepFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorSmoothStep>();
          break;
      }
      case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorDistance>();
          break;
      }
      case VisualShader::VisualShaderNode::kMixFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorMix>();
          break;
      }
      case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorCompose>(VisualShaderNodeVectorType::TYPE_VECTOR_2D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorCompose>(VisualShaderNodeVectorType::TYPE_VECTOR_3D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorCompose>(VisualShaderNodeVectorType::TYPE_VECTOR_4D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorDecompose>(VisualShaderNodeVectorType::TYPE_VECTOR_2D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorDecompose>(VisualShaderNodeVectorType::TYPE_VECTOR_3D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorDecompose>(VisualShaderNodeVectorType::TYPE_VECTOR_4D);
          break;
      }
      case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIf>();
          break;
      }
      case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
          const ProtoModel* switch_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber)))};

          const VisualShaderNodeSwitch::VisualShaderNodeSwitchOpType type {switch_model->get_sub_model(FieldPath::Of<VisualShaderNodeSwitch>(FieldPath::FieldNumber(VisualShaderNodeSwitch::kTypeFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorSwitch>(type);
          break;
      }
      case VisualShader::VisualShaderNode::kIsFieldNumber: {
          const ProtoModel* is_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIsFieldNumber)))};

          const VisualShaderNodeIs::Function func {is_model->get_sub_model(FieldPath::Of<VisualShaderNodeIs>(FieldPath::FieldNumber(VisualShaderNodeIs::kFuncFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIs>(func);
          break;
      }
      case VisualShader::VisualShaderNode::kCompareFieldNumber: {
          const ProtoModel* compare_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber)))};

          const VisualShaderNodeCompare::ComparisonType type {compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(FieldPath::FieldNumber(VisualShaderNodeCompare::kTypeFieldNumber)))->data().toInt()};
            const VisualShaderNodeCompare::Function func {compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(FieldPath::FieldNumber(VisualShaderNodeCompare::kFuncFieldNumber)))->data().toInt()};
            const VisualShaderNodeCompare::Condition cond {compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(FieldPath::FieldNumber(VisualShaderNodeCompare::kCondFieldNumber)))->data().toInt()};
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorCompare>(type, func, cond);
          break;
      }
      default:
        WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
        break;
    }
  }

  return generators;
}

std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>> to_input_output_connections_by_key(const ProtoModel* connections) noexcept {
  std::map<ConnectionKey, std::shared_ptr<Connection>> input_connections;
  std::map<ConnectionKey, std::shared_ptr<Connection>> output_connections;

  int size{connections->rowCount()};

  // Cast to ReapeatedMessageModel
  const RepeatedMessageModel* repeated_connections{dynamic_cast<const RepeatedMessageModel*>(connections)};
  CHECK_PARAM_NULLPTR_NON_VOID(repeated_connections, std::make_pair(input_connections, output_connections), "Connections is not a repeated message model.");

  for (int i{0}; i < size; ++i) {
    const MessageModel* connection_model{repeated_connections->get_sub_model(i)};

    std::shared_ptr<Connection> c = std::make_shared<Connection>();
    c->from.f_key.node = connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
        FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber)))->data().toInt();
    c->from.f_key.port = connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
        FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber)))->data().toInt();
    c->to.f_key.node = connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
        FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber)))->data().toInt();
    c->to.f_key.port = connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
        FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber)))->data().toInt();

    ConnectionKey from_key;
    from_key.f_key.node = c->from.f_key.node;
    from_key.f_key.port = c->from.f_key.port;

    output_connections[from_key] = c;

    ConnectionKey to_key;
    to_key.f_key.node = c->to.f_key.node;
    to_key.f_key.port = c->to.f_key.port;

    input_connections[to_key] = c;
  }

  return std::make_pair(input_connections, output_connections);
}

// Define generate_shader_for_each_node to use it in generate_shader
static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                 std::string& func_code,
                                                 const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes,
                                                 const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& input_connections,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& output_connections,
                                                 const int& node_id, 
                                                 std::unordered_set<int>& processed,
                                                 std::unordered_set<std::string>& global_processed) noexcept;

bool generate_shader(const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes, 
                     const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators, 
                     const std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>>& input_output_connections_by_key, 
                     std::string& code_buffer) noexcept {
  static const std::string func_name{"main"};   

  std::string global_code;
  std::string global_code_per_node;
  std::string shader_code;
  std::unordered_set<std::string> global_processed;

  std::string func_code;
  std::unordered_set<int> processed;

  func_code += "\nvoid " + func_name + "() {" + std::string("\n");

  // Generate the code for each node.
  bool status{generate_shader_for_each_node(global_code, 
                                            global_code_per_node, 
                                            func_code, 
                                            proto_nodes, 
                                            generators, 
                                            input_output_connections_by_key.first,
                                            input_output_connections_by_key.second, 0, 
                                            processed,
                                            global_processed)};

  if (!status) {
    std::cout << "Failed to generate shader for node " << 0 << std::endl;
    return false;
  }

  func_code += std::string("}") + "\n\n";
  shader_code += func_code;

  std::string generated_code{global_code};
  generated_code += global_code_per_node;

  generated_code += shader_code;

  code_buffer = generated_code;

  return true;
}

std::string generate_preview_shader(const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes, 
                                    const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators, 
                                    const std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>>& input_output_connections_by_key, 
                                    const int& node_id, const int& port) noexcept { 
  static const std::string func_name{"main"};
  static const std::string output_var{"FragColor"};

  const std::shared_ptr<IVisualShaderProtoNode> proto_node{proto_nodes.at(node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(proto_node, std::string(), "Proto node is null.");

  std::string global_code;
  std::string global_code_per_node;
  std::string shader_code;
  std::unordered_set<std::string> global_processed;

  std::unordered_set<int> processed;

  shader_code += "\nvoid " + func_name + "() {" + std::string("\n");

  // Generate the code for each node.
  bool status{generate_shader_for_each_node(global_code, 
                                            global_code_per_node, 
                                            shader_code, 
                                            proto_nodes, 
                                            generators, 
                                            input_output_connections_by_key.first,
                                            input_output_connections_by_key.second, 
                                            node_id,
                                            processed,
                                            global_processed)};
  if (!status) {
    std::cout << "Failed to generate shader for node " << node_id << std::endl;
    return std::string();
  }

  global_code += "out vec4 " + output_var + ";" + std::string("\n");

  VisualShaderNodePortType from_port_type{VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED};

  if (auto input_node = std::dynamic_pointer_cast<VisualShaderProtoNode<VisualShaderNodeInput>>(proto_node)) {
    // For Input node, type is by input type
    const std::shared_ptr<VisualShaderNodeGenerator> generator{generators.at(node_id)};
    from_port_type = shadergen_utils::get_enum_value_port_type_by_value(VisualShaderNodeInputType_descriptor(), generator->get_input_type());
  } else {
    from_port_type = proto_node->get_output_port_type(port);
  }

  switch (from_port_type) {
    case VisualShaderNodePortType::PORT_TYPE_SCALAR:
      shader_code += std::string("\t") + output_var + " = vec4(vec3(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + "), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
      shader_code += std::string("\t") + output_var + " = vec4(vec3(float(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + ")), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
      shader_code += std::string("\t") + output_var + " = vec4(vec3(float(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + ")), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
      shader_code += std::string("\t") + output_var + " = vec4(vec3(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + " ? 1.0 : 0.0), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
      shader_code += std::string("\t") + output_var + " = vec4(vec3(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + ", 0.0), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
      shader_code += std::string("\t") + output_var + " = vec4(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + ", 1.0);" + std::string("\n");
      break;
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
      shader_code += std::string("\t") + output_var + " = vec4(var_from_n" + std::to_string(node_id) + "_p" +
                     std::to_string(port) + ".xyz, 1.0);" + std::string("\n");
      break;
    default:
      shader_code += std::string("\t") + output_var + " = vec4(vec3(0.0), 1.0);" + std::string("\n");
      break;
  }

  shader_code += std::string("}") + "\n\n";

  std::string generated_code{global_code};
  generated_code += global_code_per_node;

  generated_code += shader_code;

  return generated_code;
}

static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                 std::string& func_code,
                                                 const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes,
                                                 const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& input_connections,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& output_connections,
                                                 const int& node_id, 
                                                 std::unordered_set<int>& processed,
                                                 std::unordered_set<std::string>& global_processed) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(proto_nodes.find(node_id) == proto_nodes.end(), false, "Node id not found in proto nodes.");
  CHECK_CONDITION_TRUE_NON_VOID(generators.find(node_id) == generators.end(), false, "Node id not found in generators.");

  const std::shared_ptr<IVisualShaderProtoNode> proto_node{proto_nodes.at(node_id)};
  const std::shared_ptr<VisualShaderNodeGenerator> generator{generators.at(node_id)};

  // Check inputs recursively.
  int input_port_count{proto_node->get_input_port_count()};
  for (int i{0}; i < input_port_count; i++) {
    ConnectionKey key;
    key.f_key.node = node_id;
    key.f_key.port = i;

    if (input_connections.find(key) == input_connections.end()) {
      continue;
    }

    const std::shared_ptr<Connection> c{input_connections.at(key)};

    int from_node{(int)c->from.f_key.node};

    if (processed.find(from_node) != processed.end()) {
      continue;
    }

    bool status{generate_shader_for_each_node(global_code, 
                                              global_code_per_node, 
                                              func_code, 
                                              proto_nodes, 
                                              generators, 
                                              input_connections,
                                              output_connections, 
                                              from_node,
                                              processed,
                                              global_processed)};
    if (!status) {
      std::cout << "Failed to generate shader for node " << from_node << std::endl;
      return false;
    }
  }

  // Make sure not to generate global code for the same node type more than once.
  std::string proto_name{proto_node->get_name()};
  if (global_processed.find(proto_name) == global_processed.end()) {
    global_code += generator->generate_global(node_id);
    global_code_per_node += generator->generate_global_per_node(node_id);
  }
  global_processed.insert(proto_name);

  // Generate the code for the current node.
  std::string node_name{"// " + proto_node->get_caption() + ":" + std::to_string(node_id) + "\n"};
  std::string node_code;
  std::vector<std::string> input_vars;

  input_vars.resize(proto_node->get_input_port_count());

  for (int i{0}; i < input_port_count; i++) {
    ConnectionKey key;
    key.f_key.node = node_id;
    key.f_key.port = i;

    // Check if the input is not connected.
    if (input_connections.find(key) != input_connections.end()) {
      const std::shared_ptr<Connection> c{input_connections.at(key)};

      int from_node{(int)c->from.f_key.node};
      int from_port{(int)c->from.f_key.port};

      VisualShaderNodePortType to_port_type{proto_node->get_input_port_type(i)};
      VisualShaderNodePortType from_port_type{VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED};

      if (auto input_node = std::dynamic_pointer_cast<VisualShaderProtoNode<VisualShaderNodeInput>>(proto_nodes.at(from_node))) {
        // For Input node, type is by input type
        const std::shared_ptr<VisualShaderNodeGenerator> t_generator{generators.at(from_node)};
        from_port_type = shadergen_utils::get_enum_value_port_type_by_value(VisualShaderNodeInputType_descriptor(), t_generator->get_input_type());
      } else {
        from_port_type = proto_nodes.at(from_node)->get_output_port_type(from_port);
      }

      std::string from_var{"var_from_n" + std::to_string(from_node) + "_p" + std::to_string(from_port)};

      if (to_port_type == from_port_type) {
        input_vars.at(i) = from_var;
      } else {
        switch (to_port_type) {
          case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
                input_vars.at(i) = "float(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
                input_vars.at(i) = "float(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
                input_vars.at(i) = "(" + from_var + " ? 1.0 : 0.0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
                input_vars.at(i) = from_var + ".x";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
                input_vars.at(i) = from_var + ".x";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
                input_vars.at(i) = from_var + ".x";
              } break;
              default:
                break;
            }
          } break;
          case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
                input_vars.at(i) = "int(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
                input_vars.at(i) = "int(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
                input_vars.at(i) = "(" + from_var + " ? 1 : 0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
                input_vars.at(i) = "int(" + from_var + ".x)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
                input_vars.at(i) = "int(" + from_var + ".x)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
                input_vars.at(i) = "int(" + from_var + ".x)";
              } break;
              default:
                break;
            }
          } break;
          case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
                input_vars.at(i) = "uint(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
                input_vars.at(i) = "uint(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
                input_vars.at(i) = "(" + from_var + " ? 1u : 0u)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
                input_vars.at(i) = "uint(" + from_var + ".x)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
                input_vars.at(i) = "uint(" + from_var + ".x)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
                input_vars.at(i) = "uint(" + from_var + ".x)";
              } break;
              default:
                break;
            }
          } break;
          case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
                input_vars.at(i) = from_var + " > 0.0 ? true : false";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
                input_vars.at(i) = from_var + " > 0 ? true : false";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
                input_vars.at(i) = from_var + " > 0u ? true : false";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
                input_vars.at(i) = "all(bvec2(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
                input_vars.at(i) = "all(bvec3(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
                input_vars.at(i) = "all(bvec4(" + from_var + "))";
              } break;
              default:
                break;
            }
          } break;
          case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
                input_vars.at(i) = "vec2(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
                input_vars.at(i) = "vec2(float(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
                input_vars.at(i) = "vec2(float(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
                input_vars.at(i) = "vec2(" + from_var + " ? 1.0 : 0.0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
                input_vars.at(i) = "vec2(" + from_var + ".xy)";
              } break;
              default:
                break;
            }
          } break;
          case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
                input_vars.at(i) = "vec3(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
                input_vars.at(i) = "vec3(float(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
                input_vars.at(i) = "vec3(float(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
                input_vars.at(i) = "vec3(" + from_var + " ? 1.0 : 0.0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
                input_vars.at(i) = "vec3(" + from_var + ", 0.0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
                input_vars.at(i) = "vec3(" + from_var + ".xyz)";
              } break;
              default:
                break;
            }
          } break;
          case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
            switch (from_port_type) {
              case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
                input_vars.at(i) = "vec4(" + from_var + ")";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
                input_vars.at(i) = "vec4(float(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
                input_vars.at(i) = "vec4(float(" + from_var + "))";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
                input_vars.at(i) = "vec4(" + from_var + " ? 1.0 : 0.0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
                input_vars.at(i) = "vec4(" + from_var + ", 0.0, 1.0)";
              } break;
              case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
                input_vars.at(i) = "vec4(" + from_var + ", 1.0)";
              } break;
              default:
                break;
            }
          } break;
          default:
            break;
        }  // end of switch (to_port_type)
      }  // end of if (to_port_type == from_port_type)
    } else {
      // Add the default value.

      // For Output node, type is by port
      switch (proto_node->get_input_port_type(i)) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR: {
          float val{0.0f};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          std::ostringstream oss;
          oss << std::string("\t") + "float " << input_vars.at(i) << " = " << std::fixed << std::setprecision(5) << val
              << ";" << std::string("\n");
          node_code += oss.str();
        } break;
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT: {
          int val{0};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          node_code +=
                std::string("\t") + "int " + input_vars.at(i) + " = " + std::to_string(val) + ";" + std::string("\n");
        } break;
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT: {
          unsigned val{0u};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          node_code +=
                std::string("\t") + "uint " + input_vars.at(i) + " = " + std::to_string(val) + "u;" + std::string("\n");
        } break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D: {
          float x{0.0f}, y{0.0f};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          std::ostringstream oss;
          oss << std::string("\t") + "vec2 " << input_vars.at(i) << " = " << std::fixed << std::setprecision(5)
              << "vec2(" << x << ", " << y << ");" << std::string("\n");
          node_code += oss.str();
        } break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D: {
          float x{0.0f}, y{0.0f}, z{0.0f};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          std::ostringstream oss;
          oss << std::string("\t") + "vec3 " << input_vars.at(i) << " = " << std::fixed << std::setprecision(5)
              << "vec3(" << x << ", " << y << ", " << z << ");" << std::string("\n");
          node_code += oss.str();
        } break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D: {
          float x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          std::ostringstream oss;
          oss << std::string("\t") + "vec4 " << input_vars.at(i) << " = " << std::fixed << std::setprecision(5)
              << "vec4(" << x << ", " << y << ", " << z << ", " << w << ");" << std::string("\n");
          node_code += oss.str();
        } break;
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN: {
          bool val{false};
          input_vars.at(i) = "var_to_n" + std::to_string(node_id) + "_p" + std::to_string(i);
          node_code += std::string("\t") + "bool " + input_vars.at(i) + " = " + (val ? "true" : "false") + ";" +
                       std::string("\n");
        } break;
        default:
          break;
      }  // end of switch
    }  // end of else
  }  // end of for (int i = 0; i < input_port_count; i++)

  int output_port_count{proto_node->get_output_port_count()};

  std::vector<std::string> output_vars;
  output_vars.resize(output_port_count);

  if (generator->is_simple_decl()) {
    // Generate less code for some simple_decl nodes.
    for (int i{0}; i < output_port_count; i++) {
      std::string from_var{"var_from_n" + std::to_string(node_id) + "_p" + std::to_string(i)};

      VisualShaderNodePortType from_port_type{VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED};

      if (auto input_node = std::dynamic_pointer_cast<VisualShaderProtoNode<VisualShaderNodeInput>>(proto_node)) {
        // For Input node, type is by input type
        from_port_type = shadergen_utils::get_enum_value_port_type_by_value(VisualShaderNodeInputType_descriptor(), generator->get_input_type());
      } else {
        from_port_type = proto_node->get_output_port_type(i);
      }

      switch (from_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
          output_vars.at(i) = "float " + from_var;
          break;
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
          output_vars.at(i) = "int " + from_var;
          break;
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
          output_vars.at(i) = "uint " + from_var;
          break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
          output_vars.at(i) = "vec2 " + from_var;
          break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
          output_vars.at(i) = "vec3 " + from_var;
          break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
          output_vars.at(i) = "vec4 " + from_var;
          break;
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          output_vars.at(i) = "bool " + from_var;
          break;
        default:
          break;
      }
    }
  } else {
    for (int i{0}; i < output_port_count; i++) {
      output_vars.at(i) = "var_from_n" + std::to_string(node_id) + "_p" + std::to_string(i);

      VisualShaderNodePortType from_port_type{VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED};

      if (auto input_node = std::dynamic_pointer_cast<VisualShaderProtoNode<VisualShaderNodeInput>>(proto_node)) {
        // For Input node, type is by input type
        from_port_type = shadergen_utils::get_enum_value_port_type_by_value(VisualShaderNodeInputType_descriptor(), generator->get_input_type());
      } else {
        from_port_type = proto_node->get_output_port_type(i);
      }

      switch (from_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
          func_code += std::string("\t") + "float " + output_vars.at(i) + ";" + std::string("\n");
          break;
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
          func_code += std::string("\t") + "int " + output_vars.at(i) + ";" + std::string("\n");
          break;
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
          func_code += std::string("\t") + "uint " + output_vars.at(i) + ";" + std::string("\n");
          break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
          func_code += std::string("\t") + "vec2 " + output_vars.at(i) + ";" + std::string("\n");
          break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
          func_code += std::string("\t") + "vec3 " + output_vars.at(i) + ";" + std::string("\n");
          break;
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
          func_code += std::string("\t") + "vec4 " + output_vars.at(i) + ";" + std::string("\n");
          break;
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          func_code += std::string("\t") + "bool " + output_vars.at(i) + ";" + std::string("\n");
          break;
        default:
          break;
      }
    }
  }

  node_code += generator->generate_code(node_id, input_vars, output_vars);

  if (!node_code.empty()) {
    // Add the node code to the function code buffer.
    func_code += node_name + node_code;
  }

  if (!node_code.empty()) {
    func_code += "\n\n";
  }

  processed.insert(node_id);

  return true;
}
}  // namespace shadergen_visual_shader_generator
