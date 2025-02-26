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

#ifndef AI_AGENT_UTILS_HPP
#define AI_AGENT_UTILS_HPP

#include "gui/model/schema/visual_shader.pb.h"

#include <vector>

#include "error_macros.hpp"

namespace ai_agent_utils {
inline static std::vector<int> get_node_type_population() {
    std::vector<int> node_type_population;

    node_type_population.emplace_back(VisualShader::VisualShaderNode::kInputFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kFloatConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIntConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kUIntConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kColorConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kFloatOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIntOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kUIntOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kFloatFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIntFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kUIntFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kValueNoiseFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kDotProductFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorLenFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kClampFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorDistanceFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector2DComposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector3DComposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector4DComposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIfFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIsFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kCompareFieldNumber);

    return node_type_population;
}
}  // namespace ai_agent_utils

#endif  // AI_AGENT_UTILS_HPP
