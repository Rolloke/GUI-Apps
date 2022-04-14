// LineLabel.h: Schnittstelle für die Klasse CLineLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined AFX_LINELabel_H__4D4A7EE3_E98E_11D0_9DB9_B051418EA04A__INCLUDED_
#define AFX_LINELabel_H__4D4A7EE3_E98E_11D0_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Label.h"

class CFileHeader;

class AFX_EXT_CLASS CLineLabel : public CLabel  
{
   DECLARE_SERIAL(CLineLabel)
public:
   CLineLabel();
   CLineLabel(CPoint *);
   virtual ~CLineLabel();
   virtual bool   Draw(CDC *, int bAtr = 1);
   virtual bool   DrawShape(CDC *, bool);
   virtual int    IsOnLabel(CDC *, CPoint *, CRect *);
   virtual void   Serialize(CArchive &);
   virtual CRect  GetRect(CDC * pDC, UINT nFlags = 0);
#ifdef _DEBUG
   virtual void AssertValid() const;
#endif

   void    SetLogPen(LOGPEN *);
   LOGPEN  GetLogPen();
   void    SetdValue(double dV) {m_dValue = dV;};
   double  GetdValue()          {return m_dValue;};
	void    SetDDAPen(int, TCHAR*, COLORREF);
	static void ResetGlobalObjects();

private:
   void           Init(CPoint *);
   sLineDDA       m_LDDA;
   double         m_dValue;
   CPen          *m_pPen;
   static CPen   *gm_pPen;
};

#endif // !defined(AFX_LINELabel_H__4D4A7EE3_E98E_11D0_9DB9_B051418EA04A__INCLUDED_)
