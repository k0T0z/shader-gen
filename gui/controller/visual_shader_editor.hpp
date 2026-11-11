/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2026 Seif Kandil (k0T0z)                                       */
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

#ifndef ENIGMA_VISUAL_SHADER_EDITOR_HPP
#define ENIGMA_VISUAL_SHADER_EDITOR_HPP

#include <QContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSpinBox>
// #include <QOpenGLFunctions>
#include <QCheckBox>
#include <QColorDialog>
#include <QElapsedTimer>
#include <QtOpenGL/QOpenGLFunctions_4_3_Core>  // https://stackoverflow.com/a/64288966/14629018 explains why we need this.
#include <QtOpenGL/QOpenGLShaderProgram>

#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

// #include "generator/visual_shader.hpp"
// #include "generator/visual_shader_nodes.hpp"
// #include "generator/vs_noise_nodes.hpp"

#include "gui/model/message_model.hpp"
#include "gui/model/repeated_message_model.hpp"

#include "gui/controller/vs_proto_node.hpp"

#include "gui/controller/renderer/renderer.hpp"

#include "gui/controller/ai_agent_worker.hpp"
#include "ai-agent/shared_memory.hpp"
#include "gui/controller/ai_agent_monitor.hpp"
#include "gui/controller/parameters_editor.hpp"
#include "generator/visual_shader_node_port_type_generator.hpp"

using EnumDescriptor = google::protobuf::EnumDescriptor;

class StartMatchingButton;
class StopMatchingButton;
class VisualShaderGraphicsScene;
class VisualShaderGraphicsView;
class VisualShaderNodeGraphicsObject;
class VisualShaderConnectionGraphicsObject;
class CreateNodeDialog;
class VisualShaderInputPortGraphicsObject;
class VisualShaderOutputPortGraphicsObject;
class VisualShaderNodeEmbedWidget;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderEditor                           *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

// Add const to any function that does not modify the object.

class VisualShaderEditor : public QWidget {
  Q_OBJECT

 public:
  enum {
    VS_PROTO_NODE_SHARED_PTR_USER_ROLE = Qt::UserRole + 1,
  };

  VisualShaderEditor(MessageModel* model, QWidget* parent = nullptr);
  ~VisualShaderEditor() override;

  VisualShaderGraphicsScene* get_scene() const { return scene; }
  VisualShaderGraphicsView* get_view() const { return view; }

 Q_SIGNALS:
  /**
   * @brief Request the dialog that has all kinds of nodes we can
   *        create.
   * 
   * @note This signal is emitted from two sources: 
   *       @c VisualShaderEditor::on_create_node_button_pressed and
   *       @c VisualShaderGraphicsView::on_create_node_action_triggered slots 
   *       and it is connected to the @c VisualShaderEditor::show_create_node_dialog 
   *       function.
   * 
   * @param coordinate 
   */
  void create_node_dialog_requested(const QPointF& coordinate);

 private Q_SLOTS:
  /**
   * @brief Called when @c VisualShaderEditor::create_node_button is pressed.
   * 
   * @note Connected in @c VisualShaderEditor::init function
   *       to @c QPushButton::pressed signal.
   * 
   * @note EMITS @c VisualShaderEditor::create_node_dialog_requested signal.
   * 
   */
  void on_create_node_button_pressed();
  void on_preview_shader_button_pressed();

  void on_menu_button_pressed();
  void on_load_image_button_pressed();
  void on_match_image_button_pressed();
  void on_start_matching_button_pressed();
  void on_stop_matching_button_pressed();
  void on_start_matching_timer_timeout();
  void on_stop_matching_timer_timeout();
  void on_to_csv_button_pressed();

  /**
   * @brief Show the parameters of a node in the right side panel.
   * 
   * @note Called when the scene reports a node selection change. Passing @c -1 clears the panel
   *       (e.g. when nothing is selected or the selected node has no parameters).
   * 
   * @param n_id 
   */
  void on_node_parameters_requested(const int& n_id);

  /**
   * @brief Release the parameters widget of a node that is being deleted.
   * 
   * @note If the deleted node's parameters are the ones currently shown in the panel, the panel is
   *       cleared and the widget is detached so that the scene can safely destroy it.
   * 
   * @param n_id 
   */
  void on_node_parameters_widget_removed(const int& n_id);

 private:
  QHBoxLayout* layout;

  QWidget* side_widget;
  QVBoxLayout* side_outer_layout;
  QVBoxLayout* side_layout;
  QLineEdit* name_edit;
  QPushButton* save_button;

  ////////////////////////////////////
  // Right-docked node parameters panel
  ////////////////////////////////////

  QWidget* parameters_dock;
  QScrollArea* parameters_scroll_area;
  QWidget* parameters_container;
  QVBoxLayout* parameters_layout;
  QLabel* parameters_hint_label;
  QWidget* current_parameters_widget;  // The parameters widget currently shown in the panel (or nullptr)
  int current_parameters_n_id;         // The id of the node whose parameters are shown (or -1)

  QHBoxLayout* scene_layer_layout;
  QWidget* scene_layer;  // Layer having the scene.
  VisualShaderGraphicsScene* scene;
  VisualShaderGraphicsView* view;

  QWidget* top_layer;  // Layer having the menu bar.
  QHBoxLayout* menu_bar;

  QPushButton* menu_button;
  QPushButton* create_node_button;
  QPushButton* preview_shader_button;
  QPushButton* zoom_in_button;
  QPushButton* reset_zoom_button;
  QPushButton* zoom_out_button;

  QPushButton* load_image_button;
  QPushButton* match_image_button;

  QPushButton* to_csv_button;

  QAction* create_node_action;

  ////////////////////////////////////
  // Code Previewer
  ////////////////////////////////////

  QDialog* code_previewer_dialog;
  QVBoxLayout* code_previewer_layout;
  QPlainTextEdit* code_previewer;

  ////////////////////////////////////
  // CreateNodeDialog Nodes Tree
  ////////////////////////////////////

  struct CreateNodeDialogNodesTreeItem {
    std::string caption;
    std::string category_path;
    std::string description;
    std::shared_ptr<IVisualShaderProtoNode> proto_node;

    CreateNodeDialogNodesTreeItem(const std::string& caption, const std::string& category_path,
                                  const std::string& description, const std::shared_ptr<IVisualShaderProtoNode>& proto_node)
        : caption(caption), category_path(category_path), description(description), proto_node(proto_node) {}
  };

