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

AIAgentWorker::AIAgentWorker() : process_counter(0), exit_requested(false) {
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

void AIAgentWorker::stop_matching() {}

void AIAgentWorker::worker_main() {
    std::unique_lock<std::mutex> lock(mtx);
    while (true) {
        // Wait for either start command or exit request
        cv.wait(lock, [this]() {
            return process_counter > 0 || exit_requested;
        });

        // Exit condition check
        if (exit_requested) break;

        // Process all pending requests
        while (process_counter > 0) {
            process_counter--;
            lock.unlock();
            
            // Main processing block
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            DEBUG_PRINT("Processing (" + oss.str() + ")...");
                    
            std::this_thread::sleep_for(
                std::chrono::milliseconds(500)); // Simulate work
            
            lock.lock();
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
