#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "commitmessage.h"
#include "mergedialog.h"


#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>

using namespace std;
using namespace git;


quint64 MainWindow::insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem)
{
    QDirIterator fIterator(aParentDir, QDirIterator::NoIteratorFlags);

    vector<int> fMapLevels;

    bool fTopLevelItem(false);
    if (!aParentItem)
    {
        QStringList fStrings;
        fStrings.append(aParentDir.absolutePath());
        aParentItem = new QTreeWidgetItem(fStrings);
        aParentItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|aParentItem->flags());
        aParentItem->setCheckState(Column::FileName, Qt::Checked);
        aParentItem->setData(Column::FileName, Role::isDirectory, QVariant(true));
        aParentItem->setData(Column::FileName, Role::Filter, QVariant(INT(aParentDir.filter())));

        aTree.addTopLevelItem(aParentItem);
        QDir fParent = aParentDir;
        while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
        {
            fParent.cdUp();
            mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);
            if (fParent.isRoot()) break;
        };
#if RELATIVE_GIT_PATH ==1
        if (!fParent.isRoot())
        {
            aParentItem->setData(Column::FileName, Role::GitFolder, QVariant(fParent.absolutePath()));
        }
#endif
        fTopLevelItem = true;
    }

    mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();

        if (mGitIgnore.ignoreFile(fFileInfo)) continue;

        QStringList fColumns;
        fColumns.append(fFileInfo.fileName());
        fColumns.append(fFileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
        fColumns.append(formatFileSize(fFileInfo.size()));
        fColumns.append("");

        QTreeWidgetItem* fItem = new QTreeWidgetItem(aParentItem, fColumns);
        fItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|fItem->flags());
        fItem->setCheckState(Column::FileName, Qt::Checked);
        fItem->setData(Column::FileName, Role::isDirectory, QVariant(fFileInfo.isDir()));
        fItem->setData(Column::DateTime, Role::DateTime, QVariant(fFileInfo.lastModified()));

        Type fType;
        fType.translate(fFileInfo);

        fItem->setData(Column::State, Role::Filter, QVariant(fType.type()));

        if (fFileInfo.isDir())
        {
            QDir fSubDir(fFileInfo.absoluteFilePath());
            fSubDir.setFilter(aParentDir.filter());
            quint64 fSizeOfSubFolderFiles = insertItem(fSubDir, aTree, fItem);
            fItem->setText(Column::Size, formatFileSize(fSizeOfSubFolderFiles));
            fSizeOfFiles += fSizeOfSubFolderFiles;
            fItem->setData(Column::Size, Qt::SizeHintRole, QVariant(fSizeOfSubFolderFiles));
        }
        else
        {
            fSizeOfFiles += fFileInfo.size();
            fItem->setText(Column::Size, formatFileSize(fFileInfo.size()));
            fItem->setData(Column::Size, Qt::SizeHintRole, QVariant(fFileInfo.size()));
        }
        //mIgnoreContainingNegation.reset();
    }
    while (fIterator.hasNext());


    for (const auto& fMapLevel : fMapLevels)
    {
        mGitIgnore.removeIgnoreMapLevel(fMapLevel);
    }

    if (fTopLevelItem)
    {
        aParentItem->setText(Column::Size, formatFileSize(fSizeOfFiles));
    }
    return fSizeOfFiles;
}

