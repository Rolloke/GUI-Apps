#include "customgitactions.h"
#include "ui_customgitactions.h"
#include "actions.h"
#include <QStandardItemModel>
#include <QDir>

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


    QStringList      fColumnName  = { tr("ID"), tr("Command"), tr("Name"), tr("Shortcut"), tr("Message box text"), tr("PA"), tr("Icon")};
    std::vector<int> fColumnWidth = {     42  ,       140    ,     140   ,        65     ,          140          ,    42   ,      42   };

    mListModelActions = new QStandardItemModel(0, INT(ActionsTable::Last), this);
    connect(mListModelActions, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_ActionTableListItemChanged(QStandardItem*)));

    ui->tableViewActions->setModel(mListModelActions);

    for (int fColumn = 0; fColumn<INT(ActionsTable::Last); ++fColumn)
    {
        mListModelActions->setHeaderData(fColumn, Qt::Horizontal, fColumnName[fColumn]);
        ui->tableViewActions->setColumnWidth(fColumn            , fColumnWidth[fColumn]);
    }


    mInitialize = true;
    int fRow = 0;
    for (auto fItem : mActionList.getList())
    {
        if (   fItem.first >= Cmd::FirstGitCommand
            && fItem.first <= Cmd::LastGitCommand)
        {
            const QAction* fAction = fItem.second;
            QString fCommand = fAction->statusTip();
            if (fCommand.size())
            {
                mListModelActions->insertRows(fRow, 1, QModelIndex());
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::ID)), fItem.first, Qt::EditRole);
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::Command)), fCommand, Qt::EditRole);
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::Name)), fAction->text(), Qt::EditRole);
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::Shortcut)), fAction->shortcut().toString(), Qt::EditRole);
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::MsgBoxText)), mActionList.getCustomCommandMessageBoxText(static_cast<Cmd::eCmd>(fItem.first)), Qt::EditRole);
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::PostAction)), mActionList.getCustomCommandPostAction(static_cast<Cmd::eCmd>(fItem.first)), Qt::EditRole);
                mListModelActions->setData(mListModelActions->index(fRow, INT(ActionsTable::Icon)), QIcon(mActionList.getIconPath(static_cast<Cmd::eCmd>(fItem.first))), Qt::DecorationRole);
            }
            ++fRow;
        }
    }
    mInitialize = false;

    mListModelVarious = new QStandardItemModel(0, 1, this);
    ui->tableViewVarious->setModel(mListModelVarious);

    initListIcons();
}

CustomGitActions::~CustomGitActions()
{
    delete ui;
}


void CustomGitActions::on_comboBoxVarious_currentIndexChanged(int index)
{
    switch (static_cast<VariousIndex>(index))
    {
        case VariousIndex::Icons:
            initListIcons();
            break;
        case VariousIndex::MenuSrcTree:
            initMenuList(Cmd::mContextMenuSourceTree, tr("Context Menu Source"));
            break;
        case VariousIndex::MenuEmptySrcTree:
            initMenuList(Cmd::mContextMenuEmptySourceTree, tr("Context Menu Empty Source"));
            break;
        case VariousIndex::MenuHistoryTree:
            initMenuList(Cmd::mContextMenuHistoryTree, tr("Context Menu History"));
            break;
        case VariousIndex::Toolbar1:
            initMenuList(Cmd::mToolbars[0], tr("Toolbar 1"));
            break;
        case VariousIndex::Toolbar2:
            initMenuList(Cmd::mToolbars[1], tr("Toolbar 2"));
            break;
        default:
            break;
    }
}

void CustomGitActions::on_ActionTableListItemChanged ( QStandardItem * item )
{
    if (!mInitialize)
    {
//        switch (item->column())
//        {
//            case Command:
//                if (item->row() < static_cast<int>(mChannelParameter.size()))
//                {
//                    mChannelParameter[item->row()].mName = item->text();
//                }
//                break;
//            case eProbe:
//            {
//                double fD1, fD2;
//                sscanf(item->text().toStdString().c_str(), "%lf : %lf", &fD1, &fD2);
//                mChannelParameter[item->row()].setAudioScale() = fD1 / fD2;
//            }   break;
//        }
    }
}

void CustomGitActions::initListIcons()
{
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
        ++fRow;
    }
}

void CustomGitActions::initMenuList(const std::vector<git::Cmd::eCmd>& aItems, const QString& aHeader)
{
    mListModelVarious->setHeaderData(0, Qt::Horizontal, aHeader);
    mListModelVarious->removeRows(0, mListModelVarious->rowCount());

    int fRow = 0;
    for (auto fCmd: aItems)
    {
        const QAction* fAction = mActionList.getAction(fCmd);

        mListModelVarious->insertRows(fRow, 1, QModelIndex());
        if (fAction)
        {
            mListModelVarious->setData(mListModelVarious->index(fRow, 0, QModelIndex()), fAction->text(), Qt::EditRole);
        }
        else
        {
            mListModelVarious->setData(mListModelVarious->index(fRow, 0, QModelIndex()), tr("-- Separator --"), Qt::EditRole);
        }
        ++fRow;
    }
}

void CustomGitActions::on_btnToLeft_clicked()
{

}

void CustomGitActions::on_btnToRight_clicked()
{

}

void CustomGitActions::on_btnMoveUp_clicked()
{

}

void CustomGitActions::on_btnMoveDown_clicked()
{

}

void CustomGitActions::on_btnAdd_clicked()
{

}

void CustomGitActions::on_btnDelete_clicked()
{

}
