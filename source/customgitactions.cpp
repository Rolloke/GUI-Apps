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

using namespace git;

CustomGitActions::CustomGitActions(ActionList& aList, QWidget *parent) :
    QDialog(parent)
,   ui(new Ui::CustomGitActions)
,   mActionList(aList)
,   mListModelActions(nullptr)
,   mListModelVarious(nullptr)
,   mInitialize(false)
{
    ui->setupUi(this);

    QStringList fColumnName  = { tr("ID"), tr("Icon"), tr("Command"), tr("Name"), tr("Shortcut"), tr("Message box text")};
    mActionListColumnWidth   = {  0.055  ,    0.055  ,      0.25    ,    0.25   ,      0.1      ,        0.25           };

    assert(fColumnName.size()            == ActionsTable::Last);
    assert(mActionListColumnWidth.size() == ActionsTable::Last);

    double fItemWidth = 0;
    std::for_each(mActionListColumnWidth.begin(), mActionListColumnWidth.end()-1, [&fItemWidth](float fItem ) { fItemWidth += fItem; });
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

    int fWidth = ui->tableViewVarious->rect().width();
    mListModelVarious = new VariousItemModel(0, VariousHeader::Size, this);
    ui->tableViewVarious->setModel(mListModelVarious);
    ui->tableViewVarious->setColumnWidth(VariousHeader::Icon, INT(0.2  * fWidth));
    ui->tableViewVarious->setColumnWidth(VariousHeader::Name, INT(0.75 * fWidth));

    enableButtons(0);
    on_comboBoxVarious_currentIndexChanged(VariousListIndex::Icons);
}

CustomGitActions::~CustomGitActions()
{
    delete ui;
}


void CustomGitActions::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    int fWidth = ui->tableViewActions->rect().width();

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
        if (aRow == -1) aRow = mListModelActions->rowCount();
        mListModelActions->insertRows(aRow, 1, QModelIndex());
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::ID)        , aItem.first, Qt::DisplayRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Command)   , fCommand, Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Name)      , fAction->toolTip(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Shortcut)  , fAction->shortcut().toString(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::MsgBoxText), mActionList.getCustomCommandMessageBoxText(static_cast<Cmd::eCmd>(aItem.first)), Qt::EditRole);
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
        default:
        if (isBetween(fIndex, VariousListIndex::FirstCmds, VariousListIndex::LastCmds))
        {
            ui->btnToLeft->setToolTip(tr("Remove selected item from %1").arg(getVariousListHeader(fIndex)));
            initMenuList(getCmdVector(fIndex), getVariousListHeader(fIndex));
        }   break;
    }
}

