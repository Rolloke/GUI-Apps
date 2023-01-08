#ifndef CUSTOMGITACTIONS_H
#define CUSTOMGITACTIONS_H

#include "actions.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QBitArray>

namespace Ui {
class CustomGitActions;
}

class QAbstractItemModel;
class QStandardItem;

class CustomGitActions : public QDialog
{
    Q_OBJECT

    struct VariousListIndex { enum e
    {
        Icons,
        ExternalIcons,
        MenuSrcTree,
        MenuEmptySrcTree,
        MenuGraphicView,
        MenuCodeBrowser,
        MenuHistoryTree,
        MenuBranchTree,
        MenuStashTree,
        MenuFindTextTree,
        Toolbar1,
        Toolbar2,
        MergeTool,
        Size,
        FirstCmds=MenuSrcTree,
        LastCmds=Toolbar2
    };};
    std::vector<double> mActionListColumnWidth;

    struct VariousHeader { enum e
    {
        Icon,
        Name,
        Size
    };};

    struct Btn {enum e
    {
        Add=1, Delete=2, Up=4, Down=8, Right=16, Left=32, Load=64
    }; };

public:
    explicit CustomGitActions(ActionList& aList, string2bool_map&aMergeTools, QWidget *parent = 0);
    ~CustomGitActions();
    bool isMergeToolsChanged();
    bool mExperimental;
    QString mExternalIconFiles;

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
    void on_btnLoadIcons_clicked();

    void on_btnHelp_clicked();

private:
    void initListIcons(VariousListIndex::e aIndex, QString fPath = ":/resource/24X24/");
    void initListMergeTool();
    void initMenuList(const git::Cmd::tVector& aItems, const QString& aHeader);
    void insertCmdAction(ActionList::tActionMap::const_reference aItem, int & aRow);
    const QString& iconCheck(bool check);
    git::Cmd::tVector& getCmdVector(VariousListIndex::e aIndex);
    QString getVariousListHeader(VariousListIndex::e aIndex);
    void set_tooltip(QAction*, const QString&);
    git::Cmd::eCmd getCommand(int aRow);

    Ui::CustomGitActions *ui;
    ActionList& mActionList;
    string2bool_map& mMergeTools;
    QAbstractItemModel* mListModelActions;
    QAbstractItemModel* mListModelVarious;
    bool mInitialize;
    QBitArray mMergeToolsState;
};

struct ActionsTable { enum e
{
    ID, Icon, Command, Name, Shortcut, MsgBoxText, Last
}; };

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
