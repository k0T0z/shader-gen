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

#ifndef AI_AGENT_FITNESS_HPP
#define AI_AGENT_FITNESS_HPP

#include <cstdint>
#include <cstdlib> // for std::abs

namespace ai_agent_fitness {
// Calculates the sum of per-channel absolute differences between two images.
// Each pixel is expected to be in ARGB32 (0xAARRGGBB) format.
inline static unsigned long calculate_fitness(const uint32_t* pixels1, const uint32_t* pixels2, int width, int height) {
    unsigned long total_fitness{0};
    int num_pixels{width * height};
    
    for (int i {0}; i < num_pixels; ++i) {
        uint32_t p1 = pixels1[i];
        uint32_t p2 = pixels2[i];

        // Extract ARGB components from each pixel.
        unsigned char a1 = (p1 >> 24) & 0xFF;
        unsigned char r1 = (p1 >> 16) & 0xFF;
        unsigned char g1 = (p1 >> 8)  & 0xFF;
        unsigned char b1 = p1 & 0xFF;
        
        unsigned char a2 = (p2 >> 24) & 0xFF;
        unsigned char r2 = (p2 >> 16) & 0xFF;
        unsigned char g2 = (p2 >> 8)  & 0xFF;
        unsigned char b2 = p2 & 0xFF;
        
        // Add up differences for all channels.
        total_fitness += std::abs(int(a1) - int(a2));
        total_fitness += std::abs(int(r1) - int(r2));
        total_fitness += std::abs(int(g1) - int(g2));
        total_fitness += std::abs(int(b1) - int(b2));
    }
    
    return total_fitness;
}
}  // namespace ai_agent_fitness

#endif // AI_AGENT_FITNESS_HPP
