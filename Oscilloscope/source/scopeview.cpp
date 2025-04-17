#include "main.h"
#include "scopeview.h"
#include "Channel.h"
#include "PolyLineItem.h"
#include "circlevector.h"
#include "TriggerParameter.h"
#include "matrix.h"
#include "events.h"
#include <cmath>
#include <algorithm>
#include <QMouseEvent>

ScopeView::ScopeView(QWidget*parent) :
    QGraphicsView(parent)
  , mStartTime(0)
  , mTimeStep(0.1)
  , mTimePerDivision(1)
  , mLevelPerDivision(0.1)
  , mTriggerLevel(0)
  , mTimeDivisions(10)
  , mLevelDivisions(10)
  , mLissajousView(false)
  , mCursors(this)
  , mGridCenterLinePen(QColor(Qt::darkGreen))
  , mLevelOrCursorPen(QColor(Qt::darkGreen))
  , mGridLinesPen(QColor(Qt::darkGreen))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QBrush(QColor(Qt::black), Qt::SolidPattern));
    mGridCenterLinePen.setStyle(Qt::SolidLine);
    mLevelOrCursorPen.setStyle(Qt::DashLine);
    mGridLinesPen.setStyle(Qt::DotLine);
    mGridCenterLinePen.setWidth(0);
    mLevelOrCursorPen.setWidth(0);
    mGridLinesPen.setWidth(0);
    mCursors.setID(Cursors::scope_view);
}

ScopeView::~ScopeView()
{

}

bool ScopeView::setLevelPerDivision(double aLevelPerDivision)
{
    bool fchanged = mLevelPerDivision != aLevelPerDivision;
    mLevelPerDivision = aLevelPerDivision;
    return fchanged;
}

void ScopeView::setLissajousView(bool aLissajous)
{
    mLissajousView = aLissajous;
    mCursors.setID(mLissajousView ? Cursors::lissayous_view : Cursors::scope_view);
}

bool   ScopeView::isLissayousView() const
{
    return mLissajousView;
}

void ScopeView::setTimePerDivision(double aTimePerDivision)
{
    mTimePerDivision = aTimePerDivision;
}

void ScopeView::setTriggerStartTime(double aStartTime)
{
    mStartTime = aStartTime;
}

void ScopeView::setTimeStep(double aTimeStep)
{
    mTimeStep = aTimeStep;
}

void ScopeView::setTriggerLevel(double aTriggerLevel)
{
    mTriggerLevel = aTriggerLevel;
}

double ScopeView::getTotalTime()  const
{
    return mTimePerDivision * mTimeDivisions;
}

double ScopeView::getTriggerStartTime()  const
{
    return mStartTime;
}

double ScopeView::getTimeStep()  const
{
    return mTimeStep;
}

double ScopeView::getTriggerLevel()  const
{
    return mTriggerLevel;
}

int ScopeView::getLevelDivisions() const
{
    return mLevelDivisions;
}

int ScopeView::getTimeDivisions() const
{
    return mTimeDivisions;
}
int ScopeView::getDrawnTimeSteps() const
{
    return static_cast<int>(0.5 + getTotalTime() / mTimeStep);
}

void ScopeView::setGridColor(const QColor& aColor)
{
    mGridCenterLinePen.setColor(aColor);
    mLevelOrCursorPen.setColor(aColor);
    mGridLinesPen.setColor(aColor);
}

QColor ScopeView::getGridColor() const
{
    return mGridCenterLinePen.color();
}

Cursors& ScopeView::getCursors()
{
    return mCursors;
}

