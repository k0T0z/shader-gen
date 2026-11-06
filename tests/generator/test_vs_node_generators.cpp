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
#include "generator/vs_node_pattern_generators.hpp"
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

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorValueNoiseGenerateGlobalPerNode) {
  VisualShaderNodeGeneratorValueNoise value_noise_generator{100.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{value_noise_generator.generate_global_per_node(0)};
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
  VisualShaderNodeGeneratorPerlinNoise perlin_noise_generator{
      1.0f, 2.0f, 1.0f, 0.5f, 1, 7, -1, true, std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{perlin_noise_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Perlin Noise\n"
      "\tfloat perlin_val_n0 = 0.0;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 1.000000, a.y * 2.000000, 0.0), 1, 2, 256) * "
      "1.000000;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 2.000000, a.y * 4.000000, 0.0), 2, 4, 256) * "
      "0.500000;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 4.000000, a.y * 8.000000, 0.0), 4, 8, 256) * "
      "0.250000;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 8.000000, a.y * 16.000000, 0.0), 8, 16, 256) * "
      "0.125000;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 16.000000, a.y * 32.000000, 0.0), 16, 32, 256) * "
      "0.062500;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 32.000000, a.y * 64.000000, 0.0), 32, 64, 256) * "
      "0.031250;\n"
      "\tperlin_val_n0 += perlin_sample3d_periodic_n0(vec3(a.x * 64.000000, a.y * 128.000000, 0.0), 64, 128, 256) * "
      "0.015625;\n"
      "\tperlin_val_n0 = perlin_val_n0 * 0.5 + 0.5;\n"
      "\tperlin_val_n0 *= 1.000000;\n"
      "\tperlin_val_n0 = clamp(perlin_val_n0, 0.0, 1.0);\n"
      "\tvec4 perlin_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (perlin_val_n0 >= 1.000000) {\n"
      "\t\tperlin_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (perlin_val_n0 > 0.000000) {\n"
      "\t\tif (perlin_val_n0 < 1.000000) {\n"
      "\t\t\tfloat perlin_n0_interp_0 = (perlin_val_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tperlin_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, 1.000000, "
      "1.000000), perlin_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = perlin_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorPerlinNoiseGenerateGlobal) {
  VisualShaderNodeGeneratorPerlinNoise perlin_noise_generator{
      1.0f, 2.0f, 1.0f, 0.5f, 1, 7, -1, true, std::vector<VisualShaderColorGradientStop>{}};
  std::string code{perlin_noise_generator.generate_global(0)};
  std::string expected_code{
    "int perlin_floor_to_int_n0(float a) {\n"
    "\treturn (a >= 0.0) ? int(a) : int(a) - 1;\n"
    "}\n\n"
    "float perlin_fade_n0(float t) {\n"
    "\treturn t * t * t * (t * (t * 6.0 - 15.0) + 10.0);\n"
    "}\n\n"
    "float perlin_lerp_n0(float t, float a, float b) {\n"
    "\treturn a + t * (b - a);\n"
    "}\n\n"
    "float perlin_grad_n0(int hash, float x, float y, float z) {\n"
    "\tint h = hash & 15;\n"
    "\tfloat u = (h < 8) ? x : y;\n"
    "\tfloat v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);\n"
    "\treturn ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);\n"
    "}\n\n"
    "const int perlin_p_n0[256] = int[256](151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, "
    "140, 36, 103, 30, 69, "
    "142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, "
    "219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, "
    "175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, "
    "220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, "
    "76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, "
    "3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, "
    "227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, "
    "221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, "
    "185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, "
    "145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, "
    "45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, "
    "215, 61, 156, 180);\n\n"
    "int perlin_perm_n0(int idx) {\n"
    "\treturn perlin_p_n0[idx & 255];\n"
    "}\n\n"
    "float perlin_sample3d_n0(vec3 pos, float scale) {\n"
    "\tvec3 xyz = pos * scale;\n"
    "\tint X = perlin_floor_to_int_n0(xyz.x);\n"
    "\tint Y = perlin_floor_to_int_n0(xyz.y);\n"
    "\tint Z = perlin_floor_to_int_n0(xyz.z);\n"
    "\txyz -= vec3(float(X), float(Y), float(Z));\n"
    "\tX &= 255;\n"
    "\tY &= 255;\n"
    "\tZ &= 255;\n"
    "\t\n"
    "\tfloat u = perlin_fade_n0(xyz.x);\n"
    "\tfloat v = perlin_fade_n0(xyz.y);\n"
    "\tfloat w = perlin_fade_n0(xyz.z);\n"
    "\t\n"
    "\tint A = perlin_perm_n0(X) + Y;\n"
    "\tint AA = perlin_perm_n0(A) + Z;\n"
    "\tint AB = perlin_perm_n0(A + 1) + Z;\n"
    "\tint B = perlin_perm_n0(X + 1) + Y;\n"
    "\tint BA = perlin_perm_n0(B) + Z;\n"
    "\tint BB = perlin_perm_n0(B + 1) + Z;\n"
    "\t\n"
    "\treturn perlin_lerp_n0(w, perlin_lerp_n0(v, perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AA), xyz.x, xyz.y, xyz.z), "
    "perlin_grad_n0(perlin_perm_n0(BA), xyz.x - 1.0, xyz.y, xyz.z)), perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AB), "
    "xyz.x, xyz.y - 1.0, xyz.z), perlin_grad_n0(perlin_perm_n0(BB), xyz.x - 1.0, xyz.y - 1.0, xyz.z))), perlin_lerp_n0(v, "
    "perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AA + 1), xyz.x, xyz.y, xyz.z - 1.0), perlin_grad_n0(perlin_perm_n0(BA + "
    "1), xyz.x - 1.0, xyz.y, xyz.z - 1.0)), perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AB + 1), xyz.x, xyz.y - 1.0, "
    "xyz.z - 1.0), perlin_grad_n0(perlin_perm_n0(BB + 1), xyz.x - 1.0, xyz.y - 1.0, xyz.z - 1.0))));\n"
    "}\n\n"
    "float perlin_sample3d_periodic_n0(vec3 pos, int period_x, int period_y, int period_z) {\n"
    "\tvec3 xyz = pos;\n"
    "\tint X = perlin_floor_to_int_n0(xyz.x);\n"
    "\tint Y = perlin_floor_to_int_n0(xyz.y);\n"
    "\tint Z = perlin_floor_to_int_n0(xyz.z);\n"
    "\txyz -= vec3(float(X), float(Y), float(Z));\n"
    "\tX &= 255;\n"
    "\tY &= 255;\n"
    "\tZ &= 255;\n"
    "\t\n"
    "\tint Ix = X % period_x;\n"
    "\tint Iy = Y % period_y;\n"
    "\tint Iz = Z % period_z;\n"
    "\tint Jx = (Ix + 1) % period_x;\n"
    "\tint Jy = (Iy + 1) % period_y;\n"
    "\tint Jz = (Iz + 1) % period_z;\n"
    "\t\n"
    "\tfloat u = perlin_fade_n0(xyz.x);\n"
    "\tfloat v = perlin_fade_n0(xyz.y);\n"
    "\tfloat w = perlin_fade_n0(xyz.z);\n"
    "\t\n"
    "\tint A = perlin_perm_n0(Ix);\n"
    "\tint AA = perlin_perm_n0(A + Iy);\n"
    "\tint AB = perlin_perm_n0(A + Jy);\n"
    "\tint B = perlin_perm_n0(Jx);\n"
    "\tint BA = perlin_perm_n0(B + Iy);\n"
    "\tint BB = perlin_perm_n0(B + Jy);\n"
    "\t\n"
    "\treturn perlin_lerp_n0(w, perlin_lerp_n0(v, perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AA + Iz), xyz.x, xyz.y, xyz.z), "
    "perlin_grad_n0(perlin_perm_n0(BA + Iz), xyz.x - 1.0, xyz.y, xyz.z)), perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AB + "
    "Iz), xyz.x, xyz.y - 1.0, xyz.z), perlin_grad_n0(perlin_perm_n0(BB + Iz), xyz.x - 1.0, xyz.y - 1.0, xyz.z))), "
    "perlin_lerp_n0(v, perlin_lerp_n0(u, perlin_grad_n0(perlin_perm_n0(AA + Jz), xyz.x, xyz.y, xyz.z - 1.0), "
    "perlin_grad_n0(perlin_perm_n0(BA + Jz), xyz.x - 1.0, xyz.y, xyz.z - 1.0)), perlin_lerp_n0(u, "
    "perlin_grad_n0(perlin_perm_n0(AB + Jz), xyz.x, xyz.y - 1.0, xyz.z - 1.0), perlin_grad_n0(perlin_perm_n0(BB + Jz), "
    "xyz.x - 1.0, xyz.y - 1.0, xyz.z - 1.0))));\n"
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

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorVoronoiNoiseGenerateGlobalPerNode) {
  VisualShaderNodeGeneratorVoronoiNoise voronoi_noise_generator{10.0f, 10.0f};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{voronoi_noise_generator.generate_global_per_node(0)};
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

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorColorCorrectGenerateGlobalPerNode) {
  VisualShaderNodeGeneratorColorCorrect color_correct_generator{0.0f, 1.0f, 1.0f, 0.0f};
  std::string code{color_correct_generator.generate_global_per_node(0)};
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

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorModulusGenerateGlobalPerNode) {
  VisualShaderNodeGeneratorModulus modulus_generator{0.5f, true, 0};
  std::string code{modulus_generator.generate_global_per_node(0)};
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

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorGradientGenerateCodeLinearDefaultGradient) {
  VisualShaderNodeGeneratorGradient gradient_generator{VisualShaderNodeGradient::GRADIENT_LINEAR,
                                                       std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{gradient_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Gradient\n"
      "\tvec2 pattern_uv_n0 = fract(a);\n"
      "\tfloat pattern_pos_n0 = 0.0;\n"
      "\tif (pattern_uv_n0.y <= 0.5) {\n"
      "\t\tpattern_pos_n0 = pattern_uv_n0.y * 2.0;\n"
      "\t} else {\n"
      "\t\tpattern_pos_n0 = (1.0 - pattern_uv_n0.y) * 2.0;\n"
      "\t}\n"
      "\tvec4 pattern_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (pattern_pos_n0 >= 1.000000) {\n"
      "\t\tpattern_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (pattern_pos_n0 > 0.000000) {\n"
      "\t\tif (pattern_pos_n0 < 1.000000) {\n"
      "\t\t\tfloat pattern_n0_interp_0 = (pattern_pos_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tpattern_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, "
      "1.000000, 1.000000), pattern_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = pattern_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorGradientGenerateCodeSquareCustomGradient) {
  VisualShaderNodeGeneratorGradient gradient_generator{
      VisualShaderNodeGradient::GRADIENT_SQUARE,
      std::vector<VisualShaderColorGradientStop>{{0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f, 1.0f}}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{gradient_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Gradient\n"
      "\tvec2 pattern_uv_n0 = fract(a);\n"
      "\tfloat pattern_pos_n0 = 0.0;\n"
      "\tpattern_pos_n0 = 1.0 - 2.0 * max(abs(0.5 - pattern_uv_n0.x), abs(0.5 - pattern_uv_n0.y));\n"
      "\tvec4 pattern_out_n0 = vec4(1.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (pattern_pos_n0 >= 1.000000) {\n"
      "\t\tpattern_out_n0 = vec4(0.000000, 0.000000, 1.000000, 1.000000);\n"
      "\t} else if (pattern_pos_n0 > 0.000000) {\n"
      "\t\tif (pattern_pos_n0 < 1.000000) {\n"
      "\t\t\tfloat pattern_n0_interp_0 = (pattern_pos_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tpattern_out_n0 = mix(vec4(1.000000, 0.000000, 0.000000, 1.000000), vec4(0.000000, 0.000000, "
      "1.000000, 1.000000), pattern_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = pattern_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorCheckerGenerateCode) {
  VisualShaderNodeGeneratorChecker checker_generator{4.0f, 3.0f, std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{checker_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Checker\n"
      "\tvec2 pattern_uv_n0 = fract(a);\n"
      "\tint pattern_checker_n0 = (int(floor(pattern_uv_n0.x * 4.000000 * 2.0)) + int(floor(pattern_uv_n0.y * "
      "3.000000 * 2.0))) & 1;\n"
      "\tfloat pattern_pos_n0 = pattern_checker_n0 == 0 ? 1.0 : 0.0;\n"
      "\tvec4 pattern_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (pattern_pos_n0 >= 1.000000) {\n"
      "\t\tpattern_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (pattern_pos_n0 > 0.000000) {\n"
      "\t\tif (pattern_pos_n0 < 1.000000) {\n"
      "\t\t\tfloat pattern_n0_interp_0 = (pattern_pos_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tpattern_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, "
      "1.000000, 1.000000), pattern_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = pattern_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorTileGenerateCode) {
  VisualShaderNodeGeneratorTile tile_generator{0.1f, 0.2f, 0.05f, std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{tile_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Tile\n"
      "\tvec2 pattern_uv_n0 = fract(a);\n"
      "\tfloat pattern_pos_n0 = 0.0;\n"
      "\tif (pattern_uv_n0.x < 0.100000 || pattern_uv_n0.x > 0.900000 || pattern_uv_n0.y < 0.200000 || "
      "pattern_uv_n0.y > 0.800000) {\n"
      "\t\tpattern_pos_n0 = 0.0;\n"
      "\t} else {\n"
      "\t\tfloat pattern_dist_u_n0 = min(pattern_uv_n0.x - 0.100000, 1.0 - pattern_uv_n0.x - 0.100000);\n"
      "\t\tfloat pattern_dist_v_n0 = min(pattern_uv_n0.y - 0.200000, 1.0 - pattern_uv_n0.y - 0.200000);\n"
      "\t\tfloat pattern_dist_n0 = min(pattern_dist_u_n0, pattern_dist_v_n0);\n"
      "\t\tif (pattern_dist_n0 < 0.050000) {\n"
      "\t\t\tpattern_pos_n0 = pattern_dist_n0 / 0.050000;\n"
      "\t\t} else {\n"
      "\t\t\tpattern_pos_n0 = 1.0;\n"
      "\t\t}\n"
      "\t}\n"
      "\tvec4 pattern_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (pattern_pos_n0 >= 1.000000) {\n"
      "\t\tpattern_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (pattern_pos_n0 > 0.000000) {\n"
      "\t\tif (pattern_pos_n0 < 1.000000) {\n"
      "\t\t\tfloat pattern_n0_interp_0 = (pattern_pos_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tpattern_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, "
      "1.000000, 1.000000), pattern_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = pattern_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorFunctionGenerateCodeSinMultiply) {
  VisualShaderNodeGeneratorFunction function_generator{
      VisualShaderNodeFunction::FUNC_SIN, VisualShaderNodeFunction::FUNC_SIN,
      VisualShaderNodeFunction::COMBINE_MULTIPLY, 1.0f, 1.0f, std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{function_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Function\n"
      "\tvec2 pattern_uv_n0 = fract(a);\n"
      "\tfloat pattern_su_n0 = cos(3.141592653589793 * 2.0 * 1.000000 * (pattern_uv_n0.x + 0.5)) * 0.5 + 0.5;\n"
      "\tfloat pattern_sv_n0 = cos(3.141592653589793 * 2.0 * 1.000000 * (pattern_uv_n0.y + 0.5)) * 0.5 + 0.5;\n"
      "\tfloat pattern_val_n0 = pattern_su_n0 * pattern_sv_n0;\n"
      "\tvec4 pattern_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (pattern_val_n0 >= 1.000000) {\n"
      "\t\tpattern_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (pattern_val_n0 > 0.000000) {\n"
      "\t\tif (pattern_val_n0 < 1.000000) {\n"
      "\t\t\tfloat pattern_n0_interp_0 = (pattern_val_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tpattern_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, "
      "1.000000, 1.000000), pattern_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = pattern_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}

TEST(VisualShaderNodeGeneratorTest, TestVisualShaderNodeGeneratorBrickGenerateCode) {
  VisualShaderNodeGeneratorBrick brick_generator{2, 1, 0.5f, 0.25f, 0.1f, 0.2f, 0.1f, true,
                                                std::vector<VisualShaderColorGradientStop>{}};
  std::vector<std::string> input_vars = {"a"};
  std::vector<std::string> output_vars = {"b"};
  std::string code{brick_generator.generate_code(0, input_vars, output_vars)};
  std::string expected_code{
      "\t// Brick\n"
      "\tconst float pattern_colors_n0[2] = float[2](0.721693, 0.352360);\n"
      "\tconst float pattern_shifts_n0[1] = float[1](0.133095);\n"
      "\tvec2 pattern_uv_n0 = fract(a);\n"
      "\tfloat pattern_norm_x_n0 = 2.000000;\n"
      "\tfloat pattern_norm_y_n0 = 1.000000;\n"
      "\tfloat pattern_uu_n0 = pattern_uv_n0.x * pattern_norm_x_n0;\n"
      "\tfloat pattern_vv_n0 = pattern_uv_n0.y * pattern_norm_y_n0;\n"
      "\tfloat pattern_y_n0 = pattern_vv_n0 - floor(pattern_vv_n0);\n"
      "\tif (pattern_vv_n0 >= pattern_norm_y_n0) pattern_vv_n0 -= pattern_norm_y_n0;\n"
      "\tif (pattern_uu_n0 >= pattern_norm_x_n0) pattern_uu_n0 -= pattern_norm_x_n0;\n"
      "\tint pattern_by_n0 = int(floor(pattern_vv_n0));\n"
      "\tif (pattern_vv_n0 * 0.5 - floor(pattern_vv_n0 * 0.5) >= 0.5) pattern_uu_n0 += 0.500000;\n"
      "\tpattern_uu_n0 += pattern_shifts_n0[pattern_by_n0] * 0.250000;\n"
      "\tif (pattern_uu_n0 >= pattern_norm_x_n0) pattern_uu_n0 -= pattern_norm_x_n0;\n"
      "\tint pattern_bx_n0 = int(floor(pattern_uu_n0));\n"
      "\tfloat pattern_x_n0 = pattern_uu_n0 - floor(pattern_uu_n0);\n"
      "\tfloat pattern_val_n0 = 1.0;\n"
      "\tpattern_val_n0 = pattern_colors_n0[pattern_bx_n0 + pattern_by_n0 * 2];\n"
      "\tbool pattern_inside_n0 = true;\n"
      "\tpattern_inside_n0 = pattern_inside_n0 && (pattern_x_n0 > 0.100000 * pattern_norm_x_n0);\n"
      "\tpattern_inside_n0 = pattern_inside_n0 && (pattern_y_n0 > 0.200000 * pattern_norm_y_n0);\n"
      "\tif (pattern_inside_n0) {\n"
      "\t\tfloat pattern_dist_n0 = min(min(pattern_x_n0 - 0.100000 * pattern_norm_x_n0, 1.0 - pattern_x_n0) / "
      "pattern_norm_x_n0, min(pattern_y_n0 - 0.200000 * pattern_norm_y_n0, 1.0 - pattern_y_n0) / "
      "pattern_norm_y_n0);\n"
      "\t\tpattern_dist_n0 *= min(pattern_norm_x_n0, pattern_norm_y_n0);\n"
      "\t\tif (pattern_dist_n0 < 0.100000) {\n"
      "\t\t\tpattern_val_n0 = pattern_val_n0 * pattern_dist_n0 / 0.100000;\n"
      "\t\t}\n"
      "\t} else {\n"
      "\t\tpattern_val_n0 = 0.0;\n"
      "\t}\n"
      "\tvec4 pattern_out_n0 = vec4(0.000000, 0.000000, 0.000000, 1.000000);\n"
      "\tif (pattern_val_n0 >= 1.000000) {\n"
      "\t\tpattern_out_n0 = vec4(1.000000, 1.000000, 1.000000, 1.000000);\n"
      "\t} else if (pattern_val_n0 > 0.000000) {\n"
      "\t\tif (pattern_val_n0 < 1.000000) {\n"
      "\t\t\tfloat pattern_n0_interp_0 = (pattern_val_n0 - 0.000000) / 1.000000;\n"
      "\t\t\tpattern_out_n0 = mix(vec4(0.000000, 0.000000, 0.000000, 1.000000), vec4(1.000000, 1.000000, "
      "1.000000, 1.000000), pattern_n0_interp_0);\n"
      "\t\t}\n"
      "\t}\n"
      "\tb = pattern_out_n0;\n"};
  ASSERT_EQ(code, expected_code);
}
