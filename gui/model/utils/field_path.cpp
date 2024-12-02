#include "gui/model/utils/field_path.hpp"

#include "gui/model/utils/utils.hpp"
#include "error_macros.hpp"

bool FieldPath::is_valid() const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_root_buffer_descriptor, false);

    const google::protobuf::Descriptor* current_buffer_descriptor {m_root_buffer_descriptor};

    const google::protobuf::FieldDescriptor* prev_fd {nullptr};

    std::queue<PathComponent> temp = m_components;

    while (!temp.empty()) {
        SILENT_CHECK_PARAM_NULLPTR_NON_VOID(current_buffer_descriptor, false);

        PathComponent component {temp.front()};
        temp.pop();

        if (std::holds_alternative<FieldPath::FieldNumber>(component)) {
            // Access field by number
            int fn {std::get<FieldPath::FieldNumber>(component).field};
            const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);
            SILENT_CHECK_CONDITION_TRUE_NON_VOID(shadergen_utils::is_inside_real_oneof(field), false);

            switch (field->cpp_type()) {
                case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                    if (field->is_repeated()) {
                        prev_fd = field;
                    } else {
                        current_buffer_descriptor = field->message_type();
                        prev_fd = field;
                    }
                    break;
                default:
                    current_buffer_descriptor = nullptr;
                    prev_fd = nullptr;
                    break;
            }
        } else if (std::holds_alternative<FieldPath::RepeatedAt>(component)) {
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(prev_fd, false);
            SILENT_CHECK_CONDITION_TRUE_NON_VOID(!prev_fd->is_repeated(), false);

            switch (prev_fd->cpp_type()) {
                case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                    current_buffer_descriptor = prev_fd->message_type();
                    prev_fd = nullptr;
                    break;
                default:
                    current_buffer_descriptor = nullptr;
                    prev_fd = nullptr;
                    break;
            }
        } else if (std::holds_alternative<FieldPath::OneOfFieldNumber>(component)) {
            int fn {std::get<FieldPath::OneOfFieldNumber>(component).field};
            std::string oneof {std::get<FieldPath::OneOfFieldNumber>(component).oneof};

            const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);

            SILENT_CHECK_CONDITION_TRUE_NON_VOID(!shadergen_utils::is_inside_real_oneof(field), false);

            const google::protobuf::OneofDescriptor* oneof_field {field->containing_oneof()};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(oneof_field, false);

            SILENT_CHECK_CONDITION_TRUE_NON_VOID(oneof_field->name() != oneof, false);

            switch (field->cpp_type()) {
                case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                    current_buffer_descriptor = field->message_type();
                    prev_fd = field;
                    break;
                default:
                    current_buffer_descriptor = nullptr;
                    prev_fd = nullptr;
                    break;
            }
        } else {
            FAIL_AND_RETURN_NON_VOID(false, "Invalid path component type");
        }
    }

    return true;
}

bool FieldPath::is_upcoming_field() const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
    if (std::holds_alternative<FieldPath::FieldNumber>(m_components.front())) {
        return true;
    }
    return false;
}

bool FieldPath::is_upcoming_repeated() const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
    if (std::holds_alternative<FieldPath::RepeatedAt>(m_components.front())) {
        return true;
    }
    return false;
}

bool FieldPath::is_upcoming_oneof() const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
    if (std::holds_alternative<FieldPath::OneOfFieldNumber>(m_components.front())) {
        return true;
    }
    return false;
}

bool FieldPath::skip_component() const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
    m_components.pop();
    return true;
}

std::string FieldPath::to_string() const {
    std::string path_string;
    std::queue<PathComponent> temp = m_components;

    while (!temp.empty()) {
        PathComponent component {temp.front()};
        temp.pop();

        if (std::holds_alternative<FieldPath::FieldNumber>(component)) {
            path_string += std::to_string(std::get<FieldPath::FieldNumber>(component).field) + ".";
        } else if (std::holds_alternative<FieldPath::RepeatedAt>(component)) {
            path_string += "[" + std::to_string(std::get<FieldPath::RepeatedAt>(component).index) + "].";
        } else if (std::holds_alternative<FieldPath::OneOfFieldNumber>(component)) {
            path_string += std::to_string(std::get<FieldPath::OneOfFieldNumber>(component).field) + ".";
        } else {
            FAIL_AND_RETURN_NON_VOID(path_string, "Invalid path component type");
        }
    }

    return path_string;
}

bool FieldPath::get_upcoming_field_num(int& num_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_field(), false);
    num_buffer = std::get<FieldPath::FieldNumber>(m_components.front()).field;
    return true;
}

bool FieldPath::get_upcoming_repeated_index(int& index_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_repeated(), false);
    index_buffer = std::get<FieldPath::RepeatedAt>(m_components.front()).index;
    return true;
}

bool FieldPath::get_upcoming_oneof_num(int& num_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_oneof(), false);
    num_buffer = std::get<FieldPath::OneOfFieldNumber>(m_components.front()).field;
    return true;
}

bool FieldPath::get_upcoming_oneof_name(std::string& name_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_oneof(), false);
    name_buffer = std::get<FieldPath::OneOfFieldNumber>(m_components.front()).oneof;
    return true;
}
