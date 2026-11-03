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

#include "generator/vs_node_noise_generators.hpp"
#include "generator/visual_shader_node_generators.hpp"

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorValueNoiseGenerateCode) {
  VisualShaderNodeGeneratorValueNoise value_noise_generator{100.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{value_noise_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Value Noise\n"
      "\tfloat out_buffer_n0 = 0.0;\n"
      "\tgenerate_value_noise_float(a, 100.000000, out_buffer_n0);\n"
      "\tb = vec4(out_buffer_n0, out_buffer_n0, out_buffer_n0, 1.0);\n"
      "\t\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorValueNoiseGenerateGlobal) {
  VisualShaderNodeGeneratorValueNoise value_noise_generator{100.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{value_noise_generator.generate_global(0)};
  std::string expected_code{
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
    "void generate_value_noise_float(vec2 uv, float scale, out float out_buffer) {\n"
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
    "}\n\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorPerlinNoiseGenerateCode) {
  VisualShaderNodeGeneratorPerlinNoise perlin_noise_generator{10.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{perlin_noise_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Perlin Noise\n"
      "\tfloat out_buffer_n0 = 0.0;\n"
      "\tgenerate_perlin_noise_float(a, 10.000000, out_buffer_n0);\n"
      "\tb = vec4(out_buffer_n0, out_buffer_n0, out_buffer_n0, 1.0);\n"
      "\t\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorPerlinNoiseGenerateGlobal) {
  VisualShaderNodeGeneratorPerlinNoise perlin_noise_generator{10.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{perlin_noise_generator.generate_global(0)};
  std::string expected_code{
    "vec2 perlin_noise_dir(vec2 p) {\n"
    "\tp = mod(p, 289.0);\n"
    "\t\n"
    "\tfloat x = mod((34.0 * p.x + 1.0) * p.x, 289.0) + p.y;\n"
    "\t\n"
    "\tx = mod((34.0 * x + 1.0) * x, 289.0);\n"
    "\t\n"
    "\tx = fract(x / 41.0) * 2.0 - 1.0;\n"
    "\t\n"
    "\treturn normalize(vec2(x - floor(x + 0.5), abs(x) - 0.5));\n"
    "}\n\n"
    "float perlin_noise(vec2 p) {\n"
    "\tvec2 ip = floor(p);\n"
    "\tvec2 fp = fract(p);\n"
    "\tfloat d00 = dot(perlin_noise_dir(ip), fp);\n"
    "\tfloat d01 = dot(perlin_noise_dir(ip + vec2(0, 1)), fp - vec2(0, 1));\n"
    "\tfloat d10 = dot(perlin_noise_dir(ip + vec2(1, 0)), fp - vec2(1, 0));\n"
    "\tfloat d11 = dot(perlin_noise_dir(ip + vec2(1, 1)), fp - vec2(1, 1));\n"
    "\tfp = fp * fp * fp * (fp * (fp * 6.0 - 15.0) + 10.0);\n"
    "\t\n"
    "\treturn mix(mix(d00, d01, fp.y), mix(d10, d11, fp.y), fp.x);\n"
    "}\n\n"
    "void generate_perlin_noise_float(vec2 uv, float scale, out float out_buffer) {\n"
    "\tout_buffer = perlin_noise(uv * scale) + 0.5;\n"
    "}\n\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorVoronoiNoiseGenerateCode) {
  VisualShaderNodeGeneratorVoronoiNoise voronoi_noise_generator{10.0f, 10.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{voronoi_noise_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Voronoi Noise\n"
      "\tfloat out_buffer_n0 = 0.0;\n"
      "\tfloat cells_n0 = 0.0; // TODO: How we can use this?\n"
      "\tgenerate_voronoi_noise_float(a, 10.000000, 10.000000, out_buffer_n0, cells_n0);\n"
      "\tb = vec4(out_buffer_n0, out_buffer_n0, out_buffer_n0, 1.0);\n"
      "\t\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorVoronoiNoiseGenerateGlobal) {
  VisualShaderNodeGeneratorVoronoiNoise voronoi_noise_generator{10.0f, 10.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{voronoi_noise_generator.generate_global(0)};
  std::string expected_code{
    "vec2 voronoi_noise_random_vector(vec2 uv, float offset) {\n"
    "\tmat2 m = mat2(15.27, 47.63, 99.41, 89.98);\n"
    "\t\n"
    "\tuv = fract(sin(m * uv) * 46839.32);\n"
    "\t\n"
    "\treturn vec2(sin(uv.y*+offset)*0.5+0.5, cos(uv.x*offset)*0.5+0.5);\n"
    "}\n\n"
    "void generate_voronoi_noise_float(vec2 uv, float angle_offset, float cell_density, out float out_buffer, out float cells) {\n"
    "\tvec2 g = floor(uv * cell_density);\n"
    "\tvec2 f = fract(uv * cell_density);\n"
    "\tfloat t = 8.0;\n"
    "\tvec3 res = vec3(8.0, 0.0, 0.0);\n"
    "\t\n"
    "\tfor(int y=-1; y<=1; y++) {\n"
    "\t\tfor(int x=-1; x<=1; x++) {\n"
    "\t\t\tvec2 lattice = vec2(x,y);\n"
    "\t\t\tvec2 offset = voronoi_noise_random_vector(lattice + g, angle_offset);\n"
    "\t\t\tfloat d = distance(lattice + offset, f);\n"
    "\t\t\tif(d < res.x) {\n"
    "\t\t\t\tres = vec3(d, offset.x, offset.y);\n"
    "\t\t\t\tout_buffer = res.x;\n"
    "\t\t\t\tcells = res.y;\n"
    "\t\t\t}\n"
    "\t\t}\n"
    "\t}\n"
    "}\n\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorCorrectGenerateCodeDefaults) {
  VisualShaderNodeGeneratorColorCorrect color_correct_generator{0.0f, 1.0f, 1.0f, 0.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{color_correct_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// ColorCorrect\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tout_buffer_n0 = ((out_buffer_n0 - 0.5) * 1.000000 + 0.000000) + 0.5;\n"
      "\tout_buffer_n0 = clamp(out_buffer_n0, 0.0, 1.0);\n"
      "\tout_buffer_n0 = pow(out_buffer_n0, vec4(1.000000));\n"
      "\tb = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorCorrectGenerateCodeDesaturate) {
  VisualShaderNodeGeneratorColorCorrect color_correct_generator{0.25f, 1.5f, 2.0f, 0.5f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{color_correct_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// ColorCorrect\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tout_buffer_n0 = ((out_buffer_n0 - 0.5) * 1.500000 + 0.250000) + 0.5;\n"
      "\tout_buffer_n0 = clamp(out_buffer_n0, 0.0, 1.0);\n"
      "\tvec3 out_buffer_hsv_n0 = rgb_to_hsv(out_buffer_n0.xyz);\n"
      "\tout_buffer_hsv_n0.y *= 0.500000;\n"
      "\tout_buffer_n0.xyz = hsv_to_rgb(out_buffer_hsv_n0);\n"
      "\tout_buffer_n0 = pow(out_buffer_n0, vec4(0.500000));\n"
      "\tb = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorCorrectGenerateGlobal) {
  VisualShaderNodeGeneratorColorCorrect color_correct_generator{0.0f, 1.0f, 1.0f, 0.0f};
  std::string code{color_correct_generator.generate_global(0)};
  std::string expected_code{
      "int color_correct_max_idx(vec3 c) {\n"
      "\tif (c.x > c.y) {\n"
      "\t\tif (c.x > c.z) return 0;\n"
      "\t\treturn 2;\n"
      "\t}\n"
      "\tif (c.y > c.z) return 1;\n"
      "\treturn 2;\n"
      "}\n\n"
      "int color_correct_min_idx(vec3 c) {\n"
      "\tif (c.x < c.y) {\n"
      "\t\tif (c.x < c.z) return 0;\n"
      "\t\treturn 2;\n"
      "\t}\n"
      "\tif (c.y < c.z) return 1;\n"
      "\treturn 2;\n"
      "}\n\n"
      "vec3 rgb_to_hsv(vec3 rgb) {\n"
      "\tvec3 temp = rgb;\n"
      "\tint maxi = color_correct_max_idx(temp);\n"
      "\tint mini = color_correct_min_idx(temp);\n"
      "\t\n"
      "\tfloat h = 0.0;\n"
      "\tfloat s = 0.0;\n"
      "\tfloat v = temp[maxi];\n"
      "\t\n"
      "\tif (temp[maxi] == 0.0) {\n"
      "\t\th = 0.0;\n"
      "\t\ts = 0.0;\n"
      "\t} else if (temp[maxi] == temp[mini]) {\n"
      "\t\th = 0.0;\n"
      "\t\ts = 0.0;\n"
      "\t} else {\n"
      "\t\ts = 255.0 * (1.0 - (temp[mini] / temp[maxi]));\n"
      "\t\th = 60.0 * (float(maxi * 2) + (temp[(maxi + 1) % 3] - temp[(maxi + 2) % 3]) / (temp[maxi] - temp[mini]));\n"
      "\t}\n"
      "\tif (h < 0.0) {\n"
      "\t\th += 360.0;\n"
      "\t}\n"
      "\treturn vec3(h, s, v);\n"
      "}\n\n"
      "vec3 hsv_to_rgb(vec3 hsv) {\n"
      "\tfloat h = hsv.x;\n"
      "\tfloat s = hsv.y;\n"
      "\tfloat v = hsv.z;\n"
      "\t\n"
      "\tint maxc = 0;\n"
      "\tif (h > 300.0) {\n"
      "\t\th -= 360.0;\n"
      "\t}\n"
      "\tif (h < 60.0) {\n"
      "\t\tmaxc = 0;\n"
      "\t} else if (h >= 180.0) {\n"
      "\t\tmaxc = 2;\n"
      "\t} else {\n"
      "\t\tmaxc = 1;\n"
      "\t}\n"
      "\t\n"
      "\tvec3 temp = vec3(0.0);\n"
      "\ttemp[maxc] = v;\n"
      "\t\n"
      "\tfloat mn = (v * (255.0 - s)) / 255.0;\n"
      "\tfloat temphue = (((h / 60.0) - float(2 * maxc)) * (v - mn));\n"
      "\t\n"
      "\tif (temphue > 0.0) {\n"
      "\t\ttemp[(maxc + 2) % 3] = mn;\n"
      "\t\ttemp[(maxc + 1) % 3] = mn + temphue;\n"
      "\t} else {\n"
      "\t\ttemp[(maxc + 1) % 3] = mn;\n"
      "\t\ttemp[(maxc + 2) % 3] = mn - temphue;\n"
      "\t}\n"
      "\treturn temp;\n"
      "}\n\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorMath1GenerateCodeAdd) {
  VisualShaderNodeGeneratorMath1 math1_generator{0.5f, VisualShaderNodeMath1::FUNC_ADD, true, true, true, false};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{math1_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Math1\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tout_buffer_n0.x = out_buffer_n0.x + 0.500000;\n"
      "\tout_buffer_n0.y = out_buffer_n0.y + 0.500000;\n"
      "\tout_buffer_n0.z = out_buffer_n0.z + 0.500000;\n"
      "\tb = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorMath1GenerateCodePow) {
  VisualShaderNodeGeneratorMath1 math1_generator{2.0f, VisualShaderNodeMath1::FUNC_POW, false, true, false, true};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{math1_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Math1\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tout_buffer_n0.y = pow(out_buffer_n0.y, 2.000000);\n"
      "\tout_buffer_n0.w = pow(out_buffer_n0.w, 2.000000);\n"
      "\tb = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorModulusGenerateGlobal) {
  VisualShaderNodeGeneratorModulus modulus_generator{0.5f, true, 0};
  std::string code{modulus_generator.generate_global(0)};
  std::string expected_code{
      "float modulus_mod(float x, float y) {\n"
      "\treturn x - y * trunc(x / y);\n"
      "}\n\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorModulusGenerateCodeNormalize) {
  VisualShaderNodeGeneratorModulus modulus_generator{0.5f, true, 0};
  std::vector<std::string> input_vars = {"a", "uv"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{modulus_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Modulus\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tfloat modulus_m_n0 = 0.500000;\n"
      "\tfloat modulus_s_n0 = 1.0 / modulus_m_n0;\n"
      "\tfloat modulus_bias_n0 = uv.x * modulus_m_n0 * 0.000000;\n"
      "\tout_buffer_n0.x = clamp(modulus_mod(out_buffer_n0.x + modulus_bias_n0, modulus_m_n0) * modulus_s_n0, 0.0, 1.0);\n"
      "\tout_buffer_n0.y = clamp(modulus_mod(out_buffer_n0.y + modulus_bias_n0, modulus_m_n0) * modulus_s_n0, 0.0, 1.0);\n"
      "\tout_buffer_n0.z = clamp(modulus_mod(out_buffer_n0.z + modulus_bias_n0, modulus_m_n0) * modulus_s_n0, 0.0, 1.0);\n"
      "\tb = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorModulusGenerateCodeNoNormalize) {
  VisualShaderNodeGeneratorModulus modulus_generator{0.25f, false, 2};
  std::vector<std::string> input_vars = {"a", "uv"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{modulus_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Modulus\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tfloat modulus_m_n0 = 0.250000;\n"
      "\tfloat modulus_s_n0 = 1.0;\n"
      "\tfloat modulus_bias_n0 = uv.x * modulus_m_n0 * 2.000000;\n"
      "\tout_buffer_n0.x = clamp(modulus_mod(out_buffer_n0.x + modulus_bias_n0, modulus_m_n0) * modulus_s_n0, 0.0, 1.0);\n"
      "\tout_buffer_n0.y = clamp(modulus_mod(out_buffer_n0.y + modulus_bias_n0, modulus_m_n0) * modulus_s_n0, 0.0, 1.0);\n"
      "\tout_buffer_n0.z = clamp(modulus_mod(out_buffer_n0.z + modulus_bias_n0, modulus_m_n0) * modulus_s_n0, 0.0, 1.0);\n"
      "\tb = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorCombineGenerateCodeDefaults) {
  VisualShaderNodeGeneratorCombine combine_generator{
      VisualShaderNodeCombine::CHANNEL_R, VisualShaderNodeCombine::CHANNEL_G,
      VisualShaderNodeCombine::CHANNEL_B, VisualShaderNodeCombine::CHANNEL_A};
  std::vector<std::string> input_vars = {"a", "b", "c", "d"};
  std::vector<std::string> output_vars = {"e"};
  std::string code{combine_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Combine\n"
      "\tvec4 out_buffer_n0;\n"
      "\tout_buffer_n0.x = a.x;\n"
      "\tout_buffer_n0.y = b.y;\n"
      "\tout_buffer_n0.z = c.z;\n"
      "\tout_buffer_n0.w = d.w;\n"
      "\te = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorCombineGenerateCodeReversed) {
  VisualShaderNodeGeneratorCombine combine_generator{
      VisualShaderNodeCombine::CHANNEL_A, VisualShaderNodeCombine::CHANNEL_B,
      VisualShaderNodeCombine::CHANNEL_G, VisualShaderNodeCombine::CHANNEL_R};
  std::vector<std::string> input_vars = {"a", "b", "c", "d"};
  std::vector<std::string> output_vars = {"e"};
  std::string code{combine_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Combine\n"
      "\tvec4 out_buffer_n0;\n"
      "\tout_buffer_n0.x = a.w;\n"
      "\tout_buffer_n0.y = b.z;\n"
      "\tout_buffer_n0.z = c.y;\n"
      "\tout_buffer_n0.w = d.x;\n"
      "\te = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorMaskGenerateCodeDefaults) {
  VisualShaderNodeGeneratorMask mask_generator{false};
  std::vector<std::string> input_vars = {"a", "b", "c"};
  std::vector<std::string> output_vars = {"d"};
  std::string code{mask_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Mask\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tfloat mask_w_n0 = (c.x + c.y + c.z) * (1.0 / 3.0);\n"
      "\tout_buffer_n0 = out_buffer_n0 * (1.0 - mask_w_n0);\n"
      "\tout_buffer_n0 = out_buffer_n0 + mask_w_n0 * b;\n"
      "\td = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorMaskGenerateCodeInvert) {
  VisualShaderNodeGeneratorMask mask_generator{true};
  std::vector<std::string> input_vars = {"a", "b", "c"};
  std::vector<std::string> output_vars = {"d"};
  std::string code{mask_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Mask\n"
      "\tvec4 out_buffer_n0 = a;\n"
      "\tfloat mask_w_n0 = (c.x + c.y + c.z) * (1.0 / 3.0);\n"
      "\tmask_w_n0 = 1.0 - mask_w_n0;\n"
      "\tout_buffer_n0 = out_buffer_n0 * (1.0 - mask_w_n0);\n"
      "\tout_buffer_n0 = out_buffer_n0 + mask_w_n0 * b;\n"
      "\td = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorBlendGenerateCodeNormal) {
  VisualShaderNodeGeneratorBlend blend_generator{VisualShaderNodeBlend::BLEND_NORMAL, 1.0f, false};
  std::vector<std::string> input_vars = {"a", "b"};
  std::vector<std::string> output_vars = {"c"};
  std::string code{blend_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Blend\n"
      "\tvec4 blend_color_n0 = b;\n"
      "\tfloat blend_alpha_n0 = b.w;\n"
      "\tblend_alpha_n0 *= 1.000000;\n"
      "\tblend_color_n0 = clamp(blend_color_n0, 0.0, 1.0);\n"
      "\tfloat blend_orig_w_n0 = a.w;\n"
      "\tvec4 out_buffer_n0 = a * (1.0 - blend_alpha_n0);\n"
      "\tout_buffer_n0 = out_buffer_n0 + blend_alpha_n0 * blend_color_n0;\n"
      "\tout_buffer_n0.w = blend_orig_w_n0;\n"
      "\tc = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorBlendGenerateCodeMultiplyInvertAlpha) {
  VisualShaderNodeGeneratorBlend blend_generator{VisualShaderNodeBlend::BLEND_MULTIPLY, 0.5f, true};
  std::vector<std::string> input_vars = {"a", "b"};
  std::vector<std::string> output_vars = {"c"};
  std::string code{blend_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Blend\n"
      "\tvec4 blend_color_n0 = b;\n"
      "\tfloat blend_alpha_n0 = b.w;\n"
      "\tblend_alpha_n0 = 1.0 - blend_alpha_n0;\n"
      "\tblend_alpha_n0 *= 0.500000;\n"
      "\tblend_color_n0 = blend_color_n0 * a;\n"
      "\tblend_color_n0 = clamp(blend_color_n0, 0.0, 1.0);\n"
      "\tfloat blend_orig_w_n0 = a.w;\n"
      "\tvec4 out_buffer_n0 = a * (1.0 - blend_alpha_n0);\n"
      "\tout_buffer_n0 = out_buffer_n0 + blend_alpha_n0 * blend_color_n0;\n"
      "\tout_buffer_n0.w = blend_orig_w_n0;\n"
      "\tc = out_buffer_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorizeGenerateCodeDefaultGradient) {
  VisualShaderNodeGeneratorColorize colorize_generator{std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{colorize_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Colorize\n"
      "\tfloat colorize_pos_n0 = (a.x + a.y + a.z) / 3.0;\n"
      "\tvec4 colorize_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (colorize_pos_n0 >= 1.000000) {\n"
      "\t\tcolorize_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (colorize_pos_n0 > 0.000000) {\n"
      "\t\tif (colorize_pos_n0 < 1.000000) {\n"
      "\t\t\tfloat colorize_interp_n0_0 = (colorize_pos_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tcolorize_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, "
      "1.000000, 1.000000), colorize_interp_n0_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = colorize_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorizeGenerateCodeSingleStop) {
  VisualShaderNodeGeneratorColorize colorize_generator{
      std::vector<VisualShaderColorGradientStop>{{0.25f, 0.1f, 0.2f, 0.3f, 1.0f}}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{colorize_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Colorize\n"
      "\tfloat colorize_pos_n0 = (a.x + a.y + a.z) / 3.0;\n"
      "\tvec4 colorize_out_n0 = vec4(0.100000, 0.200000, 0.300000, 1.000000);\n"
      "\tb = colorize_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorizeGenerateCodeInterpolated) {
  VisualShaderNodeGeneratorColorize colorize_generator{std::vector<VisualShaderColorGradientStop>{
      {0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, {0.5f, 0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f, 1.0f}}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{colorize_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Colorize\n"
      "\tfloat colorize_pos_n0 = (a.x + a.y + a.z) / 3.0;\n"
      "\tvec4 colorize_out_n0 = vec4(1.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (colorize_pos_n0 >= 1.000000) {\n"
      "\t\tcolorize_out_n0 = vec4(0.000000, 0.000000, 1.000000, 1.000000);\n"
      "\t} else if (colorize_pos_n0 > 0.000000) {\n"
      "\t\tif (colorize_pos_n0 < 0.500000) {\n"
      "\t\t\tfloat colorize_interp_n0_0 = (colorize_pos_n0 - 0.000000) / 0.500000;\n"
      "\t\t\tcolorize_out_n0 = mix(vec4(1.000000, 0.000000, 0.000000, 1.000000), vec4(0.000000, 1.000000, "
      "0.000000, 1.000000), colorize_interp_n0_0);\n"
      "\t\t} else {\n"
      "\t\t\tfloat colorize_interp_n0_1 = (colorize_pos_n0 - 0.500000) / 0.500000;\n"
      "\t\t\tcolorize_out_n0 = mix(vec4(0.000000, 1.000000, 0.000000, 1.000000), vec4(0.000000, 0.000000, "
      "1.000000, 1.000000), colorize_interp_n0_1);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = colorize_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}
