#include <gtest/gtest.h>
#include "gui/model/utils/field_path.hpp"
#include "tests/gui/model/schema/test_schema.pb.h"  // Generated from your .proto file

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using Person = tests::gui::model::schema::Person;
using PhoneNumber = tests::gui::model::schema::PhoneNumber;
using OrganizationTestSchema = tests::gui::model::schema::OrganizationTestSchema;

// Valid paths through different structures
TEST(FieldPathTest, ValidPathTraversals) {
    // Simple field number path
    auto path1 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(1)  // id field
    );
    EXPECT_TRUE(path1.is_valid());

    // Nested message path
    auto path2 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(3),  // home_address
        FieldPath::FieldNumber(1)   // street
    );
    EXPECT_TRUE(path2.is_valid());

    // Repeated field path (just checking descriptor validity)
    auto path3 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(4),     // phone_numbers
        FieldPath::RepeatedAt(0)       // phone_numbers[0]
    );
    EXPECT_TRUE(path3.is_valid());

    auto path4 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),     // employees
        FieldPath::RepeatedAt(0),       // employees[0]
        FieldPath::FieldNumber(5),      // emails
        FieldPath::RepeatedAt(0)        // emails[0]
    );
    EXPECT_TRUE(path4.is_valid());
}

// Invalid path scenarios
TEST(FieldPathTest, InvalidPathTraversals) {
    // Non-existent field number
    auto path1 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(99)  // Non-existent field
    );
    EXPECT_FALSE(path1.is_valid());

    // Attempting to access non-message field with nested path
    auto path2 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(1),  // id (primitive field)
        FieldPath::FieldNumber(2)   // Trying to nest into primitive
    );
    EXPECT_FALSE(path2.is_valid());
}

// Complex path validation
TEST(FieldPathTest, ComplexPathValidation) {
    // Complex nested path through multiple levels
    auto path1 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),     // employees
        FieldPath::RepeatedAt(0),       // employees[0]
        FieldPath::FieldNumber(3),     // home_address
        FieldPath::FieldNumber(2)      // city
    );
    EXPECT_TRUE(path1.is_valid());

    // Path through multiple nested structures
    auto path2 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),     // employees
        FieldPath::RepeatedAt(0),       // employees[0]
        FieldPath::FieldNumber(10),    // employment_details
        FieldPath::FieldNumber(3)  // salary range
    );
    EXPECT_TRUE(path2.is_valid());
}

// Edge case path validation
TEST(FieldPathTest, EdgeCaseValidation) {
    // Empty path
    auto path1 = FieldPath::Of<Person>();
    EXPECT_FALSE(path1.is_valid());
}
