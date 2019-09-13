#include "PlotterDC.h"
#include "xml_functions.h"

#include <limits>
#include <QtGui/qevent.h>
#ifdef max
  #undef max
#endif
#ifdef min
  #undef min
#endif


#define TRACE_NYI TRACE
//#define TRACE_NYI //

#define TRACE_POINTS TRACE
//#define TRACE_POINTS //

namespace xmlkeys
{
    static const char* sItem        = "Item";
    static const char* sFlag        = "Flag";
    static const char* sType        = "Type";
    static const char* sPoints      = "Points";
    static const char* sMoveToPoint = "MoveToPoint";
}

const char * gTypeFace[] =
{
"Line Printer or Line Draw",
"Pica",
"Elite",
"Courier",
"Helvetica",
"Times Roman",
"Letter Gothic",
"Script",
"Prestige",
"Caslon",
"Orator",
"Presentation",
"Serifa",
"Futura",
"Palatino",
"ITC Souvenir",
"Optima",
"ITC Garamond",
"Coronet",
"Broadway",
"Century Schoolbook",
"University Roman",
"ITC Korinna",
"Naskh (generic Arabic",
"Cloister Black",
"ITC Galliard",
"ITC Avant Garde Gothic",
"Brush",
"Blippo",
"Hobo",
"Windsor",
"Peignot",
"Baskerville",
"Trade Gothic",
"Goudy Old Style",
"ITC Zapf Chancery",
"Clarendon",
"ITC Zapf Dingbats",
"Cooper",
"ITC Bookman",
"Stick (default)",
"HP-GL drafting",
"HP-GL fixed arc",
"Gill Sans",
"Univers",
"Bodoni",
"Rockwell",
"Melior",
"ITC Tiffany",
"ITC Clearface",
"Amelia",
"Park Avenue",
"Handel Gothic",
"Dom Casual",
"ITC Benguiat",
"ITC Cheltenham",
"Century Expanded",
"Franklin Gothic",
"Plantin",
"Trump Mediaeval",
"Futura Black",
"ITC American Typewriter",
"Antique Olive",
"Uncial",
"ITC Bauhaus",
"Century Oldstyle",
"ITC Eras",
"ITC Friz Quadrata",
"ITC Lubalin Graph",
"Eurostile",
"Mincho",
"ITC Serif Gothic",
"Signet Roundhand",
"Souvenir Gothic",
"Stymie",
"Bernhard Modern",
"Excelsior",
"Grand Ronde Script",
"Ondine",
"P.T.Barnum",
"Kaufman",
"ITC Bolt Bold",
"Helv Monospaced",
"Revue",
"Garamond (Stempel)",
"Garth Graphic",
"ITC Ronda",
"OCR-A",
"ITC Century",
"Englische Schreibschrift",
"Flash",
"Gothic Outline (URW)",
"Stensil (ATF)",
"OCR-B",
"Akzidenz-Grotesk",
"TD Logos",
"Shannon",
"ITC Century",
"Maru Gosikku",
"Gosikku (Kaku)",
"Socho",
"Kyokasho",
"Kaisho",
"Roman-8"
};
/*

Kind 3: Pitch
The pitch characteristic is a horizontal measurement defining the number of characters-per-inch
for fixed-spaced fonts. Note: When selecting proportional fonts, do not include pitch in the font
definition instruction (SD or AD).
Values Description
0 to Characters per inch
32 767.999 9 (default:
device-dependent)

Kind 4: Height
For proportional fonts, the height characteristic defines the font point size (the height of the char-
acter cell). (Fixed-spaced fonts ignore height; the point size is calculated using the font pitch.)
There are approximately 72 points in an inch. Note that with the font definition instruction (SD
or AD), you cannot create tall, skinny characters or short, wide characters; the character aspect
ratio is preserved.
Values Description
0 to Font point size
32 767.999 9 (default:
device-dependent)

Kind 5: Posture
Posture defines the character’s vertical posture. The default posture is upright.
Values Description
0 Upright (default)
1 Italic
2 Alternate Italic
479
This is the black on page 479 (seq: 497)
This is the blue on page 479 (seq: 497)
Company confidential. HP-GL/2 and HP RTL Reference Guide, draft 2. Freeze Status: open
Fixed-spaced fonts depend on pitch to determine character size. Proportional fonts ignore pitch.
Note that with the SD and AD instruction you cannot create tall, skinny characters or short, wide
characters; the character aspect ratio is preserved unless an SI or SR instruction overrides it.Kind 6: Stroke Weight
The stroke weight characteristic defines the line thickness used in the font’s design.

The default stroke weight is medium. When relative sizing is in effect, changes in P1 and P2
cause the relative stroke weight to change in relation to the change in P1/P2. If the aspect ratio of
the P1/P2 rectangle is maintained as P1 and P2 are moved, a medium stroke weight font still
looks “medium” after it is enlarged or reduced.
Note: Available stroke weights are the same as those available within PCL.
When the Stick font (typeface 48) is selected, the value 9999 renders it using the current pen
width.
 */
const FLOAT zoomFactor = 1.2f;
const FLOAT zoomMax    = 100000.0f;
const FLOAT zoomMin    = 1.0f;

int round(float f)
{
    float korr = (f >= 0) ? 0.5f : -0.5f;
    return static_cast<int>(f + korr);
}

class BoundingRect
{
public:
   // Constructor initializes the rectangle cordinates
    BoundingRect (const PointF& initial ) : mLeftTop (initial) , mRightBottom (initial)
    {
    }

   // The function call to process the next elment
   void operator ( ) ( const PointF& elem ) \
   {
        if (elem.x < mLeftTop.x    ) mLeftTop.x     = elem.x;
        if (elem.x > mRightBottom.x) mRightBottom.x = elem.x;
        if (elem.y < mLeftTop.y    ) mLeftTop.y     = elem.y;
        if (elem.y > mRightBottom.y) mRightBottom.y = elem.y;
   }

   // return bounding rectangle
   operator BoundingRect ( )
   {
      return *this;
   }
   PointF mLeftTop;
   PointF mRightBottom;
   QRectF toQRectF();
};

QRectF BoundingRect::toQRectF()
{
    return QRectF(mLeftTop.x, mLeftTop.y, mRightBottom.x - mLeftTop.x, mRightBottom.y - mLeftTop.y);
}


//! class functions of CPlotterDC
CPlotterDC::CPlotterDC(QWidget *parent) : QGraphicsView(parent)
, mMouseStartPos(0, 0)
, mViewportZoom(1.0f)
, mWindowExtension(381, 502)
, mCurrentPoint(0, 0)
, mPenWidth(0)
, mDrawingWidth(0)
, mpPathDC(0)
, mPathActive(false)
, mDeviderForRedSquare(100)
, mRotationAngle(0)
, mAlternateFont(false)
, mReTransformationValid(false)
{
    mTransformation.identity(3);
}

CPlotterDC::~CPlotterDC()
{
}

void  CPlotterDC::clearDrawCommands()
{
    mDrawCommands.clear();
    mFontName.clear();
}

void CPlotterDC::addDrawCommand(DrawCommand& aCmd)
{
    aCmd.mCombineID = mCombineID;
    mDrawCommands.push_back(aCmd);
}

void  CPlotterDC::setRotation(FLOAT anAngle)
{
    mRotationAngle = anAngle;
}

void  CPlotterDC::reset_transformation()
{
    mTransformation.identity(3);
    if (mRotationAngle != 0)
    {
        rotate(mRotationAngle*deg2rad);
    }
}

void CPlotterDC::rotate(FLOAT aAngle)
{
    rke::matrix<FLOAT> fRotate;
    fRotate.identity(3);
    fRotate.rotate_z(aAngle);
    mTransformation = mTransformation * fRotate;
    mReTransformationValid = false;
}

