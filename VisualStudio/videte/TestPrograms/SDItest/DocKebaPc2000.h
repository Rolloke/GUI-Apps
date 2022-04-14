/* GlobalReplace: CSDITestKebaDoc --> CDocKebaPc2000 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocKebaPc2000.h $
// ARCHIVE:		$Archive: /PROJECT/tools/TstTools/sditest/DocKebaPc2000.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 22.04.99 9:08 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTKEBADOC_H__B68595A6_A44D_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTKEBADOC_H__B68595A6_A44D_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocKebaPc2000 document
class CDocKebaPc2000 : public CSDITestDoc
{
protected:
	CDocKebaPc2000();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocKebaPc2000)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocKebaPc2000)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocKebaPc2000();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern(CString sCardReader);

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocKebaPc2000)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iCardReader;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTKEBADOC_H__B68595A6_A44D_11D1_9F29_0000C036AC0D__INCLUDED_)
