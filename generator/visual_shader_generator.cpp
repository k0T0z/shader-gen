/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2026 Seif Kandil (k0T0z)                                       */
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
#include "generator/vs_node_noise_generators.hpp"
#include "gui/model/utils/utils.hpp"

using EnumDescriptor = google::protobuf::EnumDescriptor;

const std::string license_notices = 
"/***********************************************************************************/\n"
"/*  ShaderGen, a visual shader editor that can generate GLSL code using            */\n"
"/*  Artificial Intelligence.                                                       */\n"
"/*  Copyright (C) 2024 - present  Seif Kandil (k0T0z) (https://k0t0z.github.io/)   */\n"
"/*                                                                                 */\n"
"/*  This program is free software: you can redistribute it and/or modify           */\n"
"/*  it under the terms of the GNU General Public License as published by           */\n"
"/*  the Free Software Foundation, either version 3 of the License, or              */\n"
"/*  (at your option) any later version.                                            */\n"
"/*                                                                                 */\n"
"/*  This program is distributed in the hope that it will be useful,                */\n"
"/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                 */\n"
"/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  */\n"
"/*  GNU General Public License for more details.                                   */\n"
"/*                                                                                 */\n"
"/*  You should have received a copy of the GNU General Public License              */\n"
"/*  along with this program.  If not, see <https://www.gnu.org/licenses/>.         */\n"
"/***********************************************************************************/\n"
"\n";

