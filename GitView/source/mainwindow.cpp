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
#include "palettecolorselector.h"

#include <QDateTime>
#include <QAction>
#include <QMessageBox>
#include <QSettings>
#include <QKeyEvent>
#include <QSysInfo>
#include <QMenu>
#include <QToolBar>
#include <QStyleFactory>
#include <QPalette>
#include <QTextStream>
#include <QInputDialog>
#include <QStaticText>
#include <QWhatsThis>
#include <QFontDatabase>
#include <QFileDialog>
#include <QSplitter>
#include <QActionGroup>
#include <QSystemTrayIcon>


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && CORE5COMPAT == 0
#include <QStringConverter>
#else
#include <QTextCodec>
#include <QScreen>
#endif

#ifdef WEB_ENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#else
#include <QTextBrowser>
#endif

#ifdef USE_BOOST
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;
#else
using namespace std::placeholders;
#endif

#define RELATIVE_GIT_PATH 1


using namespace std;
using namespace git;

namespace config
{
constexpr char sGroupFilter[]             = "Filter";
constexpr char sGroupView[]               = "View";
constexpr char sGroupPaths[]              = "Paths";
constexpr char sSourcePath[]              = "Source";
//constexpr char sUncheckedPath[] = "Unchecked";
constexpr char sGroupLogging[]            = "Logging";
constexpr char sGroupGitCommands[]        = "GitCommands";
constexpr char sGroupFind[]               = "Find";
constexpr char sCommands[]                = "Commands";
constexpr char sCommand[]                 = "Command";
constexpr char sID[]                      = "ID";
constexpr char sName[]                    = "Name";
constexpr char sTooltip[]                 = "Tooltip";
constexpr char sCustomMessageBoxText[]    = "MessageBoxText";
constexpr char sCustomCommandPostAction[] = "PostAction";
constexpr char sFlags[]                   = "Flags";
constexpr char sFlagsEnabled[]            = "FlagsEnabled";
constexpr char sFlagsDisabled[]           = "FlagsDisabled";
constexpr char sIconPath[]                = "IconPath";
constexpr char sMenuStringList[]          = "MenuStringList";
constexpr char sShortcut[]                = "Shortcut";
//constexpr char sModified[] = "Modified";
constexpr char sMessagePattern[]          = "MessagePattern";
constexpr char sFileNamePosition[]        = "FilePosition";
constexpr char sLinePosition[]            = "LinePosition";
constexpr char sMessageTextPosition[]     = "MessageTextPosition";
constexpr char sFilterPattern[]           = "FilterPattern";
constexpr char sSearch[]                  = "Search";
constexpr char sReplace[]                 = "Replace";
constexpr char Cmd__mToolbars[]           = "Cmd__mToolbars_%1";
constexpr char Cmd__mToolbarName[]        = "Cmd__mToolbarName_%1";
} // namespace config

MainWindow::MainWindow(const QString& aConfigName, QWidget *parent)
    : QMainWindow(parent)
    , mDockedWidgetMinMaxButtons(true)
    , ui(new Ui::MainWindow)
    , mWorker(this)
    , mCurrentTask(Work::None)
    , mActions(this)
    , mConfigFileName(aConfigName)
    , mStylePath( "/opt/tools/git_view/style.qss")
    #ifdef __linux__
    , mFileCopyMimeType("x-special/mate-copied-files")
    /// x-special/mate-copied-files
    /// x-special/mate-clipboard
    /// x-special/nautilus-copied-files
    /// x-special/nautilus-clipboard
    /// x-special/gnome-copied-files
    /// x-special/gnome-clipboard
    /// use-legacy-clipboard
    , mExternalFileOpenCmd("xdg-open")
    #else
    , mFileCopyMimeType("text/plain")
    /// https://learn.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/aa359736(v=vs.85)
    , mExternalFileOpenCmd("start")
    #endif
    , mFindGrep("grep")
    , mFindFsrc("fsrc")
    , mCompare2Items("meld %1 %2")
    , mWarnOpenFileSize(1024*1024) // 1MB
    , mBranchHasSiblingsNotAdjoins(":/resource/24X24/stylesheet-vline.png")
    , mBranchHasSiblingsAdjoins(":/resource/24X24/stylesheet-branch-more.png")
    , mBranchHasChildrenNotHasSiblingsAdjoins(":/resource/24X24/stylesheet-branch-end.png")
    , mBranchClosedHasChildrenHasSibling(":/resource/24X24/stylesheet-branch-closed.png")
    , mBranchOpenHasChildrenHasSibling(":/resource/24X24/stylesheet-branch-open.png")
{
    ui->setupUi(this);
    createDockWindows();

    auto icon = QIcon(":/resource/logo@2x.png");
    setWindowIcon(icon);
    mSystemTrayMessage = new QSystemTrayIcon(icon, this);

#ifdef USE_BOOST
    mWorker.setWorkerFunction(boost::bind(&MainWindow::handleWorker, this, _1));
    mWorker.setMessageFunction(boost::bind(&MainWindow::handleMessage, this, _1));
#else
    mWorker.setWorkerFunction(std::bind(&MainWindow::handleWorker, this, _1));
    mWorker.setMessageFunction(std::bind(&MainWindow::handleMessage, this, _1));
#endif
    QObject::connect(this, SIGNAL(doWork(QVariant)), &mWorker, SLOT(doWork(QVariant)));
    connect(ui->textBrowser, SIGNAL(text_of_active_changed(bool)), this, SLOT(textBrowserChanged(bool)));
    ui->textBrowser->set_active(true);
    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)), this, SLOT(on_DockWidgetActivated(QDockWidget*)));

    QString config_filename = getConfigName();
    bool config_exists = QFileInfo(config_filename).exists();
    QSettings fSettings(config_filename, QSettings::NativeFormat);

    fSettings.beginGroup(config::sGroupView);
    {
        LOAD_STR(fSettings, mBranchHasSiblingsNotAdjoins, toString);
        LOAD_STR(fSettings, mBranchHasSiblingsAdjoins, toString);
        LOAD_STR(fSettings, mBranchHasChildrenNotHasSiblingsAdjoins, toString);
        LOAD_STR(fSettings, mBranchClosedHasChildrenHasSibling, toString);
        LOAD_STR(fSettings, mBranchOpenHasChildrenHasSibling, toString);
    }
    fSettings.endGroup();

    static const QString style_sheet_treeview_lines = tr(
            "QTreeView::branch:has-siblings:!adjoins-item {"
            "    border-image: url(%1) 0; }"
            "QTreeView::branch:has-siblings:adjoins-item {"
            "    border-image: url(%2) 0; }"
            "QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
            "    border-image: url(%3) 0; }"
            "QTreeView::branch:has-children:!has-siblings:closed,"
            "QTreeView::branch:closed:has-children:has-siblings { "
            "    border-image: none; image: url(%4); }"
            "QTreeView::branch:open:has-children:!has-siblings,"
            "QTreeView::branch:open:has-children:has-siblings  { "
            "    border-image: none; image: url(%5); "
            "}").arg(mBranchHasSiblingsNotAdjoins,
                     mBranchHasSiblingsAdjoins,
                     mBranchHasChildrenNotHasSiblingsAdjoins,
                     mBranchClosedHasChildrenHasSibling,
                     mBranchOpenHasChildrenHasSibling);

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
    connect(ui->treeSource, SIGNAL(compare_items(QString&,QString&)), this, SLOT(compare_items(QString&,QString&)));

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
    connect(ui->ckShowLineNumbers, SIGNAL(toggled(bool)), this, SLOT(set_show_line_numbers(bool)));
    connect(ui->treeHistory, SIGNAL(show_me(QWidget*)), this, SLOT(showDockedWidget(QWidget*)));
    connect(ui->treeBranches, SIGNAL(insertFileNames(QTreeWidgetItem*,int,int)), ui->treeHistory, SLOT(insertFileNames(QTreeWidgetItem*,int,int)));

    /// add status labels
    m_status_line_label = new QLabel("");
    m_status_line_label->setToolTip(tr("Line"));
    ui->statusBar->addPermanentWidget(m_status_line_label);
    connect(ui->textBrowser, SIGNAL(line_changed(int)), m_status_line_label, SLOT(setNum(int)));
    connect(ui->tableBinaryView, &qbinarytableview::cursor_changed, [&] (int) { m_status_line_label->setText(""); });

    m_status_column_label = new QLabel("");
    m_status_column_label->setToolTip(tr("Column/Position"));
    ui->statusBar->addPermanentWidget(m_status_column_label);
    connect(ui->textBrowser, SIGNAL(column_changed(int)), m_status_column_label, SLOT(setNum(int)));
    connect(ui->tableBinaryView, SIGNAL(cursor_changed(int)), m_status_column_label, SLOT(setNum(int)));

    connect(ui->btnCloseText, SIGNAL(clicked()), this, SLOT(btnCloseText_clicked()));
    connect(ui->btnStoreText, SIGNAL(clicked()), this, SLOT(btnStoreText_clicked()));

    initCodecCombo();

    fSettings.beginGroup(config::sGroupFilter);
    {
        LOAD_PTR(fSettings, ui->ckHiddenFiles, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckSymbolicLinks, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckSystemFiles, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFiles, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckDirectories, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckIgnoredFiles, setChecked, isChecked, toBool);
        read_filter(fSettings);
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFind);
    {
        LOAD_STR(fSettings, mSearchResultsAsSearchTree, toBool);
        LOAD_STR(fSettings, mFastFileSearch, toBool);
        LOAD_PTR(fSettings, ui->ckFindCaseSensitive, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFindRegEx, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->ckFindWholeWord, setChecked, isChecked, toBool);
        LOAD_PTR(fSettings, ui->comboFindBox, setCurrentIndex, currentIndex, toInt);
        LOAD_STR(fSettings, mFindGrep, toString);
        LOAD_STR(fSettings, mFindFsrc, toString);
        LOAD_STR(fSettings, mCompare2Items, toString);
    }
    fSettings.endGroup();

    comboFindBoxIndexChanged(ui->comboFindBox->currentIndex());

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
        uint fToolBars = static_cast<uint>(Cmd::mToolbars.size());
        LOAD_STR(fSettings, fToolBars, toInt);
        for (std::uint32_t i=0; i<fToolBars; ++i)
        {
            if (i >= Cmd::mToolbars.size())
            {
                Cmd::mToolbars.push_back({});
                QString name = tr(config::Cmd__mToolbarName).arg(i);
                Cmd::mToolbarNames.push_back(fSettings.value(name).toString());
            }
            auto& tool_bar = Cmd::mToolbars[i];
            QString name = tr(config::Cmd__mToolbars).arg(i);
            LOAD_STRFN(fSettings, tool_bar, name.toStdString().c_str(), Cmd::fromString, Cmd::toString, toString);
        }
        LOAD_STRF(fSettings, mMergeTools, Cmd::fromStringMT, Cmd::toStringMT, toString);

        initContextMenuActions();

        QString fTheme;
        LOAD_STR(fSettings, fTheme, toString);
        mActions.setTheme(fTheme);
        mActions.initActionIcons();
        LOAD_STR(fSettings, mInitOnlyCustomCommands, toBool);
    }
    read_commands(fSettings);
    fSettings.endGroup();

    for (uint i=0; i < Cmd::mToolbars.size(); ++i)
    {
        addCmdToolBar(i);
    }

    insertTopToolBars();
    insertBottomToolBars();

    fSettings.beginGroup(config::sGroupView);
    {
        LOAD_PTR(fSettings, ui->ckHideEmptyParent, setChecked, isChecked, toBool);
        update_widget_states(ui->ckHideEmptyParent);
        LOAD_STR(fSettings, Type::mShort, toBool);
        ui->ckShortState->setChecked(Type::mShort);
        ui->treeSource->header()->setSectionHidden(QSourceTreeWidget::Column::Size    , fSettings.value("SourceTreeWidgetColumnSize"    , ui->treeSource->header()->isSectionHidden(QSourceTreeWidget::Column::Size)).toBool());
        ui->treeSource->header()->setSectionHidden(QSourceTreeWidget::Column::DateTime, fSettings.value("SourceTreeWidgetColumnDateTime", ui->treeSource->header()->isSectionHidden(QSourceTreeWidget::Column::DateTime)).toBool());

        update_widget_states(ui->ckShortState);
        {
            QFontDatabase font_db;
            //QFontDatabase::families();
            auto fonts = font_db.families();
            for (const auto& font : std::as_const(fonts))
            {
                ui->comboFontName->addItem(font);
            }
            ui->comboFontName->setCurrentIndex(ui->comboFontName->findText("Courier", Qt::MatchContains));
            LOAD_PTR(fSettings, ui->comboFontName, setCurrentText, currentText, toString);
            LOAD_PTR(fSettings, ui->spinFontSize, setValue, value, toInt);
            connect(ui->comboFontName, SIGNAL(currentIndexChanged(int)), this, SLOT(setFontForViews(int)));
            connect(ui->spinFontSize, SIGNAL(valueChanged(int)), this, SLOT(setFontForViews(int)));
            setFontForViews(0);
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
        LOAD_STR(fSettings, mDefaultSourcePath, toString);
        QString fTypeFormatFilesLocation;
        LOAD_STR(fSettings, fTypeFormatFilesLocation, toString);
        mBinaryValuesView->m_type_format_files_location = fTypeFormatFilesLocation;
        QString fExternalFileOpenExt;
        LOAD_STR(fSettings, fExternalFileOpenExt, toString);
        if (fExternalFileOpenExt.size())
        {
            const auto extensions = fExternalFileOpenExt.split(",");
            for (const auto&extension : std::as_const(extensions))
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
        connect(ui->comboAppStyle, SIGNAL(currentTextChanged(QString)), this, SLOT(comboAppStyleTextChanged(QString)));

        LOAD_STR(fSettings, mStylePath, toString);
        bool fUseSourceTreeCheckboxes = ui->treeSource->mUseSourceTreeCheckboxes;
        LOAD_STR(fSettings, fUseSourceTreeCheckboxes, toBool);
        ui->treeSource->mUseSourceTreeCheckboxes = fUseSourceTreeCheckboxes;
        LOAD_STR(fSettings, mExperimental, toBool);
        LOAD_PTR(fSettings, ui->comboTabPosition, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->ckShowLineNumbers, setChecked, isChecked, toBool);
        update_widget_states(ui->ckShowLineNumbers);
        set_show_line_numbers(ui->ckShowLineNumbers->isChecked());
        LOAD_PTR(fSettings, ui->ckRenderGraphicFile, setChecked, isChecked, toBool);
        update_widget_states(ui->ckRenderGraphicFile);
        LOAD_STR(fSettings, mCloseAllFilesOfRepository, toBool);
        LOAD_STR(fSettings, mOutput2secondTextView, toBool);
        LOAD_STR(fSettings, mAppendToBatch, toBool);
        mWorker.setAppendToBatch(mAppendToBatch);
        LOAD_PTR(fSettings, ui->comboToolBarStyle, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->comboAppStyle, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->comboUserStyle, setCurrentIndex, currentIndex, toInt);
        LOAD_PTR(fSettings, ui->comboWordWrap, setCurrentIndex, currentIndex, toInt);
        ui->comboOpenNewEditor->setCurrentIndex(INT(AdditionalEditor::OnNewFile));
        LOAD_PTR(fSettings, ui->comboOpenNewEditor, setCurrentIndex, currentIndex, toInt);
        on_comboOpenNewEditor_currentIndexChanged(0);
        QString fDarkPaletteColors;
        LOAD_STR(fSettings, fDarkPaletteColors, toString);
        PaletteColorSelector::init_dark_palette();
        if (!fDarkPaletteColors.isEmpty())
        {
            PaletteColorSelector::set_dark_palette_colors(fDarkPaletteColors);
        }
        LOAD_PTR(fSettings, mSystemTrayMessage, setVisible, isVisible, toBool);

        setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(ui->comboToolBarStyle->currentIndex()));
        comboAppStyleTextChanged(ui->comboAppStyle->currentText());
        on_comboUserStyle_currentIndexChanged(ui->comboUserStyle->currentIndex());

        auto fTextTabStopCharacters = ui->textBrowser->getTabstopCharacters();
        LOAD_STR(fSettings, fTextTabStopCharacters, toInt);
        ui->textBrowser->setTabstopCharacters(fTextTabStopCharacters);
        ui->textBrowser->setWordWrapMode(static_cast<QTextOption::WrapMode>(ui->comboWordWrap->currentIndex()));
        ui->spinTabulator->setValue(fTextTabStopCharacters);

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
        connect(ui->comboTabPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(comboTabPositionIndexChanged(int)));
        comboTabPositionIndexChanged(ui->comboTabPosition->currentIndex());
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
    connect(ui->tableBinaryView, SIGNAL(contentChanged(bool)), this, SLOT(textBrowserChanged(bool)));

#ifdef WEB_ENGINE
    mWebEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    PreviewPage* page = new PreviewPage(this, mWebEngineView.data());
    connect(page, SIGNAL(open_link(QString)), this, SLOT(open_external_link(QString)));

    mWebEngineView->setPage(page);
    ui->textBrowser->set_page(page);

    m_markdown_proxy.reset(new MarkdownProxy);
    connect(ui->textBrowser, SIGNAL(text_changed(QString)), m_markdown_proxy.data(), SLOT(setText(QString)));

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), m_markdown_proxy.data());
    page->setWebChannel(channel);
