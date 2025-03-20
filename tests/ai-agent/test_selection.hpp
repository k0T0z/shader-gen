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
#include <vector>
#include <string>
#include <utility>
#include <map>

#include "ai-agent/selection.hpp"

class AIAgentSelectionTest : public ::testing::Test {
protected:
    void SetUp() override;
};

TEST_F(AIAgentSelectionTest, TestRouletteWheelSelection) {
    std::vector<std::pair<std::string, unsigned long>> population_fitness = {
        {"0000", 10}, {"0001", 20}, {"0010", 30}, {"0011", 40},
        {"0100", 50}, {"0101", 60}, {"0110", 70}, {"0111", 80},
        {"1000", 90}, {"1001", 100}, {"1010", 110}, {"1011", 120},
        {"1100", 130}, {"1101", 140}, {"1110", 150}, {"1111", 160}
    };

    auto selected = ai_agent_selection::roulette_wheel_select(population_fitness);
    EXPECT_NE(selected.first.first, selected.second.first);
}

TEST_F(AIAgentSelectionTest, VaryingFitness) {
    std::vector<std::pair<std::string, unsigned long>> population = {
        {"A", 1}, {"B", 2}, {"C", 3}, {"D", 4}, {"E", 5}
    };

    std::map<std::string, int> selection_count = {
        {"A", 0}, {"B", 0}, {"C", 0}, {"D", 0}, {"E", 0}
    };

    const int num_selections = 1000;
    for (int i = 0; i < num_selections; ++i) {
        auto selected = ai_agent_selection::roulette_wheel_select(population);
        selection_count[selected.first.first]++;
        selection_count[selected.second.first]++;
    }

    // Expect lower fitness values to be selected more often
    EXPECT_GT(selection_count["A"], selection_count["B"]);
    EXPECT_GT(selection_count["B"], selection_count["C"]);
    EXPECT_GT(selection_count["C"], selection_count["D"]);
    EXPECT_GT(selection_count["D"], selection_count["E"]);
}

TEST_F(AIAgentSelectionTest, EqualFitness) {
    std::vector<std::pair<std::string, unsigned long>> population = {
        {"A", 3}, {"B", 3}, {"C", 3}, {"D", 3}, {"E", 3}
    };

    std::map<std::string, int> selection_count = {
        {"A", 0}, {"B", 0}, {"C", 0}, {"D", 0}, {"E", 0}
    };

    const int num_selections = 1000;
    for (int i = 0; i < num_selections; ++i) {
        auto selected = ai_agent_selection::roulette_wheel_select(population);
        selection_count[selected.first.first]++;
        selection_count[selected.second.first]++;
    }

    // Expect roughly equal selection counts
    int avg = 2 * num_selections / (int)population.size();
    for (const auto& count : selection_count) {
        EXPECT_NEAR(count.second, avg, 50); // Allow some statistical variation
    }
}

TEST_F(AIAgentSelectionTest, MinimalPopulation) {
    std::vector<std::pair<std::string, unsigned long>> population = {
        {"A", 1}, {"B", 2}
    };

    auto selected = ai_agent_selection::roulette_wheel_select(population);
    EXPECT_NE(selected.first.first, selected.second.first);
    EXPECT_TRUE(selected.first.first == "A" || selected.first.first == "B");
    EXPECT_TRUE(selected.second.first == "A" || selected.second.first == "B");
}

TEST_F(AIAgentSelectionTest, ZeroFitness) {
    std::vector<std::pair<std::string, unsigned long>> population = {
        {"A", 0}, {"B", 0}, {"C", 1}
    };

    std::map<std::string, int> selection_count = {
        {"A", 0}, {"B", 0}, {"C", 0}
    };

    const int num_selections = 1000;
    for (int i = 0; i < num_selections; ++i) {
        auto selected = ai_agent_selection::roulette_wheel_select(population);
        selection_count[selected.first.first]++;
        selection_count[selected.second.first]++;
    }

    // Expect individuals with zero fitness to be selected more often
    EXPECT_GT(selection_count["A"], selection_count["C"]);
    EXPECT_GT(selection_count["B"], selection_count["C"]);
    EXPECT_NEAR(selection_count["A"], selection_count["B"], 100);
}

TEST_F(AIAgentSelectionTest, SingleIndividual) {
    std::vector<std::pair<std::string, unsigned long>> population = {
        {"A", 5}
    };

    auto selected = ai_agent_selection::roulette_wheel_select(population);
    EXPECT_EQ(selected.first.first, "");
    EXPECT_EQ(selected.second.first, "");
}

TEST_F(AIAgentSelectionTest, EmptyPopulation) {
    std::vector<std::pair<std::string, unsigned long>> population;

    auto selected = ai_agent_selection::roulette_wheel_select(population);
    EXPECT_EQ(selected.first.first, "");
    EXPECT_EQ(selected.second.first, "");
}

