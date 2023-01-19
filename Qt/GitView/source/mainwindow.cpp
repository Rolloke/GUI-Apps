#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"
#include "logger.h"
#include "history.h"
#include "qbranchtreewidget.h"
#include "customgitactions.h"
#include "aboutdlg.h"
#include "mergedialog.h"
#include "binary_values_view.h"

#include <QDateTime>
#include <QAction>
#include <QMessageBox>
#include <QSettings>
#include <QKeyEvent>
#include <QSysInfo>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QStyleFactory>
#include <QPalette>
#include <QTextStream>
#include <QInputDialog>
#include <QStaticText>
#include <QTextCodec>
#include <QWhatsThis>

#ifdef WEB_ENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#endif

#include <boost/bind.hpp>

#define RELATIVE_GIT_PATH 1

/// TODO: Beliebige Datei öffnen mit Basispfad aus Src-Treeview und Eintrag in Src-TreeView

using namespace std;
using namespace git;

namespace config
{
constexpr char sGroupFilter[] = "Filter";
constexpr char sGroupView[] = "View";
constexpr char sGroupPaths[] = "Paths";
constexpr char sSourcePath[] = "Source";
constexpr char sUncheckedPath[] = "Unchecked";
constexpr char sGroupLogging[] = "Logging";
constexpr char sGroupGitCommands[] = "GitCommands";
constexpr char sGroupFind[] = "Find";
constexpr char sCommands[] = "Commands";
constexpr char sCommand[] = "Command";
constexpr char sID[] = "ID";
constexpr char sName[] = "Name";
constexpr char sCustomMessageBoxText[] = "MessageBoxText";
constexpr char sCustomCommandPostAction[] = "PostAction";
constexpr char sFlags[] = "Flags";
constexpr char sFlagsEnabled[] = "FlagsEnabled";
constexpr char sFlagsDisabled[] = "FlagsDisabled";
constexpr char sIconPath[] = "IconPath";
constexpr char sMenuStringList[] = "MenuStringList";
constexpr char sShortcut[] = "Shortcut";
constexpr char sModified[] = "Modified";
} // namespace config


MainWindow::MainWindow(const QString& aConfigName, QWidget *parent)
    : QMainWindow(parent)
    , mDockedWidgetMinMaxButtons(true)
    , ui(new Ui::MainWindow)
    , mWorker(this)
    , mWorkerAction(nullptr)
    , mCurrentTask(Work::None)
    , mActions(this)
    , mConfigFileName(aConfigName)
    , mContextMenuSourceTreeItem(nullptr)
    , mFontName("Courier")
    , mFileCopyMimeType("x-special/mate-copied-files")
    , mStylePath( "/opt/tools/git_view/style.qss")
    #ifdef __linux__
    , mExternalFileOpenCmd("xdg-open")
    #else
    , mExternalFileOpenCmd("")
    #endif
    , mFindGrep("grep")
    , mFindFsrc("fsrc")
    , mWarnOpenFileSize(1024*1024) // 1MB
{
    static const QString style_sheet_treeview_lines =
            "QTreeView::branch:has-siblings:!adjoins-item {"
            "    border-image: url(:/resource/24X24/stylesheet-vline.png) 0; }"
            "QTreeView::branch:has-siblings:adjoins-item {"
            "    border-image: url(:/resource/24X24/stylesheet-branch-more.png) 0; }"
            "QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
            "    border-image: url(:/resource/24X24/stylesheet-branch-end.png) 0; }"
            "QTreeView::branch:has-children:!has-siblings:closed,"
            "QTreeView::branch:closed:has-children:has-siblings { "
            "    border-image: none; image: url(:/resource/24X24/stylesheet-branch-closed.png); }"
            "QTreeView::branch:open:has-children:!has-siblings,"
            "QTreeView::branch:open:has-children:has-siblings  { "
            "    border-image: none; image: url(:/resource/24X24/stylesheet-branch-open.png); }";

    ui->setupUi(this);
#ifdef DOCKED_VIEWS
    createDockWindows();
#endif

    setWindowIcon(QIcon(":/resource/logo@2x.png"));

    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleWorker, this, _1, _2));
    QObject::connect(this, SIGNAL(doWork(int,QVariant)), &mWorker, SLOT(doWork(int,QVariant)));
    mWorker.setMessageFunction(boost::bind(&MainWindow::handleMessage, this, _1, _2));
    connect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(textBrowserChanged()));

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    Highlighter::Language::load(fSettings);
    ui->textBrowser->reset();
    connect(ui->textBrowser, SIGNAL(updateExtension(QString)), this, SLOT(updateSelectedLanguage(QString)));
    connect(ui->textBrowser, SIGNAL(show_web_view(bool)), this, SLOT(show_web_view(bool)));
    ui->treeSource->header()->setSortIndicator(QSourceTreeWidget::Column::FileName, Qt::AscendingOrder);
    ui->treeSource->header()->setSectionResizeMode(QSourceTreeWidget::Column::FileName, QHeaderView::Stretch);
    ui->treeSource->header()->setSectionResizeMode(QSourceTreeWidget::Column::DateTime, QHeaderView::ResizeToContents);
    ui->treeSource->header()->setSectionResizeMode(QSourceTreeWidget::Column::Size    , QHeaderView::ResizeToContents);
    ui->treeSource->header()->setSectionResizeMode(QSourceTreeWidget::Column::State   , QHeaderView::ResizeToContents);
    ui->treeSource->header()->setStretchLastSection(false);

    ui->treeSource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeSource->setDragEnabled(true);
    ui->treeSource->viewport()->setAcceptDrops(true);
    ui->treeSource->setDropIndicatorShown(true);
    ui->treeSource->setDragDropMode(QAbstractItemView::InternalMove);
    connect(ui->treeSource, SIGNAL(dropped_to_target(QTreeWidgetItem*,bool*)), this, SLOT(call_git_move_rename(QTreeWidgetItem*,bool*)));

    ui->treeFindText->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeFindText->header()->setSectionResizeMode(FindColumn::FilePath, QHeaderView::ResizeToContents);
    ui->treeFindText->header()->setSectionResizeMode(FindColumn::Line, QHeaderView::ResizeToContents);
    ui->treeFindText->header()->setSectionResizeMode(FindColumn::FoundTextLine, QHeaderView::Stretch);

    ui->treeSource->setStyleSheet(style_sheet_treeview_lines);
    ui->treeHistory->setStyleSheet(style_sheet_treeview_lines);
    ui->treeBranches->setStyleSheet(style_sheet_treeview_lines);
    ui->treeStash->setStyleSheet(style_sheet_treeview_lines);
    ui->treeFindText->setStyleSheet(style_sheet_treeview_lines);
    ui->textBrowser->set_actions(&mActions);

    connect(ui->treeStash, SIGNAL(find_item(QString,QString)), ui->treeSource, SLOT(find_item(QString,QString)));
    connect(ui->ckShowLineNumbers, SIGNAL(toggled(bool)), ui->textBrowser, SLOT(set_show_line_numbers(bool)));
    connect(ui->treeHistory, SIGNAL(send_history(QStringList)), ui->graphicsView, SLOT(insert_history(QStringList)));
    connect(ui->treeHistory, SIGNAL(reset_history()), ui->graphicsView, SLOT(clear()));

    /// add status labels
    m_status_line_label = new QLabel("");
    m_status_line_label->setToolTip("Line");
    ui->statusBar->addPermanentWidget(m_status_line_label);
    connect(ui->textBrowser, SIGNAL(line_changed(int)), m_status_line_label, SLOT(setNum(int)));
    connect(ui->tableBinaryView, &qbinarytableview::cursor_changed, [&] (int) { m_status_line_label->setText(""); });

    m_status_column_label = new QLabel("");
    m_status_column_label->setToolTip("Column/Position");
    ui->statusBar->addPermanentWidget(m_status_column_label);
    connect(ui->textBrowser, SIGNAL(column_changed(int)), m_status_column_label, SLOT(setNum(int)));
    connect(ui->tableBinaryView, SIGNAL(cursor_changed(int)), m_status_column_label, SLOT(setNum(int)));

    initCodecCombo();

    fSettings.beginGroup(config::sGroupFilter);
    {
        LOAD_PTR(fSettings, ui->ckHiddenFiles, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckSymbolicLinks, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckSystemFiles, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFiles, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckDirectories, setChecked, isChecked, toBool);
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFind);
    {
        LOAD_PTR(fSettings, ui->ckSearchResultsAsSearchTree, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFastFileSearch, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFindCaseSensitive, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFindRegEx, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFindWholeWord, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->comboFindBox, setCurrentIndex, currentIndex, toInt);
        LOAD_STR(fSettings, mFindGrep, toString);
        LOAD_STR(fSettings, mFindFsrc, toString);
    }
    fSettings.endGroup();
    on_comboFindBox_currentIndexChanged(ui->comboFindBox->currentIndex());

    fSettings.beginGroup(config::sGroupLogging);
    {
        QString fSeverity;
        LOAD_STR(fSettings, fSeverity, toString);
        uint32_t fSeverityValue = fSeverity.toLong(0, 2);
        Logger::setSeverity(0xffff, false);
        Logger::setSeverity(fSeverityValue, true);
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupGitCommands);
    {
        LOAD_STRF(fSettings, Cmd::mContextMenuSourceTree, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mContextMenuEmptySourceTree, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mContextMenuHistoryTree, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mContextMenuBranchTree, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mContextMenuStashTree, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mContextMenuTextView, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mContextMenuFindTextTree, Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mToolbars[0], Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, Cmd::mToolbars[1], Cmd::fromString, Cmd::toString, toString);
        LOAD_STRF(fSettings, mMergeTools, Cmd::fromStringMT, Cmd::toStringMT, toString);

        initContextMenuActions();
        mActions.initActionIcons();

        int fItemCount = fSettings.beginReadArray(config::sCommands);
        {
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
                QStringList string_list = fSettings.value(config::sMenuStringList).toStringList();
                if (string_list.size() > 1) mActions.setMenuStringList(fCmd, string_list);
                mActions.setCustomCommandMessageBoxText(fCmd, fSettings.value(config::sCustomMessageBoxText).toString());
                mActions.setCustomCommandPostAction(fCmd, fSettings.value(config::sCustomCommandPostAction).toUInt());
            }
        }
        fSettings.endArray();
    }
    fSettings.endGroup();


