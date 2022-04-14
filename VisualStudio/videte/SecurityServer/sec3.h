/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: sec3.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/sec3.h $
// CHECKIN:		$Date: 14.08.06 16:00 $
// VERSION:		$Revision: 120 $
// LAST CHANGE:	$Modtime: 14.08.06 12:15 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __INC_SEC3_h__
#define __INC_SEC3_h__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "CIPCInputServerClient.h"
#include "CIPCOutputServerClient.h"
#include "CIPCServerControlServerSide.h"

#include "InputList.h"
#include "OutputList.h"
#include "IPCAudioServer.h"

// referenced classes:

class CMainFrame;
class CInputList;
class COutputList;
class CIPCDatabaseClientServer;
class CIPCOutputServerClient;
class CSecTimerArray;
class CProcessListDoc;
class CSecTimer;
class CIPCOutputServer;
class CProcess;
class CInput;

#include "ProcessMacro.h"
#include "TimerThread.h"
#include "EncoderThread.h"
#include "UploadThread.h"

enum ApplicationState
{
	AS_STARTED,
	AS_START_RESET,
	AS_START_DATABASE,
	AS_START_ENCODER,
	AS_START_UPLOAD,
	AS_START_OUTPUT,
	AS_SET_MD,
	AS_START_AUDIO,
	AS_START_TIMER,
	AS_START_INPUT,
	AS_RESET_COMPLETE,
	AS_UP_AND_RUNNING
};

class CSec3App : public CWinApp
{
	// construction / destruction
public:
	CSec3App();
	virtual ~CSec3App();

	// attributes
public:
	inline BOOL IsUpAndRunning() const;

	inline CInputList&			   GetInputGroups();
	inline COutputList&			   GetOutputGroups();
	inline CAudioList&			   GetAudioGroups();
	
	inline CIPCInputClientsArray&  GetInputClients();
	inline CIPCOutputClientsArray& GetOutputClients();
	inline CIPCAudioClientsArray*  GetAudioClients();
	inline CIPCOutputServer*	   GetOutputDummy() const;
	
	inline CProcessMacros&   GetProcessMacros();
	inline CUserArray&       GetUsers();
	inline CPermissionArray& GetPermissions();
	inline CHostArray&       GetHosts();
	inline CSecTimerArray&   GetTimers();
	
	inline DWORD			 GetResetTickCount() const;

	inline CIPCServerControlServerSide*	GetServerControl();
	inline CIPCDatabaseClientServer*	GetDatabase();

	inline BOOL StatProcesses() const;
	inline BOOL HasAlarmOffSpanFeature() const;
	CString GetHTMLCodePage(UINT nCP);

	//
	CProcessListDoc *m_pProcessDoc;
		
	// operations
public:
	void AddProcessListViewItem(const CString& sLine);
	void ClearProcessListView();

	void StartProcessesByTimer(const CSecTimer *pTimer);
	void KillProcessesByTimer(const CSecTimer *pTimer);
	//
	void SavePicture(Compression comp,
					 const CIPCPictureRecord& pict,								
					 CIPCFields& fields,
					 WORD wArchiveNr);
	void SaveMedia(	 const CIPCMediaSampleRecord& rec,								
					 CIPCFields& fields,
					 WORD wArchiveNr);
	void SaveAlarmData(CIPCFields& fields,
						WORD wArchiveNr);
	void ModifyStoringFlag(BOOL bNewState);	// thread safe
	void UpdateStateInfo();
	//	send an CIPCError, if access is denied
	inline BOOL AllowClientAccess(CIPC *pCipc, DWORD dwCmd);
	void DenyClientAccess(CIPC *pCipc, DWORD dwCmd);
	void StopClientProcesses(CIPCOutputServerClient* pClient);
	void StopClientProcesses(CIPCAudioServerClient* pClient);

	void OnTimerThreadRun();
	void OnTimer();

