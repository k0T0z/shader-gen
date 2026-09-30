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

#include <gtest/gtest.h>

#include <chrono>  // For timing

#include "generator/visual_shader_generator.hpp"
#include "generator/visual_shader_node_generators.hpp"
#include "generator/vs_node_noise_generators.hpp"
#include "gui/model/schema/visual_shader_nodes.pb.h"
#include "gui/controller/vs_proto_node.hpp"

TEST(VisualShaderGeneratorTest, TestGenerateShader) {

  int output_node_id{0}, time_node_id{1}, sin_node_id{2}, div_node_id{3}, uv_node_id{4}, value_noise_node_id{5}, sub_node_id{6}, round_node_id{7};

  RawVisualShaderGraph graph;
  graph.headers.resize(8);
  graph.headers[0] = "0;0;3";                                    // Output (oneof=3): id=0, no params
  graph.headers[1] = "0;1;2;1=2";                                // Input (oneof=2): id=1, type=2 (TIME)
  graph.headers[2] = "0;2;16;1=1";                               // FloatFunc (oneof=16): id=2, func_type=1 (SIN)
  graph.headers[3] = "0;3;12;1=4";                               // FloatOp (oneof=12): id=3, op_type=4 (DIV)
  graph.headers[4] = "0;4;2;1=1";                                // Input (oneof=2): id=4, type=1 (UV)
  graph.headers[5] = "0;5;20;1=100.000000";                      // ValueNoise (oneof=20): id=5, scale=100.0
  graph.headers[6] = "0;6;12;1=2";                               // FloatOp (oneof=12): id=6, op_type=2 (SUB)
  graph.headers[7] = "0;7;16;1=16";                              // FloatFunc (oneof=16): id=7, func_type=16 (ROUND)

  const std::size_t N{8};
  graph.adj_matrix.assign(N, std::vector<std::string>(N, ""));

  // c1: time(1) output 0 -> sin(2) input 0  =>  [1][2] = "0,0"
  graph.adj_matrix[1][2] = "0,0";
  // c2: sin(2) output 0 -> div(3) input 0  =>  [2][3] = "0,0"
  graph.adj_matrix[2][3] = "0,0";
  // c3: div(3) output 0 -> sub(6) input 1  =>  [3][6] = "0,1"
  graph.adj_matrix[3][6] = "0,1";
  // c4: uv(4) output 0 -> value_noise(5) input 0  =>  [4][5] = "0,0"
  graph.adj_matrix[4][5] = "0,0";
  // c5: value_noise(5) output 0 -> sub(6) input 0  =>  [5][6] = "0,0"
  graph.adj_matrix[5][6] = "0,0";
  // c6: sub(6) output 0 -> round(7) input 0  =>  [6][7] = "0,0"
  graph.adj_matrix[6][7] = "0,0";
  // c7: round(7) output 0 -> output(0) input 0  =>  [7][0] = "0,0"
  graph.adj_matrix[7][0] = "0,0";

  auto start_time {std::chrono::high_resolution_clock::now()};

  std::string generated_code;

  // Generate the shader.
  bool status{shadergen_visual_shader_generator::generate_shader(graph, generated_code)};
  ASSERT_EQ(status, true);

  auto end_time {std::chrono::high_resolution_clock::now()};
    
  // Calculate the duration in microseconds
  auto duration {std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()};
  
  // ASSERT_LE(duration, 5000);

  // Get the shader.
  std::string expected_code{license_notices +

    "in vec2 FragCoord;\n"

    "uniform float uTime;\n"

    "float noise_random_value(vec2 uv) {\n"
    "\treturn fract(sin(dot(uv, vec2(12.9898, 78.233)))*43758.5453);\n"
    "}\n\n"

    "float noise_interpolate(float a, float b, float t) {\n"
    "\treturn (1.0-t)*a + (t*b);\n"
    "}\n\n"

    "float value_noise(vec2 uv) {\n"
    "\tvec2 i = floor(uv);\n"
    "\tvec2 f = fract(uv);\n"
    "\tf = f * f * (3.0 - 2.0 * f);\n"
    "\t\n"
    "\tuv = abs(fract(uv) - 0.5);\n"
    "\tvec2 c0 = i + vec2(0.0, 0.0);\n"
    "\tvec2 c1 = i + vec2(1.0, 0.0);\n"
    "\tvec2 c2 = i + vec2(0.0, 1.0);\n"
    "\tvec2 c3 = i + vec2(1.0, 1.0);\n"
    "\tfloat r0 = noise_random_value(c0);\n"
    "\tfloat r1 = noise_random_value(c1);\n"
    "\tfloat r2 = noise_random_value(c2);\n"
    "\tfloat r3 = noise_random_value(c3);\n"
    "\t\n"
    "\tfloat bottom_of_grid = noise_interpolate(r0, r1, f.x);\n"
    "\tfloat top_of_grid = noise_interpolate(r2, r3, f.x);\n"
    "\tfloat t = noise_interpolate(bottom_of_grid, top_of_grid, f.y);\n"
    "\treturn t;\n"
    "}\n\n"

    "void generate_value_noise_float(vec2 uv, float scale, out float "
    "out_buffer) {\n"
    "\tfloat t = 0.0;\n"
    "\t\n"
    "\tfloat freq = pow(2.0, float(0));\n"
    "\tfloat amp = pow(0.5, float(3-0));\n"
    "\tt += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;\n"
    "\t\n"
    "\tfreq = pow(2.0, float(1));\n"
    "\tamp = pow(0.5, float(3-1));\n"
    "\tt += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;\n"
    "\t\n"
    "\tfreq = pow(2.0, float(2));\n"
    "\tamp = pow(0.5, float(3-2));\n"
    "\tt += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;\n"
    "\t\n"
    "\tout_buffer = t;\n"
    "}\n\n"

    "out vec4 FragColor;\n"

    "\nvoid main() {\n"
    "// Input:4\n"
    "\tvec2 var_from_n4_p0 = FragCoord;\n\n\n"
    "// ValueNoise:5\n"
    "\t// Value Noise\n"
    "\tfloat out_buffer_n5 = 0.0;\n"
    "\tgenerate_value_noise_float(var_from_n4_p0, 100.000000, out_buffer_n5);\n"
    "\tvec4 var_from_n5_p0 = vec4(out_buffer_n5, out_buffer_n5, out_buffer_n5, 1.0);\n"
    "\t\n\n\n"
    "// Input:1\n"
    "\tfloat var_from_n1_p0 = uTime;\n\n\n"
    "// FloatFunc:2\n"
    "\tfloat var_from_n2_p0 = sin(var_from_n1_p0);\n\n\n"
    "// FloatOp:3\n"
    "\tfloat var_to_n3_p1 = 0.00000;\n"
    "\tfloat var_from_n3_p0 = var_from_n2_p0 / var_to_n3_p1;\n\n\n"
    "// FloatOp:6\n"
    "\tfloat var_from_n6_p0 = var_from_n5_p0.x - var_from_n3_p0;\n\n\n"
    "// FloatFunc:7\n"
    "\tfloat var_from_n7_p0 = round(var_from_n6_p0);\n\n\n"
    "// Output:0\n"
    "\tFragColor = vec4(var_from_n7_p0);\n\n\n"
    "}\n\n"};
  ASSERT_EQ(generated_code, expected_code);

  // Send the time node.
  generated_code = shadergen_visual_shader_generator::generate_preview_shader(graph, time_node_id, 0);
  expected_code = license_notices +
    "in vec2 FragCoord;\n"
    "uniform float uTime;\n"
    "out vec4 FragColor;\n"
    "\nvoid main() {\n"
    "// Input:1\n"
    "\tfloat var_from_n1_p0 = uTime;\n\n\n"
    "\tFragColor = vec4(vec3(var_from_n1_p0), 1.0);\n"
    "}\n\n";

  ASSERT_EQ(generated_code, expected_code);

  generated_code = shadergen_visual_shader_generator::generate_preview_shader(graph, sin_node_id, 0);
  expected_code = license_notices +
    "in vec2 FragCoord;\n"
    "uniform float uTime;\n"
    "out vec4 FragColor;\n"
    "\nvoid main() {\n"
    "// Input:1\n"
    "\tfloat var_from_n1_p0 = uTime;\n\n\n"
    "// FloatFunc:2\n"
    "\tfloat var_from_n2_p0 = sin(var_from_n1_p0);\n\n\n"
    "\tFragColor = vec4(vec3(var_from_n2_p0), 1.0);\n"
    "}\n\n";

  ASSERT_EQ(generated_code, expected_code);

  generated_code = shadergen_visual_shader_generator::generate_preview_shader(graph, value_noise_node_id, 0);
  expected_code = license_notices +
    "in vec2 FragCoord;\n"
    "uniform float uTime;\n"

    "float noise_random_value(vec2 uv) {\n"
    "\treturn fract(sin(dot(uv, vec2(12.9898, 78.233)))*43758.5453);\n"
    "}\n\n"

    "float noise_interpolate(float a, float b, float t) {\n"
    "\treturn (1.0-t)*a + (t*b);\n"
    "}\n\n"

    "float value_noise(vec2 uv) {\n"
    "\tvec2 i = floor(uv);\n"
    "\tvec2 f = fract(uv);\n"
    "\tf = f * f * (3.0 - 2.0 * f);\n"
    "\t\n"
    "\tuv = abs(fract(uv) - 0.5);\n"
    "\tvec2 c0 = i + vec2(0.0, 0.0);\n"
    "\tvec2 c1 = i + vec2(1.0, 0.0);\n"
    "\tvec2 c2 = i + vec2(0.0, 1.0);\n"
    "\tvec2 c3 = i + vec2(1.0, 1.0);\n"
    "\tfloat r0 = noise_random_value(c0);\n"
    "\tfloat r1 = noise_random_value(c1);\n"
    "\tfloat r2 = noise_random_value(c2);\n"
    "\tfloat r3 = noise_random_value(c3);\n"
    "\t\n"
    "\tfloat bottom_of_grid = noise_interpolate(r0, r1, f.x);\n"
    "\tfloat top_of_grid = noise_interpolate(r2, r3, f.x);\n"
    "\tfloat t = noise_interpolate(bottom_of_grid, top_of_grid, f.y);\n"
    "\treturn t;\n"
    "}\n\n"

    "void generate_value_noise_float(vec2 uv, float scale, out float "
    "out_buffer) {\n"
    "\tfloat t = 0.0;\n"
    "\t\n"
    "\tfloat freq = pow(2.0, float(0));\n"
    "\tfloat amp = pow(0.5, float(3-0));\n"
    "\tt += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;\n"
    "\t\n"
    "\tfreq = pow(2.0, float(1));\n"
    "\tamp = pow(0.5, float(3-1));\n"
    "\tt += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;\n"
    "\t\n"
    "\tfreq = pow(2.0, float(2));\n"
    "\tamp = pow(0.5, float(3-2));\n"
    "\tt += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;\n"
    "\t\n"
    "\tout_buffer = t;\n"
    "}\n\n"

    "out vec4 FragColor;\n"

    "\nvoid main() {\n"
    "// Input:4\n"
    "\tvec2 var_from_n4_p0 = FragCoord;\n\n\n"
    "// ValueNoise:5\n"
    "\t// Value Noise\n"
    "\tfloat out_buffer_n5 = 0.0;\n"
    "\tgenerate_value_noise_float(var_from_n4_p0, 100.000000, out_buffer_n5);\n"
    "\tvec4 var_from_n5_p0 = vec4(out_buffer_n5, out_buffer_n5, out_buffer_n5, 1.0);\n"
    "\t\n\n\n"
    "\tFragColor = vec4(var_from_n5_p0.xyz, 1.0);\n"
    "}\n\n";

  ASSERT_EQ(generated_code, expected_code);
}
