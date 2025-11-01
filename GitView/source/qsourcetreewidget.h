#ifndef QSOURCETREEWIDGET_H
#define QSOURCETREEWIDGET_H

#include <QTreeWidget>
#include "gitignore.h"
#include <QString>
class QDir;

class QSourceTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:

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

    explicit QSourceTreeWidget(QWidget *parent = nullptr);

    quint64  insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem, QString do_not_ignore = {});
    bool     iterateCheckItems(QTreeWidgetItem* aParentItem, git::stringt2typemap& aPathMap, const QString* aSourceDir=0);
    quint64  sizeOfCheckedItems(QTreeWidgetItem* aParentItem);

    QString  getItemFilePath(QTreeWidgetItem* item);
    QString  getItemTopDirPath(QTreeWidgetItem* item);
    void     fillContextMenue(QMenu&, QTreeWidgetItem *item);

    QStringList saveExpandedState() const;
    void restoreExpandedState(const QStringList& list);


public Q_SLOTS:
    void find_item(const QString& git_root, const QString& filepath);
    void start_compare_to();

signals:
    void dropped_to_target(QTreeWidgetItem* target, bool* rvalue);
    void compare_items(QString &item1, QString& item2);

protected:
    void dropEvent(QDropEvent *event) override;

private:
    QTreeWidgetItem * itemFromIndex(const QModelIndex &index) const;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public:
    bool                     mUseSourceTreeCheckboxes;
    GitIgnore                mGitIgnore;
    QMap<QString, GitIgnore> mIgnoredInFolder;
    git::Type                mCompareTo;
    QTreeWidgetItem*         m1stCompareItem;
    bool                     mInseredIgnoredFiles;

private:

};

#endif // QSOURCETREEWIDGET_H
