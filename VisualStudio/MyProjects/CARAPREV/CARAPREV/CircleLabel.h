// CircleLabel.h: Schnittstelle für die Klasse CCircleLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIRCLELABEL_H__33841A64_1661_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CIRCLELABEL_H__33841A64_1661_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Label.h"

class CFileHeader;

class AFX_EXT_CLASS CCircleLabel : public CLabel  
{
   DECLARE_SERIAL(CCircleLabel)
public:
   CCircleLabel();
   CCircleLabel(CPoint *);
   CCircleLabel(CRect *);
   virtual ~CCircleLabel()
   {
      #ifdef CHECK_LABEL_DESTRUCTION
      TRACE("CCircleLabel::Destructor\n");
      #endif
   };
   virtual bool   Draw (CDC *, int bAtr = 1);
   virtual bool   DrawShape(CDC *, bool);
   virtual void   SetPoint(CPoint);
   virtual int    IsOnLabel(CDC*, CPoint *, CRect*);
   virtual void   Serialize(CArchive &);
   virtual bool   GetRgn(int, CDC *, CRgn *, UINT nFlags = 0);
   virtual CRect  GetRect(CDC *pDC, UINT nFlags = 0);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif

   void     SetLogPen(LOGPEN *);
   LOGPEN   GetLogPen();
   void     SetLogBrush(LOGBRUSH *);
   LOGBRUSH GetLogBrush();
   void     SetdValue(double dV) {m_dValue = dV;};
   double   GetdValue()          {return m_dValue;};
	static void ResetGlobalObjects();

protected:
   CPen   *m_pPen;
   CBrush *m_pBrush;


private:
   void Init(CPoint*, int);
   double         m_dValue;
   static CPen    *gm_pPen;
   static CBrush  *gm_pBrush;
   static COLORREF gm_BKColor;
};

#endif // !defined(AFX_CIRCLELABEL_H__33841A64_1661_11D2_9DB9_0000B458D891__INCLUDED_)
