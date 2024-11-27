#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>
#include "gui/model/message_model.hpp"
#include "tests/gui/model/schema/test_schema.pb.h" // Generated Protobuf classes
#include "error_macros.hpp"

using namespace google::protobuf;

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using OrganizationTestSchema = tests::gui::model::schema::OrganizationTestSchema;

// Test fixture for MessageModel
class MessageModelTest : public ::testing::Test {
protected:
    // Protobuf instance for testing
    OrganizationTestSchema test_buffer;

    void SetUp() override {
        // Load a sample JSON into the Protobuf message
        const std::string json = R"json(
        {
            
        })json";

        util::JsonStringToMessage(json, &test_buffer);
    }
};

TEST_F(MessageModelTest, TestBuildSubModels) {
    MessageModel model(&test_buffer);
}
