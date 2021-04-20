#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "workerthreadconnector.h"
#include "actions.h"
#include "gitignore.h"
#include "highlighter.h"

#include <QMainWindow>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSemaphore>
#include <QTime>


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class QBranchTreeWidget;
public:
    explicit MainWindow(const QString& aConfigName, QWidget *parent = 0);
    ~MainWindow();

Q_SIGNALS:
    void doWork(int );

private Q_SLOTS:
    void cancelCurrentWorkTask();
    void updateGitStatus();
    void textBrowserChanged();

    void on_btnStoreText_clicked();
    void on_btnCloseText_clicked();

    void on_treeSource_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_customContextMenuRequested(const QPoint &pos);

    void on_ckHideEmptyParent_clicked(bool checked);
    void on_ckShortState_clicked(bool checked);
    void on_comboShowItems_currentIndexChanged(int index);

    void on_treeHistory_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeHistory_customContextMenuRequested(const QPoint &pos);

    void on_treeBranches_customContextMenuRequested(const QPoint &pos);

#ifdef DOCKED_VIEWS
    void dockWidget_topLevelChanged(bool);
#else
    void showOrHideTrees(bool checked);
#endif
    void clearTrees();

    void performCustomGitActionSettings();

    void call_git_commit();
    void call_git_move_rename();
    void call_git_history_diff_command();
    void call_git_branch_command();
    void perform_custom_command();
    void expand_tree_items();
    void collapse_tree_items();
    void addGitSourceFolder();
    void gitview_about();
    void deleteFileOrFolder();
    void selectTextBrowserLanguage();
    void on_btnFindNext_clicked();
    void on_btnFindPrevious_clicked();

public Q_SLOTS:
    void initCustomAction(QAction* fAction);

private:

    struct Work { enum e
    {
        None,
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
        // NOTE: further git items are added here
        // show out of sync...
        Last
    }; };

    enum class Message
    {
        UpdateBytes = static_cast<int>(Work::Last),
        InsertTreeItem
    };

    struct Column { enum e
    {
        FileName,
        DateTime,
        State,
        Size
    }; };
    struct Role { enum e
    {
        isDirectory = Qt::UserRole,
        DateTime,
        Filter,
        GitFolder
    }; };

    enum class ComboShowItems
    {
        AllFiles,
        AllGitActions,
        GitModified,
        GitAdded,
        GitDeleted,
        GitUnknown,
        Gitstaged,
        GitUnmerged
    };

    void     keyPressEvent(QKeyEvent *);

    QString  getConfigName() const;

    quint64  insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem=0);
    bool     iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir=0, QTreeWidgetItem* aParentItem=0);
    bool     iterateCheckItems(QTreeWidgetItem* aParentItem, git::stringt2typemap& aPathMap, const QString* aSourceDir=0);
    void     insertSourceTree(const QDir& fSource, int fItem);
    quint64  sizeOfCheckedItems(QTreeWidgetItem* aParentItem);

    void     selectSourceFolder();
    QDir     initDir(const QString& aDirPath, int aFilter=0);

    void     apendTextToBrowser(const QString& aText, bool append=false);

    void     handleWorker(int);
    void     handleMessage(int, QVariant);

    QString  getItemFilePath(QTreeWidgetItem* item);
    QString  getItemTopDirPath(QTreeWidgetItem* item);
    void     updateTreeItemStatus(QTreeWidgetItem * aItem);
    void     getSelectedTreeItem();

    void     initContextMenuActions();

    void     applyGitCommandToFileTree(const QString& aCommand);
    QString  applyGitCommandToFilePath(const QString& fSource, const QString& fGitCmd, QString& aResultStr);

    void     parseGitStatus(const QString& fSource, const QString& aStatus, git::stringt2typemap& aFiles);
    QTreeWidget* focusedTreeWidget(bool aAlsoSource=true);

    void     find_function(bool forward);

#ifdef DOCKED_VIEWS
    void     createDockWindows();
    void     showDockedWidget(QWidget* widget);
    bool     mDockedWidgetMinMaxButtons;
#endif

    Ui::MainWindow*       ui;
    QString               mGitCommand;
    WorkerThreadConnector mWorker;
    Work::e               mCurrentTask;
    ActionList            mActions;
    QString               mConfigFileName;
    GitIgnore             mGitIgnore;
    QTreeWidgetItem*      mContextMenuSourceTreeItem;
    QSharedPointer<Highlighter> mHighlighter;
    QString               mFontName;
};

#endif // MAINWINDOW_H
