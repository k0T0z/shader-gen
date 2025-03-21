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

#ifndef AI_AGENT_CROSSOVER_HPP
#define AI_AGENT_CROSSOVER_HPP

#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <set>

#include "error_macros.hpp"
#include "ai-agent/utils/utils.hpp"
#include "ai-agent/ai_agent.hpp"

namespace ai_agent_crossover {
inline static std::pair<std::string, std::string> crossover(
    const std::string& parent1,
    const std::string& parent2,
    const int& crossover_probability, 
    const int& num_crossover_points = 3,
    const ai_agent_main::MatchingType& matching_type = ai_agent_main::MatchingType::PARAMETERS_ONLY
) {
    std::pair<std::string, std::string> children;

    switch (matching_type) {
        case ai_agent_main::MatchingType::PARAMETERS_ONLY: {
            const std::vector<std::string> parent1_entities = ai_agent_utils::split_string(parent1, ',');
            const std::vector<std::string> parent2_entities = ai_agent_utils::split_string(parent2, ',');

            // Parameters-only means we won't need to modify the connections and since both
            // parents have the same nodes, we can do crossover on the nodes only
            const std::pair<std::vector<std::string>, std::vector<std::string>> parent1_filtered = ai_agent_utils::filter_entities_into_tokens(parent1);
            const std::pair<std::vector<std::string>, std::vector<std::string>> parent2_filtered = ai_agent_utils::filter_entities_into_tokens(parent2);

            const size_t M = parent1_filtered.first.size();

            CHECK_CONDITION_TRUE_NON_VOID(parent2_filtered.first.size() != M, children, "Parent chromosomes must be of equal length");
            CHECK_CONDITION_TRUE_NON_VOID(M < 2, children, "Crossover not possible with length < 2");
            CHECK_CONDITION_TRUE_NON_VOID(M - 1 < static_cast<size_t>(num_crossover_points), children, "Not enough positions for the specified number of crossover points");

            // Generate unique crossover points (from 1 to M-1)
            std::set<size_t> points_set;
            while (points_set.size() < static_cast<size_t>(num_crossover_points)) {
                size_t point = ai_agent_utils::random_int_include_first_exclude_second<size_t>(1, M);
                points_set.insert(point);
            }
            std::vector<size_t> crossover_points(points_set.begin(), points_set.end());
            std::sort(crossover_points.begin(), crossover_points.end());

            // Perform multi-point crossover on nodes
            std::vector<std::string> child1_nodes, child2_nodes;
            size_t start = 0;
            for (size_t i = 0; i < crossover_points.size(); ++i) {
                size_t end = crossover_points.at(i);
                if (i % 2 == 0) {
                    // Even segments: child1 takes from parent1, child2 takes from parent2
                    child1_nodes.insert(child1_nodes.end(), parent1_filtered.first.begin() + start, parent1_filtered.first.begin() + end);
                    child2_nodes.insert(child2_nodes.end(), parent2_filtered.first.begin() + start, parent2_filtered.first.begin() + end);
                } else {
                    // Odd segments: child1 takes from parent2, child2 takes from parent1
                    child1_nodes.insert(child1_nodes.end(), parent2_filtered.first.begin() + start, parent2_filtered.first.begin() + end);
                    child2_nodes.insert(child2_nodes.end(), parent1_filtered.first.begin() + start, parent1_filtered.first.begin() + end);
                }
                start = end;
            }
            // Append the remaining segment
            if (start < M) {
                if (crossover_points.size() % 2 == 0) {
                    child1_nodes.insert(child1_nodes.end(), parent1_filtered.first.begin() + start, parent1_filtered.first.end());
                    child2_nodes.insert(child2_nodes.end(), parent2_filtered.first.begin() + start, parent2_filtered.first.end());
                } else {
                    child1_nodes.insert(child1_nodes.end(), parent2_filtered.first.begin() + start, parent2_filtered.first.end());
                    child2_nodes.insert(child2_nodes.end(), parent1_filtered.first.begin() + start, parent1_filtered.first.end());
                }
            }

            // Reconstruct child chromosomes using original tokens and replacing nodes
            std::vector<std::string> child1_entities = parent1_entities;
            std::vector<std::string> child2_entities = parent2_entities;
            size_t node_idx = 0;
            for (size_t i = 0; i < child1_entities.size(); ++i) {
                const std::vector<std::string> tokens = ai_agent_utils::split_string(child1_entities.at(i), ';');
                const int entity_type = std::stoi(tokens.at(0));
                if (entity_type == 0) {
                    child1_entities.at(i) = child1_nodes.at(node_idx);
                    child2_entities.at(i) = child2_nodes.at(node_idx);
                    ++node_idx;
                }
            }

            // Convert tokens back to strings
            std::string child1 = ai_agent_utils::join_string(child1_entities, ',');
            std::string child2 = ai_agent_utils::join_string(child2_entities, ',');

            // Apply crossover probability
            double r1 = ai_agent_utils::random_real_include_first_exclude_second(0.0, 1.0);
            if (r1 <= static_cast<double>(crossover_probability) / 100.0) {
                children.first = child1;
            } else {
                children.first = parent1;
            }
            double r2 = ai_agent_utils::random_real_include_first_exclude_second(0.0, 1.0);
            if (r2 <= static_cast<double>(crossover_probability) / 100.0) {
                children.second = child2;
            } else {
                children.second = parent2;
            }

            return children;
        }
        default:
            break;
    }

    return children;
}
}  // namespace ai_agent_crossover

#endif // AI_AGENT_CROSSOVER_HPP
