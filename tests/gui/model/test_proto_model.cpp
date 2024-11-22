#include <gtest/gtest.h>
#include "gui/model/proto_model.hpp"
#include "gui/model/proto_schema/VisualShader.pb.h"  // Generated from your .proto file

// Mock subclass for testing with VisualShader
class VisualShaderModel : public ProtoModel {
public:
    VisualShaderModel(google::protobuf::Message* message, QObject* parent = nullptr)
        : ProtoModel(message, parent) {}
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_message->GetReflection()->FieldSize(*m_message, m_message->GetDescriptor()->FindFieldByName("nodes"));
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1; // For simplicity, assume just one column (e.g., node information)
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) {
            const auto& nodes = m_message->GetReflection()->GetRepeatedField(*m_message, m_message->GetDescriptor()->FindFieldByName("nodes"));
            if (index.row() < nodes.size()) {
                const auto& node = nodes.Get(index.row());
                return QVariant(QString::number(node.id()));
            }
        }
        return QVariant();
    }

    // Add setData implementation for testing editing
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
        if (role == Qt::EditRole) {
            auto* node = m_message->GetReflection()->MutableRepeatedMessage(m_message, m_message->GetDescriptor()->FindFieldByName("nodes"), index.row());
            node->set_id(value.toInt());  // Set the ID of the node
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }
};

// Test loading and saving JSON
TEST(ProtoModelTest, TestJsonSerialization) {
    // Create a test VisualShader message and populate it
    VisualShader shader;
    auto* node1 = shader.add_nodes();
    node1->set_id(1);
    node1->set_x_coordinate(10.0);
    node1->set_y_coordinate(20.0);

    // Instantiate the ProtoModel with the VisualShader message
    VisualShaderModel model(&shader);

    // Save to JSON file
    QString savePath = "shader_data.json";
    ASSERT_TRUE(model.saveToJson(savePath));

    // Clear data and load from JSON
    VisualShader newShader;
    VisualShaderModel newModel(&newShader);
    ASSERT_TRUE(newModel.loadFromJson(savePath));

    // Validate the loaded data
    ASSERT_EQ(newShader.nodes_size(), 1);
    ASSERT_EQ(newShader.nodes(0).id(), 1);
    ASSERT_EQ(newShader.nodes(0).x_coordinate(), 10.0);
    ASSERT_EQ(newShader.nodes(0).y_coordinate(), 20.0);
}

// Test setting and editing data
TEST(ProtoModelTest, TestSetData) {
    VisualShader shader;
    auto* node1 = shader.add_nodes();
    node1->set_id(1);
    node1->set_x_coordinate(10.0);
    node1->set_y_coordinate(20.0);

    VisualShaderModel model(&shader);

    QModelIndex index = model.index(0, 0);  // Get the first node

    // Edit the data and ensure the model updates
    ASSERT_TRUE(model.setData(index, 42, Qt::EditRole));  // Change the ID to 42
    ASSERT_EQ(shader.nodes(0).id(), 42);  // Validate that the Protobuf model was updated

    // Save the modified data
    QString savePath = "modified_shader_data.json";
    ASSERT_TRUE(model.saveToJson(savePath));
}
