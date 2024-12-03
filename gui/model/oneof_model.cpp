#include "gui/model/oneof_model.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include "gui/model/primitive_model.hpp"
#include "gui/model/message_model.hpp"

#include "error_macros.hpp"

OneofModel::OneofModel(Message* message_buffer, const OneofDescriptor* oneof_desc, ProtoModel* parent_model, const int& index_in_parent)
    : ProtoModel(parent_model, index_in_parent), m_message_buffer(message_buffer), m_oneof_desc(oneof_desc), m_current_field_desc(nullptr), m_sub_model(nullptr) {}

void OneofModel::build_sub_models() {
    SILENT_CHECK_CONDITION_TRUE(is_set()); // You are not allowed to call this function if the oneof field is already set
    SILENT_CHECK_PARAM_NULLPTR(m_message_buffer);
    const Reflection* refl {m_message_buffer->GetReflection()};
    SILENT_CHECK_PARAM_NULLPTR(m_oneof_desc);

    const FieldDescriptor* oneof_field {refl->GetOneofFieldDescriptor(*m_message_buffer, m_oneof_desc)};
    SILENT_CHECK_PARAM_NULLPTR(oneof_field);

    int total_fields {columnCount()};

    for (int i {0}; i < total_fields; i++) {
        SILENT_CONTINUE_IF_TRUE(oneof_field->number() != m_oneof_desc->field(i)->number());

        m_current_field_desc = oneof_field; // Set the current field descriptor

        switch (oneof_field->cpp_type()) {
            case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
                ProtoModel* sub_model {new MessageModel(refl->MutableMessage(m_message_buffer, oneof_field), this, i)};
                sub_model->build_sub_models();
                m_sub_model = sub_model;
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
                ProtoModel* sub_model {new PrimitiveModel(m_message_buffer, oneof_field, this, i)};
                m_sub_model = sub_model;
                break;
            }
            default:
                WARN_PRINT("Unsupported field type: " + std::to_string(oneof_field->cpp_type()));
                break;
        }
    }
}

QVariant OneofModel::data() const {
    FAIL_AND_RETURN_NON_VOID(QVariant(), "Cannot get data from MessageModel.");
}

bool OneofModel::set_data([[maybe_unused]] const QVariant& value) {
    FAIL_AND_RETURN_NON_VOID(false, "Cannot set data in a MessageModel.");
}

const ProtoModel* OneofModel::get_sub_model(const int& field_number) const {
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_set(), nullptr);
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(m_current_field_desc->number() != field_number, nullptr);
    return m_sub_model;
}

const ProtoModel* OneofModel::get_sub_model(const FieldPath& path, const bool& for_set_data) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, nullptr);
    const Descriptor* desc {m_message_buffer->GetDescriptor()};
    CHECK_CONDITION_TRUE_NON_VOID(!path.is_valid(), nullptr, "Invalid path for " + desc->full_name());

    int fn {-1};

    CHECK_CONDITION_TRUE_NON_VOID(!path.get_upcoming_field_num(fn), nullptr, "Next component is not a field number.");

    const FieldDescriptor* field_desc {desc->FindFieldByNumber(fn)};
    CHECK_PARAM_NULLPTR_NON_VOID(field_desc, nullptr, "Field descriptor is null.");

    // We must set the oneof manually if we are setting data
    if (for_set_data) {
        SILENT_CHECK_CONDITION_TRUE_NON_VOID(!set_oneof(field_desc), nullptr);
    }

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_set(), nullptr);
    CHECK_CONDITION_TRUE_NON_VOID(m_current_field_desc->number() != field_desc->number(), nullptr, "Field number mismatch.");

    CHECK_CONDITION_TRUE_NON_VOID(!path.skip_component(), nullptr, "Failed to skip field number.");

    return m_sub_model->get_sub_model(path);
}

const FieldDescriptor* OneofModel::get_column_descriptor(const int& column) const {
    VALIDATE_INDEX_NON_VOID(column, columnCount(), nullptr, 
        "Requesting descriptor of invalid column (field index) " + std::to_string(column) + " of MessageModel " + m_oneof_desc->full_name());
    return m_oneof_desc->field(column);
}

