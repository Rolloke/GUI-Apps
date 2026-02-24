#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "binary_values_view.h"
#include "code_browser.h"

#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#if CORE5COMPAT == 1
#include <QTextCodec>
#endif
#else
#include <QTextCodec>
#include <QRegExp>
#endif


using namespace std;
using namespace git;


void MainWindow::appendTextToBrowser(const QString& aText, bool append, const QString ext, bool show, bool use_second_view)
{
    if (aText.size())
    {
        if (!append && !use_second_view)
        {
            btnCloseText_clicked(Editor::Viewer);
        }
        auto* browser = use_second_view && has_background_text_view() ? mBackgroundTextView.data() : ui->textBrowser;
        QDockWidget* parent = dynamic_cast<QDockWidget*>(browser->parent());
        const QString viewer =  parent ? parent->windowTitle() : "viewer";
        browser->setExtension(ext);

        /// QRegularExpression("\\[[0-9]{1,2}m");
        QString clean_text;
        if (aText.contains(static_cast<char>(27))
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                || aText.contains(static_cast<char>(0))
        #endif
                )
        {
            clean_text = aText;
            clean_text.replace("\033", "");
            clean_text.replace(static_cast<char>(0), ' ');
        }
        for (const auto& filter_pattern : mFilterPatterns)
        {
            bool clean_text_empty = clean_text.isEmpty();
            if (   ( clean_text_empty && aText.contains(     filter_pattern.first))
                || (!clean_text_empty && clean_text.contains(filter_pattern.first)))
            {
                if (clean_text_empty)
                {
                    clean_text = aText;
                }
                clean_text.replace(filter_pattern.first, filter_pattern.second);
            }
        }

        if (clean_text.size())
        {
            parseTextForBookmarks(clean_text, viewer);
            browser->insertPlainText(clean_text+ getLineFeed());
        }
        else
        {
            parseTextForBookmarks(aText, viewer);
            browser->insertPlainText(aText + getLineFeed());
        }
        browser->textCursor().movePosition(QTextCursor::End);
        if (show)
        {
            showDockedWidget(browser);
        }
    }
}

void MainWindow::parseTextForBookmarks(const QString &aText, const QString &viewer)
{
    QStringList lines;
    for (const auto& message_pattern : mMessagePatterns)
    {
        if (lines.isEmpty())
        {
             lines = aText.split("\n");
        }
        for (const QString& line : lines)
        {
            if (message_pattern->parse(line))
            {
                createBookmark(viewer, message_pattern.get());
            }
        }
    }
}

bool MainWindow::has_background_text_view()
{
    if (!mBackgroundTextView)
    {
        // backgound process output text view
        mBackgroundTextView.reset(create_new_text_browser("", tr("Background Log")));
        QDockWidgetX* dock = dynamic_cast<QDockWidgetX*>(mBackgroundTextView.data()->parent());
        dock->setObjectName(background_textbrowser);
        dock->setAttribute(Qt::WA_DeleteOnClose, false);
        mBackgroundTextView->setReadOnly(true);
    }
    return static_cast<bool>(mBackgroundTextView);
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
        for (const QString& language : std::as_const(language_list))
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
        auto* selection = menu.exec(check_screen_position(point, true, text_browser));

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
        case Cmd::EditTabIndent:     sel = selection::tab_indent;     break;
        case Cmd::EditTabOutdent:    sel = selection::tab_outdent;    break;
        default:
            break;
        }
        text_browser->changeSelection(sel);
    }
}

