
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"

#include <QMessageBox>

using namespace std;
using namespace git;



void MainWindow::call_git_stash_command()
{
    const QAction*     action           = qobject_cast<QAction *>(sender());
    const QVariantList variant_list     = action->data().toList();
    const QString&     message_box_text = variant_list[ActionList::Data::MsgBoxText].toString();
    const auto         action_flags     = variant_list[ActionList::Data::Flags].toUInt();
    const auto         post_action_cmd  = variant_list[ActionList::Data::PostCmdAction].toUInt();
    const QString&     stash_item       = ui->treeStash->getSelectedStashItem();
    const QString&     stash            = ui->treeStash->getStashTopItemText(QStashTreeWidget::Role::Text);
    const QString&     stash_root       = ui->treeStash->getStashTopItemText(QStashTreeWidget::Role::GitRootPath);
    const QString&     stash_commit     = ui->treeStash->getStashTopItemText(QStashTreeWidget::Role::Commit);
    const QString&     git_command      = action->statusTip();
    const QString&     top_item_path    = ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem);

    QString argument1 = stash_root;
    QString argument2 = stash;
    QString git_root_path;

    QString option;
    if (post_action_cmd == Cmd::ParseStashListText)
    {
        argument1 = top_item_path;
        argument2 = "";
    }
    else
    {
        option += get_git_command_option(Type(), action_flags, variant_list);
        if (action_flags & ActionList::Flags::DiffCmd)
        {
            argument1     = stash_commit;
            argument2     = option + stash_item;
            git_root_path = stash_root;
        }
    }

    QString result_str;
    int result = callMessageBox(message_box_text, "stash", stash);
    if (result & (QMessageBox::Yes|QMessageBox::YesToAll))
    {
        result = call_git_command(git_command, argument1, argument2, result_str, git_root_path);
    }

    if (result == NoError)
    {
        switch (post_action_cmd)
        {
        case Cmd::ParseStashListText:
            if (ui->treeStash->parseStashListText(result_str, top_item_path))
            {
                showDockedWidget(ui->treeStash);
            }
            break;
        case Cmd::UpdateStash:
        case Cmd::UpdateRootItemStatus:
            if (mActions.findID(action) != git::Cmd::StashList)
            {
                ui->treeStash->clear();
                mActions.getAction(git::Cmd::StashList)->trigger();
            }
            break;
        }
    }
    else if (!isMessageBoxNo(result))
    {
        appendTextToBrowser(git_command + tr("Error %1 occurred").arg(result));
    }
}

void MainWindow::on_treeStash_customContextMenuRequested(const QPoint &pos)
{
    ui->treeStash->on_customContextMenuRequested(mActions, pos);
}

void MainWindow::on_treeStash_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->treeStash->on_currentItemChanged(current, previous);
}

void MainWindow::on_treeStash_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    ui->treeStash->on_itemDoubleClicked(item, column);
}

