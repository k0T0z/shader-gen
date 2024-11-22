#include "proto_model.hpp"
#include <google/protobuf/util/json_util.h>
#include <QFile>
#include <QDebug>

using namespace google::protobuf::util;

// Constructor
ProtoModel::ProtoModel(google::protobuf::Message* message, QObject* parent)
    : QAbstractItemModel(parent), m_message(message) {}

// Serialize the Protobuf message to JSON
QByteArray ProtoModel::serializeToJson() const {
    std::string jsonString;
    JsonOptions options;
    options.add_whitespace = true; // Makes JSON human-readable
    MessageToJsonString(*m_message, &jsonString, options);
    return QByteArray::fromStdString(jsonString);
    return QByteArray();
}

// Deserialize the JSON data into the Protobuf message
bool ProtoModel::deserializeFromJson(const QByteArray& jsonData) {
    JsonParseOptions options;
    absl::Status status =
        JsonStringToMessage(jsonData.toStdString(), m_message, options);

    if (!status.ok()) {
        qWarning() << "Failed to deserialize JSON:" << status.ToString().c_str();
        return false;
    }
    return true;
}

// Load Protobuf data from a JSON file
bool ProtoModel::loadFromJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();
    return deserializeFromJson(jsonData);
    return true;
}

// Save Protobuf data to a JSON file
bool ProtoModel::saveToJson(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QByteArray jsonData = serializeToJson();
    file.write(jsonData);
    file.close();
    return true;
}
