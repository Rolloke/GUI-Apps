#include "PolyLineItem.h"

#include<QPainter>

PolyLineItem::PolyLineItem() :
    mpPen(0)
{

}

PolyLineItem::~PolyLineItem()
{

}

QRectF PolyLineItem::boundingRect() const
{
    return mPolyLine.boundingRect();
}

void PolyLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    if (mpPen) painter->setPen(*mpPen);
    painter->drawPolyline(mPolyLine);
}

TextItem::TextItem()
: mpFont(0)
, mpPen(0)
{

}

TextItem::TextItem(const QString &aText, const QPointF &aPos, const QRectF& aBoundigRect)
: mpFont(0)
, mpPen(0)
, mText(aText)
, mPosition(aPos)
, mBoundingRect(aBoundigRect)
{

}

TextItem::~TextItem()
{

}

QRectF TextItem::boundingRect() const
{
    return mBoundingRect;
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    if (mpFont) painter->setFont(*mpFont);
    if (mpPen)  painter->setPen(*mpPen);
    painter->drawText(mPosition, mText);
}
