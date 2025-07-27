#ifndef QHISTORYTREEWIDGET_H
#define QHISTORYTREEWIDGET_H

#include <QTreeWidget>
#include <QItemDelegate>
#include <QMap>
#include <QMouseEvent>
#include "helper.h"

class ActionList;
class QDrawGraphItemDelegate;

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

    const QString&  getSelectedHistoryHashItems();
    const QString&  getSelectedHistoryFile();
    const QString&  getSelectedHistoryDescription();
    uint            getSelectedTopLevelType();
    bool            isSelectionDiffable();
    bool            isSelectionFileDiffable();
    bool            isMouseButtonPressed(Qt::MouseButton button);

Q_SIGNALS:
    void show_me(QWidget* widget);

public Q_SLOTS:
    void clear();
    void insertFileNames();
    void insertFileNames(QTreeWidgetItem* parent_item, int child, int second_child=-1);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initialize();

    QString mHistoryFile;
    QString mHistoryHashItems;
    QString mHistoryDescription;
    uint    mSelectedTopLevelItemType { 0 };
    bool    mInitialized { false };
    int     mIsPressed { 0 };

    QDrawGraphItemDelegate* m_item_delegate;

};

class HistoryEntries
{
public:
    HistoryEntries(const QVector<QStringList>& items);
    struct Connection
    {
        enum eType { start, line, end };
        uchar level1;
        uchar level2;
        uchar color;
        uchar type;
    };

    union ConnectionUnion
    {
        quint32    connection_id;
        Connection connection;
    };

    QList<QVariant> get_connection_part(int index) const;

private:
    void determine_parents();
    void determine_connections();
    void create_connection(int parent, int index);

    const QVector<QStringList>& m_items;
    QMap<QString, int>          m_parent1;
    QMap<QString, int>          m_parent2;
    QMap<int, QVector<quint32>> m_item_connections;
    int                         m_diff = 1;
    ColorSelector               m_color;
};

class QDrawGraphItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:

    explicit QDrawGraphItemDelegate(QObject *parent = 0);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:

private:
    void drawStart(QPainter &painter, const QRect &rc, int level) const;
    void drawEnd(QPainter &painter, const QRect& rc, int level) const;
    void drawLine(QPainter &painter, const QRect &rc, int level1, int level2) const;
    qreal getLevelPosition(QRect rc, int level) const;

    static void drawBezier(QPainter &painter, qreal p1x, qreal p1y, qreal cp1x, qreal cp1y, qreal cp2x, qreal cp2y, qreal p2x, qreal p2y);

    bool  m_left_border = false;
    qreal m_level_factor = 0.15;
    int   m_pen_width = 3;
};


#endif // QHISTORYTREEWIDGET_H
