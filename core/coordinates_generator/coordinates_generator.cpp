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

#include "core/coordinates_generator/coordinates_generator.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "generator/graph_string_utils.hpp"

using namespace shadergen_graph_string_utils;

namespace shadergen_coordinates_generator {

namespace {

/**
 * @brief The rooted tree the layout is computed on.
 *
 * @c children maps a node id to the ids of the nodes that feed it, i.e. its
 * children in the tree oriented towards the shader output. @c roots holds the
 * ids of the nodes that have no parent. The graph is reduced to a spanning
 * tree: a node that feeds several parents is attached to the first parent that
 * reaches it.
 */
struct SpanningTree {
  std::unordered_map<int, std::vector<int>> children;
  std::vector<int> roots;
};

/**
 * @brief Reduces the raw graph to the rooted spanning tree the layout is
 *        computed on.
 *
 * Every node entity is formatted as
 * "0;<node_id>;<oneof_value_field_number>;<field_number>=<value>;...", so the
 * node id is its second token. The directed edges are recovered from the
 * connection entities, which are formatted as
 * "1;<connection_id>;<from_node_id>;<from_port_index>;<to_node_id>;<to_port_index>".
 */
SpanningTree build_spanning_tree(const std::string& graph) {
  SpanningTree tree;

  std::vector<std::string> node_entities;
  std::vector<std::string> connection_entities;
  for (const std::string& entity : split_string(graph, ',')) {
    if (entity.empty()) continue;
    const std::vector<std::string> tokens{split_string(entity, ';')};
    if (tokens.empty()) continue;
    const int entity_type{std::stoi(get_entity_type(tokens))};
    if (entity_type == 0) node_entities.push_back(entity);
    else if (entity_type == 1) connection_entities.push_back(entity);
  }

  std::vector<int> node_ids;
  node_ids.reserve(node_entities.size());
  for (const std::string& node_entity : node_entities) {
    const std::vector<std::string> tokens{split_string(node_entity, ';')};
    node_ids.push_back(std::stoi(get_node_entity_id(tokens)));
    tree.children[node_ids.back()];  // Make sure every node has an entry.
  }

  // A node feeds another node through an outgoing edge. Nodes without any
  // outgoing edge (e.g. the output node) can't be anybody's child, hence they
  // are the roots of the tree.
  std::unordered_set<int> has_outgoing_edge;
  std::unordered_map<int, std::vector<int>> feeders;  // to -> list of from.
  for (const std::string& connection_entity : connection_entities) {
    const std::vector<std::string> tokens{split_string(connection_entity, ';')};

    const int from{std::stoi(get_connection_entity_from_node_id(tokens))};
    const int to{std::stoi(get_connection_entity_to_node_id(tokens))};

    has_outgoing_edge.insert(from);
    feeders[to].push_back(from);
  }

  tree.roots.reserve(node_ids.size());
  for (const int& n_id : node_ids) {
    if (has_outgoing_edge.find(n_id) == has_outgoing_edge.end()) tree.roots.push_back(n_id);
  }

  // Keep the output node (id 0) as the primary root when it is present.
  const auto output_it{std::find(tree.roots.begin(), tree.roots.end(), 0)};
  if (output_it != tree.roots.end()) std::iter_swap(tree.roots.begin(), output_it);

  // Reduce the graph to a spanning tree: each node is attached to the first
  // parent that reaches it.
  std::unordered_set<int> visited;
  visited.reserve(node_ids.size());
  for (const int& root_id : tree.roots) visited.insert(root_id);

  std::vector<int> stack{tree.roots};
  while (!stack.empty()) {
    const int parent{stack.back()};
    stack.pop_back();

    const auto feeders_it{feeders.find(parent)};
    if (feeders_it == feeders.end()) continue;

    for (const int& child_id : feeders_it->second) {
      if (!visited.insert(child_id).second) continue;  // Already attached to another parent.

      tree.children[parent].push_back(child_id);
      stack.push_back(child_id);
    }
  }

  return tree;
}

/**
 * @brief Internal mutable node used by the Reingold-Tilford algorithm.
 *
 * @note @c prelim is the coordinate along the "sibling" axis (the vertical one
 *       in the final drawing) and @c mod is the offset that must be added to
 *       every member of the subtree.
 */
struct LayoutNode {
  int id{-1};

  std::vector<LayoutNode*> children;
  LayoutNode* parent{nullptr};
  LayoutNode* left_sibling{nullptr};

  int number{1};  // 1-based index among the siblings.

  double prelim{0.0};
  double mod{0.0};
  double change{0.0};
  double shift{0.0};

  LayoutNode* thread{nullptr};
  LayoutNode* ancestor{nullptr};  // Points to itself after construction.

