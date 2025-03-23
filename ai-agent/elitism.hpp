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

#ifndef AI_AGENT_ELITISM_HPP
#define AI_AGENT_ELITISM_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include "error_macros.hpp"

namespace ai_agent_elitism {
inline static std::vector<std::pair<std::string, unsigned long>> apply_elitism(
    const std::vector<std::pair<std::string, unsigned long>>& old_population_fitness,
    const std::vector<std::pair<std::string, unsigned long>>& new_population_fitness,
    const float& elitism_ratio
) {
    // Get population size
    const size_t N = new_population_fitness.size();
    
    // Calculate number of elites to preserve
    const size_t elite_num = static_cast<size_t>(std::floor(N * elitism_ratio));

    CHECK_CONDITION_TRUE_NON_VOID(elite_num == 0ULL, new_population_fitness, "Elitism ratio is too low");

    std::vector<std::pair<std::string, unsigned long>> old_sorted = old_population_fitness;
    std::sort(old_sorted.begin(), old_sorted.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    const std::vector<std::pair<std::string, unsigned long>> elites(old_sorted.begin(), old_sorted.begin() + elite_num);

    std::vector<std::pair<std::string, unsigned long>> new_sorted = new_population_fitness;
    std::sort(new_sorted.begin(), new_sorted.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    // Replace the worst elite_num individuals with elites
    for (size_t i = 0ULL; i < elite_num; ++i) new_sorted.at(N - 1 - i) = elites.at(i);

    // Sort the updated population by fitness
    std::sort(new_sorted.begin(), new_sorted.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    return new_sorted;
}
}  // namespace ai_agent_elitism

#endif // AI_AGENT_ELITISM_HPP
