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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#if CORE5COMPAT == 1
#include <QTextCodec>
#endif
#else
#include <QTextCodec>
#endif

using namespace std;
using namespace git;

bool MainWindow::iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir, QTreeWidgetItem* aParentItem)
{
    bool result  = false;
    if (aParentItem)
    {
        auto check_state = ui->treeSource->mUseSourceTreeCheckboxes ? aParentItem->checkState(QSourceTreeWidget::Column::FileName) : Qt::Checked;

        if ( is_any_equal_to(check_state, Qt::Checked, Qt::PartiallyChecked)
            || mCurrentTask == Work::InsertPathFromCommandString)
        {
            const QVariant& fIsDir = aParentItem->data(QSourceTreeWidget::Column::FileName, QSourceTreeWidget::Role::isDirectory);
            QString fileName = aParentItem->text(QSourceTreeWidget::Column::FileName);
            QString source;
            QString result_str;
            if (!fileName.startsWith(QDir::separator()) && !aSourceDir->isEmpty())
            {
                fileName =  QDir::separator() + fileName;
            }
            source      = *aSourceDir + fileName;

            if (fIsDir.toBool())
            {
                Type type;
                const QVariant fVariant = aParentItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter);
                if (fVariant.isValid())
                {
                    type = Type(fVariant.toUInt());
                }

                if (  is_any_equal_to(mCurrentTask, Work::ApplyGitCommand, Work::ApplyCommand)
                    && check_state == Qt::Checked)
                {
                    const QString cmd = applyGitCommandToFilePath(source, mGitCommand, result_str);
                    if (!cmd.isEmpty())
                    {
                        appendTextToBrowser(cmd + getLineFeed() + result_str + getLineFeed(), true);
                    }
                    return false;
                }
                int count_ok = 0;
                for (int childI_iem=0; childI_iem < aParentItem->childCount(); ++childI_iem)
                {
                    result = iterateTreeItems(aSourceTree, &source, aParentItem->child(childI_iem));
                    if (result) ++count_ok;
                }
                switch (mCurrentTask)
                {
                case Work::InsertPathFromCommandString:
                {
                    const int index = mGitCommand.indexOf(source);
                    if (index != -1)
                    {
                        TRACEX(Logger::notice, "Found dir: "<<  fileName<< ": " << count_ok);
                        if (count_ok == 0)
                        {
                            QStringList child;
                            child.append(mGitCommand.mid(source.size() + 1));
                            aParentItem->addChild(new QTreeWidgetItem(child));
                        }
                        return 1;
                    }
                    return 0;
                } break;
                case Work::ShowAdded: case Work::ShowDeleted: case Work::ShowUnknown: case Work::ShowStaged: case Work::ShowUnMerged:
                case Work::ShowModified: case Work::ShowAllGitActions: case Work::ShowSelected: case Work::ShowIgnored:
                {
                    if (ui->ckHideEmptyParent->isChecked())
                    {
                        result = getShowTypeResult(type);
                        if (result == false)
                        {
                            result = count_ok != 0;
                        }
                        aParentItem->setHidden(!result); // true means visible
                    }
                    else
                    {
                        aParentItem->setHidden(false);
                    }
                }   break;
                case Work::ShowAllFiles:
                    aParentItem->setHidden(false);
                    result = true;
                    break;
                case Work::None: case Work::Last:
                case Work::ApplyGitCommand: case Work::ApplyCommand: case Work::ApplyCommandRecursive:
                case Work::DetermineGitMergeTools: case Work::AsynchroneousCommand:
                    /// NOTE: not handled here
                    break;

                }
            }
            else
            {
                const QVariant variant = aParentItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter);
                if (variant.isValid())
                {
                    const Type type(variant.toUInt());
                    switch (mCurrentTask)
                    {
                    case Work::ApplyGitCommand:
                    case Work::ApplyCommand:
                    case Work::ApplyCommandRecursive:
                    {
                        source = "\"" + source + "\"";
                        QString cmd = applyGitCommandToFilePath(source, mGitCommand, result_str);
                        if (!cmd.isEmpty())
                        {
                            appendTextToBrowser(cmd + getLineFeed() + result_str, true);
                        }
                        result = cmd.size() != 0;
                    }   break;
                    case Work::ShowAllFiles:
                        aParentItem->setHidden(false);
                        result = true;
                        break;
                    case Work::ShowAllGitActions: case Work::ShowAdded: case Work::ShowDeleted: case Work::ShowModified:
                    case Work::ShowUnknown: case Work::ShowUnMerged: case Work::ShowStaged: case Work::ShowIgnored:
                        result = getShowTypeResult(type);
                        aParentItem->setHidden(!result); // true means visible
                        break;
                    case Work::ShowSelected:
                        result = aParentItem->isSelected();
                        aParentItem->setHidden(!result); // true means visible
                        break;
                    case Work::None: case Work::Last: case Work::DetermineGitMergeTools:
                    case Work::InsertPathFromCommandString: case Work::AsynchroneousCommand:
                        /// NOTE: not handled here
                        return false;
                    }
                }
            }
        }
        else
        {
            TRACEX(Logger::info, "Not copying unselected file %s" << aParentItem->text(QSourceTreeWidget::Column::FileName));
            result = true; // this is not an error
        }
    }
    else
    {
        result = true;
        for (int fIndex = 0; fIndex < aSourceTree.topLevelItemCount(); ++fIndex)
        {
            const QString source_dir("");
            result &= iterateTreeItems(aSourceTree, &source_dir, aSourceTree.topLevelItem(fIndex));
        }
    }
    return result;
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
    case Work::ShowIgnored:
        fResult = fType.is(Type::GitIgnored);
        break;
    case Work::ApplyGitCommand: case Work::ApplyCommand: case Work::ApplyCommandRecursive: case Work::AsynchroneousCommand:
    case Work::DetermineGitMergeTools: case Work::InsertPathFromCommandString:
    case Work::ShowAllFiles: case Work::ShowSelected: case Work::None: case Work::Last:
        /// NOTE: not handled here
        break;
    }
    return fResult;
}

