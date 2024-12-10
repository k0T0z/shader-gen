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
#include "generator/vs_node_noise_generators.hpp"

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
      case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatConstant::kValueFieldNumber)));
          const float value {model->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntConstant::kValueFieldNumber)));
          const int value {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUIntConstant::kValueFieldNumber)));
          const unsigned int value {model->data().toUInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeBooleanConstant::kValueFieldNumber)));
          const bool value {model->data().toBool()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorBoolConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber)), false, true);
          const float r {model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kRFieldNumber)))->data().toFloat()};
          const float g {model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kGFieldNumber)))->data().toFloat()};
          const float b {model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kBFieldNumber)))->data().toFloat()};
          const float a {model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kAFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorColorConstant>(r, g, b, a);
          break;
      }
      case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber)), false, true);
          const float x {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec2Constant>(FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kXFieldNumber)))->data().toFloat()};
          const float y {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec2Constant>(FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kYFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec2Constant>(x, y);
          break;
      }
      case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber)), false, true);
          const float x {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kXFieldNumber)))->data().toFloat()};
          const float y {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kYFieldNumber)))->data().toFloat()};
          const float z {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kZFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec3Constant>(x, y, z);
          break;
      }
      case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber)), false, true);
          const float x {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kXFieldNumber)))->data().toFloat()};
          const float y {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kYFieldNumber)))->data().toFloat()};
          const float z {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kZFieldNumber)))->data().toFloat()};
          const float w {model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kWFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec4Constant>(x, y, z, w);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatOp::kOpFieldNumber)));
          const int op_type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatOp>((VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType)op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntOp::kOpFieldNumber)));
          const int op_type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntOp>((VisualShaderNodeIntOp::VisualShaderNodeIntOpType)op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUIntOp::kOpFieldNumber)));
          const int op_type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntOp>((VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType)op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVectorOp::kTypeFieldNumber)));
          const int type {model->data().toInt()}; 
          auto* model2 = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorOpFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVectorOp::kOpFieldNumber)));
          const int op_type {model2->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorOp>((VisualShaderNodeVectorType)type, (VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType)op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatFunc::kFuncFieldNumber)));
          const int func_type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatFunc>((VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType)func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntFunc::kFuncFieldNumber)));
          const int func_type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntFunc>((VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType)func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintFuncFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUIntFunc::kFuncFieldNumber)));
          const int func_type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntFunc>((VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType)func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorFuncFieldNumber)), false, true);
          const int type {model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kTypeFieldNumber)))->data().toInt()};
          const int func_type {model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kFuncFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorFunc>((VisualShaderNodeVectorType)type, (VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType)func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kValueNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodeValueNoise::kScaleFieldNumber)), false, true);
          const float scale {model->data().toFloat()};
          // generators.at(i) = std::make_unique<VisualShaderNodeGeneratorValueNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodePerlinNoise::kScaleFieldNumber)), false, true);
          const float scale {model->data().toFloat()};
          // generators.at(i) = std::make_unique<VisualShaderNodeGeneratorPerlinNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber)), false, true);
          const float angle_offset {model->get_sub_model(FieldPath::Of<VisualShaderNodeVoronoiNoise>(FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber)))->data().toFloat()};
          const float cell_density {model->get_sub_model(FieldPath::Of<VisualShaderNodeVoronoiNoise>(FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber)))->data().toFloat()};
          // generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVoronoiNoise>(angle_offset, cell_density);
          break;
      }
      case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorDotProduct>();
          break;
      }
      case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorLen>();
          break;
      }
      case VisualShader::VisualShaderNode::kClampFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorClamp>();
          break;
      }
      case VisualShader::VisualShaderNode::kStepFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorStep>();
          break;
      }
      case VisualShader::VisualShaderNode::kSmoothStepFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorSmoothStep>();
          break;
      }
      case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorDistance>();
          break;
      }
      case VisualShader::VisualShaderNode::kMixFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorMix>();
          break;
      }
      case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorCompose>(VisualShaderNodeVectorType::TYPE_VECTOR_2D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorCompose>(VisualShaderNodeVectorType::TYPE_VECTOR_3D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorCompose>(VisualShaderNodeVectorType::TYPE_VECTOR_4D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorDecompose>(VisualShaderNodeVectorType::TYPE_VECTOR_2D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorDecompose>(VisualShaderNodeVectorType::TYPE_VECTOR_3D);
          break;
      }
      case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorDecompose>(VisualShaderNodeVectorType::TYPE_VECTOR_4D);
          break;
      }
      case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIf>();
          break;
      }
      case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber), FieldPath::FieldNumber(VisualShaderNodeSwitch::kTypeFieldNumber)));
          const int type {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorSwitch>((VisualShaderNodeSwitch::VisualShaderNodeSwitchOpType)type);
          break;
      }
      case VisualShader::VisualShaderNode::kIsFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIsFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIs::kFuncFieldNumber)));
          const int func {model->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIs>((VisualShaderNodeIs::Function)func);
          break;
      }
      case VisualShader::VisualShaderNode::kCompareFieldNumber: {
          auto* model = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber), FieldPath::FieldNumber(VisualShaderNodeCompare::kTypeFieldNumber)));
          const int type {model->data().toInt()};
          auto* model2 = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber), FieldPath::FieldNumber(VisualShaderNodeCompare::kFuncFieldNumber)));
          const int func {model2->data().toInt()};
          auto* model3 = oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber), FieldPath::FieldNumber(VisualShaderNodeCompare::kCondFieldNumber)));
          const int cond {model3->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorCompare>((VisualShaderNodeCompare::ComparisonType)type, 
                (VisualShaderNodeCompare::Function)func, 
                (VisualShaderNodeCompare::Condition)cond);
          break;
      }
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