QModelIndex OneofModel::index(int row, int column, [[maybe_unused]] const QModelIndex& parent) const {
    Q_UNUSED(parent);
    CHECK_CONDITION_TRUE_NON_VOID(row > 0, QModelIndex(), "A oneof model should have only one row.");
    VALIDATE_INDEX_NON_VOID(column, columnCount(), QModelIndex(), 
        "Accessing out-of-range proto column " + std::to_string(column) + " of " + std::to_string(columnCount()));

    return createIndex(row, column);
}

QModelIndex OneofModel::parent([[maybe_unused]] const QModelIndex& child) const {
    Q_UNUSED(child);
    const ProtoModel* parent_model {get_parent_model()};

    const ProtoModel* root_model {get_root_model()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_model == root_model, QModelIndex());

    MessageModel* parent_message_model {dynamic_cast<MessageModel*>(const_cast<ProtoModel*>(parent_model->get_parent_model()))};
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(parent_message_model != nullptr, createIndex(0, parent_model->get_index_in_parent()));

    return QModelIndex();
}

int OneofModel::columnCount([[maybe_unused]] const QModelIndex& parent) const {
    return m_oneof_desc->field_count();
}

QVariant OneofModel::data(const QModelIndex& index, [[maybe_unused]] int role) const {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant(), "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), QVariant(), "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, QVariant(), "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!is_set(), QVariant());

    const FieldDescriptor* field_desc {m_oneof_desc->field(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field_desc, QVariant());

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(m_current_field_desc->number() != field_desc->number(), QVariant());

    return get_sub_model(field_desc->number())->data(this->index(0, 0, index), role);
}

bool OneofModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, false, "Message buffer is null.");
    CHECK_CONDITION_TRUE_NON_VOID(!index.isValid(), false, "Supplied index was invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(!value.isValid(), false, "Supplied value is invalid.");
    CHECK_CONDITION_TRUE_NON_VOID(index.row() > 0, false, "A message model should have only one row.");
    VALIDATE_INDEX_NON_VOID(index.column(), columnCount(), false, 
        "Accessing out-of-range proto column " + std::to_string(index.column()) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field_desc {m_oneof_desc->field(index.column())};
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(field_desc, false);

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!set_oneof(field_desc), false);

    return const_cast<ProtoModel*>(get_sub_model(field_desc->number()))->setData(this->index(0, 0, index), value, role);
}

QVariant OneofModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role) const {
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m_message_buffer, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(orientation != Qt::Orientation::Horizontal, QVariant());
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(role != Qt::DisplayRole, QVariant());
    VALIDATE_INDEX_NON_VOID(section, columnCount(), QVariant(), 
        "Accessing out-of-range proto column " + std::to_string(section) + " of " + std::to_string(columnCount()));

    const FieldDescriptor* field_desc {m_oneof_desc->field(section)};
    CHECK_PARAM_NULLPTR_NON_VOID(field_desc, QVariant(), "Field is null.");

    return QString::fromStdString(field_desc->full_name());
}

void OneofModel::clear_sub_model() const { 
    const Reflection* refl {m_message_buffer->GetReflection()};
    refl->ClearOneof(m_message_buffer, m_oneof_desc);
    delete m_sub_model; m_sub_model = nullptr; m_current_field_desc = nullptr;
}

bool OneofModel::is_set() const {
    const Reflection* refl {m_message_buffer->GetReflection()};
    return m_current_field_desc != nullptr && m_sub_model != nullptr && refl->HasOneof(*m_message_buffer, m_oneof_desc);
}

bool OneofModel::set_oneof(const FieldDescriptor* field_desc) const {
    const Reflection* refl {m_message_buffer->GetReflection()};

    SILENT_CHECK_CONDITION_TRUE_NON_VOID(is_set() && m_current_field_desc->number() == field_desc->number(), true);

    if (is_set() && m_current_field_desc->number() != field_desc->number()) clear_sub_model();

    m_current_field_desc = field_desc; // Set the current field descriptor

    switch (field_desc->cpp_type()) {
        case FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
            ProtoModel* sub_model {new MessageModel(refl->MutableMessage(m_message_buffer, field_desc), const_cast<OneofModel*>(this), 0)};
            sub_model->build_sub_models();
            m_sub_model = sub_model;
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
            ProtoModel* sub_model {new PrimitiveModel(m_message_buffer, field_desc, const_cast<OneofModel*>(this), 0)};
            m_sub_model = sub_model;
            break;
        }
        default:
            WARN_PRINT("Unsupported field type: " + std::to_string(field_desc->cpp_type()));
            break;
    }

    return true;
}
