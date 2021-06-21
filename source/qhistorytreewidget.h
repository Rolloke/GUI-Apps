#ifndef QHISTORYTREEWIDGET_H
#define QHISTORYTREEWIDGET_H

#include <QTreeWidget>

class QHistoryTreeWidget : public QTreeWidget
{
    Q_OBJECT

    struct Level { enum e
    {
        Top,
        Log,
        File
    }; };

public:
    QHistoryTreeWidget(QWidget *parent = 0);

    void            parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType);
    QVariant        customContextMenuRequested(const QPoint &pos);
    QVariant        determineHistoryHashItems(QTreeWidgetItem* fSelectedHistoryItem);
    QString         itemClicked(QTreeWidgetItem *aItem, int aColumn);
    void            checkAuthorsIndex(int aIndex, bool aChecked);

    const QString&  getSelectedHistoryHashItems();
    const QString&  getSelectedHistoryFile();
    uint            getSelectedTopLevelType();
    bool            isSelectionDiffable();
    bool            isSelectionFileDiffable();

Q_SIGNALS:

public Q_SLOTS:
    void clear();
    void insertFileNames();
    void insertFileNames(QTreeWidgetItem* aParent, int aChild);

protected:

private:

    QString mHistoryFile;
    QString mHistoryHashItems;
    uint    mSelectedTopLevelItemType;

};

#endif // QHISTORYTREEWIDGET_H
