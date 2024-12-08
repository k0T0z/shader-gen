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

#include "generator/visual_shader_generator.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "generator/visual_shader_node_generators.hpp"

namespace shadergen_visual_shader_generator {
    static inline std::vector<VisualShaderNodeGenerator> to_generators(const std::vector<VisualShader::VisualShaderNode>& nodes) noexcept {
        std::vector<VisualShaderNodeGenerator> generators;
        
        return generators;
    }

    // Define generate_shader_for_each_node to use it in generate_shader
    static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                     std::string& func_code,
                                                     const int& node_id, std::unordered_set<int>& processed,
                                                     std::unordered_set<std::string>& global_processed) noexcept;

    bool generate_shader(std::string& code,
                         std::vector<VisualShader::VisualShaderNode> nodes, 
                         std::vector<VisualShader::VisualShaderConnection> connections) noexcept {

        std::vector<VisualShaderNodeGenerator> generators = to_generators(nodes);
        

        return true;
    }

    std::string generate_preview_shader(const int& node_id, const int& port) noexcept {

        return "";
    }

    static inline bool generate_shader_for_each_node(std::string& global_code, std::string& global_code_per_node,
                                                     std::string& func_code,
                                                     const int& node_id, std::unordered_set<int>& processed,
                                                     std::unordered_set<std::string>& global_processed) noexcept {
        

        return true;
    }
} // namespace shadergen_visual_shader_generator
