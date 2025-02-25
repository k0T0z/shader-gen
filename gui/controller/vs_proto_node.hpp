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

#ifndef VISUAL_SHADER_PROTO_NODE_HPP
#define VISUAL_SHADER_PROTO_NODE_HPP

#include <google/protobuf/descriptor.h>
#include <QMetaType>
#include <string>
#include <variant>
#include "error_macros.hpp"

#include "gui/model/schema/visual_shader.pb.h"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

class IVisualShaderProtoNode {
 public:
  virtual ~IVisualShaderProtoNode() = default;

  /**
   * @brief Get the name object
   * 
   * @note This function is used in the generate_shader function.
   * 
   * @return std::string 
   */
  virtual std::string get_name() const = 0;

  virtual std::string get_caption() const = 0;

  virtual int get_input_port_count() const = 0;
  virtual VisualShaderNodePortType get_input_port_type(const int& index) const = 0;
  virtual std::string get_input_port_caption(const int& index) const = 0;

  virtual int get_output_port_count() const = 0;
  virtual VisualShaderNodePortType get_output_port_type(const int& index) const = 0;
  virtual std::string get_output_port_caption(const int& index) const = 0;

  virtual VisualShaderNodeCategory get_category() const = 0;
  virtual std::string get_category_path() const = 0;

  virtual std::string get_description() const = 0;

  virtual int get_oneof_value_field_number() const = 0;
};

template <typename Proto>
class VisualShaderProtoNode : public IVisualShaderProtoNode {
 public:
  VisualShaderProtoNode() : oneof_value_field_number(set_oneof_value_field_number()) {}

  std::string get_name() const override {
    return Proto::descriptor()->name();
  }

  std::string get_caption() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_caption), "",
                                  "Node caption not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_caption);
  }

  int get_input_port_count() const override {
    CHECK_CONDITION_TRUE_NON_VOID(
        !Proto::descriptor()->options().HasExtension(gui::model::schema::node_input_port_count), 0,
        "Input port count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_count);
  }

  VisualShaderNodePortType get_input_port_type(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED,
                            "Invalid input port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_type),
                            VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_type, index);
  }

  std::string get_input_port_caption(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), "", "Invalid input port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_caption),
                            "", "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_caption, index);
  }

  int get_output_port_count() const override {
    CHECK_CONDITION_TRUE_NON_VOID(
        !Proto::descriptor()->options().HasExtension(gui::model::schema::node_output_port_count), 0,
        "Output port count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_count);
  }

  VisualShaderNodePortType get_output_port_type(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED,
                            "Invalid output port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_type),
                            VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_type, index);
  }

  std::string get_output_port_caption(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), "", "Invalid output port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_caption),
                            "", "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_caption, index);
  }

  VisualShaderNodeCategory get_category() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category),
                                  VisualShaderNodeCategory::CATEGORY_UNSPECIFIED, "Node category not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category);
  }

  std::string get_category_path() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category_path),
                                  "", "Node category path not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category_path);
  }

  std::string get_description() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_description),
                                  "", "Node description not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_description);
  }

  int get_oneof_value_field_number() const override {
    return oneof_value_field_number;
  }

 private:
  const int oneof_value_field_number;

  /**
   * @brief Set the oneof value field number object
   * 
   * @note Because the way Google Protobuf works, we can't fetch the oneof value unless it 
   *       is already set using Reflection. This function is used get the oneof value field
   *       number for the current Proto type so that we can use it to set the oneof value.
   * 
   * @return constexpr int 
   */
  static constexpr int set_oneof_value_field_number() {
    if constexpr (std::is_same_v<Proto, VisualShaderNodeInput>) return VisualShader::VisualShaderNode::kInputFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeOutput>) return VisualShader::VisualShaderNode::kOutputFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeFloatConstant>) return VisualShader::VisualShaderNode::kFloatConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeIntConstant>) return VisualShader::VisualShaderNode::kIntConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeUIntConstant>) return VisualShader::VisualShaderNode::kUintConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeBooleanConstant>) return VisualShader::VisualShaderNode::kBooleanConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeColorConstant>) return VisualShader::VisualShaderNode::kColorConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVec2Constant>) return VisualShader::VisualShaderNode::kVec2ConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVec3Constant>) return VisualShader::VisualShaderNode::kVec3ConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVec4Constant>) return VisualShader::VisualShaderNode::kVec4ConstantFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeFloatOp>) return VisualShader::VisualShaderNode::kFloatOpFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeIntOp>) return VisualShader::VisualShaderNode::kIntOpFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeUIntOp>) return VisualShader::VisualShaderNode::kUintOpFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVectorOp>) return VisualShader::VisualShaderNode::kVectorOpFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeFloatFunc>) return VisualShader::VisualShaderNode::kFloatFuncFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeIntFunc>) return VisualShader::VisualShaderNode::kIntFuncFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeUIntFunc>) return VisualShader::VisualShaderNode::kUintFuncFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVectorFunc>) return VisualShader::VisualShaderNode::kVectorFuncFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeValueNoise>) return VisualShader::VisualShaderNode::kValueNoiseFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodePerlinNoise>) return VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVoronoiNoise>) return VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeDotProduct>) return VisualShader::VisualShaderNode::kDotProductFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVectorLen>) return VisualShader::VisualShaderNode::kVectorLenFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeClamp>) return VisualShader::VisualShaderNode::kClampFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeStep>) return VisualShader::VisualShaderNode::kStepFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeSmoothStep>) return VisualShader::VisualShaderNode::kSmoothStepFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeVectorDistance>) return VisualShader::VisualShaderNode::kVectorDistanceFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeMix>) return VisualShader::VisualShaderNode::kMixFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNode2dVectorCompose>) return VisualShader::VisualShaderNode::kVector2DComposeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNode3dVectorCompose>) return VisualShader::VisualShaderNode::kVector3DComposeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNode4dVectorCompose>) return VisualShader::VisualShaderNode::kVector4DComposeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNode2dVectorDecompose>) return VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNode3dVectorDecompose>) return VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNode4dVectorDecompose>) return VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeIf>) return VisualShader::VisualShaderNode::kIfNodeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeSwitch>) return VisualShader::VisualShaderNode::kSwitchNodeFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeIs>) return VisualShader::VisualShaderNode::kIsFieldNumber;
    else if constexpr (std::is_same_v<Proto, VisualShaderNodeCompare>) return VisualShader::VisualShaderNode::kCompareFieldNumber;
    else {
      static_assert(false, "Unsupported proto type!");
      return -1;
    }
  }
};

Q_DECLARE_METATYPE(std::shared_ptr<IVisualShaderProtoNode>)  // Required for QVariant

#endif  // VISUAL_SHADER_PROTO_NODE_HPP
