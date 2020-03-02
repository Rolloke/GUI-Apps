#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "commitmessage.h"
#include "history.h"

#include <QDateTime>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QKeyEvent>
#include <QSysInfo>
#include <QMenu>

#include <boost/bind.hpp>


using namespace std;


// TODO: Eintragene Remote Repositories inkl. URLs ausgeben:
// git remote -v
// Beispiel:
// Raspi	pi@RaspiLAN:git.repository (fetch)
// Raspi	pi@RaspiLAN:git.repository (push)
// TODO: die Remote Namen Speichern

// Kapitel 1.4.12.2 Entfernte Referenzen anpassen
// TODO:  Entferntes Repository hinzufügen:
// git remote add < Name > <URL >
// Referenz entfernen:
// git remote remove < Name >
// URL korrigieren:
// git remote set - url < Name > <URL >

// TODO: Änderungen auf Remote Repository übertragen
// git push pi@RaspiLAN:git.repository

// TODO: Änderungen vom Remote Repository abfragen
// git ?

// TODO: Änderungen vom Remote Repository abholen
// git fetch pi@RaspiLAN:git.repository

// TODO: Datei(en) oder Verzeichnis(se) entfernen
// git rm --cached file/wildcard/directory
// TODO entfernte im Treview einfügen
// TODO git status (Zugehörigkeit) einer Datei erkennen

// TODO: Datei(en) oder Verzeichnis(se) Änderungen auschecken

// TODO: 1.4.10.2 Zweig erstellen


#define INT(n) static_cast<qint32>(n)

namespace config
{
const QString sGroupFilter("Filter");
const QString sGroupView("View");
const QString sGroupPaths("Paths");
const QString sSourcePath("Source");
const QString sUncheckedPath("Unchecked");
const QString sGroupLogging("Logging");
const QString sGroupGitCommands("GitCommands");
const QString sCommands("Commands");
const QString sCommand("Command");
const QString sID("ID");
const QString sName("Name");
const QString sCustomMessageBoxText("MessageBoxText");
const QString sCustomCommandPostAction("PostAction");
} // namespace config


const QString sNoCustomCommandMessageBox("None");


using namespace git;

MainWindow::MainWindow(const QString& aConfigName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mWorker(this)
    , mCurrentTask(Work::None)
    , mConfigFileName(aConfigName)
{

    ui->setupUi(this);

    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleWorker, this, _1));
    QObject::connect(this, SIGNAL(doWork(int)), &mWorker, SLOT(doWork(int)));
    mWorker.setMessageFunction(boost::bind(&MainWindow::handleMessage, this, _1, _2));
    connect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(textBrowserChanged()));

    mIgnoreMap[Folder::FolderSelf]    = Type(Type::Folder);
    mIgnoreMap[Folder::FolderUp]      = Type(Type::Folder);
    mIgnoreMap[Folder::GitRepository] = Type(Type::GitFolder);

    ui->treeSource->header()->setSectionResizeMode(INT(Column::FileName), QHeaderView::Stretch);
    ui->treeSource->header()->setSectionResizeMode(INT(Column::DateTime), QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(INT(Column::Size)    , QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(INT(Column::State)   , QHeaderView::Interactive);
    ui->treeSource->header()->setStretchLastSection(false);

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    LOAD_PTR(fSettings, ui->ckHiddenFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckSymbolicLinks, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckSystemFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckDirectories, setChecked, isChecked, toBool);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupView);
    LOAD_PTR(fSettings, ui->ckHideEmptyParent, setChecked, isChecked, toBool);
    fSettings.endGroup();


    fSettings.beginGroup(config::sGroupLogging);
    QString fSeverity;
    LOAD_STR(fSettings, fSeverity, toString);
    std::uint32_t fSeverityValue = fSeverity.toInt(0, 2);
    Logger::setSeverity(0xffff, false);
    Logger::setSeverity(fSeverityValue, true);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);
    ui->treeHistory->setVisible(false);

    int fItemCount = fSettings.beginReadArray(config::sSourcePath);
    for (int fItem = 0; fItem < fItemCount; ++fItem)
    {
        fSettings.setArrayIndex(fItem);
        const QDir fSourceDir = initDir(fSettings.value(config::sSourcePath).toString(), fSettings.value(config::sSourcePath+"/"+config::sGroupFilter).toInt());
        insertSourceTree(fSourceDir, fItem);
    }
    fSettings.endArray();

    fSettings.endGroup();

    ui->treeSource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeHistory->setContextMenuPolicy(Qt::CustomContextMenu);

    initContextMenuActions();
}

