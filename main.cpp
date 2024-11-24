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

#include "main.hpp"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <filesystem>

#include <google/protobuf/stubs/common.h>

#include "gui/controller/visual_shader_editor.hpp"

int main(int argc, char **argv) {
	// Verify that the version of the library that we linked against is
  	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	QApplication shader_gen_app(argc, argv);
	QCoreApplication::setOrganizationName(ENIGMA_ORG_NAME);
	QCoreApplication::setApplicationName(SHADER_GEN_PROJECT_NAME);
	QCoreApplication::setApplicationVersion(SHADER_GEN_PROJECT_VERSION);
	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::applicationName());
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(shader_gen_app);

	VisualShaderEditor w;

	w.resize(1440, 720);
	w.show();

	return shader_gen_app.exec();
}
