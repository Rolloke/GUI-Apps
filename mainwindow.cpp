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


#define STORE_PTR(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)   SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)        SETTING.setValue(getSettingsName(#ITEM), ITEM)
#define STORE_STRF(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), FUNC(ITEM))

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT)  ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)   ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)            ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()
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
const QString open_media_list = QObject::tr("Open Media List");
const QString media_list      = QObject::tr("Media List (*.txt *.*)");
}

enum eTable
{
    eName, eID, eGroup, eURL, eLogo, eFriendlyName, eDestination, eLast
};

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
, ui(new Ui::MainWindow)
, mFileOpenPath(QDir::homePath())
{
    ui->setupUi(this);
    mPlayer.setVideoOutput(&mVideo);

    QStringList fSectionNames = { tr("Name"), tr("ID"), tr("Gruppe"), tr("URL"), tr("Logo"), tr("Friendly Name"), tr("Medium")};
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    mListModel = new CheckboxItemModel(0, eLast, this);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
        ui->comboBoxSearchColumn->addItem(fSectionNames[fSection], fSection);
    }

    ui->comboBoxSearchColumn->setCurrentIndex(eID);
    ui->tableView->setModel(mListModel);
    ui->graphicsView->setScene(new QGraphicsScene ());

    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(100);
    ui->sliderVolume->setTickPosition(QSlider::TicksBothSides);
    ui->sliderVolume->setTickInterval(10);
    ui->sliderVolume->setSingleStep(1);

    fSettings.beginGroup(config::sGroupSettings);

    LOAD_PTR(fSettings, ui->comboBoxSearchColumn, setCurrentIndex, currentIndex, toInt);
    LOAD_PTR(fSettings, ui->sliderVolume, setValue, value, toInt);
    LOAD_STR(fSettings, mFileOpenPath, toString);

    fSettings.endGroup();
}

MainWindow::~MainWindow()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupSettings);
    STORE_PTR(fSettings, ui->comboBoxSearchColumn, currentIndex);
    STORE_PTR(fSettings, ui->sliderVolume, value);
    STORE_STR(fSettings, mFileOpenPath);
    fSettings.endGroup();

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
    mCurrentUrl = mListModel->data(mListModel->index(index.row(), eURL)).toString();
    mCurrentDestination = mListModel->data(mListModel->index(index.row(), eDestination)).toString();

    ui->graphicsView->scene()->clear();
    QString logoUrl = mListModel->data(mListModel->index(index.row(), eLogo)).toString();
    const QUrl url(logoUrl);
    QNetworkRequest request(url);

    QNetworkReply* reply = mNetManager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
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
            ui->statusBar->showMessage("image loaded");
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

void MainWindow::on_tableView_doubleClicked(const QModelIndex &)
{
    on_pushButtonStart_clicked();
}

void MainWindow::on_pushButtonStart_clicked()
{
    mPlayer.setMedia(QUrl(mCurrentUrl));
    if (mCurrentDestination == txt::tv)
    {
        mVideo.show();
    }
    mPlayer.play();
}

void MainWindow::on_sliderVolume_valueChanged(int value)
{
    mPlayer.setVolume(value);
}

void MainWindow::on_pushButtonStop_clicked()
{
    mPlayer.stop();
}

void MainWindow::on_pushButtonSelect_clicked()
{
    QString fSelect = ui->lineEditSelection->text();
    int fRows = mListModel->rowCount();
    int fSection = ui->comboBoxSearchColumn->currentIndex();
    for (int fRow=0; fRow < fRows; ++fRow)
    {
        if (mListModel->data(mListModel->index(fRow, fSection)).toString().indexOf(fSelect) != -1)
        {
            ui->tableView->selectionModel()->select(mListModel->index(fRow, fSection), QItemSelectionModel::Select);
        }
    }
}

void MainWindow::on_pushButtonOpen_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, txt::open_media_list, mFileOpenPath, txt::media_list);
    QFileInfo info(filename);
    mFileOpenPath = info.dir().absolutePath();
    open_file(filename);
}

void MainWindow::open_file(const QString& filename)
{
    const QString tvg_name    = "tvg-name=";
    const QString tvg_id      = "tvg-id=";
    const QString tvg_logo    = "tvg-logo=";
    const QString group_title = "group-title=";
    const QString is_radio    = "radio=\"true\"";

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly))
    {
        mListModel->removeRows(0, mListModel->rowCount());
        bool read_url = false;
        int fRow = 0;
        while (!file.atEnd())
        {
            QString line(file.readLine());
            line.resize(line.size()-1);
            if (read_url)
            {
                mListModel->setData(mListModel->index(fRow, eURL, QModelIndex()), line);
                ++fRow;
                read_url = false;
                continue;
            }
            int start = line.indexOf(tvg_name);
            if (start != -1)
            {
                start += tvg_name.size();
                mListModel->insertRows(fRow, 1, QModelIndex());
                int end = line.indexOf('\"', start + 2);
                QString name = line.mid(start+1, end - start -1);
                auto index = mListModel->index(fRow, eName, QModelIndex());
                mListModel->setData(index, name);
                mListModel->setData(index, true, Qt::CheckStateRole);
                mListModel->setData(mListModel->index(fRow, eDestination, QModelIndex()), line.indexOf(is_radio) != -1 ? txt::radio : txt::tv);
            }
            else
            {
                continue;
            }

            start = line.indexOf(tvg_id);
            if (start != -1)
            {
                start += tvg_id.size();
                int end = line.indexOf('\"', start + 2);
                QString name = line.mid(start+1, end - start -1);
                mListModel->setData(mListModel->index(fRow, eID, QModelIndex()), name);
            }

            start = line.indexOf(group_title);
            if (start != -1)
            {
                start += group_title.size();
                int end = line.indexOf('\"', start + 2);
                QString name = line.mid(start+1, end - start -1);
                mListModel->setData(mListModel->index(fRow, eGroup, QModelIndex()), name);
            }

            start = line.indexOf(tvg_logo, start);
            if (start != -1)
            {
                start += tvg_logo.size();
                int end = line.indexOf('\"', start + 2);
                QString name = line.mid(start+1, end - start -1);
                start = end;
                mListModel->setData(mListModel->index(fRow, eLogo, QModelIndex()), name);
            }

            start = line.indexOf(",", start);
            if (start != -1)
            {
                QString name = line.mid(start+1);
                mListModel->setData(mListModel->index(fRow, eFriendlyName, QModelIndex()), name);
                read_url = true;
            }
        };
        file.close();
    }

    ui->tableView->setColumnHidden(eURL, true);
    ui->tableView->setColumnHidden(eLogo, true);
    ui->tableView->setColumnHidden(eFriendlyName, true);
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
