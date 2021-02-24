#include "cursors.h"

QPen Cursors::mPen(Qt::DashLine);

// cursor Kommunikationsklasse
Cursors::Cursors(QObject *parent) : QObject(parent)
  , mVerticalVisible(false)
  , mHorizontalVisible(false)
  , mID(unknown_view)
{
    mCursor.resize(CursorsSize, 0);
    mCursor[verticalZoom]   = 1;
    mCursor[horizontalZoom] = 1;
}

void Cursors::setCursor(eCursor aCursor, double aValue, bool aUpdate)
{
    mCursor[aCursor] = aValue;
    if (aUpdate)
    {
        Q_EMIT update(aCursor, mID);
    }
}

double Cursors::getCursor(eCursor aCursor) const
{
    return mCursor[aCursor];
}

void Cursors::setVerticalVisible(bool aVisible)
{
    mVerticalVisible = aVisible;
    Q_EMIT update(vertical_1, mID);
}

bool Cursors::isVerticalVisible() const
{
    return mVerticalVisible;
}

void Cursors::setHorizontalVisible(bool aVisible)
{
    mHorizontalVisible = aVisible;
    Q_EMIT update(horizontal_1, mID);
}

bool Cursors::isHorizontalVisible() const
{
    return mHorizontalVisible;
}

QPen& Cursors::getPen()
{
    return mPen;
}

void Cursors::setID(Cursors::eView aID)
{
    mID = aID;
}

Cursors::eView Cursors::getID() const
{
    return mID;
}
