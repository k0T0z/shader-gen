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

// Test fixture for MessageModel
class MessageModelTest : public ::testing::Test {
protected:
    // Helper method to create a sample Person for testing
    static OrganizationTestSchema create_sample_org() {
        OrganizationTestSchema org;

        auto person1 = org.add_employees();
        person1->set_id(42);
        person1->set_name("John Doe");
        auto* address = person1->mutable_home_address();
        address->set_street("123 Test Street");
        address->set_city("Testville");
        address->set_zip_code("12345");
        auto* phone1 = person1->add_phone_numbers();
        phone1->set_number("555-1234");
        phone1->set_type(PhoneNumber::HOME);
        auto* phone2 = person1->add_phone_numbers();
        phone2->set_number("555-5678");
        phone2->set_type(PhoneNumber::MOBILE);
        person1->add_emails("john.doe@test.com");
        person1->add_emails("john@personal.com");
        person1->set_preferred_email("john.doe@test.com");
        person1->set_company("Test Corporation");

        auto person2 = org.add_employees();
        person2->set_id(22);
        person2->set_name("Mark Smith");
        auto* address2 = person2->mutable_home_address();
        address2->set_street("456 Test Street");
        address2->set_city("Testville");
        address2->set_zip_code("54321");
        auto* phone3 = person2->add_phone_numbers();
        phone3->set_number("555-4321");
        phone3->set_type(PhoneNumber::HOME);
        auto* phone4 = person2->add_phone_numbers();
        phone4->set_number("555-8765");
        phone4->set_type(PhoneNumber::MOBILE);
        person2->add_emails("mark@blah.com");
        person2->add_emails("mark10@stuff.com");
        person2->set_preferred_email("mark@blah.com");
        person2->set_job_title("Engineer");

        return org;
    }
};

TEST_F(MessageModelTest, TestBuildSubModels) {
    OrganizationTestSchema org = create_sample_org();

    // MessageModel model(&org);

    org.set_name("Test Organization");

    EXPECT_EQ(org.name(), "Test Organization");

    const FieldDescriptor* name_field = org.GetDescriptor()->FindFieldByNumber(OrganizationTestSchema::kNameFieldNumber);

    DEBUG_PRINT(name_field->name());
}