void MainWindow::check_reload(code_browser *browser, bool dont_ask_for_loading)
{
    if (browser->is_modified())
    {
        browser->update_modified();
        if (dont_ask_for_loading || callMessageBox(tr("Reload file%1?;File %1%2 has changed"), "", browser->get_file_path()) & QMessageBox::Yes)
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
                || mActivViewObjectName == graphicsviewer
                || mActivViewObjectName == background_textbrowser )
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
                    set_active(get_widget(dock_widget), false);
                }
                all_closed = true;
                for (QDockWidget* dock_widget : dock_widgets)
                {
                    QWidget* current_widget = get_widget(dock_widget);
                    if (mCloseFileFilter.size())
                    {
                        if (get_file_path(current_widget).indexOf(mCloseFileFilter) == -1)
                        {
                            continue;
                        }
                    }
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

void MainWindow::set_show_line_numbers(bool show)
{
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({textbrowser, new_textbrowser});
    for (QDockWidget* dock_widget : dock_widgets)
    {
        code_browser* browser = dynamic_cast<code_browser*>(get_widget(dock_widget));
        if (browser)
        {
            browser->set_show_line_numbers(show);
        }
    }
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
            QMessageBox save_request;
            save_request.setText(tr("The document has been modified.\n\n%1").arg(filepath));
            save_request.setInformativeText(tr("Do you want to save your changes?"));
            save_request.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            save_request.setDefaultButton(QMessageBox::Save);
            ensure_dialog_on_same_screen(&save_request, this);
            switch (save_request.exec())
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

        if (is_any_equal_to(mActivViewObjectName, binary_table_view, binaryview))
        {
            if (ui->tableBinaryView->get_binary_data().size())
            {
                ui->tableBinaryView->clear_binary_content();
                showDockedWidget(mBinaryValuesView.data(), false);
                showDockedWidget(ui->tableBinaryView);
            }
        }
        else if (is_any_equal_to(additional_editor(), AdditionalEditor::None, AdditionalEditor::One))
        {
            reset_text_browser(dynamic_cast<code_browser*>(active_widget));
        }

        if (editor == Editor::CalledFromAction)
        {
            if (mActivViewObjectName == graphicsviewer)
            {
                ui->graphicsView->clear();
            }
            if (mActivViewObjectName == background_textbrowser)
            {
                mBackgroundTextView->setText("");
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
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
    const QString& bf_path = get_file_path(mBinaryValuesView.data());
    if (bf_path.size() && QFileInfo(bf_path).isFile())
    {
        dock_widgets.append(get_dock_widget_of_name({ binary_table_view }));
    }

    for (QDockWidget* dock_widget : dock_widgets)   // find active widget first
    {
        QWidget* current_widget = get_widget(dock_widget);
        if (get_active(current_widget)) active_widget = current_widget;
        set_active(current_widget, false);
    }

    for (QDockWidget* dock_widget : dock_widgets)   // store temporarily activated widgets
    {
        QWidget* current_widget = get_widget(dock_widget);
        set_active(current_widget, true);
        btnStoreText_clicked();
        set_active(current_widget, false);
    }

    if (active_widget)                              // and set active again after storing
    {
        set_active(active_widget, true);
    }
}

void MainWindow::btnReloadAll_clicked()
{
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
    for (QDockWidget* dock_widget : dock_widgets)   // store temporarily activated widgets
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(get_widget(dock_widget));
        if (text_browser)
        {
            check_reload(text_browser, true);
        }
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && CORE5COMPAT == 0
            auto encoding = text_browser->get_encoding();
            if (ui->comboTextCodex->currentIndex())
            {
                auto current = ui->comboTextCodex->currentText();
                QStringEncoder decode_string;
                encoding =  decode_string.encodingForName(current.toStdString().c_str());
            }
            if (encoding.has_value())
            {
                QStringEncoder encode_string(encoding.value());
                file.write(encode_string(text_browser->toPlainText()));
            }
            else
            {
                const string fString = text_browser->toPlainText().toStdString();
                file.write(fString.c_str(), fString.size());
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
            const string fString = codec_selected ? text_browser->toPlainText().toLocal8Bit().toStdString() : text_browser->toPlainText().toStdString();
            file.write(fString.c_str(), fString.size());
#endif
            file.close();
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
            file.close();
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

void MainWindow::show_web_view(bool show)
{
#ifdef WEB_ENGINE
    QWidget* pView = (QWidget*)mWebEngineView.data();
    showDockedWidget(pView, show);
#else
    QWidget* pView = (QWidget*)mTextRenderView.data();
    showDockedWidget(pView, show);
#endif
}

void MainWindow::invoke_highlighter_dialog()
{
    Highlighter::Language::invokeHighlighterDlg(this);
}

void MainWindow::invoke_output_parser_dialog()
{
    OutputParser dlg(mMessagePatterns, mFilterPatterns);
    ensure_dialog_on_same_screen(&dlg, this);
    dlg.exec();
}

void MainWindow::createBookmark(QString book_mark_root_name, ParseMessagePattern* pmp)
{
    code_browser* text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
    QString current_file;
    QString found_text_line;
    int line = 0;
    if (pmp)
    {
        current_file    = pmp->get_filename();
        QFileInfo fileinfo(current_file);
        if (fileinfo.exists())
        {
            current_file    = fileinfo.absoluteFilePath();
            line            = pmp->get_line().toInt();
            found_text_line = pmp->get_message_text();
        }
        else
        {
            return;
        }
    }
    else if (text_browser)
    {
        current_file = text_browser->get_file_path();
        line = text_browser->current_line(&found_text_line);
    }

    if (current_file.size())
    {
        QTreeWidgetItem* new_tree_root_item;
        auto list = ui->treeFindText->findItems(book_mark_root_name, Qt::MatchExactly);
        if (list.size())
        {
            new_tree_root_item = list[0];
        }
        else
        {
            new_tree_root_item = new QTreeWidgetItem({book_mark_root_name, "", ""});
            ui->treeFindText->addTopLevelItem(new_tree_root_item);
        }

#ifdef __linux__
        current_file = current_file.right(current_file.size()-1);
#else
        current_file.replace("\\", "/");
#endif
        QTreeWidgetItem* new_child_item = insert_file_path(new_tree_root_item, current_file);

        new_child_item->setText(FindColumn::Line, tr("%1").arg(line));
        new_child_item->setText(FindColumn::FoundTextLine, found_text_line.trimmed());
        showDockedWidget(ui->treeFindText);
        ui->treeFindText->expandItem(new_tree_root_item);
    }
}
