#ifndef GRAPH_NODE_HPP
#define GRAPH_NODE_HPP

#include <string>
#include <variant>
#include <google/protobuf/descriptor.h>
#include "error_macros.hpp"
#include <QMetaType>

#include "gui/model/schema/visual_shader.pb.h"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

class IGraphNode {
  public:
    virtual ~IGraphNode() = default;

    virtual std::string get_caption() const = 0;

    virtual int get_input_port_count() const = 0;
    virtual VisualShaderNodePortType get_input_port_type(const int& index) const = 0;
    virtual std::string get_input_port_caption(const int& index) const = 0;

    virtual int get_output_port_count() const = 0;
    virtual VisualShaderNodePortType get_output_port_type(const int& index) const = 0;
    virtual std::string get_output_port_caption(const int& index) const = 0;

    virtual VisualShaderNodeCategory get_category() const = 0;
    virtual std::string get_category_path() const = 0;

    virtual std::string get_description() const = 0;

    /************************************************************************************************/
    /*                                  Input Node specific methods                                 */
    virtual int get_input_type_count() const = 0;
    virtual std::string get_input_type_caption(const VisualShaderNodeInputType& type) const = 0;
    virtual VisualShaderNodeInputType get_input_type(const int& index) const = 0;
    virtual VisualShaderNodePortType get_input_node_output_port_type(const int& index) const = 0;
    /************************************************************************************************/

    /************************************************************************************************/
    /*                                  Output Node specific methods                                */
    virtual int get_output_type_count() const = 0;
    virtual std::string get_output_type_caption(const VisualShaderNodeOutputType& type) const = 0;
    virtual VisualShaderNodeOutputType get_output_type(const int& index) const = 0;
    virtual VisualShaderNodePortType get_output_node_input_port_type(const int& index) const = 0;
    /************************************************************************************************/
};


template<typename Proto> 
class GraphNode : public IGraphNode {
  public:
    std::string get_caption() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_caption), "", "Node caption not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_caption);
    }

    int get_input_port_count() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_input_port_count), 0, "Input port count not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_count);
    }

    VisualShaderNodePortType get_input_port_type(const int& index) const override {
      VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_type, index);
    }

    std::string get_input_port_caption(const int& index) const override {
      VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), "", "Invalid input port index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_caption), "", "Invalid input port index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_caption, index);
    }

    int get_output_port_count() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_output_port_count), 0, "Output port count not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_count);
    }

    VisualShaderNodePortType get_output_port_type(const int& index) const override {
      VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_type, index);
    }

    std::string get_output_port_caption(const int& index) const override {
      VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), "", "Invalid output port index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_caption), "", "Invalid output port index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_caption, index);
    }

    VisualShaderNodeCategory get_category() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category), VisualShaderNodeCategory::CATEGORY_UNSPECIFIED, "Node category not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category);
    }

    std::string get_category_path() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category_path), "", "Node category path not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category_path);
    }

    std::string get_description() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_description), "", "Node description not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::node_description);
    }

    /************************************************************************************************/
    /*                                  Input Node specific methods                                 */
    int get_input_type_count() const override {
      static_assert(std::is_same_v<Proto, VisualShaderNodeInput>, "Proto must be a VisualShaderNodeInput");
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::input_type_count), 0, "Input type count not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::input_type_count);
    }

    std::string get_input_type_caption(const VisualShaderNodeInputType& type) const override {
      static_assert(std::is_same_v<Proto, VisualShaderNodeInput>, "Proto must be a VisualShaderNodeInput");
      const google::protobuf::EnumDescriptor* enum_descriptor = VisualShaderNodeInputType_descriptor();
      CHECK_CONDITION_TRUE_NON_VOID(!enum_descriptor->FindValueByNumber(type)->options().HasExtension(gui::model::schema::type_caption), "", "Input type caption not set");
      return enum_descriptor->FindValueByNumber(type)->options().GetExtension(gui::model::schema::type_caption);
    }

    VisualShaderNodeInputType get_input_type(const int& index) const override {
      static_assert(std::is_same_v<Proto, VisualShaderNodeInput>, "Proto must be a VisualShaderNodeInput");
      VALIDATE_INDEX_NON_VOID(index, get_input_type_count(), VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED, "Invalid input type index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::input_type), VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED, "Invalid input type index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::input_type, index);
    }

    VisualShaderNodePortType get_input_node_output_port_type(const int& index) const override {
      static_assert(std::is_same_v<Proto, VisualShaderNodeInput>, "Proto must be a VisualShaderNodeInput");
      VALIDATE_INDEX_NON_VOID(index, get_input_type_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input type index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::input_node_output_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input type index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::input_node_output_port_type, index);
    }
    /************************************************************************************************/

    /************************************************************************************************/
    /*                                  Output Node specific methods                                */
    int get_output_type_count() const override {
      CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::output_type_count), 0, "Output type count not set");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::output_type_count);
    }

    std::string get_output_type_caption(const VisualShaderNodeOutputType& type) const override {
      const google::protobuf::EnumDescriptor* enum_descriptor = VisualShaderNodeOutputType_descriptor();
      CHECK_CONDITION_TRUE_NON_VOID(!enum_descriptor->FindValueByNumber(type)->options().HasExtension(gui::model::schema::type_caption), "", "Output type caption not set");
      return enum_descriptor->FindValueByNumber(type)->options().GetExtension(gui::model::schema::type_caption);
    }

    VisualShaderNodeOutputType get_output_type(const int& index) const override {
      VALIDATE_INDEX_NON_VOID(index, get_output_type_count(), VisualShaderNodeOutputType::OUTPUT_TYPE_UNSPECIFIED, "Invalid output type index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::output_type), VisualShaderNodeOutputType::OUTPUT_TYPE_UNSPECIFIED, "Invalid output type index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::output_type, index);
    }

    VisualShaderNodePortType get_output_node_input_port_type(const int& index) const override {
      VALIDATE_INDEX_NON_VOID(index, get_output_type_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output type index");
      VALIDATE_INDEX_NON_VOID(index, Proto::descriptor()->options().ExtensionSize(gui::model::schema::output_node_input_port_type), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output type index");
      return Proto::descriptor()->options().GetExtension(gui::model::schema::output_node_input_port_type, index);
    }
    /************************************************************************************************/
};

Q_DECLARE_METATYPE(std::shared_ptr<IGraphNode>) // Required for QVariant

#endif // GRAPH_NODE_HPP
