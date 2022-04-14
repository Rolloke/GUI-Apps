#if !defined(AFX_DlgOptions_H__F600BB13_7098_4EDD_8FF8_956853002BC9__INCLUDED_)
#define AFX_DlgOptions_H__F600BB13_7098_4EDD_8FF8_956853002BC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions dialog

class CDlgOptions : public CSkinDialog
{
// Construction
public:
	CDlgOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOptions)
	enum  { IDD = IDD_OPTIONS };
	BOOL	m_b1PlusAlarm;
	BOOL	m_bAudibleAlarm;
	BOOL	m_bFullScreenAlarm;
	BOOL	m_bFirstCam;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOptions)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgOptions_H__F600BB13_7098_4EDD_8FF8_956853002BC9__INCLUDED_)
