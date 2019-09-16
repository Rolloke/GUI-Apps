#ifndef FFTVIEW_H
#define FFTVIEW_H

#include "Channel.h"
#include "cursors.h"

#include <QGraphicsView>
#include <QVector>
#include <QString>
#include <vector>

class FFT_View : public QGraphicsView
{
    Q_OBJECT
public:
    enum eFilter
    {
        RectFilter,
        HammingFilter,
        HanningFilter,
        CosineFilter,
        GaussFilter,
        LanczosFilter,
        InvalidFilter=-1
    };
    enum eFFT_Value
    {
        Amplitude = 0,
        Phase     = 1,
        Real      = 2,
        Imaginary = 3,
        InvalidValue =-1
    };

    FFT_View(QWidget *parent = 0);
    ~FFT_View();

    bool setLevelPerDivision(double aLevelPerDivision);
    void setFFT_Parameter(double aSampleFrequency, int aFFT_Size);
    void setLogarithmic(bool aLog);
    void setGridColor(const QColor& aColor);
    void setFont(const QFont& aFont) { mFont = aFont; }

    void updateFrequencyDivision();

    double getSampleFrequency() const;
    double getFrequencyStep() const;
    int    getLevelDivisions() const;
    Cursors&    getCursors() ;
    eFilter     getFilterFunction() const;
    eFFT_Value  getFFT_ValueFunction() const;
    QColor getGridColor() const;

    void drawDivision();
    void addPolygon(const std::vector<double>& aPoly, const Channel& aChannel, int aStart=0);
    void fitRangeInView(QRectF& range);

protected:
    virtual void	mousePressEvent ( QMouseEvent * event );
    virtual void	mouseMoveEvent( QMouseEvent * event );
    virtual void	mouseReleaseEvent ( QMouseEvent * event );

Q_SIGNALS:
    void sendEvent(const QEvent*);

public Q_SLOTS:
    void setFilterFunction(int aFunction);
    void setFFT_ValueFunction(int aFunction);
    void setFFT_FloatingAvgValues(int aValues);

private:

    double mSampleFrequency;
    double mFrequencyStep;
    int    mFrequencySteps;
    double mLowerFrequency;
    double mLevelPerDivision;
    int    mLevelDivisions;
    bool   mLogarithmic;
    int    mFloatingAverageValues;
    eFilter mFilterFunction;
    eFFT_Value mFFT_ValueFunction;
    std::vector<double> mFrequencyAxis;
    std::vector<double> mFrequency_X_Values;
    Cursors mCursors;

    QPen mGridCenterLinePen;
    QPen mLevelOrCursorPen;
    QPen mGridLinesPen;
    QFont mFont;
};

#endif // FFTVIEW_H
