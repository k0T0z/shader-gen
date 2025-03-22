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
                                                                     maximum_generations(0),
                                                                     mutation_probability(0.0f), 
                                                                     crossover_probability(0.0f), 
                                                                     elitism_ratio(0.0f), 
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

    ShaderSampler* sampler{nullptr};

    bool is_first_run = true;
    int run_id = -1;

    while (true) {
        if (is_first_run) is_first_run = false;
        else shared_memory->set_is_stopped(true);

        // Wait for either start command or exit request
        cv.wait(lock, [this]() {
            return start_requested.load() || exit_requested.load();
        });

        if (exit_requested.load()) break;

        run_id++;

        if (start_requested.load()) start_requested.store(false);
        if (stop_requested.load()) stop_requested.store(false);

        CONTINUE_IF_TRUE(shared_memory == nullptr, "Shared memory is not set");
        CONTINUE_IF_TRUE(target_image.isNull(), "Target image is not set");

        if (!sampler) {
            sampler = new ShaderSampler();
            CONTINUE_IF_TRUE(!sampler->initialize(), "Failed to initialize the sampler");
        }

        CONTINUE_IF_TRUE(!sampler->is_initialized(), "Sampler is not initialized");

        // Process with interrupt checks
        bool completed = false;
        shared_memory->reset_broken_graphs_count();

        // Generate initial population
        const std::string encoded_graph = shared_memory->get_encoded_graph();

        std::vector<std::string> initial_population;
        CONTINUE_IF_TRUE(!ai_agent_main::init(
            matching_type,
            encoded_graph,
            maximum_population_size,
            initial_population
        ), "Failed to create initial population");

        std::vector<std::pair<std::string, unsigned long>> population_fitness;
        population_fitness.reserve(maximum_population_size);
        for (const auto& genome : initial_population) {
            const unsigned long fitness = ai_agent_main::get_fitness_value(
                genome, 
                sampler, 
                reinterpret_cast<const uint32_t*>(target_image.bits()),
                target_image.width(), 
                target_image.height()
            );
            population_fitness.emplace_back(genome, fitness);
            if (stop_requested.load()) {
                completed = true;
                break;
            }
        }
        SILENT_CONTINUE_IF_TRUE(completed);

        // Sort by fitness (lower is better)
        std::sort(population_fitness.begin(), population_fitness.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });
        shared_memory->set_best_individual(population_fitness.at(0));

        // Genetic algorithm loop
        for (int i {1}; (i < maximum_generations) && !completed; i++) {
            std::vector<std::pair<std::string, unsigned long>> new_population_fitness;
            new_population_fitness.reserve(maximum_population_size);

            // Create the new population
            for (int j {0}; (j < (int)((float)maximum_population_size * 0.5f)) && !completed; j++) {
                // Selection
                const std::pair<std::pair<std::string, unsigned long>, std::pair<std::string, unsigned long>> selected_parents = ai_agent_selection::select(population_fitness);
                
                // Crossover
                const std::pair<std::string, std::string> children = ai_agent_crossover::crossover(
                    selected_parents.first.first,
                    selected_parents.second.first,
                    crossover_probability
                );

                // Mutation
                const std::string mutated_child1 = ai_agent_mutation::mutate(children.first, mutation_probability);
                const std::string mutated_child2 = ai_agent_mutation::mutate(children.second, mutation_probability);

                // Evaluate the fitness of the children
                const unsigned long fitness_child1 = ai_agent_main::get_fitness_value(
                    mutated_child1,
                    sampler,
                    reinterpret_cast<const uint32_t*>(target_image.bits()),
                    target_image.width(),
                    target_image.height()
                );

                const unsigned long fitness_child2 = ai_agent_main::get_fitness_value(
                    mutated_child2,
                    sampler,
                    reinterpret_cast<const uint32_t*>(target_image.bits()),
                    target_image.width(),
                    target_image.height()
                );

                BREAK_IF_TRUE(new_population_fitness.size() >= maximum_population_size, "Population size exceeded");

                // Add the children to the new population
                if (fitness_child1 < fitness_child2) {
                    new_population_fitness.push_back(std::make_pair(mutated_child1, fitness_child1));
                    if (new_population_fitness.size() < maximum_population_size) {
                        new_population_fitness.push_back(std::make_pair(mutated_child2, fitness_child2));
                    }
                } else {
                    new_population_fitness.push_back(std::make_pair(mutated_child2, fitness_child2));
                    if (new_population_fitness.size() < maximum_population_size) {
                        new_population_fitness.push_back(std::make_pair(mutated_child1, fitness_child1));
                    }
                }

                if (stop_requested.load()) completed = true;
            }

            // Apply elitism
            population_fitness = ai_agent_elitism::apply_elitism(
                population_fitness,
                new_population_fitness,
                elitism_ratio
            );

            // Send the best individual to the shared memory
            shared_memory->set_best_individual(population_fitness.at(0));

            if (stop_requested.load()) completed = true;
        }

        DEBUG_PRINT("Run " + std::to_string(run_id) + " completed");
        DEBUG_PRINT("Best fitness: " + std::to_string(population_fitness.at(0).second));
        DEBUG_PRINT("Best individual: " + population_fitness.at(0).first);
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