  std::vector<CreateNodeDialogNodesTreeItem> create_node_dialog_nodes_tree_items;

  CreateNodeDialog* create_node_dialog;

  // Main Model
  ProtoModel* visual_shader_model;

  // Sub-Models
  ProtoModel* nodes_model;
  ProtoModel* connections_model;

  ShaderGenSharedMemory* shared_memory;

  AIAgentWorker* ai_agent_worker;
  AIAgentMonitor* ai_agent_monitor;
  AIAgentParametersEditor* parameters_editor;
  StartMatchingButton* start_matching_button;
  StopMatchingButton* stop_matching_button;

  QComboBox* matching_type_combo_box;

  QTimer* start_matching_timer;
  QTimer* stop_matching_timer;

  /**
   * @brief Initializes the UI
   * 
   */
  void init();

  bool add_output_node();
  void load_graph();
  void refresh_node_coordinates();

  void create_node(const QPointF& coordinate);

  void add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate);

  void show_create_node_dialog(const QPointF& coordinate);

  std::vector<std::string> parse_node_category_path(const std::string& n_category_path);
  QTreeWidgetItem* find_or_create_category_item(QTreeWidgetItem* parent, const std::string& category,
                                                const std::string& category_path,
                                                QTreeWidget* create_node_dialog_nodes_tree,
                                                std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map);

  void update_graph_in_scene();
  void update_parameters_only_graph_in_scene();
  void update_parameters_and_connections_graph_in_scene();
  void update_full_graph_in_scene();
};

class StartMatchingButton : public QPushButton
{
  Q_OBJECT

public:
  StartMatchingButton(QWidget* parent = nullptr) : QPushButton(parent) {
    this->setFixedWidth(25);
  }

private:
  void paintEvent(QPaintEvent *event) override {
    QPushButton::paintEvent(event);

    QPainter painter = QPainter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w {this->width()};
    int h {this->height()};

    float min_side {(float)qMin(w, h)};
    
    QPointF center {w * 0.5f, h * 0.5f};

    float vc_length {min_side * 0.25f}; // Length from each triangle's vertex to the center of the widget

    // Calculate the vertices of the triangle based on the center of the widget
    QPointF top_v {center + QPointF(vc_length, 0)};
    QPointF bottom_left_v {center - QPointF(vc_length, vc_length)};
    QPointF bottom_right_v {center - QPointF(vc_length, -vc_length)};

    // Draw the triangle
    painter.setBrush(Qt::green);
    painter.setPen(Qt::NoPen); // Set the pen to Qt::NoPen to remove the stroke
    painter.drawPolygon(QPolygonF() << top_v << bottom_left_v << bottom_right_v);

    // Draw an outline around the triangle
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    painter.drawPolygon(QPolygonF() << top_v << bottom_left_v << bottom_right_v);
  }
};

class StopMatchingButton : public QPushButton {
  Q_OBJECT

public:
  StopMatchingButton(QWidget* parent = nullptr) : QPushButton(parent) {
    this->setFixedWidth(25);
  }

private:
  void paintEvent(QPaintEvent *event) override {
    QPushButton::paintEvent(event);

    QPainter painter = QPainter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w {this->width()};
    int h {this->height()};

    float min_side {(float)qMin(w, h)};

    QPointF center {w * 0.5f, h * 0.5f};

    float square_side_length {min_side * 0.4f}; // Side length of the square

    // Draw the square
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen); // Set the pen to Qt::NoPen to remove the stroke
    
    QRectF square {center - QPointF(square_side_length * 0.5f, square_side_length * 0.5f), QSizeF(square_side_length, square_side_length)};
    painter.drawRect(square);

