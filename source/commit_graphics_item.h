#ifndef COMMIT_GRAPHICS_ITEM_H
#define COMMIT_GRAPHICS_ITEM_H
#include <QGraphicsItem>

class commit_graphis_item : public QGraphicsItem
{
 public:
     commit_graphis_item();
     virtual ~commit_graphis_item();

     void setText(const QStringList& items) { m_items = items; }
     void setPen(const QPen* aPen)          { m_pen   = aPen;  }
     void setFont(const QFont* aPen)        { m_font  = aPen;  }
     void offsetPos(const QPointF& aOffset) { m_position += aOffset;  }

protected:
     QRectF boundingRect() const;
     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
     const QPen*     m_pen;
     const QFont*    m_font;
     QStringList m_items;
     QPointF m_position;
     QRectF  m_bounding_rect;
};

#endif // COMMIT_GRAPHICS_ITEM_H
