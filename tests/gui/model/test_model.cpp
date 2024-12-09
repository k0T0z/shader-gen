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

#include <google/protobuf/util/json_util.h>
#include <QSignalSpy>
#include "error_macros.hpp"
#include "gui/model/message_model.hpp"
#include "gui/model/primitive_model.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/model/repeated_primitive_model.hpp"
#include "tests/gui/model/schema/test_schema.pb.h"  // Generated Protobuf classes

using namespace google::protobuf;

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using Person = tests::gui::model::schema::Person;
using PhoneNumber = tests::gui::model::schema::PhoneNumber;
using OrganizationTestSchema = tests::gui::model::schema::OrganizationTestSchema;
using Address = tests::gui::model::schema::Address;
using EmploymentDetails = tests::gui::model::schema::Person::EmploymentDetails;

TEST(MessageModelTest, TestValidSetGet) {
  OrganizationTestSchema org;

  ProtoModel* model = new MessageModel(&org);
  model->build_sub_models();

  // Set OrganizationTestSchema.name
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kNameFieldNumber)),
      "test_org11"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kNameFieldNumber)))
                .toString(),
            "test_org11");
  ASSERT_EQ(org.name(), "test_org11");

  // Add a row to OrganizationTestSchema.employees
  ProtoModel* employees = const_cast<ProtoModel*>(model->get_sub_model(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber))));
  int employee_idx = employees->append_row();
  ASSERT_EQ(employee_idx, 0);

  // Set OrganizationTestSchema.employees[0].id
  ASSERT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(
                                  FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                  FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kIdFieldNumber)),
                              9));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kIdFieldNumber)))
                .toInt(),
            9);
  ASSERT_EQ(org.employees(employee_idx).id(), 9);

  // Set OrganizationTestSchema.employees[0].name
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kNameFieldNumber)),
      "John Doe"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kNameFieldNumber)))
                .toString(),
            "John Doe");
  ASSERT_EQ(org.employees(employee_idx).name(), "John Doe");

  // Set OrganizationTestSchema.employees[0].home_address.street
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(
          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx),
          FieldPath::FieldNumber(Person::kHomeAddressFieldNumber), FieldPath::FieldNumber(Address::kStreetFieldNumber)),
      "163 Test Street"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kHomeAddressFieldNumber),
                    FieldPath::FieldNumber(Address::kStreetFieldNumber)))
                .toString(),
            "163 Test Street");
  ASSERT_EQ(org.employees(employee_idx).home_address().street(), "163 Test Street");

  // Set OrganizationTestSchema.employees[0].home_address.city
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(
          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx),
          FieldPath::FieldNumber(Person::kHomeAddressFieldNumber), FieldPath::FieldNumber(Address::kCityFieldNumber)),
      "Testville"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kHomeAddressFieldNumber),
                    FieldPath::FieldNumber(Address::kCityFieldNumber)))
                .toString(),
            "Testville");
  ASSERT_EQ(org.employees(employee_idx).home_address().city(), "Testville");

  // Set OrganizationTestSchema.employees[0].home_address.zip_code
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kHomeAddressFieldNumber),
                                            FieldPath::FieldNumber(Address::kZipCodeFieldNumber)),
      "12345"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kHomeAddressFieldNumber),
                    FieldPath::FieldNumber(Address::kZipCodeFieldNumber)))
                .toString(),
            "12345");
  ASSERT_EQ(org.employees(employee_idx).home_address().zip_code(), "12345");

  // Add a row to OrganizationTestSchema.employees[0].phone_numbers
  ProtoModel* phone_numbers = const_cast<ProtoModel*>(model->get_sub_model(FieldPath::Of<OrganizationTestSchema>(
      FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx),
      FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber))));
  int phone_idx = phone_numbers->append_row();

  // Set OrganizationTestSchema.employees[0].phone_numbers[0].number
  ASSERT_TRUE(
      model->set_data(FieldPath::Of<OrganizationTestSchema>(
                          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                          FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                          FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)),
                      "555-1234"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                    FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)))
                .toString(),
            "555-1234");
  ASSERT_EQ(org.employees(employee_idx).phone_numbers(phone_idx).number(), "555-1234");

  // Set OrganizationTestSchema.employees[0].phone_numbers[0].type
  ASSERT_TRUE(
      model->set_data(FieldPath::Of<OrganizationTestSchema>(
                          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                          FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                          FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)),
                      PhoneNumber::HOME));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                    FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)))
                .toInt(),
            PhoneNumber::HOME);
  ASSERT_EQ(org.employees(employee_idx).phone_numbers(phone_idx).type(), PhoneNumber::HOME);

  // Add another row to OrganizationTestSchema.employees[0].phone_numbers
  int phone_idx2 = phone_numbers->append_row();
  ASSERT_EQ(phone_idx2, 1);

  // Set OrganizationTestSchema.employees[0].phone_numbers[1].number
  ASSERT_TRUE(
      model->set_data(FieldPath::Of<OrganizationTestSchema>(
                          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                          FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                          FieldPath::RepeatedAt(phone_idx2), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)),
                      "555-1239-00"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                    FieldPath::RepeatedAt(phone_idx2), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)))
                .toString(),
            "555-1239-00");
  ASSERT_EQ(org.employees(employee_idx).phone_numbers(phone_idx2).number(), "555-1239-00");

  // Set OrganizationTestSchema.employees[0].phone_numbers[1].type
  ASSERT_TRUE(
      model->set_data(FieldPath::Of<OrganizationTestSchema>(
                          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                          FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                          FieldPath::RepeatedAt(phone_idx2), FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)),
                      PhoneNumber::WORK));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber),
                    FieldPath::RepeatedAt(phone_idx2), FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)))
                .toInt(),
            PhoneNumber::WORK);
  ASSERT_EQ(org.employees(employee_idx).phone_numbers(phone_idx2).type(), PhoneNumber::WORK);

  // Add a row to OrganizationTestSchema.employees[0].emails
  ProtoModel* emails = const_cast<ProtoModel*>(model->get_sub_model(FieldPath::Of<OrganizationTestSchema>(
      FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx),
      FieldPath::FieldNumber(Person::kEmailsFieldNumber))));
  int email_idx = emails->append_row();

  // Set OrganizationTestSchema.employees[0].emails[0]
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(
          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx),
          FieldPath::FieldNumber(Person::kEmailsFieldNumber), FieldPath::RepeatedAt(email_idx)),
      "rr@st.coom"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmailsFieldNumber),
                    FieldPath::RepeatedAt(email_idx)))
                .toString(),
            "rr@st.coom");
  ASSERT_EQ(org.employees(employee_idx).emails(email_idx), "rr@st.coom");

  // Add another row to OrganizationTestSchema.employees[0].emails
  int email_idx2 = emails->append_row();
  ASSERT_EQ(email_idx2, 1);

  // Set OrganizationTestSchema.employees[0].emails[1]
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(
          FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx),
          FieldPath::FieldNumber(Person::kEmailsFieldNumber), FieldPath::RepeatedAt(email_idx2)),
      "uu@ra.cm"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmailsFieldNumber),
                    FieldPath::RepeatedAt(email_idx2)))
                .toString(),
            "uu@ra.cm");
  ASSERT_EQ(org.employees(employee_idx).emails(email_idx2), "uu@ra.cm");

  // Set OrganizationTestSchema.employees[0].preferred_email
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber)),
      "gg@xc.coom"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber)))
                .toString(),
            "gg@xc.coom");
  ASSERT_EQ(org.employees(employee_idx).preferred_email(), "gg@xc.coom");

  // Set OrganizationTestSchema.employees[0].preferred_phone
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                                            FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)),
      "555-1234-00"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                    FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)))
                .toString(),
            "555-1234-00");
  ASSERT_EQ(org.employees(employee_idx).preferred_phone().number(), "555-1234-00");

  // Set OrganizationTestSchema.employees[0].preferred_phone.type
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                                            FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)),
      PhoneNumber::HOME));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                    FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)))
                .toInt(),
            PhoneNumber::HOME);
  ASSERT_EQ(org.employees(employee_idx).preferred_phone().type(), PhoneNumber::HOME);

  // Set OrganizationTestSchema.employees[0].employment_details.annual_salary
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kEmploymentDetailsFieldNumber),
                                            FieldPath::FieldNumber(EmploymentDetails::kAnnualSalaryFieldNumber)),
      1000000.0));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmploymentDetailsFieldNumber),
                    FieldPath::FieldNumber(EmploymentDetails::kAnnualSalaryFieldNumber)))
                .toDouble(),
            1000000.0);
  ASSERT_EQ(org.employees(employee_idx).employment_details().annual_salary(), 1000000.0);

  // Set OrganizationTestSchema.employees[0].job_title
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kJobTitleFieldNumber)),
      "Software Engineer"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kJobTitleFieldNumber)))
                .toString(),
            "Software Engineer");
  ASSERT_EQ(org.employees(employee_idx).job_title(), "Software Engineer");

  // Add another row to OrganizationTestSchema.employees
  int employee_idx2 = employees->append_row();
  ASSERT_EQ(employee_idx2, 1);

  // Set OrganizationTestSchema.employees[1].name
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx2),
                                            FieldPath::FieldNumber(Person::kNameFieldNumber)),
      "Jane Doe"));

  // Set OrganizationTestSchema.main_office.street
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kMainOfficeFieldNumber),
                                            FieldPath::FieldNumber(Address::kStreetFieldNumber)),
      "123 Test Street"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kMainOfficeFieldNumber),
                    FieldPath::FieldNumber(Address::kStreetFieldNumber)))
                .toString(),
            "123 Test Street");
  ASSERT_EQ(org.main_office().street(), "123 Test Street");

  // Set OrganizationTestSchema.virtual_headquarters
  ASSERT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(
                                  FieldPath::FieldNumber(OrganizationTestSchema::kVirtualHeadquartersFieldNumber)),
                              "Virtual Testville"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kVirtualHeadquartersFieldNumber)))
                .toString(),
            "Virtual Testville");
  ASSERT_EQ(org.virtual_headquarters(), "Virtual Testville");

  delete model;  // For the AddressSanitizer to not complain about memory leaks
}

