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

#include "generator/vs_node_noise_generators.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>
#include <utility>
#include <vector>

namespace {

std::string to_glsl_float(const float& value) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(6) << value;
  return oss.str();
}

std::string to_glsl_color(const VisualShaderColorGradientStop& stop) {
  return "vec4(" + to_glsl_float(stop.r) + ", " + to_glsl_float(stop.g) + ", " + to_glsl_float(stop.b) + ", " +
         to_glsl_float(stop.a) + ")";
}

/**
 * @brief Emit GLSL that samples a color gradient the same way @c ColorGradient.getColor does.
 *
 * Values below the first stop use the first color, values above the last stop use the last color and
 * anything in between is linearly interpolated between the two neighbouring stops. The sampled color is
 * written to the local @c vec4 declared by this function.
 */
std::string emit_gradient_lookup(const std::vector<VisualShaderColorGradientStop>& input_gradient,
                                 const std::string& out_var, const std::string& pos_var, const std::string& prefix) {
  // Fall back to the default black -> white ramp so that the generated code is always well-formed.
  std::vector<VisualShaderColorGradientStop> stops{input_gradient};
  if (stops.empty()) {
    stops.push_back(VisualShaderColorGradientStop{0.0f, 0.0f, 0.0f, 0.0f, 1.0f});
    stops.push_back(VisualShaderColorGradientStop{1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
  }

  std::string code;

  code += std::string("\t") + "vec4 " + out_var + " = " + to_glsl_color(stops.front()) + ";" + std::string("\n");

  if (stops.size() > 1) {
    code += std::string("\t") + "if (" + pos_var + " >= " + to_glsl_float(stops.back().position) + ") {" +
            std::string("\n");
    code += std::string("\t") + std::string("\t") + out_var + " = " + to_glsl_color(stops.back()) + ";" +
            std::string("\n");
    code += std::string("\t") + "} else if (" + pos_var + " > " + to_glsl_float(stops.front().position) + ") {" +
            std::string("\n");

    const std::size_t segment_count{stops.size() - 1};
    for (std::size_t i = 0; i < segment_count; ++i) {
      const VisualShaderColorGradientStop& a{stops.at(i)};
      const VisualShaderColorGradientStop& b{stops.at(i + 1)};

      if (i == 0) {
        code += std::string("\t") + std::string("\t") + "if (" + pos_var + " < " + to_glsl_float(b.position) + ") {" +
                std::string("\n");
      } else if (i < segment_count - 1) {
        code += std::string("\t") + std::string("\t") + "} else if (" + pos_var + " < " + to_glsl_float(b.position) +
                ") {" + std::string("\n");
      } else {
        code += std::string("\t") + std::string("\t") + "} else {" + std::string("\n");
      }

      const std::string interp{prefix + "_interp_" + std::to_string(i)};
      code += std::string("\t") + std::string("\t") + std::string("\t") + "float " + interp + " = (" + pos_var +
              " - " + to_glsl_float(a.position) + ") / " + to_glsl_float(b.position - a.position) + ";" +
              std::string("\n");
      code += std::string("\t") + std::string("\t") + std::string("\t") + out_var + " = mix(" + to_glsl_color(a) +
              ", " + to_glsl_color(b) + ", " + interp + ");" + std::string("\n");
    }

    code += std::string("\t") + std::string("\t") + "}" + std::string("\n");
    code += std::string("\t") + "}" + std::string("\n");
  }

  return code;
}

/**
 * @brief Ken Perlin's reference permutation table (from "Improving Noise", 2002).
 */
const std::array<int, 256>& perlin_reference_permutation() {
  static const std::array<int, 256> permutation{
      151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225, 140, 36,  103, 30,  69,
      142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252,
      219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136, 171, 168, 68,
      175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230,
      220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
      76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186,
      3,   64,  52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,
      227, 47,  16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,  154, 163, 70,
      221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253, 19,  98,  108, 110, 79,  113, 224, 232, 178,
      185, 112, 104, 218, 246, 97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
      145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184, 84,  204, 176, 115, 121, 50,
      45,  127, 4,   150, 254, 138, 236, 205, 93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,
      215, 61,  156, 180};
  return permutation;
}

/**
 * @brief A minimal re-implementation of @c java.util.Random needed to reproduce @c nextInt(256) exactly.
 */
class JavaRandom {
 public:
  explicit JavaRandom(const std::int64_t& seed) : state((static_cast<std::uint64_t>(seed) ^ MULTIPLIER) & MASK) {}

  // Only power-of-two bounds (256) are used by the permutation shuffle, so the rejection branch of
  // @c java.util.Random.nextInt is not needed.
  int next_int(const int& bound) {
    const int r{next(31)};
    return static_cast<int>((static_cast<std::int64_t>(bound) * static_cast<std::int64_t>(r)) >> 31);
  }

 private:
  int next(const int& bits) {
    state = (state * MULTIPLIER + ADDEND) & MASK;
    return static_cast<int>(state >> (48 - bits));
  }

  std::uint64_t state{0};
  static constexpr std::uint64_t MULTIPLIER{0x5DEECE66DULL};
  static constexpr std::uint64_t ADDEND{0xBULL};
  static constexpr std::uint64_t MASK{(1ULL << 48) - 1ULL};
};

/**
 * @brief Build the Improved Perlin permutation table for a seed, mirroring @c Noise3D_ImprovedPerlin.setSeed.
 *
 * A negative seed uses Ken Perlin's reference permutation, otherwise the table is Fisher-Yates shuffled
 * with @c java.util.Random(seed) (for seeds @c >= 0).
 */
std::array<int, 256> build_perlin_permutation(const int& seed) {
  std::array<int, 256> p{};

  if (seed < 0) {
    p = perlin_reference_permutation();
    return p;
  }

  for (int i{0}; i < 256; ++i) p.at(static_cast<std::size_t>(i)) = i;

  JavaRandom random{static_cast<std::int64_t>(seed)};
  for (int i{0}; i < 256; ++i) {
    const int j{random.next_int(256)};
    std::swap(p.at(static_cast<std::size_t>(i)), p.at(static_cast<std::size_t>(j)));
  }

  return p;
}

std::string to_glsl_perlin_permutation(const std::array<int, 256>& permutation) {
  std::string code{"int[256]("};
  for (std::size_t i = 0; i < permutation.size(); ++i) {
    if (i != 0) code += ", ";
    code += std::to_string(permutation.at(i));
  }
  code += ")";
  return code;
}

}  // namespace

/*************************************/
/* Value (Simple) Noise              */
/*************************************/

std::string VisualShaderNodeGeneratorValueNoise::generate_global([[maybe_unused]] const int& id) const {
    std::string code;
  code += "float noise_random_value(vec2 uv) {" + std::string("\n");
  code += std::string("\t") + "return fract(sin(dot(uv, vec2(12.9898, 78.233)))*43758.5453);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float noise_interpolate(float a, float b, float t) {" + std::string("\n");
  code += std::string("\t") + "return (1.0-t)*a + (t*b);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float value_noise(vec2 uv) {" + std::string("\n");
  code += std::string("\t") + "vec2 i = floor(uv);" + std::string("\n");
  code += std::string("\t") + "vec2 f = fract(uv);" + std::string("\n");
  code += std::string("\t") + "f = f * f * (3.0 - 2.0 * f);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "uv = abs(fract(uv) - 0.5);" + std::string("\n");
  code += std::string("\t") + "vec2 c0 = i + vec2(0.0, 0.0);" + std::string("\n");
  code += std::string("\t") + "vec2 c1 = i + vec2(1.0, 0.0);" + std::string("\n");
  code += std::string("\t") + "vec2 c2 = i + vec2(0.0, 1.0);" + std::string("\n");
  code += std::string("\t") + "vec2 c3 = i + vec2(1.0, 1.0);" + std::string("\n");
  code += std::string("\t") + "float r0 = noise_random_value(c0);" + std::string("\n");
  code += std::string("\t") + "float r1 = noise_random_value(c1);" + std::string("\n");
  code += std::string("\t") + "float r2 = noise_random_value(c2);" + std::string("\n");
  code += std::string("\t") + "float r3 = noise_random_value(c3);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "float bottom_of_grid = noise_interpolate(r0, r1, f.x);" + std::string("\n");
  code += std::string("\t") + "float top_of_grid = noise_interpolate(r2, r3, f.x);" + std::string("\n");
  code += std::string("\t") + "float t = noise_interpolate(bottom_of_grid, top_of_grid, f.y);" + std::string("\n");
  code += std::string("\t") + "return t;" + std::string("\n");
  code += std::string("}") + "\n\n";

  code +=
      "void generate_value_noise_float(vec2 uv, float scale, out float "
      "out_buffer) {" +
      std::string("\n");
  code += std::string("\t") + "float t = 0.0;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "float freq = pow(2.0, float(0));" + std::string("\n");
  code += std::string("\t") + "float amp = pow(0.5, float(3-0));" + std::string("\n");
  code += std::string("\t") + "t += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "freq = pow(2.0, float(1));" + std::string("\n");
  code += std::string("\t") + "amp = pow(0.5, float(3-1));" + std::string("\n");
  code += std::string("\t") + "t += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "freq = pow(2.0, float(2));" + std::string("\n");
  code += std::string("\t") + "amp = pow(0.5, float(3-2));" + std::string("\n");
  code += std::string("\t") + "t += value_noise(vec2(uv.x*scale/freq, uv.y*scale/freq))*amp;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "out_buffer = t;" + std::string("\n");
  code += std::string("}") + "\n\n";

  return code;
}

std::string VisualShaderNodeGeneratorValueNoise::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(6) << scale;

  std::string code;

  code += std::string("\t") + "// Value Noise" + std::string("\n");
  code += std::string("\t") + "float out_buffer_n" + std::to_string(id) + " = 0.0;" + std::string("\n");
  code += std::string("\t") + "generate_value_noise_float(" + input_vars[0] + ", " + oss.str() + ", out_buffer_n" + std::to_string(id) + ");" +
          std::string("\n");
  code += std::string("\t") + output_vars[0] + " = vec4(out_buffer_n" + std::to_string(id) + ", out_buffer_n" + std::to_string(id) + ", out_buffer_n" + std::to_string(id) + ", 1.0);" + std::string("\n");
  code += std::string("\t") + std::string("\n");

  return code;
}

