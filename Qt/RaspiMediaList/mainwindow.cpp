#include "mainwindow.h"
#include "ui_mainwindow.h"

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


#define STORE_PTR(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)   SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)        SETTING.setValue(getSettingsName(#ITEM), ITEM)
#define STORE_STRF(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), FUNC(ITEM))

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT)  ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)   ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)            ITEM=SETTING.value(getSettingsName(#ITEM), QVariant(ITEM)).CONVERT()
#define LOAD_STRF(SETTING, ITEM, FUNC_OUT, FUNC_IN, CONVERT) ITEM = FUNC_OUT(fSettings.value(getSettingsName(#ITEM), QVariant(FUNC_IN(ITEM))).CONVERT());

QString getSettingsName(const QString& aItemName);

namespace config
{
constexpr char sGroupSettings[] = "Settings";
}

namespace txt
{
const QString radio           = QObject::tr("Radio");
const QString tv              = QObject::tr("TV");
const QString open_media_list = QObject::tr("Open Kodi raw media List");
const QString media_list      = QObject::tr("Media List (*.txt *.*)");

const QString store_kodi_fav  = QObject::tr("Store media list as favorites for Raspi");
const QString update_kodi_fav = QObject::tr("Load favorites for update media list check states");
const QString kodi_favorites  = QObject::tr("Favorites (*.xml *.*)");

}

enum eTable
{
    eName, eID, eGroup, eURL, eLogo, eFriendlyName, eMedia, eLast
};

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
, ui(new Ui::MainWindow)
, mFileOpenPath(QDir::homePath())
, mFavoritesOpenPath(QDir::homePath())
, mPlayer(this)
, mListModel(nullptr)
, mCurrentRowIndex(-1)
, mFindStartRow(0)
, mShowIcon(true)
{
    ui->setupUi(this);
    mPlayer.setVideoOutput(&mVideo);
    connect(&mPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(show_media_player_error(QMediaPlayer::Error)));

    mHiddenColumns = {eURL, eLogo, eFriendlyName };
    QStringList fSectionNames = { tr("Name"), tr("ID"), tr("Group"), tr("URL"), tr("Logo"), tr("Friendly Name"), tr("Medium")};
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
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
    ui->graphicsView->setScene(new QGraphicsScene ());

    connect(ui->actionOpen_Kodi_raw_list, SIGNAL(triggered(bool)), SLOT(on_menu_file_open()));
    connect(ui->actionSave_as_favorites, SIGNAL(triggered(bool)), SLOT(on_menu_file_save_as_favorites()));
    connect(ui->actionRead_favorites, SIGNAL(triggered(bool)), SLOT(on_menu_file_update_favorites()));

    connect(ui->actionAbout, SIGNAL(triggered(bool)), SLOT(on_menu_help_about()));
    connect(ui->actionInfo, SIGNAL(triggered(bool)), SLOT(on_menu_help_info()));

    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(100);
    ui->sliderVolume->setTickPosition(QSlider::TicksBothSides);
    ui->sliderVolume->setTickInterval(10);
    ui->sliderVolume->setSingleStep(1);

    fSettings.beginGroup(config::sGroupSettings);

    LOAD_PTR(fSettings, ui->comboBoxSearchColumn, setCurrentIndex, currentIndex, toInt);
    LOAD_PTR(fSettings, ui->sliderVolume, setValue, value, toInt);
    LOAD_STR(fSettings, mFileOpenPath, toString);
    LOAD_STR(fSettings, mFavoritesOpenPath, toString);
    LOAD_STR(fSettings, mShowIcon, toBool);

    fSettings.endGroup();

    const QString arg_file        = "--file=";
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
        }
    }
    if (filename.size())
    {
        open_file(filename);
    }
}

MainWindow::~MainWindow()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupSettings);
    STORE_PTR(fSettings, ui->comboBoxSearchColumn, currentIndex);
    STORE_PTR(fSettings, ui->sliderVolume, value);
    STORE_STR(fSettings, mFileOpenPath);
    STORE_STR(fSettings, mFavoritesOpenPath);
    STORE_STR(fSettings, mShowIcon);
    fSettings.endGroup();

    mPlayer.stop();
    delete ui;
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

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    if (index.column() == eName)
    {
        mListModel->setData(index, !mListModel->data(index, Qt::CheckStateRole).toBool(), Qt::CheckStateRole);
    }
    ui->statusBar->showMessage(mListModel->data(mListModel->index(index.row(), eName)).toString());
    mCurrentRowIndex = index.row();

    if (mShowIcon)
    {
        ui->graphicsView->scene()->clear();
        QString logoUrl = mListModel->data(mListModel->index(index.row(), eLogo)).toString();
        const QUrl url(logoUrl);
        QNetworkRequest request(url);

        QNetworkReply* reply = mNetManager.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
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
        mPlayer.setMedia(QUrl(mListModel->data(mListModel->index(mCurrentRowIndex, eURL)).toString()));
        if (mListModel->data(mListModel->index(mCurrentRowIndex, eMedia)).toString() == txt::tv)
        {
            mVideo.show();
        }
        mPlayer.play();
    }
}

