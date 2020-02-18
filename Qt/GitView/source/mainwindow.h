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
    void on_btnUpdateSearch_clicked();
    void on_editBlacklist_textEdited(const QString &arg1);
    void on_treeSource_itemClicked(QTreeWidgetItem *item, int column);
    void on_treeSource_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_ckHideParent_clicked(bool checked);
    void on_btnStoreText_clicked();
    void textBrowserChanged();
    void on_treeSource_customContextMenuRequested(const QPoint &pos);

    void on_git_commit();
    void on_remove_from_git();
    void on_show_difference();
    void on_show_status();
    void on_show_short_status();
    void on_custom_command();

    void on_btnCloseText_clicked();

    void on_comboShowItems_currentIndexChanged(int index);

private:

    enum class Work
    {
        None,
        ApplyGitCommand,
        ShowAllFiles,
        ShowAllGitActions,
        ShowModified,
        ShowDeleted,
        ShowAdded,
        ShowUnknown,
        // todo: more git items
        // out of sync...
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

    void keyPressEvent(QKeyEvent *);

    QString getConfigName() const;

    quint64 insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem=0);
    bool iterateTreeItems(const QTreeWidget& aSourceTree, const QString* aSourceDir=0, QTreeWidgetItem* aParentItem=0);
    bool iterateCheckItems(QTreeWidgetItem* aParentItem, git::stringt2typemap& aPathMap, bool aSet = false, const QString* aSourceDir=0);
    quint64 sizeOfCheckedItems(QTreeWidgetItem* aParentItem);

    void selectSourceFolder();
    QDir initDir(const QString& aDirPath, int aFilter=0);
    bool applyGitCmd(const QString& fSource, const QString& fGitCmd, QString& aResultStr);
    void parseGitStatus(const QString& fSource, const QString& aStatus, git::stringt2typemap& aFiles);

    void updateControls();

    void handleWorker(int);
    void handleMessage(int, QVariant);
    void messageBackup(bool);
    void messageUpdateBytes(qlonglong);

    void addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels);
    void removeIgnoreMapLevel(int aLevel);
    void insertSourceTree(const QDir& fSource, int fItem);
    QString getItemFilePath(QTreeWidgetItem* item);

    void     initContextMenuActions();
    QAction* createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aCommand="", const QString& aToolTip="");
    void     performGitCmd(const QString& aCommand);
    QAction* getAction(git::Cmd::eCmd aCmd);

    Ui::MainWindow *ui;
    QString     mDestination;
    QString     mGitCommand;
    QStringList mBlackList;
    Work        mCurrentTask;
    qlonglong   mBytesCopied;
    qlonglong   mBytesToCopy;
    WorkerThreadConnector mWorker;
    QString     mConfigFileName;
    QTime       mStartTime;
    git::stringt2typemap mIgnoreMap;
    QTreeWidgetItem* mContextMenuItem;
    boost::container::flat_map<int, QAction*> mActionList;
};

#endif // MAINWINDOW_H
