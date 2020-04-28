#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "workerthreadconnector.h"
#include "git_type.h"
#include "actions.h"

#include <QMainWindow>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSemaphore>
#include <QTime>
#include <map>
#include <boost/optional.hpp>


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& aConfigName, QWidget *parent = 0);
    ~MainWindow();

Q_SIGNALS:
    void doWork(int );

private Q_SLOTS:
    void cancelCurrentWorkTask();
    void addGitSourceFolder();
    void updateGitStatus();
    void textBrowserChanged();

    void on_btnStoreText_clicked();
    void on_btnCloseText_clicked();

    void on_treeSource_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_customContextMenuRequested(const QPoint &pos);

    void on_ckHideEmptyParent_clicked(bool checked);
    void on_comboShowItems_currentIndexChanged(int index);

    void on_treeHistory_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeHistory_customContextMenuRequested(const QPoint &pos);

    void showOrHideHistory(bool checked);
    void clearHistoryTree();
    void performCustomGitActionSettings();

    void call_git_commit();
    void call_git_move_rename();
    void call_git_history_diff_command();
    void perform_custom_command();
    void expand_tree_items();
    void collapse_tree_items();


private:

    enum class Work
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
        // TODO: more git items
        // show out of sync...
        Last
    };

    enum class Message
    {
        UpdateBytes = static_cast<int>(Work::Last),
        InsertTreeItem
    };

    enum class Column
    {
        FileName,
        DateTime,
        State,
        Size
    };
    enum class Role
    {
        isDirectory = Qt::UserRole,
        DateTime,
        Filter,
        GitFolder
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

    const QString& getLineFeed() const;
    void           setLineFeed(const QString& aLF=mNativeLineFeed);

    void     handleWorker(int);
    void     handleMessage(int, QVariant);

    void     addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels);
    void     removeIgnoreMapLevel(int aLevel);
    bool     ignoreFile(const QFileInfo& aFileInfo);
    QString  getItemFilePath(QTreeWidgetItem* item);
    void     updateTreeItemStatus(QTreeWidgetItem * aItem);
    void     getSelectedTreeItem();

    void     initContextMenuActions();

    void     performGitCmd(const QString& aCommand);
    QString  applyGitCmd(const QString& fSource, const QString& fGitCmd, QString& aResultStr);

    void     parseGitStatus(const QString& fSource, const QString& aStatus, git::stringt2typemap& aFiles);
    void     parseGitLogHistoryText();

    Ui::MainWindow*       ui;
    QString               mGitCommand;
    WorkerThreadConnector mWorker;
    Work                  mCurrentTask;
    ActionList            mActions;
    QString               mConfigFileName;
    git::stringt2typemap  mIgnoreMap;
    QTreeWidgetItem*      mContextMenuItem;
    QString               mHistoryHashItems;
    QString               mLineFeed;
    boost::optional<git::stringt2typemap::const_reference> mIgnoreContainingNegation;
    static const QString  mNativeLineFeed;
};

#endif // MAINWINDOW_H
