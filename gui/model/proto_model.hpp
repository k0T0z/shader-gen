#ifndef PROTO_MODEL_HPP
#define PROTO_MODEL_HPP

#include <QAbstractItemModel>
#include <google/protobuf/message.h>

class ProtoModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ProtoModel(google::protobuf::Message* message, QObject* parent = nullptr);
    virtual ~ProtoModel() override = default;

    // JSON Serialization
    bool loadFromJson(const QString& filePath);
    bool saveToJson(const QString& filePath) const;

protected:
    google::protobuf::Message* m_message; // Pointer to the Protobuf message

    // Serialization helper
    QByteArray serializeToJson() const;
    bool deserializeFromJson(const QByteArray& jsonData);
};

#endif // PROTO_MODEL_HPP
