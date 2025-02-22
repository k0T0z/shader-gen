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

#ifndef SHADER_GEN_RENDERER_HPP
#define SHADER_GEN_RENDERER_HPP

#include <QtOpenGLWidgets/QOpenGLWidget>
// #include <QOpenGLFunctions>
#include <QTimer>
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
  
    void init_buffers();
    void update_shader_program();
  
    void cleanup();
  };

#endif // SHADER_GEN_RENDERER_HPP
