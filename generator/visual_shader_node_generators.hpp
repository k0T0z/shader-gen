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
      const VisualShaderNodeInputType& input_type) : VisualShaderNodeGenerator(), input_type(input_type) {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const;

 private:
  const VisualShaderNodeInputType input_type;
};

class VisualShaderNodeGeneratorOutput : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorOutput() : VisualShaderNodeGenerator() {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const;
};

/*************************************/
/* CONSTANTS                         */
/*************************************/

class VisualShaderNodeGeneratorFloatConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorFloatConstant(const float& value) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float value;
};

class VisualShaderNodeGeneratorIntConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIntConstant(const int& value) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const int value;
};

class VisualShaderNodeGeneratorUIntConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorUIntConstant(const unsigned int& value) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const unsigned int value;
};

class VisualShaderNodeGeneratorBoolConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorBoolConstant(const bool& value) : VisualShaderNodeGenerator(), value(value) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const bool value;
};

class VisualShaderNodeGeneratorColorConstant : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorColorConstant(const float& r, const float& g, const float& b,
                                         const float& a)
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
  VisualShaderNodeGeneratorVec2Constant(const float& x, const float& y)
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
  VisualShaderNodeGeneratorVec3Constant(const float& x, const float& y, const float& z)
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
  VisualShaderNodeGeneratorVec4Constant(const float& x, const float& y, const float& z,
                                        const float& w)
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
  VisualShaderNodeGeneratorFloatOp(const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType& op)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op;
};

class VisualShaderNodeGeneratorIntOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIntOp(const VisualShaderNodeIntOp::VisualShaderNodeIntOpType& op)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op;
};

class VisualShaderNodeGeneratorUIntOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorUIntOp(const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType& op)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op;
};

class VisualShaderNodeGeneratorVectorOp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorOp(const VisualShaderNodeVectorType& type,
    const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType& op)
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
  VisualShaderNodeGeneratorFloatFunc(const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType& func)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func;
};

class VisualShaderNodeGeneratorIntFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIntFunc(const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType& func)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func;
};

class VisualShaderNodeGeneratorUIntFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorUIntFunc(const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType& func)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func;
};

class VisualShaderNodeGeneratorVectorFunc : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorFunc(const VisualShaderNodeVectorType& type,
    const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType& func)
      : VisualShaderNodeGenerator(), type(type), func(func) {}

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

class VisualShaderNodeGeneratorDotProduct : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorDotProduct()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorVectorLen : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorLen()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorClamp : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorClamp()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorStep : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorStep()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorSmoothStep : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorSmoothStep()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorVectorDistance : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorDistance()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorMix : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorMix()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorVectorCompose : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorCompose(const VisualShaderNodeVectorType& type)
      : VisualShaderNodeGenerator(), type(type) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

  private:
    const VisualShaderNodeVectorType type;
};

class VisualShaderNodeGeneratorVectorDecompose : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorVectorDecompose(const VisualShaderNodeVectorType& type)
      : VisualShaderNodeGenerator(), type(type) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

  private:
    const VisualShaderNodeVectorType type;
};

/*************************************/
/* Logic                             */
/*************************************/

class VisualShaderNodeGeneratorIf : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIf()
      : VisualShaderNodeGenerator() {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;
};

class VisualShaderNodeGeneratorSwitch : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorSwitch(const VisualShaderNodeSwitch::VisualShaderNodeSwitchOpType& op)
      : VisualShaderNodeGenerator(), op(op) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

  private:
  const VisualShaderNodeSwitch::VisualShaderNodeSwitchOpType op;
};

class VisualShaderNodeGeneratorIs : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorIs(const VisualShaderNodeIs::Function& func)
      : VisualShaderNodeGenerator(), func(func) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

  private:
  const VisualShaderNodeIs::Function func;
};

class VisualShaderNodeGeneratorCompare : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorCompare(const VisualShaderNodeCompare::ComparisonType& comp, 
                                   const VisualShaderNodeCompare::Function& func, 
                                   const VisualShaderNodeCompare::Condition& cond)
      : VisualShaderNodeGenerator(), comp(comp), func(func), cond(cond) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

  private:
  const VisualShaderNodeCompare::ComparisonType comp;
  const VisualShaderNodeCompare::Function func;
  const VisualShaderNodeCompare::Condition cond;
};

#endif  // ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP
