#include "qhistorytreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include "helper.h"
#include "history.h"
#include "git_type.h"

using namespace git;

QHistoryTreeWidget::QHistoryTreeWidget(QWidget *parent): QTreeWidget(parent)
{
    setVisible(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    header()->setSectionResizeMode(INT(History::Column::Text), QHeaderView::Stretch);
    header()->setStretchLastSection(false);
}


void QHistoryTreeWidget::clear()
{
    QTreeWidget::clear();
}


void QHistoryTreeWidget::parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType)
{
    QVector<QStringList> fList;
    History::parse(fText, fList);

    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(aFileName));
    addTopLevelItem(fNewHistoryItem);

    fNewHistoryItem->setData(INT(History::Column::Commit), INT(History::Role::ContextMenuItem), aData);
    QMap<QString, QVariant> fAuthors;

    setVisible(true);

    const int fTLI = topLevelItemCount()-1;
    int fListCount = 0;
    for (const auto& fItem : fList)
    {
        if (++fListCount == 1) continue; // ignore first item
        if (fItem.count() >= INT(History::Entry::NoOfEntries))
        {
            QTreeWidgetItem* fNewHistoryLogItem = new QTreeWidgetItem();
            topLevelItem(fTLI)->addChild(fNewHistoryLogItem);
            fNewHistoryLogItem->setText(INT(History::Column::Text), fItem[INT(History::Entry::CommitterDate)]);
            fNewHistoryLogItem->setText(INT(History::Column::Author), fItem[INT(History::Entry::Author)]);
            fAuthors[fItem[INT(History::Entry::Author)]] = true;
            for (int fRole=0; fRole < INT(History::Entry::NoOfEntries); ++fRole)
            {
                fNewHistoryLogItem->setData(INT(History::Column::Commit), fRole, QVariant(fItem[fRole]));
            }
            fNewHistoryLogItem->setData(INT(History::Column::Commit), INT(History::Entry::Type), QVariant(aType));
        }
    }

    fNewHistoryItem->setData(INT(History::Column::Commit), INT(History::Role::VisibleAuthors), QVariant(fAuthors));
}

void QHistoryTreeWidget::checkAuthorsIndex(int aIndex, bool aChecked)
{
    auto fSelected = selectionModel()->selectedIndexes();
    if (fSelected.count())
    {
        QTreeWidgetItem* fItem = itemFromIndex(fSelected.first());
        if (fItem && getItemLevel(fItem) == INT(Level::Top))
        {
            auto fItemData = fItem->data(INT(History::Column::Commit), INT(History::Role::VisibleAuthors));
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
                fItem->setData(INT(History::Column::Commit), INT(History::Role::VisibleAuthors), QVariant(fMap));
                // TODO: set checked authors visible and unchecked hidden
            }
        }
    }
}


