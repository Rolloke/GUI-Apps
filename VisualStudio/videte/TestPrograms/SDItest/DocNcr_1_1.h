/* GlobalReplace: CSDITestNcrDoc --> CDocNcr_1_1 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocNcr_1_1.h $
// ARCHIVE:		$Archive: /PROJECT/tools/TstTools/sditest/DocNcr_1_1.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 22.04.99 9:09 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTNCRDOC_H__587945C5_A839_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTNCRDOC_H__587945C5_A839_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_1 document
class CDocNcr_1_1 : public CSDITestDoc
{
protected:
	CDocNcr_1_1();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocNcr_1_1)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	virtual void	CreateDataIntern();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocNcr_1_1)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocNcr_1_1();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocNcr_1_1)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int		m_iCommand;
	BOOL	m_bIncreaseTAN;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTNCRDOC_H__587945C5_A839_11D1_9F29_0000C036AC0D__INCLUDED_)
