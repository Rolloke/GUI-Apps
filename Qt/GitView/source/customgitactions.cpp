#include "customgitactions.h"
#include "ui_customgitactions.h"
#include "actions.h"
#include "logger.h"
#include "helper.h"

#include <assert.h>
#include <QStandardItemModel>
#include <QDir>
#include <QMenu>
#include <QActionGroup>
#include <QComboBox>
#include <QScrollBar>

using namespace git;

CustomGitActions::CustomGitActions(ActionList& aList, string2bool_map&aMergeTools, QWidget *parent) :
    QDialog(parent)
,   ui(new Ui::CustomGitActions)
,   mActionList(aList)
,   mMergeTools(aMergeTools)
,   mListModelActions(nullptr)
,   mListModelVarious(nullptr)
,   mInitialize(false)
,   mMergeToolsState(aMergeTools.size())
{
    ui->setupUi(this);
    for (int i=0; i<mMergeTools.size(); ++i)
    {
        mMergeToolsState.setBit(i, (mMergeTools.begin() + i).value());
    }

    QStringList fColumnName  = { tr("ID"), tr("Icon"), tr("Command or status text"), tr("Name"), tr("Shortcut"), tr("Message box text")};
    mActionListColumnWidth   = {  0.055  ,    0.055  ,      0.25    ,    0.25   ,      0.1      ,        0.25           };

    assert(fColumnName.size()            == ActionsTable::Last);
    assert(mActionListColumnWidth.size() == ActionsTable::Last);

    double fItemWidth = 0;
    std::for_each(mActionListColumnWidth.begin(), mActionListColumnWidth.end()-1, [&fItemWidth](double fItem ) { fItemWidth += fItem; });
    mActionListColumnWidth.back() = 1.0 - fItemWidth;

    mListModelActions = new ActionItemModel(0, ActionsTable::Last, this);
    connect(mListModelActions, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_ActionTableListItemChanged(QStandardItem*)));

    ui->tableViewActions->setModel(mListModelActions);
    ui->tableViewActions->setContextMenuPolicy(Qt::CustomContextMenu);

    for (int fColumn = 0; fColumn<ActionsTable::Last; ++fColumn)
    {
        mListModelActions->setHeaderData(fColumn, Qt::Horizontal, fColumnName[fColumn], Qt::DisplayRole);
    }

    mInitialize = true;
    int fRow = 0;
    for (const auto& fItem : mActionList.getList())
    {
        insertCmdAction(fItem, fRow);
    }
    mListModelActions->insertRows(fRow, 1, QModelIndex());
    mListModelActions->setData(mListModelActions->index(fRow, ActionsTable::ID)  , git::Cmd::Separator, Qt::DisplayRole);
    mListModelActions->setData(mListModelActions->index(fRow, ActionsTable::Name), tr("-- Separator --"), Qt::DisplayRole);

    mInitialize = false;
    ui->tableViewActions->selectionModel()->setCurrentIndex(mListModelActions->index(0, ActionsTable::ID), QItemSelectionModel::SelectCurrent);

    int fWidth = ui->tableViewVarious->rect().width()-6;
    mListModelVarious = new VariousItemModel(0, VariousHeader::Size, this);
    ui->tableViewVarious->setModel(mListModelVarious);
    ui->tableViewVarious->setColumnWidth(VariousHeader::Icon, INT(0.2  * fWidth));
    ui->tableViewVarious->setColumnWidth(VariousHeader::Name, INT(0.75 * fWidth));

    const QString button_enabled_style = "QPushButton:enabled { background-color:skyblue;}";
    ui->btnToLeft->setStyleSheet(button_enabled_style);
    ui->btnToRight->setStyleSheet(button_enabled_style);
    ui->btnMoveUp->setStyleSheet(button_enabled_style);
    ui->btnMoveDown->setStyleSheet(button_enabled_style);
    ui->btnAdd->setStyleSheet(button_enabled_style);
    ui->btnDelete->setStyleSheet(button_enabled_style);

    mToolTips.insert(ui->tableViewActions, ui->tableViewActions->toolTip());
    ui->tableViewActions->setToolTip("");

    mToolTips.insert(ui->tableViewVarious, ui->tableViewVarious->toolTip());
    ui->tableViewVarious->setToolTip("");


    enableButtons(0);
    for (int i=0; i< VariousListIndex::Size; ++i)
    {
        ui->comboBoxVarious->addItem(getVariousListHeader(static_cast<VariousListIndex::e>(i)));
    }
    on_comboBoxVarious_currentIndexChanged(VariousListIndex::Icons);
}

