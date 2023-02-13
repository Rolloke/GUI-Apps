#include "qbranchtreewidget.h"
#include "actions.h"
#include "helper.h"
#include "history.h"

#include <QMenu>

using namespace git;

/// FIXME: validate diffs between branches and their files
/// git diff --name-only %1 %2 -> %1 branch 1 commit, %2 branch 2 commit
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
    const QStringList branch_lines = aBranchText.split("\n");
    QTreeWidgetItem* new_branch_item = nullptr;
    setVisible(true);
    for (const QString& branch_line : branch_lines)
    {
        if (new_branch_item == nullptr)
        {
            auto found_item = findItems(branch_line, Qt::MatchFixedString|Qt::MatchCaseSensitive, Column::Text);
            for (const auto& item : found_item)
            {
                auto* removed = takeTopLevelItem(indexOfTopLevelItem(item));
                delete removed;
            }
            new_branch_item = new QTreeWidgetItem(QStringList(branch_line));
            addTopLevelItem(new_branch_item);
            new_branch_item->setData(Column::Text, Role::GitRootPath, QVariant(aGitRootPath));
        }
        else if (branch_line.size())
        {
            QTreeWidgetItem* new_child_item = new QTreeWidgetItem();
            new_branch_item->addChild(new_child_item);
            new_child_item->setText(Column::Text, branch_line);
            QString branch = branch_line;
            branch.replace("* ", "");
            QString command = tr("git -C %1 log %2 --max-count=1 --pretty=format:\"%H<td>%T<td>%P<td>%B<td>%an<td>%ae<td>%ad<td>%cn<td>%ce<td>%cd<tr>\"").arg(aGitRootPath, branch);
            QString result;
            execute(command, result);
            const QStringList result_lines = result.split("<td>");
            int role = 0;
            const int column = History::Column::Commit;
            for (const auto& result_line : result_lines)
            {
                new_child_item->setData(column, role++, QVariant(result_line));
            }
            new_child_item->setData(column, History::role(History::Entry::Type), QVariant(static_cast<uint>(Type::Branch)));
        }
    }
    if (new_branch_item)
    {
        expandItem(new_branch_item);
    }
}

void QBranchTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    mSelectedItem = itemAt(pos);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuBranchTree);
    aActionList.getAction(Cmd::DiffOfTwoBranches)->setEnabled(selectedItems().count() == 2);
    menu.exec(mapToGlobal(pos) + menu_offset);

    mSelectedItem = nullptr;
}

void QBranchTreeWidget::diff_of_two_branches()
{
    const auto selected = selectedItems();
    if (selected.count() == 2)
    {
        QTreeWidgetItem* parent = selected[0]->parent();
        int child1 = parent->indexOfChild(selected[0]);
        int child2 = parent->indexOfChild(selected[1]);
        if (abs(child1 - child2) > 1)
        {
            Q_EMIT insertFileNames(parent, child1, child2);
        }
    }
}

void QBranchTreeWidget::on_itemDoubleClicked(const ActionList& aActionList, QTreeWidgetItem *item, int )
{
    mSelectedItem = item;
    aActionList.getAction(Cmd::BranchCheckout)->trigger();
    mSelectedItem = nullptr;
}

QString QBranchTreeWidget::getSelectedBranch()
{
    QString fItem;
    const auto fSelectedList = selectedItems();
    if (fSelectedList.size() > 1)
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
