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

#ifndef ENIGMA_VISUAL_SHADER_NODE_PORT_TYPE_GENERATOR_HPP
#define ENIGMA_VISUAL_SHADER_NODE_PORT_TYPE_GENERATOR_HPP

#include <string>
#include <vector>
#include <memory>

#include "gui/model/schema/visual_shader_nodes.pb.h"
#include "gui/controller/vs_proto_node.hpp"

using namespace gui::model::schema;

enum class VisualShaderNodePortType {
  PORT_TYPE_UNSPECIFIED = 0,
  PORT_TYPE_SCALAR = 1,
  PORT_TYPE_SCALAR_INT = 2,
  PORT_TYPE_SCALAR_UINT = 3,
  PORT_TYPE_VECTOR_2D = 4,
  PORT_TYPE_VECTOR_3D = 5,
  PORT_TYPE_VECTOR_4D = 6,
  PORT_TYPE_BOOLEAN = 7,
};

class VisualShaderNodePortTypeGenerator {
 public:
  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const = 0;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const = 0;
};

class VisualShaderNodePortTypeGeneratorInput : public VisualShaderNodePortTypeGenerator {
  public:
   VisualShaderNodePortTypeGeneratorInput(const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const VisualShaderNodeInput::VisualShaderNodeInputType& input_type) 
     : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), input_type(input_type) {}
 
  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;
 
  private:
   const std::shared_ptr<IVisualShaderProtoNode> proto_node;
   const VisualShaderNodeInput::VisualShaderNodeInputType input_type;
};

class VisualShaderNodePortTypeGeneratorOutput : public VisualShaderNodePortTypeGenerator {
  public:
   VisualShaderNodePortTypeGeneratorOutput(const std::shared_ptr<IVisualShaderProtoNode>& proto_node) 
     : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}
 
    virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
    virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;
 
  private:
   const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorFloatConstant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorFloatConstant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorIntConstant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorIntConstant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorUIntConstant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorUIntConstant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorBooleanConstant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorBooleanConstant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorColorConstant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorColorConstant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorVec2Constant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVec2Constant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorVec3Constant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVec3Constant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorVec4Constant : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVec4Constant(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorFloatOp : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorFloatOp(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType& op_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), op_type(op_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType op_type;
};

class VisualShaderNodePortTypeGeneratorIntOp : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorIntOp(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeIntOp::VisualShaderNodeIntOpType& op_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), op_type(op_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeIntOp::VisualShaderNodeIntOpType op_type;
};

class VisualShaderNodePortTypeGeneratorUIntOp : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorUIntOp(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType& op_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), op_type(op_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType op_type;
};

class VisualShaderNodePortTypeGeneratorVectorOp : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVectorOp(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeVectorType& type, const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType& op_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), type(type), op_type(op_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeVectorType type;
    const VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType op_type;
};

class VisualShaderNodePortTypeGeneratorFloatFunc : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorFloatFunc(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType& func_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), func_type(func_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType func_type;
};

class VisualShaderNodePortTypeGeneratorIntFunc : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorIntFunc(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType& func_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), func_type(func_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType func_type;
};

class VisualShaderNodePortTypeGeneratorUIntFunc : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorUIntFunc(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType& func_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), func_type(func_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType func_type;
};

class VisualShaderNodePortTypeGeneratorVectorFunc : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVectorFunc(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeVectorType& type, const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType& func_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), type(type), func_type(func_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

  private:
    const std::shared_ptr<IVisualShaderProtoNode> proto_node;
    const VisualShaderNodeVectorType type;
    const VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType func_type;
};

class VisualShaderNodePortTypeGeneratorValueNoise : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorValueNoise(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorPerlinNoise : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorPerlinNoise(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorVoronoiNoise : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVoronoiNoise(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorDotProduct : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorDotProduct(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorVectorLen : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVectorLen(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeVectorType& vec_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), vec_type(vec_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeVectorType vec_type;
};

class VisualShaderNodePortTypeGeneratorClamp : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorClamp(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeClamp::VisualShaderNodeClampType& type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), type(type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeClamp::VisualShaderNodeClampType type;
};

class VisualShaderNodePortTypeGeneratorVectorDistance : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVectorDistance(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeVectorType& vec_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), vec_type(vec_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeVectorType vec_type;
};

class VisualShaderNodePortTypeGeneratorVectorCompose : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVectorCompose(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeVectorType& vec_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), vec_type(vec_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeVectorType vec_type;
};

class VisualShaderNodePortTypeGeneratorVectorDecompose : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorVectorDecompose(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeVectorType& vec_type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), vec_type(vec_type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeVectorType vec_type;
};

class VisualShaderNodePortTypeGeneratorIf : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorIf(const std::shared_ptr<IVisualShaderProtoNode> proto_node)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
};

class VisualShaderNodePortTypeGeneratorSwitch : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorSwitch(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeSwitch::VisualShaderNodeSwitchType& type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), type(type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeSwitch::VisualShaderNodeSwitchType type;
};

class VisualShaderNodePortTypeGeneratorIs : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorIs(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeIs::VisualShaderNodeIsFunction& func)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), func(func) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeIs::VisualShaderNodeIsFunction func;
};

class VisualShaderNodePortTypeGeneratorCompare : public VisualShaderNodePortTypeGenerator {
 public:
  VisualShaderNodePortTypeGeneratorCompare(const std::shared_ptr<IVisualShaderProtoNode> proto_node, const VisualShaderNodeCompare::VisualShaderNodeCompareType& type)
      : VisualShaderNodePortTypeGenerator(), proto_node(proto_node), type(type) {}

  virtual VisualShaderNodePortType get_input_port_type(const int& port_index) const override;
  virtual VisualShaderNodePortType get_output_port_type(const int& port_index) const override;

 private:
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;
  const VisualShaderNodeCompare::VisualShaderNodeCompareType type;
};

#endif  // ENIGMA_VISUAL_SHADER_NODE_PORT_TYPE_GENERATOR_HPP
