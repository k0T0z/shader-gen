#ifndef MESSAGE_MODEL_HPP
#define MESSAGE_MODEL_HPP

#include <vector>
#include <memory>
#include <google/protobuf/message.h>
#include "gui/model/proto_model.hpp"

using Message = google::protobuf::Message;
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

    @todo Make lazy allocation of sub-models' messages
*/
class MessageModel : public ProtoModel {
    Q_OBJECT

public:
    MessageModel(Message* message_buffer, ProtoModel* parent_model = nullptr, const int& index_in_parent = -1);
    virtual ~MessageModel() override {
        clear_sub_models();
    }

    virtual void build_sub_models() override;

    QVariant data() const override;
    bool set_data(const QVariant& value) override;
    
    const ProtoModel* get_sub_model(const int& field_number) const;
    const ProtoModel* get_sub_model(const FieldPath& path) const override;

    const FieldDescriptor* get_column_descriptor(const int& column) const override;

    /**
     * @brief 
     * 
     * @note https://doc.qt.io/qt-5/model-view-programming.html#model-indexes
     * 
     * @param row 
     * @param column 
     * @param parent 
     * @return QModelIndex 
     */
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual int rowCount([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override { return 1; }
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual QVariant headerData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role = Qt::DisplayRole) const override;

    Message* get_message_buffer() const { return m_message_buffer; }

private:
    Message* m_message_buffer;
    std::unordered_map<int, ProtoModel*> m_sub_models_by_field_number;
    std::unordered_map<std::string, ProtoModel*> m_sub_models_by_oneof_name;

    void clear_sub_models();
};

#endif // MESSAGE_MODEL_HPP
