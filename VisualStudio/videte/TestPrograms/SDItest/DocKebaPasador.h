/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocKebaPasador.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/DocKebaPasador.h $
// CHECKIN:		$Date: 25.08.99 16:02 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.08.99 13:31 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_DOCKEBAPASADOR_H__B8F7F034_5AD9_11D3_BA6E_00400531137E__INCLUDED_)
#define AFX_DOCKEBAPASADOR_H__B8F7F034_5AD9_11D3_BA6E_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDITestDoc.h"

class CDocKebaPasador : public CSDITestDoc  
{
protected:
	CDocKebaPasador();
	DECLARE_DYNCREATE(CDocKebaPasador)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocKebaPasador)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocKebaPasador();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocKebaPasador)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString	m_sFileName;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_DOCKEBAPASADOR_H__B8F7F034_5AD9_11D3_BA6E_00400531137E__INCLUDED_)
