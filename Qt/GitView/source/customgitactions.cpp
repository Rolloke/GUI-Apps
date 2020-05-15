#include "customgitactions.h"
#include "ui_customgitactions.h"
#include "actions.h"
#include "logger.h"
#include "helper.h"

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


    QStringList      fColumnName  = { tr("ID"), tr("Icon"), tr("Command"), tr("Name"), tr("Shortcut"),  tr("Message box text")};
    std::vector<int> fColumnWidth = {     42  ,      42   ,       200    ,     200   ,        65     ,           200          };

    mListModelActions = new ActionItemModel(0, INT(ActionsTable::Last), this);
    connect(mListModelActions, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_ActionTableListItemChanged(QStandardItem*)));

    ui->tableViewActions->setModel(mListModelActions);
    ui->tableViewActions->setContextMenuPolicy(Qt::CustomContextMenu);

    int fWidth = ui->tableViewActions->rect().width();
    int fItemWidth = 0;
    std::for_each(fColumnWidth.begin(), fColumnWidth.end()-1, [&fItemWidth](int fItem ) { fItemWidth+= fItem; });
    fColumnWidth[INT(ActionsTable::MsgBoxText)] = fWidth - fItemWidth;

    for (int fColumn = 0; fColumn<INT(ActionsTable::Last); ++fColumn)
    {
        mListModelActions->setHeaderData(fColumn, Qt::Horizontal, fColumnName[fColumn], Qt::DisplayRole);
        ui->tableViewActions->setColumnWidth(fColumn            , fColumnWidth[fColumn]);
    }


    mInitialize = true;
    int fRow = 0;
    for (const auto fItem : mActionList.getList())
    {
        insertCmdAction(fItem, fRow);
    }
    mListModelActions->insertRows(fRow, 1, QModelIndex());
    mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::ID))  , git::Cmd::Separator, Qt::DisplayRole);
    mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::Name)), "-- Separator --", Qt::EditRole);
    mListModelActions->flags(mListModelActions->index(fRow, INT(ActionsTable::Name))) &= ~Qt::ItemIsEditable;

    mInitialize = false;
    ui->tableViewActions->selectionModel()->setCurrentIndex(mListModelActions->index(0, INT(ActionsTable::ID)), QItemSelectionModel::SelectCurrent);

    fWidth = ui->tableViewVarious->rect().width();
    mListModelVarious = new VariousItemModel(0, 1, this);
    ui->tableViewVarious->setModel(mListModelVarious);
    ui->tableViewVarious->setColumnWidth(0, fWidth);

    enableButtons(0);
    on_comboBoxVarious_currentIndexChanged(INT(VariousListIndex::Icons));
}

CustomGitActions::~CustomGitActions()
{
    delete ui;
}

void CustomGitActions::insertCmdAction(ActionList::tActionMap::const_reference aItem, int & aRow)
{
    const QAction* fAction = aItem.second;
    QString fCommand = fAction->statusTip();
    if (fCommand.size())
    {
        if (aRow == -1) aRow = mListModelActions->rowCount();
        mListModelActions->insertRows(aRow, 1, QModelIndex());
        mListModelActions->setData(mListModelActions->index(aRow, INT(ActionsTable::ID))        , aItem.first, Qt::DisplayRole);
        mListModelActions->setData(mListModelActions->index(aRow, INT(ActionsTable::Command))   , fCommand, Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, INT(ActionsTable::Name))      , fAction->toolTip(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, INT(ActionsTable::Shortcut))  , fAction->shortcut().toString(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, INT(ActionsTable::MsgBoxText)), mActionList.getCustomCommandMessageBoxText(static_cast<Cmd::eCmd>(aItem.first)), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, INT(ActionsTable::Icon))      , QIcon(mActionList.getIconPath(static_cast<Cmd::eCmd>(aItem.first))), Qt::DecorationRole);
        ++aRow;
    }
}

