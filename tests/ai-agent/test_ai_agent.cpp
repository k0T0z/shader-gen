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

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>  // for uint32_t

// #include <QGuiApplication>

#include "ai-agent/ai_agent.hpp"
#include "ai-agent/parameters.hpp"
#include "error_macros.hpp"

bool get_image_data(std::vector<std::vector<uint32_t>>& image_data_buffer) {
    std::string root_dir{SHADER_GEN_ROOTDIR};
    if (root_dir.back() != '/') root_dir += '/';

    // Load the target image pixels
    std::ifstream file(root_dir + "tests/ai-agent/test_solas_256x256_ARGB32.txt");
    if (!file.is_open()) {
        ERROR_PRINT("Failed to open the file.");
        return false;
    }

    std::string line;
    // Process the file line by line.
    while (std::getline(file, line)) {
        std::vector<uint32_t> row;
        std::istringstream lineStream(line);
        std::string token;
        
        // Split the current line by commas.
        while (std::getline(lineStream, token, ',')) {
            // Each token is expected to be in hexadecimal format like "0xAARRGGBB".
            // Convert the token from a string to a uint32_t, specifying base 16.
            uint32_t pixel = std::stoul(token, nullptr, 16);
            row.push_back(pixel);
        }
        if (!row.empty()) image_data_buffer.push_back(row);
    }
    file.close();

    return true;
}

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
    ASSERT_EQ(initial_population.size(), maximum_population_size);

    std::vector<std::vector<uint32_t>> image_data;
    image_data.resize(256);
    for (auto& row : image_data) row.resize(256);
    ASSERT_TRUE(get_image_data(image_data));

    // Convert to const uint32_t*
    std::vector<uint32_t> flat_data;
    flat_data.reserve(256 * 256); // Reserve space for 256x256 pixels
    for (const auto& row : image_data) {
        flat_data.insert(flat_data.end(), row.begin(), row.end());
    }
    const uint32_t* image_ptr = flat_data.data();

    // // Dummy arguments for QGuiApplication for the ImageExtractor
    // int argc = 1;
    // char arg[] = "test";
    // char* argv[] = { arg };
    // QGuiApplication app(argc, argv);

    // // Create the extractor
    // ImageExtractor image_extractor;
    // ASSERT_TRUE(image_extractor.initialize());

    // std::vector<unsigned long> fitness_values;
    // fitness_values.resize(maximum_population_size);
    // for (int i {0}; i < maximum_population_size; i++) {
    //     fitness_values.at(i) = ai_agent_main::get_fitness_value(
    //         initial_population.at(i),
    //         image_extractor,
    //         image_ptr,
    //         256,
    //         256
    //     );
    // }
}
