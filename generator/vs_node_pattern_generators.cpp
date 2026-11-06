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

#include "generator/vs_node_pattern_generators.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>

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

std::string to_glsl_array(const std::vector<float>& values) {
  std::string code{"float[" + std::to_string(values.size()) + "]("};
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0) code += ", ";
    code += to_glsl_float(values.at(i));
  }
  code += ")";
  return code;
}

std::string to_glsl_vec2_array(const std::vector<std::array<float, 2>>& values) {
  std::string code{"vec2[" + std::to_string(values.size()) + "]("};
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0) code += ", ";
    code += "vec2(" + to_glsl_float(values.at(i).at(0)) + ", " + to_glsl_float(values.at(i).at(1)) + ")";
  }
  code += ")";
  return code;
}

std::string to_glsl_int_array(const std::vector<int>& values) {
  std::string code{"int[" + std::to_string(values.size()) + "]("};
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0) code += ", ";
    code += std::to_string(values.at(i));
  }
  code += ")";
  return code;
}

/**
 * @brief A re-implementation of @c FMath.radicalInverse_vdC (van der Corput radical inverse).
 */
float radical_inverse_vdc(const int& base, int i) {
  double digit{1.0 / static_cast<double>(base)};
  double radical{digit};
  double inverse{0.0};
  while (i != 0) {
    inverse += digit * static_cast<double>(i % base);
    digit *= radical;
    i /= base;
  }
  return static_cast<float>(inverse);
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
 * @brief Build the Improved Perlin permutation table for a seed.
 *
 * A negative seed uses Ken Perlin's reference permutation, otherwise the identity permutation is
 * uniformly shuffled with a Mersenne Twister seeded by @p seed.
 */
std::array<int, 256> build_perlin_permutation(const int& seed) {
  std::array<int, 256> p{};

  if (seed < 0) {
    p = perlin_reference_permutation();
    return p;
  }

  for (int i{0}; i < 256; ++i) p.at(static_cast<std::size_t>(i)) = i;

  std::mt19937 generator{static_cast<std::mt19937::result_type>(seed)};
  std::shuffle(p.begin(), p.end(), generator);

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
/* Gradient                          */
/*************************************/

std::string VisualShaderNodeGeneratorGradient::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string uv{"pattern_uv_n" + std::to_string(id)};
  const std::string pos{"pattern_pos_n" + std::to_string(id)};
  const std::string out{"pattern_out_n" + std::to_string(id)};

  std::string code;

  code += std::string("\t") + "// Gradient" + std::string("\n");
  code += std::string("\t") + "vec2 " + uv + " = fract(" + input_vars.at(0) + ");" + std::string("\n");
  code += std::string("\t") + "float " + pos + " = 0.0;" + std::string("\n");

  switch (gradient_type) {
    case VisualShaderNodeGradient::GRADIENT_LINEAR: {
      code += std::string("\t") + "if (" + uv + ".y <= 0.5) {" + std::string("\n");
      code += std::string("\t") + std::string("\t") + pos + " = " + uv + ".y * 2.0;" + std::string("\n");
      code += std::string("\t") + "} else {" + std::string("\n");
      code += std::string("\t") + std::string("\t") + pos + " = (1.0 - " + uv + ".y) * 2.0;" + std::string("\n");
      code += std::string("\t") + "}" + std::string("\n");
      break;
    }
    case VisualShaderNodeGradient::GRADIENT_RADIAL: {
      code += std::string("\t") + "{" + std::string("\n");
      code += std::string("\t") + std::string("\t") + "float pattern_grad_x_n" + std::to_string(id) + " = 0.5 - " +
              uv + ".x;" + std::string("\n");
      code += std::string("\t") + std::string("\t") + "float pattern_grad_y_n" + std::to_string(id) + " = 0.5 - " +
              uv + ".y;" + std::string("\n");
      code += std::string("\t") + std::string("\t") + pos + " = 2.0 * (0.5 - sqrt(pattern_grad_x_n" +
              std::to_string(id) + " * pattern_grad_x_n" + std::to_string(id) + " + pattern_grad_y_n" +
              std::to_string(id) + " * pattern_grad_y_n" + std::to_string(id) + "));" + std::string("\n");
      code += std::string("\t") + "}" + std::string("\n");
      break;
    }
    case VisualShaderNodeGradient::GRADIENT_SQUARE: {
      code += std::string("\t") + pos + " = 1.0 - 2.0 * max(abs(0.5 - " + uv + ".x), abs(0.5 - " + uv + ".y));" +
              std::string("\n");
      break;
    }
    default:
      break;
  }

  code += emit_gradient_lookup(gradient, out, pos, "pattern_n" + std::to_string(id));
  code += std::string("\t") + output_vars.at(0) + " = " + out + ";" + std::string("\n");

  return code;
}

/*************************************/
/* Checker                           */
/*************************************/

std::string VisualShaderNodeGeneratorChecker::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string uv{"pattern_uv_n" + std::to_string(id)};
  const std::string pos{"pattern_pos_n" + std::to_string(id)};
  const std::string out{"pattern_out_n" + std::to_string(id)};
  const std::string checker{"pattern_checker_n" + std::to_string(id)};

  std::string code;

  code += std::string("\t") + "// Checker" + std::string("\n");
  code += std::string("\t") + "vec2 " + uv + " = fract(" + input_vars.at(0) + ");" + std::string("\n");
  code += std::string("\t") + "int " + checker + " = (int(floor(" + uv + ".x * " + to_glsl_float(scale_x) +
          " * 2.0)) + int(floor(" + uv + ".y * " + to_glsl_float(scale_y) + " * 2.0))) & 1;" + std::string("\n");
  code += std::string("\t") + "float " + pos + " = " + checker + " == 0 ? 1.0 : 0.0;" + std::string("\n");

  code += emit_gradient_lookup(gradient, out, pos, "pattern_n" + std::to_string(id));
  code += std::string("\t") + output_vars.at(0) + " = " + out + ";" + std::string("\n");

  return code;
}