void  CPlotterDC::scale(FLOAT x, FLOAT y)
{
    std::vector<FLOAT> fv(2);
    fv[0] = x;
    fv[1] = y;
    scale(fv);
}

void CPlotterDC::scale(const std::vector<FLOAT>& aScale)
{
    rke::matrix<FLOAT> fScale;
    fScale.scale(aScale);
    mTransformation = mTransformation * fScale;
    mReTransformationValid = false;
}

void  CPlotterDC::translate(FLOAT x, FLOAT y)
{
    std::vector<FLOAT> fv(2);
    fv[0] = x;
    fv[1] = y;
    translate(fv);
}

void  CPlotterDC::translate(const std::vector<FLOAT>& aTranslation)
{
    rke::matrix<FLOAT> fTrans;
    fTrans.translate(aTranslation);
    mTransformation = mTransformation * fTrans;
    mReTransformationValid = false;
}


void  CPlotterDC::transform_point(FLOAT& x, FLOAT& y) const
{
    PointF fPt = transform_point(PointF(x, y));
    x = fPt.x;
    y = fPt.y;
    TRACE_POINTS(Logger::debug, "x: %f, y: %f\n", x, y);
}

PointF CPlotterDC::transform_point(const PointF& aPt) const
{
    return PointF(aPt.x * mTransformation[0][0] + aPt.y * mTransformation[0][1] + mTransformation[0][2],
                  aPt.x * mTransformation[1][0] + aPt.y * mTransformation[1][1] + mTransformation[1][2]);
}

void   CPlotterDC::re_transform_point(FLOAT& x, FLOAT& y)
{
    if (!mReTransformationValid)
    {
        mReTransformation.invert(mTransformation);
        mReTransformationValid = true;
    }
    FLOAT temp = x * mReTransformation[0][0] + y * mReTransformation[0][1] + mReTransformation[0][2];
             y = x * mReTransformation[1][0] + y * mReTransformation[1][1] + mReTransformation[1][2];
    x = temp;
}

bool CPlotterDC::getStartPosition(FLOAT& x, FLOAT& y)
{
    DrawCommandList::const_iterator it;
    for (it = mDrawCommands.begin(); it != mDrawCommands.end(); ++it)
    {
        if (!it->isVisible()) continue;
        x = it->mPosition[0].x;
        y = it->mPosition[0].y;
        re_transform_point(x, y);
        return true;
    }
    return false;
}

void CPlotterDC::setAutoDetermineDrillpoints(bool aAuto)
{
    mAutoDetermineDrillpoints = aAuto;
}

void CPlotterDC::mousePressEvent ( QMouseEvent * event )
{
    QGraphicsView::mousePressEvent(event);
    mMouseStartPos = event->pos();
    if (event->button() == Qt::RightButton)
    {
        QPointF pt = mapToScene(event->pos());
        mSendPos(pt.x() / SCALE_FACTOR, pt.y() / SCALE_FACTOR);
    }
}

void CPlotterDC::mouseMoveEvent( QMouseEvent * event )
{
    QGraphicsView::mouseMoveEvent(event);
    QPointF st = mapToScene(mMouseStartPos);
    QPointF pt = mapToScene(event->pos());
    mSendPos((pt.x() - st.x()) / SCALE_FACTOR, (pt.y() - st.y()) / SCALE_FACTOR);
}

void CPlotterDC::mouseReleaseEvent ( QMouseEvent * event )
{
    QGraphicsView::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        QPointF tl = mapToScene(mMouseStartPos);
        QPointF br = mapToScene(event->pos());
        QRectF fMouseDragRect(tl, br);
        QRect rc = fMouseDragRect.toRect();
        if (rc.width() > 10 && rc.height() > 10)
        {
            setClipRect(&rc);
            redraw();
        }
    }
    else if (   event->button() == Qt::RightButton
             || mMouseStartPos == event->pos())
    {
        QPointF br = mapToScene(event->pos());
        selectItem(br.toPoint());
    }
}

void CPlotterDC::wheelEvent(QWheelEvent *event)
{

}

void CPlotterDC::getCurrentPoint(FLOAT& x, FLOAT& y) const
{
    x = mCurrentPoint.x;
    y = mCurrentPoint.y;
}

PointF CPlotterDC::getCurrentPoint() const
{
    return mCurrentPoint;
}

void CPlotterDC::setCurrentPoint(const PointF& fPt)
{
    mCurrentPoint = fPt;
}


QPointF CPlotterDC::GetCurrentPathDCPosition() const
{
    if (mpPathDC)
    {
        return mpPathDC->currentPosition();
    }
    return QPointF(0, 0);
}

bool CPlotterDC::MoveTo(const PointF& aPt )
{
    return MoveTo(aPt.x, aPt.y);
}

bool CPlotterDC::MoveTo(FLOAT x, FLOAT y)
{
    mCurrentPoint.x = x;
    mCurrentPoint.y = y;
    transform_point(x, y);
    if (mpPathDC && mPathActive)
    {
        mpPathDC->moveTo( x, y);
        return true;
    }
    DrawCommand fMoveTo(x, y, DrawCommand::MoveTo);
    addDrawCommand(fMoveTo);
    return true;
}

bool CPlotterDC::LineTo(const PointF& aPt )
{
    return LineTo(aPt.x, aPt.y);
}

bool CPlotterDC::LineTo(FLOAT x, FLOAT y)
{
    mCurrentPoint.x = x;
    mCurrentPoint.y = y;
    transform_point(x, y);
    if (mpPathDC && mPathActive)
    {
        mpPathDC->lineTo( x, y);
        return true;
    }
    DrawCommandList::reverse_iterator previous = mDrawCommands.rbegin();
    if ( previous != mDrawCommands.rend() &&
        (   previous->mType == DrawCommand::LineTo
         || previous->mType == DrawCommand::PolyLine) )
    {
        previous->mPosition.push_back(PointF(x, y));
        previous->mType = DrawCommand::PolyLine;
        updatePenWidth(*previous);
    }
    else
    {
        DrawCommand fLineTo(x, y, DrawCommand::LineTo);
        addDrawCommand(fLineTo);
    }
    return true;
}

bool CPlotterDC::RectFill(FLOAT aX, FLOAT aY, FLOAT aWidth, FLOAT aHeight)
{
    bool fReturn = true;
    if (hasVariablePenWidth())
    {
        FLOAT fOldDrawingWidth = mDrawingWidth;
        FLOAT fDummy=0;
        mDrawingWidth = aWidth;
        transform_point(mDrawingWidth, fDummy);
        MoveTo(aX+aWidth*0.5f, aY);
        fReturn = LineTo(aX+aWidth*0.5f, aY+aHeight);
        mDrawingWidth = fOldDrawingWidth;
    }
    else
    {
        MoveTo(aX       , aY);
        fReturn &= LineTo(aX+aWidth, aY);
        fReturn &= LineTo(aX+aWidth, aY+aHeight);
        fReturn &= LineTo(aX       , aY+aHeight);
        fReturn &= LineTo(aX       , aY);
    }
    return fReturn;
}

bool CPlotterDC::BeginPath()
{
    bool fReturn = true;

    delete mpPathDC;
    mpPathDC = new QPainterPath;
    mPathActive = true;
    return fReturn;
}

bool CPlotterDC::EndPath()
{
    bool fReturn = false;
    if (mpPathDC)
    {
        mpPathDC->closeSubpath();
        mPathActive = false;
    }
    return fReturn;
}

bool CPlotterDC::isPathActive()
{
    return mPathActive;
}

int CPlotterDC::SaveDC()
{
    return 0;
}

