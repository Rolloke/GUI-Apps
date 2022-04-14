// DBS.h : main header file for the DBS application
//

#if !defined(AFX_DBS_H__AB5B2365_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_DBS_H__AB5B2365_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CIPCServerControlDBS;

/////////////////////////////////////////////////////////////////////////////
#include "Clients.h"
#include "Archivs.h"
#include "SaveThread.h"
#include "VerifyThread.h"
#include "Conversion.h"
#include "Drives.h"
#include "QueryThread.h"
#include "BackupThread.h"
#include "MoveThread.h"
#include "VDBSequenceHashEntry.h"
#include "MainFrm.h"

#define DBS_LOCK_TIMEOUT 250
#define ID_DATE_CHECK	 0x7000

class CInitializeThread;
class CCheckDriveThread;

/////////////////////////////////////////////////////////////////////////////
// CDBSApp:
// See DBS.cpp for the implementation of this class
//

class CDBSApp : public CWinApp
{
	friend class CInitializeThread;
	friend class CCheckDriveThread;
public:
	CDBSApp();
	virtual ~CDBSApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBSApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	// attributes
public:
	inline BOOL			  IsReadOnly() const;
	inline BOOL			  IsDTS() const;
	inline BOOL			  IsIDIP() const;
	inline const CString& GetStartDrive() const;
	inline CMainFrame* GetMainFrame();
	inline CIPCDatabaseServer* GetCIPCDatabaseToServer();

	inline BOOL IsResetting() const;
	inline BOOL IsFullResetting() const;
	inline BOOL IsFastResetting() const;

	inline BOOL	TraceDrives() const;
	inline BOOL	TraceQueue() const;
	inline BOOL	TraceRequests() const;
	inline BOOL	TraceSequence() const;
	inline BOOL	TraceCodebase() const;
	inline BOOL	TraceQuery() const;
	inline BOOL	TraceOpenClose() const;
	inline BOOL	TraceBackup() const;
	inline BOOL	TraceDelete() const;
	inline BOOL	TraceQueryResults() const;
	inline BOOL TracePreMove() const;
	inline BOOL TraceLocks() const;
	inline BOOL IsOffsetCorrected() const;
	inline BOOL IsNetworkStorageAllowed() const;

	CHostArray& GetHosts();
	int			GetIntValue(CWK_Profile &wkp, LPCTSTR szValue, int nDefault);
	void		LoadHosts();
	inline CVDBSequenceMap& GetSequenceMap();
	inline CWK_Debugger* GetDebugger();
// Implementation
public:
	void AddConnectionRecord(const CConnectionRecord& c);
	void OnConnectionRecord();
	void OnRequestQuery();
	void OnRequestBackup();
	void OnBackupFinished();
	void OnQueryFinished();
	void CancelBackup(DWORD dwUserData);
	BOOL PostDriveChanged(BOOL bRemoved, _TCHAR cRoot);
	BOOL HandleFileException(CFileException*pFE, BOOL bThrow);
	BOOL HandleDriveError(DWORD dwLastError, CString sDrive);
	DWORD TranslateDBError(int nDBError);

	void LVAddString(const CString& s);
	BOOL LVClear();
	void LVDrives();
	void LVArchivs();
	void LVClients();
	void LVThreads();

	void DoSendAlarmFuel();
	void OnConversionComplete();
	void OffsetCorrected();

	void OnDeviceArrived(DEV_BROADCAST_HDR* pHdr);
	void OnDeviceRemoved(DEV_BROADCAST_HDR* pHdr);
	void OnDriveChanged(BOOL bRemoved, _TCHAR cRoot);
	BOOL CheckRemoveNetworkDrive(CString sDrive);

	void DoReset();
	void CheckInitializeThreadFinished();
	BOOL CreateArchiveForHost(CSecID idHost,BOOL bAlarm,BOOL bSearch);
	void OnTimer();
	void ReloadSequenceMap(BOOL bFromReset);

#if _MFC_VER >= 0x0710
	int	 SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

private:
	void InitDTS();
	void InitIDIP();

