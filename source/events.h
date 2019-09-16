#ifndef EVENTS_H
#define EVENTS_H

#include <QEvent>
#include <QString>
#include <QVariant>
#include <QGraphicsView>

class MouseClickPosEvent : public QEvent
{
public:
    MouseClickPosEvent(double aXval, double aYval, double aLDF, const QString& aXunit, const QString& aYunit, bool aLB);

    double         getXvalue()         const { return mXvalue; }
    double         getYvalue()         const { return mYvalue; }
    double         getLevelDivFactor() const { return mLevelDivFactor; }
    const QString& getXunit()          const { return mXunit; }
    const QString& getYunit()          const { return mYunit; }
    bool           isLeftButton()      const { return mLeftButton; }
    static Type    id();

private:
    double mXvalue;
    double mYvalue;
    double mLevelDivFactor;
    QString mXunit;
    QString mYunit;
    bool   mLeftButton;
};

class SetLabelEvent : public QEvent
{
public:
    SetLabelEvent();
    SetLabelEvent(const QGraphicsView* aView, const QPoint& aPos, double aValue);
    SetLabelEvent(const QGraphicsView* aView, const QPoint& aPos, const QString& aString);

    bool                 hideLabels() const { return mHideLabels; }
    const QGraphicsView* getView()    const { return mView; }
    const QPoint&        getPos()     const { return mPosition; }
    const QVariant&      getVariant() const { return mContent; }
    static Type          id();

private:
    bool mHideLabels;
    const QGraphicsView* mView;
    QPoint mPosition;
    QVariant mContent;
};

class UpdateViewEvent : public QEvent
{
public:
    UpdateViewEvent();
    static Type          id();
};

#endif // EVENTS_H
