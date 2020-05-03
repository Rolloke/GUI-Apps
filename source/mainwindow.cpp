#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "commitmessage.h"
#include "history.h"
#include "customgitactions.h"

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
// git fetch

// TODO: Änderungen vom Remote Repository abholen
// git fetch pi@RaspiLAN:git.repository

// TODO: CustomGitActionSettings bugfixing icon path to store is wrong

// TODO: branch handling
// TODO: 1.4.10.2 Zweig erstellen

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
const QString sIconPath("IconPath");
const QString sShortcut("Shortcut");
const QString sModified("Modified");

} // namespace config



#ifdef __linux__
const QString MainWindow::mNativeLineFeed = "\n";
#else
const QString MainWindow::mNativeLineFeed = "\r\n";
#endif

MainWindow::MainWindow(const QString& aConfigName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mWorker(this)
    , mCurrentTask(Work::None)
    , mActions(this)
    , mConfigFileName(aConfigName)
    , mContextMenuItem(nullptr)
    , mLineFeed(mNativeLineFeed)
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

    ui->treeSource->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->treeHistory->setVisible(false);
    ui->treeHistory->setContextMenuPolicy(Qt::CustomContextMenu);

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
    std::uint32_t fSeverityValue = fSeverity.toLong(0, 2);
    Logger::setSeverity(0xffff, false);
    Logger::setSeverity(fSeverityValue, true);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);

    int fItemCount = fSettings.beginReadArray(config::sSourcePath);
    for (int fItem = 0; fItem < fItemCount; ++fItem)
    {
        fSettings.setArrayIndex(fItem);
        const QDir fSourceDir = initDir(fSettings.value(config::sSourcePath).toString(), fSettings.value(config::sSourcePath+"/"+config::sGroupFilter).toInt());
        insertSourceTree(fSourceDir, fItem);
    }
    fSettings.endArray();

    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupGitCommands);

    LOAD_STRF(fSettings, Cmd::mContextMenuSourceTree, Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mContextMenuEmptySourceTree, Cmd::fromString, Cmd::toString, toString);
    LOAD_STRF(fSettings, Cmd::mContextMenuHistoryTree, Cmd::fromString, Cmd::toString, toString);
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
        mActions.setFlags(fCmd, fFlags);
        mActions.setIconPath(fCmd, fSettings.value(config::sIconPath).toString());
        mActions.setCustomCommandMessageBoxText(fCmd, fSettings.value(config::sCustomMessageBoxText).toString());
        mActions.setCustomCommandPostAction(fCmd, fSettings.value(config::sCustomCommandPostAction).toUInt());
    }
    fSettings.endArray();
    fSettings.endGroup();


    for (auto fToolbar : Cmd::mToolbars)
    {
        QToolBar*pTB = new QToolBar();
        mActions.fillToolbar(*pTB, fToolbar);
        ui->horizontalLayoutTool->addWidget(pTB);
    }

    TRACE(Logger::info, "%s Started", windowTitle());
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

    STORE_STRF(fSettings, Cmd::mContextMenuSourceTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mContextMenuEmptySourceTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mContextMenuHistoryTree, Cmd::toString);
    STORE_STRF(fSettings, Cmd::mToolbars[0], Cmd::toString);
    STORE_STRF(fSettings, Cmd::mToolbars[1], Cmd::toString);

    fSettings.beginWriteArray(config::sCommands);

    {
        int fIndex = 0;

        for (auto fItem : mActions.getList())
        {
            Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(fItem.first);

            if (   fItem.first >= Cmd::FirstGitCommand && fItem.first <= Cmd::LastGitCommand
                && mActions.getFlags(fCmd) & ActionList::Modified)
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
    QString fSeverity = QString::number(Logger::getSeverity()|Logger::highest, 2);
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
#if RELATIVE_GIT_PATH ==1
        if (!fParent.isRoot())
        {
            aParentItem->setData(INT(Column::FileName), INT(Role::GitFolder), QVariant(fParent.absolutePath()));
        }
#endif
        fTopLevelItem = true;
    }

    addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();

        if (ignoreFile(fFileInfo)) continue;

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

        Type fType;
        if (fFileInfo.isFile())       fType.add(Type::File);
        if (fFileInfo.isDir())        fType.add(Type::Folder);
        if (fFileInfo.isHidden())     fType.add(Type::Hidden);
        if (fFileInfo.isExecutable()) fType.add(Type::Executeable);
        if (fFileInfo.isSymLink())    fType.add(Type::SymLink);

        fItem->setData(INT(Column::State), INT(Role::Filter), QVariant(fType.mType));

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
        mIgnoreContainingNegation.reset();
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
                    QString fCmd = applyGitCmd(fSource, mGitCommand, fResultStr);
                    ui->textBrowser->insertPlainText(fCmd + getLineFeed() + fResultStr + getLineFeed());
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
                QVariant fVariant = aParentItem->data(INT(Column::State), INT(Role::Filter));
                if (fVariant.isValid())
                {
                    Type fType(fVariant.toUInt());
                    switch (mCurrentTask)
                    {
                        case Work::ApplyGitCommand:
                        {
                            QString fCmd = applyGitCmd(fSource, mGitCommand, fResultStr);
                            ui->textBrowser->insertPlainText(fCmd + getLineFeed() + fResultStr + getLineFeed());
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

            if (fFoundType->second.is(Type::GitAdded   ))  fState += Type::name(Type::GitAdded)     + fSep;
            if (fFoundType->second.is(Type::GitDeleted ))  fState += Type::name(Type::GitDeleted)   + fSep;
            if (fFoundType->second.is(Type::GitModified))  fState += Type::name(Type::GitModified)  + fSep;
            if (fFoundType->second.is(Type::GitUnTracked)) fState += Type::name(Type::GitUnTracked) + fSep;
            if (fFoundType->second.is(Type::GitRenamed ))  fState += Type::name(Type::GitRenamed)   + fSep;
            if (fFoundType->second.is(Type::GitStaged  ))  fState += Type::name(Type::GitStaged)    + fSep;
            if (fFoundType->second.is(Type::GitUnmerged))  fState += Type::name(Type::GitUnmerged)  + fSep;
            if (fFoundType->second.is(Type::GitLocal   ))  fState += Type::name(Type::GitLocal)     + fSep;
            if (fFoundType->second.is(Type::GitRemote  ))  fState += Type::name(Type::GitRemote)    + fSep;
            if (fFoundType->second.is(Type::GitBoth    ))  fState += Type::name(Type::GitBoth)      + fSep;
            aParentItem->setText(INT(Column::State), fState);
            Type fType(aParentItem->data(INT(Column::State), INT(Role::Filter)).toUInt());
            fType.add(static_cast<Type::TypeFlags>(fFoundType->second.mType));
            aParentItem->setData(INT(Column::State), INT(Role::Filter), QVariant(fType.mType));
            TRACE(Logger::info, "set state %s, %x of %s", fState.toStdString().c_str(), fFoundType->second.mType, fSourcePath.toStdString().c_str());
        }
        else
        {
            Type fType(aParentItem->data(INT(Column::State), INT(Role::Filter)).toUInt());
            fType.remove(Type::AllGitActions);
            aParentItem->setData(INT(Column::State), INT(Role::Filter), QVariant(fType.mType));
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
            fLine.remove(getLineFeed());
            int fIndex = fLine.indexOf('#');
            if (fIndex != -1)
            {
                fLine.remove(fIndex,fLine.size()-fIndex);
                fLine = fLine.trimmed();
            }
            Type fType(fLine.contains('/') ? Type::Folder : Type::File, fMapLevel);
            if (fLine.contains('*')) fType.add(Type::WildCard);
            if (fLine.contains('?')) fType.add(Type::WildCard);
            if (fLine.contains('[') && fLine.contains(']')) fType.add(Type::RegExp);
            if (fLine.contains('!')) fType.add(Type::Negation);
            fLine.remove(QChar::CarriageReturn);
            fIndex = fLine.lastIndexOf('/');
            if (fIndex != -1 && fIndex == fLine.size() - 1)
            {
                fLine.remove(fIndex, 1);
            }
            fIndex = fLine.indexOf('!');
            if (fIndex == 0)
            {
                fLine.remove(fIndex, 1);
            }

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
    for (const auto& fItem : mIgnoreMap)
    {
        if (fItem.second.is(Type::Negation))
        {
            for (auto& fSearchItem : mIgnoreMap)
            {
                if (   fItem.first.find(fSearchItem.first) != std::string::npos
                    && fItem.first != fSearchItem.first)
                {
                     fSearchItem.second.add(Type::ContainingNegation);
                }
            }
        }
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

bool MainWindow::ignoreFile(const QFileInfo& aFileInfo)
{
    const std::string& fFileName = aFileInfo.fileName().toStdString();
    const std::string& fFilePath = aFileInfo.filePath().toStdString();

    auto fFound = mIgnoreMap.find(fFileName);
    if (fFound == mIgnoreMap.end())
    {
        for (auto fItem = mIgnoreMap.begin(); fItem != mIgnoreMap.end(); ++fItem)
        {
            if (fItem->second.is(Type::WildCard) || fItem->second.is(Type::RegExp))
            {
                QRegExp fRegEx(fItem->first.c_str());
                fRegEx.setPatternSyntax(fItem->second.is(Type::WildCard) ? QRegExp::Wildcard : QRegExp::RegExp);
                if (fRegEx.exactMatch(fFileName.c_str()))
                {
                    fFound = fItem;
                    break;
                }
            }
        }
    }

    if (mIgnoreContainingNegation)
    {
        if (fFound != mIgnoreMap.end() && fFound->second.is(Type::Negation))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (fFound != mIgnoreMap.end())
        {
            if (fFound->second.is(Type::ContainingNegation))
            {
                mIgnoreContainingNegation = *fFound;
                return false;
            }
            if (fFound->second.is(Type::File)   && aFileInfo.isFile()) return true;
            if (fFound->second.is(Type::Folder) && aFileInfo.isDir() ) return true;
        }
    }
    return false;
}

void MainWindow::insertSourceTree(const QDir& fSourceDir, int aItem)
{
    QString fResultString;
    applyGitCmd(fSourceDir.path(), Cmd::getCommand(Cmd::GetStatusAll), fResultString);

    ui->textBrowser->setText(fResultString);
    ui->labelFilePath->setText("");
    stringt2typemap fCheckMap;
    parseGitStatus(fSourceDir.path() +  QDir::separator(), fResultString, fCheckMap);

    insertItem(fSourceDir, *ui->treeSource);

    // TODO: in review
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

const QString& MainWindow::getLineFeed() const
{
    return mLineFeed;
}

void  MainWindow::setLineFeed(const QString& aLF)
{
    mLineFeed = aLF;
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
    auto fLines = aStatus.split("\n");

    for (QString fLine : fLines)
    {
        const int fStateSize = 2;
        QString fState = fLine.left(fStateSize);
        auto fRelativePath = fLine.mid(fStateSize).trimmed();

        if (fRelativePath.size())
        {
            Type fType(Type::translate(fState));

            if (fType.is(Type::Repository))
            {
                aFiles[fRelativePath.toStdString()] = fType;
            }
            else
            {
                QString fFullPath = fSource + QDir::toNativeSeparators(fRelativePath);
                QFileInfo fFileInfo(fFullPath);
                if (fFileInfo.isFile())   fType.add(Type::File);
                if (fFileInfo.isDir())    fType.add(Type::Folder);
                aFiles[fFullPath.toStdString()] = fType;
            }

            TRACE(Logger::trace, "%s: %s: %x", fState.toStdString().c_str(), fRelativePath.toStdString().c_str(), fType.mType);
        }
    }
}

void MainWindow::parseGitLogHistoryText()
{
    QVector<QStringList> fList;
    History::parse(ui->textBrowser->toPlainText(), fList);
    ui->textBrowser->setPlainText("");

    QString fFileName = getItemFilePath(mContextMenuItem);
    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(fFileName));
    ui->treeHistory->addTopLevelItem(fNewHistoryItem);
    QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
    fNewHistoryItem->setData(INT(History::Column::Commit), INT(History::Role::ContextMenuItem), QVariant(fSourceHook->indexFromItem(mContextMenuItem)));

    ui->treeHistory->setVisible(true);
    mActions.getAction(Cmd::ShowHideHistoryTree)->setChecked(true);

    const int fTLI = ui->treeHistory->topLevelItemCount()-1;
    int fListCount = 0;
    for (auto fItem: fList)
    {
        if (++fListCount == 1) continue; // ignore first item
        if (fItem.count() >= INT(History::Entry::NoOfEntries))
        {
            fNewHistoryItem = new QTreeWidgetItem();
            ui->treeHistory->topLevelItem(fTLI)->addChild(fNewHistoryItem);
            fNewHistoryItem->setText(INT(History::Column::Text), fItem[INT(History::Entry::CommitterDate)]);
            fNewHistoryItem->setText(INT(History::Column::Commit), fItem[INT(History::Entry::Author)]);
            for (int fRole=0; fRole < INT(History::Entry::NoOfEntries); ++fRole)
            {
                fNewHistoryItem->setData(INT(History::Column::Commit), fRole, QVariant(fItem[fRole]));
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
                if (aTreeItem->data(INT(Column::FileName), INT(Role::GitFolder)).isValid())
                {
                    QDir::setCurrent(aTreeItem->text(INT(Column::FileName)));
                }
                else
                {
                    fFileName = aTreeItem->text(INT(Column::FileName)) + QDir::separator() + fFileName;
                }
            }
        }
    }
    return fFileName;
}

// SLOTS

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
        fSourceDirs.push_back(ui->treeSource->topLevelItem(i)->text(INT(Column::FileName)));
    }
    ui->treeSource->clear();

    qint64 fSize = 0;
    for (uint i = 0; i < fSourceDirs.size(); ++i)
    {
        insertSourceTree(fSourceDirs[i], i);
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
    mContextMenuItem = ui->treeSource->itemAt( pos );
    if (mContextMenuItem)
    {
        //Type fType(static_cast<Type::TypeFlags>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));
        QMenu menu(this);
        mActions.fillContextMenue(menu, Cmd::mContextMenuSourceTree);
        menu.exec( ui->treeSource->mapToGlobal(pos) );
        mContextMenuItem = nullptr;
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
        case ComboShowItems::Gitstaged:
            handleWorker(INT(Work::ShowStaged));
            break;
        case ComboShowItems::GitUnmerged:
            handleWorker(INT(Work::ShowUnMerged));
            break;
    }
}




void MainWindow::on_treeSource_itemClicked(QTreeWidgetItem *item, int /* column */ )
{
    mContextMenuItem = item;
}

void MainWindow::getSelectedTreeItem()
{
    if (! mContextMenuItem)
    {
        auto fSelected = ui->treeSource->selectedItems();
        if (fSelected.size())
        {
            mContextMenuItem = fSelected.at(0);
        }
    }
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

QString MainWindow::applyGitCmd(const QString& fSource, const QString& fGitCmd, QString& aResultStr)
{
    QString fCommand = QObject::tr(fGitCmd.toStdString().c_str()).arg(fSource);
    int fResult = execute(fCommand, aResultStr);
    return fResult == 0 ? fCommand : "";
}

void MainWindow::updateTreeItemStatus(QTreeWidgetItem * aItem)
{
    QFileInfo fFileInfo(getItemFilePath(aItem));
//test

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

        parseGitStatus(fRepositoryPath + QDir::separator(), fResultString, fCheckMap);

        QString fSourcePath = fFileInfo.absolutePath();
        iterateCheckItems(aItem, fCheckMap, &fSourcePath);
    }
}

void MainWindow::showOrHideHistory(bool checked)
{
    ui->treeHistory->setVisible(checked);
    mActions.getAction(Cmd::ShowHideHistoryTree)->setChecked(checked);
}

void MainWindow::on_treeHistory_itemClicked(QTreeWidgetItem *aItem, int /* aColumn */)
{
    QString fText;
//    if (aColumn == INT(History::Column::Text))
    {
        for (int fRole=INT(History::Entry::CommitHash); fRole < INT(History::Entry::NoOfEntries); ++fRole)
        {
            fText.append(History::name(static_cast<History::Entry>(fRole)));
            fText.append(getLineFeed());
            fText.append(aItem->data(INT(History::Column::Commit), fRole).toString());
            fText.append(getLineFeed());
        }
    }
//    else
//    {
//        QString fGitCmd = "git show ";
//        fGitCmd.append(aItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
//        QString fResultStr;
//        int fResult = execute(fGitCmd, fResultStr);
//        fText = fGitCmd;
//        fText.append(getLineFeed());
//        fText.append(fResultStr);

//        if (fResult)
//        {
//            fText.append(getLineFeed());
//            fText.append(tr("Result failure no: %1").arg(fResult));
//        }
//    }
    ui->textBrowser->setPlainText(fText);
}

void MainWindow::initContextMenuActions()
{
    connect(mActions.createAction(Cmd::ShowStatus     , tr("Show status")       , Cmd::getCommand(Cmd::ShowStatus))     , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    connect(mActions.createAction(Cmd::ShowDifference , tr("Show difference")   , Cmd::getCommand(Cmd::ShowDifference)) , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setStagedCmdAddOn(Cmd::ShowDifference, "--cached %1");
    connect(mActions.createAction(Cmd::CallDiffTool   , tr("Call diff tool...") , Cmd::getCommand(Cmd::CallDiffTool))   , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallDiffTool)->setShortcut(QKeySequence(Qt::Key_F9));
    mActions.setStagedCmdAddOn(Cmd::CallDiffTool, "--cached %1");
    connect(mActions.createAction(Cmd::ShowShortStatus, tr("Show short status") , Cmd::getCommand(Cmd::ShowShortStatus)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));

    connect(mActions.createAction(Cmd::Add            , tr("Add to git (stage)"), Cmd::getCommand(Cmd::Add))            , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Add, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Add)->setShortcut(QKeySequence(Qt::Key_F4));

    connect(mActions.createAction(Cmd::Unstage        , tr("Reset file (unstage)"), Cmd::getCommand(Cmd::Unstage))      , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Unstage, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Unstage)->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F4));

    connect(mActions.createAction(Cmd::History        , tr("Show History")      , Cmd::getCommand(Cmd::History))        , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::History, Cmd::ParseHistoryText);
    mActions.getAction(Cmd::History)->setShortcut(QKeySequence(Qt::Key_F10));

    connect(mActions.createAction(Cmd::Remove         , tr("Remove from git..."), Cmd::getCommand(Cmd::Remove))         , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Remove, "Remove %1 from git repository;Do you want to remove \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::Remove, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Remove)->setShortcut(QKeySequence(Qt::Key_Delete));

    connect(mActions.createAction(Cmd::Restore         , tr("Restore changes..."), Cmd::getCommand(Cmd::Restore))       , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Restore, "Restore changes;Do you want to restore changes in file \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::Restore, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Restore)->setShortcut(QKeySequence(Qt::Key_F6));

    connect(mActions.createAction(Cmd::Commit         , tr("Commit...")), SIGNAL(triggered()), this, SLOT(call_git_commit()));
    mActions.setCustomCommandPostAction(Cmd::Commit, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::Push           , tr("Push")        , Cmd::getCommand(Cmd::Push))    , SIGNAL(triggered()), this, SLOT(perform_custom_command()));

    connect(mActions.createAction(Cmd::MoveOrRename   , tr("Move / Rename...")), SIGNAL(triggered()), this, SLOT(call_git_move_rename()));
    mActions.getAction(Cmd::MoveOrRename)->setShortcut(QKeySequence(Qt::Key_F2));

    connect(mActions.createAction(Cmd::ShowHistoryDifference, tr("Show difference")  , Cmd::getCommand(Cmd::ShowHistoryDifference)), SIGNAL(triggered()), this, SLOT(call_git_history_diff_command()));
    connect(mActions.createAction(Cmd::CallHistoryDiffTool  , tr("Call diff tool..."), Cmd::getCommand(Cmd::CallHistoryDiffTool))  , SIGNAL(triggered()), this, SLOT(call_git_history_diff_command()));

    connect(mActions.createAction(Cmd::ExpandTreeItems      , tr("Expand Tree Items"), tr("Expands all tree item of focused tree")) , SIGNAL(triggered()), this, SLOT(expand_tree_items()));
    connect(mActions.createAction(Cmd::CollapseTreeItems    , tr("Collapse Tree Items"), tr("Collapses all tree item of focused tree")), SIGNAL(triggered()), this, SLOT(collapse_tree_items()));

    connect(mActions.createAction(Cmd::AddGitSourceFolder   , tr("Add git source folder"), tr("adds a git source folder to the source treeview")) , SIGNAL(triggered()), this, SLOT(addGitSourceFolder()));
    connect(mActions.createAction(Cmd::UpdateGitStatus      , tr("Update git status"), tr("Updates the git status of the selected source folder")), SIGNAL(triggered()), this, SLOT(updateGitStatus()));

    connect(mActions.createAction(Cmd::ShowHideHistoryTree  , tr("Show/Hide history tree"), tr("Shows or hides history tree")) , SIGNAL(toggled(bool)), this, SLOT(showOrHideHistory(bool)));
    mActions.getAction(Cmd::ShowHideHistoryTree)->setCheckable(true);
    connect(mActions.createAction(Cmd::ClearHistory         , tr("Clear history tree items"), tr("Cleas all history tree entries")), SIGNAL(triggered()), this, SLOT(clearHistoryTree()));

    connect(mActions.createAction(Cmd::CustomGitActionSettings, tr("Customize git actions..."), tr("Edit custom git actions, menues and toolbars")), SIGNAL(triggered()), this, SLOT(performCustomGitActionSettings()));

    for (auto fAction : mActions.getList())
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
    QTreeWidgetItem* fSelectedHistoryItem = ui->treeHistory->itemAt(pos);
    if (fSelectedHistoryItem)
    {
        QModelIndexList fSelectedHistoryIndexes = ui->treeHistory->selectionModel()->selectedIndexes();
        QTreeWidgetItem* fParentHistoryItem = fSelectedHistoryItem->parent();
        if (fParentHistoryItem)
        {
            QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
            mContextMenuItem = fSourceHook->itemFromIndex(fParentHistoryItem->data(INT(History::Column::Commit), INT(History::Role::ContextMenuItem)).toModelIndex());
            if (mContextMenuItem)
            {
                mHistoryHashItems.clear();
                for (auto fIndex = fSelectedHistoryIndexes.rbegin(); fIndex != fSelectedHistoryIndexes.rend(); ++fIndex)
                {
                    QTreeWidgetHook* fHistoryHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeHistory);
                    QTreeWidgetItem* fItem = fHistoryHook->itemFromIndex(*fIndex);
                    if (fItem && fItem->parent() == fParentHistoryItem)
                    {
                        mHistoryHashItems.append(fItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
                        mHistoryHashItems.append(" ");
                    }
                }

                QMenu menu(this);
                mActions.fillContextMenue(menu, Cmd::mContextMenuHistoryTree);
                menu.exec( ui->treeHistory->mapToGlobal(pos) );
            }
        }
    }
}

void MainWindow::clearHistoryTree()
{
    ui->treeHistory->clear();
}

void MainWindow::performCustomGitActionSettings()
{
    CustomGitActions fCustomGitActions(mActions);
    connect(&fCustomGitActions, SIGNAL(initCustomAction(QAction*)), this, SLOT(initCustomAction(QAction*)));
    fCustomGitActions.exec();
}

void  MainWindow::call_git_commit()
{
    CommitMessage fCommitMsg;

    if (fCommitMsg.exec() == QDialog::Accepted)
    {
        getSelectedTreeItem();
        QString fMessageText = fCommitMsg.getMessageText();
        std::string  fCommand  = Cmd::getCommand(Cmd::Commit).toStdString();
        QString fCommitCommand = tr(fCommand.c_str()).arg(fMessageText).arg("%1");
        if (fCommitMsg.getAutoStage())
        {
            performGitCmd(Cmd::getCommand(Cmd::Add));
        }
        performGitCmd(fCommitCommand);
        updateTreeItemStatus(mContextMenuItem);
        mContextMenuItem = nullptr;
    }
}

void MainWindow::call_git_move_rename()
{
    getSelectedTreeItem();
    if (mContextMenuItem)
    {
        bool    fOk;
        Type    fType(static_cast<Type::TypeFlags>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));
        QString fFileTypeName = Type::name(static_cast<Type::TypeFlags>(Type::FileType&fType.mType));
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
        mContextMenuItem = nullptr;
    }
}

void MainWindow::perform_custom_command()
{
    getSelectedTreeItem();
    if (mContextMenuItem)
    {
        QAction *fAction = qobject_cast<QAction *>(sender());
        QVariantList fVariantList = fAction->data().toList();
        QString fMessageBoxText = fVariantList[INT(ActionList::Data::MsgBoxText)].toString();
        QString fGitCommand = fAction->statusTip();
        QString fStagedCmdAddOn = fVariantList[INT(ActionList::Data::StagedCmdAddOn)].toString();
        Type    fType(static_cast<Type::TypeFlags>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));

        if (fStagedCmdAddOn.size())
        {
            fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fType.is(Type::GitStaged) ? fStagedCmdAddOn : "%1");
        }

        if (fMessageBoxText != ActionList::sNoCustomCommandMessageBox)
        {
            QStringList fTextList = fMessageBoxText.split(";");
            QMessageBox fSaveRequest;
            Type fType(static_cast<Type::TypeFlags>(mContextMenuItem->data(INT(Column::State), INT(Role::Filter)).toUInt()));
            QString fFileTypeName = Type::name(static_cast<Type::TypeFlags>(Type::FileType&fType.mType));
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
                performGitCmd(fGitCommand);
            }
        }
        else
        {
            performGitCmd(fGitCommand);
        }

        if (fVariantList.size() > 1)
        {
            switch (fVariantList[INT(ActionList::Data::Action)].toUInt())
            {
            case Cmd::UpdateItemStatus:
                updateTreeItemStatus(mContextMenuItem);
                break;
            case Cmd::ParseHistoryText:
                parseGitLogHistoryText();
                break;
            }
        }
        mContextMenuItem = nullptr;
    }
}

void MainWindow::call_git_history_diff_command()
{
    QAction *fAction = qobject_cast<QAction *>(sender());
    performGitCmd(tr(fAction->statusTip().toStdString().c_str()).arg(mHistoryHashItems).arg("%1"));
}


void MainWindow::expand_tree_items()
{
    if (ui->treeHistory->hasFocus())
    {
        ui->treeHistory->expandAll();
    }
    else
    {
        ui->treeSource->expandAll();
    }
}

void MainWindow::collapse_tree_items()
{
    if (ui->treeHistory->hasFocus())
    {
        ui->treeHistory->collapseAll();
    }
    else
    {
        ui->treeSource->collapseAll();
    }
}
