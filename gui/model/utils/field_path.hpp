#ifndef FIELD_PATH_HPP
#define FIELD_PATH_HPP

#include <queue>
#include <variant>
#include <type_traits>
#include <google/protobuf/message.h>

class FieldPath {
public:
    // Path operations
    struct FieldNumber {
        int field; // Protobuf field number
        explicit FieldNumber(const int& f) : field(f) {}
    };

    struct RepeatedAt {
        int index; // Index in repeated field
        explicit RepeatedAt(const int& i) : index(i) {}
    };

    struct OneOfFieldNumber {
        int field; // Field number within a oneof
        std::string oneof; // Oneof name
        explicit OneOfFieldNumber(const int& f, const std::string& o) : field(f), oneof(o) {}
    };

    // Variant to hold any path component
    using PathComponent = std::variant<FieldNumber, RepeatedAt, OneOfFieldNumber>;

private:
    const google::protobuf::Descriptor* m_root_buffer_descriptor = nullptr;
    mutable std::queue<PathComponent> m_components;

    bool is_upcoming_field() const;
    bool is_upcoming_repeated() const;
    bool is_upcoming_oneof() const;

public:
    FieldPath() = default;

    // Construct a FieldPath using variadic templates
    template <typename T, typename... Components>
    static FieldPath Of(Components... components) {
        FieldPath path;
        path.m_root_buffer_descriptor = T::GetDescriptor();
        (path.m_components.push(components), ...); // Add all components
        return path;
    }

    bool is_valid() const;

    bool get_upcoming_field_num(int& num_buffer) const;
    bool get_upcoming_repeated_index(int& index_buffer) const;
    bool get_upcoming_oneof_num(int& num_buffer) const;
    bool get_upcoming_oneof_name(std::string& name_buffer) const;

    bool skip_component() const;

    // Access the components
    const std::queue<PathComponent>& get_components() const { return m_components; }
};

#endif // FIELD_PATH_HPP
