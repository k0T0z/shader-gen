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
#include <random>
#include <limits>
#include <cmath>  // for std::nextafter
#include <type_traits>
#include <variant>
#include <sstream>
#include <string>

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

inline static std::string encode_graph(const ProtoModel* nodes, const ProtoModel* connections) {
    std::string encoded_graph;

    // Cast to ReapeatedMessageModel
    const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, encoded_graph, "Nodes is not a repeated message model.");
    
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
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model, encoded_graph, "Oneof Model is nullptr.");
        const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

        std::string encoded_node;
        encoded_node += "0;"; // 0 means Node and 1 means Connection
        encoded_node += std::to_string(n_id) + ';';
        encoded_node += std::to_string(oneof_value_field_number) + ';';

        // Cast to OneofModel
        const OneofModel* oneof_model_casted{dynamic_cast<const OneofModel*>(oneof_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(oneof_model_casted, encoded_graph, "Oneof Model is not a OneofModel.");

        const ProtoModel* node_type_model{oneof_model_casted->get_sub_model(oneof_value_field_number)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model, encoded_graph, "Node type model is nullptr.");

        // Cast to MessageModel
        const MessageModel* node_type_model_casted{dynamic_cast<const MessageModel*>(node_type_model)};
        CHECK_PARAM_NULLPTR_NON_VOID(node_type_model_casted, encoded_graph, "Node type model is not a MessageModel.");

        const int field_count{node_type_model->columnCount()};
        for (int j{0}; j < field_count; ++j) {
            const ProtoModel* field_model{node_type_model_casted->get_sub_model_by_index(j)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_model, encoded_graph, "Field model is nullptr.");

            const FieldDescriptor* field_descriptor{field_model->get_column_descriptor(0)};
            CHECK_PARAM_NULLPTR_NON_VOID(field_descriptor, encoded_graph, "Field descriptor is nullptr.");

            const int field_number{field_descriptor->number()};

            const QVariant field_value{field_model->data()};
            encoded_node += std::to_string(field_number) + '=' + field_value.toString().toStdString() + ';';
        }

        encoded_node.pop_back(); // Remove the last semicolon

        encoded_graph += encoded_node + ',';
    }

    // Cast to ReapeatedMessageModel
    const RepeatedMessageModel* repeated_connections{dynamic_cast<const RepeatedMessageModel*>(connections)};
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_connections, encoded_graph, "Connections is not a repeated message model.");

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

        std::string encoded_connection;
        encoded_connection += "1;"; // 0 means Node and 1 means Connection
        encoded_connection += std::to_string(c_id) + ';';
        encoded_connection += std::to_string(from_node_id) + ';';
        encoded_connection += std::to_string(from_port_index) + ';';
        encoded_connection += std::to_string(to_node_id) + ';';
        encoded_connection += std::to_string(to_port_index);

        encoded_graph += encoded_connection + ',';
    }

    if (!encoded_graph.empty()) encoded_graph.pop_back(); // Remove the last comma

    return encoded_graph;
}

//------------------------------------------------------------------------------
// Example: Using a non-uniform distribution
//
// With the generic random_value function you can generate random numbers
// from any distribution available in the STL (or even user-defined ones). For
// example, to sample from a normal distribution:
//
/*
   double sample = random_value<std::normal_distribution<double>>(0.0, 1.0);
*/
// Similarly, you can use
//   std::bernoulli_distribution, std::binomial_distribution,
//   std::poisson_distribution, std::exponential_distribution, etc.
//
//------------------------------------------------------------------------------

// Get a reference to a static random engine.
inline static std::mt19937& rand_engine() {
    // Seed the engine with a random device only once.
    static std::random_device rd;
    static std::mt19937 eng(rd());
    return eng;
}

//------------------------------------------------------------------------------
// Generic random value generator
//
// This template accepts any distribution type (from the STL random library,
// such as uniform_int_distribution, normal_distribution, etc.) along with its
// constructor parameters. It creates an instance of the distribution and returns
// a random sample from it.
//------------------------------------------------------------------------------
template <typename Distribution, typename... Args>
inline static auto random_value(Args&&... args)
        -> decltype(Distribution(std::forward<Args>(args)...)(rand_engine())) {
    Distribution dist(std::forward<Args>(args)...);
    return dist(rand_engine());
}

