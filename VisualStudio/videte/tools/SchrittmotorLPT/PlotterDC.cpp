#include "StdAfx.h"
#include "PlotterDC.h"

int round(FLOAT f)
{
    FLOAT korr = static_cast<FLOAT>((f >= 0) ? 0.5 : -0.5);
    return static_cast<int>(f + korr);
}

IMPLEMENT_DYNAMIC(CPlotterDC, CDC)

CPlotterDC::CPlotterDC()
{
    mTransformation.identity(3);
}

CPlotterDC::~CPlotterDC()
{
}

CPlotterDC::DrawCommand::DrawCommand(eCommand aType) : mType(aType)
{
}

CPlotterDC::DrawCommand::DrawCommand(const std::vector<FLOAT>&aPt, eCommand aType) :mPosition(aPt), mType(aType)
{
}

CPlotterDC::DrawCommand::DrawCommand(FLOAT aX, FLOAT aY, eCommand aType) : mType(aType)
{
    mPosition.push_back(aX);
    mPosition.push_back(aY);
}

void  CPlotterDC::clearDrawCommands()
{
    mDrawCommands.clear();
}

void  CPlotterDC::reset_transformation()
{
    mTransformation.identity(3);
}

void CPlotterDC::rotate(FLOAT aAngle)
{
    multibeam::matrix<FLOAT> fRotate;
    fRotate.identity(3);
    std::vector<FLOAT> v(3, 0);
    v[2] = 1;
    fRotate.rotate_z(aAngle);
    mTransformation = mTransformation * fRotate;
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
    multibeam::matrix<FLOAT> fScale;
    fScale.scale(aScale);
    mTransformation = mTransformation * fScale;
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
    multibeam::matrix<FLOAT> fTrans;
    fTrans.translate(aTranslation);
    mTransformation = mTransformation * fTrans;
}

void  CPlotterDC::transform_point(FLOAT& x, FLOAT& y)
{
    FLOAT temp = x * mTransformation[0][0] + y * mTransformation[0][1] + mTransformation[0][2];
             y = x * mTransformation[1][0] + y * mTransformation[1][1] + mTransformation[1][2];
    x = temp;
    TRACE("x: %f, y: %f\n", x, y);
}

// TODO! create MoveTo, LineTo, CurveTo, FillPath,... functions for X/Y in a Plotter device context derived from CDC

CPoint CPlotterDC::GetCurrentPosition() const
{
    return CDC::GetCurrentPosition();
}

CPoint CPlotterDC::MoveTo(FLOAT x, FLOAT y)
{
    transform_point(x, y);
    mDrawCommands.push_back(DrawCommand(x, y, DrawCommand::MoveTo));
    return CDC::MoveTo(round(x), round(y));
}

BOOL CPlotterDC::LineTo(FLOAT x, FLOAT y)
{
    transform_point(x, y);
    mDrawCommands.push_back(DrawCommand(x, y, DrawCommand::LineTo));
    return CDC::LineTo(round(x), round(y));
}

BOOL CPlotterDC::Arc(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3, FLOAT x4, FLOAT y4)
{
    return CDC::Arc(round(x1), round(y1), round(x2), round(y2),
       round(x3), round(y3), round(x4), round(y4));
}

BOOL CPlotterDC::Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
{
    return CDC::Arc(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
}

/*
BOOL CPlotterDC::Polyline(const POINT* lpPoints, int nCount)
{
    return CDC::Polyline(lpPoints, nCount);
}

BOOL CPlotterDC::AngleArc(int x, int y, int nRadius, FLOAT fStartAngle, FLOAT fSweepAngle)
{
    return CDC::AngleArc(x, y, nRadius, fStartAngle, fSweepAngle);
}
BOOL CPlotterDC::ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    return CDC::ArcTo(x1, y1, x2, y2, x3, y3, x4, y4);
}
BOOL CPlotterDC::ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
{
    return CDC::ArcTo(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
}
int CPlotterDC::GetArcDirection() const
{
    return CDC::GetArcDirection();
}
int CPlotterDC::SetArcDirection(int nArcDirection)
{
    return CDC::SetArcDirection(nArcDirection);
}

BOOL CPlotterDC::PolyDraw(const POINT* lpPoints, const BYTE* lpTypes, int nCount)
{
    return CDC::PolyDraw(lpPoints, lpTypes, nCount);
}
BOOL CPlotterDC::PolylineTo(const POINT* lpPoints, int nCount)
{
    return CDC::PolylineTo(lpPoints, nCount);
}
BOOL CPlotterDC::PolyPolyline(const POINT* lpPoints, const DWORD* lpPolyPoints, int nCount)
{
    return CDC::PolyPolyline(lpPoints, lpPolyPoints, nCount);
}
*/