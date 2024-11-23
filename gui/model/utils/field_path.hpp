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

    bool is_inside_real_oneof(const google::protobuf::FieldDescriptor* field) const {
        if (!field->containing_oneof()) return false;

        // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor.real_oneof_decl_count.details
        int real_oneofs_count {field->containing_type()->real_oneof_decl_count()};

        // Check if the field is in a real oneof, not a synthetic oneof
        for (int i = 0; i < real_oneofs_count; ++i) {
            // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor.oneof_decl.details
            // Real oneofs always come first, so iterating up to real_oneof_decl_cout() will yield all real oneofs.
            if (field->containing_oneof() == field->containing_type()->oneof_decl(i)) {
                return true;
            }
        }

        return false;
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

    bool is_valid() const;

    // Access the components
    const std::vector<PathComponent>& components() const { return m_components; }
};

#endif // FIELD_PATH_HPP
