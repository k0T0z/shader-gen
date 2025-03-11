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

#ifndef SHADER_GEN_SHARED_MEMORY_HPP
#define SHADER_GEN_SHARED_MEMORY_HPP

#include <mutex>
#include <string>

class ShaderGenSharedMemory {
public:
    ShaderGenSharedMemory() : is_stopped(false) {}

    void set_encoded_graph(const std::string& graph) {
        std::lock_guard<std::mutex> lock(mutex);
        encoded_graph = graph;
    }

    std::string get_encoded_graph() const {
        std::lock_guard<std::mutex> lock(mutex);
        return encoded_graph;
    }

    void set_is_stopped(const bool& stopped) {
        std::lock_guard<std::mutex> lock(mutex);
        is_stopped = stopped;
    }

    bool get_is_stopped() const {
        std::lock_guard<std::mutex> lock(mutex);
        return is_stopped;
    }

private:
    mutable std::mutex mutex;
    std::string encoded_graph;
    bool is_stopped;
};

#endif // SHADER_GEN_SHARED_MEMORY_HPP
