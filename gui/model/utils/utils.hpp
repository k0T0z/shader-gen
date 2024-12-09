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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <google/protobuf/descriptor.h>
#include "error_macros.hpp"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

namespace shadergen_utils {
    inline static bool is_inside_real_oneof(const google::protobuf::FieldDescriptor* field) noexcept {
        // if (!field->containing_oneof()) return false;

        // // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor.real_oneof_decl_count.details
        // int real_oneofs_count {field->containing_type()->real_oneof_decl_count()};

        // // Check if the field is in a real oneof, not a synthetic oneof
        // for (int i = 0; i < real_oneofs_count; ++i) {
        //     // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor.oneof_decl.details
        //     // Real oneofs always come first, so iterating up to real_oneof_decl_cout() will yield all real oneofs.
        //     if (field->containing_oneof() == field->containing_type()->oneof_decl(i)) {
        //         return true;
        //     }
        // }

        // return false;

        SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);
        return field->real_containing_oneof() != nullptr;
    }

    inline static std::string get_enum_value_caption(const google::protobuf::EnumDescriptor* enum_descriptor, const int& value_index) noexcept {
      VALIDATE_INDEX_NON_VOID(value_index, enum_descriptor->value_count(), "", "Invalid enum value index");
      CHECK_CONDITION_TRUE_NON_VOID(!enum_descriptor->value(value_index)->options().HasExtension(gui::model::schema::value_caption), "", "Enum value caption not set");
      return enum_descriptor->value(value_index)->options().GetExtension(gui::model::schema::value_caption);
    }

    inline static VisualShaderNodePortType get_enum_value_port_type(const google::protobuf::EnumDescriptor* enum_descriptor, const int& value_index) noexcept {
      VALIDATE_INDEX_NON_VOID(value_index, enum_descriptor->value_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid enum value index");
      CHECK_CONDITION_TRUE_NON_VOID(!enum_descriptor->value(value_index)->options().HasExtension(gui::model::schema::value_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Enum value port type not set");
      return enum_descriptor->value(value_index)->options().GetExtension(gui::model::schema::value_port_type);
    }

    inline static int get_enum_value_by_index(const google::protobuf::EnumDescriptor* enum_descriptor, const int& value_index) noexcept {
      VALIDATE_INDEX_NON_VOID(value_index, enum_descriptor->value_count(), 0, "Invalid enum value index");
      return enum_descriptor->value(value_index)->number();
    }
} // namespace shadergen_utils

#endif // UTILS_HPP
