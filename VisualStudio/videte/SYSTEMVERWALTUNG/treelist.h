/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: treelist.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/treelist.h $
// CHECKIN:		$Date: 31.07.97 13:19 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 29.07.97 14:13 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#ifndef _CTREE_LIST_H
#define _CTREE_LIST_H

/////////////////////////////////////////////////////////////////////////////
// CTreeList window

class CTreeList : public CTreeCtrl
{
// Construction
public:
	CTreeList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeList)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
