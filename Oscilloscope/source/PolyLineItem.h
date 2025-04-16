#ifndef POLYLINEITEM_H
#define POLYLINEITEM_H

#include <QGraphicsItem>
#include <QPolygonF>
#include <QPen>

class PolyLineItem : public QGraphicsItem
{
 public:
     PolyLineItem();
     virtual ~PolyLineItem();

     void setPen(const QPen* aPen) { mpPen = aPen; }
     QPolygonF& getPolyLine() { return mPolyLine; }

protected:
     QRectF boundingRect() const;
     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
     QPolygonF mPolyLine;
     const QPen*     mpPen;
 };

class TextItem : public QGraphicsItem
{
 public:
     TextItem();
     TextItem(const QString& aText, const QPointF& aPos, const QRectF& aBoundigRect);
     virtual ~TextItem();

     void setPen(const QPen* aPen) { mpPen = aPen; }
     void setFont(const QFont* aFont) { mpFont = aFont; }
     void setText(const QString& aText, const QPointF& aPos)
     { mText = aText; mPosition = aPos; }

protected:
     QRectF boundingRect() const;
     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
     const QFont*    mpFont;
     const QPen*     mpPen;
     QString mText;
     QPointF mPosition;
     QRectF  mBoundingRect;
 };

#endif // POLYLINEITEM_H
