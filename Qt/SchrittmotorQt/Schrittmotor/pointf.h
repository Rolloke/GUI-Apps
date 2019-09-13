#ifndef POINTF_H
#define POINTF_H

#include "defines.h"
#include <QString>


class PointF
{
public:
    PointF() : x(0), y(0) {}
    PointF(FLOAT ax, FLOAT ay) : x(ax), y(ay) {}
    PointF(const QPointF& aPt) : x(aPt.x()), y(aPt.y()) {}
    PointF(const std::vector<FLOAT>& aV);

    FLOAT x;
    FLOAT y;

    void   cross_product(const PointF& aP1, const PointF& aP2);

    // interfaces to Qt
    static FLOAT distance(const QPointF& a1, const QPointF& a2);
    QPoint  getQPoint() const;
    QPointF getQPointF() const;
    QString toString() const;
    bool fromString(const QString& aText);

    // friend functions
    friend PointF norm(const PointF& a1);
    friend PointF cross_product(const PointF& a1, const PointF& a2);
    friend FLOAT  scalar_product(const PointF& a1, const PointF& a2);
    friend FLOAT  abs(const PointF& aPoint);
    friend FLOAT  distance(const PointF& a1, const PointF& a2);
    friend FLOAT  spat(const PointF&a1, const PointF&a2, const PointF&a3);
    friend FLOAT  sinus(const PointF& a1, const PointF& a2);
    friend FLOAT  cosinus(const PointF&a1, const PointF&a2);
    friend FLOAT  tangens(const PointF&a1, const PointF&a2);

    // operators
    PointF operator - ();
    friend PointF operator + (const PointF& a1, const PointF& a2);
    friend PointF operator - (const PointF& a1, const PointF& a2);
    friend PointF operator * (const PointF& aPt, FLOAT aValue);
    friend PointF operator * (FLOAT aValue, const PointF& aPt);
    friend PointF operator / (const PointF& aPt, FLOAT aValue);

};




#endif // POINTF_H
