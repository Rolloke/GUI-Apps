#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"

#include <QDateTime>
#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <QKeyEvent>
#include <QSysInfo>

#include <boost/bind.hpp>

using namespace std;



#define INT(n) static_cast<int>(n)

namespace config
{
    const QString sGroupFilter("Filter");
    const QString sGroupPaths("Paths");
    const QString sSourcePath("Source");
    const QString sUncheckedPath("Unchecked");

} // namespace config


MainWindow::MainWindow(const QString& aConfigName, QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, mCurrentTask(Work::None)
, mBytesCopied(0)
, mBytesToCopy(0)
, mWorker(this)
, mConfigFileName(aConfigName)
, mCopyCommand("cp -p -u \"%1\" \"%2\"")
, mCloseAfterWork(false)
{
    ui->setupUi(this);

    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleWorker, this, _1));
    QObject::connect(this, SIGNAL(doWork(int)), &mWorker, SLOT(doWork(int)));
    mWorker.setMessageFunction(boost::bind(&MainWindow::handleMessage, this, _1, _2));

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    LOAD_PTR(fSettings, ui->editBlacklist, setText, text, toString);
    LOAD_PTR(fSettings, ui->ckHiddenFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckSymbolicLinks, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckSystemFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckDirectories, setChecked, isChecked, toBool);
    LOAD_STR(fSettings, mCopyCommand, toString);
    fSettings.endGroup();

    ui->treeSource->header()->setSectionResizeMode(INT(Column::FileName), QHeaderView::Stretch);
    ui->treeSource->header()->setSectionResizeMode(INT(Column::DateTime), QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(INT(Column::Size)    , QHeaderView::Interactive);
    ui->treeSource->header()->setStretchLastSection(false);

    ui->treeBackup->header()->setSectionResizeMode(INT(Column::FileName), QHeaderView::Stretch);
    ui->treeBackup->header()->setSectionResizeMode(INT(Column::DateTime), QHeaderView::Interactive);
    ui->treeBackup->header()->setSectionResizeMode(INT(Column::Size)    , QHeaderView::Interactive);
    ui->treeBackup->header()->setStretchLastSection(false);

    mBlackList = ui->editBlacklist->text().split(";");

    fSettings.beginGroup(config::sGroupPaths);
    LOAD_STR(fSettings, mDestination, toString);

    int fItemCount = fSettings.beginReadArray(config::sSourcePath);
    for (int fItem = 0; fItem < fItemCount; ++fItem)
    {
        fSettings.setArrayIndex(fItem);
        stringtoboolmap fCheckMap;
        int fCount = fSettings.value(config::sSourcePath+ "/" + config::sUncheckedPath, QVariant(0)).toInt();
        for (int fIndex = 0; fIndex < fCount; ++fIndex)
        {
            fCheckMap[fSettings.value(config::sSourcePath+ "/" + QString::number(fIndex)+ "/" + config::sUncheckedPath, QVariant("")).toString().toStdString()] = true;
        }

        insertItem(initDir(fSettings.value(config::sSourcePath).toString(), fSettings.value(config::sSourcePath+"/"+config::sGroupFilter).toInt()), *ui->treeSource);
        iterateCheckItems(ui->treeSource->topLevelItem(fItem), fCheckMap, true);
    }
    fSettings.endArray();

    fSettings.endGroup();

    if (mDestination.size() && QFileInfo(mDestination).exists())
    {
        insertItem(initDir(mDestination), *ui->treeBackup);
    }
    updateControls();
}

MainWindow::~MainWindow()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    STORE_PTR(fSettings, ui->editBlacklist, text);
    STORE_PTR(fSettings, ui->ckHiddenFiles, isChecked);
    STORE_PTR(fSettings, ui->ckSymbolicLinks, isChecked);
    STORE_PTR(fSettings, ui->ckSystemFiles, isChecked);
    STORE_PTR(fSettings, ui->ckFiles, isChecked);
    STORE_PTR(fSettings, ui->ckDirectories, isChecked);
    STORE_STR(fSettings, mCopyCommand);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);
    STORE_STR(fSettings, mDestination);

    fSettings.beginWriteArray(config::sSourcePath);
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSettings.setArrayIndex(i);
        QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
        stringtoboolmap fCheckMap;
        fSettings.setValue(config::sSourcePath, fItem->text(INT(Column::FileName)));
        fSettings.setValue(config::sSourcePath+ "/" + config::sGroupFilter, fItem->data(INT(Column::FileName), INT(Role::Filter)).toInt());
        iterateCheckItems(fItem, fCheckMap);
        int fIndex = 0;
        for (auto fPath : fCheckMap)
        {
            fSettings.setValue(config::sSourcePath+ "/" + QString::number(fIndex)+ "/" + config::sUncheckedPath, QVariant(QString::fromStdString(fPath.first)));
            ++fIndex;
        }
        fSettings.setValue(config::sSourcePath+ "/" + config::sUncheckedPath, QVariant(fIndex));
    }
    fSettings.endArray();

    fSettings.endGroup();

    delete ui;
}

