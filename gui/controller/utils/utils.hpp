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

#ifndef CONTROLLER_UTILS_HPP
#define CONTROLLER_UTILS_HPP

#include "gui/model/schema/visual_shader_nodes.pb.h"

#include "error_macros.hpp"

namespace controller_utils {
inline static bool is_valid_connection(const VisualShaderNodePortType& source_port,
                                       const VisualShaderNodePortType& target_port) noexcept {
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
