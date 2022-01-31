#ifndef QSOURCETREEWIDGET_H
#define QSOURCETREEWIDGET_H

#include <QTreeWidget>

#include "gitignore.h"

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

    quint64 insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem);
    bool     iterateCheckItems(QTreeWidgetItem* aParentItem, git::stringt2typemap& aPathMap, const QString* aSourceDir=0);
    quint64  sizeOfCheckedItems(QTreeWidgetItem* aParentItem);

    QString  getItemFilePath(QTreeWidgetItem* item);
    QString  getItemTopDirPath(QTreeWidgetItem* item);

public Q_SLOTS:
    void find_item(const QString& git_root, const QString& filepath);

signals:
    void dropped_to_target(QTreeWidgetItem* target, bool* rvalue);

protected:
    void dropEvent(QDropEvent *event) override;

private:
    QTreeWidgetItem * itemFromIndex(const QModelIndex &index) const;

public:
    bool mUseSourceTreeCheckboxes;
    GitIgnore             mGitIgnore;

private:

};

#endif // QSOURCETREEWIDGET_H
