#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QSettings>
#include <QGraphicsPixmapItem>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QItemSelection>
#include <QKeyEvent>
#include <QSystemTrayIcon>

#ifdef WEB_ENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
#endif

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <stdlib.h>
#endif


#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#define STORE_PTR(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)   SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)        SETTING.setValue(getSettingsName(#ITEM), ITEM)
#define STORE_STRF(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), FUNC(ITEM))

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT)  ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)   ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)            ITEM=SETTING.value(getSettingsName(#ITEM), QVariant(ITEM)).CONVERT()
#define LOAD_STRF(SETTING, ITEM, FUNC_OUT, FUNC_IN, CONVERT) ITEM = FUNC_OUT(fSettings.value(getSettingsName(#ITEM), QVariant(FUNC_IN(ITEM))).CONVERT());

QString getSettingsName(const QString& aItemName);
int execute(const QString& command, QString& aResultText);

/// TODO: download kodi file from internet site

namespace config
{
constexpr char sGroupSettings[] = "Settings";
}

namespace txt
{
const QString radio            = QObject::tr("Radio");
const QString tv               = QObject::tr("TV");
const QString open_media_list  = QObject::tr("Open Kodi raw media List");
const QString media_list       = QObject::tr("Media List (*.m3u);;Text File (*.txt);;All Files (*.*)");

const QString store_kodi_fav   = QObject::tr("Store media list as favorites for Raspi");
const QString store_downloaded_kodi_fav = QObject::tr("Store downloaded favorites from Raspi");
const QString upload_kodi_fav  = QObject::tr("Upload favorites to Raspi");
const QString update_kodi_fav  = QObject::tr("Load favorites for update media list check states");
const QString kodi_favorites   = QObject::tr("Favorites (*.xml);;All Files (*.*)");

const QString media_player_cmd = QObject::tr(
            "Edit media player command:\n"
            "- path_to_media_player %1\n"
            "- %1 Placeholder for URL\n");

const QString upload_favorite_cmd = QObject::tr(
            "Edit upload favorite command:\n"
            "- scp %1 user@address:/path/favorites.xml\n"
            "- %1 Placeholder for favorites file name\n");

const QString download_favorite_cmd = QObject::tr(
            "Edit upload favorite command:\n"
            "- scp user@address:/path/favorites.xml %1\n"
            "- %1 Placeholder for favorites file name\n");

const QStringList media_player_cmd_line =
{
    QObject::tr("Arguments:"),
    QObject::tr("-f <file>, --file=<file>"),
    QObject::tr("   open kodi m3u media file at start"),
    QObject::tr("--help show this help")
};

const QString version          = QObject::tr("1.0.0.1");
}

enum eTable
{
    eName, eID, eGroup, eURL, eLogo, eFriendlyName, eMedia, eLast
};

enum error
{
    ErrorNumberInErrno = -1,
    NoError = 0
};

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
, ui(new Ui::MainWindow)
, mPlayer(this)
, mListModel(nullptr)
, mCurrentRowIndex(-1)
, mCurrentPlayIndex(-1)
, mFileOpenPath(QDir::homePath())
, mFavoritesOpenPath(QDir::homePath())
, mFindStartRow(0)
, mShowIcon(true)
, mChecked(eID)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/36x36/applications-multimedia.png"));

    const QString arg_file = "--file=";
    const QString arg_help = "--help";
    QString filename;
    QStringList arguments = QCoreApplication::arguments();
    for (int n = 0; n < arguments.size(); ++n)
    {
        if (arguments[n] == "-f" && (n + 1) < arguments.size())
        {
            filename = arguments[n+1];
        }
        else
        {
            int pos = arguments[n].indexOf(arg_file);
            if (pos != -1)
            {
                filename = arguments[n].mid(pos + arg_file.size());
            }
            pos = arguments[n].indexOf(arg_help);
            if (pos != -1)
            {
                for (const auto& line : txt::media_player_cmd_line)
                {
                    std::cout << line.toStdString() << std::endl;
                }
                exit(0);
            }
        }
    }
    QFileInfo info(filename);
    QString settings_file_name = info.baseName();

    mPlayer.setVideoOutput(&mVideo);
    connect(&mPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(show_media_player_error(QMediaPlayer::Error)));
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    mHiddenColumns = { eURL, eLogo, eFriendlyName };
    QStringList fSectionNames = { tr("Name"), tr("ID"), tr("Group"), tr("URL"), tr("Logo"), tr("Friendly Name"), tr("Medium")};
    mListModel = new CheckboxItemModel(0, eLast, this);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
        if (!mHiddenColumns.contains(fSection))
        {
            ui->comboBoxSearchColumn->addItem(fSectionNames[fSection], fSection);
        }
    }

    ui->comboBoxSearchColumn->setCurrentIndex(eID);
    ui->tableView->setModel(mListModel);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        if (!mHiddenColumns.contains(fSection))
        {
            ui->tableView->horizontalHeader()->setSectionResizeMode(fSection , fSection == 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
        }
    }
    ui->tableView->horizontalHeader()->setStretchLastSection(false);
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(table_selectionChanged(const QItemSelection&,const QItemSelection&)));

