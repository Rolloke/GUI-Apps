#ifndef BRANCH_H
#define BRANCH_H

#include <QTreeWidget>

class ActionList;

class QBranchTreeWidget : public QTreeWidget
{
    Q_OBJECT

    struct Column { enum e { Text }; };
    struct Role   { enum e { GitRootPath = 1 }; };
public:
    QBranchTreeWidget(QWidget *parent = 0);
    ~QBranchTreeWidget();

    QString getBranchTopItemText();
    QString getSelectedBranchGitRootPath();
    QString getSelectedBranch();
    void parseBranchListText(const QString& aBranchText, const QString& aGitRootPath);
    void on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    void on_itemDoubleClicked(const ActionList& aActionList, QTreeWidgetItem *item, int );
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
