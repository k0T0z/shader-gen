#ifndef FIELD_PATH_HPP
#define FIELD_PATH_HPP

#include <queue>
#include <variant>
#include <type_traits>
#include <google/protobuf/message.h>
#include "error_macros.hpp"

class FieldPath {
public:
    /**
     * @brief Can be:
     *        - Message
     *        - Enum
     *        - Primitive
     *        - Repeated Message
     *        - Repeated Enum
     *        - Repeated Primitive
     * 
     */
    struct FieldNumber {
        int field_number;
        explicit FieldNumber(const int& f) : field_number(f) {}
    };

    struct RepeatedAt {
        int index;
        explicit RepeatedAt(const int& i) : index(i) {}
    };

    /**
     * @brief Can be:
     *        - Message
     *        - Enum
     * 
     */
    struct OneOfFieldNumber {
        int field_number;
        std::string oneof_name;
        explicit OneOfFieldNumber(const int& f, const std::string& o) : field_number(f), oneof_name(o) {}
    };

    // Variant to hold any path component
    using PathComponent = std::variant<FieldNumber, RepeatedAt, OneOfFieldNumber>;

private:
    bool m_is_valid;
    const google::protobuf::Descriptor* m_root_buffer_descriptor = nullptr;
    mutable std::queue<PathComponent> m_components;

    FieldPath() : m_is_valid(false) {}
    bool is_upcoming_field() const;
    bool is_upcoming_repeated_index() const;
    bool is_upcoming_oneof() const;

    /**
     * @brief Check if the path is valid
     * 
     * @note @c private because it should be called only on a full path. This
     *       means if you skipped a component, the path becomes invalid because
     *       the root buffer descriptor is no longer the correct one.
     * 
     * @return true 
     * @return false 
     */
    bool is_valid_path() const;

public:
    // Construct a FieldPath using variadic templates
    template <typename T, typename... Components>
    static FieldPath Of(Components... components) {
        static_assert(((std::is_same_v<Components, FieldNumber> || 
                       std::is_same_v<Components, RepeatedAt> || 
                       std::is_same_v<Components, OneOfFieldNumber>) && ...),
                       "All Components must be FieldNumber, RepeatedAt, or OneOfFieldNumber.");
        FieldPath path;
        path.m_root_buffer_descriptor = T::GetDescriptor();
        (path.m_components.push(components), ...); // Add all components
        CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid_path(), FieldPath(), "Invalid path: " + path.to_string());
        path.m_is_valid = true;
        return path;
    }

    bool is_empty() const { return m_components.empty(); }
    bool is_valid() const { return m_is_valid; }

    bool get_upcoming_field_num(int& num_buffer) const;
    bool get_upcoming_repeated_index(int& index_buffer) const;
    bool get_upcoming_oneof_field_num(int& num_buffer) const;
    bool get_upcoming_oneof_name(std::string& name_buffer) const;

    bool skip_component() const;

    std::string to_string() const;

    // Access the components
    const std::queue<PathComponent>& get_components() const { return m_components; }
};

#endif // FIELD_PATH_HPP
