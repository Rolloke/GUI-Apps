#if !defined(AFX_AVIPLAYERDLG_H__8210B413_6F64_11D4_BAE7_00400531137E__INCLUDED_)
#define AFX_AVIPLAYERDLG_H__8210B413_6F64_11D4_BAE7_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AviPlayerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAviPlayerDlg dialog

class CAviPlayerDlg : public CDialog
{
// Construction
public:
	CAviPlayerDlg(CString sFilename, WORD wMaxFrames, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAviPlayerDlg();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAviPlayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	EnableControls();

	// Generated message map functions
	//{{AFX_MSG(CAviPlayerDlg)
	afx_msg void OnBtnStop();
	afx_msg void OnBtnPlayOnce();
	afx_msg void OnBtnPlayLoop();
	afx_msg void OnBtnStepBack();
	afx_msg void OnBtnStepFor();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnPause();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	//{{AFX_DATA(CAviPlayerDlg)
	enum { IDD = IDD_PLAYER };
	CButton	m_btnPause;
	CButton	m_btnStop;
	CButton	m_btnStepFor;
	CButton	m_btnStepBack;
	CButton	m_btnPlayLoop;
	CButton	m_btnPlayOnce;
	CAnimateCtrl	m_Animation;
	CString	m_sAviFile;
	CString	m_sCurFrame;
	CString	m_sMaxFrames;
	//}}AFX_DATA
	HBITMAP		m_hStop;
	HBITMAP		m_hPause;
	HBITMAP		m_hPlayOnce;
	HBITMAP		m_hPlayLoop;
	HBITMAP		m_hStepBack;
	HBITMAP		m_hStepFor;
	WORD		m_wCurFrame;
	WORD		m_wMaxFrames;
	BOOL		m_bPlaying;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVIPLAYERDLG_H__8210B413_6F64_11D4_BAE7_00400531137E__INCLUDED_)
