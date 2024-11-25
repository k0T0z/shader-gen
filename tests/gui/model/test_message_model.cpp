#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>
#include "gui/model/message_model.hpp"
#include "tests/gui/model/schema/test_schema.pb.h" // Generated Protobuf classes

using namespace google::protobuf;

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using TestSchema = tests::gui::model::schema::TestSchema;

// Test fixture for MessageModel
class MessageModelTest : public ::testing::Test {
protected:
    // Protobuf instance for testing
    TestSchema test_schema_buffer;

    void SetUp() override {
        // Load a sample JSON into the Protobuf message
        const std::string json = R"json(
        {
            
        })json";

        util::JsonStringToMessage(json, &test_schema_buffer);
    }
};

TEST_F(MessageModelTest, TestRebuildSubModels) {
    MessageModel model(&test_schema_buffer);

    // Rebuild the sub-models
    model.rebuild_sub_models();
}
