#include "main.h"
#include "fft_view.h"
#include "Channel.h"
#include "PolyLineItem.h"
#include "FFT.h"
#include "Average.h"
#include "sse_helper.h"
#include "events.h"

#include <cmath>
#include <algorithm>
#include <QMouseEvent>

FFT_View::FFT_View(QWidget*parent) :
    QGraphicsView(parent)
  , mSampleFrequency(1)
  , mFrequencyStep(0.1)
  , mFrequencySteps(1)
  , mLowerFrequency(5)
  , mLevelPerDivision(0.1)
  , mLevelDivisions(10)
  , mLogarithmic(false)
  , mFloatingAverageValues(0)
  , mFilterFunction(RectFilter)
  , mFFT_ValueFunction(Amplitude)
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

    mCursors.setID(Cursors::fft_view);
}

FFT_View::~FFT_View()
{

}

bool FFT_View::setLevelPerDivision(double aLevelPerDivision)
{
    aLevelPerDivision *= mCursors.getCursor(Cursors::verticalZoom);
    bool fchanged = mLevelPerDivision != aLevelPerDivision;
    mLevelPerDivision = aLevelPerDivision;
    return fchanged;
}


void FFT_View::setFFT_Parameter(double aSampleFrequency, int aFFT_Size)
{
    mSampleFrequency = aSampleFrequency;
    mFrequencyStep = mSampleFrequency / (double) aFFT_Size;
    mFrequencySteps = aFFT_Size / 2;
    updateFrequencyDivision();
}

void FFT_View::setLogarithmic(bool aLog)
{
    mLogarithmic = aLog;
    updateFrequencyDivision();
}

void FFT_View::setFilterFunction(int aFunction)
{
    mFilterFunction = static_cast<FFT_View::eFilter>(aFunction);
}

void FFT_View::setFFT_ValueFunction(int aFunction)
{
    mFFT_ValueFunction = static_cast<FFT_View::eFFT_Value>(aFunction);
}

void FFT_View::setFFT_FloatingAvgValues(int aValues)
{
    mFloatingAverageValues = aValues >= 0 ? aValues : 0;
}

double FFT_View::getSampleFrequency() const
{
    return mSampleFrequency;
}

double FFT_View::getFrequencyStep() const
{
    return mFrequencyStep;
}

int FFT_View::getLevelDivisions() const
{
    return mLevelDivisions;
}

void FFT_View::setGridColor(const QColor& aColor)
{
    mGridCenterLinePen.setColor(aColor);
    mLevelOrCursorPen.setColor(aColor);
    mGridLinesPen.setColor(aColor);
}

Cursors& FFT_View::getCursors()
{
    return mCursors;
}

FFT_View::eFilter FFT_View::getFilterFunction() const
{
    return mFilterFunction;
}

FFT_View::eFFT_Value FFT_View::getFFT_ValueFunction() const
{
    return mFFT_ValueFunction;
}

QColor FFT_View::getGridColor() const
{
    return mGridCenterLinePen.color();
}


void FFT_View::updateFrequencyDivision()
{
    QRectF range;
    fitRangeInView(range);
    QPointF fPos = range.bottomLeft();

    double fUpperFrequency = mFrequencyStep * mFrequencySteps;
    mFrequencyAxis.clear();

    SetLabelEvent fHideEvent;
    Q_EMIT sendEvent(&fHideEvent);

    if (mLogarithmic)
    {
        double fFrequency, fStep;
        int fSize = (int) ((log(fUpperFrequency) - 1.0) * mLowerFrequency);
        mFrequencyAxis.reserve(fSize);
        fStep = fFrequency = 1;
        for (int i=0, j=1; fFrequency < fUpperFrequency && i < fSize; ++i, ++j)
        {
            TRACE(Logger::trace, "Frequency: %f: log: %f\n", fFrequency, log10(fFrequency));
            mFrequencyAxis.push_back(log10(fFrequency));
            fFrequency += fStep;
            if (j == 9)
            {
                fPos.setX(*mFrequencyAxis.rbegin());
                QPoint fP = mapFromScene(fPos);
                fP.setY(rect().bottom());
                SetLabelEvent fLabelEvent(this, fP, fFrequency);
                Q_EMIT sendEvent(&fLabelEvent);

                fStep *= 10;
                j=0;
            }
        }
        mFrequency_X_Values.resize(mFrequencySteps);
        fFrequency = mFrequencyStep;
        for (int i=0; i<mFrequencySteps; ++i)
        {
            mFrequency_X_Values[i] = log10(fFrequency);
            TRACE(Logger::trace, "Frequency: %f: log: %f\n", fFrequency,  mFrequency_X_Values[i]);
            fFrequency += mFrequencyStep;
        }
    }
    else
    {
        double fRange       = fUpperFrequency;
        double fLogRange    = log10(fRange);
        int    fLogRangeInt = (int) ceil(fLogRange);
        double fStepSize    = pow(10.0, (double) fLogRangeInt - 2) * floor(pow(10.0, fLogRange - floor(fLogRange)));
        int    fSize        = (int) (fRange / fStepSize);
        double fFrequency   = fStepSize;
        mFrequencyAxis.reserve(fSize);
        for (int i = 0; i < fSize && fFrequency < fUpperFrequency; i++)
        {
            TRACE(Logger::trace, "Frequency: %f\n", fFrequency);
            mFrequencyAxis.push_back(fFrequency);
            fPos.setX(fFrequency);

            QPoint fP = mapFromScene(fPos);
            fP.setY(rect().bottom());
            SetLabelEvent fLabelEvent(this, fP, fFrequency);
            Q_EMIT sendEvent(&fLabelEvent);

            fFrequency += fStepSize;
        }
    }
    SetLabelEvent fLabelEvent(this, rect().bottomRight(), "Hz");
    Q_EMIT sendEvent(&fLabelEvent);


}

