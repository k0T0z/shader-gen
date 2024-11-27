#include "gui/model/proto_model.hpp"
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include <QFile>
#include <QDebug>

#include "error_macros.hpp"

using namespace google::protobuf::util;
using Message = google::protobuf::Message;
using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

ProtoModel::ProtoModel(ProtoModel* parent_model, const int& index_in_parent)
    : QAbstractItemModel(parent_model), m_parent_model(parent_model), m_index_in_parent(index_in_parent) {}

void ProtoModel::parent_data_changed() const {
    while (true) {
        ProtoModel* m {get_parent_model()};
        SILENT_CHECK_PARAM_NULLPTR(m);
        emit m->dataChanged(m->index(0, 0), m->index(rowCount() - 1, columnCount() - 1));
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
//             const FieldDescriptor* field = descriptor->FindFieldByNumber(fieldNumber);
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

bool ProtoModel::set_data(const FieldPath& path, const QVariant& value) {
    ProtoModel* model = get_sub_model(path);
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(model, false);
    return model->set_data(value);
}

QVariant ProtoModel::data(const FieldPath& path) const {
    const ProtoModel* model = get_sub_model(path);
    SILENT_CHECK_PARAM_NULLPTR_NON_VOID(model, QVariant());
    return model->data();
}
