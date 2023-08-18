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
#include <QFileDialog>
#include <QInputDialog>
#include <QWhatsThis>

using namespace git;

bool CustomGitActions::VariousListIndex::isIcon(int index)
{
    return index == Icons || index == ExternalIcons;
}

bool CustomGitActions::VariousListIndex::isMenu(int index)
{
    return isBetween(index, FirstMenu, LastMenu);
}

bool CustomGitActions::VariousListIndex::isToolbar(int index)
{
    return index >= Toolbar1;
}

bool CustomGitActions::VariousListIndex::isCustomToolbar(int index)
{
    return index > Toolbar2;
}

CustomGitActions::VariousListIndex::e CustomGitActions::VariousListIndex::cast(int index)
{
    return static_cast<CustomGitActions::VariousListIndex::e>(index);
}

CustomGitActions::CustomGitActions(ActionList& aList, string2bool_map&aMergeTools, QWidget *parent) :
    QDialog(parent)
,   ui(new Ui::CustomGitActions)
,   mActionList(aList)
,   mMergeTools(aMergeTools)
,   mListModelActions(nullptr)
,   mListModelVarious(nullptr)
,   mInitialize(false)
,   mMergeToolsState(aMergeTools.size())
,   mSearchColumn(ActionsTable::Name)
,   mSearchRowStart(-1)
{
    ui->setupUi(this);
    QSizeF the_size = size();
    the_size *= 1.5;
    resize(the_size.width(), the_size.height());

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
    mActionListColumnWidth.back() = 1.0 - fItemWidth - 0.005;

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
    connect(ui->tableViewActions->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableViewActions_header_clicked(int)));

    mListModelActions->insertRows(++fRow, 1, QModelIndex());
    mListModelActions->setData(mListModelActions->index(fRow, ActionsTable::ID)  , git::Cmd::Submenu, Qt::DisplayRole);
    mListModelActions->setData(mListModelActions->index(fRow, ActionsTable::Command), tr("Insert submenu:"), Qt::DisplayRole);
    mListModelActions->setData(mListModelActions->index(fRow, ActionsTable::Name), tr("sub menu name"), Qt::DisplayRole);

    mInitialize = false;
    ui->tableViewActions->selectionModel()->setCurrentIndex(mListModelActions->index(0, ActionsTable::ID), QItemSelectionModel::SelectCurrent);

    const int various_width = ui->tableViewVarious->rect().width() - ui->tableViewVarious->verticalScrollBar()->sizeHint().width();
    const int icon_width = INT(0.11 * various_width);
    mListModelVarious = new VariousItemModel(0, VariousHeader::Size, this);
    ui->tableViewVarious->setModel(mListModelVarious);
    ui->tableViewVarious->setColumnWidth(VariousHeader::Icon, icon_width);
    ui->tableViewVarious->setColumnWidth(VariousHeader::Name, various_width-icon_width);
    ui->tableViewVarious->horizontalHeader()->setSectionResizeMode(VariousHeader::Icon, QHeaderView::Fixed);
    ui->tableViewVarious->horizontalHeader()->setSectionResizeMode(VariousHeader::Name, QHeaderView::Stretch);

    ui->tableViewVarious->setContextMenuPolicy(Qt::CustomContextMenu);

    const QString button_enabled_style = "QPushButton:enabled { background-color:skyblue;}";
    ui->btnToLeft->setStyleSheet(button_enabled_style);
    ui->btnToRight->setStyleSheet(button_enabled_style);
    ui->btnMoveUp->setStyleSheet(button_enabled_style);
    ui->btnMoveDown->setStyleSheet(button_enabled_style);
    ui->btnAdd->setStyleSheet(button_enabled_style);
    ui->btnDelete->setStyleSheet(button_enabled_style);

    enableButtons(0);
    for (uint i = 0; i < getVariousListSize(); ++i)
    {
        ui->comboBoxVarious->addItem(getVariousListHeader(VariousListIndex::cast(i)));
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
    QAction* action = mActionList.getAction(Cmd::CustomTestCommand);
    action->setStatusTip("");
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
    const QAction* action = aItem.second;
    QString command = action->statusTip();
    if (command.size())
    {
        uint flag = mActionList.getFlags(static_cast<Cmd::eCmd>(aItem.first), ActionList::Data::Flags);
        bool function_cmd = (flag & ActionList::Flags::FunctionCmd);
        bool modified_cmd = (flag & ActionList::Flags::Modified);
        if (aRow == -1)
        {
            int table_rows = mListModelActions->rowCount();
            for (int current_row = 0; current_row < table_rows; ++current_row)
            {
                QString sid = mListModelActions->data(mListModelActions->index(current_row, ActionsTable::ID)).toString();
                int id = atoi(sid.toStdString().c_str());
                if (id > aItem.first)
                {
                    aRow = current_row;
                    break;
                }
            }
        }
        mListModelActions->insertRows(aRow, 1, QModelIndex());
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::ID)        , tr("%1").arg(aItem.first) + (modified_cmd ? " *" : ""), Qt::DisplayRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Command)   , command, Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Name)      , action->toolTip(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Shortcut)  , action->shortcut().toString(), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::MsgBoxText), function_cmd ? "" : mActionList.getCustomCommandMessageBoxText(static_cast<Cmd::eCmd>(aItem.first)), Qt::EditRole);
        mListModelActions->setData(mListModelActions->index(aRow, ActionsTable::Icon)      , QIcon(mActionList.getIconPath(static_cast<Cmd::eCmd>(aItem.first))), Qt::DecorationRole);
        ++aRow;
    }
    else
    {
        TRACEX(Logger::warning, "no command text for " << action->toolTip());
    }
}

