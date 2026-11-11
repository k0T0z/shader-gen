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

#ifndef SHADER_GEN_SHADER_RENDERER_HPP
#define SHADER_GEN_SHADER_RENDERER_HPP

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace shadergen_renderer {

/**
 * @brief The base every renderer depends on: the shader-interface contract and the lifecycle.
 *
 * The generator emits a fragment `main()` that reads @c FragCoord, optionally reads
 * @c uniform float @c uTime and writes @c FragColor. A renderer therefore has to supply the
 * same @c \#version line, the same full-screen quad and - for a graph that does not declare
 * them itself - the same interface declarations.
 */
class ShaderRenderer {
 public:
  virtual ~ShaderRenderer() = default;

  ShaderRenderer(const ShaderRenderer&) = delete;
  ShaderRenderer& operator=(const ShaderRenderer&) = delete;

  /// The version directive prepended to every generated shader.
  static constexpr std::string_view kGlslVersion{"330 core"};
  /// The fragment input the generator reads the UV coordinate from.
  static constexpr std::string_view kFragCoordName{"FragCoord"};
  /// The fragment output the generator writes the color to.
  static constexpr std::string_view kFragColorName{"FragColor"};
  /// The uniform the generator reads the time from when a Time input node is present.
  static constexpr std::string_view kTimeUniformName{"uTime"};
  /// The vertex attribute the quad's position is bound to.
  static constexpr std::string_view kPositionAttributeName{"aPos"};
  /// The vertex attribute the quad's texture coordinate is bound to.
  static constexpr std::string_view kFragCoordAttributeName{"aFragCoord"};
  /// The size the offscreen renderers sample at, and the size a target is compared at.
  static constexpr int kDefaultResolution{256};
  /// The number of vertices of the full-screen quad.
  static constexpr std::size_t kQuadVertexCount{4};
  /// The number of floats of the full-screen quad (four interleaved (x, y, u, v) vertices).
  static constexpr std::size_t kQuadFloatCount{kQuadVertexCount * 4};

  /// The full-screen quad as interleaved (x, y, u, v), drawn as a triangle strip.
  static const std::array<float, kQuadFloatCount>& quad_vertices() noexcept;

  /// The vertex shader that forwards the quad's texture coordinates to @c FragCoord.
  static std::string vertex_shader_source() noexcept;

  /// The generated @c code, prefixed with the version directive the renderer expects.
  static std::string fragment_shader_source(const std::string& code) noexcept;

  /// The fragment shader drawn while there is no code yet.
  static std::string default_fragment_shader_source() noexcept;

  /// The fragment shader drawn when the generated code fails to compile.
  static std::string error_fragment_shader_source() noexcept;

  /// Makes @c code the shader this renderer draws.
  virtual void set_code(const std::string& code) = 0;

 protected:
  ShaderRenderer() = default;
};

}  // namespace shadergen_renderer

#endif  // SHADER_GEN_SHADER_RENDERER_HPP
