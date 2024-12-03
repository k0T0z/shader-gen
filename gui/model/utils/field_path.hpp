#ifndef FIELD_PATH_HPP
#define FIELD_PATH_HPP

#include <queue>
#include <variant>
#include <type_traits>
#include <google/protobuf/message.h>
#include "error_macros.hpp"

/**
 * @brief A class to represent a path to a field in a protobuf message.
 * 
 * @example 
 * 
 *   - Access a field in a message: 
 *        FieldPath::Of<MyMessage>(FieldPath::FieldNumber(1));
 *   - Access a field in a message in a oneof: 
 *        FieldPath::Of<MyMessage>(FieldPath::OneofFieldNumber("my_oneof", 1));
 *   - Access a repeated field in a message: 
 *        FieldPath::Of<MyMessage>(FieldPath::RepeatedFieldNumber(1, 0)); // Get 
 *        the first element from a repeated field with field number 1
 * 
 *           ----------------------------------------------------
 * 
 *   - Get a message model:
 *        FieldPath::Of<MyMessage>(FieldPath::FieldNumber(1)); // This will return
 *        a message model with the field number 1
 *   - Get a oneof model:
 *        FieldPath::Of<MyMessage>(FieldPath::OneofFieldNumber("my_oneof", -1)); 
 *        // This will return a oneof model with the oneof name "my_oneof"
 *   - Get a repeated model:
 *        FieldPath::Of<MyMessage>(FieldPath::RepeatedFieldNumber(2, -1)); // This
 *        will return a repeated model with the field number 2
 * 
 *           ----------------------------------------------------
 * 
 *   All the above examples are for the top-level message. You can nest the path
 *   - Access a field in a message in a message:
 *        FieldPath::Of<MyMessage>(FieldPath::FieldNumber(1), FieldPath::FieldNumber(2));
 *        // This will return a message model with the field number 2 in a message with 
 *        // the field number 1
 * 
 * 
 */
class FieldPath {
public:
    /**
     * @brief Can be:
     *        - Message
     *        - Enum
     *        - Primitive
     * 
     */
    struct FieldNumber {
        int field_number;
        explicit FieldNumber(const int& f) : field_number(f) {}
    };

    /**
     * @brief Can be:
     *        - Repeated Message
     *        - Repeated Enum
     *        - Repeated Primitive
     * 
     */
    struct RepeatedFieldNumber {
        int field_number;
        int index;
        explicit RepeatedFieldNumber(const int& f, const int& i) : field_number(f), index(i) {}
    };

    /**
     * @brief Can be:
     *        - Message
     *        - Enum
     * 
     */
    struct OneofFieldNumber {
        std::string oneof_name;
        int field_number;
        explicit OneofFieldNumber(const std::string& o, const int& f) : oneof_name(o), field_number(f) {}
    };

    // Variant to hold any path component
    using PathComponent = std::variant<FieldNumber, RepeatedFieldNumber, OneofFieldNumber>;

private:
    bool m_is_valid;
    const google::protobuf::Descriptor* m_root_buffer_descriptor = nullptr;
    mutable std::queue<PathComponent> m_components;

    FieldPath() : m_is_valid(false) {}
    bool is_upcoming_field() const;
    bool is_upcoming_repeated() const;
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
                       std::is_same_v<Components, RepeatedFieldNumber> || 
                       std::is_same_v<Components, OneofFieldNumber>) && ...),
                       "All Components must be FieldNumber, RepeatedFieldNumber, or OneofFieldNumber.");
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
