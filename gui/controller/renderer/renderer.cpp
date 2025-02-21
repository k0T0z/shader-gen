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

#include "gui/controller/renderer/renderer.hpp"

#include "error_macros.hpp"

RendererWidget::RendererWidget(QWidget* parent)
    : QOpenGLWidget(parent), shader_program(nullptr), VAO(0), VBO(0) {}

RendererWidget::~RendererWidget() {
  cleanup(); 
}

void RendererWidget::set_code(const std::string& new_code) {
  if (new_code == code) return;

  code = new_code;
  shader_needs_update = true;
  if (isVisible()) {
    update_shader_program();
    timer.restart();
  }
}

void RendererWidget::initializeGL() {
  CHECK_CONDITION_TRUE(!context() || !initializeOpenGLFunctions(), "Failed to initialize OpenGL functions");

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background
  init_buffers();
  init_shaders();

  timer.start();

  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RendererWidget::cleanup);
}

void RendererWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void RendererWidget::paintGL() {
  if (!isVisible()) return;

  // Ensure we have working shaders
  if (!shader_program || !shader_program->isLinked()) {
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);  // Error magenta
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }

  if (shader_needs_update) {
    update_shader_program();
  }

  float time_value = timer.elapsed() * 0.001f;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  shader_program->bind();
  shader_program->setUniformValue("uTime", time_value);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  shader_program->release();

  if (isVisible()) {
    update();  // Request a repaint
    Q_EMIT scene_update_requested();
  }
}

void RendererWidget::cleanup() {
  if (!context() || !context()->isValid()) return;

  makeCurrent();
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  VAO = VBO = 0;
  shader_program.reset();  // Destroy shader program while context is valid
  doneCurrent();
}

void RendererWidget::init_buffers() {
  float vertices[] = {
      // coordinates    // frag coords
      -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  Q_ASSERT(VAO && VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void RendererWidget::update_shader_program() {
  auto new_program = std::make_unique<QOpenGLShaderProgram>();

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

  CHECK_CONDITION_TRUE(!shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source), "Vertex shader compilation failed:" + shader_program->log().toStdString());
  CHECK_CONDITION_TRUE(!shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source.c_str()), "Fragment shader compilation failed:" + shader_program->log().toStdString());
  CHECK_CONDITION_TRUE(!shader_program->link(), "Shader program linking failed:" + shader_program->log().toStdString());
  
  // Atomic swap only if successful
  makeCurrent();
  shader_program.swap(new_program);
  doneCurrent();
  
  shader_needs_update = false;
}

void RendererWidget::init_shaders() { update_shader_program(); }

void RendererWidget::showEvent(QShowEvent* event) {
  QOpenGLWidget::showEvent(event);
  if (!timer.isValid()) {
    timer.start();  // Start the timer on first show
    update();
  }
}

void RendererWidget::hideEvent(QHideEvent* event) {
  QOpenGLWidget::hideEvent(event);
  timer.invalidate();
  cleanup();
}
