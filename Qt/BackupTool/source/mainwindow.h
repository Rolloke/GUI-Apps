#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "workerthreadconnector.h"

#include <QMainWindow>
#include <QDir>
#include <QDirIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSemaphore>
#include <QTime>
#include <map>


namespace Ui {
class MainWindow;
}

typedef std::map<std::string, bool> stringtoboolmap;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& aConfigName, QWidget *parent = 0);
    ~MainWindow();
    void doAutoBackup();

Q_SIGNALS:
    void doWork(int );

private Q_SLOTS:
    void on_btnRestore_clicked();
    void on_btnCancel_clicked();
    void on_btnBackup_clicked();
    void on_btnAddSourceFolder_clicked();
    void on_btnSelectDestination_clicked();
    void on_btnUpdateSearch_clicked();
    void on_editBlacklist_textEdited(const QString &arg1);

//    void on_treeSource_itemClicked(QTreeWidgetItem *item, int column);

//    void on_treeSource_clicked(const QModelIndex &index);

private:

    enum class Work
    {
        None,
        Backup,
        Restore,
        Last
    };

    enum class Message
    {
        UpdateBytes = static_cast<Message>(Work::Last),
        InsertTreeItem
    };

    enum class Column
    {
        FileName,
        DateTime,
        Size
    };
    enum class Role
    {
        isDirectory = Qt::UserRole,
        DateTime,
        Filter
    };

    void keyPressEvent(QKeyEvent *);

    QString getConfigName() const;

    quint64 insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem=0);
    bool copyItems(const QTreeWidget& aSourceTree, const QDir& aDestination, const QString* aSourceDir=0, QTreeWidgetItem* aParentItem=0);
    bool iterateCheckItems(QTreeWidgetItem* aParentItem, std::map<std::string, bool>& aPathMap, bool aSet = false, const QString* aSourceDir=0);
    quint64 sizeOfCheckedItems(QTreeWidgetItem* aParentItem);

    void selectSourceFolder();
    void selectDestination();
    QDir initDir(const QString& aDirPath, int aFilter=0);
    bool copy_file(const QString& fSource, const QString& fDestination);

    void updateControls();

    void handleWorker(int);
    void handleMessage(int, QVariant);
    void messageBackup(bool);
    void messageRestore(bool);
    void messageUpdateBytes(qlonglong);
    void loggingFuncton(const std::string&);

    Ui::MainWindow *ui;
    QString     mDestination;
    QStringList mBlackList;
    Work        mCurrentTask;
    qlonglong   mBytesCopied;
    qlonglong   mBytesToCopy;
    WorkerThreadConnector mWorker;
    QString     mConfigFileName;
    QTime       mStartTime;
    QString     mCopyCommand;
    bool        mCloseAfterWork;
    QFile       mLogFile;
};

#endif // MAINWINDOW_H