bool CustomGitActions::isMergeToolsChanged()
{
    for (int i=0; i<mMergeTools.size(); ++i)
    {
        if (mMergeToolsState.at(i) != (mMergeTools.begin() + i).value())
        {
            return true;
        }
    }
    return false;
}


CustomGitActions::~CustomGitActions()
{
    delete ui;
}


void CustomGitActions::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    const int vertical_width = ui->tableViewActions->verticalScrollBar()->sizeHint().width();
    int fWidth = ui->tableViewActions->rect().width() - vertical_width;

    for (int fColumn = 0; fColumn<ActionsTable::Last; ++fColumn)
    {
        ui->tableViewActions->setColumnWidth(fColumn, INT(mActionListColumnWidth[fColumn]*fWidth));
    }
}

void CustomGitActions::insertCmdAction(ActionList::tActionMap::const_reference aItem, int & aRow)
{
    const QAction* fAction = aItem.second;
    QString fCommand = fAction->statusTip();
    if (fCommand.size())
    {
        uint fFlag = mActionList.getFlags(static_cast<Cmd::eCmd>(aItem.first), ActionList::Data::Flags);
        bool function_cmd = (fFlag & ActionList::Flags::FunctionCmd);
        if (aRow == -1) aRow = mListModelActions->rowCount();
        mListModelActions->insertRows(aRow, 1, QModelIndex());
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::ID)        , aItem.first, Qt::DisplayRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Command)   , fCommand, Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Name)      , fAction->toolTip(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Shortcut)  , fAction->shortcut().toString(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::MsgBoxText), function_cmd ? "" : mActionList.getCustomCommandMessageBoxText(static_cast<Cmd::eCmd>(aItem.first)), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Icon)      , QIcon(mActionList.getIconPath(static_cast<Cmd::eCmd>(aItem.first))), Qt::DecorationRole);
        ++aRow;
    }
}

void CustomGitActions::on_comboBoxVarious_currentIndexChanged(int aIndex)
{
    auto fIndex = static_cast<VariousListIndex::e>(aIndex);
    switch (fIndex)
    {
        case VariousListIndex::Icons:
            initListIcons();
            ui->btnToLeft->setToolTip(tr("Apply selected icon in right view to selected command entry in left view"));
            break;
        case VariousListIndex::MergeTool:
            initListMergeTool();
            break;
        default:
            if (isBetween(fIndex, VariousListIndex::FirstCmds, VariousListIndex::LastCmds))
            {
                ui->btnToLeft->setToolTip(tr("Remove selected item from %1").arg(getVariousListHeader(fIndex)));
                initMenuList(getCmdVector(fIndex), getVariousListHeader(fIndex));
            }
            break;
    }
}

Cmd::tVector& CustomGitActions::getCmdVector(VariousListIndex::e aIndex)
{
    static Cmd::tVector fDummy;
    switch (aIndex)
    {
        case VariousListIndex::MenuSrcTree:         return Cmd::mContextMenuSourceTree;
        case VariousListIndex::MenuEmptySrcTree:    return Cmd::mContextMenuEmptySourceTree;
        case VariousListIndex::MenuGraphicView:     return Cmd::mContextMenuGraphicsView;
        case VariousListIndex::MenuHistoryTree:     return Cmd::mContextMenuHistoryTree;
        case VariousListIndex::MenuBranchTree:      return Cmd::mContextMenuBranchTree;
        case VariousListIndex::MenuStashTree:       return Cmd::mContextMenuStashTree;
        case VariousListIndex::Toolbar1:            return Cmd::mToolbars[0];
        case VariousListIndex::Toolbar2:            return Cmd::mToolbars[1];
        case VariousListIndex::Size:
        case VariousListIndex::MergeTool:
        case VariousListIndex::Icons:
        break;
    }
    return fDummy;
}

