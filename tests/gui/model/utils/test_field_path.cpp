#include <gtest/gtest.h>
#include "gui/model/utils/field_path.hpp"
#include "tests/gui/model/schema/test_schema.pb.h"  // Generated from your .proto file

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using TestSchema = tests::gui::model::schema::TestSchema;

TEST(FieldPathTest, TestInvalidFieldNumber) {
    
}
