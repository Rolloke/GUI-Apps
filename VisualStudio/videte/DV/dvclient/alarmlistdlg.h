// alarmlistDlg.h : header file
//

#if !defined(AFX_ALARMLISTDLG_H__B0D14217_49F3_11D5_99F0_004005A19028__INCLUDED_)
#define AFX_ALARMLISTDLG_H__B0D14217_49F3_11D5_99F0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"

#include "AlarmObject.h"

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistDlg dialog
class CConnectionRecord;
class CServerAlarmlist;
class CPanel;
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
	CSkinButton		m_ctrlAudio;
	CSkinButton		m_ctrlPrint;
	CSkinButton		m_ctrlLineUp;
	CSkinButton		m_ctrlLineDown;
	CSkinButton		m_ctrlOK;
	CSkinButton		m_ctrlCam0;
	CSkinButton		m_ctrlCam1;
	CSkinButton		m_ctrlCam2;
	CSkinButton		m_ctrlCam3;
	CSkinButton		m_ctrlCam4;
	CSkinButton		m_ctrlCam5;
	CSkinButton		m_ctrlCam6;
	CSkinButton		m_ctrlCam7;
	CSkinButton		m_ctrlCam8;
	CSkinButton		m_ctrlCam9;
	CSkinButton		m_ctrlCam10;
	CSkinButton		m_ctrlCam11;
	CSkinButton		m_ctrlCam12;
	CSkinButton		m_ctrlCam13;
	CSkinButton		m_ctrlCam14;
	CSkinButton		m_ctrlCam15;
	CFrame			m_ctrlDisplayBorder;
	CDisplay		m_ctrlDisplayTitle;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmlistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	void NotifyConnect(BOOL bOK, int iErrorCode);
	void NotifyDisconnect();

// Implementation
protected:
	void UpdateDlg(BOOL bWithCameraButtons);
	void UpdateLineUpDownButtons();
	BOOL FilterAlarm(CAlarmObject* pAlarm);
	void RequestNewAlarmList();
	void CopyAlarmObjectList(CAlarmObjectListCS& AlarmList);
	void UpdateListBox(BOOL bWithCameraButtons);
	void UpdateListBoxSortTime(BOOL bWithCameraButtons);
	void AddListBoxOrderNumbers();
	
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAlarmlistDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCameraButton();
	afx_msg void OnScrollLineUp();
	afx_msg void OnScrollPrevDay();
	afx_msg void OnScrollLineDown();
	afx_msg void OnScrollNextDay();
	afx_msg void OnDestroy();
	afx_msg void OnActivity();
	afx_msg void OnExtern1();
	afx_msg void OnExtern2();
	afx_msg void OnPrint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnAlarmlistImage();
	afx_msg void OnDblclkAlarmlistList();
	afx_msg void OnSelchangeAlarmlistList();
	afx_msg void OnAlarmlistOk();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnAlarmlistAudio();
	//}}AFX_MSG
	afx_msg long OnNotifyConnect(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyDisconnect(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyAlarmListUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyMessageScrolled(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyMessageScrollEnd(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

private:
	CSkinButtons		m_AlarmListButtons;
	CSkinButtons		m_AlarmButtons;
	CSkinButtons		m_CameraButtons;
	CImageList			m_ilLineUp;
	CImageList			m_ilLineDown;
	CImageList			m_ilPrevDay;
	CImageList			m_ilNextDay;
	CImageList			m_ilOK;
	CImageList			m_ilPrint;
	CImageList			m_ilActivity;
	CImageList			m_ilExtern1;
	CImageList			m_ilExtern2;
	CImageList			m_ilAudio;
	CImageList			m_ilPlay;
	int					m_nAlarmFilter[4];

	HCURSOR				m_hCursor;
	CServerAlarmlist*	m_pServer;
	BOOL				m_bRequesting;
	CSystemTime			m_AlarmTime;
	CString				m_sHost;
	CString				m_sPIN;
	CPanel*				m_pPanel;
	CAlarmObjectListCS	m_AlarmObjectList;

	static int	m_nWM_ScrollEnd;
	static UINT m_uWM_Scrolled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMLISTDLG_H__B0D14217_49F3_11D5_99F0_004005A19028__INCLUDED_)
