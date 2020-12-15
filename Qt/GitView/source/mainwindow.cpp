#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "commitmessage.h"
#include "history.h"
#include "qbranchtreewidget.h"
#include "customgitactions.h"
#include "aboutdlg.h"

#include <QDateTime>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QKeyEvent>
#include <QSysInfo>
#include <QMenu>
#include <QToolBar>

#include <boost/bind.hpp>

#define RELATIVE_GIT_PATH 1

using namespace std;
using namespace git;



// Kapitel 1.4.12.2 Entfernte Referenzen anpassen
// TODO:  Entferntes Repository hinzufügen:
// git remote add < Name > <URL >
// Referenz entfernen:
// URL korrigieren:
// git remote set - url < Name > <URL >

// TODO: merge command implementieren
// git merge --abort
// git merge --continue
// git mergetool
// git pull
// git merge -s <resolve|recursive<ours|theirs|patience>> obsolete
// git log --merge -p <path>

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
const QString sFlags("Flags");
const QString sFlagsEnabled("FlagsEnabled");
const QString sFlagsDisabled("FlagsDisabled");
const QString sIconPath("IconPath");
const QString sShortcut("Shortcut");
const QString sModified("Modified");
} // namespace config




MainWindow::MainWindow(const QString& aConfigName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mWorker(this)
    , mCurrentTask(Work::None)
    , mActions(this)
    , mConfigFileName(aConfigName)
    , mContextMenuSourceTreeItem(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resource/logo@2x.png"));

    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleWorker, this, _1));
    QObject::connect(this, SIGNAL(doWork(int)), &mWorker, SLOT(doWork(int)));
    mWorker.setMessageFunction(boost::bind(&MainWindow::handleMessage, this, _1, _2));
    connect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(textBrowserChanged()));

    ui->treeSource->header()->setSortIndicator(Column::FileName, Qt::AscendingOrder);
    ui->treeSource->header()->setSectionResizeMode(Column::FileName, QHeaderView::Stretch);
    ui->treeSource->header()->setSectionResizeMode(Column::DateTime, QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(Column::Size    , QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(Column::State   , QHeaderView::Interactive);
    ui->treeSource->header()->setStretchLastSection(false);

    ui->treeSource->setContextMenuPolicy(Qt::CustomContextMenu);


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
    LOAD_STR(fSettings, Type::mShort, toBool);
    ui->ckShortState->setChecked(Type::mShort);
    fSettings.endGroup();


    fSettings.beginGroup(config::sGroupLogging);
    QString fSeverity;
    LOAD_STR(fSettings, fSeverity, toString);
    std::uint32_t fSeverityValue = fSeverity.toLong(0, 2);
    Logger::setSeverity(0xffff, false);
    Logger::setSeverity(fSeverityValue, true);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);

    // NOTE: Remote Repositories nötig
    QString fCommand = "git remote -v";
    QString fResultStr;
    execute(fCommand, fResultStr);
    apendTextToBrowser(fCommand + getLineFeed() + fResultStr + getLineFeed());

    int fItemCount = fSettings.beginReadArray(config::sSourcePath);
    for (int fItem = 0; fItem < fItemCount; ++fItem)
    {
        fSettings.setArrayIndex(fItem);
        const QDir fSourceDir = initDir(fSettings.value(config::sSourcePath).toString(), fSettings.value(config::sSourcePath+"/"+config::sGroupFilter).toInt());
        insertSourceTree(fSourceDir, fItem);
    }

    ui->labelFilePath->setText("");

    fSettings.endArray();

    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupGitCommands);

    LOAD_STRF(fSettings, Cmd::mContextMenuSourceTree, Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mContextMenuEmptySourceTree, Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mContextMenuHistoryTree, Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mContextMenuBranchTree, Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mToolbars[0], Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mToolbars[1], Cmd::fromString, Cmd::toString, toString);

    initContextMenuActions();
    mActions.initActionIcons();

    fItemCount = fSettings.beginReadArray(config::sCommands);
    for (int fItem = 0; fItem < fItemCount; ++fItem)
    {
        fSettings.setArrayIndex(fItem);
        Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(fSettings.value(config::sID).toUInt());
        auto* fAction = mActions.getAction(fCmd);
        if (!fAction)
        {
            fAction = mActions.createAction(fCmd, "new", "git");
        }
        fAction->setText(fSettings.value(config::sName).toString());
        fAction->setToolTip(fSettings.value(config::sName).toString());
        fAction->setStatusTip(fSettings.value(config::sCommand).toString());
        fAction->setShortcut(QKeySequence(fSettings.value(config::sShortcut).toString()));
        uint fFlags = fSettings.value(config::sFlags).toUInt();
        if (fFlags & ActionList::Custom)
        {
            connect(fAction, SIGNAL(triggered()), this, SLOT(perform_custom_command()));
        }
        mActions.setFlags(fCmd, fFlags, Flag::replace);
        mActions.setFlags(fCmd, fSettings.value(config::sFlagsEnabled).toUInt(),  Flag::replace, ActionList::Data::StatusFlagEnable);
        mActions.setFlags(fCmd, fSettings.value(config::sFlagsDisabled).toUInt(), Flag::replace, ActionList::Data::StatusFlagDisable);
        mActions.setIconPath(fCmd, fSettings.value(config::sIconPath).toString());
        mActions.setCustomCommandMessageBoxText(fCmd, fSettings.value(config::sCustomMessageBoxText).toString());
        mActions.setCustomCommandPostAction(fCmd, fSettings.value(config::sCustomCommandPostAction).toUInt());
    }
    fSettings.endArray();
    fSettings.endGroup();


    for (const auto& fToolbar : Cmd::mToolbars)
    {
        QToolBar*pTB = new QToolBar();
        mActions.fillToolbar(*pTB, fToolbar);
        ui->horizontalLayoutTool->addWidget(pTB);
    }

    TRACE(Logger::info, "%s Started", windowTitle().toStdString().c_str());
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
    STORE_STR(fSettings,  Type::mShort);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);
    fSettings.beginWriteArray(config::sSourcePath);

    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSettings.setArrayIndex(i);
        QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
        fSettings.setValue(config::sSourcePath, fItem->text(Column::FileName));
    }

    fSettings.endArray();
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupGitCommands);

    STORE_STRF(fSettings, Cmd::mContextMenuSourceTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mContextMenuEmptySourceTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mContextMenuHistoryTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mContextMenuBranchTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mToolbars[0], Cmd::toString);
    STORE_STRF(fSettings, Cmd::mToolbars[1], Cmd::toString);

    fSettings.beginWriteArray(config::sCommands);
    {
        int fIndex = 0;

        for (const auto& fItem : mActions.getList())
        {
            Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(fItem.first);

            if (mActions.getFlags(fCmd) & ActionList::Modified)
            {
                const QAction* fAction = fItem.second;
                QString fCommand = fAction->statusTip();
                if (fCommand.size())
                {
                    fSettings.setArrayIndex(fIndex++);
                    fSettings.setValue(config::sID, fItem.first);
                    fSettings.setValue(config::sName, fAction->toolTip());
                    fSettings.setValue(config::sCommand, fCommand);
                    fSettings.setValue(config::sShortcut, fAction->shortcut().toString());
                    fSettings.setValue(config::sCustomMessageBoxText, mActions.getCustomCommandMessageBoxText(fCmd));
                    fSettings.setValue(config::sCustomCommandPostAction, mActions.getCustomCommandPostAction(fCmd));
                    fSettings.setValue(config::sFlags, mActions.getFlags(fCmd));
                    fSettings.setValue(config::sFlagsEnabled, mActions.getFlags(fCmd, ActionList::Data::StatusFlagEnable));
                    fSettings.setValue(config::sFlagsDisabled, mActions.getFlags(fCmd, ActionList::Data::StatusFlagDisable));
                    fSettings.setValue(config::sIconPath, mActions.getIconPath(fCmd));
                }
            }
        }
    }

    fSettings.endArray();
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupLogging);
    QString fSeverHlp = "_fsc____acewnidt";
    STORE_STR(fSettings, fSeverHlp);
    QString fSeverity = QString::number(Logger::getSeverity(), 2);
    STORE_STR(fSettings, fSeverity);

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

