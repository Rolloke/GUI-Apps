#ifndef CUSTOMGITACTIONS_H
#define CUSTOMGITACTIONS_H

#include "git_type.h"
#include "actions.h"

#include <QDialog>

namespace Ui {
class CustomGitActions;
}

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
        Icons, MenuSrcTree, MenuEmptySrcTree, MenuHistoryTree, Toolbar1, Toolbar2, FirstCmds=MenuSrcTree, LastCmds=Toolbar2
    };

    struct Btn {enum e
    {
        Add=1, Delete=2, Up=4, Down=8, Right=16, Left=32
    }; };

public:
    explicit CustomGitActions(ActionList& aList, QWidget *parent = 0);
    ~CustomGitActions();

Q_SIGNALS:
    void initCustomAction(QAction* fAction);

private Q_SLOTS:
    void on_comboBoxVarious_currentIndexChanged(int index);
    void on_ActionTableListItemChanged ( QStandardItem * item );
    void on_btnToLeft_clicked();
    void on_btnToRight_clicked();
    void on_btnMoveUp_clicked();
    void on_btnMoveDown_clicked();
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_tableViewActions_clicked(const QModelIndex &index);
    void on_tableViewVarious_clicked(const QModelIndex &index);
    void enableButtons(std::uint32_t aBtnFlag);

private:
    void initListIcons();
    void initMenuList(const git::Cmd::tVector& aItems, const QString& aHeader);
    void insertCmdAction(ActionList::tActionMap::const_reference aItem, int & aRow);
    git::Cmd::tVector& getCmdVector(VariousIndex aIndex);

    Ui::CustomGitActions *ui;
    ActionList& mActionList;
    QAbstractItemModel* mListModelActions;
    QAbstractItemModel* mListModelVarious;
    bool mInitialize;
};

#endif // CUSTOMGITACTIONS_H
