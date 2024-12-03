#include "gui/model/proto_model.hpp"
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include <QFile>
#include <QDebug>

#include "error_macros.hpp"

using namespace google::protobuf::util;
using Message = google::protobuf::Message;

ProtoModel::ProtoModel(ProtoModel* parent_model, const int& index_in_parent)
    : QAbstractItemModel(parent_model), m_parent_model(parent_model), m_index_in_parent(index_in_parent) {}

void ProtoModel::parent_data_changed() const {
    const ProtoModel* m {get_parent_model()};
    while (true) {
        SILENT_CHECK_PARAM_NULLPTR(m);
        Q_EMIT const_cast<ProtoModel*>(m)->dataChanged(m->index(0, 0), m->index(m->rowCount() - 1, m->columnCount() - 1));
        m = m->get_parent_model();
    }
}

// QByteArray ProtoModel::serializeToJson() const {
//     std::string jsonString;
//     JsonOptions options;
//     options.add_whitespace = true; // Makes JSON human-readable
//     MessageToJsonString(*m_message, &jsonString, options);
//     return QByteArray::fromStdString(jsonString);
//     return QByteArray();
// }

// bool ProtoModel::deserializeFromJson(const QByteArray& jsonData) {
//     JsonParseOptions options;
//     absl::Status status =
//         JsonStringToMessage(jsonData.toStdString(), m_message, options);

//     if (!status.ok()) {
//         qWarning() << "Failed to deserialize JSON:" << status.ToString().c_str();
//         return false;
//     }
//     return true;
// }

// bool ProtoModel::loadFromJson(const QString& filePath) {
//     QFile file(filePath);
//     if (!file.open(QIODevice::ReadOnly)) {
//         qWarning() << "Failed to open file for reading:" << filePath;
//         return false;
//     }

//     QByteArray jsonData = file.readAll();
//     file.close();
//     return deserializeFromJson(jsonData);
//     return true;
// }

// bool ProtoModel::saveToJson(const QString& filePath) const {
//     QFile file(filePath);
//     if (!file.open(QIODevice::WriteOnly)) {
//         qWarning() << "Failed to open file for writing:" << filePath;
//         return false;
//     }

//     QByteArray jsonData = serializeToJson();
//     file.write(jsonData);
//     file.close();
//     return true;
// }

// Message* ProtoModel::traverseToField(Message* message, const FieldPath& path, int depth) const {
//     const Descriptor* descriptor = message->GetDescriptor();
//     const Reflection* reflection = message->GetReflection();

//     for (int i = depth; i < path.components().size(); ++i) {
//         const auto& component = path.components()[i];
//         if (std::holds_alternative<FieldPath::FieldNumber>(component)) {
//             int fieldNumber = std::get<FieldPath::FieldNumber>(component).field;
//             const FieldDescriptor* field_desc = descriptor->FindFieldByNumber(fieldNumber);
//             if (field->is_repeated() || field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE)
//                 return nullptr; // Traversal ends here
//             message = reflection->MutableMessage(message, field);
//         } else if (std::holds_alternative<FieldPath::RepeatedAt>(component)) {
//             return nullptr; // Not implemented for repeated fields yet
//         } else if (std::holds_alternative<FieldPath::OneOfFieldNumber>(component)) {
//             return nullptr; // Not implemented for oneof fields yet
//         }
//     }
//     return message;
// }

QVariant ProtoModel::data(const FieldPath& path) const {
    const ProtoModel* model = get_sub_model(path);
    CHECK_PARAM_NULLPTR_NON_VOID(model, QVariant(), "Failed to get sub model " + path.to_string());
    return model->data();
}

bool ProtoModel::set_data(const FieldPath& path, const QVariant& value) {
    ProtoModel* model = const_cast<ProtoModel*>(get_sub_model(path, true));
    CHECK_PARAM_NULLPTR_NON_VOID(model, false, "Failed to get sub model " + path.to_string());
    return model->set_data(value);
}

const ProtoModel* ProtoModel::get_root_model() const {
    const ProtoModel* m {this};
    while (true) {
        SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m->get_parent_model(), m);
        m = m->get_parent_model();
    }
    return nullptr;
}
