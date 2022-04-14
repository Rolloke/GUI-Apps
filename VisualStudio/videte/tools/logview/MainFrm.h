/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: MainFrm.h $
// ARCHIVE:		$Archive: /Project/Tools/LogView/MainFrm.h $
// CHECKIN:		$Date: 3/13/06 2:14p $
// VERSION:		$Revision: 33 $
// LAST CHANGE:	$Modtime: 3/13/06 12:44p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__4DB08FE9_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
#define AFX_MAINFRM_H__4DB08FE9_BC2E_11D2_A9C4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LogDialogBar.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	void SetStatusText(const CString &sText);
	CStatusBar*	GetStatusBar() {return &m_wndStatusBar;}
	BOOL	IsSyncWindowThreadRunning();
	BOOL	IsInOnTimer() { return m_bInOnTimer; };
	inline const CString &GetCurrentLogPath() const;

// Operations
public:
	void	SetLogPath(CString&sLogPath);
	void	StopSyncWindowThread();
	void	OnEditSearchTimeValue(CView*pView);
	CDateTimeCtrl* GetDateCtrl();
	CDateTimeCtrl* GetTimeCtrl();
    static long toDays(const CTime& date);
    static void fromDays(long days, CTime& date);
    static long toSeconds(const CTime& time);
    static void fromSeconds(long seconds, CTime& time);

private:
	BOOL	OnDeleteFiles(CComboBox*pBox, CString&sPath, UINT nIDHeadline, UINT nIDNotDeleted);
	BOOL	OnSaveFiles(CComboBox*pBox, CString&sPath, UINT nIDHeadline, UINT nIDHeadline2);
	static UINT SyncWindowThread(LPVOID pParam );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropDownComboLogFiles();
	afx_msg void OnComboLogFilesViaAcc();
	afx_msg void OnOkComboLogFiles();
	afx_msg void OnDropDownComboHtmFiles();
	afx_msg void OnOkComboHtmFiles();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKeyboardChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpenFile(WPARAM, LPARAM);
	afx_msg void OnFileSaveLogfiles();
	afx_msg void OnUpdateFileSaveLogfiles(CCmdUI *pCmdUI);
	afx_msg void OnFileChangeLogPath();
	afx_msg void OnUpdateFileChangeLogPath(CCmdUI *pCmdUI);
	afx_msg void OnViewSyncTime();
	afx_msg void OnUpdateViewSyncTime(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnViewUpdateAutomatically();
	afx_msg void OnUpdateViewUpdateAutomatically(CCmdUI *pCmdUI);
	afx_msg void OnViewJumpToEnd();
	afx_msg void OnUpdateViewJumpToEnd(CCmdUI *pCmdUI);
	afx_msg void OnFileDeleteLogfiles();
	afx_msg void OnUpdateFileDeleteLogfiles(CCmdUI *pCmdUI);
	afx_msg void OnWindowSyncCompressed();
	afx_msg void OnUpdateWindowSyncCompressed(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveProtocolls();
	afx_msg void OnUpdateFileSaveProtocolls(CCmdUI *pCmdUI);
	afx_msg void OnFileDeleteProtocolls();
	afx_msg void OnUpdateFileDeleteProtocolls(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL Login();
	BOOL ResetComboBoxLog();
	BOOL FillComboBoxLog(const CString &sExt);
	BOOL ResetComboBoxHtm();
	BOOL FillComboBoxHtm(const CString &sExt);
	CComboBox* GetComboBoxLog();
	CComboBox* GetComboBoxHtm();
public:
	CStringArray m_saCmdParam;
	BOOL	m_bScrollToEndAtRefresh;
	BOOL	m_bSyncCompressed;

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CLogDialogBar  m_wndDlgBar;

private:
	CString m_sLogPath;
	CString m_sHtmPath;
	int m_nCurSelLogFile;
	int m_nCurSelHtmFile;

	volatile CWinThread *m_pSyncWindowsThread;
	long				 m_nDay, m_nSec;
	CPtrList			 m_SyncViews;

	BOOL m_bOk;
	BOOL m_bInOnTimer;
	UINT m_uTimerID;
	UINT m_uUpdateTimerID;
public:
};
/////////////////////////////////////////////////////////////////////////////
inline const CString &CMainFrame::GetCurrentLogPath() const
{
	return m_sLogPath;
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__4DB08FE9_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
