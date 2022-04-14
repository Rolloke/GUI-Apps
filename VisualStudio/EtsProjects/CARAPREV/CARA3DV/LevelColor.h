// LevelColor.h: Schnittstelle für die Klasse CLevelColor.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVELCOLOR_H__F2EB3EE0_FBC8_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_LEVELCOLOR_H__F2EB3EE0_FBC8_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CARA3DDocFriend.h"
#include "..\ColorArray.h"

class CColorView;
class CLevelColor : public CColorArray, public CCARA3DDocFriend
{
public:
	CLevelColor();
	bool     ReadColors(CArchive &);
	void     WriteColors(CArchive&);
	COLORREF GetColorFromLevel(double fLevel);
   double   GetMinLevel() {return m_dMinLevel;}
   double   GetMaxLevel() {return m_dMaxLevel;}
	void     SetMinMaxLevel(double dMin, double dMax);
   void     Invalidate() {m_bValid = false;}
   void     operator =(CColorArray &);
   void     operator =(CColorView &);

private:
	void     Validate();
                              // y =  m * x + b
   double   m_dOrigin;        // => b
   double   m_dGradient;      // => m
   double   m_dMinLevel;      // Minimaler FarbLevel
   double   m_dMaxLevel;      // Maximaler FarbLevel
   bool     m_bValid;         // Geradengleichung gültig
};

#endif // !defined(AFX_LEVELCOLOR_H__F2EB3EE0_FBC8_11D2_9E4E_0000B458D891__INCLUDED_)
