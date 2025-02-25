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

#include "generator/visual_shader_node_generators.hpp"

#include "gui/controller/vs_proto_node.hpp"
#include "gui/model/utils/utils.hpp"

#include <iomanip>
#include <sstream>

std::string VisualShaderNodeGeneratorInput::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  int size{VisualShaderNodeInputType_descriptor()->value_count()};
  for (int i{1}; i < size; ++i) { // Skip INPUT_TYPE_UNSPECIFIED
    VisualShaderNodeInputType t_input_type{shadergen_utils::get_enum_value_by_enum_index(VisualShaderNodeInputType_descriptor(), i)};

    std::string input_type_name{
        shadergen_utils::get_enum_value_name_by_index(VisualShaderNodeInputType_descriptor(), t_input_type)};

    switch (t_input_type) {
      case VisualShaderNodeInputType::INPUT_TYPE_UV: {
        code += "in vec2 " + input_type_name + ";" + std::string("\n");
      } break;
      case VisualShaderNodeInputType::INPUT_TYPE_TIME: {
        code += "uniform float " + input_type_name + ";" + std::string("\n");
      } break;
      default:
        break;
    }
  }

  return code;
}

std::string VisualShaderNodeGeneratorInput::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code;

  std::string input_type_name{
        shadergen_utils::get_enum_value_name_by_index(VisualShaderNodeInputType_descriptor(), input_type)};

  switch (input_type) {
    case VisualShaderNodeInputType::INPUT_TYPE_UV: {
      code = std::string("\t") + output_vars.at(0) + " = " + input_type_name + ";" + std::string("\n");
    } break;
    case VisualShaderNodeInputType::INPUT_TYPE_TIME: {
      code = std::string("\t") + output_vars.at(0) + " = " + input_type_name + ";" + std::string("\n");
    } break;
    default:
      code = "0.0;" + std::string("\n");
      break;
  }

  return code;
}

std::string VisualShaderNodeGeneratorOutput::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  int size{VisualShaderNodeOutputType_descriptor()->value_count()};
  for (int i{1}; i < size; ++i) { // Skip OUTPUT_TYPE_UNSPECIFIED
    VisualShaderNodeInputType ontput_type{shadergen_utils::get_enum_value_by_enum_index(VisualShaderNodeOutputType_descriptor(), i)};

    std::string ontput_type_name{
        shadergen_utils::get_enum_value_name_by_index(VisualShaderNodeOutputType_descriptor(), ontput_type)};

    switch (ontput_type) {
      case VisualShaderNodeOutputType::OUTPUT_TYPE_COLOR: {
        code += "out vec4 " + ontput_type_name + ";" + std::string("\n");
      } break;
      default:
        break;
    }
  }

  return code;
}

std::string VisualShaderNodeGeneratorOutput::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code;

  int size{VisualShaderNodeOutputType_descriptor()->value_count()};
  for (int i{1}; i < size; ++i) { // Skip OUTPUT_TYPE_UNSPECIFIED
    VisualShaderNodeInputType ontput_type{shadergen_utils::get_enum_value_by_enum_index(VisualShaderNodeOutputType_descriptor(), i)};

    std::string ontput_type_name{
        shadergen_utils::get_enum_value_name_by_index(VisualShaderNodeOutputType_descriptor(), ontput_type)};

    if (!input_vars.at(i-1).empty()) { // zero based
      code += std::string("\t") + ontput_type_name + " = " + input_vars.at(i-1) + ";" + std::string("\n");
    }
  }

  return code;
}

/*************************************/
/* CONSTANTS                         */
/*************************************/

std::string VisualShaderNodeGeneratorFloatConstant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << "\t" << output_vars.at(0) << " = " << std::fixed << std::setprecision(6) << value << ";"
      << std::string("\n");
  return oss.str();
}

std::string VisualShaderNodeGeneratorIntConstant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = " + std::to_string(value) + ";" + std::string("\n");
}