#ifdef WEB_ENGINE
    mWebEngineView.reset(new QWebEngineView(this));
    // mWebEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    QWebEnginePage* page = new QWebEnginePage(this);
    mWebEngineView->setPage(page);
    ui->verticalLayout_2->removeWidget(ui->graphicsView);
    delete ui->graphicsView;
    ui->graphicsView = nullptr;
    ui->verticalLayout_2->insertWidget(0, mWebEngineView.data());
#else
    ui->graphicsView->setScene(new QGraphicsScene ());
#endif

    m_play_status = new QLabel("");
    m_play_status->setToolTip("Play status");
    ui->statusBar->addPermanentWidget(m_play_status);

    m_play_name = new QLabel("");
    m_play_name->setToolTip("Channel name");
    ui->statusBar->addPermanentWidget(m_play_name);


    connect(ui->actionOpen_Kodi_raw_list, SIGNAL(triggered(bool)), SLOT(menu_file_open()));
    connect(ui->actionSave_as_favorites, SIGNAL(triggered(bool)), SLOT(menu_file_save_as_favorites()));
    connect(ui->actionRead_favorites, SIGNAL(triggered(bool)), SLOT(menu_file_update_favorites()));
    connect(ui->actionUpload_favorites, SIGNAL(triggered(bool)), SLOT(menu_file_upload_favorites()));
    connect(ui->actionDownload_favorites, SIGNAL(triggered(bool)), SLOT(menu_file_download_favorites()));

    connect(ui->actionCopy_URL, SIGNAL(triggered(bool)), SLOT(menu_edit_copy_url()));
    connect(ui->actionCopy_Thumb, SIGNAL(triggered(bool)), SLOT(menu_edit_copy_thumb()));
    connect(ui->actionOpen_Mediaplayer, SIGNAL(triggered(bool)), SLOT(menu_edit_open_media_player()));
    connect(ui->actionOpenMediaPlayerOnDoubleclick, &QAction::triggered, [&](bool checked) { ui->sliderVolume->setEnabled(!checked); });

    connect(ui->actionMedia_player, SIGNAL(triggered(bool)), SLOT(menu_option_media_player_command()));
    connect(ui->actionEdit_Upload_Command, SIGNAL(triggered(bool)), SLOT(menu_option_edit_upload_command()));
    connect(ui->actionEdit_Download_Command, SIGNAL(triggered(bool)), SLOT(menu_option_edit_download_command()));
    connect(ui->actionShowTrayIconAndInfo, SIGNAL(toggled(bool)), SLOT(menu_option_show_tray_icon(bool)));

    connect(ui->actionAbout, SIGNAL(triggered(bool)), SLOT(menu_help_about()));
    connect(ui->actionInfo, SIGNAL(triggered(bool)), SLOT(menu_help_info()));

    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(100);
    ui->sliderVolume->setTickPosition(QSlider::TicksBothSides);
    ui->sliderVolume->setTickInterval(10);
    ui->sliderVolume->setSingleStep(1);

    fSettings.beginGroup(config::sGroupSettings + settings_file_name);

    LOAD_PTR(fSettings, ui->comboBoxSearchColumn, setCurrentIndex, currentIndex, toInt);
    LOAD_PTR(fSettings, ui->sliderVolume, setValue, value, toInt);
    LOAD_STR(fSettings, mFileOpenPath, toString);
    LOAD_STR(fSettings, mFavoritesOpenPath, toString);
    LOAD_STR(fSettings, mDownloadFavoriteCommand, toString);
    LOAD_STR(fSettings, mUploadFavoriteCommand, toString);
    LOAD_STR(fSettings, mShowIcon, toBool);
    LOAD_STR(fSettings, mMediaPlayerCommand, toString);
    LOAD_STR(fSettings, mCurrentPlayIndex, toInt);
    LOAD_PTR(fSettings, ui->actionOpenMediaPlayerOnDoubleclick, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->actionShowTrayIconAndInfo, setChecked, isChecked, toBool);
    LOAD_STR(fSettings, mOpenFileAtStart, toString);
    LOAD_STR(fSettings, mChecked, toInt);
    mListModel->setCheckedColumn(mChecked);

    fSettings.endGroup();

    menu_option_show_tray_icon(ui->actionShowTrayIconAndInfo->isChecked());

    ui->sliderVolume->setEnabled(!ui->actionOpenMediaPlayerOnDoubleclick->isChecked());

    if (filename.size())
    {
        open_file(filename);
        if (mOpenFileAtStart.size())
        {
            mOpenFileCmdLine = mOpenFileAtStart;
        }
    }
    else if (mOpenFileAtStart.size())
    {
        ui->actionLoadLastOpenedFileAtStart->setChecked(true);
        open_file(mOpenFileAtStart);
    }
    select_index(mCurrentPlayIndex);
    display_play_status();
    m_play_name->setText(get_item_name(mCurrentPlayIndex));

