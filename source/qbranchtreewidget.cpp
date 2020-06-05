#include "qbranchtreewidget.h"
#include "actions.h"
#include "helper.h"

#include <QMenu>

using namespace git;

// TODO: diffs between branches and their files

QBranchTreeWidget::QBranchTreeWidget(QWidget *parent) : QTreeWidget(parent)
, mSelectedItem(nullptr)
{
    setVisible(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

QBranchTreeWidget::~QBranchTreeWidget()
{

}

void QBranchTreeWidget::parseBranchListText(const QString& aBranchText)
{
    QStringList fLines = aBranchText.split("\n");
    mSelectedItem = nullptr;
    setVisible(true);
    for (const auto& fLine : fLines)
    {
        if (mSelectedItem == nullptr)
        {
            auto fFoundItem = findItems(fLine, Qt::MatchFixedString|Qt::MatchCaseSensitive, Column::Text);
            for (const auto& fItem : fFoundItem)
            {
                auto*fRemoved = takeTopLevelItem(indexOfTopLevelItem(fItem));
                delete fRemoved;
            }
            mSelectedItem = new QTreeWidgetItem(QStringList(fLine));
            addTopLevelItem(mSelectedItem);
        }
        else if (fLine.size())
        {
            QTreeWidgetItem* fNewBranchItem = new QTreeWidgetItem();
            mSelectedItem->addChild(fNewBranchItem);
            fNewBranchItem->setText(Column::Text, fLine);
        }
    }
    if (mSelectedItem)
    {
        expandItem(mSelectedItem);
    }
    mSelectedItem = nullptr;
}

void QBranchTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    mSelectedItem = itemAt(pos);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuBranchTree);
    menu.exec(mapToGlobal(pos) );
}

QString QBranchTreeWidget::getBranchItem()
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
    else
    {
         fItem = (mSelectedItem && indexOfTopLevelItem(mSelectedItem) == -1) ? mSelectedItem->text(Column::Text).remove(0, 2) : "";
    }
    return fItem;
}

QString QBranchTreeWidget::getBranchTopItemText()
{
    QTreeWidgetItem* fItem = getTopLevelItem(*this, mSelectedItem);
    return (fItem) ? fItem->text(Column::Text) : "";
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
