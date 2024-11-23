#ifndef FIELD_PATH_HPP
#define FIELD_PATH_HPP

#include <vector>
#include <variant>
#include <type_traits>
#include <google/protobuf/message.h>

class FieldPath {
public:
    // Path operations
    struct FieldNumber {
        unsigned field; // Protobuf field number
        explicit FieldNumber(const unsigned& f) : field(f) {}
    };

    struct RepeatedAt {
        unsigned index; // Index in repeated field
        explicit RepeatedAt(const unsigned& i) : index(i) {}
    };

    struct OneOfFieldNumber {
        unsigned field; // Field number within a oneof
        std::string oneof; // Oneof name
        explicit OneOfFieldNumber(const unsigned& f, const std::string& o) : field(f), oneof(o) {}
    };

    // Variant to hold any path component
    using PathComponent = std::variant<FieldNumber, RepeatedAt, OneOfFieldNumber>;

private:
    const google::protobuf::Descriptor* m_root_buffer_descriptor = nullptr;
    std::vector<PathComponent> m_components;

    bool is_valid_oneof(const google::protobuf::FieldDescriptor* field) const {
        if (!field->containing_oneof()) return false;
        // A synthetic oneof has the same name as the field but with a leading underscore.
        return field->containing_oneof()->name() != ('_' + field->name());
    }

    bool is_valid_oneof(const google::protobuf::OneofDescriptor* field) const {
        return field->name() != ('_' + field->name());
    }

public:
    FieldPath() = default;

    // Construct a FieldPath using variadic templates
    template <typename T, typename... Components>
    static FieldPath Of(Components... components) {
        FieldPath path;
        path.m_root_buffer_descriptor = T::GetDescriptor();
        (path.m_components.emplace_back(components), ...); // Add all components
        return path;
    }

    bool is_valid() const {
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

                if (is_valid_oneof(field)) return false;

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

                if (!is_valid_oneof(oneof_field)) return false;

                if (oneof_field->name() != oneof) return false;

                const google::protobuf::FieldDescriptor* field {current_buffer_descriptor->FindFieldByNumber(fn)};
                if (!field) return false;

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

    // Access the components
    const std::vector<PathComponent>& components() const { return m_components; }
};

#endif // FIELD_PATH_HPP