/*************************************/
/* Brick                             */
/*************************************/

std::string VisualShaderNodeGeneratorBrick::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  // The source implementation indexes the baked arrays with the actual grid dimensions. Clamping them to at
  // least one keeps the generated code valid (and in bounds) even for degenerate zero sized grids.
  const int grid_x{std::max(1, num_x)};
  const int grid_y{std::max(1, num_y)};

  std::vector<float> colors(static_cast<std::size_t>(grid_x) * static_cast<std::size_t>(grid_y));
  {
    std::mt19937 generator{3123u};
    std::uniform_real_distribution<float> distribution{0.0f, 1.0f};
    for (float& color : colors) color = distribution(generator);
  }

  std::vector<float> shifts(static_cast<std::size_t>(grid_y));
  {
    std::mt19937 generator{31232u};
    std::uniform_real_distribution<float> distribution{0.0f, 1.0f};
    for (float& shift_value : shifts) shift_value = distribution(generator);
  }

  const std::string uv{"pattern_uv_n" + std::to_string(id)};
  const std::string out{"pattern_out_n" + std::to_string(id)};
  const std::string val{"pattern_val_n" + std::to_string(id)};
  const std::string colors_array{"pattern_colors_n" + std::to_string(id)};
  const std::string shifts_array{"pattern_shifts_n" + std::to_string(id)};
  const std::string norm_x{"pattern_norm_x_n" + std::to_string(id)};
  const std::string norm_y{"pattern_norm_y_n" + std::to_string(id)};
  const std::string uu{"pattern_uu_n" + std::to_string(id)};
  const std::string vv{"pattern_vv_n" + std::to_string(id)};
  const std::string y{"pattern_y_n" + std::to_string(id)};
  const std::string by{"pattern_by_n" + std::to_string(id)};
  const std::string bx{"pattern_bx_n" + std::to_string(id)};
  const std::string x{"pattern_x_n" + std::to_string(id)};
  const std::string inside{"pattern_inside_n" + std::to_string(id)};
  const std::string dist{"pattern_dist_n" + std::to_string(id)};

  const std::string shift_s{to_glsl_float(shift)};
  const std::string rand_shift_s{to_glsl_float(rand_shift)};
  const std::string gap_x_s{to_glsl_float(gap_x)};
  const std::string gap_y_s{to_glsl_float(gap_y)};
  const std::string smooth_s{to_glsl_float(smooth)};

  std::string code;

  code += std::string("\t") + "// Brick" + std::string("\n");
  code += std::string("\t") + "const float " + colors_array + "[" + std::to_string(colors.size()) + "] = " +
          to_glsl_array(colors) + ";" + std::string("\n");
  code += std::string("\t") + "const float " + shifts_array + "[" + std::to_string(shifts.size()) + "] = " +
          to_glsl_array(shifts) + ";" + std::string("\n");
  code += std::string("\t") + "vec2 " + uv + " = fract(" + input_vars.at(0) + ");" + std::string("\n");
  code += std::string("\t") + "float " + norm_x + " = " + to_glsl_float(static_cast<float>(num_x)) + ";" +
          std::string("\n");
  code += std::string("\t") + "float " + norm_y + " = " + to_glsl_float(static_cast<float>(num_y)) + ";" +
          std::string("\n");
  code += std::string("\t") + "float " + uu + " = " + uv + ".x * " + norm_x + ";" + std::string("\n");
  code += std::string("\t") + "float " + vv + " = " + uv + ".y * " + norm_y + ";" + std::string("\n");
  code += std::string("\t") + "float " + y + " = " + vv + " - floor(" + vv + ");" + std::string("\n");
  code += std::string("\t") + "if (" + vv + " >= " + norm_y + ") " + vv + " -= " + norm_y + ";" + std::string("\n");
  code += std::string("\t") + "if (" + uu + " >= " + norm_x + ") " + uu + " -= " + norm_x + ";" + std::string("\n");
  code += std::string("\t") + "int " + by + " = int(floor(" + vv + "));" + std::string("\n");
  code += std::string("\t") + "if (" + vv + " * 0.5 - floor(" + vv + " * 0.5) >= 0.5) " + uu + " += " + shift_s + ";" +
          std::string("\n");
  code += std::string("\t") + uu + " += " + shifts_array + "[" + by + "] * " + rand_shift_s + ";" + std::string("\n");
  code += std::string("\t") + "if (" + uu + " >= " + norm_x + ") " + uu + " -= " + norm_x + ";" + std::string("\n");
  code += std::string("\t") + "int " + bx + " = int(floor(" + uu + "));" + std::string("\n");
  code += std::string("\t") + "float " + x + " = " + uu + " - floor(" + uu + ");" + std::string("\n");
  code += std::string("\t") + "float " + val + " = 1.0;" + std::string("\n");

  if (rand_color) {
    code += std::string("\t") + val + " = " + colors_array + "[" + bx + " + " + by + " * " + std::to_string(grid_x) +
            "];" + std::string("\n");
  }

  code += std::string("\t") + "bool " + inside + " = true;" + std::string("\n");

  if (gap_x > 0.0f) {
    code += std::string("\t") + inside + " = " + inside + " && (" + x + " > " + gap_x_s + " * " + norm_x + ");" +
            std::string("\n");
  }
  if (gap_y > 0.0f) {
    code += std::string("\t") + inside + " = " + inside + " && (" + y + " > " + gap_y_s + " * " + norm_y + ");" +
            std::string("\n");
  }

  code += std::string("\t") + "if (" + inside + ") {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float " + dist + " = min(min(" + x + " - " + gap_x_s + " * " +
          norm_x + ", 1.0 - " + x + ") / " + norm_x + ", min(" + y + " - " + gap_y_s + " * " + norm_y + ", 1.0 - " +
          y + ") / " + norm_y + ");" + std::string("\n");
  code += std::string("\t") + std::string("\t") + dist + " *= min(" + norm_x + ", " + norm_y + ");" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "if (" + dist + " < " + smooth_s + ") {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + val + " = " + val + " * " + dist + " / " +
          smooth_s + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "} else {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + val + " = 0.0;" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");

  code += emit_gradient_lookup(gradient, out, val, "pattern_n" + std::to_string(id));
  code += std::string("\t") + output_vars.at(0) + " = " + out + ";" + std::string("\n");

  return code;
}

