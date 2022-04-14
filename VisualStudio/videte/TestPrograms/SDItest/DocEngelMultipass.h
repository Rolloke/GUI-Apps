/* GlobalReplace: CSDITestEngelDoc --> CDocEngelMultipass */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocEngelMultipass.h $
// ARCHIVE:		$Archive: /PROJECT/tools/TstTools/sditest/DocEngelMultipass.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 22.04.99 9:05 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTENGELDOC_H__FAD19762_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
#define AFX_SDITESTENGELDOC_H__FAD19762_AC61_11D1_8143_DB20799D17F6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocEngelMultipass document
class CDocEngelMultipass : public CSDITestDoc
{
protected:
	CDocEngelMultipass();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocEngelMultipass)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocEngelMultipass)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocEngelMultipass();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocEngelMultipass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iCommand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTENGELDOC_H__FAD19762_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
