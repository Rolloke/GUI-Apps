#pragma once

#include <list>
#include <map>
#include <cmath>
#include <stdio.h>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QString>

#include "boost/function.hpp"
#include "boost/optional.hpp"

#include "defines.h"
#include "matrix.h"
#include "pointf.h"

int round(float f);
//int round(double f);

class QDomDocument;
class QDomElement;
class QDomNode;


struct FD
{
    enum font_definition { SymbolSet=1, FontSpacing=2, Pitch=3, Height=4, Posture=5, StrokeWeight=6, Typeface=7, StretchFactor=8, Upright=0, Italic=1, AlternateItalic=2 };
};

class CPlotterDC : public QGraphicsView
{
public:
    class DrawCommand
    {
    public:
        enum eCommand {Unknown=-1, MoveTo, LineTo, Circle, PolyLine };
        enum eFlag    {Visible=1, Selected=2, DrillPoint=4, PolygoneStart=8, LastPoint=-1};
        DrawCommand();
        DrawCommand(eCommand aType);
        DrawCommand(const std::vector<FLOAT>&aPt, eCommand aType);
        DrawCommand(std::vector<QPoint>::const_iterator aBegin, std::vector<QPoint>::const_iterator aEnd, eCommand aType);
        DrawCommand(FLOAT aX, FLOAT aY, eCommand aType);
        QPoint getPoint( int ap) const;
        QPointF getPointF( int ap) const;
        QPointF getCenterPoint(QRectF *pRect=0) const;
        size_t getPoints() const { return mPosition.size(); }
        void   setVisible(bool aV);
        bool   isVisible() const { return (mFlag&Visible) != 0; }
        void   setSelected(bool aV);
        bool   isSelected() const { return (mFlag&Selected) != 0; }
        void   setDrillPoint(bool aV);
        bool   isDrillPoint() const { return (mFlag&DrillPoint) != 0; }
        void   setPolygoneStart(bool aV);
        bool   isPolygoneStart() const { return (mFlag&PolygoneStart) != 0; }
        bool   appendPolyLine(DrawCommand& aPolyline);
        void   reverse();
        bool   isReverseable();

        void   writeXml(QDomDocument& aDoc, QDomElement& aParent) const;
        void   readXml(const QDomNode& aItem);

        QString  name() const;
        static QString  name(eCommand);
        eCommand mame2type(const QString& aName) const;
        QString  info() const;

        bool   operator<(const DrawCommand& dc) const;
	    
        std::vector<PointF>  mPosition;     /// vector of point positions for the draw commands
        eCommand mType;                     /// type of draw command 
        unsigned long mFlag;                /// additional information about properties
        std::vector<int> mMoveToPoint;      /// position of a move-to-point within a polyline
        boost::optional<unsigned long> mCombineID;

        /// parameters for sorting draw commands by position
        static FLOAT mHorizontalSize;       /// horizontal size 
        static FLOAT mVerticalStep;         /// vertical step
    };

public:
    enum param { Progress, CmdIndex, SelectCmdIndex };

    typedef std::vector<DrawCommand> DrawCommandList;
    boost::function< void (float, float) > mSendPos;

    CPlotterDC(QWidget *parent = 0);
    ~CPlotterDC();

    const DrawCommandList& getDrawCommands() { return mDrawCommands; }
    void  clearDrawCommands();
    void  setAutoDetermineDrillpoints(bool aAuto);
    void  addDrawCommand(DrawCommand& aCmd);

    const  rke::matrix<FLOAT>& getTransformation() { return mTransformation; }
    void   setTransformation(const rke::matrix<FLOAT>&aTrans) { mTransformation = aTrans; }
    void   setRotation(FLOAT anAngle);
    void   setDeviderForRedSquare(FLOAT aDiv) { mDeviderForRedSquare = aDiv; }
    void   reset_transformation();
    void   rotate(FLOAT aAngle);
    void   scale(const std::vector<FLOAT>& aScale);
    void   scale(FLOAT x, FLOAT y);
    void   translate(const std::vector<FLOAT>& aScale);
    void   translate(FLOAT x, FLOAT y);
    void   transform_point(FLOAT& x, FLOAT& y) const;
    PointF transform_point(const PointF& aPt) const;
    void   re_transform_point(FLOAT& x, FLOAT& y);

    void   setWindowExtension(int cx, int cy);
    int    setClipRect(QRect* rcClip);
    int    getSelectedItem();
    void   selectItem(int aSel);
    void   selectItem(const QPointF& ptSelect);
    void   toggleAllDrillPointStates();
    void   setSelectedItemAsFirst();
    void   setSelectedtDrillPointState(bool aDrill);
    void   setDrawingWidth(FLOAT aWidth) { mDrawingWidth = aWidth; }
    void   setPenWidth(FLOAT aWidth) { mPenWidth = aWidth*SCALE_FACTOR; }
    void   updatePenWidth(CPlotterDC::DrawCommand& aDrawCmd);
    bool   hasVariablePenWidth() { return mDrawingWidth > 0 && mPenWidth > 0; }

