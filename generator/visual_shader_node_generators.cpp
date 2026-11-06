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

#include <cmath>
#include <iomanip>
#include <sstream>

VisualShaderNodeGeneratorInput::VisualShaderNodeGeneratorInput(const VisualShaderNodeInput::VisualShaderNodeInputType& input_type) 
    : VisualShaderNodeGenerator(), input_type(input_type) {
  input_types_value_names[(int)VisualShaderNodeInput::INPUT_TYPE_UNSPECIFIED] = "";
  input_types_value_names[(int)VisualShaderNodeInput::INPUT_TYPE_UV] = "FragCoord";
  input_types_value_names[(int)VisualShaderNodeInput::INPUT_TYPE_TIME] = "uTime";
}

std::string VisualShaderNodeGeneratorInput::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  // Which value name we will face first?
  // This differs from compiler to another so we do this workaround.
  // The order is important for the tests to pass.
  std::vector<std::string> temp;
  temp.resize(input_types_value_names.size());

  for (const auto& [key, value] : input_types_value_names) {
    switch (key) {
      case VisualShaderNodeInput::INPUT_TYPE_UV: {
        temp.at(0) = "in vec2 " + value + ";" + std::string("\n");
      } break;
      case VisualShaderNodeInput::INPUT_TYPE_TIME: {
        temp.at(1) = "uniform float " + value + ";" + std::string("\n");
      } break;
      default:
        break;
    }
  }

  for (const auto& value : temp) {
    if (!value.empty()) code += value;
  }

  return code;
}

std::string VisualShaderNodeGeneratorInput::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code;

  switch (input_type) {
    case VisualShaderNodeInput::INPUT_TYPE_UV: {
      code = std::string("\t") + output_vars.at(0) + " = " + input_types_value_names.at(VisualShaderNodeInput::INPUT_TYPE_UV) + ";" + std::string("\n");
    } break;
    case VisualShaderNodeInput::INPUT_TYPE_TIME: {
      code = std::string("\t") + output_vars.at(0) + " = " + input_types_value_names.at(VisualShaderNodeInput::INPUT_TYPE_TIME) + ";" + std::string("\n");
    } break;
    default:
      code = "0.0;" + std::string("\n");
      break;
  }

  return code;
}

VisualShaderNodeGeneratorOutput::VisualShaderNodeGeneratorOutput() : VisualShaderNodeGenerator() {
  output_types_value_names[0] = "FragColor";
}

std::string VisualShaderNodeGeneratorOutput::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  // Which value name we will face first?
  // This differs from compiler to another so we do this workaround.
  std::vector<std::string> temp;
  temp.resize(output_types_value_names.size());

  for (const auto& [key, value] : output_types_value_names) {
    if (value == "FragColor") temp.at(0) = "out vec4 " + value + ";" + std::string("\n");
  }

  for (const auto& value : temp) {
    if (!value.empty()) code += value;
  }

  return code;
}

