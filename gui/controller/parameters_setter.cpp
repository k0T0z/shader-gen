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

#include "gui/controller/parameters_setter.hpp"

#include "ai-agent/parameters.hpp"

AIAgentParametersSetter::AIAgentParametersSetter(QWidget* parent)
    : QWidget(parent), layout(new QVBoxLayout(this)) {
  init();
}

void AIAgentParametersSetter::init() {
  // Helper lambda to create a horizontal layout for a parameter.
  auto create_parameter_row = [this](const QString& param_name, const float& default_value) {
    QHBoxLayout* param_row_layout = new QHBoxLayout(this);
    QLabel* param_row_label = new QLabel(param_name + ":", this);
    QLineEdit* param_row_line_edit = new QLineEdit(QString::number(default_value), this);
    param_row_line_edit->setEnabled(false);
    QCheckBox* param_row_check_box = new QCheckBox(this);
    param_row_check_box->setToolTip("Check to change the default value");
    param_row_check_box->setChecked(false);
    connect(param_row_check_box, &QCheckBox::toggled, param_row_line_edit, &QLineEdit::setEnabled);

    param_row_layout->addWidget(param_row_label);
    param_row_layout->addWidget(param_row_line_edit);
    param_row_layout->addWidget(param_row_check_box);

    layout->addLayout(param_row_layout);
  };

  // Create rows for each parameter
  create_parameter_row("mutation_probability", mutation_probability);
  create_parameter_row("crossover_probability", crossover_probability);
  create_parameter_row("elitism_ratio", elitism_ratio);
  create_parameter_row("maximum_iterations", maximum_iterations);

  match_button = new QPushButton("Match", this);
  layout->addWidget(match_button);
}

