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

#ifndef AI_AGENT_HPP
#define AI_AGENT_HPP

#include <string>

#include "ai-agent/utils/image_extractor.hpp"

namespace ai_agent_main {

    enum class MatchingType {
        PARAMETERS_ONLY,
        PARAMETERS_AND_CONNECTIONS,
        FULL_GRAPH,
    };

    bool genetic_algorithm(
        const MatchingType& matching_type,
        const std::string& encoded_graph,
        const uint32_t* target_image_pixels,
        const int& maximum_population_size
    ) noexcept;

    unsigned long get_fitness_value(
        const std::string& encoded_graph, 
        ImageExtractor& extractor, 
        const uint32_t* target_image_pixels, 
        const int& width, 
        const int& height
    );

} // namespace ai_agent_main

#endif // AI_AGENT_HPP
