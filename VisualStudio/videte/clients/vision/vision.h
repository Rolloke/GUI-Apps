// Vision.h : main header file for the VISION application
//

#if !defined(AFX_VISION_H__46825266_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_VISION_H__46825266_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define ID_CONNECT_LOW	0x0E00
#define ID_CONNECT_HIGH	0x0EFF

#define ID_DISCONNECT_LOW	0x0F00
#define ID_DISCONNECT_HIGH	0x0FFF

#define ID_CHANGE_VIDEO	0x7000
#define ID_DECODE_VIDEO	0x7001

#define COUNTDOWN_MS	500

#define DISPLAY_WINDOW_CLASS _T("DisplayWindowClassVision")

#define SUB_MENU_HOST        0
#define SUB_MENU_DETECTOR    1
#define SUB_MENU_CAMERA      2
#define SUB_MENU_RELAIS      3
#define SUB_MENU_SEQUENCER   4
#define SUB_MENU_PTZ         5
#define SUB_MENU_MICO	     6
#define SUB_MENU_COCO	     7
#define SUB_MENU_PICTURES    8
#define SUB_MENU_MONITOR     9
#define SUB_MENU_HTML       10
#define SUB_MENU_AUDIO_PLAY 11
#define SUB_MENU_AUDIO_REC  12
#define SUB_MENU_CONNECT_HOST 13


class CImpIDispatch;

/////////////////////////////////////////////////////////////////////////////
// CVisionApp:
// See Vision.cpp for the implementation of this class
//

class CVisionApp : public CWinApp,
	public COsVersionInfo
{
public:
	CVisionApp();
	~CVisionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisionApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// attributes
public:
	inline CString GetDefaultURL() const;
	inline CString GetMapURL() const;
	inline BOOL	   IsReceiver() const;

	CHostArray& GetHosts();

	void CheckHostMap();
	CSecID	GetNextHtmlWindowID();

	// operations
public:
	void AlarmConnection();
	void OnErrorMessage();
	BOOL DeleteMap();

	// Implementation
protected:
	BOOL LoginAsSuperVisor();
	BOOL Login(LPCTSTR pszUser = NULL,LPCTSTR pszPassword = NULL);
	BOOL CheckUserPassword(CString& sUser, CString& sPassword);
	void RegisterWindowClass();
	void LoadSettings();
	void LoadUserSpecificSettings();
	void SaveSettings();
	BOOL SplitCommandLine(const CString sCommandLine, CStringArray& saParameters);

	//{{AFX_MSG(CVisionApp)
	afx_msg void OnNewLogin();
	afx_msg void OnOptions();
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
	afx_msg void OnViewTargetDisplay();
	afx_msg void OnUpdateViewTargetDisplay(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// data member
public:
	CAutoLogout m_AutoLogout;
	BOOL		m_bRunAnyLinkUnit;

	CWK_Dongle*  m_pDongle;
	CUser*		 m_pUser;
	CPermission* m_pPermission;

	BOOL		 m_bTraceBitrate;
	BOOL		 m_bTraceDelay;
	BOOL		 m_bTraceSwitch;
	BOOL		 m_bTraceImage;
	BOOL		 m_bStatImage;
	BOOL		 m_bTraceSequence;
	BOOL		 m_bWarnIfAudioIsBlockedAtNo;

	DWORD		 m_dwInitialTuning;
	DWORD		 m_dwSequenceIntervall;

	BOOL		 m_bCorrectBitrate;
	BOOL		 m_bCoCoQuality;
	BOOL		 m_bCoCoFit;
	BOOL		 m_bCoCoSoft;
	BOOL		 m_bConnectToLocalServer;
	BOOL		 m_bRS232Transparent;

	BOOL		 m_bAnimateAlarmConnections;
	BOOL		 m_bAlarmAnimation;
	BOOL		 m_bFirstCam;
	BOOL		 m_b1PlusAlarm;
	BOOL		 m_bFullscreenAlarm;
	BOOL		 m_bTargetDisplay;
	BOOL		 m_bDisableZoom;
	BOOL		 m_bDisableSmall;
	BOOL		 m_bCloseAlarmWindow;
	BOOL		 m_bDisableRightClick;
	BOOL		 m_bDisableDblClick;


	DWORD		m_dwMonitorFlags;
	BOOL		m_bShowTitleOfSmallWindows;
	
	HCURSOR		m_hPanLeft;
	HCURSOR		m_hPanRight;
	HCURSOR		m_hTiltUp;
	HCURSOR		m_hTiltDown;
	HCURSOR		m_hZoomIn;
	HCURSOR		m_hZoomOut;
	HCURSOR		m_hArrow;

	CRect		m_MaxMegraRect;
	CString		m_sVersion; 

	CImpIDispatch* m_pDispOM;
	CString        m_sMapImagePath;

	CErrorMessages m_ErrorMessages;
     
private:
	BOOL		m_bIsReceiver;
	CString		m_sDefaultURL;
	CString		m_sMapURL;
	CHostArray  m_Hosts;
	WORD		m_wHtmlWindowID;
};

extern CVisionApp theApp;

/////////////////////////////////////////////////////////////////////////////
inline CString CVisionApp::GetDefaultURL() const
{
	return m_sDefaultURL;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CVisionApp::IsReceiver() const
{
	return m_bIsReceiver;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CVisionApp::GetMapURL() const
{
	return m_sMapURL;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISION_H__46825266_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
