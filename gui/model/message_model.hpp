#ifndef MESSAGE_MODEL_HPP
#define MESSAGE_MODEL_HPP

#include <vector>
#include <memory>
#include <google/protobuf/message.h>
#include "gui/model/proto_model.hpp"

using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;

/**
    MessageModel represents a single Protobuf message. The structure of the model is as follows:

            field 1        field 2       field 3     field 4      ...
        ╭─────────────┬──────────────┬────────────┬────────────┬─────────╮
        │    ...      │    ...       │   ...      │    ...     │  ...    │
        ╰─────────────┴──────────────┴────────────┴────────────┴─────────╯

    @brief The message represents a single row in the model. It can have multiple fields.
           Each field represents a column in the model.

    @note Oneof fields are represented as a single column.
*/
class MessageModel : public ProtoModel {
    Q_OBJECT

public:
    MessageModel(ProtoModel* parent, google::protobuf::Message* message_buffer, int row_in_parent = -1);

    void rebuild_sub_models();

    ProtoModel* get_sub_model(int row) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        return QModelIndex();
    }

    QModelIndex parent(const QModelIndex &child) const override {
        return QModelIndex();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1;
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        const Descriptor* desc {m_message_buffer->GetDescriptor()};
        return (desc->field_count() - desc->real_oneof_decl_count()) + 1;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        return QVariant();
    }

protected:
    google::protobuf::Message* m_message_buffer; // Pointer to the associated Protobuf message
    int m_row_in_parent;                   // Row of this model in the parent

private:
    std::vector<std::unique_ptr<ProtoModel>> m_sub_models; // Stores child models
};

#endif // MESSAGE_MODEL_HPP