std::string VisualShaderNodeGeneratorUIntConstant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = " + std::to_string(value) + "u;" + std::string("\n");
}

std::string VisualShaderNodeGeneratorBoolConstant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = " + (value ? "true" : "false") + ";" + std::string("\n");
}

std::string VisualShaderNodeGeneratorColorConstant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << "\t" << output_vars.at(0) << " = vec4(" << std::fixed << std::setprecision(6) << r << ", "
      << g << ", " << b << ", " << a << ") / " << 255.0f << ";" << std::string("\n");
  return oss.str();
}

std::string VisualShaderNodeGeneratorVec2Constant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << "\t" << output_vars.at(0) << " = vec2(" << std::fixed << std::setprecision(6) << x << ", "
      << y << ");" << std::string("\n");
  return oss.str();
}

std::string VisualShaderNodeGeneratorVec3Constant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << "\t" << output_vars.at(0) << " = vec3(" << std::fixed << std::setprecision(6) << x << ", "
      << y << ", " << z << ");" << std::string("\n");
  return oss.str();
}

std::string VisualShaderNodeGeneratorVec4Constant::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << "\t" << output_vars.at(0) << " = vec4(" << std::fixed << std::setprecision(6) << x << ", "
      << y << ", " << z << ", " << w << ");" << std::string("\n");
  return oss.str();
}

/*************************************/
/* OPERATORS                         */
/*************************************/

std::string VisualShaderNodeGeneratorFloatOp::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (op) {
    case VisualShaderNodeFloatOp::OP_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_MOD:
      code += "mod(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_POW:
      code += "pow(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_ATAN2:
      code += "atan(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_STEP:
      code += "step(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    default:
      break;
  }
  return code;
}

std::string VisualShaderNodeGeneratorIntOp::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (op) {
    case VisualShaderNodeIntOp::OP_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_MOD:
      code += input_vars.at(0) + " % " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_BITWISE_AND:
      code += input_vars.at(0) + " & " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_BITWISE_OR:
      code += input_vars.at(0) + " | " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_BITWISE_XOR:
      code += input_vars.at(0) + " ^ " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_BITWISE_LEFT_SHIFT:
      code += input_vars.at(0) + " << " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_BITWISE_RIGHT_SHIFT:
      code += input_vars.at(0) + " >> " + input_vars.at(1) + ";" + std::string("\n");
      break;
    default:
      break;
  }

  return code;
}

std::string VisualShaderNodeGeneratorUIntOp::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (op) {
    case VisualShaderNodeUIntOp::OP_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_MOD:
      code += input_vars.at(0) + " % " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_BITWISE_AND:
      code += input_vars.at(0) + " & " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_BITWISE_OR:
      code += input_vars.at(0) + " | " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_BITWISE_XOR:
      code += input_vars.at(0) + " ^ " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_BITWISE_LEFT_SHIFT:
      code += input_vars.at(0) + " << " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_BITWISE_RIGHT_SHIFT:
      code += input_vars.at(0) + " >> " + input_vars.at(1) + ";" + std::string("\n");
      break;
    default:
      break;
  }

  return code;
}

std::string VisualShaderNodeGeneratorVectorOp::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (op) {
    case VisualShaderNodeVectorOp::OP_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_MOD:
      code += "mod(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_POW:
      code += "pow(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_CROSS:
      switch (type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:  // Not supported.
          code += "vec2(0.0);" + std::string("\n");
          break;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
          code += "cross(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:  // Not supported.
          code += "vec4(0.0);" + std::string("\n");
          break;
        default:
          break;
      }
      break;
    case VisualShaderNodeVectorOp::OP_ATAN2:
      code += "atan(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_REFLECT:
      code += "reflect(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_STEP:
      code += "step(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    default:
      break;
  }

  return code;
}

/*************************************/
/* Funcs Node                        */
/*************************************/