#ifdef TEST_DOWNLOAD_KODI_FILE
    const QUrl url("https://github.com/jnk22/kodinerds-iptv/blob/master/iptv/kodi/kodi.m3u");
    QNetworkRequest request(url);
    QNetworkReply* reply = mNetManager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onDownloadFiniseh()));
#endif
}


MainWindow::~MainWindow()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    QFileInfo info(mOpenFileCmdLine);

    fSettings.beginGroup(config::sGroupSettings + info.baseName());
    STORE_PTR(fSettings, ui->comboBoxSearchColumn, currentIndex);
    STORE_PTR(fSettings, ui->sliderVolume, value);
    STORE_STR(fSettings, mFileOpenPath);
    STORE_STR(fSettings, mFavoritesOpenPath);
    STORE_STR(fSettings, mDownloadFavoriteCommand);
    STORE_STR(fSettings, mUploadFavoriteCommand);
    STORE_STR(fSettings, mShowIcon);
    STORE_STR(fSettings, mMediaPlayerCommand);
    STORE_STR(fSettings, mCurrentPlayIndex);
    STORE_PTR(fSettings, ui->actionOpenMediaPlayerOnDoubleclick, isChecked);
    STORE_PTR(fSettings, ui->actionShowTrayIconAndInfo, isChecked);
    STORE_STR(fSettings, mChecked);
    if (!ui->actionLoadLastOpenedFileAtStart->isChecked())
    {
        mOpenFileAtStart.clear();
    }
    STORE_STR(fSettings, mOpenFileAtStart);

    fSettings.endGroup();

    mPlayer.stop();
    if (m_tray_message)
    {
        QMenu * menu = m_tray_message->contextMenu();
        m_tray_message->setContextMenu(nullptr);
        delete menu;
    }
    delete ui;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int  key = event->key();
    switch (key)
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        on_pushButtonStart_clicked();
        break;
    }
    QMainWindow::keyPressEvent(event);
}

QString MainWindow::getConfigName() const
{
    QString sConfigFileName = windowTitle();
    sConfigFileName.replace(" ", "");
#ifdef __linux__
    return QDir::homePath() + "/.config/" + sConfigFileName + ".ini";
#else
    return "HKEY_CURRENT_USER\\Software\\KESoft\\" + sConfigFileName;
#endif
}

QString MainWindow::get_item_name(int row) const
{
    return (row != -1) ? mListModel->data(mListModel->index(row, eName)).toString() : "";
}

void MainWindow::on_tableView_clicked(const QModelIndex &index, bool called_by_function)
{
    if (index.column() == mChecked && !called_by_function)
    {
        mListModel->setData(index, !mListModel->data(index, Qt::CheckStateRole).toBool(), Qt::CheckStateRole);
    }
    ui->statusBar->showMessage(get_item_name(index.row()));
    mCurrentRowIndex = index.row();

    if (mShowIcon)
    {
        QString logoUrl = mListModel->data(mListModel->index(index.row(), eLogo)).toString();
#ifdef WEB_ENGINE
        QString iconpage = tr("<!DOCTYPE html><html><head><title>Page Title</title></head>"
        "<body style='background-color:white;' ><img src='%1' alt='%2' width='90%' height='90%'></body>"
        "</html>").arg(logoUrl, mListModel->data(mListModel->index(mCurrentRowIndex, eFriendlyName)).toString());
        mWebEngineView->page()->setHtml(iconpage);
        ui->tableView->setFocus();
#else
        ui->graphicsView->scene()->clear();
        const QUrl url(logoUrl);
        QNetworkRequest request(url);
        QNetworkReply* reply = mNetManager.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
#endif
    }
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &)
{
    on_pushButtonStart_clicked();
}

void MainWindow::on_pushButtonStart_clicked()
{
    if (mCurrentRowIndex != -1)
    {
        mCurrentPlayIndex = mCurrentRowIndex;
        if (ui->actionOpenMediaPlayerOnDoubleclick->isChecked())
        {
            menu_edit_open_media_player();
        }
        else
        {
            mPlayer.setMedia(QUrl(mListModel->data(mListModel->index(mCurrentRowIndex, eURL)).toString()));
            if (mListModel->data(mListModel->index(mCurrentRowIndex, eMedia)).toString() == txt::tv)
            {
                mVideo.show();
            }
            mPlayer.play();
            connect(&mPlayer, SIGNAL(metaDataChanged(QString,QVariant)), this, SLOT(metaDataChanged(QString,QVariant)));

            display_play_status();
            m_play_name->setText(get_item_name(mCurrentPlayIndex));
        }
    }
}