#ifdef DOCKED_VIEWS
    for (uint i=0; i < Cmd::mToolbars.size(); ++i)
    {
        const auto& fToolbar = Cmd::mToolbars[i];
        QToolBar*pTB = new QToolBar(Cmd::mToolbarNames[i]);
        mToolBars.push_back(pTB);
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
    {
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
            ui->tableBinaryView->setFont(font);
        }
        {
            bool fDifferentEndian = CDisplayType::getDifferentEndian();
            LOAD_STR(fSettings, fDifferentEndian, toBool);
            CDisplayType::setDifferentEndian(fDifferentEndian);
            int fBinaryDisplayType = ui->tableBinaryView->get_type();
            LOAD_STR(fSettings, fBinaryDisplayType, toInt);
            ui->tableBinaryView->set_type(fBinaryDisplayType);
            int fBinaryDisplayColumns = ui->tableBinaryView->get_columns();
            LOAD_STR(fSettings, fBinaryDisplayColumns, toInt);
            ui->tableBinaryView->set_columns(fBinaryDisplayColumns);
        }

        LOAD_STR(fSettings, mWarnOpenFileSize, toULongLong);
        LOAD_STR(fSettings, mFileCopyMimeType, toString);
        LOAD_STR(fSettings, mExternalIconFiles, toString);
        LOAD_STR(fSettings, mExternalFileOpenCmd, toString);
        QString fTypeFormatFilesLocation;
        LOAD_STR(fSettings, fTypeFormatFilesLocation, toString);
        mBinaryValuesView->m_type_format_files_location = fTypeFormatFilesLocation;
        QString fExternalFileOpenExt;
        LOAD_STR(fSettings, fExternalFileOpenExt, toString);
        if (fExternalFileOpenExt.size())
        {
            auto extensions = fExternalFileOpenExt.split(",");
            for (const auto&extension : extensions)
            {
                auto parts = extension.split(":");
                mExternalFileOpenExt.insert(parts[0], parts.size() > 1 ? parts[1] : "");
            }
        }

        QStringList keys = QStyleFactory::keys();
        ui->comboAppStyle->addItems(keys);
        QString style = QApplication::style()->objectName();
        auto index = std::find_if(keys.begin(), keys.end(), [style](const QString& key) { return key.compare(style, Qt::CaseInsensitive) == 0;} );
        if (index != keys.end())
        {
            ui->comboAppStyle->setCurrentIndex(std::distance(keys.begin(), index));
        }

        LOAD_STR(fSettings, mStylePath, toString);
        bool fUseSourceTreeCheckboxes = ui->treeSource->mUseSourceTreeCheckboxes;
        LOAD_STR(fSettings, fUseSourceTreeCheckboxes, toBool);
        ui->treeSource->mUseSourceTreeCheckboxes = fUseSourceTreeCheckboxes;
        LOAD_PTR(fSettings, ui->ckExperimental, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->comboTabPosition, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->ckShowLineNumbers, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckRenderGraphicFile, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckShowHistoryGraphically, setChecked, isChecked, toBool);
        on_ckShowHistoryGraphically_clicked(ui->ckShowHistoryGraphically->isChecked());
        LOAD_PTR(fSettings, ui->comboToolBarStyle, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->comboAppStyle, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->comboUserStyle, setCurrentIndex, currentIndex, toInt);

        setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(ui->comboToolBarStyle->currentIndex()));
        on_comboAppStyle_currentTextChanged(ui->comboAppStyle->currentText());
        on_comboUserStyle_currentIndexChanged(ui->comboUserStyle->currentIndex());

        auto fTextTabStopWidth = ui->textBrowser->tabStopWidth();
        LOAD_STR(fSettings, fTextTabStopWidth, toInt);
        ui->textBrowser->setTabStopWidth(fTextTabStopWidth);
        ui->spinTabulator->setValue(fTextTabStopWidth);


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
        showDockedWidget(ui->textBrowser);
        on_comboTabPosition_currentIndexChanged(ui->comboTabPosition->currentIndex());
#endif
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);
    {
        int fItemCount = fSettings.beginReadArray(config::sSourcePath);
        {
            for (int fItem = 0; fItem < fItemCount; ++fItem)
            {
                fSettings.setArrayIndex(fItem);
                const QDir fSourceDir = initDir(fSettings.value(config::sSourcePath).toString(), fSettings.value(QString(config::sSourcePath)+"/"+config::sGroupFilter).toInt());
                insertSourceTree(fSourceDir, fItem);
            }
            ui->labelFilePath->setText("");
        }
        fSettings.endArray();
    }
    fSettings.endGroup();

    initMergeTools(true);

    auto text2browser = [this](const string&text){ appendTextToBrowser(text.c_str(), true); };
    Logger::setTextToBrowserFunction(text2browser);

    mBinaryValuesView->set_table_type(ui->tableBinaryView->get_type());
    mBinaryValuesView->set_table_columns(ui->tableBinaryView->get_columns());
    mBinaryValuesView->set_table_offset(ui->tableBinaryView->get_offset());

    connect(ui->tableBinaryView, SIGNAL(offset_changed(int)), mBinaryValuesView.data(), SLOT(set_table_offset(int)));
    connect(ui->tableBinaryView, SIGNAL(endian_changed()), mBinaryValuesView.data(), SLOT(notify_endian_changed()));
    connect(mBinaryValuesView.data(), SIGNAL(change_table_type(int)), ui->tableBinaryView, SLOT(set_type(int)));
    connect(mBinaryValuesView.data(), SIGNAL(change_table_columns(int)), ui->tableBinaryView, SLOT(set_columns(int)));
    connect(mBinaryValuesView.data(), SIGNAL(change_table_offset(int)), ui->tableBinaryView, SLOT(set_offset(int)));
    connect(mBinaryValuesView.data(), SIGNAL(status_message(QString,int)), ui->statusBar, SLOT(showMessage(QString,int)));
    connect(mBinaryValuesView.data(), SIGNAL(open_binary_format_file(QString,bool&)), ui->tableBinaryView, SLOT(open_binary_format_file(QString,bool&)));
    connect(mBinaryValuesView.data(), SIGNAL(endian_changed()), ui->tableBinaryView, SLOT(update_table()));

    connect(mBinaryValuesView.data(), SIGNAL(set_value(QByteArray,int)), ui->tableBinaryView, SLOT(receive_value(QByteArray,int)));
    connect(ui->tableBinaryView, SIGNAL(set_value(QByteArray,int)), mBinaryValuesView.data(), SLOT(receive_value(QByteArray,int)));
    connect(ui->tableBinaryView, SIGNAL(publish_has_binary_content(bool)), mBinaryValuesView.data(), SLOT(receive_external_data(bool)));
    connect(ui->tableBinaryView, SIGNAL(contentChanged()), this, SLOT(textBrowserChanged()));

#ifdef WEB_ENGINE
    mWebEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    PreviewPage* page = new PreviewPage(this, mWebEngineView.data());
    mWebEngineView->setPage(page);
    ui->textBrowser->set_page(page);

    m_markdown_proxy.reset(new MarkdownProxy);
    connect(ui->textBrowser, &code_browser::textChanged, [this]() { m_markdown_proxy->setText(ui->textBrowser->toPlainText()); });

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), m_markdown_proxy.data());
    page->setWebChannel(channel);
#endif

    TRACE(Logger::info, "%s Started", windowTitle().toStdString().c_str());
}

