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
#include "generator/visual_shader_node_generators.hpp"
#include "gui/controller/graph_node.hpp"
#include "gui/model/repeated_message_model.hpp"

namespace shadergen_visual_shader_generator {
static inline std::vector<std::unique_ptr<VisualShaderNodeGenerator>> to_generators(const ProtoModel* nodes) noexcept {
  int size{nodes->rowCount()};
  std::vector<std::unique_ptr<VisualShaderNodeGenerator>> generators;
  generators.resize(size);

  // Cast to ReapeatedMessageModel
  const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
  CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, std::vector<std::unique_ptr<VisualShaderNodeGenerator>>(),
                               "Nodes is not a repeated message model.");

  for (int i{0}; i < size; ++i) {
    const MessageModel* node_model{repeated_nodes->get_sub_model(i)};
    // I don't care about the field number, just send any field number inside the oneof model you want to get
    const ProtoModel* oneof_model{
        node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                      FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                  false, true)};
    const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

    switch (oneof_value_field_number) {
      case VisualShader::VisualShaderNode::kInputFieldNumber: {
        const VisualShaderNodeInputType input_type{
            (VisualShaderNodeInputType)oneof_model
                ->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber),
                    FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)))
                ->data()
                .toInt()};
        generators.at(i) = std::make_unique<VisualShaderNodeGeneratorInput>(input_type);
        break;
      }
      // case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
      //     const float value {oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatConstant::kValueFieldNumber)))->data().toFloat()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatConstant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
      //     const int value {oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntConstant::kValueFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntConstant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
      //     const unsigned int value {static_cast<unsigned int>(oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUintConstant::kValueFieldNumber)))->data().toUInt())};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntConstant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
      //     const bool value {oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeBooleanConstant::kValueFieldNumber)))->data().toBool()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorBooleanConstant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
      //     const QColor value {QColor(oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeColorConstant::kValueFieldNumber)))->data().toString())};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorColorConstant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
      //     const QVector2D value {QVector2D(oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kValueFieldNumber)))->data().toString())};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec2Constant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
      //     const QVector3D value {QVector3D(oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kValueFieldNumber)))->data().toString())};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec3Constant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
      //     const QVector4D value {QVector4D(oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kValueFieldNumber)))->data().toString())};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec4Constant>(value);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
      //     const VisualShaderNodeFloatOpType op_type {(VisualShaderNodeFloatOpType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatOp::kOpTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatOp>(op_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
      //     const VisualShaderNodeIntOpType op_type {(VisualShaderNodeIntOpType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntOp::kOpTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntOp>(op_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
      //     const VisualShaderNodeUintOpType op_type {(VisualShaderNodeUintOpType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUintOp::kOpTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntOp>(op_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
      //     const VisualShaderNodeVectorOpType op_type {(VisualShaderNodeVectorOpType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVectorOp::kOpTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorOp>(op_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
      //     const VisualShaderNodeFloatFuncType func_type {(VisualShaderNodeFloatFuncType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatFunc::kFuncTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatFunc>(func_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
      //     const VisualShaderNodeIntFuncType func_type {(VisualShaderNodeIntFuncType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntFunc::kFuncTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntFunc>(func_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
      //     const VisualShaderNodeUintFuncType func_type {(VisualShaderNodeUintFuncType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUintFunc::kFuncTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntFunc>(func_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
      //     const VisualShaderNodeVectorFuncType func_type {(VisualShaderNodeVectorFuncType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kFuncTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorFunc>(func_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
      //     const VisualShaderNodeValueNoiseType noise_type {(VisualShaderNodeValueNoiseType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kValueNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodeValueNoise::kNoiseTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorValueNoise>(noise_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
      //     const VisualShaderNodePerlinNoiseType noise_type {(VisualShaderNodePerlinNoiseType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodePerlinNoise::kNoiseTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorPerlinNoise>(noise_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
      //     const VisualShaderNodeVoronoiNoiseType noise_type {(VisualShaderNodeVoronoiNoiseType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kNoiseTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVoronoiNoise>(noise_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
      //     const VisualShaderNodeDotProductType dot_type {(VisualShaderNodeDotProductType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kDotProductFieldNumber), FieldPath::FieldNumber(VisualShaderNodeDotProduct::kDotTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorDotProduct>(dot_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
      //     const VisualShaderNodeVectorLenType len_type {(VisualShaderNodeVectorLenType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorLenFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVectorLen::kLenTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorLen>(len_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kClampFieldNumber: {
      //     const VisualShaderNodeClampType clamp_type {(VisualShaderNodeClampType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kClampFieldNumber), FieldPath::FieldNumber(VisualShaderNodeClamp::kClampTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorClamp>(clamp_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kStepFieldNumber: {
      //     const VisualShaderNodeStepType step_type {(VisualShaderNodeStepType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kStepFieldNumber), FieldPath::FieldNumber(VisualShaderNodeStep::kStepTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorStep>(step_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kSmoothStepFieldNumber: {
      //     const VisualShaderNodeSmoothStepType smooth_step_type {(VisualShaderNodeSmoothStepType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kSmoothStepFieldNumber), FieldPath::FieldNumber(VisualShaderNodeSmoothStep::kSmoothStepTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorSmoothStep>(smooth_step_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
      //     const VisualShaderNodeVectorDistanceType distance_type {(VisualShaderNodeVectorDistanceType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorDistanceFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVectorDistance::kDistanceTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorDistance>(distance_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kMixFieldNumber: {
      //     const VisualShaderNodeMixType mix_type {(VisualShaderNodeMixType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kMixFieldNumber), FieldPath::FieldNumber(VisualShaderNodeMix::kMixTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorMix>(mix_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
      //     const VisualShaderNodeVector2DComposeType compose_type {(VisualShaderNodeVector2DComposeType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVector2DComposeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVector2DCompose::kComposeTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVector2DCompose>(compose_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
      //     const VisualShaderNodeVector3DComposeType compose_type {(VisualShaderNodeVector3DComposeType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVector3DComposeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVector3DCompose::kComposeTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVector3DCompose>(compose_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
      //     const VisualShaderNodeVector4DComposeType compose_type {(VisualShaderNodeVector4DComposeType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVector4DComposeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVector4DCompose::kComposeTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVector4DCompose>(compose_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
      //     const VisualShaderNodeVector2DDecomposeType decompose_type {(VisualShaderNodeVector2DDecomposeType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVector2DDecompose::kDecomposeTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVector2DDecompose>(decompose_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
      //     const VisualShaderNodeVector3DDecomposeType decompose_type {(VisualShaderNodeVector3DDecomposeType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVector3DDecompose::kDecomposeTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVector3DDecompose>(decompose_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
      //     const VisualShaderNodeVector4DDecomposeType decompose_type {(VisualShaderNodeVector4DDecomposeType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVector4DDecompose::kDecomposeTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVector4DDecompose>(decompose_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
      //     const VisualShaderNodeIfType if_type {(VisualShaderNodeIfType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIfNodeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIfNode::kIfTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIfNode>(if_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
      //     const VisualShaderNodeSwitchType switch_type {(VisualShaderNodeSwitchType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeSwitchNode::kSwitchTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorSwitchNode>(switch_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kIsFieldNumber: {
      //     const VisualShaderNodeIsType is_type {(VisualShaderNodeIsType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIsFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIs::kIsTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIs>(is_type);
      //     break;
      // }
      // case VisualShader::VisualShaderNode::kCompareFieldNumber: {
      //     const VisualShaderNodeCompareType compare_type {(VisualShaderNodeCompareType)oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber), FieldPath::FieldNumber(VisualShaderNodeCompare::kCompareTypeFieldNumber)))->data().toInt()};
      //     generators.at(i) = std::make_unique<VisualShaderNodeGeneratorCompare>(compare_type);
      //     break;
      // }
      default:
        WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
        break;
    }
  }

  return generators;
}

// Define generate_shader_for_each_node to use it in generate_shader
static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                 std::string& func_code, const int& node_id,
                                                 std::unordered_set<int>& processed,
                                                 std::unordered_set<std::string>& global_processed) noexcept;

bool generate_shader(std::string& code, const ProtoModel* nodes, const ProtoModel* connections) noexcept {
  std::vector<std::unique_ptr<VisualShaderNodeGenerator>> generators = to_generators(nodes);

  return true;
}

std::string generate_preview_shader(const int& node_id, const int& port) noexcept { return ""; }

static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                 std::string& func_code, const int& node_id,
                                                 std::unordered_set<int>& processed,
                                                 std::unordered_set<std::string>& global_processed) noexcept {
  return true;
}
}  // namespace shadergen_visual_shader_generator