bool MainWindow::iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir, QTreeWidgetItem* aParentItem)
{
    bool fResult  = false;
    if (aParentItem)
    {
        if (   aParentItem->checkState(Column::FileName) == Qt::Checked
            || aParentItem->checkState(Column::FileName) == Qt::PartiallyChecked
            || mCurrentTask == Work::InsertPathFromCommandString)
        {
            const QVariant& fIsDir = aParentItem->data(Column::FileName, Role::isDirectory);
            QString fFileName = aParentItem->text(Column::FileName);
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
                const QVariant fVariant = aParentItem->data(Column::State, Role::Filter);
                if (fVariant.isValid())
                {
                    fType = Type(fVariant.toUInt());
                }

                if (   mCurrentTask == Work::ApplyGitCommand
                    && aParentItem->checkState(Column::FileName) == Qt::Checked)
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
                const QVariant fVariant = aParentItem->data(Column::State, Role::Filter);
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
            TRACE(Logger::info, "Not copying unselected file %s", aParentItem->text(Column::FileName).toStdString().c_str());
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

bool MainWindow::iterateCheckItems(QTreeWidgetItem* aParentItem, stringt2typemap& aPathMap, const QString* aSourceDir)
{
    if (aParentItem)
    {
        QString item_text = aParentItem->text(Column::FileName);
        auto pos = item_text.lastIndexOf('/');
        if (pos != -1 && aSourceDir != nullptr)
        {
            item_text = item_text.mid(pos+1);
        }
        const QString fSourcePath = aSourceDir ? *aSourceDir + "/" + item_text : item_text;

        const auto fFoundType = aPathMap.find(fSourcePath.toStdString());
        if (fFoundType != aPathMap.end())
        {
            aParentItem->setCheckState(Column::FileName, Qt::Checked);
            const QString fState = fFoundType->second.getStates();
            aParentItem->setText(Column::State, fState);
            Type fType(aParentItem->data(Column::State, Role::Filter).toUInt());
            fType.remove(Type::AllGitActions);
            fType.add(static_cast<Type::TypeFlags>(fFoundType->second.type()));
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.type()));
            TRACE(Logger::info, "set state %s, %x of %s", fState.toStdString().c_str(), fFoundType->second.type(), fSourcePath.toStdString().c_str());
        }
        else
        {
            Type fType(aParentItem->data(Column::State, Role::Filter).toUInt());
            fType.remove(Type::AllGitActions);
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.type()));
            aParentItem->setText(Column::State, "");
        }
        for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
        {
            iterateCheckItems(aParentItem->child(fChild), aPathMap, &fSourcePath);
        }
    }
    return false;
}

void MainWindow::insertSourceTree(const QDir& source_dir, int item)
{
    mGitIgnore.clear();
    QString result_string;
    applyGitCommandToFilePath(source_dir.path(), Cmd::getCommand(Cmd::GetStatusAll), result_string);

    appendTextToBrowser(tr("Repository: ")+source_dir.path(), item == 0 ? false : true);
    appendTextToBrowser(result_string, true);
    appendTextToBrowser("", true);

    stringt2typemap check_map;
    parseGitStatus(source_dir.path() +  QDir::separator(), result_string, check_map);

    insertItem(source_dir, *ui->treeSource);

    for (const auto& fItem : check_map)
    {
        if (fItem.second.is(Type::GitDeleted) || fItem.second.is(Type::GitMovedFrom))
        {
            mGitCommand = fItem.first.c_str();
            mCurrentTask = Work::InsertPathFromCommandString;
            QString fFilePath = "";
            iterateTreeItems(*ui->treeSource, &fFilePath, ui->treeSource->topLevelItem(item));
            mGitCommand.clear();
            mCurrentTask = Work::None;
        }
    }

    iterateCheckItems(ui->treeSource->topLevelItem(item), check_map);
}

void MainWindow::updateTreeItemStatus(QTreeWidgetItem * aItem)
{
    const QFileInfo file_info(getItemFilePath(aItem));

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
        iterateCheckItems(aItem, check_map, &source_path);
    }
}

quint64 MainWindow::sizeOfCheckedItems(QTreeWidgetItem* aParentItem)
{
    quint64 fSize = 0;
    if (aParentItem)
    {
        bool fIterate = false;
        switch (aParentItem->checkState(Column::FileName))
        {
            case Qt::Unchecked: return fSize;
            case Qt::PartiallyChecked:
                fIterate = true;
                break;
            case  Qt::Checked:
                if (!aParentItem->data(Column::FileName, Role::isDirectory).toBool())
                {
                    fSize = aParentItem->data(Column::Size, Qt::SizeHintRole).toLongLong();
                }
                fIterate = true;
                break;
        }
        if (fIterate)
        {
            for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
            {
                fSize += sizeOfCheckedItems(aParentItem->child(fChild));
            }
        }
    }
    return fSize;
}

QString MainWindow::getItemFilePath(QTreeWidgetItem* aTreeItem)
{
    QString file_name;
    if (aTreeItem)
    {
        file_name = aTreeItem->text(Column::FileName);
        bool is_folder = aTreeItem->data(Column::FileName, Role::GitFolder).isValid();
        bool current_dir_set = false;
        while (aTreeItem)
        {
            aTreeItem = aTreeItem->parent();
            if (aTreeItem)
            {
                if (aTreeItem->data(Column::FileName, Role::GitFolder).isValid())
                {
                    QDir::setCurrent(aTreeItem->text(Column::FileName));
                    current_dir_set = true;
                }
                else
                {
                    file_name = aTreeItem->text(Column::FileName) + QDir::separator() + file_name;
                }
            }
        }
        if (!current_dir_set && is_folder)
        {
            QDir::setCurrent(file_name);
        }
    }
    return file_name;
}

