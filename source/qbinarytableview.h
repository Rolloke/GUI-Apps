#ifndef QBINARYTABLEVIEW_H
#define QBINARYTABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include "DisplayType.h"

class BinaryTableModel;

class qbinarytableview : public QTableView
{
    Q_OBJECT
public:
    explicit qbinarytableview(QWidget *parent = nullptr);

    void update_rows();
    void set_binary_data(const QByteArray& content);
    const QByteArray& get_binary_data() const;
    void clear_binary_content();
    void set_parts_per_line(int ppl);
    int  get_parts_per_line() const;

signals:

private:
    BinaryTableModel* get_model() const;

};

class BinaryTableModel : public QStandardItemModel
{
    friend class qbinarytableview;
public:
    enum class Table
    {
        Index, Typed, Character, Last
    };
    BinaryTableModel(int rows, int columns, qbinarytableview *parent = nullptr);

    ~BinaryTableModel();

private:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant get_typed_content(int row) const;
    QVariant get_character_content(int row) const;
    int      get_rows() const;
    int      get_bytes_per_type() const;
    int      get_bytes_per_row() const;
    CDisplayType* get_type() const;
    QByteArray  m_binary_content;
    int         m_parts_per_line;
    CDisplayType::eType m_display_type;
    CDisplayType::type_map& m_display;
};

#endif // QBINARYTABLEVIEW_H