MainWindow::~MainWindow()
{
    on_btnCloseText_clicked();
    disconnect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(textBrowserChanged()));
    showDockedWidget(mBinaryValuesView.data(), true);

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    {
        STORE_PTR(fSettings, ui->ckHiddenFiles, isChecked);
        STORE_PTR(fSettings, ui->ckSymbolicLinks, isChecked);
        STORE_PTR(fSettings, ui->ckSystemFiles, isChecked);
        STORE_PTR(fSettings, ui->ckFiles, isChecked);
        STORE_PTR(fSettings, ui->ckDirectories, isChecked);
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFind);
    {
        STORE_PTR(fSettings, ui->ckSearchResultsAsSearchTree, isChecked);
        STORE_PTR(fSettings, ui->ckFastFileSearch, isChecked);
        STORE_PTR(fSettings, ui->ckFindCaseSensitive, isChecked);
        STORE_PTR(fSettings, ui->ckFindRegEx, isChecked);
        STORE_PTR(fSettings, ui->ckFindWholeWord, isChecked);
        STORE_PTR(fSettings, ui->comboFindBox, currentIndex);
        STORE_STR(fSettings, mFindGrep);
        STORE_STR(fSettings, mFindFsrc);
    }
    fSettings.endGroup();

    Highlighter::Language::store(fSettings);

    fSettings.beginGroup(config::sGroupView);
    {
        STORE_PTR(fSettings, ui->ckHideEmptyParent, isChecked);
        STORE_STR(fSettings,  Type::mShort);
        STORE_STR(fSettings, mFontName);
        {
            bool fDifferentEndian = CDisplayType::getDifferentEndian();
            STORE_STR(fSettings, fDifferentEndian);
            int fBinaryDisplayType = ui->tableBinaryView->get_type();
            STORE_STR(fSettings, fBinaryDisplayType);
            int fBinaryDisplayColumns = ui->tableBinaryView->get_columns();
            STORE_STR(fSettings, fBinaryDisplayColumns);
        }

        STORE_STR(fSettings, mWarnOpenFileSize);
        STORE_STR(fSettings, mFileCopyMimeType);
        STORE_STR(fSettings, mExternalIconFiles);
        STORE_STR(fSettings, mExternalFileOpenCmd);
        QString fTypeFormatFilesLocation = mBinaryValuesView->m_type_format_files_location;
        STORE_STR(fSettings, fTypeFormatFilesLocation);
        QString fExternalFileOpenExt;
        for (auto extension = mExternalFileOpenExt.constBegin(); extension != mExternalFileOpenExt.constEnd(); ++extension)
        {
            fExternalFileOpenExt += extension.key();
            fExternalFileOpenExt += ":";
            fExternalFileOpenExt += extension.value();
            fExternalFileOpenExt += ",";
        }
        if (fExternalFileOpenExt.size())
        {
            fExternalFileOpenExt.remove(fExternalFileOpenExt.size()-1, 1);
        }
        STORE_STR(fSettings, fExternalFileOpenExt);

        auto fWindowGeometry = saveGeometry();
        STORE_STR(fSettings, fWindowGeometry);
        auto fWindowState = saveState();
        STORE_STR(fSettings, fWindowState);
        STORE_STR(fSettings, mDockedWidgetMinMaxButtons);

        STORE_STR(fSettings, mStylePath);
        bool fUseSourceTreeCheckboxes = ui->treeSource->mUseSourceTreeCheckboxes;
        STORE_STR(fSettings, fUseSourceTreeCheckboxes);
        STORE_PTR(fSettings, ui->ckExperimental, isChecked);
        STORE_PTR(fSettings, ui->comboTabPosition, currentIndex);
        STORE_PTR(fSettings, ui->ckShowLineNumbers, isChecked);
        STORE_PTR(fSettings, ui->ckRenderGraphicFile, isChecked);
        STORE_PTR(fSettings, ui->ckShowHistoryGraphically, isChecked);
        STORE_PTR(fSettings, ui->comboToolBarStyle, currentIndex);
        STORE_PTR(fSettings, ui->comboAppStyle, currentIndex);
        STORE_PTR(fSettings, ui->comboUserStyle, currentIndex);
        auto fTextTabStopWidth = ui->textBrowser->tabStopWidth();
        STORE_STR(fSettings, fTextTabStopWidth);
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupPaths);
    {
        fSettings.beginWriteArray(config::sSourcePath);
        {
            for (int i = 0; i < ui->treeSource->topLevelItemCount(); ++i)
            {
                fSettings.setArrayIndex(i);
                QTreeWidgetItem* fItem = ui->treeSource->topLevelItem(i);
                fSettings.setValue(config::sSourcePath, fItem->text(QSourceTreeWidget::Column::FileName));
            }
        }
        fSettings.endArray();
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupGitCommands);
    {
        STORE_STRF(fSettings, Cmd::mContextMenuSourceTree, Cmd::toString);
        STORE_STRF(fSettings, Cmd::mContextMenuEmptySourceTree, Cmd::toString);
        STORE_STRF(fSettings, Cmd::mContextMenuHistoryTree, Cmd::toString);
        STORE_STRF(fSettings, Cmd::mContextMenuBranchTree, Cmd::toString);
        STORE_STRF(fSettings, Cmd::mContextMenuStashTree, Cmd::toString);
        STORE_STRF(fSettings, Cmd::mContextMenuFindTextTree, Cmd::toString);
        STORE_STRF(fSettings, Cmd::mContextMenuTextView, Cmd::toString);
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
                        QStringList list = mActions.getMenuStringList(fCmd);
                        if (list.size() > 1) fSettings.setValue(config::sMenuStringList, list);
                    }
                }
            }
        }
        fSettings.endArray();
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupLogging);
    {
        QString fSeverHlp = "_fsc____acewnidt";
        STORE_STR(fSettings, fSeverHlp);
        QString fSeverity = QString::number(Logger::getSeverity(), 2);
        STORE_STR(fSettings, fSeverity);
    }
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
    ui->treeSource->setObjectName("sourceview");

    setTabShape(QTabWidget::Rounded); // or QTabWidget::Triangular
//    setDocumentMode(false);
    setAnimated(true);
    setDockNestingEnabled(true);
//    setDockOptions(AllowTabbedDocks);
//    setDockOptions(ForceTabbedDocks); // dock widgets cannot be placed next to each other in a dock area
//    setDockOptions(VerticalTabs);
//    setDockOptions(GroupedDragging);
    QDockWidget* dock;
    // text browser
    QDockWidget *first_tab;
    dock = new QDockWidget(tr("Text Editor"), this);
    ui->comboFindBox->addItem(dock->windowTitle());
    ui->comboFindBox->addItem(tr("Go to line"));
    ui->comboFindBox->addItem("Repository View");
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("textbrowser");
    ui->verticalLayout_2->removeWidget(ui->textBrowser);
    dock->setWidget(ui->textBrowser);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    first_tab = dock;

    // graphics view
    dock = new QDockWidget(tr("Graphics View"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("graphicsviewer");
    ui->verticalLayout_2->removeWidget(ui->graphicsView);
    dock->setWidget(ui->graphicsView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);

    // binary table view
    dock = new QDockWidget(tr("Binary View"), this);
    // ui->comboFindBox->addItem(dock->windowTitle());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("binary_table_view");
    ui->verticalLayout->removeWidget(ui->tableBinaryView);
    dock->setWidget(ui->tableBinaryView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);

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

    // stash tree
    dock = new QDockWidget(tr("Stash View"), this);
    ui->comboFindBox->addItem(dock->windowTitle());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("stashview");
    ui->verticalLayout->removeWidget(ui->treeStash);
    dock->setWidget(ui->treeStash);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);

    // find tree
    dock = new QDockWidget(tr("Find Text in Files"), this);
    ui->comboFindBox->addItem(dock->windowTitle() + tr(" View"));
    ui->comboFindBox->addItem(dock->windowTitle());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("findview");
    ui->verticalLayout->removeWidget(ui->treeFindText);
    dock->setWidget(ui->treeFindText);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);

    dock = new QDockWidget(tr("Binary Values"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("binaryview");
    mBinaryValuesView.reset(new binary_values_view(this));
    dock->setWidget(mBinaryValuesView.data());
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);
    dock->setVisible(false);

#ifdef WEB_ENGINE
    // markdown view
    dock = new QDockWidget(tr("Html && Markdown"), this);
    // ui->comboFindBox->addItem(dock->windowTitle());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("markdown_view");
    mWebEngineView.reset(new QWebEngineView(this));
    ui->verticalLayout->removeWidget(mWebEngineView.data());
    dock->setWidget(mWebEngineView.data());
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    tabifyDockWidget(first_tab, dock);
    dock->setVisible(false);
#endif

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

    pTB = new QToolBar(tr("Settings"));
    pTB->setObjectName("fileflagtoolbar");
    while ((layoutItem = ui->verticalLayout_Settings->takeAt(0)) != 0)
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
    // NOTE: regard future layout items, if any
    delete ui->topLayout;
    ui->topLayout = nullptr;
    ui->horizontalLayout = nullptr;
    ui->verticalLayout_Settings = nullptr;
    ui->horizontalLayoutFind = nullptr;
    ui->horizontalLayoutForTextBrowserHead = nullptr;
    ui->horizontalLayoutForTreeViewHead = nullptr;
    ui->horizontalLayoutHistoryAndText  = nullptr;
    ui->horizontalLayoutTool = nullptr;
    ui->verticalLayout = nullptr;
    ui->verticalLayout_2 = nullptr;
    ui->verticalLayoutForTextBrowser = nullptr;
    ui->verticalLayoutForTreeView = nullptr;
}

void MainWindow::clone_code_browser()
{
    QString file_name = ui->labelFilePath->text();
    if (file_name.length())
    {
        QFileInfo info(file_name);
        file_name = info.fileName();
    }
    else
    {
        file_name = "Cloned Editor";
    }
    QDockWidget *dock = new QDockWidget(file_name, this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName("textbrowser");
    auto *docked_widget = ui->textBrowser->clone();
    dock->setWidget(docked_widget);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setAttribute(Qt::WA_DeleteOnClose);
    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
#if 1
    QDockWidget* parent = dynamic_cast<QDockWidget*>(ui->treeHistory->parent());
    if (parent)
    {
        tabifyDockWidget(parent, dock);
    }
#else
    /// TODO: set to 0, if this doc is closed
    if (m_first_cloned)
    {
        tabifyDockWidget(m_first_cloned, dock);
    }
#endif
    showDockedWidget(docked_widget);
    m_first_cloned = dock;
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

void MainWindow::showDockedWidget(QWidget* widget, bool hide)
{
#ifdef DOCKED_VIEWS
    QDockWidget* parent = dynamic_cast<QDockWidget*>(widget->parent());
    if (parent)
    {
        if (hide)
        {
            parent->setVisible(false);
        }
        else
        {
            if (!parent->isVisible())
            {
                parent->setVisible(true);
            }
            parent->raise();
        }
    }
#else
    mActions.getAction(Cmd::ShowHideTree)->setChecked(!hide);
#endif

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
    switch (aKey->key())
    {
    case Qt::Key_Delete:
    {
        auto* ftw = focusedTreeWidget(false);
        if (ftw)
        {
            deleteSelectedTreeWidgetItem(*ftw);
        }
    } break;
    case Qt::Key_Escape:
    {
        auto* ftw =  focusedTreeWidget(true);
        if (ftw)
        {
            auto selected = ftw->selectedItems();
            if (selected.size())
            {
                selected[0]->setSelected(false);
            }
        }
    }  break;
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QMainWindow::mousePressEvent(event);
}

QVariant MainWindow::handleWorker(int aWork, const QVariant& aData)
{
    QVariant work_result;
    Logger::printDebug(Logger::trace, "handleWorker(%d): %x", aWork, QThread::currentThreadId());
    switch(static_cast<Work::e>(aWork))
    {
        case Work::DetermineGitMergeTools:
        case Work::ApplyGitCommand:
            if (aData.isValid() && aData.type() == QVariant::String)
            {
                QString result_string;
                int result = execute(aData.toString().toStdString().c_str(), result_string, true);
                if (result != NoError)
                {
                    Logger::printDebug(Logger::error, "execute error (%d): %s", result, result_string.toStdString().c_str());
                }
                work_result.setValue(result_string);
            }
            break;
        default:
            break;
    }
    return work_result;
}

void MainWindow::handleMessage(int aMsg, QVariant aData)
{
    Logger::printDebug(Logger::trace, "handleMessage(%d): %x, %s", aMsg, QThread::currentThreadId(), aData.typeName());
    mActions.getAction(Cmd::KillBackgroundThread)->setEnabled(mWorker.isBusy());

    switch(static_cast<Work::e>(aMsg))
    {
        case Work::DetermineGitMergeTools:
            if (aData.isValid() && aData.type() == QVariant::String)
            {
                bool new_item_added = false;
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
                            if (!mMergeTools.contains(entry))
                            {
                                mMergeTools[entry] = true;
                                new_item_added = true;
                            }
                        }
                    }
                    if (entry.contains("not currently available"))
                    {
                        break;
                    }
                }
                if (new_item_added)
                {
                    initMergeTools();
                }
            }
            break;
        case Work::ApplyGitCommand:
            if (aData.isValid() && aData.type() == QVariant::String)
            {
                appendTextToBrowser(aData.toString());
                if (mWorkerAction)
                {
                    const QVariantList variant_list  = mWorkerAction->data().toList();
                    perform_post_cmd_action(variant_list[ActionList::Data::PostCmdAction].toUInt());
                    mWorkerAction = nullptr;
                }
            } break;
        default:  break;
    }
}

