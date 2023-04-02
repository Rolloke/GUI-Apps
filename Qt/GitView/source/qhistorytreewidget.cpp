#include "qhistorytreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include "actions.h"
#include "helper.h"
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
        header()->setSectionResizeMode(History::Column::CommitDate, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(History::Column::Description, QHeaderView::Stretch);
        header()->setStretchLastSection(false);
        mInitialized = true;
    }
}

void QHistoryTreeWidget::parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType)
{
    initialize();
    QVector<QStringList> fList;
    History::parse(fText, fList);

    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(aFileName));
    addTopLevelItem(fNewHistoryItem);

    fNewHistoryItem->setData(History::Column::Commit, History::Role::ContextMenuItem, aData);
    QMap<QString, QVariant> fAuthors;

    setVisible(true);
    if (mShowHistoryGraphically)
    {
        Q_EMIT reset_history();
    }

    const int fTLI = topLevelItemCount()-1;
    int fListCount = 0;
    for (auto& fItem : fList)
    {
        int fLF = fItem[0].indexOf('\n');
        if (fLF != -1)
        {
            fItem[0] = fItem[0].mid(fLF+1);
        }
        if (++fListCount == 1) continue; // ignore first item
        if (fItem.count() >= History::Entry::NoOfEntries)
        {
            QTreeWidgetItem* fNewHistoryLogItem = new QTreeWidgetItem();
            topLevelItem(fTLI)->addChild(fNewHistoryLogItem);
            fNewHistoryLogItem->setText(History::Column::CommitDate, fItem[History::Entry::CommitterDate]);
            fNewHistoryLogItem->setText(History::Column::Description, fItem[History::Entry::SubjectAndBody].split("\n")[0]);
            fNewHistoryLogItem->setText(History::Column::Author, fItem[History::Entry::Author]);
            fAuthors[fItem[History::Entry::Author]] = true;
            for (int fRole=0; fRole < History::Entry::NoOfEntries; ++fRole)
            {
                fNewHistoryLogItem->setData(History::Column::Commit, History::role(static_cast<History::Entry::e>(fRole)), QVariant(fItem[fRole]));
            }
            fNewHistoryLogItem->setData(History::Column::Commit, History::role(History::Entry::Type), QVariant(aType));
            if (mShowHistoryGraphically)
            {
                Q_EMIT send_history(fItem);
            }
        }
    }
    if (mShowHistoryGraphically)
    {
        Q_EMIT send_history({});
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
            if (fItemData.type() == QVariant::Map)
            {
                auto fMap = fItemData.toMap();
                if (aIndex < fMap.size())
                {
                    auto fMapItem = fMap.begin()+aIndex;
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

void QHistoryTreeWidget::setShowHistoryGraphically(bool show)
{
    mShowHistoryGraphically = show;
}

QVariant QHistoryTreeWidget::determineHistoryHashItems(QTreeWidgetItem* fSelectedHistoryItem)
{
    QVariant fItemData;
    mHistoryHashItems.clear();
    mHistoryFile.clear();
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
                    for (auto fIndex = fSelectedIndexes.rbegin(); fIndex != fSelectedIndexes.rend(); ++fIndex)
                    {
                        QTreeWidgetItem* fItem = itemFromIndex(*fIndex);
                        if (fItem && fItem->parent() == fParentHistoryItem)
                        {
                            mHistoryHashItems.append(fItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
                            mHistoryHashItems.append(" ");
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
                }
                else
                {
                    mHistoryHashItems.append(fHistoryLogItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
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
        if (aColumn == History::Column::CommitDate)
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
        }
        else
        {
            QString fGitCmd = "git show ";
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
        }
    }
    return fText;
}


void QHistoryTreeWidget::insertFileNames()
{
    const auto selected_items = selectedItems();
    if (   selected_items.count() == 2
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

    for (const auto& item : selected_items)
    {
        switch (static_cast<Level::e>(getItemLevel(item)))
        {
            case Level::Top:
            {
                int child_count = item->childCount();
                for (int child=0; child<(child_count-1); ++child)
                {
                    insertFileNames(item, child);
                }
            }   break;
            case Level::Log:
            {
                QTreeWidgetItem* parent = item->parent();
                insertFileNames(parent, parent->indexOfChild(item));
            }   break;
            case Level::File: break;
        }
    }
}

void QHistoryTreeWidget::insertFileNames(QTreeWidgetItem* parent_item, int child, int second_child)
{
    initialize();
    auto child_item    = parent_item->child(child);
    if (child_item && !child_item->isHidden())
    {
        const bool diff_over_one_step = second_child == -1;
        Type type(child_item->data(History::Column::Commit, History::role(History::Entry::Type)).toUInt());
        if ((!child_item->childCount() || !diff_over_one_step) && !type.is(Type::File))
        {
            if (diff_over_one_step)
            {
                second_child = child + 1;
            }
            auto second_item = parent_item->child(second_child);
            QString git_cmd;
            if (second_item)
            {
                git_cmd = tr("git diff --name-only %1 %2").
                          arg(second_item->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString(),
                              child_item-> data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
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
                if (!diff_over_one_step && second_item)
                {
                    QString compared_items = child_item->text(History::Column::Filename) + " <-> " + second_item->text(History::Column::Filename);
                    QTreeWidgetItem* new_child_item = new QTreeWidgetItem({compared_items});
                    parent_item->insertChild(child, new_child_item);
                    for (int role_entry=0; role_entry < History::Entry::NoOfEntries; ++role_entry)
                    {
                        int role = History::role(static_cast<History::Entry::e>(role_entry));
                        QString sep = (role_entry == History::Entry::CommitHash) ? " " : " <-> ";
                        QString role_text = child_item->data(History::Column::Commit, role).toString() + sep +
                                           second_item->data(History::Column::Commit, role).toString();
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
                for (const auto& file_path : files)
                {
                    child_item->addChild(new QTreeWidgetItem({file_path}));
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

const QString& QHistoryTreeWidget::getSelectedHistoryHashItems()
{
    return mHistoryHashItems;
}

const QString& QHistoryTreeWidget::getSelectedHistoryFile()
{
    return mHistoryFile;
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


