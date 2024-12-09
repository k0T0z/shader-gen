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
#include <QOpenGLWidget>
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
#include <QOpenGLFunctions_4_3_Core>  // https://stackoverflow.com/a/64288966/14629018 explains why we need this.
#include <QOpenGLShaderProgram>

#include <string>
#include <variant>
#include <vector>

// #include "generator/visual_shader.hpp"
// #include "generator/visual_shader_nodes.hpp"
// #include "generator/vs_noise_nodes.hpp"

#include "gui/model/message_model.hpp"
#include "gui/model/repeated_message_model.hpp"

#include "gui/controller/graph_node.hpp"

class VisualShaderGraphicsScene;
class VisualShaderGraphicsView;
class VisualShaderNodeGraphicsObject;
class VisualShaderConnectionGraphicsObject;
class CreateNodeDialog;
class VisualShaderInputPortGraphicsObject;
class VisualShaderOutputPortGraphicsObject;

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
    IGRAPHNODE_SHARED_PTR_USER_ROLE = Qt::UserRole + 1,
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
    std::shared_ptr<IGraphNode> graph_node;

    CreateNodeDialogNodesTreeItem(const std::string& caption, const std::string& category_path,
                                  const std::string& description, const std::shared_ptr<IGraphNode>& graph_node)
        : caption(caption), category_path(category_path), description(description), graph_node(graph_node) {}
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

  void create_node(const QPointF& coordinate);

  void add_node(QTreeWidgetItem* selected_item, const QPointF& coordinate);

  void show_create_node_dialog(const QPointF& coordinate);

  std::vector<std::string> parse_node_category_path(const std::string& node_category_path);
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
  ~CreateNodeDialog();

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
/*****                  ShaderPreviewerWidget                     *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/**
 * @brief This class is meant to be a temporary solution to preview the shader
 *        code. We should preview the shader code using ENIGMA's Graphics System.
 * 
 * @todo Replace this class with ENIGMA's Graphics System.
 * 
 */
class ShaderPreviewerWidget : public QOpenGLWidget {
  Q_OBJECT

 public:
  ShaderPreviewerWidget(QWidget* parent = nullptr);
  ~ShaderPreviewerWidget() override;

  void set_code(const std::string& code);

 Q_SIGNALS:
  void scene_update_requested();

 protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;

 private:
  std::unique_ptr<QOpenGLShaderProgram> shader_program;
  GLuint VAO, VBO;
  QElapsedTimer timer;

  std::string code;
  bool shader_needs_update{false};

  void init_shaders();
  void init_buffers();
  void update_shader_program();

  /**
     * @brief Cleans up the OpenGL resources.
     * 
     * @note This function is called automatically when the widget is destroyed.
     *       It is connected @c QOpenGLContext::aboutToBeDestroyed signal.
     * 
     * @note DON'T call this function in the destructor as it is 
     *       called automatically. If you call it from the destructor,
     *       it will crash as @c makeCurrent() won't be able to make the
     *       context current.
     * 
     */
  void cleanup();
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

  ~VisualShaderGraphicsScene();

  bool add_node(const std::shared_ptr<IGraphNode>& graph_node, const QPointF& coordinate);

  bool delete_node(const int& n_id, const int& in_port_count, const int& out_port_count);

  void set_model(ProtoModel* visual_shader_model) { this->visual_shader_model = visual_shader_model; }
  void set_nodes_model(ProtoModel* nodes_model) { this->nodes_model = nodes_model; }
  void set_connections_model(ProtoModel* connections_model) { this->connections_model = connections_model; }

  VisualShaderEditor* get_editor() const { return editor; }
  void set_editor(VisualShaderEditor* editor) const { this->editor = editor; }

  /**
     * @brief 
     * 
     * @note This function sets the @c temporary_connection_graphics_object if 
     *       we have a valid @c from_node_id and @c from_port_index only. Then it 
     *       resets it again if we have a valid @c to_node_id and @c to_port_index and 
     *       this is important because inside the drag and drop event, we need to know
     *       if we have a valid temporary connection or not.
     * 
     * @param from_node_id 
     * @param from_port_index 
     * @param to_node_id 
     * @param to_port_index 
     * @return true 
     * @return false 
     */
  bool add_connection(const int& from_node_id, const int& from_port_index, const int& to_node_id = -1,
                      const int& to_port_index = -1);

  bool delete_connection(const int& c_id, const int& from_node_id, const int& from_port_index,
                         const int& to_node_id = -1, const int& to_port_index = -1);

  VisualShaderNodeGraphicsObject* get_node_graphics_object(const int& n_id) const;

  static int get_new_node_id(ProtoModel* visual_shader_model, ProtoModel* nodes_model);
  static int get_new_connection_id(ProtoModel* visual_shader_model, ProtoModel* connections_model);
  static int find_node_entry(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id);
  static int find_connection_entry(ProtoModel* visual_shader_model, ProtoModel* connections_model, const int& c_id);

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