void MainWindow::insertSourceTree(const QDir& source_dir, int item)
{
    m_loading_repository = true;
    ui->treeSource->mGitIgnore.clear();
    QString result_string;
    applyGitCommandToFilePath(source_dir.path(), Cmd::getCommand(Cmd::GetStatusAll), result_string);

    appendTextToBrowser(tr("Repository: ") + source_dir.path(), (item == 0 && ui->treeSource->topLevelItemCount() == 0) ? false : true);
    appendTextToBrowser(result_string, true);
    appendTextToBrowser("", true);

    stringt2typemap check_map;
    parseGitStatus(source_dir.path() +  QDir::separator(), result_string, check_map);
    ui->treeSource->mInseredIgnoredFiles = ui->ckIgnoredFiles->isChecked();
    ui->treeSource->insertItem(source_dir, *ui->treeSource, nullptr);

    for (const auto& fItem : std::as_const(check_map))
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

    auto action = mActions.getAction(Cmd::BranchList);
    if (action && ui->treeSource->topLevelItemCount() > 0)
    {
        mContextMenuSourceTreeItem = ui->treeSource->topLevelItem(ui->treeSource->topLevelItemCount()-1);
        ui->treeSource->setCurrentItem(mContextMenuSourceTreeItem);
        ui->treeSource->setFocus();
        action->trigger();
    }
    m_loading_repository = false;
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
    const QString fSourcePath = QFileDialog::getExistingDirectory(this, tr("Select SourceFiles"), mDefaultSourcePath);
    if (fSourcePath.size() > 1)
    {
        QFileInfo info(fSourcePath);
        mDefaultSourcePath = info.absolutePath();
        insertSourceTree(initDir(fSourcePath), ui->treeSource->topLevelItemCount()+1);
    }
}

void MainWindow::removeGitSourceFolder()
{
    auto close_all_subfiles = [&](auto item)
    {
        QString repo_path = item->text(QSourceTreeWidget::Column::FileName);
        if (mCloseAllFilesOfRepository)
        {
            mCloseFileFilter = repo_path;
            btnCloseText_clicked(Editor::All);
            mCloseFileFilter.clear();
        }
        /// NOTE: remove according branches and histories
        /// branches and histories belong to the repository and use references of the repository tree
        for (int i=0; i<ui->treeBranches->topLevelItemCount(); ++i)
        {
            if (ui->treeBranches->topLevelItem(i)->text(QBranchTreeWidget::Column::Text).contains(repo_path))
            {
                ui->treeBranches->takeTopLevelItem(i);
                --i;
            }
        }
        for (int i=0; i<ui->treeHistory->topLevelItemCount(); ++i)
        {
            if (ui->treeHistory->topLevelItem(i)->text(QBranchTreeWidget::Column::Text).contains(repo_path))
            {
                ui->treeHistory->takeTopLevelItem(i);
                --i;
            }
        }
        for (int i=0; i<ui->treeStash->topLevelItemCount(); ++i)
        {
            if (ui->treeStash->topLevelItem(i)->text(QStashTreeWidget::Column::Description).contains(repo_path))
            {
                ui->treeStash->takeTopLevelItem(i);
                --i;
            }
        }
        for (int i=0; i<ui->treeFindText->topLevelItemCount(); ++i)
        {
            if (ui->treeFindText->topLevelItem(i)->text(FindColumn::RepositoryRoot).contains(repo_path))
            {
                ui->treeFindText->takeTopLevelItem(i);
                --i;
            }
        }

    };
    deleteTopLevelItemOfSelectedTreeWidgetItem(*ui->treeSource, close_all_subfiles);
    mContextMenuSourceTreeItem = nullptr;
}

