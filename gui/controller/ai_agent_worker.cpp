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

#include "gui/controller/ai_agent_worker.hpp"

#include <sstream>
#include <algorithm>
#include <limits>

#include "error_macros.hpp"
#include "generator/visual_shader_generator.hpp"

AIAgentWorker::AIAgentWorker(ShaderGenSharedMemory* shared_memory) : start_requested(false),
                                                                     exit_requested(false),
                                                                     stop_requested(false),
                                                                     maximum_population_size(0),
                                                                     mutation_probability(0.0f), 
                                                                     crossover_probability(0.0f), 
                                                                     elitism_ratio(0.0f), 
                                                                     maximum_iterations(0),
                                                                     matching_type(ai_agent_main::MatchingType::PARAMETERS_ONLY),
                                                                     shared_memory(shared_memory) {
    worker = std::thread(&AIAgentWorker::worker_main, this);
}

AIAgentWorker::~AIAgentWorker() {
    stop_thread();
    if (worker.joinable()) worker.join();
}

void AIAgentWorker::start_matching() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        start_requested = true;
    }
    cv.notify_one();
}

void AIAgentWorker::stop_matching() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop_requested = true;
    }
}

void AIAgentWorker::worker_main() {
    std::unique_lock<std::mutex> lock(mtx);

    CHECK_PARAM_NULLPTR(shared_memory, "Shared memory is not set");

    while (true) {
        // Set is_stopped to false
        shared_memory->set_is_stopped(false);

        // Wait for either start command or exit request
        cv.wait(lock, [this]() {
            return start_requested.load() || exit_requested;
        });

        // Exit condition check
        if (exit_requested) break;

        // Reset start request
        if (start_requested.load()) start_requested.store(false);

        // It doesn't make sense to stop before even starting
        if (stop_requested.load()) stop_requested.store(false);

        CONTINUE_IF_TRUE(target_image.isNull(), "Target image is not set");

        // Generate initial population
        const std::string encoded_graph = shared_memory->get_encoded_graph();

        std::vector<std::string> initial_population;
        CONTINUE_IF_TRUE(!ai_agent_main::init(
            ai_agent_main::MatchingType::PARAMETERS_ONLY,
            encoded_graph,
            maximum_population_size,
            initial_population
        ), "Failed to create initial population");

        // Create the extractor
        ImageExtractor image_extractor;
        CONTINUE_IF_TRUE(!image_extractor.initialize(), "Failed to initialize image extractor");

        // Get target image pixels
        const uint32_t* target_image_pixels = reinterpret_cast<const uint32_t*>(target_image.bits());

        std::vector<unsigned long> fitness_values;
        fitness_values.resize(maximum_population_size);
        for (int i {0}; i < maximum_population_size; i++) {
            fitness_values.at(i) = ai_agent_main::get_fitness_value(
                initial_population.at(i),
                image_extractor,
                target_image_pixels,
                256,
                256
            );
        }

        DEBUG_PRINT("Initial population generated");

        // Process all pending requests
        // while (process_counter > 0) {
        //     process_counter--;
        //     lock.unlock();

        //     // Process with interrupt checks
        //     bool completed = false;
        //     auto start = std::chrono::steady_clock::now();
            
        //     // Work simulation with interrupt checks
        //     while (!completed) {
        //         // Do chunk of work
        //         DEBUG_PRINT("Processing...");
                
        //         // Check for stop every 100ms
        //         std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
        //         // Check completion condition
        //         auto now = std::chrono::steady_clock::now();
        //         if (now - start > std::chrono::milliseconds(500)) {
        //             completed = true;
        //         }

        //         // Check for stop request
        //         if (stop_requested.load()) {
        //             DEBUG_PRINT("Interrupting current work");
        //             completed = true;
        //         }
        //     }

        //     lock.lock();
            
        //     // Exit processing loop if stop requested
        //     if (stop_requested.load()) {
        //         stop_requested.store(false);
        //         process_counter = 0; // Clear remaining tasks
        //         break;
        //     }
        // }
    }
    DEBUG_PRINT("Worker thread exiting cleanly");
}

void AIAgentWorker::stop_thread() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_requested = true;
    }
    cv.notify_one();
}
