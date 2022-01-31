#include "qsourcetreewidget.h"
#include "logger.h"
#include "helper.h"
#include "git_type.h"

#include <QDropEvent>
#include <QModelIndex>
#include <QDirIterator>
#include <QDateTime>
#include <vector>


using namespace std;
using namespace git;

QSourceTreeWidget::QSourceTreeWidget(QWidget *parent) : QTreeWidget(parent)
  , mUseSourceTreeCheckboxes(false)
{

}


void QSourceTreeWidget::dropEvent(QDropEvent *event)
{
    QModelIndex droppedIndex = indexAt( event->pos() );

    if(droppedIndex.isValid())
    {
        bool target_was_dropped = false;
        Q_EMIT dropped_to_target(itemFromIndex(droppedIndex), &target_was_dropped);
        if (target_was_dropped)
        {
            QTreeWidget::dropEvent(event);
        }
    }
}


QTreeWidgetItem * QSourceTreeWidget::itemFromIndex(const QModelIndex &index) const
{
    return QTreeWidget::itemFromIndex(index);
}

quint64 QSourceTreeWidget::insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem)
{
    QDirIterator fIterator(aParentDir, QDirIterator::NoIteratorFlags);

    vector<int> fMapLevels;

    bool fTopLevelItem(false);
    if (!aParentItem)
    {
        QStringList fStrings;
        fStrings.append(aParentDir.absolutePath());
        aParentItem = new QTreeWidgetItem(fStrings);
        if (mUseSourceTreeCheckboxes)
        {
            aParentItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|aParentItem->flags());
            aParentItem->setCheckState(Column::FileName, Qt::Checked);
        }
        aParentItem->setData(Column::FileName, Role::isDirectory, QVariant(true));
        aParentItem->setData(Column::FileName, Role::Filter, QVariant(INT(aParentDir.filter())));

        aTree.addTopLevelItem(aParentItem);
        QDir fParent = aParentDir;
        while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
        {
            fParent.cdUp();
            mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);
            if (fParent.isRoot()) break;
        };
#if RELATIVE_GIT_PATH ==1
        if (!fParent.isRoot())
        {
            aParentItem->setData(Column::FileName, Role::GitFolder, QVariant(fParent.absolutePath()));
        }
#endif
        fTopLevelItem = true;
    }

    mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();

        if (mGitIgnore.ignoreFile(fFileInfo)) continue;

        QStringList fColumns;
        fColumns.append(fFileInfo.fileName());
        fColumns.append(fFileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
        fColumns.append(formatFileSize(fFileInfo.size()));
        fColumns.append("");

        QTreeWidgetItem* fItem = new QTreeWidgetItem(aParentItem, fColumns);
        if (mUseSourceTreeCheckboxes)
        {
            fItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsAutoTristate|fItem->flags());
            fItem->setCheckState(Column::FileName, Qt::Checked);
        }
        fItem->setData(Column::FileName, Role::isDirectory, QVariant(fFileInfo.isDir()));
        fItem->setData(Column::DateTime, Role::DateTime, QVariant(fFileInfo.lastModified()));

        Type fType;
        fType.translate(fFileInfo);

        fItem->setData(Column::State, Role::Filter, QVariant(fType.type()));

        if (fFileInfo.isDir())
        {
            QDir fSubDir(fFileInfo.absoluteFilePath());
            fSubDir.setFilter(aParentDir.filter());
            quint64 fSizeOfSubFolderFiles = insertItem(fSubDir, aTree, fItem);
            fItem->setText(Column::Size, formatFileSize(fSizeOfSubFolderFiles));
            fSizeOfFiles += fSizeOfSubFolderFiles;
            fItem->setData(Column::Size, Qt::SizeHintRole, QVariant(fSizeOfSubFolderFiles));
        }
        else
        {
            fSizeOfFiles += fFileInfo.size();
            fItem->setText(Column::Size, formatFileSize(fFileInfo.size()));
            fItem->setData(Column::Size, Qt::SizeHintRole, QVariant(fFileInfo.size()));
        }
    }
    while (fIterator.hasNext());


    for (const auto& fMapLevel : fMapLevels)
    {
        mGitIgnore.removeIgnoreMapLevel(fMapLevel);
    }

    if (fTopLevelItem)
    {
        aParentItem->setText(Column::Size, formatFileSize(fSizeOfFiles));
    }
    return fSizeOfFiles;
}


