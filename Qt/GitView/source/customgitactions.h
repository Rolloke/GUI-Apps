#ifndef CUSTOMGITACTIONS_H
#define CUSTOMGITACTIONS_H

#include "git_type.h"
#include "actions.h"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class CustomGitActions;
}

class QAbstractItemModel;
class QStandardItem;

class CustomGitActions : public QDialog
{
    Q_OBJECT

    enum class VariousListIndex
    {
        Icons,
        MenuSrcTree,
        MenuEmptySrcTree,
        MenuHistoryTree,
        MenuBranchTree,
        Toolbar1,
        Toolbar2,
        FirstCmds=MenuSrcTree,
        LastCmds=Toolbar2
    };
    const std::vector<QString> mVariousListHeader =
    {
        tr("Icons"),
        tr("Context Menu Source"),
        tr("Context Menu Empty Source"),
        tr("Context Menu History"),
        tr("Context Menu Branch"),
        tr("Toolbar 1"),
        tr("Toolbar 2")
    };
    enum class VariousHeader
    {
        Icon,
        Name,
        Size
    };

    struct Btn {enum e
    {
        Add=1, Delete=2, Up=4, Down=8, Right=16, Left=32
    }; };

public:
    explicit CustomGitActions(ActionList& aList, QWidget *parent = 0);
    ~CustomGitActions();

protected:
    void resizeEvent(QResizeEvent *event);

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
    void on_tableViewActions_customContextMenuRequested(const QPoint &pos);

private:
    void initListIcons();
    void initMenuList(const git::Cmd::tVector& aItems, const QString& aHeader);
    void insertCmdAction(ActionList::tActionMap::const_reference aItem, int & aRow);
    git::Cmd::tVector& getCmdVector(VariousListIndex aIndex);

    Ui::CustomGitActions *ui;
    ActionList& mActionList;
    QAbstractItemModel* mListModelActions;
    QAbstractItemModel* mListModelVarious;
    bool mInitialize;
};

enum class ActionsTable
{
    ID, Icon, Command, Name, Shortcut, MsgBoxText, Last
};

class ActionItemModel : public QStandardItemModel
{
public:
    ActionItemModel(int rows, int columns, QObject *parent = Q_NULLPTR);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

class VariousItemModel : public QStandardItemModel
{
public:
    VariousItemModel(int rows, int columns, QObject *parent = Q_NULLPTR);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // CUSTOMGITACTIONS_H
