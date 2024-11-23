#include "gui/model/utils/field_path.hpp"

bool FieldPath::is_valid() const {
    if (m_components.empty()) return false;

    if (!m_root_buffer_descriptor) return false;

    const google::protobuf::Descriptor* current_buffer_descriptor {m_root_buffer_descriptor};

    const google::protobuf::FieldDescriptor* prev_fd {nullptr};

    for (const auto& component : m_components) {
        if (!current_buffer_descriptor) return false;

        if (std::holds_alternative<FieldPath::FieldNumber>(component)) {
            // Access field by number
            unsigned fn {std::get<FieldPath::FieldNumber>(component).field};
            const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
            if (!field) return false;

            if (is_inside_real_oneof(field)) return false;

            if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE && field->is_repeated()) {
                prev_fd = field;
            } else if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
                current_buffer_descriptor = field->message_type();
                prev_fd = field;
            } else {
                current_buffer_descriptor = nullptr;
                prev_fd = nullptr;
            }
        } else if (std::holds_alternative<FieldPath::RepeatedAt>(component)) {
            if (!prev_fd) return false;
            if (!prev_fd->is_repeated()) return false;

            if (prev_fd->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
                current_buffer_descriptor = prev_fd->message_type();
                prev_fd = nullptr;
            } else {
                current_buffer_descriptor = nullptr;
                prev_fd = nullptr;
            }
        } else if (std::holds_alternative<FieldPath::OneOfFieldNumber>(component)) {
            unsigned fn {std::get<FieldPath::OneOfFieldNumber>(component).field};
            std::string oneof {std::get<FieldPath::OneOfFieldNumber>(component).oneof};

            const google::protobuf::OneofDescriptor* oneof_field {current_buffer_descriptor->FindOneofByName(oneof)};
            if (!oneof_field) return false;

            if (oneof_field->name() != oneof) return false;

            const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
            if (!field) return false;

            if (!is_inside_real_oneof(field)) return false;

            if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
                current_buffer_descriptor = field->message_type();
                prev_fd = field;
            } else {
                current_buffer_descriptor = nullptr;
                prev_fd = nullptr;
            }
        }
    }

    return true;
}