std::string VisualShaderNodeGeneratorFloatFunc::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (func) {
    case VisualShaderNodeFloatFunc::FUNC_SIN:
      code += "sin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_COS:
      code += "cos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TAN:
      code += "tan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ASIN:
      code += "asin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ACOS:
      code += "acos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ATAN:
      code += "atan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_SINH:
      code += "sinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_COSH:
      code += "cosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TANH:
      code += "tanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_LOG:
      code += "log(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_EXP:
      code += "exp(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_SQRT:
      code += "sqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ABS:
      code += "abs(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_SIGN:
      code += "sign(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_FLOOR:
      code += "floor(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ROUND:
      code += "round(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_CEIL:
      code += "ceil(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_FRACT:
      code += "fract(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_SATURATE:
      code += "min(max(" + input_vars.at(0) + ", 0.0), 1.0);" +
              std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ACOSH:
      code += "acosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ASINH:
      code += "asinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ATANH:
      code += "atanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_DEGREES:
      code += "degrees(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_EXP2:
      code += "exp2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_INVERSE_SQRT:
      code += "inversesqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_LOG2:
      code += "log2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_RADIANS:
      code += "radians(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_RECIPROCAL:
      code += "1.0 / (" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ROUNDEVEN:
      code += "roundEven(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TRUNC:
      code += "trunc(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_ONEMINUS:
      code += "1.0 - " + input_vars.at(0) + ";" + std::string("\n");
      break;
    default:
      break;
  }
  return code;
}

std::string VisualShaderNodeGeneratorIntFunc::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (func) {
    case VisualShaderNodeIntFunc::FUNC_ABS:
      code += "abs(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntFunc::FUNC_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntFunc::FUNC_SIGN:
      code += "sign(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntFunc::FUNC_BITWISE_NOT:
      code += "~(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    default:
      break;
  }
  return code;
}

std::string VisualShaderNodeGeneratorUIntFunc::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (func) {
    case VisualShaderNodeUIntFunc::FUNC_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeUIntFunc::FUNC_BITWISE_NOT:
      code += "~(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    default:
      break;
  }
  return code;
}

std::string VisualShaderNodeGeneratorVectorFunc::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (func) {
    case VisualShaderNodeVectorFunc::FUNC_NORMALIZE:
      code += "normalize(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_SATURATE:
      switch (type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
          code += "max(min(" + input_vars.at(0) +
                  ", vec2(1.0)), vec2(0.0));" + std::string("\n");
          break;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
          code += "max(min(" + input_vars.at(0) +
                  ", vec3(1.0)), vec3(0.0));" + std::string("\n");
          break;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
          code += "max(min(" + input_vars.at(0) +
                  ", vec4(1.0)), vec4(0.0));" + std::string("\n");
          break;
        default:
          break;
      }
      break;
    case VisualShaderNodeVectorFunc::FUNC_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_RECIPROCAL:
      code += "1.0 / (" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ABS:
      code += "abs(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ACOS:
      code += "acos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ACOSH:
      code += "acosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ASIN:
      code += "asin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ASINH:
      code += "asinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ATAN:
      code += "atan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ATANH:
      code += "atanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_CEIL:
      code += "ceil(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_COS:
      code += "cos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_COSH:
      code += "cosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_DEGREES:
      code += "degrees(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_EXP:
      code += "exp(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_EXP2:
      code += "exp2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_FLOOR:
      code += "floor(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_FRACT:
      code += "fract(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_INVERSE_SQRT:
      code += "inversesqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_LOG:
      code += "log(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_LOG2:
      code += "log2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_RADIANS:
      code += "radians(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ROUND:
      code += "round(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ROUNDEVEN:
      code += "roundEven(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_SIGN:
      code += "sign(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_SIN:
      code += "sin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_SINH:
      code += "sinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_SQRT:
      code += "sqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TAN:
      code += "tan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TANH:
      code += "tanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TRUNC:
      code += "trunc(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_ONEMINUS:
      switch (type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
          code +=
              "vec2(1.0) - " + input_vars.at(0) + ";" + std::string("\n");
          break;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
          code +=
              "vec3(1.0) - " + input_vars.at(0) + ";" + std::string("\n");
          break;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
          code +=
              "vec4(1.0) - " + input_vars.at(0) + ";" + std::string("\n");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return code;
}

/*************************************/
/* MISC                              */
/*************************************/

std::string VisualShaderNodeGeneratorDotProduct::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = dot(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" +
         std::string("\n");
}

std::string VisualShaderNodeGeneratorVectorLen::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = length(" + input_vars.at(0) + ");" + std::string("\n");
}

