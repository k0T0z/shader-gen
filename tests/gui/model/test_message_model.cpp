#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>
#include "gui/model/message_model.hpp"
#include "gui/model/proto_schema/VisualShader.pb.h" // Generated Protobuf classes

using namespace google::protobuf;

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using VisualShader = model::schema::VisualShader;

// Test fixture for MessageModel
class MessageModelTest : public ::testing::Test {
protected:
    // Protobuf instance for testing
    VisualShader shader;

    void SetUp() override {
        // Load a sample JSON into the Protobuf message
        const std::string json = R"json(
        {
            "id": 123,
            "nodes": [
                {
                    "id": 0,
                    "x_coordinate": 10.5,
                    "y_coordinate": 20.5,
                    "float_constant": {
                        "value": 3.14
                    }
                },
                {
                    "id": 1,
                    "x_coordinate": 15.0,
                    "y_coordinate": 25.0,
                    "int_constant": {
                        "value": 42
                    }
                }
            ],
            "connections": [
                {
                    "id": 0,
                    "from_node_id": 1,
                    "from_port_index": 0,
                    "to_node_id": 2,
                    "to_port_index": 1
                }
            ],
            "fragment_shader_code": "void main() {}"
        })json";

        util::JsonStringToMessage(json, &shader);
    }
};

TEST_F(MessageModelTest, TestRebuildSubModels) {
    MessageModel model(&shader);

    // Rebuild the sub-models
    model.rebuild_sub_models();
}