	void SendAlarmFuel();
	void RegisterWindowClass();
	void ExitThreads();
	void Clear();
	void Initialize();
	BOOL Exit();

	void StartFullReset();
	void FastReset();
	void FinishFullReset();
	BOOL IsChanged(CWK_Profile& wkp, BOOL bDoChanges); 

	void StartThreads();
	void InitializeReadOnly();
	void Convert();
	void InitFields(CWK_Profile& wkp);
	void InitFieldsRO();
	void CancelQueries();
	void InformClients();
	void InformClientsAboutDrives();
	void InformClientsAboutArchives();
	void InformClientsAboutDatabaseFields();
	BOOL CheckDrives(BOOL bCheck);

	void InitReadOnlyModus();
	void ExitReadOnlyModus();

	void OnBackupDriveArrived(CIPCDrive* pDrive);
	void OnBackupDriveRemoved(CIPCDrive* pDrive);

	//{{AFX_MSG(CDBSApp)
	afx_msg void OnFileReset();
	afx_msg void OnFileActualize(UINT nID);
	afx_msg void OnFileDeleteall();
	afx_msg void OnTraceQueues();
	afx_msg void OnUpdateTraceQueues(CCmdUI* pCmdUI);
	afx_msg void OnTraceRequests();
	afx_msg void OnTraceSequences();
	afx_msg void OnUpdateTraceSequences(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceRequests(CCmdUI* pCmdUI);
	afx_msg void OnTraceDrives();
	afx_msg void OnUpdateTraceDrives(CCmdUI* pCmdUI);
	afx_msg void OnViewArchives();
	afx_msg void OnUpdateViewArchives(CCmdUI* pCmdUI);
	afx_msg void OnViewClients();
	afx_msg void OnUpdateViewClients(CCmdUI* pCmdUI);
	afx_msg void OnViewDrives();
	afx_msg void OnViewThreads();
	afx_msg void OnUpdateViewThreads(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDrives(CCmdUI* pCmdUI);
	afx_msg void OnTraceCodebase();
	afx_msg void OnUpdateTraceCodebase(CCmdUI* pCmdUI);
	afx_msg void OnTraceQuery();
	afx_msg void OnUpdateTraceQuery(CCmdUI* pCmdUI);
	afx_msg void OnTraceOpenClose();
	afx_msg void OnUpdateTraceOpenClose(CCmdUI* pCmdUI);
	afx_msg void OnTraceDelete();
	afx_msg void OnUpdateTraceDelete(CCmdUI* pCmdUI);
	afx_msg void OnTraceBackup();
	afx_msg void OnUpdateTraceBackup(CCmdUI* pCmdUI);
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CIPCDatabaseServer*		m_pCIPCDatabaseServerToServer;
	CIPCServerControlDBS*	m_pCIPCServerControlDBS;
	CConnectionRecords		m_aConnectionRecords;

	BOOL	m_bViewDrives;
	BOOL	m_bViewClients;
	BOOL	m_bViewArchivs;
	BOOL	m_bViewThreads;

	BOOL	m_bReadOnlyModus;
	CString	m_sStartDrive;

public:
	CArchivInfoArray m_ArchivInfos;
	CIPCFields		m_Fields;
	CClients		m_Clients;
	CArchivs		m_Archives;
	CDrives			m_Drives;			
	CString			m_sDisconnectedDrives;			
	CString			m_sArrivedDrives;
	CConversions	m_Conversions;

	CQueries		m_Queries;
	CQueryThreads	m_QueryThreads;
	
	CSaveThread		m_SaveThread;
	CVerifyThread   m_VerifyThread;
	CMoveThread     m_MoveThread;
	
	CBackups		m_Backups;
	CBackupThreads	m_BackupThreads;

private:
	volatile BOOL	m_bFullResetting;
	volatile BOOL	m_bFastResetting;
	volatile BOOL	m_bExiting;
	volatile BOOL	m_bPosted;

	BOOL			m_bTraceDrives;
	BOOL			m_bTraceQueue;
	BOOL			m_bTraceRequests;
	BOOL			m_bTraceSequence;
	BOOL			m_bTraceCodebase;
	BOOL			m_bTraceQuery;
	BOOL			m_bTraceOpenClose;
	BOOL			m_bTraceBackup;
	BOOL			m_bTraceDelete;
	BOOL			m_bTraceQueryResults;
	BOOL			m_bTracePreMove;
	BOOL			m_bTraceLocks;
	BOOL			m_bOffsetCorrected;
	BOOL			m_bAutoReduceArchiveSize;
	BOOL			m_bAllowNetworkStorage;
	BOOL			m_bThreadPoolEnabled;

	InternalProductCode		m_InternalProductCode;
	CCriticalSection		m_csHosts;
	CHostArray				m_Hosts;

	CInitializeThread* m_pInitializeThread;
	friend class CInitializeThread;
	CCheckDriveThread *m_pCheckDrivesThread;

	CVDBSequenceMap	m_SequenceMap;
	CWK_Debugger*	m_pDebugger;

public:
	CString m_sMapDirectory;
	CString m_sDatabaseDir;

	friend class CMainFrame;
	afx_msg void OnFileFastReset();
};
extern CDBSApp theApp;

/////////////////////////////////////////////////////////////////////////////
inline BOOL CDBSApp::IsReadOnly() const
{
	return m_bReadOnlyModus;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDBSApp::IsDTS() const
{
	return m_InternalProductCode == IPC_DTS;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDBSApp::IsIDIP() const
{
	return m_InternalProductCode != IPC_DTS;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CDBSApp::GetStartDrive() const
{
	return m_sStartDrive;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceDrives() const
{
	return m_bTraceDrives;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceQueue() const
{
	return m_bTraceQueue;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceRequests() const
{
	return m_bTraceRequests;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceSequence() const
{
	return m_bTraceSequence;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceCodebase() const
{
	return m_bTraceCodebase;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceOpenClose() const
{
	return m_bTraceOpenClose;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDBSApp::IsResetting() const
{
	return m_bFullResetting || m_bFastResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDBSApp::IsFullResetting() const
{
	return m_bFullResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDBSApp::IsFastResetting() const
{
	return m_bFastResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceQuery() const
{
	return m_bTraceQuery;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceBackup() const
{
	return m_bTraceBackup;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceDelete() const
{
	return m_bTraceDelete;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceQueryResults() const
{
	return m_bTraceQueryResults;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TracePreMove() const
{
	return m_bTracePreMove;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::TraceLocks() const
{
	return m_bTraceLocks;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDBSApp::IsOffsetCorrected() const
{
	return m_bOffsetCorrected;
}
/////////////////////////////////////////////////////////////////////////////
inline CVDBSequenceMap& CDBSApp::GetSequenceMap()
{
	return m_SequenceMap;
}
/////////////////////////////////////////////////////////////////////////////
inline CWK_Debugger* CDBSApp::GetDebugger()
{
	return m_pDebugger;
}
/////////////////////////////////////////////////////////////////////////////
inline CMainFrame* CDBSApp::GetMainFrame()
{
	ASSERT(m_pMainWnd != NULL);
	ASSERT_KINDOF(CMainFrame, m_pMainWnd);
	return (CMainFrame*)m_pMainWnd;
}
inline BOOL CDBSApp::IsNetworkStorageAllowed() const
{
	return m_bAllowNetworkStorage;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCDatabaseServer* CDBSApp::GetCIPCDatabaseToServer()
{
	return m_pCIPCDatabaseServerToServer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL DbsCopyFile(const CString& source, const CString& dest, BOOL bFailIfExist);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBS_H__AB5B2365_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
