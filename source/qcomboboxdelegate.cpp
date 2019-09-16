#include "qcomboboxdelegate.h"
#include <QComboBox>

QComboBoxDelegate::QComboBoxDelegate(const QStringList& aItems, QObject *parent)
    : QStyledItemDelegate(parent)
    ,mItems(aItems)
{
}

void QComboBoxDelegate::setItemList(const QStringList& aItems)
{
    mItems = aItems;
}

QWidget *QComboBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setFrame(false);
    int i=0;
    for (QString fItem : mItems)
    {
        editor->insertItem(i++, fItem);
    }


    return editor;
}

void QComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox *comboBox = static_cast<QComboBox*>(editor);

    comboBox->setCurrentIndex(comboBox->findText(value));
}

void QComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString value = comboBox->currentText();

    model->setData(index, value, Qt::EditRole);
}

void QComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
