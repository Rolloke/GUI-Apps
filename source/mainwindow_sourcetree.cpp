#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "commitmessage.h"
#include "mergedialog.h"
#include "binary_values_view.h"

#include <sstream>

#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QTextCodec>

using namespace std;
using namespace git;

bool MainWindow::iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir, QTreeWidgetItem* aParentItem)
{
    bool fResult  = false;
    if (aParentItem)
    {
        auto check_state = ui->treeSource->mUseSourceTreeCheckboxes ? aParentItem->checkState(QSourceTreeWidget::Column::FileName) : Qt::Checked;

        if (   check_state == Qt::Checked || check_state == Qt::PartiallyChecked
            || mCurrentTask == Work::InsertPathFromCommandString)
        {
            const QVariant& fIsDir = aParentItem->data(QSourceTreeWidget::Column::FileName, QSourceTreeWidget::Role::isDirectory);
            QString fFileName = aParentItem->text(QSourceTreeWidget::Column::FileName);
            QString fSource;
            QString fResultStr;
            if (!fFileName.startsWith(QDir::separator()) && !aSourceDir->isEmpty())
            {
                fFileName =  QDir::separator() + fFileName;
            }
            fSource      = *aSourceDir + fFileName;

            if (fIsDir.toBool())
            {
                Type fType;
                const QVariant fVariant = aParentItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter);
                if (fVariant.isValid())
                {
                    fType = Type(fVariant.toUInt());
                }

                if (mCurrentTask == Work::ApplyGitCommand && check_state == Qt::Checked)
                {
                    const QString fCmd = applyGitCommandToFilePath(fSource, mGitCommand, fResultStr);
                    appendTextToBrowser(fCmd + getLineFeed() + fResultStr + getLineFeed(), true);
                    return false;
                }
                int fCountOk = 0;
                for (int fChildItem=0; fChildItem < aParentItem->childCount(); ++fChildItem)
                {
                    fResult = iterateTreeItems(aSourceTree, &fSource, aParentItem->child(fChildItem));
                    if (fResult) ++fCountOk;
                }
                switch (mCurrentTask)
                {
                    case Work::InsertPathFromCommandString:
                    {
                        const int fIndex = mGitCommand.indexOf(fSource);
                        if (fIndex != -1)
                        {
                            TRACE(Logger::notice, "Found dir: %s: %d", fFileName.toStdString().c_str(), fCountOk);
                            if (fCountOk == 0)
                            {
                                QStringList fChild;
                                fChild.append(mGitCommand.mid(fSource.size() + 1));
                                aParentItem->addChild(new QTreeWidgetItem(fChild));
                            }
                            return 1;
                        }
                        return 0;
                    }
                    break;
                    case Work::ShowAdded: case Work::ShowDeleted: case Work::ShowUnknown: case Work::ShowStaged: case Work::ShowUnMerged:
                    case Work::ShowModified: case Work::ShowAllGitActions: case Work::ShowSelected:
                    {
                        if (ui->ckHideEmptyParent->isChecked())
                        {
                            fResult = getShowTypeResult(fType);
                            if (fResult == false)
                            {
                                fResult = fCountOk != 0;
                            }
                            aParentItem->setHidden(!fResult); // true means visible
                        }
                        else
                        {
                            aParentItem->setHidden(false);
                        }
                    }   break;
                    case Work::ShowAllFiles:
                        aParentItem->setHidden(false);
                        fResult = true;
                        break;
                    case Work::None: case Work::Last: case Work::ApplyGitCommand: case Work::DetermineGitMergeTools:
                        /// NOTE: not handled here
                        break;

                }
            }
            else
            {
                const QVariant fVariant = aParentItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter);
                if (fVariant.isValid())
                {
                    const Type fType(fVariant.toUInt());
                    switch (mCurrentTask)
                    {
                        case Work::ApplyGitCommand:
                        {
                            fSource = "\"" + fSource + "\"";
                            QString fCmd = applyGitCommandToFilePath(fSource, mGitCommand, fResultStr);
                            appendTextToBrowser(fCmd + getLineFeed() + fResultStr, true);
                            fResult = fCmd.size() != 0;
                        }   break;
                        case Work::ShowAllFiles:
                            aParentItem->setHidden(false);
                            fResult = true;
                            break;
                        case Work::ShowAllGitActions: case Work::ShowAdded: case Work::ShowDeleted: case Work::ShowModified:
                        case Work::ShowUnknown: case Work::ShowUnMerged: case Work::ShowStaged:
                            fResult = getShowTypeResult(fType);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowSelected:
                            fResult = aParentItem->isSelected();
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::None: case Work::Last: case Work::DetermineGitMergeTools:
                        case Work::InsertPathFromCommandString:
                            /// NOTE: not handled here
                            return false;
                    }
                }
            }
        }
        else
        {
            TRACE(Logger::info, "Not copying unselected file %s", aParentItem->text(QSourceTreeWidget::Column::FileName).toStdString().c_str());
            fResult = true; // this is not an error
        }
    }
    else
    {
        fResult = true;
        for (int fIndex = 0; fIndex < aSourceTree.topLevelItemCount(); ++fIndex)
        {
            const QString fSourceDir("");
            fResult &= iterateTreeItems(aSourceTree, &fSourceDir, aSourceTree.topLevelItem(fIndex));
        }
    }
    return fResult;
}

