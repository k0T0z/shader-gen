#ifndef PROTO_MODEL_HPP
#define PROTO_MODEL_HPP

#include <QAbstractItemModel>
#include <google/protobuf/message.h>
#include "gui/model/utils/field_path.hpp"

class ProtoModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ProtoModel(google::protobuf::Message* message, QObject* parent = nullptr);
    virtual ~ProtoModel() override = default;

    // JSON Serialization
    bool loadFromJson(const QString& filePath);
    bool saveToJson(const QString& filePath) const;

    // Set and Get data using FieldPath
    bool set_data(const FieldPath& path, const QVariant& value);
    QVariant data(const FieldPath& path) const;

protected:
    google::protobuf::Message* m_message; // Pointer to the Protobuf message

    // Serialization helper
    QByteArray serializeToJson() const;
    bool deserializeFromJson(const QByteArray& jsonData);

private:
    // Helper methods for traversing fields
    google::protobuf::Message* traverseToField(google::protobuf::Message* message, const FieldPath& path, int depth = 0) const;
    const google::protobuf::Message* traverseToField(const google::protobuf::Message* message, const FieldPath& path, int depth = 0) const;
};

#endif // PROTO_MODEL_HPP