void CustomGitActions::on_comboBoxVarious_currentIndexChanged(int aIndex)
{
    const auto icon_tooltip = tr("Apply selected icon in right view to selected command entry in left view");
    auto fIndex = VariousListIndex::cast(aIndex);
    switch (fIndex)
    {
    case VariousListIndex::Icons:
        initListIcons(fIndex);
        ui->btnToLeft->setToolTip(icon_tooltip);
        break;
    case VariousListIndex::ExternalIcons:
        initListIcons(fIndex, mExternalIconFiles);
        ui->btnToLeft->setToolTip(icon_tooltip);
        break;
    case VariousListIndex::MergeTool:
        initListMergeTool();
        break;
    default:
        if (isBetween(static_cast<std::uint32_t>(fIndex), VariousListIndex::FirstCmds, getVariousListSize() - 1))
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
    case VariousListIndex::MenuCodeBrowser:     return Cmd::mContextMenuTextView;
    case VariousListIndex::MenuHistoryTree:     return Cmd::mContextMenuHistoryTree;
    case VariousListIndex::MenuBranchTree:      return Cmd::mContextMenuBranchTree;
    case VariousListIndex::MenuStashTree:       return Cmd::mContextMenuStashTree;
    case VariousListIndex::MenuFindTextTree:    return Cmd::mContextMenuFindTextTree;
    case VariousListIndex::MergeTool:
    case VariousListIndex::Icons:
    case VariousListIndex::ExternalIcons:
        break;
    default:
        return Cmd::mToolbars[get_toolbar_index(aIndex)];
    }
    return fDummy;
}

std::uint32_t CustomGitActions::get_toolbar_index(std::uint32_t index) const
{
    index -= VariousListIndex::Toolbar1;
    return (index < Cmd::mToolbars.size()) ? index : 0;
}

std::uint32_t CustomGitActions::getVariousListSize() const
{
    return static_cast<std::uint32_t>(VariousListIndex::Toolbar1 + Cmd::mToolbars.size());
}

