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

#ifndef GRAPH_NODE_HPP
#define GRAPH_NODE_HPP

#include <google/protobuf/descriptor.h>
#include <QMetaType>
#include <string>
#include <variant>
#include "error_macros.hpp"

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

template <typename Proto>
class GraphNode : public IGraphNode {
 public:
  std::string get_caption() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_caption), "",
                                  "Node caption not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_caption);
  }

  int get_input_port_count() const override {
    CHECK_CONDITION_TRUE_NON_VOID(
        !Proto::descriptor()->options().HasExtension(gui::model::schema::node_input_port_count), 0,
        "Input port count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_count);
  }

  VisualShaderNodePortType get_input_port_type(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED,
                            "Invalid input port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_type),
                            VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_type, index);
  }

  std::string get_input_port_caption(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_input_port_count(), "", "Invalid input port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_input_port_caption),
                            "", "Invalid input port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_input_port_caption, index);
  }

  int get_output_port_count() const override {
    CHECK_CONDITION_TRUE_NON_VOID(
        !Proto::descriptor()->options().HasExtension(gui::model::schema::node_output_port_count), 0,
        "Output port count not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_count);
  }

  VisualShaderNodePortType get_output_port_type(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED,
                            "Invalid output port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_type),
                            VisualShaderNodePortType::PORT_TYPE_UNSPECIFIED, "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_type, index);
  }

  std::string get_output_port_caption(const int& index) const override {
    VALIDATE_INDEX_NON_VOID(index, get_output_port_count(), "", "Invalid output port index");
    VALIDATE_INDEX_NON_VOID(index,
                            Proto::descriptor()->options().ExtensionSize(gui::model::schema::node_output_port_caption),
                            "", "Invalid output port index");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_output_port_caption, index);
  }

  VisualShaderNodeCategory get_category() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category),
                                  VisualShaderNodeCategory::CATEGORY_UNSPECIFIED, "Node category not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category);
  }

  std::string get_category_path() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_category_path),
                                  "", "Node category path not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_category_path);
  }

  std::string get_description() const override {
    CHECK_CONDITION_TRUE_NON_VOID(!Proto::descriptor()->options().HasExtension(gui::model::schema::node_description),
                                  "", "Node description not set");
    return Proto::descriptor()->options().GetExtension(gui::model::schema::node_description);
  }
};

Q_DECLARE_METATYPE(std::shared_ptr<IGraphNode>)  // Required for QVariant

#endif  // GRAPH_NODE_HPP