void CPlotterDC::RestoreDC(int /* n */ )
{

}

void CPlotterDC::SetLineJoin(int aLineJoin )
{
    /*
PS_JOIN_BEVEL Joins are beveled.
PS_JOIN_MITER Joins are mitered when they are within the current limit set by the SetMiterLimit function. If it exceeds this limit, the join is beveled.
PS_JOIN_ROUND Joins are round.

    HPEN ExtCreatePen(
  DWORD dwPenStyle,      // pen style
  DWORD dwWidth,         // pen width
  CONST LOGBRUSH *lplb,  // brush attributes
  DWORD dwStyleCount,    // length of custom style array
  CONST DWORD *lpStyle   // custom style array
);
*/
    TRACE_NYI(Logger::info,"SetLineJoin(%d) nyi\n", aLineJoin);
}

void CPlotterDC::setCombineID(boost::optional<unsigned long> aID)
{
    mCombineID = aID;
}

void CPlotterDC::SetLineCap(int aLineCap )
{
//PS_ENDCAP_ROUND End caps are round.
//PS_ENDCAP_SQUARE End caps are square.
//PS_ENDCAP_FLAT End caps are flat.

    TRACE_NYI(Logger::info,"SetLineCap(%d) nyi\n", aLineCap);
}

void  CPlotterDC::updatePenWidth(DrawCommand& aDrawCmd)
{
    size_t positions = aDrawCmd.mPosition.size();
    if (hasVariablePenWidth() && positions >= 2)
    {
        FLOAT fFactor = mDrawingWidth / mPenWidth;
        int fRoundFactor = round(fFactor);
        if (fRoundFactor>1)
        {
            FLOAT fOffset = (mDrawingWidth - mPenWidth) * 0.5f;
            const FLOAT fLimit = mPenWidth * 0.5f;
            PointF pt1 = aDrawCmd.mPosition[positions-2];
            PointF pt2 = aDrawCmd.mPosition[positions-1];
            rke::vectorX<FLOAT> vPt1(pt1.x, pt1.y);
            rke::vectorX<FLOAT> vPt2(pt2.x, pt2.y);
            rke::vectorX<FLOAT> vPtNorm1;
            rke::vectorX<FLOAT> vPt;
            vPtNorm1.cross_product(vPt1, vPt2);
            vPtNorm1.norm();
            while (fOffset > fLimit)
            {
                aDrawCmd.mPosition.push_back(PointF(vPt2 + fOffset * vPtNorm1));
                aDrawCmd.mPosition.push_back(PointF(vPt1 + fOffset * vPtNorm1));
                aDrawCmd.mPosition.push_back(PointF(vPt1 - fOffset * vPtNorm1));
                aDrawCmd.mPosition.push_back(PointF(vPt2 - fOffset * vPtNorm1));
                fOffset -= mPenWidth;
            }
        }
    }
}

bool CPlotterDC::Circle(FLOAT ax, FLOAT ay, FLOAT aradius)
{
    std::vector<FLOAT> fDrawItems;
    fDrawItems.push_back(ax);
    fDrawItems.push_back(ay);
    FLOAT frxC(aradius), fryC(aradius);
    transform_point(fDrawItems[0], fDrawItems[1]);
    transform_point(frxC, fryC);
    fDrawItems.push_back((frxC+fryC)/2);
    DrawCommand fCircle(fDrawItems, DrawCommand::Circle);
    addDrawCommand(fCircle);
    return true;
}

bool CPlotterDC::ArcTo(const QRectF& aRect, FLOAT aStartAngle, FLOAT aAngleSpan)
{
    return ArcTo(aRect.left(), aRect.top(), aRect.right(), aRect.bottom(), aStartAngle, aAngleSpan);
}

bool CPlotterDC::ArcTo(FLOAT aLeft, FLOAT aTop, FLOAT aRight, FLOAT aBottom, FLOAT aStartAngle, FLOAT aAngleSpan)
{
    transform_point(aLeft, aTop);
    transform_point(aRight, aBottom);

    PointF fCenter = transform_point(mCurrentPoint);

    QRectF rc(QPointF(aLeft, aTop), QPointF(aRight, aBottom));
    if (mPathActive && mpPathDC)
    {
        mpPathDC->moveTo(fCenter.getQPointF());
        mpPathDC->arcTo(rc, aStartAngle, aAngleSpan);
        return true;
    }
    QPainterPath dc;

    dc.moveTo(fCenter.getQPointF());
    dc.arcTo(rc, aStartAngle, aAngleSpan);
    DrawCommand fDummy;
    if (!ExtractPath(dc, fDummy))
    {
        drawRectangle(rc);
    }

    return true;
}

bool CPlotterDC::addElipse(const QRectF& aRect)
{
    return addElipse(aRect.left(), aRect.top(), aRect.right(), aRect.bottom());
}

bool CPlotterDC::addElipse(FLOAT aLeft, FLOAT aTop, FLOAT aRight, FLOAT aBottom)
{
    transform_point(aLeft, aTop);
    transform_point(aRight, aBottom);

    QRectF rc(QPointF(aLeft, aTop), QPointF(aRight, aBottom));
    if (mPathActive && mpPathDC)
    {
        mpPathDC->addEllipse(rc);
        return true;
    }
    QPainterPath dc;

    //dc.addEllipse(fCenter.getQPointF(), rc.width()/2, rc.height()/2);
    dc.addEllipse(rc);
    DrawCommand fDummy;
    if (!ExtractPath(dc, fDummy))
    {
        drawRectangle(rc);
    }

    return true;
}

void CPlotterDC::updateCurrentPoint()
{
    DrawCommandList::reverse_iterator previous = mDrawCommands.rbegin();
    if (previous != mDrawCommands.rend())
    {
        DrawCommand &aCmd = *previous;
        QPointF ptCurrent = aCmd.getPoint(DrawCommand::LastPoint);
        mCurrentPoint.x = ptCurrent.x();
        mCurrentPoint.y = ptCurrent.y();
        re_transform_point(mCurrentPoint.x, mCurrentPoint.y);
    }
}

bool CPlotterDC::ArcTo(FLOAT ax, FLOAT ay, FLOAT adeg, bool aXisRadius)
{
    bool fInserted = false;
    FLOAT fxS(mCurrentPoint.x), fyS(mCurrentPoint.y);
    QRectF rc;
    transform_point(fxS, fyS);
    if (aXisRadius)
    {
        FLOAT fRadius = ax;
        rc.setTopLeft(QPointF(fxS-fRadius, fyS-fRadius));
        rc.setBottomRight(QPointF(fxS+fRadius, fyS+fRadius));
    }
    else
    {
        FLOAT fxC(ax), fyC(ay);
        transform_point(fxC, fyC);
        rc.setTopLeft(QPointF(fxC, fyC));
        rc.setBottomRight(QPointF(fxS, fyS));
    }
    if (mPathActive && mpPathDC)
    {
        mpPathDC->moveTo(fxS, fyS);
        mpPathDC->arcTo(rc, 180, adeg);
        return true;
    }

    QPainterPath dc;
    dc.moveTo(fxS, fyS);
    dc.arcTo(rc, 0, adeg);
    DrawCommand fDummy;
    fInserted = ExtractPath(dc, fDummy);

    if (!fInserted)
    {
        drawRectangle(rc);
    }
    return true;
}

