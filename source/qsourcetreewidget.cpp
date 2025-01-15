#include "qsourcetreewidget.h"
#include "logger.h"
#include "helper.h"
#include "git_type.h"
#include "thread_helper.h"

#include <QDropEvent>
#include <QModelIndex>
#include <QDirIterator>
#include <QDateTime>
#include <QMenu>
#include <vector>
#include <set>


using namespace std;
using namespace git;

QSourceTreeWidget::QSourceTreeWidget(QWidget *parent) : QTreeWidget(parent)
  , mUseSourceTreeCheckboxes(false)
  , m1stCompareItem(nullptr)
{
    viewport()->setMouseTracking(true);
    viewport()->installEventFilter(this);
}


void QSourceTreeWidget::dropEvent(QDropEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QModelIndex droppedIndex = indexAt( event->position().toPoint() );
#else
    QModelIndex droppedIndex = indexAt( event->pos() );
#endif

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


/// \brief QSourceTreeWidget::insertItem
/// \param aParentDir parent folder for insertion of sub files or folders to tree
/// \param aTree the tree widget to insert to
/// \param aParentItem the parent tree item to insert to
/// \param do_not_ignore text for special behaviour
/// \note behaviour description
///       - file or folder name to be inserted without git ignore rules
///       - txt::no_double_entries to iterate through all files not inserted yet ignoring double entries
/// \return total size of containing files
quint64 QSourceTreeWidget::insertItem(const QDir& aParentDir, QTreeWidget& aTree, QTreeWidgetItem* aParentItem, QString do_not_ignore)
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
        bool git_folder = true;
        while (!QDir(fParent.absolutePath() + QDir::separator() + Folder::GitRepository).exists())
        {
            fParent.cdUp();
            git_folder = false;
            mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);
            if (fParent.isRoot()) break;
        };
        Type fType;
        QFileInfo file_info (fParent.absolutePath());
        fType.translate(file_info);
        if (git_folder)
        {
            fType.add(Type::GitFolder);
            fType.add(Type::Repository);
        }
        aParentItem->setData(Column::State, Role::Filter, QVariant(fType.type()));

#if RELATIVE_GIT_PATH ==1
        if (!fParent.isRoot())
        {
            aParentItem->setData(Column::FileName, Role::GitFolder, QVariant(fParent.absolutePath()));
        }
#endif
        fTopLevelItem = true;
    }

    mGitIgnore.addGitIgnoreToIgnoreMapLevel(aParentDir, fMapLevels);
    std::set<QString> ignored_folders;

    quint64 fSizeOfFiles = 0;
    do
    {
        fIterator.next();

        const QFileInfo& fFileInfo = fIterator.fileInfo();

        if (do_not_ignore.size())
        {
            if (do_not_ignore == txt::no_double_entries)
            {
                if (is_any_equal_to(fFileInfo.fileName(), Folder::FolderSelf, Folder::FolderUp))
                {
                    continue;
                }
                if (find_child_item(aParentItem, QSourceTreeWidget::Column::FileName, fFileInfo.fileName()))
                {
                    continue;
                }
            }
            else if (do_not_ignore != fFileInfo.fileName())
            {
                continue;
            }
        }
        else if (mGitIgnore.ignoreFile(fFileInfo))
        {
            if (fFileInfo.isDir())
            {
                ignored_folders.insert(fFileInfo.fileName());
            }
            continue;
        }

        QStringList fColumns;
        fColumns.append(fFileInfo.fileName());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#else
        fColumns.append(fFileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
#endif
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
        if (do_not_ignore.size())
        {
            fType.add(Type::GitIgnore);
        }

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

    if (fMapLevels.size())
    {
        GitIgnore *ignored = nullptr;
        if (do_not_ignore.size())
        {
            auto found_ignore = mIgnoredInFolder.find(aParentDir.absolutePath());
            if (found_ignore != mIgnoredInFolder.end())
            {
                found_ignore.value().remove_entry(do_not_ignore);
            }
        }
        else
        {
            auto found_ignore = mIgnoredInFolder.find(aParentDir.absolutePath());
            if (found_ignore != mIgnoredInFolder.end())
            {
                found_ignore.value().clear();
            }
            else
            {
                GitIgnore newignore;
                found_ignore = mIgnoredInFolder.insert(aParentDir.absolutePath(), newignore);
            }
            ignored = &found_ignore.value();
        }
        for (const auto& fMapLevel : std::as_const(fMapLevels))
        {
            mGitIgnore.removeIgnoreMapLevel(fMapLevel, ignored);
        }
        for (const auto&folder : std::as_const(ignored_folders))
        {
            ignored->add_folder(folder);
        }
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

        const auto fFoundType = aPathMap.find(fSourcePath);
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
            fType.add(Type::type(fFoundType->second.type()));
            aParentItem->setData(Column::State, Role::Filter, QVariant(fType.type()));
            TRACEX(Logger::info, "set state " << fState << ", " << fFoundType->second.type() << " of " << fSourcePath);
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
    const QStringList items = filepath.split("/");
    auto list = findItems(git_root, Qt::MatchExactly);
    if (!list.empty() )
    {
        auto* found = list[0];
        for (const QString& item : std::as_const(items))
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
                parent->setExpanded(true);
#else
                setItemExpanded(parent, true);
#endif
                parent = parent->parent();
            }
            scrollToItem(found);
        }
    }
}