void MainWindow::keyPressEvent(QKeyEvent *aKey)
{
    if (aKey->key() == Qt::Key_Delete)
    {
        if (ui->treeSource->hasFocus())
        {
            deleteTopLevelItemOfSelectedTreeWidgetItem(*ui->treeSource);
            mContextMenuSourceTreeItem = nullptr;
        }
        if (ui->treeBranches->hasFocus())
        {
            deleteTopLevelItemOfSelectedTreeWidgetItem(*ui->treeBranches);
        }
        if (ui->treeHistory->hasFocus())
        {
            deleteTopLevelItemOfSelectedTreeWidgetItem(*ui->treeHistory);
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
        aParentItem->setCheckState(Column::FileName, Qt::Checked);
        aParentItem->setData(Column::FileName, Role::isDirectory, QVariant(true));
        aParentItem->setData(Column::FileName, Role::Filter, QVariant(INT(aParentDir.filter())));

        aTree.addTopLevelItem(aParentItem);
        QDir fParent = aParentDir;
        while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
        {
            fParent.cdUp();
            mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);
            if (fParent.isRoot()) break;
        };
#if RELATIVE_GIT_PATH ==1
        if (!fParent.isRoot())
        {
            aParentItem->setData(Column::FileName, Role::GitFolder, QVariant(fParent.absolutePath()));
        }
#endif
        fTopLevelItem = true;
    }

    mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();

        if (mGitIgnore.ignoreFile(fFileInfo)) continue;

        QStringList fColumns;
        fColumns.append(fFileInfo.fileName());
        fColumns.append(fFileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
        fColumns.append(formatFileSize(fFileInfo.size()));
        fColumns.append("");

        QTreeWidgetItem* fItem = new QTreeWidgetItem(aParentItem, fColumns);
        fItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|fItem->flags());
        fItem->setCheckState(Column::FileName, Qt::Checked);
        fItem->setData(Column::FileName, Role::isDirectory, QVariant(fFileInfo.isDir()));
        fItem->setData(Column::DateTime, Role::DateTime, QVariant(fFileInfo.lastModified()));

        Type fType;
        fType.translate(fFileInfo);

        fItem->setData(Column::State, Role::Filter, QVariant(fType.mType));

        if (fFileInfo.isDir())
        {
            QDir fSubDir(fFileInfo.absoluteFilePath());
            fSubDir.setFilter(aParentDir.filter());
            quint64 fSizeOfSubFolderFiles = insertItem(fSubDir, aTree, fItem);
            fItem->setText(Column::Size, formatFileSize(fSizeOfSubFolderFiles));
            fSizeOfFiles += fSizeOfSubFolderFiles;
            fItem->setData(Column::Size, Qt::SizeHintRole, QVariant(fSizeOfSubFolderFiles));
        }
        else
        {
            fSizeOfFiles += fFileInfo.size();
            fItem->setText(Column::Size, formatFileSize(fFileInfo.size()));
            fItem->setData(Column::Size, Qt::SizeHintRole, QVariant(fFileInfo.size()));
        }
        //mIgnoreContainingNegation.reset();
    }
    while (fIterator.hasNext());


    for (const auto& fMapLevel : fMapLevels)
    {
        mGitIgnore.removeIgnoreMapLevel(fMapLevel);
    }

    if (fTopLevelItem)
    {
        aParentItem->setText(Column::Size, formatFileSize(fSizeOfFiles));
    }
    return fSizeOfFiles;
}