/*************************************/
/* Tile                              */
/*************************************/

std::string VisualShaderNodeGeneratorTile::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string uv{"pattern_uv_n" + std::to_string(id)};
  const std::string pos{"pattern_pos_n" + std::to_string(id)};
  const std::string out{"pattern_out_n" + std::to_string(id)};
  const std::string dist_u{"pattern_dist_u_n" + std::to_string(id)};
  const std::string dist_v{"pattern_dist_v_n" + std::to_string(id)};
  const std::string dist{"pattern_dist_n" + std::to_string(id)};

  const std::string border_x_s{to_glsl_float(border_x)};
  const std::string border_y_s{to_glsl_float(border_y)};
  const std::string border_x_inv_s{to_glsl_float(1.0f - border_x)};
  const std::string border_y_inv_s{to_glsl_float(1.0f - border_y)};
  const std::string smooth_s{to_glsl_float(smooth)};

  std::string code;

  code += std::string("\t") + "// Tile" + std::string("\n");
  code += std::string("\t") + "vec2 " + uv + " = fract(" + input_vars.at(0) + ");" + std::string("\n");
  code += std::string("\t") + "float " + pos + " = 0.0;" + std::string("\n");
  code += std::string("\t") + "if (" + uv + ".x < " + border_x_s + " || " + uv + ".x > " + border_x_inv_s + " || " +
          uv + ".y < " + border_y_s + " || " + uv + ".y > " + border_y_inv_s + ") {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + pos + " = 0.0;" + std::string("\n");
  code += std::string("\t") + "} else {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float " + dist_u + " = min(" + uv + ".x - " + border_x_s +
          ", 1.0 - " + uv + ".x - " + border_x_s + ");" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float " + dist_v + " = min(" + uv + ".y - " + border_y_s +
          ", 1.0 - " + uv + ".y - " + border_y_s + ");" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float " + dist + " = min(" + dist_u + ", " + dist_v + ");" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + "if (" + dist + " < " + smooth_s + ") {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + pos + " = " + dist + " / " + smooth_s + ";" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + "} else {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + pos + " = 1.0;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");

  code += emit_gradient_lookup(gradient, out, pos, "pattern_n" + std::to_string(id));
  code += std::string("\t") + output_vars.at(0) + " = " + out + ";" + std::string("\n");

  return code;
}

/*************************************/
/* Function                          */
/*************************************/

std::string VisualShaderNodeGeneratorFunction::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  const std::string uv{"pattern_uv_n" + std::to_string(id)};
  const std::string su{"pattern_su_n" + std::to_string(id)};
  const std::string sv{"pattern_sv_n" + std::to_string(id)};
  const std::string val{"pattern_val_n" + std::to_string(id)};
  const std::string out{"pattern_out_n" + std::to_string(id)};

  const auto emit_function = [&id](const VisualShaderNodeFunction::VisualShaderNodeFunctionFn& function,
                                   const float& scale, const std::string& coord, const std::string& out_var,
                                   const std::string& tmp_var) -> std::string {
    std::string code;
    switch (function) {
      case VisualShaderNodeFunction::FUNC_SIN: {
        code += std::string("\t") + "float " + out_var + " = cos(3.141592653589793 * 2.0 * " +
                to_glsl_float(scale) + " * (" + coord + " + 0.5)) * 0.5 + 0.5;" + std::string("\n");
        break;
      }
      case VisualShaderNodeFunction::FUNC_SAW: {
        code += std::string("\t") + "float " + tmp_var + " = " + to_glsl_float(scale) + " * " + coord + ";" +
                std::string("\n");
        code += std::string("\t") + tmp_var + " = " + tmp_var + " - float(int(" + tmp_var + "));" + std::string("\n");
        code += std::string("\t") + "float " + out_var + " = " + tmp_var + " > 0.5 ? 2.0 * (1.0 - " + tmp_var +
                ") : 2.0 * " + tmp_var + ";" + std::string("\n");
        break;
      }
      case VisualShaderNodeFunction::FUNC_SQUARE: {
        code += std::string("\t") + "float " + tmp_var + " = " + to_glsl_float(scale) + " * " + coord + ";" +
                std::string("\n");
        code += std::string("\t") + tmp_var + " = " + tmp_var + " - float(int(" + tmp_var + "));" + std::string("\n");
        code += std::string("\t") + "float " + out_var + " = " + tmp_var + " > 0.5 ? 0.0 : 1.0;" + std::string("\n");
        break;
      }
      default: {
        code += std::string("\t") + "float " + out_var + " = 0.0;" + std::string("\n");
        break;
      }
    }
    return code;
  };

  std::string code;

  code += std::string("\t") + "// Function" + std::string("\n");
  code += std::string("\t") + "vec2 " + uv + " = fract(" + input_vars.at(0) + ");" + std::string("\n");
  code += emit_function(function_u, scale_x, uv + ".x", su, "pattern_fu_n" + std::to_string(id));
  code += emit_function(function_v, scale_y, uv + ".y", sv, "pattern_fv_n" + std::to_string(id));

  std::string val_expr;
  switch (combiner) {
    case VisualShaderNodeFunction::COMBINE_MULTIPLY:
      val_expr = su + " * " + sv;
      break;
    case VisualShaderNodeFunction::COMBINE_ADD:
      val_expr = "(" + su + " + " + sv + ") * 0.5";
      break;
    case VisualShaderNodeFunction::COMBINE_MAX:
      val_expr = "max(" + su + ", " + sv + ")";
      break;
    case VisualShaderNodeFunction::COMBINE_MIN:
      val_expr = "min(" + su + ", " + sv + ")";
      break;
    case VisualShaderNodeFunction::COMBINE_POW:
      val_expr = "pow(" + su + ", " + sv + ")";
      break;
    default:
      val_expr = "0.0";
      break;
  }

  code += std::string("\t") + "float " + val + " = " + val_expr + ";" + std::string("\n");

  code += emit_gradient_lookup(gradient, out, val, "pattern_n" + std::to_string(id));
  code += std::string("\t") + output_vars.at(0) + " = " + out + ";" + std::string("\n");

  return code;
}

