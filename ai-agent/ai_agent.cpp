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

#include "ai-agent/ai_agent.hpp"

#include <sstream>

#include "error_macros.hpp"
#include "ai-agent/utils/utils.hpp"

AIAgentWorker::AIAgentWorker() : process_counter(0), 
                                 exit_requested(false),
                                 stop_requested(false),
                                 mutation_probability(0.0f), 
                                 crossover_probability(0.0f), 
                                 elitism_ratio(0.0f), 
                                 maximum_iterations(0),
                                 fitness_calculator(nullptr),
                                 matching_type(MatchingType::PARAMETERS_ONLY),
                                 scene(nullptr) {
    worker = std::thread(&AIAgentWorker::worker_main, this);
}

AIAgentWorker::~AIAgentWorker() {
    stop_thread();
    worker.join();
}

void AIAgentWorker::start_matching() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        process_counter++;
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
    while (true) {
        // Wait for either start command or exit request
        cv.wait(lock, [this]() {
            return process_counter > 0 || exit_requested;
        });

        // Exit condition check
        if (exit_requested) break;

        // It doesn't make sense to stop before even starting
        if (stop_requested.load()) stop_requested.store(false);

        CONTINUE_IF_TRUE(fitness_calculator == nullptr, "Fitness calculator is not set");
        CONTINUE_IF_TRUE(scene == nullptr, "Scene is not set");

        // Process all pending requests
        while (process_counter > 0) {
            process_counter--;
            lock.unlock();

            // Process with interrupt checks
            bool completed = false;
            auto start = std::chrono::steady_clock::now();
            
            // Work simulation with interrupt checks
            while (!completed) {
                // Do chunk of work
                DEBUG_PRINT("Processing...");
                
                // Check for stop every 100ms
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                // Check completion condition
                auto now = std::chrono::steady_clock::now();
                if (now - start > std::chrono::milliseconds(500)) {
                    completed = true;
                }

                // Check for stop request
                if (stop_requested.load()) {
                    DEBUG_PRINT("Interrupting current work");
                    completed = true;
                }
            }

            lock.lock();
            
            // Exit processing loop if stop requested
            if (stop_requested.load()) {
                stop_requested.store(false);
                process_counter = 0; // Clear remaining tasks
                break;
            }
        }
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