void MainWindow::killBackgroundThread()
{
    QString pids;
    execute("pidof git", pids, true);
    QStringList pidlist = pids.split(" ");
    if (pidlist.size())
    {
        int result = callMessageBox(tr("Do you really whant to kill the git processes \"%1\"?"), pids, "", pidlist.size() == 1);
        if (result == QMessageBox::Yes || result == QMessageBox::YesToAll)
        {
            for (const QString &pid : pidlist)
            {
                string cmd = "kill " + pid.toStdString();
                QString cmd_result;
                execute(cmd.c_str(), cmd_result, true);
                cmd += '\n';
                appendTextToBrowser(cmd.c_str() + cmd_result, true);
            }
        }
    }
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

void MainWindow::initCodecCombo()
{
    auto codecs = QTextCodec::availableCodecs();
    QRegExp reg_ex_num ("([\\d]{1,6})");
    QRegExp reg_ex_text("([a-zA-Z-]{1,})");
    QMap<QString, QList<QString>> codec_map;
    for (const auto& codec : codecs)
    {
        int pos = reg_ex_num.indexIn(codec);
        if (pos != -1)
        {
            QString captured_num = reg_ex_num.capturedTexts().at(0);
            QString captured_text;
            pos = reg_ex_text.indexIn(codec);
            if (pos != -1)
            {
                captured_text = reg_ex_text.capturedTexts().at(0);
            }
            if (captured_text.size() == 0)
            {
                captured_text = codec;
            }
            if (codec_map.contains(captured_num))
            {
                codec_map.find(captured_num).value().push_back(captured_text);
            }
            else
            {
                codec_map.insert(captured_num, {captured_text});
            }
        }
    }
    ui->comboTextCodex->addItem("Default Codec");
    for (auto codec = codec_map.begin(); codec != codec_map.end(); ++codec)
    {
        auto& cp_val =  codec.value();
        QString cp = cp_val.at(0) + codec.key();
        for (int i=1; i<cp_val.size(); ++i)
        {
            cp += ", " + cp_val[i];
        }
        ui->comboTextCodex->addItem(cp);
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
    if (handleInThread())
    {
        mActions.getAction(Cmd::KillBackgroundThread)->setEnabled(true);
        mWorker.doWork(Work::ApplyGitCommand, QVariant(fCommand));
    }
    else
    {
        int result = execute(fCommand, aResultStr);
        if (result != NoError)
        {
            aResultStr += tr("\nError %1 occurred").arg(result);
        }
    }
    return fCommand;
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
        if (ui->treeStash->topLevelItemCount())
        {
            ui->treeStash->setVisible(checked);
            ++fTrees;
        }
        if (ui->treeFindText->topLevelItemCount())
        {
            ui->treeFindText->setVisible(checked);
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
            ui->treeStash->setVisible(checked);
            ui->treeFindText->setVisible(checked);
        }
    }

    ui->horizontalLayout->setStretch(0, 1);
    ui->horizontalLayout->setStretch(1, checked ? 2 : 1);
    mActions.getAction(Cmd::ShowHideTree)->setChecked(checked);
}
#endif

void MainWindow::initContextMenuActions()
{
    connect(mActions.createAction(Cmd::ShowDifference , tr("Show difference")   , Cmd::getCommand(Cmd::ShowDifference)) , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::ShowDifference)->setShortcut(QKeySequence(Qt::Key_F8));
    mActions.setCmdAddOn(Cmd::ShowDifference, "--cached ");
    mActions.setFlags(Cmd::ShowDifference, Type::GitModified|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::ShowDifference, ActionList::Flags::Stash|ActionList::Flags::History|ActionList::Flags::DiffCmd|ActionList::Flags::DependsOnStaged, Flag::set);
    mActions.setMenuStringList(Cmd::ShowDifference, {"--diff-algorithm", "patience", "minimal", "histogram", "*myers"});

    connect(mActions.createAction(Cmd::CallDiffTool   , tr("Call diff tool...") , Cmd::getCommand(Cmd::CallDiffTool))   , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallDiffTool)->setShortcut(QKeySequence(Qt::Key_F9));
    mActions.setCmdAddOn(Cmd::CallDiffTool, "--cached ");
    mActions.setFlags(Cmd::CallDiffTool, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::CallDiffTool, ActionList::Flags::Stash|ActionList::Flags::History|ActionList::Flags::DiffOrMergeTool|ActionList::Flags::DiffCmd|ActionList::Flags::CallInThread|ActionList::Flags::DependsOnStaged, Flag::set);

    connect(mActions.createAction(Cmd::CallMergeTool   , tr("Call merge tool...") , Cmd::getCommand(Cmd::CallMergeTool)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallMergeTool)->setShortcut(QKeySequence(Qt::Key_F7));
    mActions.setFlags(Cmd::CallMergeTool, Type::GitUnmerged, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::CallMergeTool, ActionList::Flags::DiffOrMergeTool|ActionList::Flags::DiffCmd|ActionList::Flags::CallInThread, Flag::set);

    connect(mActions.createAction(Cmd::InvokeGitMergeDialog , tr("Merge file..."), tr("Merge selected file (experimental, not working)")) , SIGNAL(triggered()), this, SLOT(invoke_git_merge_dialog()));
    mActions.setFlags(Cmd::InvokeGitMergeDialog, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::InvokeGitMergeDialog, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::InvokeHighlighterDialog, tr("Edit Highlighting..."), tr("Edit highlighting color and font")) , SIGNAL(triggered()), this, SLOT(invoke_highlighter_dialog()));
    mActions.setFlags(Cmd::InvokeHighlighterDialog, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::InvokeHighlighterDialog, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ShowStatus      , tr("Show status")       , Cmd::getCommand(Cmd::ShowStatus))     , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowStatus, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::ShowStatus, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::ShowShortStatus , tr("Show short status") , Cmd::getCommand(Cmd::ShowShortStatus)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowShortStatus, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::ShowShortStatus, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.getAction(Cmd::ShowShortStatus)->setShortcut(QKeySequence(Qt::Key_F5));

    connect(mActions.createAction(Cmd::Add             , tr("Add to git (stage)"), Cmd::getCommand(Cmd::Add))            , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Add, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Add)->setShortcut(QKeySequence(Qt::Key_F4));
    mActions.setFlags(Cmd::Add, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Add, Type::GitModified|Type::GitUnTracked|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Unstage        , tr("Reset file (unstage)"), Cmd::getCommand(Cmd::Unstage))      , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Unstage, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Unstage)->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F4));
    mActions.setFlags(Cmd::Unstage, Type::GitStaged|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::History        , tr("Show History")      , Cmd::getCommand(Cmd::History))        , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::History, Cmd::ParseHistoryText);
    mActions.getAction(Cmd::History)->setShortcut(QKeySequence(Qt::Key_F10));
    mActions.setFlags(Cmd::History, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Blame          , tr("Blame")      , Cmd::getCommand(Cmd::Blame))               , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Blame, ActionList::Flags::CallInThread, Flag::set);
    mActions.setFlags(Cmd::Blame, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::Blame, Type::Folder, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setCustomCommandPostAction(Cmd::Blame, Cmd::ParseBlameText);

    connect(mActions.createAction(Cmd::Remove         , tr("Remove from git..."), Cmd::getCommand(Cmd::Remove))         , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Remove, "Remove %1 from git repository;Do you want to remove \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::Remove, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::Remove, Type::GitUnTracked, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Remove, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Restore         , tr("Restore changes..."), Cmd::getCommand(Cmd::Restore))       , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Restore, tr("Restore %1 changes;Do you want to restore changes in \"%1\"?"));
    mActions.setCustomCommandPostAction(Cmd::Restore, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Restore)->setShortcut(QKeySequence(Qt::Key_F6));
    mActions.setFlags(Cmd::Restore, Type::GitModified|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::Restore, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Restore, ActionList::Flags::History);

    connect(mActions.createAction(Cmd::Commit         , tr("Commit..."), Cmd::getCommand(Cmd::Commit)), SIGNAL(triggered()), this, SLOT(call_git_commit()));
    mActions.setCustomCommandPostAction(Cmd::Commit, Cmd::UpdateRootItemStatus);
    mActions.setFlags(Cmd::Commit, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::Commit, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Push           , tr("Push"), Cmd::getCommand(Cmd::Push)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Push, ActionList::Flags::CallInThread, Flag::set);
    mActions.setFlags(Cmd::Push, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::Pull           , tr("Pull"), Cmd::getCommand(Cmd::Pull)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Pull, ActionList::Flags::CallInThread, Flag::set);
    mActions.setFlags(Cmd::Pull, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::Fetch          , tr("Fetch"), Cmd::getCommand(Cmd::Fetch)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Fetch, ActionList::Flags::CallInThread, Flag::set);
    mActions.setFlags(Cmd::Fetch, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::Show           , tr("Show"), Cmd::getCommand(Cmd::Show)), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Show, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    const QString stash_message = tr("Stash all entries;Do you whant to stash all entries of repository:\n\"%1\"?");
    connect(mActions.createAction(Cmd::Stash          , tr("Stash"),       Cmd::getCommand(Cmd::Stash))    ,  SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Stash, stash_message);
    mActions.setFlags(Cmd::Stash, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::Stash, ActionList::Flags::Stash, Flag::set);
    connect(mActions.createAction(Cmd::StashShow      , tr("Show stash"),  Cmd::getCommand(Cmd::StashShow)),  SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::StashShow, ActionList::Flags::Stash, Flag::set);
    mActions.setFlags(Cmd::StashShow, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashPush,       tr("Stash push"),  Cmd::getCommand(Cmd::StashPush)),  SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::StashPush, stash_message);
    mActions.setCmdAddOn(Cmd::StashPush, " -- ");
    mActions.setFlags(Cmd::StashPush, ActionList::Flags::StashCmdOption|ActionList::Flags::Stash, Flag::set);
    mActions.setFlags(Cmd::StashPush, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashPop       , tr("Stash pop"),   Cmd::getCommand(Cmd::StashPop))  ,  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setFlags(Cmd::StashPop, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashApply     , tr("Stash apply"), Cmd::getCommand(Cmd::StashApply)),  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setFlags(Cmd::StashApply, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashDrop      , tr("Stash drop"),  Cmd::getCommand(Cmd::StashDrop)) ,  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::StashDrop, "Drop stash entry;Do you whant to drop stash entry of repository:\n\"%1\"?");
    mActions.setFlags(Cmd::StashDrop, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashClear     , tr("Stash clear"), Cmd::getCommand(Cmd::StashClear)), SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::StashClear, "Remove all stash entries;Do you whant to remove all stash entries of repository:\n\"%1\"?");
    mActions.setFlags(Cmd::StashClear, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashList      , tr("List stashes"),Cmd::getCommand(Cmd::StashList)),  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandPostAction(Cmd::StashList, Cmd::ParseStashListText);
    mActions.setFlags(Cmd::StashList, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchList     , tr("List Branches"), Cmd::getCommand(Cmd::BranchList)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchList, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchList, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchList, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchListRemote, tr("List remote Branches"), Cmd::getCommand(Cmd::BranchListRemote)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListRemote, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchListRemote, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchListRemote, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchListMerged, tr("List merged Branches"), Cmd::getCommand(Cmd::BranchListMerged)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListMerged, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchListMerged, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchListMerged, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchListNotMerged, tr("List not merged Branches"), Cmd::getCommand(Cmd::BranchListNotMerged)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListNotMerged, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchListNotMerged, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchListNotMerged, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchDelete   , tr("Delete Branch"), Cmd::getCommand(Cmd::BranchDelete)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchDelete, tr("Delete %1 from git;Do you want to delete \"%1\"?"));
    mActions.setCustomCommandPostAction(Cmd::BranchDelete, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::BranchDelete, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchDelete, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchCheckout, tr("Checkout Branch"), Cmd::getCommand(Cmd::BranchCheckout)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchCheckout, tr("Checkout %1;Do you want to set \"%1\" active?;"));
    mActions.setCustomCommandPostAction(Cmd::BranchCheckout, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::BranchCheckout, ActionList::Flags::NotVariableGitCmd, Flag::set);

    connect(mActions.createAction(Cmd::BranchHistory, tr("History Branch"), Cmd::getCommand(Cmd::BranchHistory)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchHistory, Cmd::ParseHistoryText);
    mActions.setFlags(Cmd::BranchHistory, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchHistory, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchShow, tr("Show Branch"), Cmd::getCommand(Cmd::BranchShow)), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setFlags(Cmd::BranchShow, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchShow, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::MoveOrRename   , tr("Move / Rename..."), Cmd::getCommand(Cmd::MoveOrRename)), SIGNAL(triggered()), this, SLOT(call_git_move_rename()));
    mActions.getAction(Cmd::MoveOrRename)->setShortcut(QKeySequence(Qt::Key_F2));
    mActions.setFlags(Cmd::MoveOrRename, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::MoveOrRename, Type::GitUnTracked, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::MoveOrRename, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ExpandTreeItems      , tr("Expand Tree Items"), tr("Expands all tree item of focused tree")) , SIGNAL(triggered()), this, SLOT(expand_tree_items()));
    mActions.setFlags(Cmd::ExpandTreeItems, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.getAction(Cmd::ExpandTreeItems)->setShortcut(QKeySequence(Qt::Key_F11));
    mActions.setFlags(Cmd::ExpandTreeItems, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::CollapseTreeItems    , tr("Collapse Tree Items"), tr("Collapses all tree item of focused tree")), SIGNAL(triggered()), this, SLOT(collapse_tree_items()));
    mActions.setFlags(Cmd::CollapseTreeItems, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.getAction(Cmd::CollapseTreeItems)->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F11));
    mActions.setFlags(Cmd::CollapseTreeItems, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::AddGitSourceFolder   , tr("Add git source folder..."), tr("Add a git source folder to repository view")) , SIGNAL(triggered()), this, SLOT(addGitSourceFolder()));
    mActions.setFlags(Cmd::AddGitSourceFolder, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::AddGitSourceFolder, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::RemoveGitFolder, tr("Remove git source folder"), tr("Remove a git source folder from repository view")), SIGNAL(triggered()), this, SLOT(removeGitSourceFolder()));
    mActions.setFlags(Cmd::RemoveGitFolder, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::RemoveGitFolder, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::UpdateGitStatus, tr("Update git status"), tr("Updates the git status of the selected source folder")), SIGNAL(triggered()), this, SLOT(updateGitStatus()));
    mActions.setFlags(Cmd::UpdateGitStatus, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::UpdateGitStatus, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

#ifndef DOCKED_VIEWS
    connect(mActions.createAction(Cmd::ShowHideTree  , tr("Show/Hide tree"), tr("Shows or hides history or branches tree")) , SIGNAL(toggled(bool)), this, SLOT(showOrHideTrees(bool)));
    mActions.getAction(Cmd::ShowHideTree)->setCheckable(true);
    mActions.setFlags(Cmd::ShowHideTree, ActionList::Flags::FunctionCmd, Flag::set);
#endif
    connect(mActions.createAction(Cmd::ClearTreeItems       , tr("Clear all tree entries"), tr("Clears all tree entries in focused tree except repository tree")), SIGNAL(triggered()), this, SLOT(clearTrees()));
    mActions.setFlags(Cmd::ClearTreeItems, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::ClearTreeItems, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::DeleteTreeItems      , tr("Delete selected tree items"), tr("Deletes all selected tree items in focused tree except repository tree")), SIGNAL(triggered()), this, SLOT(delete_tree_item()));
    mActions.setFlags(Cmd::DeleteTreeItems, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::DeleteTreeItems, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::AddExternalFileOpenExt, tr("Set open external flag"), tr("Sets parameters to open files with this extension externally")), SIGNAL(triggered()), this, SLOT(add_file_open_extension()));
    mActions.setFlags(Cmd::AddExternalFileOpenExt, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::AddExternalFileOpenExt, Type::File, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::DeleteExternalFileOpenExt, tr("Remove open external flag"), tr("Removes open external parameters")), SIGNAL(triggered()), this, SLOT(delete_file_open_extension()));
    mActions.setFlags(Cmd::DeleteExternalFileOpenExt, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::DeleteExternalFileOpenExt, Type::File, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::OpenFileExternally, tr("Open file externally"), tr("Opens file externally")), SIGNAL(triggered()), this, SLOT(open_file_externally()));
    mActions.setFlags(Cmd::OpenFileExternally, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::OpenFileExternally, Type::File, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::WhatsThisHelp, tr("Whats this?"), tr("Whats this help")), &QAction::triggered, []() { QWhatsThis::enterWhatsThisMode(); } );
    mActions.setFlags(Cmd::WhatsThisHelp, ActionList::Flags::FunctionCmd, Flag::set);

    connect(mActions.createAction(Cmd::CustomGitActionSettings, tr("Customize git actions..."), tr("Edit custom git actions, menues and toolbars")), SIGNAL(triggered()), this, SLOT(performCustomGitActionSettings()));
    mActions.setFlags(Cmd::CustomGitActionSettings, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CustomGitActionSettings, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::InsertHashFileNames  , tr("Insert File Name List"), tr("Inserts file names that differ from previous hash")), SIGNAL(triggered()), ui->treeHistory, SLOT(insertFileNames()));
    mActions.setFlags(Cmd::InsertHashFileNames, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::InsertHashFileNames, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::About, tr("About..."), tr("Information about GitView")), SIGNAL(triggered()), this, SLOT(gitview_about()));
    mActions.setFlags(Cmd::About, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::About, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Delete, tr("Delete..."), tr("Delete file or folder")), SIGNAL(triggered()), this, SLOT(deleteFileOrFolder()));
    mActions.setFlags(Cmd::Delete, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::Delete, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::SelectTextBrowserLanguage, tr("Select Language..."), tr("Select language for text highlighting")), SIGNAL(triggered()), this, SLOT(selectTextBrowserLanguage()));
    mActions.setFlags(Cmd::SelectTextBrowserLanguage, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::SelectTextBrowserLanguage, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::KillBackgroundThread, tr("Running Background Activity..."), tr("You may try to kill git action running in background")), SIGNAL(triggered()), this, SLOT(killBackgroundThread()));
    mActions.getAction(Cmd::KillBackgroundThread)->setEnabled(false);
    mActions.setFlags(Cmd::KillBackgroundThread, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::KillBackgroundThread, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CopyFileName, tr("Copy file name"), tr("Copy file name to clipboard")), SIGNAL(triggered()), this, SLOT(copyFileName()));
    mActions.setFlags(Cmd::CopyFileName, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CopyFileName, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::CopyFilePath, tr("Copy file and path"), tr("Copy file or folder and path to clipboard")), SIGNAL(triggered()), this, SLOT(copyFilePath()));
    mActions.setFlags(Cmd::CopyFilePath, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CopyFilePath, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ZoomIn, tr("Zoom in"), tr("Zoom in (make larger)")), SIGNAL(triggered()), ui->graphicsView, SLOT(zoomIn()));
    mActions.setFlags(Cmd::ZoomIn, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::ZoomIn, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::ZoomOut, tr("Zoom out"), tr("Zoom out (make smaller)")), SIGNAL(triggered()), ui->graphicsView, SLOT(zoomOut()));
    mActions.setFlags(Cmd::ZoomOut, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::ZoomOut, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::FitInView, tr("Fit in View"), tr("Fits Item in View, when opened")), SIGNAL(triggered(bool)), ui->graphicsView, SLOT(fit_inView(bool)));
    mActions.setFlags(Cmd::FitInView, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.getAction(Cmd::FitInView)->setCheckable(true);
    mActions.setFlags(Cmd::FitInView, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CloneTextBrowser, tr("Clone text view"), tr("Opens this file in a new window")), SIGNAL(triggered()), this, SLOT(clone_code_browser()));
    mActions.setFlags(Cmd::CloneTextBrowser, ActionList::Flags::FunctionCmd, Flag::set);

    connect(mActions.createAction(Cmd::CreateBookMark, tr("Create Bookmark"), tr("Creates a Bookmark")), SIGNAL(triggered()), this, SLOT(createBookmark()));
    mActions.setFlags(Cmd::CreateBookMark, ActionList::Flags::FunctionCmd, Flag::set);

    connect(mActions.createAction(Cmd::ShowInformation, tr("Show Information"), tr("Show information about selected item")), SIGNAL(triggered()), this, SLOT(showInformation()));
    mActions.getAction(Cmd::ShowInformation)->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_I));
    mActions.setFlags(Cmd::ShowInformation, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::ShowInformation, Type::File|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    add_action_to_widgets(mActions.getAction(Cmd::ShowInformation));

    create_auto_cmd(ui->ckDirectories);
    create_auto_cmd(ui->ckFiles);
    create_auto_cmd(ui->ckHiddenFiles);
    create_auto_cmd(ui->ckShortState);
    create_auto_cmd(ui->ckSymbolicLinks);
    create_auto_cmd(ui->ckSystemFiles);
    create_auto_cmd(ui->ckRenderGraphicFile, resource + "applications-graphics.png");
    create_auto_cmd(ui->ckHideEmptyParent);
    create_auto_cmd(ui->ckFindCaseSensitive, resource + "applications-system.png");
    create_auto_cmd(ui->ckFindRegEx,         resource + "applications-system.png");
    create_auto_cmd(ui->ckFindWholeWord,     resource + "applications-system.png");
    create_auto_cmd(ui->ckExperimental);
    create_auto_cmd(ui->ckFastFileSearch);
    create_auto_cmd(ui->ckTypeConverter, resource + "format-text-direction-rtl.png");

    Cmd::eCmd new_id = Cmd::Invalid;
    std::vector<Cmd::eCmd> contextmenu_text_view;
    contextmenu_text_view.push_back(Cmd::Separator);

    create_auto_cmd(ui->ckShowLineNumbers, resource + "x-office-document.png", &new_id);
    contextmenu_text_view.push_back(new_id);
    contextmenu_text_view.push_back(Cmd::Separator);

    create_auto_cmd(ui->btnStoreText, resource + "text-x-patch.png", &new_id)->  setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_W));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->btnCloseText, "", &new_id)->                             setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_S));
    contextmenu_text_view.push_back(new_id);
    contextmenu_text_view.push_back(Cmd::Separator);

    create_auto_cmd(ui->btnFindX, resource + "edit-find.png", &new_id);
    create_auto_cmd(ui->btnFindNext, resource + "go-next.png", &new_id)->        setShortcut(QKeySequence(Qt::Key_F3));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->btnFindPrevious, resource + "go-previous.png", &new_id)->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F3));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->comboFindBox, resource + "edit-find.png", &new_id)->     setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_F));
    contextmenu_text_view.push_back(new_id);

    create_auto_cmd(ui->comboShowItems);

    if (Cmd::mContextMenuTextView.empty())
    {
        Cmd::mContextMenuTextView = contextmenu_text_view;
    }

    for (const auto& fAction : mActions.getList())
    {
        mActions.setFlags(static_cast<Cmd::eCmd>(fAction.first), ActionList::Flags::BuiltIn);
    }
}