/*************************************/
/* Cellular                          */
/*************************************/

std::string VisualShaderNodeGeneratorCellular::generate_code(
    [[maybe_unused]] const int& id, [[maybe_unused]] const std::vector<std::string>& input_vars,
    [[maybe_unused]] const std::vector<std::string>& output_vars) const {
  // The source implementation needs at least two points to evaluate the "distance to the k-th nearest
  // feature point" cell types, so clamp the requested amount to that lower bound.
  const int point_count{std::max(2, num_points)};
  const float sqrt_point_count{std::sqrt(static_cast<float>(point_count))};

  // Bake the feature points similarly to @c PatternCellular.regeneratePoints. The generator is re-created
  // for each phase (points and colors) so that both phases start from the same seed.
  std::mt19937 points_generator{static_cast<std::mt19937::result_type>(seed)};
  std::uniform_real_distribution<float> points_distribution{0.0f, 1.0f};
  std::vector<std::array<float, 2>> base_points;
  base_points.reserve(static_cast<std::size_t>(point_count));

  for (int i{0}; i < point_count; ++i) {
    const float jitter_x{(points_distribution(points_generator) - 0.5f) * jitter / sqrt_point_count};
    const float jitter_y{(points_distribution(points_generator) - 0.5f) * jitter / sqrt_point_count};

    float x{0.0f};
    float y{0.0f};

    switch (point_gen) {
      case VisualShaderNodeCellular::POINT_GEN_RANDOM: {
        x = points_distribution(points_generator) + jitter_x;
        y = points_distribution(points_generator) + jitter_y;
        break;
      }
      case VisualShaderNodeCellular::POINT_GEN_REGULAR: {
        const int mod{static_cast<int>(sqrt_point_count + 0.999f)};
        x = static_cast<float>(i % mod) / static_cast<float>(mod) + jitter_x;
        y = static_cast<float>(i / mod) / static_cast<float>(mod) + jitter_y;
        break;
      }
      case VisualShaderNodeCellular::POINT_GEN_HALTON23: {
        x = radical_inverse_vdc(2, i) + jitter_x;
        y = radical_inverse_vdc(3, i) + jitter_y;
        break;
      }
      case VisualShaderNodeCellular::POINT_GEN_HAMMERSLEY: {
        const float div{1.0f / static_cast<float>(point_count)};
        x = static_cast<float>(i) * div + jitter_x;
        y = radical_inverse_vdc(2, i) + jitter_y;
        break;
      }
      default: {
        break;
      }
    }

    base_points.push_back({x, y});
  }

  std::mt19937 colors_generator{static_cast<std::mt19937::result_type>(seed)};
  std::uniform_real_distribution<float> colors_distribution{0.0f, 1.0f};
  std::vector<float> random_colors(static_cast<std::size_t>(point_count));
  for (float& color : random_colors) color = colors_distribution(colors_generator);

  // For kd tree lookup the source replicates the points onto a torus to obtain a periodic pattern. The
  // replicas keep the index of the original point so that @c randomColors lookups stay consistent.
  std::vector<std::array<float, 2>> points{base_points};
  std::vector<int> indices;
  indices.reserve(base_points.size());
  for (int i{0}; i < point_count; ++i) indices.push_back(i);

  if (periodic) {
    for (int i{0}; i < point_count; ++i) {
      const float px{base_points.at(static_cast<std::size_t>(i)).at(0)};
      const float py{base_points.at(static_cast<std::size_t>(i)).at(1)};
      const float nx{px < 0.5f ? px + 1.0f : px - 1.0f};
      const float ny{py < 0.5f ? py + 1.0f : py - 1.0f};

      points.push_back({nx, py});
      indices.push_back(i);
      points.push_back({px, ny});
      indices.push_back(i);
      points.push_back({nx, ny});
      indices.push_back(i);
    }
  }

  const std::string id_str{std::to_string(id)};
  const std::string uv{"cell_uv_n" + id_str};
  const std::string out{"cell_out_n" + id_str};
  const std::string val{"cell_val_n" + id_str};
  const std::string points_var{"cell_points_n" + id_str};
  const std::string indices_var{"cell_idx_n" + id_str};
  const std::string colors_var{"cell_colors_n" + id_str};
  const std::string nearest_var{"cell_nearest_n" + id_str};
  const std::string d1{"cell_d1_n" + id_str};
  const std::string d2{"cell_d2_n" + id_str};
  const std::string d3{"cell_d3_n" + id_str};
  const std::string i1{"cell_i1_n" + id_str};
  const std::string i2{"cell_i2_n" + id_str};
  const std::string i3{"cell_i3_n" + id_str};

  std::string metric;
  switch (distance) {
    case VisualShaderNodeCellular::DIST_MANHATTAN: {
      metric = "abs(cell_dx_" + id_str + ") + abs(cell_dy_" + id_str + ")";
      break;
    }
    case VisualShaderNodeCellular::DIST_MAX: {
      metric = "max(abs(cell_dx_" + id_str + "), abs(cell_dy_" + id_str + "))";
      break;
    }
    case VisualShaderNodeCellular::DIST_MINKOWSKI: {
      metric = "pow(sqrt(abs(cell_dx_" + id_str + ")) + sqrt(abs(cell_dy_" + id_str + ")), 2.0)";
      break;
    }
    case VisualShaderNodeCellular::DIST_EUCLID:
    default: {
      metric = "sqrt(cell_dx_" + id_str + " * cell_dx_" + id_str + " + cell_dy_" + id_str + " * cell_dy_" + id_str +
               ")";
      break;
    }
  }

  std::string code;

  code += std::string("\t") + "// Cellular" + std::string("\n");
  code += std::string("\t") + "const vec2 " + points_var + "[" + std::to_string(points.size()) + "] = " +
          to_glsl_vec2_array(points) + ";" + std::string("\n");
  code += std::string("\t") + "const int " + indices_var + "[" + std::to_string(indices.size()) + "] = " +
          to_glsl_int_array(indices) + ";" + std::string("\n");
  code += std::string("\t") + "const float " + colors_var + "[" + std::to_string(random_colors.size()) + "] = " +
          to_glsl_array(random_colors) + ";" + std::string("\n");
  code += std::string("\t") + "vec2 " + uv + " = fract(" + input_vars.at(0) + ");" + std::string("\n");
  code += std::string("\t") + "float " + d1 + " = 1e20;" + std::string("\n");
  code += std::string("\t") + "float " + d2 + " = 1e20;" + std::string("\n");
  code += std::string("\t") + "float " + d3 + " = 1e20;" + std::string("\n");
  code += std::string("\t") + "int " + i1 + " = 0;" + std::string("\n");
  code += std::string("\t") + "int " + i2 + " = 0;" + std::string("\n");
  code += std::string("\t") + "int " + i3 + " = 0;" + std::string("\n");
  code += std::string("\t") + "for (int i = 0; i < " + std::to_string(points.size()) + "; i++) {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float cell_dx_" + id_str + " = " + uv + ".x - " + points_var +
          "[i].x;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float cell_dy_" + id_str + " = " + uv + ".y - " + points_var +
          "[i].y;" + std::string("\n");
  code += std::string("\t") + std::string("\t") + "float cell_dist_" + id_str + " = " + metric + ";" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + "if (cell_dist_" + id_str + " < " + d1 + ") {" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + d3 + " = " + d2 + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + d2 + " = " + d1 + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + d1 + " = cell_dist_" + id_str + ";" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + i3 + " = " + i2 + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + i2 + " = " + i1 + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + i1 + " = " + indices_var + "[i];" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + "} else if (cell_dist_" + id_str + " < " + d2 + ") {" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + d3 + " = " + d2 + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + d2 + " = cell_dist_" + id_str + ";" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + i3 + " = " + i2 + ";" + std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + i2 + " = " + indices_var + "[i];" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + "} else if (cell_dist_" + id_str + " < " + d3 + ") {" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + d3 + " = cell_dist_" + id_str + ";" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + std::string("\t") + i3 + " = " + indices_var + "[i];" +
          std::string("\n");
  code += std::string("\t") + std::string("\t") + "}" + std::string("\n");
  code += std::string("\t") + "}" + std::string("\n");

  code += std::string("\t") + "float " + val + " = 0.0;" + std::string("\n");

  switch (cell_type) {
    case VisualShaderNodeCellular::CELL_F1: {
      code += std::string("\t") + val + " = " + d1 + ";" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(intensity) + ";" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(sqrt_point_count) + ";" + std::string("\n");
      code += std::string("\t") + val + " = clamp(" + val + ", 0.0, 1.0);" + std::string("\n");
      code += std::string("\t") + "int " + nearest_var + " = " + i1 + ";" + std::string("\n");
      break;
    }
    case VisualShaderNodeCellular::CELL_F2: {
      code += std::string("\t") + val + " = " + d2 + " / 1.4142;" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(intensity) + ";" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(sqrt_point_count) + ";" + std::string("\n");
      code += std::string("\t") + val + " = clamp(" + val + ", 0.0, 1.0);" + std::string("\n");
      code += std::string("\t") + "int " + nearest_var + " = " + i2 + ";" + std::string("\n");
      break;
    }
    case VisualShaderNodeCellular::CELL_F3: {
      code += std::string("\t") + val + " = " + d3 + " / 2.0;" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(intensity) + ";" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(sqrt_point_count) + ";" + std::string("\n");
      code += std::string("\t") + val + " = clamp(" + val + ", 0.0, 1.0);" + std::string("\n");
      code += std::string("\t") + "int " + nearest_var + " = " + i3 + ";" + std::string("\n");
      break;
    }
    case VisualShaderNodeCellular::CELL_F2_MINUS_F1: {
      code += std::string("\t") + val + " = abs(" + d1 + " - " + d2 + ") * 1.4142;" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(intensity) + ";" + std::string("\n");
      code += std::string("\t") + val + " *= " + to_glsl_float(sqrt_point_count) + ";" + std::string("\n");
      code += std::string("\t") + val + " = clamp(" + val + ", 0.0, 1.0);" + std::string("\n");
      code += std::string("\t") + "int " + nearest_var + " = " + i1 + ";" + std::string("\n");
      break;
    }
    case VisualShaderNodeCellular::CELL_CONSTANT:
    default: {
      code += std::string("\t") + val + " = " + to_glsl_float(intensity) + ";" + std::string("\n");
      code += std::string("\t") + "int " + nearest_var + " = " + i1 + ";" + std::string("\n");
      break;
    }
  }

  if (rand_color) {
    code += std::string("\t") + val + " *= " + colors_var + "[" + nearest_var + "];" + std::string("\n");
  }

  code += emit_gradient_lookup(gradient, out, val, "cell_n" + id_str);
  code += std::string("\t") + output_vars.at(0) + " = " + out + ";" + std::string("\n");

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
