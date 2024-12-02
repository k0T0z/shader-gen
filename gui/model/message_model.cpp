#include "gui/model/message_model.hpp"

#include "gui/model/primitive_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/utils/utils.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/model/repeated_primitive_model.hpp"

#include "error_macros.hpp"

using OneofDescriptor = google::protobuf::OneofDescriptor;

MessageModel::MessageModel(Message* message_buffer, ProtoModel* parent_model, const int& index_in_parent)
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer) {}

void MessageModel::build_sub_models() {
    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
    const Reflection* refl {m_message_buffer->GetReflection()};

    int total_fields {columnCount()};

    // Iterate through all fields in the message
    for (int i {0}; i < total_fields; i++) {
        // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
        const FieldDescriptor* field {desc->field(i)};

        DEBUG_PRINT("Field: " + field->full_name());

        if (field->is_repeated()) {
            // SILENT_CONTINUE_IF_TRUE(refl->FieldSize(*m_message_buffer, field) <= 0);

            switch (field->cpp_type()) {
                case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                        ProtoModel* sub_model {new RepeatedMessageModel(m_message_buffer, field, this, i)};
                        sub_model->build_sub_models();
                        m_sub_models_by_field_number[field->number()] = sub_model;
                        break;
                    }
                case FieldDescriptor::CppType::CPPTYPE_INT32:
                case FieldDescriptor::CppType::CPPTYPE_INT64:
                case FieldDescriptor::CppType::CPPTYPE_UINT32:
                case FieldDescriptor::CppType::CPPTYPE_UINT64:
                case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
                case FieldDescriptor::CppType::CPPTYPE_FLOAT:
                case FieldDescriptor::CppType::CPPTYPE_BOOL:
                case FieldDescriptor::CppType::CPPTYPE_STRING: {
                        ProtoModel* sub_model {new RepeatedPrimitiveModel(m_message_buffer, field, this, i)};
                        sub_model->build_sub_models();
                        m_sub_models_by_field_number[field->number()] = sub_model;
                        break;
                    }
                case FieldDescriptor::CppType::CPPTYPE_ENUM:
                    WARN_PRINT("Enum is not supported yet");
                    break;
                default:
                    WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        } else {
            // Only build sub-models for field that is inside a real oneof and is set.
            // if (shadergen_utils::is_inside_real_oneof(field)) {
            //     const OneofDescriptor* oneof {field->containing_oneof()};
            //     SILENT_CONTINUE_IF_TRUE(!refl->HasOneof(*m_message_buffer, oneof));

            //     const FieldDescriptor* oneof_field {refl->GetOneofFieldDescriptor(*m_message_buffer, oneof)};
            //     SILENT_CONTINUE_IF_TRUE(oneof_field == nullptr);
            //     SILENT_CONTINUE_IF_TRUE(oneof_field->number() != field->number());
            // } else {
            //     SILENT_CONTINUE_IF_TRUE(!refl->HasField(*m_message_buffer, field));
            // }

            switch (field->cpp_type()) {
                case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                        ProtoModel* sub_model {new MessageModel(refl->MutableMessage(m_message_buffer, field), this, i)};
                        sub_model->build_sub_models();
                        m_sub_models_by_field_number[field->number()] = sub_model;
                        break;
                    }
                case FieldDescriptor::CppType::CPPTYPE_INT32:
                case FieldDescriptor::CppType::CPPTYPE_INT64:
                case FieldDescriptor::CppType::CPPTYPE_UINT32:
                case FieldDescriptor::CppType::CPPTYPE_UINT64:
                case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
                case FieldDescriptor::CppType::CPPTYPE_FLOAT:
                case FieldDescriptor::CppType::CPPTYPE_BOOL:
                case FieldDescriptor::CppType::CPPTYPE_STRING: {
                        ProtoModel* sub_model {new PrimitiveModel(m_message_buffer, field, this, i)};
                        m_sub_models_by_field_number[field->number()] = sub_model;
                        break;
                    }
                case FieldDescriptor::CppType::CPPTYPE_ENUM:
                    WARN_PRINT("Enum is not supported yet.");
                    break;
                default:
                    WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        }
    }
}

QVariant MessageModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from MessageModel.");
}

bool MessageModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data to MessageModel.");
}

const ProtoModel* MessageModel::get_sub_model(const int& field_number) const {
    auto it {m_sub_models_by_field_number.find(field_number)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_field_number.end(), nullptr);
    return it->second;
}

const ProtoModel* MessageModel::get_sub_model(const FieldPath& path) const {
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "");

    int fn {-1};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_field_num(fn), nullptr);

    auto it {m_sub_models_by_field_number.find(fn)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_field_number.end(), nullptr);

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr);

    return it->second->get_sub_model(path);
}

