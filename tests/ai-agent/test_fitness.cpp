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

#include "ai-agent/fitness.hpp"

TEST(AIAgentTest, TestFitnessForIdenticalImages2x2) {
    // Simulate a 2x2 image with the same pixel data.
    const int width = 2;
    const int height = 2;
    static const uint32_t pixels1[width * height] = {
        0xFF112233, 0xFF445566,
        0xFF778899, 0xFFAABBCC
    };
    static const uint32_t pixels2[width * height] = {
        0xFF112233, 0xFF445566,
        0xFF778899, 0xFFAABBCC
    };
    
    unsigned long diff = ai_agent_fitness::calculate_fitness(pixels1, pixels2, width, height);
    ASSERT_EQ(diff, 0);
}

TEST(AIAgentTest, TestFitnessForDifferentImages2x2) {
    // Simulate a 2x2 image with different pixel data.
    const int width = 2;
    const int height = 2;
    static const uint32_t pixels1[width * height] = {
        0xFF112233, 0xFF445566,
        0xFF778899, 0xFFAABBCC
    };
    static const uint32_t pixels2[width * height] = {
        0xFF112233, 0xFF445566,
        0xFF778899, 0xFFAABBDD
    };
    
    unsigned long diff = ai_agent_fitness::calculate_fitness(pixels1, pixels2, width, height);
    ASSERT_EQ(diff, 17);
}
