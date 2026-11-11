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

#ifndef CONTROLLER_UTILS_HPP
#define CONTROLLER_UTILS_HPP

#include "gui/model/schema/visual_shader.pb.h"
#include "gui/model/schema/visual_shader_nodes.pb.h"

#include <string>
#include <vector>

#include "error_macros.hpp"
#include "generator/graph_string_utils.hpp"
#include "gui/model/message_model.hpp"
#include "gui/model/oneof_model.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/model/utils/field_path.hpp"
#include "gui/model/utils/utils.hpp"

namespace controller_utils {

using VisualShader = gui::model::schema::VisualShader;

/**
 * @brief Encode a Qt-backed visual shader model into the semicolon/comma delimited graph string.
 *
 * @note This is the GUI/Qt counterpart of the (Qt-free) utilities in @c ai-agent/utils/utils.hpp. It
 *       is kept here so that the AI agent utilities can be consumed without any Qt dependency.
 */
inline static std::string encode_graph(const ProtoModel* nodes, const ProtoModel* connections) {
    std::vector<std::string> encoded_graph;
    encoded_graph.reserve(nodes->rowCount() + connections->rowCount());

    // Cast to ReapeatedMessageModel
    const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, "", "Nodes is not a repeated message model.");
    
    int n_size{nodes->rowCount()};
    for (int i{0}; i < n_size; ++i) {
        const MessageModel* node_model{repeated_nodes->get_sub_model(i)};

        const int n_id{node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
            FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)))->data().toInt()};

        // I don't care about the field number, just send any field number inside the oneof model you want to get
        const ProtoModel* oneof_model{
            node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                        FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                    false, true)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model, "", "Oneof Model is nullptr.");
        const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

        // Cast to OneofModel
        const OneofModel* oneof_model_casted{dynamic_cast<const OneofModel*>(oneof_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model_casted, "", "Oneof Model is not a OneofModel.");

        const ProtoModel* node_type_model{oneof_model_casted->get_sub_model(oneof_value_field_number)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model, "", "Node type model is nullptr.");

        // Cast to MessageModel
        const MessageModel* node_type_model_casted{dynamic_cast<const MessageModel*>(node_type_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model_casted, "", "Node type model is not a MessageModel.");

        const int field_count{node_type_model->columnCount()};

        std::vector<std::string> parameters;
        parameters.reserve(field_count + Matrix3x3::descriptor()->field_count());

        for (int j{0}; j < field_count; ++j) {
            const ProtoModel* field_model{node_type_model_casted->get_sub_model_by_index(j)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_model, "", "Field model is nullptr.");

            const FieldDescriptor* field_descriptor{node_type_model_casted->get_column_descriptor(j)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_descriptor, "", "Field descriptor is nullptr.");

            const int field_number{field_descriptor->number()};

            if (field_descriptor->is_repeated() && field_descriptor->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
                const RepeatedMessageModel* repeated_field_model{dynamic_cast<const RepeatedMessageModel*>(field_model)};
                CHECK_PARAM_NULLPTR_NON_VOID(repeated_field_model, "", "Repeated field model is nullptr.");

                // A repeated field is a set of parameters keyed by
                // "<field number>:<row>:<column field number>" so that every value inside a repeated
                // field behaves like an ordinary node parameter.
                for (int row{0}; row < repeated_field_model->rowCount(); ++row) {
                    const MessageModel* row_model{repeated_field_model->get_sub_model(row)};
                    CHECK_PARAM_NULLPTR_NON_VOID(row_model, "", "Repeated field row model is nullptr.");

                    for (int column{0}; column < row_model->columnCount(); ++column) {
                        const ProtoModel* column_model{row_model->get_sub_model_by_index(column)};
                        CHECK_PARAM_NULLPTR_NON_VOID(column_model, "", "Repeated field column model is nullptr.");

                        const FieldDescriptor* column_descriptor{row_model->get_column_descriptor(column)};
                        CHECK_PARAM_NULLPTR_NON_VOID(column_descriptor, "", "Repeated field column descriptor is nullptr.");

                        QVariant column_value{column_model->data()};
                        if (!column_value.isValid()) {
                            column_value = shadergen_utils::get_proto_field_default_value(column_descriptor);
                        }

                        const std::string key{std::to_string(field_number) + ':' + std::to_string(row) + ':' + std::to_string(column_descriptor->number())};
                        parameters.push_back(shadergen_graph_string_utils::join_string(
                            {key, column_value.toString().toStdString()}, '='));
                    }
                }

                continue;
            }

            // A singular message field (the coordinate consuming nodes' Matrix3x3 transformation) is a
            // set of parameters keyed by "<field number>:<column field number>" (e.g. "4:1=1" for m00),
            // i.e. exactly like a value inside a repeated message field but without the row index. The
            // identity matrix is omitted so that an untransformed node carries no redundant parameters.
            if (field_descriptor->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE &&
                field_descriptor->message_type() == Matrix3x3::descriptor()) {
                const MessageModel* matrix_model{dynamic_cast<const MessageModel*>(field_model)};
                CHECK_PARAM_NULLPTR_NON_VOID(matrix_model, "", "Matrix3x3 field model is not a MessageModel.");

                std::vector<std::string> encoded_values(matrix_model->columnCount());
                bool is_identity{true};

                for (int column{0}; column < matrix_model->columnCount(); ++column) {
                    const ProtoModel* column_model{matrix_model->get_sub_model_by_index(column)};
                    CHECK_PARAM_NULLPTR_NON_VOID(column_model, "", "Matrix3x3 column model is nullptr.");

                    const FieldDescriptor* column_descriptor{matrix_model->get_column_descriptor(column)};
                    CHECK_PARAM_NULLPTR_NON_VOID(column_descriptor, "", "Matrix3x3 column descriptor is nullptr.");

                    // The diagonal of the identity matrix is one, every other entry is zero. An entry
                    // that was never set (e.g. a node whose transformation was not seeded into the
                    // model) is an identity entry, so an untransformed node carries no parameters.
                    const bool is_diagonal{column_descriptor->number() == Matrix3x3::kM00FieldNumber ||
                                           column_descriptor->number() == Matrix3x3::kM11FieldNumber ||
                                           column_descriptor->number() == Matrix3x3::kM22FieldNumber};
                    const float identity_entry{is_diagonal ? 1.0f : 0.0f};

                    QVariant column_value{column_model->data()};
                    if (!column_value.isValid()) column_value = identity_entry;

                    if (column_value.toFloat() != identity_entry) is_identity = false;

                    encoded_values.at(column) = shadergen_graph_string_utils::join_string(
                        {std::to_string(field_number) + ':' + std::to_string(column_descriptor->number()),
                         column_value.toString().toStdString()},
                        '=');
                }

                SILENT_CONTINUE_IF_TRUE(is_identity);

                parameters.insert(parameters.end(), encoded_values.begin(), encoded_values.end());
                continue;
            }

            const QVariant field_value{field_model->data()};
            parameters.push_back(shadergen_graph_string_utils::join_string({std::to_string(field_number), field_value.toString().toStdString()}, '='));
        }

        if (parameters.empty()) {
            encoded_graph.push_back(shadergen_graph_string_utils::join_string({"0", std::to_string(n_id), std::to_string(oneof_value_field_number)}, ';'));
            continue;
        }

        encoded_graph.push_back(shadergen_graph_string_utils::join_string({"0", std::to_string(n_id), std::to_string(oneof_value_field_number), shadergen_graph_string_utils::join_string(parameters, ';')}, ';'));
    }

    // Cast to ReapeatedMessageModel
    const RepeatedMessageModel* repeated_connections{dynamic_cast<const RepeatedMessageModel*>(connections)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_connections, "", "Connections is not a repeated message model.");

    int c_size{connections->rowCount()};
    for (int i{0}; i < c_size; ++i) {
        const MessageModel* connection_model{repeated_connections->get_sub_model(i)};

        const int c_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber)))->data().toInt()};

        const int from_node_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber)))->data().toInt()};
        const int from_port_index{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber)))->data().toInt()};
        const int to_node_id{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber)))->data().toInt()};
        const int to_port_index{connection_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderConnection>(
            FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber)))->data().toInt()};

        encoded_graph.push_back(shadergen_graph_string_utils::join_string({"1", std::to_string(c_id), std::to_string(from_node_id), std::to_string(from_port_index), std::to_string(to_node_id), std::to_string(to_port_index)}, ';'));
    }

    return shadergen_graph_string_utils::join_string(encoded_graph, ',');
}

inline static bool is_valid_connection(const VisualShaderNodePortType& source_port,
                                       const VisualShaderNodePortType& target_port) noexcept {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(source_port == VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, false);
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(target_port == VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, false);
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(source_port == target_port, true);

  switch (source_port) {
    case VisualShaderNodePortType::PORT_TYPE_SCALAR:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          return false;
      }
    case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          return false;
      }
    case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          return false;
      }
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          return false;
      }
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          return false;
      }
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          return false;
      }
    case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
      switch (target_port) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
          return true;
        default:
          return false;
      }
    default:
      break;
  }

  return false;
}
}  // namespace controller_utils

#endif  // CONTROLLER_UTILS_HPP
