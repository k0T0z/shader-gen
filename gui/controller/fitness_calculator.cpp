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

#include "gui/controller/fitness_calculator.hpp"

#include "error_macros.hpp"

AIAgentFitnessCalculator::AIAgentFitnessCalculator(QWidget* parent)
    : QWidget(parent),
      layout(nullptr),
      menu_bar(nullptr),
      load_image_button(nullptr),
      outputs_layout(nullptr),
      curent_output_renderer_layout(nullptr),
      current_output_renderer_label(nullptr),
      current_output_renderer(nullptr),
      target_output_layout(nullptr),
      target_output_label(nullptr),
      target_output(nullptr) {
  resize(720, 360);

  AIAgentFitnessCalculator::init();
}

void AIAgentFitnessCalculator::init() {
  // Create the main layout.
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(5);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Create the menu bar layout.
  menu_bar = new QHBoxLayout(this);
  menu_bar->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  menu_bar->setSpacing(5);                       // Adjust spacing as needed
  menu_bar->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  menu_bar->setSizeConstraint(QLayout::SetNoConstraint);

  load_image_button = new QPushButton("Load Image", this);
  load_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  load_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  load_image_button->setToolTip("Load an image to match");
  QObject::connect(load_image_button, &QPushButton::pressed, this, &AIAgentFitnessCalculator::on_load_image_button_pressed);

  menu_bar->addWidget(load_image_button);

  layout->addLayout(menu_bar);

  outputs_layout = new QHBoxLayout(this);
  outputs_layout->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  outputs_layout->setSpacing(5);                       // Adjust spacing as needed
  outputs_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  outputs_layout->setSizeConstraint(QLayout::SetNoConstraint);

  curent_output_renderer_layout = new QVBoxLayout(this);
  curent_output_renderer_layout->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  curent_output_renderer_layout->setSpacing(5);                       // Adjust spacing as needed
  curent_output_renderer_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  curent_output_renderer_layout->setSizeConstraint(QLayout::SetNoConstraint);

  current_output_renderer_label = new QLabel("Current Output", this);
  current_output_renderer_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  current_output_renderer_label->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  current_output_renderer_label->setToolTip("The current output label");

  curent_output_renderer_layout->addWidget(current_output_renderer_label);

  current_output_renderer = new CurrentOutputRenderer(this);
  current_output_renderer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  current_output_renderer->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  current_output_renderer->setToolTip("The current output");
  current_output_renderer->setFixedSize(256, 256);

  curent_output_renderer_layout->addWidget(current_output_renderer);

  outputs_layout->addLayout(curent_output_renderer_layout);

  target_output_layout = new QVBoxLayout(this);
  target_output_layout->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  target_output_layout->setSpacing(5);                       // Adjust spacing as needed
  target_output_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  target_output_layout->setSizeConstraint(QLayout::SetNoConstraint);

  target_output_label = new QLabel("Target Output", this);
  target_output_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  target_output_label->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  target_output_label->setToolTip("The target output label");

  target_output_layout->addWidget(target_output_label);

  target_output = new QLabel(this);
  target_output->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  target_output->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  target_output->setToolTip("The target output");
  target_output->setFixedSize(256, 256);

  target_output_layout->addWidget(target_output);

  outputs_layout->addLayout(target_output_layout);

  layout->addLayout(outputs_layout);

  //////////////// Start of Footer ////////////////

  this->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Set the window title.
  this->setWindowTitle("Fitness Calculator");
  this->setLayout(layout);
}

void AIAgentFitnessCalculator::update_current_output(const std::string& code) {
  CHECK_CONDITION_TRUE(code.empty(), "Code is empty");

  current_output_renderer->set_code(code);
}

double AIAgentFitnessCalculator::get_fitness_value() const {
  CHECK_CONDITION_TRUE_NON_VOID(target_image.isNull(), -1.0, "No target image loaded");

  QImage current_image = current_output_renderer->get_pixel_data();

  CHECK_CONDITION_TRUE_NON_VOID(current_image.size() != QSize(256, 256) || target_image.size() != QSize(256, 256), -1.0, "Size mismatch");

  const uchar* current_bits = current_image.constBits();
  const uchar* target_bits = target_image.constBits();
  double sum = 0.0;

  for (int y = 0; y < 256; ++y) {
      for (int x = 0; x < 256; ++x) {
          int index = (y * 256 + x) * 3; // 3 bytes per pixel (RGB888)
          sum += std::abs(static_cast<int>(current_bits[index]) -
                          static_cast<int>(target_bits[index]));       // R
          sum += std::abs(static_cast<int>(current_bits[index + 1]) -
                          static_cast<int>(target_bits[index + 1]));   // G
          sum += std::abs(static_cast<int>(current_bits[index + 2]) -
                          static_cast<int>(target_bits[index + 2]));   // B
      }
  }

  return sum;
}

void AIAgentFitnessCalculator::on_load_image_button_pressed() {
  QString file_name = QFileDialog::getOpenFileName(this, "Load Image", "",
      "Images (*.png *.jpg *.bmp)");

  CHECK_CONDITION_TRUE(file_name.isEmpty(), "No file selected");

  QImage loaded_image;
  if (loaded_image.load(file_name)) {
    target_image = loaded_image.scaled(256, 256, Qt::IgnoreAspectRatio)
        .convertToFormat(QImage::Format_RGB888);
    target_output->clear();
    target_output->setPixmap(QPixmap::fromImage(target_image));
  } else {
    target_output->clear();
    target_output->setText("Failed to load image.");
  }
}

