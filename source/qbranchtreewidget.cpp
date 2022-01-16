#include "qbranchtreewidget.h"
#include "actions.h"
#include "helper.h"

#include <QMenu>

using namespace git;

/// FIXME: validate diffs between branches and their files
/// TODO: diff branches: git diff branch1...branch2
/// History::parse()

QBranchTreeWidget::QBranchTreeWidget(QWidget *parent) : QTreeWidget(parent)
, mSelectedItem(nullptr)
{
    setVisible(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

QBranchTreeWidget::~QBranchTreeWidget()
{

}

void QBranchTreeWidget::parseBranchListText(const QString& aBranchText, const QString& aGitRootPath)
{
    QStringList fLines = aBranchText.split("\n");
    QTreeWidgetItem* fNewBranchItem = nullptr;
    setVisible(true);
    for (const auto& fLine : fLines)
    {
        if (fNewBranchItem == nullptr)
        {
            auto fFoundItem = findItems(fLine, Qt::MatchFixedString|Qt::MatchCaseSensitive, Column::Text);
            for (const auto& fItem : fFoundItem)
            {
                auto*fRemoved = takeTopLevelItem(indexOfTopLevelItem(fItem));
                delete fRemoved;
            }
            fNewBranchItem = new QTreeWidgetItem(QStringList(fLine));
            addTopLevelItem(fNewBranchItem);
            fNewBranchItem->setData(Column::Text, Role::GitRootPath, QVariant(aGitRootPath));
        }
        else if (fLine.size())
        {
            QTreeWidgetItem* fNewChildItem = new QTreeWidgetItem();
            fNewBranchItem->addChild(fNewChildItem);
            fNewChildItem->setText(Column::Text, fLine);
        }
    }
    if (fNewBranchItem)
    {
        expandItem(fNewBranchItem);
    }
}

void QBranchTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    mSelectedItem = itemAt(pos);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuBranchTree);
    menu.exec(mapToGlobal(pos) );

    mSelectedItem = nullptr;
}

QString QBranchTreeWidget::getSelectedBranch()
{
    QString fItem;
    auto fSelectedList = selectedItems();
    if (fSelectedList.size() == 2)
    {
        for (auto fSelectedItem : fSelectedList)
        {
            fItem += fSelectedItem->text(Column::Text).remove(0, 2);
            fItem += " ";
        }
    }
    else if (mSelectedItem)
    {
         fItem = (indexOfTopLevelItem(mSelectedItem) == -1) ? mSelectedItem->text(Column::Text) : "";
         if (fItem.size() > 2)
         {
             fItem = fItem.remove(0, 2);
         }
    }
    return fItem;
}

QString QBranchTreeWidget::getBranchTopItemText()
{
    QTreeWidgetItem* fItem = getTopLevelItem(*this, mSelectedItem);
    return (fItem) ? fItem->text(Column::Text) : "";
}

QString QBranchTreeWidget::getSelectedBranchGitRootPath()
{
    QTreeWidgetItem* fItem = getTopLevelItem(*this, mSelectedItem);
    return (fItem) ? fItem->data(Column::Text, Role::GitRootPath).toString() : "";
}

void QBranchTreeWidget::deleteSelectedItem()
{
    QTreeWidgetItem*fItem = mSelectedItem;
    while (fItem)
    {
        int fIndex = indexOfTopLevelItem(fItem);
        if (fIndex != -1)
        {
            delete takeTopLevelItem(fIndex);
            break;
        }
        fItem = fItem->parent();
    }
    mSelectedItem = nullptr;
}

void QBranchTreeWidget::clear()
{
    QTreeWidget::clear();
    mSelectedItem = nullptr;
}

QModelIndex QBranchTreeWidget::indexFromSelectedItem() const
{
    return QTreeWidget::indexFromItem(mSelectedItem, Column::Text);
}

QTreeWidgetItem * QBranchTreeWidget::itemFromIndex(const QModelIndex &index) const
{
    return QTreeWidget::itemFromIndex(index);
}
