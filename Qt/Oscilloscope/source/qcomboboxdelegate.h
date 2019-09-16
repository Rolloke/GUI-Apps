#ifndef QCOMBOBOXDELEGATE_H
#define QCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

class QComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit QComboBoxDelegate(const QStringList& aItems,QObject *parent = 0);

    void    setItemList(const QStringList& aItems);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QStringList mItems;
};

#endif // QCOMBOBOXDELEGATE_H