void MainWindow::display_play_status()
{
    switch(mPlayer.state())
    {
    case QMediaPlayer::StoppedState:
        m_play_status->setText(tr("Stopped"));
        break;
    case QMediaPlayer::PausedState:
        m_play_status->setText(tr("Paused"));
        break;
    case QMediaPlayer::PlayingState:
        m_play_status->setText(tr("Playing"));
        break;
    }
    ui->pushButtonStart->setChecked(mPlayer.state() == QMediaPlayer::PlayingState);
    ui->pushButtonPause->setChecked(mPlayer.state() == QMediaPlayer::PausedState);
    ui->pushButtonStop->setChecked(mPlayer.state() == QMediaPlayer::StoppedState);
}


void MainWindow::on_sliderVolume_valueChanged(int value)
{
    mPlayer.setVolume(value);
}

void MainWindow::on_pushButtonPause_clicked()
{
    mPlayer.pause();
    display_play_status();
}

void MainWindow::on_pushButtonStop_clicked()
{
    mPlayer.stop();
    if (mVideo.isVisible())
    {
        mVideo.hide();
    }
    display_play_status();
}

void MainWindow::on_checkBoxSelectAll_clicked(bool checked)
{
    const int table_rows = mListModel->rowCount();
    for (int current_row = 0; current_row < table_rows; ++current_row)
    {
        mListModel->setData(mListModel->index(current_row, mChecked), checked, Qt::CheckStateRole);
    }
}

void MainWindow::on_pushButtonSelect_clicked()
{
    const QString select_text = ui->lineEditSelection->text();
    const int table_rows = mListModel->rowCount();
    const int search_column = ui->comboBoxSearchColumn->currentIndex();
    const Qt::CaseSensitivity case_sens = ui->checkBoxCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    int selected = 0;
    for (int current_row = 0; current_row < table_rows; ++current_row)
    {
        if (mListModel->data(mListModel->index(current_row, search_column)).toString().indexOf(select_text, 0, case_sens) != -1)
        {
            bool checked = mListModel->data(mListModel->index(current_row, mChecked),  Qt::CheckStateRole).toBool();
            mListModel->setData(mListModel->index(current_row, mChecked), !checked, Qt::CheckStateRole);
        }
        if (mListModel->data(mListModel->index(current_row, mChecked),  Qt::CheckStateRole).toBool())
        {
            ++selected;
        }
    }
    ui->statusBar->showMessage(tr("Currently selected favorites %1").arg(selected));
}

void MainWindow::on_pushButtonFind_clicked()
{
    const QString select_text = ui->lineEditSelection->text();
    const int table_rows = mListModel->rowCount();
    const int search_column = ui->comboBoxSearchColumn->currentIndex();
    const Qt::CaseSensitivity case_sens = ui->checkBoxCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    int current_row;
    for (current_row = mFindStartRow; current_row < table_rows; ++current_row)
    {
        if (mListModel->data(mListModel->index(current_row, search_column)).toString().indexOf(select_text, 0, case_sens) != -1)
        {
            select_index(current_row);
            mFindStartRow = current_row + 1;
            if (mFindStartRow == table_rows)
            {
                mFindStartRow = 0;
            }
            break;
        }
    }
    if (current_row == table_rows)
    {
        mFindStartRow = 0;
    }
}

void MainWindow::table_selectionChanged(const QItemSelection & selected, const QItemSelection & )
{
    const auto & indexes = selected.indexes();
    if (indexes.size())
    {
        on_tableView_clicked(indexes[0], true);
    }
}

void MainWindow::select_index(int select)
{
    if (select != -1)
    {
        ui->tableView->selectRow(select);
        const auto index = mListModel->index(select, eName);
        ui->tableView->scrollTo(index);
        on_tableView_clicked(index, true);
    }
}


void MainWindow::on_lineEditSelection_textChanged(const QString &)
{
    mFindStartRow = 0;
}