CurrentOutputRenderer::CurrentOutputRenderer(QWidget* parent)
    : QOpenGLWidget(parent), fbo(nullptr), texture_shader_program(nullptr), VAO(0), VBO(0), render_timer(this), compile_debounce_timer(this) {
  render_timer.setInterval(32); // ~30 FPS
  connect(&render_timer, &QTimer::timeout, this, QOverload<>::of(&CurrentOutputRenderer::update));
  connect(&render_timer, &QTimer::timeout, this, &CurrentOutputRenderer::scene_update_requested);

  compile_debounce_timer.setSingleShot(true);
  compile_debounce_timer.setInterval(300); // 300ms delay
  connect(&compile_debounce_timer, &QTimer::timeout, this, &CurrentOutputRenderer::update_shader_program);
}

CurrentOutputRenderer::~CurrentOutputRenderer() {
  cleanup();
}

void CurrentOutputRenderer::set_code(const std::string& new_code) {
  SILENT_CHECK_CONDITION_TRUE(this->code == new_code);

  this->code = new_code;

  if (!compile_debounce_timer.isActive()) compile_debounce_timer.start();
}

QImage CurrentOutputRenderer::get_pixel_data() const {
  return fbo->toImage().convertToFormat(QImage::Format_RGB888);
}

void CurrentOutputRenderer::initializeGL() {
  CHECK_CONDITION_TRUE(!initializeOpenGLFunctions(), "Failed to initialize OpenGL functions");

  init_buffers();
  update_shader_program();

  // Since update_shader_program() releases the context, we need to make it current again
  // https://doc.qt.io/qt-6/qopenglframebufferobject.html#details
  makeCurrent();

  // Initialize FBO for rendering at 256x256
  fbo = new QOpenGLFramebufferObject(256, 256);

  // Initialize texture shader program for displaying FBO texture
  texture_shader_program = new QOpenGLShaderProgram(this);
  
  const char* texture_vertex_shader = R"(
#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
  )";

  const char* texture_fragment_shader = R"(
#version 430 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
void main() {
    FragColor = texture(uTexture, TexCoord);
}
  )";

  CHECK_CONDITION_TRUE(!texture_shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex, texture_vertex_shader), "Vertex shader compilation failed: " + texture_shader_program->log().toStdString());
  CHECK_CONDITION_TRUE(!texture_shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment, texture_fragment_shader), "Fragment shader compilation failed: " + texture_shader_program->log().toStdString());
  CHECK_CONDITION_TRUE(!texture_shader_program->link(), "Shader program linking failed: " + texture_shader_program->log().toStdString());

  doneCurrent();

  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &CurrentOutputRenderer::cleanup);
}

void CurrentOutputRenderer::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void CurrentOutputRenderer::paintGL() {
  // Render to FBO at 256x256
  fbo->bind();
  glViewport(0, 0, 256, 256);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  CHECK_PARAM_NULLPTR(shader_program, "Shader program is null");
  
  CHECK_CONDITION_TRUE(!shader_program->bind(), "Failed to bind shader program");
  
  int utime_location = shader_program->uniformLocation("uTime");
  if (utime_location != -1) {
    float time_value = timer.elapsed() * 0.001f; // Convert ms to seconds
    shader_program->setUniformValue(utime_location, time_value);
  }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  shader_program->release();

  fbo->release();

  // Render FBO texture to widget
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
  glViewport(0, 0, width(), height());
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  GLuint texture = fbo->texture();
  glBindTexture(GL_TEXTURE_2D, texture);

  texture_shader_program->bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  texture_shader_program->setUniformValue("uTexture", 0);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  texture_shader_program->release();
}

void CurrentOutputRenderer::cleanup() {
  SILENT_CHECK_CONDITION_TRUE(!isInitialized());

  makeCurrent();
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  VAO = VBO = 0;
  if (fbo) {
    delete fbo;
    fbo = nullptr;
  }
  if (texture_shader_program) {
      texture_shader_program->deleteLater();
      texture_shader_program = nullptr;
  }
  doneCurrent();
  disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &CurrentOutputRenderer::cleanup);
}

void CurrentOutputRenderer::init_buffers() {
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

void CurrentOutputRenderer::update_shader_program() {
  SILENT_CHECK_CONDITION_TRUE(!isInitialized());

  makeCurrent();  // Ensure OpenGL context is current for shader operations
  
  std::unique_ptr<QOpenGLShaderProgram> new_program = std::make_unique<QOpenGLShaderProgram>(this);
  
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

void CurrentOutputRenderer::showEvent(QShowEvent* event) {
  QOpenGLWidget::showEvent(event);

  if (!render_timer.isActive()) render_timer.start();
  else {
    render_timer.stop();
    render_timer.start();
  }

  if (!timer.isValid()) timer.start();
}

void CurrentOutputRenderer::hideEvent(QHideEvent* event) {
  QOpenGLWidget::hideEvent(event);
  
  if (timer.isValid()) timer.invalidate();

  if (render_timer.isActive()) render_timer.stop();
}

