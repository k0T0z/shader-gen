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

  target_output = new QWidget(this);
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

void AIAgentFitnessCalculator::on_load_image_button_pressed() {}

CurrentOutputRenderer::CurrentOutputRenderer(QWidget* parent)
    : RendererWidget(parent) {}