QVariant QHistoryTreeWidget::customContextMenuRequested(const QPoint &pos)
{
    QVariant fItemData;
    mHistoryHashItems.clear();
    mHistoryFile.clear();

    QTreeWidgetItem* fSelectedHistoryItem = itemAt(pos);
    if (fSelectedHistoryItem)
    {
        int fColumns = header()->count();
        QModelIndexList fSelectedIndexes = selectionModel()->selectedIndexes();

        switch (static_cast<Level>(getItemLevel(fSelectedHistoryItem)))
        {
            case Level::Top:
            {
                fItemData = fSelectedHistoryItem->data(INT(History::Column::Commit), INT(History::Role::VisibleAuthors));
            }   break;
            case Level::Log:
            {
                QTreeWidgetItem* fParentHistoryItem = fSelectedHistoryItem->parent();
                if (fParentHistoryItem)
                {
                    fItemData = fParentHistoryItem->data(INT(History::Column::Commit), INT(History::Role::ContextMenuItem));

                    for (auto fIndex = fSelectedIndexes.rbegin(); fIndex != fSelectedIndexes.rend(); fIndex += fColumns)
                    {
                        QTreeWidgetItem* fItem = itemFromIndex(*fIndex);
                        if (fItem && fItem->parent() == fParentHistoryItem)
                        {
                            mHistoryHashItems.append(fItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
                            mHistoryHashItems.append(" ");
                        }
                    }
                }
            }   break;
            case Level::File:
            {
                mHistoryFile = fSelectedHistoryItem->data(INT(History::Column::Text), Qt::DisplayRole).toString();

                QTreeWidgetItem* fHistoryLogItem = fSelectedHistoryItem->parent();
                QTreeWidgetItem* fTopItem = fHistoryLogItem->parent();

                int fIndex = fTopItem->indexOfChild(fHistoryLogItem);
                if (fIndex == 0)
                {
                    mHistoryHashItems.append(fHistoryLogItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
                }
                else if (fIndex > 0)
                {
                    mHistoryHashItems.append(fHistoryLogItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
                    mHistoryHashItems.append(" ");
                    fHistoryLogItem = fTopItem->child(fIndex-1);
                    mHistoryHashItems.append(fHistoryLogItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
                }
            }   break;
        }
    }
    return fItemData;
}

QString QHistoryTreeWidget::itemClicked(QTreeWidgetItem *aItem, int aColumn )
{
    QString fText;
    int fLevel = getItemLevel(aItem);
    if (fLevel == INT(Level::Log))
    {
        if (aColumn == INT(History::Column::Text))
        {
            for (int fRole=INT(History::Entry::CommitHash); fRole < INT(History::Entry::NoOfEntries); ++fRole)
            {
                fText.append(History::name(static_cast<History::Entry>(fRole)));
                fText.append(getLineFeed());
                fText.append(aItem->data(INT(History::Column::Commit), fRole).toString());
                fText.append(getLineFeed());
            }
            auto fNoOfFiles = aItem->data(INT(History::Column::Commit), INT(History::Entry::NoOfFiles));
            if (fNoOfFiles.isValid())
            {
                fText.append(tr("Files: %1").arg(fNoOfFiles.toInt()));
                fText.append(getLineFeed());
            }
        }
        else
        {
            QString fGitCmd = "git show ";
            fGitCmd.append(aItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
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
        switch (static_cast<Level>(getItemLevel(fItem)))
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
    if (fChildItem && getItemLevel(fParent) == INT(Level::Top))
    {
        Type fType(fChildItem->data(INT(History::Column::Commit), INT(History::Entry::Type)).toUInt());
        if (!fChildItem->childCount() && !fType.is(Type::File))
        {
            int fPrevious = fChild - 1;
            auto fPreviousItem = fParent->child(fPrevious);
            QString fGitCmd;
            if (fPreviousItem)
            {
                fGitCmd = tr("git diff --name-only %1 %2").
                          arg(fChildItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString()).
                          arg(fPreviousItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
            }
            else
            {
                fGitCmd = tr("git diff --name-only %1").
                          arg(fChildItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
            }

            // TODO: validate, if this is correct
            if (fType.is(Type::Branch))
            {
                fGitCmd += " ";
                fGitCmd += fParent->text(INT(History::Column::Text));
            }

            QString fResultStr;
            int fError = execute(fGitCmd, fResultStr);
            if (!fError)
            {
                fGitCmd = fGitCmd.replace("--name-only", "%1");
                fChildItem->setData(INT(History::Column::Commit), INT(History::Entry::GitDiffCommand), fGitCmd);

                auto fFiles = fResultStr.split("\n");
                for (const auto& fFile : fFiles)
                {
                    fChildItem->addChild(new QTreeWidgetItem({fFile}));
                }
                fChildItem->setData(INT(History::Column::Commit), INT(History::Entry::NoOfFiles), fFiles.count());
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

bool QHistoryTreeWidget::isSelectionDiffable()
{
    return mHistoryHashItems.size() > 0;
}

bool QHistoryTreeWidget::isSelectionFileDiffable()
{
    return mHistoryFile.size() > 0;
}


