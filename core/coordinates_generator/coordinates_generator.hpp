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

// Reingold-Tilford algorithm
// https://reingold.co/tidier-drawings.pdf

#ifndef COORDINATES_GENERATOR_HPP
#define COORDINATES_GENERATOR_HPP

#include <QPointF>

#include <string>
#include <unordered_map>

namespace shadergen_coordinates_generator {

// Spacing defaults, expressed in scene units.
inline constexpr double DEFAULT_LEVEL_SPACING = 320.0;    // Distance between two consecutive levels (columns).
inline constexpr double DEFAULT_SIBLING_SPACING = 200.0;  // Minimum distance between two sibling subtrees (rows).

/**
 * @brief Assigns a coordinate to every node of the given raw graph using the
 *        Reingold-Tilford algorithm.
 *
 * The graph is a comma separated list of entities where every entity is
 * semicolon separated:
 *   - Nodes:       "0;<node_id>;<oneof_value_field_number>;<field_number>=<value>;..."
 *   - Connections: "1;<connection_id>;<from_node_id>;<from_port_index>;<to_node_id>;<to_port_index>"
 *
 * The graph is reduced to a rooted spanning tree oriented towards the shader
 * output: a node's children are the nodes that feed it and the nodes without an
 * outgoing edge (e.g. the output node) are used as roots, so the drawing reads
 * from the inputs (left) to the output (right).
 *
 * Each root is placed at @c x = 0 and the deeper levels extend towards the left
 * (negative @c x), while siblings are spread vertically, so the resulting
 * drawing is a tidy, non-overlapping tree. Independent trees (disconnected
 * components) are stacked vertically.
 *
 * @param graph           The raw visual shader graph string to lay out.
 * @param level_spacing   Horizontal distance between two consecutive levels.
 * @param sibling_spacing Minimum vertical distance between sibling subtrees.
 *
 * @return A map from node id to its assigned coordinate.
 */
std::unordered_map<int, QPointF> generate_coordinates(const std::string& graph,
                                                      double level_spacing = DEFAULT_LEVEL_SPACING,
                                                      double sibling_spacing = DEFAULT_SIBLING_SPACING);

}  // namespace shadergen_coordinates_generator

#endif  // COORDINATES_GENERATOR_HPP
