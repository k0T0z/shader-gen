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

#ifndef ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP
#define ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP

#include <string>
#include <vector>

#include "gui/model/schema/visual_shader_nodes.pb.h"

using namespace gui::model::schema;

class VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGenerator() = default;
  virtual ~VisualShaderNodeGenerator() = default;

  virtual std::string generate_global([[maybe_unused]] const int& id) const { return ""; }
  virtual std::string generate_global_per_node([[maybe_unused]] const int& id) const { return ""; }
  virtual std::string generate_global_per_func([[maybe_unused]] const int& id) const { return ""; }

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                    [[maybe_unused]] const std::vector<std::string>& input_vars,
                                    [[maybe_unused]] const std::vector<std::string>& output_vars) const = 0;
};

class VisualShaderNodeGeneratorInput : public VisualShaderNodeGenerator {
 public:
  VisualShaderNodeGeneratorInput(const VisualShaderNodeInputType& input_type = VisualShaderNodeInputType::INPUT_TYPE_UNSPECIFIED) : VisualShaderNodeGenerator(), 
                                                                                                                                    input_type(input_type) {}

  virtual std::string generate_global([[maybe_unused]] const int& id) const override;

  virtual std::string generate_code([[maybe_unused]] const int& id,
                                          [[maybe_unused]] const std::vector<std::string>& input_vars,
                                          [[maybe_unused]] const std::vector<std::string>& output_vars) const;

 private:
  const VisualShaderNodeInputType input_type;
};

#endif  // ENIGMA_VISUAL_SHADER_NODE_GENERATORS_HPP
