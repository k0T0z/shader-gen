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
#include <cstdint>
#include <iomanip>
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

/**
 * @brief A minimal re-implementation of @c java.util.Random used to bake the Brick pseudo-random values.
 *
 * Only @c setSeed(long) and @c nextFloat() are needed to reproduce the exact arrays the source
 * implementation generates.
 */
class JavaRandom {
 public:
  explicit JavaRandom(const std::uint64_t& seed) { set_seed(seed); }

  void set_seed(const std::uint64_t& seed) { state = (seed ^ MULTIPLIER) & MASK; }

  float next_float() { return static_cast<float>(next(24)) / static_cast<float>(1u << 24); }

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
  JavaRandom colors_random{3123};
  for (float& color : colors) color = colors_random.next_float();

  std::vector<float> shifts(static_cast<std::size_t>(grid_y));
  JavaRandom shifts_random{31232};
  for (float& shift_value : shifts) shift_value = shifts_random.next_float();

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
