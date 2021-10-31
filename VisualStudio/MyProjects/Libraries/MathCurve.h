// MathCurve.h: Schnittstelle für die Klasse CMathCurve.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHCURVE_H__CA0126C2_01B0_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_MATHCURVE_H__CA0126C2_01B0_11D1_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Curve.h"
#include "Calcmplx.h"

#define     MC_ADD   1
#define     MC_SUB   2
#define     MC_MUL   3
#define     MC_DIV   4
#define     MC_POW   5
#define     MC_LOG   6
#define     MC_LOG10 7
#define     MC_EPS   1e-13
#define     CURVE_PROGRESS_RANGE 128

struct SInterpolynom
{ 
   double c0, c1, c2,
          s0,s1;
};

struct SCInterpolynom
{ 
   Complex c0, c1, c2,
          s0,s1;
};

#if defined CARA_MFC_HEADER
class AFX_EXT_CLASS CMathCurve : public CCurve  
#else
class CMathCurve : public CCurve  
#endif
{
public:
   CMathCurve()  {m_hProgressWnd=NULL;/*TRACE("CMathCurve::Constructor()\n");*/};
   CMathCurve(CMathCurve *pmc) : CCurve(pmc) {/*TRACE("CMathCurve::Constructor()\n");*/};
   virtual ~CMathCurve() {};
   void        SortXAsc();                                     // Sortier Funktion
   void SetProgressWnd(HWND hwnd) {m_hProgressWnd=hwnd;};
   // Operationen
   bool        OperateValue(int, double, CMathCurve *pmc=NULL);
   bool        OperateCurve(int, CMathCurve *, CMathCurve *);
   bool        Derive_X(CMathCurve *pmc=NULL);
   bool        Integrate_X(double, CMathCurve *pmc=NULL);
   bool        Average(double, double, CMathCurve *);
   bool        Spline(int, CMathCurve *, double x1=0, double x2=0);
   double      Average(int, double x1=0, double x2=0);
   double      Integrate(int, double x1=0, double x2=0);
   double      GetMinXStep();
	double      Get_X_Value(double);
   double      Get_Y_Value(double x, int *startindex=NULL);
   double      Slope(double, double &, int *startindex=NULL);
   bool        MakeInterpolynom(int, int, int, SInterpolynom*);
   bool        MakeInterpolynom(int, int, int, SCInterpolynom*);
   double      PolynomValue(double x, SInterpolynom *pip);
   Complex     PolynomValue(double x, SCInterpolynom *pip);
	void        MultiplyUnit(char *pszUnit);
	void        DivideUnit(  char *pszUnit);
	void        SplitUnit(   char *pszUnit, char *pszAbove, char *pszUnder);
	bool        MultiplyUnitPart(char *pszPart, char* pszUnit);
	bool        DivideUnitPart(  char *pszPart, char* pszUnit);
	bool        SetComplexValue(int, Complex&);
	Complex     GetComplexValue(int);
	SCurve      Get_Locus_Value(double, int *pnIndex=NULL);
   private:
   HWND  m_hProgressWnd;
   static int SCurvexSortfunc( const void *, const void *);
   static int SCurvexSortfuncF(const void *, const void *);
   static int SCurvexSortfuncS(const void *, const void *);
};

#endif // !defined(AFX_MATHCURVE_H__CA0126C2_01B0_11D1_9DB9_B051418EA04A__INCLUDED_)