std::string VisualShaderNodeGeneratorOutput::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::string code;

  int i{0};
  for (const auto& [key, value] : output_types_value_names) {
    if (!input_vars.at(i).empty()) code += std::string("\t") + value + " = " + input_vars.at(i) + ";" + std::string("\n");
    ++i;
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
    case VisualShaderNodeFloatOp::OP_TYPE_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_MOD:
      code += "mod(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_POW:
      code += "pow(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_ATAN2:
      code += "atan(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatOp::OP_TYPE_STEP:
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
    case VisualShaderNodeIntOp::OP_TYPE_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_MOD:
      code += input_vars.at(0) + " % " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_BITWISE_AND:
      code += input_vars.at(0) + " & " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_BITWISE_OR:
      code += input_vars.at(0) + " | " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_BITWISE_XOR:
      code += input_vars.at(0) + " ^ " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_BITWISE_LEFT_SHIFT:
      code += input_vars.at(0) + " << " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeIntOp::OP_TYPE_BITWISE_RIGHT_SHIFT:
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
    case VisualShaderNodeUIntOp::OP_TYPE_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_MOD:
      code += input_vars.at(0) + " % " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_BITWISE_AND:
      code += input_vars.at(0) + " & " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_BITWISE_OR:
      code += input_vars.at(0) + " | " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_BITWISE_XOR:
      code += input_vars.at(0) + " ^ " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_BITWISE_LEFT_SHIFT:
      code += input_vars.at(0) + " << " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeUIntOp::OP_TYPE_BITWISE_RIGHT_SHIFT:
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
    case VisualShaderNodeVectorOp::OP_TYPE_ADD:
      code += input_vars.at(0) + " + " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_SUB:
      code += input_vars.at(0) + " - " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_MUL:
      code += input_vars.at(0) + " * " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_DIV:
      code += input_vars.at(0) + " / " + input_vars.at(1) + ";" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_MOD:
      code += "mod(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_POW:
      code += "pow(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_MAX:
      code += "max(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_MIN:
      code += "min(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_CROSS:
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
    case VisualShaderNodeVectorOp::OP_TYPE_ATAN2:
      code += "atan(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_REFLECT:
      code += "reflect(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorOp::OP_TYPE_STEP:
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
    case VisualShaderNodeFloatFunc::FUNC_TYPE_SIN:
      code += "sin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_COS:
      code += "cos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_TAN:
      code += "tan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ASIN:
      code += "asin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ACOS:
      code += "acos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ATAN:
      code += "atan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_SINH:
      code += "sinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_COSH:
      code += "cosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_TANH:
      code += "tanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_LOG:
      code += "log(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_EXP:
      code += "exp(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_SQRT:
      code += "sqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ABS:
      code += "abs(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_SIGN:
      code += "sign(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_FLOOR:
      code += "floor(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ROUND:
      code += "round(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_CEIL:
      code += "ceil(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_FRACT:
      code += "fract(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_SATURATE:
      code += "min(max(" + input_vars.at(0) + ", 0.0), 1.0);" +
              std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ACOSH:
      code += "acosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ASINH:
      code += "asinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ATANH:
      code += "atanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_DEGREES:
      code += "degrees(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_EXP2:
      code += "exp2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_INVERSE_SQRT:
      code += "inversesqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_LOG2:
      code += "log2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_RADIANS:
      code += "radians(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_RECIPROCAL:
      code += "1.0 / (" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ROUNDEVEN:
      code += "roundEven(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_TRUNC:
      code += "trunc(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeFloatFunc::FUNC_TYPE_ONEMINUS:
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
    case VisualShaderNodeIntFunc::FUNC_TYPE_ABS:
      code += "abs(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntFunc::FUNC_TYPE_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntFunc::FUNC_TYPE_SIGN:
      code += "sign(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeIntFunc::FUNC_TYPE_BITWISE_NOT:
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
    case VisualShaderNodeUIntFunc::FUNC_TYPE_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeUIntFunc::FUNC_TYPE_BITWISE_NOT:
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
    case VisualShaderNodeVectorFunc::FUNC_TYPE_NORMALIZE:
      code += "normalize(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_SATURATE:
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
    case VisualShaderNodeVectorFunc::FUNC_TYPE_NEGATE:
      code += "-(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_RECIPROCAL:
      code += "1.0 / (" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ABS:
      code += "abs(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ACOS:
      code += "acos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ACOSH:
      code += "acosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ASIN:
      code += "asin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ASINH:
      code += "asinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ATAN:
      code += "atan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ATANH:
      code += "atanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_CEIL:
      code += "ceil(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_COS:
      code += "cos(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_COSH:
      code += "cosh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_DEGREES:
      code += "degrees(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_EXP:
      code += "exp(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_EXP2:
      code += "exp2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_FLOOR:
      code += "floor(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_FRACT:
      code += "fract(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_INVERSE_SQRT:
      code += "inversesqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_LOG:
      code += "log(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_LOG2:
      code += "log2(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_RADIANS:
      code += "radians(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ROUND:
      code += "round(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ROUNDEVEN:
      code += "roundEven(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_SIGN:
      code += "sign(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_SIN:
      code += "sin(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_SINH:
      code += "sinh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_SQRT:
      code += "sqrt(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_TAN:
      code += "tan(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_TANH:
      code += "tanh(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_TRUNC:
      code += "trunc(" + input_vars.at(0) + ");" + std::string("\n");
      break;
    case VisualShaderNodeVectorFunc::FUNC_TYPE_ONEMINUS:
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
/* SPECIAL                           */
/*************************************/

std::string VisualShaderNodeGeneratorColorize::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const auto to_glsl_color = [&to_glsl_float](const VisualShaderColorGradientStop& stop) -> std::string {
    return "vec4(" + to_glsl_float(stop.r) + ", " + to_glsl_float(stop.g) + ", " + to_glsl_float(stop.b) + ", " +
           to_glsl_float(stop.a) + ")";
  };

  // Fall back to the default color gradient when no stops are provided so that
  // the generated ramp is always well-formed.
  std::vector<VisualShaderColorGradientStop> stops{gradient};
  if (stops.empty()) {
    stops.push_back(VisualShaderColorGradientStop{0.0f, 0.0f, 0.0f, 0.0f, 1.0f});
    stops.push_back(VisualShaderColorGradientStop{1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
  }

  const std::string out_buffer{"colorize_out_n" + std::to_string(id)};
  const std::string pos_buffer{"colorize_pos_n" + std::to_string(id)};

  std::string code;

  code += std::string("\t") + "// Colorize" + std::string("\n");
  code += std::string("\t") + "float " + pos_buffer + " = (" + input_vars.at(0) + ".x + " + input_vars.at(0) +
          ".y + " + input_vars.at(0) + ".z) / 3.0;" + std::string("\n");
  code += std::string("\t") + "vec4 " + out_buffer + " = " + to_glsl_color(stops.front()) + ";" + std::string("\n");

  // A single stop yields a constant color, matching the source behavior where
  // the first and last entry are the same.
  if (stops.size() > 1) {
    code += std::string("\t") + "if (" + pos_buffer + " >= " + to_glsl_float(stops.back().position) + ") {" +
            std::string("\n");
    code += std::string("\t") + std::string("\t") + out_buffer + " = " + to_glsl_color(stops.back()) + ";" +
            std::string("\n");
    code += std::string("\t") + "} else if (" + pos_buffer + " > " + to_glsl_float(stops.front().position) + ") {" +
            std::string("\n");

    const std::size_t segment_count{stops.size() - 1};
    for (std::size_t i = 0; i < segment_count; ++i) {
      const VisualShaderColorGradientStop& a{stops.at(i)};
      const VisualShaderColorGradientStop& b{stops.at(i + 1)};

      if (i == 0) {
        code += std::string("\t") + std::string("\t") + "if (" + pos_buffer + " < " + to_glsl_float(b.position) + ") {" +
                std::string("\n");
      } else if (i < segment_count - 1) {
        code += std::string("\t") + std::string("\t") + "} else if (" + pos_buffer + " < " + to_glsl_float(b.position) +
                ") {" + std::string("\n");
      } else {
        code += std::string("\t") + std::string("\t") + "} else {" + std::string("\n");
      }

      const std::string interp_buffer{"colorize_interp_n" + std::to_string(id) + "_" + std::to_string(i)};
      code += std::string("\t") + std::string("\t") + std::string("\t") + "float " + interp_buffer + " = (" +
              pos_buffer + " - " + to_glsl_float(a.position) + ") / " +
              to_glsl_float(b.position - a.position) + ";" + std::string("\n");
      code += std::string("\t") + std::string("\t") + std::string("\t") + out_buffer + " = mix(" + to_glsl_color(a) +
              ", " + to_glsl_color(b) + ", " + interp_buffer + ");" + std::string("\n");
    }

    code += std::string("\t") + std::string("\t") + "}" + std::string("\n");  // close inner chain
    code += std::string("\t") + "}" + std::string("\n");                     // close outer else-if
  }

  code += std::string("\t") + output_vars.at(0) + " = " + out_buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorColorCorrect::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  code += "int color_correct_max_idx(vec3 c) {" + std::string("\n");
  code += std::string("\t") + "if (c.x > c.y) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "if (c.x > c.z) return 0;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "return 2;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "if (c.y > c.z) return 1;" + std::string("\n");
  code += std::string("\t") + "return 2;" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "int color_correct_min_idx(vec3 c) {" + std::string("\n");
  code += std::string("\t") + "if (c.x < c.y) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "if (c.x < c.z) return 0;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "return 2;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "if (c.y < c.z) return 1;" + std::string("\n");
  code += std::string("\t") + "return 2;" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "vec3 rgb_to_hsv(vec3 rgb) {" + std::string("\n");
  code += std::string("\t") + "vec3 temp = rgb;" + std::string("\n");
  code += std::string("\t") + "int maxi = color_correct_max_idx(temp);" + std::string("\n");
  code += std::string("\t") + "int mini = color_correct_min_idx(temp);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "float h = 0.0;" + std::string("\n");
  code += std::string("\t") + "float s = 0.0;" + std::string("\n");
  code += std::string("\t") + "float v = temp[maxi];" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "if (temp[maxi] == 0.0) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "h = 0.0;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "s = 0.0;" + std::string("\n");
  code += std::string("\t") + "} else if (temp[maxi] == temp[mini]) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "h = 0.0;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "s = 0.0;" + std::string("\n");
  code += std::string("\t") + "} else {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "s = 255.0 * (1.0 - (temp[mini] / temp[maxi]));" + std::string("\n");
  code += std::string("\t") + std::string("\t") +
          "h = 60.0 * (float(maxi * 2) + (temp[(maxi + 1) % 3] - temp[(maxi + 2) % 3]) / (temp[maxi] - temp[mini]));" +
          std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "if (h < 0.0) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "h += 360.0;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "return vec3(h, s, v);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "vec3 hsv_to_rgb(vec3 hsv) {" + std::string("\n");
  code += std::string("\t") + "float h = hsv.x;" + std::string("\n");
  code += std::string("\t") + "float s = hsv.y;" + std::string("\n");
  code += std::string("\t") + "float v = hsv.z;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "int maxc = 0;" + std::string("\n");
  code += std::string("\t") + "if (h > 300.0) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "h -= 360.0;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "if (h < 60.0) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "maxc = 0;" + std::string("\n");
  code += std::string("\t") + "} else if (h >= 180.0) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "maxc = 2;" + std::string("\n");
  code += std::string("\t") + "} else {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "maxc = 1;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "vec3 temp = vec3(0.0);" + std::string("\n");
  code += std::string("\t") + "temp[maxc] = v;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "float mn = (v * (255.0 - s)) / 255.0;" + std::string("\n");
  code += std::string("\t") + "float temphue = (((h / 60.0) - float(2 * maxc)) * (v - mn));" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "if (temphue > 0.0) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "temp[(maxc + 2) % 3] = mn;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "temp[(maxc + 1) % 3] = mn + temphue;" + std::string("\n");
  code += std::string("\t") + "} else {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "temp[(maxc + 1) % 3] = mn;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "temp[(maxc + 2) % 3] = mn - temphue;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "return temp;" + std::string("\n");
  code += std::string("}") + "\n\n";

  return code;
}

std::string VisualShaderNodeGeneratorColorCorrect::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string out_buffer{"out_buffer_n" + std::to_string(id)};
  const std::string hsv_buffer{"out_buffer_hsv_n" + std::to_string(id)};

  std::string code;

  code += std::string("\t") + "// ColorCorrect" + std::string("\n");
  code += std::string("\t") + "vec4 " + out_buffer + " = " + input_vars.at(0) + ";" + std::string("\n");
  code += std::string("\t") + out_buffer + " = ((" + out_buffer + " - 0.5) * " + to_glsl_float(contrast) + " + " +
          to_glsl_float(brightness) + ") + 0.5;" + std::string("\n");
  code += std::string("\t") + out_buffer + " = clamp(" + out_buffer + ", 0.0, 1.0);" + std::string("\n");

  if (desaturate != 0.0f) {
    code += std::string("\t") + "vec3 " + hsv_buffer + " = rgb_to_hsv(" + out_buffer + ".xyz);" + std::string("\n");
    code += std::string("\t") + hsv_buffer + ".y *= " + to_glsl_float(1.0f - desaturate) + ";" + std::string("\n");
    code += std::string("\t") + out_buffer + ".xyz = hsv_to_rgb(" + hsv_buffer + ");" + std::string("\n");
  }

  code += std::string("\t") + out_buffer + " = pow(" + out_buffer + ", vec4(" + to_glsl_float(1.0f / gamma) + "));" +
          std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = " + out_buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorMath1::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string buffer{"out_buffer_n" + std::to_string(id)};
  const std::string a_str{to_glsl_float(a)};

  std::string code;
  code += std::string("\t") + "// Math1" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + " = " + input_vars.at(0) + ";" + std::string("\n");

  const auto emit_channel = [&](const std::string& swizzle) {
    const std::string channel{buffer + "." + swizzle};
    switch (function) {
      case VisualShaderNodeMath1::FUNC_ADD:
        code += std::string("\t") + channel + " = " + channel + " + " + a_str + ";" + std::string("\n");
        break;
      case VisualShaderNodeMath1::FUNC_MUL:
        code += std::string("\t") + channel + " = " + channel + " * " + a_str + ";" + std::string("\n");
        break;
      case VisualShaderNodeMath1::FUNC_SUB:
        code += std::string("\t") + channel + " = " + a_str + " - " + channel + ";" + std::string("\n");
        break;
      case VisualShaderNodeMath1::FUNC_POW:
        code += std::string("\t") + channel + " = pow(" + channel + ", " + a_str + ");" + std::string("\n");
        break;
      default:
        break;
    }
  };

  if (on_r) emit_channel("x");
  if (on_g) emit_channel("y");
  if (on_b) emit_channel("z");
  if (on_a) emit_channel("w");

  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorModulus::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  // Java's float '%' operator is a truncated remainder (sign of the dividend), while GLSL's mod() is
  // a floored remainder. This helper reproduces the exact Java semantics.
  code += "float modulus_mod(float x, float y) {" + std::string("\n");
  code += std::string("\t") + "return x - y * trunc(x / y);" + std::string("\n");
  code += std::string("}") + "\n\n";

  return code;
}

std::string VisualShaderNodeGeneratorModulus::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string id_str{std::to_string(id)};
  const std::string buffer{"out_buffer_n" + id_str};
  const std::string modulus_var{"modulus_m_n" + id_str};
  const std::string scale_var{"modulus_s_n" + id_str};
  const std::string bias_var{"modulus_bias_n" + id_str};

  std::string code;
  code += std::string("\t") + "// Modulus" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + " = " + input_vars.at(0) + ";" + std::string("\n");
  code += std::string("\t") + "float " + modulus_var + " = " + to_glsl_float(modulus) + ";" + std::string("\n");
  code += std::string("\t") + "float " + scale_var + " = " +
          (normalize ? ("1.0 / " + modulus_var) : std::string("1.0")) + ";" + std::string("\n");
  code += std::string("\t") + "float " + bias_var + " = " + input_vars.at(1) + ".x * " + modulus_var + " * " +
          to_glsl_float(static_cast<float>(x_bias)) + ";" + std::string("\n");

  for (const std::string& swizzle : {"x", "y", "z"}) {
    const std::string channel{buffer + "." + swizzle};
    code += std::string("\t") + channel + " = clamp(modulus_mod(" + channel + " + " + bias_var + ", " + modulus_var +
            ") * " + scale_var + ", 0.0, 1.0);" + std::string("\n");
  }

  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

static const char* combine_channel_swizzle(
    const VisualShaderNodeCombine::VisualShaderNodeCombineChannel& channel) {
  switch (channel) {
    case VisualShaderNodeCombine::CHANNEL_R:
      return "x";
    case VisualShaderNodeCombine::CHANNEL_G:
      return "y";
    case VisualShaderNodeCombine::CHANNEL_B:
      return "z";
    case VisualShaderNodeCombine::CHANNEL_A:
      return "w";
    default:
      break;
  }
  return "x";
}

std::string VisualShaderNodeGeneratorCombine::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string buffer{"out_buffer_n" + std::to_string(id)};

  std::string code;
  code += std::string("\t") + "// Combine" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + ";" + std::string("\n");
  code += std::string("\t") + buffer + ".x = " + input_vars.at(0) + "." + combine_channel_swizzle(input_1) + ";" +
          std::string("\n");
  code += std::string("\t") + buffer + ".y = " + input_vars.at(1) + "." + combine_channel_swizzle(input_2) + ";" +
          std::string("\n");
  code += std::string("\t") + buffer + ".z = " + input_vars.at(2) + "." + combine_channel_swizzle(input_3) + ";" +
          std::string("\n");
  code += std::string("\t") + buffer + ".w = " + input_vars.at(3) + "." + combine_channel_swizzle(input_4) + ";" +
          std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorMask::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string id_str{std::to_string(id)};
  const std::string buffer{"out_buffer_n" + id_str};
  const std::string weight_var{"mask_w_n" + id_str};
  const std::string mask{input_vars.at(2)};

  std::string code;
  code += std::string("\t") + "// Mask" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + " = " + input_vars.at(0) + ";" + std::string("\n");
  code += std::string("\t") + "float " + weight_var + " = (" + mask + ".x + " + mask + ".y + " + mask + ".z) * (1.0 / 3.0);" +
          std::string("\n");

  if (invert) {
    code += std::string("\t") + weight_var + " = 1.0 - " + weight_var + ";" + std::string("\n");
  }

  code += std::string("\t") + buffer + " = " + buffer + " * (1.0 - " + weight_var + ");" + std::string("\n");
  code += std::string("\t") + buffer + " = " + buffer + " + " + weight_var + " * " + input_vars.at(1) + ";" +
          std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorBlend::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string id_str{std::to_string(id)};
  const std::string buffer{"out_buffer_n" + id_str};
  const std::string color_var{"blend_color_n" + id_str};
  const std::string alpha_var{"blend_alpha_n" + id_str};
  const std::string orig_w_var{"blend_orig_w_n" + id_str};
  const std::string in0{input_vars.at(0)};
  const std::string in1{input_vars.at(1)};

  std::string code;
  code += std::string("\t") + "// Blend" + std::string("\n");
  code += std::string("\t") + "vec4 " + color_var + " = " + in1 + ";" + std::string("\n");
  code += std::string("\t") + "float " + alpha_var + " = " + in1 + ".w;" + std::string("\n");

  if (inv_alpha) {
    code += std::string("\t") + alpha_var + " = 1.0 - " + alpha_var + ";" + std::string("\n");
  }

  code += std::string("\t") + alpha_var + " *= " + to_glsl_float(opacity) + ";" + std::string("\n");

  switch (layer_func) {
    case VisualShaderNodeBlend::BLEND_NORMAL:
      break;
    case VisualShaderNodeBlend::BLEND_MULTIPLY:
      code += std::string("\t") + color_var + " = " + color_var + " * " + in0 + ";" + std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_DIVIDE:
      code += std::string("\t") + color_var + " = " + color_var + " * vec4(1.0 / (" + in0 + ".x + 1.0), 1.0 / (" + in0 +
              ".y + 1.0), 1.0 / (" + in0 + ".z + 1.0), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_SCREEN:
      code += std::string("\t") + color_var + " = vec4(1.0 - (1.0 - " + in0 + ".x) * (1.0 - " + in1 +
              ".x), 1.0 - (1.0 - " + in0 + ".y) * (1.0 - " + in1 + ".y), 1.0 - (1.0 - " + in0 + ".z) * (1.0 - " + in1 +
              ".z), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_OVERLAY:
      code += std::string("\t") + color_var + " = vec4(" + in0 + ".x * (" + in0 + ".x + 2.0 * " + in1 + ".x * (1.0 - " +
              in0 + ".x)), " + in0 + ".y * (" + in0 + ".y + 2.0 * " + in1 + ".y * (1.0 - " + in0 + ".y)), " + in0 +
              ".z * (" + in0 + ".z + 2.0 * " + in1 + ".z * (1.0 - " + in0 + ".z)), 1.0);" + std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_DODGE:
      code += std::string("\t") + color_var + " = vec4(" + in0 + ".x / ((1.0 - " + in1 + ".x) + 1.0), " + in0 +
              ".y / ((1.0 - " + in1 + ".y) + 1.0), " + in0 + ".z / ((1.0 - " + in1 + ".z) + 1.0), 1.0);" +
              std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_BURN:
      code += std::string("\t") + color_var + " = vec4(1.0 - ((1.0 - " + in0 + ".x) / (" + in1 + ".x + 1.0)), 1.0 - ((1.0 - " +
              in0 + ".y) / (" + in1 + ".y + 1.0)), 1.0 - ((1.0 - " + in0 + ".z) / (" + in1 + ".z + 1.0)), 1.0);" +
              std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_DIFFERENCE:
      code += std::string("\t") + color_var + " = abs(" + in0 + " - " + in1 + ");" + std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_ADDITION:
      code += std::string("\t") + color_var + " = " + color_var + " + " + in0 + ";" + std::string("\n");
      break;
    case VisualShaderNodeBlend::BLEND_SUBTRACT:
      code += std::string("\t") + color_var + " = " + in0 + " - " + in1 + ";" + std::string("\n");
      break;
    default:
      break;
  }

  code += std::string("\t") + color_var + " = clamp(" + color_var + ", 0.0, 1.0);" + std::string("\n");
  code += std::string("\t") + "float " + orig_w_var + " = " + in0 + ".w;" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + " = " + in0 + " * (1.0 - " + alpha_var + ");" + std::string("\n");
  code += std::string("\t") + buffer + " = " + buffer + " + " + alpha_var + " * " + color_var + ";" + std::string("\n");
  code += std::string("\t") + buffer + ".w = " + orig_w_var + ";" + std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

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

std::string VisualShaderNodeGeneratorVectorDistance::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  return std::string("\t") + output_vars.at(0) + " = distance(" + input_vars.at(0) + ", " + input_vars.at(1) + ");" +
         std::string("\n");
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

VisualShaderNodeGeneratorSwitch::VisualShaderNodeGeneratorSwitch(const VisualShaderNodeSwitch::VisualShaderNodeSwitchType& type)
      : VisualShaderNodeGenerator(), type(type) {
  switch (type) {
    case VisualShaderNodeSwitch::TYPE_INT:
    case VisualShaderNodeSwitch::TYPE_UINT:
    case VisualShaderNodeSwitch::TYPE_BOOLEAN:
      scoped_assignment = true;
      break;
    default:
      break;
  }
}

std::string VisualShaderNodeGeneratorSwitch::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    const std::vector<std::string>& output_vars) const {
  bool use_mix{false};

  switch (type) {
    case VisualShaderNodeSwitch::TYPE_FLOAT:
    case VisualShaderNodeSwitch::TYPE_VECTOR_2D:
    case VisualShaderNodeSwitch::TYPE_VECTOR_3D:
    case VisualShaderNodeSwitch::TYPE_VECTOR_4D:
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

VisualShaderNodeGeneratorCompare::VisualShaderNodeGeneratorCompare(const VisualShaderNodeCompare::VisualShaderNodeCompareType& comp, 
                                   const VisualShaderNodeCompare::VisualShaderNodeCompareFunction& func, 
                                   const VisualShaderNodeCompare::VisualShaderNodeCompareCondition& cond)
      : VisualShaderNodeGenerator(), comp(comp), func(func), cond(cond) {
  switch (comp) {
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR:
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_UINT:
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_INT:
    case VisualShaderNodeCompare::CMP_TYPE_BOOLEAN:
      scoped_assignment = true;
      break;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_2D:
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_3D:
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_4D:
      scoped_assignment = false;
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

/*************************************/
/* Illuminate / Emboss / NormalMap   */
/*************************************/

std::string VisualShaderNodeGeneratorIlluminate::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string id_str{std::to_string(id)};
  const std::string buffer{"illuminate_out_n" + id_str};
  const std::string normal_var{"illuminate_normal_n" + id_str};
  const std::string light_var{"illuminate_light_n" + id_str};
  const std::string reflect_var{"illuminate_reflect_n" + id_str};

  constexpr float PI{3.14159265358979323846f};
  const float theta_rad{theta * (PI / 180.0f)};
  const float phi_rad{phi * (PI / 180.0f)};

  // Mirrors @c Vector3.setDir(theta, phi).
  const float light_x{std::cos(phi_rad) * std::sin(theta_rad)};
  const float light_y{std::sin(phi_rad) * std::sin(theta_rad)};
  const float light_z{std::cos(theta_rad)};

  std::string code;
  code += std::string("\t") + "// Illuminate" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + " = " + input_vars.at(0) + ";" + std::string("\n");
  code += std::string("\t") + "vec3 " + normal_var + " = (" + input_vars.at(1) + ".xyz - 0.5) * 2.0;" + std::string("\n");
  code += std::string("\t") + "vec3 " + light_var + " = vec3(" + to_glsl_float(light_x) + ", " +
          to_glsl_float(light_y) + ", " + to_glsl_float(light_z) + ");" + std::string("\n");
  code += std::string("\t") + "vec3 " + reflect_var + " = normalize(vec3(0.0, 0.0, -1.0) - 2.0 * dot(vec3(0.0, 0.0, "
          "-1.0), " + normal_var + ") * " + normal_var + ");" + std::string("\n");
  code += std::string("\t") + "float illuminate_ar_n" + id_str + " = max(dot(" + reflect_var + ", " + light_var +
          "), 0.0);" + std::string("\n");
  code += std::string("\t") + "illuminate_ar_n" + id_str + " = pow(illuminate_ar_n" + id_str + ", " +
          to_glsl_float(shininess) + ");" + std::string("\n");
  code += std::string("\t") + buffer + " += vec4(illuminate_ar_n" + id_str + ");" + std::string("\n");
  code += std::string("\t") + buffer + " = clamp(" + buffer + ", 0.0, 1.0);" + std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorEmboss::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string id_str{std::to_string(id)};
  const std::string buffer{"emboss_out_n" + id_str};
  const std::string height_var{"emboss_height_n" + id_str};

  constexpr float PI{3.14159265358979323846f};
  const float angle_rad{angle * (PI / 180.0f)};
  const float dir_x{std::cos(angle_rad)};
  const float dir_y{std::sin(angle_rad)};

  // The source evaluates the grayscale derivative of the height input; the generated shader approximates
  // it with the screen space derivatives dFdx/dFdy.
  const float add_scale_x{dir_x * strength};
  const float add_scale_y{dir_y * strength};

  std::string code;
  code += std::string("\t") + "// Emboss" + std::string("\n");
  code += std::string("\t") + "float " + height_var + " = " + input_vars.at(1) + ";" + std::string("\n");
  code += std::string("\t") + "vec4 " + buffer + " = " + input_vars.at(0) + ";" + std::string("\n");
  code += std::string("\t") + "float emboss_add_n" + id_str + " = dFdx(" + height_var + ") * " +
          to_glsl_float(add_scale_x) + " + dFdy(" + height_var + ") * " + to_glsl_float(add_scale_y) + ";" +
          std::string("\n");
  code += std::string("\t") + buffer + ".x = clamp(" + buffer + ".x + emboss_add_n" + id_str + ", 0.0, 1.0);" +
          std::string("\n");
  code += std::string("\t") + buffer + ".y = clamp(" + buffer + ".y + emboss_add_n" + id_str + ", 0.0, 1.0);" +
          std::string("\n");
  code += std::string("\t") + buffer + ".z = clamp(" + buffer + ".z + emboss_add_n" + id_str + ", 0.0, 1.0);" +
          std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = " + buffer + ";" + std::string("\n");

  return code;
}

std::string VisualShaderNodeGeneratorNormalMap::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const auto to_glsl_float = [](const float& value) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
  };

  const std::string id_str{std::to_string(id)};
  const std::string height_var{"normal_map_height_n" + id_str};

  std::string code;
  code += std::string("\t") + "// NormalMap" + std::string("\n");
  code += std::string("\t") + "float " + height_var + " = " + input_vars.at(0) + ";" + std::string("\n");
  code += std::string("\t") + "vec3 normal_map_n" + id_str + " = normalize(vec3(dFdx(" + height_var + ") * " +
          to_glsl_float(strength) + ", dFdy(" + height_var + ") * " + to_glsl_float(strength) + ", 1.0));" +
          std::string("\n");
  code += std::string("\t") + output_vars.at(0) + " = vec4(normal_map_n" + id_str + " * 0.5 + 0.5, 1.0);" +
          std::string("\n");

  return code;
}
