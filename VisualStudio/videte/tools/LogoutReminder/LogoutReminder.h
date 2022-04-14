// LogoutReminder.h : main header file for the LOGOUTREMINDER application
//

#if !defined(AFX_LOGOUTREMINDER_H__B57190A5_F172_4EF8_850B_31AA516DC14C__INCLUDED_)
#define AFX_LOGOUTREMINDER_H__B57190A5_F172_4EF8_850B_31AA516DC14C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogoutReminderApp:
// See LogoutReminder.cpp for the implementation of this class
//
#define DONT_SHOW      0
#define SHOW_DLG       1
#define LOG_OFF        2
#define SHUT_DOWN      3
#define SHUT_DOWN_EXIT 4
#define REMIND_DLG     5

class ScheduleTask
{
public:
    ScheduleTask() : mSchedule(atTime), mIsActive(FALSE) {}
    enum eSchedule { atTime, afterStartup };
    CTime     mTime;
    CString   mTask;
    eSchedule mSchedule;
    BOOL      mIsActive;
};
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strLogOutProgPath;
	BOOL	m_bLogOutTime;
	CTime	m_LogoutTime;
	BOOL  m_bSavedTime;
	int		m_nRadio;
	BOOL	m_bCkDate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
   afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnBtnFindPath();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLogout();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCkStop();
	afx_msg void OnSaveLogoutTime();
	afx_msg void OnClose();
	afx_msg void OnCkDate();
	//}}AFX_MSG
public:
	afx_msg void OnCkLogoutOldTime();
	DECLARE_MESSAGE_MAP()
private:
	void StopTimer();
	void StartTimer();
	int  m_nTimer;
	int  m_nSeconds;
	HANDLE m_hThread;
	static unsigned int WINAPI WaitThread(void *);
	PROCESS_INFORMATION m_pi;
public:
    afx_msg void OnEnKillfocusEdtLogoutProgramPath();
};


class CLogoutReminderApp : public CWinApp
{
public:
	CLogoutReminderApp();
    BOOL  m_bShowDialog;
	bool  m_bStopTimeoutDlgBox;
    CTime m_LogoutTime;
	CTimeSpan GetIncrementTimeSpan(LPCTSTR sEntry=NULL);
    CList<ScheduleTask, ScheduleTask&> mTasks;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogoutReminderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CLogoutReminderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGOUTREMINDER_H__B57190A5_F172_4EF8_850B_31AA516DC14C__INCLUDED_)