bool MainWindow::getShowTypeResult(const Type& fType)
{
    bool fResult = false;
    switch(mCurrentTask)
    {
    case Work::ShowAllGitActions:
        fResult = fType.is(Type::AllGitActions);
        break;
    case Work::ShowAdded:
        fResult = fType.is(Type::GitAdded);
        break;
    case Work::ShowDeleted:
        fResult = fType.is(Type::GitDeleted);
        break;
    case Work::ShowModified:
        fResult = fType.is(Type::GitModified);
        break;
    case Work::ShowUnknown:
        fResult = fType.is(Type::GitUnTracked);
        break;
    case Work::ShowUnMerged:
        fResult = fType.is(Type::GitUnmerged);
        break;
    case Work::ShowStaged:
        fResult = fType.is(Type::GitStaged);
        break;
        case Work::None: case Work::Last: case Work::ApplyGitCommand: case Work::DetermineGitMergeTools:
        case Work::ShowAllFiles: case Work::InsertPathFromCommandString: case Work::ShowSelected:
            /// NOTE: not handled here
            break;
    }
    return fResult;
}

void MainWindow::insertSourceTree(const QDir& source_dir, int item)
{
    ui->treeSource->mGitIgnore.clear();
    QString result_string;
    applyGitCommandToFilePath(source_dir.path(), Cmd::getCommand(Cmd::GetStatusAll), result_string);

    appendTextToBrowser(tr("Repository: ") + source_dir.path(), (item == 0 && ui->treeSource->topLevelItemCount() == 0) ? false : true);
    appendTextToBrowser(result_string, true);
    appendTextToBrowser("", true);

    stringt2typemap check_map;
    parseGitStatus(source_dir.path() +  QDir::separator(), result_string, check_map);

    ui->treeSource->insertItem(source_dir, *ui->treeSource, nullptr);

    for (const auto& fItem : check_map)
    {
        if (fItem.second.is(Type::GitDeleted) || fItem.second.is(Type::GitMovedFrom))
        {
            mGitCommand = fItem.first;
            mCurrentTask = Work::InsertPathFromCommandString;
            QString fFilePath = "";
            iterateTreeItems(*ui->treeSource, &fFilePath, ui->treeSource->topLevelItem(item));
            mGitCommand.clear();
            mCurrentTask = Work::None;
        }
    }

    ui->treeSource->iterateCheckItems(ui->treeSource->topLevelItem(item), check_map);
}

void MainWindow::updateTreeItemStatus(QTreeWidgetItem * aItem)
{
    if (aItem)
    {
        const QFileInfo file_info(ui->treeSource->getItemFilePath(aItem));

        QDir fParent;
        if (file_info.isDir()) fParent.setPath(file_info.absoluteFilePath());
        else                   fParent.setPath(file_info.absolutePath());

        while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
        {
            fParent.cdUp();
            if (fParent.isRoot()) break;
            if (!fParent.exists()) break;
        };

        if (!fParent.isRoot())
        {
            const QString repository_path = fParent.absolutePath();
            QString fCommandString = tr(Cmd::getCommand(Cmd::GetStatusAll).toStdString().c_str()).arg(repository_path);
            fCommandString.append(" ");
            fCommandString.append(file_info.absoluteFilePath());

            QString result_string;
            applyGitCommandToFilePath(repository_path, fCommandString, result_string);

            stringt2typemap check_map;
            parseGitStatus(repository_path + QDir::separator(), result_string, check_map);

            const QString source_path = file_info.absolutePath();
            ui->treeSource->iterateCheckItems(aItem, check_map, &source_path);
        }
        on_treeSource_itemClicked(aItem, 0);
    }
}