MainWindow::~MainWindow()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    STORE_PTR(fSettings, ui->ckHiddenFiles, isChecked);
    STORE_PTR(fSettings, ui->ckSymbolicLinks, isChecked);
    STORE_PTR(fSettings, ui->ckSystemFiles, isChecked);
    STORE_PTR(fSettings, ui->ckFiles, isChecked);
    STORE_PTR(fSettings, ui->ckDirectories, isChecked);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupView);
    STORE_PTR(fSettings, ui->ckHideEmptyParent, isChecked);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);
    fSettings.beginWriteArray(config::sSourcePath);
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSettings.setArrayIndex(i);
        QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
        fSettings.setValue(config::sSourcePath, fItem->text(INT(Column::FileName)));
    }
    fSettings.endArray();
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupGitCommands);
    fSettings.beginWriteArray(config::sCommands);
    {
        int fIndex = 0;
        int fCommandID = git::Cmd::CustomCommand;
        tActionMap::const_iterator fItem;
        while ((fItem = mActionList.find(fCommandID)) != mActionList.end())
        {
            const QAction* fAction = fItem->second;
            QString fCommand = fAction->statusTip();
            if (fCommand.size())
            {
                fSettings.setArrayIndex(fIndex++);
                fSettings.setValue(config::sID, fItem->first);
                fSettings.setValue(config::sName, fAction->text());
                fSettings.setValue(config::sCommand, fCommand);
                fSettings.setValue(config::sCustomMessageBoxText, fAction->toolTip());
                fSettings.setValue(config::sCustomCommandPostAction, fAction->data());
            }
            ++fCommandID;
        }
    }
    fSettings.endArray();
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupLogging);
    QString fSeverity = QString::number(Logger::getSeverity(), 2);
    STORE_STR(fSettings, fSeverity);

    fSettings.endGroup();

    for (auto& fAction : mActionList)
    {
        delete fAction.second;
    }
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

void MainWindow::keyPressEvent(QKeyEvent *aKey)
{
    if (aKey->key() == Qt::Key_Delete)
    {
        if (ui->treeSource->hasFocus())
        {
            deleteSelectedTreeWidgetItem(*ui->treeSource);
        }
    }
}

quint64 MainWindow::insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem)
{
    QDirIterator fIterator(aParentDir, QDirIterator::NoIteratorFlags);

    std::vector<int> fMapLevels;

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
        QDir fParent = aParentDir;
        while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
        {
            fParent.cdUp();
            addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);
            if (fParent.isRoot()) break;
        };

        fTopLevelItem = true;
    }

    addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();

        auto fFound = mIgnoreMap.find(fFileInfo.fileName().toStdString());
        if (fFound == mIgnoreMap.end())
        {
            for (auto fItem = mIgnoreMap.begin(); fItem != mIgnoreMap.end(); ++fItem)
            {
                if (fItem->second.is(Type::WildCard))
                {
                    QRegExp fRegEx(fItem->first.c_str());
                    fRegEx.setPatternSyntax(QRegExp::Wildcard);
                    if (fRegEx.exactMatch(fFileInfo.fileName()))
                    {
                        fFound = fItem;
                        break;
                    }
                }
            }
        }

        if (fFound != mIgnoreMap.end())
        {
            if (fFound->second.is(Type::File)   && fFileInfo.isFile()) continue;
            if (fFound->second.is(Type::Folder) && fFileInfo.isDir())  continue;
        }

        QStringList fColumns;
        fColumns.append(fFileInfo.fileName());
        fColumns.append(fFileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
        fColumns.append(formatFileSize(fFileInfo.size()));
        fColumns.append("");

        QTreeWidgetItem* fItem = new QTreeWidgetItem(aParentItem, fColumns);
        fItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|fItem->flags());
        fItem->setCheckState(INT(Column::FileName), Qt::Checked);
        fItem->setData(INT(Column::FileName), INT(Role::isDirectory), QVariant(fFileInfo.isDir()));
        fItem->setData(INT(Column::DateTime), INT(Role::DateTime), QVariant(fFileInfo.lastModified()));
        fItem->setData(INT(Column::State), INT(Role::Filter), QVariant(static_cast<uint>(0)));

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

    for (auto fMapLevel : fMapLevels)
    {
        removeIgnoreMapLevel(fMapLevel);
    }

    if (fTopLevelItem)
    {
        aParentItem->setText(INT(Column::Size), formatFileSize(fSizeOfFiles));
    }
    return fSizeOfFiles;
}