QString MainWindow::getItemTopDirPath(QTreeWidgetItem* aItem)
{
    aItem = getTopLevelItem(*ui->treeSource, aItem);
    if (aItem)
    {
        return aItem->text(Column::FileName);
    }
    else if (ui->treeSource->topLevelItemCount())
    {
        return ui->treeSource->topLevelItem(0)->text(Column::FileName);
    }
    return "";
}

void MainWindow::cancelCurrentWorkTask()
{
    mCurrentTask = Work::None;
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
    const QString   file_name      = getItemFilePath(item);
    open_file(file_name, {});
}

void MainWindow::open_file(const QString& file_path, boost::optional<int> line_number)
{
    on_btnCloseText_clicked();
    const QFileInfo file_info(file_path);
    QString         file_extension = file_info.suffix().toLower();

    if (ui->ckRenderGraphicFile->isChecked())
    {
        if (ui->graphicsView->render_file(file_path, file_extension))
        {
            updateSelectedLanguage(file_extension);
#ifdef DOCKED_VIEWS
            showDockedWidget(ui->graphicsView);
#endif
            return;
        }
    }

    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly))
    {
        ui->labelFilePath->setText(file_path);
        mHighlighter.reset(new Highlighter(ui->textBrowser->document()));
        if (file_extension == "txt" && file_path.contains("CMakeLists.txt"))
        {
            file_extension = "cmake";
        }
        mHighlighter->setExtension(file_extension);
        if (ui->ckRenderGraphicFile->isChecked())
        {
            ui->textBrowser->setText(file.readAll());
        }
        else
        {
            ui->textBrowser->setPlainText(file.readAll());
        }
        ui->btnStoreText->setEnabled(false);
        if (line_number)
        {
            auto text_block = ui->textBrowser->document()->findBlockByLineNumber(*line_number - 1);
            QTextCursor cursor(text_block);
            cursor.select(QTextCursor::LineUnderCursor);
            ui->textBrowser->setTextCursor(cursor);
        }
#ifdef DOCKED_VIEWS
        showDockedWidget(ui->textBrowser);
#endif
    }
}

void MainWindow::updateGitStatus()
{
    vector<QString> fSourceDirs;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSourceDirs.push_back(ui->treeSource->topLevelItem(i)->text(Column::FileName));
    }
    ui->treeSource->clear();

    qint64 fSize = 0;
    for (uint i = 0; i < fSourceDirs.size(); ++i)
    {
        insertSourceTree(initDir(fSourceDirs[i]), i);
        fSize += sizeOfCheckedItems(ui->treeSource->topLevelItem(i));
    }

    ui->statusBar->showMessage(tr("Total selected bytes: ") + formatFileSize(fSize));
    on_comboShowItems_currentIndexChanged(ui->comboShowItems->currentIndex());
}

void MainWindow::on_treeSource_customContextMenuRequested(const QPoint &pos)
{
    mContextMenuSourceTreeItem = ui->treeSource->itemAt( pos );
    if (mContextMenuSourceTreeItem)
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuSourceTree);
        menu.exec( ui->treeSource->mapToGlobal(pos) );
    }
    else
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuEmptySourceTree);
        menu.exec( ui->treeSource->mapToGlobal(pos) );
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
    mContextMenuSourceTreeItem = item;
    const Type fType(static_cast<Type::TypeFlags>(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt()));
    mActions.enableItemsByType(Cmd::mContextMenuSourceTree, fType);
    if (mMergeDialog)
    {
        mMergeDialog->mGitFilePath = getItemFilePath(mContextMenuSourceTreeItem);
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
        QString fFilePath = getItemFilePath(mContextMenuSourceTreeItem->parent());
        QString ftlp = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(Column::FileName);
        fFilePath = fFilePath.mid(ftlp.length()+1);
        iterateTreeItems(*ui->treeSource, &fFilePath, mContextMenuSourceTreeItem);
        mGitCommand.clear();
        mCurrentTask = Work::None;
    }
}