/*************************************/
/* Perlin (Gradient) Noise           */
/*************************************/

std::string VisualShaderNodeGeneratorPerlinNoise::generate_global([[maybe_unused]] const int& id) const {
    std::string code;

  code += "int perlin_floor_to_int(float a) {" + std::string("\n");
  code += std::string("\t") + "return (a >= 0.0) ? int(a) : int(a) - 1;" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float perlin_fade(float t) {" + std::string("\n");
  code += std::string("\t") + "return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float perlin_lerp(float t, float a, float b) {" + std::string("\n");
  code += std::string("\t") + "return a + t * (b - a);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float perlin_grad(int hash, float x, float y, float z) {" + std::string("\n");
  code += std::string("\t") + "int h = hash & 15;" + std::string("\n");
  code += std::string("\t") + "float u = (h < 8) ? x : y;" + std::string("\n");
  code += std::string("\t") + "float v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);" + std::string("\n");
  code += std::string("\t") + "return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "int perlin_perm(in int p[256], int idx) {" + std::string("\n");
  code += std::string("\t") + "return p[idx & 255];" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float perlin_sample3d(in int p[256], vec3 pos, float scale) {" + std::string("\n");
  code += std::string("\t") + "vec3 xyz = pos * scale;" + std::string("\n");
  code += std::string("\t") + "int X = perlin_floor_to_int(xyz.x);" + std::string("\n");
  code += std::string("\t") + "int Y = perlin_floor_to_int(xyz.y);" + std::string("\n");
  code += std::string("\t") + "int Z = perlin_floor_to_int(xyz.z);" + std::string("\n");
  code += std::string("\t") + "xyz -= vec3(float(X), float(Y), float(Z));" + std::string("\n");
  code += std::string("\t") + "X &= 255;" + std::string("\n");
  code += std::string("\t") + "Y &= 255;" + std::string("\n");
  code += std::string("\t") + "Z &= 255;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "float u = perlin_fade(xyz.x);" + std::string("\n");
  code += std::string("\t") + "float v = perlin_fade(xyz.y);" + std::string("\n");
  code += std::string("\t") + "float w = perlin_fade(xyz.z);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "int A = perlin_perm(p, X) + Y;" + std::string("\n");
  code += std::string("\t") + "int AA = perlin_perm(p, A) + Z;" + std::string("\n");
  code += std::string("\t") + "int AB = perlin_perm(p, A + 1) + Z;" + std::string("\n");
  code += std::string("\t") + "int B = perlin_perm(p, X + 1) + Y;" + std::string("\n");
  code += std::string("\t") + "int BA = perlin_perm(p, B) + Z;" + std::string("\n");
  code += std::string("\t") + "int BB = perlin_perm(p, B + 1) + Z;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "return perlin_lerp(w, perlin_lerp(v, perlin_lerp(u, perlin_grad(perlin_perm(p, AA), xyz.x, xyz.y, xyz.z), perlin_grad(perlin_perm(p, BA), xyz.x - 1.0, xyz.y, xyz.z)), perlin_lerp(u, perlin_grad(perlin_perm(p, AB), xyz.x, xyz.y - 1.0, xyz.z), perlin_grad(perlin_perm(p, BB), xyz.x - 1.0, xyz.y - 1.0, xyz.z))), perlin_lerp(v, perlin_lerp(u, perlin_grad(perlin_perm(p, AA + 1), xyz.x, xyz.y, xyz.z - 1.0), perlin_grad(perlin_perm(p, BA + 1), xyz.x - 1.0, xyz.y, xyz.z - 1.0)), perlin_lerp(u, perlin_grad(perlin_perm(p, AB + 1), xyz.x, xyz.y - 1.0, xyz.z - 1.0), perlin_grad(perlin_perm(p, BB + 1), xyz.x - 1.0, xyz.y - 1.0, xyz.z - 1.0))));" + std::string("\n");
  code += std::string("}") + "\n\n";

  code += "float perlin_sample3d_periodic(in int p[256], vec3 pos, int period_x, int period_y, int period_z) {" + std::string("\n");
  code += std::string("\t") + "vec3 xyz = pos;" + std::string("\n");
  code += std::string("\t") + "int X = perlin_floor_to_int(xyz.x);" + std::string("\n");
  code += std::string("\t") + "int Y = perlin_floor_to_int(xyz.y);" + std::string("\n");
  code += std::string("\t") + "int Z = perlin_floor_to_int(xyz.z);" + std::string("\n");
  code += std::string("\t") + "xyz -= vec3(float(X), float(Y), float(Z));" + std::string("\n");
  code += std::string("\t") + "X &= 255;" + std::string("\n");
  code += std::string("\t") + "Y &= 255;" + std::string("\n");
  code += std::string("\t") + "Z &= 255;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "int Ix = X % period_x;" + std::string("\n");
  code += std::string("\t") + "int Iy = Y % period_y;" + std::string("\n");
  code += std::string("\t") + "int Iz = Z % period_z;" + std::string("\n");
  code += std::string("\t") + "int Jx = (Ix + 1) % period_x;" + std::string("\n");
  code += std::string("\t") + "int Jy = (Iy + 1) % period_y;" + std::string("\n");
  code += std::string("\t") + "int Jz = (Iz + 1) % period_z;" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "float u = perlin_fade(xyz.x);" + std::string("\n");
  code += std::string("\t") + "float v = perlin_fade(xyz.y);" + std::string("\n");
  code += std::string("\t") + "float w = perlin_fade(xyz.z);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "int A = perlin_perm(p, Ix);" + std::string("\n");
  code += std::string("\t") + "int AA = perlin_perm(p, A + Iy);" + std::string("\n");
  code += std::string("\t") + "int AB = perlin_perm(p, A + Jy);" + std::string("\n");
  code += std::string("\t") + "int B = perlin_perm(p, Jx);" + std::string("\n");
  code += std::string("\t") + "int BA = perlin_perm(p, B + Iy);" + std::string("\n");
  code += std::string("\t") + "int BB = perlin_perm(p, B + Jy);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "return perlin_lerp(w, perlin_lerp(v, perlin_lerp(u, perlin_grad(perlin_perm(p, AA + Iz), xyz.x, xyz.y, xyz.z), perlin_grad(perlin_perm(p, BA + Iz), xyz.x - 1.0, xyz.y, xyz.z)), perlin_lerp(u, perlin_grad(perlin_perm(p, AB + Iz), xyz.x, xyz.y - 1.0, xyz.z), perlin_grad(perlin_perm(p, BB + Iz), xyz.x - 1.0, xyz.y - 1.0, xyz.z))), perlin_lerp(v, perlin_lerp(u, perlin_grad(perlin_perm(p, AA + Jz), xyz.x, xyz.y, xyz.z - 1.0), perlin_grad(perlin_perm(p, BA + Jz), xyz.x - 1.0, xyz.y, xyz.z - 1.0)), perlin_lerp(u, perlin_grad(perlin_perm(p, AB + Jz), xyz.x, xyz.y - 1.0, xyz.z - 1.0), perlin_grad(perlin_perm(p, BB + Jz), xyz.x - 1.0, xyz.y - 1.0, xyz.z - 1.0))));" + std::string("\n");
  code += std::string("}") + "\n\n";

  return code;
}

std::string VisualShaderNodeGeneratorPerlinNoise::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string id_str{std::to_string(id)};
  const std::string uv{input_vars.at(0)};
  const std::string permutation_var{"perlin_p_n" + id_str};
  const std::string val_var{"perlin_val_n" + id_str};
  const std::string out_var{"perlin_out_n" + id_str};

  std::string code;

  code += std::string("\t") + "// Perlin Noise" + std::string("\n");
  code += std::string("\t") + "const int " + permutation_var + "[256] = " +
          to_glsl_perlin_permutation(build_perlin_permutation(seed)) + ";" + std::string("\n");
  code += std::string("\t") + "float " + val_var + " = 0.0;" + std::string("\n");

  // Sum the "Start Band" .. "End Band" octaves the same way @c PatternPerlinNoise._valueRGBA does. The
  // frequency is doubled per band while the amplitude follows the spectral weights (persistence^band).
  float freq{1.0f};
  for (int i{1}; i < start_band; ++i) freq *= 2.0f;

  for (int band{start_band}; band <= end_band; ++band) {
    const float mult{static_cast<float>(std::pow(static_cast<double>(persistence),
                                                 static_cast<double>(band - start_band)))};
    const std::string position{"vec3(" + uv + ".x * " + to_glsl_float(freq * scale_x) + ", " + uv + ".y * " +
                               to_glsl_float(freq * scale_y) + ", 0.0)"};

    if (periodic) {
      const int period_x{std::max(1, static_cast<int>(freq * scale_x))};
      const int period_y{std::max(1, static_cast<int>(freq * scale_y))};

      code += std::string("\t") + val_var + " += perlin_sample3d_periodic(" + permutation_var + ", " + position +
              ", " + std::to_string(period_x) + ", " + std::to_string(period_y) + ", 256) * " +
              to_glsl_float(mult) + ";" + std::string("\n");
    } else {
      code += std::string("\t") + val_var + " += perlin_sample3d(" + permutation_var + ", " + position + ", 1.0) * " +
              to_glsl_float(mult) + ";" + std::string("\n");
    }

    freq *= 2.0f;
  }

  code += std::string("\t") + val_var + " = " + val_var + " * 0.5 + 0.5;" + std::string("\n");
  code += std::string("\t") + val_var + " *= " + to_glsl_float(value_scale) + ";" + std::string("\n");
  code += std::string("\t") + val_var + " = clamp(" + val_var + ", 0.0, 1.0);" + std::string("\n");
  code += emit_gradient_lookup(gradient, out_var, val_var, "perlin_n" + id_str);
  code += std::string("\t") + output_vars.at(0) + " = " + out_var + ";" + std::string("\n");

  return code;
}

/*************************************/
/* Voronoi (Worley) Noise            */
/*************************************/

std::string VisualShaderNodeGeneratorVoronoiNoise::generate_global([[maybe_unused]] const int& id) const {
    std::string code;

  code += "vec2 voronoi_noise_random_vector(vec2 uv, float offset) {" + std::string("\n");
  code += std::string("\t") + "mat2 m = mat2(15.27, 47.63, 99.41, 89.98);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "uv = fract(sin(m * uv) * 46839.32);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "return vec2(sin(uv.y*+offset)*0.5+0.5, cos(uv.x*offset)*0.5+0.5);" + std::string("\n");
  code += std::string("}") + "\n\n";

  code +=
      "void generate_voronoi_noise_float(vec2 uv, float angle_offset, float cell_density, out float out_buffer, out "
      "float cells) {" +
      std::string("\n");
  code += std::string("\t") + "vec2 g = floor(uv * cell_density);" + std::string("\n");
  code += std::string("\t") + "vec2 f = fract(uv * cell_density);" + std::string("\n");
  code += std::string("\t") + "float t = 8.0;" + std::string("\n");
  code += std::string("\t") + "vec3 res = vec3(8.0, 0.0, 0.0);" + std::string("\n");
  code += std::string("\t") + std::string("\n");
  code += std::string("\t") + "for(int y=-1; y<=1; y++) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "for(int x=-1; x<=1; x++) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + "vec2 lattice = vec2(x,y);" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") +
          "vec2 offset = voronoi_noise_random_vector(lattice + g, angle_offset);" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + "float d = distance(lattice + offset, f);" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + "if(d < res.x) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + std::string("\t") +
          "res = vec3(d, offset.x, offset.y);" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + std::string("\t") + "out_buffer = res.x;" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + std::string("\t") + "cells = res.y;" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + std::string("}") + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("}") + std::string("\n");
  code += std::string("\t") + std::string("}") + std::string("\n");
  code += std::string("}") + "\n\n";

  return code;
}

std::string VisualShaderNodeGeneratorVoronoiNoise::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(6) << angle_offset;
  std::ostringstream oss2;
  oss2 << std::fixed << std::setprecision(6) << cell_density;

  std::string code;

  code += std::string("\t") + "// Voronoi Noise" + std::string("\n");
  code += std::string("\t") + "float out_buffer_n" + std::to_string(id) + " = 0.0;" + std::string("\n");
  code += std::string("\t") + "float cells_n" + std::to_string(id) + " = 0.0; // TODO: How we can use this?" + std::string("\n");
  code += std::string("\t") + "generate_voronoi_noise_float(" + input_vars[0] + ", " + oss.str() + ", " + oss2.str() +
          ", out_buffer_n" + std::to_string(id) + ", cells_n" + std::to_string(id) + ");" + std::string("\n");
  code += std::string("\t") + output_vars[0] + " = vec4(out_buffer_n" + std::to_string(id) + ", out_buffer_n" + std::to_string(id) + ", out_buffer_n" + std::to_string(id) + ", 1.0);" + std::string("\n");
  code += std::string("\t") + std::string("\n");

  return code;
}