namespace shadergen_visual_shader_generator {

namespace {

inline static std::vector<std::string> split_string_local(const std::string& str, const char& delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(str);
  while (std::getline(token_stream, token, delimiter)) tokens.push_back(token);
  return tokens;
}

inline static int get_header_node_id(const std::vector<std::string>& tokens) {
  return std::stoi(tokens.at(1));
}

inline static int get_header_oneof_value_field_number(const std::vector<std::string>& tokens) {
  return std::stoi(tokens.at(2));
}

inline static std::vector<std::string> get_header_parameters(const std::vector<std::string>& tokens) {
  std::vector<std::string> parameters;
  parameters.insert(parameters.end(), tokens.begin() + 3, tokens.end());
  return parameters;
}

inline static int get_param_field_number(const std::vector<std::string>& param_tokens) {
  return std::stoi(param_tokens.at(0));
}

inline static std::string get_param_value(const std::vector<std::string>& param_tokens) {
  return param_tokens.at(1);
}

static std::unordered_map<int, int> build_node_id_to_index(const RawVisualShaderGraph& graph) {
  std::unordered_map<int, int> node_id_to_index;
  for (size_t i = 0; i < graph.headers.size(); ++i) {
    const auto& header = graph.headers[i];
    const auto tokens = split_string_local(header, ';');
    const int entity_type = std::stoi(tokens.at(0));
    if (entity_type != 0) continue;
    const int n_id = get_header_node_id(tokens);
    node_id_to_index[n_id] = static_cast<int>(i);
  }
  return node_id_to_index;
}

static std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>> raw_to_proto_nodes(const RawVisualShaderGraph& graph) noexcept {
  std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>> proto_nodes;

  for (const auto& header : graph.headers) {
    const auto tokens = split_string_local(header, ';');
    const int entity_type = std::stoi(tokens.at(0));
    CONTINUE_IF_TRUE(entity_type != 0, "Entity type is not a node.");
    const int n_id = get_header_node_id(tokens);
    if (proto_nodes.find(n_id) != proto_nodes.end()) {
      FAIL_AND_RETURN_NON_VOID(proto_nodes, "Node id already exists.");
    }
    const int oneof_value_field_number = get_header_oneof_value_field_number(tokens);
    proto_nodes[n_id] = shadergen_utils::get_proto_node_by_oneof_value_field_number(oneof_value_field_number);
    CHECK_PARAM_NULLPTR_NON_VOID(proto_nodes[n_id], proto_nodes, "Proto node is nullptr.");
  }

  return proto_nodes;
}

static std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>> raw_to_generators(const RawVisualShaderGraph& graph) noexcept {
  std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>> generators;

  for (const auto& header : graph.headers) {
    const auto tokens = split_string_local(header, ';');
    const int entity_type = std::stoi(tokens.at(0));
    CONTINUE_IF_TRUE(entity_type != 0, "Entity type is not a node.");
    const int n_id = get_header_node_id(tokens);
    if (generators.find(n_id) != generators.end()) {
      FAIL_AND_RETURN_NON_VOID(generators, "Node ID already exists in the generators map.");
    }
    const int oneof_value_field_number = get_header_oneof_value_field_number(tokens);
    const auto parameters = get_header_parameters(tokens);

    switch (oneof_value_field_number) {
      case VisualShader::VisualShaderNode::kInputFieldNumber: {
        const auto parameter_tokens = split_string_local(parameters.at(0), '=');
        const int field_number = get_param_field_number(parameter_tokens);
        CONTINUE_IF_TRUE(field_number != VisualShaderNodeInput::kTypeFieldNumber, "Wrong field number.");
        const VisualShaderNodeInput::VisualShaderNodeInputType input_type{std::stoi(get_param_value(parameter_tokens))};

        generators[n_id] = std::make_shared<VisualShaderNodeGeneratorInput>(input_type);
        break;
      }
      case VisualShader::VisualShaderNode::kOutputFieldNumber: {
        generators[n_id] = std::make_shared<VisualShaderNodeGeneratorOutput>();
        break;
      }
      case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeFloatConstant::kValueFieldNumber, "Wrong field number.");
          const float value{std::stof(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorFloatConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeIntConstant::kValueFieldNumber, "Wrong field number.");
          const int value{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeUIntConstant::kValueFieldNumber, "Wrong field number.");
          const unsigned int value{std::stoul(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorUIntConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeBooleanConstant::kValueFieldNumber, "Wrong field number.");
          const bool value{(bool)std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorBoolConstant>(value);
          break;
      }
      case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeColorConstant::kRFieldNumber, "Wrong field number.");
          const float r{std::stof(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeColorConstant::kGFieldNumber, "Wrong field number.");
          const float g{std::stof(get_param_value(parameter_tokens1))};

          const auto parameter_tokens2 = split_string_local(parameters.at(2), '=');
          const int field_number2 = get_param_field_number(parameter_tokens2);
          CONTINUE_IF_TRUE(field_number2 != VisualShaderNodeColorConstant::kBFieldNumber, "Wrong field number.");
          const float b{std::stof(get_param_value(parameter_tokens2))};

          const auto parameter_tokens3 = split_string_local(parameters.at(3), '=');
          const int field_number3 = get_param_field_number(parameter_tokens3);
          CONTINUE_IF_TRUE(field_number3 != VisualShaderNodeColorConstant::kAFieldNumber, "Wrong field number.");
          const float a{std::stof(get_param_value(parameter_tokens3))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorColorConstant>(r, g, b, a);
          break;
      }
      case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeVec2Constant::kXFieldNumber, "Wrong field number.");
          const float x{std::stof(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeVec2Constant::kYFieldNumber, "Wrong field number.");
          const float y{std::stof(get_param_value(parameter_tokens1))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVec2Constant>(x, y);
          break;
      }
      case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeVec3Constant::kXFieldNumber, "Wrong field number.");
          const float x{std::stof(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeVec3Constant::kYFieldNumber, "Wrong field number.");
          const float y{std::stof(get_param_value(parameter_tokens1))};

          const auto parameter_tokens2 = split_string_local(parameters.at(2), '=');
          const int field_number2 = get_param_field_number(parameter_tokens2);
          CONTINUE_IF_TRUE(field_number2 != VisualShaderNodeVec3Constant::kZFieldNumber, "Wrong field number.");
          const float z{std::stof(get_param_value(parameter_tokens2))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVec3Constant>(x, y, z);
          break;
      }
      case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeVec4Constant::kXFieldNumber, "Wrong field number.");
          const float x{std::stof(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeVec4Constant::kYFieldNumber, "Wrong field number.");
          const float y{std::stof(get_param_value(parameter_tokens1))};

          const auto parameter_tokens2 = split_string_local(parameters.at(2), '=');
          const int field_number2 = get_param_field_number(parameter_tokens2);
          CONTINUE_IF_TRUE(field_number2 != VisualShaderNodeVec4Constant::kZFieldNumber, "Wrong field number.");
          const float z{std::stof(get_param_value(parameter_tokens2))};

          const auto parameter_tokens3 = split_string_local(parameters.at(3), '=');
          const int field_number3 = get_param_field_number(parameter_tokens3);
          CONTINUE_IF_TRUE(field_number3 != VisualShaderNodeVec4Constant::kWFieldNumber, "Wrong field number.");
          const float w{std::stof(get_param_value(parameter_tokens3))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVec4Constant>(x, y, z, w);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeFloatOp::kOpTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorFloatOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeIntOp::kOpTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIntOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeUIntOp::kOpTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorUIntOp>(op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeVectorOp::kVecTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeVectorType type{std::stoi(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeVectorOp::kOpTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type{std::stoi(get_param_value(parameter_tokens1))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorOp>(type, op_type);
          break;
      }
      case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeFloatFunc::kFuncTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorFloatFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeIntFunc::kFuncTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIntFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeUIntFunc::kFuncTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorUIntFunc>(func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeVectorFunc::kVecTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeVectorType type{std::stoi(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeVectorFunc::kFuncTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type{std::stoi(get_param_value(parameter_tokens1))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorFunc>(type, func_type);
          break;
      }
      case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeValueNoise::kScaleFieldNumber, "Wrong field number.");
          const float scale{std::stof(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorValueNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodePerlinNoise::kScaleFieldNumber, "Wrong field number.");
          const float scale{std::stof(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorPerlinNoise>(scale);
          break;
      }
      case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber, "Wrong field number.");
          const float cell_density{std::stof(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber, "Wrong field number.");
          const float angle_offset{std::stof(get_param_value(parameter_tokens1))};

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
      case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorVectorDistance>();
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
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeSwitch::kTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeSwitch::VisualShaderNodeSwitchType type{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorSwitch>(type);
          break;
      }
      case VisualShader::VisualShaderNode::kIsFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeIs::kFuncFieldNumber, "Wrong field number.");
          const VisualShaderNodeIs::VisualShaderNodeIsFunction func{std::stoi(get_param_value(parameter_tokens))};

          generators[n_id] = std::make_shared<VisualShaderNodeGeneratorIs>(func);
          break;
      }
      case VisualShader::VisualShaderNode::kCompareFieldNumber: {
          const auto parameter_tokens = split_string_local(parameters.at(0), '=');
          const int field_number = get_param_field_number(parameter_tokens);
          CONTINUE_IF_TRUE(field_number != VisualShaderNodeCompare::kTypeFieldNumber, "Wrong field number.");
          const VisualShaderNodeCompare::VisualShaderNodeCompareType type{std::stoi(get_param_value(parameter_tokens))};

          const auto parameter_tokens1 = split_string_local(parameters.at(1), '=');
          const int field_number1 = get_param_field_number(parameter_tokens1);
          CONTINUE_IF_TRUE(field_number1 != VisualShaderNodeCompare::kFuncFieldNumber, "Wrong field number.");
          const VisualShaderNodeCompare::VisualShaderNodeCompareFunction func{std::stoi(get_param_value(parameter_tokens1))};

          const auto parameter_tokens2 = split_string_local(parameters.at(2), '=');
          const int field_number2 = get_param_field_number(parameter_tokens2);
          CONTINUE_IF_TRUE(field_number2 != VisualShaderNodeCompare::kCondFieldNumber, "Wrong field number.");
          const VisualShaderNodeCompare::VisualShaderNodeCompareCondition cond{std::stoi(get_param_value(parameter_tokens2))};

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

static std::unordered_map<int, std::shared_ptr<VisualShaderNodePortTypeGenerator>> raw_to_port_type_generators(const RawVisualShaderGraph& graph) noexcept {
  std::unordered_map<int, std::shared_ptr<VisualShaderNodePortTypeGenerator>> port_type_generators;

  for (const auto& header : graph.headers) {
    port_type_generators[get_header_node_id(split_string_local(header, ';'))] = shadergen_utils::get_port_type_generator(header);
    CHECK_PARAM_NULLPTR_NON_VOID(port_type_generators[get_header_node_id(split_string_local(header, ';'))], port_type_generators, "Proto node is nullptr.");
  }

  return port_type_generators;
}

static std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>> raw_to_connections(const RawVisualShaderGraph& graph) noexcept {
  std::map<ConnectionKey, std::shared_ptr<Connection>> input_connections;
  std::map<ConnectionKey, std::shared_ptr<Connection>> output_connections;

  const auto node_id_to_index = build_node_id_to_index(graph);

  const size_t N = graph.headers.size();
  for (size_t i = 0; i < N; ++i) {
    const auto i_tokens = split_string_local(graph.headers[i], ';');
    if (std::stoi(i_tokens.at(0)) != 0) continue;
    const int from_node_id = get_header_node_id(i_tokens);

    for (size_t j = 0; j < N; ++j) {
      if (i == j) continue;
      const auto j_tokens = split_string_local(graph.headers[j], ';');
      if (std::stoi(j_tokens.at(0)) != 0) continue;
      const int to_node_id = get_header_node_id(j_tokens);

      const std::string& cell = graph.adj_matrix[i][j];
      if (cell.empty()) continue;

      const auto connection_groups = split_string_local(cell, ';');
      for (const auto& group : connection_groups) {
        const auto ports = split_string_local(group, ',');
        if (ports.size() < 2) continue;

        std::shared_ptr<Connection> c = std::make_shared<Connection>();
        c->from.f_key.node = from_node_id;
        c->from.f_key.port = std::stoi(ports.at(0));
        c->to.f_key.node = to_node_id;
        c->to.f_key.port = std::stoi(ports.at(1));

        ConnectionKey from_key;
        from_key.f_key.node = c->from.f_key.node;
        from_key.f_key.port = c->from.f_key.port;
        output_connections[from_key] = c;

        ConnectionKey to_key;
        to_key.f_key.node = c->to.f_key.node;
        to_key.f_key.port = c->to.f_key.port;
        input_connections[to_key] = c;
      }
    }
  }

  return std::make_pair(input_connections, output_connections);
}

}  // anonymous namespace

static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                 std::string& func_code,
                                                 const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes,
                                                 const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators,
                                                 const std::unordered_map<int, std::shared_ptr<VisualShaderNodePortTypeGenerator>>& port_type_generators,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& input_connections,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& output_connections,
                                                 const int& node_id, 
                                                 std::unordered_set<int>& processed,
                                                 std::unordered_set<std::string>& global_processed) noexcept;

bool generate_shader(const RawVisualShaderGraph& graph, std::string& code_buffer) noexcept {
  static const std::string func_name{"main"};

  const auto proto_nodes = raw_to_proto_nodes(graph);
  const auto generators = raw_to_generators(graph);
  const auto port_type_generators = raw_to_port_type_generators(graph);
  const auto input_output_connections_by_key = raw_to_connections(graph);

  std::string global_code;
  std::string global_code_per_node;
  std::string shader_code;
  std::unordered_set<std::string> global_processed;

  std::string func_code;
  std::unordered_set<int> processed;

  func_code += "\nvoid " + func_name + "() {" + std::string("\n");

  bool status{generate_shader_for_each_node(global_code, 
                                            global_code_per_node, 
                                            func_code, 
                                            proto_nodes, 
                                            generators, 
                                            port_type_generators,
                                            input_output_connections_by_key.first,
                                            input_output_connections_by_key.second, 0, 
                                            processed,
                                            global_processed)};

  CHECK_CONDITION_TRUE_NON_VOID(!status, false, "Failed to generate shader for node 0.");

  func_code += std::string("}") + "\n\n";
  shader_code += func_code;

  std::string generated_code{license_notices};
  generated_code += global_code;
  generated_code += global_code_per_node;

  generated_code += shader_code;

  code_buffer = generated_code;

  return true;
}

std::string generate_preview_shader(const RawVisualShaderGraph& graph,
                                    const int& node_id, const int& port) noexcept { 
  static const std::string preview_func_name{"main"};
  static const std::string output_var{"FragColor"};

  const auto proto_nodes = raw_to_proto_nodes(graph);
  const auto generators = raw_to_generators(graph);
  const auto port_type_generators = raw_to_port_type_generators(graph);
  const auto input_output_connections_by_key = raw_to_connections(graph);

  CHECK_CONDITION_TRUE_NON_VOID(proto_nodes.find(node_id) == proto_nodes.end(), std::string(), "Node ID not found in proto nodes.");
  CHECK_CONDITION_TRUE_NON_VOID(generators.find(node_id) == generators.end(), std::string(), "Node ID not found in generators.");
  CHECK_CONDITION_TRUE_NON_VOID(port_type_generators.find(node_id) == port_type_generators.end(), std::string(), "Node ID not found in port type generators.");

  const std::shared_ptr<IVisualShaderProtoNode> proto_node{proto_nodes.at(node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(proto_node, std::string(), "Proto node is null.");

  std::string global_code;
  std::string global_code_per_node;
  std::string shader_code;
  std::unordered_set<std::string> global_processed;

  std::unordered_set<int> processed;

  shader_code += "\nvoid " + preview_func_name + "() {" + std::string("\n");

  bool status{generate_shader_for_each_node(global_code, 
                                            global_code_per_node, 
                                            shader_code, 
                                            proto_nodes, 
                                            generators, 
                                            port_type_generators,
                                            input_output_connections_by_key.first,
                                            input_output_connections_by_key.second, 
                                            node_id,
                                            processed,
                                            global_processed)};

  CHECK_CONDITION_TRUE_NON_VOID(!status, std::string(), "Failed to generate shader for node " + std::to_string(node_id) + ".");

  global_code += "out vec4 " + output_var + ";" + std::string("\n");

  std::shared_ptr<VisualShaderNodePortTypeGenerator> port_type_generator{port_type_generators.at(node_id)};

  VisualShaderNodePortType from_port_type{port_type_generator->get_output_port_type(port)};

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
      WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
      break;
  }

  shader_code += std::string("}") + "\n\n";

  std::string generated_code{license_notices};
  generated_code += global_code;
  generated_code += global_code_per_node;

  generated_code += shader_code;

  return generated_code;
}

static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                 std::string& func_code,
                                                 const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes,
                                                 const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators,
                                                 const std::unordered_map<int, std::shared_ptr<VisualShaderNodePortTypeGenerator>>& port_type_generators,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& input_connections,
                                                 const std::map<ConnectionKey, std::shared_ptr<Connection>>& output_connections,
                                                 const int& node_id, 
                                                 std::unordered_set<int>& processed,
                                                 std::unordered_set<std::string>& global_processed) noexcept {
  CHECK_CONDITION_TRUE_NON_VOID(proto_nodes.find(node_id) == proto_nodes.end(), false, "Node id not found in proto nodes.");
  CHECK_CONDITION_TRUE_NON_VOID(generators.find(node_id) == generators.end(), false, "Node id not found in generators.");
  CHECK_CONDITION_TRUE_NON_VOID(port_type_generators.find(node_id) == port_type_generators.end(), false, "Node id not found in port type generators.");

  const std::shared_ptr<IVisualShaderProtoNode> proto_node{proto_nodes.at(node_id)};
  const std::shared_ptr<VisualShaderNodeGenerator> generator{generators.at(node_id)};
  const std::shared_ptr<VisualShaderNodePortTypeGenerator> port_type_generator{port_type_generators.at(node_id)};

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
                                              port_type_generators,
                                              input_connections,
                                              output_connections, 
                                              from_node,
                                              processed,
                                              global_processed)};
    
    CHECK_CONDITION_TRUE_NON_VOID(!status, false, "Failed to generate shader for node " + std::to_string(from_node) + ".");
  }

  std::string proto_name{proto_node->get_name()};
  if (global_processed.find(proto_name) == global_processed.end()) {
    global_code += generator->generate_global(node_id);
    global_code_per_node += generator->generate_global_per_node(node_id);
  }
  global_processed.insert(proto_name);

  std::string node_name{"// " + proto_node->get_caption() + ":" + std::to_string(node_id) + "\n"};
  std::string node_code;
  std::vector<std::string> input_vars;

  input_vars.resize(proto_node->get_input_port_count());

  for (int i{0}; i < input_port_count; i++) {
    ConnectionKey key;
    key.f_key.node = node_id;
    key.f_key.port = i;

    if (input_connections.find(key) != input_connections.end()) {
      const std::shared_ptr<Connection> c{input_connections.at(key)};

      int from_node{(int)c->from.f_key.node};
      int from_port{(int)c->from.f_key.port};

      VisualShaderNodePortType to_port_type{port_type_generator->get_input_port_type(i)};

      VisualShaderNodePortType from_port_type{port_type_generators.at(from_node)->get_output_port_type(from_port)};

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
              default: {
                input_vars.at(i) = "0.0";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
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
              default: {
                input_vars.at(i) = "0";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
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
              default: {
                input_vars.at(i) = "0u";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
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
              default:{
                input_vars.at(i) = "false";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
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
              default: {
                input_vars.at(i) = "vec2(0.0)";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
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
              default: {
                input_vars.at(i) = "vec3(0.0)";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
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
              default: {
                input_vars.at(i) = "vec4(vec3(0.0), 1.0)";
                WARN_PRINT("Unsupported port type: " + std::to_string((int)from_port_type));
              } break;
            }
          } break;
          default: {
            input_vars.at(i) = "0.0";
            WARN_PRINT("Unsupported port type: " + std::to_string((int)to_port_type));
          } break;
        }
      }
    } else {
      VisualShaderNodePortType in_port_type{port_type_generator->get_input_port_type(i)};

      switch (in_port_type) {
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
      }
    }
  }

  int output_port_count{proto_node->get_output_port_count()};

  std::vector<std::string> output_vars;
  output_vars.resize(output_port_count);

  if (!generator->is_scoped_assignment()) {
    for (int i{0}; i < output_port_count; i++) {
      std::string from_var{"var_from_n" + std::to_string(node_id) + "_p" + std::to_string(i)};

      VisualShaderNodePortType from_port_type{port_type_generator->get_output_port_type(i)};

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

      VisualShaderNodePortType from_port_type{port_type_generator->get_output_port_type(i)};

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
    func_code += node_name + node_code;
  }

  if (!node_code.empty()) {
    func_code += "\n\n";
  }

  processed.insert(node_id);

  return true;
}
}  // namespace shadergen_visual_shader_generator
