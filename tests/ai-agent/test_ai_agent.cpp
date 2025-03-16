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

#include <gtest/gtest.h>

#include <string>

#include "ai-agent/ai_agent.hpp"
#include "ai-agent/parameters.hpp"

TEST(AIAgentTest, TestMatchingAlgorithm) {
    // I encoded the example.json graph
    const std::string encoded_graph{"0;0;5,0;1;4;1=1,0;2;4;1=2,0;3;23;1=5,0;4;18;1=1,0;5;6;1=2,0;6;14;1=4,0;7;14;1=2,0;8;18;1=16,1;0;1;0;3;0,1;1;2;0;4;0,1;2;4;0;6;0,1;3;5;0;6;1,1;4;3;0;7;0,1;5;6;0;7;1,1;6;7;0;8;0,1;7;8;0;0;0"};

    std::vector<std::string> initial_population;
    ASSERT_TRUE(ai_agent_main::init(
        ai_agent_main::MatchingType::PARAMETERS_ONLY,
        encoded_graph,
        maximum_population_size,
        initial_population
    ));
}
