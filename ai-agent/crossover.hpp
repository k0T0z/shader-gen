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
enum class CrossoverType {
    MULTI_POINT,
};

/**
 * @brief 
 * 
 * @note Multi-point crossover is a discrete crossover operator.
 * 
 * @todo This function need to support searching continuous parameters.
 *       Using child1_i = parent1_i * beta_i + parent2_i * (1 - beta_i) and 
 *       child2_i = parent1_i * (1 - beta_i) + parent2_i * beta_i
 * 
 * @param parent1 
 * @param parent2 
 * @param crossover_probability 
 * @param matching_type 
 * @return std::pair<std::string, std::string> 
 */
inline static std::pair<std::string, std::string> multi_point_crossover(
    const std::string& parent1,
    const std::string& parent2,
    const float& crossover_probability, 
    const ai_agent_main::MatchingType& matching_type = ai_agent_main::MatchingType::PARAMETERS_ONLY
) {
    const int num_crossover_points{ai_agent_utils::random_int_inclusive<int>(2, 4)};
    std::pair<std::string, std::string> children;

    switch (matching_type) {
        case ai_agent_main::MatchingType::PARAMETERS_ONLY: {
            // Parameters-only means we won't need to modify the connections and since both
            // parents have the same nodes, we can do crossover on the nodes only
            const std::pair<std::vector<std::string>, std::vector<std::string>> parent1_filtered = ai_agent_utils::filter_entities_into_tokens(parent1);
            const std::pair<std::vector<std::string>, std::vector<std::string>> parent2_filtered = ai_agent_utils::filter_entities_into_tokens(parent2);

            const std::vector<std::string> parent1_nodes = parent1_filtered.first, parent2_nodes = parent2_filtered.first;

            const size_t M = parent1_nodes.size();

            CHECK_CONDITION_TRUE_NON_VOID(parent2_nodes.size() != M, children, "Parent chromosomes must be of equal length");
            CHECK_CONDITION_TRUE_NON_VOID(M < 2ULL, children, "Crossover not possible with length < 2");
            CHECK_CONDITION_TRUE_NON_VOID(M - 1ULL < static_cast<size_t>(num_crossover_points), children, "Not enough positions for the specified number of crossover points");

            // Generate unique crossover points (from 1 to M-1)
            std::set<size_t> points_set;
            while (points_set.size() < static_cast<size_t>(num_crossover_points)) {
                size_t point = ai_agent_utils::random_int_include_first_exclude_second<size_t>(1ULL, M); // [1, M-1]
                points_set.insert(point);
            }
            std::vector<size_t> crossover_points(points_set.begin(), points_set.end());
            std::sort(crossover_points.begin(), crossover_points.end());

            // Perform multi-point crossover on nodes
            std::vector<std::string> child1_nodes, child2_nodes;
            child1_nodes.reserve(M);
            child2_nodes.reserve(M);
            size_t start = 0ULL;
            for (size_t i = 0ULL; i < crossover_points.size(); ++i) {
                size_t end = crossover_points.at(i);
                if (i % 2ULL == 0ULL) {
                    // Even segments: child1 takes from parent1, child2 takes from parent2
                    child1_nodes.insert(child1_nodes.end(), parent1_nodes.begin() + start, parent1_nodes.begin() + end);
                    child2_nodes.insert(child2_nodes.end(), parent2_nodes.begin() + start, parent2_nodes.begin() + end);
                } else {
                    // Odd segments: child1 takes from parent2, child2 takes from parent1
                    child1_nodes.insert(child1_nodes.end(), parent2_nodes.begin() + start, parent2_nodes.begin() + end);
                    child2_nodes.insert(child2_nodes.end(), parent1_nodes.begin() + start, parent1_nodes.begin() + end);
                }
                start = end;
            }
            // Append the remaining segment
            if (start < M) {
                if (crossover_points.size() % 2ULL == 0ULL) {
                    child1_nodes.insert(child1_nodes.end(), parent1_nodes.begin() + start, parent1_nodes.end());
                    child2_nodes.insert(child2_nodes.end(), parent2_nodes.begin() + start, parent2_nodes.end());
                } else {
                    child1_nodes.insert(child1_nodes.end(), parent2_nodes.begin() + start, parent2_nodes.end());
                    child2_nodes.insert(child2_nodes.end(), parent1_nodes.begin() + start, parent1_nodes.end());
                }
            }

            // Reconstruct child chromosomes using original tokens and replacing nodes
            const std::string child1 = ai_agent_utils::combine_entities_from_tokens(child1_nodes, parent1_filtered.second);
            const std::string child2 = ai_agent_utils::combine_entities_from_tokens(child2_nodes, parent2_filtered.second);

            // Apply crossover probability
            const float r1 = ai_agent_utils::random_real_include_first_exclude_second<float>(0.0f, 1.0f);
            if (r1 <= crossover_probability) {
                children.first = child1;
            } else {
                children.first = parent1;
            }
            const float r2 = ai_agent_utils::random_real_include_first_exclude_second<float>(0.0f, 1.0f);
            if (r2 <= crossover_probability) {
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

inline static std::pair<std::string, std::string> crossover(
    const std::string& parent1,
    const std::string& parent2,
    const float& crossover_probability, 
    const CrossoverType& crossover_type = CrossoverType::MULTI_POINT,
    const ai_agent_main::MatchingType& matching_type = ai_agent_main::MatchingType::PARAMETERS_ONLY
) {
    switch (crossover_type) {
        case CrossoverType::MULTI_POINT:
            return multi_point_crossover(parent1, parent2, crossover_probability, matching_type);
        default:
            break;
    }

    return std::make_pair(parent1, parent2);
}
}  // namespace ai_agent_crossover

#endif // AI_AGENT_CROSSOVER_HPP