QString CustomGitActions::getVariousListHeader(VariousListIndex::e aIndex)
{
    switch (aIndex)
    {
        case VariousListIndex::Icons:               return tr("Icons");
        case VariousListIndex::MenuSrcTree:         return tr("Context Menu Source");
        case VariousListIndex::MenuEmptySrcTree:    return tr("Context Menu Empty Source");
        case VariousListIndex::MenuGraphicView:     return tr("Context Menu Graphics View");
        case VariousListIndex::MenuHistoryTree:     return tr("Context Menu History");
        case VariousListIndex::MenuBranchTree:      return tr("Context Menu Branch");
        case VariousListIndex::MenuStashTree:       return tr("Context Menu Stash");
        case VariousListIndex::Toolbar1:            return tr("Toolbar 1");
        case VariousListIndex::Toolbar2:            return tr("Toolbar 2");
        case VariousListIndex::MergeTool:           return tr("Merge or Diff Tool");
        case VariousListIndex::Size: break;
    }
    return "";
}


void CustomGitActions::on_ActionTableListItemChanged ( QStandardItem * item )
{
    if (!mInitialize)
    {
        int fColumn = item->column();
        Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(item->row(), ActionsTable::ID)).toInt());
        auto* fAction = mActionList.getAction(fCmd);
        if (fAction)
        {
            QString fText = item->text();
            uint fFlag = 0;
            switch (static_cast<ActionsTable::e>(fColumn))
            {
                case ActionsTable::ID:
                case ActionsTable::Last:
                case ActionsTable::Icon:
                   break;
                case ActionsTable::Command:
                    fAction->setStatusTip(fText);
                    fFlag = ActionList::Flags::Modified;
                   break;
                case ActionsTable::MsgBoxText:
                    mActionList.setCustomCommandMessageBoxText(fCmd, fText);
                    fFlag = ActionList::Flags::Modified;
                   break;
                case ActionsTable::Name:
                    fAction->setToolTip(fText);
                    fAction->setText(fText);
                    fFlag = ActionList::Flags::Modified;
                   break;
                case ActionsTable::Shortcut:
                    fAction->setShortcut(QKeySequence(fText));
                    fFlag = ActionList::Flags::Modified;
                   break;
            }
            if (fFlag)
            {
                mActionList.setFlags(fCmd, fFlag);
            }
        }
    }
}

void CustomGitActions::initListIcons()
{
    mInitialize = true;
    mListModelVarious->setHeaderData(VariousHeader::Icon, Qt::Horizontal, getVariousListHeader(VariousListIndex::Icons));
    mListModelVarious->setHeaderData(VariousHeader::Name, Qt::Horizontal, tr("Name"));
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());
    QString fPath = ":/resource/24X24/";
    QDir fResources(fPath);
    QStringList fList = fResources.entryList();

    int fRow = 0;
    for (const auto& fItem : fList)
    {
        mListModelVarious->insertRows(fRow, 1, QModelIndex());
        mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Icon, QModelIndex()), QIcon(fPath + fItem), Qt::DecorationRole);
        mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Icon, QModelIndex()), QVariant(fPath + fItem), Qt::UserRole);
        mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Name, QModelIndex()), fItem, Qt::EditRole);
        ++fRow;
    }
    mInitialize = false;
}

const QString& CustomGitActions::iconCheck(bool check)
{
    static const QString enabled  { ":/resource/24X24/emblem-default.png" };
    static const QString disabled { ":/resource/24X24/edit-delete.png"    };
    return check ? enabled : disabled;
}

void CustomGitActions::initListMergeTool()
{
    mInitialize = true;
    mListModelVarious->setHeaderData(VariousHeader::Icon, Qt::Horizontal, tr("Active"));
    mListModelVarious->setHeaderData(VariousHeader::Name, Qt::Horizontal, tr("Tools"));
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());
    int fRow = 0;
    for (auto name = mMergeTools.begin(); name != mMergeTools.end(); ++name)
    {
        mListModelVarious->insertRows(fRow, 1, QModelIndex());
        mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Icon, QModelIndex()), QIcon(iconCheck(name.value())), Qt::DecorationRole);
        mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Name, QModelIndex()), name.key(), Qt::EditRole);
        ++fRow;
    }
    mInitialize = false;
}

