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
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer), last_accessed_field_index(-1) {}

void MessageModel::build_sub_models() {
    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
    const Reflection* refl {m_message_buffer->GetReflection()};

    int total_fields {columnCount()};

    // Iterate through all fields in the message
    for (int i {0}; i < total_fields; i++) {
        // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
        const FieldDescriptor* field_desc {get_column_descriptor(i)};

        SILENT_CONTINUE_IF_TRUE(m_sub_models_by_field_number.find(field_desc->number()) != m_sub_models_by_field_number.end());

        if (field_desc->is_repeated()) {
            switch (field_desc->cpp_type()) {
                case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                    ProtoModel* sub_model {new RepeatedMessageModel(m_message_buffer, field_desc, this, i)};
                    sub_model->build_sub_models();
                    m_sub_models_by_field_number[field_desc->number()] = sub_model;
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
                    ProtoModel* sub_model {new RepeatedPrimitiveModel(m_message_buffer, field_desc, this, i)};
                    sub_model->build_sub_models();
                    m_sub_models_by_field_number[field_desc->number()] = sub_model;
                    break;
                }
                default:
                    WARN_PRINT("Unsupported field type: " + std::to_string(field_desc->cpp_type()));
                    break;
            }
        } else {
            if (shadergen_utils::is_inside_real_oneof(field_desc)) {
                const OneofDescriptor* oneof_desc {field_desc->real_containing_oneof()};
                auto it = m_sub_models_by_oneof_name.find(oneof_desc->name());
                SILENT_CONTINUE_IF_TRUE(it != m_sub_models_by_oneof_name.end());
                ProtoModel* sub_model {new OneofModel(m_message_buffer, oneof_desc, this, i)};
                sub_model->build_sub_models();
                m_sub_models_by_oneof_name[oneof_desc->name()] = sub_model;
            } else {
                switch (field_desc->cpp_type()) {
                    case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                        ProtoModel* sub_model {new MessageModel(refl->MutableMessage(m_message_buffer, field_desc), this, i)};
                        sub_model->build_sub_models();
                        m_sub_models_by_field_number[field_desc->number()] = sub_model;
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
                        ProtoModel* sub_model {new PrimitiveModel(m_message_buffer, field_desc, this, i)};
                        m_sub_models_by_field_number[field_desc->number()] = sub_model;
                        break;
                    }
                    default:
                        WARN_PRINT("Unsupported field type: " + std::to_string(field_desc->cpp_type()));
                        break;
                }
            }
        }
    }
}

void MessageModel::parent_data_changed() const {
    const ProtoModel* parent {get_parent_model()};
    SILENT_CHECK_PARAM_NULLPTR(parent);

    QModelIndex index {this->parent(QModelIndex())};
    CHECK_CONDITION_TRUE(!index.isValid(), "Parent index is invalid.");

    ProtoModel* t {const_cast<ProtoModel*>(parent)};

    if (RepeatedMessageModel* repeated_m {dynamic_cast<RepeatedMessageModel*>(t)}) {
        Q_EMIT repeated_m->dataChanged(index, index);
        repeated_m->parent_data_changed();
        return;
    } else if (MessageModel* message_m {dynamic_cast<MessageModel*>(t)}) {
        Q_EMIT message_m->dataChanged(index, index);
        message_m->parent_data_changed();
        return;
    } else if (OneofModel* oneof_m {dynamic_cast<OneofModel*>(t)}) {
        Q_EMIT oneof_m->dataChanged(index, index);
        oneof_m->parent_data_changed();
        return;
    }

    ERROR_PRINT("Parent model is not a repeated message model, message model, or oneof model.");
}

QVariant MessageModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from MessageModel.");
}

bool MessageModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a MessageModel.");
}

const ProtoModel* MessageModel::get_sub_model(const int& field_number) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const FieldDescriptor* field_desc {desc->FindFieldByNumber(field_number)};
    if (shadergen_utils::is_inside_real_oneof(field_desc)) {
        const OneofDescriptor* oneof_desc {field_desc->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof_desc->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), nullptr);
        return it->second;
    }

    auto it {m_sub_models_by_field_number.find(field_number)};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_field_number.end(), nullptr);
    return it->second;
}

const ProtoModel* MessageModel::get_sub_model(const FieldPath& path, const bool& for_set_data, const bool& for_get_oneof) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, nullptr);
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + desc->full_name());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(path.is_empty(), this);

    int fn {-1};

    CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_field_num(fn), nullptr, "Next component is not a field number.");

    const FieldDescriptor* field_desc {desc->FindFieldByNumber(fn)};

    if (shadergen_utils::is_inside_real_oneof(field_desc)) {
        const OneofDescriptor* oneof_desc {field_desc->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof_desc->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), nullptr);
        return it->second->get_sub_model(path, for_set_data, for_get_oneof);
    }
    
    auto it {m_sub_models_by_field_number.find(fn)};
    CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_field_number.end(), nullptr, "Sub-model under " + desc->full_name() + " not found for field number " + std::to_string(fn));

    CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr, "Failed to skip field number.");

    last_accessed_field_index = field_desc->index(); // This is important for back propagation of dataChanged signal
    
    return it->second->get_sub_model(path, for_set_data, for_get_oneof);
}