#else
    connect(ui->textBrowser, SIGNAL(show_web_view(bool)), this, SLOT(show_web_view(bool)));
    ui->textBrowser->set_page(mTextRenderView.data());
#endif

    m_initializing_elements = false;
    TRACEX(Logger::info, windowTitle() << " Started");
    init_miscelaneous_items();

    delete ui->topLayout;

    if (!config_exists) /// NOTE: initialize program at the first time
    {
        int width = rect().width();
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            width =  screen->geometry().width();
            if (width > 1600)
            {
                ui->comboToolBarStyle->setCurrentIndex(3);
            }
        }
        showMaximized();
        centralWidget()->setFixedWidth(width / 4);
        m_initializing_elements = true;
    }
    startTimer(100);
}

void MainWindow::set_app_path(const QString &path)
{
    mAppPath = path;
}

void MainWindow::update_widget_states(QWidget *widget)
{
    auto list = widget->actions();
    if (list.count())
    {
        const QAbstractButton*button  = dynamic_cast<QAbstractButton*>(widget);
        // const QComboBox*      combobox= dynamic_cast<QComboBox*>(widget);
        if (button && button->isCheckable())
        {
            list[0]->setChecked(button->isChecked());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (btnCloseText_clicked(Editor::All))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

MainWindow::~MainWindow()
{
    disconnect(ui->textBrowser, SIGNAL(text_of_active_changed(bool)), this, SLOT(textBrowserChanged(bool)));
    showDockedWidget(mBinaryValuesView.data(), false);

    store_settings();

    delete ui;

    if (mRestartApp)
    {
        QString command;
        QString result;
#ifdef __linux__
        command = "exec " + mAppPath + " &";
#else
        command = "start " + mAppPath;
#endif
        execute(command, result);
    }
}

void MainWindow::store_settings()
{
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);
    fSettings.beginGroup(config::sGroupFilter);
    {
        STORE_PTR(fSettings, ui->ckHiddenFiles, isChecked);
        STORE_PTR(fSettings, ui->ckSymbolicLinks, isChecked);
        STORE_PTR(fSettings, ui->ckSystemFiles, isChecked);
        STORE_PTR(fSettings, ui->ckFiles, isChecked);
        STORE_PTR(fSettings, ui->ckDirectories, isChecked);
        STORE_PTR(fSettings, ui->ckIgnoredFiles, isChecked);
        store_filter(fSettings);
    }
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupFind);
    {
        STORE_STR(fSettings, mSearchResultsAsSearchTree);
        STORE_STR(fSettings, mFastFileSearch);
        STORE_PTR(fSettings, ui->ckFindCaseSensitive, isChecked);
        STORE_PTR(fSettings, ui->ckFindRegEx, isChecked);
        STORE_PTR(fSettings, ui->ckFindWholeWord, isChecked);
        STORE_PTR(fSettings, ui->comboFindBox, currentIndex);
        STORE_STR_IF(fSettings, mFindGrep);
        STORE_STR_IF(fSettings, mFindFsrc);
        STORE_STR_IF(fSettings, mCompare2Items);
    }
    fSettings.endGroup();

    Highlighter::Language::store(fSettings);

    fSettings.beginGroup(config::sGroupView);
    {
        fSettings.setValue("SourceTreeWidgetColumnSize"    , ui->treeSource->header()->isSectionHidden(QSourceTreeWidget::Column::Size    ));
        fSettings.setValue("SourceTreeWidgetColumnDateTime", ui->treeSource->header()->isSectionHidden(QSourceTreeWidget::Column::DateTime));

        STORE_STR_IF(fSettings, mBranchHasSiblingsNotAdjoins);
        STORE_STR_IF(fSettings, mBranchHasSiblingsAdjoins);
        STORE_STR_IF(fSettings, mBranchHasChildrenNotHasSiblingsAdjoins);
        STORE_STR_IF(fSettings, mBranchClosedHasChildrenHasSibling);
        STORE_STR_IF(fSettings, mBranchOpenHasChildrenHasSibling);

        STORE_PTR(fSettings, ui->ckHideEmptyParent, isChecked);
        STORE_STR(fSettings,  Type::mShort);
        STORE_PTR(fSettings, ui->comboFontName, currentText);
        STORE_PTR(fSettings, ui->spinFontSize, value);
        {
            bool fDifferentEndian = CDisplayType::getDifferentEndian();
            STORE_STR(fSettings, fDifferentEndian);
            int fBinaryDisplayType = ui->tableBinaryView->get_type();
            STORE_STR(fSettings, fBinaryDisplayType);
            int fBinaryDisplayColumns = ui->tableBinaryView->get_columns();
            STORE_STR(fSettings, fBinaryDisplayColumns);
        }

        if (mWarnOpenFileSize) STORE_STR(fSettings, mWarnOpenFileSize);
        STORE_STR_IF(fSettings, mFileCopyMimeType);
        STORE_STR(fSettings, mExternalIconFiles);
        STORE_STR_IF(fSettings, mExternalFileOpenCmd);
        STORE_STR(fSettings, mDefaultSourcePath);
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

        STORE_STR_IF(fSettings, mStylePath);
        bool fUseSourceTreeCheckboxes = ui->treeSource->mUseSourceTreeCheckboxes;
        STORE_STR(fSettings, fUseSourceTreeCheckboxes);
        STORE_STR(fSettings, mExperimental);
        STORE_PTR(fSettings, ui->comboTabPosition, currentIndex);
        STORE_PTR(fSettings, ui->ckShowLineNumbers, isChecked);
        STORE_PTR(fSettings, ui->ckRenderGraphicFile, isChecked);
        STORE_STR(fSettings, mCloseAllFilesOfRepository);
        STORE_STR(fSettings, mOutput2secondTextView);
        STORE_STR(fSettings, mAppendToBatch);
        STORE_PTR(fSettings, ui->comboToolBarStyle, currentIndex);
        STORE_PTR(fSettings, ui->comboAppStyle, currentIndex);
        STORE_PTR(fSettings, ui->comboUserStyle, currentIndex);
        STORE_PTR(fSettings, ui->comboWordWrap, currentIndex);
        STORE_PTR(fSettings, ui->comboOpenNewEditor, currentIndex);
        QString fDarkPaletteColors = PaletteColorSelector::get_dark_palette_colors();
        STORE_STR(fSettings, fDarkPaletteColors);
        auto fTextTabStopCharacters = ui->textBrowser->getTabstopCharacters();
        STORE_STR(fSettings, fTextTabStopCharacters);
        STORE_PTR(fSettings, mSystemTrayMessage, isVisible);

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
        uint fToolBars = static_cast<uint>(Cmd::mToolbars.size());
        STORE_STR(fSettings, fToolBars);
        for (std::uint32_t i=0; i<fToolBars; ++i)
        {
            auto& tool_bar = Cmd::mToolbars[i];
            QString name = tr(config::Cmd__mToolbars).arg(i);
            STORE_STRFN(fSettings, tool_bar, name.toStdString().c_str(), Cmd::toString);
            name = tr(config::Cmd__mToolbarName).arg(i);
            fSettings.setValue(name, Cmd::mToolbarNames[i]);
        }
        STORE_STRF(fSettings, mMergeTools, Cmd::toStringMT);
        mInitOnlyCustomCommands = false;
        STORE_STR(fSettings, mInitOnlyCustomCommands);

        const QString& fTheme = mActions.getTheme();
        STORE_STR(fSettings, fTheme);
    }

    store_commands(fSettings);
    fSettings.endGroup();

    fSettings.beginGroup(config::sGroupLogging);
    {
        QString fSeverHlp = "_fscb___acewnidt";
        STORE_STR(fSettings, fSeverHlp);
        QString fSeverity = QString::number(Logger::getSeverity(), 2);
        STORE_STR(fSettings, fSeverity);
    }
    fSettings.endGroup();
}

void MainWindow::read_commands(QSettings& fSettings)
{
    int item_count = fSettings.beginReadArray(config::sCommands);

    for (int item = 0; item < item_count; ++item)
    {
        fSettings.setArrayIndex(item);
        Cmd::eCmd cmd = static_cast<Cmd::eCmd>(fSettings.value(config::sID).toUInt());
        auto* action = mActions.getAction(cmd);
        if (action)
        {
            if (mInitOnlyCustomCommands)
            {
                continue;
            }
        }
        else
        {
            action = mActions.createAction(cmd, txt::New, txt::git);
        }
        action->setText(fSettings.value(config::sName).toString());
        action->setToolTip(fSettings.value(config::sTooltip).toString());
        action->setStatusTip(fSettings.value(config::sCommand).toString());
        action->setShortcut(QKeySequence(fSettings.value(config::sShortcut).toString()));
        uint flags = fSettings.value(config::sFlags).toUInt();
        if (flags & ActionList::Flags::Custom)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(perform_custom_command()));
        }
        mActions.setFlags(cmd, flags, Flag::replace);
        mActions.setFlags(cmd, fSettings.value(config::sFlagsEnabled).toUInt(),  Flag::replace, ActionList::Data::StatusFlagEnable);
        mActions.setFlags(cmd, fSettings.value(config::sFlagsDisabled).toUInt(), Flag::replace, ActionList::Data::StatusFlagDisable);
        mActions.setIconPath(cmd, fSettings.value(config::sIconPath).toString());
        QStringList string_list = fSettings.value(config::sMenuStringList).toStringList();
        if (string_list.size() > 1) mActions.setMenuStringList(cmd, string_list);
        mActions.setCustomCommandMessageBoxText(cmd, fSettings.value(config::sCustomMessageBoxText).toString());
        mActions.setCustomCommandPostAction(cmd, fSettings.value(config::sCustomCommandPostAction).toUInt());
        if (!action->shortcut().isEmpty())
        {
            add_action_to_widgets(action);
        }
    }
    fSettings.endArray();
}

void MainWindow::store_commands(QSettings& fSettings, const QList<git::Cmd::eCmd>& commands)
{
    fSettings.beginWriteArray(config::sCommands);
    int fIndex = 0;

    for (const auto& fItem : std::as_const(mActions.getList()))
    {
        const Cmd::eCmd fCmd = static_cast<Cmd::eCmd>(fItem.first);
        if (commands.size())
        {
            if (commands.size() == 2 && commands[0] < commands[1])
            {
                if (!is_in_range(commands[0], commands[1], fCmd)) continue;
            }
            else if (!commands.contains(fCmd)) continue;
        }

        if (mActions.getFlags(fCmd) & ActionList::Flags::Modified)
        {
            const QAction* fAction = fItem.second;
            QString fCommand = fAction->statusTip();
            if (fCommand.size())
            {
                fSettings.setArrayIndex(fIndex++);
                fSettings.setValue(config::sID, fItem.first);
                fSettings.setValue(config::sName, fAction->text());
                fSettings.setValue(config::sTooltip, fAction->toolTip());
                fSettings.setValue(config::sCommand, fCommand);
                fSettings.setValue(config::sShortcut, fAction->shortcut().toString());
                fSettings.setValue(config::sCustomMessageBoxText, mActions.getCustomCommandMessageBoxText(fCmd));
                fSettings.setValue(config::sCustomCommandPostAction, mActions.getCustomCommandPostAction(fCmd));
                fSettings.setValue(config::sFlags, mActions.getFlags(fCmd));
                fSettings.setValue(config::sFlagsEnabled, mActions.getFlags(fCmd, ActionList::Data::StatusFlagEnable));
                fSettings.setValue(config::sFlagsDisabled, mActions.getFlags(fCmd, ActionList::Data::StatusFlagDisable));
                fSettings.setValue(config::sIconPath, mActions.getIconPath(fCmd));
                QStringList list = mActions.getMenuStringList(fCmd);
                if (list.empty()) list.append("");
                fSettings.setValue(config::sMenuStringList, list);
            }
        }
    }
    fSettings.endArray();
}

void MainWindow::read_filter(QSettings &fSettings)
{
    int item_count = fSettings.beginReadArray(config::sFilterPattern);
    for (int item = 0; item < item_count; ++item)
    {
        fSettings.setArrayIndex(item);
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QPair<QRegularExpression, QString> value;
#else
            QPair<QRegExp, QString> value;
#endif
            value.first.setPattern(fSettings.value(config::sSearch).toString());
            value.second = fSettings.value(config::sReplace).toString();
            mFilterPatterns.append(value);
        }
    }
    fSettings.endArray();

    item_count = fSettings.beginReadArray(config::sMessagePattern);
    for (int item = 0; item < item_count; ++item)
    {
        fSettings.setArrayIndex(item);
        {
            QSharedPointer<ParseMessagePattern> value(new ParseMessagePattern);
            value->set_pattern(fSettings.value(config::sSearch).toString());
            value->set_filename_position(fSettings.value(config::sFileNamePosition).toInt());
            value->set_line_position(fSettings.value(config::sLinePosition).toInt());
            value->set_message_text_position(fSettings.value(config::sMessageTextPosition).toInt());
            mMessagePatterns.append(value);
        }
    }
    fSettings.endArray();
}

void MainWindow::store_filter(QSettings &fSettings)
{
    fSettings.beginWriteArray(config::sFilterPattern);
    int fIndex = 0;
    for (const auto& pattern : mFilterPatterns)
    {
        fSettings.setArrayIndex(fIndex++);
        fSettings.setValue(config::sSearch, pattern.first.pattern());
        fSettings.setValue(config::sReplace, pattern.second);
    }
    fSettings.endArray();

    fSettings.beginWriteArray(config::sMessagePattern);
    fIndex = 0;
    for (const auto& pattern : mMessagePatterns)
    {
        fSettings.setArrayIndex(fIndex++);
        fSettings.setValue(config::sSearch, pattern->get_pattern());
        fSettings.setValue(config::sFileNamePosition, pattern->get_filename_position());
        fSettings.setValue(config::sLinePosition, pattern->get_line_position());
        fSettings.setValue(config::sMessageTextPosition, pattern->get_message_text_position());
    }
    fSettings.endArray();
}