QString MainWindow::getConfigName() const
{
    QString sConfigFileName = mConfigFileName.size() ? mConfigFileName : windowTitle();
    sConfigFileName.replace(" ", "");
#ifdef __linux__
    return QDir::homePath() + "/.config/" + sConfigFileName + ".ini";
#else

    return "HKEY_CURRENT_USER\\Software\\KESoft\\" + sConfigFileName;
#endif
}

void MainWindow::updateControls()
{
    bool fBackupRestoreEnabled = false;
    if (ui->treeBackup->topLevelItemCount() && ui->treeSource->topLevelItemCount())
    {
        fBackupRestoreEnabled = true;
    }
    ui->btnBackup->setEnabled(fBackupRestoreEnabled);
    ui->btnRestore->setEnabled(fBackupRestoreEnabled);
}

void MainWindow::keyPressEvent(QKeyEvent *aKey)
{
    if (aKey->key() == Qt::Key_Delete)
    {
        if (ui->treeBackup->hasFocus())
        {
            deleteSelectedTreeWidgetItem(*ui->treeBackup);
            updateControls();
        }
        else if (ui->treeSource->hasFocus())
        {
            deleteSelectedTreeWidgetItem(*ui->treeSource);
            updateControls();
        }
    }
}

quint64 MainWindow::insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem)
{
    QDirIterator fIterator(aParentDir, QDirIterator::NoIteratorFlags);
    bool fTopLevelItem(false);
    if (!aParentItem)
    {
        QStringList fStrings;
        fStrings.append(aParentDir.absolutePath());
        aParentItem = new QTreeWidgetItem(fStrings);
        aParentItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|aParentItem->flags());
        aParentItem->setCheckState(INT(Column::FileName), Qt::Checked);
        aParentItem->setData(INT(Column::FileName), INT(Role::isDirectory), QVariant(true));
        aParentItem->setData(INT(Column::FileName), INT(Role::Filter), QVariant(INT(aParentDir.filter())));

        aTree.addTopLevelItem(aParentItem);
        fTopLevelItem = true;
    }

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();
        if (fFileInfo.fileName() == ".") continue;
        if (fFileInfo.fileName() == "..") continue;
        bool isInBlackList = false;
        for (auto fBlackListItem : mBlackList)
        {
            QRegExp fRegEx(fBlackListItem);
            fRegEx.setPatternSyntax(QRegExp::Wildcard);
            if (fRegEx.exactMatch(fFileInfo.fileName()))
            {
                isInBlackList = true;
                break;
            }
        }
        if (isInBlackList) continue;

        QStringList fColumns;
        fColumns.append(fFileInfo.fileName());
        fColumns.append(fFileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
        fColumns.append(formatFileSize(fFileInfo.size()));

        QTreeWidgetItem* fItem = new QTreeWidgetItem(aParentItem, fColumns);
        fItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|fItem->flags());
        fItem->setCheckState(INT(Column::FileName), Qt::Checked);
        fItem->setData(INT(Column::FileName), INT(Role::isDirectory), QVariant(fFileInfo.isDir()));
        fItem->setData(INT(Column::DateTime), INT(Role::DateTime), QVariant(fFileInfo.lastModified()));

        if (fFileInfo.isDir())
        {
            QDir fSubDir(fFileInfo.absoluteFilePath());
            fSubDir.setFilter(aParentDir.filter());
            quint64 fSizeOfSubFolderFiles = insertItem(fSubDir, aTree, fItem);
            fItem->setText(INT(Column::Size), formatFileSize(fSizeOfSubFolderFiles));
            fSizeOfFiles += fSizeOfSubFolderFiles;
            fItem->setData(INT(Column::Size), Qt::SizeHintRole, QVariant(fSizeOfSubFolderFiles));
        }
        else
        {
            fSizeOfFiles += fFileInfo.size();
            fItem->setData(INT(Column::Size), Qt::SizeHintRole, QVariant(fFileInfo.size()));
        }
    }
    while (fIterator.hasNext());

    if (fTopLevelItem)
    {
        aParentItem->setText(INT(Column::Size), formatFileSize(fSizeOfFiles));
    }
    return fSizeOfFiles;
}

