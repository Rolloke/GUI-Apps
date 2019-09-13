#include "pointf.h"

PointF::PointF(const std::vector<FLOAT>& aV) : x(0), y(0)
{
    if (aV.size() > 1)
    {
        x = aV[0];
        y = aV[1];
    }
}


void PointF::cross_product(const PointF& aP1, const PointF& aP2)
{
    x = aP2.y - aP1.y;
    y = aP1.x - aP2.x;
}


QPoint PointF::getQPoint() const
{
    return QPoint(round(x), round(y));
}

QPointF PointF::getQPointF() const
{
    return QPointF(x, y);
}


QString PointF::toString() const
{
    return QString::number(x) + "," + QString::number(y);
}

bool PointF::fromString(const QString& aText)
{
    bool fOk = false;
    QStringList fPoint = aText.split(',');
    if (fPoint.size() == 2)
    {
        x = fPoint[0].toFloat(&fOk);
        y = fPoint[1].toFloat(&fOk);
    }
    return fOk;
}

FLOAT PointF::distance(const QPointF& a1, const QPointF& a2)
{
    return static_cast<FLOAT>(hypot(a1.x()-a2.x(), a1.y()-a2.y()));
}

PointF PointF::operator - ()
{
    return PointF(-x, -y);
}


// friend function declaration

PointF norm(const PointF& a1)
{
    return a1 / abs(a1);
}

PointF cross_product(const PointF& a1, const PointF& a2)
{
    PointF r;
    r.cross_product(a1, a2);
    return r;
}

FLOAT scalar_product(const PointF& a1, const PointF& a2)
{
    return a1.x * a2.x + a1.y * a2.y;
}

PointF operator + (const PointF& a1, const PointF& a2)
{
    return PointF(a1.x+a2.x, a1.y+a2.y);
}

PointF operator - (const PointF& a1, const PointF& a2)
{
    return PointF(a1.x-a2.x, a1.y-a2.y);
}

PointF operator * (const PointF& aPt, FLOAT aValue)
{
    return PointF(aPt.x * aValue, aPt.y * aValue);
}

PointF operator * (FLOAT aValue, const PointF& aPt)
{
    return aPt * aValue;
}

PointF operator / (const PointF& aPt, FLOAT aValue)
{
    if (aValue != std::numeric_limits<FLOAT>::epsilon())
    {
        return PointF(aPt.x / aValue, aPt.y / aValue);
    }
    else
    {
        return aPt;
    }
}

FLOAT abs(const PointF& aPoint)
{
    return hypot(aPoint.x, aPoint.y);
}

FLOAT  distance(const PointF& a1, const PointF& a2)
{
    return static_cast<FLOAT>(hypot(a1.x-a2.x, a1.y-a2.y));
}

FLOAT  spat(const PointF&a1, const PointF&a2, const PointF&a3)
{
    return scalar_product(a1, cross_product(a2, a3));
}

FLOAT sinus(const PointF& a1, const PointF& a2)
{
    FLOAT fAbs = abs(a1) * abs(a2);
    if (fAbs > std::numeric_limits<FLOAT>::epsilon())
    {
        return abs(cross_product(a1, a2)) / fAbs;
    }
    return 0;
}

FLOAT cosinus(const PointF&a1, const PointF&a2)
{
    FLOAT fAbs = abs(a1) * abs(a2);
    if (fAbs > std::numeric_limits<FLOAT>::epsilon())
    {
        return scalar_product(a1, a2) / fAbs;
    }
    return 0;
}

FLOAT tangens(const PointF&a1, const PointF&a2)
{
    FLOAT fAbs = abs(cross_product(a1, a2));
    if (fAbs > std::numeric_limits<FLOAT>::epsilon())
    {
        return scalar_product(a1, a2) / fAbs;
    }
    return 0;
}
