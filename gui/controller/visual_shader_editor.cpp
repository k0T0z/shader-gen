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

#include "gui/controller/visual_shader_editor.hpp"

#include <sstream>
#include <unordered_map>
#include "error_macros.hpp"
#include "gui/model/utils/utils.hpp"
#include "gui/model/oneof_model.hpp"

#include "gui/controller/utils/utils.hpp"

#include "generator/visual_shader_generator.hpp"

using VisualShader = gui::model::schema::VisualShader;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderEditor                           *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderEditor::VisualShaderEditor(MessageModel* model, QWidget* parent)
    : QWidget(parent),
      layout(nullptr),
      side_widget(nullptr),
      side_outer_layout(nullptr),
      side_layout(nullptr),
      name_edit(nullptr),
      save_button(nullptr),
      scene_layer_layout(nullptr),
      scene_layer(nullptr),
      scene(nullptr),
      view(nullptr),
      top_layer(nullptr),
      menu_bar(nullptr),
      menu_button(nullptr),
      create_node_button(nullptr),
      preview_shader_button(nullptr),
      zoom_in_button(nullptr),
      reset_zoom_button(nullptr),
      zoom_out_button(nullptr),
      load_image_button(nullptr),
      match_image_button(nullptr),
      create_node_action(nullptr),
      code_previewer_dialog(nullptr),
      code_previewer_layout(nullptr),
      code_previewer(nullptr),
      create_node_dialog(nullptr),
      visual_shader_model(model),
      nodes_model(nullptr),
      connections_model(nullptr),
      fitness_calculator(nullptr) {
  resize(1440, 720);

  VisualShaderEditor::init();
}

VisualShaderEditor::~VisualShaderEditor() {
  delete fitness_calculator;
}

void VisualShaderEditor::init() {
  // Create the main layout.
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(5);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Create the side widget
  side_widget = new QWidget();
  side_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  side_widget->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  side_widget->setVisible(false);

  // Create the side outer layout
  side_outer_layout = new QVBoxLayout(side_widget);
  side_outer_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  side_outer_layout->setSpacing(5);
  side_outer_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  side_outer_layout->setSizeConstraint(QLayout::SetNoConstraint);

  // Add the side inner layout
  side_layout = new QVBoxLayout();
  side_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  side_layout->setSpacing(5);
  side_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  side_layout->setSizeConstraint(QLayout::SetNoConstraint);

  // Fill in the left layout
  QHBoxLayout* name_layout = new QHBoxLayout();
  name_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  name_layout->setSpacing(5);
  name_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  name_layout->setSizeConstraint(QLayout::SetNoConstraint);

  QLabel* name_label = new QLabel("Name");
  name_layout->addWidget(name_label, 1);

  name_edit = new QLineEdit();
  name_layout->addWidget(name_edit, 4);

  side_layout->addLayout(name_layout);

  side_outer_layout->addLayout(side_layout);

  save_button = new QPushButton("Save");
  save_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  save_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  save_button->setToolTip("Save editor changes including the graph");
  // save_button->setIcon(QIcon(":/actions/accept.png"));
  side_outer_layout->addWidget(save_button);

  side_widget->setLayout(side_outer_layout);

  // Create the scene layer.
  scene_layer = new QWidget();
  scene_layer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  scene_layer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  // Create the scene layer layout.
  scene_layer_layout = new QHBoxLayout(scene_layer);
  scene_layer_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  scene_layer_layout->setSpacing(0);
  scene_layer_layout->setSizeConstraint(QLayout::SetNoConstraint);
  scene_layer_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  scene = new VisualShaderGraphicsScene(scene_layer);
  scene->set_editor(this);

  view = new VisualShaderGraphicsView(scene, scene_layer);
  view->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  scene_layer_layout->addWidget(view);

  // Set the scene layer layout.
  scene_layer->setLayout(scene_layer_layout);

  // Set the models
  scene->set_model(visual_shader_model);

  nodes_model = const_cast<ProtoModel*>(visual_shader_model->get_sub_model(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber))));
  scene->set_nodes_model(nodes_model);

  connections_model = const_cast<ProtoModel*>(visual_shader_model->get_sub_model(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber))));
  scene->set_connections_model(connections_model);

  scene->blockSignals(true);  // Temporarily block updates
  add_output_node(); // Add the output node
  load_graph();      // Load the graph
  scene->blockSignals(false);
  scene->on_scene_update_requested();

  // Create the menu bar layer on top of the scene layer.
  top_layer = new QWidget(view);
  top_layer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  top_layer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Create the menu bar layout.
  menu_bar = new QHBoxLayout(top_layer);
  menu_bar->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  menu_bar->setSpacing(5);                       // Adjust spacing as needed
  menu_bar->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  menu_bar->setSizeConstraint(QLayout::SetMinimumSize);

  // Create the menu button
  menu_button = new QPushButton("Show Menu", top_layer);
  menu_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  menu_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  menu_button->setToolTip("Toggle Menu");
  menu_bar->addWidget(menu_button);
  QObject::connect(menu_button, &QPushButton::pressed, this, &VisualShaderEditor::on_menu_button_pressed);

  // Create the create node button.
  create_node_button = new QPushButton("Create Node", top_layer);
  create_node_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_node_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_button->setToolTip("Create a new node");
  menu_bar->addWidget(create_node_button);
  QObject::connect(create_node_button, &QPushButton::pressed, this, &VisualShaderEditor::on_create_node_button_pressed);

  this->connect(this, &VisualShaderEditor::create_node_dialog_requested, this,
                &VisualShaderEditor::show_create_node_dialog);

  // Create the preview shader button.
  preview_shader_button = new QPushButton("Preview Shader", top_layer);
  preview_shader_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  preview_shader_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  preview_shader_button->setToolTip("Preview the expected generated shader code");
  menu_bar->addWidget(preview_shader_button);
  QObject::connect(preview_shader_button, &QPushButton::pressed, this,
                   &VisualShaderEditor::on_preview_shader_button_pressed);

  zoom_in_button = new QPushButton("Zoom In", scene_layer);
  zoom_in_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  zoom_in_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  zoom_in_button->setToolTip("Zoom In");
  menu_bar->addWidget(zoom_in_button);
  QObject::connect(zoom_in_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::zoom_in);

  reset_zoom_button = new QPushButton("Reset Zoom", scene_layer);
  reset_zoom_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  reset_zoom_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  reset_zoom_button->setToolTip("Reset Zoom");
  menu_bar->addWidget(reset_zoom_button);
  QObject::connect(reset_zoom_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::reset_zoom);

  zoom_out_button = new QPushButton("Zoom Out", scene_layer);
  zoom_out_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  zoom_out_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  zoom_out_button->setToolTip("Zoom Out");
  menu_bar->addWidget(zoom_out_button);
  QObject::connect(zoom_out_button, &QPushButton::pressed, view, &VisualShaderGraphicsView::zoom_out);

  // load_image_button = new QPushButton("Load Image", scene_layer);
  // load_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  // load_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // load_image_button->setToolTip("Load an image to match");
  // menu_bar->addWidget(load_image_button);
  // QObject::connect(load_image_button, &QPushButton::pressed, this, &VisualShaderEditor::on_load_image_button_pressed);

  match_image_button = new QPushButton("Match Image", scene_layer);
  match_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  match_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  match_image_button->setToolTip("Match the shader to the loaded image");
  menu_bar->addWidget(match_image_button);
  QObject::connect(match_image_button, &QPushButton::pressed, this, &VisualShaderEditor::on_match_image_button_pressed);

  fitness_calculator = new AIAgentFitnessCalculator();

  // Set the top layer layout.
  top_layer->setLayout(menu_bar);

  // Add the left layout
  layout->addWidget(side_widget, 1);

  // Add the scene layer to the main layout.
  layout->addWidget(scene_layer, 4);

  ////////////////////////////////////
  // Code Previewer
  ////////////////////////////////////

  code_previewer_dialog = new QDialog(this);
  code_previewer_dialog->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  code_previewer_dialog->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  code_previewer_layout = new QVBoxLayout(code_previewer_dialog);
  code_previewer_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  code_previewer_layout->setSpacing(0);                   // Adjust spacing as needed
  code_previewer_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  code_previewer_layout->setSizeConstraint(QLayout::SetMinimumSize);

  code_previewer = new QPlainTextEdit(code_previewer_dialog);
  code_previewer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  code_previewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  code_previewer->setReadOnly(true);
  code_previewer->setLineWrapMode(QPlainTextEdit::NoWrap);
  code_previewer->setWordWrapMode(QTextOption::NoWrap);
  code_previewer->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  code_previewer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  code_previewer->setTabChangesFocus(true);
  code_previewer->setMinimumSize(800, 600);
  QFont monoFont("Courier New");
  monoFont.setStyleHint(QFont::Monospace);
  code_previewer->setFont(monoFont);

  code_previewer_layout->addWidget(code_previewer);

  code_previewer_dialog->setLayout(code_previewer_layout);

  ////////////////////////////////////
  // CreateNodeDialog Nodes Tree
  ////////////////////////////////////

  // Create the create node dialog under the main layout.
  create_node_dialog = new CreateNodeDialog(this);
  create_node_dialog->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  //////////////////////////////////////////
  // CreateNodeDialog Nodes Tree Children
  //////////////////////////////////////////

  std::vector<std::shared_ptr<IVisualShaderProtoNode>> t_nodes;
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeInput>>());

  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatConstant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntConstant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntConstant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeBooleanConstant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeColorConstant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec2Constant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec3Constant>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVec4Constant>>());

  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatOp>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntOp>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntOp>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorOp>>());

  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeFloatFunc>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeIntFunc>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeUIntFunc>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorFunc>>());

  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeValueNoise>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodePerlinNoise>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVoronoiNoise>>());

  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeDotProduct>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorLen>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeClamp>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeStep>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeSmoothStep>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeVectorDistance>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeMix>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorCompose>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorCompose>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorCompose>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNode2dVectorDecompose>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNode3dVectorDecompose>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNode4dVectorDecompose>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeIf>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeSwitch>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeIs>>());
  t_nodes.emplace_back(std::make_shared<VisualShaderProtoNode<VisualShaderNodeCompare>>());

  for (const std::shared_ptr<IVisualShaderProtoNode>& node : t_nodes) {
    create_node_dialog_nodes_tree_items.emplace_back(
        CreateNodeDialogNodesTreeItem(node->get_caption(), node->get_category_path(), node->get_description(), node));
  }

  // Map to store category items
  std::unordered_map<std::string, QTreeWidgetItem*> category_path_map;

  for (const VisualShaderEditor::CreateNodeDialogNodesTreeItem& item : create_node_dialog_nodes_tree_items) {
    // Parse the category string into a vector of strings
    std::vector<std::string> categories{parse_node_category_path(item.category_path)};
    QTreeWidgetItem* parent{nullptr};  // Start from the root

    std::string current_category_path;
    // Create/find each level of categories
    for (const std::string& category : categories) {
      if (!current_category_path.empty()) {
        current_category_path += "/";
      }

      current_category_path += category;

      parent = find_or_create_category_item(parent, category, current_category_path,
                                            create_node_dialog->get_nodes_tree(), category_path_map);
    }

    // Now add the item to its corresponding parent category
    QTreeWidgetItem* node_item = new QTreeWidgetItem(parent);
    node_item->setText(0, QString::fromStdString(item.caption));
    node_item->setData(1, Qt::DisplayRole, QString::fromStdString(item.description));
    node_item->setData(1, VS_PROTO_NODE_SHARED_PTR_USER_ROLE, QVariant::fromValue(item.proto_node));
  }

  //////////////// Start of Footer ////////////////

  this->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Set the window title and icon.
  this->setWindowTitle("Visual Shader Editor");
  // this->setWindowIcon(QIcon(":/resources/visual_shader.png"));
  this->setLayout(layout);
}

bool VisualShaderEditor::add_output_node() {
  int node_entry{VisualShaderGraphicsScene::find_node_entry(visual_shader_model, nodes_model, 0)};

  // If the output node exists in the model, don't add it because it will be added by the load_graph function
  SILENT_CHECK_CONDITION_TRUE_NON_VOID(node_entry != -1, true);

  // Output node has the id 0
  return scene->add_node(std::make_shared<VisualShaderProtoNode<VisualShaderNodeOutput>>(), QPointF(0, 0), 0);
}

void VisualShaderEditor::load_graph() {
  const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes_model)};
  CHECK_PARAM_NULLPTR(repeated_nodes, "Nodes is not a repeated message model.");

  // Load the nodes
  int nodes_size{nodes_model->rowCount()};
  for (int i{0}; i < nodes_size; ++i) {
    int n_id = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber))).toInt();

    double x = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kXCoordinateFieldNumber))).toDouble();

    double y = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kYCoordinateFieldNumber))).toDouble();

    std::shared_ptr<IVisualShaderProtoNode> proto_node;

    const MessageModel* node_model{repeated_nodes->get_sub_model(i)};

    // I don't care about the field number, just send any field number inside the oneof model you want to get
    const ProtoModel* oneof_model{
        node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                      FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                  false, true)};
    CHECK_PARAM_NULLPTR(oneof_model, "Oneof Model is nullptr.");
    const int oneof_value_field_number{oneof_model->get_oneof_value_field_number()};

    proto_node = shadergen_utils::get_proto_node_by_oneof_value_field_number(oneof_value_field_number);
    CHECK_PARAM_NULLPTR(proto_node, "Proto node is nullptr.");

    bool result{scene->add_node_to_scene(n_id, proto_node, QPointF(x, y))};
    CONTINUE_IF_TRUE(!result, "Failed to add node to scene");
  }

  // Load the connections
  int connections_size{connections_model->rowCount()};
  for (int i{0}; i < connections_size; ++i)  {
    int c_id = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber))).toInt();

    int from_node_id = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber))).toInt();

    int from_port_index = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber))).toInt();

    int to_node_id = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber))).toInt();

    int to_port_index = visual_shader_model->data(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                    FieldPath::RepeatedAt(i),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber))).toInt();

    bool result{scene->add_connection_to_scene(c_id, from_node_id, from_port_index, to_node_id, to_port_index)};
    CONTINUE_IF_TRUE(!result, "Failed to add connection to scene");
  }
}

void VisualShaderEditor::create_node(const QPointF& coordinate) {
  QTreeWidgetItem* selected_item{create_node_dialog->get_selected_item()};

  CHECK_PARAM_NULLPTR(selected_item, "Selected item is null");

  VisualShaderEditor::add_node(selected_item, coordinate);
}

void VisualShaderEditor::add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate) {
  QVariant proto_node = selected_item->data(1, VS_PROTO_NODE_SHARED_PTR_USER_ROLE);

  CHECK_CONDITION_TRUE(!proto_node.canConvert<std::shared_ptr<IVisualShaderProtoNode>>(), "Node type is empty");

  bool result {scene->add_node(proto_node.value<std::shared_ptr<IVisualShaderProtoNode>>(), coordinate)};
  if (!result) {
    ERROR_PRINT("Failed to add node");
  }
}

void VisualShaderEditor::show_create_node_dialog(const QPointF& coordinate) {
  int status{create_node_dialog->exec()};
  switch (status) {
    case QDialog::Accepted:
      DEBUG_PRINT("Create node dialog accepted");
      VisualShaderEditor::create_node(coordinate);
      break;
    case QDialog::Rejected:
      DEBUG_PRINT("Create node dialog rejected");
      break;
    default:
      DEBUG_PRINT("Create node dialog unknown status");
      break;
  }
}

void VisualShaderEditor::on_create_node_button_pressed() {
  // Send the center of the current view port
  QPointF coordinate{view->mapToScene(view->viewport()->rect().center())};

  Q_EMIT create_node_dialog_requested(coordinate);
}

void VisualShaderEditor::on_preview_shader_button_pressed() {
  std::string code;

  bool result{shadergen_visual_shader_generator::generate_shader(
    shadergen_visual_shader_generator::to_proto_nodes(nodes_model),
    shadergen_visual_shader_generator::to_generators(nodes_model), 
    shadergen_visual_shader_generator::to_input_output_connections_by_key(connections_model), code)};
  CHECK_CONDITION_TRUE(!result, "Failed to generate shader code");

  code_previewer->setPlainText(QString::fromStdString(code));
  code_previewer_dialog->exec();

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kFragmentShaderCodeFieldNumber)),
      QString::fromStdString(code));
  if (!result) {
    ERROR_PRINT("Failed to set fragment shader code");
  }
}

void VisualShaderEditor::on_menu_button_pressed() {
  bool is_visible{side_widget->isVisible()};
  side_widget->setVisible(!is_visible);
  menu_button->setText(!is_visible ? "Hide Menu" : "Show Menu");
}

void VisualShaderEditor::on_load_image_button_pressed() {
  // TODO: Decide on how to load an image
  // For example, use QFileDialog to open an image file or
  // load an existing sprite or background from the project.
  // Then, send the image to OriginalMatchingImageWidget widget to display it.
  // R0bert — 27/09/2024 at 22:10
  // i would use resource picker and let user pick a sprite or background that exists in the project
  // Josh — 27/09/2024 at 22:13
  // sprites have multiple frames, which is a headache for this project because it's a lot more behavior we need to define
}

void VisualShaderEditor::on_match_image_button_pressed() {
  fitness_calculator->show();
}

std::vector<std::string> VisualShaderEditor::parse_node_category_path(const std::string& n_category_path) {
  std::vector<std::string> tokens;
  std::stringstream ss(n_category_path);
  std::string token;
  while (std::getline(ss, token, '/')) {
    tokens.push_back(token);
  }
  return tokens;
}

QTreeWidgetItem* VisualShaderEditor::find_or_create_category_item(
    QTreeWidgetItem* parent, const std::string& category, const std::string& category_path,
    QTreeWidget* create_node_dialog_nodes_tree, std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map) {
  // Check if category already exists under parent
  if (category_path_map.find(category_path) != category_path_map.end()) {
    return category_path_map[category_path];
  }

  // Create a new QTreeWidgetItem
  QTreeWidgetItem* new_item;

  if (parent) {
    new_item = new QTreeWidgetItem(parent);
  } else {
    new_item = new QTreeWidgetItem(create_node_dialog_nodes_tree);
  }

  new_item->setText(0, QString::fromStdString(category));

  // Add the new category to the map
  category_path_map[category_path] = new_item;

  return new_item;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               CreateNodeDialog                             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

CreateNodeDialog::CreateNodeDialog(QWidget* parent)
    : QDialog(parent),
      layout(nullptr),
      create_node_dialog_nodes_tree_layout(nullptr),
      create_node_dialog_nodes_tree(nullptr),
      create_node_dialog_nodes_description(nullptr),
      buttons_layout(nullptr),
      create_button(nullptr),
      cancel_button(nullptr),
      selected_item(nullptr) {
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Create the nodes tree layout.
  create_node_dialog_nodes_tree_layout = new QVBoxLayout();
  create_node_dialog_nodes_tree_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog_nodes_tree_layout->setSpacing(0);                   // Adjust spacing as needed
  create_node_dialog_nodes_tree_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  create_node_dialog_nodes_tree_layout->setSizeConstraint(QLayout::SetMinimumSize);

  // Add the nodes tree layout to the main layout.
  layout->addLayout(create_node_dialog_nodes_tree_layout);

  // Create the nodes tree.
  create_node_dialog_nodes_tree = new QTreeWidget();
  create_node_dialog_nodes_tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_node_dialog_nodes_tree->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog_nodes_tree->setColumnCount(1);
  create_node_dialog_nodes_tree->setHeaderHidden(true);
  this->connect(create_node_dialog_nodes_tree, &QTreeWidget::itemSelectionChanged, this,
                &CreateNodeDialog::update_selected_item);

  // Add the nodes tree to the nodes tree layout.
  create_node_dialog_nodes_tree_layout->addWidget(create_node_dialog_nodes_tree,
                                                  2);  // 2x the size of the nodes description.

  // Create the nodes description.
  create_node_dialog_nodes_description = new QTextEdit();
  create_node_dialog_nodes_description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_node_dialog_nodes_description->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_node_dialog_nodes_description->setReadOnly(true);
  create_node_dialog_nodes_description->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Add the nodes description to the nodes tree layout.
  create_node_dialog_nodes_tree_layout->addWidget(create_node_dialog_nodes_description, 1);

  // Create the buttons layout.
  buttons_layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  create_button = new QPushButton("Create");
  QObject::connect(create_button, &QPushButton::pressed, this, &CreateNodeDialog::on_create_node_button_pressed);
  create_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  create_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  create_button->setToolTip("Create the selected node.");

  cancel_button = new QPushButton("Cancel");
  QObject::connect(cancel_button, &QPushButton::pressed, this,
                   &CreateNodeDialog::on_cancel_node_creation_button_pressed);
  cancel_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  cancel_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  cancel_button->setToolTip("Cancel the node creation.");

  // Add the buttons to the buttons layout.
  buttons_layout->addWidget(create_button);
  buttons_layout->addWidget(cancel_button);

  // Add the buttons layout to the main layout.
  layout->addLayout(buttons_layout);

  //////////////// Start of Footer ////////////////

  this->setWindowTitle("Create Shader Node");

  this->setLayout(layout);
}

void CreateNodeDialog::on_create_node_button_pressed() { this->accept(); }

void CreateNodeDialog::on_cancel_node_creation_button_pressed() { this->reject(); }

void CreateNodeDialog::update_selected_item() {
  QTreeWidgetItem* item{create_node_dialog_nodes_tree->currentItem()};
  if (item) {
    selected_item = item;
    create_node_dialog_nodes_description->setText(item->data(1, Qt::DisplayRole).toString());
  } else {
    selected_item = nullptr;
    create_node_dialog_nodes_description->setText("");
  }
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsScene                    *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

//////////////////////////////
// Public functions
//////////////////////////////

VisualShaderGraphicsScene::VisualShaderGraphicsScene(QObject* parent)
    : QGraphicsScene(parent), temporary_connection_graphics_object(nullptr) {
  setItemIndexMethod(QGraphicsScene::NoIndex);  // https://doc.qt.io/qt-6/qgraphicsscene.html#ItemIndexMethod-enum
}

bool VisualShaderGraphicsScene::add_node_to_model(const int& n_id, const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const QPointF& coordinate) {
  CHECK_CONDITION_TRUE_NON_VOID(find_node_entry(visual_shader_model, connections_model, n_id) != -1, false, "Node already exists");

  int row_entry{nodes_model->append_row()};

  bool result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)),
      n_id);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set node id");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderNode::kXCoordinateFieldNumber)),
      coordinate.x());
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set node x coordinate");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderNode::kYCoordinateFieldNumber)),
      coordinate.y());
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set node y coordinate");

  // Pass any field number that is inside the oneof to enter te OneofModel.
  // You must also to pass true for `for_get_oneof` parameter.
  ProtoModel* oneof_model{const_cast<ProtoModel*>(visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)), false, true))};
  OneofModel* oneof{dynamic_cast<OneofModel*>(oneof_model)};
  CHECK_PARAM_NULLPTR_NON_VOID(oneof, false, "Failed to get oneof model");

  const int oneof_value_field_number = proto_node->get_oneof_value_field_number();
  result = oneof->set_oneof(oneof_value_field_number);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set oneof");

  return true;
}

bool VisualShaderGraphicsScene::add_node_to_scene(const int& n_id, const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const QPointF& coordinate) {
  CHECK_CONDITION_TRUE_NON_VOID(node_graphics_objects.find(n_id) != node_graphics_objects.end(), false,
                                "Graphics node object already exists");

  QList<QGraphicsView*> views{this->views()};
  CHECK_CONDITION_TRUE_NON_VOID(views.isEmpty(), false, "No views available");

  VisualShaderGraphicsView* view{dynamic_cast<VisualShaderGraphicsView*>(views.first())};

  if (coordinate.x() < view->get_x() || coordinate.x() > view->get_x() + view->get_width() ||
      coordinate.y() < view->get_y() || coordinate.y() > view->get_y() + view->get_height()) {
    WARN_PRINT("Node is out of view bounds");
  }

  VisualShaderNodeGraphicsObject* n_o{new VisualShaderNodeGraphicsObject(n_id, coordinate, proto_node)};

  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::node_moved, this, &VisualShaderGraphicsScene::on_node_moved);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::in_port_pressed, this,
                   &VisualShaderGraphicsScene::on_port_pressed);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::in_port_dragged, this,
                   &VisualShaderGraphicsScene::on_port_dragged);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::in_port_dropped, this,
                   &VisualShaderGraphicsScene::on_port_dropped);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::out_port_pressed, this,
                   &VisualShaderGraphicsScene::on_port_pressed);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::out_port_dragged, this,
                   &VisualShaderGraphicsScene::on_port_dragged);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::out_port_dropped, this,
                   &VisualShaderGraphicsScene::on_port_dropped);

  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::scene_update_requested, this,
                   &VisualShaderGraphicsScene::on_scene_update_requested);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::in_port_remove_requested, this,
                   &VisualShaderGraphicsScene::on_in_port_remove_requested);
  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::out_port_remove_requested, this,
                   &VisualShaderGraphicsScene::on_out_port_remove_requested);

  // If not the output node
  if (n_id != 0) {
    QObject::connect(n_o, &VisualShaderNodeGraphicsObject::node_deleted, this,
      &VisualShaderGraphicsScene::on_node_deleted);
  }

  int row_entry{VisualShaderGraphicsScene::find_node_entry(visual_shader_model, nodes_model, n_id)};
  int node_type_field_number{VisualShaderGraphicsScene::get_node_type_field_number(nodes_model, row_entry)};

  if (n_id != 0) {
    QWidget* embed_widget{new QWidget()};

    embed_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    embed_widget->setContentsMargins(2, 2, 2, 2);  // Left, top, right, bottom

    QVBoxLayout* embed_widget_layout = new QVBoxLayout(embed_widget);
    embed_widget_layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
    embed_widget_layout->setSizeConstraint(QLayout::SetMinimumSize);
    embed_widget_layout->setSpacing(2);
    embed_widget_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    switch (proto_node->get_oneof_value_field_number()) {
      case VisualShader::VisualShaderNode::kInputFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeInputType_descriptor(), VisualShaderNodeInput::kTypeFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kFloatConstantFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeFloatConstant::kValueFieldNumber, "Value");
        break;
      }
      case VisualShader::VisualShaderNode::kIntConstantFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_INT(VisualShaderNodeIntConstant::kValueFieldNumber, "Value");
        break;
      }
      case VisualShader::VisualShaderNode::kUintConstantFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_UINT(VisualShaderNodeUIntConstant::kValueFieldNumber, "Value");
        break;
      }
      case VisualShader::VisualShaderNode::kBooleanConstantFieldNumber: {
        REGISTER_NODE_FIELD_CHECK_BOX(VisualShaderNodeBooleanConstant::kValueFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kColorConstantFieldNumber: {
        REGISTER_NODE_FIELD_SPIN_BOX(VisualShaderNodeColorConstant::kRFieldNumber, 0, 255);
        REGISTER_NODE_FIELD_SPIN_BOX(VisualShaderNodeColorConstant::kGFieldNumber, 0, 255);
        REGISTER_NODE_FIELD_SPIN_BOX(VisualShaderNodeColorConstant::kBFieldNumber, 0, 255);
        REGISTER_NODE_FIELD_SPIN_BOX(VisualShaderNodeColorConstant::kAFieldNumber, 0, 255);

        // Retrieve the RGBA spin boxes
        QWidget* r_spin = node_field_widgets[n_id][VisualShaderNodeColorConstant::kRFieldNumber];
        QWidget* g_spin = node_field_widgets[n_id][VisualShaderNodeColorConstant::kGFieldNumber];
        QWidget* b_spin = node_field_widgets[n_id][VisualShaderNodeColorConstant::kBFieldNumber];
        QWidget* a_spin = node_field_widgets[n_id][VisualShaderNodeColorConstant::kAFieldNumber];

        VisualShaderNodeFieldSpinBox* typed_r_spin = dynamic_cast<VisualShaderNodeFieldSpinBox*>(r_spin);
        VisualShaderNodeFieldSpinBox* typed_g_spin = dynamic_cast<VisualShaderNodeFieldSpinBox*>(g_spin);
        VisualShaderNodeFieldSpinBox* typed_b_spin = dynamic_cast<VisualShaderNodeFieldSpinBox*>(b_spin);
        VisualShaderNodeFieldSpinBox* typed_a_spin = dynamic_cast<VisualShaderNodeFieldSpinBox*>(a_spin);

        // Create a color preview label
        QLabel* color_preview = new QLabel(embed_widget);
        color_preview->setFixedHeight(20);
        color_preview->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        color_preview->setContentsMargins(0, 0, 0, 0);

        // Function to update the color preview
        auto update_color_preview = [=]() {
            QColor color(
                typed_r_spin->value(),
                typed_g_spin->value(),
                typed_b_spin->value(),
                typed_a_spin->value()
            );
            QPixmap pixmap(color_preview->size());
            pixmap.fill(color);
            color_preview->setPixmap(pixmap);
        };

        // Set initial color
        update_color_preview();

        // Connect each spin box to update the color preview
        QObject::connect(typed_r_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int) { update_color_preview(); });
        QObject::connect(typed_g_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int) { update_color_preview(); });
        QObject::connect(typed_b_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int) { update_color_preview(); });
        QObject::connect(typed_a_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int) { update_color_preview(); });

        // Add the color preview to the layout
        embed_widget_layout->addWidget(color_preview);
        break;
      }
      case VisualShader::VisualShaderNode::kVec2ConstantFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec2Constant::kXFieldNumber, "X");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec2Constant::kYFieldNumber, "Y");
        break;
      }
      case VisualShader::VisualShaderNode::kVec3ConstantFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec3Constant::kXFieldNumber, "X");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec3Constant::kYFieldNumber, "Y");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec3Constant::kZFieldNumber, "Z");
        break;
      }
      case VisualShader::VisualShaderNode::kVec4ConstantFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec4Constant::kXFieldNumber, "X");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec4Constant::kYFieldNumber, "Y");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec4Constant::kZFieldNumber, "Z");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVec4Constant::kWFieldNumber, "W");
        break;
      }
      case VisualShader::VisualShaderNode::kFloatOpFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeFloatOp::VisualShaderNodeFloatOpType_descriptor(), VisualShaderNodeFloatOp::kOpFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kIntOpFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeIntOp::VisualShaderNodeIntOpType_descriptor(), VisualShaderNodeIntOp::kOpFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kUintOpFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeUIntOp::VisualShaderNodeUIntOpType_descriptor(), VisualShaderNodeUIntOp::kOpFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kVectorOpFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeVectorType_descriptor(), VisualShaderNodeVectorOp::kTypeFieldNumber);
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeVectorOp::VisualShaderNodeVectorOpType_descriptor(), VisualShaderNodeVectorOp::kOpFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kFloatFuncFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeFloatFunc::VisualShaderNodeFloatFuncType_descriptor(), VisualShaderNodeFloatFunc::kFuncFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kIntFuncFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeIntFunc::VisualShaderNodeIntFuncType_descriptor(), VisualShaderNodeIntFunc::kFuncFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kUintFuncFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeUIntFunc::VisualShaderNodeUIntFuncType_descriptor(), VisualShaderNodeUIntFunc::kFuncFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kVectorFuncFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeVectorType_descriptor(), VisualShaderNodeVectorFunc::kTypeFieldNumber);
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeVectorFunc::VisualShaderNodeVectorFuncType_descriptor(), VisualShaderNodeVectorFunc::kFuncFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kValueNoiseFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeValueNoise::kScaleFieldNumber, "Scale");
        break;
      }
      case VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodePerlinNoise::kScaleFieldNumber, "Scale");
        break;
      }
      case VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber: {
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber, "Angle Offset");
        REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber, "Cell Density");
        break;
      }
      case VisualShader::VisualShaderNode::kIsFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeIs::Function_descriptor(), VisualShaderNodeIs::kFuncFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kCompareFieldNumber: {
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeCompare::ComparisonType_descriptor(), VisualShaderNodeCompare::kTypeFieldNumber);
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeCompare::Function_descriptor(), VisualShaderNodeCompare::kFuncFieldNumber);
        REGISTER_NODE_FIELD_COMBO_BOX(VisualShaderNodeCompare::Condition_descriptor(), VisualShaderNodeCompare::kCondFieldNumber);
        break;
      }
      case VisualShader::VisualShaderNode::kDotProductFieldNumber:
      case VisualShader::VisualShaderNode::kVectorLenFieldNumber:
      case VisualShader::VisualShaderNode::kClampFieldNumber:
      case VisualShader::VisualShaderNode::kStepFieldNumber:
      case VisualShader::VisualShaderNode::kSmoothStepFieldNumber:
      case VisualShader::VisualShaderNode::kVectorDistanceFieldNumber:
      case VisualShader::VisualShaderNode::kMixFieldNumber:
      case VisualShader::VisualShaderNode::kVector2DComposeFieldNumber:
      case VisualShader::VisualShaderNode::kVector3DComposeFieldNumber:
      case VisualShader::VisualShaderNode::kVector4DComposeFieldNumber:
      case VisualShader::VisualShaderNode::kVector2DDecomposeFieldNumber:
      case VisualShader::VisualShaderNode::kVector3DDecomposeFieldNumber:
      case VisualShader::VisualShaderNode::kVector4DDecomposeFieldNumber:
      case VisualShader::VisualShaderNode::kIfNodeFieldNumber:
      case VisualShader::VisualShaderNode::kSwitchNodeFieldNumber:
        break;
      default:
        FAIL_AND_RETURN_NON_VOID(false, "Unknown node type");
        break;
    }

    // Create the button that will show/hide the shader previewer
    QPushButton* preview_shader_button = new QPushButton("Show Preview", embed_widget);
    preview_shader_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    preview_shader_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
    preview_shader_button->setToolTip("Show generated shader at this node");
    QObject::connect(preview_shader_button, &QPushButton::pressed, n_o,
      &VisualShaderNodeGraphicsObject::on_preview_shader_button_pressed);

    embed_widget_layout->addWidget(preview_shader_button);

    embed_widget->setLayout(embed_widget_layout);

    // This must be done after the widget's layout is complete
    // https://doc.qt.io/qt-6/qgraphicsproxywidget.html#setWidget
    QGraphicsProxyWidget* embed_widget_proxy{new QGraphicsProxyWidget(n_o)};
    embed_widget_proxy->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    embed_widget_proxy->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
    embed_widget_proxy->setWidget(embed_widget);

    n_o->set_embed_widget(embed_widget); // Set the embed widget to the node graphics object

    if (RendererWidget* spw{n_o->get_renderer_widget()}) {
      QObject::connect(spw, &RendererWidget::scene_update_requested, this,
                      &VisualShaderGraphicsScene::on_scene_update_requested);
    }
  }

  n_o->update_layout(); // Update the layout of the node

  // Now the update_layout function has been called and our ports are ready, let's update the current
  // index if the node is input so that our ports gets the correct type
  if (proto_node->get_oneof_value_field_number() == VisualShader::VisualShaderNode::kInputFieldNumber) {
    QVariant current_index{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
      FieldPath::RepeatedAt(row_entry),
      FieldPath::FieldNumber(node_type_field_number),
      FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)))};
    n_o->on_input_node_out_port_type_changed(current_index.toInt());
  }

  node_graphics_objects[n_id] = n_o;

  addItem(n_o);

  return true;
}

bool VisualShaderGraphicsScene::add_node(const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const QPointF& coordinate, const int& n_id) {
  int temp_n_id{n_id};

  if (temp_n_id == -1)  {
    temp_n_id = VisualShaderGraphicsScene::get_new_node_id(visual_shader_model, nodes_model);
    CHECK_CONDITION_TRUE_NON_VOID(temp_n_id == 0, false, "The id " + std::to_string(temp_n_id) + " is reserved for the output node");
  }

  return add_node_to_model(temp_n_id, proto_node, coordinate) && add_node_to_scene(temp_n_id, proto_node, coordinate);
}

bool VisualShaderGraphicsScene::delete_node_from_model(const int& n_id) {
  int row_entry{find_node_entry(visual_shader_model, nodes_model, n_id)};
  VALIDATE_INDEX_NON_VOID(row_entry, nodes_model->rowCount(), false, "Node entry not found");

  // Remove the node from the model
  return nodes_model->remove_row(row_entry);
}

bool VisualShaderGraphicsScene::delete_node_from_scene(const int& n_id, const int& in_port_count, const int& out_port_count) {
  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(n_o, false, "Node graphics object is null");

  // Remove all connections to the node
  for (int i{0}; i < in_port_count; i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!i_port || !i_port->is_connected());

    // Get the output port of the connection
    const int c_id{i_port->get_c_id()};
    VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
    SILENT_CONTINUE_IF_TRUE(!c_o);

    CONTINUE_IF_TRUE(!this->delete_connection(c_o->get_id(), c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i), "Failed to delete connection");
  }

  for (int i{0}; i < out_port_count; i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!o_port || !o_port->is_connected());

    std::unordered_set<int> c_ids{o_port->get_c_ids()};

    for (const int& c_id : c_ids) {
      VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
      SILENT_CONTINUE_IF_TRUE(!c_o);

      CONTINUE_IF_TRUE(!this->delete_connection(c_o->get_id(), n_id, i, c_o->get_to_node_id(), c_o->get_to_port_index()), "Failed to delete connection");
    }
  }

  // Remove the node from the scene
  // TODO: Why if we exchange the order of these two lines, the program crashes?
  int erased_count{(int)this->node_graphics_objects.erase(n_id)};
  CHECK_CONDITION_TRUE_NON_VOID(erased_count == 0, false, "Failed to erase node graphics object");

  remove_item(n_o);

  return true;
}

bool VisualShaderGraphicsScene::delete_node(const int& n_id, const int& in_port_count, const int& out_port_count) {
  return delete_node_from_model(n_id) && delete_node_from_scene(n_id, in_port_count, out_port_count);
}

bool VisualShaderGraphicsScene::update_node_in_model(const int& n_id, const int& field_number, const QVariant& value) {
  int row_entry{find_node_entry(visual_shader_model, nodes_model, n_id)};
  CHECK_CONDITION_TRUE_NON_VOID(row_entry == -1, false, "Failed to find node entry");

  const int oneof_value_field_number{get_node_type_field_number(nodes_model, row_entry)};
  CHECK_CONDITION_TRUE_NON_VOID(oneof_value_field_number == -1, false, "Failed to get oneof value field number");

  bool result{visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(oneof_value_field_number), FieldPath::FieldNumber(field_number)),
      value)};
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to update node in model");

  return true;
}

bool VisualShaderGraphicsScene::update_node_in_scene(const int& n_id, const int& field_number, const QVariant& value) {
  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(n_o, false, "Node graphics object is null");

  auto it = node_field_widgets.find(n_id);
  CHECK_CONDITION_TRUE_NON_VOID(it == node_field_widgets.end(), false, "Node field widgets not found");
  auto it2 = it->second.find(field_number);
  CHECK_CONDITION_TRUE_NON_VOID(it2 == it->second.end(), false, "Node field widgets not found");

  QWidget* widget{it2->second};

  widget->blockSignals(true); // Block signals to prevent saving the value to the model

  if (auto combo_box{dynamic_cast<VisualShaderNodeFieldComboBox*>(widget)}) {
    combo_box->set_current_index(value.toInt());
  } else if (auto line_edit_float{dynamic_cast<VisualShaderNodeFieldLineEditFloat*>(widget)}) {
    line_edit_float->set_current_text(value.toString().toStdString());
  } else if (auto line_edit_int{dynamic_cast<VisualShaderNodeFieldLineEditInt*>(widget)})  {
    line_edit_int->set_current_text(value.toString().toStdString());
  } else if (auto line_edit_uint{dynamic_cast<VisualShaderNodeFieldLineEditUInt*>(widget)}) {
    line_edit_uint->set_current_text(value.toString().toStdString());
  } else if (auto check_box{dynamic_cast<VisualShaderNodeFieldCheckBox*>(widget)}) {
    check_box->set_checked(value.toBool());
  } else if (auto spin_box{dynamic_cast<VisualShaderNodeFieldSpinBox*>(widget)}) {
    spin_box->set_value(value.toInt());
  } else {
    FAIL_AND_RETURN_NON_VOID(false, "Unknown widget type");
  }

  widget->blockSignals(false); // Unblock signals

  return true;
}

bool VisualShaderGraphicsScene::update_node(const int& n_id, const int& field_number, const QVariant& value) {
  return update_node_in_model(n_id, field_number, value) &&
         update_node_in_scene(n_id, field_number, value);
}

void VisualShaderGraphicsScene::on_update_renderer_widgets_requested() {
  for (auto& [n_id, n_o] : node_graphics_objects) {
    SILENT_CONTINUE_IF_TRUE(n_id == 0);  // Skip the output node

    RendererWidget* spw{n_o->get_renderer_widget()};
    if (!spw) {
      continue;
    }

    spw->set_code(shadergen_visual_shader_generator::generate_preview_shader(shadergen_visual_shader_generator::to_proto_nodes(nodes_model),
                                    shadergen_visual_shader_generator::to_generators(nodes_model), 
                                    shadergen_visual_shader_generator::to_input_output_connections_by_key(connections_model), n_id, 0));  // 0 is the output port index
  }

  on_scene_update_requested();

  // Reset the generated shader code
  bool result{visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kFragmentShaderCodeFieldNumber)), "")};
  if (!result) {
    ERROR_PRINT("Failed to reset the generated shader code");
  }
}

void VisualShaderGraphicsScene::on_in_port_remove_requested(VisualShaderInputPortGraphicsObject* in_port) {
  if (in_port->is_connected()) {
    const int c_id{in_port->get_c_id()};
    VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
    CHECK_PARAM_NULLPTR(c_o, "Connection graphics object is null");

    CHECK_CONDITION_TRUE(!delete_connection(c_o->get_id(), c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(),
                      c_o->get_to_port_index()),
                     "Failed to delete connection");
  }

  remove_item(in_port);
}

void VisualShaderGraphicsScene::on_out_port_remove_requested(VisualShaderOutputPortGraphicsObject* out_port) {
  if (out_port->is_connected()) {
    std::unordered_set<int> c_ids{out_port->get_c_ids()};

    for (const int& c_id : c_ids) {
      VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
      SILENT_CONTINUE_IF_TRUE(!c_o);

      CONTINUE_IF_TRUE(!delete_connection(c_o->get_id(), c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(),
                              c_o->get_to_port_index()),
                       "Failed to delete connection");
    }
  }

  remove_item(out_port);
}

int VisualShaderGraphicsScene::get_new_node_id(ProtoModel* visual_shader_model, ProtoModel* nodes_model) {
  int size{nodes_model->rowCount()};

  int max_id{0};
  for (int i{0}; i < size; i++) {
    // Path to the id field of the node
    FieldPath path{FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                               FieldPath::RepeatedAt(i),
                                               FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber))};
    int n_id{visual_shader_model->data(path).toInt()};
    if (n_id > max_id) max_id = n_id;
  }

  return max_id + 1;  // Minimum id is 1 (0 is reserved for the output node)
}

int VisualShaderGraphicsScene::get_new_connection_id(ProtoModel* visual_shader_model, ProtoModel* connections_model) {
  int size{connections_model->rowCount()};

  int max_id{-1};
  for (int i{0}; i < size; i++) {
    // Path to the id field of the connection
    FieldPath path{FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(i),
        FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber))};
    int c_id{visual_shader_model->data(path).toInt()};
    if (c_id > max_id) max_id = c_id;
  }

  return max_id + 1;  // Minimum id is 0
}

int VisualShaderGraphicsScene::find_node_entry(ProtoModel* visual_shader_model, ProtoModel* nodes_model,
                                               const int& n_id) {
  int size{nodes_model->rowCount()};

  for (int i{0}; i < size; i++) {
    // Path to the id field of the node
    FieldPath path{FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                               FieldPath::RepeatedAt(i),
                                               FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber))};
    if (visual_shader_model->data(path).toInt() == n_id) return i;
  }

  return -1;
}

int VisualShaderGraphicsScene::find_connection_entry(ProtoModel* visual_shader_model, ProtoModel* connections_model,
                                                     const int& c_id) {
  int size{connections_model->rowCount()};

  for (int i{0}; i < size; i++) {
    // Path to the id field of the connection
    FieldPath path{FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(i),
        FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber))};
    if (visual_shader_model->data(path).toInt() == c_id) return i;
  }

  return -1;
}

int VisualShaderGraphicsScene::get_node_type_field_number(ProtoModel* nodes_model, const int& row_entry) {
  const RepeatedMessageModel* repeated_nodes{dynamic_cast<const RepeatedMessageModel*>(nodes_model)};
  CHECK_PARAM_NULLPTR_NON_VOID(repeated_nodes, -1, "Nodes is not a repeated message model.");

  const MessageModel* node_model{repeated_nodes->get_sub_model(row_entry)};

  // I don't care about the field number, just send any field number inside the oneof model you want to get
  const ProtoModel* oneof_model{
      node_model->get_sub_model(FieldPath::Of<VisualShader::VisualShaderNode>(
                                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber)),
                                false, true)};
  CHECK_PARAM_NULLPTR_NON_VOID(oneof_model, -1, "Oneof Model is nullptr.");

  return oneof_model->get_oneof_value_field_number();
}

bool VisualShaderGraphicsScene::add_connection_to_model(const int& c_id, const int& from_node_id, const int& from_port_index, const int& to_node_id,
                              const int& to_port_index) {
  CHECK_CONDITION_TRUE_NON_VOID(find_connection_entry(visual_shader_model, connections_model, c_id) != -1, false, "Connection already exists");
  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_connection(from_node_id, from_port_index, to_node_id, to_port_index), false, "Invalid connection");

  int row_entry{connections_model->append_row()};

  bool result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber)),
      c_id);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection id");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(
          FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry),
          FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber)),
      from_node_id);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection from node id");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(
          FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry),
          FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber)),
      from_port_index);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection from port index");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber)),
      to_node_id);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection to node id");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(
          FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry),
          FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber)),
      to_port_index);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection to port index");

  return true;
}
                            
bool VisualShaderGraphicsScene::add_connection_to_scene(const int& from_node_id, const int& from_port_index, const int& to_node_id,
                              const int& to_port_index) {
  CHECK_PARAM_NULLPTR_NON_VOID(this->temporary_connection_graphics_object, false, "Temporary connection object is null");
  CHECK_CONDITION_TRUE_NON_VOID(connection_graphics_objects.find(this->temporary_connection_graphics_object->get_id()) != connection_graphics_objects.end(), false, "Connection graphics object already exists");

  // Create the connection and set its start
  VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

  VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(to_i_port->is_connected(), false, "Connection is already connected");

  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_connection(from_node_id, from_port_index, to_node_id, to_port_index), false, "Invalid connection");

  CHECK_CONDITION_TRUE_NON_VOID(!check_if_connection_out_of_bounds(from_o_port, to_i_port), false, "Connection is out of bounds");

  CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->connect(this->temporary_connection_graphics_object->get_id()), false, "Failed to connect to input port graphics object");
  this->temporary_connection_graphics_object->set_end_coordinate(to_i_port->get_global_coordinate());
  this->temporary_connection_graphics_object->set_to_node_id(to_node_id);
  this->temporary_connection_graphics_object->set_to_port_index(to_port_index);

  this->temporary_connection_graphics_object->update_layout();
  connection_graphics_objects[this->temporary_connection_graphics_object->get_id()] = this->temporary_connection_graphics_object;

  this->temporary_connection_graphics_object = nullptr;  // Make sure to reset the temporary connection object

  on_update_renderer_widgets_requested();

  return true;
}

bool VisualShaderGraphicsScene::add_connection_to_scene(const int& c_id, const int& from_node_id, const int& from_port_index, const int& to_node_id,
                               const int& to_port_index) {
  CHECK_CONDITION_TRUE_NON_VOID(connection_graphics_objects.find(c_id) != connection_graphics_objects.end(), false,
                                "Connection graphics object already exists");

  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

  VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!is_valid_connection(from_node_id, from_port_index, to_node_id, to_port_index), false, "Invalid connection");

  CHECK_CONDITION_TRUE_NON_VOID(!check_if_connection_out_of_bounds(from_o_port, to_i_port), false, "Connection is out of bounds");
  CHECK_CONDITION_TRUE_NON_VOID(to_i_port->is_connected(), false, "Connection is already connected");

  CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->connect(c_id), false, "Failed to connect from connection graphics object");
  CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->connect(c_id), false, "Failed to connect to connection graphics object");

  VisualShaderConnectionGraphicsObject* c_o{new VisualShaderConnectionGraphicsObject(
      c_id, from_node_id, from_port_index, from_o_port->get_global_coordinate())};

  c_o->set_end_coordinate(to_i_port->get_global_coordinate());
  c_o->set_to_node_id(to_node_id);
  c_o->set_to_port_index(to_port_index);

  c_o->update_layout();
  connection_graphics_objects[c_id] = c_o;

  addItem(c_o);

  on_update_renderer_widgets_requested();

  return true;
}

bool VisualShaderGraphicsScene::add_connection(const int& c_id, const int& from_node_id, const int& from_port_index,
                                               const int& to_node_id, const int& to_port_index) {
  return add_connection_to_model(c_id, from_node_id, from_port_index, to_node_id, to_port_index) &&
         add_connection_to_scene(from_node_id, from_port_index, to_node_id, to_port_index);
}

bool VisualShaderGraphicsScene::is_valid_connection(const int& from_node_id, const int& from_port_index, const int& to_node_id, const int& to_port_index) const {
  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

  VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

  return controller_utils::is_valid_connection(from_o_port->get_port_type(), to_i_port->get_port_type());
}

void VisualShaderGraphicsScene::revalidate_connections(const int& n_id) {
  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};
  CHECK_PARAM_NULLPTR(n_o, "Node graphics object is null");

  int in_port_count{n_o->get_input_port_count()};

  for (int i{0}; i < in_port_count; i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!i_port || !i_port->is_connected());

    // Get the output port of the connection
    const int c_id{i_port->get_c_id()};
    VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
    SILENT_CONTINUE_IF_TRUE(!c_o);

    if (!is_valid_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i)) {
      CONTINUE_IF_TRUE(!this->delete_connection(c_o->get_id(), c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i), "Failed to delete connection");
    }
  }

  int out_port_count{n_o->get_output_port_count()};

  for (int i{0}; i < out_port_count; i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!o_port || !o_port->is_connected());

    std::unordered_set<int> c_ids{o_port->get_c_ids()};

    for (const int& c_id : c_ids) {
      VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
      SILENT_CONTINUE_IF_TRUE(!c_o);

      if (!is_valid_connection(n_id, i, c_o->get_to_node_id(), c_o->get_to_port_index())) {
        CONTINUE_IF_TRUE(!this->delete_connection(c_o->get_id(), n_id, i, c_o->get_to_node_id(), c_o->get_to_port_index()), "Failed to delete connection");
      }
    }
  }
}

bool VisualShaderGraphicsScene::delete_connection_from_model(const int& c_id) {
  int row_entry{find_connection_entry(visual_shader_model, connections_model, c_id)};
  VALIDATE_INDEX_NON_VOID(row_entry, connections_model->rowCount(), false, "Connection entry not found");

  return connections_model->remove_row(row_entry);
}

bool VisualShaderGraphicsScene::delete_connection_from_scene(const int& c_id, const int& from_node_id, const int& from_port_index,
                                  const int& to_node_id, const int& to_port_index) {
  VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

  VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->is_connected(), false, "Output port is not connected");

  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->is_connected(), false, "Output port is not connected");
  CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->detach_connection(), false, "Failed to detach connection");
  CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->detach_connection(c_id), false, "Failed to detach connection");

  VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(c_o, false, "Failed to get connection graphics object");

  int erased_count{(int)this->connection_graphics_objects.erase(c_id)};
  CHECK_CONDITION_TRUE_NON_VOID(erased_count == 0, false, "Failed to erase connection graphics object");

  remove_item(c_o);

  on_update_renderer_widgets_requested();

  return true;
}

bool VisualShaderGraphicsScene::delete_connection(const int& c_id, const int& from_node_id, const int& from_port_index,
                                                  const int& to_node_id, const int& to_port_index) {
  return delete_connection_from_model(c_id) &&
         delete_connection_from_scene(c_id, from_node_id, from_port_index, to_node_id, to_port_index);
}

bool VisualShaderGraphicsScene::add_temporary_connection(const int& from_node_id, const int& from_port_index) {
  if  (this->temporary_connection_graphics_object != nullptr)  {
    return false;
  }

  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  int c_id{get_new_connection_id(visual_shader_model, connections_model)};
  CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->connect(c_id), false, "Failed to connect connection");
  this->temporary_connection_graphics_object = new VisualShaderConnectionGraphicsObject(
      c_id, from_node_id, from_port_index, from_o_port->get_global_coordinate());
  this->temporary_connection_graphics_object->update_layout();
  addItem(this->temporary_connection_graphics_object);

  return true;
}

bool VisualShaderGraphicsScene::delete_temporary_connection(const int& from_node_id, const int& from_port_index) {
  CHECK_PARAM_NULLPTR_NON_VOID(this->temporary_connection_graphics_object, false, "Temporary connection object is null");

  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->is_connected(), false, "Output port is not connected");

  CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->detach_connection(this->temporary_connection_graphics_object->get_id()), false, "Failed to detach connection");
  remove_item(this->temporary_connection_graphics_object);
  this->temporary_connection_graphics_object = nullptr;

  return true;
}

bool VisualShaderGraphicsScene::convert_to_temporary_connection(const int& c_id, const int& to_node_id, const int& to_port_index) {
  VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(c_o, false, "Connection graphics object is null");

  VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

  VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->is_connected(), false, "Input port is not connected");
  
  CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->detach_connection(), false, "Failed to detach connection");
  c_o->detach_end();

  int erased_count{(int)this->connection_graphics_objects.erase(c_id)};
  CHECK_CONDITION_TRUE_NON_VOID(erased_count == 0, false, "Failed to erase connection graphics object");

  on_update_renderer_widgets_requested();

  return true;
}

bool VisualShaderGraphicsScene::update_connection_in_model(const int& c_id, const int& node_id_field_number, const int& port_index_field_number, const int& node_id, const int& port_index) {
  int row_entry{find_connection_entry(visual_shader_model, connections_model, c_id)};
  VALIDATE_INDEX_NON_VOID(row_entry, connections_model->rowCount(), false, "Connection entry not found");

  bool result{visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(node_id_field_number)),
      node_id)};
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to update connection in model");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(port_index_field_number)),
      port_index);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to update connection in model");

  return true;
}

bool VisualShaderGraphicsScene::update_connection_in_scene(const int& c_id, const int& node_id_field_number, const int& node_id, const int& port_index) {
  VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(c_o, false, "Failed to get connection graphics object");

  switch (node_id_field_number) {
    case VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber: {
      VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(c_o->get_from_node_id())};
      CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

      VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(c_o->get_from_port_index())};
      CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

      VisualShaderNodeGraphicsObject* target_from_n_o{this->get_node_graphics_object(node_id)};
      CHECK_PARAM_NULLPTR_NON_VOID(target_from_n_o, false, "Failed to get from node graphics object");

      VisualShaderOutputPortGraphicsObject* target_from_o_port{target_from_n_o->get_output_port_graphics_object(port_index)};
      CHECK_PARAM_NULLPTR_NON_VOID(target_from_o_port, false,  "Failed to get from output port graphics object");

      CHECK_CONDITION_TRUE_NON_VOID(!from_o_port->detach_connection(c_id), false, "Failed to detach connection");
      CHECK_CONDITION_TRUE_NON_VOID(!target_from_o_port->connect(c_id), false, "Failed to connect connection");

      c_o->set_from_node_id(node_id);
      c_o->set_from_port_index(port_index);
      c_o->set_start_coordinate(target_from_o_port->get_global_coordinate());
      c_o->update_layout();

      break;
    }
    case VisualShader::VisualShaderConnection::kToNodeIdFieldNumber: {
      VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(c_o->get_to_node_id())};
      CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

      VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(c_o->get_to_port_index())};
      CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

      VisualShaderNodeGraphicsObject* target_to_n_o{this->get_node_graphics_object(node_id)};
      CHECK_PARAM_NULLPTR_NON_VOID(target_to_n_o, false, "Failed to get to node graphics object");

      VisualShaderInputPortGraphicsObject* target_to_i_port{target_to_n_o->get_input_port_graphics_object(port_index)};
      CHECK_PARAM_NULLPTR_NON_VOID(target_to_i_port, false, "Failed to get to input port graphics object");

      CHECK_CONDITION_TRUE_NON_VOID(!to_i_port->detach_connection(), false, "Failed to detach connection");
      CHECK_CONDITION_TRUE_NON_VOID(!target_to_i_port->connect(c_id), false, "Failed to connect connection");

      c_o->set_to_node_id(node_id);
      c_o->set_to_port_index(port_index);
      c_o->set_end_coordinate(target_to_i_port->get_global_coordinate());
      c_o->update_layout();

      break;
    }
    default: {
      FAIL_AND_RETURN_NON_VOID(false, "Unknown field number");
    }
  }

  return true;
}

bool VisualShaderGraphicsScene::update_connection(const int& c_id, const int& node_id_field_number, const int& port_index_field_number, const int& node_id, const int& port_index) {
  return update_connection_in_model(c_id, node_id_field_number, port_index_field_number, node_id, port_index) &&
         update_connection_in_scene(c_id, node_id_field_number, node_id, port_index);
}

VisualShaderNodeGraphicsObject* VisualShaderGraphicsScene::get_node_graphics_object(const int& n_id) const {
  VisualShaderNodeGraphicsObject* n_o{nullptr};

  auto it{node_graphics_objects.find(n_id)};
  if (it != node_graphics_objects.end()) {
    n_o = it->second;
  }

  return n_o;
}

VisualShaderConnectionGraphicsObject* VisualShaderGraphicsScene::get_connection_graphics_object(const int& c_id) const {
  VisualShaderConnectionGraphicsObject* c_o{nullptr};

  auto it{connection_graphics_objects.find(c_id)};
  if  (it  != connection_graphics_objects.end())  {
    c_o = it->second;
  }

  return c_o;
}

QImage VisualShaderGraphicsScene::get_output_shader_image() {
  // Find the node connected to the output node
  VisualShaderNodeGraphicsObject* n_o{get_node_graphics_object(0)};
  CHECK_PARAM_NULLPTR_NON_VOID(n_o, QImage(), "Failed to get output node graphics object");

  VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(0)};
  CHECK_PARAM_NULLPTR_NON_VOID(i_port, QImage(), "Failed to get output node input port graphics object");

  CHECK_CONDITION_TRUE_NON_VOID(!i_port->is_connected(), QImage(), "Output node is not connected");

  const int c_id{i_port->get_c_id()};

  VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(c_o, QImage(), "Failed to get connection graphics object");

  VisualShaderNodeGraphicsObject* from_n_o{get_node_graphics_object(c_o->get_from_node_id())};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, QImage(), "Failed to get from node graphics object");

  RendererWidget* spw{from_n_o->get_renderer_widget()};
  CHECK_PARAM_NULLPTR_NON_VOID(spw, QImage(), "Failed to get shader preview widget");

  return spw->grabFramebuffer();
}

void VisualShaderGraphicsScene::on_scene_update_requested() { update(); }

void VisualShaderGraphicsScene::on_node_deleted(const int& n_id, const int& in_port_count, const int& out_port_count) {
  CHECK_CONDITION_TRUE(n_id == 0, "Cannot delete the output node");

  bool result{delete_node(n_id, in_port_count, out_port_count)};
  if (!result) {
    ERROR_PRINT("Failed to delete node");
  }

  Q_EMIT visual_shader_model->dataChanged(QModelIndex(), QModelIndex());  // To update the serialized data
}

void VisualShaderGraphicsScene::on_port_pressed(QGraphicsObject* port, const QPointF& coordinate) {
  this->temporary_connection_graphics_object = nullptr;  // Reset the temporary connection object
}

void VisualShaderGraphicsScene::on_port_dragged(QGraphicsObject* port, const QPointF& coordinate) {
  VisualShaderConnectionGraphicsObject* c_o{nullptr};

  VisualShaderOutputPortGraphicsObject* o_port{dynamic_cast<VisualShaderOutputPortGraphicsObject*>(port)};

  if (!o_port) {
    VisualShaderInputPortGraphicsObject* i_port{dynamic_cast<VisualShaderInputPortGraphicsObject*>(port)};
    SILENT_CHECK_PARAM_NULLPTR(i_port);

    if (i_port->is_connected() && !temporary_connection_graphics_object) {
      const int c_id = i_port->get_c_id();
      c_o = get_connection_graphics_object(c_id);
      temporary_connection_graphics_object = c_o;  // Store the connection object for access in the next drag call
      CHECK_CONDITION_TRUE(!delete_connection_from_model(c_id), "Failed to delete connection from model");
      CHECK_CONDITION_TRUE(!convert_to_temporary_connection(c_id, i_port->get_node_id(), i_port->get_port_index()), "Failed to convert to temporary connection");
    } else if (!i_port->is_connected() && temporary_connection_graphics_object) {
      c_o = temporary_connection_graphics_object;
      on_scene_update_requested();
    } else {
      return;
    }

    c_o->set_end_coordinate(coordinate);
    c_o->update_layout();

    return;
  }

  if (o_port->is_connected() && temporary_connection_graphics_object) {
    c_o = temporary_connection_graphics_object;
  } else if (!temporary_connection_graphics_object) {
    bool result{this->add_temporary_connection(o_port->get_node_id(), o_port->get_port_index())};
    CHECK_CONDITION_TRUE(!result, "Failed to add connection");
    c_o = temporary_connection_graphics_object;
  } else {
    return;
  }

  c_o->set_end_coordinate(coordinate);
  c_o->update_layout();
}

void VisualShaderGraphicsScene::on_port_dropped(QGraphicsObject* port, const QPointF& coordinate) {
  VisualShaderConnectionGraphicsObject* c_o{temporary_connection_graphics_object};
  SILENT_CHECK_PARAM_NULLPTR(c_o);

  int c_id{c_o->get_id()};

  // Find all items under the coordinate
  QList<QGraphicsItem*> items_at_coordinate{this->items(coordinate)};

  // Iterate through the items and check if an input port is under the mouse
  VisualShaderInputPortGraphicsObject* in_p_o{nullptr};
  for (QGraphicsItem* item : items_at_coordinate) {
    // Check if the item is an input port
    in_p_o = dynamic_cast<VisualShaderInputPortGraphicsObject*>(item);
    SILENT_BREAK_IF_TRUE(in_p_o != nullptr);
  }

  VisualShaderOutputPortGraphicsObject* o_port{dynamic_cast<VisualShaderOutputPortGraphicsObject*>(port)};

  if (!o_port) {
    VisualShaderInputPortGraphicsObject* i_port{dynamic_cast<VisualShaderInputPortGraphicsObject*>(port)};
    SILENT_CHECK_PARAM_NULLPTR(i_port);

    if (!in_p_o) {
      bool result{this->delete_temporary_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};
      if (!result) {
        ERROR_PRINT("Failed to delete connection");
      }

      return;  // Return because we dragging an input port and dropped on nothing
    }

    if (in_p_o->is_connected()) {
      bool result{this->delete_temporary_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};
      if (!result) {
        ERROR_PRINT("Failed to delete connection");
      }

      return;  // Return because we dragging an input port and dropped on a connected input port
    }

    bool result{add_connection(c_id, c_o->get_from_node_id(), c_o->get_from_port_index(), in_p_o->get_node_id(),
                               in_p_o->get_port_index())};

    if (!result) {
      if (!this->delete_temporary_connection(c_o->get_from_node_id(), c_o->get_from_port_index())) {
        ERROR_PRINT("Failed to delete connection");
      }

      return;  // Return because we dragging an input port and dropped on nothing
    }

    return;
  }

  if (!in_p_o) {
    bool result{this->delete_temporary_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

    if (!result) {
      ERROR_PRINT("Failed to delete connection");
    }

    return;  // Return because we dragging an output port and dropped on nothing
  }

  if (in_p_o->is_connected()) {
    bool result{this->delete_temporary_connection(c_o->get_from_node_id(), c_o->get_from_port_index())};

    if (!result) {
      ERROR_PRINT("Failed to delete connection");
    }

    return;  // Return because we dragging an output port and dropped on a connected input port
  }

  bool result{
      add_connection(c_id, o_port->get_node_id(), o_port->get_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index())};

  if (!result) {
    if (!this->delete_temporary_connection(c_o->get_from_node_id(), c_o->get_from_port_index())) {
      ERROR_PRINT("Failed to delete connection");
    }

    return;
  }
}

void VisualShaderGraphicsScene::on_node_moved(const int& n_id, const int& in_port_count, const int& out_port_count,
                                              const QPointF& new_coordinate) {
  int row_entry{find_node_entry(visual_shader_model, nodes_model, n_id)};
  VALIDATE_INDEX(row_entry, nodes_model->rowCount(), "Node entry not found");

  bool result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderNode::kXCoordinateFieldNumber)),
      new_coordinate.x());
  CHECK_CONDITION_TRUE(!result, "Failed to set node x coordinate");

  result = visual_shader_model->set_data(
      FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber),
                                  FieldPath::RepeatedAt(row_entry),
                                  FieldPath::FieldNumber(VisualShader::VisualShaderNode::kYCoordinateFieldNumber)),
      new_coordinate.y());
  CHECK_CONDITION_TRUE(!result, "Failed to set node y coordinate");

  // Update coordinates of all connected connections
  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};

  for (int i{0}; i < in_port_count; i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!i_port || !i_port->is_connected());

    const int c_id{i_port->get_c_id()};
    VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
    SILENT_CONTINUE_IF_TRUE(!c_o);

    c_o->set_end_coordinate(i_port->get_global_coordinate());
    c_o->update_layout();
  }

  for (int i{0}; i < out_port_count; i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!o_port || !o_port->is_connected());

    std::unordered_set<int> c_ids{o_port->get_c_ids()};

    for (const int& c_id : c_ids) {
      VisualShaderConnectionGraphicsObject* c_o{get_connection_graphics_object(c_id)};
      SILENT_CONTINUE_IF_TRUE(!c_o);

      c_o->set_start_coordinate(o_port->get_global_coordinate());
      c_o->update_layout();
    }
  }
}

void VisualShaderGraphicsScene::remove_item(QGraphicsItem* item) {
  removeItem(item);
  delete item;
}

bool VisualShaderGraphicsScene::check_if_connection_out_of_bounds(VisualShaderOutputPortGraphicsObject* from_o_port, VisualShaderInputPortGraphicsObject* to_i_port) {
  QList<QGraphicsView*> views{this->views()};
  CHECK_CONDITION_TRUE_NON_VOID(views.isEmpty(), false, "No views available");

  VisualShaderGraphicsView* view{dynamic_cast<VisualShaderGraphicsView*>(views.first())};

  if (from_o_port->get_global_coordinate().x() < view->get_x() ||
      from_o_port->get_global_coordinate().x() > view->get_x() + view->get_width()) {
    WARN_PRINT("Start of connection is out of view bounds");
  }

  if (to_i_port->get_global_coordinate().y() < view->get_y() ||
        to_i_port->get_global_coordinate().y() > view->get_y() + view->get_height()) {
    WARN_PRINT("End of connection is out of view bounds");
  }

  return true;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsView                     *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

//////////////////////////////
// Public functions
//////////////////////////////

VisualShaderGraphicsView::VisualShaderGraphicsView(VisualShaderGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent),
      scene(scene),
      context_menu(nullptr),
      create_node_action(nullptr),
      zoom_in_action(nullptr),
      reset_zoom_action(nullptr),
      zoom_out_action(nullptr) {
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);

  setBackgroundBrush(this->background_color);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  setCacheMode(QGraphicsView::CacheBackground);
  setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

  // Allow dezooming 8 times from the default zoom level.
  zoom_min = (1.0f / std::pow(zoom_step, 8.0f));
  // Allow zooming 4 times from the default zoom level.
  zoom_max = (1.0f * std::pow(zoom_step, 4.0f));

  setSceneRect(rect_x, rect_y, rect_width, rect_height);

  reset_zoom();

  // Set the context menu
  context_menu = new QMenu(this);
  create_node_action = new QAction(QStringLiteral("Create Node"), context_menu);
  QObject::connect(create_node_action, &QAction::triggered, this,
                   &VisualShaderGraphicsView::on_create_node_action_triggered);
  context_menu->addAction(create_node_action);

  zoom_in_action = new QAction(QStringLiteral("Zoom In"), context_menu);
  zoom_in_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
  zoom_in_action->setShortcut(QKeySequence(QKeySequence::ZoomIn));
  QObject::connect(zoom_in_action, &QAction::triggered, this, &VisualShaderGraphicsView::zoom_in);
  context_menu->addAction(zoom_in_action);

  reset_zoom_action = new QAction(QStringLiteral("Reset Zoom"), context_menu);
  QObject::connect(reset_zoom_action, &QAction::triggered, this, &VisualShaderGraphicsView::reset_zoom);
  context_menu->addAction(reset_zoom_action);

  zoom_out_action = new QAction(QStringLiteral("Zoom Out"), context_menu);
  zoom_out_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
  zoom_out_action->setShortcut(QKeySequence(QKeySequence::ZoomOut));
  QObject::connect(zoom_out_action, &QAction::triggered, this, &VisualShaderGraphicsView::zoom_out);
  context_menu->addAction(zoom_out_action);
}

VisualShaderGraphicsView::~VisualShaderGraphicsView() {}

//////////////////////////////
// Private slots
//////////////////////////////

void VisualShaderGraphicsView::on_create_node_action_triggered() {
  VisualShaderEditor* editor{scene->get_editor()};

  Q_EMIT editor->create_node_dialog_requested(this->last_context_menu_coordinate);
}

void VisualShaderGraphicsView::zoom_in() {
  const float factor{std::pow(zoom_step, zoom)};

  QTransform t{transform()};
  t.scale(factor, factor);
  if (t.m11() >= zoom_max) {
    return;
  }

  scale(factor, factor);
  Q_EMIT zoom_changed(transform().m11());
}

void VisualShaderGraphicsView::reset_zoom() {
  if ((float)transform().m11() == zoom) {
    return;
  }

  resetTransform();  // Reset the zoom level to 1.0f
  Q_EMIT zoom_changed(transform().m11());
}

void VisualShaderGraphicsView::zoom_out() {
  const float factor{std::pow(zoom_step, -1.0f * zoom)};

  QTransform t{transform()};
  t.scale(factor, factor);
  if (t.m11() <= zoom_min) {
    return;
  }

  scale(factor, factor);
  Q_EMIT zoom_changed(transform().m11());
}

//////////////////////////////
// Private functions
//////////////////////////////

void VisualShaderGraphicsView::drawBackground(QPainter* painter, const QRectF& r) {
  QGraphicsView::drawBackground(painter, r);

  std::function<void(float)> draw_grid = [&](float grid_step) {
    QRect window_rect{this->rect()};

    QPointF tl{mapToScene(window_rect.topLeft())};
    QPointF br{mapToScene(window_rect.bottomRight())};

    float left{std::floor((float)tl.x() / grid_step)};
    float right{std::ceil((float)br.x() / grid_step)};
    float bottom{std::floor((float)tl.y() / grid_step)};
    float top{std::ceil((float)br.y() / grid_step)};

    // Vertical lines
    for (int xi{(int)left}; xi <= (int)right; ++xi) {
      QLineF line(xi * grid_step, bottom * grid_step, xi * grid_step, top * grid_step);
      painter->drawLine(line);
    }

    // Horizontal lines
    for (int yi{(int)bottom}; yi <= (int)top; ++yi) {
      QLineF line(left * grid_step, yi * grid_step, right * grid_step, yi * grid_step);
      painter->drawLine(line);
    }
  };

  QPen fine_pen(this->fine_grid_color, 1.0f);
  painter->setPen(fine_pen);
  draw_grid(15.0f);

  QPen coarse_pen(this->coarse_grid_color, 1.0f);
  painter->setPen(coarse_pen);
  draw_grid(150.0f);
}

void VisualShaderGraphicsView::contextMenuEvent(QContextMenuEvent* event) {
  QGraphicsItem* item{itemAt(event->pos())};

  // If there is an item and this item is a node object, pass the event to the
  // children
  if (item && dynamic_cast<VisualShaderNodeGraphicsObject*>(item)) {
    QGraphicsView::contextMenuEvent(event);
    return;
  } else if (item) {
    return;
  }

  this->last_context_menu_coordinate = mapToScene(event->pos());

  context_menu->exec(event->globalPos());
}

void VisualShaderGraphicsView::wheelEvent(QWheelEvent* event) {
  float t_zoom{(float)transform().m11()};

  const QPoint delta{event->angleDelta()};

  if (delta.y() == 0) {
    event->ignore();
    return;
  }

  if (delta.y() > 0 && (std::abs(t_zoom - zoom_max) > std::numeric_limits<double>::epsilon())) {
    zoom_in();
  } else if (delta.y() < 0 && (std::abs(t_zoom - zoom_min) > std::numeric_limits<double>::epsilon())) {
    zoom_out();
  } else {
    event->ignore();
  }
}

void VisualShaderGraphicsView::mousePressEvent(QMouseEvent* event) {
  QGraphicsView::mousePressEvent(event);

  switch (event->button()) {
    case Qt::LeftButton:
      last_click_coordinate = mapToScene(event->pos());
      break;
    default:
      break;
  }
}

void VisualShaderGraphicsView::mouseMoveEvent(QMouseEvent* event) {
  QGraphicsView::mouseMoveEvent(event);

  switch (event->buttons()) {
    case Qt::LeftButton: {
      QPointF current_coordinate{mapToScene(event->pos())};
      QPointF difference{last_click_coordinate - current_coordinate};
      setSceneRect(sceneRect().translated(difference.x(), difference.y()));
      last_click_coordinate = current_coordinate;
    } break;
    default:
      break;
  }
}

void VisualShaderGraphicsView::mouseReleaseEvent(QMouseEvent* event) { QGraphicsView::mouseReleaseEvent(event); }

void VisualShaderGraphicsView::showEvent(QShowEvent* event) {
  QGraphicsView::showEvent(event);

  move_view_to_fit_items();
}

void VisualShaderGraphicsView::move_view_to_fit_items() {
  if (!scene) {
    return;
  }

  if (scene->items().isEmpty()) {
    return;
  }

  DEBUG_PRINT("Changing view port to fit items...");

  QRectF items_bounding_rect{scene->itemsBoundingRect()};
  items_bounding_rect.adjust(-fit_in_view_margin, -fit_in_view_margin, fit_in_view_margin, fit_in_view_margin);

  QPointF scene_tl{scene->sceneRect().topLeft()};
  QPointF scene_br{scene->sceneRect().bottomRight()};
  QPointF items_tl{items_bounding_rect.topLeft()};
  QPointF items_br{items_bounding_rect.bottomRight()};

  // Make sure the items bounding rect is inside the scene rect
  if (items_tl.x() < scene_tl.x()) {
    items_bounding_rect.setLeft(scene_tl.x());
  }

  if (items_tl.y() > scene_tl.y()) {
    items_bounding_rect.setTop(scene_tl.y());
  }

  if (items_br.x() > scene_br.x()) {
    items_bounding_rect.setRight(scene_br.x());
  }

  if (items_br.y() < scene_br.y()) {
    items_bounding_rect.setBottom(scene_br.y());
  }

  fitInView(items_bounding_rect, Qt::KeepAspectRatio);

  centerOn(items_bounding_rect.center());

  if ((float)transform().m11() > zoom_max) {
    WARN_PRINT("Current zoom level is greater than the maximum zoom level.");
    WARN_PRINT("Maybe due to having a very large distance between the nodes.");
  }

  if ((float)transform().m11() < zoom_min) {
    WARN_PRINT("Current zoom level is less than the minimum zoom level.");
    WARN_PRINT("Maybe due to having all the nodes outside the scene bounds.");
  }
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderNodeGraphicsObject               *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderNodeGraphicsObject::VisualShaderNodeGraphicsObject(const int& n_id, const QPointF& coordinate,
                                                               const std::shared_ptr<IVisualShaderProtoNode>& proto_node,
                                                               QGraphicsItem* parent)
    : QGraphicsObject(parent),
      n_id(n_id),
      coordinate(coordinate),
      in_port_count(0),
      out_port_count(0),
      context_menu(nullptr),
      delete_node_action(nullptr),
      rect_width(0.0f),
      rect_height(0.0f),
      rect_padding(0.0f),
      rect_margin(0.0f),
      caption_rect_height(0.0f),
      embed_widget(nullptr),
      matching_image_widget(nullptr), 
      renderer_widget(nullptr) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setOpacity(this->opacity);

  setZValue(0);

  setPos(coordinate.x(), coordinate.y());

  // Set the caption
  this->caption = proto_node->get_caption();

  // Set the input and output port captions
  this->in_port_count = proto_node->get_input_port_count();
  this->out_port_count = proto_node->get_output_port_count();

  in_port_captions.resize(this->in_port_count);
  out_port_captions.resize(this->out_port_count);
  for (int i{0}; i < (int)in_port_captions.size(); i++) in_port_captions.at(i) = proto_node->get_input_port_caption(i);
  for (int i{0}; i < (int)out_port_captions.size(); i++)
    out_port_captions.at(i) = proto_node->get_output_port_caption(i);

  // Set input and output port types
  in_port_types.resize(this->in_port_count);
  out_port_types.resize(this->out_port_count);
  for (int i{0}; i < (int)in_port_types.size(); i++) in_port_types.at(i) = proto_node->get_input_port_type(i);
  for (int i{0}; i < (int)out_port_types.size(); i++) out_port_types.at(i) = proto_node->get_output_port_type(i);

  // Output node should have a matching image widget
  if (n_id == 0) {
    QGraphicsProxyWidget* matching_image_widget_proxy{new QGraphicsProxyWidget(this)};
    matching_image_widget = new OriginalMatchingImageWidget();
    matching_image_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    matching_image_widget->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

    matching_image_widget_proxy->setWidget(matching_image_widget);
  } else {
    // Create the shader previewer widget
    QGraphicsProxyWidget* renderer_widget_proxy{new QGraphicsProxyWidget(this)};
    renderer_widget = new RendererWidget();
    renderer_widget->setVisible(false);
    renderer_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    renderer_widget->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
    
    renderer_widget_proxy->setWidget(renderer_widget);
  }

  // Set the context menu
  context_menu = new QMenu();
  delete_node_action = new QAction(QStringLiteral("Delete Node"), context_menu);
  delete_node_action->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
  delete_node_action->setShortcut(QKeySequence(QKeySequence::Delete));
  QObject::connect(delete_node_action, &QAction::triggered, this,
                   &VisualShaderNodeGraphicsObject::on_delete_node_action_triggered);
  context_menu->addAction(delete_node_action);
}

VisualShaderNodeGraphicsObject::~VisualShaderNodeGraphicsObject() {
  if (context_menu) delete context_menu;
}

void VisualShaderNodeGraphicsObject::on_input_node_out_port_type_changed(const int& new_index) {
  VALIDATE_INDEX(0, out_port_count, "Invalid input port index");
  out_port_types.at(0) = shadergen_utils::get_enum_value_port_type_by_value(VisualShaderNodeInputType_descriptor(), new_index);
  out_port_graphics_objects.at(0)->set_port_type(out_port_types.at(0));
}

void VisualShaderNodeGraphicsObject::on_delete_node_action_triggered() {
  Q_EMIT node_deleted(n_id, in_port_count, out_port_count);
}

VisualShaderInputPortGraphicsObject* VisualShaderNodeGraphicsObject::get_input_port_graphics_object(
    const int& p_index) const {
  if (in_port_graphics_objects.find(p_index) != in_port_graphics_objects.end()) {
    return in_port_graphics_objects.at(p_index);
  }

  return nullptr;
}

VisualShaderOutputPortGraphicsObject* VisualShaderNodeGraphicsObject::get_output_port_graphics_object(
    const int& p_index) const {
  if (out_port_graphics_objects.find(p_index) != out_port_graphics_objects.end()) {
    return out_port_graphics_objects.at(p_index);
  }

  return nullptr;
}

void VisualShaderNodeGraphicsObject::update_layout() {
  QFont f("Arial", caption_font_size);
  f.setBold(true);
  QFontMetrics fm(f);

  QString t_caption{QString::fromStdString(caption)};

  // Set the width of the node
  rect_width = (float)(fm.horizontalAdvance(t_caption, t_caption.length()) + caption_h_padding * 2.0f);

  caption_rect_height = (float)((fm.height()) + caption_v_padding * 2.0f);

  int max_num_ports{qMax(in_port_count, out_port_count)};

  // Calculate the height of the node
  float t_rect_h{caption_rect_height};

  t_rect_h += body_rect_header_height;  // Header
  if (max_num_ports >= 0) {
    t_rect_h += (float)(max_num_ports - 1) * body_rect_port_step;  // Ports
  }
  t_rect_h += body_rect_footer_height;  // Footer

  rect_height = t_rect_h; // Set the height of the node

  // Correct node rect if it has an embed widget (if needed).
  if (embed_widget) {
    float embed_widget_width{(float)embed_widget->width()};

    // Find biggest horizontal length of input port names
    float max_in_p_width{0.0f};
    for (int i{0}; i < in_port_count; ++i) {
      QString p_n{QString::fromStdString(in_port_captions.at(i))};

      QFont f("Arial", port_caption_font_size);
      QFontMetrics fm(f);

      float w{0.0f};

      if (!p_n.isEmpty()) {
        w = (float)fm.horizontalAdvance(p_n);
      } else {
        // If the port name is empty, use a default name
        // This is because the horizontal advance of an empty string is 0 and
        // this will wrong the calculation of the rect width
        w = (float)fm.horizontalAdvance("in");
      }

      if ((w + port_caption_spacing) > max_in_p_width) {
        max_in_p_width = w + port_caption_spacing;
      }
    }

    // Find biggest horizontal length of output port names
    float max_out_p_width{0.0f};
    for (int i{0}; i < out_port_count; ++i) {
      QString p_n{QString::fromStdString(out_port_captions.at(i))};

      QFont f("Arial", port_caption_font_size);
      QFontMetrics fm(f);

      float w{0.0f};

      if (!p_n.isEmpty()) {
        w = (float)fm.horizontalAdvance(p_n);
      } else {
        // If the port name is empty, use a default name
        // This is because the horizontal advance of an empty string is 0 and
        // this will wrong the calculation of the rect width
        w = (float)fm.horizontalAdvance("out");
      }

      if ((w + port_caption_spacing) > max_out_p_width) {
        max_out_p_width = w + port_caption_spacing;
      }
    }

    float calculated_rect{max_in_p_width + embed_widget_width + max_out_p_width + embed_widget_h_margin * 2.0f};

    if (calculated_rect > rect_width) {
      rect_width = calculated_rect;
    }

    // Check the height
    float calculated_height{caption_rect_height + body_rect_header_height + embed_widget->height() +
                            body_rect_footer_height + embed_widget_v_margin * 2.0f};

    if (calculated_height > rect_height) {
      rect_height = calculated_height;
    }
  } // End of embed widget correction

  // Create a rect to calculate the rest of the layout
  QRectF r({0.0f, 0.0f}, QSizeF(rect_width, rect_height));

  // Calculate the margin
  // We add a safe area around the rect to prevent the ports from being cut off
  this->rect_margin = port_diameter * 0.5f;

  // Calculate the rect padding
  this->rect_padding = port_diameter * 0.4f;

  // Add the padding to draw the border
  r.adjust( - rect_padding, - rect_padding, rect_padding, rect_padding);

  if (n_id == 0) {
    this->matching_image_widget_coordinate = {
      (float)r.x() + (float)r.width() + spacing_between_output_node_and_matching_image,
      (float)r.y()
    };
    matching_image_widget->setGeometry(this->matching_image_widget_coordinate.x(), this->matching_image_widget_coordinate.y(), r.height(), r.height());
  } else {
    this->renderer_widget_coordinate = {
      (float)r.x(),
      (float)r.y() + (float)r.height() + spacing_between_current_node_and_shader_previewer
    };
    renderer_widget->setGeometry(this->renderer_widget_coordinate.x(), this->renderer_widget_coordinate.y(), r.width(), r.width());
  }

  // Remove the padding from the rect
  r.adjust(rect_padding, rect_padding, - rect_padding, - rect_padding);

  float rect_x{(float)r.topLeft().x()};
  float rect_y{(float)r.topLeft().y()};

  float rect_w{(float)r.width()};
  float rect_h{(float)r.height()};

  QRectF caption_rect(rect_x, rect_y, rect_w, caption_rect_height);

  {
    // Calculate the caption coordinate
    this->caption_coordinate = {
      (float)(caption_rect.center().x() - (float)fm.horizontalAdvance(t_caption) * 0.5f),
      // Instead of subtracting, add the ascent to properly align text within the rect
      (float)(caption_rect.center().y() + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent()))
    };
  }

  QPointF caption_rect_bl{caption_rect.bottomLeft()};
  QPointF first_in_port_coordinate{caption_rect_bl.x(), caption_rect_bl.y() + body_rect_header_height};

  // Correct X coordinate: Remove the padding
  first_in_port_coordinate.setX((float)first_in_port_coordinate.x() - this->rect_padding);

  {
    this->input_ports_rects.clear();
    this->input_ports_caption_coordinates.clear();

    this->input_ports_rects.resize(in_port_count);
    this->input_ports_caption_coordinates.resize(in_port_count);

    // Calculate the input ports coordinates
    for (int i{0}; i < in_port_count; ++i) {
      QPointF port_coordinate{first_in_port_coordinate.x(), first_in_port_coordinate.y() + body_rect_port_step * i};

      QRectF port_rect(port_coordinate.x(), port_coordinate.y(), port_diameter, port_diameter);

      // Adjust the port rect to be centered
      port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f,
                       -port_rect.height() * 0.5f);

      this->input_ports_rects.at(i) = port_rect;

      // Draw caption
      QString p_n{QString::fromStdString(in_port_captions.at(i))};

      if (!p_n.isEmpty()) {
        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);

        this->input_ports_caption_coordinates.at(i) = QPointF(
          rect_x + port_caption_spacing,
          (float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())
        );
      }

      if (in_port_graphics_objects.find(i) != in_port_graphics_objects.end()) continue;

      // Draw the port
      VisualShaderInputPortGraphicsObject* p_o{new VisualShaderInputPortGraphicsObject(port_rect, n_id, i, in_port_types.at(i), this)};
      in_port_graphics_objects[i] = p_o;

      // Connect the signals
      QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_pressed, this,
                       &VisualShaderNodeGraphicsObject::on_in_port_pressed);
      QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_dragged, this,
                       &VisualShaderNodeGraphicsObject::on_in_port_dragged);
      QObject::connect(p_o, &VisualShaderInputPortGraphicsObject::port_dropped, this,
                       &VisualShaderNodeGraphicsObject::on_in_port_dropped);
    }
  }

  QPointF caption_rect_br{caption_rect.bottomRight()};
  QPointF first_out_port_coordinate{caption_rect_br.x(), caption_rect_br.y() + body_rect_header_height};

  // Correct X coordinate: Remove the padding
  first_out_port_coordinate.setX((float)first_out_port_coordinate.x() + this->rect_padding);

  {
    this->output_ports_rects.clear();
    this->output_ports_caption_coordinates.clear();

    this->output_ports_rects.resize(out_port_count);
    this->output_ports_caption_coordinates.resize(out_port_count);

    // Draw Output Ports
    for (int i{0}; i < out_port_count; ++i) {
      QPointF port_coordinate{first_out_port_coordinate.x(), first_out_port_coordinate.y() + body_rect_port_step * i};

      QRectF port_rect(port_coordinate.x(), port_coordinate.y(), port_diameter, port_diameter);

      // Adjust the port rect to be centered
      port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f,
                       -port_rect.height() * 0.5f);

      this->output_ports_rects.at(i) = port_rect;

      // Draw caption
      QString p_n{QString::fromStdString(out_port_captions.at(i))};

      if (!p_n.isEmpty()) {
        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);

        this->output_ports_caption_coordinates.at(i) = QPointF(
          rect_x + rect_w - (float)fm.horizontalAdvance(p_n) - port_caption_spacing,
          (float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())
        );
      }

      if (out_port_graphics_objects.find(i) != out_port_graphics_objects.end()) continue;

      // Draw the port
      VisualShaderOutputPortGraphicsObject* p_o{new VisualShaderOutputPortGraphicsObject(port_rect, n_id, i, out_port_types.at(i), this)};
      out_port_graphics_objects[i] = p_o;

      // Connect the signals
      QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_pressed, this,
                       &VisualShaderNodeGraphicsObject::on_out_port_pressed);
      QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_dragged, this,
                       &VisualShaderNodeGraphicsObject::on_out_port_dragged);
      QObject::connect(p_o, &VisualShaderOutputPortGraphicsObject::port_dropped, this,
                       &VisualShaderNodeGraphicsObject::on_out_port_dropped);
    }
  }

  {
    // Correct the coordinate of the embed widget
    if (embed_widget) {
      this->embed_widget_coordinate = {
        rect_x + rect_w * 0.5f - embed_widget->width() * 0.5f,
        rect_y + caption_rect_height + body_rect_header_height + embed_widget_v_margin
      };

      embed_widget->setGeometry(this->embed_widget_coordinate.x(), this->embed_widget_coordinate.y(), embed_widget->width(), embed_widget->height());
    }
  }

  prepareGeometryChange();
}

void VisualShaderNodeGraphicsObject::on_preview_shader_button_pressed() {
  bool is_visible{renderer_widget->isVisible()};
  renderer_widget->setVisible(!is_visible);

  // The preview shader button is the last widget in the embed widget
  int count = embed_widget->layout()->count();
  CHECK_CONDITION_TRUE(count == 0, "No widgets in the embed widget");

  QLayout* layout = embed_widget->layout();
  CHECK_PARAM_NULLPTR(layout, "Layout is nullptr");

  QLayoutItem* item = layout->itemAt(count - 1);
  CHECK_PARAM_NULLPTR(item, "Item is nullptr");

  QWidget* last_widget = item->widget();
  CHECK_PARAM_NULLPTR(last_widget, "Last widget is nullptr");

  QPushButton* preview_shader_button{dynamic_cast<QPushButton*>(last_widget)};
  CHECK_PARAM_NULLPTR(preview_shader_button, "Preview shader button is nullptr");

  preview_shader_button->setText(!is_visible ? "Hide Preview" : "Show Preview");
}

QRectF VisualShaderNodeGraphicsObject::boundingRect() const {
  QRectF r({0.0f, 0.0f}, QSizeF(rect_width, rect_height));

  r.adjust(- rect_margin - rect_padding, 
           - rect_margin - rect_padding, 
           rect_margin + rect_padding,
           rect_margin + rect_padding);

  return r;
}

void VisualShaderNodeGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  // Get the rect without the padding
  QRectF r({0.0f, 0.0f}, QSizeF(rect_width, rect_height));

  // Add the padding to draw the border
  r.adjust( - rect_padding, - rect_padding, rect_padding, rect_padding);

  {
    // Draw Node Rect
    QColor rect_color;
    if (isSelected()) {
      rect_color = this->normal_boundary_color;
    } else {
      rect_color = this->selected_boundary_color;
    }

    QPen p(rect_color, this->pen_width);
    painter->setPen(p);

    painter->setBrush(this->fill_color);

    painter->drawRoundedRect(r, this->corner_radius, this->corner_radius);
  }

  // Remove the padding to draw the inner content
  r.adjust(rect_padding, rect_padding, - rect_padding, - rect_padding);

  {
    // Draw Caption
    QString t_caption{QString::fromStdString(caption)};

    QFont t_f{painter->font()};

    QFont f("Arial", caption_font_size);
    f.setBold(true);
    painter->setFont(f);

    painter->setPen(this->font_color);
    painter->drawText(this->caption_coordinate, t_caption);

    painter->setFont(t_f);  // Reset the font
  }

  {
    // Draw Input Ports
    for (int i{0}; i < in_port_count; ++i) {
      // Draw caption
      QString p_n{QString::fromStdString(in_port_captions.at(i))};

      if (!p_n.isEmpty()) {
        QFont t_f{painter->font()};

        QFont f("Arial", port_caption_font_size);
        painter->setFont(f);

        painter->setPen(this->font_color);
        painter->drawText(this->input_ports_caption_coordinates.at(i), p_n);

        painter->setFont(t_f);  // Reset the font
      }
    }
  }

  {
    // Draw Output Ports
    for (int i{0}; i < out_port_count; ++i) {
      // Draw caption
      QString p_n{QString::fromStdString(out_port_captions.at(i))};

      if (!p_n.isEmpty()) {
        QFont t_f{painter->font()};

        QFont f("Arial", port_caption_font_size);
        painter->setFont(f);

        painter->setPen(this->font_color);
        painter->drawText(this->output_ports_caption_coordinates.at(i), p_n);

        painter->setFont(t_f);  // Reset the font
      }
    }
  }
}

QVariant VisualShaderNodeGraphicsObject::itemChange(GraphicsItemChange change, const QVariant& value) {
  if (change == ItemScenePositionHasChanged) {
    Q_EMIT node_moved(n_id, in_port_count, out_port_count, scenePos());
  }
  return QGraphicsObject::itemChange(change, value);
}

void VisualShaderNodeGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
  context_menu->exec(event->screenPos());
}

void VisualShaderNodeGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsObject::mouseReleaseEvent(event);
}

VisualShaderInputPortGraphicsObject::VisualShaderInputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                                                         const VisualShaderNodePortType& port_type,
                                                                         QGraphicsItem* parent)
    : QGraphicsObject(parent), rect(rect), n_id(n_id), p_index(p_index), port_type(port_type), c_id(-1) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setCursor(Qt::PointingHandCursor);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setOpacity(this->opacity);

  setZValue(0);
}

QRectF VisualShaderInputPortGraphicsObject::boundingRect() const {
  QRectF t_rect{rect};
  t_rect.adjust(-padding, -padding, padding, padding);
  return t_rect;
}

void VisualShaderInputPortGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                                QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  painter->setBrush(this->connection_point_color);

  painter->drawEllipse(rect);
}

void VisualShaderInputPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  Q_EMIT port_pressed(this, event->scenePos());
  QGraphicsObject::mousePressEvent(event);
}

void VisualShaderInputPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  Q_EMIT port_dragged(this, event->scenePos());
  QGraphicsObject::mouseMoveEvent(event);
}

void VisualShaderInputPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  Q_EMIT port_dropped(this, event->scenePos());
  QGraphicsObject::mouseReleaseEvent(event);
}

VisualShaderOutputPortGraphicsObject::VisualShaderOutputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                                                           const VisualShaderNodePortType& port_type,
                                                                           QGraphicsItem* parent)
    : QGraphicsObject(parent), rect(rect), n_id(n_id), p_index(p_index), port_type(port_type) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setCursor(Qt::PointingHandCursor);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setOpacity(this->opacity);

  setZValue(0);
}

QRectF VisualShaderOutputPortGraphicsObject::boundingRect() const {
  QRectF t_rect{rect};
  t_rect.adjust(-padding, -padding, padding, padding);
  return t_rect;
}

void VisualShaderOutputPortGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                                 QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  painter->setBrush(this->connection_point_color);

  painter->drawEllipse(rect);
}

void VisualShaderOutputPortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  Q_EMIT port_pressed(this, event->scenePos());
  QGraphicsObject::mousePressEvent(event);
}

void VisualShaderOutputPortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  Q_EMIT port_dragged(this, event->scenePos());
  QGraphicsObject::mouseMoveEvent(event);
}

void VisualShaderOutputPortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  Q_EMIT port_dropped(this, event->scenePos());
  QGraphicsObject::mouseReleaseEvent(event);
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****           VisualShaderConnectionGraphicsObject             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderConnectionGraphicsObject::VisualShaderConnectionGraphicsObject(const int& c_id, const int& from_n_id,
                                                                           const int& from_p_index,
                                                                           const QPointF& start_coordinate,
                                                                           QGraphicsItem* parent)
    : QGraphicsObject(parent),
      c_id(c_id),
      from_n_id(from_n_id),
      from_p_index(from_p_index),
      to_n_id(-1),
      to_p_index(-1),
      start_coordinate(start_coordinate),
      end_coordinate(start_coordinate),
      rect_margin(0.0f) {
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setAcceptedMouseButtons(Qt::NoButton);

  setZValue(-1.0f);
}