bool MainWindow::copyItems(const QTreeWidget& aSourceTree, const QDir& aDestination, const QString* aSourceDir, QTreeWidgetItem* aParentItem)
{
    bool fResult  = false;
    if (aParentItem)
    {
        if (   aParentItem->checkState(INT(Column::FileName)) == Qt::Checked
            || aParentItem->checkState(INT(Column::FileName)) == Qt::PartiallyChecked)
        {
            const QVariant& fIsDir = aParentItem->data(INT(Column::FileName), INT(Role::isDirectory));
            QString fFileName = aParentItem->text(INT(Column::FileName));
            QString fDestination;
            QString fSource;
            if (!fFileName.startsWith(QDir::separator()))
            {
                fFileName =  QDir::separator() + fFileName;
            }
            switch (mCurrentTask)
            {
            case Work::Backup:
                fSource      = *aSourceDir + fFileName;
#ifndef __linux__
                if (fFileName.contains(":"))
                {
                    fFileName.replace(QRegExp(":(\\\\|/)"), "__Drive\\");
                    fSource.replace(0,1,"");
                }
#endif
                fDestination = aDestination.absolutePath() + fFileName;
                break;
            case Work::Restore:
            {
                if (aDestination.isRelative())
                {
                    fDestination = QDir::separator();
                    fSource      = fFileName;
                }
                else
                {
                    fDestination = aDestination.absolutePath() + fFileName;
                    fSource      = *aSourceDir + fFileName;
                }
            }   break;
            default: return false;
            }
            if (fIsDir.toBool())
            {
                fResult = aDestination.mkpath(fDestination);
                if (fResult)
                {
                    for (int fChildItem=0; fChildItem < aParentItem->childCount(); ++fChildItem)
                    {
                        fResult = copyItems(aSourceTree, fDestination, &fSource, aParentItem->child(fChildItem));
                        if (!fResult)
                        {
                            TRACE(Logger::warning, "Stopping to continue copy at index %d of %d, %s -> %s", fChildItem+1, aParentItem->childCount(),
                                  fSource.toStdString().c_str(), fDestination.toStdString().c_str());
                            break;
                        }
                    }
                }
                else
                {
                    TRACE(Logger::error, "Error creating target directory %s", fDestination.toStdString().c_str());
                }
            }
            else
            {
                fResult = copy_file(fSource, fDestination);
                if (fResult)
                {
                    QFileInfo fInfo(fSource);
                    mWorker.sendMessage(INT(Message::UpdateBytes), QVariant(fInfo.size()));
                }
                else
                {
                    TRACE(Logger::error, "Error copying file %s -> %s", fSource.toStdString().c_str(), fDestination.toStdString().c_str());
                    fResult = true;
                }

            }
        }
        else
        {
            TRACE(Logger::info, "Not copying unselected file %s", aParentItem->text(INT(Column::FileName)).toStdString().c_str());
            fResult = true; // this is not an error
        }
    }
    else
    {
        fResult = true;
        for (int fIndex = 0; fIndex < aSourceTree.topLevelItemCount(); ++fIndex)
        {
            QString fSourceDir("");
            if (mCurrentTask == Work::Backup)
            {
                TRACE(Logger::notice, "Backup Source: %s", aSourceTree.topLevelItem(fIndex)->text(INT(Column::FileName)).toStdString().c_str());
            }
            fResult &= copyItems(aSourceTree, aDestination, &fSourceDir, aSourceTree.topLevelItem(fIndex));
        }
    }
    return fResult;
}