bool MainWindow::iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir, QTreeWidgetItem* aParentItem)
{
    bool fResult  = false;
    if (aParentItem)
    {
        if (   aParentItem->checkState(INT(Column::FileName)) == Qt::Checked
            || aParentItem->checkState(INT(Column::FileName)) == Qt::PartiallyChecked)
        {
            const QVariant& fIsDir = aParentItem->data(INT(Column::FileName), INT(Role::isDirectory));
            QString fFileName = aParentItem->text(INT(Column::FileName));
            QString fSource;
            QString fResultStr;
            if (!fFileName.startsWith(QDir::separator()))
            {
                fFileName =  QDir::separator() + fFileName;
            }
            fSource      = *aSourceDir + fFileName;

            if (fIsDir.toBool())
            {
                if (   mCurrentTask == Work::ApplyGitCommand
                    && aParentItem->checkState(INT(Column::FileName)) == Qt::Checked)
                {
                    fResult = applyGitCmd(fSource, mGitCommand, fResultStr);
                    ui->textBrowser->insertPlainText(fResultStr + QChar::LineFeed);
                    return false;
                }
                int fCountOk = 0;
                for (int fChildItem=0; fChildItem < aParentItem->childCount(); ++fChildItem)
                {
                    fResult = iterateTreeItems(aSourceTree, &fSource, aParentItem->child(fChildItem));
                    if (fResult) ++fCountOk;
                }
                switch (mCurrentTask)
                {
                    case Work::InsertPathFromCommandString:
                    {
                        int fIndex = mGitCommand.indexOf(fSource);
                        if (fIndex != -1)
                        {
                            TRACE(Logger::notice, "Found dir: %s: %d", fFileName.toStdString().c_str(), fCountOk);
                            if (fCountOk == 0)
                            {
                                QStringList fChild;
                                fChild.append(mGitCommand.mid(fSource.size() + 1));
                                aParentItem->addChild(new QTreeWidgetItem(fChild));
                            }
                            return 1;
                        }
                        return 0;
                    }
                    break;
                    case Work::ShowAdded: case Work::ShowDeleted: case Work::ShowUnknown:
                    case Work::ShowModified: case Work::ShowAllGitActions:
                    {
                        if (ui->ckHideEmptyParent->isChecked())
                        {
                            fResult = fCountOk != 0;
                            aParentItem->setHidden(!fResult); // true means visible
                        }
                        else
                        {
                            aParentItem->setHidden(false);
                        }
                    }   break;
                    case Work::ShowAllFiles:
                        aParentItem->setHidden(false);
                        fResult = true;
                        break;
                    default: break;
                }
            }
            else
            {
                QVariant fVariant = aParentItem->data(INT(Column::State), INT(Role::Filter));
                if (fVariant.isValid())
                {
                    Type fType(fVariant.toUInt());
                    switch (mCurrentTask)
                    {
                        case Work::ApplyGitCommand:
                            fResult = applyGitCmd(fSource, mGitCommand, fResultStr);
                            ui->textBrowser->insertPlainText(fResultStr + QChar::LineFeed);
                            break;
                        case Work::ShowAllFiles:
                            aParentItem->setHidden(false);
                            fResult = true;
                            break;
                        case Work::ShowAllGitActions:
                            fResult = fType.is(Type::AllGitActions);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowAdded:
                            fResult = fType.is(Type::GitAdded);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowDeleted:
                            fResult = fType.is(Type::GitDeleted);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowModified:
                            fResult = fType.is(Type::GitModified);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowUnknown:
                            fResult = fType.is(Type::GitUnknown);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;

                        default: return false;
                    }
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
            fResult &= iterateTreeItems(aSourceTree, &fSourceDir, aSourceTree.topLevelItem(fIndex));
        }
    }
    return fResult;
}

bool MainWindow::iterateCheckItems(QTreeWidgetItem* aParentItem, stringt2typemap& aPathMap, const QString* aSourceDir)
{
    if (aParentItem)
    {
        QString fSourcePath = aSourceDir ? *aSourceDir + QDir::separator() + aParentItem->text(INT(Column::FileName)) : aParentItem->text(INT(Column::FileName));

        auto fFoundType = aPathMap.find(fSourcePath.toStdString());
        if (fFoundType != aPathMap.end())
        {
            aParentItem->setCheckState(INT(Column::FileName), Qt::Checked);
            const QString fSep = "|";
            QString fState = fSep;

            if (fFoundType->second.is(Type::GitAdded   )) fState += Type::name(Type::GitAdded)    + fSep;
            if (fFoundType->second.is(Type::GitDeleted )) fState += Type::name(Type::GitDeleted)  + fSep;
            if (fFoundType->second.is(Type::GitModified)) fState += Type::name(Type::GitModified) + fSep;
            if (fFoundType->second.is(Type::GitUnknown )) fState += Type::name(Type::GitUnknown)  + fSep;
            if (fFoundType->second.is(Type::GitRenamed )) fState += Type::name(Type::GitRenamed)  + fSep;
            aParentItem->setText(INT(Column::State), fState);
            aParentItem->setData(INT(Column::State), INT(Role::Filter), QVariant(static_cast<uint>(fFoundType->second.mType)));
            TRACE(Logger::info, "set state %s, %x of %s", fState.toStdString().c_str(), fFoundType->second.mType, fSourcePath.toStdString().c_str());
        }
        else
        {
            const QVariant& fIsDir = aParentItem->data(INT(Column::FileName), INT(Role::isDirectory));
            aParentItem->setData(INT(Column::State), INT(Role::Filter), QVariant(static_cast<uint>(fIsDir.toBool() ? Type::Folder : Type::File)));
            aParentItem->setText(INT(Column::State), "");
        }
        for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
        {
            iterateCheckItems(aParentItem->child(fChild), aPathMap, &fSourcePath);
        }
    }
    return false;
}

void MainWindow::addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels)
{
    QFile file(aParentDir.filePath(Folder::GitIgnoreFile));

    if (file.open(QIODevice::ReadOnly))
    {
        int fMapLevel = std::max_element(mIgnoreMap.begin(), mIgnoreMap.end(), [] (stringt2typemap::const_reference fE1, stringt2typemap::const_reference fE2)
        {
            return fE1.second.mLevel < fE2.second.mLevel;
        })->second.mLevel + 1;

        aMapLevels.push_back(fMapLevel);
        do
        {
            QString fLine = file.readLine();
            fLine.remove(QChar::LineFeed);
            int fCommentIndex = fLine.indexOf('#');
            if (fCommentIndex != -1)
            {
                fLine.remove(fCommentIndex,fLine.size()-fCommentIndex);
                fLine = fLine.trimmed();
            }
            Type fType(fLine.contains('/') ? Type::Folder : Type::File, fMapLevel);
            if (fLine.contains('*')) fType.add(Type::WildCard);
            if (fLine.contains('?')) fType.add(Type::WildCard);
            fLine.remove(QChar::CarriageReturn);
            fLine.remove('/');
            if (fLine.size())
            {
                if (mIgnoreMap.count(fLine.toStdString()))
                {
                    auto fMessage = QObject::tr("\"%1\" not inserted from %2").arg(fLine).arg(file.fileName());
                    TRACE(Logger::warning, fMessage.toStdString().c_str() );
                }
                else
                {
                    auto fMessage = QObject::tr("\"%1\" inserted from %2").arg(fLine).arg(file.fileName());
                    TRACE(Logger::debug, fMessage.toStdString().c_str() );
                    mIgnoreMap[fLine.toStdString()] = fType;
                }
            }
        }
        while (!file.atEnd());
    }

}

void MainWindow::removeIgnoreMapLevel(int aMapLevel)
{
    if (aMapLevel)
    {
        for (;;)
        {
            auto fElem = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [aMapLevel](stringt2typemap::const_reference fE)
            {
                 return fE.second.mLevel==aMapLevel;
            });

            if (fElem != mIgnoreMap.end())
            {
                mIgnoreMap.erase(fElem);
            }
            else break;
        }
    }
}