std::string VisualShaderNodeGeneratorClamp::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = clamp(" + input_vars.at(0) + ", " + input_vars.at(1) + ", " +
         input_vars.at(2) + ");" + std::string("\n");
}

std::string VisualShaderNodeGeneratorStep::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = step(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" +
         std::string("\n");
}

std::string VisualShaderNodeGeneratorSmoothStep::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = smoothstep(" + input_vars.at(0) + ", " + input_vars.at(1) + ", " +
         input_vars.at(2) + ");" + std::string("\n");
}

std::string VisualShaderNodeGeneratorVectorDistance::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = distance(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" +
         std::string("\n");
}

std::string VisualShaderNodeGeneratorMix::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = mix(" + input_vars.at(0) + ", " + input_vars.at(1) + ", " +
         input_vars.at(2) + ");" + std::string("\n");
}

std::string VisualShaderNodeGeneratorVectorCompose::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (type) {
    case VisualShaderNodeVectorType::TYPE_VECTOR_2D: {
      code += "vec2(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" +
              std::string("\n");
    } break;
    case VisualShaderNodeVectorType::TYPE_VECTOR_3D: {
      code += "vec3(" + input_vars.at(0) + ", " + input_vars.at(1) + ", " +
              input_vars.at(2) + ");" + std::string("\n");
    } break;
    case VisualShaderNodeVectorType::TYPE_VECTOR_4D: {
      code += "vec4(" + input_vars.at(0) + ", " + input_vars.at(1) + ", " +
              input_vars.at(2) + ", " + input_vars.at(3) + ");" + std::string("\n");
    } break;
    default:
      break;
  }
  return code;
}

std::string VisualShaderNodeGeneratorVectorDecompose::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  std::string code;
  switch (type) {
    case VisualShaderNodeVectorType::TYPE_VECTOR_2D: {
      code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + ".x;" + std::string("\n");
      code += std::string("\t") + output_vars.at(1) + " = " + input_vars.at(0) + ".y;" + std::string("\n");
    } break;
    case VisualShaderNodeVectorType::TYPE_VECTOR_3D: {
      code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + ".x;" + std::string("\n");
      code += std::string("\t") + output_vars.at(1) + " = " + input_vars.at(0) + ".y;" + std::string("\n");
      code += std::string("\t") + output_vars.at(2) + " = " + input_vars.at(0) + ".z;" + std::string("\n");
    } break;
    case VisualShaderNodeVectorType::TYPE_VECTOR_4D: {
      code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + ".x;" + std::string("\n");
      code += std::string("\t") + output_vars.at(1) + " = " + input_vars.at(0) + ".y;" + std::string("\n");
      code += std::string("\t") + output_vars.at(2) + " = " + input_vars.at(0) + ".z;" + std::string("\n");
      code += std::string("\t") + output_vars.at(3) + " = " + input_vars.at(0) + ".w;" + std::string("\n");
    } break;
    default:
      break;
  }

  return code;
}

/*************************************/
/* Logic                             */
/*************************************/

