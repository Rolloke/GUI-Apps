#include "qbranchtreewidget.h"
#include "git_type.h"
#include "actions.h"
#include <QMenu>

using namespace git;

QBranchTreeWidget::QBranchTreeWidget(QWidget *parent) : QTreeWidget(parent)
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
    // TODO: list branch, stash or tag
    setVisible(true);
    int fTLI = -1;
    for (auto fLine : fLines)
    {
        if (fTLI == -1)
        {
            QTreeWidgetItem* fNewBranchItem = new QTreeWidgetItem(QStringList(fLine));
            addTopLevelItem(fNewBranchItem);
            fTLI = topLevelItemCount()-1;
        }
        else
        {
            QTreeWidgetItem* fNewBranchItem = new QTreeWidgetItem();
            topLevelItem(fTLI)->addChild(fNewBranchItem);
            fNewBranchItem->setText(INT(Column::Text), fLine);
        }
    }
}

void QBranchTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    QTreeWidgetItem* fSelectedBranchItem = itemAt(pos);
    if (fSelectedBranchItem)
    {
        if (indexOfTopLevelItem(fSelectedBranchItem) != -1)
        {

        }
        else
        {
            mBranchItem = fSelectedBranchItem->text(INT(Column::Text));
        }
    }

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuBranchTree);
    menu.exec(mapToGlobal(pos) );
}

QString QBranchTreeWidget::getBranchItem()
{
    return mBranchItem;
}