bool CPlotterDC::Wedge(FLOAT aRadius, FLOAT aStartAngle, FLOAT aArcAngle)
{
    std::vector<QPointF> points;
    bool fInserted = false;
    FLOAT fxC, fyC;
    getCurrentPoint(fxC, fyC);

    QRectF rc(fxC-aRadius, fyC-aRadius, aRadius*2, aRadius*2);

    if (mPathActive && mpPathDC)
    {
        mpPathDC->arcTo(rc, aStartAngle, aArcAngle);
        return true;
    }

    DrawCommandList::reverse_iterator previous = mDrawCommands.rbegin();
    if (previous != mDrawCommands.rend())
    {
        QPainterPath dc;

        if (previous->mType == DrawCommand::MoveTo)
        {
            dc.moveTo(previous->getPoint(0));
        }
        else if (previous->mType == DrawCommand::PolyLine)
        {
            dc.moveTo(previous->getPoint(DrawCommand::LastPoint));
        }
        dc.arcTo(rc, aStartAngle, aArcAngle);
        fInserted = ExtractPath(dc, *previous);
    }
    if (!fInserted)
    {
        drawRectangle(rc);
    }
    return true;
}

bool CPlotterDC::CurveTo(const std::vector<FLOAT>& aPoints)
{
    size_t size = aPoints.size();
    std::vector<QPointF> points;
    bool fInserted = false;
    FLOAT x(0), y(0);
    mCurrentPoint.x = aPoints[size-2];
    mCurrentPoint.y = aPoints[size-1];
    for (size_t i=0; i<size-1; i+=2)
    {
        x = aPoints[i];
        y = aPoints[i+1];
        transform_point(x, y);
        points.push_back(QPointF(x, y));
    }
    if (mPathActive && mpPathDC)
    {
        size_t pts = points.size();
        for (size_t i = 0; i< pts; i+=3)
        {
            mpPathDC->cubicTo(points[i], points[i+1], points[i+2]);
        }
        return true;
    }

    DrawCommandList::reverse_iterator previous = mDrawCommands.rbegin();
    if (previous != mDrawCommands.rend())
    {
        QPainterPath dc;
        if (previous->mType == DrawCommand::MoveTo)
        {
            dc.moveTo(previous->getPoint(0));
        }
        else if (previous->mType == DrawCommand::PolyLine)
        {
            dc.moveTo(previous->getPoint(DrawCommand::LastPoint));
        }
        size_t pts = points.size();
        for (size_t i = 0; i< pts; i+=3)
        {
            dc.cubicTo(points[i], points[i+1], points[i+2]);
        }
        fInserted = ExtractPath(dc, *previous);
    }
    if (!fInserted)
    {
        for (size_t i=0; i<size-1; i+=2)
        {
            LineTo(aPoints[i], aPoints[i+1]);
        }
    }
    return true;
}

bool CPlotterDC::TextOut(std::string aString)
{
    DrawCommandList::reverse_iterator previous = mDrawCommands.rbegin();
    QPoint fPt(mCurrentPoint.x, mCurrentPoint.y);
    if (previous != mDrawCommands.rend())
    {
        fPt = previous->getPoint(0);
    }
    QPainterPath fDc;
    QFont fFont;
    if (mFontName.size())
    {
        fFont.setFamily(mFontName);
    }
    std::map<int, FLOAT>& fFD = mAlternateFont ? mAlternateFontDefinition : mFontDefinition;

    if (fFD.count(FD::SymbolSet))
    {
        // TODO FD::SymbolSet
    }
    if (fFD.count(FD::FontSpacing))
    {
        fFont.setLetterSpacing(QFont::AbsoluteSpacing, fFD[FD::FontSpacing]);
    }
    if (fFD.count(FD::Pitch))
    {
        // pitch is character per inch
        // 72 points per inch
        //fFont.setLetterSpacing(QFont::PercentageSpacing, 1);
        //fFont.setFixedPitch(fFD[FD::Pitch] == 0);
        // TODO FD::Pitch
    }
    if (fFD.count(FD::Height))
    {
        fFont.setPointSizeF(fFD[FD::Height]);
    }
    if (fFD.count(FD::Posture))
    {
        switch (static_cast<FD::font_definition>(fFD[FD::Posture]))
        {
        case FD::Upright:         fFont.setStyle(QFont::StyleNormal);  break;
        case FD::Italic:          fFont.setStyle(QFont::StyleItalic);  break;
        case FD::AlternateItalic: fFont.setStyle(QFont::StyleOblique); break;
        default:                  fFont.setStyle(QFont::StyleNormal);  break;
        }
    }
    if (fFD.count(FD::StrokeWeight))
    {
        switch (static_cast<int>(fFD[FD::StrokeWeight]))
        {
        case -1: case -2: case -3: case -4: case -5: case -6: case -7:
            fFont.setWeight(QFont::Light);
            break;
        case 0: default:
            fFont.setWeight(QFont::Normal);
            break;
        case 1: case 2:
            fFont.setWeight(QFont::DemiBold);
            break;
        case 3: case 4:
            fFont.setWeight(QFont::Bold);
            break;
        case 5: case 6: case 7:
            fFont.setWeight(QFont::Black);
            break;
        }
    }
    if (fFD.count(FD::Typeface))
    {
        int fTypeFaceIndex = static_cast<int>(fFD[FD::Typeface]);
        const int fMaxTypeFaceIndex = sizeof(gTypeFace) / sizeof(gTypeFace[0]);
        if (fTypeFaceIndex < 0)                  fTypeFaceIndex = 0;
        if (fTypeFaceIndex >= fMaxTypeFaceIndex) fTypeFaceIndex = fMaxTypeFaceIndex-1;
        fFont.setFamily(QString(gTypeFace[fTypeFaceIndex]));
    }
    if (fFD.count(FD::StretchFactor))
    {
        fFont.setStretch(static_cast<int>(fFD[FD::StretchFactor] * 100.0+0.5));
    }

    fDc.addText(fPt, fFont, QString(aString.c_str()));
    DrawCommand dummy;
    ExtractPath(fDc, dummy);

    return true;
}

void CPlotterDC::setFontName(const std::string& aName)
{
    mFontName.setRawData(QString(aName.c_str()).constData(), aName.size());
}

void CPlotterDC::setCharSize(FLOAT aWidth, FLOAT aHeight)
{
    mFontDefinition[FD::Height] = aHeight;
    mFontDefinition[FD::StretchFactor] = aHeight / aWidth;
}

void   CPlotterDC::setFontDefinition(const std::map<int, FLOAT>& aFD, bool bAlternateFont)
{
    if (bAlternateFont)
    {
        mAlternateFontDefinition = aFD;
    }
    else
    {
        mFontDefinition = aFD;
    }

}

void CPlotterDC::selectFontDefinition(bool bAlternateFont)
{
    mAlternateFont = bAlternateFont;
}

void CPlotterDC::drawRectangle(const QRectF& aRect)
{
    MoveTo(aRect.left(), aRect.top());
    LineTo(aRect.right(), aRect.top());
    LineTo(aRect.right(), aRect.bottom());
    LineTo(aRect.left(), aRect.bottom());
    LineTo(aRect.left(), aRect.top());
    scene()->addRect(aRect);
}

int CPlotterDC::setClipRect(QRect* rcClip)
{
    if (rcClip)
    {
        DrawCommandList::iterator it;
        for (it = mDrawCommands.begin(); it != mDrawCommands.end(); ++it)
        {
            it->setVisible(rcClip->contains(it->getPoint(0)));
        }
    }
    else
    {
        DrawCommandList::iterator it;
        for (it = mDrawCommands.begin(); it != mDrawCommands.end(); ++it)
        {
            it->setVisible(true);
        }
    }
    return 1;
}

void CPlotterDC::selectItem(int aSel)
{
    for (size_t i = 0; i < mDrawCommands.size(); ++i)
    {
        mDrawCommands[i].setSelected(aSel == static_cast<int>(i));
    }
}