void MainWindow::insertSourceTree(const QDir& fSourceDir, int aItem)
{
    QString fResultString;
    applyGitCmd(fSourceDir.path(), Cmd::getCommand(Cmd::GetStatusAll), fResultString);

    ui->textBrowser->setText(fResultString);
    ui->labelFilePath->setText("");
    stringt2typemap fCheckMap;
    parseGitStatus(fSourceDir.path(), fResultString, fCheckMap);

    insertItem(fSourceDir, *ui->treeSource);

    for (const auto& fItem : fCheckMap)
    {
        if (fItem.second.is(Type::GitDeleted))
        {
            mGitCommand = fItem.first.c_str();
            mCurrentTask = Work::InsertPathFromCommandString;
            QString fFilePath = "";
            iterateTreeItems(*ui->treeSource, &fFilePath, ui->treeSource->topLevelItem(aItem));
            mGitCommand.clear();
            mCurrentTask = Work::None;
        }
    }

    iterateCheckItems(ui->treeSource->topLevelItem(aItem), fCheckMap);
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


void MainWindow::selectSourceFolder()
{
    QString fSourcePath = QFileDialog::getExistingDirectory(this, "Select SourceFiles");
    if (fSourcePath.size() > 1)
    {
        insertSourceTree(initDir(fSourcePath), ui->treeSource->topLevelItemCount()+1);
    }
}

void MainWindow::handleWorker(int aWork)
{
    Logger::printDebug(Logger::trace, "handleWorker(%d): %x", aWork, QThread::currentThreadId());
    mCurrentTask = static_cast<Work>(aWork);
    switch(static_cast<Work>(aWork))
    {
        default:
            iterateTreeItems(*ui->treeSource);
            break;
    }
    mCurrentTask = Work::None;
}

void MainWindow::handleMessage(int aMsg, QVariant aData)
{
    Logger::printDebug(Logger::trace, "handleMessage(%d): %x, %s", aMsg, QThread::currentThreadId(), aData.typeName());
    switch(static_cast<Work>(aMsg))
    {
        default:  break;
    }
}

void MainWindow::parseGitStatus(const QString& fSource, const QString& aStatus, stringt2typemap& aFiles)
{
    auto fLines = aStatus.split(QChar::LineFeed);

    for (QString fLine : fLines)
    {
        fLine = fLine.trimmed();
        auto fParts = fLine.split(QChar::Space);

        if (fParts.size() > 1)
        {
            auto fRelativePath = fParts[fParts.size()-1].trimmed();
            Type fType(Type::translate(fParts[0]));

            if (fType.is(Type::Repository))
            {
                aFiles[fRelativePath.toStdString()] = fType;
            }
            else
            {
                QString fFullPath = fSource + QDir::separator() + fRelativePath;
                QFileInfo fFileInfo(fFullPath);
                if (fFileInfo.isFile()) fType.add(Type::File);
                if (fFileInfo.isDir())  fType.add(Type::Folder);
                aFiles[fFullPath.toStdString()] = fType;
            }

            TRACE(Logger::trace, "%s: %s: %x", fParts[0].toStdString().c_str(), fRelativePath.toStdString().c_str(), fType.mType);
        }
    }
}

void MainWindow::parseGitLogHistoryText()
{
    QVector<QStringList> fList;
    git::History::parse(ui->textBrowser->toPlainText(), fList);
    ui->textBrowser->setPlainText("");

    QString fFileName = mContextMenuItem->text(INT(History::Column::Text));
    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(fFileName));
    ui->treeHistory->addTopLevelItem(fNewHistoryItem);
    QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
    fNewHistoryItem->setData(INT(History::Column::Data), INT(History::Role::ContextMenuItem), QVariant(fSourceHook->indexFromItem(mContextMenuItem)));

    ui->treeHistory->setVisible(true);
    ui->btnHideHistory->setChecked(true);
    int fTLI = ui->treeHistory->topLevelItemCount()-1;
    for (auto fItem: fList)
    {
        if (fItem.count() >= INT(History::Entry::NoOfEntries))
        {
            fNewHistoryItem = new QTreeWidgetItem();
            ui->treeHistory->topLevelItem(fTLI)->addChild(fNewHistoryItem);
            fNewHistoryItem->setText(INT(History::Column::Text), fItem[INT(History::Entry::CommitterDate)]);
            fNewHistoryItem->setText(INT(History::Column::Data), fItem[INT(History::Entry::Author)]);
            for (int fRole=0; fRole < INT(History::Entry::NoOfEntries); ++fRole)
            {
                fNewHistoryItem->setData(1, fRole, QVariant(fItem[fRole]));
            }
        }
    }
}

