// DVProcess.h : main header file for the DVPROCESS application
//

#if !defined(AFX_DVPROCESS_H__43C9E48F_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_DVPROCESS_H__43C9E48F_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CIPCServerControlProcess;
class CIPCDatabaseProcess;

class CInputGroup;
class CCameraGroup;
class CInput;
class CCamera;
class CRelay;

#define NR_OF_CARDS		2
#define NR_OF_VOUT		2
#define IMAGE_SIZE_KB	(50)
#define IMAGE_SIZE_BYTE	(IMAGE_SIZE_KB*1024)
#define	UVV_NR_ALARM	3
#define MAX_UNIT		4
#define MAX_CAM			(16 * MAX_UNIT)
#define MAX_ALARM_DIALING_NUMBERS	10

#include "InputClient.h"
#include "OutputClient.h"
#include "AudioClient.h"

#include "InputList.h"
#include "OutputList.h"
#include "IPCAudioServer.h"

#include "VOutThread.h"
#include "CallThread.h"
#include "EncoderThread.h"
#include "Settings.h"

typedef enum
{
	VOUT_OFF	   = 0,
	VOUT_ACTIVITY  = 1,
	VOUT_CLIENT	   = 2,
	VOUT_SEQUENCE  = 3,
	VOUT_PERMANENT = 4,
} VOUT_MODE;
LPCTSTR NameOfEnum(VOUT_MODE vm);

CString GetTickCountEx(BOOL bStart = FALSE);

/////////////////////////////////////////////////////////////////////////////
// CDVProcessApp:
// See DVProcess.cpp for the implementation of this class
//

class CDVProcessApp : public CWinApp
{
public:
	CDVProcessApp();

	// attributes
public:
	inline BOOL IsResetting() const;
	inline BOOL IsShutDown() const;
	inline BOOL IsUpAndRunning() const;
	
	inline CInputList& GetInputGroups();
	inline COutputList& GetOutputGroups();
	inline CAudioList& GetAudioGroups();
	inline CIPCAudioServer* GetAudioGroup();
	inline CSettings& GetSettings();

	inline CInputClients& GetInputClients();
	inline COutputClients& GetOutputClients();
	inline CAudioClients& GetAudioClients();
	inline CSecTimerArray& GetTimers();
	inline CStringArray&   GetAlarmHosts();

	inline CString GetSuperPIN() const;
	inline CString GetOperatorPIN() const;
		   CString GetLocalHostName() const;
	inline BOOL GetDetectorIgnoreTimer() const;
	inline BOOL GetAlarmOutput() const;

	inline VOUT_MODE GetVOutModePort(int iCard, int iPort) const;
	inline CCallThread& GetCallThread();
	inline CEncoderThread& GetEncoderThread();
	inline CIPCDatabaseProcess* GetDatabase();
	inline const CInput* GetPTZInput();


	// operations
public:
	void AddInputConnectionRecord(const CConnectionRecord& c);
	void OnInputConnectionRecord();
	void AddOutputConnectionRecord(const CConnectionRecord& c);
	void OnOutputConnectionRecord();
	void AddAudioConnectionRecord(const CConnectionRecord& c);
	void OnAudioConnectionRecord();

	BOOL SavePicture(CCamera* pCamera,
					 const CIPCPictureRecord& pict,								
					 CIPCFields& fields,
					 WORD wArchiveNr);
	BOOL SaveMedia(const CIPCMediaSampleRecord& media,
					 WORD wArchiveNr);
	void ConfirmRecordedFrame();
	void SaveAlarmData(CIPCFields& fields,
						CSecID camID,
						WORD wArchiveNr);
	void ConfirmLivePicture(CCamera* pCamera,const CIPCPictureRecord& pict, DWORD x, DWORD y);

	void SetReset(BOOL bProcess, BOOL bDatabase, BOOL bInformClients);

