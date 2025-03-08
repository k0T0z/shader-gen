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

#include "error_macros.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/controller/vs_proto_node.hpp"
#include "gui/model/oneof_model.hpp"
#include "ai-agent/ai_agent.hpp"

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

/**
 * @brief 
 * 
 * @note Josh — 10/02/2025 20:36 "my recommendation for "how can we possibly guess 
 *       at a topology other than pure random" would be K-means clustering based on 
 *       fourier values"
 * 
 * @note Josh — 07/03/2025 21:26 "I think those are good heuristics, but what I would 
 *       probably do is start with a handful of topologies or even a few random nodes 
 *       of each kernel type, then just choose the ones that are the closest based on 
 *       that Fourier analysis"
 * 
 * @return std::vector<std::unordered_map<int, std::string>> 
 */

inline static std::vector<std::unordered_map<int, std::string>> generate_nodes_population(const AIAgentWorker::MatchingType& matching_type) {
    std::vector<std::unordered_map<int, std::string>> population;

    return population;
}

inline static std::vector<std::unordered_map<int, std::string>> generate_connections_population(const AIAgentWorker::MatchingType& matching_type) {
    std::vector<std::unordered_map<int, std::string>> population;

    return population;
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
inline static std::pair<int, int> get_range_for_continuous_field(const int& node_type, const int& field_number) {
    return std::make_pair(0, 0);
}
}  // namespace ai_agent_utils

#endif  // AI_AGENT_UTILS_HPP
