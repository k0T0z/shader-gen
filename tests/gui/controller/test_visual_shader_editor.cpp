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

#include "generator/proto_node_factory.hpp"
#include "generator/visual_shader_generator.hpp"
#include "gui/controller/utils/utils.hpp"
#include "gui/model/message_model.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/model/utils/field_path.hpp"
#include "gui/model/schema/visual_shader.pb.h"
#include "gui/model/schema/visual_shader_nodes.pb.h"

using VisualShader = gui::model::schema::VisualShader;
using VisualShaderNodeInput = gui::model::schema::VisualShaderNodeInput;

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

// Regression test for: "when i create an input then choose uv, it crashes".
// Drives the exact generator call site used by
// VisualShaderGraphicsScene::on_update_renderer_widgets_requested():
//   generate_preview_shader(controller_utils::encode_graph(nodes, connections), n_id, 0)
// The graph contains a Brick node whose bool `rand_color` is encoded as "false" by the GUI
// encoder; the generator used to throw std::invalid_argument from std::stoi on that value.
void VisualShaderEditorTest::repro_create_input_select_uv()
{
    // Exact graph captured from the crashing session (build/model.json):
    //   node 0: output
    //   node 1: brick (incl. bool randColor=false)
    //   node 2: input with type = UV
    //   connection 0: from node 2 port 0 -> node 1 port 0
    using VisualShaderNodeBrick = gui::model::schema::VisualShaderNodeBrick;
    using VisualShaderConnection = VisualShader::VisualShaderConnection;

    gui::model::schema::VisualShader visual_shader;
    {
        VisualShader::VisualShaderNode* node = visual_shader.add_nodes();
        node->set_id(0);
        node->mutable_output();
    }
    {
        VisualShader::VisualShaderNode* node = visual_shader.add_nodes();
        node->set_id(1);
        VisualShaderNodeBrick* brick = node->mutable_brick();
        brick->set_num_x(4);
        brick->set_num_y(6);
        brick->set_shift(0.5f);
        brick->set_rand_shift(0.1f);
        brick->set_gap_x(0.03f);
        brick->set_gap_y(0.03f);
        brick->set_smooth(0.05f);
        brick->set_rand_color(false);
    }
    {
        VisualShader::VisualShaderNode* node = visual_shader.add_nodes();
        node->set_id(2);
        node->mutable_input()->set_type(VisualShaderNodeInput::INPUT_TYPE_UV);
    }
    {
        VisualShaderConnection* connection = visual_shader.add_connections();
        connection->set_id(0);
        connection->set_from_node_id(2);
        connection->set_from_port_index(0);
        connection->set_to_node_id(1);
        connection->set_to_port_index(0);
    }

    MessageModel* root_model = new MessageModel(&visual_shader);
    root_model->build_sub_models();

    const ProtoModel* nodes = root_model->get_sub_model(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber)));
    const ProtoModel* connections = root_model->get_sub_model(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kConnectionsFieldNumber)));
    QVERIFY(nodes != nullptr);
    QVERIFY(connections != nullptr);

    const std::string graph = controller_utils::encode_graph(nodes, connections);
    // Regression: the GUI encoder serializes the Brick's bool `rand_color` as "false". The
    // generator must parse bool parameters without throwing std::invalid_argument.
    QVERIFY(graph.find("8=false") != std::string::npos);

    const std::string preview = shadergen_visual_shader_generator::generate_preview_shader(graph, 1, 0);
    QVERIFY(!preview.empty());

    delete root_model;
}

// Regression test for: "when i add a stop color gradient in brick, i get this:
//   ERROR: Condition "!hasIndex(row, column, parent)" is true. Returning: QModelIndex()".
// Mirrors VisualShaderGraphicsScene::on_node_repeated_field_row_add_requested() +
// VisualShaderNodeFieldRepeatedMessage::build_row(): after a row is appended the widget reads every
// column value of the freshly created row model without having set any field on it first.
void VisualShaderEditorTest::repro_add_gradient_stop_in_brick()
{
    using VisualShaderNodeBrick = gui::model::schema::VisualShaderNodeBrick;

    gui::model::schema::VisualShader visual_shader;
    {
        VisualShader::VisualShaderNode* node = visual_shader.add_nodes();
        node->set_id(1);
        node->mutable_brick();
    }

    MessageModel* root_model = new MessageModel(&visual_shader);
    root_model->build_sub_models();

    const ProtoModel* gradient_field_model = root_model->get_sub_model(
        FieldPath::Of<VisualShader>(FieldPath::FieldNumber(VisualShader::kNodesFieldNumber), FieldPath::RepeatedAt(0),
                                    FieldPath::FieldNumber(VisualShader::VisualShaderNode::kBrickFieldNumber),
                                    FieldPath::FieldNumber(VisualShaderNodeBrick::kGradientFieldNumber)));
    QVERIFY(gradient_field_model != nullptr);

    RepeatedMessageModel* gradient =
        dynamic_cast<RepeatedMessageModel*>(const_cast<ProtoModel*>(gradient_field_model));
    QVERIFY(gradient != nullptr);

    // Same as clicking "Add" on the Brick's stop color gradient.
    QVERIFY(gradient->append_row() == 0);

    MessageModel* row_model = gradient->get_sub_model(0);
    QVERIFY(row_model != nullptr);

    // The row was appended without setting any field, so its parent index must still be resolvable.
    QVERIFY(row_model->parent(QModelIndex()).isValid());

    // build_row() reads every column value of the new row - this must not emit "Invalid index.".
    for (int column{0}; column < row_model->columnCount(); ++column) {
        const ProtoModel* column_model = row_model->get_sub_model_by_index(column);
        QVERIFY(column_model != nullptr);
        column_model->data();
    }

    delete root_model;
}