	void OnRequestGetValue(CSecID id, const CString& sKey, DWORD dwParam, CIPC* pCipc);
	void OnRequestSetValue(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam, CIPC* pCipc);

	void OnRequestSetAudioCam(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam, CIPC* pCipc, CWK_Profile& wkp);
	void OnRequestSetAudioRec(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam, CIPC* pCipc, CWK_Profile& wkp);

	void SetOutputName(DWORD dwClientID, COutput* pOutput, const CString& sName);
	void SetVideoOut(int iCard, CSecID camID);
	void ActivateAlarmOutput();
	void SwitchAlarmOutput(BOOL bClose);
	void CheckAlarmOutput(DWORD dwWKTick);
	void OnSoftwareAlarm(CSecID id, BOOL bAlarm, WORD wX, WORD wY);
	void OnOutputChanged(BOOL bPTZChanged);
	void OnOutputClientDisconnected(COutputClient* pOutputClient);
	void OutStandingPictsChanged(int iOutstandingPicts);
	void OnAudioClientDisconnected(CAudioClient* pAudioClient);

	void OnTimer();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVProcessApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDVProcessApp)
	afx_msg void OnFileReset();
	afx_msg void OnDateCheck();
	afx_msg void OnTestNewDay();
	afx_msg void OnTestNewWeek();
	afx_msg void OnUpdateStoragePane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateICPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOCPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateACPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIGPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOGPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAGPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLEDPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAudioPane(CCmdUI* pCmdUI);
	afx_msg void OnTestNewMinute();
	afx_msg void OnTestNewHour();
	afx_msg void OnAudioReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void RegisterWindowClass();
	void Reset();
	void CheckReset();
	void ReadSettings();
	void WriteSettings(BOOL bShutdown);
	

	void CreateInputOutput(int iNum);
	void InternalLink(CInputGroup* pAlarm, CInputGroup* pMD, CCameraGroup* pCamera, int iNrExternal);

	void ShutDown();

	void ShutDownAudio();
	void CreateAudio();
	void ResetAudio();

	void ConnectToStorage();
	BOOL InitStorage(BOOL bDeleteAllowed);
	BOOL ResetStorage();
	void InitTimer(int iNum);
	void RecalcTimer(CSystemTime& st);
	BOOL InitDrives(CIPCDrives& drives, CWK_Profile& wkp);
	BOOL InitDrive(CIPCDrive* pDrive);
	void OnNewMinute(CSystemTime& st);
	void OnNewHour();
	void OnNewDay(BOOL bNewWeek);
	void ExportHKLMSoftware();
	void HandleExpansionCode(CIPC* pCipc,const CString& sValue);

	void CameraNameChanged(CCamera* pOutput);
	void RelayNameChanged(CRelay* pOutput);

private:
	// connection to databae
	CIPCDatabaseProcess*	m_pCIPCDatabaseProcess;

	// state
	BOOL			m_bResetting;
	BOOL			m_bMustResetProcess;
	BOOL			m_bMustInitDatabase;
	BOOL			m_bMustResetDatabase;
	BOOL			m_bShutDown;
	CSystemTime		m_currentTime;

	// incoming connections
	CIPCServerControlProcess*	m_pCIPCServerControlProcess;
	CConnectionRecords			m_InputConnectionRecords;
	CConnectionRecords			m_OutputConnectionRecords;
	CConnectionRecords			m_AudioConnectionRecords;
	// established connections
	CInputClients				m_InputClients;
	COutputClients				m_OutputClients;
	CAudioClients				m_AudioClients;

	// unit connections
	CInputList		m_Inputs;
	COutputList		m_Outputs;
	CAudioList		m_Audios;

	// VOut Switching Thread
	CVOutThread m_VOutThread;

	// timer
	CSecTimerArray	m_Timers;

	// PINs
	CString m_sSuperPIN;
	CString m_sOperatorPIN;

	CSettings	m_Settings;

	// Settings
	BOOL		m_bDetectorIgnoreTimer;
	VOUT_MODE	m_eVOutModePort[NR_OF_CARDS][NR_OF_VOUT];
	CStringArray m_sAlarmDialingNumbers;
	BOOL		 m_bAlarmOutput;
	BOOL		 m_bAlarmClosed;
	DWORD		 m_dwLastAlarmOutputTime;

	// Pictures to store
	LONG		m_dwFramesToRecord;
	BOOL		m_bStoreLED;

	// Alarm Connections
	CCallThread m_CallThread;

	// Number Of Codecs
	UINT	m_nNrOfJacobs;
	UINT    m_nNrOfAllowedJacobs;
	UINT	m_nNrOfSavics;
	UINT	m_nNrOfTashas;
	UINT	m_nNrOfQs;

	CEncoderThread m_EncoderThread;
	CWK_Debugger*	m_pDebugger;

