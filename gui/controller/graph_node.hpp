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
};

Q_DECLARE_METATYPE(std::shared_ptr<IGraphNode>) // Required for QVariant

#endif // GRAPH_NODE_HPP
