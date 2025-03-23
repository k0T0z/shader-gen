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

#include "gui/controller/ai_agent_monitor.hpp"

#include "ai-agent/fitness.hpp"
#include "ai-agent/utils/utils.hpp"
#include "error_macros.hpp"

AIAgentMonitor::AIAgentMonitor(QWidget* parent)
    : QWidget(parent),
      layout(nullptr),
      menu_bar(nullptr),
      load_image_button(nullptr),
      calculate_fitness_button(nullptr),
      matching_type_combo_box(nullptr),
      generate_random_image_button(nullptr),
      status_layout(nullptr),
      fitness_value_label(nullptr),
      fitness_value(nullptr),
      outputs_layout(nullptr),
      curent_output_renderer_layout(nullptr),
      current_output_renderer_label(nullptr),
      current_output_renderer(nullptr),
      target_output_layout(nullptr),
      target_output_label(nullptr),
      target_output(nullptr) {
  resize(720, 360);

  AIAgentMonitor::init();
}

void AIAgentMonitor::init() {
  // Create the main layout.
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(5);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Create the menu bar layout.
  menu_bar = new QHBoxLayout();
  menu_bar->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  menu_bar->setSpacing(5);                       // Adjust spacing as needed
  menu_bar->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  menu_bar->setSizeConstraint(QLayout::SetNoConstraint);

  load_image_button = new QPushButton("Load Image", this);
  load_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  load_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  load_image_button->setToolTip("Load an image to match");
  QObject::connect(load_image_button, &QPushButton::pressed, this, &AIAgentMonitor::on_load_image_button_pressed);

  menu_bar->addWidget(load_image_button);

  calculate_fitness_button = new QPushButton("Calculate Fitness", this);
  calculate_fitness_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  calculate_fitness_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  calculate_fitness_button->setToolTip("Calculate the fitness value (only for preview)");
  QObject::connect(calculate_fitness_button, &QPushButton::pressed, this, &AIAgentMonitor::on_calculate_fitness_button_pressed);

  menu_bar->addWidget(calculate_fitness_button);
  
  matching_type_combo_box = new QComboBox(this);
  matching_type_combo_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  matching_type_combo_box->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  matching_type_combo_box->setToolTip("Select the matching type");
  matching_type_combo_box->addItem("Static");
  matching_type_combo_box->addItem("Dynamic");
  QObject::connect(matching_type_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AIAgentMonitor::on_matching_type_combo_box_current_index_changed);

  menu_bar->addWidget(matching_type_combo_box);

  generate_random_image_button = new QPushButton("Generate Random Image", this);
  generate_random_image_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  generate_random_image_button->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  generate_random_image_button->setToolTip("Generate a random image for testing");
  QObject::connect(generate_random_image_button, &QPushButton::pressed, this, &AIAgentMonitor::on_generate_random_image_button_pressed);

  menu_bar->addWidget(generate_random_image_button);

  layout->addLayout(menu_bar, 1);

  // Create a H layout for printing the fitness value
  status_layout = new QHBoxLayout();
  status_layout->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  status_layout->setSpacing(5);                       // Adjust spacing as needed
  status_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  status_layout->setSizeConstraint(QLayout::SetNoConstraint);

  fitness_value_label = new QLabel("Fitness Value: ", this);
  fitness_value_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  fitness_value_label->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  fitness_value_label->setToolTip("The fitness value label");

  status_layout->addWidget(fitness_value_label);

  fitness_value = new QLabel("0", this);
  fitness_value->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  fitness_value->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  fitness_value->setToolTip("The fitness value");

  status_layout->addWidget(fitness_value);

  layout->addLayout(status_layout, 1);

  outputs_layout = new QHBoxLayout();
  outputs_layout->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  outputs_layout->setSpacing(5);                       // Adjust spacing as needed
  outputs_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  outputs_layout->setSizeConstraint(QLayout::SetNoConstraint);

  curent_output_renderer_layout = new QVBoxLayout();
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

  target_output_layout = new QVBoxLayout();
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

  layout->addLayout(outputs_layout, 3);

  //////////////// Start of Footer ////////////////

  this->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Set the window title.
  this->setWindowTitle("AI Agent Monitor");
  this->setLayout(layout);
}

