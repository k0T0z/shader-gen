#include "gui/model/message_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

#include "error_macros.hpp"

using namespace google::protobuf;

MessageModel::MessageModel(Message* message_buffer, ProtoModel* parent, int row_in_parent)
    : ProtoModel(message_buffer, parent), m_message_buffer(message_buffer), m_row_in_parent(row_in_parent) {}

void MessageModel::rebuild_sub_models() {
    // Clear existing sub-models
    m_sub_models_by_row.clear();
    m_sub_models_by_field.clear();

    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);

    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const Reflection* refl {m_message_buffer->GetReflection()};

    int total_fields {desc->field_count()};

    m_sub_models_by_row.resize(total_fields);

    // Iterate through all fields in the message
    for (int i = 0; i < total_fields; ++i) {
        const FieldDescriptor* field = desc->field(i);

        if (field->is_repeated()) {
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
                case FieldDescriptor::CppType::CPPTYPE_ENUM:
                    WARN_PRINT("Enum is not supported yet");
                    break;
                default:
                    CRASH_NOW("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        } else {
            switch (field->cpp_type()) {
                case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
                    if (refl->HasField(*m_message_buffer, field)) {
                        // const Message& sub_message = refl->GetMessage(*m_message_buffer, field);
                        // m_sub_models[i] = std::make_unique<MessageModel>(sub_message.New(), this, i);
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
                case FieldDescriptor::CppType::CPPTYPE_ENUM:
                    WARN_PRINT("Enum is not supported yet");
                    break;
                default:
                    CRASH_NOW("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        }
    }
}

ProtoModel* MessageModel::get_sub_model(int row) const {
    if (row < 0 || row >= static_cast<int>(m_sub_models_by_row.size())) {
        return nullptr; // Invalid row
    }
    return m_sub_models_by_row[row].get();
}
