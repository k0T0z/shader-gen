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
    : QOpenGLWidget(parent), shader_program(nullptr), VAO(0), VBO(0), render_timer(this), compile_debounce_timer(this) {
  setVisible(false);

  render_timer.setInterval(16); // ~60 FPS
  connect(&render_timer, &QTimer::timeout, this, QOverload<>::of(&RendererWidget::update));
  connect(&render_timer, &QTimer::timeout, this, &RendererWidget::scene_update_requested);

  compile_debounce_timer.setSingleShot(true);
  compile_debounce_timer.setInterval(300); // 300ms delay
  connect(&compile_debounce_timer, &QTimer::timeout, this, &RendererWidget::update_shader_program);
}

RendererWidget::~RendererWidget() {
  cleanup();
}

void RendererWidget::set_code(const std::string& new_code) {
  SILENT_CHECK_CONDITION_TRUE(this->code == new_code);

  this->code = new_code;

  if (!compile_debounce_timer.isActive()) compile_debounce_timer.start();
}

void RendererWidget::initializeGL() {
  CHECK_CONDITION_TRUE(!initializeOpenGLFunctions(), "Failed to initialize OpenGL functions");

  init_buffers();
  update_shader_program();

  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RendererWidget::cleanup);
}

void RendererWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void RendererWidget::paintGL() {
  SILENT_CHECK_CONDITION_TRUE(!isVisible());

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  CHECK_PARAM_NULLPTR(shader_program, "Shader program is null");
  
  CHECK_CONDITION_TRUE(!shader_program->bind(), "Failed to bind shader program");
  
  float time_value = timer.elapsed() * 0.001f; // Convert ms to seconds
  int utime_location = shader_program->uniformLocation("uTime");
  CHECK_CONDITION_TRUE(utime_location == -1, "Failed to get uniform location for uTime");
  shader_program->setUniformValue(utime_location, time_value);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  shader_program->release();
}

void RendererWidget::cleanup() {
  makeCurrent();
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  VAO = VBO = 0;
  doneCurrent();
  disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RendererWidget::cleanup);
}

void RendererWidget::init_buffers() {
  const float vertices[] = {
    -1.0f,  1.0f,  0.0f,  1.0f,
    -1.0f, -1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  0.0f
  };

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
  makeCurrent();  // Ensure OpenGL context is current for shader operations
  
  std::unique_ptr<QOpenGLShaderProgram> new_program = std::make_unique<QOpenGLShaderProgram>(this);
  
  const char* vertex_shader_source = R"(
      #version 430 core
      layout(location = 0) in vec2 aPos;
      layout(location = 1) in vec2 aFragCoord;

      out vec2 FragCoord;

      void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        FragCoord = aFragCoord;
      }
  )";

  std::string fragment_shader_source;
  if (code.empty()) {
    fragment_shader_source = R"(
        #version 430 core
        out vec4 FragColor;
        in vec2 FragCoord;

        uniform float uTime;

        void main() {
          FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    )";
  } else {
    fragment_shader_source = "#version 430 core\n\n" + code;
  }

  // Add shaders to new_program instead of shader_program
  if (!new_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source)) {
    WARN_PRINT("Vertex shader compilation failed:" + new_program->log().toStdString());
    doneCurrent();
    return;
  }
  
  if (!new_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source.c_str())) {
    WARN_PRINT("Fragment shader compilation failed:" + new_program->log().toStdString());
    doneCurrent();
    return;
  }
  
  if (!new_program->link()) {
    WARN_PRINT("Shader program linking failed:" + new_program->log().toStdString());
    doneCurrent();
    return;
  }
  
  shader_program.swap(new_program);
  
  doneCurrent();  // Release the context
}

void RendererWidget::showEvent(QShowEvent* event) {
  QOpenGLWidget::showEvent(event);
  render_timer.start();
  if (!timer.isValid()) timer.start();
}

void RendererWidget::hideEvent(QHideEvent* event) {
  QOpenGLWidget::hideEvent(event);
  if (timer.isValid()) timer.invalidate();
  render_timer.stop();
}