void  MainWindow::call_git_commit()
{
    CommitMessage fCommitMsg;

    if (fCommitMsg.exec() == QDialog::Accepted)
    {
        on_btnCloseText_clicked();
        const QString fMessageText = fCommitMsg.getMessageText();
        const string  fCommand  = Cmd::getCommand(Cmd::Commit).toStdString();
        const QString fCommitCommand = tr(fCommand.c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem), fMessageText);
        if (fCommitMsg.getAutoStage())
        {
            getSelectedTreeItem();
            applyGitCommandToFileTree(Cmd::getCommand(Cmd::Add));
            QString fResultStr;
            int result = execute(fCommitCommand, fResultStr);
            if (result != NoError)
            {
                fResultStr += tr("\nError %1 occurred").arg(result);
            }
            appendTextToBrowser(fCommitCommand + getLineFeed() + fResultStr);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
        }
        else
        {
            QString fResultStr;
            int result = execute(fCommitCommand, fResultStr);
            if (result != NoError)
            {
                fResultStr += tr("\nError %1 occurred").arg(result);
            }
            appendTextToBrowser(fCommitCommand + getLineFeed() + fResultStr);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
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
        QString first_git_repo =ui->treeSource->topLevelItem(0)->text(Column::FileName);
        mWorker.doWork(Work::DetermineGitMergeTools, QVariant(tr("git -C %1 difftool --tool-help").arg(first_git_repo)));
    }
}

void MainWindow::call_git_move_rename()
{
    getSelectedTreeItem();
    if (mContextMenuSourceTreeItem)
    {
        bool      fOk {false};
        const Type fType(static_cast<Type::TypeFlags>(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt()));
        const QString   fFileTypeName = Type::name(static_cast<Type::TypeFlags>(Type::FileType&fType.type()));
        const QFileInfo fPath(getItemFilePath(mContextMenuSourceTreeItem));

        QString       fOldName = mContextMenuSourceTreeItem->text(Column::FileName);
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
                fCommand = tr(fFormatCmd.c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem), fOldName, fNewGitName);
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
                    mContextMenuSourceTreeItem->setText(Column::FileName, fNewName);
                    updateTreeItemStatus(mContextMenuSourceTreeItem);
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
            type.setType(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt());
            QDir::setCurrent(getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(Column::FileName));
        }

        if (git_command.contains("-C %1"))
        {
            on_btnCloseText_clicked();
            QString result_str;
            git_command = tr(git_command.toStdString().c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem));
            QString cmd_option = get_git_command_option(type, command_flags, variant_list);
            if (cmd_option.size())
            {
                git_command += cmd_option;
            }
            int result = QMessageBox::Yes;
            if (message_box_text != ActionList::sNoCustomCommandMessageBox)
            {
                result = callMessageBox(message_box_text, "", getItemTopDirPath(mContextMenuSourceTreeItem), false);
            }

            if (result == QMessageBox::Yes || result == QMessageBox::YesToAll)
            {
                if (handleInThread() && !mWorker.isBusy())
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

            int fResult = QMessageBox::Yes;
            if (message_box_text != ActionList::sNoCustomCommandMessageBox)
            {
                fResult = callMessageBox(message_box_text, type.type_name(), mContextMenuSourceTreeItem->text(Column::FileName), type.is(Type::File));
            }

            if (fResult == QMessageBox::Yes || fResult == QMessageBox::YesToAll)
            {
                applyGitCommandToFileTree(git_command);
            }
        }
        else
        {
            QString fOption;
            git_command = tr(git_command.toStdString().c_str()).arg(fOption, "");
//            QString result;
//            execute(git_command, result);
//            ui->textBrowser->setText(result);
        }

        switch (variant_list[ActionList::Data::PostCmdAction].toUInt())
        {
            case Cmd::UpdateItemStatus:
                updateTreeItemStatus(mContextMenuSourceTreeItem);
                break;
            case Cmd::ParseHistoryText:
            {
                QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                ui->treeHistory->parseGitLogHistoryText(ui->textBrowser->toPlainText(), fSourceHook->indexFromItem(mContextMenuSourceTreeItem), getItemFilePath(mContextMenuSourceTreeItem), type.type());
                ui->textBrowser->setPlainText("");
#ifdef DOCKED_VIEWS
                showDockedWidget(ui->treeHistory);
#else
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
#endif
            }   break;
        }
    }
}

