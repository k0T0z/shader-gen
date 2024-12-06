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
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_caption);
  }

  int get_input_port_count() const {
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_count);
  }

  VisualShaderNodePortType get_input_port_type(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_type, index);
  }

  std::string get_input_port_caption(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), std::string(), "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_caption, index);
  }

  int get_output_port_count() const {
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_count);
  }

  VisualShaderNodePortType get_output_port_type(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_type, index);
  }

  std::string get_output_port_caption(const int& index) const {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), std::string(), "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_caption, index);
  }

  VisualShaderNodeCategory get_category() const {
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category);
  }

  std::string get_category_path() const {
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category_path);
  }

  std::string get_description() const {
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_description);
  }
};

using variant_node_type = std::variant<GraphNode<VisualShaderNodeFloatConstant>, 
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


#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_FLOAT_CONSTANT_INDEX 0
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_INT_CONSTANT_INDEX 1
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_UINT_CONSTANT_INDEX 2
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_BOOLEAN_CONSTANT_INDEX 3
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_COLOR_CONSTANT_INDEX 4
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC2_CONSTANT_INDEX 5
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC3_CONSTANT_INDEX 6
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC4_CONSTANT_INDEX 7

#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VALUE_NOISE_INDEX 8
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_PERLIN_NOISE_INDEX 9
#define VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VORONOI_NOISE_INDEX 10

#endif // GRAPH_NODE_HPP
