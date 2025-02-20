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

#ifndef ENIGMA_VISUAL_SHADER_EDITOR_HPP
#define ENIGMA_VISUAL_SHADER_EDITOR_HPP

#include <QContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
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

using EnumDescriptor = google::protobuf::EnumDescriptor;

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
  ~VisualShaderEditor() override = default;

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

 private:
  QHBoxLayout* layout;

  QWidget* side_widget;
  QVBoxLayout* side_outer_layout;
  QVBoxLayout* side_layout;
  QLineEdit* name_edit;
  QPushButton* save_button;

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

  /**
   * @brief Initializes the UI
   * 
   */
  void init();

  bool add_output_node();
  void load_graph();

  void create_node(const QPointF& coordinate);

  void add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate);

  void show_create_node_dialog(const QPointF& coordinate);

  std::vector<std::string> parse_node_category_path(const std::string& n_category_path);
  QTreeWidgetItem* find_or_create_category_item(QTreeWidgetItem* parent, const std::string& category,
                                                const std::string& category_path,
                                                QTreeWidget* create_node_dialog_nodes_tree,
                                                std::unordered_map<std::string, QTreeWidgetItem*>& category_path_map);
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

  ~VisualShaderGraphicsScene() override = default;

  bool add_node_to_model(const int& n_id, const std::shared_ptr<IVisualShaderProtoNode>& proto_node,
                         const QPointF& coordinate);
  bool add_node_to_scene(const int& n_id, const std::shared_ptr<IVisualShaderProtoNode>& proto_node,
                         const QPointF& coordinate);
  bool add_node(const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const QPointF& coordinate, const int& n_id = -1);

  bool delete_node_from_model(const int& n_id);
  bool delete_node_from_scene(const int& n_id, const int& in_port_count, const int& out_port_count);
  bool delete_node(const int& n_id, const int& in_port_count, const int& out_port_count);

  bool update_node_in_model(const int& n_id, const int& field_number, const QVariant& value);
  bool update_node_in_scene(const int& n_id, const int& field_number, const QVariant& value);
  bool update_node(const int& n_id, const int& field_number, const QVariant& value);

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

  bool is_valid_connection(const int& from_node_id, const int& from_port_index, const int& to_node_id, const int& to_port_index) const;

  bool delete_connection_from_model(const int& c_id);
  bool delete_connection_from_scene(const int& c_id, const int& from_node_id, const int& from_port_index,
                                    const int& to_node_id, const int& to_port_index);
  bool delete_connection(const int& c_id, const int& from_node_id, const int& from_port_index,
                         const int& to_node_id, const int& to_port_index);

  bool add_temporary_connection(const int& from_node_id, const int& from_port_index);
  bool delete_temporary_connection(const int& from_node_id, const int& from_port_index);
  bool convert_to_temporary_connection(const int& c_id, const int& from_node_id, const int& from_port_index);

  bool update_connection_in_model(const int& c_id, const int& node_id_field_number, const int& port_index_field_number, const int& node_id, const int& port_index);
  bool update_connection_in_scene(const int& c_id, const int& node_id_field_number, const int& node_id, const int& port_index);
  bool update_connection(const int& c_id, const int& node_id_field_number, const int& port_index_field_number, const int& node_id, const int& port_index);

  VisualShaderNodeGraphicsObject* get_node_graphics_object(const int& n_id) const;
  VisualShaderConnectionGraphicsObject* get_connection_graphics_object(const int& c_id) const;

  static int get_new_node_id(ProtoModel* visual_shader_model, ProtoModel* nodes_model);
  static int get_new_connection_id(ProtoModel* visual_shader_model, ProtoModel* connections_model);
  static int find_node_entry(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id);
  static int find_connection_entry(ProtoModel* visual_shader_model, ProtoModel* connections_model, const int& c_id);
  static int get_node_type_field_number(ProtoModel* nodes_model, const int& n_id);

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
  void on_node_moved(const int& n_id, const int& in_port_count, const int& out_port_count,
                     const QPointF& new_coordinate);

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

 private:
  VisualShaderEditor* editor;

  std::unordered_map<int, VisualShaderNodeGraphicsObject*> node_graphics_objects;
  std::unordered_map<int, VisualShaderConnectionGraphicsObject*> connection_graphics_objects;

  VisualShaderConnectionGraphicsObject* temporary_connection_graphics_object;

  // n_id -> field number -> QWidget*
  std::unordered_map<int, std::unordered_map<int, QWidget*>> node_field_widgets;

  // Main Model
  ProtoModel* visual_shader_model;

  // Sub-Models
  ProtoModel* nodes_model;
  ProtoModel* connections_model;

  void remove_item(QGraphicsItem* item);
  bool check_if_connection_out_of_bounds(VisualShaderOutputPortGraphicsObject* from_o_port, VisualShaderInputPortGraphicsObject* to_i_port);
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

  VisualShaderInputPortGraphicsObject* get_input_port_graphics_object(const int& p_index) const;
  VisualShaderOutputPortGraphicsObject* get_output_port_graphics_object(const int& p_index) const;

  QWidget* get_embed_widget() const { return embed_widget; }
  void set_embed_widget(QWidget* embed_widget) { this->embed_widget = embed_widget; }

  RendererWidget* get_renderer_widget() const { return renderer_widget; }

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
  void node_moved(const int& n_id, const int& in_port_count, const int& out_port_count, const QPointF& new_coordinate);

  void in_port_pressed(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);
  void in_port_dragged(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);
  void in_port_dropped(VisualShaderInputPortGraphicsObject* port, const QPointF& coordinate);

  void out_port_pressed(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);
  void out_port_dragged(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);
  void out_port_dropped(VisualShaderOutputPortGraphicsObject* port, const QPointF& coordinate);

  void scene_update_requested();

  void in_port_remove_requested(VisualShaderInputPortGraphicsObject* in_port);
  void out_port_remove_requested(VisualShaderOutputPortGraphicsObject* out_port);

 public Q_SLOTS:
  void on_preview_shader_button_pressed();

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
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
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
  const VisualShaderNodePortType port_type;

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
  const VisualShaderNodePortType port_type;

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