bool QSourceTreeWidget::iterateCheckItems(QTreeWidgetItem* aParentItem, stringt2typemap& aPathMap, const QString* aSourceDir)
{
    if (aParentItem)
    {
        QString item_text = aParentItem->text(Column::FileName);
        auto pos = item_text.lastIndexOf('/');
        if (pos != -1 && aSourceDir != nullptr)
        {
            item_text = item_text.mid(pos+1);
        }
        const QString fSourcePath = aSourceDir ? *aSourceDir + "/" + item_text : item_text;

        const auto fFoundType = aPathMap.find(fSourcePath.toStdString());
        if (fFoundType != aPathMap.end())
        {
            if (mUseSourceTreeCheckboxes)
            {
                aParentItem->setCheckState(Column::FileName, Qt::Checked);
            }
            const QString fState = fFoundType->second.getStates();
            aParentItem->setText(Column::State, fState);
            Type fType(aParentItem->data(Column::State, Role::Filter).toUInt());
            fType.remove(Type::AllGitActions);
            fType.add(static_cast<Type::TypeFlags>(fFoundType->second.type()));
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.type()));
            TRACE(Logger::info, "set state %s, %x of %s", fState.toStdString().c_str(), fFoundType->second.type(), fSourcePath.toStdString().c_str());
        }
        else
        {
            Type fType(aParentItem->data(Column::State, Role::Filter).toUInt());
            fType.remove(Type::AllGitActions);
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.type()));
            aParentItem->setText(Column::State, "");
        }
        for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
        {
            iterateCheckItems(aParentItem->child(fChild), aPathMap, &fSourcePath);
        }
    }
    return false;
}


quint64 QSourceTreeWidget::sizeOfCheckedItems(QTreeWidgetItem* aParentItem)
{
    quint64 fSize = 0;
    if (aParentItem)
    {
        bool fIterate = false;
        if (mUseSourceTreeCheckboxes)
        {
            switch (aParentItem->checkState(Column::FileName))
            {
                case Qt::Unchecked: return fSize;
                case Qt::PartiallyChecked:
                    fIterate = true;
                    break;
                case  Qt::Checked:
                    if (!aParentItem->data(Column::FileName, Role::isDirectory).toBool())
                    {
                        fSize = aParentItem->data(Column::Size, Qt::SizeHintRole).toLongLong();
                    }
                    fIterate = true;
                    break;
            }
        }
        else
        {
            fIterate = true;
        }
        if (fIterate)
        {
            for (int fChild = 0; fChild < aParentItem->childCount(); ++fChild)
            {
                fSize += sizeOfCheckedItems(aParentItem->child(fChild));
            }
        }
    }
    return fSize;
}

QString QSourceTreeWidget::getItemFilePath(QTreeWidgetItem* aTreeItem)
{
    QString file_name;
    if (aTreeItem)
    {
        file_name = aTreeItem->text(Column::FileName);
        bool is_folder = aTreeItem->data(Column::FileName, Role::GitFolder).isValid();
        bool current_dir_set = false;
        while (aTreeItem)
        {
            aTreeItem = aTreeItem->parent();
            if (aTreeItem)
            {
                if (aTreeItem->data(Column::FileName, Role::GitFolder).isValid())
                {
                    QDir::setCurrent(aTreeItem->text(Column::FileName));
                    current_dir_set = true;
                }
                else
                {
                    file_name = aTreeItem->text(Column::FileName) + QDir::separator() + file_name;
                }
            }
        }
        if (!current_dir_set && is_folder)
        {
            QDir::setCurrent(file_name);
        }
    }
    return file_name;
}

QString QSourceTreeWidget::getItemTopDirPath(QTreeWidgetItem* aItem)
{
    aItem = getTopLevelItem(*this, aItem);
    if (aItem)
    {
        return aItem->text(Column::FileName);
    }
    else if (topLevelItemCount())
    {
        return topLevelItem(0)->text(Column::FileName);
    }
    return "";
}

void QSourceTreeWidget::find_item(const QString& git_root, const QString& filepath)
{
    selectionModel()->clearSelection();
    QStringList items = filepath.split("/");
    auto list = findItems(git_root, Qt::MatchExactly);
    if (!list.empty() )
    {
        auto* found = list[0];
        for (const QString& item : items)
        {
            int32_t i;
            if (found != nullptr)
            {
                const auto children = found->childCount();
                for (i=0; found != nullptr && i<children; ++i)
                {
                    if (found->child(i)->text(0) == item)
                    {
                        found = found->child(i);
                        break;
                    }
                }
                if (i == children) found = nullptr;
            }
        }
        if (found)
        {
            found->setSelected(true);
            auto* parent = found->parent();
            while (parent)
            {
                setItemExpanded(parent, true);
                parent = parent->parent();
            }
            scrollToItem(found);
        }
    }
}

