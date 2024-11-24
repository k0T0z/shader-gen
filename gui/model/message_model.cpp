#include "gui/model/message_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

#include "error_macros.hpp"

MessageModel::MessageModel(google::protobuf::Message* message_buffer, ProtoModel* parent_model, const int& column_in_parent)
    : ProtoModel(parent_model, column_in_parent), m_message_buffer(message_buffer) {}

void MessageModel::rebuild_sub_models() {
    // Clear existing sub-models
    m_sub_models_by_column.clear();
    m_sub_models_by_field.clear();

    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
    const Reflection* refl {m_message_buffer->GetReflection()};

    int total_fields {desc->field_count()};

    m_sub_models_by_column.resize(total_fields);

    // Iterate through all fields in the message
    for (int i {0}; i < total_fields; ++i) {
        // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
        const FieldDescriptor* field {desc->field(i)};

        DEBUG_PRINT("Field: " + field->full_name());

        if (field->is_repeated()) {
            SILENT_CONTINUE_IF_TRUE(refl->FieldSize(*m_message_buffer, field) <= 0);

            switch (field->cpp_type()) {
                case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
                case FieldDescriptor::CppType::CPPTYPE_INT32:
                case FieldDescriptor::CppType::CPPTYPE_INT64:
                case FieldDescriptor::CppType::CPPTYPE_UINT32:
                case FieldDescriptor::CppType::CPPTYPE_UINT64:
                case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
                case FieldDescriptor::CppType::CPPTYPE_FLOAT:
                case FieldDescriptor::CppType::CPPTYPE_BOOL:
                case FieldDescriptor::CppType::CPPTYPE_STRING:
                    break;
                case FieldDescriptor::CppType::CPPTYPE_ENUM:
                    WARN_PRINT("Enum is not supported yet");
                    break;
                default:
                    WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        } else {
            switch (field->cpp_type()) {
                case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                        ProtoModel* sub_model {new MessageModel(refl->MutableMessage(m_message_buffer, field), this, i)};
                        m_sub_models_by_field[field->number()] = sub_model;
                        m_sub_models_by_column[i] = sub_model;
                    }
                    break;
                case FieldDescriptor::CppType::CPPTYPE_INT32:
                case FieldDescriptor::CppType::CPPTYPE_INT64:
                case FieldDescriptor::CppType::CPPTYPE_UINT32:
                case FieldDescriptor::CppType::CPPTYPE_UINT64:
                case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
                case FieldDescriptor::CppType::CPPTYPE_FLOAT:
                case FieldDescriptor::CppType::CPPTYPE_BOOL:
                case FieldDescriptor::CppType::CPPTYPE_STRING:
                    break;
                case FieldDescriptor::CppType::CPPTYPE_ENUM:
                    WARN_PRINT("Enum is not supported yet.");
                    break;
                default:
                    WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        }
    }
}