void CustomGitActions::initMenuList(const Cmd::tVector& aItems, const QString& aHeader)
{
    mInitialize = true;
    mListModelVarious->setHeaderData(VariousHeader::Icon, Qt::Horizontal, tr("Icon"));
    mListModelVarious->setHeaderData(VariousHeader::Name, Qt::Horizontal, aHeader);
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());

    int fRow = 0;
    for (const auto& fCmd: aItems)
    {
        const QAction* fAction = mActionList.getAction(fCmd);

        mListModelVarious->insertRows(fRow, 1, QModelIndex());
        if (fAction)
        {
            mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Icon, QModelIndex()), fAction->icon(), Qt::DecorationRole);
            mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Name, QModelIndex()), fAction->toolTip(), Qt::EditRole);
        }
        else
        {
            mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Name, QModelIndex()), tr("-- Separator --"), Qt::EditRole);
        }
        ++fRow;
    }
    mInitialize = false;
}

void CustomGitActions::on_btnToLeft_clicked()
{
    if (ui->comboBoxVarious->currentIndex() == VariousListIndex::Icons)
    {
        int fIconRow   = ui->tableViewVarious->currentIndex().row();
        int fActionRow = ui->tableViewActions->currentIndex().row();
        Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fActionRow, ActionsTable::ID)).toInt());
        QString fIconPath = mListModelVarious->data(mListModelVarious->index(fIconRow, VariousHeader::Icon), Qt::UserRole).toString();
        if (fIconPath.size())
        {
            mListModelActions->setData(mListModelActions->index(fActionRow, ActionsTable::Icon), QIcon(fIconPath), Qt::DecorationRole);
            mActionList.setIconPath(fCmd, fIconPath);
            mActionList.setFlags(fCmd, ActionList::Flags::Modified, Flag::set);
        }
    }
    else
    {
        int fIconRow     = ui->tableViewVarious->currentIndex().row();
        auto& fCmdVector = getCmdVector(static_cast<VariousListIndex::e>(ui->comboBoxVarious->currentIndex()));
        fCmdVector.erase(fCmdVector.begin()+fIconRow);
        int fSelected = static_cast<int>(fCmdVector.size()-1);
        on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
        ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fSelected, ActionsTable::ID), QItemSelectionModel::Select);
        on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
    }
}

void CustomGitActions::on_btnToRight_clicked()
{
    int fActionRow = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fActionRow, ActionsTable::ID)).toInt());
    auto& fCmdVector = getCmdVector(static_cast<VariousListIndex::e>(ui->comboBoxVarious->currentIndex()));
    fCmdVector.push_back(fCmd);
    int fSelected = static_cast<int>(fCmdVector.size()-1);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fSelected, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnMoveUp_clicked()
{
    int fRow      = ui->tableViewVarious->currentIndex().row();
    int fMovedRow = fRow-1;
    auto& fCmdVector = getCmdVector(static_cast<VariousListIndex::e>(ui->comboBoxVarious->currentIndex()));
    std::swap(fCmdVector[fRow], fCmdVector[fMovedRow]);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fMovedRow, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnMoveDown_clicked()
{
    int fRow      = ui->tableViewVarious->currentIndex().row();
    int fMovedRow = fRow+1;
    auto& fCmdVector = getCmdVector(static_cast<VariousListIndex::e>(ui->comboBoxVarious->currentIndex()));
    std::swap(fCmdVector[fRow], fCmdVector[fMovedRow]);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fMovedRow, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnAdd_clicked()
{
    auto  fCmd    = mActionList.createNewID(Cmd::CustomCommand);
    auto* fAction = mActionList.createAction(fCmd, tr("command name"), "git ");
    if (fAction)
    {
        int fRow = -1;
        mActionList.setIconPath(fCmd, "://resource/24X24/window-close.png");
        insertCmdAction(*mActionList.getList().find(fCmd), fRow);
        mActionList.setFlags(fCmd, ActionList::Flags::Custom);
        Q_EMIT initCustomAction(fAction);
    }
}

void CustomGitActions::on_btnDelete_clicked()
{
    int fRow = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fRow, ActionsTable::ID)).toInt());
    if (fCmd != Cmd::Separator && !(mActionList.getFlags(fCmd) & ActionList::Flags::BuiltIn))
    {
        mListModelActions->removeRow(fRow);
        mActionList.deleteAction(fCmd);
        for (int i = VariousListIndex::FirstCmds; i < VariousListIndex::Toolbar2; ++i)
        {
            auto& fVector = getCmdVector(static_cast<VariousListIndex::e>(i));
            auto fFound = std::find_if(fVector.begin(), fVector.end(), [fCmd](Cmd::eCmd fI) {return fI == fCmd;});
            if (fFound != fVector.end() )
            {
                if (i == ui->comboBoxVarious->currentIndex())
                {
                    on_comboBoxVarious_currentIndexChanged(i);
                    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(static_cast<int>(fVector.size()-1), ActionsTable::ID), QItemSelectionModel::Select);
                    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
                }
            }
        }
    }
}