QImage AIAgentMonitor::create_random_image(const int& width, const int& height) {
  QImage image(width, height, QImage::Format_ARGB32);
  
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int alpha = 255;
      // Generate random RGB components (0-255 inclusive)
      const int red = ai_agent_utils::random_int_inclusive<int>(0, 255);
      const int green = ai_agent_utils::random_int_inclusive<int>(0, 255);
      const int blue = ai_agent_utils::random_int_inclusive<int>(0, 255);
      
      // Combine into ARGB32 pixel format and set the pixel
      image.setPixel(x, y, qRgba(red, green, blue, alpha));
    }
  }
  
  return image;
}

void AIAgentMonitor::update_current_output(const std::string& code) {
  CHECK_CONDITION_TRUE(code.empty(), "Code is empty");

  current_output_renderer->set_code(code);
}

void AIAgentMonitor::update_fitness_value() {
  fitness_value->setText(QString::number(get_fitness_value()));
}

unsigned long AIAgentMonitor::get_fitness_value() const {
  CHECK_CONDITION_TRUE_NON_VOID(target_image.isNull(), std::numeric_limits<unsigned long>::max(), "No target image loaded");

  const QImage current_image = current_output_renderer->get_pixel_data();

  CHECK_CONDITION_TRUE_NON_VOID(current_image.isNull(), std::numeric_limits<unsigned long>::max(), "Failed to retrieve current image");
  CHECK_CONDITION_TRUE_NON_VOID(current_image.size() != target_image.size(), std::numeric_limits<unsigned long>::max(), "Size mismatch");

  // Retrieve pointers to the pixel data.
  // QImage::bits() returns a pointer to the first pixel, and since our format is ARGB32,
  // we can safely reinterpret_cast to a uint32_t pointer.
  const uint32_t* pixels1 = reinterpret_cast<const uint32_t*>(current_image.bits());
  const uint32_t* pixels2 = reinterpret_cast<const uint32_t*>(target_image.bits());
  const int width = current_image.width();
  const int height = current_image.height();

  return ai_agent_fitness::calculate_fitness(pixels1, pixels2, width, height);
}

QImage AIAgentMonitor::get_target_image() const {
  CHECK_CONDITION_TRUE_NON_VOID(target_image.isNull(), QImage(), "No target image loaded");
  return target_image;
}

void AIAgentMonitor::on_load_image_button_pressed() {
  QString file_name = QFileDialog::getOpenFileName(this, "Load Target Image", "",
      "Images (*.png *.jpg *.jpeg *.bmp)");

  CHECK_CONDITION_TRUE(file_name.isEmpty(), "No file selected");

  QImage loaded_image;
  if (loaded_image.load(file_name)) {
    target_image = loaded_image.scaled(256, 256, Qt::IgnoreAspectRatio)
        .convertToFormat(QImage::Format_ARGB32);
    target_output->clear();
    target_output->setPixmap(QPixmap::fromImage(target_image));
  } else {
    target_output->clear();
    target_output->setText("Failed to load image.");
  }
}

void AIAgentMonitor::on_calculate_fitness_button_pressed() { update_fitness_value(); }

void AIAgentMonitor::on_matching_type_combo_box_current_index_changed(int index) {
  if (index == 0) {
    DEBUG_PRINT("Static matching");
    current_output_renderer->set_is_dynamic(false);
  } else {
    DEBUG_PRINT("Dynamic matching");
    current_output_renderer->set_is_dynamic(true);
  }
}

