#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "commitmessage.h"
#include "history.h"
#include "qbranchtreewidget.h"
#include "customgitactions.h"
#include "aboutdlg.h"
#include "mergedialog.h"

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
#include <QDockWidget>

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

// TODO: merge command mit Dialog implementieren
// git merge --abort
// git merge --continue
// git merge -s <resolve|recursive <ours|theirs|patience> > obsolete
// git mergetool
// Preview anzeigen ...
// git log --merge -p <path>
// TODO

namespace config
{
const QString sGroupFilter("Filter");
const QString sGroupView("View");
const QString sGroupPaths("Paths");
const QString sSourcePath("Source");
const QString sUncheckedPath("Unchecked");
const QString sGroupLogging("Logging");
const QString sGroupGitCommands("GitCommands");
const QString sGroupFind("Find");
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
    , mDockedWidgetMinMaxButtons(true)
    , ui(new Ui::MainWindow)
    , mWorker(this)
    , mCurrentTask(Work::None)
    , mActions(this)
    , mConfigFileName(aConfigName)
    , mContextMenuSourceTreeItem(nullptr)
    , mFontName("Courier")
{
    ui->setupUi(this);
#ifdef DOCKED_VIEWS
    createDockWindows();
#endif

    setWindowIcon(QIcon(":/resource/logo@2x.png"));
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleWorker, this, _1, _2));
    QObject::connect(this, SIGNAL(doWork(int, QVariant)), &mWorker, SLOT(doWork(int,QVariant)));
    mWorker.setMessageFunction(boost::bind(&MainWindow::handleMessage, this, _1, _2));
    connect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(textBrowserChanged()));

    Highlighter::Language::load(fSettings);

    mHighlighter.reset(new Highlighter(ui->textBrowser->document()));

    ui->treeSource->header()->setSortIndicator(Column::FileName, Qt::AscendingOrder);
    ui->treeSource->header()->setSectionResizeMode(Column::FileName, QHeaderView::Stretch);
    ui->treeSource->header()->setSectionResizeMode(Column::DateTime, QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(Column::Size    , QHeaderView::Interactive);
    ui->treeSource->header()->setSectionResizeMode(Column::State   , QHeaderView::Interactive);
    ui->treeSource->header()->setStretchLastSection(false);

    ui->treeSource->setContextMenuPolicy(Qt::CustomContextMenu);

    fSettings.beginGroup(config::sGroupFilter);
    LOAD_PTR(fSettings, ui->ckHiddenFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckSymbolicLinks, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckSystemFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckFiles, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckDirectories, setChecked, isChecked, toBool);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFind);
    LOAD_PTR(fSettings, ui->ckFindCaseSensitive, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckFindRegEx, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->ckFindWholeWord, setChecked, isChecked, toBool);
    LOAD_PTR(fSettings, ui->comboFindBox, setCurrentIndex, currentIndex, toInt);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupLogging);
    QString fSeverity;
    LOAD_STR(fSettings, fSeverity, toString);
    std::uint32_t fSeverityValue = fSeverity.toLong(0, 2);
    Logger::setSeverity(0xffff, false);
    Logger::setSeverity(fSeverityValue, true);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);

    // NOTE: Remote Repositories noetig
    const QString fCommand = "git remote -v";
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
    LOAD_STRF(fSettings, mMergeTools, Cmd::fromStringMT, Cmd::toStringMT, toString);

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
        if (fFlags & ActionList::Flags::Custom)
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

    initMergeTools();

#ifdef DOCKED_VIEWS
    for (uint i=0; i < Cmd::mToolbars.size(); ++i)
    {
        const auto& fToolbar = Cmd::mToolbars[i];
        QToolBar*pTB = new QToolBar(Cmd::mToolbarNames[i]);
        mActions.fillToolbar(*pTB, fToolbar);
        pTB->setObjectName(Cmd::mToolbarNames[i]);
        pTB->setIconSize(QSize(24,24));
        addToolBar(Qt::TopToolBarArea, pTB);
        if (i==0) insertToolBarBreak(pTB);
    }
#else
    for (const auto& fToolbar : Cmd::mToolbars)
    {
        QToolBar*pTB = new QToolBar();
        mActions.fillToolbar(*pTB, fToolbar);
        ui->horizontalLayoutTool->addWidget(pTB);
    }
#endif

    fSettings.beginGroup(config::sGroupView);
    LOAD_PTR(fSettings, ui->ckHideEmptyParent, setChecked, isChecked, toBool);
    LOAD_STR(fSettings, Type::mShort, toBool);
    ui->ckShortState->setChecked(Type::mShort);
    {
        LOAD_STR(fSettings, mFontName, toString);
        QFont font;
        font.setFamily(mFontName);
        font.setFixedPitch(true);
        font.setPointSize(10);
        ui->textBrowser->setFont(font);
    }

#ifdef DOCKED_VIEWS
    QByteArray fWindowGeometry;
    LOAD_STR(fSettings, fWindowGeometry, toByteArray);
    QByteArray fWindowState;
    LOAD_STR(fSettings, fWindowState, toByteArray);
    LOAD_STR(fSettings, mDockedWidgetMinMaxButtons, toBool);

    if (fWindowGeometry.size())
    {
        restoreGeometry(fWindowGeometry);
    }
    if (fWindowState.size())
    {
        restoreState(fWindowState);
    }
