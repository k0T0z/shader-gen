#ifndef SHADER_GEN_RENDERER_HPP
#define SHADER_GEN_RENDERER_HPP

#include <QtOpenGLWidgets/QOpenGLWidget>
// #include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <QtOpenGL/QOpenGLFunctions_4_3_Core>  // https://stackoverflow.com/a/64288966/14629018 explains why we need this.
#include <QtOpenGL/QOpenGLShaderProgram>

/**
 * @brief This class is meant to be a temporary solution to preview the shader
 *        code. We should preview the shader code using ENIGMA's Graphics System.
 * 
 * @todo Replace this class with ENIGMA's Graphics System.
 * 
 */
class RendererWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core {
    Q_OBJECT
  
   public:
    RendererWidget(QWidget* parent = nullptr);
    ~RendererWidget() override;
  
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
  
    void cleanup();
  };

#endif // SHADER_GEN_RENDERER_HPP
