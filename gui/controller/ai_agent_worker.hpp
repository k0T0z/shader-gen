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

#ifndef AI_AGENT_WORKER_HPP
#define AI_AGENT_WORKER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <QImage>

#include "ai-agent/ai_agent.hpp"
#include "ai-agent/shared_memory.hpp"
#include "ai-agent/utils/image_extractor.hpp"
#include "gui/controller/ai_agent_monitor.hpp"

class AIAgentWorker {
public:
    AIAgentWorker(ShaderGenSharedMemory* shared_memory);
    ~AIAgentWorker();

    void start_matching();
    void stop_matching();

    void worker_main();

    void set_maximum_population_size(const int& maximum_population_size) { this->maximum_population_size = maximum_population_size; }
    void set_mutation_probability(const float& mutation_probability) { this->mutation_probability = mutation_probability; }
    void set_crossover_probability(const float& crossover_probability) { this->crossover_probability = crossover_probability; }
    void set_elitism_ratio(const float& elitism_ratio) { this->elitism_ratio = elitism_ratio; }
    void set_maximum_iterations(const int& maximum_iterations) { this->maximum_iterations = maximum_iterations; }

    void set_matching_type(const ai_agent_main::MatchingType& matching_type) { this->matching_type = matching_type; }
    void set_target_image(const QImage& target_image) { this->target_image = target_image; }
    
    void set_ai_agent_monitor(AIAgentMonitor* ai_agent_monitor) { this->ai_agent_monitor = ai_agent_monitor; }
    
private:
    std::thread worker;
    
    std::mutex mtx;
    std::condition_variable cv;
    int process_counter;
    std::atomic<bool> start_requested;
    std::atomic<bool> exit_requested;
    std::atomic<bool> stop_requested;

    int maximum_population_size;
    float mutation_probability;
    float crossover_probability;
    float elitism_ratio;
    int maximum_iterations;

    ai_agent_main::MatchingType matching_type;
    QImage target_image;

    ShaderGenSharedMemory* shared_memory;

    AIAgentMonitor* ai_agent_monitor;

    void stop_thread();
};

#endif // AI_AGENT_WORKER_HPP
