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
            btnCloseText_clicked(Editor::Viewer);
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

    code_browser*text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
    if (text_browser)
    {
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

            if (language == text_browser->currentLanguage())
            {
                action->setCheckable(true);
                action->setChecked(true);
            }
        }

        const QPoint point = text_browser->rect().topLeft();
        auto* selection = menu.exec(text_browser->mapToGlobal(point) + menu_offset);

        const int index = actionlist.indexOf(selection);
        if (index != -1)
        {
            bool enabled = ui->btnStoreText->isEnabled();
            text_browser->setLanguage(language_list[index]);
            set_widget_and_action_enabled(ui->btnStoreText, enabled);
        }
    }
}

void MainWindow::modify_text()
{
    using namespace git;
    code_browser*text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
    if (text_browser)
    {
        const QAction*     action       = qobject_cast<QAction *>(sender());
        const QVariantList variant_list = action->data().toList();
        const Cmd::eCmd    id           = static_cast<Cmd::eCmd>(variant_list[ActionList::Data::Cmd].toUInt());
        selection sel {selection::unknown};
        switch (id)
        {
        case Cmd::EditToUpper:       sel = selection::to_upper;       break;
        case Cmd::EditToLower:       sel = selection::to_lower;       break;
        case Cmd::EditToggleComment: sel = selection::toggle_comment; break;
        case Cmd::EditToSnakeCase:   sel = selection::to_snake_case;  break;
        case Cmd::EditToCamelCase:   sel = selection::to_camel_case;  break;
        default:
            break;
        }
        text_browser->changeSelection(sel);
    }
}

void MainWindow::check_reload(code_browser *browser)
{
    if (browser->is_modified())
    {
        browser->update_modified();
        if (callMessageBox(tr("Reload file%1?;File %1%2 has changed"), "", browser->get_file_path()) == QMessageBox::Yes)
        {
            open_file(browser->get_file_path(), browser->current_line(), true);
        }
    }
}

void MainWindow::reset_text_browser(code_browser* text_browser)
{
    if (text_browser)
    {
        text_browser->setText("");
        text_browser->set_file_path("");
        text_browser->set_changed(false);
        text_browser->reset();
    }
}

bool MainWindow::close_editable_widgets(QWidget*& active_widget, Editor editor, bool& all_closed)
{
    if (additional_editor() != AdditionalEditor::None)
    {
        switch (editor)
        {
        case Editor::Viewer:
            reset_text_browser(ui->textBrowser);
            return false;
        case Editor::CalledFromAction:
            send_close_to_editable_widget(dynamic_cast<code_browser*>(active_widget));
            if (mActivViewObjectName == textbrowser)
            {
                reset_text_browser(ui->textBrowser);
            }
            if (   mActivViewObjectName == binary_table_view
                || mActivViewObjectName == binaryview
                || mActivViewObjectName == graphicsviewer)
            {
                return true;
            }
            return false;
        case Editor::Active:
            return true;
        case Editor::All:
            if (additional_editor() == AdditionalEditor::OnNewFile)
            {
                QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({new_textbrowser, binary_table_view});
                for (QDockWidget* dock_widget : dock_widgets)
                {
                    set_active(dock_widget->widget(), false);
                }
                all_closed = true;
                for (QDockWidget* dock_widget : dock_widgets)
                {
                    QWidget* current_widget = dock_widget->widget();
                    set_active(current_widget, true);
                    all_closed = send_close_to_editable_widget(current_widget);
                    set_active(current_widget, false);
                    if (!all_closed) break;
                }
                if (all_closed)
                {
                    if (ui->tableBinaryView->get_changed())
                    {
                        ui->tableBinaryView->set_active(true);
                        active_widget = ui->tableBinaryView;
                        return true;
                    }
                }
                return false;
            }
            break;
        case Editor::CalledFromWidget:
            break;
        }
    }
    return true;
}

void MainWindow::btnCloseAll_clicked()
{
    btnCloseText_clicked(Editor::All);
}