void CustomGitActions::on_tableViewActions_clicked(const QModelIndex & /* index */)
{
    if (ui->comboBoxVarious->currentIndex() == VariousListIndex::Icons)
    {
        enableButtons(Btn::Add|Btn::Delete);
    }
    else
    {
        enableButtons(Btn::Add|Btn::Delete|Btn::Right);
    }
}

void CustomGitActions::on_tableViewVarious_clicked(const QModelIndex &index)
{
    ui->btnAdd->setEnabled(false);
    if (ui->comboBoxVarious->currentIndex() == VariousListIndex::Icons)
    {
        enableButtons(Btn::Left);
    }
    else if (ui->comboBoxVarious->currentIndex() == VariousListIndex::MergeTool)
    {
        enableButtons(0);
        if (index.column() == 0 && index.row() >= 0 && index.row() < mMergeTools.size())
        {
            auto currentItem = mMergeTools.begin() + index.row();
            currentItem.value() = !currentItem.value();
            mListModelVarious->setData(index, QIcon(iconCheck(currentItem.value())), Qt::DecorationRole);
        }
    }
    else
    {
        std::uint32_t fFlags = Btn::Left;
        if (index.row() > 0)                               fFlags |= Btn::Up;
        if (index.row() < mListModelVarious->rowCount()-1) fFlags |= Btn::Down;
        enableButtons(fFlags);
    }
}

void CustomGitActions::enableButtons(std::uint32_t aBtnFlag)
{
    ui->btnAdd->setEnabled(     (aBtnFlag&Btn::Add)    != 0);
    ui->btnDelete->setEnabled(  (aBtnFlag&Btn::Delete) != 0);
    ui->btnMoveUp->setEnabled(  (aBtnFlag&Btn::Up)     != 0);
    ui->btnMoveDown->setEnabled((aBtnFlag&Btn::Down)   != 0);
    ui->btnToRight->setEnabled( (aBtnFlag&Btn::Right)  != 0);
    ui->btnToLeft->setEnabled(  (aBtnFlag&Btn::Left)   != 0);
}

