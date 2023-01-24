#ifndef QHISTORYTREEWIDGET_H
#define QHISTORYTREEWIDGET_H

#include <QTreeWidget>

class ActionList;

class QHistoryTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    struct Level { enum e
    {
        Top,
        Log,
        File
    }; };
    QHistoryTreeWidget(QWidget *parent = 0);

    void            parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType);
    void            customContextMenuRequested(const QPoint &pos, const ActionList&, QTreeWidgetItem**);
    QVariant        determineHistoryHashItems(QTreeWidgetItem* fSelectedHistoryItem);
    QString         clickItem(QTreeWidgetItem *aItem, int aColumn);
    void            checkAuthorsIndex(int aIndex, bool aChecked);
    void            setShowHistoryGraphically(bool show);

    const QString&  getSelectedHistoryHashItems();
    const QString&  getSelectedHistoryFile();
    uint            getSelectedTopLevelType();
    bool            isSelectionDiffable();
    bool            isSelectionFileDiffable();

Q_SIGNALS:
    void send_history(const QStringList& list);
    void reset_history();

public Q_SLOTS:
    void clear();
    void insertFileNames();
    void insertFileNames(QTreeWidgetItem* parent, int child, int second_child=-1);

protected:

private:

    QString mHistoryFile;
    QString mHistoryHashItems;
    uint    mSelectedTopLevelItemType { 0 };
    bool    mInitialized { false };
    bool    mShowHistoryGraphically;

};

#endif // QHISTORYTREEWIDGET_H
