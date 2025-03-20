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

#ifndef AI_AGENT_SELECTION_HPP
#define AI_AGENT_SELECTION_HPP

#include <vector>
#include <string>
#include <utility>     // std::pair, std::make_pair
#include <algorithm>   // std::max_element, std::sort, std::lower_bound
#include <numeric>     // std::distance (though technically in <iterator>, often included via <numeric>)

#include "error_macros.hpp"
#include "ai-agent/utils/utils.hpp"

namespace ai_agent_selection {
inline static std::pair<std::string, std::string> select(const std::vector<std::pair<std::string, unsigned long>>& population_fitness) {
    // Check if population size is sufficient to select two different parents
    CHECK_CONDITION_TRUE_NON_VOID(population_fitness.size() < 2, std::make_pair("", ""), "Population size is less than 2");

    // Find the maximum fitness value
    auto max_it = std::max_element(population_fitness.begin(), population_fitness.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    unsigned long fitness_max = max_it->second;

    // Compute transformed fitness: fitness_max - fitness + 1
    // This transforms fitness so that lower original fitness (better) gets higher transformed fitness
    std::vector<unsigned long> transformed_fitness(population_fitness.size());
    for (size_t i = 0; i < population_fitness.size(); ++i) {
        transformed_fitness[i] = fitness_max - population_fitness[i].second + 1;
    }

    // Compute sum of transformed fitness as double to avoid overflow and enable floating-point division
    double sum_fitness = 0.0;
    for (const auto& tf : transformed_fitness) {
        sum_fitness += static_cast<double>(tf);
    }

    // Normalize fitness values
    std::vector<double> normalized(population_fitness.size());
    for (size_t i = 0; i < population_fitness.size(); ++i) {
        normalized[i] = static_cast<double>(transformed_fitness[i]) / sum_fitness;
    }

    // Pair each normalized value with its original index
    std::vector<std::pair<double, size_t>> indexed_normalized;
    for (size_t i = 0; i < population_fitness.size(); ++i) {
        indexed_normalized.emplace_back(normalized[i], i);
    }

    // Sort by normalized value in ascending order (to match Python implementation)
    std::sort(indexed_normalized.begin(), indexed_normalized.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    // Compute cumulative probabilities
    std::vector<double> cumulative(indexed_normalized.size());
    double current = 0.0;
    for (size_t j = 0; j < indexed_normalized.size(); ++j) {
        current += indexed_normalized[j].first;
        cumulative[j] = current;
    }

    // Select first parent
    double r1 = ai_agent_utils::random_real_include_first_exclude_second<double>(0.0, 1.0);
    auto it1 = std::lower_bound(cumulative.begin(), cumulative.end(), r1);
    size_t idx1;
    if (it1 == cumulative.end()) {
        idx1 = indexed_normalized.back().second;
    } else {
        idx1 = indexed_normalized[std::distance(cumulative.begin(), it1)].second;
    }

    // Select second parent, ensuring it is different from the first
    size_t idx2;
    do {
        double r2 = ai_agent_utils::random_real_include_first_exclude_second<double>(0.0, 1.0);
        auto it2 = std::lower_bound(cumulative.begin(), cumulative.end(), r2);
        if (it2 == cumulative.end()) {
            idx2 = indexed_normalized.back().second;
        } else {
            idx2 = indexed_normalized[std::distance(cumulative.begin(), it2)].second;
        }
    } while (idx2 == idx1);

    // Return the selected chromosomes as a pair of strings
    return std::make_pair(population_fitness[idx1].first, population_fitness[idx2].first);
}
}  // namespace ai_agent_selection

#endif // AI_AGENT_SELECTION_HPP
