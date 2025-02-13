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

#ifndef ENIGMA_VISUAL_SHADER_NODE_NOISE_GENERATORS_HPP
#define ENIGMA_VISUAL_SHADER_NODE_NOISE_GENERATORS_HPP

#include <string>
#include <vector>

#include "gui/model/schema/visual_shader_nodes.pb.h"
#include "generator/visual_shader_node_generators.hpp"

/*************************************/
/* Value (Simple) Noise              */
/*************************************/

class VisualShaderNodeGeneratorValueNoise : public VisualShaderNodeGenerator {
    public:
    VisualShaderNodeGeneratorValueNoise(const float& scale) : VisualShaderNodeGenerator(), scale(scale) {}

    virtual std::string generate_global([[maybe_unused]] const int& id) const override;

    virtual std::string generate_code([[maybe_unused]] const int& id,
                                      [[maybe_unused]] const std::vector<std::string>& input_vars,
                                      [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

    private:
    const float scale;
};

/*************************************/
/* Perlin (Gradient) Noise           */
/*************************************/

class VisualShaderNodeGeneratorPerlinNoise : public VisualShaderNodeGenerator {
    public:
    VisualShaderNodeGeneratorPerlinNoise(const float& scale) : VisualShaderNodeGenerator(), scale(scale) {}

    virtual std::string generate_global([[maybe_unused]] const int& id) const override;

    virtual std::string generate_code([[maybe_unused]] const int& id,
                                      [[maybe_unused]] const std::vector<std::string>& input_vars,
                                      [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

    private:
    const float scale;
};

/*************************************/
/* Voronoi (Worley) Noise            */
/*************************************/

class VisualShaderNodeGeneratorVoronoiNoise : public VisualShaderNodeGenerator {
    public:
    VisualShaderNodeGeneratorVoronoiNoise(const float& angle_offset, const float& cell_density)
        : VisualShaderNodeGenerator(), angle_offset(angle_offset), cell_density(cell_density) {}

    virtual std::string generate_global([[maybe_unused]] const int& id) const override;

    virtual std::string generate_code([[maybe_unused]] const int& id,
                                      [[maybe_unused]] const std::vector<std::string>& input_vars,
                                      [[maybe_unused]] const std::vector<std::string>& output_vars) const override;

    private:
    const float angle_offset;
    const float cell_density;
};

#endif  // ENIGMA_VISUAL_SHADER_NODE_NOISE_GENERATORS_HPP
