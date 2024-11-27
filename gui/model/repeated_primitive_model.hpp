#ifndef REPEATED_PRIMITIVE_MODEL_HPP
#define REPEATED_PRIMITIVE_MODEL_HPP

#include "gui/model/primitive_model.hpp"

/**
    RepeatedPrimitiveModel represents a multiple Protobuf message. The structure of the model is as follows:

            field 1   
        ╭────────────╮
        │    0       │
        ├────────────┤
        │    ...     │
        ├────────────┤
        │    N       │
        ╰────────────╯

    @brief The repeated message represents multiple rows in the model.
*/
class RepeatedPrimitiveModel : public PrimitiveModel {
    Q_OBJECT

public:
    RepeatedPrimitiveModel(Message* message_buffer, const FieldDescriptor* field_desc, ProtoModel* parent_model, const int& index_in_parent);
    ~RepeatedPrimitiveModel() override {
        clear_sub_models();
    }

    void build_sub_models() override;

    QVariant data() const override;
    bool set_data(const QVariant& value) override;
    
    ProtoModel* get_sub_model([[maybe_unused]] const FieldPath& path) override { return nullptr; }
    const ProtoModel* get_sub_model(const FieldPath& path) const override;
    PrimitiveModel* get_sub_model(const int& index) const;

    const FieldDescriptor* get_column_descriptor(const int& column) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    int append_row();
    bool remove_row(const int& row);

    int field_to_column(const int& fn) const;

    Message* get_message_buffer() const { return m_message_buffer; }

private:
    Message* m_message_buffer;
    const FieldDescriptor* m_field_desc;
    std::vector<PrimitiveModel*> m_sub_models;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void clear_sub_models();
};

#endif // REPEATED_PRIMITIVE_MODEL_HPP

   