void CustomGitActions::on_comboBoxVarious_currentIndexChanged(int aIndex)
{
    switch (static_cast<VariousListIndex>(aIndex))
    {
        case VariousListIndex::Icons:
            initListIcons();
            ui->btnToLeft->setToolTip(tr("Apply selected icon in right view to selected command entry in left view"));
        break;
        default:
        if (isBetween(aIndex, 0, mVariousListHeader[aIndex].size()))
        {
            ui->btnToLeft->setToolTip(tr("Remove selected item from %1").arg(mVariousListHeader[aIndex]));
            initMenuList(getCmdVector(static_cast<VariousListIndex>(aIndex)), mVariousListHeader[aIndex]);
        }   break;
    }
}

Cmd::tVector& CustomGitActions::getCmdVector(VariousListIndex aIndex)
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


void CustomGitActions::on_ActionTableListItemChanged ( QStandardItem * item )
{
    if (!mInitialize)
    {
        int fColumn = item->column();
        Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(item->row(), INT(ActionsTable::ID))).toInt());
        auto* fAction = mActionList.getAction(fCmd);
        if (fAction)
        {
            QString fText = item->text();
            uint fFlag = 0;
            switch (static_cast<ActionsTable>(fColumn))
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
    mListModelVarious->setHeaderData(0, Qt::Horizontal, tr("Icons"));
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());
    QString fPath = ":/resource/24X24/";
    QDir fResources(fPath);
    QStringList fList = fResources.entryList();

    int fRow = 0;
    for (const auto& fItem : fList)
    {
        mListModelVarious->insertRows(fRow, 1, QModelIndex());
        mListModelVarious->setData(mListModelVarious->index(fRow, 0, QModelIndex()), QIcon(fPath + fItem), Qt::DecorationRole);
        mListModelVarious->setData(mListModelVarious->index(fRow, 0, QModelIndex()), QVariant(fPath + fItem), Qt::UserRole);
        ++fRow;
    }
    mInitialize = false;
}

void CustomGitActions::initMenuList(const Cmd::tVector& aItems, const QString& aHeader)
{
    mInitialize = true;
    mListModelVarious->setHeaderData(0, Qt::Horizontal, aHeader);
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());

    int fRow = 0;
    for (auto fCmd: aItems)
    {
        const QAction* fAction = mActionList.getAction(fCmd);

        mListModelVarious->insertRows(fRow, 1, QModelIndex());
        if (fAction)
        {
            mListModelVarious->setData(mListModelVarious->index(fRow, 0, QModelIndex()), fAction->toolTip(), Qt::EditRole);
        }
        else
        {
            mListModelVarious->setData(mListModelVarious->index(fRow, 0, QModelIndex()), tr("-- Separator --"), Qt::EditRole);
        }
        ++fRow;
    }
    mInitialize = false;
}

void CustomGitActions::on_btnToLeft_clicked()
{
    if (ui->comboBoxVarious->currentIndex() == INT(VariousListIndex::Icons))
    {
        int fIconRow   = ui->tableViewVarious->currentIndex().row();
        int fActionRow = ui->tableViewActions->currentIndex().row();
        Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fActionRow, INT(ActionsTable::ID))).toInt());
        QString fIconPath = mListModelVarious->data(mListModelVarious->index(fIconRow, 0), Qt::UserRole).toString();
        if (fIconPath.size())
        {
            mListModelActions->setData(mListModelActions->index(fActionRow, INT(ActionsTable::Icon)), QIcon(fIconPath), Qt::DecorationRole);
            mActionList.setIconPath(fCmd, fIconPath);
        }
    }
    else
    {
        int fIconRow     = ui->tableViewVarious->currentIndex().row();
        auto& fCmdVector = getCmdVector(static_cast<VariousListIndex>(ui->comboBoxVarious->currentIndex()));
        fCmdVector.erase(fCmdVector.begin()+fIconRow);
        int fSelected = fCmdVector.size()-1;
        on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
        ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fSelected, INT(ActionsTable::ID)), QItemSelectionModel::Select);
        on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
    }
}

