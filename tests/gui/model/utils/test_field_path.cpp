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
        FieldPath::RepeatedAt(0)       // index is structurally valid
    );
    EXPECT_TRUE(path3.is_valid());

    // Oneof field path
    auto path4 = FieldPath::Of<Person>(
        FieldPath::OneOfFieldNumber(6, "contact_method")  // preferred_email
    );
    EXPECT_TRUE(path4.is_valid());

    // Nested oneof path
    auto path5 = FieldPath::Of<Person>(
        FieldPath::OneOfFieldNumber(10, "employment"),    // employment_details
        FieldPath::OneOfFieldNumber(2, "salary_info")  // annual_salary
    );
    EXPECT_TRUE(path5.is_valid());
}

// Invalid path scenarios
TEST(FieldPathTest, InvalidPathTraversals) {
    // Non-existent field number
    auto path1 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(99)  // Non-existent field
    );
    EXPECT_FALSE(path1.is_valid());

    // Invalid oneof name
    auto path2 = FieldPath::Of<Person>(
        FieldPath::OneOfFieldNumber(6, "non_existent_oneof")
    );
    EXPECT_FALSE(path2.is_valid());

    // Field number in wrong oneof
    auto path3 = FieldPath::Of<Person>(
        FieldPath::OneOfFieldNumber(8, "contact_method")  // company in wrong oneof
    );
    EXPECT_FALSE(path3.is_valid());

    // Attempting to access non-message field with nested path
    auto path4 = FieldPath::Of<Person>(
        FieldPath::FieldNumber(1),  // id (primitive field)
        FieldPath::FieldNumber(2)   // Trying to nest into primitive
    );
    EXPECT_FALSE(path4.is_valid());
}

// Complex path validation
TEST(FieldPathTest, ComplexPathValidation) {
    // Complex nested path through multiple levels
    auto path1 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),     // employees
        FieldPath::RepeatedAt(0),      // first employee
        FieldPath::FieldNumber(3),     // home_address
        FieldPath::FieldNumber(2)      // city
    );
    EXPECT_TRUE(path1.is_valid());

    // Path through multiple nested structures
    auto path2 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),     // employees
        FieldPath::RepeatedAt(0),      // first employee
        FieldPath::OneOfFieldNumber(10, "employment"),    // employment_details
        FieldPath::OneOfFieldNumber(3, "salary_info")  // salary range
    );
    EXPECT_TRUE(path2.is_valid());
}

// Oneof specific validation
TEST(FieldPathTest, OneofValidation) {
    // Valid oneof field selection
    auto path1 = FieldPath::Of<Person>(
        FieldPath::OneOfFieldNumber(7, "contact_method")  // preferred_phone
    );
    EXPECT_TRUE(path1.is_valid());

    // Invalid oneof field selection
    auto path2 = FieldPath::Of<Person>(
        FieldPath::OneOfFieldNumber(8, "contact_method")  // company (wrong oneof)
    );
    EXPECT_FALSE(path2.is_valid());
}

// Edge case path validation
TEST(FieldPathTest, EdgeCaseValidation) {
    // Empty path
    auto path1 = FieldPath::Of<Person>();
    EXPECT_FALSE(path1.is_valid());
}

TEST(FieldPathTest, InvalidOneofName) {
    auto path1 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::OneOfFieldNumber(2, "headquarters")
    );
    EXPECT_FALSE(path1.is_valid());

    auto path2 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(5, "blah blah")

    );
    EXPECT_FALSE(path2.is_valid());
}

TEST(FieldPathTest, WrongOneofName) {
    auto path1 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::OneOfFieldNumber(3, "blah blah")
    );
    EXPECT_FALSE(path1.is_valid());

    auto path2 = FieldPath::Of<OrganizationTestSchema>(
        FieldPath::FieldNumber(2),
        FieldPath::RepeatedAt(0),
        FieldPath::OneOfFieldNumber(10, "blah blah")

    );
    EXPECT_FALSE(path2.is_valid());
}