void MainWindow::read_custom_commands(const QString &file_name)
{
    QSettings settings(file_name, QSettings::NativeFormat);
    read_commands(settings);
}

void MainWindow::store_custom_commands(const QString& file_name)
{
    QSettings settings(file_name, QSettings::NativeFormat);
    store_commands(settings, {git::Cmd::CustomCommand, git::Cmd::CustomTestCommand} );
}

void MainWindow::createDockWindows()
{
    /// use initialized widgets from forms user interface
    /// and redirect user interface widgets to docked layout
    /// - first remove the widgets from layout items
    /// - add widgets
    ///   - to central widget
    ///   - to dock widgets
    ///   - to toolbars
    /// NOTE: reguard future items, if any

    /// git files view as central widget
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
    QDockWidget *first_tab = nullptr;
    dock = create_dock_widget(ui->textBrowser, tr("Text View/Editor"), textbrowser);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->comboFindBox->addItem(dock->windowTitle());
    ui->comboFindBox->addItem(tr("Go to line"));
    ui->comboFindBox->addItem(tr("Repository View"));
    ui->verticalLayout_2->removeWidget(ui->textBrowser);
    first_tab = dock;

    // graphics view
    dock = create_dock_widget(ui->graphicsView, tr("Graphics View"), graphicsviewer);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->verticalLayout_2->removeWidget(ui->graphicsView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    tabifyDockWidget(first_tab, dock);

    // binary table view
    dock = create_dock_widget(ui->tableBinaryView, tr("Binary View"), binary_table_view);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->verticalLayout->removeWidget(ui->tableBinaryView);
    tabifyDockWidget(first_tab, dock);


#ifdef WEB_ENGINE
    // markdown view
    mWebEngineView.reset(new QWebEngineView(this));
    dock = create_dock_widget(mWebEngineView.data(), tr("Html and Markdown"), markdown_view, true, Qt::Horizontal);
#else
    mTextRenderView.reset(new QTextBrowser(this));
    dock = create_dock_widget(mTextRenderView.data(), tr("Html and Markdown"), markdown_view, true, Qt::Horizontal);
    mTextRenderView->setOpenExternalLinks(true);
    mTextRenderView->setReadOnly(true);
#endif
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    dock->setVisible(false);

    // history tree
    dock = create_dock_widget(ui->treeHistory, tr("History View"), historyview);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->comboFindBox->addItem(dock->windowTitle());
    ui->verticalLayout->removeWidget(ui->treeHistory);
    first_tab = dock;

    // branch tree
    dock = create_dock_widget(ui->treeBranches, tr("Branch View"), branchview);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->comboFindBox->addItem(dock->windowTitle());
    ui->verticalLayout->removeWidget(ui->treeBranches);
    tabifyDockWidget(first_tab, dock);

    // stash tree
    dock = create_dock_widget(ui->treeStash, tr("Stash View"), stashview);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->comboFindBox->addItem(dock->windowTitle());
    ui->verticalLayout->removeWidget(ui->treeStash);
    tabifyDockWidget(first_tab, dock);

    // find tree
    dock = create_dock_widget(ui->treeFindText, tr("Found in Text Files"), findview);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    ui->comboFindBox->addItem(dock->windowTitle() + tr(" View"));
    ui->verticalLayout->removeWidget(ui->treeFindText);
    tabifyDockWidget(first_tab, dock);

    mBinaryValuesView.reset(new binary_values_view(this));
    dock = create_dock_widget(mBinaryValuesView.data(), tr("Binary Values"), binaryview);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_tree_view(QDockWidgetX*,bool&)));
    tabifyDockWidget(first_tab, dock);
    dock->setVisible(false);

    ui->comboFindBox->addItem(tr("Execute Git Command"));

    mDockAreaNames.append(tr("Left"));
    mDockAreaNames.append(tr("Right"));
    mDockAreaNames.append(tr("Bottom"));

    connect(ui->comboFindBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboFindBoxIndexChanged(int)));
    // - remove obsolete layout
    // NOTE: regard future layout items, if any
    ui->topLayout = nullptr;
    ui->horizontalLayout = nullptr;
    ui->horizontalLayoutHistoryAndText  = nullptr;
    ui->horizontalLayoutTool = nullptr;
    ui->verticalLayout = nullptr;
    ui->verticalLayout_2 = nullptr;
    ui->verticalLayoutForTextBrowser = nullptr;
    ui->verticalLayoutForTreeView = nullptr;
}

void MainWindow::addCmdToolBar(int i)
{
    const auto& fToolbar = Cmd::mToolbars[i];
    QToolBar*pTB = new QToolBar(Cmd::mToolbarNames[i]);
    mToolBars.push_back(pTB);
    mActions.fillToolbar(*pTB, fToolbar);
    pTB->setObjectName(Cmd::mToolbarNames[i]);
    pTB->setIconSize(QSize(24,24));
    addToolBar(i==0 ? Qt::LeftToolBarArea : Qt::TopToolBarArea, pTB);
}

void MainWindow::insertTopToolBars()
{
    addToolBarBreak(Qt::TopToolBarArea);

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
    ui->horizontalLayoutForTreeViewHead = nullptr;

    pTB = new QToolBar(tr("Text Editor / Viewer Control"));
    pTB->setObjectName("textviewtoolbar");
    while ((layoutItem = ui->horizontalLayoutForTextBrowserHead->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::TopToolBarArea, pTB);
    ui->horizontalLayoutForTextBrowserHead = nullptr;
}

void MainWindow::insertBottomToolBars()
{
    QLayoutItem *layoutItem {nullptr};
    QToolBar* pTB {nullptr};

    pTB = new QToolBar(tr("Settings"));
    pTB->setObjectName("fileflagtoolbar");
    while ((layoutItem = ui->verticalLayout_Settings->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::BottomToolBarArea, pTB);
    ui->verticalLayout_Settings = nullptr;

    addToolBarBreak(Qt::BottomToolBarArea);

    pTB = new QToolBar(tr("Find"));
    pTB->setObjectName("findtoolbar");
    while ((layoutItem = ui->horizontalLayoutFind->takeAt(0)) != 0)
    {
        pTB->addWidget(layoutItem->widget());
        delete layoutItem;
    }
    addToolBar(Qt::BottomToolBarArea, pTB);
    ui->horizontalLayoutFind = nullptr;
}

void MainWindow::removeCmdToolBar(const QString& toolbar_name)
{
    for (uint i=0; i<mToolBars.size(); ++i)
    {
        if (mToolBars[i]->windowTitle() == toolbar_name)
        {
            removeToolBar(mToolBars[i]);
            delete mToolBars[i];
            mToolBars.erase(mToolBars.begin()+i);
            break;
        }
    }
}

void MainWindow::clone_code_browser()
{
    code_browser *active_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
    if (active_browser)
    {
        QString file_name = active_browser->get_file_path();
        if (file_name.length())
        {
            QFileInfo info(file_name);
            file_name = info.fileName();
        }
        else
        {
            file_name = "Cloned Editor";
        }
        /// TODO: test splited view and implement new features
        /// Splitted View
        /// o   Dupliziert als gleicher Editor?
        /// o   Scrollen synchronisierbar
        /// o   auch mit verschiedenen Dateien zum vergleichen
        ///     §  (drag and drop)?
        ///     §  Diff anzeige
        code_browser* cloned_browser = active_browser->clone();
        QDockWidget*dock = create_dock_widget(cloned_browser, file_name, cloned_textbrowser, true);
        dock->setAttribute(Qt::WA_DeleteOnClose);
        connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_text_browser(QDockWidgetX*,bool&)));
//        cloned_browser->setReadOnly(false);
//        QObject::connect(active_browser, SIGNAL(text_changed(QString)), cloned_browser, SLOT(setPlainText(QString)));
        QDockWidget* parent = get_first_dock_tab(FirstTab::web_view);
        if (parent)
        {
            tabifyDockWidget(parent, dock);
        }
        showDockedWidget(cloned_browser);
    }
}

QDockWidgetX* MainWindow::create_dock_widget(QWidget* widget, const QString& name, const QString& object_name, bool connect_dock, Qt::Orientation orientation)
{
    QDockWidgetX* dock = new QDockWidgetX(name, this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName(object_name);
    dock->setWidget(widget);
    addDockWidget(Qt::RightDockWidgetArea, dock, orientation);

    if (connect_dock)
    {
        connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidget_topLevelChanged(bool)));
    }
    return dock;
}

void MainWindow::dockWidget_topLevelChanged(bool)
{
    QDockWidget* dw = dynamic_cast<QDockWidget*>(QObject::sender());
    if (dw && mDockedWidgetMinMaxButtons)
    {
        static Qt::WindowFlags oldDocWidgetFlags = Qt::Widget;
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

QWidget* MainWindow::get_widget(QDockWidget*dock)
{
    QWidget* child_widget = dock->widget();
    QSplitter* splitter = dynamic_cast<QSplitter*>(child_widget);
    if (splitter && splitter->count())
    {
        child_widget = splitter->widget(0);
    }
    return child_widget;
}

void MainWindow::init_miscelaneous_items(bool load)
{
    static const QString compare_2_items                                = tr("Compare tool command");
    static const QString find_grep                                      = tr("Path of tool: grep");
    static const QString find_fsrc                                      = tr("Path of tool: fsrc");
    static const QString fast_file_search                               = tr("Enable tool fsrc for fast file search");
    static const QString search_results_as_tree                         = tr("Show search results as tree");
    static const QString append_to_batch                                = tr("Append background command to batch");
    static const QString output_2_second_view                           = tr("Send command output to second view");
    static const QString external_file_open_cmd                         = tr("System command open files");
    static const QString file_copy_mime_type                            = tr("Edit mime type for file copy");
    static const QString style_path                                     = tr("Edit path to style qss file");
    static const QString warn_open_file_fize                            = tr("Warn size for open file (bytes)");
    static const QString repository_tree_date                           = tr("Show repository view date");
    static const QString repository_tree_size                           = tr("Show repository view size");
    static const QString system_tray_messages                           = tr("Show messages in Systemtray");
    static const QString close_all_files_of_repository                  = tr("Close all files of repository");
    static const QString experimental                                   = tr("Use experimental functions");
    static const QString log_severity                                   = tr("Select logging severity");
    static const QString branch_has_siblings_not_adjoins                = tr("Icon: HasSiblingsNotAdjoins");
    static const QString branch_has_siblings_adjoins                    = tr("Icon: HasSiblingsAdjoins");
    static const QString branch_has_children_not_has_siblings_adjoins   = tr("Icon: HasChildrenNotHasSiblingsAdjoins");
    static const QString branch_closed_has_children_has_sibling         = tr("Icon: ClosedHasChildrenHasSibling");
    static const QString branch_open_has_children_has_sibling           = tr("Icon: OpenHasChildrenHasSibling");
#ifdef __linux__
    static const QString linux_theme                                    = tr("Edit linux theme name");
//    static const QString linux_icon_path                              = tr("Edit linux icon path");
#endif

    static const QString invalid_severity                               = Logger::getName(Logger::invalid);

    if (load)
    {
        std::uint32_t severity = Logger::getSeverity();
        string2variant_map severity_map;
        for (std::uint32_t bit=1; (bit & 0x0ffff); bit <<= 1)
        {
            severity_map[Logger::getName(static_cast<Logger::eSeverity>(bit))] = QVariant((severity & bit) != 0 ? true : false);
        }
        severity_map.remove(invalid_severity);

        mMiscelaneousItems[tr("Find words in text files")] = QVariant();
        mMiscelaneousItems[find_grep]                                    = QVariant(mFindGrep);
        mMiscelaneousItems[find_fsrc]                                    = QVariant(mFindFsrc);
        mMiscelaneousItems[fast_file_search]                             = QVariant(mFastFileSearch);
        mMiscelaneousItems[search_results_as_tree]                       = QVariant(mSearchResultsAsSearchTree);
        mMiscelaneousItems[tr("Repository parameter")] = QVariant();
        mMiscelaneousItems[repository_tree_date]                         = QVariant(!ui->treeSource->isColumnHidden(QSourceTreeWidget::Column::DateTime));
        mMiscelaneousItems[repository_tree_size]                         = QVariant(!ui->treeSource->isColumnHidden(QSourceTreeWidget::Column::Size));
        mMiscelaneousItems[close_all_files_of_repository]                = QVariant(mCloseAllFilesOfRepository);
        mMiscelaneousItems[compare_2_items]                              = QVariant(mCompare2Items);
        mMiscelaneousItems[warn_open_file_fize]                          = QVariant(mWarnOpenFileSize);
        mMiscelaneousItems[external_file_open_cmd]                       = QVariant(mExternalFileOpenCmd);
        mMiscelaneousItems[file_copy_mime_type]                          = QVariant(mFileCopyMimeType);
        mMiscelaneousItems[tr("Tree view icons")] = QVariant();
        mMiscelaneousItems[branch_has_siblings_not_adjoins]              = QVariant(mBranchHasSiblingsNotAdjoins);
        mMiscelaneousItems[branch_has_siblings_adjoins]                  = QVariant(mBranchHasSiblingsAdjoins);
        mMiscelaneousItems[branch_has_children_not_has_siblings_adjoins] = QVariant(mBranchHasChildrenNotHasSiblingsAdjoins);
        mMiscelaneousItems[branch_closed_has_children_has_sibling]       = QVariant(mBranchClosedHasChildrenHasSibling);
        mMiscelaneousItems[branch_open_has_children_has_sibling]         = QVariant(mBranchOpenHasChildrenHasSibling);
        mMiscelaneousItems[tr("Miscelaneous parameter")] = QVariant();
        mMiscelaneousItems[system_tray_messages]                         = QVariant(mSystemTrayMessage->isVisible());
        mMiscelaneousItems[append_to_batch]                              = QVariant(mAppendToBatch);
        mMiscelaneousItems[output_2_second_view]                         = QVariant(mOutput2secondTextView);
        mMiscelaneousItems[experimental]                                 = QVariant(mExperimental);
        mMiscelaneousItems[style_path]                                   = QVariant(mStylePath);

#ifdef __linux__
        string2variant_map themes_map;
        QDirIterator iter("/usr/share/icons", {"*"}, QDir::AllEntries);
        for (; iter.hasNext(); iter.next())
        {
            QFileInfo sub(iter.filePath()+ "/24x24");
            if (sub.exists())
            {
                themes_map[iter.fileName()+ "/24x24"] = QVariant(static_cast<int>(0));
            }
        }
        themes_map[""] = QVariant(static_cast<int>(0));
        mMiscelaneousItems[linux_theme] = themes_map;
//        mMiscelaneousItems[linux_icon_path]                              = QVariant(mActions.getIconLocation());
#endif
        mMiscelaneousItems[log_severity]                                 = QVariant(severity_map);
    }
    else
    {
        QMap<QString,QVariant> severity_map = mMiscelaneousItems[log_severity].toMap();
        Logger::setSeverity(0xffff, false);
        for (std::uint32_t bit = 1; (bit & 0xffff); bit <<= 1)
        {
            Logger::setSeverity(bit, severity_map[Logger::getName(static_cast<Logger::eSeverity>(bit))].toBool());
        }
        severity_map.remove(invalid_severity);
        mCompare2Items                          = mMiscelaneousItems[compare_2_items].toString();
        mFindGrep                               = mMiscelaneousItems[find_grep].toString();
        mFindFsrc                               = mMiscelaneousItems[find_fsrc].toString();
        mExternalFileOpenCmd                    = mMiscelaneousItems[external_file_open_cmd].toString();
        mFileCopyMimeType                       = mMiscelaneousItems[file_copy_mime_type].toString();
        mStylePath                              = mMiscelaneousItems[style_path].toString();
        mWarnOpenFileSize                       = mMiscelaneousItems[warn_open_file_fize].toLongLong();
        mBranchHasSiblingsNotAdjoins            = mMiscelaneousItems[branch_has_siblings_not_adjoins].toString();
        mBranchHasSiblingsAdjoins               = mMiscelaneousItems[branch_has_siblings_adjoins].toString();
        mBranchHasChildrenNotHasSiblingsAdjoins = mMiscelaneousItems[branch_has_children_not_has_siblings_adjoins].toString();
        mBranchClosedHasChildrenHasSibling      = mMiscelaneousItems[branch_closed_has_children_has_sibling].toString();
        mBranchOpenHasChildrenHasSibling        = mMiscelaneousItems[branch_open_has_children_has_sibling].toString();
        ui->treeSource->setColumnHidden(QSourceTreeWidget::Column::DateTime,
                                                 !mMiscelaneousItems[repository_tree_date].toBool());
        ui->treeSource->setColumnHidden(QSourceTreeWidget::Column::Size,
                                                 !mMiscelaneousItems[repository_tree_size].toBool());
        mSystemTrayMessage->setVisible(mMiscelaneousItems[system_tray_messages].toBool());
        mCloseAllFilesOfRepository              = mMiscelaneousItems[close_all_files_of_repository].toBool();
        mExperimental                           = mMiscelaneousItems[experimental].toBool();
        mFastFileSearch                         = mMiscelaneousItems[fast_file_search].toBool();
        mAppendToBatch                          = mMiscelaneousItems[append_to_batch].toBool();
        mOutput2secondTextView                  = mMiscelaneousItems[output_2_second_view].toBool();
        mSearchResultsAsSearchTree              = mMiscelaneousItems[search_results_as_tree].toBool();
#ifdef __linux__
        QMap<QString,QVariant> themes_map       = mMiscelaneousItems[linux_theme].toMap();
        for (auto theme = themes_map.begin(); theme != themes_map.end(); ++theme)
        {
            if (theme.value().toBool())
            {
                mActions.setTheme(theme.key());
            }
        }
//        mActions.setIconLocation(mMiscelaneousItems[linux_icon_path].toString());
#endif
    }
}

void MainWindow::on_DockWidgetActivated(QDockWidget *dockWidget)
{
    if (dockWidget)
    {
        // TRACEX(Logger::info, "on_DockWidgetActivated("<< dockWidget->objectName() << ":" << dockWidget->windowTitle() << ")");
        code_browser* textBrowser = dynamic_cast<code_browser*>(get_widget(dockWidget));
        if (textBrowser)
        {
            QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser, textbrowser, background_textbrowser });
            for (QDockWidget* current_widget : dock_widgets)
            {
                code_browser* browser = dynamic_cast<code_browser*>(get_widget(current_widget));
                if (browser && browser->set_active(current_widget == dockWidget))
                {
                    check_reload(browser);
                }
            }
            if (!m_tree_source_item_double_clicked)
            {
                QString file_path_part = textBrowser->get_file_path();
                ui->labelFilePath->setText(file_path_part);
                set_widget_and_action_enabled(ui->btnStoreText, textBrowser->get_changed());
                updateSelectedLanguage(textBrowser->currentLanguage());
                QString repository_root;
                find_root_and_partial_path(*ui->treeSource, repository_root, file_path_part);
                ui->treeSource->find_item(repository_root, file_path_part);
            }
        }
        mActivViewObjectName = dockWidget->objectName();
        bool bv_active = mActivViewObjectName == binary_table_view;
        ui->tableBinaryView->set_active(bv_active);
        if (bv_active)
        {
            ui->labelFilePath->setText(ui->tableBinaryView->get_file_path());
            set_widget_and_action_enabled(ui->btnStoreText, ui->tableBinaryView->get_changed());
        }
    }
}