void ScopeView::drawDivision()
{
    QRectF range;
    fitRangeInView(range);

    if (isLissayousView())
    {
        double fLevel = range.left()+mLevelPerDivision;
        for (int i=1; i<mTimeDivisions; ++i, fLevel += mLevelPerDivision)
        {
            if (fabs(fLevel) < 0.001) scene()->addLine(fLevel, range.top(), fLevel, range.bottom(), mGridCenterLinePen);
            else                      scene()->addLine(fLevel, range.top(), fLevel, range.bottom(), mGridLinesPen);
        }
    }
    else
    {
        double fTime = range.left()+mTimePerDivision;
        for (int i=1; i<mTimeDivisions; ++i, fTime += mTimePerDivision)
        {
            scene()->addLine(fTime, range.top(), fTime, range.bottom(), mGridLinesPen);
        }
        scene()->addLine(mStartTime, range.top(), mStartTime, range.bottom(), mLevelOrCursorPen);

        double fTriggerLevel = mTriggerLevel/mLevelDivisions;
        scene()->addLine(range.left(), fTriggerLevel, range.right(), fTriggerLevel, mLevelOrCursorPen);
    }

    double fLevel = range.bottom()+mLevelPerDivision;
    for (int i=1; i<mLevelDivisions; ++i, fLevel += mLevelPerDivision)
    {
        if (fabs(fLevel) < 0.001) scene()->addLine(range.left(), fLevel, range.right(), fLevel, mGridCenterLinePen);
        else                      scene()->addLine(range.left(), fLevel, range.right(), fLevel, mGridLinesPen);
    }

    if (mCursors.isVerticalVisible())
    {
        scene()->addLine(range.left(), mCursors.getCursor(Cursors::vertical_1), range.right(), mCursors.getCursor(Cursors::vertical_1), mCursors.getPen());
        scene()->addLine(range.left(), mCursors.getCursor(Cursors::vertical_2), range.right(), mCursors.getCursor(Cursors::vertical_2), mCursors.getPen());
    }
    if (mCursors.isHorizontalVisible())
    {
        scene()->addLine(mCursors.getCursor(Cursors::horizontal_1), range.top(), mCursors.getCursor(Cursors::horizontal_1), range.bottom(), mCursors.getPen());
        scene()->addLine(mCursors.getCursor(Cursors::horizontal_2), range.top(), mCursors.getCursor(Cursors::horizontal_2), range.bottom(), mCursors.getPen());
    }
    if (mDrawSelectedRange)
    {
        QColor fColor(mGridLinesPen.color());
        fColor.setAlphaF(0.5);
        scene()->addRect(mDrawSelectedRange.get(), QPen(Qt::NoPen), QBrush(fColor, Qt::SolidPattern));
    }
}

void ScopeView::addPolygon(const std::vector<double>& aPoly, const Channel& aChannel, double aZeroPointCrossing, int aStart)
{
    if (aChannel.mVisible)
    {
        QRectF view = frameRect();
        // calculate stepwidth for 1 pixel
        int fDrawnSteps = getDrawnTimeSteps();
        int fStep = static_cast<int>(static_cast<double>(fDrawnSteps) / view.width());
        if (fStep < 1)
        {
            fStep=1;
        }
        double fTimeStep = fStep * mTimeStep;
        double fPx((1-aZeroPointCrossing)*mTimeStep);
        while (fPx > 0)
        {
            fPx -= mTimeStep;
            --aStart;
            ++fDrawnSteps;
        }

        double fFactor = 1.0 / (static_cast<double>(mLevelDivisions) * aChannel.getLevelPerDivision());
        double fOffset = aChannel.getOffset();

        if (fOffset != 0.0)
        {
            QPen fOffsetPen(Qt::DotLine);
            fOffsetPen.setColor(aChannel.mPen.color());
            fOffsetPen.setWidth(0);
            scene()->addLine(0, fOffset, mTimeDivisions*mTimePerDivision, fOffset, fOffsetPen);
        }

        PolyLineItem*pPolyLineItem = new PolyLineItem;
        QPolygonF& fPolyLine = pPolyLineItem->getPolyLine();
        pPolyLineItem->setPen(&aChannel.mPen);

        const circlevector<double> &fPoly = static_cast<const circlevector<double>&>(aPoly);
        circlevector<double>::const_circlevector_iterator fPy = fPoly.begin(aStart);
        for (int i=0; i<fDrawnSteps; ++i, fPy+=fStep, fPx += fTimeStep)
        {
            fPolyLine.append(QPointF(fPx, fOffset + *fPy * fFactor));
        }

        scene()->addItem(pPolyLineItem);
    }
}

