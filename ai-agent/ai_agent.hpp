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

#ifndef AI_AGENT_HPP
#define AI_AGENT_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <QGraphicsScene>

#include "gui/controller/fitness_calculator.hpp"

class AIAgentWorker {
public:
    enum class MatchingType {
        PARAMETERS_ONLY,
        PARAMETERS_AND_CONNECTIONS,
        FULL_GRAPH,
    };

    AIAgentWorker();
    ~AIAgentWorker();

    void start_matching();
    void stop_matching();

    void worker_main();

    void set_mutation_probability(const float& mutation_probability) { this->mutation_probability = mutation_probability; }
    void set_crossover_probability(const float& crossover_probability) { this->crossover_probability = crossover_probability; }
    void set_elitism_ratio(const float& elitism_ratio) { this->elitism_ratio = elitism_ratio; }
    void set_maximum_iterations(const int& maximum_iterations) { this->maximum_iterations = maximum_iterations; }

    void set_fitness_calculator(AIAgentFitnessCalculator* fitness_calculator) { this->fitness_calculator = fitness_calculator; }

    void set_matching_type(const MatchingType& matching_type) { this->matching_type = matching_type; }

    void set_scene(QGraphicsScene* scene) { this->scene = scene; }
    
private:
    std::thread worker;
    
    std::mutex mtx;
    std::condition_variable cv;
    int process_counter;
    bool exit_requested;
    std::atomic<bool> stop_requested;

    float mutation_probability;
    float crossover_probability;
    float elitism_ratio;
    int maximum_iterations;

    AIAgentFitnessCalculator* fitness_calculator;

    MatchingType matching_type;

    QGraphicsScene* scene;

    void stop_thread();

    std::vector<std::pair<std::unordered_map<int, std::string>, std::unordered_map<int, std::string>>> generate_population(const int& population_size);
};

#endif // AI_AGENT_HPP
