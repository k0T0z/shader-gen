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

#include "gui/model/utils/field_path.hpp"

#include "gui/model/utils/utils.hpp"

#define PATH_COMPONENT_VARIANT_FIELD_NUMBER_INDEX 0
#define PATH_COMPONENT_VARIANT_REPEATED_AT_INDEX 1

bool FieldPath::is_valid_path() const {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
  SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_root_buffer_descriptor, false);

  const google::protobuf::Descriptor* current_buffer_descriptor{m_root_buffer_descriptor};

  const google::protobuf::FieldDescriptor* repeated_field{nullptr};

  std::queue<PathComponent> temp = m_components;

  while (!temp.empty()) {
    PathComponent component{temp.front()};
    temp.pop();

    switch (component.index()) {
      case PATH_COMPONENT_VARIANT_FIELD_NUMBER_INDEX: {
        SILENT_CHECK_PARAM_NULLPTR_NON_VOID(current_buffer_descriptor, false);
        int fn{std::get<FieldPath::FieldNumber>(component).field_number};
        const google::protobuf::FieldDescriptor* field{current_buffer_descriptor->FindFieldByNumber(fn)};
        CHECK_PARAM_NULLPTR_NON_VOID(
            field, false,
            "Field " + std::to_string(fn) + " not found in descriptor " + current_buffer_descriptor->full_name());
        current_buffer_descriptor = field->message_type();
        if (field->is_repeated())
          repeated_field = field;
        else
          repeated_field = nullptr;
        break;
      }
      case PATH_COMPONENT_VARIANT_REPEATED_AT_INDEX: {
        CHECK_PARAM_NULLPTR_NON_VOID(repeated_field, false, "No repeated field to access.");
        CHECK_CONDITION_TRUE_NON_VOID(!repeated_field->is_repeated(), false,
                                      "Field " + repeated_field->full_name() + " is not repeated.");
        current_buffer_descriptor = repeated_field->message_type();
        repeated_field = nullptr;
        break;
      }
      default: {
        FAIL_AND_RETURN_NON_VOID(false, "Invalid path component type");
      }
    }
  }

  return true;
}

bool FieldPath::is_upcoming_field() const {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
  if (m_components.front().index() == PATH_COMPONENT_VARIANT_FIELD_NUMBER_INDEX) {
    return true;
  }
  return false;
}

bool FieldPath::is_upcoming_repeated_index() const {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
  if (m_components.front().index() == PATH_COMPONENT_VARIANT_REPEATED_AT_INDEX) {
    return true;
  }
  return false;
}

bool FieldPath::skip_component() const {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
  m_components.pop();
  return true;
}

std::string FieldPath::to_string() const {
  std::string path_string;
  std::queue<PathComponent> temp = m_components;

  while (!temp.empty()) {
    PathComponent component{temp.front()};
    temp.pop();

    switch (component.index()) {
      case PATH_COMPONENT_VARIANT_FIELD_NUMBER_INDEX: {
        path_string += "FN(" + std::to_string(std::get<FieldPath::FieldNumber>(component).field_number) + ")";
        break;
      }
      case PATH_COMPONENT_VARIANT_REPEATED_AT_INDEX: {
        path_string += "AT[" + std::to_string(std::get<FieldPath::RepeatedAt>(component).index) + "]";
        break;
      }
      default: {
        FAIL_AND_RETURN_NON_VOID(path_string, "Invalid path component type");
      }
    }

    if (!temp.empty()) {
      path_string += " --> ";
    }
  }

  return path_string;
}

bool FieldPath::get_upcoming_field_num(int& num_buffer) const {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_field(), false);
  num_buffer = std::get<FieldPath::FieldNumber>(m_components.front()).field_number;
  return true;
}

bool FieldPath::get_upcoming_repeated_index(int& index_buffer) const {
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_repeated_index(), false);
  index_buffer = std::get<FieldPath::RepeatedAt>(m_components.front()).index;
  return true;
}
