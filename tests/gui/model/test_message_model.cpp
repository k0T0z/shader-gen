#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>
#include "gui/model/message_model.hpp"
#include "tests/gui/model/schema/test_schema.pb.h" // Generated Protobuf classes
#include "error_macros.hpp"

using namespace google::protobuf;

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using Person = tests::gui::model::schema::Person;
using PhoneNumber = tests::gui::model::schema::PhoneNumber;
using OrganizationTestSchema = tests::gui::model::schema::OrganizationTestSchema;
using Address = tests::gui::model::schema::Address;

TEST(MessageModelTest, TestBuildSubModels) {
    OrganizationTestSchema org;

    ProtoModel* model = new MessageModel(&org);

    model->build_sub_models();

    EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kNameFieldNumber)), "test_org11"));
    EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kNameFieldNumber))).toString(), "test_org11");
    EXPECT_EQ(org.name(), "test_org11");

    // Add a row to OrganizationTestSchema.employees
    ProtoModel* employees = const_cast<ProtoModel*>(model->get_sub_model(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber))));
    int employee_idx = employees->append_row();

    // Set OrganizationTestSchema.employees[0].name
    EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kNameFieldNumber)), "John Doe"));
    EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kNameFieldNumber))).toString(), "John Doe");
    EXPECT_EQ(org.employees(employee_idx).name(), "John Doe");

    // Set OrganizationTestSchema.employees[0].home_address.street
    EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kHomeAddressFieldNumber), FieldPath::FieldNumber(Address::kStreetFieldNumber)), "163 Test Street"));
    EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kHomeAddressFieldNumber), FieldPath::FieldNumber(Address::kStreetFieldNumber))).toString(), "163 Test Street");
    EXPECT_EQ(org.employees(employee_idx).home_address().street(), "163 Test Street");

    // Add a row to OrganizationTestSchema.employees[0].phone_numbers
    ProtoModel* phone_numbers = const_cast<ProtoModel*>(model->get_sub_model(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber))));
    int phone_idx = phone_numbers->append_row();

    // Set OrganizationTestSchema.employees[0].phone_numbers[0].number
    EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber), FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)), "555-1234"));
    EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber), FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber))).toString(), "555-1234");
    EXPECT_EQ(org.employees(employee_idx).phone_numbers(phone_idx).number(), "555-1234");

    // Set OrganizationTestSchema.employees[0].phone_numbers[0].type
    EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber), FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber)), PhoneNumber::HOME));
    EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPhoneNumbersFieldNumber), FieldPath::RepeatedAt(phone_idx), FieldPath::FieldNumber(PhoneNumber::kTypeFieldNumber))).toInt(), PhoneNumber::HOME);
    EXPECT_EQ(org.employees(employee_idx).phone_numbers(phone_idx).type(), PhoneNumber::HOME);

    // Add a row to OrganizationTestSchema.employees[0].emails
    ProtoModel* emails = const_cast<ProtoModel*>(model->get_sub_model(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmailsFieldNumber))));
    int email_idx = emails->append_row();

    // Set OrganizationTestSchema.employees[0].emails[0]
    EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmailsFieldNumber), FieldPath::RepeatedAt(email_idx)), "rr@st.coom"));
    EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmailsFieldNumber), FieldPath::RepeatedAt(email_idx))).toString(), "rr@st.coom");
    EXPECT_EQ(org.employees(employee_idx).emails(email_idx), "rr@st.coom");

    // // Set OrganizationTestSchema.employees[0].preferred_email
    // EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber)), "gg@xc.coom"));
    // EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredEmailFieldNumber))).toString(), "gg@xc.coom");
    // EXPECT_EQ(org.employees(employee_idx).preferred_email(), "gg@xc.coom");

    // // Set OrganizationTestSchema.employees[0].preferred_phone
    // EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber)), "555-1234-00"));
    // EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kPreferredPhoneFieldNumber), FieldPath::FieldNumber(PhoneNumber::kNumberFieldNumber))).toString(), "555-1234-00");
    // EXPECT_EQ(org.employees(employee_idx).preferred_phone().number(), "555-1234-00");

    // // Set OrganizationTestSchema.employees[0].employment_details.annual_salary
    // EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmploymentDetailsFieldNumber), FieldPath::FieldNumber(Person::EmploymentDetails::kAnnualSalaryFieldNumber)), "1000000"));
    // EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kEmployeesFieldNumber), FieldPath::RepeatedAt(employee_idx), FieldPath::FieldNumber(Person::kEmploymentDetailsFieldNumber), FieldPath::FieldNumber(Person::EmploymentDetails::kAnnualSalaryFieldNumber))).toString(), "1000000");
    // EXPECT_EQ(org.employees(employee_idx).employment_details().annual_salary(), 1000000);

    // // Set OrganizationTestSchema.main_office.street
    // EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kMainOfficeFieldNumber), FieldPath::FieldNumber(Address::kStreetFieldNumber)), "123 Test Street"));
    // EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kMainOfficeFieldNumber), FieldPath::FieldNumber(Address::kStreetFieldNumber))).toString(), "123 Test Street");
    // EXPECT_EQ(org.main_office().street(), "123 Test Street");

    // // Set OrganizationTestSchema.virtual_headquarters
    // EXPECT_TRUE(model->set_data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kVirtualHeadquartersFieldNumber)), "Virtual Testville"));
    // EXPECT_EQ(model->data(FieldPath::Of<OrganizationTestSchema>(FieldPath::FieldNumber(OrganizationTestSchema::kVirtualHeadquartersFieldNumber))).toString(), "Virtual Testville");
    // EXPECT_EQ(org.virtual_headquarters(), "Virtual Testville");
}
