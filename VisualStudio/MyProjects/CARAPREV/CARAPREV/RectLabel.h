// RectLabel.h: Schnittstelle für die Klasse CRectLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined AFX_RECTLabel_H__4D4A7EE4_E98E_11D0_9DB9_B051418EA04A__INCLUDED_
#define AFX_RECTLabel_H__4D4A7EE4_E98E_11D0_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Label.h"
class CFileHeader;

class AFX_EXT_CLASS CRectLabel : public CLabel  
{
   DECLARE_SERIAL(CRectLabel)
public:
   CRectLabel();
   CRectLabel(CPoint *);
   virtual  ~CRectLabel() 
   {
      #ifdef CHECK_LABEL_DESTRUCTION
      TRACE("CRectLabel::Destructor\n");
      #endif
   };
   virtual  bool  Draw(CDC *, int bAtr = 1);
   virtual  bool  DrawShape(CDC *, bool);
   virtual  void  Serialize(CArchive &);
   virtual  CRect GetRect(CDC * pDC=NULL, UINT nFlags = 0);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif

   void     SetRect(CRect &rc);
   void     SetLogPen(LOGPEN *);
   LOGPEN   GetLogPen();
   void     SetLogBrush(LOGBRUSH *);
   LOGBRUSH GetLogBrush();
	static void ResetGlobalObjects();

protected:
   void           Init(CPoint *);

   CBrush          *m_pBrush;
   CPen            *m_pPen;
   static CPen     *gm_pPen;
   static CBrush   *gm_pBrush;
   static COLORREF  gm_BKColor;
};

#endif // !defined(AFX_RECTLabel_H__4D4A7EE4_E98E_11D0_9DB9_B051418EA04A__INCLUDED_)