void QSourceTreeWidget::start_compare_to()
{
    mCompareTo.setType(0);
    auto seleted = selectedItems();
    if (seleted.count() == 1)
    {
        m1stCompareItem = seleted[0];
        const Type type(Type::type(m1stCompareItem->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
        if      (type.is(Type::File))   mCompareTo.setType(Type::File);
        else if (type.is(Type::Folder)) mCompareTo.setType(Type::Folder);
        else m1stCompareItem = nullptr;
    }
}

void QSourceTreeWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (mCompareTo.type())
    {
        Qt::CursorShape cursor_shape { Qt::ForbiddenCursor };
        QTreeWidgetItem * item = itemAt(event->pos());
        if (item && item != m1stCompareItem)
        {
            const Type type(Type::type(item->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
            if (type.is(Type::type(mCompareTo.type())))
            {
                cursor_shape = Qt::BusyCursor;
            }
        }
        setCursor(QCursor(cursor_shape));
    }
    else
    {
        QTreeView::mouseMoveEvent(event);
    }
}

void QSourceTreeWidget::mousePressEvent(QMouseEvent * event)
{
    QTreeView::mousePressEvent(event);
    if (mCompareTo.type())
    {
        unsetCursor();
        auto seleted = selectedItems();
        if (seleted.count() == 1 && seleted[0] != m1stCompareItem)
        {
            const Type type(Type::type(seleted[0]->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
            if (type.is(Type::type(mCompareTo.type())))
            {
                QString compare_item1 = getItemFilePath(m1stCompareItem);
                QString compare_item2 = getItemFilePath(seleted[0]);
                Q_EMIT compare_items(compare_item1, compare_item2);
            }
        }
        mCompareTo.setType(0);
        m1stCompareItem = nullptr;
    }
}

void QSourceTreeWidget::fillContextMenue(QMenu &menu, QTreeWidgetItem *item)
{
    if (item)
    {
        const Type type(Type::type(item->data(QSourceTreeWidget::Column::State, QSourceTreeWidget::Role::Filter).toUInt()));
        if (type.is(Type::Folder))
        {
            QString path = getItemFilePath(item);
            const auto found_ignored = mIgnoredInFolder.find(path);
            if (found_ignored != mIgnoredInFolder.end())
            {
                QMenu*submenu = nullptr;
                const auto& themap = found_ignored->getIgnoreMap();
                for (size_t index=0; index<themap.size(); ++index )
                {
                    if (std::get<_type>(themap[index]).is(Type::GitFolder))           continue;
                    if (std::get<_type>(themap[index]).is(Type::FolderForNavigation)) continue;
                    QDir dir_check(path + "/" + std::get<_string>(themap[index]));
                    if (dir_check.exists())
                    {
                        if (!submenu)
                        {
                            submenu = menu.addMenu(tr("Insert Ignored Folder"));
                        }
                        QString do_not_ignore = std::get<_string>(themap[index]);
                        QAction *action = submenu->addAction(do_not_ignore);
                        connect(action, &QAction::triggered, [this, path, item, do_not_ignore]( )
                        { this->insertItem(path, *this, item, do_not_ignore); });
                    }
                }
            }
            {
                QDirIterator iterator(path, QDirIterator::NoIteratorFlags);
                QMenu*submenu = nullptr;
                do
                {
                    iterator.next();
                    const QFileInfo& file_info = iterator.fileInfo();
                    if (file_info.isDir()) continue;

                    bool found = false;
                    for (int i=0; i<item->childCount(); ++i)
                    {
                        if (item->child(i)->text(QSourceTreeWidget::Column::FileName) == file_info.fileName())
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        if (!submenu)
                        {
                            submenu = menu.addMenu(tr("Insert Ignored File"));
                        }
                        QString file = file_info.fileName();
                        QAction *action = submenu->addAction(file);
                        connect(action, &QAction::triggered, [this, path, item, file]( )
                        { this->insertItem(path, *this, item, file); });
                    }
                }
                while (iterator.hasNext());
            }
        }
    }
}
