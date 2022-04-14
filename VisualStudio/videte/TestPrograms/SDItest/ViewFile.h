#if !defined(AFX_VIEWFILE_H__FCF4EAB0_7FE0_11D3_99D2_004005A19028__INCLUDED_)
#define AFX_VIEWFILE_H__FCF4EAB0_7FE0_11D3_99D2_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocFile.h"

/////////////////////////////////////////////////////////////////////////////
// CViewFile form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CViewFile : public CSDITestView
{
protected:
	CViewFile();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewFile)

// Form Data
public:
	//{{AFX_DATA(CViewFile)
	enum { IDD = IDD_FILE_FORM };
	CString	m_sPathName;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewFile();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewFile)
	afx_msg void OnBrowse();
	afx_msg void OnCom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWFILE_H__FCF4EAB0_7FE0_11D3_99D2_004005A19028__INCLUDED_)