    void   updateWindowMapping();
    void   concatenateMoveToAndPolyLine();
    void   combineDrawCommands(unsigned long aDrillPointID);
    void   concatenateDrawCommands();
    void   sortDrawCommands();
    void   drawRectangle(const QRectF& aRect);
    bool   redraw();
    QSize  zoomIn();
    QSize  zoomOut();

    void   setCombineID(boost::optional<unsigned long> aID);
    bool   getStartPosition(FLOAT& x, FLOAT& y) ;
    void   getCurrentPoint(FLOAT& x, FLOAT& y) const;
    PointF getCurrentPoint() const;
    void   setCurrentPoint(const PointF& fPt);
    FLOAT  getPenWidth() const { return mPenWidth/SCALE_FACTOR; }

    QPointF GetCurrentPathDCPosition() const;
    bool   MoveTo(const PointF& aPt);
    bool   MoveTo(FLOAT x, FLOAT y);
    bool   LineTo(const PointF& aPt);
    bool   LineTo(FLOAT x, FLOAT y);
    bool   CurveTo(const std::vector<FLOAT>& aPoints);
    bool   Circle(FLOAT ax, FLOAT ay, FLOAT aradius);
    bool   ArcTo(const QRectF& aRect, FLOAT aStartAngle, FLOAT aAngleSpan);
    bool   ArcTo(FLOAT aLeft, FLOAT aTop, FLOAT aRight, FLOAT aBottom, FLOAT aStartAngle, FLOAT aAngleSpan);
    bool   ArcTo(FLOAT ax, FLOAT ay, FLOAT adeg, bool aXisRadius=false);
    bool   addElipse(const QRectF& aRect);
    bool   addElipse(FLOAT aLeft, FLOAT aTop, FLOAT aRight, FLOAT aBottom);
    bool   Wedge(FLOAT aRadius, FLOAT aStartAngle, FLOAT aArcAngle);
    bool   ExtractPath(QPainterPath& aDc, DrawCommand& apPrevious);
    bool   StrokePath(bool aFill=false);
    bool   RectFill(FLOAT aX, FLOAT aY, FLOAT aWidth, FLOAT aHeight);
    bool   TextOut(std::string aString);
    void   setFontName(const std::string& aName);
    void   setCharSize(FLOAT aWidth, FLOAT aHeight);
    void   setFontDefinition(const std::map<int, FLOAT>& aFD, bool bAlternateFont);
    void   selectFontDefinition(bool bAlternateFont);

    bool   BeginPath();
    bool   EndPath();
    bool   isPathActive();
    int    SaveDC();
    void   RestoreDC(int n);

    void   SetLineJoin(int aLineJoin);
    void   SetLineCap(int aLineCap);

protected:
    virtual void	mousePressEvent ( QMouseEvent * event );
    virtual void	mouseMoveEvent( QMouseEvent * event );
    virtual void	mouseReleaseEvent ( QMouseEvent * event );
    virtual void    wheelEvent(QWheelEvent *event);

    //bool Arc(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    //bool Arc(QRect* lpRect, QPoint ptStart, QPoint ptEnd);
    //bool Polyline(const QPoint* lpPoints, int nCount);
    //bool AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle);
    //bool ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    //bool ArcTo(QRect* lpRect, QPoint ptStart, QPoint ptEnd);
	//int GetArcDirection() const;
	//int SetArcDirection(int nArcDirection);
    //bool PolyDraw(const QPoint* lpPoints, const BYTE* lpTypes, int nCount);
    //bool PolylineTo(const QPoint* lpPoints, int nCount);
    //bool PolyPolyline(const QPoint* lpPoints,
	//	const DWORD* lpPolyPoints, int nCount);
    void updateCurrentPoint();

private:
    DrawCommandList mDrawCommands;
    rke::matrix<float> mTransformation;
    rke::matrix<float> mReTransformation;
    QPoint  mMouseStartPos;
    float  mViewportZoom;
    QSize  mWindowExtension;
    PointF mCurrentPoint;
    FLOAT  mPenWidth;
    FLOAT  mDrawingWidth;
    QPainterPath* mpPathDC;
    bool   mPathActive;
    FLOAT  mDeviderForRedSquare;
    FLOAT  mRotationAngle;
    QRectF mBoundingRect;

    QString mFontName;
    std::map<int, FLOAT> mFontDefinition;
    std::map<int, FLOAT> mAlternateFontDefinition;
    bool mAlternateFont;
    bool mReTransformationValid;
    boost::optional<unsigned long> mCombineID;
    bool mAutoDetermineDrillpoints;
};