void MainWindow::add_action_to_widgets(QAction * action)
{
    ui->textBrowser->addAction(action);
    ui->treeSource->addAction(action);
    ui->treeBranches->addAction(action);
    ui->treeFindText->addAction(action);
    ui->treeHistory->addAction(action);
    ui->treeStash->addAction(action);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
}

QAction* MainWindow::create_auto_cmd(QWidget *widget, const QString& icon_path, Cmd::eCmd *new_id)
{
    const auto comand_id = mActions.createNewID(Cmd::AutoCommand);

    const QAbstractButton*button  = dynamic_cast<QAbstractButton*>(widget);
    const QComboBox*      combobox= dynamic_cast<QComboBox*>(widget);
    QString               name    = button != 0 ? button->text() : "";
    QString               command = widget != 0 ? widget->toolTip() : "";
    if (combobox)
    {
        name    = combobox->toolTip();
        command = combobox->statusTip();
    }
    QAction* action  = mActions.createAction(comand_id, name, command, widget);
    mActions.setFlags(comand_id, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    if (icon_path.size())
    {
        mActions.setIconPath(comand_id, icon_path);
    }
    else
    {
        mActions.setIconPath(comand_id, ":/resource/24X24/window-close.png");
    }
    mActions.setFlags(comand_id, ActionList::Flags::FunctionCmd);

    if (new_id)
    {
        *new_id = comand_id;
    }
    if (widget)
    {
        widget->addAction(action);
    }
    add_action_to_widgets(action);

    if (button)
    {
        if (button->isCheckable())
        {
            action->setCheckable(true);
            connect(button, SIGNAL(clicked(bool)), action, SLOT(setChecked(bool)));
            connect(action, SIGNAL(triggered(bool)), button, SLOT(setChecked(bool)));
            action->setChecked(button->isChecked());
        }
        else
        {
            connect(action, SIGNAL(triggered()), button, SLOT(click()));
        }
    }
    if (combobox)
    {
        connect(action, SIGNAL(triggered()), this, SLOT(combo_triggered()));
    }

    return action;
}

void MainWindow::initCustomAction(QAction* fAction)
{
    connect(fAction, SIGNAL(triggered()), this, SLOT(perform_custom_command()));
}


void MainWindow::clearTrees()
{
    QTreeWidget * tree = focusedTreeWidget(false);
    if (tree)
    {
        tree->clear();
    }
}

void MainWindow::delete_tree_item()
{
    QTreeWidget * tree = focusedTreeWidget(false);
    if (tree)
    {
        deleteSelectedTreeWidgetItem(*tree);
    }
}

void MainWindow::add_file_open_extension()
{
    if (mContextMenuSourceTreeItem)
    {
        const QFileInfo file_info(ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem));
        const QString   file_extension = file_info.suffix().toLower();
        bool ok = false;
        const QString program_parameter = QInputDialog::getText(0, tr("Program parameter"),
            tr("Set open program for '%1' file extension\nLeave empty for default program '%2'").arg(file_extension, mExternalFileOpenCmd),
            QLineEdit::Normal, mExternalFileOpenExt.value(file_extension), &ok);

        if (ok)
        {
            mExternalFileOpenExt[file_extension] = program_parameter;
        }
    }
}

