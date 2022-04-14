#if !defined(AFX_CRTERRORDLG_H__6B2452FA_AC6F_11D3_8E06_004005A11E32__INCLUDED_)
#define AFX_CRTERRORDLG_H__6B2452FA_AC6F_11D3_8E06_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CRTErrorDlg.h : header file
//
#define RTE_CONFIRMED _T("/RTE Confirmed/")

#define MAX_RTE 1000
#define RTE_FILENAME _T("rterrors.dat")
#define RTE_HDDFAILED_PATH _T("HDDFailed")

/////////////////////////////////////////////////////////////////////////////
// CRTErrorDlg dialog
class CRTErrorDlg : public CTransparentDialog
{
// Construction
public:
	BOOL Create();
	CRTErrorDlg(CWK_RunTimeError* pLastError);

// Dialog Data
	//{{AFX_DATA(CRTErrorDlg)
	enum { IDD = IDD_RTERROR_DLG };
	CFrame		m_ctrlBorderRTError;
	CSkinButton	m_ctrlButtonOk;
	CDisplay	m_ctrlDisplayRTError;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTErrorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRTErrorDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void StartCountdownThread();
	void AddRTEToRTEFile(BOOL bConfirmed, CString sPathToRTEFile = _T(""));

private:
	static UINT CountdownThread(LPVOID pData);

private:
	CWK_RunTimeError*	m_pLastError;
	CString				m_sErrorText;
	CWinThread*			m_pCountdownThread;
	CImageList			m_ilConfirm;
	BOOL				m_bRun;
	BOOL				m_bEnableCountdown;
	HCURSOR				m_hCursor;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRTERRORDLG_H__6B2452FA_AC6F_11D3_8E06_004005A11E32__INCLUDED_)
