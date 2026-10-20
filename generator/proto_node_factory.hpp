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

#ifndef SHADERGEN_PROTO_NODE_FACTORY_HPP
#define SHADERGEN_PROTO_NODE_FACTORY_HPP

#include <memory>
#include <string>

#include "error_macros.hpp"
#include "generator/proto_node.hpp"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

namespace shadergen_utils {
inline static std::shared_ptr<IVisualShaderProtoNode> get_proto_node_by_oneof_value_field_number(const int& oneof_value_field_number) noexcept {
  switch (oneof_value_field_number) {
    case VisualShader::VisualShaderNode::kInputFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeInput>>();
    }
    case VisualShader::VisualShaderNode::kOutputFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeOutput>>();
    }
    case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatConstant>>();
    }
    case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntConstant>>();
    }
    case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntConstant>>();
    }
    case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeBooleanConstant>>();
    }
    case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeColorConstant>>();
    }
    case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec2Constant>>();
    }
    case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec3Constant>>();
    }
    case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec4Constant>>();
    }
    case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatOp>>();
    }
    case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntOp>>();
    }
    case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntOp>>();
    }
    case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorOp>>();
    }
    case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatFunc>>();
    }
    case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntFunc>>();
    }
    case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntFunc>>();
    }
    case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorFunc>>();
    }
    case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeValueNoise>>();
    }
    case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodePerlinNoise>>();
    }
    case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVoronoiNoise>>();
    }
    case VisualShader::VisualShaderNode::kDotProductFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeDotProduct>>();
    }
    case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorLen>>();
    }
    case VisualShader::VisualShaderNode::kClampFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeClamp>>();
    }
    case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorDistance>>();
    }
    case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorCompose>>();
    }
    case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorCompose>>();
    }
    case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorCompose>>();
    }
    case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorDecompose>>();
    }
    case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorDecompose>>();
    }
    case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorDecompose>>();
    }
    case VisualShader::VisualShaderNode::kIfNodeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIf>>();
    }
    case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeSwitch>>();
    }
    case VisualShader::VisualShaderNode::kIsFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeIs>>();
    }
    case VisualShader::VisualShaderNode::kCompareFieldNumber: {
      return std::make_shared<VisualShaderProtoNode<VisualShaderNodeCompare>>();
    }
    default:
      WARN_PRINT("Unsupported node type: " + std::to_string(oneof_value_field_number));
      break;
  }
  return nullptr;
}
}  // namespace shadergen_utils

#endif  // SHADERGEN_PROTO_NODE_FACTORY_HPP
