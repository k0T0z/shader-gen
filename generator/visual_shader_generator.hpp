/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2026 Seif Kandil (k0T0z)                                       */
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

#ifndef ENIGMA_VISUAL_SHADER_GENERATOR_HPP
#define ENIGMA_VISUAL_SHADER_GENERATOR_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <utility>

#include "generator/visual_shader_node_generators.hpp"
#include "generator/visual_shader_node_port_type_generator.hpp"
#include "generator/utils/utils.hpp"

extern const std::string license_notices;

namespace shadergen_visual_shader_generator {

using ConnectionKey = std::pair<int, int>;

struct Connection {
  ConnectionKey from;
  ConnectionKey to;
};

/**
 * @brief Generates the GLSL code for the given raw graph string.
 *
 * The graph is a comma separated list of entities where every entity is semicolon separated:
 *   - Nodes:       "0;<node_id>;<oneof_value_field_number>;<field_number>=<value>;..."
 *   - Connections: "1;<connection_id>;<from_node_id>;<from_port_index>;<to_node_id>;<to_port_index>"
 */
bool generate_shader(const std::string& graph, std::string& code_buffer) noexcept;

std::string generate_preview_shader(const std::string& graph, const int& node_id, const int& port) noexcept;
}  // namespace shadergen_visual_shader_generator

#endif  // ENIGMA_VISUAL_SHADER_GENERATOR_HPP
