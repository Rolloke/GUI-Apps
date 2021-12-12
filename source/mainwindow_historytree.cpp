#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "history.h"

#include <QMenu>
#include <QMessageBox>

using namespace std;
using namespace git;

void MainWindow::on_treeHistory_customContextMenuRequested(const QPoint &pos)
{
    auto * old_item = mContextMenuSourceTreeItem;
    ui->treeHistory->customContextMenuRequested(pos, mActions, &mContextMenuSourceTreeItem);
    mContextMenuSourceTreeItem = old_item;
}

void MainWindow::call_git_history_diff_command()
{
    QString history_hash_items        = ui->treeHistory->getSelectedHistoryHashItems();
    const QString &history_file       = ui->treeHistory->getSelectedHistoryFile();
    const QAction *action             = qobject_cast<QAction *>(sender());
    const Type    type(ui->treeHistory->getSelectedTopLevelType());
    const QVariantList variant_list   = action->data().toList();
    const uint    command_flags       = variant_list[ActionList::Data::Flags].toUInt();

    int result = callMessageBox(variant_list[ActionList::Data::MsgBoxText].toString(), tr("history"), history_file);
    if (result == QMessageBox::Yes || result == QMessageBox::YesToAll)
    {
        QString path;
        if (type.is(Type::Branch) && mContextMenuSourceTreeItem)
        {
            path =  mContextMenuSourceTreeItem->text(0);
            mContextMenuSourceTreeItem = nullptr;
        }
        if (mContextMenuSourceTreeItem)
        {
            QDir::setCurrent(getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(Column::FileName));
            QString tool_cmd = get_git_command_option(type, command_flags, variant_list);
            if (((command_flags & ActionList::Flags::DiffCmd)==0) && history_hash_items.contains(" "))
            {
                history_hash_items = history_hash_items.left(history_hash_items.indexOf(" "));
            }
            QString command = tr(action->statusTip().toStdString().c_str()).arg(history_hash_items + tool_cmd, "-- %1");
            if (history_file.size())
            {
                const QString quoted_history_file = "\"" + history_file + "\"";
                command = tr(command.toStdString().c_str()).arg(quoted_history_file);
                QString result_str;
                int result = execute(command, result_str);
                if (result != NoError)
                {
                    result_str += tr("\nError %1 occurred").arg(result);
                }
                appendTextToBrowser(command + getLineFeed() + result_str);
            }
            else
            {
                applyGitCommandToFileTree(command);
            }
            if (variant_list[ActionList::Data::PostCmdAction].toUInt() == Cmd::UpdateItemStatus)
            {
                updateTreeItemStatus(mContextMenuSourceTreeItem);
            }
        }
        else
        {
            QString command;
            QString format_string = action->statusTip();
            if (path.size())
            {
                format_string.replace(QString("git "), tr("git -C %1 ").arg(path));
            }

            if (history_file.size())
            {
                command = tr(format_string.toStdString().c_str()).arg(history_hash_items, history_file);
            }
            else
            {
                command = tr(format_string.toStdString().c_str()).arg(history_hash_items, "");
            }
            QString result_str;
            int result = execute(command, result_str);
            if (result != NoError)
            {
                result_str += tr("\nError %1 occurred").arg(result);
            }
            appendTextToBrowser(command + getLineFeed() + result_str);
        }
    }
}

void MainWindow::on_treeHistory_itemClicked(QTreeWidgetItem *aItem, int aColumn)
{
    on_btnCloseText_clicked();
    mHighlighter->setExtension("");
    appendTextToBrowser(ui->treeHistory->itemClicked(aItem, aColumn));
#ifdef DOCKED_VIEWS
    showDockedWidget(ui->textBrowser);
#endif
    const auto fItemData = ui->treeHistory->determineHistoryHashItems(ui->treeHistory->currentItem());
    if (fItemData.type() == QVariant::ModelIndex)
    {
        QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
        mContextMenuSourceTreeItem = fSourceHook->itemFromIndex(fItemData.toModelIndex());
    };

    mActions.enableItemsByType(Cmd::mContextMenuHistoryTree, Type::None);
    if (!ui->treeHistory->isSelectionDiffable())
    {
        mActions.getAction(Cmd::CallDiffTool)->setEnabled(false);
        mActions.getAction(Cmd::ShowDifference)->setEnabled(false);
    }
    if (!ui->treeHistory->isSelectionFileDiffable())
    {
        mActions.getAction(Cmd::CallDiffTool)->setEnabled(false);
    }
    if (mHistoryFile.size() == HistoryFile::Size)
    {
        appendTextToBrowser(mHistoryFile[HistoryFile::Content], false, mHistoryFile[HistoryFile::Extension]);
        ui->labelFilePath->setText(mHistoryFile[HistoryFile::FilePath]);
        ui->btnStoreText->setEnabled(false);
    }
    mHistoryFile.clear();
}

void MainWindow::on_treeHistory_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (   column == History::Column::Filename
        && getItemLevel(item) == QHistoryTreeWidget::Level::File)
    {
        QString history_hash_items        = ui->treeHistory->getSelectedHistoryHashItems();
        const QString &history_file       = ui->treeHistory->getSelectedHistoryFile();
        if (history_hash_items.contains(" "))
        {
            history_hash_items = history_hash_items.mid(history_hash_items.indexOf(" "));
        }
        QString command = tr("git show %1:%2").arg(history_hash_items, history_file);
        mHistoryFile.append("");
        int result = execute(command, mHistoryFile[HistoryFile::Content]);
        if (result == NoError)
        {
            int pos = history_file.lastIndexOf('.');
            if (pos != -1)
            {
                mHistoryFile.append(history_file.mid(pos+1));
            }
            QString root_path;
            if (mContextMenuSourceTreeItem)
            {
                root_path = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(Column::FileName);
            }
            mHistoryFile.append(root_path + "/" + history_file);
        }
        else
        {
            mHistoryFile.clear();
        }
    }
}