std::string VisualShaderNodeGeneratorIf::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  std::string code;
  code += std::string("\t") + "if(abs(" + input_vars.at(0) + " - " + input_vars.at(1) + ") < " + input_vars.at(2) +
          ") {" + std::string("\n");  // abs(p1 - p2) < tolerance eg. p1 == p2
  code +=
      std::string("\t") + std::string("\t") + output_vars.at(0) + " = " + input_vars.at(3) + ";" + std::string("\n");
  code += std::string("\t") + "} else if(" + input_vars.at(0) + " < " + input_vars.at(1) + ") {" +
          std::string("\n");  // p1 < p2
  code +=
      std::string("\t") + std::string("\t") + output_vars.at(0) + " = " + input_vars.at(5) + ";" + std::string("\n");
  code +=
      std::string("\t") + "} else {" + std::string("\n");  // p1 > p2 (or p1 >= p2 if abs(p1 - p2) < tolerance is false)
  code +=
      std::string("\t") + std::string("\t") + output_vars.at(0) + " = " + input_vars.at(4) + ";" + std::string("\n");
  code += std::string("\t") + std::string("}") + std::string("\n");
  return code;
}

std::string VisualShaderNodeGeneratorSwitch::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  bool use_mix{false};

  switch (op) {
    case VisualShaderNodeSwitch::OP_TYPE_FLOAT:
    case VisualShaderNodeSwitch::OP_TYPE_VECTOR_2D:
    case VisualShaderNodeSwitch::OP_TYPE_VECTOR_3D:
    case VisualShaderNodeSwitch::OP_TYPE_VECTOR_4D:
      use_mix = true;
      break;
    default:
      break;
  }

  std::string code;
  if (use_mix) {
    code += std::string("\t") + output_vars.at(0) + " = mix(" + input_vars.at(2) + ", " + input_vars.at(1) +
            ", float(" + input_vars.at(0) + "));" + std::string("\n");
  } else {
    code += std::string("\t") + "if (" + input_vars.at(0) + ") {" + std::string("\n");
    code +=
        std::string("\t") + std::string("\t") + output_vars.at(0) + " = " + input_vars.at(1) + ";" + std::string("\n");
    code += std::string("\t") + "} else {" + std::string("\n");
    code +=
        std::string("\t") + std::string("\t") + output_vars.at(0) + " = " + input_vars.at(2) + ";" + std::string("\n");
    code += std::string("\t") + std::string("}") + std::string("\n");
  }
  return code;
}

std::string VisualShaderNodeGeneratorIs::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  std::string code{std::string("\t") + output_vars.at(0) + " = "};
  switch (func) {
    case VisualShaderNodeIs::FUNC_IS_INF:
      code += "isinf(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIs::FUNC_IS_NAN:
      code += "isnan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    default:
      break;
  }
  return code;
}

VisualShaderNodeGeneratorCompare::VisualShaderNodeGeneratorCompare(const VisualShaderNodeCompare::ComparisonType& comp, 
                                   const VisualShaderNodeCompare::Function& func, 
                                   const VisualShaderNodeCompare::Condition& cond)
      : VisualShaderNodeGenerator(), comp(comp), func(func), cond(cond) {
  switch (comp) {
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR:
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_UINT:
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_INT:
    case VisualShaderNodeCompare::CMP_TYPE_BOOLEAN:
      simple_decl = true;
      break;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_2D:
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_3D:
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_4D:
      simple_decl = false;
      break;
    default:
      break;
  }
}