void MainWindow::addGitSourceFolder()
{
    const QString fSourcePath = QFileDialog::getExistingDirectory(this, tr("Select SourceFiles"));
    if (fSourcePath.size() > 1)
    {
        insertSourceTree(initDir(fSourcePath), ui->treeSource->topLevelItemCount()+1);
    }
}

void MainWindow::removeGitSourceFolder()
{
    deleteTopLevelItemOfSelectedTreeWidgetItem(*ui->treeSource);
    mContextMenuSourceTreeItem = nullptr;
}

void MainWindow::on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int /* column */ )
{
    if (item)
    {
        const Type fType(Type::type(item->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
        if (fType.is(Type::Folder))
        {
            toggle_expand_item(item);
            return;
        }
    }
    const QString   file_name      = ui->treeSource->getItemFilePath(item);
    open_file(file_name);
}

void MainWindow::open_file_externally()
{
    if (mContextMenuSourceTreeItem)
    {
        const QString   file_name  = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        const QFileInfo file_info(file_name);
        const QString   file_extension = file_info.suffix().toLower();
        QString open_file_cmd = mExternalFileOpenCmd + " " + file_name;
        if (mExternalFileOpenExt.contains(file_extension))
        {
            auto program_parameter = mExternalFileOpenExt.value(file_extension);
            if (program_parameter.size())
            {
                open_file_cmd = program_parameter + " " + file_name;
            }
        }
        int result = system(open_file_cmd.toStdString().c_str());
        if (result != 0)
        {
            TRACE(Logger::warning, "could not open %s, error %d", file_name.toStdString().c_str(), result);
        }
    }
}

void MainWindow::open_file(const QString& file_path, boost::optional<int> line_number)
{
    on_btnCloseText_clicked();
    const QFileInfo file_info(file_path);
    QString         file_extension = file_info.suffix().toLower();

    if (mExternalFileOpenExt.contains(file_extension))
    {
        QString open_file_cmd = mExternalFileOpenCmd + " " + file_path;
        auto program_parameter = mExternalFileOpenExt.value(file_extension);
        if (program_parameter.size())
        {
            open_file_cmd = program_parameter + " " + file_path;
        }
        int result = system(open_file_cmd.toStdString().c_str());
        if (result == 0)
        {
            return;
        }
    }

    if (ui->ckRenderGraphicFile->isChecked())
    {
        ui->graphicsView->clear();
        if (ui->graphicsView->render_file(file_path, file_extension))
        {
            updateSelectedLanguage(file_extension);
            showDockedWidget(ui->graphicsView);
            return;
        }
    }

    QFile file(file_path);
    QFileInfo info(file_path);
    if (info.size() > mWarnOpenFileSize)
    {
        if (QMessageBox::warning(this,
            tr("Open large file %1?").arg(info.baseName()),
            tr("File size %1 is larger than\nwarning level size %2.\n"
               "This could be time consuming").
                arg(formatFileSize(info.size()), formatFileSize(mWarnOpenFileSize)),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
    }

    if (file.open(QIODevice::ReadOnly))
    {
        QWidget* widget_to_be_shown = ui->textBrowser;
        ui->labelFilePath->setText(file_path);
        ui->textBrowser->reset();
        if (file_extension == "txt" && file_path.contains("CMakeLists.txt"))
        {
            file_extension = "cmake";
        }
        if (qbinarytableview::is_binary(file))
        {
            ui->textBrowser->setExtension("bin");
            ui->tableBinaryView->set_binary_data(file.readAll());
            widget_to_be_shown = ui->tableBinaryView;
            showDockedWidget(mBinaryValuesView.data());
        }
        else
        {
            bool codec_selected = false;
            if (ui->comboTextCodex->currentIndex())
            {
                // Umlaute windows-437, windows-850, windows-1252
                auto current = ui->comboTextCodex->currentText();
                int pos = current.indexOf(",");
                if (pos == -1) pos = current.size();
                auto name = current.left(pos).toStdString();
                QByteArray codec(&name[0], static_cast<int>(name.size()));
                QTextCodec::setCodecForLocale(QTextCodec::codecForName(codec));
                codec_selected = true;
            }
            else
            {
                QTextCodec::setCodecForLocale(nullptr);
            }

            ui->textBrowser->setExtension(file_extension);
            if (ui->ckRenderGraphicFile->isChecked())
            {
                if (codec_selected)
                {
                    ui->textBrowser->setText(QString::fromLocal8Bit(file.readAll()));
                }
                else
                {
                    ui->textBrowser->setText(file.readAll());
                }
            }
            else
            {
                if (codec_selected)
                {
                    ui->textBrowser->setPlainText(QString::fromLocal8Bit(file.readAll()));
                }
                else
                {
                    ui->textBrowser->setPlainText(file.readAll());
                }
            }
        }
        set_widget_and_action_enabled(ui->btnStoreText, false);

        if (line_number)
        {
            ui->textBrowser->go_to_line(*line_number);
        }
        showDockedWidget(widget_to_be_shown);
    }
}


void MainWindow::updateRepositoryStatus(bool append)
{
    QString   file_name;
    if (mContextMenuSourceTreeItem)
    {
        file_name = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
    }
    int selected_item = -1;
    vector<QString> fSourceDirs;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSourceDirs.push_back(ui->treeSource->topLevelItem(i)->text(QSourceTreeWidget::Column::FileName));
        if (file_name == fSourceDirs.back())
        {
            selected_item = i;
        }
    }

    if (selected_item != -1)
    {
        if (!append)
        {
            on_btnCloseText_clicked();
        }
        auto item = ui->treeSource->topLevelItem(selected_item);
        ui->treeSource->removeItemWidget(item, 0);
        delete item;
        insertSourceTree(initDir(fSourceDirs[selected_item]), selected_item);
    }
    else
    {
        ui->treeSource->clear();

        qint64 fSize = 0;
        for (uint i = 0; i < fSourceDirs.size(); ++i)
        {
            insertSourceTree(initDir(fSourceDirs[i]), i);
            fSize += ui->treeSource->sizeOfCheckedItems(ui->treeSource->topLevelItem(i));
        }
        ui->statusBar->showMessage(tr("Total selected bytes: ") + formatFileSize(fSize));
        on_comboShowItems_currentIndexChanged(ui->comboShowItems->currentIndex());
    }

}

void MainWindow::on_treeSource_customContextMenuRequested(const QPoint &pos)
{
    mContextMenuSourceTreeItem = ui->treeSource->itemAt( pos );
    if (mContextMenuSourceTreeItem)
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuSourceTree);
        ui->treeSource->fillContextMenue(menu, mContextMenuSourceTreeItem);
        menu.exec(ui->treeSource->mapToGlobal(pos) + menu_offset);
    }
    else
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuEmptySourceTree);
        menu.exec(ui->treeSource->mapToGlobal(pos) + menu_offset);
    }
}

