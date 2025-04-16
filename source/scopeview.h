#ifndef SCOPEVIEW_H
#define SCOPEVIEW_H

#include "Channel.h"
#include "cursors.h"
#include <boost/optional.hpp>

#include <QGraphicsView>
#include <QVector>

class ScopeView : public QGraphicsView
{
    Q_OBJECT
public:
    ScopeView(QWidget *parent = 0);
    ~ScopeView();

    bool setLevelPerDivision(double aLevelPerDivision);
    void setTimePerDivision(double aTimePerDivision);
    void setLissajousView(bool aLissajous);
    void setTimeStep(double aTimeStep);
    void setTriggerStartTime(double aStartTime);
    void setTriggerLevel(double aTriggerLevel);
    void setGridColor(const QColor& aColor);

    double getTotalTime() const;
    double getTimeStep() const;
    double getTriggerStartTime() const;
    double getTriggerLevel() const;
    int    getLevelDivisions() const;
    int    getTimeDivisions() const;
    int    getDrawnTimeSteps() const;
    QColor getGridColor() const;
    Cursors& getCursors();
    bool   isLissayousView() const;


    void drawDivision();
    void addPolygon(const std::vector<double>& aPoly, const Channel& aChannel, double fZeroPointCrossing, int aStart=0);
    void addLissajous(const std::vector<double>& aPolyX, const Channel& aChannelX,
                      const std::vector<double>& aPolyY, const Channel& aChannelY,
                      int aStart, int aStop);
    void fitRangeInView(QRectF& range);

protected:
    virtual void	mouseDoubleClickEvent ( QMouseEvent * event );
    virtual void	mousePressEvent ( QMouseEvent * event );
    virtual void	mouseMoveEvent( QMouseEvent * event );
    virtual void	mouseReleaseEvent ( QMouseEvent * event );
    virtual void	wheelEvent ( QWheelEvent * event );

Q_SIGNALS:
    void sendSelectedRange(const QRectF& fSelectedRange);
    void sendEvent(const QEvent*);

public Q_SLOTS:
    void onSelectRange(const QRectF& fSelectedRange);

private:
    double mStartTime;
    double mTimeStep;
    double mTimePerDivision;
    double mLevelPerDivision;
    double mTriggerLevel;
    int    mTimeDivisions;
    int    mLevelDivisions;
    bool   mLissajousView;
    Cursors mCursors;

    boost::optional<QPoint> mSelectionStartPoint;
    Qt::MouseButtons        mButtonsWerePressed;
    boost::optional<QRectF> mSelectedRange;
    boost::optional<QRectF> mDrawSelectedRange;

    QPen mGridCenterLinePen;
    QPen mLevelOrCursorPen;
    QPen mGridLinesPen;
};

#endif // SCOPEVIEW_H