void MainWindow::OpenFile()
{
    const QString fSourcePath = QFileDialog::getOpenFileName(this, tr("Select file to open"));
    if (!fSourcePath.isEmpty())
    {
        open_file(fSourcePath);
    }
}

void MainWindow::SaveFileAs()
{
    const QString fDestinationPath = QFileDialog::getSaveFileName(this, tr("Enter file new name"));
    if (!fDestinationPath.isEmpty())
    {
        QWidget* active_widget = get_active_editable_widget();
        if (set_file_path(active_widget, fDestinationPath))
        {
            btnStoreText_clicked();
            on_DockWidgetActivated(dynamic_cast<QDockWidgetX*>(active_widget->parentWidget()));
        }
    }
}

void MainWindow::on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int /* column */ )
{
    m_tree_source_item_double_clicked = true;
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
    m_tree_source_item_double_clicked = false;
}

void MainWindow::open_file_externally()
{
    if (mContextMenuSourceTreeItem)
    {
        const QString   file_name  = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        const QFileInfo file_info(file_name);
        const QString   file_extension = file_info.suffix().toLower();
        QString open_file_cmd = mExternalFileOpenCmd + " \"" + file_name + "\"";
        if (mExternalFileOpenExt.contains(file_extension))
        {
            auto program_parameter = mExternalFileOpenExt.value(file_extension);
            if (program_parameter.size())
            {
                open_file_cmd = program_parameter + " \"" + file_name + "\"";
            }
        }
        int result = system(open_file_cmd.toStdString().c_str());
        if (result != 0)
        {
            TRACEX(Logger::to_browser, "could not open " << open_file_cmd << " error " << result);
        }
    }
}