void MainWindow::showDockedWidget(QWidget* widget, bool show)
{
    QDockWidget* parent = dynamic_cast<QDockWidget*>(widget->parent());
    if (parent)
    {
        if (show)
        {
            if (!parent->isVisible())
            {
                parent->setVisible(true);
            }

            parent->raise();
        }
        else
        {
            parent->setVisible(false);
        }
    }
}

QList<QDockWidget *> MainWindow::get_dock_widget_of_name(QStringList names)
{
    QList<QDockWidget *> dock_widgets = findChildren<QDockWidget *>();
    for (auto dock_widget = dock_widgets.begin(); dock_widget != dock_widgets.end();)
    {
        if (names.contains((*dock_widget)->objectName()))
        {
            ++dock_widget;
        }
        else
        {
            dock_widget = dock_widgets.erase(dock_widget);
        }

    }
    return dock_widgets;
}

MainWindow::AdditionalEditor MainWindow::additional_editor()
{
    return static_cast<AdditionalEditor>(ui->comboOpenNewEditor->currentIndex());
}

QDockWidget *MainWindow::get_first_dock_tab(FirstTab::e number)
{
    QObject* widget = nullptr;
    switch (number)
    {
    case FirstTab::text_view: widget = ui->textBrowser->parent(); break;
    case FirstTab::web_view:
#ifdef WEB_ENGINE
        widget = mWebEngineView.data()->parent();
#else
        widget = mTextRenderView.data()->parent();
#endif
        break;
    case FirstTab::tree_view: widget = ui->treeHistory->parent();
    }
    return dynamic_cast<QDockWidget*>(widget);
}

QWidget* MainWindow::get_active_editable_widget(const QString& file_path)
{
    if (file_path.isEmpty())
    {
        QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser, binary_table_view, background_textbrowser });
        for (QDockWidget* dock_widget : dock_widgets)
        {
            auto* widget = get_widget(dock_widget);
            if (get_active(widget))
            {
                return widget;
            }
        }
        return ui->textBrowser;
    }
    else
    {
        QWidget* editable_with_file_path = nullptr;
        QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser, textbrowser, binary_table_view });
        for (QDockWidget* dock_widget : dock_widgets)
        {
            auto* widget = get_widget(dock_widget);
            if (get_file_path(widget) == file_path)
            {
                editable_with_file_path = widget;
            }
        }
        if (editable_with_file_path)
        {
            showDockedWidget(editable_with_file_path);
        }
        return editable_with_file_path;
    }
}

code_browser* MainWindow::create_new_text_browser(const QString &file_path, const QString &name)
{
    bool set_filename = true;
    switch (additional_editor())
    {
    case AdditionalEditor::None:
        return ui->textBrowser;
        break;
    case AdditionalEditor::One:
    {
        set_filename = false;
        QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
        if (dock_widgets.size())
        {
            return dynamic_cast<code_browser*>(get_widget(dock_widgets[0]));
        }
    }   break;
    case AdditionalEditor::OnNewFile:
        break;
    }

    QString file_name;
    if (file_path.length() && set_filename)
    {
        QFileInfo info(file_path);
        file_name = info.fileName();
    }
    else if (name.length())
    {
        file_name = name;
    }
    else
    {
        file_name = "Editor";
    }

    auto *docked_browser =  ui->textBrowser->clone(true, false);
    connect(docked_browser, SIGNAL(text_of_active_changed(bool)), this, SLOT(textBrowserChanged(bool)));
    connect(docked_browser, SIGNAL(updateExtension(QString)), this, SLOT(updateSelectedLanguage(QString)));
    connect(docked_browser, SIGNAL(check_reload(code_browser*)), this, SLOT(check_reload(code_browser*)));
    connect(docked_browser, SIGNAL(line_changed(int)), m_status_line_label, SLOT(setNum(int)));
    connect(docked_browser, SIGNAL(column_changed(int)), m_status_column_label, SLOT(setNum(int)));
#ifdef WEB_ENGINE
    connect(docked_browser, SIGNAL(text_changed(QString)),m_markdown_proxy.data(), SLOT(setText(QString)));
#endif
    connect(docked_browser, SIGNAL(show_web_view(bool)), this, SLOT(show_web_view(bool)));

    docked_browser->setReadOnly(false);
    connect(docked_browser, SIGNAL(send_focused(QDockWidget*)), this, SLOT(on_DockWidgetActivated(QDockWidget*)));
    QDockWidgetX*dock = create_dock_widget(docked_browser, file_name, new_textbrowser, true);
    dock->setAttribute(Qt::WA_DeleteOnClose);
    connect(dock, SIGNAL(signal_close(QDockWidgetX*,bool&)), this, SLOT(close_text_browser(QDockWidgetX*,bool&)));
    connect(dock, SIGNAL(signal_dock_widget_activated(QDockWidget*)), this, SLOT(on_DockWidgetActivated(QDockWidget*)));
    connect(dock, SIGNAL(visibilityChanged(bool)), docked_browser, SLOT(change_visibility(bool)));
    QDockWidget* parent = get_first_dock_tab(FirstTab::text_view);
    if (parent)
    {
        tabifyDockWidget(parent, dock);
    }
    ui->comboOpenNewEditor->setEnabled(false);
    return docked_browser;

}

bool MainWindow::send_close_to_editable_widget(QWidget*editable_widget)
{
    if (editable_widget && !is_any_equal_to(editable_widget, ui->textBrowser, ui->tableBinaryView, mBackgroundTextView.data()) )
    {
        QDockWidgetX*dw = dynamic_cast<QDockWidgetX*>(editable_widget->parent());
        if (dw)
        {
            QAction* action = dw->toggleViewAction();
            action->setChecked(true);
            action->trigger();
            return dw->is_closing();
        }
    }
    return true;
}

void MainWindow::close_text_browser(QDockWidgetX* widget, bool&closed)
{
    closed = btnCloseText_clicked(Editor::CalledFromWidget);
    if (closed)
    {
        remove_text_browser(widget);
    }
}

void MainWindow::close_tree_view(QDockWidgetX *widget, bool &closed)
{
    closed = widget->isVisible();
}


void MainWindow::remove_text_browser(QDockWidgetX *dock_widget)
{
    if (dock_widget && dock_widget->objectName() == new_textbrowser)
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(dock_widget->widget());
        disconnect(text_browser, SIGNAL(text_of_active_changed(bool)), this, SLOT(textBrowserChanged(bool)));
        disconnect(text_browser, SIGNAL(updateExtension(QString)), this, SLOT(updateSelectedLanguage(QString)));
        disconnect(text_browser, SIGNAL(check_reload(code_browser*)), this, SLOT(check_reload(code_browser*)));
        disconnect(text_browser, SIGNAL(line_changed(int)), m_status_line_label, SLOT(setNum(int)));
        disconnect(text_browser, SIGNAL(column_changed(int)), m_status_column_label, SLOT(setNum(int)));
        disconnect(dock_widget, SIGNAL(visibilityChanged(bool)), text_browser, SLOT(change_visibility(bool)));
#ifdef WEB_ENGINE
        disconnect(text_browser, SIGNAL(text_changed(QString)), m_markdown_proxy.data(), SLOT(setText(QString)));
#endif
        show_web_view(false);
        disconnect(text_browser, SIGNAL(show_web_view(bool)), this, SLOT(show_web_view(bool)));
        show_web_view(false);
        QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
        ui->comboOpenNewEditor->setEnabled(dock_widgets.size() <= 1);
    }
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

QVariant MainWindow::handleWorker(const QVariant& aData)
{
    QVariant work_result;
    Logger::printDebug(Logger::trace, "handleWorker: %x, %s", QThread::currentThreadId(), aData.typeName());
    if (aData.isValid())
    {
        auto data_map = aData.toMap();
        if (data_map.contains(Worker::repository))
        {
            QDir::setCurrent(data_map[Worker::repository].toString());
        }
        switch(static_cast<Work>(data_map[Worker::work].toInt()))
        {
        case Work::DetermineGitMergeTools:
        case Work::ApplyGitCommand:
        case Work::ApplyCommand:
        {
            QString result_string;
            int result = execute(data_map[Worker::command].toString().toStdString().c_str(), result_string, true);
            if (result != NoError)
            {
                Logger::printDebug(Logger::error, "execute error (%d): %s", result, result_string.toStdString().c_str());
            }
            data_map[Worker::result] = result_string;
            work_result.setValue(data_map);
        } break;
        case Work::AsynchroneousCommand:
        {
            QString result_string;
#ifdef USE_BOOST
            int result = execute(data_map[Worker::command].toString().toStdString().c_str(), result_string, true, boost::bind(&MainWindow::on_emit_temp_file_path, this, _1));
#else
            int result = execute(data_map[Worker::command].toString().toStdString().c_str(), result_string, true, std::bind(&MainWindow::on_emit_temp_file_path, this, _1));
#endif
            if (result != NoError)
            {
                Logger::printDebug(Logger::error, "execute error (%d): %s", result, result_string.toStdString().c_str());
            }
            data_map[Worker::result] = result_string;
            work_result.setValue(data_map);
        } break;
        default:
            work_result.setValue(data_map);
            break;
        }
    }
    return work_result;
}

void MainWindow::handleMessage(QVariant aData)
{
    auto* action = mActions.getAction(Cmd::KillBackgroundThread);
    if (action)
    {
        action->setEnabled(mWorker.isBusy());
        action->setToolTip(mWorker.getBatchToolTip());
    }
    Logger::printDebug(Logger::trace, "handleMessage(): %x, %s", QThread::currentThreadId(), aData.typeName());
    if (aData.isValid())
    {
        auto data_map = aData.toMap();
        QString message = tr("command: %1").arg(data_map[Worker::command].toString());
        if (data_map.contains(Worker::repository))
        {
            message += tr("\nRepository: %1").arg(data_map[Worker::repository].toString());
        }
        mSystemTrayMessage->showMessage(tr("Backgound command finished"), message, QSystemTrayIcon::Information, 5000);
        switch(static_cast<Work>(data_map[Worker::work].toInt()))
        {
        case Work::DetermineGitMergeTools:
        {
            bool new_item_added = false;
            auto result_list = data_map[Worker::result].toString().split("\n");
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
        } break;
        case Work::ApplyGitCommand:
        case Work::ApplyCommand:
        {
            auto cmd_action = static_cast<Cmd::ePostAction>(data_map[Worker::action].toUInt());
            appendTextToBrowser(data_map[Worker::command].toString(), cmd_action == 0);
            appendTextToBrowser(data_map[Worker::result].toString(), true);
            perform_post_cmd_action(cmd_action, {}, static_cast<git::Cmd::eCmd>(data_map[Worker::command_id].toInt()));
        } break;
        case Work::AsynchroneousCommand:
            break;

        default:  break;
        }
    }
}

