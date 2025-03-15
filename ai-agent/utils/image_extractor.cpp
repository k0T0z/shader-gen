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

#include "ai-agent/utils/image_extractor.hpp"

#include "error_macros.hpp"
#include "generator/visual_shader_generator.hpp"

ImageExtractor::ImageExtractor() :
    context(new QOpenGLContext),
    surface(new QOffscreenSurface),
    VAO(0),
    VBO(0),
    initialized(false) {
  QSurfaceFormat format;
  format.setVersion(4, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  context->setFormat(format);
}

ImageExtractor::~ImageExtractor() {
  if (initialized) {
    context->makeCurrent(surface);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    VAO = VBO = 0;
    context->doneCurrent();
  }
  delete surface;
  delete context;
}

bool ImageExtractor::initialize() {
  CHECK_CONDITION_TRUE_NON_VOID(!context->create(), false, "Failed to create OpenGL context");

  surface->setFormat(context->format());
  surface->create();

  CHECK_CONDITION_TRUE_NON_VOID(!context->makeCurrent(surface), false, "Failed to make OpenGL context current");

  CHECK_CONDITION_TRUE_NON_VOID(!initializeOpenGLFunctions(), false, "Failed to initialize OpenGL functions");
  init_buffers();

  // Create FBO with size 256x256
  fbo = std::make_unique<QOpenGLFramebufferObject>(256, 256);
  initialized = true;
  context->doneCurrent();
  return true;
}

void ImageExtractor::init_buffers() {
  const float vertices[] = {
    -1.0f,  1.0f,  0.0f,  1.0f,  // Vertex 1: Position (x,y), FragCoord (u,v)
    -1.0f, -1.0f,  0.0f,  0.0f,  // Vertex 2: Position (x,y), FragCoord (u,v)
     1.0f,  1.0f,  1.0f,  1.0f,  // Vertex 3: Position (x,y), FragCoord (u,v)
     1.0f, -1.0f,  1.0f,  0.0f   // Vertex 4: Position (x,y), FragCoord (u,v)
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

bool ImageExtractor::compile_shader(const std::string& code, QOpenGLShaderProgram& program) {
  const char* vertex_shader_source = R"(

#version 430 core

#line 0  // Reset line numbers for better error messages
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

#line 0  // Reset line numbers for better error messages
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
  
  CHECK_CONDITION_TRUE_NON_VOID(!program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_source), false, "Vertex shader compilation failed: " + program.log().toStdString());
  CHECK_CONDITION_TRUE_NON_VOID(!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_source.c_str()), false, "Fragment shader compilation failed: " + program.log().toStdString());
  CHECK_CONDITION_TRUE_NON_VOID(!program.link(), false, "Shader program linking failed: " + program.log().toStdString());

  return true;
}

QImage ImageExtractor::render(const std::string& code) {
  CHECK_CONDITION_TRUE_NON_VOID(!initialized || !context->makeCurrent(surface), QImage(), "Failed to make OpenGL context current");

  QOpenGLShaderProgram program;
  if (!compile_shader(code, program)) {
    WARN_PRINT("Failed to compile shader");
    context->doneCurrent();
    return QImage();
  }

  fbo->bind();
  glViewport(0, 0, 256, 256);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  program.bind();

  if (program.uniformLocation("uTime") != -1) {
    WARN_PRINT("uTime uniform found in static shader code");
  }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  program.release();
  fbo->release();

  QImage extracted_image = fbo->toImage().convertToFormat(QImage::Format_ARGB32);
  context->doneCurrent();
  return extracted_image;
}