void MainWindow::delete_file_open_extension()
{
    if (mContextMenuSourceTreeItem)
    {
        const QFileInfo file_info(ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem));
        const QString   file_extension = file_info.suffix().toLower();
        mExternalFileOpenExt.remove(file_extension);
    }
}

bool MainWindow::handleInThread()
{
    const QAction *fAction = qobject_cast<QAction *>(sender());
    if (fAction && !mWorker.isBusy())
    {
        mWorkerAction = fAction;
        return (fAction->data().toList()[ActionList::Data::Flags].toUInt() & ActionList::Flags::CallInThread) != 0;
    }
    return false;
}

void MainWindow::expand_tree_items()
{
    QTreeWidget* ftw = focusedTreeWidget();
    if (ftw)
    {
        ftw->expandAll();
    }
}

void MainWindow::collapse_tree_items()
{
    QTreeWidget* ftw = focusedTreeWidget();
    if (ftw)
    {
        ftw->collapseAll();
    }
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
    else if (ui->treeStash->hasFocus())
    {
        return ui->treeStash;
    }
    else if (ui->treeStash->hasFocus())
    {
        return ui->treeStash;
    }
    else if (ui->treeFindText->hasFocus())
    {
        return ui->treeFindText;
    }
    else if (aAlsoSource)
    {
        return ui->treeSource;
    }
    return nullptr;
}

