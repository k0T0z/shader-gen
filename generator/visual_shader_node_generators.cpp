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

#include "gui/controller/graph_node.hpp"

std::string VisualShaderNodeGeneratorInput::generate_global([[maybe_unused]] const int& id) const {
  std::string code;

  GraphNode<VisualShaderNodeInput> input_node;

  std::string input_type_caption{input_node.get_input_type_caption(input_type)};

  switch (input_type) {
    case VisualShaderNodeInputType::INPUT_TYPE_UV: {
        code += "in vec2 " + input_type_caption + ";" + std::string("\n");
    } break;
    case VisualShaderNodeInputType::INPUT_TYPE_TIME: {
        code += "uniform float " + input_type_caption + ";" + std::string("\n");
    } break;
    default:
        break;
  }

  return code;
}

std::string VisualShaderNodeGeneratorInput::generate_code([[maybe_unused]] const int& id,
                                                          [[maybe_unused]] const std::vector<std::string>& input_vars,
                                                          [[maybe_unused]] const std::vector<std::string>& output_vars) const {
    std::string code;
    GraphNode<VisualShaderNodeInput> input_node;

    std::string input_type_caption{input_node.get_input_type_caption(input_type)};
    
    switch (input_type) {
        case VisualShaderNodeInputType::INPUT_TYPE_UV: {
            return std::string("\t") + output_vars.at(0) + " = " + input_type_caption + ";" + std::string("\n");
        } break;
        case VisualShaderNodeInputType::INPUT_TYPE_TIME: {
            return std::string("\t") + output_vars.at(0) + " = " + input_type_caption + ";" + std::string("\n");
        } break;
        default:
            code = std::string("\t") + output_vars.at(0) + " = 0.0;" + std::string("\n");
            break;
    }

  return code;
}
