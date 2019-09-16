#include "events.h"

enum class Events
{
    MouseClickPosEvent = QEvent::User + 1,
    SetLabelEvent,
    UpdateViewEvent
};

MouseClickPosEvent::MouseClickPosEvent(double aXval, double aYval, double aLDF, const QString& aXunit, const QString& aYunit, bool aLB)
    : QEvent(static_cast<Type>(Events::MouseClickPosEvent))
    ,  mXvalue(aXval)
    , mYvalue(aYval)
    , mLevelDivFactor(aLDF)
    , mXunit(aXunit)
    , mYunit(aYunit)
    , mLeftButton(aLB)
{

}

QEvent::Type MouseClickPosEvent::id()
{
    return static_cast<QEvent::Type>(Events::MouseClickPosEvent);
}

SetLabelEvent::SetLabelEvent()
    : QEvent(static_cast<Type>(Events::SetLabelEvent))
    , mHideLabels(true)
    , mView(0)
    , mPosition(0,0)
    , mContent(0)
{

}

SetLabelEvent::SetLabelEvent(const QGraphicsView* aView, const QPoint& aPos, double aValue)
    : QEvent(static_cast<Type>(Events::SetLabelEvent))
    , mHideLabels(false)
    , mView(aView)
    , mPosition(aPos)
    , mContent(aValue)
{
    mPosition = mView->mapToParent(mPosition);
}

SetLabelEvent::SetLabelEvent(const QGraphicsView* aView, const QPoint& aPos, const QString& aString)
    : QEvent(static_cast<Type>(Events::SetLabelEvent))
    , mHideLabels(false)
    , mView(aView)
    , mPosition(aPos)
    , mContent(aString)
{
    mPosition = mView->mapToParent(mPosition);
}

QEvent::Type SetLabelEvent::id()
{
    return static_cast<QEvent::Type>(Events::SetLabelEvent);
}

UpdateViewEvent::UpdateViewEvent()
    : QEvent(static_cast<Type>(Events::UpdateViewEvent))
{

}

QEvent::Type UpdateViewEvent::id()
{
    return static_cast<QEvent::Type>(Events::UpdateViewEvent);
}
