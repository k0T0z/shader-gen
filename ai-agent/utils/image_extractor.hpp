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

#ifndef SHADER_GEN_AI_AGENT_IMAGE_EXTRACTOR_HPP
#define SHADER_GEN_AI_AGENT_IMAGE_EXTRACTOR_HPP

#include <QtOpenGL/QOpenGLFunctions_4_3_Core>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QImage>
#include <memory>
#include <string>

class ImageExtractor : protected QOpenGLFunctions_4_3_Core {
  public:
  ImageExtractor();
  ~ImageExtractor();

  bool initialize();
  QImage render(const std::string& code);

private:
  void init_buffers();
  bool compile_shader(const std::string& code, QOpenGLShaderProgram& program);

  QOpenGLContext* context;
  QOffscreenSurface* surface;
  std::unique_ptr<QOpenGLFramebufferObject> fbo;
  
  GLuint VAO;
  GLuint VBO;
  bool initialized;
};

#endif  // SHADER_GEN_AI_AGENT_IMAGE_EXTRACTOR_HPP