bool MainWindow::shall_save(Editor editor)
{
    if (editor == Editor::Active && additional_editor() == AdditionalEditor::OnNewFile)
    {
        return false;
    }
    return true;
}

bool MainWindow::btnCloseText_clicked(Editor editor)
{
    QWidget* active_widget = get_active_editable_widget();
    bool all_closed = false;
    if (close_editable_widgets(active_widget, editor, all_closed))
    {
        QString filepath = get_file_path(active_widget);
        if (shall_save(editor) && get_changed(active_widget) && filepath.length() > 0)
        {
            QMessageBox fSaveRequest;
            fSaveRequest.setText(tr("The document has been modified.\n\n%1").arg(filepath));
            fSaveRequest.setInformativeText(tr("Do you want to save your changes?"));
            fSaveRequest.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            fSaveRequest.setDefaultButton(QMessageBox::Save);
            switch (fSaveRequest.exec())
            {
            case QMessageBox::Save:
                btnStoreText_clicked();
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
                return false; // do not close app
            }
        }

        if (   mActivViewObjectName == binary_table_view
            || mActivViewObjectName == binaryview)
        {
            if (ui->tableBinaryView->get_binary_data().size())
            {
                ui->tableBinaryView->clear_binary_content();
                showDockedWidget(mBinaryValuesView.data(), false);
                showDockedWidget(ui->tableBinaryView);
            }
        }
        else if (   additional_editor() == AdditionalEditor::None
                 || additional_editor() == AdditionalEditor::One)
        {
            reset_text_browser(dynamic_cast<code_browser*>(active_widget));
        }

        if (mActivViewObjectName == graphicsviewer)
        {
            //if (!ui->graphicsView->has_rendered_graphic())
            {
                ui->graphicsView->clear();
            }
        }

        set_widget_and_action_enabled(ui->btnStoreText, false);
        ui->labelFilePath->setText("");
        return true; // can close app
    }
    return all_closed;
}

void MainWindow::btnStoreAll_clicked()
{
    QWidget* active_widget = nullptr;
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({new_textbrowser, binary_table_view});
    for (QDockWidget* dock_widget : dock_widgets)
    {
        QWidget* current_widget = dock_widget->widget();
        if (get_active(current_widget)) active_widget = current_widget;
        set_active(current_widget, false);
    }

    for (QDockWidget* dock_widget : dock_widgets)
    {
        QWidget* current_widget = dock_widget->widget();
        set_active(current_widget, true);
        btnStoreText_clicked();
        set_active(current_widget, false);
    }

    if (active_widget)
    {
        set_active(active_widget, true);
    }
}

void MainWindow::btnStoreText_clicked()
{
    QWidget* active_widget = get_active_editable_widget();
    QString file_name = get_file_path(active_widget);
    if (file_name.length() == 0)
    {
        file_name = QFileDialog::getSaveFileName(this, tr("Save content of text editor"));
    }
    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(active_widget);
        if (text_browser)
        {
            const string fString = text_browser->toPlainText().toStdString();
            file.write(fString.c_str(), fString.size());
            text_browser->set_changed(false);
        }
        qbinarytableview* binary_table = dynamic_cast<qbinarytableview*>(active_widget);
        if (binary_table)
        {
            const auto & binary_data = ui->tableBinaryView->get_binary_data();
            if (binary_data.size())
            {
                file.write(binary_data);
            }
            binary_table->set_changed(false);
        }
        set_widget_and_action_enabled(ui->btnStoreText, false);
        perform_post_cmd_action(Cmd::UpdateItemStatus);
    }
}


void MainWindow::textBrowserChanged(bool changed)
{
    set_widget_and_action_enabled(ui->btnStoreText, changed);
}

void MainWindow::updateSelectedLanguage(const QString& language)
{
    ui->labelLanguage->setText(tr(" Type: ") + language);
}

#ifdef WEB_ENGINE
void MainWindow::show_web_view(bool show)
{
    QWidget* pView = (QWidget*)mWebEngineView.data();
    showDockedWidget(pView, show);
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
