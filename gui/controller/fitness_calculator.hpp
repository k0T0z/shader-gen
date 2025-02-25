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

#ifndef SHADER_GEN_AI_AGENT_FITNESS_CALCULATOR_HPP
#define SHADER_GEN_AI_AGENT_FITNESS_CALCULATOR_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QImage>
#include <QtOpenGLWidgets/QOpenGLWidget>
// #include <QOpenGLFunctions>
#include <QTimer>
#include <QElapsedTimer>
#include <QtOpenGL/QOpenGLFunctions_4_3_Core>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

class CurrentOutputRenderer;

class AIAgentFitnessCalculator : public QWidget {
  Q_OBJECT

 public:
  AIAgentFitnessCalculator(QWidget* parent = nullptr);
  ~AIAgentFitnessCalculator() override = default;

  void update_current_output(const std::string& code);

  double get_fitness_value() const;

 private Q_SLOTS:
  void on_load_image_button_pressed();

 private:
  QVBoxLayout* layout;

  QHBoxLayout* menu_bar;
  QPushButton* load_image_button;
  QComboBox* matching_type_combo_box;

  QHBoxLayout* outputs_layout;

  QVBoxLayout* curent_output_renderer_layout;
  QLabel* current_output_renderer_label;
  CurrentOutputRenderer* current_output_renderer;

  QVBoxLayout* target_output_layout;
  QLabel* target_output_label;
  QLabel* target_output;

  QImage target_image; // On every load, the target image is stored here


  /**
   * @brief Initializes the UI
   * 
   */
  void init();
};

class CurrentOutputRenderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core  {
 Q_OBJECT
  
 public:
  CurrentOutputRenderer(QWidget* parent = nullptr);
  ~CurrentOutputRenderer() override;
 
  void set_code(const std::string& code);
  
  QImage get_pixel_data() const;

 private Q_SLOTS:
  void update_shader_program();

 private:
  std::unique_ptr<QOpenGLShaderProgram> shader_program;
  GLuint VAO, VBO;

  QTimer render_timer;
  QElapsedTimer timer;

  /**
   * @brief We use this timer to debounce the compilation of 
   *        the shader code. Too fast changes in the graph 
   *        editor aren't worth compiling.
   * 
   */
  QTimer compile_debounce_timer;

  std::string code;

  // For rendering to FBO and displaying the texture
  QOpenGLFramebufferObject* fbo;
  QOpenGLShaderProgram* texture_shader_program;

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;

  void init_buffers();

  void cleanup();
};

#endif  // SHADER_GEN_AI_AGENT_FITNESS_CALCULATOR_HPP
  