QString CustomGitActions::getVariousListHeader(VariousListIndex::e aIndex)
{
    switch (aIndex)
    {
    case VariousListIndex::Icons:               return tr("Icons");
    case VariousListIndex::ExternalIcons:       return tr("External Icons");
    case VariousListIndex::MenuSrcTree:         return tr("Context Menu Source");
    case VariousListIndex::MenuEmptySrcTree:    return tr("Context Menu Empty Source");
    case VariousListIndex::MenuGraphicView:     return tr("Context Menu Graphics View");
    case VariousListIndex::MenuCodeBrowser:     return tr("Context Menu Text View");
    case VariousListIndex::MenuHistoryTree:     return tr("Context Menu History");
    case VariousListIndex::MenuBranchTree:      return tr("Context Menu Branch");
    case VariousListIndex::MenuStashTree:       return tr("Context Menu Stash");
    case VariousListIndex::MenuFindTextTree:    return tr("Context Menu Find Text");
    case VariousListIndex::MergeTool:           return tr("Merge or Diff Tool");
    default: return Cmd::mToolbarNames[get_toolbar_index(aIndex)];
    }
    return "";
}


void CustomGitActions::on_ActionTableListItemChanged ( QStandardItem * item )
{
    if (!mInitialize)
    {
        int fColumn = item->column();
        Cmd::eCmd fCmd = getCommand(item->row());
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

void CustomGitActions::initListIcons(VariousListIndex::e aIndex, QString aPath)
{
    mInitialize = true;
    mListModelVarious->setHeaderData(VariousHeader::Icon, Qt::Horizontal, getVariousListHeader(aIndex));
    mListModelVarious->setHeaderData(VariousHeader::Name, Qt::Horizontal, tr("Name"));
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());

    if (aPath.size() != 0)
    {
        QDir fResources(aPath);
        QStringList fList = fResources.entryList();

        auto remove_item = [&fList](const QString& item)
        {
            auto found_item = std::find_if(fList.begin(), fList.end(), [&item] (auto& entry) { return entry == item; });
            if (found_item != fList.end())
            {
                fList.removeAt(std::distance(fList.begin(), found_item));
            }
        };

        remove_item(".");
        remove_item("..");

        int fRow = 0;
        for (const auto& fItem : fList)
        {
            mListModelVarious->insertRows(fRow, 1, QModelIndex());
            mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Icon, QModelIndex()), QIcon(aPath + fItem), Qt::DecorationRole);
            mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Icon, QModelIndex()), QVariant(aPath + fItem), Qt::UserRole);
            mListModelVarious->setData(mListModelVarious->index(fRow, VariousHeader::Name, QModelIndex()), fItem, Qt::EditRole);
            ++fRow;
        }
    }
    else
    {
        mListModelVarious->insertRows(0, 1, QModelIndex());
        mListModelVarious->setData(mListModelVarious->index(0, VariousHeader::Name, QModelIndex()), tr("< empty >"), Qt::EditRole);
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

git::Cmd::eCmd CustomGitActions::getCommand(int aRow)
{
    auto text = mListModelActions->data(mListModelActions->index(aRow, ActionsTable::ID)).toString();
    return static_cast<Cmd::eCmd>(text.split(' ')[0].toInt());
}

void CustomGitActions::on_btnToLeft_clicked()
{
    if (VariousListIndex::isIcon(ui->comboBoxVarious->currentIndex()))
    {
        int icon_row   = ui->tableViewVarious->currentIndex().row();
        int action_row = ui->tableViewActions->currentIndex().row();
        Cmd::eCmd cmd = getCommand(action_row);
        QString icon_path = mListModelVarious->data(mListModelVarious->index(icon_row, VariousHeader::Icon), Qt::UserRole).toString();
        if (icon_path.size())
        {
            mListModelActions->setData(mListModelActions->index(action_row, ActionsTable::Icon), QIcon(icon_path), Qt::DecorationRole);
            mActionList.setIconPath(cmd, icon_path);
            mActionList.setFlags(cmd, ActionList::Flags::Modified, Flag::set);
        }
    }
    else
    {
        int icon_row     = ui->tableViewVarious->currentIndex().row();
        auto& cmd_vector = getCmdVector(VariousListIndex::cast(ui->comboBoxVarious->currentIndex()));
        Cmd::eCmd cmd = cmd_vector[icon_row];
        if (cmd >= Cmd::Submenu)
        {
            mActionList.deleteAction(cmd);
        }
        cmd_vector.erase(cmd_vector.begin()+icon_row);
        int selected = static_cast<int>(cmd_vector.size()-1);
        on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
        ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(selected, ActionsTable::ID), QItemSelectionModel::Select);
        on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
    }
}

