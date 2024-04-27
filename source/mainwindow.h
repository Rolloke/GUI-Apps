#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "workerthreadconnector.h"
#include "actions.h"
#include "qdockwidgetx.h"

#include <QMainWindow>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSemaphore>
#include <QTime>
#include <QMutex>
#include <boost/optional.hpp>


namespace Ui
{
class MainWindow;
}

class MergeDialog;
class QGraphicsItem;
class QAbstractButton;
class binary_values_view;
class QLabel;
class code_browser;
#ifdef WEB_ENGINE
class QWebEngineView;
class MarkdownProxy;
#else
class QTextBrowser;
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class QBranchTreeWidget;
public:
    explicit MainWindow(const QString& aConfigName, QWidget *parent = 0);
    ~MainWindow();

private:

    enum class find
    {
        forward, backward, all, replace
    };

    enum class copy
    {
        name, path, file
    };

    enum class Work
    {
        None,
        DetermineGitMergeTools,
        ApplyGitCommand,
        ApplyCommand,
        ApplyCommandRecursive,
        AsynchroneousCommand,
        InsertPathFromCommandString,
        ShowAllFiles,
        ShowAllGitActions,
        ShowModified,
        ShowDeleted,
        ShowAdded,
        ShowUnknown,
        ShowStaged,
        ShowUnMerged,
        ShowSelected,
        // NOTE: further git items are added here
        // show out of sync...
        Last
    };

    enum class Message
    {
        UpdateBytes = static_cast<int>(Work::Last),
        InsertTreeItem
    };

    enum class ComboShowItems
    {
        AllFiles,
        AllGitActions,
        GitModified,
        GitAdded,
        GitDeleted,
        GitUnknown,
        Gitstaged,
        GitUnmerged,
        GitSelected
    };

    enum class FindView
    {
        Text,
        GoToLineInText,
        Source,
        History,
        Branch,
        Stash,
        FindTextInFilesView
    };

    struct FindColumn { enum e
    {
        FilePath,
        Line,
        FoundTextLine,
        Size
    }; };

    struct HistoryFile { enum e
    {
        Content,
        Extension,
        FilePath,
        Size
    }; };

    struct UserStyle { enum e
    {
        None,
        User,
        Palette,
        LoadStyle
    }; };

    enum class AdditionalEditor
    {
        None,
        One,
        OnNewFile
    };

    enum class Editor
    {
        Viewer,
        Active,
        CalledFromAction,
        CalledFromWidget,
        All
    };
    struct FirstTab { enum e
    {
        text_view,
        web_view,
        tree_view
    }; };

    QTreeWidgetItem* insert_file_path(QTreeWidgetItem* , const QString& );

    void     call_git_history_diff_command();
    int      call_git_command(QString, const QString&, const QString&, QString&, const QString& git_root_path={});

    QString  get_git_command_option(const git::Type& type, uint command_flags, const QVariantList& variant_list);
    void     perform_post_cmd_action(uint post_cmd, const git::Type& type = {});

    QString  getConfigName() const;

    bool     iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir=0, QTreeWidgetItem* aParentItem=0);
    void     insertSourceTree(const QDir& fSource, int fItem);

    QDir     initDir(const QString& aDirPath, int aFilter=0);
    void     initCodecCombo();

    void     appendTextToBrowser(const QString& aText, bool append=false, const QString ext = {}, bool show=true, bool use_second_view=false);
    void     open_file(const QString& file_path, boost::optional<int> line_number = {}, bool reopen_file = true);

    QVariant handleWorker(const QVariant&);
    void     handleMessage(QVariant);
    bool     handleInThread(bool force_thread=false);
    void     on_emit_temp_file_path(const QString& path);

    void     updateTreeItemStatus(QTreeWidgetItem * aItem);
    void     getSelectedTreeItem();

    void     initContextMenuActions();
    void     initMergeTools(bool read_new_items = false);

    void     applyCommandToFileTree(const QString& aCommand);
    QString  applyGitCommandToFilePath(const QString& a_source, const QString& a_git_cmd, QString& a_result_str, bool force_thread=false);

    QTreeWidget* focusedTreeWidget(bool aAlsoSource=true);

    void     find_function(find forward);
    void     find_in_tree_views(find forward);
    void     find_in_text_view(find forward);
    void     find_text_in_files();

    bool     getShowTypeResult(const git::Type& fType);
    QWidget *get_active_editable_widget(const QString &file_path = {});
    code_browser* create_new_text_browser(const QString &file_path, const QString &name = {});
    bool     send_close_to_editable_widget(QWidget *editable_widget);
    void     reset_text_browser(code_browser*text_browser);
    QDockWidget *get_first_dock_tab(FirstTab::e number=FirstTab::web_view);

    void     copy_file(copy command);
    QAction* create_auto_cmd(QWidget*, const QString &icon_path="", git::Cmd::eCmd *new_id=nullptr);
    void     add_action_to_widgets(QAction * action);
    void     keyPressEvent(QKeyEvent *) override;
    void     mousePressEvent(QMouseEvent *event) override;
    void     timerEvent(QTimerEvent* event) override;
    void     closeEvent(QCloseEvent *event) override;
    void     dragEnterEvent(QDragEnterEvent *event);
    void     dropEvent(QDropEvent *even);

    AdditionalEditor additional_editor();
    bool close_editable_widgets(QWidget *&active_widget, Editor editor, bool &all_closed);
    bool shall_save(Editor editor);
    void update_widget_states(QWidget *widget);
    void check_set_current_path(QString &git_command);


