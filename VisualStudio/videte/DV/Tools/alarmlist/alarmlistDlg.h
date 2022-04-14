// alarmlistDlg.h : header file
//

#if !defined(AFX_ALARMLISTDLG_H__B0D14217_49F3_11D5_99F0_004005A19028__INCLUDED_)
#define AFX_ALARMLISTDLG_H__B0D14217_49F3_11D5_99F0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistDlg dialog

class CConnectionRecord;
class CServerAlarmlist;
class CAlarmObject;
class CAlarmlistDlg : public CTransparentDialog
{
// Construction
public:
	CAlarmlistDlg(const CString& sHost, const CString& sPIN, CWnd* pParent = NULL);	

	virtual ~CAlarmlistDlg();
// Dialog Data
	//{{AFX_DATA(CAlarmlistDlg)
	enum { IDD = IDD_ALARMLIST_DIALOG };
	CSkinListBox	m_ctrlList;
	CSkinButton		m_ctrlNextDay;
	CSkinButton		m_ctrlPrevDay;
	CSkinButton		m_ctrlImage;
	CSkinButton		m_ctrlExtern2;
	CSkinButton		m_ctrlExtern1;
	CSkinButton		m_ctrlActivity;
	CSkinButton		m_ctrlPrint;
	CSkinButton		m_ctrlLineUp;
	CSkinButton		m_ctrlLineDown;
	CSkinButton		m_ctrlOK;
	CSkinButton		m_ctrlCam[16];
	CFrame			m_ctrlDisplayBorder;
	CDisplay		m_ctrlDisplayTitle;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmlistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void NotifyConnect(BOOL bOK, int iErrorCode);
	void NotifyDisconnect();

// Implementation
protected:
	void UpdateDlg();
	void UpdateLineUpDownButtons();
	BOOL ClearListBox();
	BOOL FilterAlarm(CAlarmObject* pAlarm);
	void RequestNewAlarmList();
	
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAlarmlistDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCam0();
	afx_msg void OnCam1();
	afx_msg void OnCam2();
	afx_msg void OnCam3();
	afx_msg void OnCam4();
	afx_msg void OnCam5();
	afx_msg void OnCam6();
	afx_msg void OnCam7();
	afx_msg void OnCam8();
	afx_msg void OnCam9();
	afx_msg void OnCam10();
	afx_msg void OnCam11();
	afx_msg void OnCam12();
	afx_msg void OnCam13();
	afx_msg void OnCam14();
	afx_msg void OnCam15();
	afx_msg void OnScrollLineUp();
	afx_msg void OnScrollPrevDay();
	afx_msg void OnScrollLineDown();
	afx_msg void OnScrollNextDay();
	afx_msg void OnDestroy();
	afx_msg void OnCancelMode();
	afx_msg void OnActivity();
	afx_msg void OnExtern1();
	afx_msg void OnExtern2();
	afx_msg void OnPrint();
	
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	afx_msg long OnNotifyConnect(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyDisconnect(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyAlarmListUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyMessageScrollEnd(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

private:
	CSkinButtons		m_AlarmListButtons;
	CImageList			m_ilLineUp;
	CImageList			m_ilLineDown;
	CImageList			m_ilPrevDay;
	CImageList			m_ilNextDay;
	CImageList			m_ilOK;
	CImageList			m_ilPrint;
	CImageList			m_ilActivity;
	CImageList			m_ilExtern1;
	CImageList			m_ilExtern2;
	CImageList			m_ilPlay;
	int					m_nAlarmFilter[3];

	CServerAlarmlist*	m_pServer;
	CSystemTime			m_AlarmTime;
	CString				m_sHost;
	CString				m_sPIN;

	static int m_nWM_ScrollEnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMLISTDLG_H__B0D14217_49F3_11D5_99F0_004005A19028__INCLUDED_)