void MainWindow::on_sliderVolume_valueChanged(int value)
{
    mPlayer.setVolume(value);
}

void MainWindow::on_pushButtonStop_clicked()
{
    mPlayer.stop();
    if (mVideo.isVisible())
    {
        mVideo.hide();
    }
}

void MainWindow::on_checkBoxSelectAll_clicked(bool checked)
{
    const int table_rows = mListModel->rowCount();
    for (int current_row = 0; current_row < table_rows; ++current_row)
    {
        mListModel->setData(mListModel->index(current_row, eName), checked, Qt::CheckStateRole);
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
            bool checked = mListModel->data(mListModel->index(current_row, eName),  Qt::CheckStateRole).toBool();
            mListModel->setData(mListModel->index(current_row, eName), !checked, Qt::CheckStateRole);
        }
        if (mListModel->data(mListModel->index(current_row, eName),  Qt::CheckStateRole).toBool())
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
            ui->tableView->selectRow(current_row);
            const auto index = mListModel->index(current_row, eID);
            ui->tableView->scrollTo(index);
            on_tableView_clicked(index);
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

void MainWindow::on_lineEditSelection_textChanged(const QString &)
{
    mFindStartRow = 0;
}

void MainWindow::on_menu_file_save_as_favorites()
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
                if (mListModel->data(mListModel->index(current_row, eName), Qt::CheckStateRole).toBool())
                {
                    QString line = tr("<favourite name=\"%1\" thumb=\"%2\">PlayMedia(&quot;%3&quot;)</favourite>\n").
                            arg(mListModel->data(mListModel->index(current_row, eName)).toString(),
                            mListModel->data(mListModel->index(current_row, eLogo)).toString(),
                            mListModel->data(mListModel->index(current_row, eURL)).toString());
                    file.write(line.toStdString().c_str());
                }
            }
            file.write("</favourites>\n");
        }
    }
}

void MainWindow::on_menu_file_update_favorites()
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

            const QString tag = "<favourite name=\"";

            while (!file.atEnd())
            {
                QString line(file.readLine());
                line.resize(line.size()-1);
                int start = line.indexOf(tag);
                if (start != -1)
                {
                    start += tag.size();
                    int end = line.indexOf('\"', start + 1);
                    const int table_rows = mListModel->rowCount();

                    QString select_text = line.mid(start, end - start);
                    for (int current_row = 0; current_row < table_rows; ++current_row)
                    {
                        if (mListModel->data(mListModel->index(current_row, eName)).toString().indexOf(select_text, 0, Qt::CaseSensitive) != -1)
                        {
                            mListModel->setData(mListModel->index(current_row, eName), true, Qt::CheckStateRole);
                            break;
                        }
                    }
                }
            };
            file.close();
        }
    }
}

void MainWindow::on_menu_file_open()
{
    QString filename = QFileDialog::getOpenFileName(this, txt::open_media_list, mFileOpenPath, txt::media_list);
    if (filename.size())
    {
        QFileInfo info(filename);
        mFileOpenPath = info.dir().absolutePath();
        open_file(filename);
    }
}

void MainWindow::on_menu_help_about()
{
    QMessageBox::about(this, windowTitle(),
         tr("About Kodi media list viewer and editor\n"
            "\n"
            "The program is provided AS IS with NO WARRANTY OF ANY KIND\n"
            "\n"
            "Build on:\t%1 , %2\n"
            "Author:\tRolf Kary Ehlers\n"
            "Version:\t1.0.0.0\n"
            "License:\tGNU GPL Version 2\n"
            "Email:\trolf-kary-ehlers@t-online.de\n").arg(__DATE__, __TIME__));
}

void MainWindow::on_menu_help_info()
{
    if (mCurrentRowIndex != -1)
    {
        QMessageBox::about(this, windowTitle(),
        tr("Information about selected item\n"
           "\n"
           "Name:\t%1\n"
           "Web:\t%2\n"
           "Media URL:\t%3\n\n"
           "Thumb:\t%4\n").arg(
           mListModel->data(mListModel->index(mCurrentRowIndex, eName)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eID)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eURL)).toString(),
           mListModel->data(mListModel->index(mCurrentRowIndex, eLogo)).toString()));
    }
}

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
    ui->statusBar->showMessage(tr("%1: %2").arg(static_cast<int>(error)).arg(mPlayer.errorString()));
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
                QString name = line.mid(start+1);
                mListModel->setData(mListModel->index(current_row, eFriendlyName, QModelIndex()), name);
                // insert the checkbox and the media type
                mListModel->setData(mListModel->index(current_row, eName, QModelIndex()), true, Qt::CheckStateRole);
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
    QStandardItemModel(rows, columns, parent)
{
}

QVariant CheckboxItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole && index.column() == eName)
    {
        return QStandardItemModel::data(index, role).toBool() ? Qt::Checked : Qt::Unchecked;
    }
    return QStandardItemModel::data(index, role);
}