void MainWindow::performCustomGitActionSettings()
{
    CustomGitActions edit_custom_git_actions(mActions, mMergeTools, this);
    edit_custom_git_actions.mExperimental = ui->ckExperimental->isChecked();
    edit_custom_git_actions.mExternalIconFiles = mExternalIconFiles;

    connect(&edit_custom_git_actions, SIGNAL(initCustomAction(QAction*)), this, SLOT(initCustomAction(QAction*)));
    if (edit_custom_git_actions.exec() == QDialog::Accepted)
    {
        if (edit_custom_git_actions.isMergeToolsChanged())
        {
            initMergeTools();
        }
        mExternalIconFiles = edit_custom_git_actions.mExternalIconFiles;
    }
    for (unsigned int i=0; i<mToolBars.size(); ++i)
    {
        mToolBars[i]->clear();
        mActions.fillToolbar(*mToolBars[i], Cmd::mToolbars[i]);
    }
}

void MainWindow::invoke_git_merge_dialog()
{
    if (!mMergeDialog)
    {
        mMergeDialog.reset(new MergeDialog(this));
        connect(mMergeDialog->getAction(), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    }

    mMergeDialog->mGitFilePath = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
    mMergeDialog->show();
}

void MainWindow::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{
    ui->graphicsView->on_customContextMenuRequested(mActions, pos);
}

void MainWindow::gitview_about()
{
    AboutDlg dlg(this);
    dlg.exec();
}
void MainWindow::on_comboToolBarStyle_currentIndexChanged(int index)
{
#if 0
    // for single toolbar
    mToolBars[0]->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(index));
#else
    mActions.setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(index));
    setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(index));
#endif
}

void MainWindow::on_comboAppStyle_currentTextChanged(const QString &style)
{
    QApplication::setStyle(QStyleFactory::create(style));
}

void MainWindow::on_comboUserStyle_currentIndexChanged(int index)
{
    switch (index)
    {
    case UserStyle::None:
        setStyleSheet("");
        QApplication::setPalette(QGuiApplication::palette());
        ui->textBrowser->set_dark_mode(false);
        break;
    case UserStyle::User:
    {
        /// TODO: find a good dark style
        QFile f(mStylePath);
        if (!f.exists())
        {
            TRACE(Logger::error, "Unable to set stylesheet, file %s not found\n", mStylePath.toStdString().c_str());
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            setStyleSheet(ts.readAll());
            ui->textBrowser->set_dark_mode(true);
        }
        break;
    }
    case UserStyle::Palette:
    {
        const auto button_color = QColor(53, 53, 53);
        QPalette palette = QGuiApplication::palette();
        palette.setColor(QPalette::Window, button_color);
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(25, 25, 25));
        palette.setColor(QPalette::AlternateBase, button_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, Qt::black);
        palette.setColor(QPalette::Button, button_color);              // invert normal behavour
        palette.setColor(QPalette::Light, button_color.darker(50));    // Lighter than Button color
        palette.setColor(QPalette::Midlight, button_color.lighter(50));// Between Button and Light.
        palette.setColor(QPalette::Mid, button_color.lighter(100));    // Between Button and Dark.
        palette.setColor(QPalette::Dark, button_color.lighter(150));   // Darker than Button.
        QApplication::setPalette(palette);
        ui->textBrowser->set_dark_mode(true);
        break;
    }
    }
}

void MainWindow::on_comboFindBox_currentIndexChanged(int index)
{
    auto find = static_cast<FindView>(index);
    if (find == FindView::Text)
    {
        ui->btnFindX->setText(tr("Replace"));
        set_widget_and_action_enabled(ui->btnFindX, true);
    }
    else
    {
        ui->btnFindX->setText(tr("All"));
        set_widget_and_action_enabled(ui->btnFindX, find != FindView::GoToLineInText);
    }
    set_widget_and_action_enabled(ui->btnFindNext, find != FindView::FindTextInFiles);
    set_widget_and_action_enabled(ui->btnFindPrevious, find != FindView::FindTextInFiles && find != FindView::GoToLineInText);
    ui->ckFindWholeWord->setEnabled(!(find == FindView::FindTextInFiles && ui->ckFastFileSearch->isChecked()));

    switch(find)
    {
    case FindView::Text:                ui->statusBar->showMessage(tr("Search in Text Editor")); break;
    case FindView::GoToLineInText:      ui->statusBar->showMessage(tr("Go to line in Text Editor")); break;
    case FindView::FindTextInFiles:     ui->statusBar->showMessage(tr("Search for text in files under selected folder in Repository View")); break;
    case FindView::FindTextInFilesView: ui->statusBar->showMessage(tr("Search for text in find results")); break;
    case FindView::Source:              ui->statusBar->showMessage(tr("Search files or folders in Repository View")); break;
    case FindView::History:             ui->statusBar->showMessage(tr("Search item in History View")); break;
    case FindView::Branch:              ui->statusBar->showMessage(tr("Search item in Branch View")); break;
    case FindView::Stash:               ui->statusBar->showMessage(tr("Search in Stash View")); break;
    }
}

void MainWindow::combo_triggered()
{
    const QAction* action = qobject_cast<QAction *>(sender());
    const auto combofind_actions = ui->comboFindBox->actions();
    if (combofind_actions.size() && combofind_actions.first() == action)
    {
        FindView index = FindView::FindTextInFiles;
        if      (ui->textBrowser->hasFocus())  index = FindView::Text;
        else if (ui->treeHistory->hasFocus())  index = FindView::History;
        else if (ui->treeBranches->hasFocus()) index = FindView::Branch;
        else if (ui->treeStash->hasFocus())    index = FindView::Stash;
        else if (ui->treeSource->hasFocus())   index = FindView::Source;

        QString find_text;
        QTreeWidget* tree_view = nullptr;
        switch (index)
        {
        case FindView::Text:
            find_text = ui->textBrowser->textCursor().selectedText();
            break;
        case FindView::History:             tree_view = ui->treeHistory; break;
        case FindView::Branch:              tree_view = ui->treeBranches; break;
        case FindView::Stash:               tree_view = ui->treeStash; break;
        case FindView::Source:              tree_view = ui->treeSource; break;
        case FindView::FindTextInFilesView: tree_view = ui->treeFindText; break;
        case FindView::GoToLineInText:
        case FindView::FindTextInFiles:
            break;
        }

        ui->comboFindBox->setCurrentIndex(static_cast<int>(index));
        if (tree_view && tree_view->currentItem())
        {
            find_text = tree_view->currentItem()->text(0);
        }
        if (find_text.size())
        {
            ui->edtFindText->setText(find_text);
            ui->edtFindText->setModified(true);
        }
        else
        {
            ui->edtFindText->selectAll();
            ui->edtFindText->setFocus();
        }
    }
    const auto combofind_show_items = ui->comboShowItems->actions();
    if (combofind_show_items.size() && combofind_show_items.first() == action)
    {
        auto index = ui->comboShowItems->currentIndex();
        if (index < ui->comboShowItems->count()-1)
        {
            ui->comboShowItems->setCurrentIndex(index + 1);
        }
        else
        {
            ui->comboShowItems->setCurrentIndex(0);
        }
    }
}

MainWindow::tree_find_properties::tree_find_properties() : mFlags(-1), mIndex(0), mColumn(0)
{
}

void MainWindow::on_btnFindNext_clicked()
{
    find_function(find::forward);
}

void MainWindow::on_btnFindPrevious_clicked()
{
    find_function(find::backward);
}

void MainWindow::on_btnFindX_clicked()
{
    if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::Text))
    {
        find_in_text_view(find::replace);
    }
    else
    {
        find_function(find::all);
    }
}

void MainWindow::find_function(find find_item)
{
    if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::Text))
    {
        find_in_text_view(find_item);
    }
    else if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::GoToLineInText))
    {
        ui->textBrowser->go_to_line(ui->edtFindText->text().toInt());
    }
    else if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::FindTextInFiles) && mContextMenuSourceTreeItem)
    {
        find_text_in_files();
    }
    else
    {
        find_in_tree_views(find_item);
    }
}

