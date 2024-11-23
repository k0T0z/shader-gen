#include "gui/model/message_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

using namespace google::protobuf;

MessageModel::MessageModel(ProtoModel* parent, Message* message_buffer, int row_in_parent)
    : ProtoModel(message_buffer, parent), m_message_buffer(message_buffer), m_row_in_parent(row_in_parent) {}

void MessageModel::rebuild_sub_models() {
    // Clear existing sub-models
    m_sub_models.clear();

    if (!m_message_buffer) return;

    const Descriptor* descriptor = m_message_buffer->GetDescriptor();
    const Reflection* reflection = m_message_buffer->GetReflection();

    // Iterate through all fields in the message
    for (int i = 0; i < descriptor->field_count(); ++i) {
        const FieldDescriptor* field = descriptor->field(i);

        if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE && !field->is_repeated()) {
            // Create a sub-model for a nested message
            Message* sub_message = reflection->MutableMessage(m_message_buffer, field);
            m_sub_models.emplace_back(std::make_unique<MessageModel>(this, sub_message, i));
        }
    }
}

ProtoModel* MessageModel::get_sub_model(int row) const {
    if (row < 0 || row >= static_cast<int>(m_sub_models.size())) {
        return nullptr; // Invalid row
    }
    return m_sub_models[row].get();
}
