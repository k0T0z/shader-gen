#include "gui/model/message_model.hpp"

#include "gui/model/primitive_model.hpp"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/utils/utils.hpp"

#include "error_macros.hpp"

MessageModel::MessageModel(google::protobuf::Message* message_buffer, ProtoModel* parent_model, const int& column_in_parent)
    : ProtoModel(parent_model, column_in_parent), m_message_buffer(message_buffer) {
    rebuild_sub_models();
}

void MessageModel::rebuild_sub_models() {
    // Clear existing sub-models
    m_sub_models_by_column.clear();
    m_sub_models_by_field.clear();

    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#Descriptor
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
    const Reflection* refl {m_message_buffer->GetReflection()};

    int total_fields {desc->field_count()};

    m_sub_models_by_column.resize(total_fields);

    // Iterate through all fields in the message
    for (int i {0}; i < total_fields; ++i) {
        // https://protobuf.dev/reference/cpp/api-docs/google.protobuf.descriptor/#FieldDescriptor
        const FieldDescriptor* field {desc->field(i)};

        DEBUG_PRINT("Field: " + field->full_name());

        if (field->is_repeated()) {
            if (shadergen_utils::is_inside_real_oneof(field)) {
                WARN_PRINT("Repeated fields inside oneof are not supported yet.");
            } else {
                SILENT_CONTINUE_IF_TRUE(refl->FieldSize(*m_message_buffer, field) <= 0);

                switch (field->cpp_type()) {
                    case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
                    case FieldDescriptor::CppType::CPPTYPE_INT32:
                    case FieldDescriptor::CppType::CPPTYPE_INT64:
                    case FieldDescriptor::CppType::CPPTYPE_UINT32:
                    case FieldDescriptor::CppType::CPPTYPE_UINT64:
                    case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
                    case FieldDescriptor::CppType::CPPTYPE_FLOAT:
                    case FieldDescriptor::CppType::CPPTYPE_BOOL:
                    case FieldDescriptor::CppType::CPPTYPE_STRING:
                        break;
                    case FieldDescriptor::CppType::CPPTYPE_ENUM:
                        WARN_PRINT("Enum is not supported yet");
                        break;
                    default:
                        WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
                        break;
                }
            }
        } else {
            if (shadergen_utils::is_inside_real_oneof(field)) {
                WARN_PRINT("Oneof fields are not supported yet.");
            } else {
                switch (field->cpp_type()) {
                    case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                            ProtoModel* sub_model {new MessageModel(refl->MutableMessage(m_message_buffer, field), this, i)};
                            m_sub_models_by_field[field->number()] = sub_model;
                            m_sub_models_by_column[i] = sub_model;
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
                            ProtoModel* sub_model {new PrimitiveModel(field, this, i)};
                            m_sub_models_by_field[field->number()] = sub_model;
                            m_sub_models_by_column[i] = sub_model;
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
}

const FieldDescriptor* MessageModel::get_column_descriptor(const int& column) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    VALIDATE_INDEX_NON_VOID(column, desc->field_count(), nullptr, 
        "Requesting descriptor of invalid column (field index) " + std::to_string(column) + " of MessageModel " + desc->full_name());
    return desc->field(column);
}

bool MessageModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    CHECK_CONDITION_TRUE_NON_VOID(index.isValid(), false, "Supplied index was invalid: " + std::to_string(index.row()) + ", " + std::to_string(index.column()));

    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const Reflection* refl {m_message_buffer->GetReflection()};
    const FieldDescriptor* field {desc->field(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, false);
    const QVariant old_value {this->data(index, role)};

    switch (field->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
            FAIL_AND_RETURN_NON_VOID(false, "Trying to set a message field.");
            break;
        case FieldDescriptor::CppType::CPPTYPE_INT32: refl->SetInt32(m_message_buffer, field, value.toInt()); break;
        case FieldDescriptor::CppType::CPPTYPE_INT64: refl->SetInt64(m_message_buffer, field, value.toLongLong()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT32: refl->SetUInt32(m_message_buffer, field, value.toUInt()); break;
        case FieldDescriptor::CppType::CPPTYPE_UINT64: refl->SetUInt64(m_message_buffer, field, value.toULongLong()); break;
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: refl->SetDouble(m_message_buffer, field, value.toDouble()); break;
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: refl->SetFloat(m_message_buffer, field, value.toFloat()); break;
        case FieldDescriptor::CppType::CPPTYPE_BOOL: refl->SetBool(m_message_buffer, field, value.toBool()); break;
        case FieldDescriptor::CppType::CPPTYPE_STRING: refl->SetString(m_message_buffer, field, value.toString().toStdString()); break;
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
            break;
    }

    emit dataChanged(index, index, {role});
    ProtoModel::parent_data_changed();

    return true;
}

const ProtoModel* MessageModel::get_sub_model(const FieldPath& path) const {
    FAIL_AND_RETURN_NON_VOID(nullptr, "Method not implemented.");
}

QVariant MessageModel::data() const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    return QVariant();
}

bool MessageModel::set_data(const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Method not implemented.");
}

QVariant MessageModel::data(const QModelIndex& index, int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    CHECK_CONDITION_TRUE_NON_VOID(index.isValid(), false, "Supplied index was invalid: " + std::to_string(index.row()) + ", " + std::to_string(index.column()));

    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    const Reflection* refl {m_message_buffer->GetReflection()};

    VALIDATE_INDEX_NON_VOID(index.column(), desc->field_count(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(desc->field_count()));

    const FieldDescriptor* field {desc->field(index.column())};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, QVariant());
    CHECK_CONDITION_TRUE_NON_VOID(field->is_repeated(), QVariant(), "Field is repeated.");

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!refl->HasField(*m_message_buffer, field), QVariant());

    switch (field->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
            FAIL_AND_RETURN_NON_VOID(false, "Trying to get a message field.");
            break;
        case FieldDescriptor::CppType::CPPTYPE_INT32: return refl->GetInt32(*m_message_buffer, field);
        case FieldDescriptor::CppType::CPPTYPE_INT64: return QVariant::fromValue(refl->GetInt64(*m_message_buffer, field));
        case FieldDescriptor::CppType::CPPTYPE_UINT32: return refl->GetUInt32(*m_message_buffer, field);
        case FieldDescriptor::CppType::CPPTYPE_UINT64: return QVariant::fromValue(refl->GetUInt64(*m_message_buffer, field));
        case FieldDescriptor::CppType::CPPTYPE_DOUBLE: return refl->GetDouble(*m_message_buffer, field);
        case FieldDescriptor::CppType::CPPTYPE_FLOAT: return refl->GetFloat(*m_message_buffer, field);
        case FieldDescriptor::CppType::CPPTYPE_BOOL: return refl->GetBool(*m_message_buffer, field);
        case FieldDescriptor::CppType::CPPTYPE_STRING: return QString::fromStdString(refl->GetString(*m_message_buffer, field));
        case FieldDescriptor::CppType::CPPTYPE_ENUM:
            WARN_PRINT("Enum is not supported yet.");
            break;
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(field->cpp_type()));
            break;
    }

    return QVariant();
}

QModelIndex MessageModel::parent(const QModelIndex& child) const {
    return QModelIndex();
}

int MessageModel::columnCount(const QModelIndex& parent) const {
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    int total_fields {desc->field_count()};
    int real_oneofs_count {desc->real_oneof_decl_count()};

    for (int i = 0; i < real_oneofs_count; ++i) {
        total_fields -= desc->oneof_decl(i)->field_count(); // Subtract the fields in the oneof
        total_fields += 1; // One column for the oneof
    }
    
    return total_fields;
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    const Descriptor* desc {m_message_buffer->GetDescriptor()};

    VALIDATE_INDEX_NON_VOID(section, desc->field_count(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(section) + " of " + std::to_string(desc->field_count()));

    const FieldDescriptor* field {desc->field(section)};

    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field, QVariant());

    return QString::fromStdString(field->name());
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex& parent) const {
    return this->createIndex(row, column);
}