void CustomGitActions::on_btnToRight_clicked()
{
    int fActionRow = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fActionRow, 0)).toInt());
    auto& fCmdVector = getCmdVector(static_cast<VariousListIndex>(ui->comboBoxVarious->currentIndex()));
    fCmdVector.push_back(fCmd);
    int fSelected = fCmdVector.size()-1;
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fSelected, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnMoveUp_clicked()
{
    int fRow      = ui->tableViewVarious->currentIndex().row();
    int fMovedRow = fRow-1;
    auto& fCmdVector = getCmdVector(static_cast<VariousListIndex>(ui->comboBoxVarious->currentIndex()));
    std::swap(fCmdVector[fRow], fCmdVector[fMovedRow]);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fMovedRow, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnMoveDown_clicked()
{
    int fRow      = ui->tableViewVarious->currentIndex().row();
    int fMovedRow = fRow+1;
    auto& fCmdVector = getCmdVector(static_cast<VariousListIndex>(ui->comboBoxVarious->currentIndex()));
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
    Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fRow, INT(ActionsTable::ID))).toInt());
    if (fCmd != Cmd::Separator && !(mActionList.getFlags(fCmd) & ActionList::BuiltIn))
    {
        mListModelActions->removeRow(fRow);
        mActionList.deleteAction(fCmd);
        for (int i = INT(VariousListIndex::FirstCmds); i < INT(VariousListIndex::Toolbar2); ++i)
        {
            auto& fVector = getCmdVector(static_cast<VariousListIndex>(i));
            auto fFound = std::find_if(fVector.begin(), fVector.end(), [fCmd](Cmd::eCmd fI) {return fI == fCmd;});
            if (fFound != fVector.end() )
            {
                if (i == ui->comboBoxVarious->currentIndex())
                {
                    on_comboBoxVarious_currentIndexChanged(i);
                    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fVector.size()-1, INT(ActionsTable::ID)), QItemSelectionModel::Select);
                    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
                }
            }
        }
    }
}


void CustomGitActions::on_tableViewActions_clicked(const QModelIndex & /* index */)
{
    if (ui->comboBoxVarious->currentIndex() == INT(VariousListIndex::Icons))
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
    if (ui->comboBoxVarious->currentIndex() == INT(VariousListIndex::Icons))
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
    Cmd::eCmd fCmd   = static_cast<Cmd::eCmd>(mListModelActions->data(mListModelActions->index(fRow, INT(ActionsTable::ID))).toInt());

    fMenu.addAction(mActionList.getAction(fCmd));
    fMenu.addSeparator();

    int fPostAction = mActionList.getCustomCommandPostAction(fCmd);
    QActionGroup fPostActionGroup(this);
    fPostActionGroup.setExclusive(true);
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::DoNothing)));
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::UpdateItemStatus)));
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::ParseHistoryText)));
    fMenu.addAction(fPostActionGroup.addAction(Cmd::toString(Cmd::ParseBranchListText)));
    for (auto*fItem : fPostActionGroup.actions())
    {
        fItem->setCheckable(true);
    }
    if (isBetween(fPostAction, 0, fPostActionGroup.actions().size()))
    {
        fPostActionGroup.actions().at(fPostAction)->setChecked(true);
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

    fMenu.addAction(tr("Cancel"));

    QAction* fSelected = fMenu.exec(mapToGlobal(pos));
    if (fSelected)
    {
        int fIndex =  fPostActionGroup.actions().indexOf(fSelected);
        if (fIndex != -1)
        {
            mActionList.setCustomCommandPostAction(fCmd, fIndex);
            mActionList.setFlags(fCmd, ActionList::Modified, true);
        }
        if (fA_Modified == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Modified, fA_Modified->isChecked());
        }
        if (fA_BranchCmd == fSelected)
        {
            mActionList.setFlags(fCmd, ActionList::Branch, fA_BranchCmd->isChecked());
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

    Cmd::eCmd fCmd   = static_cast<Cmd::eCmd>(data(index(aIndex.row(), INT(ActionsTable::ID))).toInt());
    if (fCmd == Cmd::Separator)
    {
        fFlags &= ~Qt::ItemIsEditable;
    }
    else
    {
        switch (static_cast<ActionsTable>(aIndex.column()))
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
