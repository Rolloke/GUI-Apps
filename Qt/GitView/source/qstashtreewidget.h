#ifndef QSTASHTREEWIDGET_H
#define QSTASHTREEWIDGET_H

#include <QTreeWidget>
#include "git_type.h"

class ActionList;

class QStashTreeWidget : public QTreeWidget
{
    Q_OBJECT

    struct Column { enum e
        {
            Text,
            Description
        }; };

public:
    struct Role   { enum e
        {
            Text = 0,
            Commit = 1,
            GitRootPath = 2
        }; };

    QStashTreeWidget(QWidget *parent = 0);

    bool     parseStashListText(const QString& aBranchText, const QString& aGitRootPath);
    QString  getSelectedStashItem();
    QString  getStashTopItemText(Role::e role);
    void     on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    void     on_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void     on_itemDoubleClicked(QTreeWidgetItem *item, int column);

Q_SIGNALS:
    void find_item_in_treeSource(const QString&, const QString&);

public Q_SLOTS:
    void clear();

protected:

private:

    bool    mInitialized { false };
    QTreeWidgetItem* mSelectedItem { nullptr };
};

#endif // QSTASHTREEWIDGET_H
