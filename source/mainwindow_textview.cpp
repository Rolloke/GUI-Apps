#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "binary_values_view.h"

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
            on_btnCloseText_clicked();
        }
        mHighlighter->setExtension(ext);
        ui->textBrowser->insertPlainText(aText + getLineFeed());
        ui->textBrowser->textCursor().movePosition(QTextCursor::End);
    #ifdef DOCKED_VIEWS
        showDockedWidget(ui->textBrowser);
    #endif
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
        if (language == mHighlighter->currentLanguage())
        {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }

    const QPoint point = ui->textBrowser->rect().topLeft();
    auto* selection = menu.exec(ui->textBrowser->mapToGlobal(point));

    const int index = actionlist.indexOf(selection);
    if (index != -1)
    {
        bool enabled = ui->btnStoreText->isEnabled();
        mHighlighter->setLanguage(language_list[index]);
        set_widget_and_action_enabled(ui->btnStoreText, enabled);
    }
}

void MainWindow::on_btnStoreText_clicked()
{
    QString fFileName = ui->labelFilePath->text();
    if (fFileName.length() == 0)
    {
        fFileName = QFileDialog::getSaveFileName(this, tr("Save content of text editor"));
    }
    QFile file(fFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        const auto & binary_data = ui->textBrowser->get_binary_data();
        if (binary_data.size())
        {
            file.write(binary_data);
        }
        else
        {
            const string fString = ui->textBrowser->toPlainText().toStdString();
            file.write(fString.c_str(), fString.size());
        }
        set_widget_and_action_enabled(ui->btnStoreText, false);
    }
}

void MainWindow::textBrowserChanged()
{
    set_widget_and_action_enabled(ui->btnStoreText, true);
}

void MainWindow::on_btnCloseText_clicked()
{
    if (ui->btnStoreText->isEnabled() && ui->labelFilePath->text().length() > 0)
    {
        QMessageBox fSaveRequest;
        fSaveRequest.setText(tr("The document has been modified."));
        fSaveRequest.setInformativeText(tr("Do you want to save your changes?"));
        fSaveRequest.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        fSaveRequest.setDefaultButton(QMessageBox::Save);
        switch (fSaveRequest.exec())
        {
            case QMessageBox::Save:
                on_btnStoreText_clicked();
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
                return;
        }
    }
    ui->textBrowser->setText("");
    if (ui->textBrowser->get_binary_data().size())
    {
        ui->textBrowser->clear_binary_content();
#ifdef DOCKED_VIEWS
        showDockedWidget(mBinaryValuesView.data(), true);
#endif
    }

    set_widget_and_action_enabled(ui->btnStoreText, false);

    ui->labelFilePath->setText("");
    ui->graphicsView->scene()->clear();

    //ui->graphicsView->addItem2graphicsView(new commit_graphis_item());

}


void MainWindow::updateSelectedLanguage(const QString& language)
{
    ui->labelLanguage->setText(tr(" Type: ") + language);
}

void MainWindow::invoke_highlighter_dialog()
{
    Highlighter::Language::invokeHighlighterDlg();
}
