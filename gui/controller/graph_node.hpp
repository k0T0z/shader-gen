#ifndef GRAPH_NODE_HPP
#define GRAPH_NODE_HPP

#include <string>
#include <variant>
#include <google/protobuf/descriptor.h>
#include "error_macros.hpp"

#include "gui/model/schema/visual_shader.pb.h"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

template<typename Proto> 
class GraphNode {
 public:
  std::string get_caption() const {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_caption), std::string(), "Node caption not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_caption);
  }

  int get_input_port_count() const {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_input_port_count), 0, "Input port count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_count);
  }

  VisualShaderNodePortType get_input_port_type(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_type, index);
  }

  std::string get_input_port_caption(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), std::string(), "Invalid input port index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_caption), std::string(), "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_caption, index);
  }

  int get_output_port_count() const {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_output_port_count), 0, "Output port count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_count);
  }

  VisualShaderNodePortType get_output_port_type(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_type, index);
  }

  std::string get_output_port_caption(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), std::string(), "Invalid output port index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_caption), std::string(), "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_caption, index);
  }

  VisualShaderNodeCategory get_category() const {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category), VisualShaderNodeCategory::CATEGORY_UNSPECIFIED, "Node category not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category);
  }

  std::string get_category_path() const {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category_path), std::string(), "Node category path not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category_path);
  }

  std::string get_description() const {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_description), std::string(), "Node description not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_description);
  }

  /************************************************************************************************/
  /*                                  Input Node specific methods                                 */
  int get_input_type_count() const {
    static_assert(std::is_base_of<VisualShaderNodeInput, Proto>::value, "Proto must be a VisualShaderNodeInput");
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::input_type_count), 0, "Input type count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::input_type_count);
  }

  std::string get_input_type_caption(const VisualShaderNodeInputType& type) const {
    static_assert(std::is_base_of<VisualShaderNodeInput, Proto>::value, "Proto must be a VisualShaderNodeInput");
    const google::protobuf::EnumDescriptor* enum_descriptor = VisualShaderNodeInputType_descriptor();
    CHECK_CONDITION_TRUE_NON_VOID(!enum_descriptor->FindValueByNumber(type)->options().HasExtension(gui::model::schema::type_caption), std::string(), "Input type caption not set");
    return enum_descriptor->FindValueByNumber(type)->options().GetExtension(gui::model::schema::type_caption);
  }

  VisualShaderNodeInputType get_input_type(const int& index) const {
    static_assert(std::is_base_of<VisualShaderNodeInput, Proto>::value, "Proto must be a VisualShaderNodeInput");
    VALIDATE_INDEX_NON_VOID(index, get_input_type_count(), VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED, "Invalid input type index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::input_type), VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED, "Invalid input type index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::input_type, index);
  }

  VisualShaderNodePortType get_input_node_output_port_type(const int& index) const {
    static_assert(std::is_base_of<VisualShaderNodeInput, Proto>::value, "Proto must be a VisualShaderNodeInput");
    VALIDATE_INDEX_NON_VOID(index, get_input_type_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input type index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::input_node_output_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input type index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::input_node_output_port_type, index);
  }
  /************************************************************************************************/

  /************************************************************************************************/
  /*                                  Output Node specific methods                                */
  int get_output_type_count() const {
    static_assert(std::is_base_of<VisualShaderNodeOutput, Proto>::value, "Proto must be a VisualShaderNodeOutput");
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::output_type_count), 0, "Output type count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::output_type_count);
  }

  std::string get_output_type_caption(const VisualShaderNodeOutputType& type) const {
    static_assert(std::is_base_of<VisualShaderNodeOutput, Proto>::value, "Proto must be a VisualShaderNodeOutput");
    const google::protobuf::EnumDescriptor* enum_descriptor = VisualShaderNodeOutputType_descriptor();
    CHECK_CONDITION_TRUE_NON_VOID(!enum_descriptor->FindValueByNumber(type)->options().HasExtension(gui::model::schema::type_caption), std::string(), "Output type caption not set");
    return enum_descriptor->FindValueByNumber(type)->options().GetExtension(gui::model::schema::type_caption);
  }

  VisualShaderNodeOutputType get_output_type(const int& index) const {
    static_assert(std::is_base_of<VisualShaderNodeOutput, Proto>::value, "Proto must be a VisualShaderNodeOutput");
    VALIDATE_INDEX_NON_VOID(index, get_output_type_count(), VisualShaderNodeOutputType::OUTPUT_TYPE_UNSPECIFIED, "Invalid output type index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::output_type), VisualShaderNodeOutputType::OUTPUT_TYPE_UNSPECIFIED, "Invalid output type index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::output_type, index);
  }

  VisualShaderNodePortType get_output_node_input_port_type(const int& index) const {
    static_assert(std::is_base_of<VisualShaderNodeOutput, Proto>::value, "Proto must be a VisualShaderNodeOutput");
    VALIDATE_INDEX_NON_VOID(index, get_output_type_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output type index");
    VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::output_node_input_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output type index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::output_node_input_port_type, index);
  }
  /************************************************************************************************/
};

using variant_node_type = std::variant<GraphNode<VisualShaderNodeInput>,
                                       GraphNode<VisualShaderNodeOutput>,

                                       GraphNode<VisualShaderNodeFloatConstant>, 
                                       GraphNode<VisualShaderNodeIntConstant>,
                                       GraphNode<VisualShaderNodeUIntConstant>,
                                       GraphNode<VisualShaderNodeBooleanConstant>,
                                       GraphNode<VisualShaderNodeColorConstant>,
                                       GraphNode<VisualShaderNodeVec2Constant>,
                                       GraphNode<VisualShaderNodeVec3Constant>,
                                       GraphNode<VisualShaderNodeVec4Constant>,
                                       
                                       GraphNode<VisualShaderNodeValueNoise>,
                                       GraphNode<VisualShaderNodePerlinNoise>,
                                       GraphNode<VisualShaderNodeVoronoiNoise>>;


#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_INPUT_INDEX 0
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_OUTPUT_INDEX 1

#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_FLOAT_CONSTANT_INDEX 2
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_INT_CONSTANT_INDEX 3
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_UINT_CONSTANT_INDEX 4
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_BOOLEAN_CONSTANT_INDEX 5
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_COLOR_CONSTANT_INDEX 6
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC2_CONSTANT_INDEX 7
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC3_CONSTANT_INDEX 8
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC4_CONSTANT_INDEX 9

#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VALUE_NOISE_INDEX 10
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_PERLIN_NOISE_INDEX 11
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VORONOI_NOISE_INDEX 12

#endif // GRAPH_NODE_HPP
