#include "gui/model/utils/field_path.hpp"

#include "gui/model/utils/utils.hpp"

bool FieldPath::is_valid_path() const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_empty(), false);
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_root_buffer_descriptor, false);

    const google::protobuf::Descriptor* current_buffer_descriptor {m_root_buffer_descriptor};

    const google::protobuf::FieldDescriptor* repeated_field {nullptr};

    std::queue<PathComponent> temp = m_components;

    while (!temp.empty()) {
        PathComponent component {temp.front()};
        temp.pop();

        if (std::holds_alternative<FieldPath::FieldNumber>(component)) {
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(current_buffer_descriptor, false);
            int fn {std::get<FieldPath::FieldNumber>(component).field_number};
            const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);
            SILENT_CHECK_CONDITION_TRUE_NON_VOID(shadergen_utils::is_inside_real_oneof(field), false);
            current_buffer_descriptor = field->message_type();
            if (field->is_repeated()) repeated_field = field; 
            else repeated_field = nullptr;
        } else if (std::holds_alternative<FieldPath::RepeatedAt>(component)) {
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(repeated_field, false);
            SILENT_CHECK_CONDITION_TRUE_NON_VOID(!repeated_field->is_repeated(), false);
            current_buffer_descriptor = repeated_field->message_type();
            repeated_field = nullptr;
        } else if (std::holds_alternative<FieldPath::OneOfFieldNumber>(component)) {
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(current_buffer_descriptor, false);
            int fn {std::get<FieldPath::OneOfFieldNumber>(component).field_number};
            std::string oneof_n {std::get<FieldPath::OneOfFieldNumber>(component).oneof_name};

            const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);

            SILENT_CHECK_CONDITION_TRUE_NON_VOID(field->is_repeated(), false);
            SILENT_CHECK_CONDITION_TRUE_NON_VOID(!shadergen_utils::is_inside_real_oneof(field), false);

            const google::protobuf::OneofDescriptor* oneof_field {field->real_containing_oneof()};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(oneof_field, false);

            SILENT_CHECK_CONDITION_TRUE_NON_VOID(oneof_field->name() != oneof_n, false);

            current_buffer_descriptor = field->message_type();
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

bool FieldPath::is_upcoming_repeated_index() const {
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
            path_string += std::to_string(std::get<FieldPath::FieldNumber>(component).field_number);
        } else if (std::holds_alternative<FieldPath::RepeatedAt>(component)) {
            path_string += "at(" + std::to_string(std::get<FieldPath::RepeatedAt>(component).index) + ")";
        } else if (std::holds_alternative<FieldPath::OneOfFieldNumber>(component)) {
            path_string += std::get<FieldPath::OneOfFieldNumber>(component).oneof_name + ": " + std::to_string(std::get<FieldPath::OneOfFieldNumber>(component).field_number);
        } else {
            FAIL_AND_RETURN_NON_VOID(path_string, "Invalid path component type");
        }

        if (!temp.empty()) {
            path_string +=  " --> ";
        }
    }

    return path_string;
}

bool FieldPath::get_upcoming_field_num(int& num_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_field(), false);
    num_buffer = std::get<FieldPath::FieldNumber>(m_components.front()).field_number;
    return true;
}

bool FieldPath::get_upcoming_repeated_index(int& index_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_repeated_index(), false);
    index_buffer = std::get<FieldPath::RepeatedAt>(m_components.front()).index;
    return true;
}

bool FieldPath::get_upcoming_oneof_field_num(int& num_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_oneof(), false);
    num_buffer = std::get<FieldPath::OneOfFieldNumber>(m_components.front()).field_number;
    return true;
}

bool FieldPath::get_upcoming_oneof_name(std::string& name_buffer) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_upcoming_oneof(), false);
    name_buffer = std::get<FieldPath::OneOfFieldNumber>(m_components.front()).oneof_name;
    return true;
}