  /**
   * @brief Updates the code inside all the nodes except the output node.
   * 
   */
  void on_update_shader_previewer_widgets_requested();

  void on_scene_update_requested();

  void on_in_port_remove_requested(VisualShaderInputPortGraphicsObject* in_port);
  void on_out_port_remove_requested(VisualShaderOutputPortGraphicsObject* out_port);

 private:
  // VisualShader* vs;
  mutable VisualShaderEditor* editor;

  std::unordered_map<int, VisualShaderNodeGraphicsObject*> node_graphics_objects;

  VisualShaderConnectionGraphicsObject* temporary_connection_graphics_object;

  // Main Model
  ProtoModel* visual_shader_model;

  // Sub-Models
  ProtoModel* nodes_model;
  ProtoModel* connections_model;

  void remove_item(QGraphicsItem* item);
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
  VisualShaderNodeGraphicsObject(const int& n_id, const QPointF& coordinate, const std::string& caption,
                                 const std::vector<std::string>& in_port_captions,
                                 const std::vector<std::string>& out_port_captions, QGraphicsItem* parent = nullptr);
  ~VisualShaderNodeGraphicsObject();

  VisualShaderInputPortGraphicsObject* get_input_port_graphics_object(const int& p_index) const;
  VisualShaderOutputPortGraphicsObject* get_output_port_graphics_object(const int& p_index) const;

  QWidget* get_embed_widget() const { return embed_widget; }
  void set_embed_widget(QWidget* embed_widget) { this->embed_widget = embed_widget; }

  ShaderPreviewerWidget* get_shader_previewer_widget() const { return shader_previewer_widget; }

 public Q_SLOTS:
  void on_node_update_requested();

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

  const std::string caption;
  const int in_port_count;
  const int out_port_count;
  const std::vector<std::string> in_port_captions;
  const std::vector<std::string> out_port_captions;

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

  float caption_h_padding = 10.0f;
  float caption_v_padding = 8.0f;

  mutable float rect_width;           // Calculated in boundingRect()
  mutable float caption_rect_height;  // Calculated in boundingRect()

  mutable float rect_height;  // Calculated in boundingRect()
  float body_rect_header_height = 30.0f;
  float body_rect_port_step = 40.0f;
  float body_rect_footer_height = 30.0f;

  mutable float rect_padding;  // Calculated in boundingRect()
  mutable float rect_margin;   // Calculated in boundingRect()

  float port_caption_spacing = 4.0f;  // Distance between the port and its caption

  // Ports Style
  float connected_port_diameter = 8.0f;
  float unconnected_port_diameter = 6.0f;

  // Caption
  float caption_font_size = 18.0f;
  float port_caption_font_size = 8.0f;

  QWidget* embed_widget;
  float embed_widget_h_padding = 15.0f;
  float embed_widget_v_padding = 5.0f;

  OriginalMatchingImageWidget* matching_image_widget;
  float spacing_between_output_node_and_matching_image = 10.0f;

  ShaderPreviewerWidget* shader_previewer_widget;
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
};

class VisualShaderInputPortGraphicsObject : public QGraphicsObject {
  Q_OBJECT

 public:
  VisualShaderInputPortGraphicsObject(const QRectF& rect, const int& n_id, const int& p_index,
                                      QGraphicsItem* parent = nullptr);
  ~VisualShaderInputPortGraphicsObject();

  QPointF get_global_coordinate() const { return mapToScene(rect.center()); }

  int get_node_id() const { return n_id; }
  int get_port_index() const { return p_index; }

  VisualShaderConnectionGraphicsObject* get_connection_graphics_object() const { return connection_graphics_object; }
  void connect(VisualShaderConnectionGraphicsObject* c_g_o) const { this->connection_graphics_object = c_g_o; }
  void detach_connection() const { this->connection_graphics_object = nullptr; }
  bool is_connected() const { return connection_graphics_object != nullptr; }

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
  int n_id;
  int p_index;
  QRectF rect;