const FieldDescriptor* MessageModel::get_column_descriptor(const int& column) const {
    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    VALIDATE_INDEX_NON_VOID(column, columnCount(), nullptr, 
        "Requesting descriptor of invalid column (field index) " + std::to_string(column) + " of MessageModel " + desc->full_name());
    return desc->field(column);
}

QModelIndex MessageModel::parent([[maybe_unused]] const QModelIndex& child) const {
    const ProtoModel* parent {get_parent_model()};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(parent, QModelIndex());

    ProtoModel* t {const_cast<ProtoModel*>(parent)};

    /*
        Here we need to return an index that represents the parent of the this message model.
        The index should contain the row and column of the parent model in its parent. 
        
        If the parent model is a RepeatedMessageModel, then the row should be the index in it.
        While the column will be last_accessed_field_index value. Check last_accessed_field_index
        for more information.
        
        If the parent model is a MessageModel, then the row should be 0 and the column should 
        be the index in it. Same goes for OneofModel.
        
        Note that the parent model can be a RepeatedMessageModel, MessageModel, or OneofModel 
        as primitive models cannot have children except for repeated primitive models which are
        only allowed to have children of type PrimitiveModel.
    */
    if (RepeatedMessageModel* repeated_m {dynamic_cast<RepeatedMessageModel*>(t)}) {
        return repeated_m->index(m_index_in_parent, last_accessed_field_index, repeated_m->parent(QModelIndex()));
    } else if (MessageModel* message_m {dynamic_cast<MessageModel*>(t)}) {
        return message_m->index(0, m_index_in_parent, message_m->parent(QModelIndex()));
    } else if (OneofModel* oneof_m {dynamic_cast<OneofModel*>(t)}) {
        return oneof_m->index(0, m_index_in_parent, oneof_m->parent(QModelIndex()));
    }

    FAIL_AND_RETURN_NON_VOID(QModelIndex(), "Parent model is not a repeated message model, message model, or oneof model.");
}

int MessageModel::columnCount([[maybe_unused]] const QModelIndex& parent) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    return desc->field_count();
}

QVariant MessageModel::data(const QModelIndex& index, [[maybe_unused]] int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, QVariant(), "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field_desc {get_column_descriptor(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field_desc, QVariant());

    CHECK_CONDITION_TRUE_NON_VOID(field_desc->is_repeated(), QVariant(), "Field is repeated.");

    if (shadergen_utils::is_inside_real_oneof(field_desc)) {
        const OneofDescriptor* oneof_desc {field_desc->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof_desc->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), QVariant());
        return get_sub_model(field_desc->number())->data(this->index(0, map_to_oneof_index(index.column()), index), role);
    }

    return get_sub_model(field_desc->number())->data(this->index(0, 0, index), role);
}

bool MessageModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(value.isNull(), false, "Value is null.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, false, "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), false, 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field_desc {get_column_descriptor(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field_desc, false);

    CHECK_CONDITION_TRUE_NON_VOID(field_desc->is_repeated(), false, "Field is repeated.");

    if (shadergen_utils::is_inside_real_oneof(field_desc)) {
        const OneofDescriptor* oneof_desc {field_desc->real_containing_oneof()};
        auto it {m_sub_models_by_oneof_name.find(oneof_desc->name())};
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(it == m_sub_models_by_oneof_name.end(), false);
        return const_cast<ProtoModel*>(get_sub_model(field_desc->number()))->setData(this->index(0, map_to_oneof_index(index.column()), index), value, role);
    }

    return const_cast<ProtoModel*>(get_sub_model(field_desc->number()))->setData(this->index(0, 0, index), value, role);
}

QVariant MessageModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(orientation != Qt::Orientation::Horizontal, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(role != Qt::DisplayRole, QVariant());
    VALIDATE_INDEX_NON_VOID(section, columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(section) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field_desc {get_column_descriptor(section)};
    CHECK_PARAM_NULLPTR_NON_VOID(field_desc, QVariant(), "Field is null.");

    return QString::fromStdString(field_desc->full_name());
}

void MessageModel::clear_sub_models() {
    for (auto& [field_number, sub_model] : m_sub_models_by_field_number) delete sub_model;
    m_sub_models_by_field_number.clear();

    for (auto& [oneof_name, sub_model] : m_sub_models_by_oneof_name) delete sub_model;
    m_sub_models_by_oneof_name.clear();
}

int MessageModel::map_to_oneof_index(const int& field_index) const {
    const FieldDescriptor* field_desc {get_column_descriptor(field_index)};
    
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!shadergen_utils::is_inside_real_oneof(field_desc), -1);

    const OneofDescriptor* oneof_desc {field_desc->real_containing_oneof()};

    // Find the index of this field within its oneof
    for (int i {0}; i < oneof_desc->field_count(); ++i) {
        if (oneof_desc->field(i)->number() == field_desc->number()) return i;
    }

    return -1;
}
