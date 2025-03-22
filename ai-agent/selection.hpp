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
enum class SelectionType {
    ROULETTE_WHEEL
};

inline static std::pair<std::pair<std::string, unsigned long>, std::pair<std::string, unsigned long>> roulette_wheel_select(const std::vector<std::pair<std::string, unsigned long>>& population_fitness) {
    // Check if population size is sufficient to select two different parents
    CHECK_CONDITION_TRUE_NON_VOID(population_fitness.size() < 2ULL, std::make_pair(std::make_pair("", 0UL), std::make_pair("", 0UL)), "Population size is less than 2");
    CHECK_CONDITION_TRUE_NON_VOID(population_fitness.size() == 2ULL, std::make_pair(population_fitness.at(0ULL), population_fitness.at(1ULL)), "Population size is exactly 2");

    // Compute sum of fitness
    unsigned long sum_fitness = 0UL;
    for (const auto& pair : population_fitness) {
        if (pair.second == 0UL) {
            sum_fitness += 1UL;
        } else {
            sum_fitness += pair.second;
        }
    }

    // Normalize fitness values
    std::vector<long double> normalized(population_fitness.size());
    for (size_t i = 0ULL; i < population_fitness.size(); ++i) {
        const long double i_fitness = population_fitness.at(i).second == 0UL ? 1.0L : static_cast<long double>(population_fitness.at(i).second);
        normalized.at(i) = i_fitness / static_cast<long double>(sum_fitness);
    }

    // Pair each normalized value with its original index
    std::vector<std::pair<long double, size_t>> indexed_normalized;
    for (size_t i = 0ULL; i < population_fitness.size(); ++i) {
        indexed_normalized.emplace_back(normalized.at(i), i);
    }

    // Sort by normalized value in ascending order
    std::sort(indexed_normalized.begin(), indexed_normalized.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    // Compute cumulative probabilities
    std::vector<long double> cumulative(indexed_normalized.size());
    long double current = 0.0L;
    for (size_t j = 0ULL; j < indexed_normalized.size(); ++j) {
        current += indexed_normalized.at(j).first;
        cumulative.at(j) = current;
    }

    // Select first parent
    long double r1 = ai_agent_utils::random_real_include_first_exclude_second<long double>(0.0L, 1.0L);
    auto it1 = std::lower_bound(cumulative.begin(), cumulative.end(), r1);
    size_t idx1;
    if (it1 == cumulative.end()) {
        idx1 = indexed_normalized.back().second;
    } else {
        idx1 = indexed_normalized.at(std::distance(cumulative.begin(), it1)).second;
    }

    // Select second parent, ensuring it is different from the first
    size_t idx2;
    do {
        long double r2 = ai_agent_utils::random_real_include_first_exclude_second<long double>(0.0L, 1.0L);
        auto it2 = std::lower_bound(cumulative.begin(), cumulative.end(), r2);
        if (it2 == cumulative.end()) {
            idx2 = indexed_normalized.back().second;
        } else {
            idx2 = indexed_normalized.at(std::distance(cumulative.begin(), it2)).second;
        }
    } while (idx2 == idx1);

    // Return the selected chromosomes as a pair of strings
    return std::make_pair(population_fitness.at(idx1), population_fitness.at(idx2));
}

inline static std::pair<std::pair<std::string, unsigned long>, std::pair<std::string, unsigned long>> select(const std::vector<std::pair<std::string, unsigned long>>& population_fitness, const SelectionType& selection_type = SelectionType::ROULETTE_WHEEL) {
    switch (selection_type) {
        case SelectionType::ROULETTE_WHEEL:
            return roulette_wheel_select(population_fitness);
        default:
            break;
    }

    return std::make_pair(std::make_pair("", 0UL), std::make_pair("", 0UL));
}
}  // namespace ai_agent_selection

#endif // AI_AGENT_SELECTION_HPP