void ScopeView::addLissajous(const std::vector<double>& aPolyX, const Channel& aChannelX,
                             const std::vector<double>& aPolyY, const Channel& aChannelY,
                             int aStart, int aStop)
{
    double fFactorX = 1.0 / (static_cast<double>(mLevelDivisions) * aChannelX.getLevelPerDivision());
    double fOffsetX = aChannelX.getOffset();

    double fFactorY = 1.0 / (static_cast<double>(mTimeDivisions) * aChannelY.getLevelPerDivision());
    double fOffsetY = aChannelY.getOffset();

    if (fOffsetX != 0.0 || fOffsetY != 0.0)
    {
        QPen fOffsetPen(Qt::SolidLine);
        fOffsetPen.setColor(aChannelY.mPen.color());
        fOffsetPen.setWidth(0);
        double fSize = mLevelPerDivision * 0.2;
        scene()->addLine(fOffsetX, fOffsetY-fSize, fOffsetX, fOffsetY+fSize, fOffsetPen);
        scene()->addLine(fOffsetX-fSize, fOffsetY, fOffsetX+fSize, fOffsetY, fOffsetPen);
    }

    PolyLineItem* pPolyLineItem = new PolyLineItem;
    QPolygonF& fPolyLine = pPolyLineItem->getPolyLine();
    pPolyLineItem->setPen(&aChannelY.mPen);

    for (int fIndex = aStart; fIndex<aStop; ++fIndex)
    {
        fPolyLine.append(QPointF(fOffsetX + aPolyX[fIndex] * fFactorX, fOffsetY + aPolyY[fIndex] * fFactorY));
    }
    scene()->addItem(pPolyLineItem);
}

void ScopeView::fitRangeInView(QRectF& range)
{
    QRectF view = frameRect();
    if (mSelectedRange)
    {
        range = mSelectedRange.get();
    }
    else
    {
        if (isLissayousView())
        {
            int fHalfLevelDivisions = mTimeDivisions / 2;
            range.setLeft (-fHalfLevelDivisions*mLevelPerDivision);
            range.setRight( fHalfLevelDivisions*mLevelPerDivision);
        }
        else
        {
            range.setLeft(0);
            range.setRight(mTimeDivisions*mTimePerDivision);
        }

        int fHalfLevelDivisions = mLevelDivisions / 2;
        range.setTop(    fHalfLevelDivisions*mLevelPerDivision);
        range.setBottom(-fHalfLevelDivisions*mLevelPerDivision);
    }

    resetTransform();
    translate(-range.left(), -range.top());
    scale(view.width() / range.width(), view.height() / range.height());
    translate(view.left(), view.top());
    setSceneRect(range);
}

void ScopeView::mouseDoubleClickEvent ( QMouseEvent * event )
{
    QGraphicsView::mouseDoubleClickEvent(event);

    Q_EMIT sendSelectedRange(QRectF(0,0,0,0));
    mDrawSelectedRange.reset();
}