TEST(MessageModelTest, TestAddRemoveRow) {
  OrganizationTestSchema org;

  ProtoModel* model = new MessageModel(&org);
  model->build_sub_models();

  // Add a row to OrganizationTestSchema.employees
  ProtoModel* employees = const_cast<ProtoModel*>(model->get_sub_model(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber))));
  int employee_idx = employees->append_row();
  ASSERT_EQ(employee_idx, 0);

  // Add another row to OrganizationTestSchema.employees
  int employee_idx2 = employees->append_row();
  ASSERT_EQ(employee_idx2, 1);

  // Set OrganizationTestSchema.employees[0].name
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kNameFieldNumber)),
      "John Doe"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kNameFieldNumber)))
                .toString(),
            "John Doe");
  ASSERT_EQ(org.employees(employee_idx).name(), "John Doe");

  // Set OrganizationTestSchema.employees[1].name
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx2),
                                            FieldPath::FieldNumber(Person::kNameFieldNumber)),
      "Stuff Jane"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx2), FieldPath::FieldNumber(Person::kNameFieldNumber)))
                .toString(),
            "Stuff Jane");
  ASSERT_EQ(org.employees(employee_idx2).name(), "Stuff Jane");

  // Remove the first row from OrganizationTestSchema.employees
  ASSERT_TRUE(employees->remove_row(employee_idx));
  ASSERT_EQ(employees->rowCount(), 1);

  // Try to set second row after removing the first row
  ASSERT_FALSE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx2),
                                            FieldPath::FieldNumber(Person::kNameFieldNumber)),
      "Stuff Jane"));
  ASSERT_FALSE(model
                   ->data(FieldPath::Of<OrganizationTestSchema>(
                       FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                       FieldPath::RepeatedAt(employee_idx2), FieldPath::FieldNumber(Person::kNameFieldNumber)))
                   .isValid());

  // Read the first row name now
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kNameFieldNumber)))
                .toString(),
            "Stuff Jane");
  ASSERT_EQ(org.employees(employee_idx).name(), "Stuff Jane");

  // Remove the first row from OrganizationTestSchema.employees again
  ASSERT_TRUE(employees->remove_row(employee_idx));

  delete model;  // For the AddressSanitizer to not complain about memory leaks
}

