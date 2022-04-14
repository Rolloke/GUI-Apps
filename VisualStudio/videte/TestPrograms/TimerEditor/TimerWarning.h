#if !defined(AFX_TIMERWARNING_H__1F2E4E14_793B_11D2_B96B_00C095ECA33E__INCLUDED_)
#define AFX_TIMERWARNING_H__1F2E4E14_793B_11D2_B96B_00C095ECA33E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerWarning.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimerWarning dialog

class CTimerWarning : public CDialog
{
// Construction
public:
	CTimerWarning(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimerWarning)
	enum { IDD = IDD_TIMER_WARNING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerWarning)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerWarning)
	afx_msg void OnTimerOk();
	afx_msg void OnTimerCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERWARNING_H__1F2E4E14_793B_11D2_B96B_00C095ECA33E__INCLUDED_)