void MainWindow::menu_file_save_as_favorites()
{
    QString filename = QFileDialog::getSaveFileName(this, txt::store_kodi_fav, mFavoritesOpenPath, txt::kodi_favorites);
    if (filename.size())
    {
        QFileInfo info(filename);
        mFavoritesOpenPath = info.dir().absolutePath();

        QFile file(filename);

        if (file.open(QIODevice::WriteOnly))
        {
            file.write("<favourites>\n");
            const int table_rows = mListModel->rowCount();
            for (int current_row = 0; current_row < table_rows; ++current_row)
            {
                if (mListModel->data(mListModel->index(current_row, mChecked), Qt::CheckStateRole).toBool())
                {
                    QString line = tr("<favourite name=\"%1\" thumb=\"%2\">PlayMedia(&quot;%3&quot;)</favourite>\n").
                            arg(get_item_name(current_row),
                            mListModel->data(mListModel->index(current_row, eLogo)).toString(),
                            mListModel->data(mListModel->index(current_row, eURL)).toString());
                    file.write(line.toStdString().c_str());
                }
            }
            file.write("</favourites>\n");
        }
    }
}

void MainWindow::menu_file_update_favorites()
{
    QString filename = QFileDialog::getOpenFileName(this, txt::update_kodi_fav, mFavoritesOpenPath, txt::kodi_favorites);
    if (filename.size())
    {
        QFileInfo info(filename);
        mFavoritesOpenPath = info.dir().absolutePath();

        QFile file(filename);

        if (file.open(QIODevice::ReadOnly))
        {
            on_checkBoxSelectAll_clicked(false);

            const QString tag        = "<favourite name=\"";
            const QString thumb_tag  = "thumb=\"";
            const QString media_tag  = "PlayMedia(&quot;";
            const QString plugin_tag = "plugin://";

            while (!file.atEnd())
            {
                QString line(file.readLine());
                line.resize(line.size()-1);
                int start = line.indexOf(tag);
                if (start != -1)
                {
                    const int table_rows = mListModel->rowCount();

                    start += tag.size();
                    int end = line.indexOf('\"', start + 1);
                    QString select_text = line.mid(start, end - start);

                    bool found_table_entry = false;
                    for (int current_row = 0; current_row < table_rows; ++current_row)
                    {
                        if (get_item_name(current_row).indexOf(select_text, 0, Qt::CaseSensitive) != -1)
                        {
                            mListModel->setData(mListModel->index(current_row, mChecked), true, Qt::CheckStateRole);
                            found_table_entry = true;
                            break;
                        }
                    }

                    if (!found_table_entry) // save non streaming TV favorites
                    {
                        start = line.indexOf(thumb_tag); // extract image url
                        start += thumb_tag.size();
                        end = line.indexOf('\"', start + 1);
                        QString image_url = line.mid(start, end - start);

                        start = line.indexOf(media_tag); // extract media url
                        start += media_tag.size();
                        end = line.indexOf("&quot;", start + 1);
                        QString media_url = line.mid(start, end - start);

                        start = line.indexOf(plugin_tag); // extract Plugin
                        start += media_tag.size();
                        end = line.indexOf("/", start + 1);
                        QString plugin_txt = "non streaming: " + line.mid(start, end - start);

                        int current_row = table_rows;
                        mListModel->insertRows(current_row, 1, QModelIndex());
                        mListModel->setData(mListModel->index(current_row, eName, QModelIndex()), select_text);
                        mListModel->setData(mListModel->index(current_row, eURL, QModelIndex()), media_url);
                        mListModel->setData(mListModel->index(current_row, eLogo, QModelIndex()), image_url);
                        mListModel->setData(mListModel->index(current_row, eGroup, QModelIndex()), plugin_txt);
                        mListModel->setData(mListModel->index(current_row, mChecked), true, Qt::CheckStateRole);
                    }
                }
            };
            file.close();
        }
    }
}

void MainWindow::menu_file_open()
{
    QString filename = QFileDialog::getOpenFileName(this, txt::open_media_list, mFileOpenPath, txt::media_list);
    if (filename.size())
    {
        QFileInfo info(filename);
        mFileOpenPath = info.dir().absolutePath();
        open_file(filename);
    }
}

void MainWindow::menu_help_about()
{
    QString message = tr("<h3>About Kodi media list viewer and editor</h3><br><br>"
            "Store tv channels from m3u files as favorites within raspberry kodi<br>"
            "The program is provided AS IS with NO WARRANTY OF ANY KIND<br>"
            "<br>"
            "<li>Based on Qt:\t%1</li>"
            "<li>Built on:\t\t%2, %3</li>"
            "<li>Author:\t\tRolf Kary Ehlers</li>"
            "<li>Version:\t\t%4</li>"
            "<li>License:\t\tGNU GPL Version 2</li>"
            "<li>Email:\t\trolf-kary-ehlers@t-online.de</li>"
            "</ul>"
            "<br>Command Line "
            ).arg(qVersion(), __DATE__, __TIME__, txt::version);
    for (const auto& line : txt::media_player_cmd_line)
    {
        message += line;
        message += "<br>";
    }
    QMessageBox::about(this, windowTitle(), message);
}

