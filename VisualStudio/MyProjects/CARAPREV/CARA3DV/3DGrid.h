// 3DGrid.h: Schnittstelle für die Klasse C3DGrid.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DGRID_H__D4B90FC3_F8B6_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_3DGRID_H__D4B90FC3_F8B6_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CARA3DDocFriend.h"

class CVector;

#define SHOW_AXIS                   0x00000001
#define SHOW_GRID                   0x00000002
#define SHOW_POLAR_GRID             0x00000004
#define SHOW_INTELLI_GRID           0x00000008

#define SHOW_X_UNIT1                0x00000010
#define SHOW_X_UNIT2                0x00000020
#define SHOW_X_UNIT3                0x00000030
#define SHOW_X_UNIT4                0x00000040
#define SHOW_X_UNIT                 0x000000F0

#define SHOW_Y_UNIT1                0x00000100
#define SHOW_Y_UNIT2                0x00000200
#define SHOW_Y_UNIT3                0x00000400
#define SHOW_Y_UNIT4                0x00000800
#define SHOW_Y_UNIT                 0x00000F00

#define SHOW_Z_UNIT1                0x00001000
#define SHOW_Z_UNIT2                0x00002000
#define SHOW_Z_UNIT3                0x00004000
#define SHOW_Z_UNIT4                0x00008000
#define SHOW_Z_UNIT                 0x0000F000

#define SHOW_X_UNIT_BOTTOM          0x00010000
#define SHOW_Y_UNIT_BOTTOM          0x00020000
#define SHOW_Z_UNIT_BOTTOM          0x00040000

#define SHOW_X_CHARACTER            0x00080000
#define SHOW_Y_CHARACTER            0x00100000
#define SHOW_Z_CHARACTER            0x00200000
#define SHOW_CHARACTER              0x00380000

#define SHOW_X_CHARACTER_BOTTOM     0x00400000
#define SHOW_X_CHARACTER_RIGHT      0x00800000

#define SHOW_Y_CHARACTER_BOTTOM     0x01000000
#define SHOW_Y_CHARACTER_RIGHT      0x02000000

#define SHOW_Z_CHARACTER_BOTTOM     0x04000000
#define SHOW_Z_CHARACTER_RIGHT      0x08000000

#define GRID_INVALID                0x10000000

class C3DGrid : public CCARA3DDocFriend
{
public:
   struct GridParam
   {
      int    nNumMode;
      int    nRounding;
      int    nNumModeL;
      int    nRoundingL;
      DWORD  dwAxis;
      double dXstep;
      double dZstep;
      double dLevelStep;
   };
	C3DGrid();
   // Zeichnen
	void  DrawAxes();
	void  DrawGrid();
	void  DrawText(HDC, int);
   // Datenzugriff
	void     SetAxes(DWORD);
	void     SetStep(double, double, double);
   bool     DoDrawAxes();
   bool     DoDrawGrid();
   DWORD    GetAxes();
   void     SetFont(LOGFONT lf)        {m_Font         = lf;}
   void     SetNumMode(int nM)         {m_nNumMode     = nM;}
   void     SetRounding(int nR)        {m_nRounding    = nR;}
   void     SetNumModeLevel(int nM)    {m_nNumModeL    = nM;}
   void     SetRoundingLevel(int nR)   {m_nRoundingL   = nR;}
   LOGFONT  GetFont()                  {return m_Font;}
   int      GetNumMode()               {return m_nNumMode;}
   int      GetRounding()              {return m_nRounding;}
   int      GetNumModeLevel()          {return m_nNumModeL;}
   int      GetRoundingLevel()         {return m_nRoundingL;}
   double   GetGridStepX()             {return m_dXstep;}
   double   GetGridStepZ()             {return m_dZstep;}
   double   GetGridStepLevel()         {return m_dLevelStep;}
   void     GetGridParam(C3DGrid::GridParam&);
   void     SetGridParam(C3DGrid::GridParam&);
   // Updatefunktionen
   void  InvalidateStep() {m_nXValues = -1;}
   void  InvalidateGrid() {m_dwNewAxis |= GRID_INVALID;}


private:
	void  CalculateSteps();
   int   CalcDiv(double &, double &, double &, double);
	void  CheckGridUnits(DWORD);
   void  GetAxlePositions(double&axe_x, double&axe_y, double&axe_ymin, double&axe_ymax, double&axe_z);

public:
   int      m_nCurrentID;

private:
	double   m_dXmin;
	double   m_dMinLevel;
	double   m_dZmin;
	double   m_dXmax;
	double   m_dMaxLevel;
	double   m_dZmax;
	double   m_dXstep;
	double   m_dLevelStep;
	double   m_dZstep;
   int      m_nXValues;
   int      m_nLevelValues;
   int      m_nZValues;
   DWORD    m_dwAxis;
   DWORD    m_dwNewAxis;
   LOGFONT  m_Font;
   int      m_nNumMode;
   int      m_nRounding;
   int      m_nNumModeL;
   int      m_nRoundingL;
};

#endif // !defined(AFX_3DGRID_H__D4B90FC3_F8B6_11D2_9E4E_0000B458D891__INCLUDED_)