    // Draw an outline around the square
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    painter.drawRect(square);
  }
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               CreateNodeDialog                             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class CreateNodeDialog : public QDialog {
  Q_OBJECT

 public:
  CreateNodeDialog(QWidget* parent = nullptr);
  ~CreateNodeDialog() override = default;

  QTreeWidget* get_nodes_tree() const { return create_node_dialog_nodes_tree; }

  QTreeWidgetItem* get_selected_item() const { return selected_item; }

 private Q_SLOTS:
  void on_create_node_button_pressed();
  void on_cancel_node_creation_button_pressed();

  void update_selected_item();

 private:
  QVBoxLayout* layout;

  QVBoxLayout* create_node_dialog_nodes_tree_layout;

  QTreeWidget* create_node_dialog_nodes_tree;
  QTextEdit* create_node_dialog_nodes_description;

  QHBoxLayout* buttons_layout;
  QPushButton* create_button;
  QPushButton* cancel_button;

  QTreeWidgetItem* selected_item;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               OriginalMatchingImageWidget                  *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class OriginalMatchingImageWidget : public QWidget {
 public:
  OriginalMatchingImageWidget(QWidget* parent = nullptr) : QWidget(parent) {
    pixmap = QPixmap(size());
    pixmap.fill(Qt::red);  // Fill it with the red color
  }

 protected:
  // Override the paintEvent to display the pixmap
  void paintEvent([[maybe_unused]] QPaintEvent* event) override {
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);  // Draw the pixmap starting at (0, 0)
  }

 private:
  QPixmap pixmap;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsScene                    *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderGraphicsScene : public QGraphicsScene {
  Q_OBJECT

 public:
  VisualShaderGraphicsScene(QObject* parent = nullptr);

  ~VisualShaderGraphicsScene() override;

  bool add_node_to_model(const int& n_id, const std::shared_ptr<IVisualShaderProtoNode>& proto_node,
                         const QPointF& coordinate);
  bool add_node_to_scene(const int& n_id, const std::shared_ptr<IVisualShaderProtoNode>& proto_node,
                         const QPointF& coordinate);
  bool add_node(const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const QPointF& coordinate, const int& n_id = -1);

  QVariant get_node_value(const int& n_id, const int& field_number, const int& row_entry = -1) const;
  QVariant get_node_field_value(const int& n_id, const int& field_number, const int& row_entry = -1) const;

  /**
   * @brief Write the Protobuf default value of every field of a node that is still not set.
   *
   * @note Nodes created from the UI or loaded from @c model.json can hold unset optional fields
   *       which the models report as an invalid @c QVariant. This is used when a node is created
   *       and when a graph is loaded to repair such nodes.
   *
   * @param n_id
   * @return bool
   */
  bool write_default_values_to_node(const int& n_id);

  /**
   * @brief Seed the default rows of a repeated message field of a node (e.g. a color gradient's stops).
   *
   * @note proto3 repeated fields default to empty, so nodes ported from implementations that started
   *       with a non-empty list (e.g. a default color ramp) would otherwise be created without any row.
   *       Does nothing when the field already has rows (e.g. nodes loaded from @c model.json).
   *
   * @param n_id
   * @param field_descriptor The descriptor of the repeated message field inside the node.
   * @param default_rows One @c QVariantMap entry per row, mapping the field number (as a string) of a
   *        column inside the repeated message to its default value.
   * @return bool
   */
  bool write_default_repeated_field_values_to_node(const int& n_id, const google::protobuf::FieldDescriptor* field_descriptor,
                                                   const QVariantList& default_rows);

  /**
   * @brief Seed the default values of a singular message field of a node (e.g. a node's optional
   *        Matrix3x3 transformation).
   *
   * @note proto3 message fields default to zero, so a node ported from an implementation that started
   *       with a non-zero default (e.g. an identity transformation) would otherwise be created with a
   *       zeroed message. Entries that are already set (e.g. nodes loaded from @c model.json) are left
   *       untouched.
   *
   * @param n_id
   * @param field_descriptor The descriptor of the singular message field inside the node.
   * @param default_entries A @c QVariantMap mapping the field number (as a string) of a column inside
   *        the message to its default value.
   * @return bool
   */
  bool write_default_message_field_values_to_node(const int& n_id,
                                                  const google::protobuf::FieldDescriptor* field_descriptor,
                                                  const QVariantMap& default_entries);

  bool delete_node_from_model(const int& n_id);
  bool delete_node_from_scene(const int& n_id, const int& in_port_count, const int& out_port_count);
  bool delete_node(const int& n_id, const int& in_port_count, const int& out_port_count);
  bool delete_all_nodes();

  bool update_node_in_model(const int& n_id, const int& field_number, const QVariant& value, const int& row_entry = -1);
  bool update_node_field_in_model(const int& n_id, const int& field_number, const QVariant& value);

  /**
   * @brief Write one value inside a repeated message field of a node.
   *
   * @note Addresses the value the same way the graph string does: by the row index inside the
   *       repeated field and the field number of the column inside the repeated message.
   *
   * @param n_id
   * @param field_number The field number of the repeated field inside the node.
   * @param row The row index inside the repeated field.
   * @param sub_field_number The field number of the column inside the repeated message.
   * @param value
   * @return bool
   */
  bool update_node_repeated_field_in_model(const int& n_id, const int& field_number, const int& row,
                                           const int& sub_field_number, const QVariant& value);

  /**
   * @brief Write one entry of a coordinate consuming node's Matrix3x3 transformation.
   *
   * @note The transformation is a field of the node type message (e.g. VisualShaderNodeChecker), so it
   *       is addressed through the node_type oneof value (fields <node type>:<transformation field
   *       number>:<column field number>).
   *
   * @param n_id
   * @param transformation_field_number The field number of the Matrix3x3 transformation on the node
   *                                    type message (e.g. VisualShaderNodeCellular::kTransformationFieldNumber).
   * @param column_field_number The field number of the entry inside the Matrix3x3 message (1..9).
   * @param value
   * @return bool
   */
  bool update_node_transformation_in_model(const int& n_id, const int& transformation_field_number,
                                           const int& column_field_number, const QVariant& value);

  bool update_node_in_scene(const int& n_id, const int& field_number, const QVariant& value);
  bool update_node_field_in_scene(const int& n_id, const int& field_number, const QVariant& value);
  bool update_node(const int& n_id, const int& field_number, const QVariant& value);

  void move_node(const int& n_id, const QPointF& new_coordinate);

  void set_model(ProtoModel* visual_shader_model) { this->visual_shader_model = visual_shader_model; }
  void set_nodes_model(ProtoModel* nodes_model) { this->nodes_model = nodes_model; }
  void set_connections_model(ProtoModel* connections_model) { this->connections_model = connections_model; }

  VisualShaderEditor* get_editor() const { return editor; }
  void set_editor(VisualShaderEditor* editor) { this->editor = editor; }

  bool add_connection_to_model(const int& c_id, const int& from_node_id, const int& from_port_index, const int& to_node_id,
                               const int& to_port_index);
  bool add_connection_to_scene(const int& from_node_id, const int& from_port_index, const int& to_node_id,
                               const int& to_port_index);
  bool add_connection_to_scene(const int& c_id, const int& from_node_id, const int& from_port_index, const int& to_node_id,
                               const int& to_port_index);
  bool add_connection(const int& c_id, const int& from_node_id, const int& from_port_index, const int& to_node_id,
                      const int& to_port_index);

  int get_connection_value(const int& c_id, const int& field_number, const int& row_entry = -1) const;

  bool is_valid_connection(const int& from_node_id, const int& from_port_index, const int& to_node_id, const int& to_port_index) const;

  /**
   * @brief This function is used to make sure that all connections that 
   *        are connection to @c n_id are still valid after some modifications
   *        in the node fields.
   * 
   * @param n_id 
   */
  void revalidate_connections(const int& n_id);

  bool delete_connection_from_model(const int& c_id);
  bool delete_connection_from_scene(const int& c_id, const int& from_node_id, const int& from_port_index,
                                    const int& to_node_id, const int& to_port_index);
  bool delete_connection(const int& c_id, const int& from_node_id, const int& from_port_index,
                         const int& to_node_id, const int& to_port_index);

  bool add_temporary_connection(const int& from_node_id, const int& from_port_index);
  bool delete_temporary_connection(const int& from_node_id, const int& from_port_index);
  bool convert_to_temporary_connection(const int& c_id, const int& from_node_id, const int& from_port_index);

  bool update_connection_in_model(const int& c_id, const int& field_number, const int& value, const int& row_entry = -1);
  bool update_connection_in_scene(const int& c_id, const int& field_number, const int& node_id, const int& port_index);
  bool update_connection(const int& c_id, const int& node_id_field_number, const int& port_index_field_number, const int& node_id, const int& port_index);

  void move_connection(const int& c_id, const bool& start_c_i, const QPointF& new_start_c_i_coordinate,
                       const bool& end_c_i, const QPointF& new_end_c_i_coordinate);
  void move_connection(VisualShaderConnectionGraphicsObject* c_o, const bool& start_c_i,
                       const QPointF& new_start_c_i_coordinate, const bool& end_c_i,
                       const QPointF& new_end_c_i_coordinate);

  VisualShaderNodeGraphicsObject* get_node_graphics_object(const int& n_id) const;
  VisualShaderConnectionGraphicsObject* get_connection_graphics_object(const int& c_id) const;

  /**
   * @brief Get the parameters widget of a node.
   *
   * @note The widget is built together with the node and holds one labeled row per editable field.
   *       It is not part of the scene; the editor moves it into the right-docked parameters panel
   *       when the node is selected.
   *
   * @param n_id
   * @return QWidget* The parameters widget, or @c nullptr when the node has no parameters.
   */
  QWidget* get_node_parameters_widget(const int& n_id) const;

  int get_new_node_id() const;
  int get_new_connection_id() const;
  int find_node_entry(const int& n_id) const;
  int find_connection_entry(const int& c_id) const;
  int get_node_type_field_number(const int& n_id) const;

 Q_SIGNALS:
  /**
   * @brief Request the editor to show the parameters of a node in its right-docked panel.
   *
   * @note EMITTED when a node is selected (with its @c n_id) or when the selection is cleared
   *       (with @c -1). Connected to @c VisualShaderEditor::on_node_parameters_requested.
   *
   * @param n_id
   */
  void node_parameters_requested(const int& n_id);

  /**
   * @brief Notify the editor that the parameters widget of a node is about to be destroyed.
   *
   * @note EMITTED from @c VisualShaderGraphicsScene::delete_node_from_scene. Connected to
   *       @c VisualShaderEditor::on_node_parameters_widget_removed.
   *
   * @param n_id
   */
  void node_parameters_widget_removed(const int& n_id);

 public Q_SLOTS:
  void on_scene_update_requested();

  /**
   * @brief Updates the code inside all the nodes except the output node.
   * 
   */
  void on_update_renderer_widgets_requested();

 private Q_SLOTS:
  /**
   * @brief Called when an interaction with a port is made.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::paint function
   *       to @c VisualShaderInputPortGraphicsObject::port_* signals.
   * 
   * @param port 
   * @param coordinate 
   */
  void on_port_pressed(QGraphicsObject* port, const QPointF& coordinate);
  void on_port_dragged(QGraphicsObject* port, const QPointF& coordinate);
  void on_port_dropped(QGraphicsObject* port, const QPointF& coordinate);

  /**
   * @brief Called when a node is moved.
   * 
   * @note Connected in @c VisualShaderGraphicsScene::VisualShaderGraphicsScene constructor
   *       to @c VisualShaderGraphicsScene::node_moved signal.
   * 
   * @param n_id 
   * @param new_coordinate 
   */
  void on_node_moved(const int& n_id, const QPointF& new_coordinate);

  /**
   * @brief Called when a delete node action is triggered.
   * 
   * @note Connected in @c VisualShaderGraphicsScene::add_node function
   *       to @c VisualShaderNodeGraphicsObject::node_deleted signal.
   * 
   * @param n_id 
   */
  void on_node_deleted(const int& n_id, const int& in_port_count, const int& out_port_count);

  void on_in_port_remove_requested(VisualShaderInputPortGraphicsObject* in_port);
  void on_out_port_remove_requested(VisualShaderOutputPortGraphicsObject* out_port);

  void on_port_type_generator_requested(const int& n_id);

  void on_node_repeated_field_row_add_requested(const int& n_id, const int& field_number);
  void on_node_repeated_field_row_remove_requested(const int& n_id, const int& field_number, const int& row);
  void on_node_repeated_field_sort_requested(const int& n_id, const int& field_number, const int& sort_field_number);

 private:
  VisualShaderEditor* editor;

  std::unordered_map<int, VisualShaderNodeGraphicsObject*> node_graphics_objects;
  std::unordered_map<int, VisualShaderConnectionGraphicsObject*> connection_graphics_objects;

  VisualShaderConnectionGraphicsObject* temporary_connection_graphics_object;

  // n_id -> field number -> QWidget*
  std::unordered_map<int, std::unordered_map<int, QWidget*>> node_field_widgets;

  // n_id -> parameters widget (shown in the editor's right-docked panel). Not part of the scene.
  std::unordered_map<int, QWidget*> node_parameters_widgets;

  // Main Model
  ProtoModel* visual_shader_model;

  // Sub-Models
  ProtoModel* nodes_model;
  ProtoModel* connections_model;

  VisualShaderNodeGraphicsObject* newest_node_graphics_object;

  void remove_item(QGraphicsItem* item);
  bool check_if_connection_out_of_bounds(VisualShaderOutputPortGraphicsObject* from_o_port, VisualShaderInputPortGraphicsObject* to_i_port);

  /**
   * @brief Add a labeled row (a label followed by an editor widget) to a parameters layout.
   *
   * @note Used by the @c REGISTER_NODE_FIELD_* macros to build a node's parameters widget.
   *
   * @param layout The parameters layout the row is appended to.
   * @param parent The parameters widget that owns the row.
   * @param label The text shown on the left side of the row.
   * @param field_widget The editor widget shown on the right side of the row.
   */
  void add_parameter_row(QVBoxLayout* layout, QWidget* parent, const QString& label, QWidget* field_widget);

  RepeatedMessageModel* get_node_repeated_field_model(const int& n_id, const int& field_number) const;
  void refresh_node_repeated_field_widget(const int& n_id, const int& field_number);

 protected:
  /**
   * @brief Clear the parameters panel when the empty canvas is clicked.
   */
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderGraphicsView                     *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderGraphicsView : public QGraphicsView {
  Q_OBJECT

 public:
  VisualShaderGraphicsView(VisualShaderGraphicsScene* scene, QWidget* parent = nullptr);

  ~VisualShaderGraphicsView();

  float get_x() const { return rect_x; }
  float get_y() const { return rect_y; }
  float get_width() const { return rect_width; }
  float get_height() const { return rect_height; }

 public Q_SLOTS:
  /**
   * @todo If the button is pressed then zoom in from the center of the view.
   */
  void zoom_in();
  void reset_zoom();
  void zoom_out();

 private Q_SLOTS:
  /**
   * @brief Called when @c VisualShaderGraphicsView::create_node_action is triggered.
   * 
   * @note Connected in @c VisualShaderGraphicsView::VisualShaderGraphicsView constructor
   *       to @c QAction::triggered signal.
   * 
   * @note EMITS @c VisualShaderEditor::create_node_dialog_requested signal.
   * 
   */
  void on_create_node_action_triggered();

 Q_SIGNALS:
  void zoom_changed(const float& zoom);

 private:
  VisualShaderGraphicsScene* scene;

  // Style
  QColor background_color = QColor(40, 40, 40);   // Dark Charcoal
  QColor fine_grid_color = QColor(50, 50, 50);    // Soft Dark Gray
  QColor coarse_grid_color = QColor(30, 30, 30);  // Muted Deep Gray

  // Scene Rect
  float t_size = std::numeric_limits<short>::max();  // 32767
  float rect_x = -1.0f * t_size * 0.5f;
  float rect_y = -1.0f * t_size * 0.5f;
  float rect_width = t_size;
  float rect_height = t_size;

  float fit_in_view_margin = 50.0f;

  // Zoom
  float zoom = 1.0f;
  float zoom_step = 1.2f;
  float zoom_min;
  float zoom_max;

  QMenu* context_menu;
  QPointF last_context_menu_coordinate = {0, 0};
  QAction* create_node_action;

  QAction* zoom_in_action;
  QAction* reset_zoom_action;
  QAction* zoom_out_action;

  QPointF last_click_coordinate;

  void drawBackground(QPainter* painter, const QRectF& r) override;
  void contextMenuEvent(QContextMenuEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void showEvent(QShowEvent* event) override;

  void move_view_to_fit_items();
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****               VisualShaderNodeGraphicsObject               *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderNodeGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderNodeGraphicsObject(const int& n_id, const QPointF& coordinate,
                                  const std::shared_ptr<IVisualShaderProtoNode>& proto_node, QGraphicsItem* parent = nullptr);
  ~VisualShaderNodeGraphicsObject();

  void set_x_coordinate(const float& x) { coordinate.setX(x); }
  void set_y_coordinate(const float& y) { coordinate.setY(y); }

  int get_input_port_count() const { return in_port_count; }
  int get_output_port_count() const { return out_port_count; }

  VisualShaderInputPortGraphicsObject* get_input_port_graphics_object(const int& p_index) const;
  VisualShaderOutputPortGraphicsObject* get_output_port_graphics_object(const int& p_index) const;

  QWidget* get_embed_widget() const { return embed_widget; }
  void set_embed_widget(QWidget* embed_widget) { this->embed_widget = embed_widget; }

  RendererWidget* get_renderer_widget() const { return renderer_widget; }

  void update_port_types(const std::shared_ptr<VisualShaderNodePortTypeGenerator>& port_type_generator);

  void update_layout();

 Q_SIGNALS:
  /**
   * @brief Send a request to delete a node.
   * 
   * @note EMITTED from @c VisualShaderNodeGraphicsObject::on_delete_node_action_triggered slot.
   * 
   * @note Connected in @c VisualShaderGraphicsScene::add_node function to 
   *       @c VisualShaderGraphicsScene::on_node_deleted slot.
   * 
   * @param n_id 
   */
  void node_deleted(const int& n_id, const int& in_port_count, const int& out_port_count);

  /**
   * @brief Notify the scene that a node has been moved.
   * 
   * @note EMITTED from @c VisualShaderNodeGraphicsObject::itemChange function.
   * 
   * @note Connected to @c VisualShaderGraphicsScene::on_node_moved slot in 
   *       @c VisualShaderGraphicsScene::VisualShaderGraphicsScene constructor.
   * 
   * @param n_id 
   * @param new_coordinate 
   */
  void node_moved(const int& n_id, const QPointF& new_coordinate);

  void in_port_pressed(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);
  void in_port_dragged(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);
  void in_port_dropped(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);

  void out_port_pressed(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);
  void out_port_dragged(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);
  void out_port_dropped(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);

  void scene_update_requested();

  /**
   * @brief Notify that this node was clicked, so the editor can show its parameters.
   * 
   * @note EMITTED from @c VisualShaderNodeGraphicsObject::mousePressEvent. Connected in
   *       @c VisualShaderGraphicsScene::add_node_to_scene directly to
   *       @c VisualShaderGraphicsScene::node_parameters_requested.
   * 
   * @param n_id 
   */
  void node_selected(const int& n_id);

  void in_port_remove_requested(VisualShaderInputPortGraphicsObject* in_port);
  void out_port_remove_requested(VisualShaderOutputPortGraphicsObject* out_port);

  void port_type_update_requested();
  void port_type_generator_requested(const int& n_id);

 public Q_SLOTS:
  void on_preview_shader_button_pressed();
  void on_port_type_update_requested();

  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

 private Q_SLOTS:
  /**
   * @brief Called when @c VisualShaderNodeGraphicsObject::delete_node_action is triggered.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::VisualShaderNodeGraphicsObject constructor
   *       to @c QAction::triggered signal.
   * 
   * @note EMITS @c VisualShaderNodeGraphicsObject::node_deleted signal.
   * 
   */
  void on_delete_node_action_triggered();

  void on_in_port_pressed(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate) {
    Q_EMIT in_port_pressed(port, coordinate);
  }
  void on_in_port_dragged(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate) {
    Q_EMIT in_port_dragged(port, coordinate);
  }
  void on_in_port_dropped(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate) {
    Q_EMIT in_port_dropped(port, coordinate);
  }

  void on_out_port_pressed(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate) {
    Q_EMIT out_port_pressed(port, coordinate);
  }
  void on_out_port_dragged(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate) {
    Q_EMIT out_port_dragged(port, coordinate);
  }
  void on_out_port_dropped(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate) {
    Q_EMIT out_port_dropped(port, coordinate);
  }

 private:
  int n_id;
  QPointF coordinate;
  const std::shared_ptr<IVisualShaderProtoNode> proto_node;

  std::string caption;
  int in_port_count;
  int out_port_count;
  std::vector<std::string> in_port_captions;
  std::vector<std::string> out_port_captions;
  std::vector<VisualShaderNodePortType> in_port_types;
  std::vector<VisualShaderNodePortType> out_port_types;

  QMenu* context_menu;
  QAction* delete_node_action;

  std::unordered_map<int, VisualShaderInputPortGraphicsObject*> in_port_graphics_objects;
  std::unordered_map<int, VisualShaderOutputPortGraphicsObject*> out_port_graphics_objects;

  // Style
  QColor normal_boundary_color = QColor(220, 20, 60);   // Crimson Red
  QColor selected_boundary_color = QColor(255, 69, 0);  // Red-Orange
  QColor font_color = QColor(255, 255, 255);            // Pure White
  QColor fill_color = QColor(40, 40, 40, 200);          // Semi-transparent Dark Gray

  float pen_width = 1.0f;

  float opacity = 0.8f;
  float corner_radius = 3.0f;
  float port_diameter = 8.0f;

  float rect_width; // Calculated in update_layout()
  float rect_height; // Calculated in update_layout()

  float rect_padding;  // Calculated in update_layout()
  float rect_margin;   // Calculated in update_layout()

  float caption_rect_height;  // Calculated in update_layout()
  float caption_h_padding = 10.0f;
  float caption_v_padding = 8.0f;
  QPointF caption_coordinate;

  float body_rect_header_height = 30.0f;
  float body_rect_port_step = 40.0f;
  float body_rect_footer_height = 30.0f;

  std::vector<QRectF> input_ports_rects;
  std::vector<QRectF> output_ports_rects;
  std::vector<QPointF> input_ports_caption_coordinates;
  std::vector<QPointF> output_ports_caption_coordinates;

  float port_caption_spacing = 5.0f;  // Distance between the port and its caption

  // Ports Style
  float connected_port_diameter = 8.0f;
  float unconnected_port_diameter = 6.0f;

  // Caption
  float caption_font_size = 18.0f;
  float port_caption_font_size = 8.0f;

  QWidget* embed_widget;
  QPointF embed_widget_coordinate;  // Calculated in update_layout()
  float embed_widget_h_margin = 15.0f;
  float embed_widget_v_margin = 5.0f;

  OriginalMatchingImageWidget* matching_image_widget;
  QPointF matching_image_widget_coordinate; // Calculated in update_layout()
  float spacing_between_output_node_and_matching_image = 10.0f;

  RendererWidget* renderer_widget;
  QPointF renderer_widget_coordinate;  // Calculated in update_layout()
  float spacing_between_current_node_and_shader_previewer = 10.0f;

  QRectF boundingRect() const override;

  /**
   * @brief 
   * 
   * @note This function contains some commented code that is meant to be used
   *       for debugging purposes.
   * 
   * @param painter 
   * @param option 
   * @param widget 
   */
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

class VisualShaderInputPortGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderInputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                      const VisualShaderNodePortType& port_type,
                                      QGraphicsItem* parent = nullptr);
  ~VisualShaderInputPortGraphicsObject() = default;

  QPointF get_global_coordinate() const { return mapToScene(rect.center()); }

  int get_node_id() const { return n_id; }
  int get_port_index() const { return p_index; }
  void set_port_type(const VisualShaderNodePortType& port_type) { this->port_type = port_type; }
  VisualShaderNodePortType get_port_type() const { return port_type; }

  int get_c_id() const { return c_id; }
  bool connect(const int& c_id) {
    CHECK_CONDITION_TRUE_NON_VOID(this->c_id != -1, false, "Input port is already connected.");
    this->c_id = c_id;
    return true;
  }
  bool detach_connection() {
    CHECK_CONDITION_TRUE_NON_VOID(this->c_id == -1, false, "Input port is not connected.");
    this->c_id = -1;
    return true;
  }
  bool is_connected() const { return this->c_id != -1; }

 Q_SIGNALS:
  /**
   * @brief Called when the an interaction with the port is made.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::paint function to
   *       @c VisualShaderGraphicsScene::on_port_* slots.
   * 
   * @param port 
   * @param coordinate 
   */
  void port_pressed(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);
  void port_dragged(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);
  void port_dropped(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);

 private:
  QRectF rect;
  const int n_id;
  const int p_index;
  VisualShaderNodePortType port_type;

  int c_id;

  float padding = 0.5f;

  // Style
  QColor font_color = QColor(255, 255, 255);
  QColor connection_point_color = QColor(220, 20, 60);  // Crimson

  float opacity = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

class VisualShaderOutputPortGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderOutputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                        const VisualShaderNodePortType& port_type,
                                       QGraphicsItem* parent = nullptr);
  ~VisualShaderOutputPortGraphicsObject() = default;

  QPointF get_global_coordinate() const { return mapToScene(rect.center()); }

  int get_node_id() const { return n_id; }
  int get_port_index() const { return p_index; }
  void set_port_type(const VisualShaderNodePortType& port_type) { this->port_type = port_type; }
  VisualShaderNodePortType get_port_type() const { return port_type; }

  std::unordered_set<int> get_c_ids() const {
    return c_ids;
  }
  bool connect(const int& c_id) {
    auto result = this->c_ids.insert(c_id);
    CHECK_CONDITION_TRUE_NON_VOID(result.second == false, false, "Failed to insert connection to output port.");
    return true;
  }
  bool detach_connection(const int& c_id) {
    size_t erased_count {c_ids.erase(c_id)};
    CHECK_CONDITION_TRUE_NON_VOID(erased_count == 0, false, "Failed to erase connection from output port.");
    return true;
  }
  bool is_connected() const { return c_ids.size() > 0; }

 Q_SIGNALS:
  /**
   * @brief Called when the an interaction with the port is made.
   * 
   * @note Connected in @c VisualShaderNodeGraphicsObject::paint function to
   *       @c VisualShaderGraphicsScene::on_port_* slots.
   * 
   * @param port 
   * @param coordinate 
   */
  void port_pressed(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);
  void port_dragged(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);
  void port_dropped(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);

 private:
  QRectF rect;
  const int n_id;
  const int p_index;
  VisualShaderNodePortType port_type;

  // An output port can have multiple connections.
  std::unordered_set<int> c_ids;

  float padding = 0.5f;

  // Style
  QColor font_color = QColor(255, 255, 255);
  QColor connection_point_color = QColor(220, 20, 60);  // Crimson

  float opacity = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****           VisualShaderConnectionGraphicsObject             *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderConnectionGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderConnectionGraphicsObject(const int& c_id, const int& from_n_id, const int& from_p_index,
                                       const QPointF& start_coordinate, QGraphicsItem* parent = nullptr);
  ~VisualShaderConnectionGraphicsObject() = default;

  void update_layout();

  int get_id() const { return c_id; }

  int get_from_node_id() const { return from_n_id; }
  int get_from_port_index() const { return from_p_index; }

  int get_to_node_id() const { return to_n_id; }
  int get_to_port_index() const { return to_p_index; }

  void detach_end() {
    this->set_to_node_id(-1);
    this->set_to_port_index(-1);
  }

  void set_from_node_id(const int& from_n_id) { this->from_n_id = from_n_id;  }
  void set_from_port_index(const int& from_p_index) { this->from_p_index = from_p_index;  }
  void set_to_node_id(const int& to_n_id) { this->to_n_id = to_n_id; }
  void set_to_port_index(const int& to_p_index) { this->to_p_index = to_p_index; }

  void set_start_coordinate(const QPointF& start_coordinate) {
    this->start_coordinate = start_coordinate;
    update();
  }
  void set_end_coordinate(const QPointF& end_coordinate) {
    this->end_coordinate = end_coordinate;
    update();
  }

 private:
  int c_id;
  int from_n_id;
  int from_p_index;
  int to_n_id;
  int to_p_index;

  QPointF start_coordinate;
  QPointF end_coordinate;

  // Style
  QColor construction_color = QColor(139, 0, 0);  // Dark Red
  QColor normal_color = QColor(178, 34, 34);      // Firebrick Red
  QColor selected_color = QColor(55, 55, 55);     // Dark Gray
  QColor connection_point_color = QColor(211, 211, 211);

  float line_width = 3.0f;
  float construction_line_width = 2.0f;
  float point_diameter = 10.0f;

  QRectF rect; // Calculated in update_layout()
  QRectF start_rect; // Calculated in update_layout()
  QRectF end_rect; // Calculated in update_layout()

  float rect_margin;  // Calculated in update_layout()

  std::pair<QPointF, QPointF> control_points; // Calculated in update_layout()
  QPainterPath cubic_path;  // Calculated in update_layout()

  float h_abnormal_offset = 50.0f;
  float v_abnormal_offset = 40.0f;
  float abnormal_face_to_back_control_width_expansion_factor = 0.5f;
  float abnormal_face_to_back_control_height_expansion_factor = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  int detect_quadrant(const QPointF& reference, const QPointF& target) const;
  QRectF calculate_bounding_rect_from_coordinates() const;
  std::pair<QPointF, QPointF> calculate_control_points() const;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****                 Field Widgets                              *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

// TODO: Create a debouncer for the text changed signals.
class VisualShaderNodeFieldComboBox : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeFieldComboBox(const QVariant& initial_value, const int& n_id,
                                    const EnumDescriptor* enum_descriptor, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldComboBox() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);
  void port_type_update_requested();
  void revalidate_connections_requested(const int& n_id);

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  int n_id;
  const EnumDescriptor* enum_descriptor;
  int field_number;
};

class VisualShaderNodeFieldLineEditFloat : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFieldLineEditFloat(const QVariant& initial_value, const int& n_id, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldLineEditFloat() = default;

  void set_current_text(const std::string& text) { this->setText(QString::fromStdString(text)); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  int n_id;
  int field_number;
};

class VisualShaderNodeFieldLineEditInt : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFieldLineEditInt(const QVariant& initial_value, const int& n_id, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldLineEditInt() = default;

  void set_current_text(const std::string& text) { this->setText(QString::fromStdString(text)); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  int n_id;
  int field_number;
};

class VisualShaderNodeFieldLineEditUInt : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFieldLineEditUInt(const QVariant& initial_value, const int& n_id, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldLineEditUInt() = default;

  void set_current_text(const std::string& text) { this->setText(QString::fromStdString(text)); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  int n_id;
  int field_number;
};

class VisualShaderNodeFieldCheckBox : public QCheckBox {
  Q_OBJECT

 public:
  VisualShaderNodeFieldCheckBox(const QVariant& initial_value, const int& n_id, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldCheckBox() = default;

  void set_checked(const bool& checked) { this->setChecked(checked); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_state_changed(const int& state);

 private:
  int n_id;
  int field_number;
};

class VisualShaderNodeFieldSpinBox : public QSpinBox {
  Q_OBJECT
public:
  VisualShaderNodeFieldSpinBox(const QVariant& initial_value, const int& min_value, const int& max_value, const int& n_id, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldSpinBox() = default;

  void set_value(const int& value) { this->setValue(value); }

Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

private Q_SLOTS:
  void on_value_changed(const int& value);

private:
  int n_id;
  int field_number;
};

/**
    @brief A widget that edits a repeated message field (e.g. a color gradient) as a list of rows.

    @note Each row exposes one editor per column of the repeated message. Every edit is forwarded to
          the scene as a scalar update addressed by the row index and the column's field number, so
          that a repeated field is edited exactly like a set of ordinary node parameters. Adding and
          removing rows, as well as sorting them, is also requested from the scene.
*/
class VisualShaderNodeFieldRepeatedMessage : public QWidget {
  Q_OBJECT

 public:
  VisualShaderNodeFieldRepeatedMessage(RepeatedMessageModel* repeated_model, const int& sort_field_number,
                                       const int& n_id, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldRepeatedMessage() = default;

  void refresh_from_model();

 Q_SIGNALS:
  void node_repeated_field_update_requested(const int& n_id, const int& field_number, const int& row,
                                            const int& sub_field_number, const QVariant& value);
  void node_repeated_field_row_add_requested(const int& n_id, const int& field_number);
  void node_repeated_field_row_remove_requested(const int& n_id, const int& field_number, const int& row);
  void node_repeated_field_sort_requested(const int& n_id, const int& field_number, const int& sort_field_number);

 private:
  void clear_rows();
  void build_row(const int& row);

  RepeatedMessageModel* m_repeated_model;
  int m_sort_field_number;
  int m_n_id;
  int m_field_number;
  QVBoxLayout* m_rows_layout;
};

/**
    @brief A widget that edits a node's coordinate transformation (a Matrix3x3) as a 3x3 grid.

    @note Each of the nine entries is edited as a float and forwarded to the scene addressed 
          by its column field number, exactly like an ordinary node parameter.

    @note A 3x3 cluster of transform buttons sits next to the grid. Every button (except the identity 
          reset) right-multiplies the current matrix by a translate/scale/rotate step and re-emits 
          the nine entries. Holding shift shrinks the step.
*/
class VisualShaderNodeFieldTransformation : public QWidget {
  Q_OBJECT

 public:
  VisualShaderNodeFieldTransformation(const MessageModel* transformation_model, const int& transformation_field_number,
                                      const int& n_id, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldTransformation() = default;

 Q_SIGNALS:
  void node_transformation_update_requested(const int& n_id, const int& transformation_field_number,
                                            const int& column_field_number, const QVariant& value);

 private:
  /// The number of cells of the 3x3 matrix (m00..m22).
  static constexpr int kMatrixCellCount{9};

  /**
   * @brief Right-multiply the current matrix by @c transformation and mirror the result.
   *
   * @note The current matrix is right-multiplied (@c M = M * T). Both matrices are row-major. The 
   *       nine entries are refreshed in place and re-emitted once the product is computed.
   *
   * @param transformation The row-major 3x3 transformation to apply on the right.
   */
  void apply_transformation(const float* transformation);

  /**
   * @brief Mirror @c m_entries into the entry widgets and re-emit the nine column updates.
   *
   * @note The entry text is assigned with the signals blocked, so the refresh does not re-enter 
   *       this widget as a per-entry update; the nine updates are emitted explicitly.
   */
  void write_transformation();

  int m_n_id;
  /// The field number of the Matrix3x3 transformation on the node type message, used to address the model.
  int m_transformation_field_number;
  /// The current matrix values, row-major (m00..m22), kept in sync with the entry widgets.
  float m_entries[kMatrixCellCount];
  /// The column field number backing each entry, used to address the model.
  int m_column_field_numbers[kMatrixCellCount];
  /// The entry widgets, refreshed in place after a transform button is pressed.
  VisualShaderNodeFieldLineEditFloat* m_entry_widgets[kMatrixCellCount];
};

#define REGISTER_NODE_FIELD_COMBO_BOX(field_number) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    const ProtoModel* node_field_model = visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number))); \
    const EnumDescriptor* enum_descriptor = node_field_model->get_column_descriptor(0)->enum_type(); \
    CHECK_PARAM_NULLPTR_NON_VOID(enum_descriptor, false, "EnumDescriptor is nullptr."); \
    VisualShaderNodeFieldComboBox* node_field_widget = new VisualShaderNodeFieldComboBox(initial_value, n_id, enum_descriptor, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString::fromStdString(std::string(node_field_model->get_column_descriptor(0)->name())), node_field_widget); \
    QObject::connect(node_field_widget, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldComboBox::node_update_requested, this, &VisualShaderGraphicsScene::update_node_field_in_model); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldComboBox::port_type_update_requested, n_o, &VisualShaderNodeGraphicsObject::port_type_update_requested); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(field_number, placeholder) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldLineEditFloat* node_field_widget = new VisualShaderNodeFieldLineEditFloat(initial_value, n_id, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString(placeholder), node_field_widget); \
    QObject::connect(node_field_widget, &QLineEdit::textChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldLineEditFloat::node_update_requested, this, &VisualShaderGraphicsScene::update_node_field_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_LINE_EDIT_INT(field_number, placeholder) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldLineEditInt* node_field_widget = new VisualShaderNodeFieldLineEditInt(initial_value, n_id, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString(placeholder), node_field_widget); \
    QObject::connect(node_field_widget, &QLineEdit::textChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldLineEditInt::node_update_requested, this, &VisualShaderGraphicsScene::update_node_field_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_LINE_EDIT_UINT(field_number, placeholder) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldLineEditUInt* node_field_widget = new VisualShaderNodeFieldLineEditUInt(initial_value, n_id, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString(placeholder), node_field_widget); \
    QObject::connect(node_field_widget, &QLineEdit::textChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldLineEditUInt::node_update_requested, this, &VisualShaderGraphicsScene::update_node_field_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_CHECK_BOX(field_number) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    const ProtoModel* node_field_model = visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number))); \
    VisualShaderNodeFieldCheckBox* node_field_widget = new VisualShaderNodeFieldCheckBox(initial_value, n_id, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString::fromStdString(std::string(node_field_model->get_column_descriptor(0)->name())), node_field_widget); \
    QObject::connect(node_field_widget, &QCheckBox::stateChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldCheckBox::node_update_requested, this, &VisualShaderGraphicsScene::update_node_field_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_SPIN_BOX(field_number, min_value, max_value) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    const ProtoModel* node_field_model = visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number))); \
    VisualShaderNodeFieldSpinBox* node_field_widget = new VisualShaderNodeFieldSpinBox(initial_value, min_value, max_value, n_id, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString::fromStdString(std::string(node_field_model->get_column_descriptor(0)->name())), node_field_widget); \
    QObject::connect(node_field_widget, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldSpinBox::node_update_requested, this, &VisualShaderGraphicsScene::update_node_field_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_REPEATED_MESSAGE(field_number, sort_field_number, label) \
  if (true) { \
    const ProtoModel* node_field_model = visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number))); \
    RepeatedMessageModel* repeated_field_model = dynamic_cast<RepeatedMessageModel*>(const_cast<ProtoModel*>(node_field_model)); \
    CHECK_PARAM_NULLPTR_NON_VOID(repeated_field_model, false, "Repeated field model is not a RepeatedMessageModel."); \
    VisualShaderNodeFieldRepeatedMessage* node_field_widget = new VisualShaderNodeFieldRepeatedMessage(repeated_field_model, sort_field_number, n_id, field_number, params_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    add_parameter_row(params_layout, params_widget, QString(label), node_field_widget); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldRepeatedMessage::node_repeated_field_update_requested, this, &VisualShaderGraphicsScene::update_node_repeated_field_in_model); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldRepeatedMessage::node_repeated_field_update_requested, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldRepeatedMessage::node_repeated_field_row_add_requested, this, &VisualShaderGraphicsScene::on_node_repeated_field_row_add_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldRepeatedMessage::node_repeated_field_row_remove_requested, this, &VisualShaderGraphicsScene::on_node_repeated_field_row_remove_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldRepeatedMessage::node_repeated_field_sort_requested, this, &VisualShaderGraphicsScene::on_node_repeated_field_sort_requested); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_TRANSFORMATION(field_number) \
  if (true) { \
    const ProtoModel* transformation_model = visual_shader_model->get_sub_model(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number))); \
    const MessageModel* transformation_model_casted = dynamic_cast<const MessageModel*>(transformation_model); \
    if (transformation_model_casted != nullptr) { \
      VisualShaderNodeFieldTransformation* node_field_widget = new VisualShaderNodeFieldTransformation(transformation_model_casted, field_number, n_id, params_widget); \
      node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed); \
      node_field_widget->setContentsMargins(0, 0, 0, 0);  \
      add_parameter_row(params_layout, params_widget, QString("Transformation"), node_field_widget); \
      QObject::connect(node_field_widget, &VisualShaderNodeFieldTransformation::node_transformation_update_requested, this, &VisualShaderGraphicsScene::update_node_transformation_in_model); \
      QObject::connect(node_field_widget, &VisualShaderNodeFieldTransformation::node_transformation_update_requested, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    } \
  } else                                                                                                        \
    ((void)0)

#endif  // ENIGMA_VISUAL_SHADER_EDITOR_HPP