void MainWindow::on_ckHideEmptyParent_clicked(bool )
{
    on_comboShowItems_currentIndexChanged(ui->comboShowItems->currentIndex());
}

void MainWindow::on_ckShortState_clicked(bool checked)
{
    Type::mShort = checked;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        const QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
        for (int j=0; j<fItem->childCount(); ++j)
        {
            updateTreeItemStatus(fItem->child(j));
        }
    }
}

void MainWindow::on_comboShowItems_currentIndexChanged(int index)
{
    switch (static_cast<ComboShowItems>(index))
    {
    case ComboShowItems::AllFiles:      mCurrentTask = Work::ShowAllFiles;      break;
    case ComboShowItems::AllGitActions: mCurrentTask = Work::ShowAllGitActions; break;
    case ComboShowItems::GitModified:   mCurrentTask = Work::ShowModified;      break;
    case ComboShowItems::GitAdded:      mCurrentTask = Work::ShowAdded;         break;
    case ComboShowItems::GitDeleted:    mCurrentTask = Work::ShowDeleted;       break;
    case ComboShowItems::GitUnknown:    mCurrentTask = Work::ShowUnknown;       break;
    case ComboShowItems::Gitstaged:     mCurrentTask = Work::ShowStaged;        break;
    case ComboShowItems::GitUnmerged:   mCurrentTask = Work::ShowUnMerged;      break;
    case ComboShowItems::GitSelected:   mCurrentTask = Work::ShowSelected;      break;
    }
    iterateTreeItems(*ui->treeSource);
    mCurrentTask = Work::None;
}