public:
	BOOL m_bErrorOccurred;
};
extern CDVProcessApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVProcessApp::IsResetting() const
{
	return m_bResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVProcessApp::IsShutDown() const
{
	return m_bShutDown;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVProcessApp::IsUpAndRunning() const
{
	return !IsResetting() && !IsShutDown();
}
/////////////////////////////////////////////////////////////////////////////
inline CInputList& CDVProcessApp::GetInputGroups()
{
	return m_Inputs;
}
/////////////////////////////////////////////////////////////////////////////
inline COutputList& CDVProcessApp::GetOutputGroups()
{
	return m_Outputs;
}
/////////////////////////////////////////////////////////////////////////////
inline CAudioList& CDVProcessApp::GetAudioGroups()
{
	return m_Audios;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCAudioServer* CDVProcessApp::GetAudioGroup()
{
	if (m_Audios.GetSize() == 1)
	{
		return m_Audios.GetAtFast(0);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline CInputClients& CDVProcessApp::GetInputClients()
{
	return m_InputClients;
}
/////////////////////////////////////////////////////////////////////////////
inline COutputClients& CDVProcessApp::GetOutputClients()
{
	return m_OutputClients;
}
/////////////////////////////////////////////////////////////////////////////
inline CAudioClients& CDVProcessApp::GetAudioClients()
{
	return m_AudioClients;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecTimerArray& CDVProcessApp::GetTimers()
{
	return m_Timers;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CDVProcessApp::GetSuperPIN() const
{
	return m_sSuperPIN;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CDVProcessApp::GetOperatorPIN() const
{
	return m_sOperatorPIN;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVProcessApp::GetDetectorIgnoreTimer() const
{
	return m_bDetectorIgnoreTimer;
}
/////////////////////////////////////////////////////////////////////////////
inline VOUT_MODE CDVProcessApp::GetVOutModePort(int iCard, int iPort) const
{
	return m_eVOutModePort[iCard][iPort];
}
/////////////////////////////////////////////////////////////////////////////
inline CStringArray& CDVProcessApp::GetAlarmHosts()
{
	return m_sAlarmDialingNumbers;
}
/////////////////////////////////////////////////////////////////////////////
inline CCallThread& CDVProcessApp::GetCallThread()
{
	return m_CallThread;
}
/////////////////////////////////////////////////////////////////////////////
inline CEncoderThread& CDVProcessApp::GetEncoderThread()
{
	return m_EncoderThread;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVProcessApp::GetAlarmOutput() const
{
	return m_bAlarmOutput;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCDatabaseProcess* CDVProcessApp::GetDatabase()
{
	return m_pCIPCDatabaseProcess;
}
/////////////////////////////////////////////////////////////////////////////
inline CSettings& CDVProcessApp::GetSettings()
{
	return m_Settings;
}
/////////////////////////////////////////////////////////////////////////////
inline const CInput* CDVProcessApp::GetPTZInput()
{
	return m_Inputs.GetPTZInput();
}

extern CDVProcessApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVPROCESS_H__43C9E48F_8B84_11D3_99EB_004005A19028__INCLUDED_)
