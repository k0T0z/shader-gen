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
  auto path1 = FieldPath::Of<Person>(FieldPath::FieldNumber(1)  // id field
  );
  ASSERT_TRUE(path1.is_valid());

  // Nested message path
  auto path2 = FieldPath::Of<Person>(FieldPath::FieldNumber(3),  // home_address
                                     FieldPath::FieldNumber(1)   // street
  );
  ASSERT_TRUE(path2.is_valid());

  // Repeated field path (just checking descriptor validity)
  auto path3 = FieldPath::Of<Person>(FieldPath::FieldNumber(4),  // phone_numbers
                                     FieldPath::RepeatedAt(0)    // phone_numbers[0]
  );
  ASSERT_TRUE(path3.is_valid());

  auto path4 = FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(2),  // employees
                                                     FieldPath::RepeatedAt(0),   // employees[0]
                                                     FieldPath::FieldNumber(5),  // emails
                                                     FieldPath::RepeatedAt(0)    // emails[0]
  );
  ASSERT_TRUE(path4.is_valid());
}

// Invalid path scenarios
TEST(FieldPathTest, InvalidPathTraversals) {
  // Non-existent field number
  auto path1 = FieldPath::Of<Person>(FieldPath::FieldNumber(99)  // Non-existent field
  );
  ASSERT_FALSE(path1.is_valid());

  // Attempting to access non-message field with nested path
  auto path2 = FieldPath::Of<Person>(FieldPath::FieldNumber(1),  // id (primitive field)
                                     FieldPath::FieldNumber(2)   // Trying to nest into primitive
  );
  ASSERT_FALSE(path2.is_valid());
}

// Complex path validation
TEST(FieldPathTest, ComplexPathValidation) {
  // Complex nested path through multiple levels
  auto path1 = FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(2),  // employees
                                                     FieldPath::RepeatedAt(0),   // employees[0]
                                                     FieldPath::FieldNumber(3),  // home_address
                                                     FieldPath::FieldNumber(2)   // city
  );
  ASSERT_TRUE(path1.is_valid());

  // Path through multiple nested structures
  auto path2 = FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(2),   // employees
                                                     FieldPath::RepeatedAt(0),    // employees[0]
                                                     FieldPath::FieldNumber(10),  // employment_details
                                                     FieldPath::FieldNumber(3)    // salary range
  );
  ASSERT_TRUE(path2.is_valid());
}

// Edge case path validation
TEST(FieldPathTest, EdgeCaseValidation) {
  // Empty path
  auto path1 = FieldPath::Of<Person>();
  ASSERT_FALSE(path1.is_valid());
}