bool MainWindow::iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir, QTreeWidgetItem* aParentItem)
{
    bool fResult  = false;
    if (aParentItem)
    {
        if (   aParentItem->checkState(Column::FileName) == Qt::Checked
            || aParentItem->checkState(Column::FileName) == Qt::PartiallyChecked)
        {
            const QVariant& fIsDir = aParentItem->data(Column::FileName, Role::isDirectory);
            QString fFileName = aParentItem->text(Column::FileName);
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
                    && aParentItem->checkState(Column::FileName) == Qt::Checked)
                {
                    QString fCmd = applyGitCommandToFilePath(fSource, mGitCommand, fResultStr);
                    apendTextToBrowser(fCmd + getLineFeed() + fResultStr + getLineFeed());
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
                    case Work::ShowAdded: case Work::ShowDeleted: case Work::ShowUnknown: case Work::ShowStaged: case Work::ShowUnMerged:
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
                QVariant fVariant = aParentItem->data(Column::State, Role::Filter);
                if (fVariant.isValid())
                {
                    Type fType(fVariant.toUInt());
                    switch (mCurrentTask)
                    {
                        case Work::ApplyGitCommand:
                        {
                            QString fCmd = applyGitCommandToFilePath(fSource, mGitCommand, fResultStr);
                            apendTextToBrowser(fCmd + getLineFeed() + fResultStr);
                            fResult = fCmd.size() != 0;
                        }   break;
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
                            fResult = fType.is(Type::GitUnTracked);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowUnMerged:
                            fResult = fType.is(Type::GitUnmerged);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;
                        case Work::ShowStaged:
                            fResult = fType.is(Type::GitStaged);
                            aParentItem->setHidden(!fResult); // true means visible
                            break;

                        default: return false;
                    }
                }
            }
        }
        else
        {
            TRACE(Logger::info, "Not copying unselected file %s", aParentItem->text(Column::FileName).toStdString().c_str());
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
//        QString fSourcePath = aSourceDir ? *aSourceDir + QDir::separator() + aParentItem->text(Column::FileName) : aParentItem->text(Column::FileName);
        QString fSourcePath = aSourceDir ? *aSourceDir + "/" + aParentItem->text(Column::FileName) : aParentItem->text(Column::FileName);

        auto fFoundType = aPathMap.find(fSourcePath.toStdString());
        if (fFoundType != aPathMap.end())
        {
            aParentItem->setCheckState(Column::FileName, Qt::Checked);
            QString fState = fFoundType->second.getStates();
            aParentItem->setText(Column::State, fState);
            Type fType(aParentItem->data(Column::State, Role::Filter).toUInt());
            fType.add(static_cast<Type::TypeFlags>(fFoundType->second.mType));
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.mType));
            TRACE(Logger::info, "set state %s, %x of %s", fState.toStdString().c_str(), fFoundType->second.mType, fSourcePath.toStdString().c_str());
        }
        else
        {
            Type fType(aParentItem->data(Column::State, Role::Filter).toUInt());
            fType.remove(Type::AllGitActions);
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.mType));
            aParentItem->setText(Column::State, "");
        }
        for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
        {
            iterateCheckItems(aParentItem->child(fChild), aPathMap, &fSourcePath);
        }
    }
    return false;
}

void MainWindow::insertSourceTree(const QDir& fSourceDir, int aItem)
{
    QString fResultString;
    applyGitCommandToFilePath(fSourceDir.path(), Cmd::getCommand(Cmd::GetStatusAll), fResultString);

    apendTextToBrowser(fResultString);
    stringt2typemap fCheckMap;
    parseGitStatus(fSourceDir.path() +  QDir::separator(), fResultString, fCheckMap);

    insertItem(fSourceDir, *ui->treeSource);

    // TODO: bug: are all deleted items shown?
    for (const auto& fItem : fCheckMap)
    {
        if (fItem.second.is(Type::GitDeleted) || fItem.second.is(Type::GitMovedFrom))
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
        switch (aParentItem->checkState(Column::FileName))
        {
            case Qt::Unchecked: return fSize;
            case Qt::PartiallyChecked:
                fIterate = true;
                break;
            case  Qt::Checked:
                if (!aParentItem->data(Column::FileName, Role::isDirectory).toBool())
                {
                    fSize = aParentItem->data(Column::Size, Qt::SizeHintRole).toLongLong();
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
        if (ui->ckFiles->isChecked())          fFilter |= QDir::Files;
        if (ui->ckDirectories->isChecked())    fFilter |= QDir::Dirs;
        if (ui->ckHiddenFiles->isChecked())    fFilter |= QDir::Hidden;
        if (ui->ckSystemFiles->isChecked())    fFilter |= QDir::System;
        if (!ui->ckSymbolicLinks->isChecked()) fFilter |= QDir::NoSymLinks;
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

void MainWindow::apendTextToBrowser(const QString& aText)
{
    on_btnCloseText_clicked();
    ui->textBrowser->insertPlainText(aText + getLineFeed());
    ui->textBrowser->textCursor().setPosition(QTextCursor::End);
}



void MainWindow::handleWorker(int aWork)
{
    Logger::printDebug(Logger::trace, "handleWorker(%d): %x", aWork, QThread::currentThreadId());
    mCurrentTask = static_cast<Work::e>(aWork);
    switch(static_cast<Work::e>(aWork))
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
    switch(static_cast<Work::e>(aMsg))
    {
        default:  break;
    }
}

void MainWindow::parseGitStatus(const QString& fSource, const QString& aStatus, stringt2typemap& aFiles)
{
    auto fLines = aStatus.split("\n");

    for (QString fLine : fLines)
    {
        const int fStateSize = 2;
        QString fState = fLine.left(fStateSize);
        auto fRelativePath = fLine.mid(fStateSize).trimmed();

        if (fRelativePath.size())
        {
            Type fType;
            fType.translate(fState);

            if (fType.is(Type::Repository))
            {
                aFiles[fRelativePath.toStdString()] = fType;
            }
            else
            {
                QString fFullPath = fSource + fRelativePath;
                if (fType.is(Type::GitRenamed) && fRelativePath.contains("->"))
                {
                    auto fPaths = fRelativePath.split(" -> ");
                    if (fPaths.size() > 1)
                    {
                        fFullPath = fSource + fPaths[1];
                        QFileInfo fFileInfo(fFullPath);
                        fType.translate(fFileInfo);
                        Type fDestinationType = fType;
                        fDestinationType.add(Type::GitMovedTo);
                        aFiles[fFullPath.toStdString()] = fDestinationType;
                        fType.add(Type::GitMovedFrom);
                        fFullPath = fSource + fPaths[0];
                    }
                }
                QFileInfo fFileInfo(fFullPath);
                fType.translate(fFileInfo);
                aFiles[fFileInfo.filePath().toStdString()] = fType;
            }

            TRACE(Logger::trace, "%s: %s: %x", fState.toStdString().c_str(), fRelativePath.toStdString().c_str(), fType.mType);
        }
    }
}


QString MainWindow::getItemFilePath(QTreeWidgetItem* aTreeItem)
{
    QString fFileName;
    if (aTreeItem)
    {
        fFileName = aTreeItem->text(Column::FileName);
        while (aTreeItem)
        {
            aTreeItem = aTreeItem->parent();
            if (aTreeItem)
            {
                if (aTreeItem->data(Column::FileName, Role::GitFolder).isValid())
                {
                    QDir::setCurrent(aTreeItem->text(Column::FileName));
                }
                else
                {
                    fFileName = aTreeItem->text(Column::FileName) + QDir::separator() + fFileName;
                }
            }
        }
    }
    return fFileName;
}

QString MainWindow::getItemTopDirPath(QTreeWidgetItem* aItem)
{
    aItem = getTopLevelItem(*ui->treeSource, aItem);
    if (aItem)
    {
        return aItem->text(Column::FileName);
    }
    else if (ui->treeSource->topLevelItemCount())
    {
        return ui->treeSource->topLevelItem(0)->text(Column::FileName);
    }
    return "";
}

void MainWindow::cancelCurrentWorkTask()
{
    mCurrentTask = Work::None;
}

void MainWindow::addGitSourceFolder()
{
    selectSourceFolder();
}

void MainWindow::updateGitStatus()
{
    std::vector<QString> fSourceDirs;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        fSourceDirs.push_back(ui->treeSource->topLevelItem(i)->text(Column::FileName));
    }
    ui->treeSource->clear();

    qint64 fSize = 0;
    for (uint i = 0; i < fSourceDirs.size(); ++i)
    {
        insertSourceTree(initDir(fSourceDirs[i]), i);
        fSize += sizeOfCheckedItems(ui->treeSource->topLevelItem(i));
    }

    ui->statusBar->showMessage("Total selected bytes: " + formatFileSize(fSize));
    on_comboShowItems_currentIndexChanged(ui->comboShowItems->currentIndex());
}

// SLOTS

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
    mContextMenuSourceTreeItem = ui->treeSource->itemAt( pos );
    if (mContextMenuSourceTreeItem)
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuSourceTree);
        menu.exec( ui->treeSource->mapToGlobal(pos) );
        mContextMenuSourceTreeItem = nullptr;
    }
    else
    {
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuEmptySourceTree);
        menu.exec( ui->treeSource->mapToGlobal(pos) );
    }
}