void ScopeView::mousePressEvent ( QMouseEvent * event )
{
    QGraphicsView::mousePressEvent(event);
    QPointF fP = mapToScene(event->pos());

    mButtonsWerePressed = event->buttons();
    if (event->buttons() & Qt::LeftButton)
    {
        mSelectionStartPoint = event->pos();
    }
    if (isLissayousView())
    {
        MouseClickPosEvent fMCE(fP.x(), fP.y(), static_cast<double>(getLevelDivisions()), "V", "V", true);
        Q_EMIT sendEvent(&fMCE);
    }
    else if (event->buttons() & Qt::RightButton) // determine slope
    {
        QGraphicsItem *fItem = scene()->itemAt(fP, transform());
        PolyLineItem* pPolyLineItem = dynamic_cast<PolyLineItem*> (fItem);
        if (pPolyLineItem)
        {
            float fX = fP.x();
            auto& fPoly  = pPolyLineItem->getPolyLine();
            auto  fPoint = std::find_if(fPoly.begin(), fPoly.end(),
                         [fX](const QPointF& aP) { return aP.x() >= fX; });
            if (fPoint != fPoly.end())
            {
                auto   fNext  = fPoint + 1;
                auto   fNext2  = fNext + 1;
                Algorithmics::VectorX::vectorND<qreal> fXval = { fPoint->x(), fNext->x(), fNext2->x() };
                std::vector<qreal> fYval = { fPoint->y(), fNext->y(), fNext2->y()  };
                fXval *= 1000;
                double fSlope = Trigger::calculateSlope(fXval, fYval, fX);
                fSlope *= 1000;
                MouseClickPosEvent fMCE(fP.x(), fSlope, static_cast<double>(getLevelDivisions()), "s", "V/s", false);
                Q_EMIT sendEvent(&fMCE);
            }
        }
    }
    else
    {
        MouseClickPosEvent fMCE(fP.x(), fP.y(), static_cast<double>(getLevelDivisions()), "s", "V", true);
        Q_EMIT sendEvent(&fMCE);
    }
}

void ScopeView::mouseMoveEvent( QMouseEvent * event )
{
    QGraphicsView::mouseMoveEvent(event);
}

void ScopeView::mouseReleaseEvent ( QMouseEvent * event )
{
    QGraphicsView::mouseReleaseEvent(event);
    bool fNotDraggedRange = true;
    if (dragMode() == RubberBandDrag && mSelectionStartPoint)
    {
        QRect fSelection = QRect(mSelectionStartPoint.get(), event->pos()).normalized();
        if (fSelection.width() > 3 && fSelection.height() > 3)
        {
            QRectF fSceneRect(mapToScene(fSelection.topLeft()), mapToScene(fSelection.bottomRight()));
            Q_EMIT sendSelectedRange(fSceneRect);
            mDrawSelectedRange = fSceneRect;
            fNotDraggedRange = false;
        }
        mSelectionStartPoint.reset();
    }
    if (fNotDraggedRange)
    {
        QPointF fP = mapToScene(event->pos());
        if (mCursors.isVerticalVisible())
        {
            if (mButtonsWerePressed == Qt::LeftButton)
            {
                mCursors.setCursor(Cursors::vertical_1, fP.y());
            }

            if (mButtonsWerePressed == Qt::RightButton)
            {
                mCursors.setCursor(Cursors::vertical_2, fP.y());
            }
        }

        if (mCursors.isHorizontalVisible())
        {
            if (mButtonsWerePressed == Qt::LeftButton)
            {
                mCursors.setCursor(Cursors::horizontal_1, fP.x());
            }

            if (mButtonsWerePressed == Qt::RightButton)
            {
                mCursors.setCursor(Cursors::horizontal_2, fP.x());
            }
        }
    }
    UpdateViewEvent fUpdate;
    Q_EMIT sendEvent(&fUpdate);
}

void ScopeView::onSelectRange(const QRectF& fSelectedRange)
{
    if (isVisible())
    {
        if (fSelectedRange.width() && fSelectedRange.height())
        {
            mSelectedRange = fSelectedRange;
        }
        else
        {
            mSelectedRange.reset();
        }
    }
}

void ScopeView::wheelEvent ( QWheelEvent * event )
{
    QGraphicsView::wheelEvent(event);
    QPointF fP = mapToScene(QPoint(event->position().x(), event->position().y()));
    TRACE(Logger::info, "wheelEvent: %f, %f, delta: %d, %d\n", fP.x(), fP.y(), event->angleDelta().x(), event->angleDelta().y());
}
