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
        std::vector<std::vector<std::string>> population;
        population.resize(maximum_population_size);

        switch (matching_type) {
            case MatchingType::PARAMETERS_ONLY: {
                const std::pair<std::vector<std::string>, std::vector<std::string>> filtered_graph_tokens{ai_agent_utils::filter_entities_into_tokens(encoded_graph)};
                const std::vector<std::string> node_entities{filtered_graph_tokens.first};
                for (const auto& node_entity : node_entities) {
                    const std::vector<std::string> entity_tokens{ai_agent_utils::split_string(node_entity, ';')};
                    const int entity_type = std::stoi(ai_agent_utils::get_entity_type(entity_tokens));
                    CONTINUE_IF_TRUE(entity_type != 0, "Entity type is not a node");

                    // Add the node entity to all individuals in the population with random parameters
                    for (int i{0}; i < maximum_population_size; ++i) {
                        const int n_id = std::stoi(ai_agent_utils::get_node_entity_id(entity_tokens));
                        const int oneof_value_field_number = std::stoi(ai_agent_utils::get_node_entity_oneof_value_field_number(entity_tokens));
                        const std::vector<std::string> parameters{ai_agent_utils::get_node_entity_parameters(entity_tokens)};

                        std::vector<std::string> new_parameters;
                        new_parameters.resize(parameters.size());

                        for (int j{0}; j < parameters.size(); ++j) {
                            const std::string parameter{parameters.at(j)}; // Format: field_number=value
                            const std::vector<std::string> parameter_tokens{ai_agent_utils::split_string(parameter, '=')};
                            const int field_number = std::stoi(ai_agent_utils::get_node_entity_parameter_field_number(parameter_tokens));
                            const ai_agent_utils::DiscreteContinuousRangeVariant range{ai_agent_utils::get_range_for_field(oneof_value_field_number, field_number)};
                            std::string new_parameter;
                            if (std::holds_alternative<std::vector<int>>(range)) {
                                const std::vector<int> discrete_range{std::get<std::vector<int>>(range)};
                                const int random_index{ai_agent_utils::random_int_inclusive<int>(0, (int)discrete_range.size() - 1)};
                                new_parameter = ai_agent_utils::join_string({std::to_string(field_number), std::to_string(discrete_range.at(random_index))}, '=');
                            } else if (std::holds_alternative<std::pair<ai_agent_utils::ContinuousRangeVariant, ai_agent_utils::ContinuousRangeVariant>>(range)) {
                                const std::pair<ai_agent_utils::ContinuousRangeVariant, ai_agent_utils::ContinuousRangeVariant> continuous_range{std::get<std::pair<ai_agent_utils::ContinuousRangeVariant, ai_agent_utils::ContinuousRangeVariant>>(range)};
                                if (std::holds_alternative<int>(continuous_range.first) && std::holds_alternative<int>(continuous_range.second)) {
                                    const int lower_bound{std::get<int>(continuous_range.first)};
                                    const int upper_bound{std::get<int>(continuous_range.second)};
                                    const float random_value{ai_agent_utils::random_real_inclusive<float>(lower_bound, upper_bound)};
                                    new_parameter = ai_agent_utils::join_string({std::to_string(field_number), std::to_string(random_value)}, '=');
                                } else if (std::holds_alternative<float>(continuous_range.first) && std::holds_alternative<float>(continuous_range.second)) {
                                    const float lower_bound{std::get<float>(continuous_range.first)};
                                    const float upper_bound{std::get<float>(continuous_range.second)};
                                    const float random_value{ai_agent_utils::random_real_inclusive<float>(lower_bound, upper_bound)};
                                    new_parameter = ai_agent_utils::join_string({std::to_string(field_number), std::to_string(random_value)}, '=');
                                }
                            }
                            new_parameters.at(j) = new_parameter;
                        }

                        // Add entity_type, node_id, oneof_value_field_number, and new_parameters to the ith individual
                        population.at(i).push_back(ai_agent_utils::join_string({std::to_string(entity_type), std::to_string(n_id), std::to_string(oneof_value_field_number), ai_agent_utils::join_string(new_parameters, ';')}, ';'));
                    } // for (int i{0}; i < maximum_population_size; ++i)
                } // for (const auto& node_entity : node_entities)

                // Add the connections as-is
                const std::vector<std::string> connection_entities{filtered_graph_tokens.second};
                for (int i{0}; i < maximum_population_size; ++i) {
                    population.at(i).insert(population.at(i).end(), connection_entities.begin(), connection_entities.end());
                }
                break;
            }
            default:
                break;
        }

        population_buffer.reserve(maximum_population_size);
        for (int i{0}; i < maximum_population_size; ++i) {
            population_buffer.push_back(ai_agent_utils::join_string(population.at(i), ','));
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

        QImage extracted_image = shader_sampler->sample_once(code);

        CHECK_CONDITION_TRUE_NON_VOID(extracted_image.isNull(), std::numeric_limits<unsigned long>::max(), "Failed to get fitness value");

        // Retrieve pointers to the pixel data.
        // QImage::bits() returns a pointer to the first pixel, and since our format is ARGB32,
        // we can safely reinterpret_cast to a uint32_t pointer.
        const uint32_t* extracted_image_pixels = reinterpret_cast<const uint32_t*>(extracted_image.bits());

        return ai_agent_fitness::calculate_fitness(extracted_image_pixels, target_image_pixels, width, height);
    }

} // namespace ai_agent_main
