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

#ifndef SHADER_GEN_AI_AGENT_PARAMETERS_EDITOR_HPP
#define SHADER_GEN_AI_AGENT_PARAMETERS_EDITOR_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

class AIAgentParametersEditor : public QWidget {
  Q_OBJECT

 public:
  AIAgentParametersEditor(QWidget* parent = nullptr);
  ~AIAgentParametersEditor() override = default;

  int get_maximum_population_size() const { return parameter_line_edits[0]->text().toInt(); }
  int get_maximum_generations() const { return parameter_line_edits[1]->text().toInt(); }
  float get_mutation_probability() const { return parameter_line_edits[2]->text().toFloat(); }
  float get_crossover_probability() const { return parameter_line_edits[3]->text().toFloat(); }
  float get_elitism_ratio() const { return parameter_line_edits[4]->text().toFloat(); }
  int get_maximum_nodes_per_graph() const { return parameter_line_edits[5]->text().toInt(); }

 private:
  QVBoxLayout* layout;

  std::vector<QLineEdit*> parameter_line_edits;

  /**
   * @brief Initializes the UI
   * 
   */
  void init();
};

#endif  // SHADER_GEN_AI_AGENT_PARAMETERS_EDITOR_HPP
