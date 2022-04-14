#if !defined(AFX_SYSTEMPAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_SYSTEMPAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SoftwarePage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSoftwarePage dialog

class CSoftwarePage : public CSVPage
{
	DECLARE_DYNAMIC(CSoftwarePage)

// Construction
public:
	CSoftwarePage(CSVView*  pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSoftwarePage)
	enum { IDD = IDD_SOFTWARE };
	CString	m_sProduct;
	CString	m_sVersion;
	CString	m_strText;
	//}}AFX_DATA

public:
	void Create();
	virtual BOOL CanNew();
	virtual BOOL CanDelete();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftwarePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSoftwarePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

private:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMPAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_)