void MainWindow::on_treeSource_itemClicked(QTreeWidgetItem *item, int /* column */ )
{
    if (item)
    {
        mContextMenuSourceTreeItem = item;
        const Type fType(Type::type(mContextMenuSourceTreeItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
        mActions.enableItemsByType(Cmd::mContextMenuSourceTree, fType);
        mActions.enableItemsByType(Cmd::mToolbars[0], fType);
        if (mMergeDialog)
        {
            mMergeDialog->mGitFilePath = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        }
    }
}

void MainWindow::getSelectedTreeItem()
{
    if (! mContextMenuSourceTreeItem)
    {
        const auto fSelected = ui->treeSource->selectedItems();
        if (fSelected.size())
        {
            mContextMenuSourceTreeItem = fSelected.at(0);
        }
    }
}

void  MainWindow::applyGitCommandToFileTree(const QString& aCommand)
{
    if (mContextMenuSourceTreeItem)
    {
        on_btnCloseText_clicked();
        mGitCommand = aCommand;
        mCurrentTask = Work::ApplyGitCommand;
        QString fFilePath = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem->parent());
        QString ftlp = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(QSourceTreeWidget::Column::FileName);
        fFilePath = fFilePath.mid(ftlp.length()+1);
        iterateTreeItems(*ui->treeSource, &fFilePath, mContextMenuSourceTreeItem);
        mGitCommand.clear();
        mCurrentTask = Work::None;
    }
}

void  MainWindow::call_git_commit()
{
    CommitMessage commit_message;

    if (commit_message.exec() == QDialog::Accepted)
    {
        on_btnCloseText_clicked();
        const QString message_text   = commit_message.getMessageText();
        string        command        = Cmd::getCommand(Cmd::Commit).toStdString();
        const QString commit_command = tr(command.c_str()).arg(ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem), message_text);
        QString result_str;
        if (commit_message.getAutoStage())
        {
            getSelectedTreeItem();
            mWorker.setOnceBusy();
            mActions.getAction(Cmd::Add)->trigger();

            int result = execute(commit_command, result_str);
            if (result != NoError)
            {
                result_str += tr("\nError %1 occurred").arg(result);
            }
            appendTextToBrowser(commit_command + getLineFeed() + result_str);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
        }
        else
        {
            int result = execute(commit_command, result_str);
            if (result != NoError)
            {
                result_str += tr("\nError %1 occurred").arg(result);
            }
            appendTextToBrowser(commit_command + getLineFeed() + result_str);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
        }
        if (commit_message.getAndPush())
        {
            mActions.getAction(Cmd::Push)->trigger();
        }
    }
}

void MainWindow::initMergeTools(bool read_new_items)
{
    if (mMergeTools.size())
    {
        if (ui->comboDiffTool->count() > 1)
        {
            QString first_entry = ui->comboDiffTool->itemText(0);
            ui->comboDiffTool->clear();
            ui->comboDiffTool->addItem(first_entry);
        }
        for (auto item=mMergeTools.begin(); item != mMergeTools.end(); ++item)
        {
            if (item.value())
            {
                ui->comboDiffTool->addItem(item.key());
            }
        }
    }
    if (read_new_items && ui->treeSource->topLevelItemCount())
    {
        mActions.getAction(Cmd::KillBackgroundThread)->setEnabled(true);
        QString first_git_repo =ui->treeSource->topLevelItem(0)->text(QSourceTreeWidget::Column::FileName);
        mWorker.doWork(Work::DetermineGitMergeTools, QVariant(tr("git -C %1 difftool --tool-help").arg(first_git_repo)));
    }
}

void MainWindow::call_git_move_rename(QTreeWidgetItem* dropped_target, bool *was_dropped)
{
    getSelectedTreeItem();
    if (mContextMenuSourceTreeItem)
    {
        bool      fOk {false};
        const Type fType(Type::type(mContextMenuSourceTreeItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
        const QString   fFileTypeName = Type::name(Type::type(Type::FileType&fType.type()));
        const QFileInfo fPath(ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem));

        QString       fOldName = mContextMenuSourceTreeItem->text(QSourceTreeWidget::Column::FileName);
        if (dropped_target)
        {
            QString repository = ui->treeSource->getItemTopDirPath(dropped_target);
            if (repository == ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem))
            {
                fOldName = ui->treeSource->getItemFilePath(dropped_target);
            }
            else
            {
                return;
            }
        }

        const QString fNewName = QInputDialog::getText(this,
                       tr("Move or rename %1").arg(fFileTypeName),
                       tr("Enter a new name or destination for \"%1\".\n"
                          "To move the %3 insert the destination path before.\n"
                          "To rename just modify the name.").arg(fPath.filePath(), fFileTypeName),
                       QLineEdit::Normal, fOldName, &fOk);

        if (fOk && !fNewName.isEmpty())
        {
            const string fFormatCmd = Cmd::getCommand(Cmd::MoveOrRename).toStdString().c_str();
            const QString fNewGitName = "\"" + fNewName + "\"";
            QString     fCommand;
            bool        fMoved = false;
            if (fNewGitName.contains("/"))
            {
                fMoved   = true;
                fOldName = "\"" + fPath.filePath() + "\"";
                fCommand = tr(fFormatCmd.c_str()).arg(ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem), fOldName, fNewGitName);
            }
            else
            {
                fOldName = "\"" + fOldName + "\"";
                fCommand  = tr(fFormatCmd.c_str()).arg(fPath.absolutePath(), fOldName, fNewGitName);
            }

            QString fResultStr;
            int fResult = execute(fCommand, fResultStr);
            if (fResult == NoError)
            {
                if (fMoved)
                {
                    mActions.getAction(Cmd::UpdateGitStatus)->trigger();
                }
                else
                {
                    mContextMenuSourceTreeItem->setText(QSourceTreeWidget::Column::FileName, fNewName);
                    updateTreeItemStatus(mContextMenuSourceTreeItem);
                }
                if (was_dropped)
                {
                    *was_dropped = true;
                }
            }
            else
            {
                appendTextToBrowser(fCommand + fResultStr + tr("\nError %1 occurred").arg(fResult));
            }
        }
        mContextMenuSourceTreeItem = nullptr;
    }
}

