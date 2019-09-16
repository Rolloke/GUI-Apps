#include "qwidgetitemdelegate.h"
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

QWidgetItemDelegate::QWidgetItemDelegate( eWidget aWidget, QObject *parent)
    : QStyledItemDelegate(parent)
    , mWidgetType(aWidget)
{

}


void QWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   QStyledItemDelegate::paint(painter, option, index);
}

QWidget *QWidgetItemDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & /* option */,
    const QModelIndex & /* index */ ) const
{
    QWidget* editor = NULL;
    switch (mWidgetType)
    {
    case eButton:   editor = new QPushButton(parent); break;
    case eCheckBox: editor = new QCheckBox(parent);   break;
    case eSpinBox:  editor = new QSpinBox(parent);   break;
    case eStatic:   editor = new QLabel(parent);   break;
    }
    return editor;
}

void QWidgetItemDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    switch (mWidgetType)
    {
    case eButton:
    {
        QPushButton *pushButton = static_cast<QPushButton*>(editor);
        pushButton->setText(index.model()->data(index, Qt::EditRole).toString());
    } break;
    case eCheckBox:
    {
        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        checkBox->setCheckState(static_cast<Qt::CheckState>(index.model()->data(index, Qt::CheckStateRole).toInt()));
    } break;
    case eSpinBox:
    {
        QSpinBox *pinBox  = static_cast<QSpinBox*>(editor);
        pinBox->setValue(index.model()->data(index, Qt::EditRole).toInt());
    } break;
    case eStatic:
    {
        QLabel* fLabel  = static_cast<QLabel*>(editor);
        fLabel->setText(index.model()->data(index, Qt::DisplayRole).toString());
    } break;
    }

}

void QWidgetItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    switch (mWidgetType)
    {
    case eButton:
    {
        QPushButton *pushButton = static_cast<QPushButton*>(editor);
        model->setData(index, QVariant(pushButton->text()), Qt::EditRole);
    } break;
    case eCheckBox:
    {
        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        model->setData(index, QVariant(checkBox->checkState()), Qt::CheckStateRole);
    } break;
    case eSpinBox:
    {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        model->setData(index, QVariant(spinBox->value()), Qt::EditRole);
    } break;
    case eStatic:
    {
        QLabel *fLabel = static_cast<QLabel*>(editor);
        model->setData(index, QVariant(fLabel->text()), Qt::EditRole);
    } break;
    }

}

void QWidgetItemDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
