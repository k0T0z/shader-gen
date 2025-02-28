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

#include "gui/controller/parameters_editor.hpp"

#include "ai-agent/parameters.hpp"

AIAgentParametersEditor::AIAgentParametersEditor(QWidget* parent)
    : QWidget(parent), layout(nullptr) {
  init();
}

void AIAgentParametersEditor::init() {
  // Create the main layout.
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);  // Left, top, right, bottom
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->setSpacing(5);
  layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  //////////////// End of Header ////////////////

  // Helper lambda to create a horizontal layout for a parameter.
  auto create_parameter_row = [this](const QString& param_name, const float& default_value) {
    QHBoxLayout* param_row_layout = new QHBoxLayout();
    QLabel* param_row_label = new QLabel(param_name + ":", this);
    QLineEdit* param_row_line_edit = new QLineEdit(QString::number(default_value), this);
    param_row_line_edit->setEnabled(false);
    QCheckBox* param_row_check_box = new QCheckBox(this);
    param_row_check_box->setToolTip("Check to change the default value");
    param_row_check_box->setChecked(false);
    connect(param_row_check_box, &QCheckBox::toggled, param_row_line_edit, &QLineEdit::setEnabled);

    parameter_line_edits.emplace_back(param_row_line_edit);

    param_row_layout->addWidget(param_row_label);
    param_row_layout->addWidget(param_row_line_edit);
    param_row_layout->addWidget(param_row_check_box);

    layout->addLayout(param_row_layout);
  };

  // Create rows for each parameter
  create_parameter_row("Mutation Probability", mutation_probability);
  create_parameter_row("Crossover Probability", crossover_probability);
  create_parameter_row("Elitism Ratio", elitism_ratio);
  create_parameter_row("Maximum Iterations", maximum_iterations);

  //////////////// Start of Footer ////////////////

  this->setContentsMargins(0, 0, 0, 0);  // Left, top, right, bottom
  // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Set the window title and icon.
  this->setWindowTitle("Parameters Editor");
  this->setLayout(layout);
}

