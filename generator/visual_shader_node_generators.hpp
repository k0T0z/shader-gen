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

#ifndef ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP
#define ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP

#include <string>
#include <vector>

#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

class VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGenerator() = default;
  virtual ~VisualShaderNodeGenerator() = default;

  virtual std::string generate_global([[maybe_unused]] const int& id) const { return ""; }
  virtual std::string generate_global_per_node([[maybe_unused]] const int& id) const { return ""; }
  virtual std::string generate_global_per_func([[maybe_unused]] const int& id) const { return ""; }

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const = 0;
};

class VisualShaderNodeGeneratorInput : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorInput(
      const VisualShaderNodeInputType& input_type = VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED)
      : VisualShaderNodeGenerator(), input_type(input_type) {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const;

 private:
  const VisualShaderNodeInputType input_type;
};

class VisualShaderNodeGeneratorOutput : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorOutput(
      const VisualShaderNodeOutputType& output_type = VisualShaderNodeOutputType::OUTPUT_TYPE_UNSPECIFIED)
      : VisualShaderNodeGenerator(), output_type(output_type) {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const;

 private:
  const VisualShaderNodeOutputType output_type;
};

/*************************************/
/* CONSTANTS                         */
/*************************************/

class VisualShaderNodeGeneratorFloatConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorFloatConstant(const float& value = 0.0f) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float value;
};

class VisualShaderNodeGeneratorIntConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIntConstant(const int& value = 0) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const int value;
};

class VisualShaderNodeGeneratorUIntConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorUIntConstant(const unsigned int& value = 0) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const unsigned int value;
};

class VisualShaderNodeGeneratorBoolConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorBoolConstant(const bool& value = false) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const bool value;
};

class VisualShaderNodeGeneratorColorConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorColorConstant(const float& r = 0.0f, const float& g = 0.0f, const float& b = 0.0f,
                                         const float& a = 1.0f)
      : VisualShaderNodeGenerator(), r(r), g(g), b(b), a(a) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float r;
  const float g;
  const float b;
  const float a;
};

class VisualShaderNodeGeneratorVec2Constant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVec2Constant(const float& x = 0.0f, const float& y = 0.0f)
      : VisualShaderNodeGenerator(), x(x), y(y) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float x;
  const float y;
};

class VisualShaderNodeGeneratorVec3Constant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVec3Constant(const float& x = 0.0f, const float& y = 0.0f, const float& z = 0.0f)
      : VisualShaderNodeGenerator(), x(x), y(y), z(z) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float x;
  const float y;
  const float z;
};

class VisualShaderNodeGeneratorVec4Constant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVec4Constant(const float& x = 0.0f, const float& y = 0.0f, const float& z = 0.0f,
                                        const float& w = 0.0f)
      : VisualShaderNodeGenerator(), x(x), y(y), z(z), w(w) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float x;
  const float y;
  const float z;
  const float w;
};

/*************************************/
/* OPERATORS                         */
/*************************************/

class VisualShaderNodeGeneratorFloatOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorFloatOp(const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType& op = VisualShaderNodeFloatOp::OP_UNSPECIFIED)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op;
};

class VisualShaderNodeGeneratorIntOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIntOp(const VisualShaderNodeIntOp::VisualShaderNodeIntOpType& op = VisualShaderNodeIntOp::OP_UNSPECIFIED)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op;
};

class VisualShaderNodeGeneratorUIntOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorUIntOp(const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType& op = VisualShaderNodeUIntOp::OP_UNSPECIFIED)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op;
};

class VisualShaderNodeGeneratorVectorOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorOp(const VisualShaderNodeVectorType& type = VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED,
    const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType& op = VisualShaderNodeVectorOp::OP_UNSPECIFIED)
      : VisualShaderNodeGenerator(), type(type), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeVectorType type;
  const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op;
};

/*************************************/
/* Funcs Node                        */
/*************************************/

class VisualShaderNodeGeneratorFloatFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorFloatFunc(const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType& func = VisualShaderNodeFloatFunc::FUNC_UNSPECIFIED)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_global_per_func([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func;
};

class VisualShaderNodeGeneratorIntFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIntFunc(const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType& func = VisualShaderNodeIntFunc::FUNC_UNSPECIFIED)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_global_per_func([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func;
};

class VisualShaderNodeGeneratorUIntFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorUIntFunc(const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType& func = VisualShaderNodeUIntFunc::FUNC_UNSPECIFIED)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_global_per_func([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func;
};

class VisualShaderNodeGeneratorVectorFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorFunc(const VisualShaderNodeVectorType& type = VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED,
    const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType& func = VisualShaderNodeVectorFunc::FUNC_UNSPECIFIED)
      : VisualShaderNodeGenerator(), type(type), func(func) {}

  virtual std::string generate_global_per_func([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeVectorType type;
  const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func;
};

/*************************************/
/* MISC                              */
/*************************************/

#endif  // ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP
