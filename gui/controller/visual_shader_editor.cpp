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

#include <unordered_map>
#include <sstream>
#include "error_macros.hpp"

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
      connections_model(nullptr) {
  VisualShaderEditor::init();

  scene->set_model(visual_shader_model);

  nodes_model = const_cast<ProtoModel*>(visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber))));
  scene->set_nodes_model(nodes_model);

  connections_model = const_cast<ProtoModel*>(visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber))));
  scene->set_connections_model(connections_model);
}

VisualShaderEditor::~VisualShaderEditor() {}

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
  save_button->setIcon(QIcon(":/actions/accept.png"));
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

  scene = new VisualShaderGraphicsScene();
  scene->set_editor(this);

  view = new VisualShaderGraphicsView(scene, scene_layer);
  view->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  scene_layer_layout->addWidget(view);

  // Set the scene layer layout.
  scene_layer->setLayout(scene_layer_layout);

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

  load_image_button = new QPushButton("Load Image", scene_layer);
  load_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  load_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  load_image_button->setToolTip("Load an image to match");
  menu_bar->addWidget(load_image_button);
  QObject::connect(load_image_button, &QPushButton::pressed, this, &VisualShaderEditor::on_load_image_button_pressed);

  match_image_button = new QPushButton("Match Image", scene_layer);
  match_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  match_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  match_image_button->setToolTip("Match the shader to the loaded image");
  menu_bar->addWidget(match_image_button);
  QObject::connect(match_image_button, &QPushButton::pressed, this, &VisualShaderEditor::on_match_image_button_pressed);

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

  std::shared_ptr<IGraphNode> input_node = std::make_shared<GraphNode<VisualShaderNodeInput>>();
  create_node_dialog_nodes_tree_items.emplace_back(input_node->get_caption(), input_node->get_category_path(),
                                                   input_node->get_description(), input_node);

  // GraphNode<VisualShaderNodeFloatConstant> float_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(float_constant_node.get_caption(), float_constant_node.get_category_path(),
  //                                                  float_constant_node.get_description(), float_constant_node);
  // GraphNode<VisualShaderNodeIntConstant> int_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(int_constant_node.get_caption(), int_constant_node.get_category_path(),
  //                                                  int_constant_node.get_description(), int_constant_node);
  // GraphNode<VisualShaderNodeUIntConstant> uint_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(uint_constant_node.get_caption(), uint_constant_node.get_category_path(),
  //                                                  uint_constant_node.get_description(), uint_constant_node);
  // GraphNode<VisualShaderNodeBooleanConstant> boolean_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(boolean_constant_node.get_caption(), boolean_constant_node.get_category_path(),
  //                                                  boolean_constant_node.get_description(), boolean_constant_node);
  // GraphNode<VisualShaderNodeColorConstant> color_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(color_constant_node.get_caption(), color_constant_node.get_category_path(),
  //                                                  color_constant_node.get_description(), color_constant_node);
  // GraphNode<VisualShaderNodeVec2Constant> vec2_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(vec2_constant_node.get_caption(), vec2_constant_node.get_category_path(),
  //                                                  vec2_constant_node.get_description(), vec2_constant_node);
  // GraphNode<VisualShaderNodeVec3Constant> vec3_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(vec3_constant_node.get_caption(), vec3_constant_node.get_category_path(),
  //                                                  vec3_constant_node.get_description(), vec3_constant_node);
  // GraphNode<VisualShaderNodeVec4Constant> vec4_constant_node;
  // create_node_dialog_nodes_tree_items.emplace_back(vec4_constant_node.get_caption(), vec4_constant_node.get_category_path(),
  //                                                  vec4_constant_node.get_description(), vec4_constant_node);

  // GraphNode<VisualShaderNodeValueNoise> value_noise_node;
  // create_node_dialog_nodes_tree_items.emplace_back(value_noise_node.get_caption(), value_noise_node.get_category_path(),
  //                                                  value_noise_node.get_description(), value_noise_node);
  // GraphNode<VisualShaderNodePerlinNoise> perlin_noise_node;
  // create_node_dialog_nodes_tree_items.emplace_back(perlin_noise_node.get_caption(), perlin_noise_node.get_category_path(),
  //                                                  perlin_noise_node.get_description(), perlin_noise_node);
  // GraphNode<VisualShaderNodeVoronoiNoise> voronoi_noise_node;
  // create_node_dialog_nodes_tree_items.emplace_back(voronoi_noise_node.get_caption(), voronoi_noise_node.get_category_path(),
  //                                                  voronoi_noise_node.get_description(), voronoi_noise_node);

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
    node_item->setText(0, QString::fromStdString(item.name));
    node_item->setData(1, Qt::DisplayRole, QString::fromStdString(item.description));
    node_item->setData(1, IGRAPHNODE_SHARED_PTR_USER_ROLE, QVariant::fromValue(item.graph_node));
  }

  //////////////// Start of Footer ////////////////

  this->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Set the window title and icon.
  this->setWindowTitle("Visual Shader Editor");
  // this->setWindowIcon(QIcon(":/resources/visual_shader.png"));
  this->setLayout(layout);
}

void VisualShaderEditor::create_node(const QPointF& coordinate) {
  QTreeWidgetItem* selected_item{create_node_dialog->get_selected_item()};

  if (!selected_item) {
    return;
  }

  VisualShaderEditor::add_node(selected_item, coordinate);
}

void VisualShaderEditor::add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate) {
  QVariant graph_node = selected_item->data(1, IGRAPHNODE_SHARED_PTR_USER_ROLE);

  CHECK_CONDITION_TRUE(!graph_node.canConvert<std::shared_ptr<IGraphNode>>(), "Node type is empty");

  scene->add_node(graph_node.value<std::shared_ptr<IGraphNode>>(), coordinate);
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
  std::vector<VisualShader::VisualShaderNode> nodes;
  std::vector<VisualShader::VisualShaderConnection> connections;

  bool result{shadergen_visual_shader_generator::generate_shader(code, nodes, connections)};
  CHECK_CONDITION_TRUE(!result, "Failed to generate shader code");
  code_previewer->setPlainText(QString::fromStdString(code));
  code_previewer_dialog->exec();
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

void VisualShaderEditor::on_match_image_button_pressed() {}

std::vector<std::string> VisualShaderEditor::parse_node_category_path(const std::string& node_category_path) {
  std::vector<std::string> tokens;
  std::stringstream ss(node_category_path);
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

CreateNodeDialog::~CreateNodeDialog() {}

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
/*****                  ShaderPreviewerWidget                     *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

ShaderPreviewerWidget::ShaderPreviewerWidget(QWidget* parent)
    : QOpenGLWidget(parent), shader_program(nullptr), VAO(0), VBO(0) {}

ShaderPreviewerWidget::~ShaderPreviewerWidget() {}

void ShaderPreviewerWidget::set_code(const std::string& new_code) {
  if (new_code == code) return;

  code = new_code;
  shader_needs_update = true;
  if (isVisible()) {
    update_shader_program();
    timer.restart();
  }
}

void ShaderPreviewerWidget::initializeGL() {
  QOpenGLFunctions_4_3_Core* f{QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>()};

  if (!f) {
    qWarning() << "Failed to get OpenGL 4.3 functions";
    return;
  }

  if (!f->initializeOpenGLFunctions()) {
    qWarning() << "Failed to initialize OpenGL functions";
    return;
  }

  f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background
  init_buffers();
  init_shaders();

  timer.start();

  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ShaderPreviewerWidget::cleanup);
}

void ShaderPreviewerWidget::resizeGL(int w, int h) {
  QOpenGLFunctions_4_3_Core* f{QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>()};

  if (!f) {
    qWarning() << "Failed to get OpenGL 4.3 functions";
    return;
  }

  f->glViewport(0, 0, w, h);
}

void ShaderPreviewerWidget::paintGL() {
  // Check https://doc.qt.io/qt-5/qopenglwidget.html#isValid
  // At start, the widget is hidden so this call returns false which results
  // in returning (no painting happens).
  if (!isValid()) return;

  QOpenGLFunctions_4_3_Core* f{QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>()};

  if (!f) {
    qWarning() << "Failed to get OpenGL 4.3 functions";
    return;
  }

  if (shader_needs_update) {
    update_shader_program();
  }

  if (!shader_program || !shader_program->isLinked()) {
    qWarning() << "Shader program is not linked.";
    return;
  }

  float time_value{timer.elapsed() * 0.001f};

  f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  f->glClear(GL_COLOR_BUFFER_BIT);

  shader_program->bind();
  shader_program->setUniformValue("uTime", time_value);

  f->glBindVertexArray(VAO);
  f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  f->glBindVertexArray(0);

  shader_program->release();

  update();  // Request a repaint
  Q_EMIT scene_update_requested();
}

void ShaderPreviewerWidget::cleanup() {
  makeCurrent();

  QOpenGLFunctions_4_3_Core* f{QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>()};

  if (!f) {
    qWarning() << "Failed to get OpenGL 4.3 functions";
    return;
  }

  f->glDeleteVertexArrays(1, &VAO);
  f->glDeleteBuffers(1, &VBO);
}

void ShaderPreviewerWidget::init_buffers() {
  QOpenGLFunctions_4_3_Core* f{QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>()};

  if (!f) {
    qWarning() << "Failed to get OpenGL 4.3 functions";
    return;
  }

  float vertices[] = {
      // coordinates    // frag coords
      -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f};

  f->glGenVertexArrays(1, &VAO);
  f->glGenBuffers(1, &VBO);

  f->glBindVertexArray(VAO);

  f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
  f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  f->glEnableVertexAttribArray(0);

  f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  f->glEnableVertexAttribArray(1);

  f->glBindVertexArray(0);
}

void ShaderPreviewerWidget::update_shader_program() {
  shader_program.reset(new QOpenGLShaderProgram());

  const char* vertex_shader_source = R"(
      #version 330 core
      layout(location = 0) in vec2 aPos;
      layout(location = 1) in vec2 aFragCoord;

      out vec2 FragCoord;

      void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        FragCoord = aFragCoord;
      }
  )";

  std::string fragment_shader_source{code.empty() ? R"(
      #version 330 core
      out vec4 FragColor;
      in vec2 FragCoord;

      uniform float uTime;

      void main() {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
      }
  )"
                                                  : "#version 330 core\n\n" + code};

  if (!shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source)) {
    qWarning() << "Vertex shader compilation failed:" << shader_program->log();
  }

  if (!shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source.c_str())) {
    qWarning() << "Fragment shader compilation failed:" << shader_program->log();
  }

  if (!shader_program->link()) {
    qWarning() << "Shader program linking failed:" << shader_program->log();
  }

  shader_needs_update = false;
}

void ShaderPreviewerWidget::init_shaders() { update_shader_program(); }

void ShaderPreviewerWidget::showEvent(QShowEvent* event) {
  QOpenGLWidget::showEvent(event);
  if (!timer.isValid()) {
    // See https://doc.qt.io/qt-5/qelapsedtimer.html#start.
    timer.start();  // Start the timer on first show
  }
}

void ShaderPreviewerWidget::hideEvent(QHideEvent* event) {
  QOpenGLWidget::hideEvent(event);
  // See https://doc.qt.io/qt-5/qelapsedtimer.html#invalidate.
  timer.invalidate();
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

VisualShaderGraphicsScene::~VisualShaderGraphicsScene() {}

bool VisualShaderGraphicsScene::add_node(const std::shared_ptr<IGraphNode>& graph_node, const QPointF& coordinate) {
  int n_id{VisualShaderGraphicsScene::get_new_node_id(visual_shader_model, nodes_model)};
  CHECK_CONDITION_TRUE_NON_VOID(n_id == 0, false, "The id " + std::to_string(n_id) + " is reserved for the output node");

  int row_entry{nodes_model->append_row()};
  // CHECK_CONDITION_TRUE_NON_VOID(row_entry == 0, false, "The row entry " + std::to_string(row_entry) + " is reserved for the output node");

  bool result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber)), n_id);
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set node id");

  result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kXCoordinateFieldNumber)), coordinate.x());
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set node x coordinate");

  result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kYCoordinateFieldNumber)), coordinate.y());
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set node y coordinate");

  if (auto input_node = std::dynamic_pointer_cast<GraphNode<VisualShaderNodeInput>>(graph_node)) {
    result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber), FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)), VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED);
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set input node");
  } else {
    FAIL_AND_RETURN_NON_VOID(false, "Unknown node type");
  }

  // switch (graph_node.index()) {
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_FLOAT_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kFloatConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeFloatConstant::kValueFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set float constant value");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_INT_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIntConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeIntConstant::kValueFieldNumber)), 0);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set int constant value");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_UINT_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kUintConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeUIntConstant::kValueFieldNumber)), 0u);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set uint constant value");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_BOOLEAN_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kBooleanConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeBooleanConstant::kValueFieldNumber)), false);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set boolean constant value");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_COLOR_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeColorConstant::kRFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set color constant R");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeColorConstant::kGFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set color constant G");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeColorConstant::kBFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set color constant B");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kColorConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeColorConstant::kAFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set color constant A");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC2_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kXFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec2 constant X");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec2ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec2Constant::kYFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec2 constant Y");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC3_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kXFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec3 constant X");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kYFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec3 constant Y");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec3ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec3Constant::kZFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec3 constant Z");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VEC4_CONSTANT_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kXFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec4 constant X");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kYFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec4 constant Y");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kZFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec4 constant Z");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVec4ConstantFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVec4Constant::kWFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set vec4 constant W");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VALUE_NOISE_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kValueNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodeValueNoise::kScaleFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set value noise scale");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_PERLIN_NOISE_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kPerlinNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodePerlinNoise::kScaleFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set perlin noise scale");
    //   break;
    // case VARIANT_NODE_TYPE_VISUAL_SHADER_NODE_VORONOI_NOISE_INDEX:
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kAngleOffsetFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set voronoi noise angle offset");
    //   result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kVoronoiNoiseFieldNumber), FieldPath::FieldNumber(VisualShaderNodeVoronoiNoise::kCellDensityFieldNumber)), 0.0f);
    //   CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set voronoi noise cell density");
    //   break;
  //   default:
  //     FAIL_AND_RETURN_NON_VOID(false, "Unknown node type");
  // }

  CHECK_CONDITION_TRUE_NON_VOID(node_graphics_objects.find(n_id) != node_graphics_objects.end(), false, "Graphics node object already exists");

  QList<QGraphicsView*> views{this->views()};
  if (views.isEmpty()) {
    DEBUG_PRINT("No views available");
    return false;
  }

  VisualShaderGraphicsView* view{dynamic_cast<VisualShaderGraphicsView*>(views.first())};

  if (coordinate.x() < view->get_x() ||
      coordinate.x() > view->get_x() + view->get_width() ||
      coordinate.y() < view->get_y() ||
      coordinate.y() > view->get_y() + view->get_height()) {
    DEBUG_PRINT("Node is out of view bounds");
  }

  std::vector<std::string> in_port_captions;
  std::vector<std::string> out_port_captions;
  in_port_captions.resize(graph_node->get_input_port_count());
  out_port_captions.resize(graph_node->get_output_port_count());

  for (int i{0}; i < (int)in_port_captions.size(); i++) in_port_captions.at(i) = graph_node->get_input_port_caption(i);
  for (int i{0}; i < (int)out_port_captions.size(); i++) out_port_captions.at(i) = graph_node->get_output_port_caption(i);

  VisualShaderNodeGraphicsObject* n_o{new VisualShaderNodeGraphicsObject(n_id, coordinate, graph_node->get_caption(), in_port_captions, out_port_captions)};

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

  VisualShaderNodeEmbedWidget* embed_widget{new VisualShaderNodeEmbedWidget(visual_shader_model, nodes_model, n_id, graph_node)};
  QGraphicsProxyWidget* embed_widget_proxy{new QGraphicsProxyWidget(n_o)};
  embed_widget_proxy->setWidget(embed_widget);
  n_o->set_embed_widget(embed_widget);
  QObject::connect(embed_widget, &VisualShaderNodeEmbedWidget::shader_preview_update_requested, this,
                    &VisualShaderGraphicsScene::on_update_shader_previewer_widgets_requested);

  QObject::connect(embed_widget, &VisualShaderNodeEmbedWidget::node_update_requested, n_o,
                    &VisualShaderNodeGraphicsObject::on_node_update_requested);

  // Send the shader previewer widget
  embed_widget->set_shader_previewer_widget(n_o->get_shader_previewer_widget());

  if (ShaderPreviewerWidget* spw{n_o->get_shader_previewer_widget()}) {
    QObject::connect(spw, &ShaderPreviewerWidget::scene_update_requested, this,
                     &VisualShaderGraphicsScene::on_scene_update_requested);
  }

  QObject::connect(n_o, &VisualShaderNodeGraphicsObject::node_deleted, this,
                   &VisualShaderGraphicsScene::on_node_deleted);

  node_graphics_objects[n_id] = n_o;

  addItem(n_o);

  return true;
}

bool VisualShaderGraphicsScene::delete_node(const int& n_id, const int& in_port_count, const int& out_port_count) {
  int row_entry{find_node_entry(visual_shader_model, nodes_model, n_id)};
  VALIDATE_INDEX_NON_VOID(row_entry, nodes_model->rowCount(), false, "Node entry not found");

  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(n_o, false, "Node graphics object is null");

  // Remove all connections to the node
  for (int i{0}; i < in_port_count; i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!i_port || !i_port->is_connected());

    // Get the output port of the connection
    VisualShaderConnectionGraphicsObject* c_o{i_port->get_connection_graphics_object()};
    SILENT_CONTINUE_IF_TRUE(!c_o);

    bool result{this->delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), n_id, i)};
    CONTINUE_IF_TRUE(!result, "Failed to delete connection");
  }

  for (int i{0}; i < out_port_count; i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!o_port || !o_port->is_connected());

    std::vector<VisualShaderConnectionGraphicsObject*> c_os{o_port->get_connection_graphics_objects()};

    for (VisualShaderConnectionGraphicsObject* c_o : c_os) {
      SILENT_CONTINUE_IF_TRUE(!c_o);

      bool result{this->delete_connection(n_id, i, c_o->get_to_node_id(), c_o->get_to_port_index())};
      CONTINUE_IF_TRUE(!result, "Failed to delete connection");
    }
  }

  // Remove the node from the model
  bool result{nodes_model->remove_row(row_entry)};
  CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to remove row");

  // Remove the node from the scene
  // TODO: Why if we exchange the order of these two lines, the program crashes?
  int erased_count {(int)this->node_graphics_objects.erase(n_id)};
  CHECK_CONDITION_TRUE_NON_VOID(erased_count == 0, false, "Failed to erase node graphics object");
  
  remove_item(n_o);

  return true;
}

void VisualShaderGraphicsScene::on_update_shader_previewer_widgets_requested() {
  for (auto& [n_id, n_o] : node_graphics_objects) {
    SILENT_CONTINUE_IF_TRUE(n_id == 0);  // Skip the output node

    ShaderPreviewerWidget* spw{n_o->get_shader_previewer_widget()};
    if (!spw) {
      continue;
    }

    spw->set_code(shadergen_visual_shader_generator::generate_preview_shader(n_id, 0));  // 0 is the output port index
  }

  on_scene_update_requested();
}

void VisualShaderGraphicsScene::on_scene_update_requested() { update(); }

void VisualShaderGraphicsScene::on_in_port_remove_requested(VisualShaderInputPortGraphicsObject* in_port) {
  if (in_port->is_connected()) {
    VisualShaderConnectionGraphicsObject* c_o{in_port->get_connection_graphics_object()};
    delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(),
                      c_o->get_to_port_index());
  }

  remove_item(in_port);
}

void VisualShaderGraphicsScene::on_out_port_remove_requested(VisualShaderOutputPortGraphicsObject* out_port) {
  if (out_port->is_connected()) {
    std::vector<VisualShaderConnectionGraphicsObject*> c_os{out_port->get_connection_graphics_objects()};
    for (VisualShaderConnectionGraphicsObject* c_o : c_os) {
      delete_connection(c_o->get_from_node_id(), c_o->get_from_port_index(), c_o->get_to_node_id(),
                        c_o->get_to_port_index());
    }
  }

  remove_item(out_port);
}

int VisualShaderGraphicsScene::get_new_node_id(ProtoModel* visual_shader_model, ProtoModel* nodes_model) {
  int size {nodes_model->rowCount()};

  int max_id {0};
  for (int i{0}; i < size; i++) {
    // Path to the id field of the node
    FieldPath path{FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(i), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber))};
    int n_id {visual_shader_model->data(path).toInt()};
    if (n_id > max_id) max_id = n_id;
  }

  return max_id + 1; // Minimum id is 1 (0 is reserved for the output node)
}

int VisualShaderGraphicsScene::get_new_connection_id(ProtoModel* visual_shader_model, ProtoModel* connections_model) {
  int size {connections_model->rowCount()};

  int max_id {-1};
  for (int i{0}; i < size; i++) {
    // Path to the id field of the connection
    FieldPath path{FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(i), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber))};
    int c_id {visual_shader_model->data(path).toInt()};
    if (c_id > max_id) max_id = c_id;
  }

  return max_id + 1;
}

int VisualShaderGraphicsScene::find_node_entry(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id) {
  int size {nodes_model->rowCount()};

  for (int i{0}; i < size; i++) {
    // Path to the id field of the node
    FieldPath path{FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(i), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kIdFieldNumber))};
    if (visual_shader_model->data(path).toInt() == n_id) return i;
  }

  return -1;
}

int VisualShaderGraphicsScene::find_connection_entry(ProtoModel* visual_shader_model, ProtoModel* connections_model, const int& c_id) {
  int size {connections_model->rowCount()};

  for (int i{0}; i < size; i++) {
    // Path to the id field of the connection
    FieldPath path{FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(i), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber))};
    if (visual_shader_model->data(path).toInt() == c_id) return i;
  }

  return -1;
}

bool VisualShaderGraphicsScene::add_connection(const int& from_node_id, const int& from_port_index,
                                               const int& to_node_id, const int& to_port_index) {
  QList<QGraphicsView*> views{this->views()};
  CHECK_CONDITION_TRUE_NON_VOID(views.isEmpty(), false, "No views available");

  // Create the connection and set its start
  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  VisualShaderGraphicsView* view{dynamic_cast<VisualShaderGraphicsView*>(views.first())};

  if (from_o_port->get_global_coordinate().x() < view->get_x() ||
      from_o_port->get_global_coordinate().x() > view->get_x() + view->get_width()) {
    DEBUG_PRINT("Start of connection is out of view bounds");
  }

  if (!this->temporary_connection_graphics_object) {
    int c_id{get_new_connection_id(visual_shader_model, connections_model)};
    this->temporary_connection_graphics_object =
        new VisualShaderConnectionGraphicsObject(c_id, from_node_id, from_port_index, from_o_port->get_global_coordinate());
    from_o_port->connect(this->temporary_connection_graphics_object);
    addItem(this->temporary_connection_graphics_object);
    return true;
  }

  if (to_node_id != -1 && to_port_index != -1) {
    // Set the end of the connection
    VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
    CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

    VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
    CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

    if (to_i_port->get_global_coordinate().y() < view->get_y() ||
        to_i_port->get_global_coordinate().y() > view->get_y() + view->get_height()) {
      DEBUG_PRINT("End of connection is out of view bounds");
    }

    int c_id{get_new_connection_id(visual_shader_model, connections_model)};

    int row_entry{connections_model->append_row()};

    bool result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kIdFieldNumber)), c_id);
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection id");

    result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromNodeIdFieldNumber)), from_node_id);
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection from node id");

    result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kFromPortIndexFieldNumber)), from_port_index);
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection from port index");

    result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToNodeIdFieldNumber)), to_node_id);
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection to node id");

    result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderConnection::kToPortIndexFieldNumber)), to_port_index);
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to set connection to port index");

    this->temporary_connection_graphics_object->set_end_coordinate(to_i_port->get_global_coordinate());
    to_i_port->connect(this->temporary_connection_graphics_object);
    this->temporary_connection_graphics_object->set_to_node_id(to_node_id);
    this->temporary_connection_graphics_object->set_to_port_index(to_port_index);
    this->temporary_connection_graphics_object = nullptr;  // Make sure to reset the temporary connection object

    on_update_shader_previewer_widgets_requested();

    return true;
  }

  return false;
}

bool VisualShaderGraphicsScene::delete_connection(const int& c_id, const int& from_node_id, const int& from_port_index,
                                                  const int& to_node_id, const int& to_port_index) {
  VisualShaderNodeGraphicsObject* from_n_o{this->get_node_graphics_object(from_node_id)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_n_o, false, "Failed to get from node graphics object");

  VisualShaderOutputPortGraphicsObject* from_o_port{from_n_o->get_output_port_graphics_object(from_port_index)};
  CHECK_PARAM_NULLPTR_NON_VOID(from_o_port, false, "Failed to get from output port graphics object");

  if (this->temporary_connection_graphics_object) {
    from_o_port->detach_connection(this->temporary_connection_graphics_object);
    remove_item(this->temporary_connection_graphics_object);
    this->temporary_connection_graphics_object = nullptr;
    return true;
  }

  // If we have a complete connection, then we can disconnect the nodes
  if (to_node_id != -1 && to_port_index != -1) {
    VisualShaderConnectionGraphicsObject* c_o{from_o_port->get_connection_graphics_object(to_node_id, to_port_index)};
    CHECK_PARAM_NULLPTR_NON_VOID(c_o, false, "Failed to get connection graphics object");

    VisualShaderNodeGraphicsObject* to_n_o{this->get_node_graphics_object(to_node_id)};
    CHECK_PARAM_NULLPTR_NON_VOID(to_n_o, false, "Failed to get to node graphics object");

    VisualShaderInputPortGraphicsObject* to_i_port{to_n_o->get_input_port_graphics_object(to_port_index)};
    CHECK_PARAM_NULLPTR_NON_VOID(to_i_port, false, "Failed to get to input port graphics object");

    int row_entry{find_connection_entry(visual_shader_model, connections_model, c_id)};
    VALIDATE_INDEX_NON_VOID(row_entry, connections_model->rowCount(), false, "Connection entry not found");

    bool result{connections_model->remove_row(row_entry)};
    CHECK_CONDITION_TRUE_NON_VOID(!result, false, "Failed to remove connection entry");

    to_i_port->detach_connection();
    from_o_port->detach_connection(c_o);
    remove_item(c_o);

    on_update_shader_previewer_widgets_requested();

    return true;
  }

  return false;
}

VisualShaderNodeGraphicsObject* VisualShaderGraphicsScene::get_node_graphics_object(const int& n_id) const {
  VisualShaderNodeGraphicsObject* n_o{nullptr};

  auto it{node_graphics_objects.find(n_id)};
  if (it != node_graphics_objects.end()) {
    n_o = it->second;
  }

  return n_o;
}

void VisualShaderGraphicsScene::on_node_deleted(const int& n_id, const int& in_port_count, const int& out_port_count) {
  CHECK_CONDITION_TRUE(n_id == 0, "Cannot delete the output node");

  bool result{delete_node(n_id, in_port_count, out_port_count)};
  if (!result) {
    DEBUG_PRINT("Failed to delete node");
  }

  Q_EMIT visual_shader_model->dataChanged(QModelIndex(), QModelIndex()); // To update the serialized data
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
      c_o = i_port->get_connection_graphics_object();
      temporary_connection_graphics_object = c_o;  // Store the connection object for access in the next drag call
      int c_id{c_o->get_id()};
      int row_entry{find_connection_entry(visual_shader_model, connections_model, c_id)};
      VALIDATE_INDEX(row_entry, connections_model->rowCount(), "Connection entry not found");
      bool result{connections_model->remove_row(row_entry)};
      if (!result) {
        DEBUG_PRINT("Failed to remove connection entry");
      }
      i_port->detach_connection();
      c_o->detach_end();

      on_update_shader_previewer_widgets_requested();
    } else if (!i_port->is_connected() && temporary_connection_graphics_object) {
      c_o = temporary_connection_graphics_object;
      on_scene_update_requested();
    } else {
      return;
    }

    c_o->set_end_coordinate(coordinate);

    return;
  }

  if (o_port->is_connected() && temporary_connection_graphics_object) {
    c_o = temporary_connection_graphics_object;
  } else if (!temporary_connection_graphics_object) {
    bool result{this->add_connection(o_port->get_node_id(), o_port->get_port_index())};
    CHECK_CONDITION_TRUE(!result, "Failed to add connection");
    c_o = temporary_connection_graphics_object;
  } else {
    return;
  }

  c_o->set_end_coordinate(coordinate);
}

void VisualShaderGraphicsScene::on_port_dropped(QGraphicsObject* port, const QPointF& coordinate) {
  VisualShaderConnectionGraphicsObject* c_o{temporary_connection_graphics_object};
  SILENT_CHECK_PARAM_NULLPTR(c_o);

  // Find all items under the coordinate
  QList<QGraphicsItem*> items_at_coordinate{this->items(coordinate)};

  // Iterate through the items and check if an input port is under the mouse
  VisualShaderInputPortGraphicsObject* in_p_o{nullptr};
  for (QGraphicsItem* item : items_at_coordinate) {
    // Check if the item is an input port
    in_p_o = dynamic_cast<VisualShaderInputPortGraphicsObject*>(item);
    SILENT_BREAK_IF_TRUE(in_p_o);
  }

  VisualShaderOutputPortGraphicsObject* o_port{dynamic_cast<VisualShaderOutputPortGraphicsObject*>(port)};

  if (!o_port) {
    VisualShaderInputPortGraphicsObject* i_port{dynamic_cast<VisualShaderInputPortGraphicsObject*>(port)};
    SILENT_CHECK_PARAM_NULLPTR(i_port);

    if (!in_p_o) {
      int c_id{c_o->get_id()};
      bool result{this->delete_connection(c_id, c_o->get_from_node_id(),
                               c_o->get_from_port_index())};
      if (!result) {
        DEBUG_PRINT("Failed to delete connection");
      }

      return;  // Return because we dragging an input port and dropped on nothing
    }

    bool result{add_connection(c_o->get_from_node_id(),
                               c_o->get_from_port_index(), in_p_o->get_node_id(),
                               in_p_o->get_port_index())};

    if (!result) {
      int c_id{c_o->get_id()};
      bool result{this->delete_connection(c_id, c_o->get_from_node_id(),
                                          c_o->get_from_port_index())};

      if (!result) {
        DEBUG_PRINT("Failed to delete connection");
      }

      return;  // Return because we dragging an input port and dropped on nothing
    }

    return;
  }

  if (!in_p_o) {
    int c_id{c_o->get_id()};
    bool result{this->delete_connection(c_id, c_o->get_from_node_id(),
                                        c_o->get_from_port_index())};

    if (!result) {
      DEBUG_PRINT("Failed to delete connection");
    }

    return;  // Return because we dragging an output port and dropped on nothing
  }

  bool result{
      add_connection(o_port->get_node_id(), o_port->get_port_index(), in_p_o->get_node_id(), in_p_o->get_port_index())};

  if (!result) {
    int c_id{c_o->get_id()};
    bool result{this->delete_connection(c_id, c_o->get_from_node_id(),
                                        c_o->get_from_port_index())};

    if (!result) {
      DEBUG_PRINT("Failed to delete connection");
    }

    return;
  }
}

void VisualShaderGraphicsScene::on_node_moved(const int& n_id, const int& in_port_count, const int& out_port_count, const QPointF& new_coordinate) {
  int row_entry{find_node_entry(visual_shader_model, nodes_model, n_id)};
  VALIDATE_INDEX(row_entry, nodes_model->rowCount(), "Node entry not found");

  bool result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kXCoordinateFieldNumber)), new_coordinate.x());
  CHECK_CONDITION_TRUE(!result, "Failed to set node x coordinate");

  result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kYCoordinateFieldNumber)), new_coordinate.y());
  CHECK_CONDITION_TRUE(!result, "Failed to set node y coordinate");

  // Update coordinates of all connected connections
  VisualShaderNodeGraphicsObject* n_o{this->get_node_graphics_object(n_id)};

  for (int i{0}; i < in_port_count; i++) {
    VisualShaderInputPortGraphicsObject* i_port{n_o->get_input_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!i_port || !i_port->is_connected());

    VisualShaderConnectionGraphicsObject* c_o{i_port->get_connection_graphics_object()};
    SILENT_CONTINUE_IF_TRUE(!c_o);

    c_o->set_end_coordinate(i_port->get_global_coordinate());
  }

  for (int i{0}; i < out_port_count; i++) {
    VisualShaderOutputPortGraphicsObject* o_port{n_o->get_output_port_graphics_object(i)};
    SILENT_CONTINUE_IF_TRUE(!o_port || !o_port->is_connected());

    std::vector<VisualShaderConnectionGraphicsObject*> c_os{o_port->get_connection_graphics_objects()};

    for (VisualShaderConnectionGraphicsObject* c_o : c_os) {
      SILENT_CONTINUE_IF_TRUE(!c_o);

      c_o->set_start_coordinate(o_port->get_global_coordinate());
    }
  }
}

void VisualShaderGraphicsScene::remove_item(QGraphicsItem* item) {
  removeItem(item);
  delete item;
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
    DEBUG_PRINT("Current zoom level is greater than the maximum zoom level.");
    DEBUG_PRINT("Maybe due to having a very large distance between the nodes.");
  }

  if ((float)transform().m11() < zoom_min) {
    DEBUG_PRINT("Current zoom level is less than the minimum zoom level.");
    DEBUG_PRINT("Maybe due to having all the nodes outside the scene bounds.");
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
                                                               const std::string& caption,
                                                               const std::vector<std::string>& in_port_captions,
                                                               const std::vector<std::string>& out_port_captions, 
                                                               QGraphicsItem* parent)
    : QGraphicsObject(parent),
      n_id(n_id),
      coordinate(coordinate),
      caption(caption),
      in_port_count(in_port_captions.size()),
      out_port_count(out_port_captions.size()),
      in_port_captions(in_port_captions),
      out_port_captions(out_port_captions),
      context_menu(nullptr),
      delete_node_action(nullptr),
      rect_width(0.0f),
      caption_rect_height(0.0f),
      rect_height(0.0f),
      rect_margin(0.0f),
      rect_padding(0.0f),
      embed_widget(nullptr),
      matching_image_widget(nullptr)// ,
      { //shader_previewer_widget(nullptr) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setVisible(true);
  setOpacity(this->opacity);

  setZValue(0);

  setPos(coordinate.x(), coordinate.y());

  // Output node should have a matching image widget
  if (n_id == 0) {
    QGraphicsProxyWidget* matching_image_widget_proxy{new QGraphicsProxyWidget(this)};
    matching_image_widget = new OriginalMatchingImageWidget();
    matching_image_widget_proxy->setWidget(matching_image_widget);
  } else {
    // Create the shader previewer widget
    QGraphicsProxyWidget* shader_previewer_widget_proxy{new QGraphicsProxyWidget(this)};
    shader_previewer_widget = new ShaderPreviewerWidget();
    shader_previewer_widget->setVisible(false);
    shader_previewer_widget_proxy->setWidget(shader_previewer_widget);
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

void VisualShaderNodeGraphicsObject::on_delete_node_action_triggered() { Q_EMIT node_deleted(n_id, in_port_count, out_port_count); }

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

void VisualShaderNodeGraphicsObject::on_node_update_requested() {
  // Here if the number of ports changed, for example, changing the operation type in a decompose node,
  // we need to remove any extra ports that are not needed anymore. Don't forget to remove the connections
  // as well.
  if ((int)in_port_graphics_objects.size() > in_port_count) {
    int p_index{in_port_count};
    int size{(int)in_port_graphics_objects.size() - p_index};
    for (int i{0}; i < size; ++i) {
      Q_EMIT in_port_remove_requested(in_port_graphics_objects.at(p_index));
      in_port_graphics_objects.erase(p_index);
      p_index++;
    }
  }

  if ((int)out_port_graphics_objects.size() > out_port_count) {
    int p_index{out_port_count};
    int size{(int)out_port_graphics_objects.size() - p_index};
    for (int i{0}; i < size; ++i) {
      Q_EMIT out_port_remove_requested(out_port_graphics_objects.at(p_index));
      out_port_graphics_objects.erase(p_index);
      p_index++;
    }
  }

  update();
  Q_EMIT scene_update_requested();
}

QRectF VisualShaderNodeGraphicsObject::boundingRect() const {
  QFont f("Arial", caption_font_size);
  f.setBold(true);
  QFontMetrics fm(f);

  QString t_caption{QString::fromStdString(caption)};

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

  rect_height = t_rect_h;

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
        w = (float)fm.horizontalAdvance("Input");
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
        w = (float)fm.horizontalAdvance("Output");
      }

      if ((w + port_caption_spacing) > max_out_p_width) {
        max_out_p_width = w + port_caption_spacing;
      }
    }

    float calculated_rect{max_in_p_width + embed_widget_width + max_out_p_width + embed_widget_h_padding * 2.0f};

    if (calculated_rect > rect_width) {
      rect_width = calculated_rect;
    }

    // Check the height
    float calculated_height{caption_rect_height + body_rect_header_height + embed_widget->height() +
                            body_rect_footer_height + embed_widget_v_padding * 2.0f};

    if (calculated_height > rect_height) {
      rect_height = calculated_height;
    }
  }

  QRectF r({0.0f, 0.0f}, QSizeF(rect_width, rect_height));

  // Calculate the margin
  this->rect_margin = port_diameter * 0.5f;

  // Calculate the rect padding
  // We add a safe area around the rect to prevent the ports from being cut off
  this->rect_padding = port_diameter * 0.5f;

  r.adjust(-rect_margin - rect_padding, -rect_margin - rect_padding, rect_margin + rect_padding,
           rect_margin + rect_padding);

  return r;
}

void VisualShaderNodeGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setClipRect(option->exposedRect);

  // Get the rect without the padding
  QRectF r{this->boundingRect()};

  // {
  //   // Draw Node Rect
  //   painter->setPen(Qt::red);
  //   painter->setBrush(Qt::NoBrush);
  //   painter->drawRect(r);
  // }

  // Add the padding to the rect
  r.adjust(rect_padding, rect_padding, -rect_padding, -rect_padding);

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

  // Draw Matching Image Widget
  if (n_id == 0) {
    float matching_image_widget_x{(float)r.x() + (float)r.width() + spacing_between_output_node_and_matching_image};
    float matching_image_widget_y{(float)r.y()};

    matching_image_widget->setGeometry(matching_image_widget_x, matching_image_widget_y, r.height(), r.height());
  } else {
    // Draw Shader Previewer Widget
    // float shader_previewer_widget_x{(float)r.x()};
    // float shader_previewer_widget_y{(float)r.y() + (float)r.height() +
    //                                 spacing_between_current_node_and_shader_previewer};
    // shader_previewer_widget->setGeometry(shader_previewer_widget_x, shader_previewer_widget_y, r.width(), r.width());
  }

  // Add the margin to the rect
  r.adjust(rect_margin, rect_margin, -rect_margin, -rect_margin);

  // {
  //   // Draw Node Rect
  //   painter->setPen(Qt::red);
  //   painter->setBrush(Qt::NoBrush);
  //   painter->drawRect(r);
  // }

  float rect_x{(float)r.topLeft().x()};
  float rect_y{(float)r.topLeft().y()};

  float rect_w{(float)r.width()};
  float rect_h{(float)r.height()};

  QRectF caption_rect(rect_x, rect_y, rect_w, caption_rect_height);

  {
    // Draw Caption
    QString t_caption{QString::fromStdString(caption)};

    QFont t_f{painter->font()};

    QFont f("Arial", caption_font_size);
    f.setBold(true);
    QFontMetrics fm(f);
    painter->setFont(f);

    // Calculate the coordinates of the caption
    float x{(float)(caption_rect.center().x() - (float)fm.horizontalAdvance(t_caption) * 0.5f)};

    // Instead of subtracting, add the ascent to properly align text within the rect
    float y{(float)(caption_rect.center().y() + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent()))};

    // {
    //   painter->setPen(Qt::red);
    //   painter->setBrush(Qt::NoBrush);
    //   painter->drawRect(QRectF(x,y, (float)fm.horizontalAdvance(caption), (float)(fm.ascent() + fm.descent())));
    // }

    QPointF coordinate{x, y};

    painter->setPen(this->font_color);
    painter->drawText(coordinate, t_caption);

    painter->setFont(t_f);  // Reset the font
  }

  QPointF caption_rect_bl{caption_rect.bottomLeft()};
  QPointF first_in_port_coordinate{caption_rect_bl.x(), caption_rect_bl.y() + body_rect_header_height};

  // Correct X coordinate: Remove the margin
  first_in_port_coordinate.setX((float)first_in_port_coordinate.x() - this->rect_margin);

  {
    // Draw Input Ports
    for (int i{0}; i < in_port_count; ++i) {
      QPointF port_coordinate{first_in_port_coordinate.x(), first_in_port_coordinate.y() + body_rect_port_step * i};

      QRectF port_rect(port_coordinate.x(), port_coordinate.y(), port_diameter, port_diameter);

      // Adjust the port rect to be centered
      port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f,
                       -port_rect.height() * 0.5f);

      // Draw caption
      QString p_n{QString::fromStdString(in_port_captions.at(i))};

      if (!p_n.isEmpty()) {
        QFont t_f{painter->font()};

        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);
        painter->setFont(f);

        float x{rect_x + port_caption_spacing};

        float y{(float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())};

        // {
        //   painter->setPen(Qt::red);
        //   painter->setBrush(Qt::NoBrush);
        //   painter->drawRect(QRectF(x,y, (float)fm.horizontalAdvance(p_n), (float)(fm.ascent() + fm.descent())));
        // }

        QPointF coordinate{x, y};

        painter->setPen(this->font_color);
        painter->drawText(coordinate, p_n);

        painter->setFont(t_f);  // Reset the font
      }

      if (in_port_graphics_objects.find(i) != in_port_graphics_objects.end()) continue;

      // Draw the port
      VisualShaderInputPortGraphicsObject* p_o{new VisualShaderInputPortGraphicsObject(port_rect, n_id, i, this)};
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

  // Correct X coordinate: Remove the margin
  first_out_port_coordinate.setX((float)first_out_port_coordinate.x() + this->rect_margin);

  {
    // Draw Output Ports
    for (int i{0}; i < out_port_count; ++i) {
      QPointF port_coordinate{first_out_port_coordinate.x(), first_out_port_coordinate.y() + body_rect_port_step * i};

      QRectF port_rect(port_coordinate.x(), port_coordinate.y(), port_diameter, port_diameter);

      // Adjust the port rect to be centered
      port_rect.adjust(-port_rect.width() * 0.5f, -port_rect.height() * 0.5f, -port_rect.width() * 0.5f,
                       -port_rect.height() * 0.5f);

      // Draw caption
      QString p_n{QString::fromStdString(out_port_captions.at(i))};

      if (!p_n.isEmpty()) {
        QFont t_f{painter->font()};

        QFont f("Arial", port_caption_font_size);
        QFontMetrics fm(f);
        painter->setFont(f);

        float x{rect_x + rect_w - (float)fm.horizontalAdvance(p_n) - port_caption_spacing};

        float y{(float)(port_rect.center().y()) + (float)((fm.ascent() + fm.descent()) * 0.5f - fm.descent())};

        // {
        //   painter->setPen(Qt::red);
        //   painter->setBrush(Qt::NoBrush);
        //   painter->drawRect(QRectF(x,y, (float)fm.horizontalAdvance(p_n), (float)(fm.ascent() + fm.descent())));
        // }

        QPointF coordinate{x, y};

        painter->setPen(this->font_color);
        painter->drawText(coordinate, p_n);

        painter->setFont(t_f);  // Reset the font
      }

      if (out_port_graphics_objects.find(i) != out_port_graphics_objects.end()) continue;

      // Draw the port
      VisualShaderOutputPortGraphicsObject* p_o{new VisualShaderOutputPortGraphicsObject(port_rect, n_id, i, this)};
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
      float embed_widget_x{rect_x + rect_w * 0.5f - embed_widget->width() * 0.5f};
      float embed_widget_y{rect_y + caption_rect_height + body_rect_header_height + embed_widget_v_padding};

      embed_widget->setGeometry(embed_widget_x, embed_widget_y, embed_widget->width(), embed_widget->height());

      // {
      //   // Draw Embed Widget
      //   painter->setPen(Qt::red);
      //   painter->setBrush(Qt::NoBrush);
      //   painter->drawRect(embed_widget_x, embed_widget_y, embed_widget->width(), embed_widget->height());
      // }
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

VisualShaderInputPortGraphicsObject::VisualShaderInputPortGraphicsObject(const QRectF& rect, const int& n_id,
                                                                         const int& p_index, QGraphicsItem* parent)
    : QGraphicsObject(parent), rect(rect), n_id(n_id), p_index(p_index), connection_graphics_object(nullptr) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setCursor(Qt::PointingHandCursor);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setVisible(true);
  setOpacity(this->opacity);

  setZValue(0);
}

VisualShaderInputPortGraphicsObject::~VisualShaderInputPortGraphicsObject() {}

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

VisualShaderOutputPortGraphicsObject::VisualShaderOutputPortGraphicsObject(const QRectF& rect, const int& n_id,
                                                                           const int& p_index, QGraphicsItem* parent)
    : QGraphicsObject(parent), rect(rect), n_id(n_id), p_index(p_index) {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setCursor(Qt::PointingHandCursor);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setVisible(true);
  setOpacity(this->opacity);

  setZValue(0);
}

VisualShaderOutputPortGraphicsObject::~VisualShaderOutputPortGraphicsObject() {}

VisualShaderConnectionGraphicsObject* VisualShaderOutputPortGraphicsObject::get_connection_graphics_object(
    const int& to_node_id, const int& to_port_index) const {
  for (auto c_g_o : connection_graphics_objects) {
    if (c_g_o->get_to_node_id() == to_node_id && c_g_o->get_to_port_index() == to_port_index) {
      return c_g_o;
    }
  }
  return nullptr;
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

VisualShaderConnectionGraphicsObject::VisualShaderConnectionGraphicsObject(const int& c_id, 
                                                                           const int& from_n_id,
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
      rect_padding(0.0f) {
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setAcceptedMouseButtons(Qt::NoButton);

  setZValue(-1.0f);
}

VisualShaderConnectionGraphicsObject::~VisualShaderConnectionGraphicsObject() {}

QRectF VisualShaderConnectionGraphicsObject::boundingRect() const {
  QRectF r{calculate_bounding_rect_from_coordinates(start_coordinate, end_coordinate)};

  // Calculate the rect padding
  // We add a safe area around the rect to prevent the ports from being cut off
  // Due to inaccuracy in the calculation of the bounding rect we use the point diameter not the radius
  this->rect_padding = this->point_diameter;

  r.adjust(-rect_padding, -rect_padding, rect_padding, rect_padding);

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

    std::pair<QPointF, QPointF> control_points{calculate_control_points(start_coordinate, end_coordinate)};

    QPainterPath cubic(start_coordinate);
    cubic.cubicTo(control_points.first, control_points.second, end_coordinate);

    p.setColor(this->normal_color);

    if (selected) {
      p.setColor(this->selected_color);
    }

    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    painter->drawPath(cubic);
  }

  painter->setBrush(this->connection_point_color);

  {
    // Draw start point
    QRectF start_rect(start_coordinate.x(), start_coordinate.y(), this->point_diameter, this->point_diameter);

    // Adjust the port rect to be centered
    start_rect.adjust(-start_rect.width() * 0.5f, -start_rect.height() * 0.5f, -start_rect.width() * 0.5f,
                      -start_rect.height() * 0.5f);

    painter->drawEllipse(start_rect);
  }

  {
    // Draw end point
    QRectF end_rect(end_coordinate.x(), end_coordinate.y(), this->point_diameter, this->point_diameter);

    // Adjust the port rect to be centered
    end_rect.adjust(-end_rect.width() * 0.5f, -end_rect.height() * 0.5f, -end_rect.width() * 0.5f,
                    -end_rect.height() * 0.5f);

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

QRectF VisualShaderConnectionGraphicsObject::calculate_bounding_rect_from_coordinates(
    const QPointF& start_coordinate, const QPointF& end_coordinate) const {
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

std::pair<QPointF, QPointF> VisualShaderConnectionGraphicsObject::calculate_control_points(
    const QPointF& start_coordinate, [[maybe_unused]] const QPointF& end_coordinated) const {
  QPointF cp1;
  QPointF cp2;

  const float x1{(float)start_coordinate.x()};
  const float y1{(float)start_coordinate.y()};
  const float x2{(float)end_coordinate.x()};
  const float y2{(float)end_coordinate.y()};

  QRectF r{calculate_bounding_rect_from_coordinates(start_coordinate, end_coordinate)};

  const bool in_h_abnormal_region{x2 < (x1 + h_abnormal_offset)};
  const bool in_v_abnormal_region{std::abs(y2 - y1) < v_abnormal_offset};

  const int quadrant{detect_quadrant({x1, y1}, {x2, y2})};

  // We will expand the bounding rect horizontally so that our connection don't get cut off
  const float a_width_expansion{((x1 + h_abnormal_offset) - x2) * abnormal_face_to_back_control_width_expansion_factor};
  const float a_height_expansion{a_width_expansion * abnormal_face_to_back_control_height_expansion_factor};

  const float cp_x_delta_factor{0.8f};
  const float cp_y_delta_factor{0.25f};

  // Normal region control points deltas
  const float cp_x_delta{(float)r.width() * cp_x_delta_factor};
  const float cp_y_delta{(float)r.height() * cp_y_delta_factor};

  // Abnormal region control points deltas
  const float a_cp_x_delta{((float)r.width() - a_width_expansion) * cp_x_delta_factor};
  const float a_cp_y_delta{((float)r.height() - a_height_expansion) * cp_y_delta_factor};

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
/*****                 Embed Widgets                              *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

VisualShaderNodeEmbedWidget::VisualShaderNodeEmbedWidget(ProtoModel* visual_shader_model, 
                                                         ProtoModel* nodes_model, 
                                                         const int& n_id, 
                                                         const std::shared_ptr<IGraphNode>& graph_node, 
                                                         QWidget* parent) : QWidget(parent), 
                                                                            layout(nullptr), 
                                                                            preview_shader_button(nullptr), 
                                                                            shader_previewer_widget(nullptr) {
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(2);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  if (auto input_node = std::dynamic_pointer_cast<GraphNode<VisualShaderNodeInput>>(graph_node)) {
    VisualShaderNodeInputEmbedWidget* embed_widget = new VisualShaderNodeInputEmbedWidget(visual_shader_model, nodes_model, n_id, graph_node);
    layout->addWidget(embed_widget);
    QObject::connect(embed_widget, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                    &VisualShaderNodeEmbedWidget::on_shader_preview_update_requested);
  } else {
    FAIL_AND_RETURN("Unknown node type");
  }

  // Create the button that will show/hide the shader previewer
  preview_shader_button = new QPushButton("Show Preview", this);
  preview_shader_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  preview_shader_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  preview_shader_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  preview_shader_button->setToolTip("Create a new node");
  layout->addWidget(preview_shader_button);
  QObject::connect(preview_shader_button, &QPushButton::pressed, this,
                   &VisualShaderNodeEmbedWidget::on_preview_shader_button_pressed);

  this->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  setLayout(layout);
}

/*************************************/
/* Input Node                        */
/*************************************/

VisualShaderNodeInputEmbedWidget::VisualShaderNodeInputEmbedWidget(ProtoModel* visual_shader_model, 
                                                                   ProtoModel* nodes_model, 
                                                                   const int& n_id, 
                                                                   const std::shared_ptr<IGraphNode>& graph_node) : QComboBox(), 
                                                                                                           visual_shader_model(visual_shader_model),
                                                                                                           nodes_model(nodes_model), 
                                                                                                           n_id(n_id),
                                                                                                           graph_node(graph_node) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

  int input_type_count{graph_node->get_input_type_count()};
  for (int i{0}; i < input_type_count; ++i) {
    VisualShaderNodeInputType input_type{graph_node->get_input_type(i)};
    addItem(QString::fromStdString(graph_node->get_input_type_caption(input_type)));
  }

  int row_entry{VisualShaderGraphicsScene::find_node_entry(visual_shader_model, nodes_model, n_id)};
  setCurrentIndex(visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber), FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber))).toInt());

  QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                   &VisualShaderNodeInputEmbedWidget::on_current_index_changed);
}

VisualShaderNodeInputEmbedWidget::~VisualShaderNodeInputEmbedWidget() {}

void VisualShaderNodeInputEmbedWidget::on_current_index_changed(const int& index) {
  int row_entry{VisualShaderGraphicsScene::find_node_entry(visual_shader_model, nodes_model, n_id)};
  bool result = visual_shader_model->set_data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(row_entry), FieldPath::FieldNumber(VisualShader::VisualShaderNode::kInputFieldNumber), FieldPath::FieldNumber(VisualShaderNodeInput::kTypeFieldNumber)), (VisualShaderNodeInputType)index);
  if (!result) {
    ERROR_PRINT("Failed to set data");
  }
}

/*************************************/
/* Float Op Node                     */
/*************************************/

// VisualShaderNodeFloatOpEmbedWidget::VisualShaderNodeFloatOpEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Add", (int)VisualShaderNodeFloatOp::OP_ADD);
//   addItem("Subtract", (int)VisualShaderNodeFloatOp::OP_SUB);
//   addItem("Multiply", (int)VisualShaderNodeFloatOp::OP_MUL);
//   addItem("Divide", (int)VisualShaderNodeFloatOp::OP_DIV);
//   addItem("Modulus", (int)VisualShaderNodeFloatOp::OP_MOD);
//   addItem("Power", (int)VisualShaderNodeFloatOp::OP_POW);
//   addItem("Maximum", (int)VisualShaderNodeFloatOp::OP_MAX);
//   addItem("Minimum", (int)VisualShaderNodeFloatOp::OP_MIN);
//   addItem("Arc Tangent 2", (int)VisualShaderNodeFloatOp::OP_ATAN2);
//   addItem("Step", (int)VisualShaderNodeFloatOp::OP_STEP);

//   setCurrentIndex((int)node->get_operator());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeFloatOpEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeFloatOpEmbedWidget::~VisualShaderNodeFloatOpEmbedWidget() {}

// void VisualShaderNodeFloatOpEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_operator((VisualShaderNodeFloatOp::Operator)itemData(index).toInt());
// }

// /*************************************/
// /* Int Op Node                       */
// /*************************************/

// VisualShaderNodeIntOpEmbedWidget::VisualShaderNodeIntOpEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Add", (int)VisualShaderNodeIntOp::OP_ADD);
//   addItem("Subtract", (int)VisualShaderNodeIntOp::OP_SUB);
//   addItem("Multiply", (int)VisualShaderNodeIntOp::OP_MUL);
//   addItem("Divide", (int)VisualShaderNodeIntOp::OP_DIV);
//   addItem("Modulus", (int)VisualShaderNodeIntOp::OP_MOD);
//   addItem("Maximum", (int)VisualShaderNodeIntOp::OP_MAX);
//   addItem("Minimum", (int)VisualShaderNodeIntOp::OP_MIN);
//   addItem("Bitwise AND", (int)VisualShaderNodeIntOp::OP_BITWISE_AND);
//   addItem("Bitwise OR", (int)VisualShaderNodeIntOp::OP_BITWISE_OR);
//   addItem("Bitwise XOR", (int)VisualShaderNodeIntOp::OP_BITWISE_XOR);
//   addItem("Bitwise Left Shift", (int)VisualShaderNodeIntOp::OP_BITWISE_LEFT_SHIFT);
//   addItem("Bitwise Right Shift", (int)VisualShaderNodeIntOp::OP_BITWISE_RIGHT_SHIFT);

//   setCurrentIndex((int)node->get_operator());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeIntOpEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeIntOpEmbedWidget::~VisualShaderNodeIntOpEmbedWidget() {}

// void VisualShaderNodeIntOpEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_operator((VisualShaderNodeIntOp::Operator)itemData(index).toInt());
// }

// /*************************************/
// /* UInt Op Node                      */
// /*************************************/

// VisualShaderNodeUIntOpEmbedWidget::VisualShaderNodeUIntOpEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Add", (int)VisualShaderNodeUIntOp::OP_ADD);
//   addItem("Subtract", (int)VisualShaderNodeUIntOp::OP_SUB);
//   addItem("Multiply", (int)VisualShaderNodeUIntOp::OP_MUL);
//   addItem("Divide", (int)VisualShaderNodeUIntOp::OP_DIV);
//   addItem("Modulus", (int)VisualShaderNodeUIntOp::OP_MOD);
//   addItem("Maximum", (int)VisualShaderNodeUIntOp::OP_MAX);
//   addItem("Minimum", (int)VisualShaderNodeUIntOp::OP_MIN);
//   addItem("Bitwise AND", (int)VisualShaderNodeUIntOp::OP_BITWISE_AND);
//   addItem("Bitwise OR", (int)VisualShaderNodeUIntOp::OP_BITWISE_OR);
//   addItem("Bitwise XOR", (int)VisualShaderNodeUIntOp::OP_BITWISE_XOR);
//   addItem("Bitwise Left Shift", (int)VisualShaderNodeUIntOp::OP_BITWISE_LEFT_SHIFT);
//   addItem("Bitwise Right Shift", (int)VisualShaderNodeUIntOp::OP_BITWISE_RIGHT_SHIFT);

//   setCurrentIndex((int)node->get_operator());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeUIntOpEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeUIntOpEmbedWidget::~VisualShaderNodeUIntOpEmbedWidget() {}

// void VisualShaderNodeUIntOpEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_operator((VisualShaderNodeUIntOp::Operator)itemData(index).toInt());
// }

// /*************************************/
// /* Float Funcs Node                  */
// /*************************************/

// VisualShaderNodeFloatFuncEmbedWidget::VisualShaderNodeFloatFuncEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Sin", (int)VisualShaderNodeFloatFunc::FUNC_SIN);
//   addItem("Cos", (int)VisualShaderNodeFloatFunc::FUNC_COS);
//   addItem("Tan", (int)VisualShaderNodeFloatFunc::FUNC_TAN);
//   addItem("Arc Sine", (int)VisualShaderNodeFloatFunc::FUNC_ASIN);
//   addItem("Arc Cosine", (int)VisualShaderNodeFloatFunc::FUNC_ACOS);
//   addItem("Arc Tangent", (int)VisualShaderNodeFloatFunc::FUNC_ATAN);
//   addItem("Sine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_SINH);
//   addItem("Cosine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_COSH);
//   addItem("Tangent Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_TANH);
//   addItem("Logarithm", (int)VisualShaderNodeFloatFunc::FUNC_LOG);
//   addItem("Exponential", (int)VisualShaderNodeFloatFunc::FUNC_EXP);
//   addItem("Square Root", (int)VisualShaderNodeFloatFunc::FUNC_SQRT);
//   addItem("Absolute", (int)VisualShaderNodeFloatFunc::FUNC_ABS);
//   addItem("Sign", (int)VisualShaderNodeFloatFunc::FUNC_SIGN);
//   addItem("Floor", (int)VisualShaderNodeFloatFunc::FUNC_FLOOR);
//   addItem("Round", (int)VisualShaderNodeFloatFunc::FUNC_ROUND);
//   addItem("Ceil", (int)VisualShaderNodeFloatFunc::FUNC_CEIL);
//   addItem("Fraction", (int)VisualShaderNodeFloatFunc::FUNC_FRACT);
//   addItem("Saturate", (int)VisualShaderNodeFloatFunc::FUNC_SATURATE);
//   addItem("Negate", (int)VisualShaderNodeFloatFunc::FUNC_NEGATE);
//   addItem("Arc Cosine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_ACOSH);
//   addItem("Arc Sine Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_ASINH);
//   addItem("Arc Tangent Hyperbolic", (int)VisualShaderNodeFloatFunc::FUNC_ATANH);
//   addItem("Degrees", (int)VisualShaderNodeFloatFunc::FUNC_DEGREES);
//   addItem("Exponential 2", (int)VisualShaderNodeFloatFunc::FUNC_EXP2);
//   addItem("Inverse Square Root", (int)VisualShaderNodeFloatFunc::FUNC_INVERSE_SQRT);
//   addItem("Logarithm 2", (int)VisualShaderNodeFloatFunc::FUNC_LOG2);
//   addItem("Radians", (int)VisualShaderNodeFloatFunc::FUNC_RADIANS);
//   addItem("Reciprocal", (int)VisualShaderNodeFloatFunc::FUNC_RECIPROCAL);
//   addItem("Round Even", (int)VisualShaderNodeFloatFunc::FUNC_ROUNDEVEN);
//   addItem("Truncate", (int)VisualShaderNodeFloatFunc::FUNC_TRUNC);
//   addItem("One Minus", (int)VisualShaderNodeFloatFunc::FUNC_ONEMINUS);

//   setCurrentIndex((int)node->get_function());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeFloatFuncEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeFloatFuncEmbedWidget::~VisualShaderNodeFloatFuncEmbedWidget() {}

// void VisualShaderNodeFloatFuncEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeFloatFunc::Function)itemData(index).toInt());
// }

// /*************************************/
// /* Int Funcs Node                    */
// /*************************************/

// VisualShaderNodeIntFuncEmbedWidget::VisualShaderNodeIntFuncEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Abs", (int)VisualShaderNodeIntFunc::FUNC_ABS);
//   addItem("Negate", (int)VisualShaderNodeIntFunc::FUNC_NEGATE);
//   addItem("Sign", (int)VisualShaderNodeIntFunc::FUNC_SIGN);
//   addItem("Bitwise NOT", (int)VisualShaderNodeIntFunc::FUNC_BITWISE_NOT);

//   setCurrentIndex((int)node->get_function());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeIntFuncEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeIntFuncEmbedWidget::~VisualShaderNodeIntFuncEmbedWidget() {}

// void VisualShaderNodeIntFuncEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeIntFunc::Function)itemData(index).toInt());
// }

// /*************************************/
// /* UInt Funcs Node                   */
// /*************************************/

// VisualShaderNodeUIntFuncEmbedWidget::VisualShaderNodeUIntFuncEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Negate", (int)VisualShaderNodeUIntFunc::FUNC_NEGATE);
//   addItem("Bitwise NOT", (int)VisualShaderNodeUIntFunc::FUNC_BITWISE_NOT);

//   setCurrentIndex((int)node->get_function());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeUIntFuncEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeUIntFuncEmbedWidget::~VisualShaderNodeUIntFuncEmbedWidget() {}

// void VisualShaderNodeUIntFuncEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeUIntFunc::Function)itemData(index).toInt());
// }

// /*************************************/
// /* Vector Base                       */
// /*************************************/

// VisualShaderNodeVectorBaseEmbedWidget::VisualShaderNodeVectorBaseEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Vector 2D", (int)VisualShaderNodeVectorBase::OP_TYPE_VECTOR_2D);
//   addItem("Vector 3D", (int)VisualShaderNodeVectorBase::OP_TYPE_VECTOR_3D);
//   addItem("Vector 4D", (int)VisualShaderNodeVectorBase::OP_TYPE_VECTOR_4D);

//   // set the current index
//   setCurrentIndex((int)node->get_op_type());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeVectorBaseEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeVectorBaseEmbedWidget::~VisualShaderNodeVectorBaseEmbedWidget() {}

// void VisualShaderNodeVectorBaseEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_op_type((VisualShaderNodeVectorBase::OpType)itemData(index).toInt());
// }

// /*************************************/
// /* Vector Op Node                    */
// /*************************************/

// VisualShaderNodeVectorOpEmbedWidget::VisualShaderNodeVectorOpEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Add", (int)VisualShaderNodeVectorOp::OP_ADD);
//   addItem("Subtract", (int)VisualShaderNodeVectorOp::OP_SUB);
//   addItem("Multiply", (int)VisualShaderNodeVectorOp::OP_MUL);
//   addItem("Divide", (int)VisualShaderNodeVectorOp::OP_DIV);
//   addItem("Modulus", (int)VisualShaderNodeVectorOp::OP_MOD);
//   addItem("Power", (int)VisualShaderNodeVectorOp::OP_POW);
//   addItem("Maximum", (int)VisualShaderNodeVectorOp::OP_MAX);
//   addItem("Minimum", (int)VisualShaderNodeVectorOp::OP_MIN);
//   addItem("Cross Product", (int)VisualShaderNodeVectorOp::OP_CROSS);
//   addItem("Arc Tangent 2", (int)VisualShaderNodeVectorOp::OP_ATAN2);
//   addItem("Reflect", (int)VisualShaderNodeVectorOp::OP_REFLECT);
//   addItem("Step", (int)VisualShaderNodeVectorOp::OP_STEP);

//   setCurrentIndex((int)node->get_operator());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeVectorOpEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeVectorOpEmbedWidget::~VisualShaderNodeVectorOpEmbedWidget() {}

// void VisualShaderNodeVectorOpEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_operator((VisualShaderNodeVectorOp::Operator)itemData(index).toInt());
// }

// /*************************************/
// /* Vector Funcs Node                 */
// /*************************************/

// VisualShaderNodeVectorFuncEmbedWidget::VisualShaderNodeVectorFuncEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Normalize", (int)VisualShaderNodeVectorFunc::FUNC_NORMALIZE);
//   addItem("Saturate", (int)VisualShaderNodeVectorFunc::FUNC_SATURATE);
//   addItem("Negate", (int)VisualShaderNodeVectorFunc::FUNC_NEGATE);
//   addItem("Reciprocal", (int)VisualShaderNodeVectorFunc::FUNC_RECIPROCAL);
//   addItem("Abs", (int)VisualShaderNodeVectorFunc::FUNC_ABS);
//   addItem("Arc Cosine", (int)VisualShaderNodeVectorFunc::FUNC_ACOS);
//   addItem("Arc Cosine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_ACOSH);
//   addItem("Arc Sine", (int)VisualShaderNodeVectorFunc::FUNC_ASIN);
//   addItem("Arc Sine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_ASINH);
//   addItem("Arc Tangent", (int)VisualShaderNodeVectorFunc::FUNC_ATAN);
//   addItem("Arc Tangent Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_ATANH);
//   addItem("Ceil", (int)VisualShaderNodeVectorFunc::FUNC_CEIL);
//   addItem("Cos", (int)VisualShaderNodeVectorFunc::FUNC_COS);
//   addItem("Cosine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_COSH);
//   addItem("Degrees", (int)VisualShaderNodeVectorFunc::FUNC_DEGREES);
//   addItem("Exp", (int)VisualShaderNodeVectorFunc::FUNC_EXP);
//   addItem("Exp2", (int)VisualShaderNodeVectorFunc::FUNC_EXP2);
//   addItem("Floor", (int)VisualShaderNodeVectorFunc::FUNC_FLOOR);
//   addItem("Fraction", (int)VisualShaderNodeVectorFunc::FUNC_FRACT);
//   addItem("Inverse Square Root", (int)VisualShaderNodeVectorFunc::FUNC_INVERSE_SQRT);
//   addItem("Log", (int)VisualShaderNodeVectorFunc::FUNC_LOG);
//   addItem("Log2", (int)VisualShaderNodeVectorFunc::FUNC_LOG2);
//   addItem("Radians", (int)VisualShaderNodeVectorFunc::FUNC_RADIANS);
//   addItem("Round", (int)VisualShaderNodeVectorFunc::FUNC_ROUND);
//   addItem("Round Even", (int)VisualShaderNodeVectorFunc::FUNC_ROUNDEVEN);
//   addItem("Sign", (int)VisualShaderNodeVectorFunc::FUNC_SIGN);
//   addItem("Sin", (int)VisualShaderNodeVectorFunc::FUNC_SIN);
//   addItem("Sine Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_SINH);
//   addItem("Sqrt", (int)VisualShaderNodeVectorFunc::FUNC_SQRT);
//   addItem("Tan", (int)VisualShaderNodeVectorFunc::FUNC_TAN);
//   addItem("Tangent Hyperbolic", (int)VisualShaderNodeVectorFunc::FUNC_TANH);
//   addItem("Truncate", (int)VisualShaderNodeVectorFunc::FUNC_TRUNC);
//   addItem("One Minus", (int)VisualShaderNodeVectorFunc::FUNC_ONEMINUS);

//   setCurrentIndex((int)node->get_function());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeVectorFuncEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeVectorFuncEmbedWidget::~VisualShaderNodeVectorFuncEmbedWidget() {}

// void VisualShaderNodeVectorFuncEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeVectorFunc::Function)itemData(index).toInt());
// }

// /*************************************/
// /* Color Constant Node               */
// /*************************************/

// VisualShaderNodeColorConstantEmbedWidget::VisualShaderNodeColorConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QPushButton(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   {
//     QPalette palette{this->palette()};
//     Color c{node->get_constant()};
//     palette.setColor(QPalette::Button, QColor(c.r, c.g, c.b, c.a));
//     this->setPalette(palette);
//   }

//   QObject::connect(this, &QPushButton::pressed, this, &VisualShaderNodeColorConstantEmbedWidget::on_pressed);
// }

// VisualShaderNodeColorConstantEmbedWidget::~VisualShaderNodeColorConstantEmbedWidget() {}

// void VisualShaderNodeColorConstantEmbedWidget::on_pressed() {
//   Color c{node->get_constant()};
//   QColor color{QColorDialog::getColor(QColor(c.r, c.g, c.b, c.a), this, "Select Color")};

//   // If a valid color is picked, update the button and store the color
//   if (color.isValid()) {
//     node->set_constant({(float)color.red(), (float)color.green(), (float)color.blue(), (float)color.alpha()});
//     QPalette palette{this->palette()};
//     palette.setColor(QPalette::Button, color);
//     this->setPalette(palette);
//     this->update();
//     Q_EMIT color_changed();
//   } else {
//     // If the user cancels the color dialog, reset the button to the previous color
//     QColor previous_color{QColor(c.r, c.g, c.b, c.a)};
//     QPalette palette{this->palette()};
//     palette.setColor(QPalette::Button, previous_color);
//     this->setPalette(palette);
//     this->update();
//   }
// }

// /*************************************/
// /* Boolean Constant Node             */
// /*************************************/

// VisualShaderNodeBooleanConstantEmbedWidget::VisualShaderNodeBooleanConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QCheckBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setCheckState(node->get_constant() ? Qt::Checked : Qt::Unchecked);

//   QObject::connect(this, &QCheckBox::stateChanged, this, &VisualShaderNodeBooleanConstantEmbedWidget::on_state_changed);
// }

// VisualShaderNodeBooleanConstantEmbedWidget::~VisualShaderNodeBooleanConstantEmbedWidget() {}

// void VisualShaderNodeBooleanConstantEmbedWidget::on_state_changed(const int& state) {
//   node->set_constant(state == Qt::Checked);
// }

// /*************************************/
// /* Float Constant                    */
// /*************************************/

// VisualShaderNodeFloatConstantEmbedWidget::VisualShaderNodeFloatConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Value");

//   setText(QString::number(node->get_constant()));

//   QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeFloatConstantEmbedWidget::on_text_changed);
// }

// VisualShaderNodeFloatConstantEmbedWidget::~VisualShaderNodeFloatConstantEmbedWidget() {}

// void VisualShaderNodeFloatConstantEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant(0.0f);
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant(text.toFloat());
//     } else {
//       node->set_constant(0.0f);
//       setText("");
//     }
//   }
// }

// /*************************************/
// /* Int Constant                      */
// /*************************************/

// VisualShaderNodeIntConstantEmbedWidget::VisualShaderNodeIntConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Value");

//   setText(QString::number(node->get_constant()));

//   QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeIntConstantEmbedWidget::on_text_changed);
// }

// VisualShaderNodeIntConstantEmbedWidget::~VisualShaderNodeIntConstantEmbedWidget() {}

// void VisualShaderNodeIntConstantEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant(0);
//   } else {
//     bool ok;
//     text.toInt(&ok);
//     if (ok) {
//       node->set_constant(text.toInt());
//     } else {
//       node->set_constant(0);
//       setText("");
//     }
//   }
// }

// /*************************************/
// /* UInt Constant                     */
// /*************************************/

// VisualShaderNodeUIntConstantEmbedWidget::VisualShaderNodeUIntConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Value");

//   setText(QString::number(node->get_constant()));

//   QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeUIntConstantEmbedWidget::on_text_changed);
// }

// VisualShaderNodeUIntConstantEmbedWidget::~VisualShaderNodeUIntConstantEmbedWidget() {}

// void VisualShaderNodeUIntConstantEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant(0);
//   } else {
//     bool ok;
//     text.toUInt(&ok);
//     if (ok) {
//       node->set_constant(text.toUInt());
//     } else {
//       node->set_constant(0);
//       setText("");
//     }
//   }
// }

// /*************************************/
// /* Vec2 Constant Node                */
// /*************************************/

// VisualShaderNodeVec2ConstantEmbedWidget::VisualShaderNodeVec2ConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QVBoxLayout(), graph_node(graph_node) {
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
//   setSizeConstraint(QLayout::SetNoConstraint);
//   setSpacing(2);
//   setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

//   x_edit_widget = new QLineEdit();
//   y_edit_widget = new QLineEdit();

//   x_edit_widget->setPlaceholderText("X");
//   y_edit_widget->setPlaceholderText("Y");

//   x_edit_widget->setText(QString::number(node->get_constant().x));
//   y_edit_widget->setText(QString::number(node->get_constant().y));

//   QObject::connect(x_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec2ConstantEmbedWidget::on_x_text_changed);
//   QObject::connect(y_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec2ConstantEmbedWidget::on_y_text_changed);

//   addWidget(x_edit_widget);
//   addWidget(y_edit_widget);
// }

// VisualShaderNodeVec2ConstantEmbedWidget::~VisualShaderNodeVec2ConstantEmbedWidget() {}

// void VisualShaderNodeVec2ConstantEmbedWidget::on_x_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({0.0f, node->get_constant().y});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({text.toFloat(), node->get_constant().y});
//     } else {
//       node->set_constant({0.0f, node->get_constant().y});
//       x_edit_widget->setText("");
//     }
//   }
// }

// void VisualShaderNodeVec2ConstantEmbedWidget::on_y_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({node->get_constant().x, 0.0f});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({node->get_constant().x, text.toFloat()});
//     } else {
//       node->set_constant({node->get_constant().x, 0.0f});
//       y_edit_widget->setText("");
//     }
//   }
// }

// /*************************************/
// /* Vec3 Constant Node                */
// /*************************************/

// VisualShaderNodeVec3ConstantEmbedWidget::VisualShaderNodeVec3ConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QVBoxLayout(), graph_node(graph_node) {
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
//   setSizeConstraint(QLayout::SetNoConstraint);
//   setSpacing(2);
//   setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

//   x_edit_widget = new QLineEdit();
//   y_edit_widget = new QLineEdit();
//   z_edit_widget = new QLineEdit();

//   x_edit_widget->setPlaceholderText("X");
//   y_edit_widget->setPlaceholderText("Y");
//   z_edit_widget->setPlaceholderText("Z");

//   x_edit_widget->setText(QString::number(node->get_constant().x));
//   y_edit_widget->setText(QString::number(node->get_constant().y));
//   z_edit_widget->setText(QString::number(node->get_constant().z));

//   QObject::connect(x_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec3ConstantEmbedWidget::on_x_text_changed);
//   QObject::connect(y_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec3ConstantEmbedWidget::on_y_text_changed);
//   QObject::connect(z_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec3ConstantEmbedWidget::on_z_text_changed);

//   addWidget(x_edit_widget);
//   addWidget(y_edit_widget);
//   addWidget(z_edit_widget);
// }

// VisualShaderNodeVec3ConstantEmbedWidget::~VisualShaderNodeVec3ConstantEmbedWidget() {}

// void VisualShaderNodeVec3ConstantEmbedWidget::on_x_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({0.0f, node->get_constant().y, node->get_constant().z});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({text.toFloat(), node->get_constant().y, node->get_constant().z});
//     } else {
//       node->set_constant({0.0f, node->get_constant().y, node->get_constant().z});
//       x_edit_widget->setText("");
//     }
//   }
// }

// void VisualShaderNodeVec3ConstantEmbedWidget::on_y_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({node->get_constant().x, 0.0f, node->get_constant().z});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({node->get_constant().x, text.toFloat(), node->get_constant().z});
//     } else {
//       node->set_constant({node->get_constant().x, 0.0f, node->get_constant().z});
//       y_edit_widget->setText("");
//     }
//   }
// }

// void VisualShaderNodeVec3ConstantEmbedWidget::on_z_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({node->get_constant().x, node->get_constant().y, 0.0f});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({node->get_constant().x, node->get_constant().y, text.toFloat()});
//     } else {
//       node->set_constant({node->get_constant().x, node->get_constant().y, 0.0f});
//       z_edit_widget->setText("");
//     }
//   }
// }

// /*************************************/
// /* Vec4 Constant Node                */
// /*************************************/

// VisualShaderNodeVec4ConstantEmbedWidget::VisualShaderNodeVec4ConstantEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QVBoxLayout(), graph_node(graph_node) {
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
//   setSizeConstraint(QLayout::SetNoConstraint);
//   setSpacing(2);
//   setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

//   x_edit_widget = new QLineEdit();
//   y_edit_widget = new QLineEdit();
//   z_edit_widget = new QLineEdit();
//   w_edit_widget = new QLineEdit();

//   x_edit_widget->setPlaceholderText("X");
//   y_edit_widget->setPlaceholderText("Y");
//   z_edit_widget->setPlaceholderText("Z");
//   w_edit_widget->setPlaceholderText("W");

//   x_edit_widget->setText(QString::number(node->get_constant().x));
//   y_edit_widget->setText(QString::number(node->get_constant().y));
//   z_edit_widget->setText(QString::number(node->get_constant().z));
//   w_edit_widget->setText(QString::number(node->get_constant().w));

//   QObject::connect(x_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec4ConstantEmbedWidget::on_x_text_changed);
//   QObject::connect(y_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec4ConstantEmbedWidget::on_y_text_changed);
//   QObject::connect(z_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec4ConstantEmbedWidget::on_z_text_changed);
//   QObject::connect(w_edit_widget, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVec4ConstantEmbedWidget::on_w_text_changed);

//   addWidget(x_edit_widget);
//   addWidget(y_edit_widget);
//   addWidget(z_edit_widget);
//   addWidget(w_edit_widget);
// }

// VisualShaderNodeVec4ConstantEmbedWidget::~VisualShaderNodeVec4ConstantEmbedWidget() {}

// void VisualShaderNodeVec4ConstantEmbedWidget::on_x_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({0.0f, node->get_constant().y, node->get_constant().z, node->get_constant().w});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({text.toFloat(), node->get_constant().y, node->get_constant().z, node->get_constant().w});
//     } else {
//       node->set_constant({0.0f, node->get_constant().y, node->get_constant().z, node->get_constant().w});
//       x_edit_widget->setText("");
//     }
//   }
// }

// void VisualShaderNodeVec4ConstantEmbedWidget::on_y_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({node->get_constant().x, 0.0f, node->get_constant().z, node->get_constant().w});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({node->get_constant().x, text.toFloat(), node->get_constant().z, node->get_constant().w});
//     } else {
//       node->set_constant({node->get_constant().x, 0.0f, node->get_constant().z, node->get_constant().w});
//       y_edit_widget->setText("");
//     }
//   }
// }

// void VisualShaderNodeVec4ConstantEmbedWidget::on_z_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({node->get_constant().x, node->get_constant().y, 0.0f, node->get_constant().w});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({node->get_constant().x, node->get_constant().y, text.toFloat(), node->get_constant().w});
//     } else {
//       node->set_constant({node->get_constant().x, node->get_constant().y, 0.0f, node->get_constant().w});
//       z_edit_widget->setText("");
//     }
//   }
// }

// void VisualShaderNodeVec4ConstantEmbedWidget::on_w_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_constant({node->get_constant().x, node->get_constant().y, node->get_constant().z, 0.0f});
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_constant({node->get_constant().x, node->get_constant().y, node->get_constant().z, text.toFloat()});
//     } else {
//       node->set_constant({node->get_constant().x, node->get_constant().y, node->get_constant().z, 0.0f});
//       w_edit_widget->setText("");
//     }
//   }
// }

// /*************************************/
// /* Derivative Func Node              */
// /*************************************/

// VisualShaderNodeDerivativeFuncEmbedWidget::VisualShaderNodeDerivativeFuncEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QVBoxLayout(), graph_node(graph_node) {
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
//   setSizeConstraint(QLayout::SetNoConstraint);
//   setSpacing(2);
//   setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

//   op_type_combo_box = new QComboBox();
//   function_combo_box = new QComboBox();
//   precision_combo_box = new QComboBox();

//   op_type_combo_box->addItem("Scalar", (int)VisualShaderNodeDerivativeFunc::OP_TYPE_SCALAR);
//   op_type_combo_box->addItem("Vector 2D", (int)VisualShaderNodeDerivativeFunc::OP_TYPE_VECTOR_2D);
//   op_type_combo_box->addItem("Vector 3D", (int)VisualShaderNodeDerivativeFunc::OP_TYPE_VECTOR_3D);
//   op_type_combo_box->addItem("Vector 4D", (int)VisualShaderNodeDerivativeFunc::OP_TYPE_VECTOR_4D);

//   op_type_combo_box->setCurrentIndex((int)node->get_op_type());

//   function_combo_box->addItem("Sum", (int)VisualShaderNodeDerivativeFunc::FUNC_SUM);
//   function_combo_box->addItem("X", (int)VisualShaderNodeDerivativeFunc::FUNC_X);
//   function_combo_box->addItem("Y", (int)VisualShaderNodeDerivativeFunc::FUNC_Y);

//   function_combo_box->setCurrentIndex((int)node->get_function());

//   precision_combo_box->addItem("None", (int)VisualShaderNodeDerivativeFunc::PRECISION_NONE);
//   precision_combo_box->addItem("Coarse", (int)VisualShaderNodeDerivativeFunc::PRECISION_COARSE);
//   precision_combo_box->addItem("Fine", (int)VisualShaderNodeDerivativeFunc::PRECISION_FINE);

//   precision_combo_box->setCurrentIndex((int)node->get_precision());

//   QObject::connect(op_type_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeDerivativeFuncEmbedWidget::on_op_type_current_index_changed);
//   QObject::connect(function_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeDerivativeFuncEmbedWidget::on_function_current_index_changed);
//   QObject::connect(precision_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeDerivativeFuncEmbedWidget::on_precision_current_index_changed);

//   addWidget(op_type_combo_box);
//   addWidget(function_combo_box);
//   addWidget(precision_combo_box);
// }

// VisualShaderNodeDerivativeFuncEmbedWidget::~VisualShaderNodeDerivativeFuncEmbedWidget() {}

// void VisualShaderNodeDerivativeFuncEmbedWidget::on_op_type_current_index_changed(const int& index) {
//   node->set_op_type((VisualShaderNodeDerivativeFunc::OpType)op_type_combo_box->itemData(index).toInt());
// }

// void VisualShaderNodeDerivativeFuncEmbedWidget::on_function_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeDerivativeFunc::Function)function_combo_box->itemData(index).toInt());
// }

// void VisualShaderNodeDerivativeFuncEmbedWidget::on_precision_current_index_changed(const int& index) {
//   node->set_precision((VisualShaderNodeDerivativeFunc::Precision)precision_combo_box->itemData(index).toInt());
// }

// /*************************************/
// /* Value Noise Node                  */
// /*************************************/

// VisualShaderNodeValueNoiseEmbedWidget::VisualShaderNodeValueNoiseEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Scale");

//   setText(QString::number(node->get_scale()));

//   QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodeValueNoiseEmbedWidget::on_text_changed);
// }

// VisualShaderNodeValueNoiseEmbedWidget::~VisualShaderNodeValueNoiseEmbedWidget() {}

// void VisualShaderNodeValueNoiseEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_scale(100.0f);
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_scale(text.toFloat());
//     } else {
//       node->set_scale(0.0f);
//       setText("");
//     }
//   }
// }

// /*************************************/
// /* Perlin Noise Node                 */
// /*************************************/

// VisualShaderNodePerlinNoiseEmbedWidget::VisualShaderNodePerlinNoiseEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Scale");

//   setText(QString::number(node->get_scale()));

//   QObject::connect(this, &QLineEdit::textChanged, this, &VisualShaderNodePerlinNoiseEmbedWidget::on_text_changed);
// }

// VisualShaderNodePerlinNoiseEmbedWidget::~VisualShaderNodePerlinNoiseEmbedWidget() {}

// void VisualShaderNodePerlinNoiseEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_scale(10.0f);
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_scale(text.toFloat());
//     } else {
//       node->set_scale(0.0f);
//       setText("");
//     }
//   }
// }

// /*************************************/
// /* Voronoi Noise Node                */
// /*************************************/

// VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget::VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Angle Offset");

//   setText(QString::number(node->get_angle_offset()));

//   QObject::connect(this, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget::on_text_changed);
// }

// VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget::~VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget() {}

// void VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_angle_offset(10.0f);
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_angle_offset(text.toFloat());
//     } else {
//       node->set_angle_offset(0.0f);
//       setText("");
//     }
//   }
// }

// VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget::VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QLineEdit(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   setPlaceholderText("Cell Density");

//   setText(QString::number(node->get_cell_density()));

//   QObject::connect(this, &QLineEdit::textChanged, this,
//                    &VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget::on_text_changed);
// }

// VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget::~VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget() {}

// void VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget::on_text_changed(const QString& text) {
//   if (text.isEmpty()) {
//     node->set_cell_density(10.0f);
//   } else {
//     bool ok;
//     text.toFloat(&ok);
//     if (ok) {
//       node->set_cell_density(text.toFloat());
//     } else {
//       node->set_cell_density(0.0f);
//       setText("");
//     }
//   }
// }

// /*************************************/
// /* Logic                             */
// /*************************************/

// /*************************************/
// /* Compare Node                      */
// /*************************************/

// VisualShaderNodeCompareEmbedWidget::VisualShaderNodeCompareEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QVBoxLayout(), graph_node(graph_node) {
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
//   setSizeConstraint(QLayout::SetNoConstraint);
//   setSpacing(2);
//   setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

//   comparison_type_combo_box = new QComboBox();
//   func_combo_box = new QComboBox();
//   condition_combo_box = new QComboBox();

//   comparison_type_combo_box->addItem("Scalar", (int)VisualShaderNodeCompare::CMP_TYPE_SCALAR);
//   comparison_type_combo_box->addItem("Scalar Int", (int)VisualShaderNodeCompare::CMP_TYPE_SCALAR_INT);
//   comparison_type_combo_box->addItem("Scalar UInt", (int)VisualShaderNodeCompare::CMP_TYPE_SCALAR_UINT);
//   comparison_type_combo_box->addItem("Vector 2D", (int)VisualShaderNodeCompare::CMP_TYPE_VECTOR_2D);
//   comparison_type_combo_box->addItem("Vector 3D", (int)VisualShaderNodeCompare::CMP_TYPE_VECTOR_3D);
//   comparison_type_combo_box->addItem("Vector 4D", (int)VisualShaderNodeCompare::CMP_TYPE_VECTOR_4D);
//   comparison_type_combo_box->addItem("Boolean", (int)VisualShaderNodeCompare::CMP_TYPE_BOOLEAN);

//   comparison_type_combo_box->setCurrentIndex((int)node->get_comparison_type());

//   func_combo_box->addItem("==", (int)VisualShaderNodeCompare::FUNC_EQUAL);
//   func_combo_box->addItem("!=", (int)VisualShaderNodeCompare::FUNC_NOT_EQUAL);
//   func_combo_box->addItem(">", (int)VisualShaderNodeCompare::FUNC_GREATER_THAN);
//   func_combo_box->addItem(">=", (int)VisualShaderNodeCompare::FUNC_GREATER_THAN_EQUAL);
//   func_combo_box->addItem("<", (int)VisualShaderNodeCompare::FUNC_LESS_THAN);
//   func_combo_box->addItem("<=", (int)VisualShaderNodeCompare::FUNC_LESS_THAN_EQUAL);

//   func_combo_box->setCurrentIndex((int)node->get_function());

//   condition_combo_box->addItem("All", (int)VisualShaderNodeCompare::COND_ALL);
//   condition_combo_box->addItem("Any", (int)VisualShaderNodeCompare::COND_ANY);

//   condition_combo_box->setCurrentIndex((int)node->get_condition());

//   addWidget(comparison_type_combo_box);
//   addWidget(func_combo_box);
//   addWidget(condition_combo_box);
// }

// VisualShaderNodeCompareEmbedWidget::~VisualShaderNodeCompareEmbedWidget() {}

// void VisualShaderNodeCompareEmbedWidget::on_comparison_type_current_index_changed(const int& index) {
//   node->set_comparison_type(
//       (VisualShaderNodeCompare::ComparisonType)comparison_type_combo_box->itemData(index).toInt());
// }
// void VisualShaderNodeCompareEmbedWidget::on_func_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeCompare::Function)func_combo_box->itemData(index).toInt());
// }
// void VisualShaderNodeCompareEmbedWidget::on_condition_current_index_changed(const int& index) {
//   node->set_condition((VisualShaderNodeCompare::Condition)condition_combo_box->itemData(index).toInt());
// }

// /*************************************/
// /* Switch Node                       */
// /*************************************/

// VisualShaderNodeSwitchEmbedWidget::VisualShaderNodeSwitchEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Float", (int)VisualShaderNodeSwitch::OP_TYPE_FLOAT);
//   addItem("Int", (int)VisualShaderNodeSwitch::OP_TYPE_INT);
//   addItem("UInt", (int)VisualShaderNodeSwitch::OP_TYPE_UINT);
//   addItem("Vector 2D", (int)VisualShaderNodeSwitch::OP_TYPE_VECTOR_2D);
//   addItem("Vector 3D", (int)VisualShaderNodeSwitch::OP_TYPE_VECTOR_3D);
//   addItem("Vector 4D", (int)VisualShaderNodeSwitch::OP_TYPE_VECTOR_4D);
//   addItem("Boolean", (int)VisualShaderNodeSwitch::OP_TYPE_BOOLEAN);

//   setCurrentIndex((int)node->get_op_type());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeSwitchEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeSwitchEmbedWidget::~VisualShaderNodeSwitchEmbedWidget() {}

// void VisualShaderNodeSwitchEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_op_type((VisualShaderNodeSwitch::OpType)itemData(index).toInt());
// }

// /*************************************/
// /* Is Node                           */
// /*************************************/

// VisualShaderNodeIsEmbedWidget::VisualShaderNodeIsEmbedWidget(const std::shared_ptr<IGraphNode>& graph_node)
//     : QComboBox(), graph_node(graph_node) {
//   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//   setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom

//   addItem("Is Inf", (int)VisualShaderNodeIs::FUNC_IS_INF);
//   addItem("Is NaN", (int)VisualShaderNodeIs::FUNC_IS_NAN);

//   setCurrentIndex((int)node->get_function());

//   QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
//                    &VisualShaderNodeIsEmbedWidget::on_current_index_changed);
// }

// VisualShaderNodeIsEmbedWidget::~VisualShaderNodeIsEmbedWidget() {}

// void VisualShaderNodeIsEmbedWidget::on_current_index_changed(const int& index) {
//   node->set_function((VisualShaderNodeIs::Function)itemData(index).toInt());
// }
