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

#include "generator/renderer/shader_renderer.hpp"

namespace shadergen_renderer {

const std::array<float, ShaderRenderer::kQuadFloatCount>& ShaderRenderer::quad_vertices() noexcept {
  static const std::array<float, kQuadFloatCount> vertices{
      -1.0f, 1.0f,  0.0f, 1.0f,  // Vertex 1: Position (x, y), FragCoord (u, v)
      -1.0f, -1.0f, 0.0f, 0.0f,  // Vertex 2: Position (x, y), FragCoord (u, v)
      1.0f,  1.0f,  1.0f, 1.0f,  // Vertex 3: Position (x, y), FragCoord (u, v)
      1.0f,  -1.0f, 1.0f, 0.0f   // Vertex 4: Position (x, y), FragCoord (u, v)
  };
  return vertices;
}

std::string ShaderRenderer::vertex_shader_source() noexcept {
  std::string source;

  source += "#version " + std::string(kGlslVersion) + "\n\n";
  source += "in vec2 " + std::string(kPositionAttributeName) + ";\n";
  source += "in vec2 " + std::string(kFragCoordAttributeName) + ";\n";
  source += "\n";
  source += "out vec2 " + std::string(kFragCoordName) + ";\n";
  source += "\n";
  source += "void main() {\n";
  source += std::string("\t") + "gl_Position = vec4(" + std::string(kPositionAttributeName) + ", 0.0, 1.0);\n";
  source += std::string("\t") + std::string(kFragCoordName) + " = " + std::string(kFragCoordAttributeName) + ";\n";
  source += "}\n";

  return source;
}

std::string ShaderRenderer::fragment_shader_source(const std::string& code) noexcept {
  std::string source;

  source += "#version " + std::string(kGlslVersion) + "\n\n";
  source += code;

  return source;
}

std::string ShaderRenderer::default_fragment_shader_source() noexcept {
  std::string source;

  source += "#version " + std::string(kGlslVersion) + "\n\n";
  source += "out vec4 " + std::string(kFragColorName) + ";\n";
  source += "in vec2 " + std::string(kFragCoordName) + ";\n\n";
  source += "uniform float " + std::string(kTimeUniformName) + ";\n\n";
  source += "void main() {\n";
  source += std::string("\t") + std::string(kFragColorName) + " = vec4(0.0, 0.0, 0.0, 1.0);\n";
  source += "}\n";

  return source;
}

std::string ShaderRenderer::error_fragment_shader_source() noexcept {
  std::string source;

  source += "#version " + std::string(kGlslVersion) + "\n\n";
  source += "in vec2 " + std::string(kFragCoordName) + ";\n";
  source += "out vec4 " + std::string(kFragColorName) + ";\n\n";
  source += "void main() {\n";
  source += std::string("\t") + std::string(kFragColorName) + " = vec4(1.0, 0.0, 0.0, 1.0);\n";
  source += "}\n";

  return source;
}

}  // namespace shadergen_renderer