void CPlotterDC::setSelectedtDrillPointState(bool aDrill)
{
    auto fSelected = std::find_if(mDrawCommands.begin(), mDrawCommands.end(), [](const DrawCommand& aCmd){ return aCmd.isSelected();} );
    if (fSelected != mDrawCommands.end())
    {
        fSelected->setDrillPoint(aDrill);
    }
}

int CPlotterDC::getSelectedItem()
{
    auto fSelected = std::find_if(mDrawCommands.begin(), mDrawCommands.end(), [](const DrawCommand& aCmd){ return aCmd.isSelected();} );
    if (fSelected != mDrawCommands.end())
    {
        return std::distance(mDrawCommands.begin(), fSelected);
    }
    return -1;
}

void CPlotterDC::toggleAllDrillPointStates()
{
    bool fhasDrillPoints = false;
    for (size_t i = 0; i < mDrawCommands.size(); ++i)
    {
        if (mDrawCommands[i].isDrillPoint())
        {
            fhasDrillPoints = true;
            break;
        }
    }

    for (size_t i = 0; i < mDrawCommands.size(); ++i)
    {
        mDrawCommands[i].setDrillPoint(!fhasDrillPoints);
    }
}

void CPlotterDC::setSelectedItemAsFirst()
{
    DrawCommandList::iterator current;
    for (current = mDrawCommands.begin(); current != mDrawCommands.end(); ++current)
    {
        if (current->isSelected())
        {
            DrawCommand cmd = *current;
            mDrawCommands.erase(current);
            mDrawCommands.insert(mDrawCommands.begin(), cmd);
            break;
        }
    }
}

void CPlotterDC::selectItem(const QPointF& ptSelect)
{
    DrawCommandList::iterator it, hit = mDrawCommands.end(), oldhit = hit;
    FLOAT min_amount = std::numeric_limits<FLOAT>::max();
    int fIndex = 0;

    for (it = mDrawCommands.begin(); it != mDrawCommands.end(); ++it, ++fIndex)
    {
        for (size_t j=0; j<it->getPoints(); ++j)
        {
            QPointF ptdiff = ptSelect - it->getPointF(static_cast<int>(j));
            FLOAT amount = ptdiff.x() * ptdiff.x() + ptdiff.y() * ptdiff.y();
            if (amount < min_amount)
            {
                min_amount = amount;
                hit = it;
            }
        }
        if (it->isSelected())
        {
            oldhit = it;
        }
        it->setSelected(false);
    }

    if (hit != mDrawCommands.end())
    {
        hit->setSelected(true);
        if (hit != oldhit)
        {
            redraw();
        }
    }
}

void CPlotterDC::setWindowExtension(int cx, int cy)
{
    mWindowExtension.setWidth(cx);
    mWindowExtension.setHeight(cy);
}

QSize CPlotterDC::zoomOut()
{
    mViewportZoom *= zoomFactor;
    if (mViewportZoom > zoomMax)
    {
        mViewportZoom = zoomMax;
    }
    fitInView(0, 0, mViewportZoom, mViewportZoom, Qt::KeepAspectRatio);
    centerOn(mBoundingRect.center());

    return QSize(0, 0);
}

QSize CPlotterDC::zoomIn()
{
    mViewportZoom /= zoomFactor;
    if (mViewportZoom < zoomMin)
    {
        mViewportZoom = zoomMin;
        return QSize(0, 0);
    }
    fitInView(0, 0, mViewportZoom, mViewportZoom, Qt::KeepAspectRatio);
    centerOn(mBoundingRect.center());

    return QSize(0, 0);
}


