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

#ifndef ENIGMA_VISUAL_SHADER_NODE_PATTERN_GENERATORS_HPP
#define ENIGMA_VISUAL_SHADER_NODE_PATTERN_GENERATORS_HPP

#include <string>
#include <vector>

#include "gui/model/schema/visual_shader_nodes.pb.h"
#include "generator/visual_shader_node_generators.hpp"

/*************************************/
/* Gradient                          */
/*************************************/

class VisualShaderNodeGeneratorGradient : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorGradient(const VisualShaderNodeGradient::VisualShaderNodeGradientType& gradient_type,
                                    const std::vector<VisualShaderColorGradientStop>& gradient,
                                    const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        gradient_type(gradient_type),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeGradient::VisualShaderNodeGradientType gradient_type;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Checker                           */
/*************************************/

class VisualShaderNodeGeneratorChecker : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorChecker(const float& scale_x, const float& scale_y,
                                   const std::vector<VisualShaderColorGradientStop>& gradient,
                                   const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        scale_x(scale_x),
        scale_y(scale_y),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float scale_x;
  const float scale_y;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Brick                             */
/*************************************/

class VisualShaderNodeGeneratorBrick : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorBrick(const int& num_x, const int& num_y, const float& shift, const float& rand_shift,
                                 const float& gap_x, const float& gap_y, const float& smooth, const bool& rand_color,
                                 const std::vector<VisualShaderColorGradientStop>& gradient,
                                 const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        num_x(num_x),
        num_y(num_y),
        shift(shift),
        rand_shift(rand_shift),
        gap_x(gap_x),
        gap_y(gap_y),
        smooth(smooth),
        rand_color(rand_color),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const int num_x;
  const int num_y;
  const float shift;
  const float rand_shift;
  const float gap_x;
  const float gap_y;
  const float smooth;
  const bool rand_color;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Tile                              */
/*************************************/

class VisualShaderNodeGeneratorTile : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorTile(const float& border_x, const float& border_y, const float& smooth,
                                const std::vector<VisualShaderColorGradientStop>& gradient,
                                const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        border_x(border_x),
        border_y(border_y),
        smooth(smooth),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float border_x;
  const float border_y;
  const float smooth;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Function                          */
/*************************************/

class VisualShaderNodeGeneratorFunction : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorFunction(const VisualShaderNodeFunction::VisualShaderNodeFunctionFn& function_u,
                                    const VisualShaderNodeFunction::VisualShaderNodeFunctionFn& function_v,
                                    const VisualShaderNodeFunction::VisualShaderNodeFunctionCombiner& combiner,
                                    const float& scale_x, const float& scale_y,
                                    const std::vector<VisualShaderColorGradientStop>& gradient,
                                    const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        function_u(function_u),
        function_v(function_v),
        combiner(combiner),
        scale_x(scale_x),
        scale_y(scale_y),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeFunction::VisualShaderNodeFunctionFn function_u;
  const VisualShaderNodeFunction::VisualShaderNodeFunctionFn function_v;
  const VisualShaderNodeFunction::VisualShaderNodeFunctionCombiner combiner;
  const float scale_x;
  const float scale_y;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Cellular                          */
/*************************************/

class VisualShaderNodeGeneratorCellular : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorCellular(
      const VisualShaderNodeCellular::VisualShaderNodeCellularCellType& cell_type,
      const VisualShaderNodeCellular::VisualShaderNodeCellularDistance& distance,
      const VisualShaderNodeCellular::VisualShaderNodeCellularPointGen& point_gen, const int& seed,
      const float& intensity, const float& jitter, const int& num_points, const bool& rand_color,
      const bool& periodic, const std::vector<VisualShaderColorGradientStop>& gradient,
      const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        cell_type(cell_type),
        distance(distance),
        point_gen(point_gen),
        seed(seed),
        intensity(intensity),
        jitter(jitter),
        num_points(num_points),
        rand_color(rand_color),
        periodic(periodic),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const VisualShaderNodeCellular::VisualShaderNodeCellularCellType cell_type;
  const VisualShaderNodeCellular::VisualShaderNodeCellularDistance distance;
  const VisualShaderNodeCellular::VisualShaderNodeCellularPointGen point_gen;
  const int seed;
  const float intensity;
  const float jitter;
  const int num_points;
  const bool rand_color;
  const bool periodic;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Value (Simple) Noise              */
/*************************************/

class VisualShaderNodeGeneratorValueNoise : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorValueNoise(const float& scale,
                                      const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(), scale(scale), transformation(transformation) {}

  virtual std::string generate_global_per_node([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float scale;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

/*************************************/
/* Perlin (Gradient) Noise           */
/*************************************/

class VisualShaderNodeGeneratorPerlinNoise : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorPerlinNoise(const float& scale_x, const float& scale_y, const float& value_scale,
                                       const float& persistence, const int& start_band, const int& end_band,
                                       const int& seed, const bool& periodic,
                                       const std::vector<VisualShaderColorGradientStop>& gradient,
                                       const Matrix3x3& transformation = make_identity_transformation())
      : VisualShaderNodeGenerator(),
        scale_x(scale_x),
        scale_y(scale_y),
        value_scale(value_scale),
        persistence(persistence),
        start_band(start_band),
        end_band(end_band),
        seed(seed),
        periodic(periodic),
        gradient(gradient),
        transformation(transformation) {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

 private:
  const float scale_x;
  const float scale_y;
  const float value_scale;
  const float persistence;
  const int start_band;
  const int end_band;
  const int seed;
  const bool periodic;
  const std::vector<VisualShaderColorGradientStop> gradient;
  // The node's coordinate transformation (a Matrix3x3 field of the node type message).
  const Matrix3x3 transformation;
};

#endif  // ENIGMA_VISUAL_SHADER_NODE_PATTERN_GENERATORS_HPP