	void DoReset();
	void EmergencyReset();
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSec3App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSec3App)
	afx_msg void OnAppAbout();
	afx_msg void OnEinstellungenSupervisor();
	afx_msg void OnUpdatePaneUnits(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneClients(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneDb(CCmdUI* pCmdUI);
	afx_msg void OnLogProcesses();
	afx_msg void OnLogRequests();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// implementation
protected:
	void	RegisterWindowClass();
	void	InitTickCount();
	void	Reset();
	void	ResetHosts(CWK_Profile &wkp);
	void	DeleteAll();
	void	EnableMDOnOutputs();
	
	// debug options:
	void ReadDebugConfiguration();
	void WriteDebugConfiguration();
	void DisplayDebugOptions();
	//
	void MarkLockInputs();
	BOOL ConnectToDatabase();
	//
	void OnStarted();
	void OnStartReset();
	void OnStartDatabase();
	void OnStartEncoder();
	void OnStartUpload();
	void OnStartOutput();
	void OnSetMD();
	void OnStartAudio();
	void OnStartTimer();
	void OnStartInput();
	void OnResetComplete();
	void OnIsUpAndRunning();
	void OnNewDay();

	void SetState(ApplicationState as);

public:
	BOOL m_bTraceVeryOld;
	BOOL m_bTraceTimer;
	BOOL m_bTraceLocks;
	BOOL m_bMpeg4Transmission;
	BOOL m_bMpeg4Recording;
	int  m_iMaxNrOfClients;

	// data member
private:
	ApplicationState m_State;
	DWORD			 m_dwCounter;
	CTimerThread     m_TimerThread;
	CEncoderThread   m_EncoderThread;
	//
	BOOL m_bStoring;
	CCriticalSection m_csStoreFlag;
	//
	CIPCInputClientsArray m_inputClientsDummy;
	CIPCOutputClientsArray m_outputClientsDummy;
	//
	int m_iTraceMili;
	//
	CIPCOutputServer *m_pOutputDummy;	// used for ISDN processes
	//
	CTimedMessage m_tmIamAlive;
	
	//
	CProcessMacros		m_ProcessMacros;
	CUserArray			m_Users;
	CPermissionArray	m_Permissions;
	CHostArray			m_Hosts;
	CSecTimerArray		m_Timers;
	CSystemTime			m_stTime;
	
	// Input Units
	CInputList	m_InputGroups;
	COutputList	m_OutputGroups;
	CAudioList  m_AudioGroups;
	
	// default CIPC Objects
	CIPCDatabaseClientServer*		m_pDatabase;
	CIPCServerControlServerSide*	m_pServerControl;

	BOOL	m_bHasAlarmOffSpanFeature;
	BOOL	m_bResetPosted;

	DWORD				m_dwLastTimerThreadRunTick;
	CCriticalSection	m_csLastTimerThreadRunTick;

public:
	CUploadThread	 m_UploadThread;
	LPSTR			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	LPSTR			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;
};
////////////////////////////////////////////////////////////////////////
extern CSec3App theApp;
////////////////////////////////////////////////////////////////////////
inline CInputList& CSec3App::GetInputGroups()
{
	return m_InputGroups;
}
////////////////////////////////////////////////////////////////////////
inline COutputList& CSec3App::GetOutputGroups()
{
	return m_OutputGroups;
}
////////////////////////////////////////////////////////////////////////
inline CAudioList& CSec3App::GetAudioGroups()
{
	return m_AudioGroups;
}
////////////////////////////////////////////////////////////////////////
inline CProcessMacros& CSec3App::GetProcessMacros()
{
	return m_ProcessMacros;
}
////////////////////////////////////////////////////////////////////////
inline CUserArray& CSec3App::GetUsers()
{
	return m_Users;
}
////////////////////////////////////////////////////////////////////////
inline CPermissionArray& CSec3App::GetPermissions()
{
	return m_Permissions;
}
////////////////////////////////////////////////////////////////////////
inline CHostArray& CSec3App::GetHosts()
{
	return m_Hosts;
}
////////////////////////////////////////////////////////////////////////
inline CSecTimerArray& CSec3App::GetTimers()
{
	return m_Timers;
}
////////////////////////////////////////////////////////////////////////
inline CIPCServerControlServerSide*	CSec3App::GetServerControl()
{
	return m_pServerControl;
}
////////////////////////////////////////////////////////////////////////
inline CIPCDatabaseClientServer* CSec3App::GetDatabase()
{
	return m_pDatabase;
}
////////////////////////////////////////////////////////////////////////
inline CIPCInputClientsArray& CSec3App::GetInputClients()
{
	if (m_pServerControl) 
	{
		return m_pServerControl->m_inputClients;
	} else {
		return m_inputClientsDummy;
	}
}
////////////////////////////////////////////////////////////////////////
inline CIPCOutputClientsArray& CSec3App::GetOutputClients()
{
	if (m_pServerControl) {
		return m_pServerControl->m_outputClients;
	} else {
		return m_outputClientsDummy;
	}
}
////////////////////////////////////////////////////////////////////////
inline CIPCAudioClientsArray* CSec3App::GetAudioClients()
{
	if (m_pServerControl) 
	{
		return &(m_pServerControl->m_audioClients);
	} 
	else 
	{
		return NULL;
	}
}
////////////////////////////////////////////////////////////////////////
inline BOOL CSec3App::AllowClientAccess(CIPC *pCipc, DWORD dwCmd)
{
	if (IsUpAndRunning()) 
	{
		return TRUE;
	} 
	else 
	{
		DenyClientAccess(pCipc,dwCmd);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSec3App::IsUpAndRunning() const
{
	return m_State == AS_UP_AND_RUNNING;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCOutputServer* CSec3App::GetOutputDummy() const
{
	return m_pOutputDummy;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSec3App::HasAlarmOffSpanFeature() const
{
	return m_bHasAlarmOffSpanFeature;
}
/////////////////////////////////////////////////////////////////////////////
BOOL SeparatSAToWA(CStringArray& sa, CWordArray& waCams, char cSeparator);
/////////////////////////////////////////////////////////////////////////////
#define CHECK_ACCESS(dwCmd) {if(!theApp.AllowClientAccess(this,dwCmd))return;}
#endif