void MainWindow::killBackgroundThread()
{
    QStringList commands = mWorker.getCurrentCmdName().split(" ");
    if (commands.size())
    {
        QStringList pidlist;
        if (get_pid_list(commands[0], pidlist))
        {
            int msgbox_buttons = to_all;
            if (pidlist.size() == 1)
            {
                msgbox_buttons = to_one;
            }
            if (mWorker.hasBatchList())
            {
                msgbox_buttons = to_all_or_one;
            }
            int result = callMessageBox(
                tr("Do you really whant to kill all background processes \"%1\"?;%1\n"
                   "Yes kills process ID: %2\n"
                   "Yes To All empties also batch list"), pidlist.join(" "), mWorker.getBatchToolTip(), msgbox_buttons);
            if (result & (QMessageBox::Yes|QMessageBox::YesToAll))
            {
                for (const QString &pid : std::as_const(pidlist))
                {
#ifdef __linux__
                    string cmd = "kill " + pid.toStdString();
#else
                    string cmd = "taskkill " + pid.toStdString();
#endif
                    QString cmd_result;
                    execute(cmd.c_str(), cmd_result, true);
                    cmd += '\n';
                    appendTextToBrowser(cmd.c_str() + cmd_result, true);
                }
                if (result & QMessageBox::YesToAll)
                {
                    mWorker.clearBatchList();
                }
            }
        }
        else
        {
            appendTextToBrowser(tr("no pids found for %1").arg(commands[0]));
        }
    }
    else
    {
        appendTextToBrowser(tr("no command found: %1").arg(mWorker.getCurrentCmdName()));
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && CORE5COMPAT == 0
    ui->comboTextCodex->addItem("Default Codec");
    QStringEncoder sc;
    for (int codec=0; codec < QStringConverter::LastEncoding; ++codec)
    {
        ui->comboTextCodex->addItem(sc.nameForEncoding(static_cast<QStringConverter::Encoding>(codec)));
    }
#else
    const auto codecs = QTextCodec::availableCodecs();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const static auto reg_ex_num = QRegularExpression("([\\d]{1,6})");
    const static auto reg_ex_text = QRegularExpression("([a-zA-Z-]{1,})");
    QMap<QString, QList<QString>> codec_map;
    for (const auto& codec : std::as_const(codecs))
    {
        auto match_num = reg_ex_num.match(codec);
        if (match_num.hasMatch())
        {
            QString captured_num = match_num.captured();
            QString captured_text;
            auto match_text = reg_ex_text.match(codec);
            if (match_text.hasMatch())
            {
                captured_text = match_text.captured();
            }
#else
    QRegExp reg_ex_num ("([\\d]{1,6})");
    QRegExp reg_ex_text("([a-zA-Z-]{1,})");
    QMap<QString, QList<QString>> codec_map;
    for (const auto& codec : std::as_const(codecs))
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
#endif
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
#endif
}

void MainWindow::compare_items(QString& item1, QString& item2)
{
    QString command = tr(mCompare2Items.toStdString().c_str()).arg(item1, item2);
    QString result;
    mCurrentTask = Work::ApplyCommand;
    applyGitCommandToFilePath({}, command, result, true);
    mCurrentTask = Work::None;
}

QString MainWindow::applyGitCommandToFilePath(const QString& a_source, const QString& a_git_cmd, QString& a_result_str, bool force_thread)
{
    QString command;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const static auto regex_numbers = QRegularExpression("%[0-9]+");
#else
    const static auto regex_numbers = QRegExp("%[0-9]+");
#endif
    auto *sender_action = qobject_cast<QAction *>(sender());
    auto fPos = a_git_cmd.indexOf(regex_numbers);
    if (fPos != -1)
    {
        command = QObject::tr(a_git_cmd.toStdString().c_str()).arg(a_source);
    }
    else
    {
        command = a_git_cmd;
    }
    if (handleInThread(force_thread))
    {
        auto* kill_background_thread = mActions.getAction(Cmd::KillBackgroundThread);
        if (kill_background_thread)
        {
            kill_background_thread->setEnabled(true);

            const QVariantList variant_list = sender_action->data().toList();
            Work work_command { mCurrentTask };
            if (variant_list[ActionList::Data::Flags].toUInt() & ActionList::Flags::Asynchroneous)
            {
                work_command = Work::AsynchroneousCommand;
            }
            QVariantMap workmap;
            if (mContextMenuSourceTreeItem)
            {
                workmap.insert(Worker::repository, ui->treeSource->getItemTopDirPath(mContextMenuSourceTreeItem));
            }
            workmap.insert(Worker::command_id, INT(mActions.findID(sender_action)));
            workmap.insert(Worker::command, command);
            workmap.insert(Worker::action, variant_list[ActionList::Data::PostCmdAction].toUInt());
            workmap.insert(Worker::flags, variant_list[ActionList::Data::Flags].toUInt());
            workmap.insert(Worker::work, INT(work_command));
            mWorker.doWork(QVariant(workmap));
            kill_background_thread->setToolTip(mWorker.getBatchToolTip());
            if (mOutput2secondTextView && mBackgroundTextView)
            {
                showDockedWidget(mBackgroundTextView.data());
            }
            command.clear();
        }
    }
    else
    {
        if (sender_action)
        {
            if (mActions.findID(sender_action) == git::Cmd::CustomTestCommand)
            {
                QDir::setCurrent(a_source);
            }
        }
        int result = execute(command, a_result_str);
        if (result != NoError)
        {
            a_result_str += tr("\nError %1 occurred").arg(result);
        }
    }
    return command;
}

void MainWindow::initContextMenuActions()
{
    connect(mActions.createAction(Cmd::ShowDifference, tr("Show difference"), Cmd::getCommand(Cmd::ShowDifference), this) , SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::ShowDifference)->setShortcut(QKeySequence(Qt::Key_F8));
    mActions.setCmdAddOn(Cmd::ShowDifference, "--cached ");
    mActions.setFlags(Cmd::ShowDifference, Type::GitModified|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::ShowDifference, ActionList::Flags::Stash|ActionList::Flags::History|ActionList::Flags::DiffCmd|ActionList::Flags::DependsOnStaged, Flag::set);
    mActions.setMenuStringList(Cmd::ShowDifference, {"--diff-algorithm", "patience", "minimal", "histogram", "*myers"});

    connect(mActions.createAction(Cmd::CallDiffTool, tr("Call diff tool...") ,Cmd::getCommand(Cmd::CallDiffTool), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.getAction(Cmd::CallDiffTool)->setShortcut(QKeySequence(Qt::Key_F9));
    mActions.setCmdAddOn(Cmd::CallDiffTool, "--cached ");
    mActions.setFlags(Cmd::CallDiffTool, Type::GitModified, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::CallDiffTool, ActionList::Flags::Stash|ActionList::Flags::History|ActionList::Flags::DiffOrMergeTool|ActionList::Flags::DiffCmd|ActionList::Flags::CallInThread|ActionList::Flags::DependsOnStaged, Flag::set);

    connect(mActions.createAction(Cmd::CallMergeTool, tr("Call merge tool..."), Cmd::getCommand(Cmd::CallMergeTool), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::CallMergeTool, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::CallMergeTool)->setShortcut(QKeySequence(Qt::Key_F7));
    mActions.setFlags(Cmd::CallMergeTool, Type::GitUnmerged, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::CallMergeTool, ActionList::Flags::DiffOrMergeTool|ActionList::Flags::DiffCmd|ActionList::Flags::CallInThread, Flag::set);

    connect(mActions.createAction(Cmd::InvokeGitMergeDialog , tr("Merge file..."), tr("Merge selected file (experimental, not working)"), this) , SIGNAL(triggered()), this, SLOT(invoke_git_merge_dialog()));
    mActions.setFlags(Cmd::InvokeGitMergeDialog, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::InvokeGitMergeDialog, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::InvokeHighlighterDialog, tr("Edit Highlighting..."), tr("Edit highlighting color and font"), this) , SIGNAL(triggered()), this, SLOT(invoke_highlighter_dialog()));
    mActions.setFlags(Cmd::InvokeHighlighterDialog, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::InvokeHighlighterDialog, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::InvokeOutputParserDialog, tr("Edit Output Parser..."), tr("Edit output parser"), this) , SIGNAL(triggered()), this, SLOT(invoke_output_parser_dialog()));
    mActions.setFlags(Cmd::InvokeOutputParserDialog, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::InvokeOutputParserDialog, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ShowStatus      , tr("Show status"), Cmd::getCommand(Cmd::ShowStatus), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowStatus, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::ShowStatus, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::ShowShortStatus , tr("Show short status") , Cmd::getCommand(Cmd::ShowShortStatus), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::ShowShortStatus, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::ShowShortStatus, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.getAction(Cmd::ShowShortStatus)->setShortcut(QKeySequence(Qt::Key_F5));

    connect(mActions.createAction(Cmd::Add, tr("Add to git (stage)"), Cmd::getCommand(Cmd::Add), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Add, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Add)->setShortcut(QKeySequence(Qt::Key_F4));
    mActions.setFlags(Cmd::Add, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Add, Type::GitModified|Type::GitUnTracked|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Unstage, tr("Reset file (unstage)"), Cmd::getCommand(Cmd::Unstage), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Unstage, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Unstage)->setShortcut(QKeySequence(Qt::ShiftModifier| Qt::Key_F4));
    mActions.setFlags(Cmd::Unstage, Type::GitStaged|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::History, tr("Show History"), Cmd::getCommand(Cmd::History), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::History, Cmd::ParseHistoryText);
    mActions.getAction(Cmd::History)->setShortcut(QKeySequence(Qt::Key_F10));
    mActions.setFlags(Cmd::History, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Blame, tr("Blame"), Cmd::getCommand(Cmd::Blame), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Blame, ActionList::Flags::CallInThread|ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::Blame, Type::Folder, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setCustomCommandPostAction(Cmd::Blame, Cmd::ParseBlameText);

    connect(mActions.createAction(Cmd::Remove, tr("Remove from git..."), Cmd::getCommand(Cmd::Remove), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Remove, "Remove %1 from git repository;Do you want to remove \"%1\"?");
    mActions.setCustomCommandPostAction(Cmd::Remove, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::Remove, Type::GitUnTracked, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Remove, Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Restore, tr("Restore changes..."), Cmd::getCommand(Cmd::Restore), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Restore, tr("Restore %1 changes;Do you want to restore the changes in \"%1\"?"));
    mActions.setCustomCommandPostAction(Cmd::Restore, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::Restore)->setShortcut(QKeySequence(Qt::Key_F6));
    mActions.setFlags(Cmd::Restore, Type::GitModified|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::Restore, Type::GitStaged, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::Restore, ActionList::Flags::History);

    connect(mActions.createAction(Cmd::UndoCommitSoft, tr("Undo commit soft..."), Cmd::getCommand(Cmd::UndoCommitSoft), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::UndoCommitSoft, tr("Undo commit %1 and leave local changes;Do you want to undo the commit(s) in \"%1\"?"));
    mActions.setFlags(Cmd::UndoCommitSoft, ActionList::Flags::History);

    connect(mActions.createAction(Cmd::UndoCommitHard, tr("Undo commit hard..."), Cmd::getCommand(Cmd::UndoCommitHard), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::UndoCommitHard, tr("Undo commit %1 and overwrite local changes;Do you want to undo the commit(s) in \"%1\"?"));
    mActions.setFlags(Cmd::UndoCommitHard, ActionList::Flags::History);

    connect(mActions.createAction(Cmd::Commit, tr("Commit..."), Cmd::getCommand(Cmd::Commit), this), SIGNAL(triggered()), this, SLOT(call_git_commit()));
    mActions.setCustomCommandPostAction(Cmd::Commit, Cmd::UpdateRootItemStatus);
    mActions.setFlags(Cmd::Commit, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::Commit, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Clone, tr("Clone"), Cmd::getCommand(Cmd::Clone), this), SIGNAL(triggered()), this, SLOT(call_git_clone()));
    mActions.setFlags(Cmd::Clone, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::Push, tr("Push"), Cmd::getCommand(Cmd::Push), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Push, ActionList::Flags::NoHide, Flag::set);
    mActions.setFlags(Cmd::Push, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::Pull, tr("Pull"), Cmd::getCommand(Cmd::Pull), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandPostAction(Cmd::Pull, Cmd::UpdateRepository);
    mActions.setFlags(Cmd::Pull, ActionList::Flags::NoHide, Flag::set);
    mActions.setFlags(Cmd::Pull, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::Fetch, tr("Fetch"), Cmd::getCommand(Cmd::Fetch), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Fetch, ActionList::Flags::Asynchroneous, Flag::set);
    mActions.setFlags(Cmd::Fetch, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::Show           , tr("Show"), Cmd::getCommand(Cmd::Show), this), SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::Show, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    const QString stash_message = tr("Stash all entries;Do you whant to stash all entries of repository:\n\"%1\"?");
    connect(mActions.createAction(Cmd::Stash, tr("Stash"),       Cmd::getCommand(Cmd::Stash))    ,  SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::Stash, stash_message);
    mActions.setCustomCommandPostAction(Cmd::Stash, Cmd::UpdateStash);
    mActions.setFlags(Cmd::Stash, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setFlags(Cmd::Stash, ActionList::Flags::Stash, Flag::set);
    connect(mActions.createAction(Cmd::StashShow, tr("Show stash"),  Cmd::getCommand(Cmd::StashShow), this),  SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setFlags(Cmd::StashShow, ActionList::Flags::Stash, Flag::set);
    mActions.setFlags(Cmd::StashShow, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashPush, tr("Stash push"),  Cmd::getCommand(Cmd::StashPush), this),  SIGNAL(triggered()), this, SLOT(perform_custom_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::StashPush, stash_message);
    mActions.setCustomCommandPostAction(Cmd::StashPush, Cmd::UpdateRootItemStatus);
    mActions.setCmdAddOn(Cmd::StashPush, " -- ");
    mActions.setFlags(Cmd::StashPush, ActionList::Flags::StashCmdOption|ActionList::Flags::Stash, Flag::set);
    mActions.setFlags(Cmd::StashPush, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashPop, tr("Stash pop"),   Cmd::getCommand(Cmd::StashPop), this)  ,  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandPostAction(Cmd::StashPop, Cmd::UpdateRootItemStatus);
    mActions.setFlags(Cmd::StashPop, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashApply, tr("Stash apply"), Cmd::getCommand(Cmd::StashApply), this),  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandPostAction(Cmd::StashApply, Cmd::UpdateRootItemStatus);
    mActions.setFlags(Cmd::StashApply, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::StashDrop, tr("Stash drop"),  Cmd::getCommand(Cmd::StashDrop), this) ,  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::StashDrop, "Drop stash entry;Do you whant to drop stash entry of repository:\n\"%1\"?");
    mActions.setFlags(Cmd::StashDrop, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setCustomCommandPostAction(Cmd::StashDrop, Cmd::UpdateStash);
    connect(mActions.createAction(Cmd::StashClear, tr("Stash clear"), Cmd::getCommand(Cmd::StashClear), this), SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::StashClear, "Remove all stash entries;Do you whant to remove all stash entries of repository:\n\"%1\"?");
    mActions.setFlags(Cmd::StashClear, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.setCustomCommandPostAction(Cmd::StashClear, Cmd::UpdateStash);
    connect(mActions.createAction(Cmd::StashList, tr("List stashes"),Cmd::getCommand(Cmd::StashList), this),  SIGNAL(triggered()), this, SLOT(call_git_stash_command()));
    mActions.setCustomCommandPostAction(Cmd::StashList, Cmd::ParseStashListText);
    mActions.setFlags(Cmd::StashList, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchList, tr("List Branches"), Cmd::getCommand(Cmd::BranchList), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchList, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchList, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchList, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchListRemote, tr("List remote Branches"), Cmd::getCommand(Cmd::BranchListRemote), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListRemote, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchListRemote, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchListRemote, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchListMerged, tr("List merged Branches"), Cmd::getCommand(Cmd::BranchListMerged), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListMerged, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchListMerged, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchListMerged, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::BranchListNotMerged, tr("List not merged Branches"), Cmd::getCommand(Cmd::BranchListNotMerged), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandPostAction(Cmd::BranchListNotMerged, Cmd::ParseBranchListText);
    mActions.setFlags(Cmd::BranchListNotMerged, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchListNotMerged, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchDelete   , tr("Delete Branch"), Cmd::getCommand(Cmd::BranchDelete), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchDelete, tr("Delete %1 from git;Do you want to delete<br>- %1 ?"));
    mActions.setCustomCommandPostAction(Cmd::BranchDelete, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::BranchDelete, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchDelete, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchCheckout, tr("Checkout Branch"), Cmd::getCommand(Cmd::BranchCheckout), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchCheckout, tr("Checkout %1;Do you want to set \"%1\" active?;"));
    mActions.setCustomCommandPostAction(Cmd::BranchCheckout, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::BranchCheckout, ActionList::Flags::NotVariableGitCmd, Flag::set);

    connect(mActions.createAction(Cmd::BranchCreate, tr("Create Branch"), Cmd::getCommand(Cmd::BranchCreate), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setCustomCommandMessageBoxText(Cmd::BranchCreate, tr("Create Branch;Do you want to create branch?;"));
    mActions.setCustomCommandPostAction(Cmd::BranchCreate, Cmd::UpdateItemStatus);
    mActions.setFlags(Cmd::BranchCreate, ActionList::Flags::NotVariableGitCmd, Flag::set);

    connect(mActions.createAction(Cmd::DiffOfTwoBranches, tr("Diff between Branches"), Cmd::getCommand(Cmd::DiffOfTwoBranches), ui->treeBranches), SIGNAL(triggered()), ui->treeBranches, SLOT(diff_of_two_branches()));
    mActions.setFlags(Cmd::DiffOfTwoBranches, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::DiffOfTwoBranches, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::MergeTwoBranches, tr("Merge Branch"), Cmd::getCommand(Cmd::MergeTwoBranches), ui->treeBranches), SIGNAL(triggered()), ui->treeBranches, SLOT(merge_branch()));
    mActions.setFlags(Cmd::MergeTwoBranches, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::MergeTwoBranches, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::BranchShow, tr("Show Branch"), Cmd::getCommand(Cmd::BranchShow), this), SIGNAL(triggered()), this, SLOT(call_git_branch_command()));
    mActions.setFlags(Cmd::BranchShow, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::BranchShow, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::MoveOrRename   , tr("Move / Rename..."), Cmd::getCommand(Cmd::MoveOrRename), this), SIGNAL(triggered()), this, SLOT(call_git_move_rename()));
    mActions.setCustomCommandPostAction(Cmd::MoveOrRename, Cmd::UpdateItemStatus);
    mActions.getAction(Cmd::MoveOrRename)->setShortcut(QKeySequence(Qt::Key_F2));
    mActions.setFlags(Cmd::MoveOrRename, ActionList::Flags::NotVariableGitCmd, Flag::set);
    mActions.setFlags(Cmd::MoveOrRename, Type::GitUnTracked, Flag::set, ActionList::Data::StatusFlagDisable);
    mActions.setFlags(Cmd::MoveOrRename, Type::Folder|Type::File, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ExpandTreeItems      , tr("Expand Tree Items"), tr("Expands all tree item of focused tree")) , SIGNAL(triggered()), this, SLOT(expand_tree_items()));
    mActions.setFlags(Cmd::ExpandTreeItems, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.getAction(Cmd::ExpandTreeItems)->setShortcut(QKeySequence(Qt::Key_F11));
    mActions.setFlags(Cmd::ExpandTreeItems, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::CollapseTreeItems    , tr("Collapse Tree Items"), tr("Collapses all tree item of focused tree")), SIGNAL(triggered()), this, SLOT(collapse_tree_items()));
    mActions.setFlags(Cmd::CollapseTreeItems, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.getAction(Cmd::CollapseTreeItems)->setShortcut(QKeySequence(Qt::ShiftModifier | Qt::Key_F11));
    mActions.setFlags(Cmd::CollapseTreeItems, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::AddGitSourceFolder   , tr("Add git source folder..."), tr("Add a git source folder to repository view")) , SIGNAL(triggered()), this, SLOT(addGitSourceFolder()));
    mActions.setFlags(Cmd::AddGitSourceFolder, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::AddGitSourceFolder, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::RemoveGitFolder, tr("Remove git source folder"), tr("Remove a git source folder from repository view")), SIGNAL(triggered()), this, SLOT(removeGitSourceFolder()));
    mActions.setFlags(Cmd::RemoveGitFolder, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::RemoveGitFolder, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::OpenFile, tr("Open File..."), tr("Open arbitrary file")) , SIGNAL(triggered()), this, SLOT(OpenFile()));
    mActions.setFlags(Cmd::OpenFile, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::OpenFile, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.getAction(Cmd::OpenFile)->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_O));
    connect(mActions.createAction(Cmd::SaveAs, tr("Save File as..."), tr("Save file under alternative name")) , SIGNAL(triggered()), this, SLOT(SaveFileAs()));
    mActions.setFlags(Cmd::SaveAs, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::SaveAs, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ReplaceAll, tr("Replace All"), tr("Replace all found items")) , SIGNAL(triggered()), this, SLOT(FindReplaceAll()));
    mActions.setFlags(Cmd::ReplaceAll, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::ReplaceAll, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::GoToLine, tr("Go to line"), tr("go to line given in edit field")) , SIGNAL(triggered()), this, SLOT(GoToLine()));
    mActions.setFlags(Cmd::GoToLine, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::GoToLine, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.getAction(Cmd::GoToLine)->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_L));

    connect(mActions.createAction(Cmd::EditToUpper, tr("To Upper."), tr("Modify selected text to upper case"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditToUpper, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditToUpper, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::EditToLower, tr("To Lower"), tr("Modify selected text to lower case"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditToLower, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditToLower, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::EditToggleComment, tr("Comment / uncomment"), tr("Comment or uncomment selected text lines"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditToggleComment, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditToggleComment, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::EditToSnakeCase, tr("To Snake Case"), tr("Modify selected text to snake_case"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditToSnakeCase, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditToSnakeCase, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::EditToCamelCase, tr("To Camel Case"), tr("Modify selected text to CameCase"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditToCamelCase, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditToCamelCase, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::EditTabIndent, tr("Tab Indent"), tr("Indent selected text by Tab"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditTabIndent, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditTabIndent, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::EditTabOutdent, tr("Tab Outdent"), tr("Outdent selected text by Tab"), this), SIGNAL(triggered()), this, SLOT(modify_text()));
    mActions.setFlags(Cmd::EditTabOutdent, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::EditTabOutdent, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::UpdateGitStatus, tr("Update"), tr("Updates the git status of all or the selected source folder or reloads the selected file")), SIGNAL(triggered()), this, SLOT(updateRepositoryStatus()));
    mActions.setFlags(Cmd::UpdateGitStatus, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::UpdateGitStatus, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::ClearTreeItems, tr("Clear all tree entries"), tr("Clears all tree entries in focused tree except repository tree")), SIGNAL(triggered()), this, SLOT(clearTrees()));
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
    mActions.setFlags(Cmd::OpenFileExternally, Type::File|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::WhatsThisHelp, tr("Whats this?"), tr("Whats this help")), &QAction::triggered, []() { QWhatsThis::enterWhatsThisMode(); });
    mActions.setFlags(Cmd::WhatsThisHelp, ActionList::Flags::FunctionCmd, Flag::set);

    connect(mActions.createAction(Cmd::CompareTo, tr("Compare to..."), tr("Start compare mode to select other file ore folder")), SIGNAL(triggered()), ui->treeSource, SLOT(start_compare_to()));
    mActions.setFlags(Cmd::CompareTo, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CompareTo, Type::File|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CloseAll, tr("Close all"), tr("Closes all documents")), SIGNAL(triggered()), this, SLOT(btnCloseAll_clicked()));
    connect(mActions.createAction(Cmd::SaveAll, tr("Save all"), tr("Saves all documents")), SIGNAL(triggered()), this, SLOT(btnStoreAll_clicked()));
    connect(mActions.createAction(Cmd::ReloadAll, tr("Reload all"), tr("Reloads all documents")), SIGNAL(triggered()), this, SLOT(btnReloadAll_clicked()));

    connect(mActions.createAction(Cmd::CustomGitActionSettings, tr("Customize git actions..."), tr("Edit custom git actions, menues and toolbars")), SIGNAL(triggered()), this, SLOT(performCustomGitActionSettings()));
    mActions.setFlags(Cmd::CustomGitActionSettings, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CustomGitActionSettings, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.getAction(Cmd::CustomGitActionSettings)->setShortcut(QKeySequence(Qt::Key_F12));
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

    connect(mActions.createAction(Cmd::KillBackgroundThread, tr("Background Activity..."), tr("You may try to kill git action running in background")), SIGNAL(triggered()), this, SLOT(killBackgroundThread()));
    mActions.getAction(Cmd::KillBackgroundThread)->setEnabled(false);
    mActions.setFlags(Cmd::KillBackgroundThread, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::KillBackgroundThread, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CopyFileName, tr("Copy file name"), tr("Copy file name to clipboard")), SIGNAL(triggered()), this, SLOT(copyFileName()));
    mActions.setFlags(Cmd::CopyFileName, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CopyFileName, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    connect(mActions.createAction(Cmd::CopyFilePath, tr("Copy file and path"), tr("Copy file or folder and path to clipboard")), SIGNAL(triggered()), this, SLOT(copyFilePath()));
    mActions.setFlags(Cmd::CopyFilePath, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::CopyFilePath, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
    mActions.getAction(Cmd::CopyFilePath)->setShortcut(QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_C));

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

    connect(mActions.createAction(Cmd::CloneTextBrowser, tr("Split view"), tr("Splits view into two views and vice versa")), SIGNAL(triggered()), this, SLOT(clone_code_browser()));
    mActions.setFlags(Cmd::CloneTextBrowser, ActionList::Flags::FunctionCmd, Flag::set);

    connect(mActions.createAction(Cmd::CreateBookMark, tr("Create Bookmark"), tr("Creates a Bookmark")), SIGNAL(triggered()), this, SLOT(createBookmark()));
    mActions.setFlags(Cmd::CreateBookMark, ActionList::Flags::FunctionCmd, Flag::set);

    connect(mActions.createAction(Cmd::ShowInformation, tr("Show Information"), tr("Show information about selected item")), SIGNAL(triggered()), this, SLOT(showInformation()));
    mActions.getAction(Cmd::ShowInformation)->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_I));
    mActions.setFlags(Cmd::ShowInformation, ActionList::Flags::FunctionCmd, Flag::set);
    mActions.setFlags(Cmd::ShowInformation, Type::File|Type::Folder, Flag::set, ActionList::Data::StatusFlagEnable);

    connect(mActions.createAction(Cmd::CustomTestCommand, tr("test command"), tr("")), SIGNAL(triggered()), this, SLOT(perform_custom_command()));

    Cmd::eCmd new_id = Cmd::AutoCommand;
    create_auto_cmd(ui->ckDirectories, new_id);
    create_auto_cmd(ui->ckFiles, new_id);
    create_auto_cmd(ui->ckHiddenFiles, new_id);
    create_auto_cmd(ui->ckShortState, new_id);
    create_auto_cmd(ui->ckSymbolicLinks, new_id);
    create_auto_cmd(ui->ckSystemFiles, new_id);
    create_auto_cmd(ui->ckRenderGraphicFile, new_id, mActions.check_location("applications-graphics.png"));
    create_auto_cmd(ui->ckHideEmptyParent, new_id);
    create_auto_cmd(ui->ckFindCaseSensitive, new_id, mActions.check_location("applications-system.png"));
    create_auto_cmd(ui->ckFindRegEx, new_id,         mActions.check_location("applications-system.png"));
    create_auto_cmd(ui->ckFindWholeWord, new_id,     mActions.check_location("applications-system.png"));
    /// NOTE: free ID 511
    /// NOTE: free ID 512
    new_id += 3;
    create_auto_cmd(ui->ckTypeConverter, new_id, mActions.check_location("format-text-direction-rtl.png"));

    std::vector<Cmd::eCmd> contextmenu_text_view;
    contextmenu_text_view.push_back(Cmd::Separator);

    create_auto_cmd(ui->ckShowLineNumbers, new_id, mActions.check_location("x-office-document.png"));
    contextmenu_text_view.push_back(new_id);

    contextmenu_text_view.insert(contextmenu_text_view.end(), { Cmd::SubFiles, Cmd::OpenFile});
    create_auto_cmd(ui->btnStoreText, new_id, mActions.check_location("text-x-patch.png"))->  setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_W));
    mActions.getAction(new_id)->setText(tr("Save"));
    contextmenu_text_view.push_back(new_id);
    contextmenu_text_view.insert(contextmenu_text_view.end(), { Cmd::SaveAs, Cmd::SaveAll});
    create_auto_cmd(ui->btnCloseText, new_id)->                                               setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_S));
    mActions.getAction(new_id)->setText(tr("Close"));
    contextmenu_text_view.push_back(new_id);
    contextmenu_text_view.insert(contextmenu_text_view.end(), {Cmd::CloseAll});
    contextmenu_text_view.insert(contextmenu_text_view.end(), {Cmd::ReloadAll});

    contextmenu_text_view.push_back(Cmd::SubFind);
    create_auto_cmd(ui->btnFindAll, new_id, mActions.check_location("edit-find.png"));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->btnFindNext, new_id, mActions.check_location("go-next.png"))->        setShortcut(QKeySequence(Qt::Key_F3));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->btnFindPrevious, new_id, mActions.check_location("go-previous.png"))->setShortcut(QKeySequence(Qt::ShiftModifier | Qt::Key_F3));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->comboFindBox, new_id, mActions.check_location("edit-find.png"))->     setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_F));
    contextmenu_text_view.push_back(new_id);

    /// NOTE: free ID 521
    new_id += 2;
    create_auto_cmd(ui->ckIgnoredFiles, new_id);

    create_auto_cmd(ui->comboShowItems, new_id);
    create_auto_cmd(ui->comboFontName, new_id);
    create_auto_cmd(ui->comboAppStyle, new_id);
    create_auto_cmd(ui->comboDiffTool, new_id);
    create_auto_cmd(ui->comboOpenNewEditor, new_id);
    create_auto_cmd(ui->comboTabPosition, new_id);
    create_auto_cmd(ui->comboTextCodex, new_id);
    create_auto_cmd(ui->comboToolBarStyle, new_id);
    create_auto_cmd(ui->comboUserStyle, new_id);
    /// NOTE: free ID 532
    /// NOTE: free ID 533
    new_id += 3;
    create_auto_cmd(ui->btnFindReplace, new_id, mActions.check_location("edit-find-replace.png"));
    contextmenu_text_view.push_back(new_id);
    create_auto_cmd(ui->comboWordWrap, new_id);
    /// NOTE: free ID 536
    new_id += 2;
    create_auto_cmd(ui->spinFontSize, new_id);

    create_auto_cmd(ui->comboFindBox, new_id, "")->setShortcut(QKeySequence(Qt::ControlModifier | Qt::AltModifier | Qt::Key_L));
    mActions.getAction(new_id)->setToolTip(tr("Go to Line"));

    create_auto_cmd(ui->comboFindBox, new_id, "")->setShortcut(QKeySequence(Qt::ControlModifier | Qt::AltModifier | Qt::Key_E));
    mActions.getAction(new_id)->setToolTip(tr("Execute"));

    contextmenu_text_view.insert(contextmenu_text_view.end(), { Cmd::SubExtra, Cmd::EditToUpper, Cmd::EditToLower, Cmd::EditToggleComment, Cmd::EditToSnakeCase, Cmd::EditToCamelCase, Cmd::EditTabIndent, Cmd::EditTabOutdent });


    auto found_sub = std::find_if(Cmd::mContextMenuTextView.begin(), Cmd::mContextMenuTextView.end(), [](auto cmd) { return is_any_equal_to(cmd, Cmd::SubFind, Cmd::SubFiles, Cmd::SubExtra); });
    if (found_sub == Cmd::mContextMenuTextView.end())
    {
        Cmd::mContextMenuTextView.insert(Cmd::mContextMenuTextView.end(), contextmenu_text_view.begin(), contextmenu_text_view.end());
    }

    auto cmd = Cmd::SubFiles;
    QString name = Cmd::getCommand(cmd);
    mActions.createAction(cmd, name, name);
    mActions.setFlags(cmd, ActionList::Flags::Modified, Flag::set);

    cmd = Cmd::SubFind;
    name = Cmd::getCommand(cmd);
    mActions.createAction(cmd, name, name);
    mActions.setFlags(cmd, ActionList::Flags::Modified, Flag::set);

    cmd = Cmd::SubExtra;
    name = Cmd::getCommand(cmd);
    mActions.createAction(cmd, name, name);
    mActions.setFlags(cmd, ActionList::Flags::Modified, Flag::set);

    for (const auto& fAction : std::as_const(mActions.getList()))
    {
        auto cmd = static_cast<Cmd::eCmd>(fAction.first);
        mActions.setFlags(cmd, ActionList::Flags::BuiltIn);
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
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
    for (QDockWidget* dock_widget : std::as_const(dock_widgets))
    {
        get_widget(dock_widget)->addAction(action);
    }
}