#endif

    fSettings.endGroup();
    TRACE(Logger::info, "%s Started", windowTitle().toStdString().c_str());
}

MainWindow::~MainWindow()
{
    disconnect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(textBrowserChanged()));

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    STORE_PTR(fSettings, ui->ckHiddenFiles, isChecked);
    STORE_PTR(fSettings, ui->ckSymbolicLinks, isChecked);
    STORE_PTR(fSettings, ui->ckSystemFiles, isChecked);
    STORE_PTR(fSettings, ui->ckFiles, isChecked);
    STORE_PTR(fSettings, ui->ckDirectories, isChecked);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFind);
    STORE_PTR(fSettings, ui->ckFindCaseSensitive, isChecked);
    STORE_PTR(fSettings, ui->ckFindRegEx, isChecked);
    STORE_PTR(fSettings, ui->ckFindWholeWord, isChecked);
    STORE_PTR(fSettings, ui->comboFindBox, currentIndex);
    fSettings.endGroup();

    Highlighter::Language::store(fSettings);

    fSettings.beginGroup(config::sGroupView);
    STORE_PTR(fSettings, ui->ckHideEmptyParent, isChecked);
    STORE_STR(fSettings,  Type::mShort);
    STORE_STR(fSettings, mFontName);
    auto fWindowGeometry = saveGeometry();
    STORE_STR(fSettings, fWindowGeometry);
    auto fWindowState = saveState();
    STORE_STR(fSettings, fWindowState);
    STORE_STR(fSettings, mDockedWidgetMinMaxButtons);
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
    STORE_STRF(fSettings, mMergeTools, Cmd::toStringMT);

    fSettings.beginWriteArray(config::sCommands);
    {
        int fIndex = 0;

        for (const auto& fItem : mActions.getList())
        {
            const Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(fItem.first);

            if (mActions.getFlags(fCmd) & ActionList::Flags::Modified)
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

#ifdef DOCKED_VIEWS
void MainWindow::createDockWindows()
{
    // use initialized widgets from forms user interface
    // and redirect user interface widgets to docked layout
    // - first remove the widgets from layout items
    // - add widgets
    //   - to central widget
    //   - to dock widgets
    //   - to toolbars
    // NOTE: reguard future items, if any

    // git files view as central widget
    ui->verticalLayoutForTreeView->removeWidget(ui->treeSource);
    setCentralWidget(ui->treeSource);

    QDockWidget* dock;
    // text browser
    dock = new QDockWidget(tr("Text Editor"), this);
    ui->comboFindBox->addItem(dock->windowTitle());
    ui->comboFindBox->addItem("Source View");
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("textbrowser");
    ui->horizontalLayoutHistoryAndText->removeWidget(ui->textBrowser);
    dock->setWidget(ui->textBrowser);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));

    QDockWidget *first_tab;
    // history tree
    dock = new QDockWidget(tr("History View"), this);
    ui->comboFindBox->addItem(dock->windowTitle());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("historyview");
    ui->verticalLayout->removeWidget(ui->treeHistory);
    dock->setWidget(ui->treeHistory);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    first_tab = dock;

    // branch tree
    dock = new QDockWidget(tr("Branch View"), this);
    ui->comboFindBox->addItem(dock->windowTitle());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("branchview");
    ui->verticalLayout->removeWidget(ui->treeBranches);
    dock->setWidget(ui->treeBranches);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);

    QLayoutItem *layoutItem {nullptr};
    QToolBar* pTB {nullptr};
    pTB = new QToolBar(tr("Git File View States"));
    pTB->setObjectName("fileviewtoolbar");
    while ((layoutItem = ui->horizontalLayoutForTreeViewHead->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::TopToolBarArea, pTB);

    pTB = new QToolBar(tr("Text Editor / Viewer Control"));
    pTB->setObjectName("textviewtoolbar");
    while ((layoutItem = ui->horizontalLayoutForTextBrowserHead->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::TopToolBarArea, pTB);

    pTB = new QToolBar(tr("File Flags"));
    pTB->setObjectName("fileflagtoolbar");
    while ((layoutItem = ui->horizontalLayoutFlags->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::BottomToolBarArea, pTB);

    pTB = new QToolBar(tr("Find"));
    pTB->setObjectName("findtoolbar");
    while ((layoutItem = ui->horizontalLayoutFind->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::BottomToolBarArea, pTB);

    // - remove obsolete layout
    // NOTE: reguard future layout items, if any
    delete ui->topLayout;
    ui->topLayout = nullptr;
    ui->horizontalLayout = nullptr;
    ui->horizontalLayoutFlags = nullptr;
    ui->horizontalLayoutFind = nullptr;
    ui->horizontalLayoutForTextBrowserHead = nullptr;
    ui->horizontalLayoutForTreeViewHead = nullptr;
    ui->horizontalLayoutHistoryAndText  = nullptr;
    ui->horizontalLayoutTool = nullptr;
    ui->verticalLayout = nullptr;
    ui->verticalLayoutForTextBrowser = nullptr;
    ui->verticalLayoutForTreeView = nullptr;
}

void MainWindow::showDockedWidget(QWidget* widget)
{
    QDockWidget* parent = dynamic_cast<QDockWidget*>(widget->parent());
    if (parent)
    {
        if (!parent->isVisible())
        {
            parent->setVisible(true);
        }
        parent->raise();
    }
}

void MainWindow::dockWidget_topLevelChanged(bool)
{
    QDockWidget* dw = dynamic_cast<QDockWidget*>(QObject::sender());
    if (dw && mDockedWidgetMinMaxButtons)
    {
        static Qt::WindowFlags oldDocWidgetFlags = 0;
        if (dw->isFloating())
        {
            auto customized = Qt::CustomizeWindowHint |
                              Qt::Window | Qt::WindowMinimizeButtonHint |
                              Qt::WindowMaximizeButtonHint |
                              Qt::WindowCloseButtonHint;

            dw->setWindowFlags(customized);

            auto old_flags = dw->windowFlags();
            old_flags &= ~customized;

            if (!oldDocWidgetFlags)
            {
                oldDocWidgetFlags = old_flags;
            }
            dw->show();
        }
        else
        {
            dw->setWindowFlags(oldDocWidgetFlags);
            dw->show();
        }
    }
}

#endif

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
            || aParentItem->checkState(Column::FileName) == Qt::PartiallyChecked
            || mCurrentTask == Work::InsertPathFromCommandString)
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
                    const QString fCmd = applyGitCommandToFilePath(fSource, mGitCommand, fResultStr);
                    apendTextToBrowser(fCmd + getLineFeed() + fResultStr + getLineFeed(), true);
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
                        const int fIndex = mGitCommand.indexOf(fSource);
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
                    case Work::ShowModified: case Work::ShowAllGitActions: case Work::ShowSelected:
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
                const QVariant fVariant = aParentItem->data(Column::State, Role::Filter);
                if (fVariant.isValid())
                {
                    const Type fType(fVariant.toUInt());
                    switch (mCurrentTask)
                    {
                        case Work::ApplyGitCommand:
                        {
                            fSource = "\"" + fSource + "\"";
                            QString fCmd = applyGitCommandToFilePath(fSource, mGitCommand, fResultStr);
                            apendTextToBrowser(fCmd + getLineFeed() + fResultStr, true);
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
                        case Work::ShowSelected:
                            fResult = aParentItem->isSelected();
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
            const QString fSourceDir("");
            fResult &= iterateTreeItems(aSourceTree, &fSourceDir, aSourceTree.topLevelItem(fIndex));
        }
    }
    return fResult;
}

bool MainWindow::iterateCheckItems(QTreeWidgetItem* aParentItem, stringt2typemap& aPathMap, const QString* aSourceDir)
{
    if (aParentItem)
    {
        const QString fSourcePath = aSourceDir ? *aSourceDir + "/" + aParentItem->text(Column::FileName) : aParentItem->text(Column::FileName);

        const auto fFoundType = aPathMap.find(fSourcePath.toStdString());
        if (fFoundType != aPathMap.end())
        {
            aParentItem->setCheckState(Column::FileName, Qt::Checked);
            const QString fState = fFoundType->second.getStates();
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

    apendTextToBrowser(tr("Repository: ")+fSourceDir.path(), aItem == 0 ? false : true);
    apendTextToBrowser(fResultString, true);
    apendTextToBrowser("", true);

    stringt2typemap fCheckMap;
    parseGitStatus(fSourceDir.path() +  QDir::separator(), fResultString, fCheckMap);

    insertItem(fSourceDir, *ui->treeSource);

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


void MainWindow::apendTextToBrowser(const QString& aText, bool append)
{
    if (!append)
    {
        on_btnCloseText_clicked();
    }
    mHighlighter->setExtension("");
    ui->textBrowser->insertPlainText(aText + getLineFeed());
    ui->textBrowser->textCursor().setPosition(QTextCursor::End);
#ifdef DOCKED_VIEWS
        showDockedWidget(ui->textBrowser);
#endif
}



QVariant MainWindow::handleWorker(int aWork, const QVariant& aData)
{
    QVariant work_result;
    Logger::printDebug(Logger::trace, "handleWorker(%d): %x", aWork, QThread::currentThreadId());
    switch(static_cast<Work::e>(aWork))
    {
    case Work::ApplyGitCommand:
        if (aData.isValid() && aData.type() == QVariant::String)
    {
        QString result_string;
        int result = execute(aData.toString().toStdString().c_str(), result_string, true);
        if (result == NoError)
        {
            work_result.setValue(result_string);
        }
     } break;
        default:
            break;
    }
    return work_result;
}

void MainWindow::handleMessage(int aMsg, QVariant aData)
{
    Logger::printDebug(Logger::trace, "handleMessage(%d): %x, %s", aMsg, QThread::currentThreadId(), aData.typeName());
    switch(static_cast<Work::e>(aMsg))
    {
        case Work::ApplyGitCommand:
            if (aData.isValid() && aData.type() == QVariant::String)
            {
                auto result_list = aData.toString().split("\n");
                for (auto&entry : result_list)
                {
                    if (entry.size() > 1) // text with two tabs marks an entry
                    {
                        if (entry[0] == '\t' && entry[1] == '\t')
                        {
                            entry = entry.trimmed();
                            int pos = entry.indexOf(".");
                            if (pos != -1)
                            {
                                entry = entry.left(pos);
                            }
                            mMergeTools[entry] = true;
                        }
                    }
                    if (entry.contains("not currently available"))
                    {
                        break;
                    }
                }
                for (auto entry = mMergeTools.begin(); entry != mMergeTools.end(); ++entry)
                {
                    ui->comboDiffTool->addItem(entry.key());
                }

            }
            break;
        default:  break;
    }
}

void MainWindow::parseGitStatus(const QString& fSource, const QString& aStatus, stringt2typemap& aFiles)
{
    const auto fLines = aStatus.split("\n");

    for (QString fLine : fLines)
    {
        const int     fStateSize = 2;
        const QString fState = fLine.left(fStateSize);
        const auto    fRelativePath = fLine.mid(fStateSize).trimmed();

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
    const QString fSourcePath = QFileDialog::getExistingDirectory(this, tr("Select SourceFiles"));
    if (fSourcePath.size() > 1)
    {
        insertSourceTree(initDir(fSourcePath), ui->treeSource->topLevelItemCount()+1);
    }
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

    ui->statusBar->showMessage(tr("Total selected bytes: ") + formatFileSize(fSize));
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
        const std::string fString = ui->textBrowser->toPlainText().toStdString();
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

    const QString fFileName = getItemFilePath(item);

    QFile file(fFileName);
    if (file.open(QIODevice::ReadOnly))
    {
        const QFileInfo file_info(fFileName);
        ui->labelFilePath->setText(fFileName);
        mHighlighter.reset(new Highlighter(ui->textBrowser->document()));
        mHighlighter->setExtension(file_info.suffix());
        ui->labelLanguage->setText("L: " + mHighlighter->currentLanguage());
        ui->textBrowser->setText(file.readAll());
        ui->btnStoreText->setEnabled(false);
#ifdef DOCKED_VIEWS
        showDockedWidget(ui->textBrowser);
#endif
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
        const QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
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
        case ComboShowItems::AllFiles:      mCurrentTask = Work::ShowAllFiles;      break;
        case ComboShowItems::AllGitActions: mCurrentTask = Work::ShowAllGitActions; break;
        case ComboShowItems::GitModified:   mCurrentTask = Work::ShowModified;      break;
        case ComboShowItems::GitAdded:      mCurrentTask = Work::ShowAdded;         break;
        case ComboShowItems::GitDeleted:    mCurrentTask = Work::ShowDeleted;       break;
        case ComboShowItems::GitUnknown:    mCurrentTask = Work::ShowUnknown;       break;
        case ComboShowItems::Gitstaged:     mCurrentTask = Work::ShowStaged;        break;
        case ComboShowItems::GitUnmerged:   mCurrentTask = Work::ShowUnMerged;      break;
        case ComboShowItems::GitSelected:   mCurrentTask = Work::ShowSelected;      break;
    }
    iterateTreeItems(*ui->treeSource);
    mCurrentTask = Work::None;
}

void MainWindow::on_treeSource_itemClicked(QTreeWidgetItem *item, int /* column */ )
{
    mContextMenuSourceTreeItem = item;
    const Type fType(static_cast<Type::TypeFlags>(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt()));
    mActions.enableItemsByType(Cmd::mContextMenuSourceTree, fType);
    if (mMergeDialog)
    {
        mMergeDialog->mGitFilePath = getItemFilePath(mContextMenuSourceTreeItem);
    }
}

void MainWindow::getSelectedTreeItem()
{
    if (! mContextMenuSourceTreeItem)
    {
        const auto fSelected = ui->treeSource->selectedItems();
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
    execute(fCommand, aResultStr);
    return fCommand;
}

void MainWindow::updateTreeItemStatus(QTreeWidgetItem * aItem)
{
    const QFileInfo fFileInfo(getItemFilePath(aItem));

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
        const QString fRepositoryPath = fParent.absolutePath();
        QString fCommandString = tr(Cmd::getCommand(Cmd::GetStatusAll).toStdString().c_str()).arg(fRepositoryPath);
        fCommandString.append(" ");
        fCommandString.append(fFileInfo.absoluteFilePath());

        QString fResultString;
        applyGitCommandToFilePath(fRepositoryPath, fCommandString, fResultString);

        stringt2typemap fCheckMap;

        parseGitStatus(fRepositoryPath + QDir::separator(), fResultString, fCheckMap);

        const QString fSourcePath = fFileInfo.absolutePath();
        iterateCheckItems(aItem, fCheckMap, &fSourcePath);
    }
}

#ifndef DOCKED_VIEWS
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
#endif

void MainWindow::on_treeHistory_itemClicked(QTreeWidgetItem *aItem, int aColumn)
{
    on_btnCloseText_clicked();
    mHighlighter->setExtension("");
    ui->textBrowser->setPlainText(ui->treeHistory->itemClicked(aItem, aColumn));
#ifdef DOCKED_VIEWS
    showDockedWidget(ui->textBrowser);
#endif
    const auto fItemData = ui->treeHistory->determineHistoryHashItems(ui->treeHistory->currentItem());
    if (fItemData.type() == QVariant::ModelIndex)
    {
        QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
        mContextMenuSourceTreeItem = fSourceHook->itemFromIndex(fItemData.toModelIndex());
    };

    mActions.enableItemsByType(Cmd::mContextMenuHistoryTree, Type::None);
    if (!ui->treeHistory->isSelectionDiffable())
    {
        mActions.getAction(Cmd::CallDiffTool)->setEnabled(false);
        mActions.getAction(Cmd::ShowDifference)->setEnabled(false);
    }
    if (!ui->treeHistory->isSelectionFileDiffable())
    {
        mActions.getAction(Cmd::CallDiffTool)->setEnabled(false);
    }
}

void MainWindow::initContextMenuActions()
{
    connect(mActions.createAction(Cmd::ShowDifference , tr("Show difference")   , Cmd::getCommand(Cmd::ShowDifference)) , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::ShowDifference)->setShortcut(QKeySequence(Qt::Key_F8));
    mActions.setStagedCmdAddOn(Cmd::ShowDifference, "--cached ");
    mActions.setFlags(Cmd::ShowDifference, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::ShowDifference, ActionList::Flags::History, Flag::set);

    connect(mActions.createAction(Cmd::CallDiffTool   , tr("Call diff tool...") , Cmd::getCommand(Cmd::CallDiffTool))   , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallDiffTool)->setShortcut(QKeySequence(Qt::Key_F9));
    mActions.setStagedCmdAddOn(Cmd::CallDiffTool, "--cached ");
    mActions.setFlags(Cmd::CallDiffTool, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::CallDiffTool, ActionList::Flags::History, Flag::set);
    mActions.setFlags(Cmd::CallDiffTool, ActionList::Flags::DiffOrMergeTool, Flag::set);

    connect(mActions.createAction(Cmd::CallMergeTool   , tr("Call merge tool...") , Cmd::getCommand(Cmd::CallMergeTool)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallMergeTool)->setShortcut(QKeySequence(Qt::Key_F7));
    mActions.setFlags(Cmd::CallMergeTool, Type::GitUnmerged, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::CallMergeTool, ActionList::Flags::DiffOrMergeTool, Flag::set);

    connect(mActions.createAction(Cmd::InvokeGitMergeDialog , tr("Merge file..."), tr("Merge selected file")) , SIGNAL(triggered()), this, SLOT(invoke_git_merge_dialog()));
    connect(mActions.createAction(Cmd::InvokeHighlighterDialog, tr("Edit Highlighting..."), tr("Edit highlighting color and font")) , SIGNAL(triggered()), this, SLOT(invoke_highlighter_dialog()));

    connect(mActions.createAction(Cmd::ShowStatus      , tr("Show status")       , Cmd::getCommand(Cmd::ShowStatus))     , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowStatus, Cmd::UpdateItemStatus);
    connect(mActions.createAction(Cmd::ShowShortStatus , tr("Show short status") , Cmd::getCommand(Cmd::ShowShortStatus)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowShortStatus, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::Add             , tr("Add to git (stage)"), Cmd::getCommand(Cmd::Add))            , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
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
    mActions.setCustomCommandMessageBoxText(Cmd::Restore, tr("Restore changes;Do you want to restore changes in file \"%1\"?"));
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
    mActions.setCustomCommandMessageBoxText(Cmd::BranchDelete, tr("Delete %1 from git;Do you want to delete \"%1\"?"));
    mActions.setCustomCommandPostAction(Cmd::BranchDelete, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::BranchCheckout, tr("Checkout Branch"), Cmd::getCommand(Cmd::BranchCheckout)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchCheckout, tr("Checkout %1;Do you want to set \"%1\" active?"));
    mActions.setCustomCommandPostAction(Cmd::BranchCheckout, Cmd::UpdateItemStatus);

    connect(mActions.createAction(Cmd::BranchHistory, tr("History Branch"), Cmd::getCommand(Cmd::BranchHistory)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchHistory, Cmd::ParseHistoryText);
    connect(mActions.createAction(Cmd::BranchShow, tr("Show Branch"), Cmd::getCommand(Cmd::BranchShow)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));

    connect(mActions.createAction(Cmd::MoveOrRename   , tr("Move / Rename..."), Cmd::getCommand(Cmd::MoveOrRename)), SIGNAL(triggered()), this, SLOT(call_git_move_rename()));
    mActions.getAction(Cmd::MoveOrRename)->setShortcut(QKeySequence(Qt::Key_F2));

    connect(mActions.createAction(Cmd::ExpandTreeItems      , tr("Expand Tree Items"), tr("Expands all tree item of focused tree")) , SIGNAL(triggered()), this, SLOT(expand_tree_items()));
    connect(mActions.createAction(Cmd::CollapseTreeItems    , tr("Collapse Tree Items"), tr("Collapses all tree item of focused tree")), SIGNAL(triggered()), this, SLOT(collapse_tree_items()));

    connect(mActions.createAction(Cmd::AddGitSourceFolder   , tr("Add git source folder"), tr("Adds a git source folder to the source treeview")) , SIGNAL(triggered()), this, SLOT(addGitSourceFolder()));
    connect(mActions.createAction(Cmd::UpdateGitStatus      , tr("Update git status"), tr("Updates the git status of the selected source folder")), SIGNAL(triggered()), this, SLOT(updateGitStatus()));

#ifndef DOCKED_VIEWS
    connect(mActions.createAction(Cmd::ShowHideTree  , tr("Show/Hide tree"), tr("Shows or hides history or branches tree")) , SIGNAL(toggled(bool)), this, SLOT(showOrHideTrees(bool)));
    mActions.getAction(Cmd::ShowHideTree)->setCheckable(true);
#endif
    connect(mActions.createAction(Cmd::ClearTreeItems       , tr("Clear tree items"), tr("Clears all history or branch tree entries")), SIGNAL(triggered()), this, SLOT(clearTrees()));

    connect(mActions.createAction(Cmd::CustomGitActionSettings, tr("Customize git actions..."), tr("Edit custom git actions, menues and toolbars")), SIGNAL(triggered()), this, SLOT(performCustomGitActionSettings()));
    connect(mActions.createAction(Cmd::InsertHashFileNames  , tr("Insert File Name List"), tr("Inserts file names that differ from previous hash")), SIGNAL(triggered()), ui->treeHistory, SLOT(insertFileNames()));
    connect(mActions.createAction(Cmd::About, tr("About..."), tr("Information about GitView")), SIGNAL(triggered()), this, SLOT(gitview_about()));

    connect(mActions.createAction(Cmd::Delete, tr("Delete..."), tr("Delete file or folder")), SIGNAL(triggered()), this, SLOT(deleteFileOrFolder()));
    connect(mActions.createAction(Cmd::SelectTextBrowserLanguage, tr("Select Language..."), tr("Select language for text highlighting")), SIGNAL(triggered()), this, SLOT(selectTextBrowserLanguage()));


    for (const auto& fAction : mActions.getList())
    {
        mActions.setFlags(static_cast<Cmd::eCmd>(fAction.first), ActionList::Flags::BuiltIn);
    }
}

void MainWindow::initCustomAction(QAction* fAction)
{
    connect(fAction, SIGNAL(triggered()), this, SLOT(perform_custom_command()));
}

void MainWindow::initMergeTools()
{
    if (mMergeTools.size())
    {
        if (ui->comboDiffTool->count() > 1)
        {
            QString first_entry = ui->comboDiffTool->itemText(0);
            ui->comboDiffTool->clear();
            ui->comboDiffTool->addItem(first_entry);
        }
        for (auto item=mMergeTools.begin(); item != mMergeTools.end(); ++item)
        {
            if (item.value())
            {
                ui->comboDiffTool->addItem(item.key());
            }
        }
    }
    else if (ui->treeSource->topLevelItemCount())
    {
        QString first_git_repo =ui->treeSource->topLevelItem(0)->text(Column::FileName);
        mWorker.doWork(Work::ApplyGitCommand, QVariant(tr("git -C %1 difftool --tool-help").arg(first_git_repo)));
    }
}

void MainWindow::on_treeHistory_customContextMenuRequested(const QPoint &pos)
{
    const QVariant fItemData = ui->treeHistory->customContextMenuRequested(pos);
    auto * old_item = mContextMenuSourceTreeItem;
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
                fPostActionGroup.addAction(tr("Enable all"));
                fPostActionGroup.addAction(tr("Disable all"));
            }   break;
            default:
                break;
        }
    }

    QMenu menu(this);
    QMenu*fAuthorsMenu = nullptr;
    if (fPostActionGroup.actions().size())
    {
        fAuthorsMenu = menu.addMenu(tr("Authors"));
        fAuthorsMenu->addActions(fPostActionGroup.actions());
    }

    mActions.fillContextMenue(menu, Cmd::mContextMenuHistoryTree);
    QAction* fAction = menu.exec(ui->treeHistory->mapToGlobal(pos));
    if (fAction && fAuthorsMenu)
    {
        int fIndex = fAuthorsMenu->actions().indexOf(fAction);
        ui->treeHistory->checkAuthorsIndex(fIndex, fAction->isChecked());
    }

    mContextMenuSourceTreeItem = old_item;
}

void MainWindow::clearTrees()
{
    QTreeWidget * tree = focusedTreeWidget(false);
    if (tree)
    {
        tree->clear();
    }
}

void  MainWindow::call_git_commit()
{
    CommitMessage fCommitMsg;

    if (fCommitMsg.exec() == QDialog::Accepted)
    {
        on_btnCloseText_clicked();
        const QString fMessageText = fCommitMsg.getMessageText();
        const std::string  fCommand  = Cmd::getCommand(Cmd::Commit).toStdString();
        const QString fCommitCommand = tr(fCommand.c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem)).arg(fMessageText);
        if (fCommitMsg.getAutoStage())
        {
            getSelectedTreeItem();
            applyGitCommandToFileTree(Cmd::getCommand(Cmd::Add));
            QString fResultStr;
            execute(fCommitCommand, fResultStr);
            apendTextToBrowser(fCommitCommand + getLineFeed() + fResultStr);
            updateTreeItemStatus(mContextMenuSourceTreeItem);
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
        bool      fOk {false};
        const Type fType(static_cast<Type::TypeFlags>(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt()));
        const QString   fFileTypeName = Type::name(static_cast<Type::TypeFlags>(Type::FileType&fType.mType));
        const QFileInfo fPath(getItemFilePath(mContextMenuSourceTreeItem));

        QString       fOldName = mContextMenuSourceTreeItem->text(Column::FileName);
        const QString fNewName = QInputDialog::getText(this,
                       tr("Move or rename %1").arg(fFileTypeName),
                       tr("Enter a new name or destination for \"./%1\".\n"
                          "To move the %3 insert the destination path before.\n"
                          "To rename just modify the name.").arg(fPath.filePath()).arg(fFileTypeName),
                       QLineEdit::Normal, fOldName, &fOk);

        if (fOk && !fNewName.isEmpty())
        {
            const std::string fFormatCmd = Cmd::getCommand(Cmd::MoveOrRename).toStdString().c_str();
            const QString fNewGitName = "\"" + fNewName + "\"";
            QString     fCommand;
            bool        fMoved = false;
            if (fNewGitName.contains("/"))
            {
                fMoved   = true;
                fOldName = "\"" + fPath.filePath() + "\"";
                fCommand = tr(fFormatCmd.c_str()).arg(getItemTopDirPath(mContextMenuSourceTreeItem)).arg(fOldName).arg(fNewGitName);
            }
            else
            {
                fOldName = "\"" + fOldName + "\"";
                fCommand  = tr(fFormatCmd.c_str()).arg(fPath.absolutePath()).arg(fOldName).arg(fNewGitName);
            }

            QString fResultStr;
            int fResult = execute(fCommand, fResultStr);
            if (fResult == NoError)
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
    const QAction     *fAction       = qobject_cast<QAction *>(sender());
    const QVariantList fVariantList  = fAction->data().toList();
    const uint         command_flags = fVariantList[ActionList::Data::Flags].toUInt();

    QString fGitCommand = fAction->statusTip();

    if      (command_flags & ActionList::Flags::History && ui->treeHistory->hasFocus())
    {
        getSelectedTreeItem();
        call_git_history_diff_command();
    }
    else if (command_flags & ActionList::Flags::Branch)
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
            QString fOption;
            fType.mType = mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt();

            if (fStagedCmdAddOn.size() && fType.is(Type::GitStaged))
            {
                fOption += fStagedCmdAddOn;
            }
            if (command_flags & ActionList::Flags::DiffOrMergeTool && ui->comboDiffTool->currentIndex() != 0)
            {
                fOption += "--tool=";
                fOption += ui->comboDiffTool->currentText();
            }

            fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fOption).arg("%1");

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
        else
        {
            QString fOption;
            fGitCommand = tr(fGitCommand.toStdString().c_str()).arg(fOption).arg("");
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
#ifdef DOCKED_VIEWS
                showDockedWidget(ui->treeHistory);
#else
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
#endif
            }   break;
        }
    }
}

void MainWindow::call_git_history_diff_command()
{
    const QString &fHistoryHashItems = ui->treeHistory->getSelectedHistoryHashItems();
    const QString &fHistoryFile      = ui->treeHistory->getSelectedHistoryFile();
    const QAction *fAction           = qobject_cast<QAction *>(sender());
    const Type    fType(ui->treeHistory->getSelectedTopLevelType());

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
            const QString fQuotedHistoryFile = "\"" + fHistoryFile + "\"";
            fCmd = tr(fCmd.toStdString().c_str()).arg(fQuotedHistoryFile);
            QString fResult;
            execute(fCmd, fResult);
            apendTextToBrowser(fCmd + getLineFeed() + fResult);
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
        execute(fCmd, fResult);
        apendTextToBrowser(fCmd + getLineFeed() + fResult);
    }
}

void MainWindow::call_git_branch_command()
{
    const QAction*fAction            = qobject_cast<QAction *>(sender());
    const QVariantList  fVariantList = fAction->data().toList();
    const QString fMessageBoxText    = fVariantList[ActionList::Data::MsgBoxText].toString();
    const QString fBranchItem        = ui->treeBranches->getSelectedBranch();
    const QString fBranchGitRootPath = ui->treeBranches->getSelectedBranchGitRootPath();
    QString       fGitCommand        = fAction->statusTip();
    QString       fTopItemPath       = getItemTopDirPath(mContextMenuSourceTreeItem);

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

        execute(fGitCommand, fResultStr);
        fResultStr = fGitCommand + getLineFeed() + fResultStr;
        apendTextToBrowser(fResultStr);
    }

    if (fResult == 0)
    {
        switch (fVariantList[ActionList::Data::Action].toUInt())
        {
            case Cmd::UpdateItemStatus:
            {
                fResultStr.clear();
                fGitCommand  = ui->treeBranches->getBranchTopItemText();
                fTopItemPath = ui->treeBranches->getSelectedBranchGitRootPath();
                execute(fGitCommand, fResultStr);
                QString fParseText = fGitCommand + getLineFeed() + fResultStr + getLineFeed();
                ui->treeBranches->parseBranchListText(fParseText, fTopItemPath);
                mHighlighter->setExtension("");
                ui->textBrowser->setPlainText(fParseText);
#ifdef DOCKED_VIEWS
                showDockedWidget(ui->treeBranches);
#else
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
#endif
            }   break;
            case Cmd::ParseBranchListText:
            {
                ui->treeBranches->parseBranchListText(fResultStr, fTopItemPath);
                mHighlighter->setExtension("");
                ui->textBrowser->setPlainText(fGitCommand);
#ifdef DOCKED_VIEWS
                showDockedWidget(ui->treeBranches);
#else
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
#endif
            }    break;
            case Cmd::ParseHistoryText:
            {
                auto fItems = ui->treeSource->findItems(fBranchGitRootPath, Qt::MatchExactly);
                QTreeWidgetHook*fSourceHook = reinterpret_cast<QTreeWidgetHook*>(ui->treeSource);
                ui->treeHistory->parseGitLogHistoryText(fResultStr, fItems.size() ? QVariant(fSourceHook->indexFromItem(fItems[0])) : QVariant(fBranchGitRootPath), fBranchItem, Type::Branch);
                mHighlighter->setExtension("");
                ui->textBrowser->setPlainText(fGitCommand);
#ifdef DOCKED_VIEWS
                showDockedWidget(ui->treeHistory);
#else
                mActions.getAction(Cmd::ShowHideTree)->setChecked(true);
#endif
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
    CustomGitActions fCustomGitActions(mActions, mMergeTools);
    connect(&fCustomGitActions, SIGNAL(initCustomAction(QAction*)), this, SLOT(initCustomAction(QAction*)));
    if (fCustomGitActions.exec() == QDialog::Accepted)
    {
        if (fCustomGitActions.isMergeToolsChanged())
        {
            initMergeTools();
        }
    }
}

void MainWindow::invoke_git_merge_dialog()
{
    if (!mMergeDialog)
    {
        mMergeDialog.reset(new MergeDialog(this));
        connect(mMergeDialog->getAction(), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    }

    mMergeDialog->mGitFilePath = getItemFilePath(mContextMenuSourceTreeItem);
    mMergeDialog->show();
}

void MainWindow::invoke_highlighter_dialog()
{
    Highlighter::Language::invokeHighlighterDlg();
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

void MainWindow::deleteFileOrFolder()
{
    if (mContextMenuSourceTreeItem)
    {
        const QString fTopItemPath  = getItemTopDirPath(mContextMenuSourceTreeItem);
        const QString fItemPath     = getItemFilePath(mContextMenuSourceTreeItem);
        const Type fType(mContextMenuSourceTreeItem->data(Column::State, Role::Filter).toUInt());
        if (callMessageBox(tr("Delete %1"), fType.type_name(), fItemPath) == QMessageBox::Yes)
        {
            const bool result = QFile::remove(fTopItemPath + "/" + fItemPath);
            if (result)
            {
                if (fType.is(Type::AllGitActions) && !fType.is(Type::GitUnTracked))
                {
                    updateTreeItemStatus(mContextMenuSourceTreeItem);
                }
                else
                {
                    QTreeWidgetItem *parent = mContextMenuSourceTreeItem->parent();
                    if (parent)
                    {
                        parent->removeChild(mContextMenuSourceTreeItem);
                    }
                    mContextMenuSourceTreeItem = 0;
                }
            }
        }
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
        ui->btnStoreText->setEnabled(enabled);
    }
}

void MainWindow::on_btnFindNext_clicked()
{
    find_function(true);
}

void MainWindow::on_btnFindPrevious_clicked()
{
    find_function(false);
}

void MainWindow::find_function(bool forward)
{
    if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::Text))
    {
        Qt::CaseSensitivity reg_ex_case = Qt::CaseInsensitive;
        int                 find_flag   = forward ? 0 : QTextDocument::FindBackward;

        if (ui->ckFindCaseSensitive->isChecked())
        {
            reg_ex_case = Qt::CaseSensitive;
            find_flag |= QTextDocument::FindCaseSensitively;
        }
        if (ui->ckFindWholeWord->isChecked())
        {
            find_flag |= QTextDocument::FindWholeWords;
        }

        bool found_text = false;
        if (ui->ckFindRegEx->isChecked())
        {
            found_text = ui->textBrowser->find(QRegExp(ui->edtFindText->text(), reg_ex_case), static_cast<QTextDocument::FindFlag>(find_flag));
        }
        else
        {
            found_text = ui->textBrowser->find(ui->edtFindText->text(), static_cast<QTextDocument::FindFlag>(find_flag));
        }
        if (found_text)
        {
            showDockedWidget(ui->textBrowser);
        }
    }
    else
    {
        QTreeWidget *tree_view {nullptr};
        switch (static_cast<FindView>(ui->comboFindBox->currentIndex()))
        {
            case FindView::Source:  tree_view = ui->treeSource; break;
            case FindView::History: tree_view = ui->treeHistory; break;
            case FindView::Branch:  tree_view = ui->treeBranches; break;
            case FindView::Text: break;
        }

        if (tree_view)
        {
            const auto& text_to_find    = ui->edtFindText->text();
            int  tree_match_flag = Qt::MatchExactly;

            if (!ui->ckFindWholeWord->isChecked())
            {
                tree_match_flag = Qt::MatchContains;
            }
            if (ui->ckFindRegEx->isChecked())
            {
                tree_match_flag = Qt::MatchRegExp;
            }
            else if (text_to_find.contains('*') || text_to_find.contains('?'))
            {
                tree_match_flag = Qt::MatchWildcard;
            }

            if (ui->ckFindCaseSensitive->isChecked())
            {
                tree_match_flag |= Qt::MatchCaseSensitive;
            }
            tree_match_flag |= Qt::MatchRecursive;

            //! NOTE: also possible flags
            //  MatchStartsWith = 2,
            //  MatchEndsWith = 3,
            //  MatchFixedString = 8,
            //  MatchWrap = 32,

            const auto found_items = tree_view->findItems(text_to_find, static_cast<Qt::MatchFlag>(tree_match_flag));
            if (found_items.size())
            {
                for (auto item : found_items)
                {
                    tree_view->setItemSelected(item, true);
                    auto* parent = item->parent();
                    while (parent)
                    {
                        tree_view->setItemExpanded(parent, true);
                        parent = parent->parent();
                    }
                }
                showDockedWidget(tree_view);
            }
        }
    }
}



void MainWindow::on_treeSource_currentItemChanged(QTreeWidgetItem * /* current */, QTreeWidgetItem *previous)
{
    if (   mContextMenuSourceTreeItem
        && mContextMenuSourceTreeItem == previous)
    {
        mContextMenuSourceTreeItem = 0;
    }
}
