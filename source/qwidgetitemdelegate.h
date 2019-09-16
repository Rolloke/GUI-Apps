#ifndef QWIDGETITEMDELEGATE_H
#define QWIDGETITEMDELEGATE_H

#include <QStyledItemDelegate>

class QWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    enum eWidget { eCheckBox, eButton, eSpinBox, eStatic };
    explicit QWidgetItemDelegate(eWidget aWidget, QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const ;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    eWidget mWidgetType;
};

#endif // QWIDGETITEMDELEGATE_H
