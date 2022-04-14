/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: CntrItem.h $
// ARCHIVE:		$Archive: /Project/Tools/LogView/CntrItem.h $
// CHECKIN:		$Date: 16.04.99 12:51 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 16.04.99 12:51 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CNTRITEM_H__4DB08FF1_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
#define AFX_CNTRITEM_H__4DB08FF1_BC2E_11D2_A9C4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogViewDoc;
class CLogViewView;

class CLogViewCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CLogViewCntrItem)

// Constructors
public:
	CLogViewCntrItem(REOBJECT* preo = NULL, CLogViewDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CLogViewDoc* GetDocument()
		{ return (CLogViewDoc*)CRichEditCntrItem::GetDocument(); }
	CLogViewView* GetActiveView()
		{ return (CLogViewView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CLogViewCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__4DB08FF1_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