TEST(MessageModelTest, TestOneofModel) {
  OrganizationTestSchema org;

  ProtoModel* model = new MessageModel(&org);
  model->build_sub_models();

  ProtoModel* employees = const_cast<ProtoModel*>(model->get_sub_model(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber))));

  // Add a row to OrganizationTestSchema.employees
  int employee_idx = employees->append_row();
  ASSERT_EQ(employee_idx, 0);

  // Set preferred_email
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber)),
      "rf@pl,ck"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber)))
                .toString(),
            "rf@pl,ck");
  ASSERT_EQ(org.employees(employee_idx).preferred_email(), "rf@pl,ck");

  // Get preferred_phone.number
  ASSERT_FALSE(model
                   ->data(FieldPath::Of<OrganizationTestSchema>(
                       FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                       FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                       FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)))
                   .isValid());

  // Set preferred_phone.number
  ASSERT_TRUE(model->set_data(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                                            FieldPath::RepeatedAt(employee_idx),
                                            FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                                            FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)),
      "555-1234-00"));
  ASSERT_EQ(model
                ->data(FieldPath::Of<OrganizationTestSchema>(
                    FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                    FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber),
                    FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)))
                .toString(),
            "555-1234-00");
  ASSERT_EQ(org.employees(employee_idx).preferred_phone().number(), "555-1234-00");

  // Get preferred_email
  ASSERT_FALSE(model
                   ->data(FieldPath::Of<OrganizationTestSchema>(
                       FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber),
                       FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber)))
                   .isValid());

  delete model;  // For the AddressSanitizer to not complain about memory leaks
}