void FFT_View::drawDivision()
{
    QRectF range;
    fitRangeInView(range);

    for (double fFrequency : mFrequencyAxis)
    {
        scene()->addLine(fFrequency, range.top(), fFrequency, range.bottom(), mGridLinesPen);
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
        double fX1 = mCursors.getCursor(Cursors::horizontal_1);
        double fX2 = mCursors.getCursor(Cursors::horizontal_2);
        if (mLogarithmic)
        {
            fX1 = log10(fX1);
            fX2 = log10(fX2);
        }
        scene()->addLine(fX1, range.top(), fX1, range.bottom(), mCursors.getPen());
        scene()->addLine(fX2, range.top(), fX2, range.bottom(), mCursors.getPen());
    }
}

void FFT_View::addPolygon(const std::vector<double>& aValues, const Channel& aChannel, int aStart)
{
    if (aChannel.mVisible)
    {
        std::vector<double> fFFTArray;
        int fFFTindex, fValueIndex, fFFTvalues = mFrequencySteps*2;
        int fSizeValues = aValues.size();
        fFFTArray.resize(fFFTvalues);

        if (aStart < 0)
        {
            aStart = aValues.size() + aStart;
        }

        double (*fFilter)(int, int) = 0;
        switch (mFilterFunction)
        {
            case RectFilter:    fFilter = &calcRectFilter;    break;
            case HammingFilter: fFilter = &calcHammingFilter; break;
            case HanningFilter: fFilter = &calcHanningFilter; break;
            case CosineFilter:  fFilter = &calcCosineFilter;  break;
            case LanczosFilter: fFilter = &calcLanczosFilter; break;
            case GaussFilter:   fFilter = &calcGaussFilter;   break;
            default:            fFilter = &calcRectFilter;    break;
        }

        for (fFFTindex = 0, fValueIndex = aStart; fFFTindex < fFFTvalues; ++fFFTindex, ++fValueIndex)
        {
            if (fValueIndex >= fSizeValues)
            {
                fValueIndex = 0;
            }
            fFFTArray[fFFTindex] = fFilter(fFFTvalues, fFFTindex) * aValues[fValueIndex];
        }
        int fFFTOrder = 0;
        while (fFFTvalues >>= 1) ++fFFTOrder;

        calcFFT(0, fFFTOrder, &fFFTArray[0]);

        std::vector<double> fY_Values;
        std::vector<double> fY_AvgValues;
        fY_Values.resize(mFrequencySteps);

        void (*calcFFT_Value)(const std::complex<double>*, std::vector<double>&) = 0;
        switch (mFFT_ValueFunction)
        {
            case Amplitude: calcFFT_Value = &calcAmplitude; break;
            case Phase:     calcFFT_Value = &calcPhase;     break;
            case Real:      calcFFT_Value = &calcReal;      break;
            case Imaginary: calcFFT_Value = &calcImaginary; break;
            default:        calcFFT_Value = &calcAmplitude; break;
        }

        calcFFT_Value(reinterpret_cast<std::complex<double>*>(&fFFTArray[0]), fY_Values);

        double fFactor = 1.0 / (static_cast<double>(mLevelDivisions) * aChannel.getLevelPerDivision() * mCursors.getCursor(Cursors::verticalZoom));
        double fOffset = aChannel.getOffset();

        if (fOffset != 0.0)
        {
            QPen fOffsetPen(Qt::DotLine);
            fOffsetPen.setColor(aChannel.mPen.color());
            fOffsetPen.setWidth(0);
            scene()->addLine(0, fOffset, mFrequencyStep*mFrequencySteps, fOffset, fOffsetPen);
        }

        PolyLineItem*pPolyLineItem = new PolyLineItem;
        QPolygonF& fPolyLine = pPolyLineItem->getPolyLine();
        pPolyLineItem->setPen(&aChannel.mPen);

        int fBoundary      = mFloatingAverageValues / 2;
        int fUpper         = mFrequencySteps - fBoundary;
        PolyLineItem*pPolyLineItemAvg(0);
        QPolygonF* fPolyLineAvg(0);
        if (mFloatingAverageValues)
        {
            pPolyLineItemAvg = new PolyLineItem;
            fPolyLineAvg = &pPolyLineItemAvg->getPolyLine();
            pPolyLineItemAvg->setPen(&aChannel.mPen);
            calculateFloatingAverage(fY_Values, mFloatingAverageValues, fY_AvgValues);
        }


        if (mLogarithmic)
        {
            fPolyLine.append(QPointF(0,fOffset));
            if (pPolyLineItemAvg)
            {
                fPolyLineAvg->append(QPointF(0,fOffset));
            }
            for (fFFTindex=0; fFFTindex<mFrequencySteps; ++fFFTindex)
            {
                fPolyLine.append(QPointF(mFrequency_X_Values[fFFTindex], fOffset));
                fPolyLine.append(QPointF(mFrequency_X_Values[fFFTindex], fOffset + fY_Values[fFFTindex] * fFactor));
                fPolyLine.append(QPointF(mFrequency_X_Values[fFFTindex], fOffset));
                if (pPolyLineItemAvg && fFFTindex >= fBoundary && fFFTindex < fUpper)
                {
                    fPolyLineAvg->append(QPointF(mFrequency_X_Values[fFFTindex-fBoundary], fOffset + fY_AvgValues[fFFTindex-fBoundary] * fFactor));
                }
            }
        }
        else
        {
            double fX_Value = mFrequencyStep;
            fPolyLine.append(QPointF(0,fOffset));
            if (pPolyLineItemAvg)
            {
                fPolyLineAvg->append(QPointF(0,fOffset));
            }
            for (fFFTindex=0; fFFTindex<mFrequencySteps; ++fFFTindex)
            {
                fPolyLine.append(QPointF(fX_Value, fOffset + fY_Values[fFFTindex] * fFactor));
                fX_Value += mFrequencyStep;
                if (pPolyLineItemAvg && fFFTindex >= fBoundary && fFFTindex < fUpper)
                {
                    fPolyLineAvg->append(QPointF(fX_Value, fOffset + fY_AvgValues[fFFTindex-fBoundary] * fFactor));
                }
            }
        }
        scene()->addItem(pPolyLineItem);
        if (pPolyLineItemAvg)
        {
            scene()->addItem(pPolyLineItemAvg);
        }
    }
}