QAction* MainWindow::create_auto_cmd(QWidget *widget, Cmd::eCmd &new_id, const QString& icon_path)
{
    const auto comand_id = mActions.createNewID(new_id);

    QAbstractButton*      button   = dynamic_cast<QAbstractButton*>(widget);
    QComboBox*            combobox = dynamic_cast<QComboBox*>(widget);
    QSpinBox*             spinbox  = dynamic_cast<QSpinBox*>(widget);
    QString               name     = button != 0 ? button->text() : "";
    QString               command  = widget != 0 ? widget->toolTip() : "";
    if (combobox)
    {
        name    = combobox->toolTip();
        command = combobox->statusTip();
        if  (command.isEmpty())
        {
            combobox->setStatusTip(name);
            command = name;
        }
    }
    if (button)
    {
        if (command.isEmpty())
        {
            command = button->statusTip();
        }
    }
    if (spinbox)
    {
        name    = spinbox->toolTip() + tr(" up");
        command = spinbox->statusTip();
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

    new_id = comand_id;

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
    if (spinbox)
    {
        connect(action, SIGNAL(triggered()), spinbox, SLOT(stepUp()));
        /// NOTE: special for spin box are two actions
        const auto comand_id2 = mActions.createNewID(new_id);
        action  = mActions.createAction(comand_id2, spinbox->toolTip() + tr(" down"), command, widget);
        mActions.setFlags(comand_id2, Type::IgnoreTypeStatus, Flag::set, ActionList::Data::StatusFlagEnable);
        mActions.setIconPath(comand_id2, ":/resource/24X24/window-close.png");
        mActions.setFlags(comand_id2, ActionList::Flags::FunctionCmd);
        connect(action, SIGNAL(triggered()), spinbox, SLOT(stepDown()));
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

bool MainWindow::handleInThread(bool force_thread)
{
    const QAction *action = qobject_cast<QAction *>(sender());
    if (action && (!mWorker.isBusy() || mAppendToBatch))
    {
        const uint flags = action->data().toList()[ActionList::Data::Flags].toUInt();
        if ((flags & (ActionList::Flags::CallInThread|ActionList::Flags::Asynchroneous)) != 0)
        {
            return true;
        }
        return false;
    }
    return force_thread && !mWorker.isBusy() && action;
}

void MainWindow::on_emit_temp_file_path(const QString& path)
{
    QMutexLocker lock(&mTempFileMutex);
    mTempFilePath = path;
    if (path.isEmpty())
    {
        mDeleteTempFile = true;
    }
}

void MainWindow::timerEvent(QTimerEvent *  event )
{
    bool temp_file_is_open = mTempFile.isOpen();
    if (temp_file_is_open)
    {
        int size {0};
        do
        {
            QByteArray array = mTempFile.readLine();
            size = array.size();
            if (size)
            {
                if (array[size-1] == '\n')
                {
                    array[size-1] = 0;
                }
                if (mOutput2secondTextView)
                {
                    if (!mBackgroundTextView)
                    {
                        // backgound process output text view
                        mBackgroundTextView.reset(create_new_text_browser("", tr("Background Log")));
                        QDockWidgetX* dock = dynamic_cast<QDockWidgetX*>(mBackgroundTextView.data()->parent());
                        dock->setObjectName(background_textbrowser);
                        dock->setAttribute(Qt::WA_DeleteOnClose, false);
                        mBackgroundTextView->setReadOnly(true);
                    }
                    if (mBackgroundTextView)
                    {
                        appendTextToBrowser(array, true, "", !mBackgroundTextView->isVisible(), true);
                        mBackgroundTextView->moveCursor(QTextCursor::End);
                    }
                }
                else
                {
                    appendTextToBrowser(array, true, "", false);
                    ui->textBrowser->moveCursor(QTextCursor::End);
                }
            }
        }
        while (size > 0);

        QMutexLocker lock(&mTempFileMutex);
        if (mTempFilePath.isEmpty() || mDeleteTempFile)
        {
            mDeleteTempFile = false;
            lock.unlock();
            mTempFile.close();
            temp_file_is_open = false;
        }
    }

    if (!temp_file_is_open)
    {
        QMutexLocker lock(&mTempFileMutex);
        if (mTempFilePath.size())
        {
            mTempFile.setFileName(mTempFilePath);
            lock.unlock();
            mTempFile.open(QFile::ReadOnly|QFile::Text);
        }
    }

    int id = event->timerId();
    if (mTimerTask.contains(id))
    {
        killTimer(event->timerId());
        QVariantList& list = mTimerTask[id];
        uint post_cmd = list.at(0).toUInt();
        Cmd::eCmd cmd = list.size() > 1 ? static_cast<Cmd::eCmd>(list.at(1).toUInt()) : Cmd::Invalid;
        perform_post_cmd_action(static_cast<Cmd::ePostAction>(post_cmd), {}, cmd);
        mTimerTask.remove(id);
    }

    // if (m_initializing_elements)
    // {
    //     centralWidget()->setMinimumWidth(200);
    //     centralWidget()->setMaximumWidth(QWIDGETSIZE_MAX);
    //     m_initializing_elements = false;
    // }
}

void MainWindow::expand_tree_items()
{
    QTreeWidget* ftw = focusedTreeWidget();
    if (ftw)
    {
        ftw->expandAll();
    }
    else
    {
        code_browser*text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
        if (text_browser)
        {
            text_browser->set_sections_visible(true);
        }
    }
}

void MainWindow::collapse_tree_items()
{
    QTreeWidget* ftw = focusedTreeWidget();
    if (ftw)
    {
        ftw->collapseAll();
    }
    else
    {
        code_browser*text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
        if (text_browser)
        {
            text_browser->set_sections_visible(false);
        }
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
    else if (aAlsoSource && ui->treeSource->hasFocus())
    {
        return ui->treeSource;
    }
    return nullptr;
}

void MainWindow::performCustomGitActionSettings()
{
    CustomGitActions edit_custom_git_actions(mActions, mMergeTools, mMiscelaneousItems, this);
    edit_custom_git_actions.mExperimental = mExperimental;
    edit_custom_git_actions.mExternalIconFiles = mExternalIconFiles;

    connect(&edit_custom_git_actions, SIGNAL(initCustomAction(QAction*)), this, SLOT(initCustomAction(QAction*)));
    connect(&edit_custom_git_actions, SIGNAL(removeCustomToolBar(QString)), this, SLOT(removeCmdToolBar(QString)));
    connect(&edit_custom_git_actions, SIGNAL(read_commands_from(QString)), this, SLOT(read_custom_commands(QString)));
    connect(&edit_custom_git_actions, SIGNAL(store_commands_to(QString)), this, SLOT(store_custom_commands(QString)));

    auto null_function  = g_test_command_only;
#ifdef USE_BOOST
    g_test_command_only = boost::bind(&CustomGitActions::display_command_text, &edit_custom_git_actions, _1);
#else
    g_test_command_only = std::bind(&CustomGitActions::display_command_text, &edit_custom_git_actions, _1);
#endif

    ensure_dialog_on_same_screen(&edit_custom_git_actions, this);
    if (edit_custom_git_actions.exec() == QDialog::Accepted)
    {
        if (edit_custom_git_actions.isMergeToolsChanged())
        {
            initMergeTools();
        }
        mExternalIconFiles = edit_custom_git_actions.mExternalIconFiles;

        if (edit_custom_git_actions.isMiscelaneousItemChanged())
        {
            init_miscelaneous_items(false);
        }
    }

    for (unsigned int i=0; i < Cmd::mToolbars.size(); ++i)
    {
        if (i < mToolBars.size())
        {
            mToolBars[i]->clear();
            mActions.fillToolbar(*mToolBars[i], Cmd::mToolbars[i]);
        }
        else
        {
            addCmdToolBar(i);
        }
    }

    swap(g_test_command_only, null_function);
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
    ensure_dialog_on_same_screen(&dlg, this);
    dlg.exec();
}
void MainWindow::on_comboToolBarStyle_currentIndexChanged(int index)
{
#if 0
    // for single toolbar
    mToolBars[0]->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(index));
#else
    setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(index));
#endif
}

void MainWindow::comboAppStyleTextChanged(const QString &style)
{
    QApplication::setStyle(QStyleFactory::create(style));
}

void MainWindow::on_comboUserStyle_currentIndexChanged(int index)
{
    bool restart_necessary = false;
    switch (index)
    {
    case UserStyle::None:
    {
        setStyleSheet("");
        QApplication::setPalette(PaletteColorSelector::get_default_palette());
        ColorSelector::set_dark_mode(false);
    } break;
    case UserStyle::User:
    {
        QFile f(mStylePath);
        if (!f.exists())
        {
            TRACEX(Logger::error, "Unable to set stylesheet, file " <<  mStylePath << " not found");
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            setStyleSheet(ts.readAll());
            ColorSelector::set_dark_mode(true);
            restart_necessary = true;
        }
        break;
    }
    case UserStyle::Palette:
    {
        PaletteColorSelector dlg;
        if (!m_initializing_elements)
        {
            ensure_dialog_on_same_screen(&dlg, this);
            if (dlg.exec() == QDialog::Accepted)
            {
                QApplication::setPalette(dlg.get_palette());
                ColorSelector::set_dark_mode(true);
                restart_necessary = true;
            }
            else
            {
                ui->comboUserStyle->setCurrentIndex(0);
                on_comboUserStyle_currentIndexChanged(0);
                return;
            }
        }
        else
        {
            QApplication::setPalette(dlg.get_palette());
            ColorSelector::set_dark_mode(true);
        }
        break;
    }
    case UserStyle::LoadStyle:
    {
        QString style_file = QFileDialog::getOpenFileName(this, tr("Select style for app"), "", tr("Style File (*.qss);;All Files (*.*)"));
        QFile f(style_file);
        if (!f.exists())
        {
            TRACEX(Logger::error, "Unable to set stylesheet, file " <<  mStylePath << " not found");
            ui->comboUserStyle->setCurrentIndex(0);
            on_comboUserStyle_currentIndexChanged(0);
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            setStyleSheet(ts.readAll());
            restart_necessary = true;
        }
    } break;
    }

    Highlighter::Language::mSelectedLineBackground = ColorSelector::is_dark_mode() ? Qt::darkYellow : Qt::yellow;
    if (restart_necessary)
    {
        QMessageBox request(QMessageBox::Question, windowTitle(), tr("Restart Gitview"), QMessageBox::Yes);
        request.addButton(QMessageBox::No);
        ensure_dialog_on_same_screen(&request);
        if (request.exec() == QMessageBox::Yes)
        {
            mRestartApp = true;
            close();
        }
    }
}


void MainWindow::comboFindBoxIndexChanged(int index)
{
    static QString oldFindAllToolTip;
    static QString oldFindAllText;
    static QMetaObject::Connection find_all_connected;
    static QMetaObject::Connection find_next_connected;

    if (oldFindAllToolTip.isEmpty())
    {
        oldFindAllToolTip = ui->btnFindAll->toolTip();
    }

    if (oldFindAllText.isEmpty())
    {
        oldFindAllText = ui->btnFindAll->text();
    }

    enum eflag
    {
       Next=1, Previous=2, All=4, Replace=8
    };

    FindView find = static_cast<FindView>(index);
    if (find == FindView::ExecuteCommand )
    {
        ui->btnFindAll->setText(tr("Execute"));
        ui->edtFindText->setToolTip(tr("Enter git or bash command and hit \"Return\" to execute"));
        if (!find_all_connected)
        {
            find_all_connected = connect(ui->edtFindText, SIGNAL(returnPressed()), ui->btnFindAll, SLOT(click()));
        }
    }
    else if (find == FindView::GoToLineInText )
    {
        ui->btnFindAll->setText(tr("Go"));
        ui->edtFindText->setToolTip(tr("Enter line number and hit \"Return\" to go to line"));
        if (!find_all_connected)
        {
            find_all_connected = connect(ui->edtFindText, SIGNAL(returnPressed()), ui->btnFindAll, SLOT(click()));
        }
    }
    else
    {
        if (find_all_connected)
        {
            disconnect(ui->edtFindText, SIGNAL(returnPressed()), ui->btnFindAll, SLOT(click()));
            find_all_connected = {};
        }
        if (!find_next_connected)
        {
            find_next_connected = connect(ui->edtFindText, SIGNAL(returnPressed()), ui->btnFindNext, SLOT(click()));
        }
        ui->edtFindText->setToolTip(oldFindAllToolTip);
        ui->btnFindAll->setText(oldFindAllText);
    }

    if (ui->btnFindAll->text() != oldFindAllText && find_next_connected)
    {
        disconnect(ui->edtFindText, SIGNAL(returnPressed()), ui->btnFindNext, SLOT(click()));
        find_next_connected = {};
    }


    ui->ckFindWholeWord->setEnabled(!mFastFileSearch);
    uint32_t flags = 0;
    switch(find)
    {
    case FindView::Text:                ui->statusBar->showMessage(tr("Search in Text Editor"));                       flags = Next|Previous|All|Replace; break;
    case FindView::GoToLineInText:      ui->statusBar->showMessage(tr("Go to line in Text Editor"));                   flags = All; break;
    case FindView::FindTextInFilesView: ui->statusBar->showMessage(tr("Search for text in find results"));             flags = Next|Previous;  break;
    case FindView::Source:              ui->statusBar->showMessage(tr("Search files or folders in Repository View"));  flags = Next|Previous|All; break;
    case FindView::History:             ui->statusBar->showMessage(tr("Search item in History View"));                 flags = Next|Previous; break;
    case FindView::Branch:              ui->statusBar->showMessage(tr("Search item in Branch View"));                  flags = Next|Previous; break;
    case FindView::Stash:               ui->statusBar->showMessage(tr("Search in Stash View"));                        flags = Next|Previous; break;
    case FindView::ExecuteCommand:      ui->statusBar->showMessage(tr("Execute git command for selected Repository")); flags = All; break;
    }

    set_widget_and_action_enabled(ui->btnFindNext,     flags & Next     ? true : false);
    set_widget_and_action_enabled(ui->btnFindPrevious, flags & Previous ? true : false);
    set_widget_and_action_enabled(ui->btnFindAll,      flags & All      ? (ui->edtFindText->text().size() > 0) : false);
    set_widget_and_action_enabled(ui->btnFindReplace,  flags & Replace  ? true : false);
}

void MainWindow::combo_triggered()
{
    const auto action = qobject_cast<QAction *>(sender());
    const auto combofind_actions = ui->comboFindBox->actions();
    if (combofind_actions.size() && combofind_actions.contains(action))
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
        FindView index = FindView::ExecuteCommand;
        if      (text_browser && text_browser->hasFocus())
        {
            switch(combofind_actions.indexOf(action))
            {
            case 0: index = FindView::Text; break;
            case 1: index = FindView::GoToLineInText; break;
            case 2: index = FindView::ExecuteCommand; break;
            }
        }
        else if (ui->treeHistory->hasFocus())  index = FindView::History;
        else if (ui->treeBranches->hasFocus()) index = FindView::Branch;
        else if (ui->treeStash->hasFocus())    index = FindView::Stash;
        else if (ui->treeSource->hasFocus())   index = FindView::Source;

        QString find_text;
        QTreeWidget* tree_view = nullptr;
        switch (index)
        {
        case FindView::Text:
        {
            const auto & cursor = text_browser->textCursor();
            find_text = cursor.selectedText();
            if (find_text.isEmpty())
            {
                find_text = get_word_at_position(cursor.block().text(), cursor.positionInBlock());
            }
        }   break;
        case FindView::History:             tree_view = ui->treeHistory; break;
        case FindView::Branch:              tree_view = ui->treeBranches; break;
        case FindView::Stash:               tree_view = ui->treeStash; break;
        case FindView::Source:              tree_view = ui->treeSource; break;
        case FindView::FindTextInFilesView: tree_view = ui->treeFindText; break;
        case FindView::GoToLineInText:
        case FindView::ExecuteCommand:
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
            ui->edtFindText->setFocus();
        }
        else
        {
            ui->edtFindText->selectAll();
            ui->edtFindText->setFocus();
        }
    }

    const auto combo_show_items_actions = ui->comboShowItems->actions();
    if (combo_show_items_actions.size() && combo_show_items_actions.first() == action)
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

void MainWindow::FindReplaceAll()
{
    while (ui->btnFindReplace->isEnabled())
    {
        find_function(find::replace);
    }
}

void MainWindow::GoToLine()
{
    ui->comboFindBox->setCurrentIndex(static_cast<int>(FindView::GoToLineInText));
    ui->edtFindText->selectAll();
    ui->edtFindText->setFocus();
}

void MainWindow::on_btnFindNext_clicked()
{
    find_function(find::forward);
}

void MainWindow::on_btnFindPrevious_clicked()
{
    find_function(find::backward);
}

void MainWindow::on_btnFindAll_clicked()
{
    if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::Text))
    {
        find_function(find::all);
    }
    else if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::GoToLineInText))
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
        if (text_browser)
        {
            text_browser->go_to_line(ui->edtFindText->text().toInt());
        }
    }
    else
    {
        QAction* action = mActions.getAction(Cmd::CustomTestCommand);
        if (action)
        {
            action->setStatusTip(ui->edtFindText->text());
            action->trigger();
        }
    }
}