TEST(MessageModelTest, TestDataChangedSignalPrimitiveModel) {
  OrganizationTestSchema org;

  ProtoModel* model = new MessageModel(&org);
  model->build_sub_models();

  ProtoModel* employees = const_cast<ProtoModel*>(model->get_sub_model(
      FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber))));

  // Add a row to OrganizationTestSchema.employees
  int employee_idx = employees->append_row();
  ASSERT_EQ(employee_idx, 0);

  RepeatedMessageModel* t_employees = dynamic_cast<RepeatedMessageModel*>(employees);

  ProtoModel* first_employee = dynamic_cast<ProtoModel*>(t_employees->get_sub_model(employee_idx));
  ASSERT_TRUE(first_employee);

  ProtoModel* phone_numbers = const_cast<ProtoModel*>(
      first_employee->get_sub_model(FieldPath::Of<Person>(FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber))));
  int phone_idx = phone_numbers->append_row();

  RepeatedMessageModel* t_phone_numbers = dynamic_cast<RepeatedMessageModel*>(phone_numbers);

  ProtoModel* first_phone_number = dynamic_cast<ProtoModel*>(t_phone_numbers->get_sub_model(phone_idx));
  ASSERT_TRUE(first_phone_number);

  ProtoModel* number = const_cast<ProtoModel*>(first_phone_number->get_sub_model(
      FieldPath::Of<PhoneNumber>(FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber))));
  ASSERT_TRUE(number);

  QModelIndex top_left = number->index(0, 0);
  ASSERT_TRUE(top_left.isValid());

  QSignalSpy model_spy(model, &ProtoModel::dataChanged);
  ASSERT_TRUE(model_spy.isValid());
  QSignalSpy employees_spy(employees, &ProtoModel::dataChanged);
  ASSERT_TRUE(employees_spy.isValid());
  QSignalSpy first_employee_spy(first_employee, &ProtoModel::dataChanged);
  ASSERT_TRUE(first_employee_spy.isValid());
  QSignalSpy phone_numbers_spy(phone_numbers, &ProtoModel::dataChanged);
  ASSERT_TRUE(phone_numbers_spy.isValid());
  QSignalSpy first_phone_number_spy(first_phone_number, &ProtoModel::dataChanged);
  ASSERT_TRUE(first_phone_number_spy.isValid());
  QSignalSpy number_spy(number, &ProtoModel::dataChanged);
  ASSERT_TRUE(number_spy.isValid());

  number->setData(top_left, "555-1234-00");

  ASSERT_EQ(model_spy.count(), 1);
  ASSERT_EQ(employees_spy.count(), 1);
  ASSERT_EQ(first_employee_spy.count(), 1);
  ASSERT_EQ(phone_numbers_spy.count(), 1);
  ASSERT_EQ(first_phone_number_spy.count(), 1);
  ASSERT_EQ(number_spy.count(), 1);

  // Verify the emitted indices
  auto verify_indices = [](QSignalSpy& spy, const QModelIndex& expected_top_left,
                           const QModelIndex& expected_bottom_right) {
    QList<QVariant> arguments = spy.takeFirst();
    QModelIndex actual_top_left = arguments.at(0).value<QModelIndex>();
    QModelIndex actual_bottom_right = arguments.at(1).value<QModelIndex>();

    DEBUG_PRINT("Actual top left: " + std::to_string(actual_top_left.row()) + ", " +
                std::to_string(actual_top_left.column()));
    DEBUG_PRINT("Expected top left: " + std::to_string(expected_top_left.row()) + ", " +
                std::to_string(expected_top_left.column()));

    DEBUG_PRINT("Actual bottom right: " + std::to_string(actual_bottom_right.row()) + ", " +
                std::to_string(actual_bottom_right.column()));
    DEBUG_PRINT("Expected bottom right: " + std::to_string(expected_bottom_right.row()) + ", " +
                std::to_string(expected_bottom_right.column()));

    ASSERT_EQ(actual_top_left, expected_top_left);
    ASSERT_EQ(actual_bottom_right, expected_bottom_right);
  };

  // Expected to change row 0 and column 0
  verify_indices(number_spy, top_left, top_left);

  // Expected to change row 0 and column 0 (field number)
  QModelIndex expected_top_left = first_phone_number->index(0, 0);
  ASSERT_TRUE(expected_top_left.isValid());
  verify_indices(first_phone_number_spy, expected_top_left, expected_top_left);

  // Expected to change row 0 and column 0 (field number)
  QModelIndex expected_top_left2 = phone_numbers->index(0, 0);
  ASSERT_TRUE(expected_top_left2.isValid());
  verify_indices(phone_numbers_spy, expected_top_left2, expected_top_left2);

  // Expected to change row 0 and column 3 (field phone_numbers)
  QModelIndex expected_top_left3 = first_employee->index(0, 3);
  ASSERT_TRUE(expected_top_left3.isValid());
  verify_indices(first_employee_spy, expected_top_left3, expected_top_left3);

  // Expected to change row 0 and column 3 (field phone_numbers)
  QModelIndex expected_top_left4 = employees->index(0, 3);
  ASSERT_TRUE(expected_top_left4.isValid());
  verify_indices(employees_spy, expected_top_left4, expected_top_left4);

  // Expected to change row 0 and column 1 (field employees)
  QModelIndex expected_top_left5 = model->index(0, 1);
  ASSERT_TRUE(expected_top_left5.isValid());
  verify_indices(model_spy, expected_top_left5, expected_top_left5);

  delete model;  // For the AddressSanitizer to not complain about memory leaks
}