void CustomGitActions::on_tableViewActions_customContextMenuRequested(const QPoint &pos)
{
    QMenu fMenu(this);

    int fRow = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd fCmd   = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fRow, ActionsTable::ID)).toInt());
    uint fFlags      = mActionList.getFlags(fCmd);

    fMenu.addAction(mActionList.getAction(fCmd));
    fMenu.addSeparator();

    int fPostAction = mActionList.getCustomCommandPostAction(fCmd);
    QActionGroup fPostActionGroup(this);
    fPostActionGroup.setExclusive(true);
    bool variable_cmd = (fFlags & ActionList::Flags::NotVariableGitCmd) == 0;
    bool function_cmd = (fFlags & ActionList::Flags::FunctionCmd);

    auto *fDoNothing = fPostActionGroup.addAction(Cmd::toString(Cmd::DoNothing));
    fDoNothing->setEnabled(variable_cmd);
    set_tooltip(fDoNothing, tr("Do nothing after command execution"));
    fMenu.addAction(fDoNothing);

    auto*fUpdateStatus = fPostActionGroup.addAction(Cmd::toString(Cmd::UpdateItemStatus));
    fUpdateStatus->setEnabled(variable_cmd);
    set_tooltip(fUpdateStatus, tr("Update item status after command execution"));
    fMenu.addAction(fUpdateStatus);

    auto*fParseHistory = fPostActionGroup.addAction(Cmd::toString(Cmd::ParseHistoryText));
    fParseHistory->setEnabled(variable_cmd);
    set_tooltip(fParseHistory, tr("Parse command result for history view"));
    fMenu.addAction(fParseHistory);

    auto*fParseBranch = fPostActionGroup.addAction(Cmd::toString(Cmd::ParseBranchListText));
    fParseBranch->setEnabled(variable_cmd);
    set_tooltip(fParseBranch, tr("Parse command result for branch view"));
    fMenu.addAction(fParseBranch);

    for (auto& fItem : fPostActionGroup.actions())
    {
        fItem->setCheckable(true);
    }

    if (isBetween(fPostAction, 0, fPostActionGroup.actions().size()))
    {
        fPostActionGroup.actions().at(fPostAction)->setChecked(true);
    }
    std::vector<int> parse_action { Cmd::ParseHistoryText, Cmd::ParseBranchListText};

    ulong fEnableFlag  = mActionList.getFlags(fCmd, ActionList::Data::StatusFlagEnable);
    ulong fDisableFlag = mActionList.getFlags(fCmd, ActionList::Data::StatusFlagDisable);

    std::vector<Type::TypeFlags> fGitStatusArray =
    {
        Type::GitAdded, Type::GitModified, Type::GitStaged, Type::GitDeleted,
        Type::GitUnTracked, Type::GitUnmerged, Type::GitLocal, Type::GitRemote, Type::Folder
    };

    fMenu.addSeparator();

    QActionGroup fGitStatusEnableGroup(this);
    fGitStatusEnableGroup.setExclusive(false);

    QActionGroup fGitStatusDisableGroup(this);
    fGitStatusDisableGroup.setExclusive(false);

    QActionGroup fGitStatusEnableNotGroup(this);
    fGitStatusEnableNotGroup.setExclusive(false);

    if (! function_cmd)
    {
        QMenu* fEnableMenu = fMenu.addMenu(tr("Enable for git status set"));
        QMenu* fDisableMenu = fMenu.addMenu(tr("Disable for git status set"));
        QMenu* fEnableNotMenu = fMenu.addMenu(tr("Enable for git status not set"));

        for (auto fGitStatus : fGitStatusArray)
        {
            bool fEnabled  = fEnableFlag  & fGitStatus;
            bool fDisabled = fDisableFlag & fGitStatus;
            if (fGitStatus == Type::None)
            {
                fEnableMenu->addSeparator();
                fDisableMenu->addSeparator();
                fEnableNotMenu->addSeparator();
                continue;
            }
            QAction* fAction = fGitStatusEnableGroup.addAction(Type::name(fGitStatus));
            fEnableMenu->addAction(fAction);
            fAction->setCheckable(true);
            fAction->setChecked(fEnabled && !fDisabled);
            set_tooltip(fAction, tr("Enable command if this git status is set"));

            fAction = fGitStatusDisableGroup.addAction(Type::name(fGitStatus));
            fDisableMenu->addAction(fAction);
            fAction->setCheckable(true);
            fAction->setChecked(!fEnabled && fDisabled);
            set_tooltip(fAction, tr("Disable command if this git status is set"));

            fAction = fGitStatusEnableNotGroup.addAction(Type::name(fGitStatus));
            fEnableNotMenu->addAction(fAction);
            fAction->setCheckable(true);
            fAction->setChecked(fEnabled && fDisabled);
            set_tooltip(fAction, tr("Enable command if this git status is not set"));
        }
    }
    fMenu.addSeparator();

    QAction* fA_Modified = nullptr;
    if (fFlags & ActionList::Flags::BuiltIn && fFlags & ActionList::Flags::Modified)
    {
         fA_Modified = fMenu.addAction(tr("Reset modifications"));
         set_tooltip(fA_Modified, tr("After program restart all modifications of this command are removed"));
    }

    bool custom_enabled = (fFlags & ActionList::Flags::Custom) || mExperimental;
    QAction* fA_BranchCmd  = fMenu.addAction(tr("Branch command"));
    fA_BranchCmd->setCheckable(true);
    fA_BranchCmd->setChecked(fFlags & ActionList::Flags::Branch);
    fA_BranchCmd->setEnabled(custom_enabled);
    set_tooltip(fA_BranchCmd, tr("The git command is called in branch view context"));

    QAction* fA_HistoryCmd = fMenu.addAction(tr("History command"));
    fA_HistoryCmd->setCheckable(true);
    fA_HistoryCmd->setChecked(fFlags & ActionList::Flags::History);
    fA_HistoryCmd->setEnabled(custom_enabled);
    set_tooltip(fA_HistoryCmd, tr("The git command is called in history view context"));

    QAction* fA_StashCmd = fMenu.addAction(tr("Stash command"));
    fA_StashCmd->setCheckable(true);
    fA_StashCmd->setChecked(fFlags & ActionList::Flags::Stash);
    fA_StashCmd->setEnabled(custom_enabled);
    set_tooltip(fA_StashCmd, tr("The git command is called in stash view context"));

    QAction* fA_ThreadCmd  = fMenu.addAction(tr("Invoke command unattached"));
    fA_ThreadCmd->setCheckable(true);
    fA_ThreadCmd->setChecked(fFlags & ActionList::Flags::CallInThread);
    set_tooltip(fA_ThreadCmd, tr("The git command is called without blocking the program"));

    fMenu.addSeparator();
    if (function_cmd && !mExperimental)
    {
        fMenu.removeAction(fDoNothing);
        fMenu.removeAction(fUpdateStatus);
        fMenu.removeAction(fParseHistory);
        fMenu.removeAction(fParseBranch);
        fMenu.removeAction(fA_BranchCmd);
        fMenu.removeAction(fA_HistoryCmd);
        fMenu.removeAction(fA_ThreadCmd);
    }
    /// NOTE: a parsed command cannot be processed asynchronously
    if (std::find(parse_action.begin(), parse_action.end(), fPostAction) != parse_action.end())
    {
        fMenu.removeAction(fA_ThreadCmd);
    }

    set_tooltip(fMenu.addAction(tr("Cancel")), tr("Don't change command"));

    QAction* fSelected = fMenu.exec(mapToGlobal(pos));
    if (fSelected)
    {
        bool modified = false;
        int fIndex =  fPostActionGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setCustomCommandPostAction(fCmd, fIndex);
            modified = true;
        }
        fIndex = fGitStatusEnableGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagEnable);
            modified = true;
        }
        fIndex = fGitStatusDisableGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagDisable);
            modified = true;
        }
        fIndex = fGitStatusEnableNotGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagEnable);
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagDisable);
            modified = true;
        }
        if (fA_Modified == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Flags::Modified, Flag::remove);
        }
        if (fA_BranchCmd == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Flags::Branch, fA_BranchCmd->isChecked() ? Flag::set : Flag::remove);
            modified = true;
        }
        if (fA_StashCmd == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Flags::Stash, fA_StashCmd->isChecked() ? Flag::set : Flag::remove);
            modified = true;
        }
        if (fA_ThreadCmd == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Flags::CallInThread, fA_ThreadCmd->isChecked() ? Flag::set : Flag::remove);
            modified = true;
        }
        if (modified)
        {
            mActionList.setFlags(fCmd, ActionList::Flags::Modified, Flag::set);
        }
    }
}