void MainWindow::menu_help_info()
{
    if (mCurrentRowIndex != -1)
    {
        QString info = tr("<h3>Information about selected item</h3><br>"
          "<ul>"
           "<li>Name:\t%1</li>"
           "<li>Web:\t%2</li>"
           "<li>Pretty name:\t%3</li>"
           "<li>Media URL:\t%4</li>"
           "<li>Thumb:\t%5</li></ul>").arg(
           mListModel->data(mListModel->index(mCurrentRowIndex, eName)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eID)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eFriendlyName)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eURL)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eLogo)).toString());

        info += tr("<br>Meta Info:<br><ul>");
        for (auto metainfo = mCurrentMetainfo.begin(); metainfo != mCurrentMetainfo.end(); ++metainfo)
        {
            const auto& value = metainfo.value();
            info += "<li>";
            info += metainfo.key();
            info += ": ";
            switch(value.type())
            {
            case QVariant::String:
                info += value.toString();
                break;
            case QVariant::StringList:
                for (auto& string : value.toStringList())
                {
                    info += string;
                    info += ", ";
                }
                break;
            case QVariant::Int:
                info += QString::number(value.toInt());
                break;
            case QVariant::UInt:
                info += QString::number(value.toUInt());
                break;
            default:
                break;
            }
            info += "</li>";
        }
        info += "</ul>";
        QMessageBox::about(this, windowTitle(), info);
    }
}

void MainWindow::traymenu_hide_window()
{
    setWindowFlags(Qt::SubWindow);
    // TODO: do not close this window with about... or info...
}

void MainWindow::traymenu_show_window()
{
    Qt::WindowFlags flag = windowFlags();
    flag &= ~Qt::SubWindow;
    setWindowFlags(flag);
    show();
}

void MainWindow::menu_edit_copy_url()
{
    if (mCurrentRowIndex != -1)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(mListModel->data(mListModel->index(mCurrentRowIndex, eURL)).toString());
    }
}

void MainWindow::menu_edit_copy_thumb()
{
    if (mCurrentRowIndex != -1)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(mListModel->data(mListModel->index(mCurrentRowIndex, eLogo)).toString());
    }
}

void MainWindow::menu_edit_open_media_player()
{
    if (mCurrentRowIndex != -1)
    {
        if (mMediaPlayerCommand.size())
        {
            QString command = tr(mMediaPlayerCommand.toStdString().c_str()).arg(mListModel->data(mListModel->index(mCurrentRowIndex, eURL)).toString());
            int result = system(command.toStdString().c_str());
            if (result != 0)
            {
                ui->statusBar->showMessage(QString(strerror(errno)));
            }
        }
        else
        {
            ui->statusBar->showMessage(tr("No media Player command defined, define under \"Options/Media Player...\""));
        }
    }
    else
    {
        ui->statusBar->showMessage(tr("Select a list entry first"));
    }
}

void MainWindow::menu_option_media_player_command()
{
    QString text = QInputDialog::getText(this, windowTitle(), txt::media_player_cmd, QLineEdit::Normal, mMediaPlayerCommand);
    if (text.size())
    {
        mMediaPlayerCommand = text;
    }
}

#ifdef TEST_DOWNLOAD_KODI_FILE
void  MainWindow::onDownloadFiniseh()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            const int available = reply->bytesAvailable();
            if (available > 0)
            {
                const QByteArray data(reply->readAll());
                QFile file("/home/rolf/mediadatei.txt");
                if (file.open(QIODevice::WriteOnly))
                {
                    file.write(data);
                }
            }
        }
    }
}
#endif

void MainWindow::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            const int available = reply->bytesAvailable();
            if (available > 0)
            {
                const QByteArray data(reply->readAll());
                QImage image;
                image.loadFromData(data);
                ui->graphicsView->scene()->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(image)));
                auto items = ui->graphicsView->scene()->items();
                if (items.size())
                {
                    ui->graphicsView->fitInView(items[0], Qt::KeepAspectRatio);
                }
            }
        }
        else
        {
            ui->statusBar->showMessage(tr("Error: %1 status: %2").arg(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()));
        }
        reply->deleteLater();
    }
    else
    {
        ui->statusBar->showMessage(tr("Download failed. Check internet connection"));
    }
}

void MainWindow::show_media_player_error(QMediaPlayer::Error error)
{
    ui->statusBar->showMessage(tr("Media error number: %1: %2").arg(static_cast<int>(error)).arg(mPlayer.errorString()));
}