void CustomGitActions::on_btnToRight_clicked()
{
    int action_row = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd cmd = getCommand(action_row);
    if (cmd == Cmd::Submenu)
    {
        cmd = mActionList.createNewID(static_cast<Cmd::eCmd>(cmd + 1));
        QString name = mListModelActions->data(mListModelActions->index(action_row, ActionsTable::Name)).toString();
        mActionList.createAction(cmd, name, name);
        mActionList.setFlags(cmd, ActionList::Flags::Modified, Flag::set);
    }
    auto& cmd_vector = getCmdVector(VariousListIndex::cast(ui->comboBoxVarious->currentIndex()));
    cmd_vector.push_back(cmd);
    int selected = static_cast<int>(cmd_vector.size()-1);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(selected, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnMoveUp_clicked()
{
    int fRow      = ui->tableViewVarious->currentIndex().row();
    int fMovedRow = fRow-1;
    auto& fCmdVector = getCmdVector(VariousListIndex::cast(ui->comboBoxVarious->currentIndex()));
    std::swap(fCmdVector[fRow], fCmdVector[fMovedRow]);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fMovedRow, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnMoveDown_clicked()
{
    int fRow      = ui->tableViewVarious->currentIndex().row();
    int fMovedRow = fRow+1;
    auto& fCmdVector = getCmdVector(VariousListIndex::cast(ui->comboBoxVarious->currentIndex()));
    std::swap(fCmdVector[fRow], fCmdVector[fMovedRow]);
    on_comboBoxVarious_currentIndexChanged(ui->comboBoxVarious->currentIndex());
    ui->tableViewVarious->selectionModel()->setCurrentIndex(mListModelVarious->index(fMovedRow, 0), QItemSelectionModel::Select);
    on_tableViewVarious_clicked(ui->tableViewVarious->selectionModel()->currentIndex());
}

void CustomGitActions::on_btnAdd_clicked()
{
    add_command();
}

void CustomGitActions::add_command(const QString& command)
{
    auto  cmd    = mActionList.createNewID(Cmd::CustomCommand);
    auto* action = mActionList.createAction(cmd, tr("command name"), command);
    if (action)
    {
        int row = -1;
        mActionList.setIconPath(cmd, "://resource/24X24/window-close.png");
        insertCmdAction(*mActionList.getList().find(cmd), row);
        if (row != -1)
        {
            --row; // decrease, it was increased in insertCmdAction
            ui->tableViewActions->selectRow(row);
            const auto index = mListModelActions->index(row, ActionsTable::ID);
            ui->tableViewActions->scrollTo(index);
        }
        mActionList.setFlags(cmd, ActionList::Flags::Custom);
        Q_EMIT initCustomAction(action);
    }
}

void CustomGitActions::on_btnDelete_clicked()
{
    int fRow = ui->tableViewActions->currentIndex().row();
    Cmd::eCmd fCmd = getCommand(fRow);
    if (fCmd != Cmd::Separator && !(mActionList.getFlags(fCmd) & ActionList::Flags::BuiltIn))
    {
        mListModelActions->removeRow(fRow);
        mActionList.deleteAction(fCmd);
        for (uint i = VariousListIndex::FirstCmds; i < getVariousListSize(); ++i)
        {
            auto& fVector = getCmdVector(VariousListIndex::cast(i));
            auto fFound = std::find_if(fVector.begin(), fVector.end(), [fCmd](Cmd::eCmd fI) {return fI == fCmd;});
            if (fFound != fVector.end() )
            {
                if (i == static_cast<uint>(ui->comboBoxVarious->currentIndex()))
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
    const int row = ui->tableViewActions->currentIndex().row();
    const Cmd::eCmd cmd   = getCommand(row);
    if (mActionList.hasAction(cmd) || cmd == Cmd::Separator || cmd == Cmd::Submenu)
    {
        const uint button = mActionList.getFlags(cmd) & ActionList::Flags::Custom ? Btn::Delete : 0;

        if (VariousListIndex::isIcon(ui->comboBoxVarious->currentIndex()))
        {
            enableButtons(Btn::Add|button);
        }
        else
        {
            enableButtons(Btn::Add|button|Btn::Right);
        }
    }
        mSearchRowStart = row;
}

void CustomGitActions::tableViewActions_header_clicked(int index )
{
    mSearchColumn = index;
    switch (mSearchColumn)
    {
    case ActionsTable::Name: case ActionsTable::Command: case ActionsTable::MsgBoxText: case ActionsTable::Shortcut:
        break;
    default:
    {
        mSearchColumn = ActionsTable::Name;
        //ui->tableViewActions->horizontalHeader()->seindex.siblingAtColumn(mSearchColumn));
    }   break;
    }
}

void CustomGitActions::on_tableViewVarious_clicked(const QModelIndex &index)
{
    ui->btnAdd->setEnabled(false);
    if (VariousListIndex::isIcon(ui->comboBoxVarious->currentIndex()))
    {
        enableButtons(Btn::Left|(ui->comboBoxVarious->currentIndex() == VariousListIndex::ExternalIcons ? Btn::Load : 0));
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
    ui->btnAdd->setEnabled(      (aBtnFlag&Btn::Add)    != 0);
    ui->btnDelete->setEnabled(   (aBtnFlag&Btn::Delete) != 0);
    ui->btnMoveUp->setEnabled(   (aBtnFlag&Btn::Up)     != 0);
    ui->btnMoveDown->setEnabled( (aBtnFlag&Btn::Down)   != 0);
    ui->btnToRight->setEnabled(  (aBtnFlag&Btn::Right)  != 0);
    ui->btnToLeft->setEnabled(   (aBtnFlag&Btn::Left)   != 0);
    ui->btnLoadIcons->setEnabled((aBtnFlag&Btn::Load)   != 0);
}
namespace
{
    struct s_post_actions
    {
        s_post_actions(Cmd::ePostAction pa, const QString& tt) : post_action(pa), tooltip(tt), action(nullptr) {}
        Cmd::ePostAction post_action;
        QString tooltip;
        QAction* action;
    };

    struct s_action_commands
    {
        s_action_commands(ActionList::Flags::e f, const QString& n, const QString& tt) : flag(f), name(n), tooltip(tt), action(nullptr) {}
        ActionList::Flags::e flag;
        QString name;
        QString tooltip;
        QAction* action;
    };
}

void CustomGitActions::on_tableViewActions_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);

    const int row = ui->tableViewActions->currentIndex().row();
    const Cmd::eCmd cmd   = getCommand(row);
    if (!mActionList.hasAction(cmd)) return;
    const uint flags      = mActionList.getFlags(cmd);

    menu.addAction(mActionList.getAction(cmd));
    menu.addSeparator();

    const int post_action = mActionList.getCustomCommandPostAction(cmd);
    QActionGroup post_action_group(this);
    post_action_group.setExclusive(true);   // radio box

    const bool variable_cmd = (flags & ActionList::Flags::NotVariableGitCmd) == 0;
    const bool function_cmd = (flags & ActionList::Flags::FunctionCmd);

    QList<s_post_actions> post_action_parts
    {
        {Cmd::DoNothing           , tr("Do nothing after command execution")},
        {Cmd::UpdateItemStatus    , tr("Update item status after command execution")},
        {Cmd::ParseHistoryText    , tr("Parse command result for history view")},
        {Cmd::ParseBranchListText , tr("Parse command result for branch view")},
        {Cmd::ParseStashListText  , tr("Parse command result for stash view")},
        {Cmd::ParseBlameText      , tr("Parse command result for git blame")},
        {Cmd::UpdateRootItemStatus, tr("Update root item status after command execution")},
        {Cmd::UpdateRepository    , tr("Update repository after command execution")}
    };

    for (auto& part : post_action_parts)
    {
        part.action = post_action_group.addAction(Cmd::toString(part.post_action));
        part.action->setEnabled(variable_cmd);
        set_tooltip(part.action, part.tooltip);
        menu.addAction(part.action);
        part.action->setCheckable(true);
    }

    if (isBetween(post_action, 0, post_action_group.actions().size()))
    {
        post_action_group.actions().at(post_action)->setChecked(true);
    }
    std::vector<int> parse_action { Cmd::ParseHistoryText, Cmd::ParseBranchListText};

    const ulong enable_flag  = mActionList.getFlags(cmd, ActionList::Data::StatusFlagEnable);
    const ulong disable_flag = mActionList.getFlags(cmd, ActionList::Data::StatusFlagDisable);

    std::vector<Type::TypeFlags> git_status_array =
    {
        Type::GitAdded, Type::GitModified, Type::GitStaged, Type::GitDeleted,
        Type::GitUnTracked, Type::GitUnmerged, Type::GitLocal, Type::GitRemote,
        Type::Folder, Type::IgnoreTypeStatus
    };

    menu.addSeparator();

    QActionGroup git_status_enable_group(this);
    git_status_enable_group.setExclusive(false);    // check box

    QActionGroup git_status_disable_group(this);
    git_status_disable_group.setExclusive(false);   // check box

    QActionGroup git_status_enable_not_group(this);
    git_status_enable_not_group.setExclusive(false);// check box

    if (! function_cmd)
    {
        QMenu* git_status_enable_menu     = menu.addMenu(tr("Enable for git status set"));
        QMenu* git_status_disable_menu    = menu.addMenu(tr("Disable for git status set"));
        QMenu* git_status_enable_not_menu = menu.addMenu(tr("Enable for git status not set"));

        for (auto git_status : git_status_array)
        {
            const bool enabled  = enable_flag  & git_status;
            const bool disabled = disable_flag & git_status;
            if (git_status == Type::None)
            {
                git_status_enable_menu->addSeparator();
                git_status_disable_menu->addSeparator();
                git_status_enable_not_menu->addSeparator();
                continue;
            }
            QAction* action = git_status_enable_group.addAction(Type::name(git_status));
            git_status_enable_menu->addAction(action);
            action->setCheckable(true);
            action->setChecked(enabled && !disabled);
            if (git_status == Type::IgnoreTypeStatus)
            {
                set_tooltip(action, tr("Ignore git file type status"));
                continue;
            }
            else
            {
                set_tooltip(action, tr("Enable command if this git status is set"));
            }
            action = git_status_disable_group.addAction(Type::name(git_status));
            git_status_disable_menu->addAction(action);
            action->setCheckable(true);
            action->setChecked(!enabled && disabled);
            set_tooltip(action, tr("Disable command if this git status is set"));

            action = git_status_enable_not_group.addAction(Type::name(git_status));
            git_status_enable_not_menu->addAction(action);
            action->setCheckable(true);
            action->setChecked(enabled && disabled);
            set_tooltip(action, tr("Enable command if this git status is not set"));
        }
    }
    menu.addSeparator();

    QAction* action_modified = nullptr;
    if (flags & ActionList::Flags::BuiltIn && flags & ActionList::Flags::Modified)
    {
         action_modified = menu.addAction(tr("Reset modifications"));
         set_tooltip(action_modified, tr("After program restart all modifications of this command are removed"));
    }

    QList<s_action_commands> action_commands =
    {
        {ActionList::Flags::Branch       , tr("Branch command")           , tr("The git command is called in branch view context") },
        {ActionList::Flags::History      , tr("History command")          , tr("The git command is called in history view context")},
        {ActionList::Flags::Stash        , tr("Stash command")            , tr("The git command is called in stash view context")},
        {ActionList::Flags::CallInThread , tr("Invoke command unattached"), tr("The git command is called without blocking the program")},
        {ActionList::Flags::Asynchroneous, tr("Unattached instant output"), tr("The git command is called without blocking the program with instant output")},
    #ifndef __linux__
        {ActionList::Flags::NoHide       , tr("Dont hide command window") , tr("The command window is not hidden to allow password input")},
    #endif
    };

    bool custom_enabled = (flags & ActionList::Flags::Custom) || mExperimental;
    for (auto& command : action_commands )
    {
        command.action = menu.addAction(command.name);
        command.action->setCheckable(true);
        command.action->setChecked(flags & command.flag);
        command.action->setEnabled(custom_enabled);
        set_tooltip(command.action, command.tooltip);
    }

    menu.addSeparator();
    if (function_cmd && !mExperimental)
    {
        for (const auto&part : post_action_parts)
        {
            menu.removeAction(part.action);
        }
        post_action_parts.clear();
        for (const auto& command : action_commands )
        {
            menu.removeAction(command.action);
        }
        action_commands.clear();
    }
    /// NOTE: a parsed command cannot be processed asynchronously
    if (std::find(parse_action.begin(), parse_action.end(), post_action) != parse_action.end())
    {
        auto cmd = std::find_if(action_commands.begin(), action_commands.end(), [] (auto& ac) { return  ac.flag == ActionList::Flags::CallInThread; });
        if (cmd != action_commands.end())
        {
            menu.removeAction(cmd->action);
        }
    }

    QAction* edit_menu_string_list = nullptr;
    if (!function_cmd)
    {
        edit_menu_string_list = menu.addAction(tr("Edit toolbar button menu"));
        set_tooltip(edit_menu_string_list, tr("Edit or create toolbar button menu list for custom command"));
    }

    set_tooltip(menu.addAction(tr("Cancel")), tr("Don't change command"));

    QAction* selected_item = menu.exec(mapToGlobal(pos) + menu_offset);
    if (selected_item)
    {
        bool modified = false;
        int index =  post_action_group.actions().indexOf(selected_item);
        if (index != -1)
        {
            mActionList.setCustomCommandPostAction(cmd, index);
            modified = true;
        }
        index = git_status_enable_group.actions().indexOf(selected_item);
        if (index != -1)
        {
            mActionList.setFlags(cmd, git_status_array[index], selected_item->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagEnable);
            modified = true;
        }
        index = git_status_disable_group.actions().indexOf(selected_item);
        if (index != -1)
        {
            mActionList.setFlags(cmd, git_status_array[index], selected_item->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagDisable);
            modified = true;
        }
        index = git_status_enable_not_group.actions().indexOf(selected_item);
        if (index != -1)
        {
            mActionList.setFlags(cmd, git_status_array[index], selected_item->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagEnable);
            mActionList.setFlags(cmd, git_status_array[index], selected_item->isChecked() ? Flag::set : Flag::remove, ActionList::Data::StatusFlagDisable);
            modified = true;
        }

        if (action_modified == selected_item)
        {
            mActionList.setFlags(cmd, ActionList::Flags::Modified, Flag::remove);
        }
        for (const auto& command : action_commands )
        {
            if (command.action == selected_item)
            {
                mActionList.setFlags(cmd, command.flag, command.action->isChecked() ? Flag::set : Flag::remove);
                modified = true;
            }
        }

        if (edit_menu_string_list == selected_item)
        {
            bool ok;
            QStringList list = mActionList.getMenuStringList(cmd);
            QString text = QInputDialog::getText(0, tr("Edit menu for toolbar button"),
                tr("Enter comma separated list for command option menu<br>"
                   "(--option, o1,o2[,o3,..])<br>"
                   "Mark default option with * like this: *o1"), QLineEdit::Normal, list.join(","), &ok);
            if (ok)
            {
                if (text.size()) mActionList.setMenuStringList(cmd, text.split(","));
                else             mActionList.setMenuStringList(cmd, {});
                modified = true;
            }
        }

        if (modified)
        {
            mActionList.setFlags(cmd, ActionList::Flags::Modified, Flag::set);
        }
    }
}

QAction* CustomGitActions::set_tooltip(QAction* action, const QString& tool_tip)
{
    action->setParent(this->parent());
    action->setStatusTip(tool_tip);
    return action;
}

ActionItemModel::ActionItemModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows, columns, parent)
{
}

Qt::ItemFlags ActionItemModel::flags(const QModelIndex &aIndex) const
{
    Qt::ItemFlags fFlags = QStandardItemModel::flags(aIndex);
    auto text = data(index(aIndex.row(), ActionsTable::ID)).toString();
    Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(text.split(' ')[0].toInt());

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

void CustomGitActions::on_btnLoadIcons_clicked()
{
    const QString fSourcePath = QFileDialog::getExistingDirectory(this, tr("Select Icon Files"), mExternalIconFiles);
    if (fSourcePath.size() > 1)
    {
        mExternalIconFiles = fSourcePath + '/';
        initListIcons(VariousListIndex::ExternalIcons, mExternalIconFiles);
    }
}

void CustomGitActions::on_btnHelp_clicked()
{
    QWhatsThis::enterWhatsThisMode();
}

void CustomGitActions::on_tableViewVarious_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);

    const QAction* create_toolbar = set_tooltip(menu.addAction(tr("Create Toolbar...")), tr("Create a custom toolbar"));
    QAction* delete_toolbar {nullptr};
    const int combo_index = ui->comboBoxVarious->currentIndex();
    if (VariousListIndex::isCustomToolbar(combo_index))
    {
        delete_toolbar = menu.addAction(tr("Delete %1").arg(getVariousListHeader(VariousListIndex::cast(combo_index))));
        set_tooltip(delete_toolbar, tr("Delete selected custom toolbar"));
    }
    menu.addSeparator();
    set_tooltip(menu.addAction(tr("Cancel")), tr("No Action"));

    const QAction* selected_item = menu.exec(ui->tableViewVarious->mapToGlobal(pos) + menu_offset);
    if (selected_item == create_toolbar)
    {
        bool ok {false};
        const QString toolbar_name = QInputDialog::getText(0, create_toolbar->toolTip(),
            tr("Enter name for a custom toolbar"), QLineEdit::Normal, tr("Toolbar Custom"), &ok);
        if (ok)
        {
            Cmd::mToolbars.push_back({Cmd::Separator});
            Cmd::mToolbarNames.push_back(toolbar_name);
            ui->comboBoxVarious->addItem(toolbar_name);
            ui->comboBoxVarious->setCurrentIndex(ui->comboBoxVarious->count() - 1);
        }
    }

    if (selected_item == delete_toolbar && VariousListIndex::isCustomToolbar(combo_index))
    {
        int index = get_toolbar_index(combo_index);
        ui->comboBoxVarious->removeItem(combo_index);
        ui->comboBoxVarious->setCurrentIndex(ui->comboBoxVarious->count() - 1);
        Q_EMIT removeCustomToolBar(Cmd::mToolbarNames[index]);
        Cmd::mToolbars.erase(Cmd::mToolbars.begin()+index);
        Cmd::mToolbarNames.erase(Cmd::mToolbarNames.begin()+index);
    }
}

void CustomGitActions::on_btnExecute_clicked()
{
    QAction* action = mActionList.getAction(Cmd::CustomTestCommand);
    action->setStatusTip(ui->edtCommand->text());
    action->trigger();
}

void CustomGitActions::on_btnAddCommand_clicked()
{
    add_command(ui->edtCommand->text());
}

void CustomGitActions::on_btnFind_clicked()
{
    QString text = ui->edtCommand->text();
    const int rows = mListModelVarious->rowCount();
    const int start = mSearchRowStart != -1 ? mSearchRowStart : 0;
    mSearchRowStart = -1;
    for (int row = start; row < rows; ++row)
    {
        if (mListModelActions->data(mListModelActions->index(row, mSearchColumn)).toString().contains(text, Qt::CaseInsensitive))
        {
            ui->tableViewActions->selectRow(row);
            const auto index = mListModelActions->index(row, ActionsTable::ID);
            ui->tableViewActions->scrollTo(index);
            mSearchRowStart = row+1;
            break;
        }
    }
}