void MainWindow::perform_custom_command()
{
    const QAction     *action        = qobject_cast<QAction *>(sender());
    const QVariantList variant_list  = action->data().toList();
    const uint         command_flags = variant_list[ActionList::Data::Flags].toUInt();

    QString git_command = action->statusTip();

    if      (command_flags & ActionList::Flags::History && ui->treeHistory->hasFocus())
    {
        getSelectedTreeItem();
        call_git_history_diff_command();
    }
    else if (command_flags & ActionList::Flags::Branch)
    {
        call_git_branch_command();
    }
    else if (command_flags & ActionList::Flags::Stash && ui->treeStash->hasFocus())
    {
        call_git_stash_command();
    }
    else
    {
        Type    type;
        QString message_box_text = variant_list[ActionList::Data::MsgBoxText].toString();
        getSelectedTreeItem();
        if (mContextMenuSourceTreeItem)
        {
            type.setType(mContextMenuSourceTreeItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt());
            QDir::setCurrent(getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(QSourceTreeWidget::Column::FileName));
        }

        if (git_command.contains("-C %1"))
        {
            on_btnCloseText_clicked();
            QString result_str;
            git_command = tr(git_command.toStdString().c_str()).arg(ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem));
            QString cmd_option = get_git_command_option(type, command_flags, variant_list);
            if (cmd_option.size())
            {
                git_command += cmd_option;
            }

            int result = callMessageBox(message_box_text, "", ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem), false);
            if (result == QMessageBox::Yes || result == QMessageBox::YesToAll)
            {
                if (handleInThread())
                {
                    mActions.getAction(Cmd::KillBackgroundThread)->setEnabled(true);
                    mWorker.doWork(Work::ApplyGitCommand, QVariant(git_command));
                }
                else
                {
                    result = execute(git_command, result_str);
                    if (result != NoError)
                    {
                        result_str += tr("\nError %1 occurred").arg(result);
                    }
                    appendTextToBrowser(git_command + getLineFeed() + result_str);
                }
            }
        }
        else if (mContextMenuSourceTreeItem)
        {
            QString option = get_git_command_option(type, command_flags, variant_list);
            git_command = tr(git_command.toStdString().c_str()).arg(option, "%1");

            int fResult = callMessageBox(message_box_text, type.type_name(), mContextMenuSourceTreeItem->text(QSourceTreeWidget::Column::FileName), type.is(Type::File));
            if (fResult == QMessageBox::Yes || fResult == QMessageBox::YesToAll)
            {
                applyGitCommandToFileTree(git_command);
            }
        }
        else
        {
            QString fOption;
            git_command = tr(git_command.toStdString().c_str()).arg(fOption, "");
        }

        if (!(command_flags & ActionList::Flags::CallInThread))
        {
            perform_post_cmd_action(variant_list[ActionList::Data::PostCmdAction].toUInt(), type);
        }
    }
}

void MainWindow::perform_post_cmd_action(uint post_cmd, const git::Type& type)
{
    switch (post_cmd)
    {
    case Cmd::UpdateItemStatus:
        updateTreeItemStatus(mContextMenuSourceTreeItem);
        break;
    case Cmd::UpdateRootItemStatus:
        mContextMenuSourceTreeItem = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem);
        updateRepositoryStatus(true);
        break;
    case Cmd::UpdateRepository:
        updateTreeItemStatus(getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem));
        break;
    case Cmd::ParseHistoryText:
    {
        QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
        ui->treeHistory->parseGitLogHistoryText(ui->textBrowser->toPlainText(), fSourceHook->indexFromItem(mContextMenuSourceTreeItem), ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem), type.type());
        ui->textBrowser->setPlainText("");
        showDockedWidget(ui->treeHistory);
        if (ui->ckShowHistoryGraphically->isChecked())
        {
            showDockedWidget(ui->graphicsView);
        }
    }   break;
    case Cmd::ParseBlameText:
    {
        const QString   file_name = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        const QFileInfo file_info(file_name);
        ui->textBrowser->setExtension(file_info.suffix().toLower());
        ui->textBrowser->parse_blame(ui->textBrowser->toPlainText());
    }   break;
    }
}