void MainWindow::open_file(const QString& filename)
{
    QMap<int, QString> tvg_tags // the tag of the text file containing the media urls
    {
        { eName,  "tvg-name=\""    },
        { eID,    "tvg-id=\""      },
        { eLogo,  "tvg-logo=\""    },
        { eGroup, "group-title=\"" }
    };

    const QString is_radio    = "radio=\"true\"";

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly))
    {
        mOpenFileAtStart = filename;

        bool read_url = false;
        int current_row = 0;

        mListModel->removeRows(0, mListModel->rowCount());
        mListModel->insertRows(current_row, 1, QModelIndex());
        while (!file.atEnd())
        {
            QString line(file.readLine());
            line.resize(line.size()-1);
            if (read_url)           // the url is in the next line
            {
                mListModel->setData(mListModel->index(current_row, eURL, QModelIndex()), line);
                ++current_row;      // insert the nex row regardless, whether the may or not be one more line
                mListModel->insertRows(current_row, 1, QModelIndex());
                read_url = false;   // read tags again
                continue;           // read the next line
            }

            int start = 0;          // read all tags in the first line
            for (auto tag = tvg_tags.begin(); tag != tvg_tags.end(); ++tag )
            {
                start = line.indexOf(tag.value());
                if (start != -1)
                {
                    start += tag.value().size();
                    int end = line.indexOf('\"', start + 1);
                    QString name = line.mid(start, end - start);
                    mListModel->setData(mListModel->index(current_row, tag.key(), QModelIndex()), name);
                }
            }
            if (start == -1) continue; // if no tag matches, read the next line

            start = line.indexOf(",", start);
            if (start != -1)        // the friendly name resides behind the comma
            {
                QString friendly_name = line.mid(start+1);
                QString ordinary_name = get_item_name(current_row);
                if (ordinary_name.size() < friendly_name.size())
                {   // just swap the names, the longer name is the visible name
                    mListModel->setData(mListModel->index(current_row, eFriendlyName, QModelIndex()), ordinary_name);
                    mListModel->setData(mListModel->index(current_row, eName, QModelIndex()), friendly_name);
                }
                else
                {
                    mListModel->setData(mListModel->index(current_row, eFriendlyName, QModelIndex()), friendly_name);
                }
                // insert the checkbox and the media type
                mListModel->setData(mListModel->index(current_row, mChecked, QModelIndex()), true, Qt::CheckStateRole);
                mListModel->setData(mListModel->index(current_row, eMedia, QModelIndex()), line.indexOf(is_radio) != -1 ? txt::radio : txt::tv);
                read_url = true;
            }
        };
        file.close();

        // remove the last row at the end
        mListModel->removeRows(current_row, 1);
        ui->checkBoxSelectAll->setChecked(true);
    }

    for (auto& column : mHiddenColumns)
    {
        ui->tableView->setColumnHidden(column, true);
    }
}

void MainWindow::menu_file_upload_favorites()
{
    if (mUploadFavoriteCommand.size())
    {
        QString filename = QFileDialog::getOpenFileName(this, txt::upload_kodi_fav, mFavoritesOpenPath, txt::kodi_favorites);
        if (filename.size())
        {
            QString command_str = tr(mUploadFavoriteCommand.toStdString().c_str()).arg(filename);
            QString result_str;
            int result = execute(command_str, result_str);
            ui->statusBar->showMessage(tr("upload favorites: %1, %2").arg(result).arg(result_str));
        }
    }
    else
    {
        ui->statusBar->showMessage(tr("Define upload favorite command"));
    }
}

void MainWindow::menu_file_download_favorites()
{
    if (mDownloadFavoriteCommand.size())
    {
        QString filename = QFileDialog::getSaveFileName(this, txt::store_downloaded_kodi_fav, mFavoritesOpenPath, txt::kodi_favorites);
        if (filename.size())
        {
            QString command_str = tr(mDownloadFavoriteCommand.toStdString().c_str()).arg(filename);
            QString result_str;
            int result = execute(command_str, result_str);
            ui->statusBar->showMessage(tr("download favorites: %1, %2").arg(result).arg(result_str));
        }
    }
    else
    {
        ui->statusBar->showMessage(tr("Define download favorite command"));
    }
}

void MainWindow::menu_option_edit_upload_command()
{
    QString text = QInputDialog::getText(this, windowTitle(), txt::upload_favorite_cmd, QLineEdit::Normal, mUploadFavoriteCommand);
    if (text.size())
    {
        mUploadFavoriteCommand = text;
    }
}

void MainWindow::menu_option_edit_download_command()
{
    QString text = QInputDialog::getText(this, windowTitle(), txt::download_favorite_cmd, QLineEdit::Normal, mDownloadFavoriteCommand);
    if (text.size())
    {
        mDownloadFavoriteCommand = text;
    }
}

