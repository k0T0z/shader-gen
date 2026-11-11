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

#include "gui/controller/shader_sampler.hpp"

#include <array>

#include "error_macros.hpp"
#include "generator/visual_shader_generator.hpp"

ShaderSampler::ShaderSampler() : context(nullptr),
                                 surface(nullptr),
                                 fbo(nullptr),
                                 VAO(0),
                                 VBO(0) {}

ShaderSampler::~ShaderSampler() {
  if (isInitialized()) {
    CHECK_CONDITION_TRUE(!context->makeCurrent(surface), "Failed to make OpenGL context current");
    delete fbo;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    VAO = VBO = 0;
    context->doneCurrent();
  }
  delete surface;
  delete context;
}

bool ShaderSampler::initialize() {
  context = new QOpenGLContext();
  CHECK_CONDITION_TRUE_NON_VOID(!context->create(), false, "Failed to create OpenGL context");
  
  surface = new QOffscreenSurface();
  surface->setFormat(context->format());
  surface->create();

  CHECK_CONDITION_TRUE_NON_VOID(!context->makeCurrent(surface), false, "Failed to make OpenGL context current");

  if (!initializeOpenGLFunctions()) {
    WARN_PRINT("Failed to initialize OpenGL functions");
    context->doneCurrent();
    delete surface;
    delete context;
    return false;
  }

  init_buffers();

  // Create an FBO for the fixed resolution.
  QOpenGLFramebufferObjectFormat fbo_format;
  fbo_format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  fbo = new QOpenGLFramebufferObject(shadergen_renderer::ShaderRenderer::kDefaultResolution,
                                     shadergen_renderer::ShaderRenderer::kDefaultResolution, fbo_format);
  
  context->doneCurrent();
  return true;
}

bool ShaderSampler::is_initialized() const {
  return isInitialized();
}

void ShaderSampler::init_buffers() {
  const std::array<float, shadergen_renderer::ShaderRenderer::kQuadFloatCount>& vertices{
      shadergen_renderer::ShaderRenderer::quad_vertices()};

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

  // Position attribute (x, y)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

  // FragCoord attribute (u, v)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // Unbind both VBO and VAO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool ShaderSampler::compile_shader(QOpenGLShaderProgram* program) {
  const std::string vertex_shader_source{shadergen_renderer::ShaderRenderer::vertex_shader_source()};

  // The generated code already carries the version directive's body (see
  // ShaderRenderer::fragment_shader_source); an empty graph falls back to the default shader.
  const std::string fragment_shader_source{
      code.empty() ? shadergen_renderer::ShaderRenderer::default_fragment_shader_source()
                   : shadergen_renderer::ShaderRenderer::fragment_shader_source(code)};

  CHECK_CONDITION_TRUE_NON_VOID(!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source.c_str()), false, "Vertex shader compilation failed: " + program->log().toStdString());
  CHECK_CONDITION_TRUE_NON_VOID(!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source.c_str()), false, "Fragment shader compilation failed: " + program->log().toStdString());
  CHECK_CONDITION_TRUE_NON_VOID(!program->link(), false, "Shader program linking failed: " + program->log().toStdString());

  return true;
}

void ShaderSampler::set_code(const std::string& new_code) { this->code = new_code; }

QImage ShaderSampler::sample_once() {
  CHECK_CONDITION_TRUE_NON_VOID(!isInitialized(), QImage(), "ShaderSampler is not initialized");

  CHECK_CONDITION_TRUE_NON_VOID(!context->makeCurrent(surface), QImage(), "Failed to make OpenGL context current");

  QOpenGLShaderProgram* program = new QOpenGLShaderProgram();
  if (!compile_shader(program)) {
    ERROR_PRINT("Failed to compile shader code");
    context->doneCurrent();
    return QImage();
  }

  if (!fbo->bind()) {
    ERROR_PRINT("Failed to bind framebuffer object");
    context->doneCurrent();
    return QImage();
  }
  glViewport(0, 0, shadergen_renderer::ShaderRenderer::kDefaultResolution,
             shadergen_renderer::ShaderRenderer::kDefaultResolution);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!program->bind()) {
    ERROR_PRINT("Failed to bind shader program");
    fbo->release();
    context->doneCurrent();
    return QImage();
  }

  // if (program->uniformLocation("uTime") != -1) {
  //   WARN_PRINT("uTime uniform found in static shader code");
  // }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(shadergen_renderer::ShaderRenderer::kQuadVertexCount));
  glBindVertexArray(0);

  program->release();
  fbo->release();

  delete program;

  const QImage extracted_image = fbo->toImage();
  
  context->doneCurrent();

  // Save the image to disk for debugging
  // extracted_image.save("extracted_image.png");

  return extracted_image.convertToFormat(QImage::Format_ARGB32);
}

