#include "qhistorytreewidget.h"
#include <QHeaderView>
#include <QMenu>
#include "helper.h"
#include "history.h"

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


void QHistoryTreeWidget::parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName)
{
    QVector<QStringList> fList;
    History::parse(fText, fList);

    QTreeWidgetItem* fNewHistoryItem = new QTreeWidgetItem(QStringList(aFileName));
    addTopLevelItem(fNewHistoryItem);

    fNewHistoryItem->setData(INT(History::Column::Commit), INT(History::Role::ContextMenuItem), aData);

    setVisible(true);

    const int fTLI = topLevelItemCount()-1;
    int fListCount = 0;
    for (auto fItem: fList)
    {
        if (++fListCount == 1) continue; // ignore first item
        if (fItem.count() >= INT(History::Entry::NoOfEntries))
        {
            fNewHistoryItem = new QTreeWidgetItem();
            topLevelItem(fTLI)->addChild(fNewHistoryItem);
            fNewHistoryItem->setText(INT(History::Column::Text), fItem[INT(History::Entry::CommitterDate)]);
            fNewHistoryItem->setText(INT(History::Column::Author), fItem[INT(History::Entry::Author)]);
            for (int fRole=0; fRole < INT(History::Entry::NoOfEntries); ++fRole)
            {
                fNewHistoryItem->setData(INT(History::Column::Commit), fRole, QVariant(fItem[fRole]));
            }
        }
    }
}

QVariant QHistoryTreeWidget::customContextMenuRequested(const QPoint &pos)
{
    QVariant fItemData;
    QTreeWidgetItem* fSelectedHistoryItem = itemAt(pos);
    if (fSelectedHistoryItem)
    {
        int fColumns = header()->count();
        QModelIndexList fSelectedHistoryIndexes = selectionModel()->selectedIndexes();
        QTreeWidgetItem* fParentHistoryItem     = fSelectedHistoryItem->parent();
        if (fParentHistoryItem)
        {
            fItemData = fParentHistoryItem->data(INT(History::Column::Commit), INT(History::Role::ContextMenuItem));

            mHistoryHashItems.clear();
            for (auto fIndex = fSelectedHistoryIndexes.rbegin(); fIndex != fSelectedHistoryIndexes.rend(); fIndex += fColumns)
            {
                QTreeWidgetItem* fItem = itemFromIndex(*fIndex);
                if (fItem && fItem->parent() == fParentHistoryItem)
                {
                    mHistoryHashItems.append(fItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
                    mHistoryHashItems.append(" ");
                }
            }
        }
    }
    return fItemData;
}

QString QHistoryTreeWidget::itemClicked(QTreeWidgetItem *aItem, int /* aColumn */)
{
    QString fText;
//    if (aColumn == INT(History::Column::Text))
    {
        for (int fRole=INT(History::Entry::CommitHash); fRole < INT(History::Entry::NoOfEntries); ++fRole)
        {
            fText.append(History::name(static_cast<History::Entry>(fRole)));
            fText.append(getLineFeed());
            fText.append(aItem->data(INT(History::Column::Commit), fRole).toString());
            fText.append(getLineFeed());
        }
    }
//    else
//    {
//        QString fGitCmd = "git show ";
//        fGitCmd.append(aItem->data(INT(History::Column::Commit), INT(History::Entry::CommitHash)).toString());
//        QString fResultStr;
//        int fResult = execute(fGitCmd, fResultStr);
//        fText = fGitCmd;
//        fText.append(getLineFeed());
//        fText.append(fResultStr);

//        if (fResult)
//        {
//            fText.append(getLineFeed());
//            fText.append(tr("Result failure no: %1").arg(fResult));
//        }
//    }
    return fText;
}

const QString& QHistoryTreeWidget::getSelectedHistoryHashItems()
{
    return mHistoryHashItems;
}


