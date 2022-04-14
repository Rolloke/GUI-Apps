/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocSniCom.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/DocSniCom.h $
// CHECKIN:		$Date: 18.06.99 12:57 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 18.06.99 12:07 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_DOCSNICOM_H__CD112DF1_255D_11D3_BA46_00400531137E__INCLUDED_)
#define AFX_DOCSNICOM_H__CD112DF1_255D_11D3_BA46_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocSniCom document
class CDocSniCom : public CSDITestDoc  
{
public:
	CDocSniCom();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocSniCom)
// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocSniCom)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocSniCom();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocSniCom)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
protected:
	CStringArray	m_sDataArray;
	int				m_iDataIndex;
	int				m_iLastDataIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCSNICOM_H__CD112DF1_255D_11D3_BA46_00400531137E__INCLUDED_)
