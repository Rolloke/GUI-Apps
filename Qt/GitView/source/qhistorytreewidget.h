#ifndef QHISTORYTREEWIDGET_H
#define QHISTORYTREEWIDGET_H

#include <QTreeWidget>

class QHistoryTreeWidget : public QTreeWidget
{
    Q_OBJECT

    enum class Level
    {
        Top,
        Log,
        File
    };

public:
    QHistoryTreeWidget(QWidget *parent = 0);

    void            parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName, uint aType);
    QVariant        customContextMenuRequested(const QPoint &pos);
    QString         itemClicked(QTreeWidgetItem *aItem, int aColumn);
    void            checkAuthorsIndex(int aIndex, bool aChecked);

    const QString&  getSelectedHistoryHashItems();
    const QString&  getSelectedHistoryFile();
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
};

#endif // QHISTORYTREEWIDGET_H
