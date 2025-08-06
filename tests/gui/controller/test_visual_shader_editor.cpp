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

#include <QtTest/QtTest>

#include "error_macros.hpp"
#include "tests/gui/controller/test_visual_shader_editor.hpp"

VisualShaderEditorTest::VisualShaderEditorTest() : editor(nullptr) {}

void VisualShaderEditorTest::initTestCase()
{
    DEBUG_PRINT("Called before everything else.");
}

void VisualShaderEditorTest::cleanupTestCase()
{
    DEBUG_PRINT("Called after everything else.");
}

void VisualShaderEditorTest::init() {
    DEBUG_PRINT("Called before each test.");
}

void VisualShaderEditorTest::cleanup() {
    DEBUG_PRINT("Called after each test.");
}

void VisualShaderEditorTest::my_first_test()
{
    QVERIFY(true); // check that a condition is satisfied
    QCOMPARE(1, 1); // compare two values
}

void VisualShaderEditorTest::my_second_test()
{
    QVERIFY(my_condition());
    QVERIFY(1 != 2);
}

bool VisualShaderEditorTest::my_condition()
{
    return true;
}
