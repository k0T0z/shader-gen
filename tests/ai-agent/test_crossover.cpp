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
#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "ai-agent/crossover.hpp"
#include "ai-agent/parameters.hpp"

const std::string parent1 = "0;0;5,0;1;4;1=1,0;2;4;1=2,0;3;23;1=39.734821,0;4;18;1=15,0;5;6;1=16.666733,0;6;14;1=9,0;7;14;1=2,0;8;18;1=13,1;0;1;0;3;0,1;1;2;0;4;0,1;2;4;0;6;0,1;3;5;0;6;1,1;4;3;0;7;0,1;5;6;0;7;1,1;6;7;0;8;0,1;7;8;0;0;0";
const std::string parent2 = "0;0;5,0;1;4;1=1,0;2;4;1=1,0;3;23;1=91.669510,0;4;18;1=7,0;5;6;1=58.666428,0;6;14;1=2,0;7;14;1=9,0;8;18;1=13,1;0;1;0;3;0,1;1;2;0;4;0,1;2;4;0;6;0,1;3;5;0;6;1,1;4;3;0;7;0,1;5;6;0;7;1,1;6;7;0;8;0,1;7;8;0;0;0";

TEST(CrossoverTest, BasicCrossover) {
    auto result = ai_agent_crossover::crossover(parent1, parent2, crossover_probability);
    DEBUG_PRINT(result.first);
    DEBUG_PRINT(result.second);
}

