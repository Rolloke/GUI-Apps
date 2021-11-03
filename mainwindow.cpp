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
constexpr char sGroupSettings[] = "Settinga";
}

enum eTable
{
    eName, eID, eGroup, eURL, eLogo, eFriendlyName, eLast = eFriendlyName
};

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
, ui(new Ui::MainWindow)
, mFileOpenPath(QDir::homePath())
{
    ui->setupUi(this);
    mPlayer.setVideoOutput(&mVideo);

    QStringList fSectionNames = { tr("Name"), tr("ID"), tr("Gruppe"), tr("URL"), tr("Logo"), tr("Friendly Name")};
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    mListModel = new QStandardItemModel(0, eLast, this);
    for (int fSection = 0; fSection <= eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
        ui->comboBoxSearchColumn->addItem(fSectionNames[fSection], fSection);
    }
    ui->comboBoxSearchColumn->setCurrentIndex(eID);
    ui->tableView->setModel(mListModel);

    fSettings.beginGroup(config::sGroupSettings);

    LOAD_PTR(fSettings, ui->comboBoxSearchColumn, setCurrentIndex, currentIndex, toInt);
    LOAD_STR(fSettings, mFileOpenPath, toString);

    fSettings.endGroup();
}

MainWindow::~MainWindow()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupSettings);
    STORE_PTR(fSettings, ui->comboBoxSearchColumn, currentIndex);
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


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString mediaUrl = mListModel->data(mListModel->index(index.row(), eURL)).toString();
    mPlayer.setMedia(QUrl(mediaUrl));
    mPlayer.setVolume(50);
    mPlayer.play();
    if (!mIsRadio[index.row()])
    {
        mVideo.show();
    }
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
    const QString tvg_name    = "tvg-name=";
    const QString tvg_id      = "tvg-id=";
    const QString tvg_logo    = "tvg-logo=";
    const QString group_title = "group-title=";
    const QString is_radio    = "radio=\"true\"";

    QString filename = QFileDialog::getOpenFileName(this, "Open Media List", mFileOpenPath, tr("Media List (*.txt *.*)"));
    QFileInfo info(filename);
    mFileOpenPath = info.dir().absolutePath();

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly))
    {
        mListModel->removeRows(0, mListModel->rowCount());
        mIsRadio.clear();
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
                mListModel->setData(mListModel->index(fRow, eName, QModelIndex()), name);
                mIsRadio.append(line.indexOf(is_radio) != -1);
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