void MainWindow::on_ckHideEmptyParent_clicked(bool )
{
    on_comboShowItems_currentIndexChanged(ui->comboShowItems->currentIndex());
}

void MainWindow::on_ckShortState_clicked(bool checked)
{
    Type::mShort = checked;
    for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
        for (int j=0; j<fItem->childCount(); ++j)
        {
            updateTreeItemStatus(fItem->child(j));
        }
    }
}

void MainWindow::on_comboShowItems_currentIndexChanged(int index)
{
    switch (static_cast<ComboShowItems>(index))
    {
        case ComboShowItems::AllFiles:
            handleWorker(Work::ShowAllFiles);
            break;
        case ComboShowItems::AllGitActions:
            handleWorker(Work::ShowAllGitActions);
            break;
        case ComboShowItems::GitModified:
            handleWorker(Work::ShowModified);
            break;
        case ComboShowItems::GitAdded:
            handleWorker(Work::ShowAdded);
            break;
        case ComboShowItems::GitDeleted:
            handleWorker(Work::ShowDeleted);
            break;
        case ComboShowItems::GitUnknown:
            handleWorker(Work::ShowUnknown);
            break;
        case ComboShowItems::Gitstaged:
            handleWorker(Work::ShowStaged);
            break;
        case ComboShowItems::GitUnmerged:
            handleWorker(Work::ShowUnMerged);
            break;
    }
}

void MainWindow::on_treeSource_itemClicked(QTreeWidgetItem *item, int /* column */ )
{
    mContextMenuSourceTreeItem = item;
    Type fType(static_cast<Type::TypeFlags>(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt()));
    mActions.enableItemsByType(Cmd::mContextMenuSourceTree, fType);
}

void MainWindow::getSelectedTreeItem()
{
    if (! mContextMenuSourceTreeItem)
    {
        auto fSelected = ui->treeSource->selectedItems();
        if (fSelected.size())
        {
            mContextMenuSourceTreeItem = fSelected.at(0);
        }
    }
}

void  MainWindow::applyGitCommandToFileTree(const QString& aCommand)
{
    if (mContextMenuSourceTreeItem)
    {
        on_btnCloseText_clicked();
        mGitCommand = aCommand;
        mCurrentTask = Work::ApplyGitCommand;
        QString fFilePath = getItemFilePath(mContextMenuSourceTreeItem->parent());
        iterateTreeItems(*ui->treeSource, &fFilePath, mContextMenuSourceTreeItem);
        mGitCommand.clear();
        mCurrentTask = Work::None;
    }
}

