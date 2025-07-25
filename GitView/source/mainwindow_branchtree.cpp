#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"

#include <QMessageBox>

using namespace std;
using namespace git;

void MainWindow::call_git_branch_command()
{
    const QAction*     action               = qobject_cast<QAction *>(sender());
    const QVariantList variant_list         = action->data().toList();
    const QString&     message_box_text     = variant_list[ActionList::Data::MsgBoxText].toString();
    const QString&     branch_git_root_path = ui->treeBranches->getSelectedBranchGitRootPath();
    QString            branch_item          = ui->treeBranches->getSelectedBranch();
    QString            git_command          = action->statusTip();
    QString            top_item_path        = branch_git_root_path;
    if (ui->treeSource->hasFocus() && mContextMenuSourceTreeItem)
    {
        top_item_path = ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem);
    }

    int result = callMessageBox(message_box_text, "branch", branch_item);
    QString result_str;
    if (result & (QMessageBox::Yes|QMessageBox::YesToAll))
    {
        result = call_git_command(git_command, top_item_path, branch_item, result_str);
    }

    if (result == NoError)
    {
        switch (variant_list[ActionList::Data::PostCmdAction].toUInt())
        {
            case Cmd::UpdateItemStatus:
            {
                result_str.clear();
                git_command  = ui->treeBranches->getBranchTopItemText();
                top_item_path = ui->treeBranches->getSelectedBranchGitRootPath();
                result = execute(git_command, result_str);
                if (result != NoError)
                {
                    result_str += tr("\nError %1 occurred").arg(result);
                }
                QString parse_text = git_command + "\n" + result_str + "\n";
                ui->treeBranches->parseBranchListText(parse_text, top_item_path);
                ui->textBrowser->setExtension("");
                ui->textBrowser->setPlainText(parse_text);
                showDockedWidget(ui->treeBranches);
            }   break;
            case Cmd::ParseBranchListText:
            {
                ui->treeBranches->parseBranchListText(result_str, top_item_path);
                ui->textBrowser->setExtension("");
                ui->textBrowser->setPlainText(git_command);
                showDockedWidget(ui->treeBranches);
            }    break;
            case Cmd::ParseHistoryText:
            {
                auto items = ui->treeSource->findItems(branch_git_root_path, Qt::MatchExactly);
                QTreeWidgetHook*source_hook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                ui->treeHistory->parseGitLogHistoryText(result_str, items.size() ? QVariant(source_hook->indexFromItem(items[0])) : QVariant(branch_git_root_path), branch_item, Type::Branch);
                ui->textBrowser->setExtension("");
                ui->textBrowser->setPlainText(git_command);
                showDockedWidget(ui->treeHistory);
            }   break;
        }
    }
    else if (!isMessageBoxNo(result))
    {
        appendTextToBrowser(git_command + tr("Error %1 occurred").arg(result));
    }
}

void MainWindow::on_treeBranches_customContextMenuRequested(const QPoint &pos)
{
    ui->treeBranches->on_customContextMenuRequested(mActions, pos);
}

void MainWindow::on_treeBranches_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    ui->treeBranches->on_itemDoubleClicked(mActions, item, column);
}
