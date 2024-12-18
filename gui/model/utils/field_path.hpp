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

#ifndef FIELD_PATH_HPP
#define FIELD_PATH_HPP

#include <google/protobuf/message.h>
#include <queue>
#include <type_traits>
#include <variant>
#include "error_macros.hpp"

/**
 * @brief A class to represent a path to a field in a protobuf message.
 * 
 * @example 
 * 
 *   - Access a field in a message: 
 *        FieldPath::Of<MyMessage>(FieldPath::FieldNumber(1));
 *   - Access a field in a message in a oneof: 
 *        FieldPath::Of<MyMessage>(FieldPath::OneofFieldNumber("my_oneof", 1));
 *   - Access a repeated field in a message: 
 *        FieldPath::Of<MyMessage>(FieldPath::RepeatedAt(1, 0)); // Get 
 *        the first element from a repeated field with field number 1
 * 
 *           ----------------------------------------------------
 * 
 *   - Get a message model:
 *        FieldPath::Of<MyMessage>(FieldPath::FieldNumber(1)); // This will return
 *        a message model with the field number 1
 *   - Get a oneof model:
 *        FieldPath::Of<MyMessage>(FieldPath::OneofFieldNumber("my_oneof", STOPPING_FIELD_NUMBER)); 
 *        // This will return a oneof model with the oneof name "my_oneof"
 *   - Get a repeated model:
 *        FieldPath::Of<MyMessage>(FieldPath::RepeatedAt(2, STOPPING_REPEATED_INDEX)); // This
 *        will return a repeated model with the field number 2
 * 
 *           ----------------------------------------------------
 * 
 *   All the above examples are for the top-level message. You can nest the path
 *   - Access a field in a message in a message:
 *        FieldPath::Of<MyMessage>(FieldPath::FieldNumber(1), FieldPath::FieldNumber(2));
 *        // This will return a message model with the field number 2 in a message with 
 *        // the field number 1
 * 
 * 
 */
class FieldPath {
 public:
  /**
     * @brief Can be:
     *        - Message
     *        - Enum
     *        - Primitive
     *        - Repeated Message
     *        - Repeated Enum
     *        - Repeated Primitive
     * 
     */
  struct FieldNumber {
    int field_number;
    explicit FieldNumber(const int& f) : field_number(f) {}
  };

  struct RepeatedAt {
    int index;
    explicit RepeatedAt(const int& i) : index(i) {}
  };

  // Variant to hold any path component
  using PathComponent = std::variant<FieldNumber, RepeatedAt>;

 private:
  bool m_is_valid;
  const google::protobuf::Descriptor* m_root_buffer_descriptor;
  mutable std::queue<PathComponent> m_components;

  FieldPath() : m_is_valid(false), m_root_buffer_descriptor(nullptr) {}
  bool is_upcoming_field() const;
  bool is_upcoming_repeated_index() const;

  /**
     * @brief Check if the path is valid
     * 
     * @note @c private because it should be called only on a full path. This
     *       means if you skipped a component, the path becomes invalid because
     *       the root buffer descriptor is no longer the correct one.
     * 
     * @return true 
     * @return false 
     */
  bool is_valid_path() const;

 public:
  // Construct a FieldPath using variadic templates
  template <typename T, typename... Components>
  static FieldPath Of(Components... components) {
    static_assert(((std::is_same_v<Components, FieldNumber> || std::is_same_v<Components, RepeatedAt>) && ...),
                  "All Components must be FieldNumber or RepeatedAt.");
    FieldPath path;
    path.m_root_buffer_descriptor = T::GetDescriptor();
    (path.m_components.push(components), ...);  // Add all components
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid_path(), FieldPath(), "Invalid path: " + path.to_string());
    path.m_is_valid = true;
    return path;
  }

  bool is_empty() const { return m_components.empty(); }
  bool is_valid() const { return m_is_valid; }

  bool get_upcoming_field_num(int& num_buffer) const;
  bool get_upcoming_repeated_index(int& index_buffer) const;

  bool skip_component() const;

  std::string to_string() const;

  // Access the components
  const std::queue<PathComponent>& get_components() const { return m_components; }
};

#endif  // FIELD_PATH_HPP