#ifdef USE_BOOST
void MainWindow::open_file(const QString& file_path, boost::optional<int> line_number, bool reopen_file)
#else
void MainWindow::open_file(const QString& file_path, std::optional<int> line_number, bool reopen_file)
#endif
{
    btnCloseText_clicked(Editor::Active);
    const QFileInfo file_info(file_path);
    QString         file_extension = file_info.suffix().toLower();

    if (mExternalFileOpenExt.contains(file_extension))
    {
        QString open_file_cmd = mExternalFileOpenCmd + " " + file_path;
        auto program_parameter = mExternalFileOpenExt.value(file_extension);
        if (program_parameter.size())
        {
            open_file_cmd = program_parameter + " \"" + file_path + "\"";
        }
        int result = system(open_file_cmd.toStdString().c_str());
        if (result != 0)
        {
            TRACEX(Logger::to_browser, "could not open " << open_file_cmd << " error " << result);
        }
        else
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        std::optional<QStringConverter::Encoding> encoding;
        bool is_binary_file = qbinarytableview::is_binary(file, encoding);
#else
        bool is_binary_file = qbinarytableview::is_binary(file);
#endif
        switch (static_cast<OpenFileAs>(ui->comboOpenFile->currentIndex()))
        {
        case OpenFileAs::Text:
            is_binary_file = false;
            break;
        case OpenFileAs::Binary:
            is_binary_file = true;
            break;
        case OpenFileAs::Automatic:
        default:
            break;
        }
        code_browser * text_browser = dynamic_cast<code_browser*>(get_active_editable_widget(file_path));
        if (!text_browser && !is_binary_file)
        {
            text_browser = create_new_text_browser(file_path);
            reopen_file = true;
        }

        QWidget* widget_to_be_shown = text_browser;
        ui->labelFilePath->setText(file_path);
        if (file_extension == "txt" && file_path.contains("CMakeLists.txt"))
        {
            file_extension = "cmake";
        }

        if (text_browser && reopen_file)
        {
            text_browser->set_file_path(file_path);
            if (!text_browser->hasExtension(file_extension))
            {
                text_browser->reset();
            }
        }

        if (is_binary_file)
        {
            updateSelectedLanguage(tr("binary"));
            ui->tableBinaryView->set_binary_data(file.readAll());
            ui->tableBinaryView->set_file_path(file_path);
            widget_to_be_shown = ui->tableBinaryView;
            showDockedWidget(mBinaryValuesView.data());
        }
        else if (reopen_file && text_browser)
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)  && CORE5COMPAT == 0
            if (encoding.has_value())
            {
                QStringDecoder decode_string;
                TRACEX(Logger::to_browser, "opened file has enncoding" << decode_string.nameForEncoding(encoding.value()));
            }
            if (!text_browser->hasExtension(file_extension))
            {
                text_browser->setExtension(file_extension);
            }
            if (ui->ckRenderGraphicFile->isChecked())
            {
                if (encoding.has_value())
                {
                    QStringDecoder decode_string(encoding.value());
                    text_browser->set_do_preview(false);
                    if (file_extension.contains("md", Qt::CaseInsensitive))
                    {
                        text_browser->setMarkdown(decode_string(file.readAll()));
                    }
                    else
                    {
                        text_browser->setText(decode_string(file.readAll()));
                    }
                    text_browser->set_encoding(encoding);
                }
                else if (file_extension.compare("md", Qt::CaseInsensitive) == 0)
                {
                    text_browser->setMarkdown(file.readAll());
                }
                else
                {
                    text_browser->setText(file.readAll());
                }
            }
            else
            {
                text_browser->set_do_preview(true);
                if (encoding.has_value())
                {
                    QStringDecoder decode_string(encoding.value());
                    text_browser->setPlainText(decode_string(file.readAll()));
                    text_browser->set_encoding(encoding);
                }
                else
                {
                    text_browser->setPlainText(file.readAll());
                }
            }
#else
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
            if (!text_browser->hasExtension(file_extension))
            {
                text_browser->setExtension(file_extension);
            }
            if (ui->ckRenderGraphicFile->isChecked())
            {
                text_browser->set_do_preview(false);
                if (codec_selected)
                {
                    if (file_extension.contains("md", Qt::CaseInsensitive))
                    {
                        text_browser->setMarkdown(QString::fromLocal8Bit(file.readAll()));
                    }
                    else if (file_extension.contains("htm", Qt::CaseInsensitive))
                    {
                        text_browser->setHtml(QString::fromLocal8Bit(file.readAll()));
                    }
                    else
                    {
                        text_browser->setText(QString::fromLocal8Bit(file.readAll()));
                    }
                }
                else
                {
                    if (file_extension.contains("md", Qt::CaseInsensitive))
                    {
                        text_browser->setMarkdown(file.readAll());
                    }
                    else if (file_extension.contains("htm", Qt::CaseInsensitive))
                    {
                        text_browser->setHtml(file.readAll());
                    }
                    else
                    {
                        text_browser->setText(file.readAll());
                    }
                }
            }
            else
            {
                text_browser->set_do_preview(true);
                if (codec_selected)
                {
                    text_browser->setPlainText(QString::fromLocal8Bit(file.readAll()));
                }
                else
                {
                    text_browser->setPlainText(file.readAll());
                }
            }
#endif

            text_browser->set_changed(false);
            textBrowserChanged(false);
        }

        showDockedWidget(widget_to_be_shown);

        if (text_browser)
        {
            if (line_number)
            {
                text_browser->go_to_line(*line_number);
            }
        }
    }
}

void MainWindow::updateRepositoryStatus(bool clear_viewer)
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
        if (clear_viewer)
        {
            btnCloseText_clicked(Editor::Viewer);
        }
        auto item = ui->treeSource->topLevelItem(selected_item);

#if 1
        auto list = ui->treeSource->saveExpandedState();
#else
        auto list = ui->treeSource->saveExpandedState(item);
#endif

        ui->treeSource->removeItemWidget(item, 0);
        delete item;

        mContextMenuSourceTreeItem = nullptr;
        insertSourceTree(initDir(fSourceDirs[selected_item]), selected_item);
        QList<QTreeWidgetItem*> found_items = ui->treeSource->findItems(fSourceDirs[selected_item], Qt::MatchExactly);
        for (const auto&found_item : found_items)
        {
            if (found_item->text(QSourceTreeWidget::Column::FileName) == fSourceDirs[selected_item])
            {
                mContextMenuSourceTreeItem = found_item;
                ui->treeSource->setCurrentItem(mContextMenuSourceTreeItem);
            }
        }
#if 1
        ui->treeSource->restoreExpandedState(list);
#else
        ui->treeSource->restoreExpandedState(list, mContextMenuSourceTreeItem);
#endif
    }
    else
    {
        if (file_name.size())
        {
            QFileInfo info(file_name);
            if (info.isFile())
            {
                QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
                for (QDockWidget* dock_widget : dock_widgets)
                {
                    code_browser* cb = dynamic_cast<code_browser*>(get_widget(dock_widget));
                    if (cb && cb->get_file_path() == file_name)
                    {
                        open_file(file_name, {}, true);
                        return;
                    }
                }
            }
            else
            {
                ui->treeSource->insertItem(file_name, *ui->treeSource, mContextMenuSourceTreeItem, txt::no_double_entries);
                return;
            }
        }
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
        menu.exec(check_screen_position(pos, true, ui->treeSource));
    }
    else
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuEmptySourceTree);
        menu.exec(check_screen_position(pos, true, ui->treeSource));
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
    case ComboShowItems::GitIgnored:    mCurrentTask = Work::ShowIgnored;       break;
    }
    iterateTreeItems(*ui->treeSource);
    mCurrentTask = Work::None;
}

