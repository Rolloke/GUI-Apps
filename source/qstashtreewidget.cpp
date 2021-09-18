#include "qstashtreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include <QFileInfo>

#include "actions.h"
#include "helper.h"
#include "logger.h"

using namespace git;
// TODO: show stash difference of single files
// > git stash list
// stash@{0}: WIP on master: 924f420 show stashed files in repository tree
// > git show stash@{0}
// commit 14206bae46ae44e641b458435895eb22b2262aad (refs/stash)
// Merge: 924f420 ca100e5
// Author: Rolf (Werkstatt) <rolf-kary-ehlers@t-online.de>
// Date:   Mon Jul 5 21:04:38 2021 +0200
//   WIP on master: 924f420 show stashed files in repository tree
// > git stash show stash@{0}
// Qt/ArduinoUnittest/ArduinoUnittest/mainwindow.cpp  |  32 +-
// > git difftool 14206bae46ae44e641b458435895eb22b2262aad Qt/ArduinoUnittest/ArduinoUnittest/mainwindow.cpp

// TODO: doubleclick on stashed item selects item in repository view

// applyGitCommandToFilePath(source_dir.path(), Cmd::getCommand(Cmd::StashShow), result_string);
// parseGitStash(source_dir.path() +  QDir::separator(), result_string, check_map);

//// TODO: stash
//applyGitCommandToFilePath(source_dir.path(), "git stash list", result_string);

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
            QString git_command = tr("git -C %1 show %2").arg(git_root_path).arg(stash_no);
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
                git_command = tr("git -C %1 stash show %2").arg(git_root_path).arg(stash_no);
                int result = execute(git_command, result_string);
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
            }
        }
    }
    return items_inserted;
}

void QStashTreeWidget::parseGitStash(const QString& aSource, const QString& aStatus, stringt2typemap& aFiles)
{
    const auto lines = aStatus.split("\n");

    for (QString line : lines)
    {
        QString relative_path;
        int position = line.indexOf('|');
        if (position != -1)
        {
            relative_path = line.left(position).trimmed();
        }
        else
        {
            relative_path = line.trimmed();
        }
        QString full_path = aSource + relative_path;
        QFileInfo file_info(full_path);
        if (file_info.exists())
        {
            Type type;
            auto file_path = file_info.filePath().toStdString();
            if (aFiles.count(file_path))
            {
                type = aFiles[file_path];
            }
            else
            {
                type.translate(file_info);
            }
            type.add(Type::GitStashed);
            aFiles[file_path] = type;
        }
    }
}

void QStashTreeWidget::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    mSelectedItem = itemAt(pos);

    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuStashTree);
    menu.exec(mapToGlobal(pos) );
}

void QStashTreeWidget::on_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem * /* previous */)
{
    mSelectedItem = current;
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


