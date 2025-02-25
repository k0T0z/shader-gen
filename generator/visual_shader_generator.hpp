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

#ifndef ENIGMA_VISUAL_SHADER_GENERATOR_HPP
#define ENIGMA_VISUAL_SHADER_GENERATOR_HPP

#include <string>
#include <vector>
#include "gui/model/proto_model.hpp"

#include "generator/visual_shader_node_generators.hpp"
#include <map>
#include <unordered_map>
#include "generator/utils/utils.hpp"
#include "gui/controller/vs_proto_node.hpp"

namespace shadergen_visual_shader_generator {
/**
  * @brief This union is a 64-bit integer that can be treated as
  *        two 32-bit integers. The first 32 bits (first fragment) 
  *        are for the node id and the second 32 bits (second fragment) 
  *        are for the port id.
  */
union ConnectionKey {
  struct FragmentedKey {
    uint64_t node : 32;
    uint64_t port : 32;
  } f_key;
  uint64_t key;
  ConnectionKey() : key(0) {}
  bool operator<(const ConnectionKey& key) const { return this->key < key.key; }
};

struct Connection {
  ConnectionKey from;
  ConnectionKey to;
};

std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>> to_proto_nodes(const ProtoModel* nodes) noexcept;

std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>> to_generators(const ProtoModel* nodes) noexcept;

std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>> to_input_output_connections_by_key(const ProtoModel* connections) noexcept;

/**
 * @brief This function generates the shader code from the visual shader graph.
 * 
 * @note @c compile_graph is another good name for this function.
 * 
 * @param proto_nodes 
 * @param generators 
 * @param input_output_connections_by_key 
 * @param code_buffer 
 * @return true 
 * @return false 
 */
bool generate_shader(
  const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes, 
  const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators, 
  const std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>>& input_output_connections_by_key, 
  std::string& code_buffer) noexcept;

std::string generate_preview_shader(const std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>>& proto_nodes, 
  const std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>>& generators, 
  const std::pair<std::map<ConnectionKey, std::shared_ptr<Connection>>, std::map<ConnectionKey, std::shared_ptr<Connection>>>& input_output_connections_by_key, 
  const int& node_id, const int& port) noexcept;
}  // namespace shadergen_visual_shader_generator

#endif  // ENIGMA_VISUAL_SHADER_GENERATOR_HPP
