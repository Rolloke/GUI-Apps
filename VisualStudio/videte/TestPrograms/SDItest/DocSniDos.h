/* GlobalReplace: CSDITestSniDoc --> CDocSniDos */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocSniDos.h $
// ARCHIVE:		$Archive: /PROJECT/tools/TstTools/sditest/DocSniDos.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 22.04.99 9:11 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTSNIDOC_H__FAD19764_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
#define AFX_SDITESTSNIDOC_H__FAD19764_AC61_11D1_8143_DB20799D17F6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocSniDos document
class CDocSniDos : public CSDITestDoc
{
protected:
	CDocSniDos();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocSniDos)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocSniDos)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocSniDos();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocSniDos)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iPointOfData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTSNIDOC_H__FAD19764_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
