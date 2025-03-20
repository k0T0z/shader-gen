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
#include "gui/controller/shader_sampler.hpp"
#include "ai-agent/selection.hpp"
#include "ai-agent/crossover.hpp"
#include "ai-agent/mutation.hpp"
#include "ai-agent/elitism.hpp"

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

    ShaderSampler* sampler = new ShaderSampler();
    CHECK_CONDITION_TRUE(!sampler->initialize(), "Failed to initialize the image extractor");

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

        // Process with interrupt checks
        bool completed = false;

        // Generate initial population
        const std::string encoded_graph = shared_memory->get_encoded_graph();

        std::vector<std::string> initial_population;
        CONTINUE_IF_TRUE(!ai_agent_main::init(
            matching_type,
            encoded_graph,
            maximum_population_size,
            initial_population
        ), "Failed to create initial population");

        std::vector<unsigned long> fitness_values;
        fitness_values.resize(maximum_population_size);
        for (int i {0}; (i < maximum_population_size) && !completed; i++) {
            fitness_values.at(i) = ai_agent_main::get_fitness_value(
                initial_population.at(i),
                sampler,
                reinterpret_cast<const uint32_t*>(target_image.bits()),
                target_image.width(),
                target_image.height()
            );

            // Sleep for 200ms
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            if (stop_requested.load()) completed = true;
        }

        SILENT_CONTINUE_IF_TRUE(stop_requested.load());
        completed = false;

        // Create a vector of pairs of population and fitness values
        std::vector<std::pair<std::string, unsigned long>> population_fitness;
        population_fitness.resize(maximum_population_size);
        for (int i {0}; (i < maximum_population_size) && !completed; i++) {
            population_fitness.at(i) = std::make_pair(initial_population.at(i), fitness_values.at(i));

            if (stop_requested.load()) completed = true;
        }

        SILENT_CONTINUE_IF_TRUE(stop_requested.load());
        completed = false;

        // Sort the population based on fitness values
        std::sort(population_fitness.begin(), population_fitness.end(), [](const std::pair<std::string, unsigned long>& a, const std::pair<std::string, unsigned long>& b) {
            return a.second < b.second; // Ascending order
        });

        // Send the best individual to the shared memory
        shared_memory->set_encoded_graph(population_fitness.at(0).first);

        // Main loop
        // Starts from one because the initial population is already evaluated
        for (int i {1}; (i < maximum_population_size) && !completed; i++) {
            const std::pair<std::pair<std::string, unsigned long>, std::pair<std::string, unsigned long>> selected_parents = ai_agent_selection::select(population_fitness);

            if (stop_requested.load()) completed = true;
        }

        SILENT_CONTINUE_IF_TRUE(stop_requested.load());
        completed = false;

    }

    delete sampler; // TODO: Deleting this object causes a crash
    DEBUG_PRINT("Worker thread exiting cleanly");
}

void AIAgentWorker::stop_thread() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_requested = true;
    }
    cv.notify_one();
}