QString MainWindow::get_git_command_option(const Type& type, uint command_flags, const QVariantList& variant_list)
{
    QString option;
    QString cmd_add_on = variant_list[ActionList::Data::CmdAddOn].toString();
    if (cmd_add_on.size() )
    {
        if (   ((command_flags & ActionList::Flags::DependsOnStaged) != 0 && type.is(Type::GitStaged))
            || ((command_flags & ActionList::Flags::StashCmdOption) != 0 && type.is(Type::FileType) && !type.is(Type::Repository)))
        {
            option += " ";
            option += cmd_add_on;
            option += " ";
        }
    }
    if (command_flags & ActionList::Flags::DiffOrMergeTool && ui->comboDiffTool->currentIndex() != 0)
    {
        option += " --tool=";
        option += ui->comboDiffTool->currentText();
        option += " ";
    }
    if (command_flags & ActionList::Flags::MenuOption)
    {
        const QStringList list = variant_list[ActionList::Data::MenuStrings].toStringList();
        for (const QString & item : list)
        {
            if (item.indexOf("--") == 0)
            {
                option += " ";
                option += item;
                option += "=";
            }
            if (item.indexOf("*") == 0)
            {
                option += item.right(item.size()-1);
                option += " ";
            }
        }
    }
    return option;
}


int MainWindow::call_git_command(QString git_command, const QString& argument1, const QString& argument2, QString&result_str, const QString& git_root_path)
{
    on_btnCloseText_clicked();

    if (git_command.contains("-C %1"))
    {
        if (git_command.contains("%2"))
        {
            git_command = tr(git_command.toStdString().c_str()).arg(argument1, argument2);
        }
        else
        {
            git_command = tr(git_command.toStdString().c_str()).arg(argument1);
        }
    }
    else
    {
        if (git_root_path.size())
        {
            git_command.replace("git ", "git -C " + git_root_path + " ");
        }
        if (git_command.contains("%2"))
        {
            git_command = tr(git_command.toStdString().c_str()).arg(argument1, argument2);
        }
        else if (git_command.contains("%1"))
        {
            git_command = tr(git_command.toStdString().c_str()).arg(argument1);
        }
    }

    int result = execute(git_command, result_str);
    result_str = git_command + getLineFeed() + result_str;
    appendTextToBrowser(result_str);
    return result;
}


void MainWindow::deleteFileOrFolder()
{
    if (mContextMenuSourceTreeItem)
    {
        const QString fItemPath     = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        const Type fType(mContextMenuSourceTreeItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt());
        if (callMessageBox(tr("Delete %1"), fType.type_name(), fItemPath) == QMessageBox::Yes)
        {
            bool result = false;
            if (fType.is(Type::Folder))
            {
                QDir dir;
                result = dir.rmdir(fItemPath);
            }
            else
            {
                result = QFile::remove(fItemPath);
            }

            if (result)
            {
                if (fType.is(Type::AllGitActions) && !fType.is(Type::GitUnTracked))
                {
                    updateTreeItemStatus(mContextMenuSourceTreeItem);
                }
                else
                {
                    QTreeWidgetItem *parent = mContextMenuSourceTreeItem->parent();
                    if (parent)
                    {
                        parent->removeChild(mContextMenuSourceTreeItem);
                    }
                }
            }
            else
            {
                appendTextToBrowser(tr("Could not delete %1 %2\n").arg(fType.type_name(), fItemPath));
                if (errno)
                {
                    appendTextToBrowser(strerror(errno), true);
                }
            }
        }
    }
}

void MainWindow::on_treeSource_currentItemChanged(QTreeWidgetItem *  current, QTreeWidgetItem *previous)
{
    if (   mContextMenuSourceTreeItem
        && mContextMenuSourceTreeItem == previous)
    {
        if (current)
        {
            on_treeSource_itemClicked(current, 0);
        }
        else
        {
            mContextMenuSourceTreeItem = nullptr;
        }
    }
}

void MainWindow::copyFileName()
{
    copy_file(copy::name);
}

