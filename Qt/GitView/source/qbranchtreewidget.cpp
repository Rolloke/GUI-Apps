#include "qbranchtreewidget.h"
#include "git_type.h"
#include "actions.h"
#include <QMenu>

using namespace git;

QBranchTreeWidget::QBranchTreeWidget(QWidget *parent) : QTreeWidget(parent)
, mSelectedBranchItem(nullptr)
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
    mSelectedBranchItem = nullptr;
    setVisible(true);
    for (auto fLine : fLines)
    {
        if (mSelectedBranchItem == nullptr)
        {
            auto fFoundItem = findItems(fLine, Qt::MatchFixedString|Qt::MatchCaseSensitive, INT(Column::Text));
            for (const auto& fItem : fFoundItem)
            {
                auto*fRemoved =takeTopLevelItem(indexOfTopLevelItem(fItem));
                delete fRemoved;
            }
            mSelectedBranchItem = new QTreeWidgetItem(QStringList(fLine));
            addTopLevelItem(mSelectedBranchItem);
        }
        else if (fLine.size())
        {
            QTreeWidgetItem* fNewBranchItem = new QTreeWidgetItem();
            mSelectedBranchItem->addChild(fNewBranchItem);
            fNewBranchItem->setText(INT(Column::Text), fLine);
        }
    }
    if (mSelectedBranchItem)
    {
        expandItem(mSelectedBranchItem);
    }
    mSelectedBranchItem = nullptr;
}

void QBranchTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    mSelectedBranchItem = itemAt(pos);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuBranchTree);
    menu.exec(mapToGlobal(pos) );
}

QString QBranchTreeWidget::getBranchItem()
{
    return (mSelectedBranchItem && indexOfTopLevelItem(mSelectedBranchItem) == -1) ? mSelectedBranchItem->text(INT(Column::Text)) : "";
}

QString QBranchTreeWidget::getBranchTopItem()
{
    QTreeWidgetItem*fItem = mSelectedBranchItem;
    while (fItem && indexOfTopLevelItem(fItem) == -1)
    {
        fItem = fItem->parent();
    }
    return (fItem) ? fItem->text(INT(Column::Text)) : "";
}

void QBranchTreeWidget::deleteSelected()
{
    QTreeWidgetItem*fItem = mSelectedBranchItem;
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
}
