// Recherche.h : main header file for the RECHERCHE application
//

#if !defined(AFX_RECHERCHE_H__517E1EFA_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_RECHERCHE_H__517E1EFA_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define COUNTDOWN_MS	500

#define ID_CONNECT_LOW	0x0E00
#define ID_CONNECT_HIGH	0x0EFF

#define ID_DISCONNECT_LOW	0x0F00
#define ID_DISCONNECT_HIGH	0x0FFF

#define ID_SEARCH_LOW	0x1000
#define ID_SEARCH_HIGH	0x10FF

#define ID_CHANGE_VIDEO				0x7000
#define ID_DECODE_VIDEO				0x7001
#define ID_SEARCH_RESULT			0x7002
#define ID_SEARCH_CANCELED			0x7003
#define ID_SEARCH_FINISHED			0x7004
#define ID_ARCHIV_INFO				0x7005
#define ID_BACKUP_CONFIRM			0x7006
#define ID_BACKUP_CANCEL_CONFIRM	0x7007
#define ID_BACKUP_ERROR				0x7008
#define ID_CAMERA_NAMES				0x7009
#define ID_SEARCH_RESULT_COPY		0x700A
#define ID_REDRAW_VIDEO				0x700B
#define ID_ALARM_LIST				0x700C
#define ID_DRIVE_INFO				0x700D
#define ID_CHANGE_AUDIO				0x700E
#define ID_UPDATE_DIALOGS			0x700F

#define SMALL_WINDOW_CLASS _T("SmallWindowClassRecherche")

#define SECTION_RECHERCHE     _T("Recherche")

class CIPCServerControlRecherche;

#include "QueryParameter.h"

class CImpIDispatch;
/////////////////////////////////////////////////////////////////////////////
// CRechercheApp:
// See Recherche.cpp for the implementation of this class
//

typedef enum PlayStatus
{
	PS_PLAY_BACK	=	1,
	PS_PLAY_FORWARD	=	2,
	PS_STOP			=	3,
	PS_FAST_BACK	=	4,
	PS_FAST_FORWARD	=	5,
	PS_SKIP_BACK	= 	8,
	PS_SKIP_FORWARD	= 	9,
};

CString NameOfEnum(PlayStatus ps);

class CRechercheApp : public CWinApp,
	public COsVersionInfo
{
public:
	CRechercheApp();
	~CRechercheApp();

	// attributes
public:
	inline CString GetDefaultURL() const;
	inline CString GetMapURL() const;
	inline BOOL	   IsReceiver() const;
	inline BOOL	   IsDemo() const;
	DWORD GetCPUClockFrequency(){return m_dwCPUClock;};

	CHostArray& GetHosts();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRechercheApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	// attributes
public:
	inline CString GetStationName();
	inline const CMapStringToString& GetFieldMap() const;

	// Operation
public:
	CString	GetMappedString(const CString& sKey);
	inline CQueryParameter& GetQueryParameter();

	//function for realtime playback
	BOOL	CanPlayRealtime();

// Implementation
protected:
	BOOL LoginAsSuperVisor();
	BOOL Login();
	void RegisterWindowClass();

	void LoadSettings();
	void SaveSettings();

	//{{AFX_MSG(CRechercheApp)
	afx_msg void OnNewLogin();
	afx_msg void OnEditStation();
	afx_msg void OnFilePrintSetup();
	afx_msg void OnSearchAtStart();
	afx_msg void OnUpdateSearchAtStart(CCmdUI* pCmdUI);
	afx_msg void OnViewTargetDisplay();
	afx_msg void OnUpdateViewTargetDisplay(CCmdUI* pCmdUI);
	afx_msg void OnSequenceRealtime();
	afx_msg void OnUpdateSequenceRealtime(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// data member
public:
	CAutoLogout m_AutoLogout;
	BOOL		m_bRunAnyLinkUnit;
	CString		m_sSyncTime;
	DWORD     m_dwMonitorFlags;

	CWK_Dongle::OemCode m_OemCode;
	BOOL		 m_bIsDemo;
	BOOL		 m_bAllowMiCo;
	BOOL		 m_bAllowCoCo;
	BOOL		 m_bAllowPT;
	BOOL		 m_bAllowBackup;
	BOOL		 m_bReadOnlyModus;
	BOOL		 m_bBackupModus;
	BOOL		 m_bSearchMaskAtStart;
	BOOL		 m_bTargetDisplay;
	int       m_nMaxRequestedRecords;
	int       m_nMaxSentSamples;
	CUser*		 m_pUser;
	CPermission* m_pPermission;
	CIPCServerControlRecherche* m_pCIPCServerControlRecherche;

	CErrorMessages m_ErrorMessages;

public:
	BOOL InitFields();
protected:
	void InitFieldMap();
	void InitField(CWK_Profile& wkp,const CString& sName, UINT nID);
private:
	CMapStringToString	m_FieldMap;
	CString				m_sStationName;
	// query params
	CQueryParameter		m_QueryParameter;
	CString				m_sDefaultURL;
	CString				m_sMapURL;
	CString				m_sVersion;
	CString				m_sOemIni;
	CHostArray			m_Hosts;
	BOOL				m_bIsReceiver;
	DWORD				m_dwCPUClock;

	//member for realtime playback
	BOOL				m_bCanPlayRealtime;
public:
	CImpIDispatch* m_pDispOM;
};
/////////////////////////////////////////////////////////////////////////////
inline CString CRechercheApp::GetStationName()
{
#ifdef _DTS
	return m_Hosts.GetLocalHost()->GetName();
#else
	return m_sStationName;
#endif
}
/////////////////////////////////////////////////////////////////////////////
inline CQueryParameter& CRechercheApp::GetQueryParameter()
{
	return m_QueryParameter;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CRechercheApp::GetDefaultURL() const
{
	return m_sDefaultURL;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CRechercheApp::GetMapURL() const
{
	return m_sMapURL;
}
/////////////////////////////////////////////////////////////////////////////
inline const CMapStringToString& CRechercheApp::GetFieldMap() const
{
	return m_FieldMap;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CRechercheApp::IsReceiver() const
{
	return m_bIsReceiver;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CRechercheApp::IsDemo() const
{
	return m_bIsDemo;
}
extern CRechercheApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECHERCHE_H__517E1EFA_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