QString MainWindow::get_git_command_option(const Type& type, uint command_flags, const QVariantList& variant_list)
{
    QString option;
    QString cmd_add_on = variant_list[ActionList::Data::CmdAddOn].toString();
    if (cmd_add_on.size() )
    {
        if ((command_flags & ActionList::Flags::DependsOnStaged) != 0 && type.is(Type::GitStaged))
        {
            option += " ";
            option += cmd_add_on;
            option += " ";
        }
        if ((command_flags & ActionList::Flags::StashCmdOption) != 0 && type.is(Type::FileType) && !type.is(Type::Repository))
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

void MainWindow::parseGitStatus(const QString& aSource, const QString& aStatus, stringt2typemap& aFiles)
{
    const auto fLines = aStatus.split("\n");

    for (const QString& fLine : fLines)
    {
        const int     fStateSize = 2;
        const QString fState = fLine.left(fStateSize);
        auto          fRelativePath = fLine.mid(fStateSize).trimmed();

        if (fRelativePath.size())
        {
            Type fType;
            fType.translate(fState);
            while (fRelativePath.indexOf('"') != -1)
            {
                fRelativePath = fRelativePath.remove('"');
            }
            if (fType.is(Type::Repository))
            {
                aFiles[fRelativePath.toStdString()] = fType;
            }
            else
            {
                QString fFullPath = aSource + fRelativePath;
                if (fType.is(Type::GitRenamed) && fRelativePath.contains("->"))
                {
                    auto fPaths = fRelativePath.split(" -> ");
                    if (fPaths.size() > 1)
                    {
                        fFullPath = aSource + fPaths[1];
                        QFileInfo fFileInfo(fFullPath);
                        fType.translate(fFileInfo);
                        Type fDestinationType = fType;
                        fDestinationType.add(Type::GitMovedTo);
                        aFiles[fFullPath.toStdString()] = fDestinationType;
                        fType.add(Type::GitMovedFrom);
                        fFullPath = aSource + fPaths[0];
                    }
                }
                QFileInfo fFileInfo(fFullPath);
                fType.translate(fFileInfo);
                auto file_path = fFileInfo.filePath().toStdString();
                if (fType.is(Type::Folder))
                {
                    if (file_path.back() == '/')
                    {
                        file_path.resize(file_path.size()-1);
                    }
                }
                aFiles[file_path] = fType;
            }

            TRACE(Logger::trace, "%s: %s: %x", fState.toStdString().c_str(), fRelativePath.toStdString().c_str(), fType.type());
        }
    }
}

void MainWindow::deleteFileOrFolder()
{
    if (mContextMenuSourceTreeItem)
    {
        const QString fTopItemPath  = getItemTopDirPath(mContextMenuSourceTreeItem);
        const QString fItemPath     = getItemFilePath(mContextMenuSourceTreeItem);
        const Type fType(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt());
        if (callMessageBox(tr("Delete %1"), fType.type_name(), fItemPath) == QMessageBox::Yes)
        {
            const bool result = QFile::remove(fTopItemPath + "/" + fItemPath);
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
        }
    }
}

void MainWindow::on_treeSource_currentItemChanged(QTreeWidgetItem * /* current */, QTreeWidgetItem *previous)
{
    if (   mContextMenuSourceTreeItem
        && mContextMenuSourceTreeItem == previous)
    {
        mContextMenuSourceTreeItem = 0;
    }
}

void MainWindow::find_item_in_treeSource(const QString& git_root, const QString& filepath)
{
    QStringList items = filepath.split("/");
    auto list = ui->treeSource->findItems(git_root, Qt::MatchExactly);
    if (!list.empty() )
    {
        auto* found = list[0];
        for (const QString& item : items)
        {
            int32_t i;
            if (found != nullptr)
            {
                const auto children = found->childCount();
                for (i=0; found != nullptr && i<children; ++i)
                {
                    if (found->child(i)->text(0) == item)
                    {
                        found = found->child(i);
                        break;
                    }
                }
                if (i == children) found = nullptr;
            }
        }
        if (found)
        {
            found->setSelected(true);
            auto* parent = found->parent();
            while (parent)
            {
                ui->treeSource->setItemExpanded(parent, true);
                parent = parent->parent();
            }
            ui->treeSource->scrollToItem(found);
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
        const QString fTopItemPath  = getItemTopDirPath(mContextMenuSourceTreeItem);
        const QString fItemPath     = getItemFilePath(mContextMenuSourceTreeItem);
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

