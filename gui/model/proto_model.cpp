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

QByteArray ProtoModel::serializeToJson() const {
    const ProtoModel* root_model = get_root_model();
    CHECK_PARAM_NULLPTR_NON_VOID(root_model, QByteArray(), "Failed to get root model");
    Message* message_buffer = root_model->get_message_buffer();
    CHECK_PARAM_NULLPTR_NON_VOID(message_buffer, QByteArray(), "Failed to get message buffer");

    std::string jsonString;
    JsonOptions options;
    options.add_whitespace = true; // Makes JSON human-readable
    MessageToJsonString(*message_buffer, &jsonString, options);
    return QByteArray::fromStdString(jsonString);
}

bool ProtoModel::deserializeFromJson(const QByteArray& jsonData) {
    const ProtoModel* root_model = get_root_model();
    CHECK_PARAM_NULLPTR_NON_VOID(root_model, false, "Failed to get root model");
    Message* message_buffer = root_model->get_message_buffer();
    CHECK_PARAM_NULLPTR_NON_VOID(message_buffer, false, "Failed to get message buffer");

    JsonParseOptions options;
    absl::Status status =
        JsonStringToMessage(jsonData.toStdString(), message_buffer, options);

    CHECK_CONDITION_TRUE_NON_VOID(!status.ok(), false, "Failed to deserialize JSON:" + status.ToString());

    return true;
}

bool ProtoModel::loadFromJson(const QString& filePath) {
    QFile file(filePath);
    CHECK_CONDITION_TRUE_NON_VOID(!file.open(QIODevice::ReadOnly), false, "Failed to open file for reading: " + filePath.toStdString());

    QByteArray jsonData = file.readAll();
    file.close();
    return deserializeFromJson(jsonData);
    return true;
}

bool ProtoModel::saveToJson(const QString& filePath) const {
    QFile file(filePath);
    CHECK_CONDITION_TRUE_NON_VOID(!file.open(QIODevice::WriteOnly), false, "Failed to open file for writing: " + filePath.toStdString());

    QByteArray jsonData = serializeToJson();
    file.write(jsonData);
    file.close();
    return true;
}

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

QModelIndex ProtoModel::index(int row, int column, [[maybe_unused]] const QModelIndex& parent) const {
    CHECK_CONDITION_TRUE_NON_VOID(!hasIndex(row, column, parent), QModelIndex(), "Invalid index.");
    SILENT_CHECK_CONDITION_TRUE_NON_VOID(!parent.isValid(), createIndex(row, column));
    return createIndex(row, column, parent.internalPointer());
}

const ProtoModel* ProtoModel::get_root_model() const {
    const ProtoModel* m {this};
    while (true) {
        SILENT_CHECK_PARAM_NULLPTR_NON_VOID(m->get_parent_model(), m);
        m = m->get_parent_model();
    }
    return nullptr;
}