  // Filled by the second walk, @c x holds the depth and @c y the vertical position.
  double x{0.0};
  double y{0.0};
};

LayoutNode* build_layout_tree(const int& id, const SpanningTree& tree,
                              std::unordered_map<int, LayoutNode*>& layout_nodes) {
  LayoutNode* node{new LayoutNode()};
  node->id = id;
  node->ancestor = node;
  layout_nodes[id] = node;

  const auto it{tree.children.find(id)};
  if (it == tree.children.end()) return node;

  for (const int& child_id : it->second) {
    // A node may be referenced by several parents but it belongs to a single
    // tree, so skip the ones already attached.
    if (layout_nodes.find(child_id) != layout_nodes.end()) continue;

    LayoutNode* child{build_layout_tree(child_id, tree, layout_nodes)};
    child->parent = node;
    child->number = static_cast<int>(node->children.size()) + 1;
    child->left_sibling = node->children.empty() ? nullptr : node->children.back();

    node->children.push_back(child);
  }

  return node;
}

LayoutNode* next_left(LayoutNode* v) { return v->children.empty() ? v->thread : v->children.front(); }
LayoutNode* next_right(LayoutNode* v) { return v->children.empty() ? v->thread : v->children.back(); }

void move_subtree(LayoutNode* w_minus, LayoutNode* w_plus, const double& shift) {
  const int subtrees{w_plus->number - w_minus->number};

  w_plus->change -= shift / subtrees;
  w_plus->shift += shift;
  w_minus->change += shift / subtrees;
  w_plus->prelim += shift;
  w_plus->mod += shift;
}

LayoutNode* ancestor(LayoutNode* v_iminus, LayoutNode* v, LayoutNode* default_ancestor) {
  return v_iminus->ancestor->parent == v->parent ? v_iminus->ancestor : default_ancestor;
}

LayoutNode* apportion(LayoutNode* v, LayoutNode* default_ancestor, const double& distance) {
  LayoutNode* w{v->left_sibling};

  if (w != nullptr) {
    LayoutNode* v_iminus{w};         // Inner left contour.
    LayoutNode* v_iplus{v};          // Inner right contour.
    LayoutNode* v_ominus{w};         // Outer left contour.
    LayoutNode* v_oplus{v};          // Outer right contour.

    double s_iminus{w->mod};
    double s_iplus{v->mod};
    double s_ominus{w->mod};
    double s_oplus{v->mod};

    while (next_right(v_iminus) != nullptr && next_left(v_iplus) != nullptr) {
      v_iminus = next_right(v_iminus);
      v_iplus = next_left(v_iplus);
      v_ominus = next_left(v_ominus);
      v_oplus = next_right(v_oplus);
      v_oplus->ancestor = v;

      const double shift{(v_iminus->prelim + s_iminus) - (v_iplus->prelim + s_iplus) + distance};
      if (shift > 0.0) {
        move_subtree(ancestor(v_iminus, v, default_ancestor), v, shift);
        s_iplus += shift;
        s_oplus += shift;
      }

      s_iminus += v_iminus->mod;
      s_iplus += v_iplus->mod;
      s_ominus += v_ominus->mod;
      s_oplus += v_oplus->mod;
    }

    if (next_right(v_iminus) != nullptr && next_right(v_oplus) == nullptr) {
      v_oplus->thread = next_right(v_iminus);
      v_oplus->mod += s_iminus - s_oplus;
    }

    if (next_left(v_iplus) != nullptr && next_left(v_ominus) == nullptr) {
      v_ominus->thread = next_left(v_iplus);
      v_ominus->mod += s_iplus - s_ominus;
      default_ancestor = v;
    }
  }

  return default_ancestor;
}

void execute_shifts(LayoutNode* v) {
  double shift{0.0};
  double change{0.0};

  for (auto it{v->children.rbegin()}; it != v->children.rend(); ++it) {
    LayoutNode* w{*it};
    w->prelim += shift;
    w->mod += shift;

    change += w->change;
    shift += w->shift + change;
  }
}

void first_walk(LayoutNode* v, const double& distance) {
  if (v->children.empty()) {
    v->prelim = v->left_sibling != nullptr ? v->left_sibling->prelim + distance : 0.0;
    return;
  }

  LayoutNode* default_ancestor{v->children.front()};
  for (LayoutNode* w : v->children) {
    first_walk(w, distance);
    default_ancestor = apportion(w, default_ancestor, distance);
  }
  execute_shifts(v);

  const double midpoint{(v->children.front()->prelim + v->children.back()->prelim) / 2.0};

  if (v->left_sibling != nullptr) {
    v->prelim = v->left_sibling->prelim + distance;
    v->mod = v->prelim - midpoint;
  } else {
    v->prelim = midpoint;
  }
}

void second_walk(LayoutNode* v, const double& m, const int& depth) {
  v->x = static_cast<double>(depth);
  v->y = v->prelim + m;

  for (LayoutNode* w : v->children) {
    second_walk(w, m + v->mod, depth + 1);
  }
}

}  // namespace

std::unordered_map<int, QPointF> generate_coordinates(const std::string& graph,
                                                      const double level_spacing,
                                                      const double sibling_spacing) {
  const SpanningTree tree{build_spanning_tree(graph)};

  std::unordered_map<int, QPointF> coordinates;

  double vertical_offset{0.0};

  for (const int& root_id : tree.roots) {
    std::unordered_map<int, LayoutNode*> layout_nodes;
    LayoutNode* root{build_layout_tree(root_id, tree, layout_nodes)};

    first_walk(root, sibling_spacing);
    second_walk(root, 0.0, 0);

    double min_y{root->y};
    double max_y{root->y};
    for (const auto& entry : layout_nodes) {
      min_y = std::min(min_y, entry.second->y);
      max_y = std::max(max_y, entry.second->y);
    }

    const double shift_y{vertical_offset - min_y};
    for (const auto& [n_id, node] : layout_nodes) {
      coordinates[n_id] = QPointF(-node->x * level_spacing, node->y + shift_y);
    }

    vertical_offset += (max_y - min_y) + sibling_spacing;

    for (const auto& entry : layout_nodes) delete entry.second;
  }

  return coordinates;
}

}  // namespace shadergen_coordinates_generator