//------------------------------------------------------------------------------
// Specialized functions for floating-point random generation
//
// These functions include extra endpoint adjustments using std::nextafter.
//------------------------------------------------------------------------------

// Returns a random number in [a, b] – both endpoints included.
template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline static T random_real_inclusive(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a > b, a, "Invalid range for [a, b]");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for [a, b]");
    // Adjust upper so that b is included.
    const T upper = std::nextafter(b, std::numeric_limits<T>::max());
    return random_value<std::uniform_real_distribution<T>>(a, upper);
}

// Returns a random number in (a, b) – both endpoints excluded.
template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline static T random_real_exclusive(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a + std::numeric_limits<T>::epsilon() > b - std::numeric_limits<T>::epsilon(), a, "Invalid range for (a, b)");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for (a, b)");
    const T lower = std::nextafter(a, b);
    const T upper = std::nextafter(b, a);  // largest representable < b
    return random_value<std::uniform_real_distribution<T>>(lower, upper);
}

// Returns a random number in [a, b) – include first, exclude second.
template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline static T random_real_include_first_exclude_second(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a > b - std::numeric_limits<T>::epsilon(), a, "Invalid range for [a, b)");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for [a, b)");
    // Standard uniform_real_distribution is [a, b)
    return random_value<std::uniform_real_distribution<T>>(a, b);
}

// Returns a random number in (a, b] – exclude first, include second.
template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline static T random_real_exclude_first_include_second(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a + std::numeric_limits<T>::epsilon() > b, a, "Invalid range for (a, b]");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for (a, b]");
    const T lower = std::nextafter(a, b);  // smallest representable > a
    const T upper = std::nextafter(b, std::numeric_limits<T>::max());  // just above b so b can be returned
    return random_value<std::uniform_real_distribution<T>>(lower, upper);
}

//------------------------------------------------------------------------------
// Specialized functions for integer random generation
//------------------------------------------------------------------------------

// Returns a random integer in [a, b] – both endpoints included.
template<typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline static T random_int_inclusive(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a > b, a, "Invalid range for [a, b]");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for [a, b]");
    return random_value<std::uniform_int_distribution<T>>(a, b);
}

// Returns a random integer in (a, b) – both endpoints excluded.
// For example, if a = 5 and b = 10, returns one of {6, 7, 8, 9}.
template<typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline static T random_int_exclusive(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a + 1 > b - 1, a, "Invalid range for (a, b)");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for (a, b)");
    return random_value<std::uniform_int_distribution<T>>(a + 1, b - 1);
}

// Returns a random integer in [a, b) – include first, exclude second.
template<typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline static T random_int_include_first_exclude_second(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a > b - 1, a, "Invalid range for [a, b)");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for [a, b)");
    return random_value<std::uniform_int_distribution<T>>(a, b - 1);
}

// Returns a random integer in (a, b] – exclude first, include second.
template<typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline static T random_int_exclude_first_include_second(const T& a, const T& b) {
    CHECK_CONDITION_TRUE_NON_VOID(a + 1 > b, a, "Invalid range for (a, b]");
    CHECK_CONDITION_TRUE_NON_VOID(a == b, a, "Upper and lower bounds are equal for (a, b]");
    return random_value<std::uniform_int_distribution<T>>(a + 1, b);
}

/**
 * @brief Get the range for continuous field object. This range will be used for Mutation.
 * 
 * @param field_number 
 * @return std::pair<int, int> 
 */
