#if !defined(AFX_STMPAGE_H__EECD7FA2_186A_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_STMPAGE_H__EECD7FA2_186A_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// STMPage.h : header file
//

#include "SVPage.h"

class COutputList;
/////////////////////////////////////////////////////////////////////////////
// CSTMPage dialog

class CSTMPage : public CSVPage
{
// Construction
public:
	CSTMPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSTMPage)
	enum { IDD = IDD_STM };
	BOOL	m_bSTM;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTMPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSTMPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckDisturb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COutputList*			m_pOutputList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STMPAGE_H__EECD7FA2_186A_11D1_93E0_00C095ECA33E__INCLUDED_)
