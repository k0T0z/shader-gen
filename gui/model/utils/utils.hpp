#ifndef UTILS_HPP
#define UTILS_HPP

#include <google/protobuf/descriptor.h>

namespace shadergen_utils {
    inline static bool is_inside_real_oneof(const google::protobuf::FieldDescriptor* field) noexcept {
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
} // namespace shadergen_utils

#endif // UTILS_HPP