QString MainWindow::getItemFilePath(QTreeWidgetItem* aTreeItem)
{
    QString fFileName;
    if (aTreeItem)
    {
        fFileName = aTreeItem->text(INT(Column::FileName));
        while (aTreeItem)
        {
            aTreeItem = aTreeItem->parent();
            if (aTreeItem)
            {
                fFileName = aTreeItem->text(INT(Column::FileName)) + QDir::separator() + fFileName;
            }
        }
    }
    return fFileName;
}

// SLOTS

void MainWindow::on_btnCancel_clicked()
{
    mCurrentTask = Work::None;
}

void MainWindow::on_btnAddSourceFolder_clicked()
{
    selectSourceFolder();
}

void MainWindow::on_btnUpdateStatus_clicked()
{
    qint64 fSize = 0;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSize += sizeOfCheckedItems(ui->treeSource->topLevelItem(i));
    }

    ui->statusBar->showMessage("Total selected bytes: " + formatFileSize(fSize));
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
        std::string fString = ui->textBrowser->toPlainText().toStdString();
        file.write(fString.c_str(), fString.size());
        ui->btnStoreText->setEnabled(false);
    }
}

void MainWindow::textBrowserChanged()
{
    ui->btnStoreText->setEnabled(true);
}

void MainWindow::on_btnCloseText_clicked()
{
    if (ui->btnStoreText->isEnabled() && ui->labelFilePath->text().length() > 0)
    {
        QMessageBox fSaveRequest;
        fSaveRequest.setText("The document has been modified.");
        fSaveRequest.setInformativeText("Do you want to save your changes?");
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
    ui->btnStoreText->setEnabled(false);
    ui->labelFilePath->setText("");
}

void MainWindow::on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int /* column */ )
{
    on_btnCloseText_clicked();

    QString fFileName = getItemFilePath(item);

    QFile file(fFileName);
    if (file.open(QIODevice::ReadOnly))
    {
        ui->labelFilePath->setText(fFileName);
        ui->textBrowser->setText(file.readAll());
        ui->btnStoreText->setEnabled(false);
    }
}

