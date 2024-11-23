#include <gtest/gtest.h>
#include "gui/model/utils/field_path.hpp"
#include "gui/model/proto_schema/VisualShader.pb.h"  // Generated from your .proto file

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using VisualShader = model::schema::VisualShader;

TEST(FieldPathTest, TestMultipleValidVisualShaderPaths) {
    std::string oneof = "node_type";

    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(1)
    );

    FieldPath p2 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(1)
    );

    FieldPath p3 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(2)
    );

    FieldPath p4 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(3)
    );

    FieldPath p5 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(4, oneof),
        FieldPath::FieldNumber(1)
    );

    FieldPath p6 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(1),
        FieldPath::OneOfFieldNumber(5, oneof),
        FieldPath::FieldNumber(1)
    );

    FieldPath p7 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(3),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(1)
    );

    FieldPath p8 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(3),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(2)
    );

    FieldPath p9 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(3),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(3)
    );

    FieldPath p10 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(3),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(4)
    );

    FieldPath p11 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(3),
        FieldPath::RepeatedAt(0),
        FieldPath::FieldNumber(5)
    );

    FieldPath p12 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(4)
    );

    EXPECT_TRUE(p1.is_valid());
    EXPECT_TRUE(p2.is_valid());
    EXPECT_TRUE(p3.is_valid());
    EXPECT_TRUE(p4.is_valid());
    EXPECT_TRUE(p5.is_valid());
    EXPECT_TRUE(p6.is_valid());
    EXPECT_TRUE(p7.is_valid());
    EXPECT_TRUE(p8.is_valid());
    EXPECT_TRUE(p9.is_valid());
    EXPECT_TRUE(p10.is_valid());
    EXPECT_TRUE(p11.is_valid());
    EXPECT_TRUE(p12.is_valid());
}

TEST(FieldPathTest, TestInvalidFieldNumber) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(0)
    );

    FieldPath p2 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(10)
    );

    EXPECT_FALSE(p1.is_valid());
    EXPECT_FALSE(p2.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessRepeatedFromNonRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(1),
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessRepeatedFromRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOneOfFromPrimitive) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(1),
        FieldPath::OneOfFieldNumber(1, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOneOfFromRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::FieldNumber(2),
        FieldPath::OneOfFieldNumber(1, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfOneOf) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::OneOfFieldNumber(4, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfOneOfRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfRepeatedOneOf) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(1, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfRepeatedRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::RepeatedAt(0),
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfOneOfOneOf) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::OneOfFieldNumber(1, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfOneOfRepeatedOneOf) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(1, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfRepeatedOneOfRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfRepeatedOneOfRepeatedOneOf) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(1, "node_type")
    );

    EXPECT_FALSE(p1.is_valid());
}

TEST(FieldPathTest, TestInvalidAccessOfOneOfRepeatedOneOfRepeated) {
    FieldPath p1 = FieldPath::Of<VisualShader>(
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(1, "node_type"),
        FieldPath::RepeatedAt(0)
    );

    EXPECT_FALSE(p1.is_valid());
}