  mutable VisualShaderConnectionGraphicsObject* connection_graphics_object;

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
                                       QGraphicsItem* parent = nullptr);
  ~VisualShaderOutputPortGraphicsObject();

  QPointF get_global_coordinate() const { return mapToScene(rect.center()); }

  int get_node_id() const { return n_id; }
  int get_port_index() const { return p_index; }

  std::vector<VisualShaderConnectionGraphicsObject*> get_connection_graphics_objects() const {
    return connection_graphics_objects;
  }
  VisualShaderConnectionGraphicsObject* get_connection_graphics_object(const int& to_node_id,
                                                                       const int& to_port_index) const;
  void connect(VisualShaderConnectionGraphicsObject* c_o) { this->connection_graphics_objects.emplace_back(c_o); }
  void detach_connection(VisualShaderConnectionGraphicsObject* c_o) {
    connection_graphics_objects.erase(
        std::remove(connection_graphics_objects.begin(), connection_graphics_objects.end(), c_o),
        connection_graphics_objects.end());
  }
  bool is_connected() const { return connection_graphics_objects.size() > 0; }

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
  int n_id;
  int p_index;
  QRectF rect;

  // An output port can have multiple connections.
  std::vector<VisualShaderConnectionGraphicsObject*> connection_graphics_objects;

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
  ~VisualShaderConnectionGraphicsObject();

  int get_id() const { return c_id; }

  int get_from_node_id() const { return from_n_id; }
  int get_from_port_index() const { return from_p_index; }

  int get_to_node_id() const { return to_n_id; }
  int get_to_port_index() const { return to_p_index; }

  void detach_end() const {
    this->set_to_node_id(-1);
    this->set_to_port_index(-1);
  }

  void set_to_node_id(const int& to_n_id) const { this->to_n_id = to_n_id; }
  void set_to_port_index(const int& to_p_index) const { this->to_p_index = to_p_index; }

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
  mutable int to_n_id;
  int from_p_index;
  mutable int to_p_index;

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

  mutable float rect_padding;  // Calculated in boundingRect()

  float h_abnormal_offset = 50.0f;
  float v_abnormal_offset = 40.0f;
  float abnormal_face_to_back_control_width_expansion_factor = 0.5f;
  float abnormal_face_to_back_control_height_expansion_factor = 1.0f;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  int detect_quadrant(const QPointF& reference, const QPointF& target) const;
  QRectF calculate_bounding_rect_from_coordinates(const QPointF& start_coordinate, const QPointF& end_coordinate) const;
  std::pair<QPointF, QPointF> calculate_control_points(const QPointF& start_coordinate,
                                                       const QPointF& end_coordinate) const;
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*****                                                            *****/
/*****                 Embed Widgets                              *****/
/*****                                                            *****/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

class VisualShaderNodeEmbedWidget : public QWidget {
  Q_OBJECT

 public:
  VisualShaderNodeEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                              const std::shared_ptr<IGraphNode>& graph_node, QWidget* parent = nullptr);
  ~VisualShaderNodeEmbedWidget() = default;

  void set_shader_previewer_widget(QWidget* shader_previewer_widget) {
    this->shader_previewer_widget = shader_previewer_widget;
  }

 Q_SIGNALS:
  void shader_preview_update_requested();
  void node_update_requested();

 private Q_SLOTS:
  void on_preview_shader_button_pressed() {
    bool is_visible{shader_previewer_widget->isVisible()};
    shader_previewer_widget->setVisible(!is_visible);
    preview_shader_button->setText(!is_visible ? "Hide Preview" : "Show Preview");
  }

  void on_shader_preview_update_requested() { Q_EMIT shader_preview_update_requested(); }

  void on_node_update_requested() { Q_EMIT node_update_requested(); }

 private:
  QVBoxLayout* layout;

  QPushButton* preview_shader_button;

  QWidget* shader_previewer_widget;
};

/*************************************/
/* Input Node                        */
/*************************************/

class VisualShaderNodeInputEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeInputEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                   const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeInputEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

/*************************************/
/* CONSTANTS                         */
/*************************************/

class VisualShaderNodeFloatConstantEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeFloatConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                           const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeFloatConstantEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeIntConstantEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeIntConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                         const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeIntConstantEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeUIntConstantEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeUIntConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                          const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeUIntConstantEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeColorConstantEmbedWidget : public QPushButton {
  Q_OBJECT

 public:
  VisualShaderNodeColorConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                           const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeColorConstantEmbedWidget() = default;

 Q_SIGNALS:
  void color_changed();