Q_SIGNALS:
    void doWork(QVariant);

private Q_SLOTS:
    void updateRepositoryStatus(bool append=false);
    void textBrowserChanged(bool changed=false);

    void btnStoreText_clicked();
    void btnStoreAll_clicked();
    bool btnCloseText_clicked(Editor editor=Editor::CalledFromAction);
    void btnCloseAll_clicked();

    void on_treeSource_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_customContextMenuRequested(const QPoint &pos);
    void on_treeSource_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_ckHideEmptyParent_clicked(bool checked);
    void on_ckShortState_clicked(bool checked);
    void on_comboShowItems_currentIndexChanged(int index);

    void on_treeHistory_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeHistory_customContextMenuRequested(const QPoint &pos);
    void on_treeHistory_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeHistory_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_ckShowHistoryGraphically_clicked(bool checked);

    void on_treeBranches_customContextMenuRequested(const QPoint &pos);
    void on_treeBranches_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeStash_customContextMenuRequested(const QPoint &pos);
    void on_treeStash_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treeStash_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeFindText_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeFindText_customContextMenuRequested(const QPoint &pos);

    void on_ckTypeConverter_stateChanged(int active);
    void on_graphicsView_customContextMenuRequested(const QPoint &pos);

    void FindReplaceAll();
    void on_btnFindNext_clicked();
    void on_btnFindPrevious_clicked();
    void comboFindBoxIndexChanged(int index);
    void combo_triggered();

    void on_comboToolBarStyle_currentIndexChanged(int index);
    void comboAppStyleTextChanged(const QString &arg1);
    void on_comboUserStyle_currentIndexChanged(int index);
    void on_spinTabulator_valueChanged(int width);
    void comboTabPositionIndexChanged(int index);
    void on_comboOpenNewEditor_currentIndexChanged(int index);
    void setFontForViews(int);
    void close_text_browser(QDockWidgetX *widget, bool &closed);
    void remove_text_browser(QDockWidgetX *dock_widget);

    void showDockedWidget(QWidget* widget, bool show=true);
    QList<QDockWidget *> get_dock_widget_of_name(QStringList names);

    void dockWidget_topLevelChanged(bool);
    QDockWidgetX *create_dock_widget(QWidget *widget, const QString &name, const QString &object_name, bool connect_dock=true, Qt::Orientation orientation=Qt::Vertical);
    void clone_code_browser();
    void on_DockWidgetActivated(QDockWidget *dockWidget);

    void clearTrees();
    void delete_tree_item();
    void add_file_open_extension();
    void delete_file_open_extension();
    void open_file_externally();

    void perform_custom_command();
    void call_git_branch_command();
    void call_git_stash_command();

    void invoke_git_merge_dialog();
    void invoke_highlighter_dialog();
    void performCustomGitActionSettings();
    void call_git_commit();
    void call_git_clone();
    void call_git_move_rename(QTreeWidgetItem* dropped_target=0, bool *was_dropped = nullptr);
    void expand_tree_items();
    void collapse_tree_items();
    void addGitSourceFolder();
    void removeGitSourceFolder();
    void gitview_about();
    void deleteFileOrFolder();
    void selectTextBrowserLanguage();
    void killBackgroundThread();
    void copyFileName();
    void copyFilePath();
    void createBookmark();
    void showInformation();
    void compare_items(QString& item1, QString& item2);
    void OpenFile();
    void SaveFileAs();
    void modify_text();
    void check_reload(code_browser*);
    void set_show_line_numbers(bool);
    void on_ckAppendToBatch_clicked(bool checked);

    void on_btnFindAll_clicked();

    void on_btnFindReplace_clicked();

