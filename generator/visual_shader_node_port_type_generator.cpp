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

#include "generator/visual_shader_node_port_type_generator.hpp"

#include "gui/controller/vs_proto_node.hpp"
#include "gui/model/utils/utils.hpp"

#include <iomanip>
#include <sstream>

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorInput::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Input node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorInput::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Input port index out of range.");
    switch (input_type) {
    case VisualShaderNodeInput::INPUT_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeInput::INPUT_TYPE_UV:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeInput::INPUT_TYPE_TIME:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorOutput::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Input port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorOutput::get_output_port_type(const int& port_index) const {
    WARN_PRINT("Output node does not have output ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorFloatConstant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Float constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorFloatConstant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Float constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIntConstant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Int constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIntConstant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Int constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorUIntConstant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("UInt constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorUIntConstant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "UInt constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorBooleanConstant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Boolean constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorBooleanConstant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Boolean constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_BOOLEAN;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorColorConstant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Color constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorColorConstant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Color constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVec2Constant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Vec2 constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVec2Constant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vec2 constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVec3Constant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Vec3 constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVec3Constant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vec3 constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVec4Constant::get_input_port_type(const int& port_index) const {
    WARN_PRINT("Vec4 constant node does not have input ports.");
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVec4Constant::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vec4 constant port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorFloatOp::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Float operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeFloatOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorFloatOp::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Float operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeFloatOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIntOp::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Int operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeIntOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIntOp::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Int operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeIntOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorUIntOp::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "UInt operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeUIntOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorUIntOp::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "UInt operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeUIntOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorOp::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeVectorOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        switch (type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED:
            return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
        default:
            break;
        }
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorOp::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector operator port index out of range.");
    switch (op_type) {
    case VisualShaderNodeVectorOp::OP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        switch (type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED:
            return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
        default:
            break;
        }
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorFloatFunc::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Float function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeFloatFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorFloatFunc::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Float function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeFloatFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIntFunc::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Int function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeIntFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIntFunc::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Int function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeIntFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorUIntFunc::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "UInt function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeUIntFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorUIntFunc::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "UInt function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeUIntFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorFunc::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeVectorFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorFunc::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector function port index out of range.");
    switch (func_type) {
    case VisualShaderNodeVectorFunc::FUNC_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorValueNoise::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Value noise port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorValueNoise::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Value noise port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorPerlinNoise::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Perlin noise port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorPerlinNoise::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Perlin noise port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVoronoiNoise::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Voronoi noise port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVoronoiNoise::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Voronoi noise port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorDotProduct::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Dot product port index out of range.");
    switch (port_index) {
    case 0:
    case 1:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorDotProduct::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Dot product port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorLen::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector length port index out of range.");
    switch (vec_type) {
    case VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorLen::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector length port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorClamp::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Clamp port index out of range.");
    switch (type) {
    case VisualShaderNodeClamp::TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeClamp::TYPE_FLOAT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    case VisualShaderNodeClamp::TYPE_INT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
    case VisualShaderNodeClamp::TYPE_UINT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
    case VisualShaderNodeClamp::TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeClamp::TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeClamp::TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorClamp::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Clamp port index out of range.");
    switch (type) {
    case VisualShaderNodeClamp::TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeClamp::TYPE_FLOAT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    case VisualShaderNodeClamp::TYPE_INT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
    case VisualShaderNodeClamp::TYPE_UINT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
    case VisualShaderNodeClamp::TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeClamp::TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeClamp::TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorDistance::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector distance port index out of range.");
    switch (vec_type) {
    case VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorDistance::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Vector distance port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorCompose::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "2D vector compose port index out of range.");
    return VisualShaderNodePortType::PORT_TYPE_SCALAR;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorCompose::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "2D vector compose port index out of range.");
    switch (port_index) {
    case 0:
        switch (vec_type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED:
            return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
        default:
            break;
        }
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorDecompose::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "3D vector decompose port index out of range.");
    switch (port_index) {
    case 0:
        switch (vec_type) {
        case VisualShaderNodeVectorType::TYPE_VECTOR_UNSPECIFIED:
            return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
        case VisualShaderNodeVectorType::TYPE_VECTOR_2D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_3D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
        case VisualShaderNodeVectorType::TYPE_VECTOR_4D:
            return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
        default:
            break;
        }
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorVectorDecompose::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "3D vector decompose port index out of range.");
    return VisualShaderNodePortType::PORT_TYPE_SCALAR;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIf::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "If port index out of range.");
    switch (port_index) {
    case 0:
    case 1:
    case 2:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    case 3:
    case 4:
    case 5:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIf::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "If port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorSwitch::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Switch port index out of range.");
    switch (type) {
    case VisualShaderNodeSwitch::TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeSwitch::TYPE_FLOAT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    case VisualShaderNodeSwitch::TYPE_INT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
    case VisualShaderNodeSwitch::TYPE_UINT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
    case VisualShaderNodeSwitch::TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeSwitch::TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeSwitch::TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    case VisualShaderNodeSwitch::TYPE_BOOLEAN:
        return VisualShaderNodePortType::PORT_TYPE_BOOLEAN;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorSwitch::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Switch port index out of range.");
    switch (type) {
    case VisualShaderNodeSwitch::TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeSwitch::TYPE_FLOAT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    case VisualShaderNodeSwitch::TYPE_INT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
    case VisualShaderNodeSwitch::TYPE_UINT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
    case VisualShaderNodeSwitch::TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeSwitch::TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeSwitch::TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    case VisualShaderNodeSwitch::TYPE_BOOLEAN:
        return VisualShaderNodePortType::PORT_TYPE_BOOLEAN;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIs::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Is port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorIs::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Is port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_BOOLEAN;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorCompare::get_input_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Compare port index out of range.");
    switch (type) {
    case VisualShaderNodeCompare::CMP_TYPE_UNSPECIFIED:
        return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR;
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_INT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_INT;
    case VisualShaderNodeCompare::CMP_TYPE_SCALAR_UINT:
        return VisualShaderNodePortType::PORT_TYPE_SCALAR_UINT;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_2D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_2D;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_3D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_3D;
    case VisualShaderNodeCompare::CMP_TYPE_VECTOR_4D:
        return VisualShaderNodePortType::PORT_TYPE_VECTOR_4D;
    case VisualShaderNodeCompare::CMP_TYPE_BOOLEAN:
        return VisualShaderNodePortType::PORT_TYPE_BOOLEAN;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}

VisualShaderNodePortType VisualShaderNodePortTypeGeneratorCompare::get_output_port_type(const int& port_index) const {
    VALIDATE_INDEX_NON_VOID(port_index, proto_node->get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Compare port index out of range.");
    switch (port_index) {
    case 0:
        return VisualShaderNodePortType::PORT_TYPE_BOOLEAN;
    default:
        break;
    }
    return VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED;
}
