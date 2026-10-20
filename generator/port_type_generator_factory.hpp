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

#ifndef SHADERGEN_PORT_TYPE_GENERATOR_FACTORY_HPP
#define SHADERGEN_PORT_TYPE_GENERATOR_FACTORY_HPP

#include <memory>
#include <string>
#include <vector>

#include "error_macros.hpp"
#include "generator/graph_string_utils.hpp"
#include "generator/proto_node_factory.hpp"
#include "generator/visual_shader_node_port_type_generator.hpp"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

namespace shadergen_utils {
inline static std::shared_ptr<VisualShaderNodePortTypeGenerator> get_port_type_generator(const std::string& node_entity) noexcept {
  const std::vector<std::string> entity_tokens{shadergen_graph_string_utils::split_string(node_entity, ';')};
  const int entity_type = std::stoi(shadergen_graph_string_utils::get_entity_type(entity_tokens));
  CHECK_CONDITION_TRUE_NON_VOID(entity_type != 0, nullptr, "Wrong entity type.");
  const int oneof_value_field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_oneof_value_field_number(entity_tokens));
  const std::vector<std::string> parameters{shadergen_graph_string_utils::get_node_entity_parameters(entity_tokens)};
  
  const std::shared_ptr<IVisualShaderProtoNode> proto_node{get_proto_node_by_oneof_value_field_number(oneof_value_field_number)};

  switch (oneof_value_field_number) {
    case VisualShader::VisualShaderNode::kInputFieldNumber: {
      const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
      const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
      CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeInput::kTypeFieldNumber, nullptr, "Wrong field number.");
      const VisualShaderNodeInput::VisualShaderNodeInputType input_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

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
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeFloatOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeIntOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorIntOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeUIntOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntOp>(proto_node, op_type);
    }
    case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorOp::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        const std::vector<std::string> parameter_tokens1{shadergen_graph_string_utils::split_string(parameters.at(1), '=')};
        const int field_number1 = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens1));
        CHECK_CONDITION_TRUE_NON_VOID(field_number1 != VisualShaderNodeVectorOp::kOpTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens1))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorOp>(proto_node, type, op_type);
    }
    case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeFloatFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorFloatFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeIntFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorIntFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeUIntFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorUIntFunc>(proto_node, func_type);
    }
    case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorFunc::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        const std::vector<std::string> parameter_tokens1{shadergen_graph_string_utils::split_string(parameters.at(1), '=')};
        const int field_number1 = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens1));
        CHECK_CONDITION_TRUE_NON_VOID(field_number1 != VisualShaderNodeVectorFunc::kFuncTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens1))};

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
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorLen::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorVectorLen>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kClampFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeClamp::kTypeFieldNumber, nullptr, "Wrong field number."); 
        const VisualShaderNodeClamp::VisualShaderNodeClampType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorClamp>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeVectorDistance::kVecTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeVectorType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

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
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeSwitch::kTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeSwitch::VisualShaderNodeSwitchType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorSwitch>(proto_node, type);
    }
    case VisualShader::VisualShaderNode::kIsFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeIs::kFuncFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeIs::VisualShaderNodeIsFunction func{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorIs>(proto_node, func);
    }
    case VisualShader::VisualShaderNode::kCompareFieldNumber: {
        const std::vector<std::string> parameter_tokens{shadergen_graph_string_utils::split_string(parameters.at(0), '=')};
        const int field_number = std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_field_number(parameter_tokens));
        CHECK_CONDITION_TRUE_NON_VOID(field_number != VisualShaderNodeCompare::kTypeFieldNumber, nullptr, "Wrong field number.");
        const VisualShaderNodeCompare::VisualShaderNodeCompareType type{std::stoi(shadergen_graph_string_utils::get_node_entity_parameter_value(parameter_tokens))};

        return std::make_shared<VisualShaderNodePortTypeGeneratorCompare>(proto_node, type);
    }
    default:
      WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
      break;
  }
  return nullptr;
}
}  // namespace shadergen_utils

#endif  // SHADERGEN_PORT_TYPE_GENERATOR_FACTORY_HPP
