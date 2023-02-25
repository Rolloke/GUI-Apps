#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "binary_values_view.h"
#include "code_browser.h"

#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>


using namespace std;
using namespace git;


void MainWindow::appendTextToBrowser(const QString& aText, bool append, const QString ext)
{
    if (aText.size())
    {
        if (!append)
        {
            on_btnCloseText_clicked(Editor::Viewer);
        }
        ui->textBrowser->setExtension(ext);
        ui->textBrowser->insertPlainText(aText + getLineFeed());
        ui->textBrowser->textCursor().movePosition(QTextCursor::End);
        showDockedWidget(ui->textBrowser);
    }
}

void MainWindow::selectTextBrowserLanguage()
{
    QMenu menu(this);
    const auto& language_list = Highlighter::getLanguages();
    QMap<char, QMenu*> start_char_map;
    QList<QAction*> actionlist;
    for (const QString& language : language_list)
    {
        char start_char = toupper(language.toStdString()[0]);
        auto entry = start_char_map.find(start_char);
        QMenu* submenu = nullptr;
        if (entry != start_char_map.end())
        {
            submenu = entry.value();
        }
        else
        {
            submenu = new QMenu(tr("%1").arg(start_char));
            menu.addMenu(submenu);
            start_char_map[start_char] = submenu;
        }
        auto* action = submenu->addAction(language);
        actionlist.append(action);
        if (language == ui->textBrowser->currentLanguage())
        {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }

    const QPoint point = ui->textBrowser->rect().topLeft();
    auto* selection = menu.exec(ui->textBrowser->mapToGlobal(point) + menu_offset);

    const int index = actionlist.indexOf(selection);
    if (index != -1)
    {
        bool enabled = ui->btnStoreText->isEnabled();
        ui->textBrowser->setLanguage(language_list[index]);
        set_widget_and_action_enabled(ui->btnStoreText, enabled);
    }
}

bool MainWindow::on_btnCloseText_clicked(Editor::e editor)
{
    code_browser* text_browser = get_active_text_browser();
    if (ui->comboOpenNewEditor->currentIndex() != AdditionalEditor::None)
    {
        switch (editor)
        {
        case Editor::Viewer:
            ui->textBrowser->setText("");
            return true;
        case Editor::Active:
            if (text_browser != ui->textBrowser)
            {
                removeDockWidget(dynamic_cast<QDockWidget*>(text_browser->parent()));
                return true;
            }
            break;
        case Editor::All:
            if (ui->comboOpenNewEditor->currentIndex() == AdditionalEditor::OnNewFile)
            {
                // TODO: close all
            }
            break;
        case Editor::ActiveFromWidget:
            break;
        }
    }

    if (text_browser->get_changed() && text_browser->get_file_path().length() > 0)
    {
        QMessageBox fSaveRequest;
        fSaveRequest.setText(tr("The document has been modified.\n\n%1").arg(ui->labelFilePath->text()));
        fSaveRequest.setInformativeText(tr("Do you want to save your changes?"));
        fSaveRequest.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        fSaveRequest.setDefaultButton(QMessageBox::Save);
        switch (fSaveRequest.exec())
        {
        case QMessageBox::Save:
            on_btnStoreText_clicked(text_browser);
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return false; // do not close app
        }
    }

    if (ui->comboOpenNewEditor->currentIndex() == AdditionalEditor::None)
    {
        text_browser->reset();
        text_browser->setText("");
    }

    if (ui->tableBinaryView->get_binary_data().size())
    {
        ui->tableBinaryView->clear_binary_content();
        showDockedWidget(mBinaryValuesView.data(), true);
    }

    set_widget_and_action_enabled(ui->btnStoreText, false);

    ui->labelFilePath->setText("");

    if (!ui->graphicsView->has_rendered_graphic())
    {
        ui->graphicsView->clear();
    }

    return true; // can close app
}

void MainWindow::on_btnStoreText_clicked(code_browser* text_browser)
{
    if (!text_browser)
    {
        text_browser = get_active_text_browser();
    }
    QString fFileName = text_browser->get_file_path();
    if (fFileName.length() == 0)
    {
        fFileName = QFileDialog::getSaveFileName(this, tr("Save content of text editor"));
    }
    QFile file(fFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        const auto & binary_data = ui->tableBinaryView->get_binary_data();
        if (binary_data.size())
        {
            file.write(binary_data);
        }
        else
        {
            const string fString = text_browser->toPlainText().toStdString();
            file.write(fString.c_str(), fString.size());
        }
        if (ui->comboOpenNewEditor->currentIndex() == AdditionalEditor::None)
        {
            set_widget_and_action_enabled(ui->btnStoreText, false);
        }
        perform_post_cmd_action(Cmd::UpdateItemStatus);
    }
}


void MainWindow::textBrowserChanged()
{
    set_widget_and_action_enabled(ui->btnStoreText, true);
}

void MainWindow::updateSelectedLanguage(const QString& language)
{
    ui->labelLanguage->setText(tr(" Type: ") + language);
}

#ifdef WEB_ENGINE
void MainWindow::show_web_view(bool show)
{
    QWidget* pView = (QWidget*)mWebEngineView.data();
    showDockedWidget(pView, !show);
}
#endif

void MainWindow::invoke_highlighter_dialog()
{
    Highlighter::Language::invokeHighlighterDlg();
}

void MainWindow::createBookmark()
{
    QString current_file= ui->labelFilePath->text();
    if (current_file.size())
    {
        const QString book_mark = "Bookmarks";
        QTreeWidgetItem* new_tree_root_item;
        auto list = ui->treeFindText->findItems(book_mark, Qt::MatchExactly);
        if (list.size())
        {
            new_tree_root_item = list[0];
        }
        else
        {
            new_tree_root_item = new QTreeWidgetItem({book_mark, "", ""});
            ui->treeFindText->addTopLevelItem(new_tree_root_item);
        }

        QString found_text_line;
#ifdef __linux__
        current_file = current_file.right(current_file.size()-1);
#else
        current_file.replace("\\", "/");
#endif
        QTreeWidgetItem* new_child_item = insert_file_path(new_tree_root_item, current_file);
        new_child_item->setText(FindColumn::Line, tr("%1").arg(ui->textBrowser->current_line(&found_text_line)));
        new_child_item->setText(FindColumn::FoundTextLine, found_text_line.trimmed());
    }
}
