#ifndef REPEATED_MESSAGE_MODEL_H
#define REPEATED_MESSAGE_MODEL_H

#include "gui/model/message_model.hpp"

/**
    RepeatedMessageModel represents a multiple Protobuf message. The structure of the model is as follows:

            field 1        field 2       field 3     field 4      ...
        ╭─────────────┬──────────────┬────────────┬────────────┬─────────╮
        │    0        │    ...       │   ...      │    ...     │   M     │
        ├─────────────┼──────────────┼────────────┼────────────┼─────────┤
        │    ...      │     ...      │   ...      │   ...      │  ...    │
        ├─────────────┼──────────────┼────────────┼────────────┼─────────┤
        │    N        │   ...        │   ...      │  ...       │  ...    │
        ╰─────────────┴──────────────┴────────────┴────────────┴─────────╯

    @brief The repeated message represents multiple rows in the model. It can have multiple fields.
           Each field represents a column in the model.
*/
class RepeatedMessageModel : public MessageModel {
    Q_OBJECT

public:
    RepeatedMessageModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent);

    void build_sub_models();

    QVariant data() const override;
    bool set_data(const QVariant& value) override;
    
    ProtoModel* get_sub_model([[maybe_unused]] const FieldPath& path) override {
        return nullptr;
    }

    MessageModel* get_sub_model(const int& index) const;

    int field_to_column(const int& fn) const;
    
    const ProtoModel* get_sub_model(const FieldPath& path) const override;

    const FieldDescriptor* get_column_descriptor(const int& column) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole) override;
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());

    Message* get_message_buffer() const { return m_message_buffer; }

private:
    Message* m_message_buffer;
    const FieldDescriptor* m_field_desc;
    std::vector<MessageModel*> m_sub_models;

    void clear_sub_models();
};

#endif // REPEATED_MESSAGE_MODEL_H

   