//"%12"
QString MainWindow::applyGitCommandToFilePath(const QString& fSource, const QString& fGitCmd, QString& aResultStr)
{
    QString fCommand;
    auto fPos = fGitCmd.indexOf(QRegExp("%[0-9]+"));
    if (fPos != -1)
    {
        fCommand = QObject::tr(fGitCmd.toStdString().c_str()).arg(fSource);
    }
    else
    {
        fCommand = fGitCmd;
    }
    int fResult = execute(fCommand, aResultStr);
    return fResult == 0 ? fCommand : "";
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
        if (!fParent.exists()) break;
    };

    if (!fParent.isRoot())
    {
        QString fRepositoryPath = fParent.absolutePath();
        QString fResultString;
        QString fCommandString = tr(Cmd::getCommand(Cmd::GetStatusAll).toStdString().c_str()).arg(fRepositoryPath);
        fCommandString.append(" ");
        fCommandString.append(fFileInfo.absoluteFilePath());

        applyGitCommandToFilePath(fRepositoryPath, fCommandString, fResultString);

        stringt2typemap fCheckMap;

        parseGitStatus(fRepositoryPath + QDir::separator(), fResultString, fCheckMap);

        QString fSourcePath = fFileInfo.absolutePath();
        iterateCheckItems(aItem, fCheckMap, &fSourcePath);
    }
}

void MainWindow::showOrHideTrees(bool checked)
{
    if (checked)
    {
        int fTrees = 0;
        if (ui->treeHistory->topLevelItemCount())
        {
            ui->treeHistory->setVisible(checked);
            ++fTrees;
        }
        if (ui->treeBranches->topLevelItemCount())
        {
            ui->treeBranches->setVisible(checked);
            ++fTrees;
        }
        checked = (fTrees != 0);
    }
    else
    {
        QTreeWidget* fFocused = focusedTreeWidget(false);
        if (fFocused)
        {
            fFocused->setVisible(checked);
        }
        else
        {
            ui->treeHistory->setVisible(checked);
            ui->treeBranches->setVisible(checked);
        }
    }
    ui->horizontalLayout->setStretch(0, 1);
    ui->horizontalLayout->setStretch(1, checked ? 2 : 1);
    mActions.getAction(Cmd::ShowHideTree)->setChecked(checked);
}

void MainWindow::on_treeHistory_itemClicked(QTreeWidgetItem *aItem, int aColumn)
{
    on_btnCloseText_clicked();
    ui->textBrowser->setPlainText(ui->treeHistory->itemClicked(aItem, aColumn));
}