class VisualShaderNodeFieldComboBox : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeFieldComboBox(const QVariant& initial_value, const int& n_id,
                                   const std::shared_ptr<IVisualShaderProtoNode>& proto_node,
                                    const EnumDescriptor* enum_descriptor, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldComboBox() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  int n_id;
  std::shared_ptr<IVisualShaderProtoNode> proto_node;
  int field_number;
};

class VisualShaderNodeFieldLineEditFloat : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFieldLineEditFloat(const QVariant& initial_value, const int& n_id,
                                           const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldLineEditFloat() = default;

  void set_current_text(const std::string& text) { this->setText(QString::fromStdString(text)); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  int n_id;
  std::shared_ptr<IVisualShaderProtoNode> proto_node;
  int field_number;
};

class VisualShaderNodeFieldLineEditInt : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFieldLineEditInt(const QVariant& initial_value, const int& n_id,
                                           const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldLineEditInt() = default;

  void set_current_text(const std::string& text) { this->setText(QString::fromStdString(text)); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  int n_id;
  std::shared_ptr<IVisualShaderProtoNode> proto_node;
  int field_number;
};

class VisualShaderNodeFieldLineEditUInt : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFieldLineEditUInt(const QVariant& initial_value, const int& n_id,
                                           const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldLineEditUInt() = default;

  void set_current_text(const std::string& text) { this->setText(QString::fromStdString(text)); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  int n_id;
  std::shared_ptr<IVisualShaderProtoNode> proto_node;
  int field_number;
};

class VisualShaderNodeFieldCheckBox : public QCheckBox {
  Q_OBJECT

 public:
  VisualShaderNodeFieldCheckBox(const QVariant& initial_value, const int& n_id,
                                             const std::shared_ptr<IVisualShaderProtoNode>& proto_node, const int& field_number, QWidget* parent = nullptr);
  ~VisualShaderNodeFieldCheckBox() = default;

  void set_checked(const bool& checked) { this->setChecked(checked); }

 Q_SIGNALS:
  void node_update_requested(const int& n_id, const int& field_number, const QVariant& value);

 private Q_SLOTS:
  void on_state_changed(const int& state);

 private:
  int n_id;
  std::shared_ptr<IVisualShaderProtoNode> proto_node;
  int field_number;
};

#define REGISTER_NODE_FIELD_COMBO_BOX(enum_descriptor, field_number) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldComboBox* node_field_widget = new VisualShaderNodeFieldComboBox(initial_value, n_id, proto_node, enum_descriptor, field_number, embed_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    embed_widget_layout->addWidget(node_field_widget); \
    QObject::connect(node_field_widget, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldComboBox::node_update_requested, this, &VisualShaderGraphicsScene::update_node_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_LINE_EDIT_FLOAT(field_number, placeholder) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldLineEditFloat* node_field_widget = new VisualShaderNodeFieldLineEditFloat(initial_value, n_id, proto_node, field_number, embed_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    embed_widget_layout->addWidget(node_field_widget); \
    node_field_widget->setPlaceholderText(placeholder); \
    QObject::connect(node_field_widget, &QLineEdit::textChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldLineEditFloat::node_update_requested, this, &VisualShaderGraphicsScene::update_node_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_LINE_EDIT_INT(field_number, placeholder) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldLineEditInt* node_field_widget = new VisualShaderNodeFieldLineEditInt(initial_value, n_id, proto_node, field_number, embed_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    embed_widget_layout->addWidget(node_field_widget); \
    node_field_widget->setPlaceholderText(placeholder); \
    QObject::connect(node_field_widget, &QLineEdit::textChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldLineEditInt::node_update_requested, this, &VisualShaderGraphicsScene::update_node_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_LINE_EDIT_UINT(field_number, placeholder) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldLineEditUInt* node_field_widget = new VisualShaderNodeFieldLineEditUInt(initial_value, n_id, proto_node, field_number, embed_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    embed_widget_layout->addWidget(node_field_widget); \
    node_field_widget->setPlaceholderText(placeholder); \
    QObject::connect(node_field_widget, &QLineEdit::textChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldLineEditUInt::node_update_requested, this, &VisualShaderGraphicsScene::update_node_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#define REGISTER_NODE_FIELD_CHECK_BOX(field_number) \
  if (true) { \
    QVariant initial_value{visual_shader_model->data(FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), \
      FieldPath::RepeatedAt(row_entry), \
      FieldPath::FieldNumber(node_type_field_number), \
      FieldPath::FieldNumber(field_number)))}; \
    VisualShaderNodeFieldCheckBox* node_field_widget = new VisualShaderNodeFieldCheckBox(initial_value, n_id, proto_node, field_number, embed_widget); \
    node_field_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); \
    node_field_widget->setContentsMargins(0, 0, 0, 0);  \
    embed_widget_layout->addWidget(node_field_widget); \
    QObject::connect(node_field_widget, &QCheckBox::stateChanged, this, &VisualShaderGraphicsScene::on_update_renderer_widgets_requested); \
    QObject::connect(node_field_widget, &VisualShaderNodeFieldCheckBox::node_update_requested, this, &VisualShaderGraphicsScene::update_node_in_model); \
    node_field_widgets[n_id][field_number] = node_field_widget; \
  } else                                                                                                        \
    ((void)0)

#endif  // ENIGMA_VISUAL_SHADER_EDITOR_HPP
