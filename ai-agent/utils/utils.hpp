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

#ifndef AI_AGENT_UTILS_HPP
#define AI_AGENT_UTILS_HPP

#include "gui/model/schema/visual_shader.pb.h"

#include <vector>

#include "error_macros.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/controller/vs_proto_node.hpp"
#include "gui/model/oneof_model.hpp"

using VisualShader = gui::model::schema::VisualShader;

namespace ai_agent_utils {
inline static std::vector<int> get_node_type_population() {
    std::vector<int> node_type_population;

    node_type_population.emplace_back(VisualShader::VisualShaderNode::kInputFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kFloatConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIntConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kUintConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kColorConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kFloatOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIntOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kUintOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorOpFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kFloatFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIntFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kUintFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorFuncFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kValueNoiseFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kDotProductFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorLenFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kClampFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVectorDistanceFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector2DComposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector3DComposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector4DComposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIfNodeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kSwitchNodeFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kIsFieldNumber);
    node_type_population.emplace_back(VisualShader::VisualShaderNode::kCompareFieldNumber);

    return node_type_population;
}

inline static std::unordered_map<int, std::string> encode_nodes(const ProtoModel* nodes) {
    std::unordered_map<int, std::string> encoded_nodes;
    
    // Cast to ReapeatedMessageModel
    const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, encoded_nodes, "Nodes is not a repeated message model.");
    
    int size{nodes->rowCount()};
    for (int i{0}; i < size; ++i) {
        const MessageModel* node_model{repeated_nodes->get_sub_model(i)};

        const int n_id{node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)))->data().toInt()};

        CHECK_CONDITION_TRUE_NON_VOID(encoded_nodes.find(n_id) != encoded_nodes.end(), encoded_nodes, "Node id already exists.");

        // I don't care about the field number, just send any field number inside the oneof model you want to get
        const ProtoModel* oneof_model{
            node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                        FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                    false, true)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model, encoded_nodes, "Oneof Model is nullptr.");
        const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

        std::string encoded_node;
        encoded_node += "0;"; // 0 means Node and 1 means Connection
        encoded_node += std::to_string(n_id) + ';';
        encoded_node += std::to_string(oneof_value_field_number) + ';';

        // Cast to OneofModel
        const OneofModel* oneof_model_casted{dynamic_cast<const OneofModel*>(oneof_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model_casted, encoded_nodes, "Oneof Model is not a OneofModel.");

        const ProtoModel* node_type_model{oneof_model_casted->get_sub_model(oneof_value_field_number)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model, encoded_nodes, "Node type model is nullptr.");

        // Cast to MessageModel
        const MessageModel* node_type_model_casted{dynamic_cast<const MessageModel*>(node_type_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model_casted, encoded_nodes, "Node type model is not a MessageModel.");

        const int field_count{node_type_model->columnCount()};
        for (int j{0}; j < field_count; ++j) {
            const ProtoModel* field_model{node_type_model_casted->get_sub_model_by_index(j)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_model, encoded_nodes, "Field model is nullptr.");

            const QVariant field_value{field_model->data()};
            encoded_node += field_value.toString().toStdString() + ';';
        }

        encoded_nodes[n_id] = encoded_node;
    }

    return encoded_nodes;
}

inline static std::unordered_map<int, std::string> encode_connections(const ProtoModel* connections) {
    std::unordered_map<int, std::string> encoded_connections;

    // Cast to ReapeatedMessageModel
    const RepeatedMessageModel* repeated_connections{dynamic_cast<const RepeatedMessageModel*>(connections)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_connections, encoded_connections, "Connections is not a repeated message model.");

    int size{connections->rowCount()};
    for (int i{0}; i < size; ++i) {
        const MessageModel* connection_model{repeated_connections->get_sub_model(i)};

        const int c_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber)))->data().toInt()};

        CHECK_CONDITION_TRUE_NON_VOID(encoded_connections.find(c_id) != encoded_connections.end(), encoded_connections, "Connection id already exists.");

        const int from_node_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber)))->data().toInt()};
        const int from_port_index{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber)))->data().toInt()};
        const int to_node_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber)))->data().toInt()};
        const int to_port_index{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber)))->data().toInt()};

        std::string encoded_connection;
        encoded_connection += "1;"; // 0 means Node and 1 means Connection
        encoded_connection += std::to_string(c_id) + ';';
        encoded_connection += std::to_string(from_node_id) + ';';
        encoded_connection += std::to_string(from_port_index) + ';';
        encoded_connection += std::to_string(to_node_id) + ';';
        encoded_connection += std::to_string(to_port_index) + ';';

        encoded_connections[c_id] = encoded_connection;
    }

    return encoded_connections;
}
}  // namespace ai_agent_utils

#endif  // AI_AGENT_UTILS_HPP
