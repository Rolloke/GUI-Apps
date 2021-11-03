// ColorLabel.h: Schnittstelle für die Klasse CColorLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORLABEL_H__19D80E40_EC59_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_COLORLABEL_H__19D80E40_EC59_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RectLabel.h"
#include "ScalePainter.h"	// Hinzugefügt von der Klassenansicht

class CColorArray;
class CFileHeader;

class AFX_EXT_CLASS CColorLabel : public CRectLabel  
{
   DECLARE_SERIAL(CColorLabel)
public:
	CColorLabel();
	bool           Attach(CColorArray*);
	CColorArray*   Detach();

   virtual       ~CColorLabel();
   virtual  bool  Draw(CDC *, int bAtr = 1);
   virtual  void  Serialize(CArchive &);

#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif
   // Temporäre Parameter
	void        SetUnit(TCHAR*);
	void        SetMinMaxLevel(double, double);
   // Darstellungsparameter set
   void        SetTextColor(COLORREF c) {m_ScalePainter.m_TextColor = c;};
   void        SetStepWidth(int n)      {m_ScalePainter.m_Div.stepwidth = n;}
   void        SetNumMode(int n)        {m_ScalePainter.m_Ntx.nmode = n;};
   void        SetRound(int n)          {m_ScalePainter.m_Ntx.nround = n;};
   void        SetLogFont(LOGFONT lf)   {m_ScalePainter.m_LogFont = lf;};
   void        SetShowScale(bool b)     {m_bShowScale = b;};
	bool        SetNumColors(int n);
	bool        SetColors(COLORREF*pc, int n);

   // Darstellungsparameter get
   COLORREF    GetTextColor()           {return m_ScalePainter.m_TextColor;};
   int         GetStepWidth()           {return m_ScalePainter.m_Div.stepwidth;}
   int         GetNumMode()             {return m_ScalePainter.m_Ntx.nmode;};
   int         GetRound()               {return m_ScalePainter.m_Ntx.nround;};
   LOGFONT     GetLogFont()             {return m_ScalePainter.m_LogFont;};
   bool        GetShowScale()           {return m_bShowScale;};
   bool        IsHorizontal()           {return m_ScalePainter.m_bHorz;};
   double      GetMinLevel()            {return m_ScalePainter.m_Div.f1;};
   double      GetMaxLevel()            {return m_ScalePainter.m_Div.f2;};
	CSize       GetMinScaleSize(CDC*pDC);
	int         GetNumColors();
	bool        GetColors(COLORREF *pc, int n);
private:
	CScalePainter  m_ScalePainter;
	CColorArray*   m_pColorArray;
   bool           m_bShowScale;
};

#endif // !defined(AFX_COLORLABEL_H__19D80E40_EC59_11D2_9E4E_0000B458D891__INCLUDED_)