void MainWindow::on_edtFindText_textChanged(const QString &arg)
{
    ui->btnFindAll->setEnabled(arg.size() > 0);
}

void MainWindow::on_btnFindReplace_clicked()
{
    if (ui->comboFindBox->currentIndex() == static_cast<int>(FindView::Text))
    {
        find_in_text_view(find::replace);
    }
}

void MainWindow::find_function(find find_item)
{
    if (   ui->comboFindBox->currentIndex() == static_cast<int>(FindView::Text))
    {
        if (find_item == find::all && mContextMenuSourceTreeItem)
        {
            find_text_in_files();
        }
        else
        {
            find_in_text_view(find_item);
        }
    }
    else
    {
        find_in_tree_views(find_item);
    }
}

void MainWindow::find_in_text_view(find find_item)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression::PatternOption reg_ex_case = QRegularExpression::CaseInsensitiveOption;
#else
    Qt::CaseSensitivity reg_ex_case = Qt::CaseInsensitive;
#endif
    int                 find_flag   = find_item == find::forward || find_item == find::replace ? 0 : QTextDocument::FindBackward;

    code_browser* text_browser = dynamic_cast<code_browser*>(get_active_editable_widget());
    if (text_browser)
    {
        if (text_browser->extraSelections().size() && find_item == find::replace)
        {
            text_browser->insertPlainText(ui->edtReplaceText->text());
        }

        if (ui->ckFindCaseSensitive->isChecked())
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            reg_ex_case = QRegularExpression::NoPatternOption;
#else
            reg_ex_case = Qt::CaseSensitive;
#endif
            find_flag |= QTextDocument::FindCaseSensitively;
        }
        if (ui->ckFindWholeWord->isChecked())
        {
            find_flag |= QTextDocument::FindWholeWords;
        }

        bool found_text = false;
        if (ui->ckFindRegEx->isChecked())
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            found_text = text_browser->find(QRegularExpression(ui->edtFindText->text(), reg_ex_case), static_cast<QTextDocument::FindFlag>(find_flag));
#else
            found_text = text_browser->find(QRegExp(ui->edtFindText->text(), reg_ex_case), static_cast<QTextDocument::FindFlag>(find_flag));
