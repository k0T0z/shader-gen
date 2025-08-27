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

#ifndef GRAPH_TABLE_EDITOR_HPP
#define GRAPH_TABLE_EDITOR_HPP

#include <vector>
#include <unordered_map>
#include <variant>

struct GraphTableNode {
    int n_id;
    int field_number;
    std::unordered_map<int, std::variant<int, float, double>> params;
};

struct GraphTableConnection {
    int c_id;
    int from_p_index;
    int to_p_index;
    bool is_valid; // Can this connection be connected and the graph stay valid?
};

struct GraphTable {
    std::vector<GraphTableNode> nodes;
    std::vector<std::vector<GraphTableConnection>> connections;
};

#endif // GRAPH_TABLE_EDITOR_HPP
