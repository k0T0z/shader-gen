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

#ifndef SHADER_GEN_AI_AGENT_FITNESS_CALCULATOR_HPP
#define SHADER_GEN_AI_AGENT_FITNESS_CALCULATOR_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "gui/controller/renderer/renderer.hpp"

class CurrentOutputRenderer;

class AIAgentFitnessCalculator : public QWidget {
  Q_OBJECT

 public:
  AIAgentFitnessCalculator(QWidget* parent = nullptr);
  ~AIAgentFitnessCalculator() override = default;

 private Q_SLOTS:
  void on_load_image_button_pressed();

 private:
  QVBoxLayout* layout;

  QHBoxLayout* menu_bar;
  QPushButton* load_image_button;

  QHBoxLayout* outputs_layout;

  QVBoxLayout* curent_output_renderer_layout;
  QLabel* current_output_renderer_label;
  CurrentOutputRenderer* current_output_renderer;

  QVBoxLayout* target_output_layout;
  QLabel* target_output_label;
  QWidget* target_output;


  /**
   * @brief Initializes the UI
   * 
   */
  void init();
};

class CurrentOutputRenderer : public RendererWidget {
  Q_OBJECT

 public:
  CurrentOutputRenderer(QWidget* parent = nullptr);
  ~CurrentOutputRenderer() override = default;
};

#endif  // SHADER_GEN_AI_AGENT_FITNESS_CALCULATOR_HPP
  