void VisualShaderConnectionGraphicsObject::update_layout() {
  this->rect = calculate_bounding_rect_from_coordinates();

  // Calculate the rect padding
  // We add a safe area around the rect to prevent the ports from being cut off
  // Due to inaccuracy in the calculation of the bounding rect we use the point diameter not the radius
  this->rect_margin = this->point_diameter;

  this->control_points = calculate_control_points();
  this->cubic_path.clear(); // Clear the path
  this->cubic_path.moveTo(start_coordinate);
  this->cubic_path.cubicTo(control_points.first, control_points.second, end_coordinate);

  this->start_rect = QRectF(start_coordinate.x(), start_coordinate.y(), this->point_diameter, this->point_diameter);

  // Adjust the port rect to be centered
  start_rect.adjust(-start_rect.width() * 0.5f, -start_rect.height() * 0.5f, -start_rect.width() * 0.5f,
                    -start_rect.height() * 0.5f);

  this->end_rect = QRectF(end_coordinate.x(), end_coordinate.y(), this->point_diameter, this->point_diameter);

  // Adjust the port rect to be centered
  end_rect.adjust(-end_rect.width() * 0.5f, -end_rect.height() * 0.5f, -end_rect.width() * 0.5f,
                  -end_rect.height() * 0.5f);

  prepareGeometryChange();
}

QRectF VisualShaderConnectionGraphicsObject::boundingRect() const {
  QRectF r{this->rect};

  r.adjust(-rect_margin, -rect_margin, rect_margin, rect_margin);

  return r;
}

void VisualShaderConnectionGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                                 QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  {
    // Draw the connection
    QPen p;
    p.setWidth(this->line_width);

    const bool selected{this->isSelected()};

    p.setColor(this->normal_color);

    if (selected) {
      p.setColor(this->selected_color);
    }

    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    painter->drawPath(cubic_path);
  }

  painter->setBrush(this->connection_point_color);

  {
    painter->drawEllipse(start_rect);
  }

  {
    painter->drawEllipse(end_rect);
  }
}

int VisualShaderConnectionGraphicsObject::detect_quadrant(const QPointF& reference, const QPointF& target) const {
  float relative_x{(float)(target.x() - reference.x())};
  float relative_y{(float)(target.y() - reference.y())};

  // Note that the default coordinate system in Qt is as follows:
  // - X-axis: Positive to the right, negative to the left
  // - Y-axis: Positive downwards, negative upwards

  // Check if the point is on an axis or the origin
  if (relative_x == 0 && relative_y == 0) {
    return 0;  // Stack on the reference
  } else if (relative_y == 0) {
    return (relative_x > 0) ? 5 : 6;  // On X-axis: 5 is the +ve part while 6 is the -ve one.
  } else if (relative_x == 0) {
    return (relative_y < 0) ? 7 : 8;  // On Y-axis: 7 is the +ve part while 8 is the -ve one.
  }

  // Determine the quadrant based on the relative coordinates
  if (relative_x > 0 && relative_y < 0) {
    return 1;  // Quadrant I
  } else if (relative_x < 0 && relative_y < 0) {
    return 2;  // Quadrant II
  } else if (relative_x < 0 && relative_y > 0) {
    return 3;  // Quadrant III
  } else if (relative_x > 0 && relative_y > 0) {
    return 4;  // Quadrant IV
  }

  // Default case (should not reach here)
  return -1;
}

QRectF VisualShaderConnectionGraphicsObject::calculate_bounding_rect_from_coordinates() const {
  const float x1{(float)start_coordinate.x()};
  const float y1{(float)start_coordinate.y()};
  const float x2{(float)end_coordinate.x()};
  const float y2{(float)end_coordinate.y()};

  // Calculate the expanded rect
  const float min_x{qMin(x1, x2)};
  const float min_y{qMin(y1, y2)};
  const float max_x{qMax(x1, x2)};
  const float max_y{qMax(y1, y2)};

  QRectF r({min_x, min_y}, QSizeF(max_x - min_x, max_y - min_y));

  const bool in_h_abnormal_region{x2 < (x1 + h_abnormal_offset)};
  const bool in_v_abnormal_region{std::abs(y2 - y1) < v_abnormal_offset};

  const int quadrant{detect_quadrant({x1, y1}, {x2, y2})};

  // We will expand the bounding rect horizontally so that our connection don't get cut off
  const float a_width_expansion{((x1 + h_abnormal_offset) - x2) * abnormal_face_to_back_control_width_expansion_factor};
  const float a_height_expansion{a_width_expansion * abnormal_face_to_back_control_height_expansion_factor};

  if (in_h_abnormal_region) {
    r.adjust(-a_width_expansion, 0.0f, a_width_expansion, 0.0f);
  }

  switch (quadrant) {
    case 2:  // Quadrant II: Abnormal face to back
    case 3:  // Quadrant III: Abnormal face to back
    case 6:  // On -ve X-axis: Abnormal face to back
      // Elipse like curve
      if (in_v_abnormal_region) {
        r.adjust(0.0f, -a_height_expansion, 0.0f, a_height_expansion);
      }
      break;
    default:
      break;
  }

  return r;
}

std::pair<QPointF, QPointF> VisualShaderConnectionGraphicsObject::calculate_control_points() const {
  QPointF cp1;
  QPointF cp2;

  const float x1{(float)start_coordinate.x()};
  const float y1{(float)start_coordinate.y()};
  const float x2{(float)end_coordinate.x()};
  const float y2{(float)end_coordinate.y()};

  const bool in_h_abnormal_region{x2 < (x1 + h_abnormal_offset)};
  const bool in_v_abnormal_region{std::abs(y2 - y1) < v_abnormal_offset};

  const int quadrant{detect_quadrant({x1, y1}, {x2, y2})};

  // We will expand the bounding rect horizontally so that our connection don't get cut off
  const float a_width_expansion{((x1 + h_abnormal_offset) - x2) * abnormal_face_to_back_control_width_expansion_factor};
  const float a_height_expansion{a_width_expansion * abnormal_face_to_back_control_height_expansion_factor};

  const float cp_x_delta_factor{0.8f};
  const float cp_y_delta_factor{0.25f};

  // Normal region control points deltas
  const float cp_x_delta{(float)rect.width() * cp_x_delta_factor};
  const float cp_y_delta{(float)rect.height() * cp_y_delta_factor};

  // Abnormal region control points deltas
  const float a_cp_x_delta{((float)rect.width() - a_width_expansion) * cp_x_delta_factor};
  const float a_cp_y_delta{((float)rect.height() - a_height_expansion) * cp_y_delta_factor};

  switch (quadrant) {
    case 1:  // Quadrant I: Normal face to back
      // Find out if the connection is going from left to right normally
      if (in_h_abnormal_region) {
        // The connection is not going from left to right normally
        // Our control points will be outside the end_coordinate and start_coordinate bounding rect
        // We will expand the bounding rect horizontally to make it easier to get an accurate coordinate of the size

        // Here we cover cases of nodes not facing each other.
        // This means we can't just send the path straight to the node.

        // Treated as inside Quadrant II
        cp1 = {x1 + a_cp_x_delta, y1};
        cp2 = {x2 - a_cp_x_delta, y2};
      } else {
        // Treated as inside Quadrant I
        cp1 = {x1 + cp_x_delta, y1 - cp_y_delta};
        cp2 = {x2 - cp_x_delta, y2 + cp_y_delta};
      }
      break;
    case 2:  // Quadrant II: Abnormal face to back
      if (in_v_abnormal_region) {
        cp1 = {x1 + a_cp_x_delta, y1 - a_cp_y_delta};
        cp2 = {x2 - a_cp_x_delta, y2 - a_cp_y_delta};
      } else {
        cp1 = {x1 + a_cp_x_delta, y1};
        cp2 = {x2 - a_cp_x_delta, y2};
      }
      break;
    case 3:  // Quadrant III: Abnormal face to back
      if (in_v_abnormal_region) {
        cp1 = {x1 + a_cp_x_delta, y1 - a_cp_y_delta};
        cp2 = {x2 - a_cp_x_delta, y2 - a_cp_y_delta};
      } else {
        cp1 = {x1 + a_width_expansion, y1};
        cp2 = {x2 - a_width_expansion, y2};
      }
      break;
    case 4:  // Quadrant IV: Normal face to back
      if (in_h_abnormal_region) {
        // Treated as inside Quadrant III
        cp1 = {x1 + a_cp_x_delta, y1};
        cp2 = {x2 - a_cp_x_delta, y2};
      } else {
        // Treated as inside Quadrant IV
        cp1 = {x1 + cp_x_delta, y1 + cp_y_delta};
        cp2 = {x2 - cp_x_delta, y2 - cp_y_delta};
      }
      break;
    case 5:  // On +ve X-axis: Normal face to back
      // Straight line
      cp1 = {x1, y1};
      cp2 = {x2, y2};
      break;
    case 6:  // On -ve X-axis: Abnormal face to back
      // Elipse like curve
      cp1 = {x1 + a_cp_x_delta, y1 - a_cp_y_delta};
      cp2 = {x2 - a_cp_x_delta, y2 - a_cp_y_delta};
      break;
    case 7:  // On +ve Y-axis: Abnormal face to back
    case 8:  // On -ve Y-axis: Abnormal face to back
      cp1 = {x1 + a_cp_x_delta, y1};
      cp2 = {x2 - a_cp_x_delta, y2};
      break;
    default:
      return std::make_pair(start_coordinate, end_coordinate);
  }

  return std::make_pair(cp1, cp2);
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****                 Field Widgets                              *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderNodeFieldComboBox::VisualShaderNodeFieldComboBox(const QVariant& initial_value, const int& n_id,
                                                             const EnumDescriptor* enum_descriptor, const int& field_number, const bool& is_input_node, QWidget* parent)
    : QComboBox(parent),
      n_id(n_id),
      field_number(field_number),
      is_input_node(is_input_node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  int size{enum_descriptor->value_count()};
  for (int i{0}; i < size; ++i) {
    int input_type{shadergen_utils::get_enum_value_by_enum_index(enum_descriptor, i)};
    addItem(QString::fromStdString(
        shadergen_utils::get_enum_value_caption_by_value(enum_descriptor, input_type)));
  }

  setCurrentIndex(initial_value.toInt());

  QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                   &VisualShaderNodeFieldComboBox::on_current_index_changed);
}

void VisualShaderNodeFieldComboBox::on_current_index_changed(const int& index) {
  Q_EMIT node_update_requested(n_id, field_number, index);

  if (is_input_node) {
    Q_EMIT input_node_out_port_type_changed(index);
    Q_EMIT revalidate_connections_requested(n_id);
  }
}

VisualShaderNodeFieldLineEditFloat::VisualShaderNodeFieldLineEditFloat(const QVariant& initial_value, const int& n_id,
                                                                        const int& field_number, QWidget* parent)
    : QLineEdit(parent), n_id(n_id), field_number(field_number) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  setText(QString::number(initial_value.toFloat()));

  QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeFieldLineEditFloat::on_text_changed);
}

void VisualShaderNodeFieldLineEditFloat::on_text_changed(const QString& text) {
  SILENT_CHECK_CONDITION_TRUE(text.isEmpty());

  bool ok;
  float t {text.toFloat(&ok)};

  SILENT_CHECK_CONDITION_TRUE(!ok);

  Q_EMIT node_update_requested(n_id, field_number, t);
}

VisualShaderNodeFieldLineEditInt::VisualShaderNodeFieldLineEditInt(const QVariant& initial_value, const int& n_id,
   const int& field_number, QWidget* parent) : QLineEdit(parent), n_id(n_id), field_number(field_number) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  setText(QString::number(initial_value.toInt()));

  QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeFieldLineEditInt::on_text_changed);
}

void VisualShaderNodeFieldLineEditInt::on_text_changed(const QString& text) {
  SILENT_CHECK_CONDITION_TRUE(text.isEmpty());

  bool ok;
  int t {text.toInt(&ok)};

  SILENT_CHECK_CONDITION_TRUE(!ok);

  Q_EMIT node_update_requested(n_id, field_number, t);
}

VisualShaderNodeFieldLineEditUInt::VisualShaderNodeFieldLineEditUInt(const QVariant& initial_value, const int& n_id,
     const int& field_number, QWidget* parent) : QLineEdit(parent), n_id(n_id), field_number(field_number) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  setText(QString::number(initial_value.toUInt()));

  QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeFieldLineEditUInt::on_text_changed);
}

void VisualShaderNodeFieldLineEditUInt::on_text_changed(const QString& text) {
  SILENT_CHECK_CONDITION_TRUE(text.isEmpty());

  bool ok;
  unsigned t {text.toUInt(&ok)};

  SILENT_CHECK_CONDITION_TRUE(!ok);

  Q_EMIT node_update_requested(n_id, field_number, t);
}

VisualShaderNodeFieldCheckBox::VisualShaderNodeFieldCheckBox(const QVariant& initial_value, const int& n_id,
     const int& field_number, QWidget* parent) : QCheckBox(parent), n_id(n_id), field_number(field_number) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  setChecked(initial_value.toBool());

  QObject::connect(this, &QCheckBox::stateChanged, this, &VisualShaderNodeFieldCheckBox::on_state_changed);
}

void VisualShaderNodeFieldCheckBox::on_state_changed(const int& state) {
  Q_EMIT node_update_requested(n_id, field_number, state);
}

VisualShaderNodeFieldSpinBox::VisualShaderNodeFieldSpinBox(const QVariant& initial_value, const int& min_value, const int& max_value, const int& n_id, const int& field_number, QWidget* parent)
      : QSpinBox(parent), n_id(n_id), field_number(field_number) {
  setRange(min_value, max_value);
  setValue(initial_value.toInt());
  connect(this, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualShaderNodeFieldSpinBox::on_value_changed);
}

void VisualShaderNodeFieldSpinBox::on_value_changed(const int& value) {
  Q_EMIT node_update_requested(n_id, field_number, value);
}

