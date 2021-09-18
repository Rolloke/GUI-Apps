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
    void     parseGitStash(const QString& fSource, const QString& aStatus, git::stringt2typemap& aFiles);
    QString  getSelectedStashItem();
    QString  getStashTopItemText(Role::e role);
    void     on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    void     on_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

Q_SIGNALS:

public Q_SLOTS:
    void clear();

protected:

private:

    bool    mInitialized { false };
    QTreeWidgetItem* mSelectedItem { nullptr };
};

#endif // QSTASHTREEWIDGET_H
