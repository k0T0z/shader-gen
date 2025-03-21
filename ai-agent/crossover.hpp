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
    const std::pair<std::string, 
    std::string>& parents, 
    const int& crossover_probability, 
    const int& num_crossover_points = 3,
    const ai_agent_main::MatchingType& matching_type = ai_agent_main::MatchingType::PARAMETERS_ONLY
) {
    // Extract parent chromosomes
    const std::string& parent1 = parents.first;
    const std::string& parent2 = parents.second;
    size_t M = parent1.size();

    // Validate input
    CHECK_CONDITION_TRUE_NON_VOID(parent2.size() != M, parents, "Parent chromosomes must be of equal length");
    CHECK_CONDITION_TRUE_NON_VOID(M < 2, parents, "Crossover not possible with length < 2");
    CHECK_CONDITION_TRUE_NON_VOID(M - 1 < static_cast<size_t>(num_crossover_points), parents, "Not enough positions for the specified number of crossover points");

    // Generate unique crossover points (from 1 to M-1)
    std::set<size_t> points_set;
    while (points_set.size() < static_cast<size_t>(num_crossover_points)) {
        size_t point = ai_agent_utils::random_int_include_first_exclude_second<size_t>(1, M);
        points_set.insert(point);
    }
    std::vector<size_t> crossover_points(points_set.begin(), points_set.end());
    std::sort(crossover_points.begin(), crossover_points.end());

    // Perform multi-point crossover
    std::string child1, child2;
    size_t start = 0;
    for (size_t i = 0; i < crossover_points.size(); ++i) {
        size_t end = crossover_points[i];
        if (i % 2 == 0) {
            // Even segments: child1 gets from parent1, child2 from parent2
            child1 += parent1.substr(start, end - start);
            child2 += parent2.substr(start, end - start);
        } else {
            // Odd segments: child1 gets from parent2, child2 from parent1
            child1 += parent2.substr(start, end - start);
            child2 += parent1.substr(start, end - start);
        }
        start = end;
    }
    // Append the remaining segment
    if (start < M) {
        if (crossover_points.size() % 2 == 0) {
            child1 += parent1.substr(start);
            child2 += parent2.substr(start);
        } else {
            child1 += parent2.substr(start);
            child2 += parent1.substr(start);
        }
    }

    // Apply crossover probability to decide output
    std::string result1, result2;
    double r1 = ai_agent_utils::random_real_include_first_exclude_second<double>(0.0, 1.0);
    if (r1 <= static_cast<double>(crossover_probability) / 100.0) {
        result1 = child1;
    } else {
        result1 = parent1;
    }
    double r2 = ai_agent_utils::random_real_include_first_exclude_second<double>(0.0, 1.0);
    if (r2 <= static_cast<double>(crossover_probability) / 100.0) {
        result2 = child2;
    } else {
        result2 = parent2;
    }

    return std::make_pair(result1, result2);
}
}  // namespace ai_agent_crossover

#endif // AI_AGENT_CROSSOVER_HPP
