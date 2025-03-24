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

#ifndef AI_AGENT_MUTATION_HPP
#define AI_AGENT_MUTATION_HPP

#include <vector>
#include <string>
#include <stdexcept>

#include "error_macros.hpp"
#include "ai-agent/utils/utils.hpp"
#include "ai-agent/ai_agent.hpp"

namespace ai_agent_mutation {
inline static std::string mutate(
    const std::string& child, 
    const float& mutation_probability,
    const ai_agent_main::MatchingType& matching_type = ai_agent_main::MatchingType::PARAMETERS_ONLY
) {
    switch (matching_type) {
        case ai_agent_main::MatchingType::PARAMETERS_ONLY: {
            const std::pair<std::vector<std::string>, std::vector<std::string>> filtered_graph_tokens{ai_agent_utils::filter_entities_into_tokens(child)};
            const std::vector<std::string> node_entities{filtered_graph_tokens.first};
            const std::vector<std::string> connection_entities{filtered_graph_tokens.second};

            std::vector<std::string> mutated_entities;
            mutated_entities.reserve(node_entities.size() + connection_entities.size());
            
            // Process each entity
            for (const auto& node_entity : node_entities) {
                const std::vector<std::string> entity_tokens{ai_agent_utils::split_string(node_entity, ';')};
                const int entity_type = std::stoi(ai_agent_utils::get_entity_type(entity_tokens));
                CONTINUE_IF_TRUE(entity_type != 0, "Entity type is not a node");

                const int n_id = std::stoi(ai_agent_utils::get_node_entity_id(entity_tokens));
                const int oneof_value_field_number = std::stoi(ai_agent_utils::get_node_entity_oneof_value_field_number(entity_tokens));
                const std::vector<std::string> parameters{ai_agent_utils::get_node_entity_parameters(entity_tokens)};

                std::vector<std::string> new_parameters;
                new_parameters.resize(parameters.size());

                for (int i{0}; i < parameters.size(); ++i) {
                    const float r{ai_agent_utils::random_real_inclusive<float>(0.0f, 1.0f)};
                    if (r > mutation_probability) {
                        new_parameters.at(i) = parameters.at(i);
                        continue;
                    }

                    const std::string parameter{parameters.at(i)}; // Format: field_number=value
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
                    new_parameters.at(i) = new_parameter;
                }

                mutated_entities.push_back(ai_agent_utils::join_string({std::to_string(entity_type), std::to_string(n_id), std::to_string(oneof_value_field_number), ai_agent_utils::join_string(new_parameters, ';')}, ';'));
            }

            // Add the connections as-is
            mutated_entities.insert(mutated_entities.end(), connection_entities.begin(), connection_entities.end());
            
            return ai_agent_utils::join_string(mutated_entities, ',');
        }
        default:
            break;
    }
    
    return "";
}
}  // namespace ai_agent_mutation

#endif // AI_AGENT_MUTATION_HPP