void FFT_View::fitRangeInView(QRectF& range)
{
    QRectF view = frameRect();

    double fUpperFrequency = mFrequencyStep * mFrequencySteps;
    if (mLogarithmic)
    {
        range.setLeft(log10(mLowerFrequency));
        range.setRight(log10(fUpperFrequency));
    }
    else
    {
        range.setLeft(mLowerFrequency);
        range.setRight(fUpperFrequency);
    }

    int fHalfLevelDivisions = mLevelDivisions / 2;
    range.setTop(fHalfLevelDivisions*mLevelPerDivision);
    range.setBottom(-fHalfLevelDivisions*mLevelPerDivision);

    resetTransform();
    translate(-range.left(), -range.top());
    scale(view.width()/range.width(), view.height()/range.height());
    translate(view.left(), view.top());
    setSceneRect(range);
}

void FFT_View::mousePressEvent ( QMouseEvent * event )
{
    QGraphicsView::mousePressEvent(event);

    QPointF fP = mapToScene(event->pos());
    double fX = fP.x();
    if (mLogarithmic)
    {
        fX = pow(10, fP.x());
    }

    if (event->buttons() == Qt::LeftButton)
    {
        MouseClickPosEvent fMCE(fX, fP.y(), static_cast<double>(getLevelDivisions()), "Hz", "V", true);
        Q_EMIT sendEvent(&fMCE);
    }

    if (mCursors.isVerticalVisible())
    {
        if (event->buttons() == Qt::LeftButton)
        {
            mCursors.setCursor(Cursors::vertical_1, fP.y());
        }

        if (event->buttons() == Qt::RightButton)
        {
            mCursors.setCursor(Cursors::vertical_2, fP.y());
        }
    }

    if (mCursors.isHorizontalVisible())
    {
        if (event->buttons() == Qt::LeftButton)
        {
            mCursors.setCursor(Cursors::horizontal_1, fX);
        }

        if (event->buttons() == Qt::RightButton)
        {
            mCursors.setCursor(Cursors::horizontal_2, fX);
        }
    }
}

void FFT_View::mouseMoveEvent( QMouseEvent * event )
{
    QGraphicsView::mouseMoveEvent(event);
}

void FFT_View::mouseReleaseEvent ( QMouseEvent * event )
{
    QGraphicsView::mouseReleaseEvent(event);
    UpdateViewEvent fUpdate;
    Q_EMIT sendEvent(&fUpdate);
}

void FFT_View::resizeEvent(QResizeEvent* event)
{
    if (event->oldSize() != event->size())
    {
        updateFrequencyDivision();
    }
}
