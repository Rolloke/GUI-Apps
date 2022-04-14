#if !defined(AFX_DEFAULTTREEKNODE_H__81E20B52_A2F3_4E2E_9F28_CA369B03C3FD__INCLUDED_)
#define AFX_DEFAULTTREEKNODE_H__81E20B52_A2F3_4E2E_9F28_CA369B03C3FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefaultTreeKnode.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CDefaultTreeKnode dialog
#define MAX_CHILDITEMS 11

class CDefaultTreeKnode : public CSVPage
{
// Construction
public:
	CDefaultTreeKnode(CSVView* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CDefaultTreeKnode)

// Dialog Data
	//{{AFX_DATA(CDefaultTreeKnode)
	enum { IDD = IDD_DEF_TREE_KNODE };
	CString	m_strDscr;
	//}}AFX_DATA
	CString	m_strItem[MAX_CHILDITEMS];
	HTREEITEM m_hSelected;

public:
	void Create();
	CString m_strGroup;
/*
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
*/
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefaultTreeKnode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefaultTreeKnode)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
/*
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();
*/ 
private:
   static LRESULT CALLBACK SubClassProc(HWND,UINT, WPARAM, LPARAM);
   CFont  m_FontSelected;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFAULTTREEKNODE_H__81E20B52_A2F3_4E2E_9F28_CA369B03C3FD__INCLUDED_)
