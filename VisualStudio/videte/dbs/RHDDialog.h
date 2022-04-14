#if !defined(AFX_RHDDIALOG_H__2FA39803_25A3_4150_BB07_83E77A6FEF3D__INCLUDED_)
#define AFX_RHDDIALOG_H__2FA39803_25A3_4150_BB07_83E77A6FEF3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RHDDialog.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CRHDDialog dialog

class CRHDDialog : public CDialog
{
// Construction
public:
	CRHDDialog(BOOL bStart, CIPCDrive* pDrive, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRHDDialog)
	enum { IDD = IDD_DIALOG_REMOVEABLE_HDD };
	CProgressCtrl	m_ctrlProgress;
	CStatic	m_txtToWrite;
	CButton	m_ctrlToWrite;
	CStatic	m_txtToRO;
	CButton	m_ctrlToRO;
	CStatic	m_ctrlDrive;
	//}}AFX_DATA

public:
	inline BOOL GetROToRW() const;
	inline BOOL GetRWToRO() const;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRHDDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRHDDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnButtonToRo();
	afx_msg void OnButtonToWrite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bStart;
	BOOL	m_bIsEmpty;
	BOOL	m_bIsRO;
	BOOL	m_bIsWrite;
	CIPCDrive* m_pDrive;
	UINT	m_uTimer;
	int		m_iAction;
};
///////////////////////////////////////////////////////////////////////////
inline BOOL CRHDDialog::GetROToRW() const
{
	return m_iAction == 2;
}
///////////////////////////////////////////////////////////////////////////
inline BOOL CRHDDialog::GetRWToRO() const
{
	return m_iAction == 1;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RHDDIALOG_H__2FA39803_25A3_4150_BB07_83E77A6FEF3D__INCLUDED_)
