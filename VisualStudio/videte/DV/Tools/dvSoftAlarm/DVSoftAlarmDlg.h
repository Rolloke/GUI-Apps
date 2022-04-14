// DVSoftAlarmDlg.h : header file
//

#if !defined(AFX_DVSOFTALARMDLG_H__9ACE97E5_A2BF_4125_9F3D_74A0B7A6B83C__INCLUDED_)
#define AFX_DVSOFTALARMDLG_H__9ACE97E5_A2BF_4125_9F3D_74A0B7A6B83C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDVSoftAlarmDlg dialog

class CDVSoftAlarmDlg : public CDialog
{
// Construction
public:
	CDVSoftAlarmDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDVSoftAlarmDlg)
	enum { IDD = IDD_DVSOFTALARM_DIALOG };
	CStatic	m_Icon;
	UINT	m_x;
	UINT	m_y;
	int m_nWAI;
	int m_nRTE;
	int m_nREL;
	int m_nParam1;
	int m_nParam2;
	CString m_sErrorText;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVSoftAlarmDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDVSoftAlarmDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCheckS1();
	afx_msg void OnCheckS10();
	afx_msg void OnCheckS11();
	afx_msg void OnCheckS12();
	afx_msg void OnCheckS13();
	afx_msg void OnCheckS14();
	afx_msg void OnCheckS15();
	afx_msg void OnCheckS16();
	afx_msg void OnCheckS2();
	afx_msg void OnCheckS3();
	afx_msg void OnCheckS4();
	afx_msg void OnCheckS5();
	afx_msg void OnCheckS6();
	afx_msg void OnCheckS7();
	afx_msg void OnCheckS8();
	afx_msg void OnCheckS9();
	afx_msg void OnCheckA1();
	afx_msg void OnCheckA10();
	afx_msg void OnCheckA11();
	afx_msg void OnCheckA12();
	afx_msg void OnCheckA13();
	afx_msg void OnCheckA14();
	afx_msg void OnCheckA15();
	afx_msg void OnCheckA16();
	afx_msg void OnCheckA2();
	afx_msg void OnCheckA3();
	afx_msg void OnCheckA4();
	afx_msg void OnCheckA5();
	afx_msg void OnCheckA6();
	afx_msg void OnCheckA7();
	afx_msg void OnCheckA8();
	afx_msg void OnCheckA9();
	afx_msg void OnButtonMd(UINT nID);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnIconNervesAndPeriodicMD();
	afx_msg void OnBnClickedCheckAllExtern();
	afx_msg void OnBnClickedButtonAllMd();
	afx_msg void OnDestroy();
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnBnClickedButtonRte();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnCbnSelchangeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DoAlarm(DWORD id, BOOL bAlarm, WORD wMD=0);
	void DoMDAlarm(DWORD id);

private:
	UINT	m_nTimer;
	BOOL	m_b2Left;
	int		m_iCounter;
	int		m_nMDBtnID;
	int		m_nEdtID;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSOFTALARMDLG_H__9ACE97E5_A2BF_4125_9F3D_74A0B7A6B83C__INCLUDED_)
