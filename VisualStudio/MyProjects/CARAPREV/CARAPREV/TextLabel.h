// TextLabel.h: Schnittstelle für die Klasse CTextLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTLabel_H__64D220E2_01C6_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_TEXTLabel_H__64D220E2_01C6_11D1_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Label.h"

class CFileHeader;

#define TA_VCENTER 0x0028

#if _MFC_VER < 0x0710
class CEtsString : public CString
{
   public:
      bool   Attach(LPTSTR);
      LPTSTR Detach();
};
#endif
class AFX_EXT_CLASS CTextLabel : public CLabel
{
   DECLARE_SERIAL(CTextLabel)
public:
   CTextLabel();
   CTextLabel(CPoint *, const TCHAR*);
   CTextLabel(CRect *, const TCHAR*);
   CTextLabel(CTextLabel*);
   virtual ~CTextLabel();
   virtual  bool  Draw(CDC *, int bAtr = 1);
   virtual  bool  DrawShape(CDC *, bool);
   virtual  CRect GetRect(CDC *pDC, UINT nFlags=0);
   virtual  void  Serialize(CArchive &);
   virtual  void  SetPoint(CPoint);
   virtual  void  MoveLabel(CPoint);
   virtual  void  ScaleLabel(CSize, CSize);
#ifdef _DEBUG
      virtual void AssertValid() const;
#endif

   void           SetChar(UINT);
   void           SetText(const TCHAR *);
   const TCHAR*   GetText() {return LPCTSTR(m_strText);};
   CString&       GetString() {m_State.textchanged=true; return m_strText;};
   bool           ReadText( CArchive&, WORD, bool bForce = false);
   void           WriteText(CArchive&, WORD);
   const CFont*   GetFont() {return m_pFont;}

   void           SetTextColor(COLORREF c);
   COLORREF       GetTextColor()           {return m_Color;};
   void           SetTextAlign(UINT align);
   UINT           GetTextAlign()           {return m_nAlign;};
   void           SetClipRect(CRect*prcClip, bool bClip = false);
   void           SetLogFont(LOGFONT*);
   LOGFONT        GetLogFont();
   void           SetLogBrush(LOGBRUSH *);
   int            GetExtraSpacing() {return m_nExtraSpacing;};
   void           SetExtraSpacing(int);
   LOGBRUSH       GetLogBrush();
   void           SetLogPen(LOGPEN *);
   LOGPEN         GetLogPen();
   bool           SetStringLength(int);
   void           SetTextChanged()     {m_State.textchanged  = true;};
   void           InvalidateTextRect() {m_State.calctextrect = true;};
   bool           IsEmpty() {return (m_strText.IsEmpty()!=0) ? true : false;};

   static int     ClipTextRect(CLabel*, void*);
   static void    ResetGlobalObjects();

private:
   BOOL           DrawMultiLineText(CDC*, bool bCalcRect=false);
   UINT           MultiTextLineFormat();
   void           Init(CPoint*, const TCHAR *);
protected:
   COLORREF          m_Color;       // Textattribute
   static COLORREF   gm_Color;
   CFont            *m_pFont;
   static CFont     *gm_pFont;
   CPen             *m_pPen;
   static CPen      *gm_pPen;
   CBrush           *m_pBrush;
   static CBrush    *gm_pBrush;
   UINT              m_nAlign;
   static UINT       gm_nAlign;
   static COLORREF   gm_BKColor;
   int               m_nExtraSpacing;
   static int        gm_nExtraSpacing;
   static BSState    gm_TextStates;

   CRect             m_rcClip;
   CString           m_strText;
   int               m_nTextCursor;
};

#endif // !defined(AFX_TEXTLabel_H__64D220E2_01C6_11D1_9DB9_B051418EA04A__INCLUDED_)
