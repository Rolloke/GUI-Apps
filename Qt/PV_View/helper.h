#ifndef HELPER_H
#define HELPER_H

#include <QLayout>
#include <QString>
#include <QStandardItemModel>

QWidget* find_widget(QLayout *layout, const QString& text);
void clearLayout(QLayout *layout);

class CheckboxItemModel : public QStandardItemModel
{
public:
    CheckboxItemModel(int rows, int columns, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setCheckedColumn(int checked);
private:
    int mChecked = 0;
};

#endif // HELPER_H
