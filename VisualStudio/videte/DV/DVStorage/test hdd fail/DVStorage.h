// DVStorage.h : main header file for the DVSTORAGE application
//

#if !defined(AFX_DVSTORAGE_H__F4B60C0B_8ACE_11D3_99EA_004005A19028__INCLUDED_)
#define AFX_DVSTORAGE_H__F4B60C0B_8ACE_11D3_99EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define DVS_TIMEOUT			250
#define DVS_FLUSHTIME		60
//#define DVS_SEQUENCE_SIZE	(16 * 1024 * 1024)

#include "Collections.h"
#include "Drives.h"
#include "Client.h"
#include "SearchThread.h"
#include "BackupThread.h"
#include "SequenceHashEntry.h"
#include "SaveThread.h"

#define REGKEY_STORAGE _T("Storage")
#define _TRACE_TIME_VALUES_ 1

#define ADD_MICRO_TIME_SPAN(TIME, TCK, OLDTCK) TCK    = theTimer.GetMicroTicks(); \
												TIME  += (DWORD) (TCK.QuadPart - OLDTCK.QuadPart); \
					                            OLDTCK = TCK;

class CIPCServerControlStorage;
class CInitializeThread;
class CBackupScanThread;
/////////////////////////////////////////////////////////////////////////////
// CDVStorageApp:
// See DVStorage.cpp for the implementation of this class
//
#include "DlgSettings.h"

class CDVStorageApp : public CWinApp
{
public:
	CDVStorageApp();
	virtual ~CDVStorageApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVStorageApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	// attributes
public:
	inline BOOL IsResetting() const;
	inline BOOL IsFastResetting() const;
	inline BOOL IsReadOnly() const;
	inline BOOL IsInBackup() const;
	inline BOOL IsShuttingDown() const;
	inline BOOL GetNoFrag() const;
	inline const CString& GetStartDrive() const;
	inline CVDBSequenceMap& GetSequenceMap();

	inline CIPCDatabaseStorage* GetCIPCDatabaseStorage();
	int		GetInValue(LPCTSTR sKey, int nDefault=0);
	BOOL	HandleFileException(DWORD dwLastError, LPCTSTR sFilename);

	// operations
public:
	void OnError();
	void EmergencyExit();

	void OnNewDay();

	void OnRequestBackup();
	void OnBackupFinished();
	void OnRequestQuery();
	void OnQueryFinished();
	void OnBackupScanFinished();
	void OnVerifyFinished();

// Implementation
	//{{AFX_MSG(CDVStorageApp)
	afx_msg void OnFileReset();
	afx_msg void OnUpdatePaneClients(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneProcess(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneFPS(CCmdUI* pCmdUI);
	afx_msg void OnFileFastReset();
	afx_msg void OnDateCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	void SetCollectionName(DWORD dwClientID, CSecID id, const CString& sName);
	void AddConnectionRecord(const CConnectionRecord& c);
	void OnConnectionRecord();
	void OnSearch();

	void OnBackup();
	void CancelBackup(DWORD dwUserData);

	void OnDeviceArrived(DEV_BROADCAST_HDR* pHdr);
	void OnDeviceRemoved(DEV_BROADCAST_HDR* pHdr);
	
	void PrepareToShutDown_NoArchives(); //if no archives were loaded from Reg, restart DVStorage

private:
	CString GetSupervisorPIN() const;
	CString GetOperatorPIN() const;
	void RegisterWindowClass();
	void ExitThreads();
	void StartFullReset(BOOL bWriteSequenceMap);
	void FinishFullReset();
	void FastReset();
	BOOL IsChanged(CWK_Profile& wkp, BOOL bDoChanges); 
	void ShutDown(BOOL bWriteSequenceMap);
	void Initialize(CWK_Profile& wkp);
	void StartThreads();
	void InitializeReadOnly();
	void InformClients(CWK_Profile& wkp);
	void CancelSearchs();
	void InitReadOnlyModus();
	void ExitReadOnlyModus();

	void OnBackupDriveArrived(CIPCDrive* pDrive);
	void OnBackupDriveRemoved(CIPCDrive* pDrive);

	BOOL ReadOldSequenceMap();

private:
	BOOL m_bResetting;
	BOOL m_bFastResetting;
	BOOL m_bFullResetting;
	BOOL m_bPosted;
	BOOL m_bShuttingDown;
	BOOL m_bNoFrag;
	CSystemTime				m_currentTime;

	CIPCServerControlStorage* m_pCIPCServerControlStorage;
	CIPCDatabaseStorage*	  m_pCIPCDatabaseStorage;
	CConnectionRecords		m_aConnectionRecords;

	BOOL	m_bReadOnlyModus;
	BOOL	m_bClientStarted;
	CString	m_sStartDrive;
	CStringArray m_sArrivedDrives;

	CVDBSequenceMap	m_SequenceMap;
	BOOL			m_bPrepareToShutDown_NoArchives;

public:
	BOOL			m_bTraceOpen;
	BOOL			m_bTraceBackup;
	BOOL			m_bTraceVerify;
	BOOL			m_bTraceSaveThread;
	BOOL			m_bEqualArchiveDistribution;
	CCollections	m_Archives;
	CDrives			m_Drives;			
	CClients		m_Clients;
	CSearchThreads  m_SearchThreads;
	CSearchs		m_Searchs;
	CArchivInfoArray m_ArchivInfos;
	CBackups		m_Backups;
	CBackupThreads	m_BackupThreads;
	CSaveThread		m_SaveThread;
	CDlgSettings	m_Dlg;

private:
	CInitializeThread*	m_pInitializeThread;
	CBackupScanThread*	m_pBackupScanThread;

	friend class CInitializeThread;
};
/////////////////////////////////////////////////////////////////////////////
extern CDVStorageApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVStorageApp::IsResetting() const
{
	return m_bResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVStorageApp::IsFastResetting() const
{
	return m_bFastResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVStorageApp::IsInBackup() const
{
	return m_BackupThreads.GetSize()>0;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVStorageApp::IsReadOnly() const
{
	return m_bReadOnlyModus;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CDVStorageApp::GetStartDrive() const
{
	return m_sStartDrive;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVStorageApp::IsShuttingDown() const
{
	return m_bShuttingDown;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVStorageApp::GetNoFrag() const
{
	return m_bNoFrag;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCDatabaseStorage* CDVStorageApp::GetCIPCDatabaseStorage()
{
    return m_pCIPCDatabaseStorage;
}
/////////////////////////////////////////////////////////////////////////////
inline CVDBSequenceMap& CDVStorageApp::GetSequenceMap()
{
	return m_SequenceMap;
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTORAGE_H__F4B60C0B_8ACE_11D3_99EA_004005A19028__INCLUDED_)