const FieldDescriptor* MessageModel::get_column_descriptor(const int& column) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    VALIDATE_INDEX_NON_VOID(column, columnCount(), nullptr, 
        "Requesting descriptor of invalid column (field index) " + std::to_string(column) + " of MessageModel " + desc->full_name());
    return desc->field(column);
}

QModelIndex MessageModel::index(int row, int column, [[maybe_unused]] const QModelIndex& parent) const {
    Q_UNUSED(parent);
    CHECK_CONDITION_TRUE_NON_VOID(row > 0, QModelIndex(), "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(column, columnCount(), QModelIndex(), 
        "Accessing out-of-range proto column " + std::to_string(column) + " of " + std::to_string(columnCount()));

    return createIndex(row, column);
}

QModelIndex MessageModel::parent([[maybe_unused]] const QModelIndex& child) const {
    Q_UNUSED(child);
    const ProtoModel* parent_model {get_parent_model()};

    const ProtoModel* root_model {get_root_model()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_model == root_model, QModelIndex());

    /*
        Here we need to return an index that represents the parent of the child parameter.
        The index should contain the row and column of the parent model in its parent. If 
        the parent model is a RepeatedMessageModel, then the get_index_in_parent() should be
        the row. If the parent model is a MessageModel, then the get_index_in_parent() should
        be the column because a MessageModel is allowed to have only one row. Note that the
        parent model can be a RepeatedMessageModel or a MessageModel only as primitive models
        are not allowed to have children.
    */
    RepeatedMessageModel* parent_repeat_model {dynamic_cast<RepeatedMessageModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_repeat_model != nullptr, createIndex(parent_model->get_index_in_parent(), -1));

    MessageModel* parent_message_model {dynamic_cast<MessageModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_message_model != nullptr, createIndex(0, parent_model->get_index_in_parent()));

    return QModelIndex();
}

int MessageModel::columnCount([[maybe_unused]] const QModelIndex& parent) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    int total_fields {desc->field_count()};
    // int real_oneofs_count {desc->real_oneof_decl_count()};

    // for (int i = 0; i < real_oneofs_count; ++i) {
    //     total_fields -= desc->oneof_decl(i)->field_count(); // Subtract the fields in the oneof
    //     total_fields += 1; // One column for the oneof
    // }
    
    return total_fields;
}

QVariant MessageModel::data(const QModelIndex& index, [[maybe_unused]] int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, QVariant(), "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const Reflection* refl {m_message_buffer->GetReflection()};

    const FieldDescriptor* field {desc->field(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, QVariant());

    CHECK_CONDITION_TRUE_NON_VOID(field->is_repeated(), QVariant(), "Field is repeated.");

    if (shadergen_utils::is_inside_real_oneof(field)) {
        const OneofDescriptor* oneof {field->containing_oneof()};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(!refl->HasOneof(*m_message_buffer, oneof), QVariant());

        const FieldDescriptor* oneof_field {refl->GetOneofFieldDescriptor(*m_message_buffer, oneof)};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(oneof_field == nullptr, QVariant());
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(oneof_field->number() != field->number(), QVariant());
    } else {
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(!refl->HasField(*m_message_buffer, field), QVariant());
    }

    return get_sub_model(field->number())->data(this->index(0, 0, index), role);
}

bool MessageModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, false, "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), false, 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const Reflection* refl {m_message_buffer->GetReflection()};
    const FieldDescriptor* field {desc->field(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);

    CHECK_CONDITION_TRUE_NON_VOID(field->is_repeated(), false, "Field is repeated.");

    if (shadergen_utils::is_inside_real_oneof(field)) {
        const OneofDescriptor* oneof {field->containing_oneof()};

        if (refl->HasOneof(*m_message_buffer, oneof)) {
            const FieldDescriptor* oneof_field {refl->GetOneofFieldDescriptor(*m_message_buffer, oneof)};
            SILENT_CHECK_PARAM_NULLPTR_NON_VOID(oneof_field, false);
            if (oneof_field->number() != field->number()) refl->ClearOneof(m_message_buffer, oneof);
        }
    }

    return const_cast<ProtoModel*>(get_sub_model(field->number()))->setData(this->index(0, 0, index), value, role);
}

QVariant MessageModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(orientation != Qt::Orientation::Horizontal, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(role != Qt::DisplayRole, QVariant());
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    VALIDATE_INDEX_NON_VOID(section, columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(section) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field {desc->field(section)};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, QVariant());

    return QString::fromStdString(field->full_name());
}

void MessageModel::clear_sub_models() {
    for (auto& [field_number, sub_model] : m_sub_models_by_field_number) {
        delete sub_model;
    }

    m_sub_models_by_field_number.clear();
}