void MainWindow::on_treeSource_customContextMenuRequested(const QPoint &pos)
{
    mContextMenuItem = ui->treeSource->itemAt( pos );
    if (mContextMenuItem)
    {
        Type fType(static_cast<Type::eType>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));

        QMenu menu(this);
        menu.addAction(getAction(git::Cmd::Add));
        menu.addAction(getAction(git::Cmd::Remove));
        menu.addAction(getAction(git::Cmd::ShowDifference));
        menu.addAction(getAction(git::Cmd::CallDiffTool));
        menu.addAction(getAction(git::Cmd::ShowShortStatus));
        menu.addAction(getAction(git::Cmd::ShowStatus));
        menu.addAction(getAction(git::Cmd::Commit));
        menu.addAction(getAction(git::Cmd::MoveOrRename));
        menu.addAction(getAction(git::Cmd::Restore));
        menu.addAction(getAction(git::Cmd::History));

        menu.exec( ui->treeSource->mapToGlobal(pos) );
        mContextMenuItem = nullptr;
    }
}

void MainWindow::on_ckHideEmptyParent_clicked(bool )
{
    on_comboShowItems_currentIndexChanged(ui->comboShowItems->currentIndex());
//        Q_EMIT doWork(INT(Work::ShowAll));
//        handleWorker(INT(Work::ShowAll));
}

void MainWindow::on_comboShowItems_currentIndexChanged(int index)
{
    switch (static_cast<ComboShowItems>(index))
    {
        case ComboShowItems::AllFiles:
            handleWorker(INT(Work::ShowAllFiles));
            break;
        case ComboShowItems::AllGitActions:
            handleWorker(INT(Work::ShowAllGitActions));
            break;
        case ComboShowItems::GitModified:
            handleWorker(INT(Work::ShowModified));
            break;
        case ComboShowItems::GitAdded:
            handleWorker(INT(Work::ShowAdded));
            break;
        case ComboShowItems::GitDeleted:
            handleWorker(INT(Work::ShowDeleted));
            break;
        case ComboShowItems::GitUnknown:
            handleWorker(INT(Work::ShowUnknown));
            break;
    }
}

QAction * MainWindow::createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand)
{
    QAction *fNewAction = new QAction(aName, this);
    mActionList[aCmd] = fNewAction;
    fNewAction->setStatusTip(aGitCommand);
    fNewAction->setToolTip(sNoCustomCommandMessageBox);
    return fNewAction;
}

