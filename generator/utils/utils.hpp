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

#ifndef GENERATOR_UTILS_HPP
#define GENERATOR_UTILS_HPP

#include <cmath>
#include <limits>
#include "gui/model/schema/visual_shader_nodes.pb.h"

namespace generator_utils {
inline static bool are_floats_almost_equal(const float& a, const float& b) {
  // Check for exact equality first, required to handle "infinity" values.
  if (a == b) {
    return true;
  }

  // Then check for approximate equality.
  float tolerance{std::numeric_limits<float>::epsilon() * abs(a)};
  if (tolerance < std::numeric_limits<float>::epsilon()) {
    tolerance = std::numeric_limits<float>::epsilon();
  }

  return std::fabs(a - b) < tolerance;
}

inline static bool is_valid_connection(const VisualShaderNodePortType& from_port_type, const VisualShaderNodePortType& to_port_type) {
  if (from_port_type == to_port_type) {
    return true;
  }

  switch (from_port_type) {
    case VisualShaderNodePortType::PORT_TYPE_SCALAR:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          break;
      }
      break;
    case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          break;
      }
      break;
    case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          break;
      }
      break;
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          break;
      }
      break;
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          break;
      }
      break;
    case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
          return true;
        default:
          break;
      }
      break;
    case VisualShaderNodePortType::PORT_TYPE_BOOLEAN:
      switch (to_port_type) {
        case VisualShaderNodePortType::PORT_TYPE_SCALAR:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_INT:
        case VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_2D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_3D:
        case VisualShaderNodePortType::PORT_TYPE_VECTOR_4D:
          return true;
        default:
          break;
      }
      break;
    default:
      break;
  }

  return false;
}
}  // namespace generator_utils

#endif  // GENERATOR_UTILS_HPP
