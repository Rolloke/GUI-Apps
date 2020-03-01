#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "workerthreadconnector.h"
#include "git_type.h"

#include <QMainWindow>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSemaphore>
#include <QTime>
#include <map>


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
    void on_btnCancel_clicked();
    void on_btnAddSourceFolder_clicked();
    void on_btnUpdateStatus_clicked();

    void on_btnStoreText_clicked();
    void textBrowserChanged();
    void on_btnCloseText_clicked();

    void on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_customContextMenuRequested(const QPoint &pos);
    void on_ckHideEmptyParent_clicked(bool checked);
    void on_comboShowItems_currentIndexChanged(int index);

    void on_btnHideHistory_clicked(bool checked);
    void on_treeHistory_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeHistory_customContextMenuRequested(const QPoint &pos);
    void on_btnClearHistory_clicked();

    void on_git_commit();
    void on_git_move_rename();
    void on_git_history_diff_command();
    void on_custom_command();



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
        // TODO: more git items
        // show out of sync...
        Last
    };

    enum class Message
    {
        UpdateBytes = Work::Last,
        InsertTreeItem
    };

    enum class Column
    {
        FileName,
        DateTime,
        Size,
        State
    };
    enum class Role
    {
        isDirectory = Qt::UserRole,
        DateTime,
        Filter
    };

    enum class ComboShowItems
    {
        AllFiles,
        AllGitActions,
        GitModified,
        GitAdded,
        GitDeleted,
        GitUnknown
    };
#if MAP_TYPE == 0
    typedef boost::container::flat_map<int, QAction*> tActionMap;
#elif MAP_TYPE == 1
    typedef std::map<int, QAction*> tActionMap;
#endif

    void keyPressEvent(QKeyEvent *);

    QString getConfigName() const;

    quint64 insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem=0);
    bool iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir=0, QTreeWidgetItem* aParentItem=0);
    bool iterateCheckItems(QTreeWidgetItem* aParentItem, git::stringt2typemap& aPathMap, const QString* aSourceDir=0);
    void insertSourceTree(const QDir& fSource, int fItem);
    quint64 sizeOfCheckedItems(QTreeWidgetItem* aParentItem);

    void selectSourceFolder();
    QDir initDir(const QString& aDirPath, int aFilter=0);

    void handleWorker(int);
    void handleMessage(int, QVariant);

    void addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels);
    void removeIgnoreMapLevel(int aLevel);
    QString getItemFilePath(QTreeWidgetItem* item);
    void updateTreeItemStatus(QTreeWidgetItem * aItem);

    QAction* createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aCommand="");
    void     initContextMenuActions();
    QAction* getAction(git::Cmd::eCmd aCmd);
    void     setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText);
    void     setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction);

    void     performGitCmd(const QString& aCommand);
    bool     applyGitCmd(const QString& fSource, const QString& fGitCmd, QString& aResultStr);

    void     parseGitStatus(const QString& fSource, const QString& aStatus, git::stringt2typemap& aFiles);
    void     parseGitLogHistoryText();

    Ui::MainWindow*     ui;
    QString             mGitCommand;
    WorkerThreadConnector mWorker;
    Work                mCurrentTask;
    QString             mConfigFileName;
    git::stringt2typemap mIgnoreMap;
    QTreeWidgetItem*    mContextMenuItem;
    tActionMap          mActionList;
    QString             mHistoryHashItems;

};

#endif // MAINWINDOW_H
