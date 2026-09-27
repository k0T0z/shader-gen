/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2026 Seif Kandil (k0T0z)                                       */
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

#include "gui/adapters/raw_visual_shader_graph_adapter.hpp"

#include <sstream>
#include <unordered_map>

#include "error_macros.hpp"
#include "gui/model/schema/visual_shader.pb.h"
#include "gui/model/repeated_message_model.hpp"
#include "gui/model/oneof_model.hpp"
#include "gui/model/utils/field_path.hpp"

using VisualShader = gui::model::schema::VisualShader;

namespace {

inline static std::vector<std::string> split_string_local(const std::string& str, const char& delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) tokens.push_back(token);
    return tokens;
}

inline static std::string join_string_local(const std::vector<std::string>& tokens, const char& delimiter) {
    if (tokens.empty()) return "";
    if (tokens.size() == 1) return tokens.at(0);

    std::ostringstream joined;
    joined << tokens.at(0);
    for (std::size_t i = 1; i < tokens.size(); ++i) joined << delimiter << tokens.at(i);
    return joined.str();
}

}  // namespace

namespace shadergen_gui_adapters {

RawVisualShaderGraph to_raw_graph(const ProtoModel* nodes, const ProtoModel* connections) noexcept {
    RawVisualShaderGraph graph;

    CHECK_PARAM_NULLPTR_NON_VOID(nodes, graph, "Nodes model is nullptr.");
    CHECK_PARAM_NULLPTR_NON_VOID(connections, graph, "Connections model is nullptr.");

    const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, graph, "Nodes is not a repeated message model.");

    std::unordered_map<int, int> node_id_to_index;

    const int n_size{nodes->rowCount()};
    graph.headers.reserve(n_size);
    node_id_to_index.reserve(n_size);

    for (int i{0}; i < n_size; ++i) {
        const MessageModel* node_model{repeated_nodes->get_sub_model(i)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_model, graph, "Node model is nullptr.");

        const int n_id{node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)))->data().toInt()};

        const ProtoModel* oneof_model{
            node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                        FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                    false, true)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model, graph, "Oneof Model is nullptr.");
        const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

        const OneofModel* oneof_model_casted{dynamic_cast<const OneofModel*>(oneof_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model_casted, graph, "Oneof Model is not a OneofModel.");

        const ProtoModel* node_type_model{oneof_model_casted->get_sub_model(oneof_value_field_number)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model, graph, "Node type model is nullptr.");

        const MessageModel* node_type_model_casted{dynamic_cast<const MessageModel*>(node_type_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model_casted, graph, "Node type model is not a MessageModel.");

        const int field_count{node_type_model->columnCount()};

        node_id_to_index[n_id] = i;

        if (field_count == 0) {
            graph.headers.push_back(join_string_local({"0", std::to_string(n_id), std::to_string(oneof_value_field_number)}, ';'));
            continue;
        }

        std::vector<std::string> parameters;
        parameters.reserve(field_count);

        for (int j{0}; j < field_count; ++j) {
            const ProtoModel* field_model{node_type_model_casted->get_sub_model_by_index(j)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_model, graph, "Field model is nullptr.");

            const FieldDescriptor* field_descriptor{field_model->get_column_descriptor(0)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_descriptor, graph, "Field descriptor is nullptr.");

            const int field_number{field_descriptor->number()};

            const QVariant field_value{field_model->data()};
            parameters.push_back(join_string_local({std::to_string(field_number), field_value.toString().toStdString()}, '='));
        }

        graph.headers.push_back(join_string_local({"0", std::to_string(n_id), std::to_string(oneof_value_field_number), join_string_local(parameters, ';')}, ';'));
    }

    const std::size_t N{graph.headers.size()};
    graph.adj_matrix.assign(N, std::vector<std::string>(N, ""));

    const RepeatedMessageModel* repeated_connections{dynamic_cast<const RepeatedMessageModel*>(connections)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_connections, graph, "Connections is not a repeated message model.");

    const int c_size{connections->rowCount()};
    for (int i{0}; i < c_size; ++i) {
        const MessageModel* connection_model{repeated_connections->get_sub_model(i)};
        CHECK_PARAM_NULLPTR_NON_VOID(connection_model, graph, "Connection model is nullptr.");

        const int from_node_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber)))->data().toInt()};
        const int from_port_index{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber)))->data().toInt()};
        const int to_node_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber)))->data().toInt()};
        const int to_port_index{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber)))->data().toInt()};

        const auto from_it{node_id_to_index.find(from_node_id)};
        const auto to_it{node_id_to_index.find(to_node_id)};
        if (from_it == node_id_to_index.end() || to_it == node_id_to_index.end()) {
            WARN_PRINT("Connection references unknown node id: from=" + std::to_string(from_node_id) + ", to=" + std::to_string(to_node_id));
            continue;
        }

        const int from_idx{from_it->second};
        const int to_idx{to_it->second};

        const std::string fragment{join_string_local({std::to_string(from_port_index), std::to_string(to_port_index)}, ',')};

        std::string& cell{graph.adj_matrix[static_cast<std::size_t>(from_idx)][static_cast<std::size_t>(to_idx)]};
        if (cell.empty()) {
            cell = fragment;
        } else {
            cell = join_string_local({cell, fragment}, ';');
        }
    }

    return graph;
}

}  // namespace shadergen_gui_adapters