void MainWindow::initContextMenuActions()
{
    connect(mActions.createAction(Cmd::ShowDifference , tr("Show difference")   , Cmd::getCommand(Cmd::ShowDifference)) , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::ShowDifference)->setShortcut(QKeySequence(Qt::Key_F8));
    mActions.setStagedCmdAddOn(Cmd::ShowDifference, "--cached %1");
    mActions.setFlags(Cmd::ShowDifference, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CallDiffTool   , tr("Call diff tool...") , Cmd::getCommand(Cmd::CallDiffTool))   , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallDiffTool)->setShortcut(QKeySequence(Qt::Key_F9));
    mActions.setStagedCmdAddOn(Cmd::CallDiffTool, "--cached %1");
    mActions.setFlags(Cmd::CallDiffTool, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CallMergeTool   , tr("Call merge tool...") , Cmd::getCommand(Cmd::CallMergeTool)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallMergeTool)->setShortcut(QKeySequence(Qt::Key_F7));
    mActions.setFlags(Cmd::CallMergeTool, Type::GitUnmerged, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ShowStatus     , tr("Show status")       , Cmd::getCommand(Cmd::ShowStatus))     , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowStatus, Cmd::UpdateItemStatus);
    connect(mActions.createAction(Cmd::ShowShortStatus, tr("Show short status") , Cmd::getCommand(Cmd::ShowShortStatus)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowShortStatus, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::Add            , tr("Add to git (stage)"), Cmd::getCommand(Cmd::Add))            , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Add, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Add)->setShortcut(QKeySequence(Qt::Key_F4));
    mActions.setFlags(Cmd::Add, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Add, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::Add, Type::GitUnTracked, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Unstage        , tr("Reset file (unstage)"), Cmd::getCommand(Cmd::Unstage))      , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Unstage, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Unstage)->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F4));
    mActions.setFlags(Cmd::Unstage, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::History        , tr("Show History")      , Cmd::getCommand(Cmd::History))        , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::History, Cmd::ParseHistoryText);
    mActions.getAction(Cmd::History)->setShortcut(QKeySequence(Qt::Key_F10));

    connect(mActions.createAction(Cmd::Remove         , tr("Remove from git..."), Cmd::getCommand(Cmd::Remove))         , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Remove, "Remove %1 from git repository;Do you want to remove \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::Remove, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::Restore         , tr("Restore changes..."), Cmd::getCommand(Cmd::Restore))       , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Restore, "Restore changes;Do you want to restore changes in file \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::Restore, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Restore)->setShortcut(QKeySequence(Qt::Key_F6));
    mActions.setFlags(Cmd::Restore, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::Restore, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagDisable);

    connect(mActions.createAction(Cmd::Commit         , tr("Commit..."), Cmd::getCommand(Cmd::Commit)), SIGNAL(triggered()), this, SLOT(call_git_commit()));
    mActions.setCustomCommandPostAction(Cmd::Commit, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::Push           , tr("Push"), Cmd::getCommand(Cmd::Push)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    connect(mActions.createAction(Cmd::Pull           , tr("Pull"), Cmd::getCommand(Cmd::Pull)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    connect(mActions.createAction(Cmd::Show           , tr("Show"), Cmd::getCommand(Cmd::Show)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));

    connect(mActions.createAction(Cmd::BranchList     , tr("List Branches"), Cmd::getCommand(Cmd::BranchList)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchList, Cmd::ParseBranchListText);
    connect(mActions.createAction(Cmd::BranchListRemote, tr("List remote Branches"), Cmd::getCommand(Cmd::BranchListRemote)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListRemote, Cmd::ParseBranchListText);
    connect(mActions.createAction(Cmd::BranchListMerged, tr("List merged Branches"), Cmd::getCommand(Cmd::BranchListMerged)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListMerged, Cmd::ParseBranchListText);
    connect(mActions.createAction(Cmd::BranchListNotMerged, tr("List not merged Branches"), Cmd::getCommand(Cmd::BranchListNotMerged)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListNotMerged, Cmd::ParseBranchListText);

    connect(mActions.createAction(Cmd::BranchDelete   , tr("Delete Branch"), Cmd::getCommand(Cmd::BranchDelete)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchDelete, "Delete %1 from git;Do you want to delete \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::BranchDelete, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::BranchCheckout, tr("Checkout Branch"), Cmd::getCommand(Cmd::BranchCheckout)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchCheckout, "Checkout %1;Do you want to set \"%1\" active?");
    mActions.setCustomCommandPostAction(Cmd::BranchCheckout, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::BranchHistory, tr("History Branch"), Cmd::getCommand(Cmd::BranchHistory)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchHistory, Cmd::ParseHistoryText);
    connect(mActions.createAction(Cmd::BranchShow, tr("Show Branch"), Cmd::getCommand(Cmd::BranchShow)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));

    connect(mActions.createAction(Cmd::MoveOrRename   , tr("Move / Rename..."), Cmd::getCommand(Cmd::MoveOrRename)), SIGNAL(triggered()), this, SLOT(call_git_move_rename()));
    mActions.getAction(Cmd::MoveOrRename)->setShortcut(QKeySequence(Qt::Key_F2));

    connect(mActions.createAction(Cmd::ShowHistoryDifference, tr("Show difference")  , Cmd::getCommand(Cmd::ShowHistoryDifference)), SIGNAL(triggered()), this, SLOT(call_git_history_diff_command()));
    connect(mActions.createAction(Cmd::CallHistoryDiffTool  , tr("Call diff tool..."), Cmd::getCommand(Cmd::CallHistoryDiffTool))  , SIGNAL(triggered()), this, SLOT(call_git_history_diff_command()));

    connect(mActions.createAction(Cmd::ExpandTreeItems      , tr("Expand Tree Items"), tr("Expands all tree item of focused tree")) , SIGNAL(triggered()), this, SLOT(expand_tree_items()));
    connect(mActions.createAction(Cmd::CollapseTreeItems    , tr("Collapse Tree Items"), tr("Collapses all tree item of focused tree")), SIGNAL(triggered()), this, SLOT(collapse_tree_items()));

    connect(mActions.createAction(Cmd::AddGitSourceFolder   , tr("Add git source folder"), tr("Adds a git source folder to the source treeview")) , SIGNAL(triggered()), this, SLOT(addGitSourceFolder()));
    connect(mActions.createAction(Cmd::UpdateGitStatus      , tr("Update git status"), tr("Updates the git status of the selected source folder")), SIGNAL(triggered()), this, SLOT(updateGitStatus()));

    connect(mActions.createAction(Cmd::ShowHideTree  , tr("Show/Hide tree"), tr("Shows or hides history or branches tree")) , SIGNAL(toggled(bool)), this, SLOT(showOrHideTrees(bool)));
    mActions.getAction(Cmd::ShowHideTree)->setCheckable(true);
    connect(mActions.createAction(Cmd::ClearTreeItems       , tr("Clear tree items"), tr("Clears all history or branch tree entries")), SIGNAL(triggered()), this, SLOT(clearTrees()));

    connect(mActions.createAction(Cmd::CustomGitActionSettings, tr("Customize git actions..."), tr("Edit custom git actions, menues and toolbars")), SIGNAL(triggered()), this, SLOT(performCustomGitActionSettings()));
    connect(mActions.createAction(Cmd::InsertHashFileNames  , tr("Insert File Name List"), tr("Inserts file names that differ from previous hash")), SIGNAL(triggered()), ui->treeHistory, SLOT(insertFileNames()));
    connect(mActions.createAction(Cmd::About, tr("About..."), tr("Information about GitView")), SIGNAL(triggered()), this, SLOT(gitview_about()));


    for (const auto& fAction : mActions.getList())
    {
        mActions.setFlags(static_cast<Cmd::eCmd>(fAction.first), ActionList::BuiltIn);
    }
}

void MainWindow::initCustomAction(QAction* fAction)
{
    connect(fAction, SIGNAL(triggered()), this, SLOT(perform_custom_command()));
}

void MainWindow::on_treeHistory_customContextMenuRequested(const QPoint &pos)
{
    QVariant fItemData = ui->treeHistory->customContextMenuRequested(pos);
    mContextMenuSourceTreeItem = nullptr;
    QActionGroup fPostActionGroup(this);
    fPostActionGroup.setExclusive(false);
    if (fItemData.isValid())
    {
        switch (fItemData.type())
        {
            case QVariant::ModelIndex:
            {
                QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                mContextMenuSourceTreeItem = fSourceHook->itemFromIndex(fItemData.toModelIndex());
            }   break;
            case QVariant::Map:
            {
                auto fMap = fItemData.toMap();
                for (auto fMapItem = fMap.begin(); fMapItem != fMap.end(); ++fMapItem)
                {
                    QAction* fAction = fPostActionGroup.addAction(fMapItem.key());
                    fAction->setCheckable(true);
                    fAction->setChecked(fMapItem.value().toBool());
                }
                fPostActionGroup.addAction("Enable all");
                fPostActionGroup.addAction("Disable all");
            }   break;
            default:
                break;
        }
    }

    QMenu menu(this);
    QMenu*fAuthorsMenu = nullptr;
    if (fPostActionGroup.actions().size())
    {
        fAuthorsMenu = menu.addMenu("Authors");
        fAuthorsMenu->addActions(fPostActionGroup.actions());
    }
    if (!ui->treeHistory->isSelectionDiffable())
    {
        mActions.getAction(Cmd::CallHistoryDiffTool)->setEnabled(false);
        mActions.getAction(Cmd::ShowHistoryDifference)->setEnabled(false);
    }
    if (!ui->treeHistory->isSelectionFileDiffable())
    {
        mActions.getAction(Cmd::CallHistoryDiffTool)->setEnabled(false);
    }

    mActions.fillContextMenue(menu, Cmd::mContextMenuHistoryTree);
    QAction* fAction = menu.exec(ui->treeHistory->mapToGlobal(pos));
    if (fAction && fAuthorsMenu)
    {
        int fIndex = fAuthorsMenu->actions().indexOf(fAction);
        ui->treeHistory->checkAuthorsIndex(fIndex, fAction->isChecked());
    }

    mActions.getAction(Cmd::CallHistoryDiffTool)->setEnabled(true);
    mActions.getAction(Cmd::ShowHistoryDifference)->setEnabled(true);
    mContextMenuSourceTreeItem = nullptr;
}

void MainWindow::clearTrees()
{
    if (ui->treeHistory->isVisible())
    {
        ui->treeHistory->clear();
    }
    if (ui->treeBranches->isVisible())
    {
        ui->treeBranches->clear();
    }
}

void  MainWindow::call_git_commit()
{
    CommitMessage fCommitMsg;

    if (fCommitMsg.exec() == QDialog::Accepted)
    {
        on_btnCloseText_clicked();
        QString fMessageText = fCommitMsg.getMessageText();
        std::string  fCommand  = Cmd::getCommand(Cmd::Commit).toStdString();
        QString fCommitCommand = tr(fCommand.c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem)).arg(fMessageText);
        if (fCommitMsg.getAutoStage())
        {
            getSelectedTreeItem();
            applyGitCommandToFileTree(Cmd::getCommand(Cmd::Add));
            QString fResultStr;
            execute(fCommitCommand, fResultStr);
            apendTextToBrowser(fCommitCommand + getLineFeed() + fResultStr);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
            mContextMenuSourceTreeItem = nullptr;
        }
        else
        {
            QString fResultStr;
            execute(fCommitCommand, fResultStr);
            apendTextToBrowser(fCommitCommand + getLineFeed() + fResultStr);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
        }
    }
}

void MainWindow::call_git_move_rename()
{
    getSelectedTreeItem();
    if (mContextMenuSourceTreeItem)
    {
        bool      fOk;
        Type      fType(static_cast<Type::TypeFlags>(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt()));
        QString   fFileTypeName = Type::name(static_cast<Type::TypeFlags>(Type::FileType&fType.mType));
        QFileInfo fPath(getItemFilePath(mContextMenuSourceTreeItem));
        QString   fOldName      = mContextMenuSourceTreeItem->text(Column::FileName);
        QString   fNewName      = QInputDialog::getText(this,
                       tr("Move or rename %1").arg(fFileTypeName),
                       tr("Enter a new name or destination for \"./%1\".\n"
                          "To move the %3 insert the destination path before.\n"
                          "To rename just modify the name.").arg(fPath.filePath()).arg(fFileTypeName),
                       QLineEdit::Normal, fOldName, &fOk);

        if (fOk && !fNewName.isEmpty())
        {
            std::string fFormatCmd = Cmd::getCommand(Cmd::MoveOrRename).toStdString().c_str();
            QString     fCommand;
            bool        fMoved = false;
            if (fNewName.contains("/"))
            {
                fMoved   = true;
                fOldName = fPath.filePath();
                fCommand = tr(fFormatCmd.c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem)).arg(fOldName).arg(fNewName);
            }
            else
            {
                fCommand  = tr(fFormatCmd.c_str()).arg(fPath.absolutePath()).arg(fOldName).arg(fNewName);
            }

            QString fResultStr;
            int fResult = execute(fCommand, fResultStr);
            if (fResult == 0)
            {
                if (fMoved)
                {
                    mActions.getAction(Cmd::UpdateGitStatus)->trigger();
                }
                else
                {
                    mContextMenuSourceTreeItem->setText(Column::FileName, fNewName);
                    updateTreeItemStatus(mContextMenuSourceTreeItem);
                }
            }
        }
        mContextMenuSourceTreeItem = nullptr;
    }
}

void MainWindow::perform_custom_command()
{
    QAction *fAction = qobject_cast<QAction *>(sender());
    QString fGitCommand = fAction->statusTip();
    QVariantList fVariantList = fAction->data().toList();

    if (fVariantList[ActionList::Data::Flags].toUInt() & ActionList::Branch)
    {
        call_git_branch_command();
    }
    else
    {
        Type    fType;
        getSelectedTreeItem();
        if (fGitCommand.contains("-C %1"))
        {
            on_btnCloseText_clicked();
            QString fResultStr;
            fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem));
            execute(fGitCommand, fResultStr);
            apendTextToBrowser(fGitCommand + getLineFeed() + fResultStr);
        }
        else if (mContextMenuSourceTreeItem)
        {
            QString fMessageBoxText = fVariantList[ActionList::Data::MsgBoxText].toString();
            QString fStagedCmdAddOn = fVariantList[ActionList::Data::StagedCmdAddOn].toString();
            fType.mType = mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt();

            if (fStagedCmdAddOn.size())
            {
                fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fType.is(Type::GitStaged) ? fStagedCmdAddOn : "%1");
            }

            int fResult = QMessageBox::Yes;
            if (fMessageBoxText != ActionList::sNoCustomCommandMessageBox)
            {
                fResult = callMessageBox(fMessageBoxText, fType.type_name(), mContextMenuSourceTreeItem->text(Column::FileName), fType.is(Type::File));
            }

            if (fResult == QMessageBox::Yes || fResult == QMessageBox::YesToAll)
            {
                applyGitCommandToFileTree(fGitCommand);
            }
        }

        switch (fVariantList[ActionList::Data::Action].toUInt())
        {
            case Cmd::UpdateItemStatus:
                updateTreeItemStatus(mContextMenuSourceTreeItem);
                break;
            case Cmd::ParseHistoryText:
            {
                QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                ui->treeHistory->parseGitLogHistoryText(ui->textBrowser->toPlainText(), fSourceHook->indexFromItem(mContextMenuSourceTreeItem), getItemFilePath(mContextMenuSourceTreeItem), fType.mType);
                ui->textBrowser->setPlainText("");
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
            }   break;
        }
        mContextMenuSourceTreeItem = nullptr;
    }
}

void MainWindow::call_git_history_diff_command()
{
    const QString &fHistoryHashItems = ui->treeHistory->getSelectedHistoryHashItems();
    const QString &fHistoryFile      = ui->treeHistory->getSelectedHistoryFile();
    QAction       *fAction           = qobject_cast<QAction *>(sender());
    Type           fType(ui->treeHistory->getSelectedTopLevelType());

    QString fPath;
    if (fType.is(Type::Branch) && mContextMenuSourceTreeItem)
    {
        fPath =  mContextMenuSourceTreeItem->text(0);
        mContextMenuSourceTreeItem = nullptr;
    }
    if (mContextMenuSourceTreeItem)
    {
        QString fCmd = tr(fAction->statusTip().toStdString().c_str()).arg(fHistoryHashItems).arg("-- %1");
        if (fHistoryFile.size())
        {
            fCmd = tr(fCmd.toStdString().c_str()).arg(fHistoryFile);
            QString fResult;
            int fError = execute(fCmd, fResult);
            if (fError)
            {
                apendTextToBrowser(fCmd + getLineFeed() + tr("Error occurred: %1").arg(fError));
            }
            else
            {
                apendTextToBrowser(fCmd + getLineFeed() + fResult);
            }
        }
        else
        {
            applyGitCommandToFileTree(fCmd);
        }
    }
    else
    {
        QString fCmd;
        QString fFormatString = fAction->statusTip();
        if (fPath.size())
        {
            fFormatString.replace(QString("git "), tr("git -C %1 ").arg(fPath));
        }

        if (fHistoryFile.size())
        {
            fCmd = tr(fFormatString.toStdString().c_str()).arg(fHistoryHashItems).arg(fHistoryFile);
        }
        else
        {
            fCmd = tr(fFormatString.toStdString().c_str()).arg(fHistoryHashItems).arg("");
        }
        QString fResult;
        int fError = execute(fCmd, fResult);
        if (fError)
        {
            apendTextToBrowser(fCmd + getLineFeed() + tr("Error occurred: %1").arg(fError));
        }
        else
        {
            apendTextToBrowser(fCmd + getLineFeed() + fResult);
        }
    }
}

void MainWindow::call_git_branch_command()
{
    const QAction*fAction            = qobject_cast<QAction *>(sender());
    QString       fGitCommand        = fAction->statusTip();
    QVariantList  fVariantList       = fAction->data().toList();
    QString       fTopItemPath       = getItemTopDirPath(mContextMenuSourceTreeItem);
    const QString fMessageBoxText    = fVariantList[ActionList::Data::MsgBoxText].toString();
    const QString fBranchItem        = ui->treeBranches->getSelectedBranch();
    const QString fBranchGitRootPath = ui->treeBranches->getSelectedBranchGitRootPath();

    int fResult = QMessageBox::Yes;
    if (fMessageBoxText != ActionList::sNoCustomCommandMessageBox)
    {
        fResult = callMessageBox(fMessageBoxText, "branch", fBranchItem);
    }

    QString fResultStr;
    if (fResult == QMessageBox::Yes || fResult == QMessageBox::YesToAll)
    {
        on_btnCloseText_clicked();
        if (fGitCommand.contains("-C %1"))
        {
            if (fGitCommand.contains("%2") && fBranchItem.size())
            {
                fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fTopItemPath).arg(fBranchItem);
            }
            else
            {
                fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fTopItemPath);
            }
        }
        else if (fGitCommand.contains("%1") && fBranchItem.size())
        {
            fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fBranchItem);
        }

        fResult = execute(fGitCommand, fResultStr);
        if (fResult == 0)
        {
            fResultStr = fGitCommand + getLineFeed() + fResultStr;
        }
        else
        {
            fResultStr = tr("result of command \"%1\" is %2:%3%4").arg(fGitCommand).arg(fResult).arg(getLineFeed()).arg(fResultStr);
        }
        apendTextToBrowser(fResultStr);
    }

    if (fResult == 0)
    {
        switch (fVariantList[ActionList::Data::Action].toUInt())
        {
            case Cmd::UpdateItemStatus:
                fResultStr.clear();
                fGitCommand  = ui->treeBranches->getBranchTopItemText();
                fTopItemPath = ui->treeBranches->getSelectedBranchGitRootPath();
                execute(fGitCommand, fResultStr);
                ui->treeBranches->parseBranchListText(fGitCommand + getLineFeed() + fResultStr + getLineFeed(), fTopItemPath);
                break;
            case Cmd::ParseBranchListText:
                ui->treeBranches->parseBranchListText(fResultStr, fTopItemPath);
                ui->textBrowser->setPlainText(fGitCommand);
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
                break;
            case Cmd::ParseHistoryText:
            {
                auto fItems = ui->treeSource->findItems(fBranchGitRootPath, Qt::MatchExactly);
                QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                ui->treeHistory->parseGitLogHistoryText(fResultStr, fItems.size() ? QVariant(fSourceHook->indexFromItem(fItems[0])) : QVariant(fBranchGitRootPath), fBranchItem, Type::Branch);
                ui->textBrowser->setPlainText(fGitCommand);
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
            }    break;
        }
    }
}


void MainWindow::expand_tree_items()
{
    focusedTreeWidget()->expandAll();
}

void MainWindow::collapse_tree_items()
{
    focusedTreeWidget()->collapseAll();
}

QTreeWidget* MainWindow::focusedTreeWidget(bool aAlsoSource)
{
    if (ui->treeHistory->hasFocus())
    {
        return ui->treeHistory;
    }
    else if (ui->treeBranches->hasFocus())
    {
        return ui->treeBranches;
    }
    else if (aAlsoSource)
    {
        return ui->treeSource;
    }
    return nullptr;
}

void MainWindow::performCustomGitActionSettings()
{
    CustomGitActions fCustomGitActions(mActions);
    connect(&fCustomGitActions, SIGNAL(initCustomAction(QAction*)), this, SLOT(initCustomAction(QAction*)));
    fCustomGitActions.exec();
}

void MainWindow::on_treeBranches_customContextMenuRequested(const QPoint &pos)
{
    ui->treeBranches->on_customContextMenuRequested(mActions, pos);
}

void MainWindow::gitview_about()
{
    AboutDlg dlg(this);
    dlg.exec();
}

