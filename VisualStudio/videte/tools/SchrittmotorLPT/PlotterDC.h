#pragma once
#include "afxwin.h"
#include "matrix.h"

typedef float FLOAT; 

int round(FLOAT f);

class CPlotterDC : public CDC
{
	DECLARE_DYNAMIC(CPlotterDC)
public:
    struct DrawCommand
    {
        enum eCommand { MoveTo, LineTo, Drill };
        DrawCommand(eCommand aType);
        DrawCommand(const std::vector<float>&aPt, eCommand aType);
        DrawCommand(float aX, float aY, eCommand aType);
        std::vector<float>  mPosition;
        eCommand mType;
    };
    CPlotterDC();
    ~CPlotterDC();

    const std::vector<DrawCommand>& getDrawCommands() { return mDrawCommands; };
    void  clearDrawCommands();

    const multibeam::matrix<float>& getTransformation() { return mTransformation; };
    void  reset_transformation();
    void  rotate(float aAngle);
    void  scale(const std::vector<float>& aScale);
    void  scale(float x, float y);
    void  translate(const std::vector<float>& aScale);
    void  translate(float x, float y);
    void  transform_point(float& x, float& y);

	CPoint GetCurrentPosition() const;
	CPoint MoveTo(float x, float y);
	BOOL LineTo(float x, float y);
	BOOL Arc(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	BOOL Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd);

	//BOOL Polyline(const POINT* lpPoints, int nCount);

	//BOOL AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle);
	//BOOL ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	//BOOL ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
	//int GetArcDirection() const;
	//int SetArcDirection(int nArcDirection);

	//BOOL PolyDraw(const POINT* lpPoints, const BYTE* lpTypes, int nCount);
	//BOOL PolylineTo(const POINT* lpPoints, int nCount);
	//BOOL PolyPolyline(const POINT* lpPoints,
	//	const DWORD* lpPolyPoints, int nCount);

private:
    std::vector<DrawCommand> mDrawCommands;
    multibeam::matrix<float> mTransformation;
};
