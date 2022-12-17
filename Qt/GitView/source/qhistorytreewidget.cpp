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


void QHistoryTreeWidget::parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType)
{
    if (!mInitialized)
    {
        header()->setSectionResizeMode(History::Column::CommitDate, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(History::Column::Description, QHeaderView::Stretch);
        header()->setStretchLastSection(false);
        mInitialized = true;
    }
    QVector<QStringList> fList;
    History::parse(fText, fList);

    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(aFileName));
    addTopLevelItem(fNewHistoryItem);

    fNewHistoryItem->setData(History::Column::Commit, History::Role::ContextMenuItem, aData);
    QMap<QString, QVariant> fAuthors;

    setVisible(true);
    Q_EMIT reset_history();

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
//            Q_EMIT send_history(fItem);
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
    auto fSelected = selectedItems();
    for (const auto& fItem : fSelected)
    {
        switch (static_cast<Level::e>(getItemLevel(fItem)))
        {
            case Level::Top:
            {
                int fCount = fItem->childCount();
                for (int fChild=0; fChild<(fCount-1); ++fChild)
                {
                    insertFileNames(fItem, fChild);
                }
            }   break;
            case Level::Log:
            {
                QTreeWidgetItem* fParent = fItem->parent();
                insertFileNames(fParent, fParent->indexOfChild(fItem));
            }   break;
            case Level::File: break;
        }
    }
}

void QHistoryTreeWidget::insertFileNames(QTreeWidgetItem* fParent, int fChild)
{
    auto fChildItem    = fParent->child(fChild);
    if (fChildItem && !fChildItem->isHidden())
    {
        Type fType(fChildItem->data(History::Column::Commit, History::role(History::Entry::Type)).toUInt());
        if (!fChildItem->childCount() && !fType.is(Type::File))
        {
            auto fNextItem = fParent->child(fChild + 1);
            QString fGitCmd;
            if (fNextItem)
            {
                fGitCmd = tr("git diff --name-only %1 %2").
                          arg(fNextItem->data( History::Column::Commit, History::role(History::Entry::CommitHash)).toString(),
                              fChildItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
            }
            else
            {
                fGitCmd = tr("git diff --name-only %1").
                          arg(fChildItem->data(History::Column::Commit, History::role(History::Entry::CommitHash)).toString());
            }


            if (fType.is(Type::Branch))
            {   // FIXME: validate, if this is correct also for branch
                fGitCmd += " -- ";
                fGitCmd += fParent->text(History::Column::CommitDate);
            }
            else
            {
                fGitCmd += " -- ";
                fGitCmd += fParent->text(History::Column::CommitDate);
            }

            QString fResultStr;
            int fError = execute(fGitCmd, fResultStr);
            if (!fError)
            {
                fGitCmd = fGitCmd.replace("--name-only", "%1");
                fChildItem->setData(History::Column::Commit, History::role(History::Entry::GitDiffCommand), fGitCmd);

                auto fFiles = fResultStr.split("\n");
                for (const auto& fFile : fFiles)
                {
                    fChildItem->addChild(new QTreeWidgetItem({fFile}));
                }
                fChildItem->setData(History::Column::Commit, History::role(History::Entry::NoOfFiles), fFiles.count());
            }
            else
            {
                TRACE(Logger::to_browser, tr("error inserting filenames:\n%1\n%2\nerror number: %d").arg(fGitCmd, fResultStr).toStdString().c_str(), fError);
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


