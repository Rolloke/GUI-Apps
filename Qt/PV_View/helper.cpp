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
