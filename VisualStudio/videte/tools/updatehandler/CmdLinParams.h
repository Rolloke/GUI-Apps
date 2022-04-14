#if !defined(AFX_CMDLINPARAMS_H__B7AEA432_D2DF_4387_AB4A_674D04683C50__INCLUDED_)
#define AFX_CMDLINPARAMS_H__B7AEA432_D2DF_4387_AB4A_674D04683C50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdLinParams.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdLineParamDlg dialog

class CCmdLineParamDlg : public CDialog
{
// Construction
public:
	CCmdLineParamDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCmdLineParamDlg)
	enum { IDD = IDD_CMD_LINE_PARAMS };
	CString	m_sCmdLine;
	CString m_sHeadline;
	int m_nWaitTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdLineParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCmdLineParamDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDLINPARAMS_H__B7AEA432_D2DF_4387_AB4A_674D04683C50__INCLUDED_)