void MainWindow::on_treeSource_itemClicked(QTreeWidgetItem *item, int /* column */ )
{
    if (item)
    {
        mContextMenuSourceTreeItem = item;
        QTreeWidgetItem*top_item = getTopLevelItem(*ui->treeSource, item);
        if (top_item)
        {
            ui->treeBranches->select_branch(top_item->text(QSourceTreeWidget::Column::FileName));
        }
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

void  MainWindow::applyCommandToFileTree(const QString& aCommand)
{
    if (mContextMenuSourceTreeItem)
    {
        if (!handleInThread())
        {
            btnCloseText_clicked(Editor::Viewer);
        }
        mGitCommand = aCommand;
        QString       parent_path = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem->parent());
        if (mGitCommand.startsWith(txt::git))
        {
            mCurrentTask = Work::ApplyGitCommand;
        }
        else
        {
            const QString ftlp = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(QSourceTreeWidget::Column::FileName);
            QString  file_path = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
            const QFileInfo info(file_path);
            file_path = file_path.mid(ftlp.length()+1);

            mCurrentTask = Work::ApplyCommand;
            bool set_current_path = mGitCommand.contains(txt::set_currentpath);
            bool cmd_is_recursive = mGitCommand.contains(txt::cmd_recursive);
            if (mGitCommand.contains("%1"))
            {
                mGitCommand.replace("%1", file_path);
            }
            if (mGitCommand.contains(txt::root_path))
            {
                mGitCommand.replace(txt::root_path, ftlp);
            }
            if (mGitCommand.contains(txt::file_basename))
            {
                mGitCommand.replace(txt::file_basename, info.baseName());
            }
            if (mGitCommand.contains(txt::file_name))
            {
                mGitCommand.replace(txt::file_name, info.fileName());
            }
            if (mGitCommand.contains(txt::file_ext))
            {
                mGitCommand.replace(txt::file_ext, info.suffix());
            }
            if (mGitCommand.contains(txt::relative_path))
            {
                mGitCommand.replace(txt::relative_path, file_path);
            }

            if (set_current_path)
            {
                QDir::setCurrent(ftlp + "/" + file_path);
                mGitCommand.replace(txt::set_currentpath, "");
                file_path.clear();
            }
            if (cmd_is_recursive)
            {
                mGitCommand.replace(txt::cmd_recursive, "");
                mCurrentTask = Work::ApplyCommandRecursive;
            }
        }
        iterateTreeItems(*ui->treeSource, &parent_path, mContextMenuSourceTreeItem);
        mGitCommand.clear();
        mCurrentTask = Work::None;
    }
}

void  MainWindow::call_git_commit()
{
    CommitMessage commit_message;
    ensure_dialog_on_same_screen(&commit_message, this);
    if (commit_message.exec() == QDialog::Accepted)
    {
        btnCloseText_clicked(Editor::Viewer);
        const QString message_text   = commit_message.getMessageText();
        string        command        = Cmd::getCommand(Cmd::Commit).toStdString();
        const QString commit_command = tr(command.c_str()).arg(ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem), message_text);
        QString result_str;
        if (commit_message.getAutoStage())
        {
            getSelectedTreeItem();
            mWorker.setOnceBusy();
            auto* action = mActions.getAction(Cmd::Add);
            if (action) action->trigger();

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
            auto* action = mActions.getAction(Cmd::Push);
            if (action) action->trigger();
        }
    }
}

