#include "qstashtreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include <QFileInfo>

#include "actions.h"
#include "helper.h"
#include "logger.h"

using namespace git;
/// TODO: update tree after stash command

QStashTreeWidget::QStashTreeWidget(QWidget *parent): QTreeWidget(parent)
{
    setVisible(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
}


void QStashTreeWidget::clear()
{
    QTreeWidget::clear();
}


bool QStashTreeWidget::parseStashListText(const QString& branch_text, const QString& git_root_path)
{
    if (!mInitialized)
    {
        header()->setSectionResizeMode(Column::Text, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(Column::Description, QHeaderView::Stretch);
        mInitialized = true;
    }

    QStringList result_lines = branch_text.split('\n');
    int line_index = 0;
    bool items_inserted {false};
    for (const QString& line : result_lines)
    {
        if (++line_index == 1) continue; // first line contains git command
        auto pos = line.indexOf("stash");
        if (pos != -1)
        {
            QTreeWidgetItem* new_tree_root_item = nullptr;
            QString stash_no = line.split(':')[0];
            QString git_command = tr("git -C %1 show %2").arg(git_root_path, stash_no);
            QString result_string;
            int result = execute(git_command, result_string);
            if (result != -1)
            {
                auto shown_stash = result_string.split('\n');
                auto commit = shown_stash[0].split(' ');
                if (commit[0] == "commit")
                {
                    new_tree_root_item = new QTreeWidgetItem({stash_no});
                    addTopLevelItem(new_tree_root_item);
                    new_tree_root_item->setData(Column::Description, Role::GitRootPath, QVariant(git_root_path));
                    new_tree_root_item->setData(Column::Description, Role::Commit, QVariant(commit[1]));
                }
            }
            if (new_tree_root_item)
            {
                items_inserted = true;
                git_command = tr("git -C %1 stash show %2").arg(git_root_path, stash_no);
                result = execute(git_command, result_string);
                if (result != -1)
                {
                    auto shown_items = result_string.split('\n');
                    for (const QString& item : shown_items)
                    {
                        QStringList item_parts = item.split('|');
                        if (item_parts.size() >= 2)
                        {
                            QTreeWidgetItem* new_child_item = new QTreeWidgetItem();
                            new_tree_root_item->addChild(new_child_item);
                            new_child_item->setText(Column::Text, item_parts[0].trimmed());
                            new_child_item->setText(Column::Description, item_parts[1].trimmed());
                        }
                    }
                }
                expandItem(new_tree_root_item);
            }
        }
    }
    return items_inserted;
}

void QStashTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    mSelectedItem = itemAt(pos);

    aActionList.getAction(Cmd::StashClear)->setEnabled(getItemLevel(mSelectedItem) == 0);
    aActionList.getAction(Cmd::StashDrop)->setEnabled(getItemLevel(mSelectedItem) == 0);
    aActionList.getAction(Cmd::StashApply)->setEnabled(getItemLevel(mSelectedItem) == 0);
    aActionList.getAction(Cmd::StashPop)->setEnabled(getItemLevel(mSelectedItem) == 0);
    aActionList.getAction(Cmd::CallDiffTool)->setEnabled(getItemLevel(mSelectedItem) == 1);
    aActionList.getAction(Cmd::ShowDifference)->setEnabled(getItemLevel(mSelectedItem) == 1);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuStashTree);
    menu.exec(mapToGlobal(pos));
}

void QStashTreeWidget::on_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem * /* previous */)
{
    mSelectedItem = current;
}

void QStashTreeWidget::on_itemDoubleClicked(QTreeWidgetItem *item, int /* column */)
{
    mSelectedItem = item;
    emit find_item_in_treeSource(getStashTopItemText(Role::GitRootPath), getSelectedStashItem());
}


QString QStashTreeWidget::getSelectedStashItem()
{
    return (mSelectedItem && indexOfTopLevelItem(mSelectedItem) == -1) ? mSelectedItem->text(Column::Text) : "";
}

QString QStashTreeWidget::getStashTopItemText(QStashTreeWidget::Role::e role)
{
    QTreeWidgetItem* fItem = getTopLevelItem(*this, mSelectedItem);
    if (fItem)
    {
        switch (role)
        {
        case Role::Text:
            return fItem->text(Column::Text);
        case Role::Commit: case Role::GitRootPath:
            return fItem->data(Column::Description, role).toString();
        default:
            return "";
        }
    }
    return  "";
}