void MainWindow::initContextMenuActions()
{
    // TODO: create the necessary actions, slots and enumerations for recognition
    connect(createAction(git::Cmd::ShowStatus     , tr("Show status")       , Cmd::getCommand(Cmd::ShowStatus))     , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    connect(createAction(git::Cmd::ShowDifference , tr("Show difference")   , Cmd::getCommand(Cmd::ShowDifference)) , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    connect(createAction(git::Cmd::CallDiffTool   , tr("Call diff tool...") , Cmd::getCommand(Cmd::CallDiffTool))   , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    connect(createAction(git::Cmd::ShowShortStatus, tr("Show short status") , Cmd::getCommand(Cmd::ShowShortStatus)), SIGNAL(triggered()), this, SLOT(on_custom_command()));

    connect(createAction(git::Cmd::Add            , tr("Add to git")        , Cmd::getCommand(Cmd::Add))            , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    setCustomCommandPostAction(git::Cmd::Add, git::Cmd::UpdateItemStatus);

    connect(createAction(git::Cmd::History        , tr("Show History")      , Cmd::getCommand(Cmd::History))        , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    setCustomCommandPostAction(git::Cmd::History, git::Cmd::ParseHistoryText);

    connect(createAction(git::Cmd::Remove         , tr("Remove from git..."), Cmd::getCommand(Cmd::Remove))         , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    setCustomCommandMessageBoxText(git::Cmd::Remove, "Remove %1 from git repository;Do you want to remove \"%1\"?");
    setCustomCommandPostAction(git::Cmd::Remove, git::Cmd::UpdateItemStatus);

    connect(createAction(git::Cmd::Restore         , tr("Restore changes..."), Cmd::getCommand(Cmd::Restore))       , SIGNAL(triggered()), this, SLOT(on_custom_command()));
    setCustomCommandMessageBoxText(git::Cmd::Restore, "Restore changes;Do you want to restore changes in file \"%1\"?");
    setCustomCommandPostAction(git::Cmd::Restore, git::Cmd::UpdateItemStatus);

    connect(createAction(git::Cmd::Commit         , tr("Commit...")), SIGNAL(triggered()), this, SLOT(on_git_commit()));
    setCustomCommandPostAction(git::Cmd::Commit, git::Cmd::UpdateItemStatus);

    connect(createAction(git::Cmd::MoveOrRename   , tr("Move / Rename...")), SIGNAL(triggered()), this, SLOT(on_git_move_rename()));

    connect(createAction(git::Cmd::ShowHistoryDifference, tr("Show difference")  , Cmd::getCommand(Cmd::ShowHistoryDifference)), SIGNAL(triggered()), this, SLOT(on_git_history_diff_command()));
    connect(createAction(git::Cmd::CallHistoryDiffTool  , tr("Call diff tool..."), Cmd::getCommand(Cmd::CallHistoryDiffTool))  , SIGNAL(triggered()), this, SLOT(on_git_history_diff_command()));

//    connect(createAction(fID, fName, fCommand), SIGNAL(triggered()), this, SLOT(on_custom_command()));
//    setCustomCommandMessageBoxText(fID, fMessageBoxText);
//    setCustomCommandPostAction(fID, fPostAction);

    //    Commands\1\Command=git rm --cached %1
    //    Commands\1\ID=4
    //    Commands\1\MessageBoxText="Remove %1 from git repository;Do you want to remove \"%1\"?"
    //    Commands\1\Name=Remove from git...
    //    Commands\1\PostAction=1
    //    Commands\size=1

}

QAction* MainWindow::getAction(git::Cmd::eCmd aCmd)
{
    auto fItem = mActionList.find(aCmd);
    if (fItem != mActionList.end())
    {
        return fItem->second;
    }
    return 0;
}

void  MainWindow::setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText)
{
    getAction(aCmd)->setToolTip(aText);
}

void  MainWindow::setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction)
{
    getAction(aCmd)->setData(aAction);
}

void  MainWindow::performGitCmd(const QString& aCommand)
{
    if (mContextMenuItem)
    {
        on_btnCloseText_clicked();
        mGitCommand = aCommand;
        mCurrentTask = Work::ApplyGitCommand;
        QString fFilePath = getItemFilePath(mContextMenuItem->parent());
        iterateTreeItems(*ui->treeSource, &fFilePath, mContextMenuItem);
        mGitCommand.clear();
        mCurrentTask = Work::None;
    }
}

bool MainWindow::applyGitCmd(const QString& fSource, const QString& fGitCmd, QString& aResultStr)
{
    QString fCommand = QObject::tr(fGitCmd.toStdString().c_str()).arg(fSource);
    int fResult = execute(fCommand, aResultStr);
    return fResult == 0;
}

void MainWindow::updateTreeItemStatus(QTreeWidgetItem * aItem)
{
    QFileInfo fFileInfo(getItemFilePath(aItem));

    QDir fParent;
    if (fFileInfo.isDir()) fParent.setPath(fFileInfo.absoluteFilePath());
    else                   fParent.setPath(fFileInfo.absolutePath());

    while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
    {
        fParent.cdUp();
        if (fParent.isRoot()) break;
    };

    if (!fParent.isRoot())
    {
        QString fRepositoryPath = fParent.absolutePath();
        QString fResultString;
        QString fCommandString = tr(Cmd::getCommand(Cmd::GetStatusAll).toStdString().c_str()).arg(fRepositoryPath);
        fCommandString.append(" ");
        fCommandString.append(fFileInfo.absoluteFilePath());

        applyGitCmd(fRepositoryPath, fCommandString, fResultString);

        stringt2typemap fCheckMap;
        parseGitStatus(fRepositoryPath, fResultString, fCheckMap);

        QString fSourcePath = fFileInfo.absolutePath();
        iterateCheckItems(aItem, fCheckMap, &fSourcePath);
    }
}

void MainWindow::on_btnHideHistory_clicked(bool checked)
{
    ui->treeHistory->setVisible(checked);
}

void MainWindow::on_treeHistory_itemClicked(QTreeWidgetItem *aItem, int /* column */)
{
    QString fText;
    for (int fRole=INT(History::Entry::Subject); fRole < INT(History::Entry::NoOfEntries); ++fRole)
    {
        fText.append(History::name(static_cast<History::Entry>(fRole)));
        fText.append(QChar::LineFeed);
        fText.append(aItem->data(INT(History::Column::Data), fRole).toString());
        fText.append(QChar::LineFeed);
    }
    ui->textBrowser->setPlainText(fText);
}

void MainWindow::on_treeHistory_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* fSelectedHistoryItem = ui->treeHistory->itemAt(pos);
    if (fSelectedHistoryItem)
    {
        QModelIndexList fSelectedHistoryIndexes = ui->treeHistory->selectionModel()->selectedIndexes();
        QTreeWidgetItem* fParentHistoryItem = fSelectedHistoryItem->parent();
        if (fParentHistoryItem)
        {
            QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
            mContextMenuItem = fSourceHook->itemFromIndex(fParentHistoryItem->data(INT(History::Column::Data), INT(History::Role::ContextMenuItem)).toModelIndex());
            if (mContextMenuItem)
            {
                mHistoryHashItems.clear();
                for (auto fIndex : fSelectedHistoryIndexes)
                {
                    QTreeWidgetHook* fHistoryHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeHistory);
                    QTreeWidgetItem* fItem = fHistoryHook->itemFromIndex(fIndex);
                    if (fItem && fItem->parent() == fParentHistoryItem)
                    {
                        mHistoryHashItems.append(fItem->data(INT(History::Column::Data), INT(History::Entry::CommitHash)).toString());
                        mHistoryHashItems.append(" ");
                    }
                }

                QMenu menu(this);
                menu.addAction(getAction(git::Cmd::CallHistoryDiffTool));
                menu.addAction(getAction(git::Cmd::ShowHistoryDifference));
                menu.exec( ui->treeHistory->mapToGlobal(pos) );
                mContextMenuItem = nullptr;
            }
        }
    }
}

void MainWindow::on_btnClearHistory_clicked()
{
    ui->treeHistory->clear();
}

void  MainWindow::on_git_commit()
{
    CommitMessage fCommitMsg;

    if (fCommitMsg.exec() == QDialog::Accepted)
    {
        QString fMessageText = fCommitMsg.getMessageText();
        std::string  fCommand  = Cmd::getCommand(Cmd::Commit).toStdString();
        QString fCommitCommand = tr(fCommand.c_str()).arg(fMessageText).arg("%1");
        if (fCommitMsg.getAutoStage())
        {
            performGitCmd(Cmd::getCommand(Cmd::Add));
        }
        performGitCmd(fCommitCommand);
        updateTreeItemStatus(mContextMenuItem);
    }
}

void MainWindow::on_git_move_rename()
{
    bool    fOk;
    Type    fType(static_cast<Type::eType>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));
    QString fFileTypeName = Type::name(static_cast<Type::eType>(Type::FileType&fType.mType));
    QString fOldName      = mContextMenuItem->text(INT(Column::FileName));
    QString fNewName      = QInputDialog::getText(this,
                   tr("Move or rename %1").arg(fFileTypeName),
                   tr("Enter a new name or destination for \"%1\".").arg(fOldName),
                   QLineEdit::Normal, "", &fOk);

    if (fOk && !fNewName.isEmpty())
    {
        QFileInfo   fPath(getItemFilePath(mContextMenuItem));
        std::string fFormatCmd = Cmd::getCommand(Cmd::MoveOrRename).toStdString().c_str();
        QString     fCommand   = tr(fFormatCmd.c_str()).arg(fPath.absolutePath()).arg(fOldName).arg(fNewName);
        QString fResultStr;
        int fResult = execute(fCommand, fResultStr);
        if (fResult == 0)
        {
            mContextMenuItem->setText(INT(Column::FileName), fNewName);
            updateTreeItemStatus(mContextMenuItem);
        }
    }
}

void MainWindow::on_custom_command()
{
    QAction *fAction = qobject_cast<QAction *>(sender());
    QString fMessageBoxText = fAction->toolTip();

    if (fMessageBoxText != sNoCustomCommandMessageBox)
    {
        QStringList fTextList = fMessageBoxText.split(";");
        QMessageBox fSaveRequest;
        Type fType(static_cast<Type::eType>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));
        QString fFileTypeName = Type::name(static_cast<Type::eType>(Type::FileType&fType.mType));
        QString fFileName     = mContextMenuItem->text(INT(Column::FileName));
        std::string fText1   = fTextList[0].toStdString().c_str();
        switch (fTextList.size())
        {
            case 1:
                fSaveRequest.setText(tr(fText1.c_str()).arg(fFileTypeName));
                fSaveRequest.setInformativeText(fFileName);
                break;
            case 2:
                fSaveRequest.setText(tr(fText1.c_str()).arg(fFileTypeName));
                fSaveRequest.setInformativeText(tr(fTextList[1].toStdString().c_str()).arg(fFileName));
                break;
        }

        fSaveRequest.setStandardButtons(fType.is(Type::File) ? QMessageBox::Yes | QMessageBox::No : QMessageBox::YesToAll | QMessageBox::NoToAll);
        fSaveRequest.setDefaultButton(  fType.is(Type::File) ? QMessageBox::Yes : QMessageBox::YesToAll);

        auto fResult = fSaveRequest.exec();
        if (fResult == QMessageBox::Yes || fResult == QMessageBox::YesToAll)
        {
            performGitCmd(fAction->statusTip());
        }
    }
    else
    {
        performGitCmd(fAction->statusTip());
    }

    if (fAction->data().isValid())
    {
        switch (fAction->data().toUInt())
        {
            case git::Cmd::UpdateItemStatus:
                updateTreeItemStatus(mContextMenuItem);
                break;
            case git::Cmd::ParseHistoryText:
                parseGitLogHistoryText();
                break;
        }
    }
}

void MainWindow::on_git_history_diff_command()
{
    QAction *fAction = qobject_cast<QAction *>(sender());
    performGitCmd(tr(fAction->statusTip().toStdString().c_str()).arg(mHistoryHashItems).arg("%1"));
}

