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
    const std::vector<PathComponent>& get_components() const { return m_components; }
};

#endif // FIELD_PATH_HPP