void CPlotterDC::updateWindowMapping()
{
    if (mDrawCommands.size())
    {
        BoundingRect br = BoundingRect(mDrawCommands[0].mPosition[0]);
        for (unsigned int i=0; i<mDrawCommands.size(); ++i)
        {
            br = std::for_each(mDrawCommands[i].mPosition.begin(), mDrawCommands[i].mPosition.end(), br);
        }

        mBoundingRect = br.toQRectF();
        mViewportZoom = hypot(mBoundingRect.width(), mBoundingRect.height());
        setSceneRect(mBoundingRect);
        fitInView(0, 0, mViewportZoom, mViewportZoom, Qt::KeepAspectRatio);
        centerOn(mBoundingRect.center());
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void CPlotterDC::combineDrawCommands(unsigned long aDrillPointID)
{
    boost::optional<unsigned long> fDrillPointID = aDrillPointID;
    for (auto& fCmd : mDrawCommands)
    {
        if (fCmd.mCombineID && *fCmd.mCombineID >= aDrillPointID)
        {
            fCmd.setDrillPoint(mAutoDetermineDrillpoints);
            if (*fCmd.mCombineID == aDrillPointID)
            {
                fCmd.mCombineID = boost::none;
            }
        }
    }

    while (1)
    {
        auto fCombineableCmd = std::find_if(mDrawCommands.begin(), mDrawCommands.end(), [](const DrawCommand& aCmd) { return aCmd.mCombineID;} );
        if (fCombineableCmd != mDrawCommands.end())
        {
            while (1)
            {
                auto fNext = fCombineableCmd + 1;
                fNext = std::find_if(fNext, mDrawCommands.end(), [&fCombineableCmd](const DrawCommand& aCmd) { return aCmd.mCombineID == fCombineableCmd->mCombineID;} );
                if (   fNext != mDrawCommands.end()
                    && fCombineableCmd->appendPolyLine(*fNext))
                {
                    mDrawCommands.erase(fNext);
                }
                else
                {
                    break;
                }
            }
            fCombineableCmd->mCombineID.reset();
        }
        else
        {
            break;
        }
    }
    TRACE(Logger::info,"Number of Commands: %lu\n", mDrawCommands.size());
}

void CPlotterDC::concatenateMoveToAndPolyLine()
{
    // concatenate MoveTo and PolyLine
    TRACE(Logger::info,"Number of Commands: %lu\n", mDrawCommands.size());
    DrawCommandList::iterator current;
    for (size_t i=0; i<mDrawCommands.size(); ++i)
    {
        current = mDrawCommands.begin() + i;
        auto fMoveToCmd = std::find_if(current, mDrawCommands.end(), [](const DrawCommand& aCmd){ return aCmd.mType == DrawCommand::MoveTo;} );
        if (fMoveToCmd != mDrawCommands.end())
        {
            auto fNextCmd = fMoveToCmd;
            ++fNextCmd;
            if (fNextCmd != mDrawCommands.end())
            {
                if (fNextCmd->mType == DrawCommand::MoveTo)
                {
                    mDrawCommands.erase(fMoveToCmd);
                    --i;
                }
                else if (   fNextCmd->mType == DrawCommand::PolyLine
                    && (fNextCmd->mMoveToPoint.size() == 0 || fNextCmd->mMoveToPoint[0] != 0))
                {
                    fNextCmd->mMoveToPoint.insert(fNextCmd->mMoveToPoint.begin(), 1, 0);
                    fNextCmd->mPosition.insert(fNextCmd->mPosition.begin(), 1, fMoveToCmd->mPosition[0]);
                    mDrawCommands.erase(fMoveToCmd);
                    --i;
                }
            }
        }
    }
    TRACE(Logger::info,"Number of Commands: %lu\n", mDrawCommands.size());
}

void CPlotterDC::concatenateDrawCommands()
{
    // concatenate points and lines
    TRACE(Logger::info,"Number of Commands: %lu\n", mDrawCommands.size());
    DrawCommandList::iterator current, compare, nearest;
    for (size_t i=0; i<mDrawCommands.size(); ++i)
    {
        current = mDrawCommands.begin()+i;
        QPoint current_first = current->getPoint(0);
        QPoint current_last  = current->getPoint(DrawCommand::LastPoint);
        for (compare=mDrawCommands.begin(); compare!=mDrawCommands.end(); ++compare)
        {
            if (current == compare) continue;
            if (   current->getPoints() == compare->getPoints()
                && current_first == compare->getPoint(0)
                && current_last  == compare->getPoint(DrawCommand::LastPoint) )
            {
                mDrawCommands.erase(compare);
                --i;
                break;
            }
            if (current_last == compare->getPoint(0))
            {
                if (compare->mMoveToPoint.size())
                {
                    std::transform(compare->mMoveToPoint.begin(), compare->mMoveToPoint.end(), compare->mMoveToPoint.begin(), rke::add_value<int>(static_cast<int>(current->mPosition.size())-1));
                    current->mMoveToPoint.insert(current->mMoveToPoint.end(), compare->mMoveToPoint.begin(), compare->mMoveToPoint.end());
                }
                current->mPosition.insert(current->mPosition.end(), compare->mPosition.begin()+1, compare->mPosition.end());
                current->mType = DrawCommand::PolyLine;
                mDrawCommands.erase(compare);
                --i;
                break;
            }
            if (current_first == compare->getPoint(DrawCommand::LastPoint))
            {
                if (current->mMoveToPoint.size())
                {
                    std::transform(current->mMoveToPoint.begin(), current->mMoveToPoint.end(), current->mMoveToPoint.begin(), rke::add_value<int>(static_cast<int>(current->mPosition.size())-1));
                    compare->mMoveToPoint.insert(compare->mMoveToPoint.end(), current->mMoveToPoint.begin(), current->mMoveToPoint.end());
                }
                compare->mPosition.insert(compare->mPosition.end(), current->mPosition.begin()+1, current->mPosition.end());
                compare->mType = DrawCommand::PolyLine;
                mDrawCommands.erase(current);
                --i;
                break;
            }
        }
    }

    int dp(0);
    if (mAutoDetermineDrillpoints)
    {
        // determine number of drill points, if any
        for (current=mDrawCommands.begin(); current!=mDrawCommands.end(); ++current)
        {
            QRectF bounding_rect;
            QPointF centerPt = current->getCenterPoint(&bounding_rect);
            FLOAT dist, radius = bounding_rect.width() / 2;
            FLOAT rmin = radius*0.8;
            FLOAT rmax = radius*1.2;
            int   count = 0;
            if (current->getPoints() > 8)
            {
                for (unsigned int i=0; i<current->getPoints(); ++i)
                {
                    dist = PointF::distance(centerPt, current->getPoint(i));
                    if (rke::isBetween(dist, rmin, rmax )) ++count;
                }
                if (count >= static_cast<int>(current->getPoints()*0.9))
                {
                    current->setDrillPoint(true);
                    ++dp;
                }
            }
        }
        // combine drill points
        for (size_t i=0; i<mDrawCommands.size(); ++i)
        {
            current = mDrawCommands.begin()+i;
            if (current->isDrillPoint())
            {
                QPointF pt1 = current->getCenterPoint();
                for (compare=mDrawCommands.begin(); compare!=mDrawCommands.end(); ++compare)
                {
                    if (current != compare && compare->isDrillPoint())
                    {
                        QPointF pt2 = compare->getCenterPoint();
                        FLOAT fDist = PointF::distance(pt1, pt2);
                        if (fDist < 1) // if the distance of the center point is smaller than 0.1 mm
                        {
                            compare->setDrillPoint(false);
                            // TODO_C fill area of drill point
                            if (current->appendPolyLine(*compare))
                            {
                                nearest = compare;
                                --compare;
                                mDrawCommands.erase(nearest);
                                --dp;
                            }
                        }
                    }
                }
            }
        }
    }
    TRACE(Logger::info,"Number of concatenated Commands: %lu\nNumber of Drillpoints: %d\n", mDrawCommands.size(), dp);
}

void CPlotterDC::sortDrawCommands()
{
    // first move marked drillpoints to the end
    DrawCommandList fDrillPoints;
    while (1)
    {
        auto fDrillPoint = std::find_if(mDrawCommands.begin(), mDrawCommands.end(), [](const DrawCommand& aCmd){ return aCmd.isDrillPoint();} );
        if (fDrillPoint != mDrawCommands.end())
        {
            fDrillPoints.push_back(*fDrillPoint);
            mDrawCommands.erase(fDrillPoint);
        }
        else
        {
            break;
        }
    }

    // then sort only non drillpoint elements
    DrawCommandList::iterator current, compare, nearest;
    for (int i=0; i<static_cast<int>(mDrawCommands.size())-1; ++i)
    {
        current = mDrawCommands.begin()+i;
        nearest  = mDrawCommands.end();
        long min_amount = 1000000;
        bool bReverse   = false;
        QPoint current_pt = current->getPoint(DrawCommand::LastPoint);
        for (compare=current+1; compare!=mDrawCommands.end(); ++compare)
        {
            if (compare->isDrillPoint()) continue;
            QPoint diff = current_pt - compare->getPoint(0);
            long current_amount = diff.x() * diff.x() + diff.y() * diff.y();
            if (current_amount < min_amount)
            {
                min_amount = current_amount;
                nearest = compare;
                bReverse = false;
            }
            if (compare->isReverseable())
            {
                diff = current_pt - compare->getPoint(DrawCommand::LastPoint);
                current_amount = diff.x() * diff.x() + diff.y() * diff.y();
                if (current_amount < min_amount)
                {
                    min_amount = current_amount;
                    nearest = compare;
                    bReverse = true;
                }
            }
        }
        if (nearest != mDrawCommands.end())
        {
            DrawCommand cmd = *nearest;
            mDrawCommands.erase(nearest);
            if (bReverse)
            {
                cmd.reverse();
            }
            mDrawCommands.insert(current+1, cmd);
        }
    }

    if (fDrillPoints.size())
    {
        // determine bounding rectangle
        BoundingRect br = std::for_each(fDrillPoints[0].mPosition.begin()+1, fDrillPoints[0].mPosition.end(), BoundingRect(fDrillPoints[0].mPosition[0]));
        for(DrawCommandList::const_iterator it = fDrillPoints.begin()+1; it!=fDrillPoints.end(); ++it)
        {
            br = std::for_each(it->mPosition.begin(), it->mPosition.end(), br);
        }
        PointF aPT = br.mLeftTop;
        aPT.y += (br.mLeftTop.y - br.mRightBottom.y) * 50;
        std::sort(fDrillPoints.begin(), fDrillPoints.end(), [&aPT] (const DrawCommand& aCmd1, const DrawCommand& aCmd2)
        {
            return distance(aCmd1.getCenterPoint(), aPT) < distance(aCmd2.getCenterPoint(), aPT);
        } );
    }

    mDrawCommands.insert(mDrawCommands.end(), fDrillPoints.begin(), fDrillPoints.end());
}

bool CPlotterDC::redraw()
{
    QRect rc(0, 0, mWindowExtension.width(), mWindowExtension.height());
    scene()->clear();

    bool isFirstPoint(true);
    QPen redPen(QColor(255,0,0));
    QPen greenPen(QColor(0,255,0));
    QPen blackPen(QColor(0,0,0));
    QBrush redBrush(QColor(255,0,0));
    QPen *currentPen = 0;
    QPointF ptStart, ptEnd;
    redPen.setWidth(0);
    greenPen.setWidth(0);
    blackPen.setWidth(0);

//   TODO draw scale
//    scene()->addLine(0, 0, 0, mBoundingRect.top() - mBoundingRect.height());
//    scene()->addLine(0 , 0, mBoundingRect.left() + mBoundingRect.width(), 0);

    for (auto fDrawCmd :  mDrawCommands)
    {
        if (!fDrawCmd.isVisible()) continue;
        if (fDrawCmd.isSelected())
        {
            currentPen = &redPen;
        }
        else if (fDrawCmd.isDrillPoint())
        {
            currentPen = &greenPen;
        }
        else
        {
            currentPen = &blackPen;
        }
        if (isFirstPoint)
        {
            QPoint pt(fDrawCmd.getPoint(0));
            int ext = mWindowExtension.height() / mDeviderForRedSquare;
            QRect rc(pt.x()-ext/2, pt.y()-ext/2, ext, ext);
            scene()->addRect(rc, redPen, redBrush);
            isFirstPoint = false;
        }

        switch (fDrawCmd.mType)
        {
        case  DrawCommand::MoveTo:
            ptStart = fDrawCmd.getPointF(0);
            break;
        case  DrawCommand::LineTo:
            ptEnd = fDrawCmd.getPointF(0);
            scene()->addLine(QLineF(ptStart, ptEnd), *currentPen);
            ptStart = ptEnd;
            break;
        case  DrawCommand::PolyLine:
        {
            auto fMvToPt = fDrawCmd.mMoveToPoint.begin();
            int  size    = fDrawCmd.mPosition.size();
            for (int i=0; i < size; ++i)
            {
                if (    fMvToPt != fDrawCmd.mMoveToPoint.end()
                    && *fMvToPt == static_cast<int>(i))
                {
                    ptStart = fDrawCmd.getPointF(static_cast<int>(i));
                    ++fMvToPt;
                    continue;
                }
                ptEnd = fDrawCmd.getPointF(static_cast<int>(i));
                scene()->addLine(QLineF(ptStart, ptEnd), *currentPen);
                ptStart = ptEnd;

            }
        } break;
        case DrawCommand::Circle:
            break;
        default: break;

        }
    }

    scene()->invalidate();
    return true;
}

bool CPlotterDC::StrokePath(bool aFill)
{
    bool fReturn = false;
    if (mpPathDC && mpPathDC->elementCount() > 0)
    {
        if (mPathActive) EndPath();
        DrawCommand dummy;
        fReturn = ExtractPath(*mpPathDC, dummy);
        delete mpPathDC;
        mpPathDC = 0;
        if (fReturn)
        {
            if (aFill)
            {
                DrawCommandList::reverse_iterator lastCmd = mDrawCommands.rbegin();
                if (lastCmd != mDrawCommands.rend())
                {
                    // TODO! Fill path
                    updatePenWidth(*lastCmd);
                }
            }
        }
    }
    return fReturn;
}

bool CPlotterDC::ExtractPath(QPainterPath& aDc, DrawCommand& apPrevious)
{
    QList<QPolygonF> fPolygons = aDc.toSubpathPolygons();

    int fNumPoly = fPolygons.count();
    if (fNumPoly > 0)
    {
        DrawCommand drawcmd(DrawCommand::PolyLine);
        for (int fPoly=0; fPoly<fNumPoly; ++fPoly)
        {
            int fPoints = fPolygons[fPoly].count();
            if (fPoly > 0 && fPoints > 0)
            {
                drawcmd.mMoveToPoint.push_back(drawcmd.mPosition.size());
            }
            else
            {
                drawcmd.mMoveToPoint.push_back(0);
            }
            TRACE(Logger::info,"Polygon %d: %d pts\n", fPoly+1, fPoints);
            for (int fPt=0; fPt<fPoints; ++fPt)
            {
                drawcmd.mPosition.push_back(fPolygons[fPoly][fPt]);
            }
        }
        if (apPrevious.mType == DrawCommand::MoveTo)
        {
            apPrevious.mType     = DrawCommand::PolyLine;
            apPrevious.mPosition = drawcmd.mPosition;
        }
        else if (apPrevious.mType == DrawCommand::PolyLine)
        {
            int nIndex = 1;
            apPrevious.mPosition.insert(apPrevious.mPosition.end(), drawcmd.mPosition.begin()+nIndex, drawcmd.mPosition.end());
        }
        else
        {
            addDrawCommand(drawcmd);
        }
        return true;
    }

   return false;
}



//bool CPlotterDC::Arc(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3, FLOAT x4, FLOAT y4)
//{
//    return CDC::Arc(round(x1), round(y1), round(x2), round(y2),
//       round(x3), round(y3), round(x4), round(y4));
//}
//
//bool CPlotterDC::Arc(QRect* lpRect, QPoint ptStart, QPoint ptEnd)
//{
//    return CDC::Arc(lpRect->left(), lpRect->top(), lpRect->right(), lpRect->bottom(), ptStart.x(), ptStart.y(), ptEnd.x(), ptEnd.y());
//}

/*
bool CPlotterDC::Polyline(const QPoint* lpPoints, int nCount)
{
    return CDC::Polyline(lpPoints, nCount);
}

bool CPlotterDC::AngleArc(int x, int y, int nRadius, FLOAT fStartAngle, FLOAT fSweepAngle)
{
    return CDC::AngleArc(x, y, nRadius, fStartAngle, fSweepAngle);
}
bool CPlotterDC::ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    return CDC::ArcTo(x1, y1, x2, y2, x3, y3, x4, y4);
}
bool CPlotterDC::ArcTo(QRect* lpRect, QPoint ptStart, QPoint ptEnd)
{
    return CDC::ArcTo(lpRect->left(), lpRect->top(), lpRect->right(), lpRect->bottom(), ptStart.x(), ptStart.y(), ptEnd.x(), ptEnd.y());
}
int CPlotterDC::GetArcDirection() const
{
    return CDC::GetArcDirection();
}
int CPlotterDC::SetArcDirection(int nArcDirection)
{
    return CDC::SetArcDirection(nArcDirection);
}

bool CPlotterDC::PolyDraw(const QPoint* lpPoints, const BYTE* lpTypes, int nCount)
{
    return CDC::PolyDraw(lpPoints, lpTypes, nCount);
}
bool CPlotterDC::PolylineTo(const QPoint* lpPoints, int nCount)
{
    return CDC::PolylineTo(lpPoints, nCount);
}
bool CPlotterDC::PolyPolyline(const QPoint* lpPoints, const DWORD* lpPolyPoints, int nCount)
{
    return CDC::PolyPolyline(lpPoints, lpPolyPoints, nCount);
}
*/


//! class functions of CPlotterDC::DrawCommand

FLOAT CPlotterDC::DrawCommand::mHorizontalSize = 0;
FLOAT CPlotterDC::DrawCommand::mVerticalStep   = 1;

CPlotterDC::DrawCommand::DrawCommand() : mType(Unknown), mFlag(true)
{
}

CPlotterDC::DrawCommand::DrawCommand(eCommand aType) : mType(aType), mFlag(true)
{
}

CPlotterDC::DrawCommand::DrawCommand(const std::vector<FLOAT>&aPt, eCommand aType) : mType(aType), mFlag(true)
{
    for (size_t i=0; i<aPt.size()-1; i+=2)
    {
        mPosition.push_back(PointF(aPt[i], aPt[i+1]));
    }
}

CPlotterDC::DrawCommand::DrawCommand(std::vector<QPoint>::const_iterator aBegin, std::vector<QPoint>::const_iterator aEnd, eCommand aType): mType(aType), mFlag(true)
{
    for (std::vector<QPoint>::const_iterator it = aBegin; it != aEnd; ++it)
    {
        mPosition.push_back(PointF(static_cast<FLOAT>(it->x()), static_cast<FLOAT>(it->y())));
    }
}

CPlotterDC::DrawCommand::DrawCommand(FLOAT aX, FLOAT aY, eCommand aType) : mType(aType), mFlag(true)
{
    mPosition.push_back(PointF(aX, aY));
}

QPoint CPlotterDC::DrawCommand::getPoint(int ap) const
{
    QPointF pt = getPointF(ap);
    return QPoint(round(pt.x()), round(pt.y()));
}

QPointF CPlotterDC::DrawCommand::getPointF(int ap) const
{
    if (ap == LastPoint)
    {
        ap = static_cast<int>(mPosition.size()-1);
    }
    if (ap >= 0 && ap < static_cast<int>(mPosition.size()))
    {
        return QPointF(mPosition[ap].x, mPosition[ap].y);
    }

    return QPointF(0, 0);
}

void CPlotterDC::DrawCommand::setVisible(bool aV)
{
    if (aV)
    {
        mFlag |= Visible;
    }
    else
    {
        mFlag &= ~Visible;
    }
}

void CPlotterDC::DrawCommand::setSelected(bool aV)
{
    if (aV)
    {
        mFlag |= Selected;
    }
    else
    {
        mFlag &= ~Selected;
    }
}

void CPlotterDC::DrawCommand::setDrillPoint(bool aV)
{
    if (aV)
    {
        mFlag |= DrillPoint;
    }
    else
    {
        mFlag &= ~DrillPoint;
    }
}

void CPlotterDC::DrawCommand::setPolygoneStart(bool aV)
{
    if (aV)
    {
        mFlag |= PolygoneStart;
    }
    else
    {
        mFlag &= ~PolygoneStart;
    }
}

QPointF CPlotterDC::DrawCommand::getCenterPoint(QRectF *pRect) const
{
    if (mPosition.size())
    {
        BoundingRect br = std::for_each(mPosition.begin()+1, mPosition.end(), BoundingRect(mPosition[0]));
        if (pRect)
        {
            pRect->setCoords(br.mLeftTop.x, br.mLeftTop.y, br.mRightBottom.x, br.mRightBottom.y);
        }
        return QPointF((br.mLeftTop.x+br.mRightBottom.x)*0.5f, (br.mLeftTop.y+br.mRightBottom.y)*0.5f);
    }
    return QPointF(0, 0);
}

bool CPlotterDC::DrawCommand::appendPolyLine(DrawCommand& aPolyline)
{
    if (mType == PolyLine && aPolyline.mType == PolyLine
//            && aPolyline.mMoveToPoint.size() && aPolyline.mMoveToPoint[0] == 0
            )
    {
        int fSize = mPosition.size();
        for (int fPt : aPolyline.mMoveToPoint)
        {
            mMoveToPoint.push_back(fSize+fPt);
        }

        mPosition.insert(mPosition.end(), aPolyline.mPosition.begin(), aPolyline.mPosition.end());
        return true;
    }
    return false;
}

bool CPlotterDC::DrawCommand::isReverseable()
{
    return mType == PolyLine && mMoveToPoint.size() > 0 && !isDrillPoint();
}

void CPlotterDC::DrawCommand::reverse()
{
    if ( mType == PolyLine )
    {
        std::vector<PointF> temp = mPosition;
        mPosition.assign(temp.rbegin(), temp.rend());
        if (mMoveToPoint.size())
        {
            for (unsigned int i = 1, j = mMoveToPoint.size()-1; i < mMoveToPoint.size(); ++i, --j)
            {
                mMoveToPoint[i] = mPosition.size() - mMoveToPoint[j];
            }
        }
    }
}

bool  CPlotterDC::DrawCommand::operator < (const CPlotterDC::DrawCommand& dc) const
{
    const QPointF pt1      = getCenterPoint();
    const int    line1     = static_cast<int>(pt1.x() / mVerticalStep);
    const FLOAT  distance1 = line1 * mHorizontalSize + ((line1 & 1) ? pt1.x() : mHorizontalSize - pt1.x());
    const QPointF pt2      = dc.getCenterPoint();
    const int    line2     = static_cast<int>(pt2.y() / mVerticalStep);
    const FLOAT  distance2 = line2 * mHorizontalSize + ((line2 & 1) ? pt2.x() : mHorizontalSize - pt2.x());
    return (distance1 < distance2);
}

void CPlotterDC::DrawCommand::writeXml(QDomDocument& aDoc, QDomElement& aParent) const
{
    QDomElement fItem = aDoc.createElement(xmlkeys::sItem);
    aParent.appendChild(fItem);
    std::vector<PointF>::const_iterator it;
    QString fPointArray;
    for (std::vector<PointF>::const_iterator it=mPosition.begin(); it != mPosition.end(); ++it)
    {
        fPointArray += it->toString();
        fPointArray += ";";
    }
    if (fPointArray.size())
    {
        fPointArray.remove(fPointArray.size()-1, 1);
    }
    addElementS(aDoc, fItem, xmlkeys::sPoints, fPointArray);

    fPointArray.clear();
    for (std::vector<int>::const_iterator it=mMoveToPoint.begin(); it != mMoveToPoint.end(); ++it)
    {
        fPointArray += QString::number(*it);
        fPointArray += ";";
    }
    if (fPointArray.size())
    {
        fPointArray.remove(fPointArray.size()-1, 1);
        addElementS(aDoc, fItem, xmlkeys::sMoveToPoint, fPointArray);
    }
    addElementS( aDoc, fItem, xmlkeys::sType, name());
    addElement( aDoc, fItem, xmlkeys::sFlag, mFlag);
}


#define RETURN_NAME(X) case X: return #X

QString CPlotterDC::DrawCommand::name(eCommand aCmd)
{
    switch (aCmd)
    {
    RETURN_NAME(Unknown);
    RETURN_NAME(MoveTo);
    RETURN_NAME(LineTo);
    RETURN_NAME(Circle);
    RETURN_NAME(PolyLine);
    }
    return "";
}

QString CPlotterDC::DrawCommand::name() const
{
    return name(mType);
}

#undef RETURN_NAME

CPlotterDC::DrawCommand::eCommand CPlotterDC::DrawCommand::mame2type(const QString& aName) const
{
    if      (name(MoveTo)   == aName) return MoveTo;
    else if (name(LineTo)   == aName) return LineTo;
    else if (name(Circle)   == aName) return Circle;
    else if (name(PolyLine) == aName) return PolyLine;
    return Unknown;
}



QString CPlotterDC::DrawCommand::info() const
{
    QPointF fCenter = getCenterPoint();
    return name() + ": " + QString::number(mPosition.size()) + " points in (" + QString::number(fCenter.x()) + ", " +  QString::number(fCenter.y()) + ")";
}

void CPlotterDC::DrawCommand::readXml(const QDomNode& aItem)
{
    QDomNode fPoints = findElement(aItem, xmlkeys::sPoints);
    if (!fPoints.isNull())
    {
        QString fValues = fPoints.toElement().text();
        QStringList fPointArray = fValues.split(';');
        for (QStringList::ConstIterator it = fPointArray.begin(); it != fPointArray.end(); ++it)
        {
            PointF fP;
            fP.fromString(*it);
            mPosition.push_back(fP);
        }
    }

    fPoints = findElement(aItem, xmlkeys::sMoveToPoint);
    if (!fPoints.isNull())
    {
        QString fValues = fPoints.toElement().text();
        QStringList fPointArray = fValues.split(';');
        for (QStringList::ConstIterator it = fPointArray.begin(); it != fPointArray.end(); ++it)
        {
            mMoveToPoint.push_back(it->toInt());
        }
    }
    mFlag = getValue(findElement(aItem, xmlkeys::sFlag), 0);
    mType = mame2type(getValue(findElement(aItem, xmlkeys::sType), QString("Unknown")));
}

