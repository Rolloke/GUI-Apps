#include "qstashtreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include <QDir>
#include <QFileInfo>

#include "actions.h"
#include "helper.h"
#include "logger.h"

using namespace git;

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

    const QDir root_path(git_root_path);
    const QStringList result_lines = branch_text.split('\n');
    int  line_index = 0;
    bool items_inserted {false};
    for (const QString& line : std::as_const(result_lines))
    {
        if (++line_index == 1) continue; // first line contains git command
        auto pos = line.indexOf("stash");
        if (pos != -1)
        {
            QTreeWidgetItem* new_tree_root_item = nullptr;
            QString stash_no = line.split(':')[0];
            QList<QTreeWidgetItem*> list = findItems(stash_no, Qt::MatchExactly);
            bool already_inserted = false;
            for (QTreeWidgetItem* item :  list)
            {
                if (item->data(Column::Description, Role::GitRootPath).toString() == git_root_path)
                {
                    already_inserted = true;
                }
            }
            if (already_inserted)
            {
                break;
            }
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
                git_command = tr("git -C %1 stash show --name-only %2").arg(git_root_path, stash_no);
                result = execute(git_command, result_string);
                if (result != -1)
                {
                    const auto shown_items = result_string.split('\n');
                    for (const QString& item : std::as_const(shown_items))
                    {
                        QStringList item_parts = item.split('|');
                        if (item_parts.size() > 0 && item_parts[0].size() > 0)
                        {
                            QTreeWidgetItem* new_child_item = new QTreeWidgetItem();
                            new_tree_root_item->addChild(new_child_item);
                            new_child_item->setText(Column::Text, root_path.relativeFilePath(item_parts[0].trimmed()));
                            if (item_parts.size() > 1)
                            {
                                new_child_item->setText(Column::Description, item_parts[1].trimmed());
                            }
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

    auto* action = aActionList.getAction(Cmd::StashClear);
    if (action) action->setEnabled(getItemLevel(mSelectedItem) == 0);
    action = aActionList.getAction(Cmd::StashDrop);
    if (action) action->setEnabled(getItemLevel(mSelectedItem) == 0);
    action = aActionList.getAction(Cmd::StashApply);
    if (action) action->setEnabled(getItemLevel(mSelectedItem) == 0);
    action = aActionList.getAction(Cmd::StashPop);
    if (action) action->setEnabled(getItemLevel(mSelectedItem) == 0);
    action = aActionList.getAction(Cmd::CallDiffTool);
    if (action) action->setEnabled(getItemLevel(mSelectedItem) == 1);
    action = aActionList.getAction(Cmd::ShowDifference);
    if (action) action->setEnabled(getItemLevel(mSelectedItem) == 1);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuStashTree);
    menu.exec(mapToGlobal(pos) + menu_offset);
}

void QStashTreeWidget::on_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem * /* previous */)
{
    mSelectedItem = current;
}

void QStashTreeWidget::on_itemDoubleClicked(QTreeWidgetItem *item, int /* column */)
{
    mSelectedItem = item;
    emit find_item(getStashTopItemText(Role::GitRootPath), getSelectedStashItem());
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