void MainWindow::call_git_clone()
{
    const QAction *action        = qobject_cast<QAction *>(sender());
    const QString  git_command   = action->statusTip();
    const QString  clone_title   = tr("Clone git Repository");
    const QString  git_source    = QInputDialog::getText(this, clone_title, tr("Enter git repository address:"));
    const QString  base_name     = QFileInfo(git_source).baseName();
    const QString  destination   = QFileDialog::getExistingDirectory(this, tr("Select destination for repository \"%1\"").arg(base_name), mDefaultSourcePath);
    const QString  clone_command = tr(git_command.toStdString().c_str()).arg(git_source);


#if 0   /// TODO: handle command within thread.

    QVariantMap workmap;
    workmap.insert(Worker::command, clone_command);
    workmap.insert(Worker::work   , INT(Work::ApplyGitCommand));
    workmap.insert(Worker::action , git::Cmd::InsertRepository);
    QString insert_source_dir = destination+ "/" + base_name;
    workmap.insert(Worker::result , insert_source_dir);
    workmap.insert(Worker::repository, destination);
    mWorker.doWork(QVariant(workmap));

#else

    QDir::setCurrent(destination);

    QString result_str;
    int result = execute(clone_command, result_str);
    if (result != NoError)
    {
        result_str += tr("\nError %1 occurred").arg(result);
    }
    appendTextToBrowser(clone_command + getLineFeed() + result_str);

    QString insert_source_dir = destination+ "/" + base_name;
    if (!QFileInfo(insert_source_dir).exists())
    {
        insert_source_dir = destination+ "/" + git_source.mid(git_source.indexOf(":")+1);
    }

    if (result == NoError )
    {
        insertSourceTree(initDir(insert_source_dir), ui->treeSource->topLevelItemCount()+1);
    }
#endif
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
        auto*action = mActions.getAction(Cmd::KillBackgroundThread);
        if (action) action->setEnabled(true);
        QString first_git_repo =ui->treeSource->topLevelItem(0)->text(QSourceTreeWidget::Column::FileName);
        QVariantMap workmap;
        workmap.insert(Worker::command, tr("git -C %1 difftool --tool-help").arg(first_git_repo));
        workmap.insert(Worker::work, INT(Work::DetermineGitMergeTools));
        mWorker.doWork(QVariant(workmap));
    }
}


void MainWindow::call_git_move_rename(QTreeWidgetItem* dropped_target, bool *was_dropped)
{
    getSelectedTreeItem();
    if (mContextMenuSourceTreeItem)
    {
        int         result_copy = 2;
        bool        git_move_possible = true;
        bool        same_repository   = true;
        QString     dialog_title;
        QString     edit_label;
        QStringList button_names = {tr("Cancel")};
        QString     old_name = mContextMenuSourceTreeItem->text(QSourceTreeWidget::Column::FileName);

        const Type      type(Type::type(mContextMenuSourceTreeItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
        const QString   file_type_name = Type::name(Type::type(Type::FileType&type.type()));
        const QFileInfo path(ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem));

        if (dropped_target)
        {
            same_repository   = ui->treeSource->getItemTopDirPath(dropped_target) == ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem);
            git_move_possible = same_repository && !type.is(Type::GitUnTracked) && !type.is(Type::GitIgnored);
            old_name     = ui->treeSource->getItemFilePath(dropped_target);
            dialog_title = tr("Copy or %2 Move %1").arg(file_type_name, (git_move_possible? "Git" : ""));
            edit_label   = tr("Target for the %2 \"%1\":\n").arg(path.fileName(), file_type_name);
            button_names.append(tr("Move"));
            button_names.append(tr("Copy"));
            if (!same_repository)
            {
                mContextMenuSourceTreeItem = dropped_target;
            }
        }
        else
        {
            dialog_title  = tr("Rename %1").arg(file_type_name);
            button_names.append(tr("Rename"));
        }

        QString new_name;
        int result = getInputText(dialog_title, edit_label, old_name, button_names, new_name);
        if (result != QDialog::Rejected && !new_name.isEmpty())
        {
            /// TODO: support also windows commands
#ifdef __linux__
            const QString copy_cmd = "cp";
            const QString move_cmd = "mv";
#else
            const QString copy_cmd = "copy";
            const QString move_cmd = "rename";
#endif
            QString     command;
            bool        moved = false;
            if (result == result_copy)
            {
                if (new_name.contains("/"))
                {
                    const QString new_copied = "\"" + new_name + "\"";
                    moved   = true;
                    old_name = "\"" + path.filePath() + "\"";
                    command = tr("%1 %2 %3").arg(copy_cmd, old_name, new_copied);
                }
            }
            else if (git_move_possible)
            {
                const string format_cmd = Cmd::getCommand(Cmd::MoveOrRename).toStdString();
                const QString new_renamed = "\"" + new_name + "\"";
                if (new_renamed.contains("/"))
                {
                    moved   = true;
                    old_name = "\"" + path.filePath() + "\"";
                    command = tr(format_cmd.c_str()).arg(ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem), old_name, new_renamed);
                }
                else
                {
                    old_name = "\"" + old_name + "\"";
                    command  = tr(format_cmd.c_str()).arg(path.absolutePath(), old_name, new_renamed);
                }
            }
            else
            {
                if (new_name.contains("/"))
                {
                    const QString new_renamed = "\"" + new_name + "\"";
                    moved   = true;
                    old_name = "\"" + path.filePath() + "\"";
                    command = tr("%3 %1 %2").arg(old_name, new_renamed, move_cmd);
                }
                else
                {
                    old_name = "\"" + old_name + "\"";
                    command  = tr("%4 \"%1/%2\" \"%1/%3\"").arg(path.absolutePath(),old_name, new_name, move_cmd);
                }
            }

            QString result_str;
            int exe_result = execute(command, result_str);
            if (exe_result == NoError)
            {
                if (moved)
                {
#if 1
                    perform_post_cmd_action(Cmd::UpdateRootItemStatus);
#else
                    QVariantList parameter;
                    parameter.append(QVariant(Cmd::UpdateRootItemStatus));
                    int id = startTimer(1000);
                    mTimerTask[id] = parameter;
#endif
                }
                else
                {
                    mContextMenuSourceTreeItem->setText(QSourceTreeWidget::Column::FileName, new_name);
                    updateTreeItemStatus(mContextMenuSourceTreeItem);
                }
                if (was_dropped)
                {
                    *was_dropped = true;
                }
            }
            else
            {
                appendTextToBrowser(command + result_str + tr("\nError %1 occurred").arg(exe_result));
            }
        }
        mContextMenuSourceTreeItem = nullptr;
    }
}