void AIAgentMonitor::on_generate_random_image_button_pressed() {
  target_image = create_random_image(256, 256);
  target_output->clear();
  target_output->setPixmap(QPixmap::fromImage(target_image));
}

CurrentOutputRenderer::CurrentOutputRenderer(QWidget* parent) : QOpenGLWidget(parent), 
                                                                static_rendered(false),
                                                                is_dynamic(false), 
                                                                fbo(nullptr), 
                                                                texture_shader_program(nullptr), 
                                                                VAO(0), 
                                                                VBO(0), 
                                                                render_timer(this), 
                                                                compile_debounce_timer(this) {
  render_timer.setInterval(32); // ~30 FPS
  connect(&render_timer, &QTimer::timeout, this, QOverload<>::of(&CurrentOutputRenderer::update));

  compile_debounce_timer.setSingleShot(true);
  compile_debounce_timer.setInterval(300); // 300ms delay
  connect(&compile_debounce_timer, &QTimer::timeout, this, &CurrentOutputRenderer::update_shader_program);
}

CurrentOutputRenderer::~CurrentOutputRenderer() {
  cleanup();
}

void CurrentOutputRenderer::set_is_dynamic(const bool& is_dynamic) {
  SILENT_CHECK_CONDITION_TRUE(this->is_dynamic == is_dynamic);

  this->is_dynamic = is_dynamic;

  if (this->is_dynamic) {
    // Start the render timer if the widget is visible
    if (isVisible() && !render_timer.isActive()) render_timer.start();
  } else {
    // Stop the timer and render once for static display
    if (render_timer.isActive()) render_timer.stop();
    update();  // Trigger a single render for static content
  }
}

void CurrentOutputRenderer::force_set_code(const std::string& code) {
  SILENT_CHECK_CONDITION_TRUE(code == this->code);

  this->code = code;
  update_shader_program();
}

void CurrentOutputRenderer::set_code(const std::string& new_code) {
  SILENT_CHECK_CONDITION_TRUE(this->code == new_code);

  this->code = new_code;

  if (!compile_debounce_timer.isActive()) compile_debounce_timer.start();
}

QImage CurrentOutputRenderer::get_pixel_data() {
  makeCurrent();
  QImage image = fbo->toImage();
  doneCurrent();
  return image.convertToFormat(QImage::Format_ARGB32);
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
  if (is_dynamic) {
    CHECK_PARAM_NULLPTR(shader_program, "Shader program is null");
    
    // Render to FBO at 256x256
    fbo->bind();
    glViewport(0, 0, 256, 256);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!shader_program->bind()) {
      WARN_PRINT("Failed to bind shader program");
      fbo->release();
      return;
    }
    
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
  } else if (!static_rendered) {
    // For static shaders, render once to FBO
    CHECK_PARAM_NULLPTR(shader_program, "Shader program is null");

    fbo->bind();
    glViewport(0, 0, 256, 256);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!shader_program->bind()) {
      WARN_PRINT("Failed to bind shader program");
      fbo->release();
      return;
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    shader_program->release();
    fbo->release();
    update();  // Display the static render
    static_rendered = true;
  }

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

  // if (new_program->uniformLocation("uTime") != -1 && !is_dynamic) {
  //   WARN_PRINT("uTime uniform found in static shader code");
  // }
  
  shader_program.swap(new_program);

  this->static_rendered = false;
  
  doneCurrent();  // Release the context
}

void CurrentOutputRenderer::showEvent(QShowEvent* event) {
  QOpenGLWidget::showEvent(event);
  if (is_dynamic && !render_timer.isActive()) render_timer.start();
  else if (!render_timer.isActive()) {
    render_timer.stop();
    render_timer.start();
  }
  if (!timer.isValid()) timer.start();
}

void CurrentOutputRenderer::hideEvent(QHideEvent* event) {
  QOpenGLWidget::hideEvent(event);
  if (render_timer.isActive()) render_timer.stop();
  if (timer.isValid()) timer.invalidate();
}

