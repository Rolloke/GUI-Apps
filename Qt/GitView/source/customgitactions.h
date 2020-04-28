#ifndef CUSTOMGITACTIONS_H
#define CUSTOMGITACTIONS_H

#include "git_type.h"

#include <QDialog>

namespace Ui {
class CustomGitActions;
}

class ActionList;
class QAbstractItemModel;
class QStandardItem;

class CustomGitActions : public QDialog
{
    Q_OBJECT

    enum class ActionsTable
    {
        ID, Command, Name, Shortcut, MsgBoxText, PostAction, Icon, Last
    };

    enum class VariousIndex
    {
        Icons, MenuSrcTree, MenuEmptySrcTree, MenuHistoryTree, Toolbar1, Toolbar2
    };


public:
    explicit CustomGitActions(ActionList& aList, QWidget *parent = 0);
    ~CustomGitActions();

private slots:
    void on_comboBoxVarious_currentIndexChanged(int index);
    void on_ActionTableListItemChanged ( QStandardItem * item );
    void on_btnToLeft_clicked();
    void on_btnToRight_clicked();
    void on_btnMoveUp_clicked();
    void on_btnMoveDown_clicked();
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();

private:
    void initListIcons();
    void initMenuList(const std::vector<git::Cmd::eCmd>& aItems, const QString& aHeader);

    Ui::CustomGitActions *ui;
    ActionList& mActionList;
    QAbstractItemModel* mListModelActions;
    QAbstractItemModel* mListModelVarious;
    bool mInitialize;
};

#endif // CUSTOMGITACTIONS_H
