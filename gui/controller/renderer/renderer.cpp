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

#include "gui/controller/renderer/renderer.hpp"

#include <array>

#include "error_macros.hpp"

RendererWidget::RendererWidget(QWidget* parent)
    : QOpenGLWidget(parent), shader_program(nullptr), VAO(0), VBO(0), render_timer(this), compile_debounce_timer(this) {
  render_timer.setInterval(32); // ~30 FPS
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
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  CHECK_PARAM_NULLPTR(shader_program, "Shader program is null");
  
  CHECK_CONDITION_TRUE(!shader_program->bind(), "Failed to bind shader program");
  
  const std::string time_uniform_name{shadergen_renderer::ShaderRenderer::kTimeUniformName};
  int utime_location = shader_program->uniformLocation(time_uniform_name.c_str());
  if (utime_location != -1) {
    float time_value = timer.elapsed() * 0.001f; // Convert ms to seconds
    shader_program->setUniformValue(utime_location, time_value);
  }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(shadergen_renderer::ShaderRenderer::kQuadVertexCount));
  glBindVertexArray(0);

  shader_program->release();
}

void RendererWidget::cleanup() {
  SILENT_CHECK_CONDITION_TRUE(!isInitialized());

  makeCurrent();
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  VAO = VBO = 0;
  doneCurrent();
  disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RendererWidget::cleanup);
}

void RendererWidget::init_buffers() {
  const std::array<float, shadergen_renderer::ShaderRenderer::kQuadFloatCount>& vertices{
      shadergen_renderer::ShaderRenderer::quad_vertices()};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  /// Check for OpenGL errors after resource allocation
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    ERROR_PRINT("Failed to generate VAO or VBO: OpenGL error " + std::to_string(err));
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return;
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

  // Check for errors after buffer setup
  err = glGetError();
  if (err != GL_NO_ERROR) {
    ERROR_PRINT("Failed to set up VBO: OpenGL error " + std::to_string(err));
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glBindVertexArray(0);
    return;
  }

  // Position attribute (x, y)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // FragCoord attribute (u, v)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Unbind both VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  err = glGetError();
  if (err != GL_NO_ERROR) {
    ERROR_PRINT("Error setting vertex attributes: OpenGL error " + std::to_string(err));
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return;
  }
}

void RendererWidget::update_shader_program() {
  SILENT_CHECK_CONDITION_TRUE(!isInitialized());

  makeCurrent();  // Ensure OpenGL context is current for shader operations
  
  std::unique_ptr<QOpenGLShaderProgram> new_program = std::make_unique<QOpenGLShaderProgram>(this);

  const std::string vertex_shader_source{shadergen_renderer::ShaderRenderer::vertex_shader_source()};

  // The generated code already carries the version directive's body (see
  // ShaderRenderer::fragment_shader_source); an empty graph falls back to the default shader.
  const std::string fragment_shader_source{
      code.empty() ? shadergen_renderer::ShaderRenderer::default_fragment_shader_source()
                   : shadergen_renderer::ShaderRenderer::fragment_shader_source(code)};

  // Add shaders to new_program instead of shader_program
  if (!new_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source.c_str())) {
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

  if (!render_timer.isActive()) render_timer.start();
  else {
    render_timer.stop();
    render_timer.start();
  }

  if (!timer.isValid()) timer.start();
}

void RendererWidget::hideEvent(QHideEvent* event) {
  QOpenGLWidget::hideEvent(event);
  
  if (render_timer.isActive()) render_timer.stop();
  if (timer.isValid()) timer.invalidate();
}
