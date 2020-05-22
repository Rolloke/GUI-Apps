#ifndef QHISTORYTREEWIDGET_H
#define QHISTORYTREEWIDGET_H

#include <QTreeWidget>

class QHistoryTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    QHistoryTreeWidget(QWidget *parent = 0);

    void            parseGitLogHistoryText(const QString& fText, const QVariant& aData, const QString& aFileName);
    QVariant        customContextMenuRequested(const QPoint &pos);
    const QString&  getSelectedHistoryHashItems();
    QString         itemClicked(QTreeWidgetItem *aItem, int aColumn);

Q_SIGNALS:

public Q_SLOTS:
    void clear();

protected:

private:

    QString mHistoryHashItems;
};

#endif // QHISTORYTREEWIDGET_H
