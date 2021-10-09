#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"

#include <QMenu>

using namespace std;
using namespace git;

void MainWindow::on_treeHistory_customContextMenuRequested(const QPoint &pos)
{
    const QVariant fItemData = ui->treeHistory->customContextMenuRequested(pos);
    auto * old_item = mContextMenuSourceTreeItem;
    QActionGroup fPostActionGroup(this);
    fPostActionGroup.setExclusive(false);
    if (fItemData.isValid())
    {
        switch (fItemData.type())
        {
            case QVariant::ModelIndex:
            {
                QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                mContextMenuSourceTreeItem = fSourceHook->itemFromIndex(fItemData.toModelIndex());
            }   break;
            case QVariant::Map:
            {
                auto fMap = fItemData.toMap();
                for (auto fMapItem = fMap.begin(); fMapItem != fMap.end(); ++fMapItem)
                {
                    QAction* fAction = fPostActionGroup.addAction(fMapItem.key());
                    fAction->setCheckable(true);
                    fAction->setChecked(fMapItem.value().toBool());
                }
                fPostActionGroup.addAction(tr("Enable all"));
                fPostActionGroup.addAction(tr("Disable all"));
            }   break;
            default:
                break;
        }
    }

    QMenu menu(this);
    QMenu*fAuthorsMenu = nullptr;
    if (fPostActionGroup.actions().size())
    {
        fAuthorsMenu = menu.addMenu(tr("Authors"));
        fAuthorsMenu->addActions(fPostActionGroup.actions());
    }

    mActions.fillContextMenue(menu, Cmd::mContextMenuHistoryTree);
    QAction* fAction = menu.exec(ui->treeHistory->mapToGlobal(pos));
    if (fAction && fAuthorsMenu)
    {
        int fIndex = fAuthorsMenu->actions().indexOf(fAction);
        ui->treeHistory->checkAuthorsIndex(fIndex, fAction->isChecked());
    }

    mContextMenuSourceTreeItem = old_item;
}

void MainWindow::call_git_history_diff_command()
{
    const QString &history_hash_items = ui->treeHistory->getSelectedHistoryHashItems();
    const QString &history_file       = ui->treeHistory->getSelectedHistoryFile();
    const QAction *action             = qobject_cast<QAction *>(sender());
    const Type    type(ui->treeHistory->getSelectedTopLevelType());
    const QVariantList variant_list   = action->data().toList();
    const uint    command_flags       = variant_list[ActionList::Data::Flags].toUInt();

    QString path;
    if (type.is(Type::Branch) && mContextMenuSourceTreeItem)
    {
        path =  mContextMenuSourceTreeItem->text(0);
        mContextMenuSourceTreeItem = nullptr;
    }
    if (mContextMenuSourceTreeItem)
    {
        QString tool_cmd = get_git_command_option(type, command_flags, variant_list);
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
}
