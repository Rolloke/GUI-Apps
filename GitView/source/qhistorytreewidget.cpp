#include "qhistorytreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include <QActionGroup>
#include <QPainter>
#include <QPainterPath>
#include <iostream>
#include "actions.h"
#include "history.h"
#include "git_type.h"
#include "logger.h"

using namespace git;

QHistoryTreeWidget::QHistoryTreeWidget(QWidget *parent): QTreeWidget(parent)
{
    setVisible(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void QHistoryTreeWidget::clear()
{
    QTreeWidget::clear();
}

void QHistoryTreeWidget::initialize()
{
    if (!mInitialized)
    {
        header()->setSectionResizeMode(History::Column::CommitDate , QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(History::Column::CommitHash , QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(History::Column::CommitGraph, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(History::Column::Author     , QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(History::Column::Description, QHeaderView::Stretch);
        header()->setStretchLastSection(false);

        m_item_delegate = new QDrawGraphItemDelegate(this);
        setItemDelegate(m_item_delegate);

        mInitialized = true;
    }
}

void QHistoryTreeWidget::parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType)
{
    initialize();
    QVector<QStringList> fList;
    History::parse(fText, fList);

    HistoryEntries entries(fList);

    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(aFileName));
    addTopLevelItem(fNewHistoryItem);

    fNewHistoryItem->setData(History::Column::Commit, History::Role::ContextMenuItem, aData);
    QMap<QString, QVariant> fAuthors;

    setVisible(true);

    const int fTLI = topLevelItemCount()-1;
    int list_index = -1;
    for (auto& fItem : fList)
    {
        if (++list_index == 0) continue; // ignore first item
        if (fItem.count() >= History::Entry::NoOfEntries)
        {
            QTreeWidgetItem* fNewHistoryLogItem = new QTreeWidgetItem();
            topLevelItem(fTLI)->addChild(fNewHistoryLogItem);
            fNewHistoryLogItem->setText(History::Column::CommitDate , fItem[History::Entry::CommitterDate]);
            fNewHistoryLogItem->setText(History::Column::Description, fItem[History::Entry::SubjectAndBody].split("\n")[0]);
            fNewHistoryLogItem->setText(History::Column::CommitHash , fItem[History::Entry::CommitHash]);
            fNewHistoryLogItem->setText(History::Column::Author     , fItem[History::Entry::Author]);
            fAuthors[fItem[History::Entry::Author]] = true;
            for (int fRole=0; fRole < History::Entry::NoOfEntries; ++fRole)
            {
                fNewHistoryLogItem->setData(History::Column::Commit, History::role(static_cast<History::Entry::e>(fRole)), QVariant(fItem[fRole]));
            }
            fNewHistoryLogItem->setData(History::Column::Commit, History::role(History::Entry::Type), QVariant(aType));

            auto draw_items = entries.get_connection_part(list_index);
            if (draw_items.size())
            {
                fNewHistoryLogItem->setData(History::Column::CommitGraph, History::role(History::Entry::DrawItems), QVariant(draw_items));
            }

        }
    }

    fNewHistoryItem->setData(History::Column::Commit, History::Role::VisibleAuthors, QVariant(fAuthors));
}

void QHistoryTreeWidget::checkAuthorsIndex(int aIndex, bool aChecked)
{
    auto fSelected = selectionModel()->selectedIndexes();
    if (aIndex >= 0 && fSelected.count())
    {
        QTreeWidgetItem* fItem = itemFromIndex(fSelected.first());
        if (fItem && getItemLevel(fItem) == Level::Top)
        {
            auto fItemData = fItem->data(History::Column::Commit, History::Role::VisibleAuthors);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if (fItemData.typeId() == QMetaType::QVariantMap)
#else
            if (fItemData.type() == QVariant::Map)
#endif
            {
                auto fMap = fItemData.toMap();
                if (aIndex < fMap.size())
                {
                    auto fMapItem = std::next(fMap.begin(), aIndex);
                    fMap[fMapItem.key()] = aChecked;
                }
                else
                {
                    bool fChecked =  aIndex == fMap.size();
                    for (auto fMapItem = fMap.begin(); fMapItem != fMap.end(); ++fMapItem)
                    {
                        fMap[fMapItem.key()] = fChecked;
                    }
                }
                fItem->setData(History::Column::Commit, History::Role::VisibleAuthors, QVariant(fMap));

                int fCount = fItem->childCount();
                for (int fChild=0; fChild<fCount; ++fChild)
                {
                    auto fChildItem = fItem->child(fChild);
                    auto fBoolItem = fMap.find(fChildItem->data(History::Column::Commit, History::role(History::Entry::Author)).toString());
                    if (fBoolItem != fMap.end())
                    {
                        fChildItem->setHidden(!fBoolItem.value().toBool());
                    }
                }
            }
        }
    }
}

QVariant QHistoryTreeWidget::determineHistoryHashItems(QTreeWidgetItem* fSelectedHistoryItem)
{
    QVariant fItemData;
    mHistoryHashItems.clear();
    mHistoryFile.clear();
    mHistoryDescription.clear();
    mSelectedTopLevelItemType = 0;
    if (fSelectedHistoryItem)
    {
        QModelIndexList fSelectedIndexes = selectionModel()->selectedIndexes();

        switch (static_cast<Level::e>(getItemLevel(fSelectedHistoryItem)))
        {
            case Level::Top:
            {
                fItemData = fSelectedHistoryItem->data(History::Column::Commit, History::Role::VisibleAuthors);
                mSelectedTopLevelItemType = fSelectedHistoryItem->data(History::Column::Commit, History::role(History::Entry::Type)).toUInt();
            }   break;
            case Level::Log:
            {
                QTreeWidgetItem* fParentHistoryItem = fSelectedHistoryItem->parent();
                if (fParentHistoryItem)
                {
                    fItemData = fParentHistoryItem->data(History::Column::Commit, History::Role::ContextMenuItem);
                    Type fType(fSelectedHistoryItem->data(History::Column::Commit, History::role(History::Entry::Type)).toUInt());
                    mSelectedTopLevelItemType = fType.type();
                    if (fType.is(Type::File))
                    {
                        mHistoryFile = fParentHistoryItem->data(History::Column::CommitDate, Qt::DisplayRole).toString();
                    }
                    std::sort(fSelectedIndexes.begin(), fSelectedIndexes.end());
                    for (auto fIndex = fSelectedIndexes.rbegin(); fIndex != fSelectedIndexes.rend(); ++fIndex)
                    {
                        QTreeWidgetItem* fItem = itemFromIndex(*fIndex);
                        if (fItem && fItem->parent() == fParentHistoryItem)
                        {
                            mHistoryHashItems.append(fItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
                            mHistoryHashItems.append(" ");
                            mHistoryDescription = fItem->data(History::Column::Commit, History::role(History::Entry::SubjectAndBody)).toString();
                        }
                    }
                }
            }   break;
            case Level::File:
            {
                QTreeWidgetItem* fTopLevelItem   = getTopLevelItem(*this, fSelectedHistoryItem);
                mHistoryFile                     = fSelectedHistoryItem->data(History::Column::CommitDate, Qt::DisplayRole).toString();
                fItemData                        = fTopLevelItem->data(History::Column::Commit, History::Role::ContextMenuItem);
                QTreeWidgetItem* fHistoryLogItem = fSelectedHistoryItem->parent();
                QTreeWidgetItem* fLogItemParent  = fHistoryLogItem->parent();
                mSelectedTopLevelItemType        = fHistoryLogItem->data(History::Column::Commit, History::role(History::Entry::Type)).toUInt();

                int fIndex = fLogItemParent->indexOfChild(fHistoryLogItem);
                auto fNextItem = fLogItemParent->child(fIndex+1);
                if (Type(mSelectedTopLevelItemType).is(Type::DiffOf2Commits))
                {
                    fNextItem = nullptr;
                }
                if (fNextItem)
                {
                    mHistoryHashItems.append(fNextItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
                    mHistoryHashItems.append(" ");
                    mHistoryHashItems.append(fHistoryLogItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
                    mHistoryDescription = fNextItem->data(History::Column::Commit, History::role(History::Entry::SubjectAndBody)).toString();
                }
                else
                {
                    mHistoryHashItems.append(fHistoryLogItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
                    mHistoryDescription = fHistoryLogItem->data(History::Column::Commit, History::role(History::Entry::SubjectAndBody)).toString();
                }
            }   break;
        }
    }
    return fItemData;
}

void QHistoryTreeWidget::customContextMenuRequested(const QPoint &pos, const ActionList& actions, QTreeWidgetItem**context_menu_source_treeItem)
{
    const QVariant fItemData = determineHistoryHashItems(itemAt(pos));

    QActionGroup fPostActionGroup(this);
    fPostActionGroup.setExclusive(false);
    if (fItemData.isValid())
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        switch (fItemData.typeId())
        {
            case QMetaType::QModelIndex:
            {
                QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(this);
                *context_menu_source_treeItem = fSourceHook->itemFromIndex(fItemData.toModelIndex());
            }   break;
            case QMetaType::QVariantMap:
            {
                auto fMap = fItemData.toMap();
                for (auto fMapItem = fMap.begin(); fMapItem != fMap.end(); ++fMapItem)
                {
                    QAction* fAction = fPostActionGroup.addAction(fMapItem.key());
                    fAction->setCheckable(true);
                    fAction->setChecked(fMapItem.value().toBool());
                }
                fPostActionGroup.addAction(tr("Enable all"));
                fPostActionGroup.addAction(tr("Disable all"));
            }   break;
            default:
                break;
        }
#else
        switch (fItemData.type())
        {
            case QVariant::ModelIndex:
            {
                QTreeWidgetHook* fSourceHook = reinterpret_cast<QTreeWidgetHook*>(this);
                *context_menu_source_treeItem = fSourceHook->itemFromIndex(fItemData.toModelIndex());
            }   break;
            case QVariant::Map:
            {
                auto fMap = fItemData.toMap();
                for (auto fMapItem = fMap.begin(); fMapItem != fMap.end(); ++fMapItem)
                {
                    QAction* fAction = fPostActionGroup.addAction(fMapItem.key());
                    fAction->setCheckable(true);
                    fAction->setChecked(fMapItem.value().toBool());
                }
                fPostActionGroup.addAction(tr("Enable all"));
                fPostActionGroup.addAction(tr("Disable all"));
            }   break;
            default:
                break;
        }
#endif
    }

    QMenu menu(this);
    QMenu*fAuthorsMenu = nullptr;
    if (fPostActionGroup.actions().size())
    {
        fAuthorsMenu = menu.addMenu(tr("Authors"));
        fAuthorsMenu->addActions(fPostActionGroup.actions());
    }

    actions.fillContextMenue(menu, Cmd::mContextMenuHistoryTree);
    QAction* fAction = menu.exec(mapToGlobal(pos) + menu_offset);
    if (fAction && fAuthorsMenu)
    {
        int fIndex = fAuthorsMenu->actions().indexOf(fAction);
        checkAuthorsIndex(fIndex, fAction->isChecked());
    }
}

QString QHistoryTreeWidget::clickItem(QTreeWidgetItem *aItem, int aColumn )
{
    QString fText;
    int fLevel = getItemLevel(aItem);
    if (fLevel == Level::Log)
    {
        switch (aColumn)
        {
        case History::Column::CommitDate:
        case History::Column::Author:
        {
            for (int fRole=History::Entry::CommitHash; fRole < History::Entry::NoOfEntries; ++fRole)
            {
                fText.append(History::name(static_cast<History::Entry::e>(fRole)));
                fText.append(getLineFeed());
                fText.append(aItem->data(History::Column::Commit, History::role(static_cast<History::Entry::e>(fRole))).toString());
                fText.append(getLineFeed());
            }
            auto fNoOfFiles = aItem->data(History::Column::Commit, History::role(History::Entry::NoOfFiles));
            if (fNoOfFiles.isValid())
            {
                fText.append(tr("Files: %1").arg(fNoOfFiles.toInt()));
                fText.append(getLineFeed());
            }
        } break;
        case History::Column::CommitHash:
        case History::Column::Description:
        {
            QString fGitCmd = "git show ";
            if (aColumn == History::Column::Description) fGitCmd += "--compact-summary ";
            fGitCmd.append(aItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
            QString fResultStr;
            int fResult = execute(fGitCmd, fResultStr);
            fText = fGitCmd;
            fText.append(getLineFeed());
            fText.append(fResultStr);

            if (fResult)
            {
                fText.append(getLineFeed());
                fText.append(tr("Result failure no: %1").arg(fResult));
            }
        } break;
        }
    }
    return fText;
}


void QHistoryTreeWidget::insertFileNames()
{
    const auto selected_items = selectedItems();
    if (   selected_items.count() == History::Diff::two_commits
        && getItemLevel(selected_items[0]) == Level::Log
        && getItemLevel(selected_items[1]) == Level::Log)
    {
        QTreeWidgetItem* parent = selected_items[0]->parent();
        int child1 = parent->indexOfChild(selected_items[0]);
        int child2 = parent->indexOfChild(selected_items[1]);
        if (abs(child1 - child2) > 1)
        {
            insertFileNames(parent, child1, child2);
            return;
        }
    }

    for (const auto& item : std::as_const(selected_items))
    {
        switch (static_cast<Level::e>(getItemLevel(item)))
        {
            case Level::Top:
            {
                int child_count = item->childCount();
                for (int child=0; child<(child_count-1); ++child)
                {
                    insertFileNames(item, child, History::Diff::list_files_to_parent);
                }
            }   break;
            case Level::Log:
            {
                QTreeWidgetItem* parent = item->parent();
                insertFileNames(parent, parent->indexOfChild(item), History::Diff::list_files_to_parent);
            }   break;
            case Level::File: break;
        }
    }
}

/// TODO: diff of single file does not work correct in every case
///       it sometimes shows only the file diff to nothing and not diff to previous version
///       this might be caused by the history extracted from the origin repostiory
/// TODO: revert branch to a previous commit, or undo a commit
/// TODO: implement merge branch to current branch
/// TODO: implement merge two branches
/// also remote branches
/// drag and drop?
/// git merge --no-commit --edit -F <file> <commit1> [<commit2>]
/// --no-commit: -> nicht committen
/// --edit     : -> merge tool aufrufen?
/// -s <strategy> --strategy=<strategy> (ours obsolete, theirs, ort, recursive
/// -X <strategy-option>
/// --stat -n, --no-stat Show a diffstat at the end of the merge. The diffstat is also controlled by the configuration option merge.stat.
/// --no-verify
/// merge.guitool -g --gui
/// git mergetool -F <file> <commit>

void QHistoryTreeWidget::insertFileNames(QTreeWidgetItem* parent_item, int child, int second_child)
{
    initialize();
    auto child_item    = parent_item->child(child);
    if (child_item && !child_item->isHidden())
    {
        const bool diff_over_one_step = second_child == History::Diff::to_next_commit;
        Type type(child_item->data(History::Column::Commit, History::role(History::Entry::Type)).toUInt());
        if ((!child_item->childCount() || !diff_over_one_step) && !type.is(Type::File))
        {
            if (diff_over_one_step)
            {
                second_child = child + 1;
            }
            QTreeWidgetItem* second_item = (second_child == History::Diff::to_current) ? nullptr : parent_item->child(second_child);
            QString git_cmd;
            if (second_item)
            {
                git_cmd = tr("git diff --name-only %1 %2").
                          arg(second_item->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString(),
                              child_item-> data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
            }
            else if (second_child == History::Diff::list_files_to_parent)
            {
                git_cmd = tr("git diff --name-only %1 %2").
                          arg(child_item->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString()).
                          arg(child_item->data(History::Column::Commit, History::role(History::Entry::ParentHash)).toString());
            }
            else
            {
                git_cmd = tr("git diff --name-only %1").
                          arg(child_item->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
            }

            if (type.is(Type::Branch))
            {
                QString file_name = parent_item->text(History::Column::Filename);
                int pos = file_name.indexOf("branch");
                if (pos != -1)
                {
                    file_name = file_name.left(pos);
                }
                git_cmd.replace("git ", file_name);

                file_name.replace("git -C ", "");
                auto tree_columns = QStringList(tr("Branch difference for repository %1").arg(file_name));
                QTreeWidgetItem* branch_diffs {nullptr};
                auto list = findItems(tree_columns[0], Qt::MatchExactly);
                if (list.size())
                {
                    branch_diffs = list[0];
                    child = branch_diffs->childCount();
                }
                else
                {
                    branch_diffs = new QTreeWidgetItem(tree_columns);
                    addTopLevelItem(branch_diffs);
                    child = 0;  // insert at top position
                }
                parent_item = branch_diffs;
            }
            else
            {
                git_cmd += " -- ";
                git_cmd += parent_item->text(History::Column::Filename);
            }

            QString result_string;
            int error = execute(git_cmd, result_string);
            if (!error)
            {
                TRACEX(Logger::to_browser, git_cmd);
                if (!diff_over_one_step && second_child != History::Diff::list_files_to_parent)
                {
                    QString second_name = second_item != 0 ? second_item->text(History::Column::Filename) : "current";
                    QString compared_items = child_item->text(History::Column::Filename) + " <-> " + second_name;
                    QTreeWidgetItem* new_child_item = new QTreeWidgetItem({compared_items});
                    parent_item->insertChild(child, new_child_item);
                    for (int role_entry=0; role_entry < History::Entry::NoOfEntries; ++role_entry)
                    {
                        int role = History::role(static_cast<History::Entry::e>(role_entry));
                        QString sep = (role_entry == History::Entry::CommitHash) ? " " : " <-> ";
                        QString role_text = child_item->data(History::Column::Commit, role).toString() +
                                           (second_item ? (sep + second_item->data(History::Column::Commit, role).toString()) : "");
                        new_child_item->setData(History::Column::Commit, role, QVariant(role_text));
                    }
                    Type type(Type::Folder);
                    type.add(Type::DiffOf2Commits);
                    new_child_item->setData(History::Column::Commit, History::role(History::Entry::Type), QVariant(type.type()));

                    child_item = new_child_item;
                }
                git_cmd = git_cmd.replace("--name-only", "%1");
                int pos = git_cmd.indexOf(" -- ");
                if (pos != -1)
                {
                    git_cmd = git_cmd.left(pos);
                }
                child_item->setData(History::Column::Commit, History::role(History::Entry::GitDiffCommand), git_cmd);

                const auto files = result_string.split("\n");
                for (const auto& file_path : std::as_const(files))
                {
                    if (file_path.size())
                    {
                        child_item->addChild(new QTreeWidgetItem({file_path}));
                    }
                }
                child_item->setData(History::Column::Commit, History::role(History::Entry::NoOfFiles), files.count());
                if (type.is(Type::Branch))
                {
                    expandAll();
                    Q_EMIT show_me(this);
                }
            }
            else
            {
                TRACEX(Logger::to_browser, "error inserting filenames:\n" << git_cmd << "\n" <<  result_string << "\nerror number: " << error);
            }
        }
    }
}

void QHistoryTreeWidget::mousePressEvent(QMouseEvent *event)
{
    mIsPressed |= event->button();
    QTreeWidget::mousePressEvent(event);
}

void QHistoryTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mIsPressed &= ~event->button();
    QTreeWidget::mouseReleaseEvent(event);
}



const QString& QHistoryTreeWidget::getSelectedHistoryHashItems()
{
    return mHistoryHashItems;
}

const QString& QHistoryTreeWidget::getSelectedHistoryFile()
{
    return mHistoryFile;
}

const QString &QHistoryTreeWidget::getSelectedHistoryDescription()
{
    return mHistoryDescription;
}

uint QHistoryTreeWidget::getSelectedTopLevelType()
{
    return mSelectedTopLevelItemType;
}

bool QHistoryTreeWidget::isSelectionDiffable()
{
    return mHistoryHashItems.size() > 0;
}

bool QHistoryTreeWidget::isSelectionFileDiffable()
{
    return mHistoryFile.size() > 0;
}

bool QHistoryTreeWidget::isMouseButtonPressed(Qt::MouseButton button)
{
    return (mIsPressed & button) != 0;
}

QDrawGraphItemDelegate::QDrawGraphItemDelegate(QObject *parent) : QItemDelegate(parent)
{

}

QWidget *QDrawGraphItemDelegate::createEditor(QWidget * /* parent */, const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const
{
    return nullptr;
}

void QDrawGraphItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == History::Column::CommitGraph)
    {
        /// NOTE: clear background
        QBrush brush = option.backgroundBrush;
        painter->setBrush(brush);
        QColor color (Qt::GlobalColor::white);
        painter->setPen(color);
        painter->drawRect(option.rect);


        auto rect = option.rect;
        QVariant variant = index.model()->data(index, History::role(History::Entry::DrawItems));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (variant.isValid() && variant.typeId() == QMetaType::QVariantList)
#else
        if (variant.isValid() && variant.type() == QVariant::List)
#endif
        {
            const auto& variant_list = variant.toList();
            for (const auto& draw_item : variant_list)
            {
                HistoryEntries::ConnectionUnion cu;
                cu.connection_id = draw_item.toUInt();

                QColor pen_color(static_cast<Qt::GlobalColor>(cu.connection.color));
                QPen pen(pen_color);
                pen.setWidth(m_pen_width);
                painter->setPen(pen);

                switch (cu.connection.type)
                {
                case HistoryEntries::Connection::start:
                    drawStart(*painter, rect, cu.connection.level2);
                    break;
                case HistoryEntries::Connection::line:
                    drawLine(*painter, rect, cu.connection.level1, cu.connection.level2);
                    break;
                case HistoryEntries::Connection::end:
                    drawEnd(*painter, rect, cu.connection.level1);
                    break;
                }
            }
        }
    }
    else
    {
        QItemDelegate::paint(painter, option, index);
    }
}

void QDrawGraphItemDelegate::drawStart(QPainter &painter, const QRect& rc, int level) const
{
    qreal p1x = getLevelPosition(rc, 0);
    qreal p1y = rc.center().y();
    qreal p2x = getLevelPosition(rc, level);
    qreal p2y = rc.top();
#if 1
    qreal cpx = p2x + (p1x - p2x) * 0.25;
    qreal cpy = p1y + (p2y - p1y) * 0.25;
    drawBezier(painter, p1x, p1y, cpx, cpy, cpx, cpy, p2x, p2y);
#else
    painter.drawLine(p1x, p1y, p2x, p2y);
#endif
}

void QDrawGraphItemDelegate::drawEnd(QPainter &painter, const QRect &rc, int level) const
{
    qreal p1x = getLevelPosition(rc, level);
    qreal p1y = rc.bottom();
    qreal p2x = getLevelPosition(rc, 0);
    qreal p2y = rc.center().y();
#if 1
    qreal cpx = p1x + (p2x - p1x) * 0.25;
    qreal cpy = p2y + (p1y - p2y) * 0.25;
    drawBezier(painter, p1x, p1y, cpx, cpy, cpx, cpy, p2x, p2y);
#else
    painter.drawLine(p1x, p1y, p2x, p2y);
#endif
}

void QDrawGraphItemDelegate::drawLine(QPainter &painter, const QRect& rc, int level1, int level2) const
{
    qreal p1x = getLevelPosition(rc, level1);
    qreal p1y = rc.bottom();
    qreal p2x = getLevelPosition(rc, level2);
    qreal p2y = rc.top();
    qreal pmy = rc.center().y();
    drawBezier(painter, p1x, p1y, p1x, pmy, p2x, pmy, p2x, p2y);
}

qreal QDrawGraphItemDelegate::getLevelPosition(QRect rc, int level) const
{
    if (m_left_border)
    {
        return rc.left() + rc.width() * level * m_level_factor;
    }
    else
    {
        return rc.right() - rc.width() * level * m_level_factor;
    }
}

void QDrawGraphItemDelegate::drawBezier(QPainter& painter, qreal p1x, qreal p1y, qreal cp1x, qreal cp1y, qreal cp2x, qreal cp2y, qreal p2x, qreal p2y)
{
    QPainterPath path;
    path.moveTo(p1x, p1y);
    path.cubicTo(cp1x, cp1y, cp2x, cp2y, p2x, p2y);
    painter.drawPath(path);
}

HistoryEntries::HistoryEntries(const QVector<QStringList> &items) : m_items(items)
{
    m_color.unapply_color(Qt::yellow);
    determine_parents();
    determine_connections();
}

QList<QVariant> HistoryEntries::get_connection_part(int index) const
{
    if (m_item_connections.contains(index))
    {
        QList<QVariant> list;
        for (const auto& item : m_item_connections[index])
        {
            list.push_back(item);
        }
        return list;
    }
    return {};
}

void HistoryEntries::determine_parents()
{
    int i = -1;
    for (const auto& item :  m_items)
    {
        if (++i == 0) continue;
        QStringList parents = item[History::Entry::ParentHash].split(" ");
        m_parent1[parents[0]] = i;
        if (parents.size() > 1)
        {
            m_parent2[parents[1]] = i;
        }
    }
}

void HistoryEntries::determine_connections()
{
    int index = -1;
    for (const auto& item :  m_items)
    {
        if (++index == 0) continue;
        int parent1 = m_parent1.value(item[History::Entry::CommitHash], -1);
        if (parent1 != -1 && (index - parent1) > m_diff)
        {
            create_connection(parent1, index);
        }
        int parent2 = m_parent2.value(item[History::Entry::CommitHash], -1);
        if (parent2 != -1 && (index - parent2) > m_diff)
        {
            create_connection(parent2, index);
        }
    }

    /// reduce level at double connections
    for (auto connection = m_item_connections.begin(); connection != m_item_connections.end(); ++connection)
    {
        auto& connection_vector = connection.value();
        if (connection_vector.size() > 1 && reinterpret_cast<Connection*>(&connection_vector[0])->type == Connection::start)
        {
            auto pt00 = reinterpret_cast<ConnectionUnion*>(&connection_vector[0]);
            auto pt01 = reinterpret_cast<ConnectionUnion*>(&connection_vector[1]);
            if (pt00->connection.type == Connection::start && pt01->connection.type == Connection::end)
            {
                pt01->connection.level1--;
                for (int i=2; i < connection_vector.size(); ++i)
                {
                    auto pt10 = reinterpret_cast<ConnectionUnion*>(&connection_vector[i]);
                    pt10->connection.level1--;
                }

                auto connection_p_1    = connection;
                ++connection_p_1;
                auto & connection_vector_p_1 = connection_p_1.value();
                for (int i=0; i < connection_vector_p_1.size(); ++i)
                {
                    auto pt10 = reinterpret_cast<ConnectionUnion*>(&connection_vector_p_1[i]);
                    pt10->connection.level2--;
                }
            }
        }
    }
}

void HistoryEntries::create_connection(int parent, int index)
{
    ulong color_enum;
    ulong             last_item_level = 0;
    Connection::eType type            = Connection::start;
    int               item            = index;
    ConnectionUnion   con_unit;

    m_color.get_color_and_increment(&color_enum);

    for (; item > parent; --item)
    {
        auto&   connection_vector = m_item_connections[item];
        quint32 next_item_level   = 1;
        int     next_item         = item - 1;

        if (m_item_connections.contains(next_item))
        {
            next_item_level = m_item_connections[next_item].size() + 1;
        }

        con_unit.connection.color  = color_enum;
        con_unit.connection.type   = type;
        if (con_unit.connection.type == Connection::start)
        {
            con_unit.connection.level1 = 0;
            con_unit.connection.level2 = std::max(static_cast<quint32>(connection_vector.size()), next_item_level);
        }
        else // Connection::line
        {
            con_unit.connection.level1 = last_item_level;
            con_unit.connection.level2 = next_item_level;
        }
        connection_vector.push_back(con_unit.connection_id);

        last_item_level = con_unit.connection.level2;
        type            = Connection::line;
    }
    auto& connection_vector    = m_item_connections[item];
    con_unit.connection.level1 = last_item_level;
    con_unit.connection.level2 = 0;
    con_unit.connection.type   = Connection::end;
    connection_vector.push_back(con_unit.connection_id);
}

