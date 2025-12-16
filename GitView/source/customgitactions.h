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

    struct VariousListIndex
    {
        enum e
        {
            Icons,
            ExternalIcons,
            MergeTool,
            Miscelaneous,
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
            Size,
            FirstCmds=MenuSrcTree,
            FirstMenu=MenuSrcTree,
            LastMenu=MenuFindTextTree,
            LastCmds=Toolbar2
        };
        static bool isIcon(int);
        static bool isMenu(int);
        static bool isToolbar(int);
        static bool isCustomToolbar(int);
        static e    cast(int);
    };
    std::vector<double> mActionListColumnWidth;

    struct VariousHeader { enum e
    {
        Icon,
        Name,
        Size
    };};

    struct Btn {enum e
    {
        Add=1, Delete=2, Up=4, Down=8, Right=16, Left=32, Load=64, KeyShortcut=128
    }; };

    friend class ActionItemModel;
    friend class VariousItemModel;

public:
    explicit CustomGitActions(ActionList& aList, string2bool_map&aMergeTools, string2miscelaneous_map &aMiscItems, QWidget *parent = 0);
    ~CustomGitActions();
    bool isMergeToolsChanged();
    bool isMiscelaneousItemChanged();
    bool mExperimental;
    QString mExternalIconFiles;
    void display_command_text(const QString& cmd);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

Q_SIGNALS:
    void initCustomAction(QAction* fAction);
    void removeCustomToolBar(const QString&);
    void store_commands_to(const QString&);
    void read_commands_from(const QString&);

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
    void on_tableViewVarious_customContextMenuRequested(const QPoint &pos);
    void on_btnLoadIcons_clicked();
    void on_btnHelp_clicked();
    void on_btnAddCommand_clicked();
    void on_btnFind_clicked();
    void tableViewActions_header_clicked(int index);
    void on_btnStoreCustom_clicked();
    void on_btnLoadCustom_clicked();

private:
    void initListIcons(VariousListIndex::e aIndex, QString fPath = ":/resource/24X24/");
    void initListMergeTool();
    void initListMiscelaneous();
    void initMenuList(const git::Cmd::tVector& aItems, const QString& aHeader);
    void insertCmdAction(ActionList::tActionMap::const_reference aItem, int & aRow);
    const QString& iconCheck(bool check);
    QString iconValueType(const QVariant &variant, bool use_text);
    git::Cmd::tVector& getCmdVector(VariousListIndex::e aIndex);
    QString getVariousListHeader(VariousListIndex::e aIndex);
    QAction *set_tooltip(QAction*, const QString&);
    git::Cmd::eCmd getCommand(int aRow);
    std::uint32_t get_toolbar_index(std::uint32_t index) const;
    std::uint32_t getVariousListSize() const;
    void add_command(const QString& command = "git ");

    Ui::CustomGitActions *ui;
    ActionList& mActionList;
    string2bool_map& mMergeTools;
    string2miscelaneous_map& mMiscelaneousItems;
    QAbstractItemModel* mListModelActions;
    QAbstractItemModel* mListModelVarious;
    bool mInitialize;
    bool mIsMiscelaneousItemChanged;
    QBitArray mMergeToolsState;
    int       mSearchColumn;
    int       mSearchRowStart;
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
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
};

class VariousItemModel : public QStandardItemModel
{
public:
    VariousItemModel(int rows, int columns, QObject *parent = Q_NULLPTR);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // CUSTOMGITACTIONS_H
