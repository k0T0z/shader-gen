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
        const ProtoModel* input_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)))};

        const VisualShaderNodeInputType input_type{input_model->get_sub_model(FieldPath::Of<VisualShaderNodeInput>(
            FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)))->data().toInt()};
        generators.at(i) = std::make_unique<VisualShaderNodeGeneratorInput>(input_type);
        break;
      }
      case VisualShader::VisualShaderNode::kOutputFieldNumber: {
        generators.at(i) = std::make_unique<VisualShaderNodeGeneratorOutput>();
        break;
      }
      case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
          const ProtoModel* float_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatConstantFieldNumber)))};

          const float value {float_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatConstant>(FieldPath::FieldNumber(VisualShaderNodeFloatConstant::kValueFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
          const ProtoModel* int_model_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntConstantFieldNumber)))};

          const int value {int_model_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntConstant>(FieldPath::FieldNumber(VisualShaderNodeIntConstant::kValueFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
          const ProtoModel* uint_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintConstantFieldNumber)))};

          const unsigned int value {uint_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntConstant>(FieldPath::FieldNumber(VisualShaderNodeUIntConstant::kValueFieldNumber)))->data().toUInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
          const ProtoModel* bool_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber)))};

          const bool value {bool_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeBooleanConstant>(FieldPath::FieldNumber(VisualShaderNodeBooleanConstant::kValueFieldNumber)))->data().toBool()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorBoolConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
          const ProtoModel* color_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber)))};

          const float r {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kRFieldNumber)))->data().toFloat()};
          const float g {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kGFieldNumber)))->data().toFloat()};
          const float b {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kBFieldNumber)))->data().toFloat()};
          const float a {color_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeColorConstant>(FieldPath::FieldNumber(VisualShaderNodeColorConstant::kAFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorColorConstant>(r, g, b, a);
          break;
      }
      case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
          const ProtoModel* vec2_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber)))};

          const float x {vec2_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec2Constant>(FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kXFieldNumber)))->data().toFloat()};
          const float y {vec2_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec2Constant>(FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kYFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec2Constant>(x, y);
          break;
      }
      case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
          const ProtoModel* vec3_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber)))};

          const float x {vec3_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kXFieldNumber)))->data().toFloat()};
          const float y {vec3_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kYFieldNumber)))->data().toFloat()};
          const float z {vec3_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec3Constant>(FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kZFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec3Constant>(x, y, z);
          break;
      }
      case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
          const ProtoModel* vec4_constant_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber)))};

          const float x {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kXFieldNumber)))->data().toFloat()};
          const float y {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kYFieldNumber)))->data().toFloat()};
          const float z {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kZFieldNumber)))->data().toFloat()};
          const float w {vec4_constant_model->get_sub_model(FieldPath::Of<VisualShaderNodeVec4Constant>(FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kWFieldNumber)))->data().toFloat()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVec4Constant>(x, y, z, w);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
          const ProtoModel* float_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatOpFieldNumber)))};

          const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type {float_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatOp>(FieldPath::FieldNumber(VisualShaderNodeFloatOp::kOpFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
          const ProtoModel* int_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntOpFieldNumber)))};

          const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type {int_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntOp>(FieldPath::FieldNumber(VisualShaderNodeIntOp::kOpFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
          const ProtoModel* uint_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintOpFieldNumber)))};

          const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type {uint_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntOp>(FieldPath::FieldNumber(VisualShaderNodeUIntOp::kOpFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
          const ProtoModel* vector_op_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorOpFieldNumber)))};

          const VisualShaderNodeVectorType type {vector_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorOp>(FieldPath::FieldNumber(VisualShaderNodeVectorOp::kTypeFieldNumber)))->data().toInt()};
          const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type {vector_op_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorOp>(FieldPath::FieldNumber(VisualShaderNodeVectorOp::kOpFieldNumber)))->data().toInt()};

          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorOp>(type, op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
          const ProtoModel* float_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatFuncFieldNumber)))};

          const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type {float_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeFloatFunc>(FieldPath::FieldNumber(VisualShaderNodeFloatFunc::kFuncFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorFloatFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
          const ProtoModel* int_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntFuncFieldNumber)))};

          const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type {int_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeIntFunc>(FieldPath::FieldNumber(VisualShaderNodeIntFunc::kFuncFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIntFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
          const ProtoModel* uint_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintFuncFieldNumber)))};

          const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type {uint_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeUIntFunc>(FieldPath::FieldNumber(VisualShaderNodeUIntFunc::kFuncFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorUIntFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
          const ProtoModel* vector_func_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVectorFuncFieldNumber)))};

          const VisualShaderNodeVectorType type {vector_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kTypeFieldNumber)))->data().toInt()};
          const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type {vector_func_model->get_sub_model(FieldPath::Of<VisualShaderNodeVectorFunc>(FieldPath::FieldNumber(VisualShaderNodeVectorFunc::kFuncFieldNumber)))->data().toInt()};

          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorVectorFunc>(type, func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
          const ProtoModel* value_noise_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kValueNoiseFieldNumber)))};

          const float scale {value_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodeValueNoise>(FieldPath::FieldNumber(VisualShaderNodeValueNoise::kScaleFieldNumber)))->data().toFloat()};
        //   generators.at(i) = std::make_unique<VisualShaderNodeGeneratorValueNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
            const ProtoModel* perlin_noise_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                FieldPath::FieldNumber(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber)))};

            const float scale {perlin_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodePerlinNoise>(FieldPath::FieldNumber(VisualShaderNodePerlinNoise::kScaleFieldNumber)))->data().toFloat()};
          // generators.at(i) = std::make_unique<VisualShaderNodeGeneratorPerlinNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
          const ProtoModel* voronoi_noise_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber)))};

          const float angle_offset {voronoi_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodeVoronoiNoise>(FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber)))->data().toFloat()};
          const float cell_density {voronoi_noise_model->get_sub_model(FieldPath::Of<VisualShaderNodeVoronoiNoise>(FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber)))->data().toFloat()};
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
          const ProtoModel* switch_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber)))};

          const VisualShaderNodeSwitch::VisualShaderNodeSwitchOpType type {switch_model->get_sub_model(FieldPath::Of<VisualShaderNodeSwitch>(FieldPath::FieldNumber(VisualShaderNodeSwitch::kTypeFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorSwitch>(type);
          break;
      }
      case VisualShader::VisualShaderNode::kIsFieldNumber: {
          const ProtoModel* is_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIsFieldNumber)))};

          const VisualShaderNodeIs::Function func {is_model->get_sub_model(FieldPath::Of<VisualShaderNodeIs>(FieldPath::FieldNumber(VisualShaderNodeIs::kFuncFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorIs>(func);
          break;
      }
      case VisualShader::VisualShaderNode::kCompareFieldNumber: {
          const ProtoModel* compare_model{oneof_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kCompareFieldNumber)))};

          const VisualShaderNodeCompare::ComparisonType type {compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(FieldPath::FieldNumber(VisualShaderNodeCompare::kTypeFieldNumber)))->data().toInt()};
            const VisualShaderNodeCompare::Function func {compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(FieldPath::FieldNumber(VisualShaderNodeCompare::kFuncFieldNumber)))->data().toInt()};
            const VisualShaderNodeCompare::Condition cond {compare_model->get_sub_model(FieldPath::Of<VisualShaderNodeCompare>(FieldPath::FieldNumber(VisualShaderNodeCompare::kCondFieldNumber)))->data().toInt()};
          generators.at(i) = std::make_unique<VisualShaderNodeGeneratorCompare>(type, func, cond);
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
