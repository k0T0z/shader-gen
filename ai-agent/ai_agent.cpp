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

#include "ai-agent/ai_agent.hpp"

#include <algorithm>
#include <QMetaObject>
#include <QImage>

#include "ai-agent/utils/utils.hpp"
#include "generator/visual_shader_generator.hpp"
#include "ai-agent/fitness.hpp"

namespace ai_agent_main {
    
    /**
     * @brief 
     * 
     * @note Josh — 10/02/2025 20:36 "my recommendation for "how can we possibly guess 
     *       at a topology other than pure random" would be K-means clustering based on 
     *       fourier values"
     * 
     * @note Josh — 07/03/2025 21:26 "I think those are good heuristics, but what I would 
     *       probably do is start with a handful of topologies or even a few random nodes 
     *       of each kernel type, then just choose the ones that are the closest based on 
     *       that Fourier analysis"
     * 
     * @return std::vector<std::string>
     */
    bool init(
        const MatchingType& matching_type,
        const std::string& encoded_graph,
        const int& maximum_population_size, 
        std::vector<std::string>& population_buffer
    ) noexcept {
        population_buffer.clear();
        population_buffer.resize(maximum_population_size);

        switch (matching_type) {
            case MatchingType::PARAMETERS_ONLY: {
                const std::vector<std::string> entities{ai_agent_utils::split_string(encoded_graph, ',')};
                for (const auto& entity : entities) {
                    const std::vector<std::string> tokens{ai_agent_utils::split_string(entity, ';')};
                    const int entity_type = std::stoi(tokens.at(0));
                    if (entity_type == 0) {
                        // Node
                        // Format: entity_type;node_id;node_type;field_number=value;field_number=value;...
                        const int n_id = std::stoi(tokens.at(1));
                        const int oneof_value_field_number = std::stoi(tokens.at(2));
                        const std::vector<std::string> parameters{tokens.begin() + 3, tokens.end()};
                        for (int i{0}; i < maximum_population_size; ++i) {
                            std::vector<std::string> new_parameters;
                            new_parameters.resize(parameters.size());
                            for (int j{0}; j < parameters.size(); ++j) {
                                const std::string parameter{parameters.at(j)}; // Format: field_number=value
                                const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameter, '=')};
                                const int field_number = std::stoi(parameter_tokens.at(0));
                                const ai_agent_utils::DiscreteContinuousRangeVariant range{ai_agent_utils::get_range_for_field(oneof_value_field_number, field_number)};
                                std::string new_parameter;
                                new_parameter += std::to_string(field_number) + '=';
                                if (std::holds_alternative<std::vector<int>>(range)) {
                                    const std::vector<int> discrete_range{std::get<std::vector<int>>(range)};
                                    const int random_index{ai_agent_utils::random_int_inclusive<int>(0, (int)discrete_range.size() - 1)};
                                    new_parameter += std::to_string(discrete_range.at(random_index)) + ';';
                                } else if (std::holds_alternative<std::pair<ai_agent_utils::ContinuousRangeVariant, ai_agent_utils::ContinuousRangeVariant>>(range)) {
                                    const std::pair<ai_agent_utils::ContinuousRangeVariant, ai_agent_utils::ContinuousRangeVariant> continuous_range{std::get<std::pair<ai_agent_utils::ContinuousRangeVariant, ai_agent_utils::ContinuousRangeVariant>>(range)};
                                    if (std::holds_alternative<int>(continuous_range.first) && std::holds_alternative<int>(continuous_range.second)) {
                                        const int lower_bound{std::get<int>(continuous_range.first)};
                                        const int upper_bound{std::get<int>(continuous_range.second)};
                                        const float random_value{ai_agent_utils::random_real_inclusive<float>(lower_bound, upper_bound)};
                                        new_parameter += std::to_string(random_value) + ';';
                                    } else if (std::holds_alternative<float>(continuous_range.first) && std::holds_alternative<float>(continuous_range.second)) {
                                        const float lower_bound{std::get<float>(continuous_range.first)};
                                        const float upper_bound{std::get<float>(continuous_range.second)};
                                        const float random_value{ai_agent_utils::random_real_inclusive<float>(lower_bound, upper_bound)};
                                        new_parameter += std::to_string(random_value) + ';';
                                    }
                                } else {
                                    new_parameter += "0;";
                                }
                                new_parameter.pop_back(); // Remove the last semicolon
                                new_parameters.at(j) = new_parameter;
                            }
                            std::string encoded_node;
                            encoded_node += std::to_string(entity_type) + ';';
                            encoded_node += std::to_string(n_id) + ';';
                            encoded_node += std::to_string(oneof_value_field_number) + ';';
                            for (const auto& new_parameter : new_parameters) encoded_node += new_parameter + ';';

                            encoded_node.pop_back(); // Remove the last semicolon

                            population_buffer.at(i) += encoded_node + ',';
                        }
                    } else {
                        // Connection
                        for (int i{0}; i < maximum_population_size; ++i) population_buffer.at(i) += entity + ',';
                    }
                }

                for (auto& entity : population_buffer) {
                    if (!entity.empty()) entity.pop_back(); // Remove the last comma
                }
                break;
            }
            default:
                break;
        }

        return true;
    }

    unsigned long get_fitness_value(
        const std::string& encoded_graph, 
        ShaderSampler* shader_sampler,
        const uint32_t* target_image_pixels, 
        const int& width,
        const int& height
    ) noexcept {
        std::string code;
      
        bool result{shadergen_visual_shader_generator::generate_shader(
          shadergen_visual_shader_generator::to_proto_nodes(encoded_graph),
          shadergen_visual_shader_generator::to_generators(encoded_graph), 
          shadergen_visual_shader_generator::to_port_type_generators(encoded_graph),
          shadergen_visual_shader_generator::to_input_output_connections_by_key(encoded_graph), code)};
        CHECK_CONDITION_TRUE_NON_VOID(!result, std::numeric_limits<unsigned long>::max(), "Failed to generate shader code");

        QImage extracted_image;

        // Call the sample_once method on the GUI thread and block until it returns
        bool success = QMetaObject::invokeMethod(
            shader_sampler, 
            "sample_once", 
            Qt::BlockingQueuedConnection, 
            Q_RETURN_ARG(QImage, extracted_image), 
            Q_ARG(const std::string, code) 
        );
        CHECK_CONDITION_TRUE_NON_VOID(!success, std::numeric_limits<unsigned long>::max(), "Failed to get fitness value");

        CHECK_CONDITION_TRUE_NON_VOID(extracted_image.isNull(), std::numeric_limits<unsigned long>::max(), "Failed to get fitness value");

        // Retrieve pointers to the pixel data.
        // QImage::bits() returns a pointer to the first pixel, and since our format is ARGB32,
        // we can safely reinterpret_cast to a uint32_t pointer.
        const uint32_t* extracted_image_pixels = reinterpret_cast<const uint32_t*>(extracted_image.bits());

        return ai_agent_fitness::calculate_fitness(extracted_image_pixels, target_image_pixels, width, height);
    }

} // namespace ai_agent_main