public Q_SLOTS:
    void initCustomAction(QAction* fAction);
    void updateSelectedLanguage(const QString&);
    void removeCmdToolBar(const QString &toolbar_name);
    void show_web_view(bool );

private:

    void     createDockWindows();
    void     addCmdToolBar(int i);
    QWidget* get_widget(QDockWidget*dock);
    bool     mDockedWidgetMinMaxButtons;

    Ui::MainWindow*       ui;
    QString               mGitCommand;
    WorkerThreadConnector mWorker;
    Work                  mCurrentTask;
    ActionList            mActions;
    QString               mConfigFileName;
    QTreeWidgetItem*      mContextMenuSourceTreeItem { nullptr };
    QSharedPointer<MergeDialog> mMergeDialog;
    QSharedPointer<binary_values_view> mBinaryValuesView;
#ifdef WEB_ENGINE
    QSharedPointer<QWebEngineView> mWebEngineView;
    QSharedPointer<MarkdownProxy>  m_markdown_proxy;
#else
    QSharedPointer<QTextBrowser> mTextRenderView;
#endif
    QSharedPointer<code_browser> mBackgroundTextView;

    string2bool_map       mMergeTools;
    std::vector<QToolBar*> mToolBars;
    struct tree_find_properties
    {
        tree_find_properties();
        int                   mFlags;
        int                   mIndex;
        int                   mColumn;
        QList<QTreeWidgetItem *> mItems;
    };

    QMap <QString, tree_find_properties> mTreeFindProperties;
    QStringList mHistoryFile;
    QString mStylePath;
    QString mFileCopyMimeType;
    QString mExternalIconFiles;
    QString mExternalFileOpenCmd;
    QString mFindGrep;
    QString mFindFsrc;
    QString mCompare2Items;
    QString mActivViewObjectName;
    QString mDefaultSourcePath;
    QMap<QString, QString> mExternalFileOpenExt;
    qint64  mWarnOpenFileSize;
    QLabel *m_status_line_label { nullptr };
    QLabel *m_status_column_label { nullptr };
    QMutex  mTempFileMutex;
    QString mTempFilePath;
    QFile   mTempFile;
    bool    mDeleteTempFile { false };
    bool    m_tree_source_item_double_clicked { false };
    QString mBranchHasSiblingsNotAdjoins;
    QString mBranchHasSiblingsAdjoins;
    QString mBranchHasChildrenNotHasSiblingsAdjoins;
    QString mBranchClosedHasChildrenHasSibling;
    QString mBranchOpenHasChildrenHasSibling;
    bool    m_initializing_elements { true };

    static constexpr char new_textbrowser[]    = "new_textbrowser";
    static constexpr char textbrowser[]        = "textbrowser";
    static constexpr char graphicsviewer[]     = "graphicsviewer";
    static constexpr char binary_table_view[]  = "binary_table_view";
    static constexpr char historyview[]        = "historyview";
    static constexpr char branchview[]         = "branchview";
    static constexpr char stashview[]          = "stashview";
    static constexpr char findview[]           = "findview";
    static constexpr char binaryview[]         = "binaryview";
    static constexpr char markdown_view[]      = "markdown_view";
    static constexpr char cloned_textbrowser[] = "cloned_textbrowser";
    static constexpr char background_textbrowser[] = "background_textbrowser";

};

#endif // MAINWINDOW_H
