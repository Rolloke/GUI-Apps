#ifndef QBINARYTABLEVIEW_H
#define QBINARYTABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>


class BinaryTableModel;

class qbinarytableview : public QTableView
{
    Q_OBJECT
public:
    explicit qbinarytableview(QWidget *parent = nullptr);

    void set_rows(int);
    void set_content(const QByteArray& content);

signals:

};

class BinaryTableModel : public QStandardItemModel
{
public:
    enum class Table
    {
        Index, Typed, Character, Last
    };
    BinaryTableModel(int rows, int columns, qbinarytableview *parent = nullptr);

    ~BinaryTableModel();
    void set_content(const QByteArray& content);

private:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant get_typed_content(int row) const;
    QVariant get_character_content(int row) const;
    QByteArray  m_binary_content;
};

#endif // QBINARYTABLEVIEW_H
