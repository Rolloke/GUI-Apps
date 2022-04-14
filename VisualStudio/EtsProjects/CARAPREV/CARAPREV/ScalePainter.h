// ScalePainter.h: Schnittstelle für die Klasse CScalePainter.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALEPAINTER_H__65FFF0E1_1DB3_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_SCALEPAINTER_H__65FFF0E1_1DB3_11D3_B6EC_0000B458D891__INCLUDED_

#include "CEtsDiv.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CScalePainter  
{
public:
	CScalePainter();

public:
	void  Draw(CDC *, CRect*);
	CSize GetMinSize(CDC*);
	void  SetUnit(TCHAR*psz);

   ETSDIV_DIV     m_Div;
   ETSDIV_NUTOTXT m_Ntx;
   LOGFONT     m_LogFont;
	bool        m_bHorz;
   COLORREF    m_TextColor;
private:
   CString     m_strUnit;

   struct DrawScale
   {
      CDC   *pDC;
      CRect *prcOut;
      POINT  ptLine;
      bool   bNegY;
   };

	static bool DrawHorizontal(ETSDIV_SCALE*);
	static bool DrawVertikal(ETSDIV_SCALE*);
};

#endif // !defined(AFX_SCALEPAINTER_H__65FFF0E1_1DB3_11D3_B6EC_0000B458D891__INCLUDED_)