void MainWindow::menu_option_show_tray_icon(bool show)
{
    if (!m_tray_message && show)
    {
        m_tray_message = new QSystemTrayIcon(QIcon(":/36x36/applications-multimedia.png"), this);
        QMenu*menu = new QMenu;
        QAction*action = menu->addAction(tr("Show %1").arg(windowTitle()));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(traymenu_show_window()));
        action = menu->addAction(tr("Hide %1").arg(windowTitle()));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(traymenu_hide_window()));
        menu->addSeparator();
        menu->addAction(ui->actionAbout);
        menu->addAction(ui->actionInfo);

        m_tray_message->setContextMenu(menu);
    }
    if (m_tray_message)
    {
        m_tray_message->setVisible(show);
    }
}

void MainWindow::metaDataChanged(const QString &key, const QVariant & value)
{
    if (value.isValid())
    {
        QString message = key;
        message += ": ";
        if (value.type() == QVariant::String && key.contains("title", Qt::CaseInsensitive))
        {
            message += value.toString();
            ui->statusBar->showMessage(message);
            if (m_tray_message && m_tray_message->isVisible())
            {
                m_tray_message->setToolTip("<b>" + key + ":</b><br>" + value.toString());
                m_tray_message->showMessage(key, value.toString());
            }
        }
    }
    mCurrentMetainfo[key] = value;

}

QString getSettingsName(const QString& aItemName)
{
    QRegExp fRegEx("([A-Z][A-Za-z0-9:\[]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        const auto captured = fRegEx.capturedTexts();
        QString fTemp = captured[0];
        fTemp = fTemp.replace(":", "_");
        fTemp = fTemp.replace("[", "_");
        return fTemp;
    }
    else return aItemName;
}

CheckboxItemModel::CheckboxItemModel(int rows, int columns, QObject *parent):
    QStandardItemModel(rows, columns, parent),
    mChecked(0)
{
}

QVariant CheckboxItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole && index.column() == mChecked)
    {
        return QStandardItemModel::data(index, role).toBool() ? Qt::Checked : Qt::Unchecked;
    }
    return QStandardItemModel::data(index, role);
}

void CheckboxItemModel::setCheckedColumn(int checked)
{
    mChecked = checked;
}

#ifdef _WIN32
int win_system(const char *command, bool hide)
{
    // Windows has a system() function which works, but it opens a command prompt window.
    unsigned long       ret_val {0};
    PROCESS_INFORMATION process_info = {0};
    STARTUPINFOA        startup_info = {0};

    std::string tmp_command = "/c ";
    tmp_command += command;

    startup_info.cb = sizeof(STARTUPINFOA);
    size_t cmd_exe_path_size = 0;
    char* cmd_exe_path_ptr =nullptr;
    _dupenv_s(&cmd_exe_path_ptr, &cmd_exe_path_size, "COMSPEC");
    _flushall();  // required for Windows system() calls, probably a good idea here too

    if (CreateProcessA(cmd_exe_path_ptr, (LPSTR)tmp_command.c_str(), NULL, NULL, 0, hide ? CREATE_NO_WINDOW:0, NULL, NULL, &startup_info, &process_info))
    {
        WaitForSingleObject(process_info.hProcess, INFINITE);
        GetExitCodeProcess(process_info.hProcess, &ret_val);
        CloseHandle(process_info.hProcess);
        CloseHandle(process_info.hThread);
    }
    else
    {
        ret_val = GetLastError();
    }

    free(cmd_exe_path_ptr);
    return(ret_val);
}
#endif

int execute(const QString& command, QString& aResultText)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    static QRandomGenerator rg(123);
    QDir fTemp = QDir::tempPath() + "/cmd_" + QString::number(rg.generate()) + "_result.tmp";
#else
    QDir fTemp = QDir::tempPath() + "/cmd_" + QString::number(qrand()) + "_result.tmp";
#endif
    QString fTempResultFileNameAndPath = fTemp.path();
    QString system_cmd = command + " > " + fTempResultFileNameAndPath;
#ifdef __linux__
    system_cmd += " 2>&1 ";
    auto fResult = system(system_cmd.toStdString().c_str());
#endif
#ifdef _WIN32
    auto fResult = win_system(system_cmd.toStdString().c_str(), true);
#endif

    std::ostringstream fStringStream;
    std::ifstream fFile(fTempResultFileNameAndPath.toStdString());
    fStringStream << fFile.rdbuf();
    std::string fStreamString = fStringStream.str();
    boost::algorithm::trim_right(fStreamString);
    if (fResult != NoError)
    {
        fStringStream << QObject::tr("Error occurred executing command: ").toStdString() << fResult;
        if (fResult == ErrorNumberInErrno)
        {
            fStringStream << QObject::tr("Error number : ").toStdString() << errno;
        }
    }
    aResultText = fStreamString.c_str();

    fTemp.remove(fTemp.path());
    return fResult;
}
