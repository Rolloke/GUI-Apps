#include "helper.h"

#include<QPushButton>

QWidget* find_widget(QLayout *layout, const QString& text)
{
    if (layout)
    {
        int items = layout->count();
        for (int i=0; i< items; ++i)
        {
            QLayoutItem *item = layout->itemAt(i);
            if (item)
            {
                if (QPushButton *widget = dynamic_cast<QPushButton*>(item->widget()))
                {
                    if (widget->text().contains(text))
                    {
                        return widget;
                    }
                }
                if (QLayout *childLayout = item->layout())
                {
                    find_widget(childLayout, text); // recursive for nested layouts
                }
            }

        }
    }
    return nullptr;
}

void clearLayout(QLayout *layout)
{
    if (layout)
    {
        while (QLayoutItem *item = layout->takeAt(0))
        {
            if (QWidget *widget = item->widget())
            {
                widget->deleteLater();   // remove and delete widget
            }
            if (QLayout *childLayout = item->layout())
            {
                clearLayout(childLayout); // recursive for nested layouts
            }
            delete item; // delete the layout item itself
        }
    }
}

CheckboxItemModel::CheckboxItemModel(int rows, int columns, QObject *parent):
    QStandardItemModel(rows, columns, parent),
    mChecked(0)
{
}

QVariant CheckboxItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole && index.column() == mChecked)
    {
        return QStandardItemModel::data(index, role).toBool() ? Qt::Checked : Qt::Unchecked;
    }
    return QStandardItemModel::data(index, role);
}

void CheckboxItemModel::setCheckedColumn(int checked)
{
    mChecked = checked;
}

