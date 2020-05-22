#ifndef BRANCH_H
#define BRANCH_H

#include <QTreeWidget>

class ActionList;

class QBranchTreeWidget : public QTreeWidget
{
    Q_OBJECT

    enum class Column
    {
        Text
    };
public:
    QBranchTreeWidget(QWidget *parent = 0);
    ~QBranchTreeWidget();

    QString getBranchTopItemText();
    QString getBranchItem();
    void parseBranchListText(const QString& aBranchText);
    void on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    void deleteSelectedItem();

    QModelIndex indexFromSelectedItem() const;
    QTreeWidgetItem * itemFromIndex(const QModelIndex &index) const;

Q_SIGNALS:

public Q_SLOTS:

   void clear();

private:
    QTreeWidgetItem* mSelectedItem;
};

#endif // BRANCH_H