void MainWindow::copyFilePath()
{
    copy_file(copy::file);
}

void MainWindow::copy_file(copy command)
{
    if (mContextMenuSourceTreeItem)
    {
        const QString fTopItemPath  = ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem);
        const QString fItemPath     = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        QFileInfo fileInfo;
        if (fItemPath.contains(fTopItemPath))
        {
            fileInfo = fItemPath;
        }
        else
        {
            fileInfo = fTopItemPath + QDir::separator() + fItemPath;
        }
        QClipboard *clipboard = QApplication::clipboard();
#if 0
        auto testData = clipboard->mimeData();
        if (testData)
        {
            auto text = testData->text();
            auto urls = testData->urls();
            auto formats = testData->formats();
            auto data = testData->data("");
            for (auto format : formats)
            {
                data = testData->data(format);
            }
            int f = 0;
        }
#endif
        if (command == copy::name)
        {
            clipboard->setText(fileInfo.fileName());
        }
        else if (command == copy::path)
        {
            clipboard->setText(fileInfo.filePath());
        }
        else // copy::file
        {
            QMimeData* mime_data = new QMimeData();
            // Copy path of file
            mime_data->setText(fileInfo.filePath());
            // Copy file
            auto url = QUrl::fromLocalFile(fileInfo.filePath());
            mime_data->setUrls({ url });
            // Copy file (mFileCopyMimeType may be edited in ini-file)
            QByteArray copied_files = QByteArray("copy\n").append(url.toEncoded());
            mime_data->setData(mFileCopyMimeType, copied_files);
            // Set the mimedata
            clipboard->setMimeData(mime_data);
        }
    }
}
/*
#include <type_traits>

#define define_has_member(member_name)                                         \
    template <typename T>                                                      \
    class has_member_##member_name                                             \
    {                                                                          \
        typedef char yes_type;                                                 \
        typedef long no_type;                                                  \
        template <typename U> static yes_type test(decltype(&U::member_name)); \
        template <typename U> static no_type  test(...);                       \
    public:                                                                    \
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);  \
    }

/// Shorthand for testing if "class_" has a member called "member_name"
///
/// @note "define_has_member(member_name)" must be used
///       before calling "has_member(class_, member_name)"
///
#define has_member(class_, member_name)  has_member_##member_name<class_>::value

define_has_member(birthTime);
*/

void MainWindow::showInformation()
{
    if (mContextMenuSourceTreeItem)
    {
        QMessageBox message_box;
        message_box.setParent(parentWidget());
        const QString repository = ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem);
        const QString file_path  = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        QFileInfo file_info;
        if (file_path.contains(repository))
        {
            file_info = file_path;
        }
        else
        {
            file_info = repository + QDir::separator() + file_path;
        }
        bool old = Type::mShort;
        Type::mShort = false;
        Type type(mContextMenuSourceTreeItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt());
        type.translate(file_info);

        message_box.setText(tr("Information about %1\n%2").arg(type.is(Type::Folder) ? Type::name(Type::Folder) : Type::name(Type::File),
                                                               type.is(Type::Folder) ? file_info.completeBaseName() : file_info.fileName()));

        QString states = type.getStates(true);
        states.replace("|", " ");
        std::stringstream information;

        information << "States: " << states.toStdString() << std::endl;
        information << "Size: " << formatFileSize(file_info.size()).toStdString() << std::endl;
#ifdef __linux__
        if (file_info.birthTime().isValid())
        {
            information << "\nCreated: " << file_info.birthTime().toString(Qt::SystemLocaleShortDate).toStdString();
        }
#endif
        information << "\nModified: " << file_info.lastModified().toString(Qt::SystemLocaleShortDate).toStdString();
        information << "\nLast Access: " << file_info.lastRead().toString(Qt::SystemLocaleShortDate).toStdString();
#ifdef __linux__
        if (file_info.metadataChangeTime().isValid())
        {
            information << "\nMetadata Access: " << file_info.metadataChangeTime().toString(Qt::SystemLocaleShortDate).toStdString();
        }
#endif
        information << "\nPermissions:\n" << formatPermissions(file_info.permissions()).toStdString() << std::endl;
#ifdef __linux__
        information << "\nOwner: " << file_info.owner().toStdString();
        information << "\nGroup: " << file_info.group().toStdString();
#endif
        if (file_info.isSymLink())
        {
            information << "\nSymbolic link target:\n" << file_info.symLinkTarget().toStdString();
        }

        message_box.setInformativeText(information.str().c_str());

        Type::mShort = old;
        message_box.exec();
    }
}
