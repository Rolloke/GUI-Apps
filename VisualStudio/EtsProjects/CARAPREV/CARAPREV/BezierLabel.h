// BezierLabel.h: Schnittstelle für die Klasse CBezierLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIERLabel_H__F29F9982_EBB8_11D0_9DB9_B051418EA04A__INCLUDED_)
#define AFX_BEZIERLabel_H__F29F9982_EBB8_11D0_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Label.h"

class CFileHeader;

class AFX_EXT_CLASS CBezierLabel : public CLabel  
{
   DECLARE_SERIAL(CBezierLabel)
public:
   CBezierLabel();
   CBezierLabel(CPoint *);
   virtual ~CBezierLabel()
   {
      #ifdef CHECK_LABEL_DESTRUCTION
      TRACE("CBezierLabel::Destructor\n");
      #endif
   };
   virtual bool   Draw(CDC *, int bAtr = 1);
   virtual bool   DrawShape(CDC*, bool);
   virtual void   SetPoint(CPoint);
   virtual int    IsOnLabel(CDC*, CPoint *, CRect*);
   virtual void   Serialize(CArchive &);
   virtual CRect  GetRect(CDC*, UINT nFlags = 0);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif

   void     SetLogPen(LOGPEN *);
   LOGPEN   GetLogPen();
	static void ResetGlobalObjects();
protected:
   CPen  *m_pPen;

private:
   void Init(CPoint*);
   static CPen *gm_pPen;
};

#endif // !defined(AFX_BEZIERLabel_H__F29F9982_EBB8_11D0_9DB9_B051418EA04A__INCLUDED_)