using ContinuousRangeVariant = std::variant<int, float>;
inline static std::pair<ContinuousRangeVariant, ContinuousRangeVariant> get_range_for_continuous_field(const int& node_type, const int& field_number) {
    switch (node_type) {
        case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeFloatConstant::kValueFieldNumber: {
                    return std::make_pair(-100.0f, 100.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeIntConstant::kValueFieldNumber: {
                    return std::make_pair(-100, 100);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeUIntConstant::kValueFieldNumber: {
                    return std::make_pair(0, 100);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeColorConstant::kRFieldNumber:
                case VisualShaderNodeColorConstant::kGFieldNumber:
                case VisualShaderNodeColorConstant::kBFieldNumber:
                case VisualShaderNodeColorConstant::kAFieldNumber: {
                    return std::make_pair(0.0f, 1.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVec2Constant::kXFieldNumber:
                case VisualShaderNodeVec2Constant::kYFieldNumber: {
                    return std::make_pair(-100.0f, 100.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVec3Constant::kXFieldNumber:
                case VisualShaderNodeVec3Constant::kYFieldNumber:
                case VisualShaderNodeVec3Constant::kZFieldNumber: {
                    return std::make_pair(-100.0f, 100.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVec4Constant::kXFieldNumber:
                case VisualShaderNodeVec4Constant::kYFieldNumber:
                case VisualShaderNodeVec4Constant::kZFieldNumber:
                case VisualShaderNodeVec4Constant::kWFieldNumber: {
                    return std::make_pair(-100.0f, 100.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeValueNoise::kScaleFieldNumber: {
                    return std::make_pair(0.0f, 100.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
            switch (field_number) {
                case VisualShaderNodePerlinNoise::kScaleFieldNumber: {
                    return std::make_pair(0.0f, 100.0f);
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber: {
                    return std::make_pair(0.0f, 100.0f);
                }
                case VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber: {
                    return std::make_pair(0.0f, 360.0f);
                }
                default: 
                    break;
            }
            break;
        }
        default:
          WARN_PRINT("Unsupported node type: " + std::to_string(node_type));
          break;
    }

    return std::make_pair(0, 0);
}

/**
 * @brief Get the range for discrete field object. This range will be used for Mutation.
 * 
 * @param field_number 
 * @return std::vector<int> 
 */
inline static std::vector<int> get_range_for_discrete_field(const int& node_type, const int& field_number) {
    switch (node_type) {
        case VisualShader::VisualShaderNode::kInputFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeInput::kTypeFieldNumber: {
                    return {
                            VisualShaderNodeInput::INPUT_TYPE_UV, 
                            VisualShaderNodeInput::INPUT_TYPE_TIME
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeBooleanConstant::kValueFieldNumber: {
                    return {0, 1}; // false, true
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeFloatOp::kOpTypeFieldNumber: {
                    return {
                            VisualShaderNodeFloatOp::OP_TYPE_ADD, 
                            VisualShaderNodeFloatOp::OP_TYPE_SUB, 
                            VisualShaderNodeFloatOp::OP_TYPE_MUL,
                            VisualShaderNodeFloatOp::OP_TYPE_DIV,
                            VisualShaderNodeFloatOp::OP_TYPE_MOD,
                            VisualShaderNodeFloatOp::OP_TYPE_POW,
                            VisualShaderNodeFloatOp::OP_TYPE_MAX,
                            VisualShaderNodeFloatOp::OP_TYPE_MIN,
                            VisualShaderNodeFloatOp::OP_TYPE_ATAN2,
                            VisualShaderNodeFloatOp::OP_TYPE_STEP
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeIntOp::kOpTypeFieldNumber: {
                    return {
                            VisualShaderNodeIntOp::OP_TYPE_ADD, 
                            VisualShaderNodeIntOp::OP_TYPE_SUB, 
                            VisualShaderNodeIntOp::OP_TYPE_MUL,
                            VisualShaderNodeIntOp::OP_TYPE_DIV,
                            VisualShaderNodeIntOp::OP_TYPE_MOD,
                            VisualShaderNodeIntOp::OP_TYPE_MAX,
                            VisualShaderNodeIntOp::OP_TYPE_MIN,
                            VisualShaderNodeIntOp::OP_TYPE_BITWISE_AND,
                            VisualShaderNodeIntOp::OP_TYPE_BITWISE_OR,
                            VisualShaderNodeIntOp::OP_TYPE_BITWISE_XOR,
                            VisualShaderNodeIntOp::OP_TYPE_BITWISE_LEFT_SHIFT,
                            VisualShaderNodeIntOp::OP_TYPE_BITWISE_RIGHT_SHIFT
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeUIntOp::kOpTypeFieldNumber: {
                    return {
                            VisualShaderNodeUIntOp::OP_TYPE_ADD, 
                            VisualShaderNodeUIntOp::OP_TYPE_SUB, 
                            VisualShaderNodeUIntOp::OP_TYPE_MUL,
                            VisualShaderNodeUIntOp::OP_TYPE_DIV,
                            VisualShaderNodeUIntOp::OP_TYPE_MOD,
                            VisualShaderNodeUIntOp::OP_TYPE_MAX,
                            VisualShaderNodeUIntOp::OP_TYPE_MIN,
                            VisualShaderNodeUIntOp::OP_TYPE_BITWISE_AND,
                            VisualShaderNodeUIntOp::OP_TYPE_BITWISE_OR,
                            VisualShaderNodeUIntOp::OP_TYPE_BITWISE_XOR,
                            VisualShaderNodeUIntOp::OP_TYPE_BITWISE_LEFT_SHIFT,
                            VisualShaderNodeUIntOp::OP_TYPE_BITWISE_RIGHT_SHIFT
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVectorOp::kVecTypeFieldNumber: {
                    return {
                            VisualShaderNodeVectorType::TYPE_VECTOR_2D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_3D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_4D
                        };
                    }
                case VisualShaderNodeVectorOp::kOpTypeFieldNumber: {
                    return {
                            VisualShaderNodeVectorOp::OP_TYPE_ADD, 
                            VisualShaderNodeVectorOp::OP_TYPE_SUB, 
                            VisualShaderNodeVectorOp::OP_TYPE_MUL,
                            VisualShaderNodeVectorOp::OP_TYPE_DIV,
                            VisualShaderNodeVectorOp::OP_TYPE_MOD,
                            VisualShaderNodeVectorOp::OP_TYPE_POW,
                            VisualShaderNodeVectorOp::OP_TYPE_MAX,
                            VisualShaderNodeVectorOp::OP_TYPE_MIN,
                            VisualShaderNodeVectorOp::OP_TYPE_CROSS,
                            VisualShaderNodeVectorOp::OP_TYPE_ATAN2,
                            VisualShaderNodeVectorOp::OP_TYPE_REFLECT,
                            VisualShaderNodeVectorOp::OP_TYPE_STEP
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeFloatFunc::kFuncTypeFieldNumber: {
                    return {
                            VisualShaderNodeFloatFunc::FUNC_TYPE_SIN, 
                            VisualShaderNodeFloatFunc::FUNC_TYPE_COS,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_TAN,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ASIN,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ACOS,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ATAN,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_SINH,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_COSH,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_TANH,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_LOG,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_EXP,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_SQRT,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ABS,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_SIGN,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_FLOOR,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ROUND,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_CEIL,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_FRACT,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_SATURATE,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_NEGATE,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ACOSH,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ASINH,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ATANH,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_DEGREES,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_EXP2,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_INVERSE_SQRT,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_LOG2,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_RADIANS,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_RECIPROCAL,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ROUNDEVEN,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_TRUNC,
                            VisualShaderNodeFloatFunc::FUNC_TYPE_ONEMINUS
                    };
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeIntFunc::kFuncTypeFieldNumber: {
                    return {
                            VisualShaderNodeIntFunc::FUNC_TYPE_ABS, 
                            VisualShaderNodeIntFunc::FUNC_TYPE_NEGATE,
                            VisualShaderNodeIntFunc::FUNC_TYPE_SIGN,
                            VisualShaderNodeIntFunc::FUNC_TYPE_BITWISE_NOT
                    };
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeUIntFunc::kFuncTypeFieldNumber: {
                    return {
                            VisualShaderNodeUIntFunc::FUNC_TYPE_NEGATE,
                            VisualShaderNodeUIntFunc::FUNC_TYPE_BITWISE_NOT
                    };
                }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVectorFunc::kVecTypeFieldNumber: {
                    return {
                            VisualShaderNodeVectorType::TYPE_VECTOR_2D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_3D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_4D
                        };
                    }
                case VisualShaderNodeVectorFunc::kFuncTypeFieldNumber: {
                    return {
                            VisualShaderNodeVectorFunc::FUNC_TYPE_NORMALIZE, 
                            VisualShaderNodeVectorFunc::FUNC_TYPE_SATURATE,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_NEGATE,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_RECIPROCAL,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ABS,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ACOS,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ACOSH,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ASIN,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ASINH,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ATAN,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ATANH,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_CEIL,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_COS,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_COSH,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_DEGREES,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_EXP,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_EXP2,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_FLOOR,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_FRACT,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_INVERSE_SQRT,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_LOG,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_LOG2,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_RADIANS,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ROUND,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ROUNDEVEN,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_SIGN,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_SIN,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_SINH,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_SQRT,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_TAN,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_TANH,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_TRUNC,
                            VisualShaderNodeVectorFunc::FUNC_TYPE_ONEMINUS
                    };
                }
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVectorLenFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVectorLen::kVecTypeFieldNumber: {
                    return {
                            VisualShaderNodeVectorType::TYPE_VECTOR_2D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_3D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_4D
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kClampFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeClamp::kTypeFieldNumber: {
                    return {
                            VisualShaderNodeClamp::TYPE_FLOAT, 
                            VisualShaderNodeClamp::TYPE_INT, 
                            VisualShaderNodeClamp::TYPE_UINT,
                            VisualShaderNodeClamp::TYPE_VECTOR_2D,
                            VisualShaderNodeClamp::TYPE_VECTOR_3D,
                            VisualShaderNodeClamp::TYPE_VECTOR_4D
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeVectorDistance::kVecTypeFieldNumber: {
                    return {
                            VisualShaderNodeVectorType::TYPE_VECTOR_2D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_3D, 
                            VisualShaderNodeVectorType::TYPE_VECTOR_4D
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeSwitch::kTypeFieldNumber: {
                    return {
                            VisualShaderNodeSwitch::TYPE_FLOAT, 
                            VisualShaderNodeSwitch::TYPE_INT, 
                            VisualShaderNodeSwitch::TYPE_UINT,
                            VisualShaderNodeSwitch::TYPE_VECTOR_2D,
                            VisualShaderNodeSwitch::TYPE_VECTOR_3D,
                            VisualShaderNodeSwitch::TYPE_VECTOR_4D,
                            VisualShaderNodeSwitch::TYPE_BOOLEAN
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kIsFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeIs::kFuncFieldNumber: {
                    return {
                            VisualShaderNodeIs::FUNC_IS_INF, 
                            VisualShaderNodeIs::FUNC_IS_NAN
                        };
                    }
                default: 
                    break;
            }
            break;
        }
        case VisualShader::VisualShaderNode::kCompareFieldNumber: {
            switch (field_number) {
                case VisualShaderNodeCompare::kTypeFieldNumber: {
                    return {
                            VisualShaderNodeCompare::CMP_TYPE_SCALAR, 
                            VisualShaderNodeCompare::CMP_TYPE_SCALAR_INT, 
                            VisualShaderNodeCompare::CMP_TYPE_SCALAR_UINT,
                            VisualShaderNodeCompare::CMP_TYPE_VECTOR_2D,
                            VisualShaderNodeCompare::CMP_TYPE_VECTOR_3D,
                            VisualShaderNodeCompare::CMP_TYPE_VECTOR_4D,
                            VisualShaderNodeCompare::CMP_TYPE_BOOLEAN
                        };
                    }
                case VisualShaderNodeCompare::kFuncFieldNumber: {
                    return {
                            VisualShaderNodeCompare::FUNC_EQUAL, 
                            VisualShaderNodeCompare::FUNC_NOT_EQUAL, 
                            VisualShaderNodeCompare::FUNC_GREATER_THAN,
                            VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL,
                            VisualShaderNodeCompare::FUNC_LESS_THAN,
                            VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL
                        };
                    }
                case VisualShaderNodeCompare::kCondFieldNumber: {
                    return {
                            VisualShaderNodeCompare::COND_ALL, 
                            VisualShaderNodeCompare::COND_ANY
                        };
                    }
                default: 
                    break;
            }
            break;
        }
    }

    return {};
}

using DiscreteContinuousRangeVariant = std::variant<std::vector<int>, std::pair<ContinuousRangeVariant, ContinuousRangeVariant>>;
inline static DiscreteContinuousRangeVariant get_range_for_field(const int& node_type, const int& field_number) {
    const std::vector<int> discrete_range{get_range_for_discrete_field(node_type, field_number)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!discrete_range.empty(), discrete_range);

    return get_range_for_continuous_field(node_type, field_number);
}

inline static std::vector<std::string> split_string(const std::string& str, const char& delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) tokens.push_back(token);
    return tokens;
}
}  // namespace ai_agent_utils

#endif  // AI_AGENT_UTILS_HPP
