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

#include <gtest/gtest.h>

#include <chrono>  // For timing

#include "generator/visual_shader_generator.hpp"
#include "generator/visual_shader_node_generators.hpp"
#include "generator/vs_node_noise_generators.hpp"
#include "gui/model/schema/visual_shader_nodes.pb.h"
#include "gui/controller/vs_proto_node.hpp"

TEST(VisualShaderGeneratorTest, TestGenerateShader) {

  int output_node_id{0}, time_node_id{1}, sin_node_id{2}, div_node_id{3}, uv_node_id{4}, value_noise_node_id{5}, sub_node_id{6}, round_node_id{7};

  std::unordered_map<int, std::shared_ptr<IVisualShaderProtoNode>> proto_nodes;
  proto_nodes[output_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeOutput>>();
  proto_nodes[time_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeInput>>();
  proto_nodes[sin_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatFunc>>();
  proto_nodes[div_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatOp>>();
  proto_nodes[uv_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeInput>>();
  proto_nodes[value_noise_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeValueNoise>>();
  proto_nodes[sub_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatOp>>();
  proto_nodes[round_node_id] = std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatFunc>>();

  std::unordered_map<int, std::shared_ptr<VisualShaderNodeGenerator>> generators;
  generators[output_node_id] = std::make_shared<VisualShaderNodeGeneratorOutput>(); // Create an output node
  generators[time_node_id] = std::make_shared<VisualShaderNodeGeneratorInput>(VisualShaderNodeInputType::INPUT_TYPE_TIME); // Create a time input
  generators[sin_node_id] = std::make_shared<VisualShaderNodeGeneratorFloatFunc>(VisualShaderNodeFloatFunc::FUNC_SIN); // Create a sin func
  generators[div_node_id] = std::make_shared<VisualShaderNodeGeneratorFloatOp>(VisualShaderNodeFloatOp::OP_DIV); // Create a divide operator
  generators[uv_node_id] = std::make_shared<VisualShaderNodeGeneratorInput>(VisualShaderNodeInputType::INPUT_TYPE_UV); // Create a UV input
  generators[value_noise_node_id] = std::make_shared<VisualShaderNodeGeneratorValueNoise>(100.0f); // Create a Value Noise node
  generators[sub_node_id] = std::make_shared<VisualShaderNodeGeneratorFloatOp>(VisualShaderNodeFloatOp::OP_SUB); // Create a subtract operator
  generators[round_node_id] = std::make_shared<VisualShaderNodeGeneratorFloatFunc>(VisualShaderNodeFloatFunc::FUNC_ROUND); // Create a float func

  shadergen_visual_shader_generator::ConnectionKey time_output_key;
  time_output_key.f_key.node = time_node_id;
  time_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey sin_input_key;
  sin_input_key.f_key.node = sin_node_id;
  sin_input_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey div_input_key;
  div_input_key.f_key.node = div_node_id;
  div_input_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey value_noise_input_key;
  value_noise_input_key.f_key.node = value_noise_node_id;
  value_noise_input_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey sub_input_key1;
  sub_input_key1.f_key.node = sub_node_id;
  sub_input_key1.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey sub_input_key2;
  sub_input_key2.f_key.node = sub_node_id;
  sub_input_key2.f_key.port = 1;

  shadergen_visual_shader_generator::ConnectionKey round_input_key;
  round_input_key.f_key.node = round_node_id;
  round_input_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey sin_output_key;
  sin_output_key.f_key.node = sin_node_id;
  sin_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey div_output_key;    
  div_output_key.f_key.node = div_node_id;
  div_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey uv_output_key;
  uv_output_key.f_key.node = uv_node_id;
  uv_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey value_noise_output_key;
  value_noise_output_key.f_key.node = value_noise_node_id;
  value_noise_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey sub_output_key;
  sub_output_key.f_key.node = sub_node_id;
  sub_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey round_output_key;
  round_output_key.f_key.node = round_node_id;
  round_output_key.f_key.port = 0;

  shadergen_visual_shader_generator::ConnectionKey output_key;
  output_key.f_key.node = output_node_id;
  output_key.f_key.port = 0;

  // Connect `output port 0` of time input to `input port 0` of sin func.
  shadergen_visual_shader_generator::Connection c1;
  c1.from = time_output_key;
  c1.to = sin_input_key;  

  // Connect `output port 0` of sin func to `input port 0` of divide operator.
  shadergen_visual_shader_generator::Connection c2;
  c2.from = sin_output_key;
  c2.to = div_input_key;

  // Connect `output port 0` of divide operator to `input port 1` of subtract
  shadergen_visual_shader_generator::Connection c3;
  c3.from = div_output_key;
  c3.to = sub_input_key2;

  // Connect `output port 0` of UV input to `input port 0` of value noise node.
  shadergen_visual_shader_generator::Connection c4;
  c4.from = uv_output_key;
  c4.to = value_noise_input_key;

  // Connect `output port 0` of value noise node to `input port 0` of subtract
  shadergen_visual_shader_generator::Connection c5;
  c5.from = value_noise_output_key;
  c5.to = sub_input_key1;

  // Connect `output port 0` of subtract operator to `input port 0` of float
  shadergen_visual_shader_generator::Connection c6;
  c6.from = sub_output_key;
  c6.to = round_input_key;

  // Connect `output port 0` of float func to `input port 0` of output node.
  shadergen_visual_shader_generator::Connection c7;
  c7.from = round_output_key;
  c7.to = output_key;

  std::map<shadergen_visual_shader_generator::ConnectionKey, std::shared_ptr<shadergen_visual_shader_generator::Connection>> output_connections;
  output_connections[time_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c1);
  output_connections[sin_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c2);
  output_connections[div_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c3);
  output_connections[uv_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c4);
  output_connections[value_noise_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c5);
  output_connections[sub_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c6);
  output_connections[round_output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c7);

  std::map<shadergen_visual_shader_generator::ConnectionKey, std::shared_ptr<shadergen_visual_shader_generator::Connection>> input_connections;
  input_connections[sin_input_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c1);
  input_connections[div_input_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c2);
  input_connections[sub_input_key2] = std::make_shared<shadergen_visual_shader_generator::Connection>(c3);
  input_connections[value_noise_input_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c4);
  input_connections[sub_input_key1] = std::make_shared<shadergen_visual_shader_generator::Connection>(c5);
  input_connections[round_input_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c6);
  input_connections[output_key] = std::make_shared<shadergen_visual_shader_generator::Connection>(c7);

  auto start_time {std::chrono::high_resolution_clock::now()};

  std::string generated_code;

  // Generate the shader.
  bool status{shadergen_visual_shader_generator::generate_shader(proto_nodes,
                                                                 generators,
                                                                 std::make_pair(input_connections, output_connections),
                                                                 generated_code)};
  ASSERT_EQ(status, true);

  auto end_time {std::chrono::high_resolution_clock::now()};
    
  // Calculate the duration in microseconds
  auto duration {std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()};
  
  ASSERT_LE(duration, 500);

  // Get the shader.
  std::string expected_code{
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
  generated_code = shadergen_visual_shader_generator::generate_preview_shader(proto_nodes,
                                                                 generators,
                                                                 std::make_pair(input_connections, output_connections),
                                                                 time_node_id, 0);
  expected_code = 
    "in vec2 FragCoord;\n"
    "uniform float uTime;\n"
    "out vec4 FragColor;\n"
    "\nvoid main() {\n"
    "// Input:1\n"
    "\tfloat var_from_n1_p0 = uTime;\n\n\n"
    "\tFragColor = vec4(vec3(var_from_n1_p0), 1.0);\n"
    "}\n\n";

  ASSERT_EQ(generated_code, expected_code);

  generated_code = shadergen_visual_shader_generator::generate_preview_shader(proto_nodes,
                                                                 generators,
                                                                 std::make_pair(input_connections, output_connections),
                                                                 sin_node_id, 0);
  expected_code = 
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

  generated_code = shadergen_visual_shader_generator::generate_preview_shader(proto_nodes,
                                                                 generators,
                                                                 std::make_pair(input_connections, output_connections),
                                                                 value_noise_node_id, 0);
  expected_code = 
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