Cmd::tVector& CustomGitActions::getCmdVector(VariousListIndex::e aIndex)
{
    static Cmd::tVector fDummy;
    switch (aIndex)
    {
        case VariousListIndex::MenuSrcTree:         return Cmd::mContextMenuSourceTree;
        case VariousListIndex::MenuEmptySrcTree:    return Cmd::mContextMenuEmptySourceTree;
        case VariousListIndex::MenuHistoryTree:     return Cmd::mContextMenuHistoryTree;
        case VariousListIndex::MenuBranchTree:      return Cmd::mContextMenuBranchTree;
        case VariousListIndex::Toolbar1:            return Cmd::mToolbars[0];
        case VariousListIndex::Toolbar2:            return Cmd::mToolbars[1];
        default: break;
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
        case VariousListIndex::MenuHistoryTree:     return tr("Context Menu History");
        case VariousListIndex::MenuBranchTree:      return tr("Context Menu Branch");
        case VariousListIndex::Toolbar1:            return tr("Toolbar 1");
        case VariousListIndex::Toolbar2:            return tr("Toolbar 2");
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
                    fFlag = ActionList::Modified;
                   break;
                case ActionsTable::MsgBoxText:
                    mActionList.setCustomCommandMessageBoxText(fCmd, fText);
                    fFlag = ActionList::Modified;
                   break;
                case ActionsTable::Name:
                    fAction->setToolTip(fText);
                    fAction->setText(fText);
                    fFlag = ActionList::Modified;
                   break;
                case ActionsTable::Shortcut:
                    fAction->setShortcut(QKeySequence(fText));
                    fFlag = ActionList::Modified;
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
    auto  fCmd    = mActionList.getNextCustomID();
    auto* fAction = mActionList.createAction(fCmd, "command name", "git ");
    if (fAction)
    {
        int fRow = -1;
        mActionList.setIconPath(fCmd, "://resource/24X24/window-close.png");
        insertCmdAction(*mActionList.getList().find(fCmd), fRow);
        mActionList.setFlags(fCmd, ActionList::Custom);
        Q_EMIT(initCustomAction(fAction));
    }
}

void CustomGitActions::on_btnDelete_clicked()
{
    int fRow = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fRow, ActionsTable::ID)).toInt());
    if (fCmd != Cmd::Separator && !(mActionList.getFlags(fCmd) & ActionList::BuiltIn))
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

    fMenu.addAction(mActionList.getAction(fCmd));
    fMenu.addSeparator();

    int fPostAction = mActionList.getCustomCommandPostAction(fCmd);
    QActionGroup fPostActionGroup(this);
    fPostActionGroup.setExclusive(true);
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::DoNothing)));
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::UpdateItemStatus)));
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::ParseHistoryText)));
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::ParseBranchListText)));
    for (auto& fItem : fPostActionGroup.actions())
    {
        fItem->setCheckable(true);
    }
    if (isBetween(fPostAction, 0, fPostActionGroup.actions().size()))
    {
        fPostActionGroup.actions().at(fPostAction)->setChecked(true);
    }

    ulong fEnableFlag  = mActionList.getFlags(fCmd, ActionList::Data::StatusFlagEnable);
    ulong fDisableFlag = mActionList.getFlags(fCmd, ActionList::Data::StatusFlagDisable);

    std::vector<Type::TypeFlags> fGitStatusArray =
    {
        Type::GitAdded, Type::GitModified, Type::GitStaged, Type::GitDeleted,
        Type::GitUnTracked, Type::GitUnmerged, Type::GitLocal, Type::GitRemote
    };

    fMenu.addSeparator();

    QMenu* fEnableMenu = fMenu.addMenu("Enable for git status set");
    QActionGroup fGitStatusEnableGroup(this);
    fGitStatusEnableGroup.setExclusive(false);

    QMenu* fDisableMenu = fMenu.addMenu("Disable for git status set");
    QActionGroup fGitStatusDisableGroup(this);
    fGitStatusDisableGroup.setExclusive(false);

    QMenu* fEnableNotMenu = fMenu.addMenu("Enable for git status not set");
    QActionGroup fGitStatusEnableNotGroup(this);
    fGitStatusEnableNotGroup.setExclusive(false);

    for (auto fGitStatus : fGitStatusArray)
    {
        bool fEnabled  = fEnableFlag  & fGitStatus;
        bool fDisabled = fDisableFlag & fGitStatus;
        QAction* fAction = fGitStatusEnableGroup.addAction(Type::name(fGitStatus));
        fEnableMenu->addAction(fAction);
        fAction->setCheckable(true);
        fAction->setChecked(fEnabled && !fDisabled);

        fAction = fGitStatusDisableGroup.addAction(Type::name(fGitStatus));
        fDisableMenu->addAction(fAction);
        fAction->setCheckable(true);
        fAction->setChecked(!fEnabled && fDisabled);

        fAction = fGitStatusEnableNotGroup.addAction(Type::name(fGitStatus));
        fEnableNotMenu->addAction(fAction);
        fAction->setCheckable(true);
        fAction->setChecked(fEnabled && fDisabled);
    }

    fMenu.addSeparator();

    uint fFlags      = mActionList.getFlags(fCmd);
    QAction* fA_Modified = nullptr;
    if (fFlags & ActionList::BuiltIn && fFlags & ActionList::Modified)
    {
         fA_Modified = fMenu.addAction(tr("Reset modifications"));
         fA_Modified->setCheckable(true);
         fA_Modified->setChecked(fFlags & ActionList::Modified);
    }

    QAction* fA_BranchCmd = nullptr;
    if (fFlags & ActionList::Custom)
    {
         fA_BranchCmd = fMenu.addAction(tr("Branch command"));
         fA_BranchCmd->setCheckable(true);
         fA_BranchCmd->setChecked(fFlags & ActionList::Branch);
    }
    fMenu.addSeparator();

    fMenu.addAction(tr("Cancel"));

    QAction* fSelected = fMenu.exec(mapToGlobal(pos));
    if (fSelected)
    {
        int fIndex =  fPostActionGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setCustomCommandPostAction(fCmd, fIndex);
            mActionList.setFlags(fCmd, ActionList::Modified, Flag::set);
        }
        fIndex = fGitStatusEnableGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagEnable);
        }
        fIndex = fGitStatusDisableGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagDisable);
        }
        fIndex = fGitStatusEnableNotGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagEnable);
            mActionList.setFlags(fCmd, fGitStatusArray[fIndex], fSelected->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagDisable);
        }
        if (fA_Modified == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Modified, fA_Modified->isChecked() ? Flag::set : Flag::remove);
        }
        if (fA_BranchCmd == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Branch, fA_BranchCmd->isChecked() ? Flag::set : Flag::remove);
        }
    }
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
