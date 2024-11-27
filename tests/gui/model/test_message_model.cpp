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
    static Person create_sample_person() {
        Person person;
        
        // Set basic fields
        person.set_id(42);
        person.set_name("John Doe");
        
        // Set nested address
        auto* address = person.mutable_home_address();
        address->set_street("123 Test Street");
        address->set_city("Testville");
        address->set_zip_code("12345");
        
        // Add phone numbers
        auto* phone1 = person.add_phone_numbers();
        phone1->set_number("555-1234");
        phone1->set_type(PhoneNumber::HOME);
        
        auto* phone2 = person.add_phone_numbers();
        phone2->set_number("555-5678");
        phone2->set_type(PhoneNumber::MOBILE);
        
        // Add emails
        person.add_emails("john.doe@test.com");
        person.add_emails("john@personal.com");
        
        // Set oneof contact method
        person.set_preferred_email("john.doe@test.com");
        
        // Set oneof employment
        person.set_company("Test Corporation");
        
        return person;
    }
};

TEST_F(MessageModelTest, TestBuildSubModels) {
    // Person person = create_sample_person();

    // MessageModel model(&person);
}