void MainWindow::check_set_current_path(QString & git_command)
{
    if (mContextMenuSourceTreeItem)
    {
        bool set_current_path = git_command.contains(txt::set_currentpath);
        if (set_current_path)
        {
            QString       fFilePath = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem->parent());
            const QString ftlp      = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(QSourceTreeWidget::Column::FileName);
            fFilePath = fFilePath.mid(ftlp.length()+1);

            QDir::setCurrent(ftlp + "/" + fFilePath);
            git_command.replace(txt::set_currentpath, "");
        }
    }
}

void MainWindow::perform_custom_command()
{
    const QAction     *action        = qobject_cast<QAction *>(sender());
    const QVariantList variant_list  = action->data().toList();
    const uint         command_flags = variant_list[ActionList::Data::Flags].toUInt();
    const bool         hide          = command_flags&ActionList::Flags::NoHide ? false : true;

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
            bool handle_in_thread = handleInThread();
            if (!handle_in_thread)
            {
                btnCloseText_clicked(Editor::Viewer);
            }
            QString repository = ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem);
            QString result_str;
            git_command = tr(git_command.toStdString().c_str()).arg(repository);
            QString cmd_option = get_git_command_option(type, command_flags, variant_list);
            if (cmd_option.size())
            {
                git_command += cmd_option;
            }

            int result = callMessageBox(message_box_text, "", repository, false);
            if (result & (QMessageBox::Yes|QMessageBox::YesToAll))
            {
                check_set_current_path(git_command);
                if (handle_in_thread)
                {
                    auto*action_kbgcmd = mActions.getAction(Cmd::KillBackgroundThread);
                    if (action_kbgcmd)
                    {
                        action_kbgcmd->setEnabled(true);
                        QVariantMap workmap;
                        workmap.insert(Worker::repository, repository);
                        workmap.insert(Worker::command_id, mActions.findID(action_kbgcmd));
                        workmap.insert(Worker::command, git_command);
                        workmap.insert(Worker::action , variant_list[ActionList::Data::PostCmdAction].toUInt());
                        workmap.insert(Worker::flags  , variant_list[ActionList::Data::Flags].toUInt());
                        workmap.insert(Worker::work   , INT(Work::ApplyGitCommand));
                        mWorker.doWork(QVariant(workmap));
                        action_kbgcmd->setToolTip(mWorker.getBatchToolTip());
                        if (mOutput2secondTextView && mBackgroundTextView)
                        {
                            showDockedWidget(mBackgroundTextView.data());
                        }
                    }
                }
                else
                {
                    result = execute(git_command, result_str, hide);
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
            if (git_command.startsWith(txt::git))
            {
                QString option = get_git_command_option(type, command_flags, variant_list);
                git_command = tr(git_command.toStdString().c_str()).arg(option, "%1");
            }
            QString name = mContextMenuSourceTreeItem->text(QSourceTreeWidget::Column::FileName);
            int fResult = callMessageBox(message_box_text, type.type_name(), name, type.is(Type::File));
            if (fResult & (QMessageBox::Yes|QMessageBox::YesToAll))
            {
                if (fResult & QMessageBox::Apply)
                {
                    git_command = git_command.arg(name);
                }
                applyCommandToFileTree(git_command);
            }
        }
        else
        {
            QString fOption;
            git_command = tr(git_command.toStdString().c_str()).arg(fOption, "");
        }

        if (!(command_flags & ActionList::Flags::CallInThread))
        {
            perform_post_cmd_action(static_cast<Cmd::ePostAction>(variant_list[ActionList::Data::PostCmdAction].toUInt()), type, mActions.findID(action));
        }
    }
}

