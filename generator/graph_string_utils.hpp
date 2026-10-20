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

#ifndef SHADERGEN_GRAPH_STRING_UTILS_HPP
#define SHADERGEN_GRAPH_STRING_UTILS_HPP

#include <sstream>
#include <string>
#include <vector>

#include "error_macros.hpp"

/**
 * @brief String helpers for the encoded graph format used by the generator and the AI agent.
 *
 * @note This header is intentionally free of any GUI/Qt dependency so that the generator can be
 *       built standalone.
 */
namespace shadergen_graph_string_utils {

inline static std::vector<std::string> split_string(const std::string& str, const char& delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) tokens.push_back(token);
    return tokens;
}

inline static std::string join_string(const std::vector<std::string>& tokens, const char& delimiter) {
    CHECK_CONDITION_TRUE_NON_VOID(tokens.empty(), "", "Tokens is empty.");
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(tokens.size() == 1, tokens.at(0));

    std::ostringstream joined;
    joined << tokens.at(0);
    for (std::size_t i = 1; i < tokens.size(); ++i) joined << delimiter << tokens.at(i);
    return joined.str();
}

inline static std::string get_entity_type(const std::vector<std::string>& node_tokens) {
    return node_tokens.at(0);
}

inline static std::string get_node_entity_id(const std::vector<std::string>& node_tokens) {
    return node_tokens.at(1);
}

inline static std::string get_node_entity_oneof_value_field_number(const std::vector<std::string>& node_tokens) {
    return node_tokens.at(2);
}

inline static std::vector<std::string> get_node_entity_parameters(const std::vector<std::string>& node_tokens) {
    std::vector<std::string> parameters;
    parameters.insert(parameters.end(), node_tokens.begin() + 3, node_tokens.end());
    return parameters;
}

inline static std::string get_node_entity_parameter_field_number(const std::vector<std::string>& parameter_tokens) {
    return parameter_tokens.at(0);
}

inline static std::string get_node_entity_parameter_value(const std::vector<std::string>& parameter_tokens) {
    return parameter_tokens.at(1);
}

inline static std::string get_connection_entity_id(const std::vector<std::string>& connection_tokens) {
    return connection_tokens.at(1);
}

inline static std::string get_connection_entity_from_node_id(const std::vector<std::string>& connection_tokens) {
    return connection_tokens.at(2);
}

inline static std::string get_connection_entity_from_port_index(const std::vector<std::string>& connection_tokens) {
    return connection_tokens.at(3);
}

inline static std::string get_connection_entity_to_node_id(const std::vector<std::string>& connection_tokens) {
    return connection_tokens.at(4);
}

inline static std::string get_connection_entity_to_port_index(const std::vector<std::string>& connection_tokens) {
    return connection_tokens.at(5);
}

}  // namespace shadergen_graph_string_utils

#endif  // SHADERGEN_GRAPH_STRING_UTILS_HPP
