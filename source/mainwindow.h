#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "workerthreadconnector.h"
#include "actions.h"

#include <QMainWindow>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSemaphore>
#include <QTime>
#include <boost/optional.hpp>


namespace Ui
{
class MainWindow;
}

class MergeDialog;
class QGraphicsItem;
class QAbstractButton;
class binary_values_view;
#ifdef WEB_ENGINE
class QWebEngineView;
class MarkdownProxy;
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class QBranchTreeWidget;
public:
    explicit MainWindow(const QString& aConfigName, QWidget *parent = 0);
    ~MainWindow();

Q_SIGNALS:
    void doWork(int, QVariant);

private Q_SLOTS:
    void cancelCurrentWorkTask();
    void updateGitStatus();
    void textBrowserChanged();

    void on_btnStoreText_clicked();
    void on_btnCloseText_clicked();

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

    void on_treeBranches_customContextMenuRequested(const QPoint &pos);
    void on_treeBranches_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeStash_customContextMenuRequested(const QPoint &pos);
    void on_treeStash_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treeStash_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeFindText_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeFindText_customContextMenuRequested(const QPoint &pos);

    void on_ckTypeConverter_stateChanged(int arg1);

    void on_graphicsView_customContextMenuRequested(const QPoint &pos);

    void on_btnFindNext_clicked();
    void on_btnFindPrevious_clicked();
    void on_btnFindAll_clicked();
    void on_comboFindBox_currentIndexChanged(int index);
    void combo_triggered();

    void on_comboToolBarStyle_currentIndexChanged(int index);
    void on_comboAppStyle_currentTextChanged(const QString &arg1);
    void on_comboUserStyle_currentIndexChanged(int index);
    void on_spinTabulator_valueChanged(int width);

#ifdef DOCKED_VIEWS
    void dockWidget_topLevelChanged(bool);
#else
    void showOrHideTrees(bool checked);
#endif
    void clearTrees();
    void delete_tree_item();

    void perform_custom_command();
    void call_git_history_diff_command();
    void call_git_branch_command();
    void call_git_stash_command();
    int  call_git_command(QString, const QString&, const QString&, QString&, const QString& git_root_path={});
    QString get_git_command_option(const git::Type& type, uint command_flags, const QVariantList& variant_list);

    void perform_post_cmd_action(uint post_cmd, const git::Type& type = {});
    void invoke_git_merge_dialog();
    void invoke_highlighter_dialog();
    void performCustomGitActionSettings();
    void call_git_commit();
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

public Q_SLOTS:
    void initCustomAction(QAction* fAction);
    void updateSelectedLanguage(const QString&);
#ifdef WEB_ENGINE
    void show_web_view(bool );
#endif
private:

    struct Work { enum e
    {
        None,
        DetermineGitMergeTools,
        ApplyGitCommand,
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
    }; };

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
        FindTextInFiles
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
        Palette

    }; };

    void     keyPressEvent(QKeyEvent *) override;
    void     mousePressEvent(QMouseEvent *event) override;

    QString  getConfigName() const;

    bool     iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir=0, QTreeWidgetItem* aParentItem=0);
    void     insertSourceTree(const QDir& fSource, int fItem);

    QDir     initDir(const QString& aDirPath, int aFilter=0);

    void     appendTextToBrowser(const QString& aText, bool append=false, const QString ext="");
    void     open_file(const QString& file_path, boost::optional<int> line_number);

    QVariant handleWorker(int, const QVariant&);
    void     handleMessage(int, QVariant);
    bool     handleInThread();

    void     updateTreeItemStatus(QTreeWidgetItem * aItem);
    void     getSelectedTreeItem();

    void     initContextMenuActions();
    void     initMergeTools(bool read_new_items = false);

    void     applyGitCommandToFileTree(const QString& aCommand);
    QString  applyGitCommandToFilePath(const QString& fSource, const QString& fGitCmd, QString& aResultStr);

    QTreeWidget* focusedTreeWidget(bool aAlsoSource=true);

    enum class find { forward, backward, all };
    void     find_function(find forward);
    void     find_in_tree_views(find forward);
    void     find_in_text_view(find forward);
    void     find_text_in_files();
    bool     getShowTypeResult(const git::Type& fType);

    enum class copy { name, path, file };
    void     copy_file(copy command);

    QAction* create_auto_cmd(QWidget*, const std::string& icon_path="", git::Cmd::eCmd *new_id=nullptr);

    void     showDockedWidget(QWidget* widget, bool hide=false);

#ifdef DOCKED_VIEWS
    void     createDockWindows();
    bool     mDockedWidgetMinMaxButtons;
#endif

    Ui::MainWindow*       ui;
    QString               mGitCommand;
    WorkerThreadConnector mWorker;
    const QAction*        mWorkerAction;
    Work::e               mCurrentTask;
    ActionList            mActions;
    QString               mConfigFileName;
    QTreeWidgetItem*      mContextMenuSourceTreeItem;
    QSharedPointer<MergeDialog> mMergeDialog;
    QSharedPointer<binary_values_view> mBinaryValuesView;
#ifdef WEB_ENGINE
    QSharedPointer<QWebEngineView> mWebEngineView;
    QSharedPointer<MarkdownProxy>  m_markdown_proxy;
#endif

    QString               mFontName;
    QString               mFileCopyMimeType;
    string2bool_map       mMergeTools;
    std::vector<QToolBar*> mToolBars;
    struct tree_find_properties
    {
        tree_find_properties();
        int                   mFlags;
        int                   mIndex;
        QList<QTreeWidgetItem *> mItems;
    };

    QMap <QString, tree_find_properties> mTreeFindProperties;
    QStringList mHistoryFile;
    QString mStylePath;
    QString mExternalIconFiles;

};

#endif // MAINWINDOW_H
