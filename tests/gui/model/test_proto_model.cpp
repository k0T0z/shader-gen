#include <gtest/gtest.h>
#include "gui/model/proto_model.hpp"
#include "gui/model/proto_schema/VisualShader.pb.h"  // Generated from your .proto file

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using VisualShader = model::schema::VisualShader;

// Mock subclass for testing with VisualShader
class VisualShaderModel : public ProtoModel {
public:
    VisualShaderModel(google::protobuf::Message* message, QObject* parent = nullptr)
        : ProtoModel(message, parent) {}

    /*
            id     x_coordinate    y_coordinate    node_type
        ╭───────┬────────────┬───────────────────┬──────────────────╮
        │  0    │   397.6    │   115.0           │  float_constant  │
        ├───────┼────────────┼───────────────────┼──────────────────┼
        │  1    │   278.1    │   -17.0           │  int_constant    │
        ├───────┼────────────┼───────────────────┼──────────────────┼
        │  ...  │   ...      │   ...             │  ...             │
        ╰───────┴────────────┴───────────────────┴──────────────────╯

        Note: `node_type` is a oneof field. It can be specified as `float_constant`, `int_constant`, etc.
    */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_message->GetReflection()->FieldSize(*m_message, m_message->GetDescriptor()->FindFieldByName("nodes"));
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        // TODO: The oneof field `node_type` must be stored in only one column.
        return m_message->GetDescriptor()->FindFieldByName("nodes")->message_type()->field_count();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) {
            // See https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
            const Descriptor* desc = m_message->GetDescriptor();

            // See https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
            const Reflection* refl = m_message->GetReflection();

            // See https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
            const FieldDescriptor* field = desc->FindFieldByName("nodes");

            int size = refl->FieldSize(*m_message, field);
            // std::cout << "Size: " << size << std::endl;

            if (index.row() < size) {
                const auto& node = refl->GetRepeatedMessage(*m_message, field, index.row());
                // Get the id field and set the new value
                const Reflection* node_refl = node.GetReflection();
                const FieldDescriptor* id_field = node.GetDescriptor()->FindFieldByName("id");
                return QVariant(QString::number(node_refl->GetInt32(node, id_field)));
            }
        }
        return QVariant();
    }

    // Add setData implementation for testing editing
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
        if (role == Qt::EditRole) {
            const Descriptor* desc = m_message->GetDescriptor();
            const Reflection* refl = m_message->GetReflection();
            auto* node = refl->MutableRepeatedMessage(m_message, desc->FindFieldByName("nodes"), index.row());
            // Get the id field and set the new value
            const Reflection* node_refl = node->GetReflection();
            const FieldDescriptor* id_field = node->GetDescriptor()->FindFieldByName("id");
            node_refl->SetInt32(node, id_field, value.toInt());
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        // std::cout << "Index: " << row << ", " << column << std::endl;
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &child) const override {
        return QModelIndex();
    }
};

// Test loading and saving JSON
TEST(ProtoModelTest, TestJsonSerialization) {
    // Create a test VisualShader message and populate it
    VisualShader shader;
    auto* node1 = shader.add_nodes();
    node1->set_id(0);
    node1->set_x_coordinate(10.0);
    node1->set_y_coordinate(20.0);

    // Instantiate the ProtoModel with the VisualShader message
    VisualShaderModel model(&shader);

    QModelIndex index = model.index(0, -1);  // Get the first node

    // Validate the initial data
    ASSERT_EQ(model.data(index).toInt(), 0);

    // Save to JSON file
    QString savePath = "shader_data.json";
    ASSERT_TRUE(model.saveToJson(savePath));

    // Clear data and load from JSON
    VisualShader newShader;
    VisualShaderModel newModel(&newShader);
    ASSERT_TRUE(newModel.loadFromJson(savePath));

    // Validate the loaded data
    ASSERT_EQ(newShader.nodes_size(), 1);
    ASSERT_EQ(newShader.nodes(0).id(), 0);
    ASSERT_EQ(newShader.nodes(0).x_coordinate(), 10.0);
    ASSERT_EQ(newShader.nodes(0).y_coordinate(), 20.0);
}

// Test setting and editing data
TEST(ProtoModelTest, TestSetData) {
    VisualShader shader;
    auto* node1 = shader.add_nodes();
    node1->set_id(0);
    node1->set_x_coordinate(10.0);
    node1->set_y_coordinate(20.0);

    VisualShaderModel model(&shader);

    QModelIndex index = model.index(0, -1);  // Get the first node

    // Validate the initial data
    ASSERT_EQ(model.data(index).toInt(), 0);

    // Edit the data and ensure the model updates
    ASSERT_TRUE(model.setData(index, 42, Qt::EditRole));  // Change the ID to 42
    ASSERT_EQ(shader.nodes(0).id(), 42);  // Validate that the Protobuf model was updated

    // Validate the updated data
    ASSERT_EQ(model.data(index).toInt(), 42);

    // Save the modified data
    QString savePath = "modified_shader_data.json";
    ASSERT_TRUE(model.saveToJson(savePath));
}

// Test setting multiple data
TEST(ProtoModelTest, TestSetDataMultipleTimes) {
    VisualShader shader;
    auto* node1 = shader.add_nodes();
    node1->set_id(0);
    node1->set_x_coordinate(10.0);
    node1->set_y_coordinate(20.0);

    auto* node2 = shader.add_nodes();
    node2->set_id(1);
    node2->set_x_coordinate(30.0);
    node2->set_y_coordinate(40.0);

    auto* node3 = shader.add_nodes();
    node3->set_id(2);
    node3->set_x_coordinate(50.0);
    node3->set_y_coordinate(60.0);

    VisualShaderModel model(&shader);

    QModelIndex index = model.index(0, -1);  // Get the first node

    // Validate the initial data
    ASSERT_EQ(model.data(index).toInt(), 0);

    QModelIndex index2 = model.index(1, -1);  // Get the second node
    ASSERT_EQ(model.data(index2).toInt(), 1);

    QModelIndex index3 = model.index(2, -1);  // Get the third node
    ASSERT_EQ(model.data(index3).toInt(), 2);

    // Edit the data and ensure the model updates
    ASSERT_TRUE(model.setData(index2, 42, Qt::EditRole));  // Change the ID to 42
    ASSERT_EQ(shader.nodes(1).id(), 42);  // Validate that the Protobuf model was updated

    // Validate the updated data
    ASSERT_EQ(model.data(index2).toInt(), 42);

    // Save the modified data
    QString savePath = "modified_shader_data.json";
    ASSERT_TRUE(model.saveToJson(savePath));
}