#endif
        }
        else
        {
            found_text = text_browser->find(ui->edtFindText->text(), static_cast<QTextDocument::FindFlag>(find_flag));
        }
        if (found_text)
        {
            showDockedWidget(text_browser);
        }
        ui->btnFindReplace->setEnabled(found_text);
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
        case FindView::Text: break;
        case FindView::GoToLineInText: break;
        case FindView::ExecuteCommand: break;
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            tree_match_flag = Qt::MatchRegularExpression;
#else
            tree_match_flag = Qt::MatchRegExp;
#endif
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

        /// NOTE: also possible flags
        /// - MatchStartsWith = 2,
        /// - MatchEndsWith = 3,
        /// - MatchFixedString = 8,
        /// - MatchWrap = 32,

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
                item->setSelected(true);
#else
                tree_view->setItemSelected(item, true);
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
                item->setSelected(i == property.mIndex);
#else
                tree_view->setItemSelected(item, i == property.mIndex);
#endif
                if (i == property.mIndex)
                {
                    auto* parent = item->parent();
                    while (parent)
                    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
                        parent->setExpanded(true);
#else
                        tree_view->setItemExpanded(parent, true);
#endif
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
    if (mSearchResultsAsSearchTree)
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
    const bool fast_search = mFastFileSearch;
    QString find_result;
    QString find_command;
    QString search_path    = ui->treeSource->getItemFilePath(mContextMenuSourceTreeItem);
    QString search_pattern = ui->edtFindText->text();
    if (fast_search)
    {
        /// NOTE: Usage  : fsrc [options] term
        /// Options:
        ///  -d [ --dir ] arg      Search folder
        ///  -e [ --ext ] arg      Search only in files with extension <arg>, equiv. to
        ///                        --glob '*.ext'
        ///  -g [ --glob ] arg     Search only in files filtered by <arg> glob, e.g.
        ///                        '*.txt'; overrides --ext
        ///  -h [ --help ]         Help
        ///  --html                open web page with results
        ///  -i [ --ignore-case ]  Case insensitive search
        ///  --no-git              Disable search with 'git ls-files'
        ///  --no-colors           Disable colorized output
        ///  --no-piped            Disable piped output
        ///  --no-uri              Print w/out file:// prefix
        ///  --only-files          Only print filenames
        ///  --piped               Enable piped output
        ///  -q [ --quiet ]        only print status
        ///  -r [ --regex ]        Regex search (slower)

        QFileInfo info(search_path);
        QString options = "--no-piped";
        if (!ui->ckFindCaseSensitive->isChecked())
        {   /// i: ignore case
            options += " -i";
        }
        if (ui->ckFindRegEx->isChecked())
        {   /// E: regular expression
            options += " -r";
        }
        if (info.isFile())
        {
            find_command = tr("%1 -d %2 -g %3 %4 '%5'").arg(mFindFsrc, info.absolutePath(), info.fileName(), options, search_pattern);
        }
        else
        {
            find_command = tr("%1 -d %2 %3 '%4'").arg(mFindFsrc, search_path, options, search_pattern );
        }
    }
    else
    {
        /// NOTE: grep -rnHsI[oiEw] 'search_pattern' 'path'
        /// r: recursive
        /// n: line number
        /// H: file name
        /// s: no message
        /// I: no text search in binary files
        /// o: show only match without line
        QString options = "-rnHsI";
        if (!ui->ckFindCaseSensitive->isChecked())
        {   /// i: ignore case
            options += "i";
        }
        if (ui->ckFindRegEx->isChecked())
        {   /// E: regular expression
            options += "E";
        }
        if (ui->ckFindWholeWord->isChecked())
        {   /// w: whole word
            options += "w";
        }
        find_command = tr("%1 %2 '%3' '%4'").arg(mFindGrep, options, search_pattern, search_path);
    }
    int result = execute(find_command, find_result);
    if (result == 0)
    {
        QString repository_root = getTopLevelItem(*ui->treeSource, mContextMenuSourceTreeItem)->text(QSourceTreeWidget::Column::FileName);
        QStringList strings =  {tr("Search expression: %1").arg(search_pattern), "", repository_root};
        auto new_tree_root_item    = find_child_item(ui->treeFindText->invisibleRootItem(), FindTextColumn::File, strings[0]);
        auto repository_root_item  = find_child_item(ui->treeFindText->invisibleRootItem(), FindTextColumn::FoundText, strings[2]);
        if (!new_tree_root_item || !repository_root_item)
        {
            new_tree_root_item = new QTreeWidgetItem(strings);
        }
        ui->treeFindText->addTopLevelItem(new_tree_root_item);
        const auto found_items = find_result.split('\n');
        const QFileInfo info_search_path(search_path);
        QTreeWidgetItem *search_path_item = mContextMenuSourceTreeItem;
        if (info_search_path.isFile() && search_path_item)
        {
            search_path      = info_search_path.absolutePath();
            search_path_item = search_path_item->parent();
        }

        if (fast_search)
        {
            const QString file_id = "file://";
            QString current_file;
            for (const QString& found_item : std::as_const(found_items))
            {
                int pos = found_item.indexOf(file_id);
                if (pos != -1)
                {
                    current_file = found_item.mid(pos + file_id.size());
                    if (containsPathAsChildren(search_path_item, QSourceTreeWidget::Column::FileName, current_file.mid(search_path.size() + 1)))
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
            for (const QString& found_item : std::as_const(found_items))
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
                    if (containsPathAsChildren(search_path_item, QSourceTreeWidget::Column::FileName, file_path.mid(search_path.size() + 1)))
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
            if (mSearchResultsAsSearchTree)
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
                open_file(repository_root + "/" + file_path_part, item->text(FindColumn::Line).toInt(), false);
            }
            else
            {
                open_file(file_path_part, item->text(FindColumn::Line).toInt(), false);
                find_root_and_partial_path(*ui->treeSource, repository_root, file_path_part);
            }

            ui->treeSource->find_item(repository_root, file_path_part);
        }
    }
}

void MainWindow::on_treeFindText_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    mActions.fillContextMenue(menu, Cmd::mContextMenuFindTextTree);
    menu.exec(check_screen_position(pos, true, ui->treeFindText));
}

void MainWindow::on_ckTypeConverter_stateChanged(int active)
{
    mBinaryValuesView->receive_external_data(active);
    showDockedWidget(mBinaryValuesView.data(), active);
}

void MainWindow::on_spinTabulator_valueChanged(int width)
{
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ textbrowser, new_textbrowser });
    for (QDockWidget* dock_widget : dock_widgets)
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(get_widget(dock_widget));
        text_browser->setTabstopCharacters(width);
    }
}

void MainWindow::comboTabPositionIndexChanged(int index)
{
    setTabPosition(Qt::RightDockWidgetArea, static_cast<QTabWidget::TabPosition>(index));
}

void MainWindow::on_comboWordWrap_currentIndexChanged(int index)
{
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ textbrowser, new_textbrowser });
    for (QDockWidget* dock_widget : dock_widgets)
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(get_widget(dock_widget));
        text_browser->setWordWrapMode(static_cast<QTextOption::WrapMode>(index));
    }
}


void MainWindow::setFontForViews(int)
{
    QFont font;
    font.setFamily(ui->comboFontName->currentText());
    font.setFixedPitch(true);
    font.setPointSize(ui->spinFontSize->value());
    ui->textBrowser->setFont(font);
    ui->tableBinaryView->setFont(font);
    QList<QDockWidget *> dock_widgets = get_dock_widget_of_name({ new_textbrowser });
    for (QDockWidget* dock_widget : dock_widgets)
    {
        code_browser* text_browser = dynamic_cast<code_browser*>(get_widget(dock_widget));
        if (text_browser)
        {
            text_browser->setFont(font);
        }
    }
}

void MainWindow::on_comboOpenNewEditor_currentIndexChanged(int )
{
    bool allfiles = additional_editor() == AdditionalEditor::OnNewFile;
    auto*action = mActions.getAction(Cmd::CloseAll);
    if (action) action->setEnabled(allfiles);
    action = mActions.getAction(Cmd::SaveAll);
    if (action) action->setEnabled(allfiles);
    action = mActions.getAction(Cmd::ReloadAll);
    if (action) action->setEnabled(allfiles);
    QDockWidget*dock = dynamic_cast<QDockWidget*>(ui->textBrowser->parent());
    if (dock)
    {
        bool only_viewer = additional_editor() != AdditionalEditor::None;
        dock->setWindowTitle(only_viewer ? tr("Text Viewer") : tr("Text Viewer / Editor"));
    }
}

void MainWindow::on_ckAppendToBatch_clicked(bool checked)
{
    mWorker.setAppendToBatch(checked);
}

void MainWindow::move_active_window_to(FirstTab::e tab, QDockWidget* dock)
{
    QDockWidget* parent = get_first_dock_tab(tab);
    if (parent)
    {
        tabifyDockWidget(parent, dock);
    }
}

void MainWindow::open_external_link(const QString& link)
{
    QString command = mExternalFileOpenCmd;
    QString result;
    command += " ";
    command += link;
    execute(command, result);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
#if 1
    QDockWidget* dock = nullptr;
    QList<QDockWidget *> dock_widgets = findChildren<QDockWidget *>();
    for (auto dock_widget = dock_widgets.begin(); dock_widget != dock_widgets.end(); ++dock_widget)
    {
        auto* widget = get_widget(*dock_widget);
        if (get_active(widget))
        {
            dock = *dock_widget;
            break;
        }
    }

    QMenu *menu = createPopupMenu();
    QActionGroup move_tab(this);

    if (dock != nullptr &&
        dock != get_first_dock_tab(FirstTab::text_view) &&
        dock != get_first_dock_tab(FirstTab::tree_view) &&
        dock != get_first_dock_tab(FirstTab::web_view))
    {
        QMenu*sub_menu = menu->addMenu(tr("Move [ %1 ] to").arg(dock->windowTitle()));
        for (const auto &name : std::as_const(mDockAreaNames))
        {
            QAction* action = sub_menu->addAction(name);
            move_tab.addAction(action);
        }
    }
    QAction* selected = menu->exec(check_screen_position(event->globalPos()));
    int index = move_tab.actions().indexOf(selected);
    if (index != -1)
    {
        move_active_window_to(static_cast<FirstTab::e>(index), dock);
    }

    delete menu;
#else
    QMainWindow::contextMenuEvent(event);
#endif

}

