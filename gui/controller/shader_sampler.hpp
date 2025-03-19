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

#ifndef SHADER_GEN_AI_AGENT_SHADER_SAMPLER_HPP
#define SHADER_GEN_AI_AGENT_SHADER_SAMPLER_HPP

#include <QtOpenGL/QOpenGLFunctions_4_3_Core>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <memory>
#include <string>
#include <QImage>
#include <QObject>

class ShaderSampler : public QObject, protected QOpenGLFunctions_4_3_Core {
  Q_OBJECT

 public:
  ShaderSampler(QObject* parent = nullptr);
  ~ShaderSampler();

  bool initialize();

 public Q_SLOTS:
  QImage sample_once(const std::string& code);

 private:
  void init_buffers();
  bool compile_shader(const std::string& code, QOpenGLShaderProgram* program);

  QOpenGLContext* context;
  QOffscreenSurface* surface;
  QOpenGLFramebufferObject* fbo;
  
  GLuint VAO;
  GLuint VBO;
};

#endif  // SHADER_GEN_AI_AGENT_SHADER_SAMPLER_HPP
