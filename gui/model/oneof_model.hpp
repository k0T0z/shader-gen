#ifndef ONEOF_MODEL_HPP
#define ONEOF_MODEL_HPP

#include <vector>
#include <memory>
#include <google/protobuf/message.h>
#include "gui/model/proto_model.hpp"

using Message = google::protobuf::Message;
using Descriptor = google::protobuf::Descriptor;
using Reflection = google::protobuf::Reflection;
using FieldDescriptor = google::protobuf::FieldDescriptor;
using OneofDescriptor = google::protobuf::OneofDescriptor;

class OneofModel : public ProtoModel {
    Q_OBJECT

public:
    OneofModel(Message* message_buffer, const OneofDescriptor* oneof_desc, ProtoModel* parent_model, const int& index_in_parent);
    ~OneofModel() override {
        clear_sub_model();
    }

    void build_sub_models() override;

    QVariant data() const override;
    bool set_data(const QVariant& value) override;
    
    const ProtoModel* get_sub_model(const int& field_number) const;
    const ProtoModel* get_sub_model(const FieldPath& path, const bool& for_set_data = false) const override;

    const FieldDescriptor* get_column_descriptor(const int& column) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override { return 1; }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData([[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation, [[maybe_unused]] int role = Qt::DisplayRole) const override;

private:
    Message* m_message_buffer;
    const OneofDescriptor* m_oneof_desc;

    mutable const FieldDescriptor* m_current_field_desc;
    mutable ProtoModel* m_sub_model;

    void clear_sub_models() override {}
    void clear_sub_model() const;
    bool is_set() const;
    bool set_oneof(const FieldDescriptor* field_desc) const;
};

#endif // ONEOF_MODEL_HPP
