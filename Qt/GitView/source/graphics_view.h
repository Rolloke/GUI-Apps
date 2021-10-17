#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>

class ActionList;

class graphics_view : public QGraphicsView
{
    Q_OBJECT
public:
    explicit graphics_view(QWidget *parent = 0);

    void addItem2graphicsView(QGraphicsItem*item);
    void on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    void clear();
    bool render_file(const QString& file_name, const QString& file_extension);


signals:

public slots:
    void zoomIn();
    void zoomOut();
    void fit_inView(bool);

private:
    double mGraphicsScale;
    bool   mFitInView;
};

class commit_graphis_item : public QGraphicsItem
{
 public:
     commit_graphis_item();
     virtual ~commit_graphis_item();

     void setPen(const QPen* aPen) { m_pen = aPen; }
     void setFont(const QFont* aPen) { m_font= aPen; }

protected:
     QRectF boundingRect() const;
     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
     const QPen*     m_pen;
     const QFont*    m_font;
     QString m_text;
     QPointF m_position;
     QRectF  m_bounding_rect;
};

#endif // GRAPHICS_VIEW_H