std::string VisualShaderNodeGeneratorCompare::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  std::string code;
  switch (comp) {
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR: {
      switch (func) {
        case VisualShaderNodeCompare::FUNC_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = (abs(" + input_vars.at(0) + " - " + input_vars.at(1) +
                  ") < " + input_vars.at(2) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_NOT_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = !(abs(" + input_vars.at(0) + " - " + input_vars.at(1) +
                  ") < " + input_vars.at(2) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " > " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " >= " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " < " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " <= " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        default:
          break;
      }
    } break;
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_UINT:
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_INT: {
      switch (func) {
        case VisualShaderNodeCompare::FUNC_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " == " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_NOT_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " != " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " > " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " >= " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " < " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " <= " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        default:
          break;
      }
    } break;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_2D: {
      code += std::string("\t") + std::string("{") + std::string("\n");
      switch (func) {
        case VisualShaderNodeCompare::FUNC_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec2 _bv = equal(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_NOT_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec2 _bv = notEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN:
          code += std::string("\t") + std::string("\t") + "bvec2 _bv = greaterThan(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec2 _bv = greaterThanEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN:
          code += std::string("\t") + std::string("\t") + "bvec2 _bv = lessThan(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec2 _bv = lessThanEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        default:
          break;
      }

      switch (cond) {
        case VisualShaderNodeCompare::COND_ALL:
          code += std::string("\t") + std::string("\t") + output_vars.at(0) + " = all(_bv);" + std::string("\n");
          break;
        case VisualShaderNodeCompare::COND_ANY:
          code += std::string("\t") + std::string("\t") + output_vars.at(0) + " = any(_bv);" + std::string("\n");
          break;
        default:
          break;
      }

      code += std::string("\t") + std::string("}") + std::string("\n");
    } break;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_3D: {
      code += std::string("\t") + std::string("{") + std::string("\n");

      switch (func) {
        case VisualShaderNodeCompare::FUNC_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec3 _bv = equal(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_NOT_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec3 _bv = notEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN:
          code += std::string("\t") + std::string("\t") + "bvec3 _bv = greaterThan(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec3 _bv = greaterThanEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN:
          code += std::string("\t") + std::string("\t") + "bvec3 _bv = lessThan(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec3 _bv = lessThanEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        default:
          break;
      }

      switch (cond) {
        case VisualShaderNodeCompare::COND_ALL:
          code += std::string("\t") + std::string("\t") + output_vars.at(0) + " = all(_bv);" + std::string("\n");
          break;
        case VisualShaderNodeCompare::COND_ANY:
          code += std::string("\t") + std::string("\t") + output_vars.at(0) + " = any(_bv);" + std::string("\n");
          break;
        default:
          break;
      }

      code += std::string("\t") + std::string("}") + std::string("\n");
    } break;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_4D: {
      code += std::string("\t") + std::string("{") + std::string("\n");

      switch (func) {
        case VisualShaderNodeCompare::FUNC_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec4 _bv = equal(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_NOT_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec4 _bv = notEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN:
          code += std::string("\t") + std::string("\t") + "bvec4 _bv = greaterThan(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec4 _bv = greaterThanEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN:
          code += std::string("\t") + std::string("\t") + "bvec4 _bv = lessThan(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL:
          code += std::string("\t") + std::string("\t") + "bvec4 _bv = lessThanEqual(" + input_vars.at(0) + ", " +
                  input_vars.at(1) + ");" + std::string("\n");
          break;
        default:
          break;
      }

      switch (cond) {
        case VisualShaderNodeCompare::COND_ALL:
          code += std::string("\t") + std::string("\t") + output_vars.at(0) + " = all(_bv);" + std::string("\n");
          break;
        case VisualShaderNodeCompare::COND_ANY:
          code += std::string("\t") + std::string("\t") + output_vars.at(0) + " = any(_bv);" + std::string("\n");
          break;
        default:
          break;
      }

      code += std::string("\t") + std::string("}") + std::string("\n");
    } break;
    case VisualShaderNodeCompare::CMP_TYPE_BOOLEAN: {
      switch (func) {
        case VisualShaderNodeCompare::FUNC_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " == " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_NOT_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = " + input_vars.at(0) + " != " + input_vars.at(1) + ";" +
                  std::string("\n");
          break;
        case VisualShaderNodeCompare::FUNC_GREATER_THAN:
        case VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL:
        case VisualShaderNodeCompare::FUNC_LESS_THAN:
        case VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL:
          code += std::string("\t") + output_vars.at(0) + " = false;" + std::string("\n");
          break;
        default:
          break;
      }
    } break;
    default:
      break;
  }
  return code;
}