 private Q_SLOTS:
  void on_pressed();

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeBooleanConstantEmbedWidget : public QCheckBox {
  Q_OBJECT

 public:
  VisualShaderNodeBooleanConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                             const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeBooleanConstantEmbedWidget() = default;

 private Q_SLOTS:
  void on_state_changed(const int& state);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeVec2ConstantEmbedWidget : public QVBoxLayout {
  Q_OBJECT

 public:
  VisualShaderNodeVec2ConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                          const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVec2ConstantEmbedWidget() = default;

  QLineEdit* get_x_edit_widget() const { return x_edit_widget; }
  QLineEdit* get_y_edit_widget() const { return y_edit_widget; }

 private Q_SLOTS:

  void on_x_text_changed(const QString& text);
  void on_y_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;

  QLineEdit* x_edit_widget;
  QLineEdit* y_edit_widget;
};

class VisualShaderNodeVec3ConstantEmbedWidget : public QVBoxLayout {
  Q_OBJECT

 public:
  VisualShaderNodeVec3ConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                          const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVec3ConstantEmbedWidget() = default;

  QLineEdit* get_x_edit_widget() const { return x_edit_widget; }
  QLineEdit* get_y_edit_widget() const { return y_edit_widget; }
  QLineEdit* get_z_edit_widget() const { return z_edit_widget; }

 private Q_SLOTS:

  void on_x_text_changed(const QString& text);
  void on_y_text_changed(const QString& text);
  void on_z_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;

  QLineEdit* x_edit_widget;
  QLineEdit* y_edit_widget;
  QLineEdit* z_edit_widget;
};

class VisualShaderNodeVec4ConstantEmbedWidget : public QVBoxLayout {
  Q_OBJECT

 public:
  VisualShaderNodeVec4ConstantEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                          const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVec4ConstantEmbedWidget() = default;

  QLineEdit* get_x_edit_widget() const { return x_edit_widget; }
  QLineEdit* get_y_edit_widget() const { return y_edit_widget; }
  QLineEdit* get_z_edit_widget() const { return z_edit_widget; }
  QLineEdit* get_w_edit_widget() const { return w_edit_widget; }

 private Q_SLOTS:

  void on_x_text_changed(const QString& text);
  void on_y_text_changed(const QString& text);
  void on_z_text_changed(const QString& text);
  void on_w_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;

  QLineEdit* x_edit_widget;
  QLineEdit* y_edit_widget;
  QLineEdit* z_edit_widget;
  QLineEdit* w_edit_widget;
};

/*************************************/
/* OPERATORS                         */
/*************************************/

class VisualShaderNodeFloatOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeFloatOpEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                     const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeFloatOpEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeIntOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeIntOpEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                   const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeIntOpEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeUIntOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeUIntOpEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                    const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeUIntOpEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeVectorBaseEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeVectorBaseEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                        const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVectorBaseEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeVectorOpEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeVectorOpEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                      const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVectorOpEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

/*************************************/
/* Funcs Node                        */
/*************************************/

class VisualShaderNodeFloatFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeFloatFuncEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                       const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeFloatFuncEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeIntFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeIntFuncEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                     const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeIntFuncEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeUIntFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeUIntFuncEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                      const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeUIntFuncEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeVectorFuncEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeVectorFuncEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                        const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVectorFuncEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

/*************************************/
/* Value Noise Node                  */
/*************************************/

class VisualShaderNodeValueNoiseEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeValueNoiseEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                        const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeValueNoiseEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

/*************************************/
/* Perlin Noise Node                 */
/*************************************/

class VisualShaderNodePerlinNoiseEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodePerlinNoiseEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                         const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodePerlinNoiseEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

/*************************************/
/* Voronoi Noise Node                */
/*************************************/

class VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model,
                                                     const int& n_id, const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVoronoiNoiseAngleOffsetEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget : public QLineEdit {
  Q_OBJECT

 public:
  VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model,
                                                     const int& n_id, const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeVoronoiNoiseCellDensityEmbedWidget() = default;

 private Q_SLOTS:
  void on_text_changed(const QString& text);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

/*************************************/
/* MISC                              */
/*************************************/

class VisualShaderNodeCompareEmbedWidget : public QVBoxLayout {
  Q_OBJECT

 public:
  VisualShaderNodeCompareEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                     const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeCompareEmbedWidget() = default;

  QComboBox* get_comparison_type_combo_box() const { return comparison_type_combo_box; }
  QComboBox* get_func_combo_box() const { return func_combo_box; }
  QComboBox* get_condition_combo_box() const { return condition_combo_box; }

 private Q_SLOTS:
  void on_comparison_type_current_index_changed(const int& index);
  void on_func_current_index_changed(const int& index);
  void on_condition_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;

  QComboBox* comparison_type_combo_box;
  QComboBox* func_combo_box;
  QComboBox* condition_combo_box;
};

class VisualShaderNodeSwitchEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeSwitchEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                    const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeSwitchEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

class VisualShaderNodeIsEmbedWidget : public QComboBox {
  Q_OBJECT

 public:
  VisualShaderNodeIsEmbedWidget(ProtoModel* visual_shader_model, ProtoModel* nodes_model, const int& n_id,
                                const std::shared_ptr<IGraphNode>& graph_node);
  ~VisualShaderNodeIsEmbedWidget() = default;

  void set_current_index(const int& index) { this->setCurrentIndex(index); }

 private Q_SLOTS:
  void on_current_index_changed(const int& index);

 private:
  ProtoModel* visual_shader_model;
  ProtoModel* nodes_model;
  int n_id;
  std::shared_ptr<IGraphNode> graph_node;
};

#endif  // ENIGMA_VISUAL_SHADER_EDITOR_HPP