void MainWindow::find_in_text_view(find find_item)
{
    Qt::CaseSensitivity reg_ex_case = Qt::CaseInsensitive;
    int                 find_flag   = find_item == find::forward || find_item == find::replace ? 0 : QTextDocument::FindBackward;

    if (ui->textBrowser->extraSelections().size() && find_item == find::replace)
    {
        ui->textBrowser->insertPlainText(ui->edtReplaceText->text());
    }

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

void MainWindow::find_in_tree_views(find find_item)
{
    QTreeWidget *tree_view {nullptr};
    switch (static_cast<FindView>(ui->comboFindBox->currentIndex()))
    {
        case FindView::Source:              tree_view = ui->treeSource;   break;
        case FindView::History:             tree_view = ui->treeHistory;  break;
        case FindView::Branch:              tree_view = ui->treeBranches; break;
        case FindView::Stash:               tree_view = ui->treeStash;    break;
        case FindView::FindTextInFilesView: tree_view = ui->treeFindText; break;
        case FindView::Text: case FindView::FindTextInFiles:  break;
        case FindView::GoToLineInText: break;
    }

    if (tree_view)
    {
        tree_find_properties& property = mTreeFindProperties[tree_view->objectName()];
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

        auto& found_items = property.mItems;

        int column = tree_view->currentColumn();
        if (column < 0) column = 0;
        if (   tree_match_flag != property.mFlags
            || column != property.mColumn
            || ui->edtFindText->isModified())
        {
            tree_view->clearSelection();
            found_items      = tree_view->findItems(text_to_find, static_cast<Qt::MatchFlag>(tree_match_flag), column);
            property.mFlags  = tree_match_flag;
            property.mIndex  = 0;
            property.mColumn = column;
            ui->edtFindText->setModified(false);
        }
        else
        {
            if (find_item == find::forward)
            {
                if (++property.mIndex >= found_items.size())
                {
                    property.mIndex = 0;
                }
            }
            else
            {
                if (--property.mIndex <= 0 )
                {
                    property.mIndex = found_items.size() -1;
                }
            }
        }
        if (find_item == find::all)
        {
            for (auto& item : found_items)
            {
                tree_view->setItemSelected(item, true);
            }
            ui->comboShowItems->setCurrentIndex(static_cast<int>(ComboShowItems::GitSelected));
            on_comboShowItems_currentIndexChanged(static_cast<int>(ComboShowItems::GitSelected));
            expand_tree_items();
        }
        else if (found_items.size())
        {
            int i=0;
            for (auto& item : found_items)
            {
                tree_view->setItemSelected(item, i == property.mIndex);
                if (i == property.mIndex)
                {
                    auto* parent = item->parent();
                    while (parent)
                    {
                        tree_view->setItemExpanded(parent, true);
                        parent = parent->parent();
                    }
                    tree_view->scrollToItem(item);
                }
                ++i;
            }
            showDockedWidget(tree_view);
        }
    }
}

QTreeWidgetItem* MainWindow::insert_file_path(QTreeWidgetItem* new_tree_root_item, const QString& current_file)
{
    QTreeWidgetItem* new_child_item { nullptr };
    if (ui->ckSearchResultsAsSearchTree->isChecked())
    {
        QStringList tree_items = current_file.split("/");
        new_child_item = insert_as_tree(new_tree_root_item, FindColumn::FilePath, tree_items);
    }
    else
    {
        new_child_item = new QTreeWidgetItem();
        new_tree_root_item->addChild(new_child_item);
        new_child_item->setText(FindColumn::FilePath, current_file);
    }
    return new_child_item;
}

void MainWindow::find_text_in_files()
{
    const bool fast_search = ui->ckFastFileSearch->isChecked();
    QString find_result;
    QString find_command;
    QString search_path    = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
    QString search_pattern = ui->edtFindText->text();
    if (fast_search)
    {
        // Usage  : fsrc [options] term
        // Options:
        //  -d [ --dir ] arg      Search folder
        //  -e [ --ext ] arg      Search only in files with extension <arg>, equiv. to
        //                        --glob '*.ext'
        //  -g [ --glob ] arg     Search only in files filtered by <arg> glob, e.g.
        //                        '*.txt'; overrides --ext
        //  -h [ --help ]         Help
        //  --html                open web page with results
        //  -i [ --ignore-case ]  Case insensitive search
        //  --no-git              Disable search with 'git ls-files'
        //  --no-colors           Disable colorized output
        //  --no-piped            Disable piped output
        //  --no-uri              Print w/out file:// prefix
        //  --only-files          Only print filenames
        //  --piped               Enable piped output
        //  -q [ --quiet ]        only print status
        //  -r [ --regex ]        Regex search (slower)
        QString options = "--no-piped";
        if (!ui->ckFindCaseSensitive->isChecked())
        {   // i: ignore case
            options += " -i";
        }
        if (ui->ckFindRegEx->isChecked())
        {   // E: regular expression
            options += " -r";
        }
        find_command = tr("%1 -d %2 %3 '%4'").arg(mFindFsrc, search_path, options, search_pattern );
    }
    else
    {
        //"grep -rnHsI[oiEw] 'search_pattern' 'path'"
        // r: recursive
        // n: line number
        // H: file name
        // s: no message
        // I: no text search in binary files
        // o: show only match without line
        QString options = "-rnHsI";
        if (!ui->ckFindCaseSensitive->isChecked())
        {   // i: ignore case
            options += "i";
        }
        if (ui->ckFindRegEx->isChecked())
        {   // E: regular expression
            options += "E";
        }
        if (ui->ckFindWholeWord->isChecked())
        {   // w: whole word
            options += "w";
        }
        find_command = tr("%1 %2 '%3' '%4'").arg(mFindGrep, options, search_pattern, search_path);
    }
    int result = execute(find_command, find_result);
    if (result == 0)
    {
        QString repository_root = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(QSourceTreeWidget::Column::FileName);
        auto new_tree_root_item = new QTreeWidgetItem({tr("Search expression: %1").arg(search_pattern), "", repository_root});
        ui->treeFindText->addTopLevelItem(new_tree_root_item);
        auto found_items = find_result.split('\n');

        if (fast_search)
        {
            const QString file_id = "file://";
            QString current_file;
            for (const QString& found_item : found_items)
            {
                int pos = found_item.indexOf(file_id);
                if (pos != -1)
                {
                    current_file = found_item.mid(pos + file_id.size());
                    if (containsPathAsChildren(mContextMenuSourceTreeItem, QSourceTreeWidget::Column::FileName, current_file.mid(search_path.size() + 1)))
                    {
                        current_file = current_file.mid(repository_root.size() + 1);
                    }
                    else
                    {
                        current_file.clear();
                    }
                    continue;
                }
                if (current_file.size())
                {
                    QStringList found_item_parts = found_item.split(':');
                    if (found_item_parts.size() >= 2 && found_item_parts[0].size() && found_item_parts[0][0] == 'L')
                    {
                        QTreeWidgetItem* new_child_item = insert_file_path(new_tree_root_item, current_file);
                        found_item_parts[0][0] = ' ';
                        new_child_item->setText(FindColumn::Line, found_item_parts[0].trimmed());
                        QString found_text_line = found_item_parts[1];
                        for (int i = 2; i<found_item_parts.size(); ++i)
                        {
                            found_text_line += ":";
                            found_text_line += found_item_parts[i];
                        }
                        new_child_item->setText(FindColumn::FoundTextLine, found_text_line.trimmed());
                    }
                }
            }
        }
        else
        {
            for (const QString& found_item : found_items)
            {
                QStringList found_item_parts = found_item.split(':');
                if (found_item_parts.size() >= 2)
                {
#ifdef __linux__
                    QString file_path = found_item_parts[FindColumn::FilePath];
#else
                    found_item_parts[1] = found_item_parts[0] + ":" + found_item_parts[1];
                    found_item_parts.erase(found_item_parts.begin());
                    QString file_path = found_item_parts[FindColumn::FilePath];
                    QFileInfo fi(file_path);
                    file_path = fi.filePath();
#endif
                    if (containsPathAsChildren(mContextMenuSourceTreeItem, QSourceTreeWidget::Column::FileName, file_path.mid(search_path.size() + 1)))
                    {
                        QString current_file = file_path.mid(repository_root.size() + 1);
                        QTreeWidgetItem* new_child_item  = insert_file_path(new_tree_root_item, current_file);
                        new_child_item->setText(FindColumn::Line, found_item_parts[FindColumn::Line].trimmed());
                        QString found_text_line = found_item_parts[FindColumn::FoundTextLine];
                        for (int i = FindColumn::FoundTextLine + 1; i<found_item_parts.size(); ++i)
                        {
                            found_text_line += ":";
                            found_text_line += found_item_parts[i];
                        }
                        new_child_item->setText(FindColumn::FoundTextLine, found_text_line.trimmed());
                    }
                }
            }
        }
        if (found_items.size() > 0)
        {
            showDockedWidget(ui->treeFindText);
            ui->treeFindText->expandItem(new_tree_root_item);
        }
    }
    else
    {
        appendTextToBrowser(find_command + "\n" + find_result + tr("\nresult error: %1").arg(result));
    }
}

void MainWindow::on_treeFindText_itemDoubleClicked(QTreeWidgetItem *item, int /* column */ )
{
    if (item)
    {
        QString repository_root;
        QString file_path_part;
        if (item->childCount())
        {
            toggle_expand_item(item);
        }
        else
        {
            if (ui->ckSearchResultsAsSearchTree->isChecked())
            {
                QStringList list;
                auto function = [&list] (QTreeWidgetItem *item)
                {
                    list.append(item->text(0));
                };
                QTreeWidgetItem *parent = getTopLevelItem(*ui->treeFindText, item, function);
                if (parent)
                {
                    repository_root = parent->text(FindColumn::FoundTextLine);
#ifdef __linux__
                    if (repository_root.isEmpty())
                    {
                        file_path_part = "/";
                    }
#endif
                    for (int i = list.size()-1; i>=0; --i)
                    {
                        file_path_part += list[i];
                        if (i != 0) file_path_part += "/";
                    }
                }
            }
            else
            {
                QTreeWidgetItem *parent = item->parent();
                if (parent)
                {
                    repository_root = parent->text(FindColumn::FoundTextLine);
#ifdef __linux__
                    if (repository_root.isEmpty())
                    {
                        file_path_part = "/";
                    }
#endif
                    file_path_part += item->text(FindColumn::FilePath);
                }
            }
            if (repository_root.size())
            {
                open_file(repository_root + "/" + file_path_part, item->text(FindColumn::Line).toInt());
            }
            else
            {
                open_file(file_path_part, item->text(FindColumn::Line).toInt());
                for (int i=0; i<ui->treeSource->topLevelItemCount(); ++i)
                {
                    const QString text = ui->treeSource->topLevelItem(i)->text(0);
                    if (file_path_part.indexOf(text) == 0)
                    {
                        repository_root = text;
                        file_path_part = file_path_part.right(file_path_part.size() - text.size() - 1);
                        break;
                    }
                }
            }

            ui->treeSource->find_item(repository_root, file_path_part);
        }
    }
}

void MainWindow::on_treeFindText_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    mActions.fillContextMenue(menu, Cmd::mContextMenuFindTextTree);
    menu.exec(ui->treeFindText->mapToGlobal(pos) + menu_offset);
}

void MainWindow::on_ckTypeConverter_stateChanged(int arg1)
{
    mBinaryValuesView->receive_external_data(!arg1);
    showDockedWidget(mBinaryValuesView.data(), !arg1);
}

void MainWindow::on_spinTabulator_valueChanged(int width)
{
    ui->textBrowser->setTabStopWidth(width);
}

void MainWindow::on_comboTabPosition_currentIndexChanged(int index)
{
    setTabPosition(Qt::RightDockWidgetArea, static_cast<QTabWidget::TabPosition>(index));
}