void CustomGitActions::set_tooltip(QAction* action, const QString& tool_tip)
{
    action->setParent(this->parent());
    action->setStatusTip(tool_tip);
}

ActionItemModel::ActionItemModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows, columns, parent)
{
}

Qt::ItemFlags ActionItemModel::flags(const QModelIndex &aIndex) const
{
    Qt::ItemFlags fFlags = QStandardItemModel::flags(aIndex);

    Cmd::eCmd fCmd   = static_cast<Cmd::eCmd>(data(index(aIndex.row(), ActionsTable::ID)).toInt());

    if (fCmd == Cmd::Separator)
    {
        fFlags &= ~Qt::ItemIsEditable;
    }
    else
    {
        switch (static_cast<ActionsTable::e>(aIndex.column()))
        {
            case ActionsTable::ID:
            case ActionsTable::Icon:
                fFlags &= ~Qt::ItemIsEditable;
                break;
            case ActionsTable::MsgBoxText:
            {
                auto variant     = data(index(aIndex.row(), ActionsTable::MsgBoxText));
                if (variant.isValid() && variant.toString().size() == 0)
                {
                    fFlags &= ~Qt::ItemIsEditable;
                }
            }  break;
            default:break;
        }
    }
    return  fFlags;
}

VariousItemModel::VariousItemModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows, columns, parent)
{
}

Qt::ItemFlags VariousItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags fFlags = QStandardItemModel::flags(index);
    fFlags &= ~Qt::ItemIsEditable;
    return  fFlags;
}

void CustomGitActions::on_btnHelp_clicked(bool checked)
{
    for (auto tooltip = mToolTips.begin(); tooltip != mToolTips.end(); ++tooltip)
    {
        tooltip.key()->setToolTip(checked ? tooltip.value() : "");
    }
}