bool MainWindow::iterateCheckItems(QTreeWidgetItem* aParentItem, std::map<std::string, bool>& aPathMap, bool aSet, const QString* aSourceDir)
{
    if (aParentItem)
    {
        QString fSourcePath = aSourceDir ? *aSourceDir + QDir::separator() + aParentItem->text(INT(Column::FileName)) : aParentItem->text(INT(Column::FileName));

        if (aSet)
        {
            if (aPathMap.find(fSourcePath.toStdString()) != aPathMap.end())
            {
                aParentItem->setCheckState(0, Qt::Unchecked);
            }
            for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
            {
                iterateCheckItems(aParentItem->child(fChild), aPathMap, aSet, &fSourcePath);
            }
        }
        else
        {
            switch (aParentItem->checkState(INT(Column::FileName)))
            {
            case Qt::Unchecked:
                aPathMap[fSourcePath.toStdString()] = true;
                break;
            case Qt::PartiallyChecked:
                for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
                {
                    iterateCheckItems(aParentItem->child(fChild), aPathMap, aSet, &fSourcePath);
                }
                break;
            case  Qt::Checked:
                return true;
            }
            return true;
        }
    }
    return false;
}

quint64 MainWindow::sizeOfCheckedItems(QTreeWidgetItem* aParentItem)
{
    quint64 fSize = 0;
    if (aParentItem)
    {
        bool fIterate = false;
        switch (aParentItem->checkState(INT(Column::FileName)))
        {
        case Qt::Unchecked: return fSize;
        case Qt::PartiallyChecked:
            fIterate = true;
            break;
        case  Qt::Checked:
            if (!aParentItem->data(INT(Column::FileName), INT(Role::isDirectory)).toBool())
            {
                fSize = aParentItem->data(INT(Column::Size), Qt::SizeHintRole).toLongLong();
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

QDir MainWindow::initDir(const QString& aDirPath, int aFilter)
{
    int fFilter = QDir::Drives;
    if (aFilter)
    {
        fFilter = aFilter;
    }
    else
    {
        if (ui->ckFiles)          fFilter |= QDir::Files;
        if (ui->ckDirectories)    fFilter |= QDir::Dirs;
        if (ui->ckHiddenFiles)    fFilter |= QDir::Hidden;
        if (ui->ckSystemFiles)    fFilter |= QDir::System;
        if (!ui->ckSymbolicLinks) fFilter |= QDir::NoSymLinks;
    }

    QDir fDir;
    fDir.setPath(aDirPath);
    fDir.setFilter(static_cast<QDir::Filter>(fFilter));
    return fDir;
}


void MainWindow::selectDestination()
{
    deleteAllTreeWidgetItem(*ui->treeBackup);
    mDestination = QFileDialog::getExistingDirectory(this, "Select Destination");

    insertItem(initDir(mDestination, QDir::AllEntries|QDir::Hidden|QDir::System), *ui->treeBackup);
    updateControls();
}

void MainWindow::selectSourceFolder()
{
    QString fSourcePath = QFileDialog::getExistingDirectory(this, "Select SourceFiles");

    insertItem(initDir(fSourcePath), *ui->treeSource);
    updateControls();
}


void MainWindow::on_btnRestore_clicked()
{
    if (ui->btnRestore->isEnabled())
    {
        Q_EMIT doWork(INT(Work::Restore));
    }
}


void MainWindow::on_btnCancel_clicked()
{
    mCurrentTask = Work::None;
}

void MainWindow::doAutoBackup()
{
    mCloseAfterWork = true;
    on_btnBackup_clicked();
}

void MainWindow::on_btnBackup_clicked()
{
    if (ui->btnBackup->isEnabled())
    {
        Q_EMIT doWork(INT(Work::Backup));
    }
}

void MainWindow::on_btnAddSourceFolder_clicked()
{
    selectSourceFolder();
}

void MainWindow::on_btnSelectDestination_clicked()
{
    selectDestination();
}

void MainWindow::on_btnUpdateSearch_clicked()
{
    qint64 fSize = 0;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSize += sizeOfCheckedItems(ui->treeSource->topLevelItem(i));
    }

    ui->statusBar->showMessage("Total selected bytes: " + formatFileSize(fSize));
    // TODO on_btnUpdateSearch_clicked
}

void MainWindow::on_editBlacklist_textEdited(const QString &aList)
{
    mBlackList = aList.split(";");
}

void MainWindow::handleWorker(int aWork)
{
    Logger::printDebug(Logger::trace, "handleWorker(%d): %x", aWork, QThread::currentThreadId());
    mCurrentTask = static_cast<Work>(aWork);
    switch(static_cast<Work>(aWork))
    {
    case Work::Backup:  copyItems(*ui->treeSource, mDestination); break;
    case Work::Restore: copyItems(*ui->treeBackup, QDir(""));     break;
    default:
        Logger::printDebug(Logger::error, "handleWorker(%d) Id is unknown", aWork);
        break;
    }
    mCurrentTask = Work::None;
}

void MainWindow::handleMessage(int aMsg, QVariant aData)
{
    Logger::printDebug(Logger::trace, "handleMessage(%d): %x, %s", aMsg, QThread::currentThreadId(), aData.typeName());
    switch(static_cast<Work>(aMsg))
    {
    case Work::Backup:  messageBackup(aData.toBool()); break;
    case Work::Restore: messageRestore(aData.toBool()); break;
    default:
        switch(static_cast<Message>(aMsg))
        {
        case Message::UpdateBytes: messageUpdateBytes(aData.toLongLong()); break;
        default:
            Logger::printDebug(Logger::error, "handleWorker(%d) Id is unknown: %s", aMsg, aData.typeName());
            break;
        }
        break;
    }
}

void MainWindow::messageBackup(bool aStart)
{
    if (aStart)
    {
        mStartTime.start();
        Logger::openLogFile((mDestination + QDir::separator() + "Backup.log").toStdString());
        QDateTime fNow = QDateTime::currentDateTime();
        TRACE(Logger::notice, "Backup date: %s", fNow.toString().toStdString().c_str());
        TRACE(Logger::notice, "Backup host: %s", QSysInfo::machineHostName().toStdString().c_str());
        mBytesCopied = 0;
        mBytesToCopy = 0;
        for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
        {
            mBytesToCopy += sizeOfCheckedItems(ui->treeSource->topLevelItem(i));
        }
    }
    else
    {
        double fPredictedTime_s = mStartTime.elapsed() * 0.001;
        int    fPredictedtime_m = static_cast<int>(fPredictedTime_s / 60.0);
        QString fPredictedTime = QString::asprintf("%02d:%02d",fPredictedtime_m, static_cast<int>(fPredictedTime_s) - 60 * fPredictedtime_m);
        TRACE(Logger::notice, "Bytes copied: %d of %d", mBytesCopied, mBytesToCopy);
        Logger::closeLogFile();
        ui->statusBar->showMessage("Finished Backup in " + fPredictedTime + " min");
        deleteAllTreeWidgetItem(*ui->treeBackup);
        insertItem(initDir(mDestination), *ui->treeBackup);
        if (mCloseAfterWork)
        {
            close();
        }
    }
    ui->btnBackup->setEnabled(!aStart);
}

void MainWindow::messageRestore(bool aStart)
{
    ui->btnRestore->setEnabled(!aStart);
    if (aStart)
    {
        mBytesCopied = 0;
        mBytesToCopy = 0;
        for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
        {
            mBytesToCopy += sizeOfCheckedItems(ui->treeBackup->topLevelItem(i));
        }
    }
    else
    {
        ui->statusBar->showMessage("Finished Restore");
        // TODO Show restored Items
        if (mCloseAfterWork)
        {
            close();
        }
    }
}

void MainWindow::messageUpdateBytes(qlonglong aBytes)
{
    double fElapsed_s = 0.001 * mStartTime.elapsed();
    mBytesCopied += aBytes;
    double fPredictedTime_s = static_cast<double>(mBytesToCopy - mBytesCopied) / (static_cast<double>(mBytesCopied) / fElapsed_s);
    int    fPredictedtime_m = static_cast<int>(fPredictedTime_s / 60.0);
    QString fPredictedTime = QString::asprintf("%02d:%02d", fPredictedtime_m, static_cast<int>(fPredictedTime_s) - 60 * fPredictedtime_m);
    QString fPercent = QString::number(100.0 * mBytesCopied / mBytesToCopy, 'f', 1);
    ui->statusBar->showMessage("Bytes copied: " + formatFileSize(mBytesCopied) + " of " + formatFileSize(mBytesToCopy) + ": "
                               + fPercent + "%" + ", estimated duration: " + fPredictedTime + "min");
}

bool MainWindow::copy_file(const QString& fSource, const QString& fDestination)
{
#ifdef __linux__
    QString fCommand = mCopyCommand.arg(fSource).arg(fDestination);
    int fResult = system(fCommand.toStdString().c_str());
    return fResult == 0;
#else
    return QFile::copy(fSource, fDestination);
#endif
}


//void MainWindow::on_treeSource_itemClicked(QTreeWidgetItem *item, int column)
//{
//    TRACE(Logger::notice, "on_treeSource_itemClicked(%s, %d)", item->text(column).toStdString().c_str(), column);
//}

//void MainWindow::on_treeSource_clicked(const QModelIndex &index)
//{

//}
