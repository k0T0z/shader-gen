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
            std::vector<std::string> entities = ai_agent_utils::split_string(child, ',');

            std::vector<std::string> mutated_entities;
            mutated_entities.reserve(entities.size());
            
            // Process each entity
            for (const auto& entity : entities) {
                std::vector<std::string> entity_tokens = ai_agent_utils::split_string(entity, ';');
                const int entity_type = std::stoi(entity_tokens.at(0));
                
                if (entity_type == 0) {
                    // Node
                    // Format: entity_type;node_id;node_type;field_number=value;field_number=value;...
                    const int n_id = std::stoi(entity_tokens.at(1));
                    const int oneof_value_field_number = std::stoi(entity_tokens.at(2));
                    const std::vector<std::string> parameters{entity_tokens.begin() + 3, entity_tokens.end()};
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
                        new_parameters.at(i) = new_parameter;
                    }

                    std::string encoded_node;
                    encoded_node += std::to_string(entity_type) + ';';
                    encoded_node += std::to_string(n_id) + ';';
                    encoded_node += std::to_string(oneof_value_field_number) + ';';
                    for (const auto& new_parameter : new_parameters) encoded_node += new_parameter + ';';

                    encoded_node.pop_back(); // Remove the last semicolon

                    mutated_entities.push_back(encoded_node);
                } else if (entity_type == 1) {
                    // Connection: keep unchanged
                    mutated_entities.push_back(entity);
                } else {
                    FAIL_AND_RETURN_NON_VOID("", "Invalid entity type: " + std::to_string(entity_type));
                }
            }
            
            return ai_agent_utils::join_string(mutated_entities, ',');
        }
        default:
            break;
    }
    
    return "";
}
}  // namespace ai_agent_mutation

#endif // AI_AGENT_MUTATION_HPP