void MainWindow::perform_post_cmd_action(Cmd::ePostAction post_cmd, const git::Type& type, Cmd::eCmd cmd)
{
    switch (post_cmd)
    {
    case Cmd::UpdateItemStatus:
        updateTreeItemStatus(mContextMenuSourceTreeItem);
        break;
    case Cmd::UpdateRootItemStatus:
        mContextMenuSourceTreeItem = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem);
        updateRepositoryStatus(cmd == Cmd::ClearView);
        break;
    case Cmd::UpdateRepository:
        updateTreeItemStatus(getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem));
        break;
    case Cmd::UpdateRepositorySubFolder:
        if (mContextMenuSourceTreeItem)
        {
             const QString path = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
             ui->treeSource->insertItem(path, *ui->treeSource, mContextMenuSourceTreeItem, txt::no_double_entries);
        }
        break;
    case Cmd::ParseHistoryText:
    {
        QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
        ui->treeHistory->parseGitLogHistoryText(ui->textBrowser->toPlainText(), fSourceHook->indexFromItem(mContextMenuSourceTreeItem), ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem), type.type());
        ui->textBrowser->setPlainText("");
        showDockedWidget(ui->treeHistory);
    }   break;
    case Cmd::ParseBlameText:
    {
        const QString   file_name = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
        const QFileInfo file_info(file_name);
        ui->textBrowser->setExtension(file_info.suffix().toLower());
        ui->textBrowser->parse_blame(ui->textBrowser->toPlainText());
    }   break;
    case Cmd::UpdateStash:
        if (cmd != git::Cmd::StashList)
        {
            ui->treeStash->clear();
            auto *action = mActions.getAction(git::Cmd::StashList);
            if (action) action->trigger();
        }
        break;
    case Cmd::ParseBranchListText:
    case Cmd::ParseStashListText:
    case Cmd::InsertRepository:
    case Cmd::DoNothing:
        break;
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
        for (const QString & item : std::as_const(list))
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
    btnCloseText_clicked(Editor::Viewer);

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
        if (callMessageBox(tr("Delete %1"), fType.type_name(), fItemPath) & QMessageBox::Yes)
        {
            bool result = false;
            if (fType.is(Type::Folder))
            {
                QDir dir;
                dir.setPath(fItemPath);
                result = dir.removeRecursively();
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
                    appendTextToBrowser(get_errno_text(), true);
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
            fileInfo.setFile(fItemPath);
        }
        else
        {
            fileInfo.setFile(fTopItemPath + QDir::separator() + fItemPath);
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
            if (!mFileCopyMimeType.isEmpty())
            {
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
}

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
            file_info.setFile(file_path);
        }
        else
        {
            file_info.setFile(repository + QDir::separator() + file_path);
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
#if QT_DEPRECATED_SINCE(5, 15)
        auto dateformat = "dd.MM.yyyy hh:mm:ss";
        if (file_info.birthTime().isValid())
        {
            information << "\nCreated: " << file_info.birthTime().toString(dateformat).toStdString();
        }
        if (file_info.metadataChangeTime().isValid())
        {
            information << "\nMetadata Access: " << file_info.metadataChangeTime().toString(dateformat).toStdString();
        }
        if (file_info.lastModified().isValid())
        {
            information << "\nModified: " << file_info.lastModified().toString(dateformat).toStdString();
        }
        if (file_info.lastRead().isValid())
        {
            information << "\nLast Access: " << file_info.lastRead().toString(dateformat).toStdString();
        }
#else
        auto dateformat = Qt::SystemLocaleShortDate;
        if (file_info.created().isValid())
        {
            information << "\nCreated: " << file_info.created().toString(dateformat).toStdString();
        }
        if (file_info.birthTime().isValid())
        {
            information << "\nCreated: " << file_info.birthTime().toString(dateformat).toStdString();
        }
        if (file_info.metadataChangeTime().isValid())
        {
            information << "\nMetadata Access: " << file_info.metadataChangeTime().toString(dateformat).toStdString();
        }
        if (file_info.lastModified().isValid())
        {
            information << "\nModified: " << file_info.lastModified().toString(dateformat).toStdString();
        }
        if (file_info.lastRead().isValid())
        {
            information << "\nLast Access: " << file_info.lastRead().toString(dateformat).toStdString();
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
        ensure_dialog_on_same_screen(&message_box, this);
        message_box.exec();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *even)
{
    for (const QUrl &url : even->mimeData()->urls())
    {
        QString fileName = url.toLocalFile();
        open_file(fileName);
    }
}

