#include "gui/model/message_model.hpp"

#include "gui/model/primitive_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/utils/utils.hpp"
#include "gui/model/repeated_message_model.hpp"
#include "gui/model/repeated_primitive_model.hpp"
#include "gui/model/oneof_model.hpp"

#include "error_macros.hpp"

using OneofDescriptor = google::protobuf::OneofDescriptor;

MessageModel::MessageModel(Message* message_buffer, ProtoModel* parent_model, const int& index_in_parent)
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer) {}

void MessageModel::build_sub_models() {
    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
    const Reflection* refl {m_message_buffer->GetReflection()};

    int total_fields {columnCount()};

    // Iterate through all fields in the message
    for (int i {0}; i < total_fields; i++) {
        // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
        const FieldDescriptor* field {get_column_descriptor(i)};

        if (field->is_repeated()) {
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
                case FieldDescriptor::CppType::CPPTYPE_STRING: 
                case FieldDescriptor::CppType::CPPTYPE_ENUM: {
                        ProtoModel* sub_model {new RepeatedPrimitiveModel(m_message_buffer, field, this, i)};
                        sub_model->build_sub_models();
                        m_sub_models_by_field_number[field->number()] = sub_model;
                        break;
                    }
                default:
                    WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
                    break;
            }
        } else {
            if (shadergen_utils::is_inside_real_oneof(field)) {
                const OneofDescriptor* oneof {field->real_containing_oneof()};
                auto it = m_sub_models_by_oneof_name.find(oneof->name());
                if (it == m_sub_models_by_oneof_name.end()) {
                    ProtoModel* sub_model {new OneofModel(m_message_buffer, oneof, this, i)};
                    sub_model->build_sub_models();
                    m_sub_models_by_oneof_name[oneof->name()] = sub_model;
                }
                continue;
            }

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
                case FieldDescriptor::CppType::CPPTYPE_STRING:
                case FieldDescriptor::CppType::CPPTYPE_ENUM: {
                        ProtoModel* sub_model {new PrimitiveModel(m_message_buffer, field, this, i)};
                        m_sub_models_by_field_number[field->number()] = sub_model;
                        break;
                    }
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
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a MessageModel.");
}

const ProtoModel* MessageModel::get_sub_model(const int& field_number) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const FieldDescriptor* field {desc->FindFieldByNumber(field_number)};
    if (shadergen_utils::is_inside_real_oneof(field)) {
        const OneofDescriptor* oneof {field->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), nullptr);
        return it->second;
    }

    auto it {m_sub_models_by_field_number.find(field_number)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_field_number.end(), nullptr);
    return it->second;
}

const ProtoModel* MessageModel::get_sub_model(const FieldPath& path) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, nullptr);
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + desc->full_name());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(path.is_empty(), this);

    int fn {-1};

    CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_field_num(fn) && !path.get_upcoming_oneof_field_num(fn), nullptr, "Next component is not a field number.");

    const FieldDescriptor* field {desc->FindFieldByNumber(fn)};
    if (shadergen_utils::is_inside_real_oneof(field)) {
        const OneofDescriptor* oneof {field->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), nullptr);
        return it->second->get_sub_model(path);
    }
    
    auto it {m_sub_models_by_field_number.find(fn)};
    CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_field_number.end(), nullptr, "Sub-model under " + desc->full_name() + " not found for field number " + std::to_string(fn));

    CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr, "Failed to skip field number.");

    return it->second->get_sub_model(path);
}

const FieldDescriptor* MessageModel::get_column_descriptor(const int& column) const {
    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
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

    const FieldDescriptor* field {get_column_descriptor(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, QVariant());

    CHECK_CONDITION_TRUE_NON_VOID(field->is_repeated(), QVariant(), "Field is repeated.");

    if (shadergen_utils::is_inside_real_oneof(field)) {
        const OneofDescriptor* oneof {field->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), QVariant());
        return get_sub_model(field->number())->data(this->index(0, map_to_oneof_index(index.column()), index), role);
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

    const FieldDescriptor* field {get_column_descriptor(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);

    CHECK_CONDITION_TRUE_NON_VOID(field->is_repeated(), false, "Field is repeated.");

    if (shadergen_utils::is_inside_real_oneof(field)) {
        const OneofDescriptor* oneof {field->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), false);
        return const_cast<ProtoModel*>(get_sub_model(field->number()))->setData(this->index(0, map_to_oneof_index(index.column()), index), value, role);
    }

    return const_cast<ProtoModel*>(get_sub_model(field->number()))->setData(this->index(0, 0, index), value, role);
}

QVariant MessageModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(orientation != Qt::Orientation::Horizontal, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(role != Qt::DisplayRole, QVariant());
    VALIDATE_INDEX_NON_VOID(section, columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(section) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field {get_column_descriptor(section)};
    CHECK_PARAM_NULLPTR_NON_VOID(field, QVariant(), "Field is null.");

    return QString::fromStdString(field->full_name());
}

void MessageModel::clear_sub_models() {
    for (auto& [field_number, sub_model] : m_sub_models_by_field_number) {
        delete sub_model;
    }

    m_sub_models_by_field_number.clear();
}

int MessageModel::map_to_oneof_index(const int& field_index) const {
    const FieldDescriptor* field {get_column_descriptor(field_index)};
    
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!shadergen_utils::is_inside_real_oneof(field), -1);

    const OneofDescriptor* oneof {field->real_containing_oneof()};

    // Find the index of this field within its oneof
    for (int i {0}; i < oneof->field_count(); ++i) {
        if (oneof->field(i)->number() == field->number()) {
            return i;
        }
    }

    return -1;
}
