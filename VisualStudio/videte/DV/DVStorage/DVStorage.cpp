// DVStorage.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DVStorage.h"

#include "MainFrm.h"
#include "DVStorageDoc.h"
#include "DVStorageView.h"

#include "CIPCServerControlStorage.h"
#include "InitializeThread.h"
#include "BackupScanThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "wk_timer.h"
CWK_Timer theTimer;

/////////////////////////////////////////////////////////////////////////////
// CDVStorageApp

BEGIN_MESSAGE_MAP(CDVStorageApp, CWinApp)
	//{{AFX_MSG_MAP(CDVStorageApp)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	ON_UPDATE_COMMAND_UI(IDS_PANE_CLIENTS, OnUpdatePaneClients)
	ON_UPDATE_COMMAND_UI(IDS_PANE_PROCESS, OnUpdatePaneProcess)
	ON_UPDATE_COMMAND_UI(IDS_PANE_FPS, OnUpdatePaneFPS)
	ON_COMMAND(ID_FILE_FAST_RESET, OnFileFastReset)
	ON_COMMAND(ID_DATE_CHECK, OnDateCheck)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVStorageApp construction

CDVStorageApp::CDVStorageApp()
{
/*
	TwoWords data[5];
	TestFunction(5,data);
*/

	m_bResetting = FALSE;
	m_bFastResetting = FALSE;
	m_bFullResetting = FALSE;
	m_bPosted = FALSE;
	m_bShuttingDown = FALSE;
	m_bNoFrag = TRUE;
	m_pCIPCServerControlStorage = NULL;
	m_pCIPCDatabaseStorage = NULL;
	m_bClientStarted = FALSE;
	m_pInitializeThread = NULL;
	m_pBackupScanThread = NULL;
	m_currentTime.GetLocalTime();

	m_bTraceOpen   = FALSE;
	m_bTraceBackup = FALSE;
	m_bTraceVerify = FALSE;
	m_bTraceSaveThread = FALSE;
	m_bPrepareToShutDown_NoArchives = FALSE;
}

CDVStorageApp::~CDVStorageApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDVStorageApp object

CDVStorageApp theApp;

/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::RegisterWindowClass()
{
    WNDCLASS  wndclass;

    // register window class
    wndclass.style =         0;
    wndclass.lpfnWndProc =   DefWindowProc;
    wndclass.cbClsExtra =    0;
    wndclass.cbWndExtra =    0;
    wndclass.hInstance =     m_hInstance;
    wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
    wndclass.hCursor =       LoadCursor(IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
    wndclass.lpszMenuName =  0L;
    wndclass.lpszClassName = WK_APP_NAME_DB_SERVER;

    AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
// CDVStorageApp initialization

BOOL CDVStorageApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_DB_SERVER)==FALSE) 
	{
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	RegisterWindowClass();
	InitReadOnlyModus();
		
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDVStorageDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDVStorageView));
	AddDocTemplate(pDocTemplate);

	if (IsReadOnly())
	{
		InitDebugger(_T("dvsro.log"),WAI_DATABASE_SERVER);
#ifndef _DEBUG
		m_nCmdShow = SW_HIDE;
#endif
	}
	else
	{
		InitDebugger(_T("dvs.log"),WAI_DATABASE_SERVER);
	}

	
	// do not touch
	if (m_nCmdShow!=0) // do not touch if already hidden
	{	
		m_nCmdShow = SW_HIDE;
	}
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->UpdateWindow();

	StartFullReset(FALSE);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::InitReadOnlyModus()
{
	_TCHAR szBuffer[_MAX_PATH];
	CString sFile;
	CString sTest1,sTest2;
	int p;

	GetModuleFileName(m_hInstance,szBuffer,sizeof(szBuffer));
	sFile = szBuffer;
#ifdef _DEBUG
	m_sStartDrive = _T("g:\\");
	//m_sStartDrive = sFile.Left(3);
#else
	m_sStartDrive = sFile.Left(3);
#endif

	p = sFile.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sTest1 = m_sStartDrive + _T("autorun.inf");
		sTest2 = m_sStartDrive + _T("dvb0001");
		if (DoesFileExist(sTest1) && DoesFileExist(sTest2))
		{
			m_bReadOnlyModus = TRUE;
		}
	}

	if (m_bReadOnlyModus)
	{
		WK_ALONE(WK_DB_SERVER_READ_ONLY);
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::ExitReadOnlyModus()
{
	// clean up something
	if (IsReadOnly())
	{
		DeleteFile(_T("c:\\log\\dvsro.log"));
		DeleteFile(_T("c:\\log\\dvcro.log"));
		DeleteFile(_T("c:\\log\\audiounit1ro.log"));
		RemoveDirectory(_T("c:\\log"));

		//CD auswerfen (Schublade auf) wieder rausgenommen, da es teilw. 
		//zu Abstürzen kan.
//		CIPCDrive d(m_sStartDrive,DVR_CDR_BACKUP_DRIVE);
//		d.OpenCD();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::StartFullReset(BOOL bWriteSequenceMap)
{
	if (m_BackupThreads.GetSize()==0)
	{
		m_bResetting = TRUE;
		if (!m_bFullResetting)
		{
			m_bFullResetting = TRUE;
			((CFrameWnd*)m_pMainWnd)->RedrawWindow();

			WK_TRACE(_T("FULL RESET ...\n"));
			WK_TRACE(_T("Main Thread is %08lx\n"),GetCurrentThreadId());

			ShutDown(bWriteSequenceMap);

			if (IsReadOnly())
			{
				InitializeReadOnly();
				WK_TRACE(_T("FULL RESET ... SUCCESS\n"));
				m_bResetting = FALSE;
				m_bFullResetting = FALSE;
				((CFrameWnd*)m_pMainWnd)->RedrawWindow();
			}
			else
			{
				CString sPath;
#ifndef _DEBUG
				GetModuleFileName(m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
				sPath.ReleaseBuffer();
				sPath = sPath.Left(sPath.ReverseFind('\\'));
#else
				sPath = _T("c:\\dv");
#endif
				CString sFilepath = sPath + _T("\\sequence.dbf");
				if (DoesFileExist(sPath + _T("\\sequence.dat")))
				{
					m_SequenceMap.Create(sFilepath);
					ReadOldSequenceMap();
				}
				else
				{
					m_SequenceMap.Load(sFilepath);
				}

				m_pInitializeThread = new CInitializeThread();
				m_pInitializeThread->StartThread();
			}
		}
	}
	else
	{
		WK_TRACE(_T("cannot reset during backup\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::FinishFullReset()
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	InformClients(wkp);

	StartThreads();

	WK_TRACE(_T("FULL RESET ... SUCCESS\n"));
	m_pMainWnd->InvalidateRect(NULL);
	
	if (m_bPrepareToShutDown_NoArchives)
	{
		//no archives were loaded from registry, restart DVStorage
		WK_TRACE_ERROR(_T("*** Shut down DVStorage because no archives found in Registry\n"));
		EmergencyExit();
	}
	m_bResetting = FALSE;
	m_bFullResetting = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVStorageApp::IsChanged(CWK_Profile& wkp, BOOL bDoChanges)
{
	BOOL bDrivesChanged = FALSE;
	BOOL bArchivesChanged = FALSE;

	// Fast Reset Drives
	CIPCDrives newDrives;
	CIPCDrive* pDrive;
	newDrives.Init(wkp);

	// first find out new drives
	for (int d=0;d<newDrives.GetSize();d++)
	{
		if (newDrives.GetAtFast(d)->IsDatabase())
		{
			pDrive = m_Drives.GetDrive(newDrives.GetAtFast(d)->GetRootString());

			if (pDrive)
			{
				if (pDrive->IsDatabase())
				{
					// unchanged
				}
				else
				{
					// change
					if (bDoChanges)
					{
						WK_TRACE(_T("new drive %s\n"),pDrive->GetRootString());
						pDrive->SetUsedAs(DVR_DB_DRIVE);
						m_Archives.ScanDatabaseDrive(pDrive);
					}
					bDrivesChanged = TRUE;
				}
			}
			else
			{
				// a new drive !?
				if (bDoChanges)
				{
					WK_TRACE(_T("new drive %s\n"),pDrive->GetRootString());
					pDrive = new CIPCDrive(*newDrives.GetAtFast(d));
					m_Drives.SafeAdd(pDrive);
					m_Archives.ScanDatabaseDrive(pDrive);
				}
				bDrivesChanged = TRUE;
			}
		}
		else if (newDrives.GetAtFast(d)->IsWriteBackup())
		{
			pDrive = m_Drives.GetDrive(newDrives.GetAtFast(d)->GetRootString());

			if (pDrive)
			{
				if (pDrive->IsWriteBackup())
				{
					// unchanged
				}
				else
				{
					// change
					if (bDoChanges)
					{
						WK_TRACE(_T("new backup drive %s\n"),pDrive->GetRootString());
						pDrive->SetUsedAs(DVR_WRITE_BACKUP_DRIVE);
					}
					bDrivesChanged = TRUE;
				}
			}
			else
			{
				// a new drive !?
				if (bDoChanges)
				{
					WK_TRACE(_T("new backup drive %s\n"),pDrive->GetRootString());
					pDrive = new CIPCDrive(*newDrives.GetAtFast(d));
					m_Drives.SafeAdd(pDrive);
				}
				bDrivesChanged = TRUE;
			}
		}
	}

	// then delete old drives
	for (d=0;d<m_Drives.GetSize();d++)
	{
		if (m_Drives.GetAtFast(d)->IsDatabase())
		{
			pDrive = newDrives.GetDrive(newDrives.GetAtFast(d)->GetRootString());

			if (pDrive)
			{
				if (pDrive->IsDatabase())
				{
					// unchanged
				}
				else
				{
					// change to
					if (bDoChanges)
					{
						WK_TRACE(_T("delete drive %s\n"),m_Drives.GetAtFast(d)->GetRootString());
						m_Drives.GetAtFast(d)->SetUsedAs(DVR_NORMAL_DRIVE);
						m_Archives.DeleteDatabaseDrive(m_Drives.GetAtFast(d)->GetRootString());
					}
					bDrivesChanged = TRUE;
				}
			}
			else
			{
				// drive does no longer exist !?
				if (bDoChanges)
				{
					WK_TRACE(_T("delete drive %s\n"),m_Drives.GetAtFast(d)->GetRootString());
					m_Archives.DeleteDatabaseDrive(newDrives.GetAtFast(d)->GetRootString());
				}
				bDrivesChanged = TRUE;
			}
		}
		else if (m_Drives.GetAtFast(d)->IsWriteBackup())
		{
			pDrive = newDrives.GetDrive(newDrives.GetAt(d)->GetRootString());

			if (pDrive)
			{
				if (pDrive->IsWriteBackup())
				{
					// unchanged
				}
				else
				{
					// change to 
					if (bDoChanges)
					{
						WK_TRACE(_T("delete backup drive %s\n"),m_Drives.GetAtFast(d)->GetRootString());
						m_Drives.GetAtFast(d)->SetUsedAs(DVR_NORMAL_DRIVE);
					}
					bDrivesChanged = TRUE;
				}
			}
		}
	}

	if (   bDrivesChanged
		&& bDoChanges)
	{

		m_Clients.DoIndicateDrives();
	}

	if (   !bDoChanges
		&& bDrivesChanged)
	{
		// no check archives necessary if drives already changed
		return TRUE;
	}

	CArchivInfo* pArchivInfo;
	CCollection* pCollection;
	CArchivInfoArray newArchives;

	newArchives.Load(wkp);

	// first find out deleted archives
	for (int c = m_Archives.GetSize()-1;c>=0;c--)
	{
		pCollection = m_Archives.GetAt(c);
		WORD wCollectionNr = pCollection->GetNr();
		CSecID id(SECID_GROUP_ARCHIVE,wCollectionNr);

		//			TRACE(_T("checking existing %s %04lx\n"),pCollection->GetName(),wCollectionNr);
		pArchivInfo = newArchives.GetArchivInfo(id);

		if (pArchivInfo == NULL)
		{
			// it's deleted, so delete the Collection
			if (bDoChanges)
			{
				CString s = pCollection->GetName();
				WK_TRACE(_T("deleting %s %d\n"),s,wCollectionNr);
				if (m_Archives.DeleteCollection(wCollectionNr))
				{
					WK_TRACE(_T("deleted %s %d\n"),s,wCollectionNr);
				}
				else
				{
					WK_TRACE(_T("cannot delete %s %d\n"),s,wCollectionNr);
				}
			}
			pCollection = NULL;
			bArchivesChanged = TRUE;
		}
		else
		{
			// new size/name ?
			if (pCollection->GetName() != pArchivInfo->GetName())
			{
				if (bDoChanges)
				{
					WK_TRACE(_T("new name %s -> %s\n"),pArchivInfo->GetName(),pCollection->GetName());
					pCollection->SetName(pArchivInfo->GetName());
					m_Clients.DoIndicateNewCollectionName((DWORD)-1,*pCollection);
				}
				bArchivesChanged = TRUE;
			}
			if (pCollection->GetSizeMB() != pArchivInfo->GetSizeMB())
			{
				if (bDoChanges)
				{
					WK_TRACE(_T("new size %d -> %d\n"),
						pCollection->GetSizeMB(),
						pArchivInfo->GetSizeMB());
					pCollection->SetSize(pArchivInfo->GetSizeMB());
				}
				bArchivesChanged = TRUE;
			}
		}
	}

	// then find out new created archives
	for (int a=0;a<newArchives.GetSize();a++)
	{
		pArchivInfo = newArchives.GetAt(a);
		// TRACE(_T("checking new %s\n"),pArchivInfo->GetName());
		WORD wCollectionNr = pArchivInfo->GetArchivNr();
		pCollection = m_Archives.GetCollection(wCollectionNr);

		if (pCollection == NULL)
		{
			// it's a new collection
			if (bDoChanges)
			{
				WK_TRACE(_T("creating %s %d\n"),pArchivInfo->GetName(),wCollectionNr);
				pCollection = new CCollection(*pArchivInfo);
				m_Archives.SafeAdd(pCollection);
				m_Clients.DoIndicateNewCollection(*pCollection);
			}
			bArchivesChanged = TRUE;
		}
		else
		{
			// gibt's schon oben!
			//				TRACE(_T("already handled %s\n"),pCollection->GetName());
		}
	}

	if (m_Archives.GetSize() != newArchives.GetSize())
	{
		WK_TRACE(_T("internal error in fast reset\n"));
	}

	return bDrivesChanged || bArchivesChanged;
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::FastReset()
{
	if (m_BackupThreads.GetSize()==0)
	{
		if (   !m_bFastResetting
			&& !m_bResetting)
		{
			m_bResetting = TRUE;
			m_bFastResetting = TRUE;
			((CFrameWnd*)m_pMainWnd)->RedrawWindow();
			
			CWK_StopWatch w;
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
			WK_TRACE(_T("FAST RESET ...\n"));
			BOOL bChanged = IsChanged(wkp,FALSE);

			if (bChanged)
			{
				m_SaveThread.StopThread();

				m_SequenceMap.Close();
				CString sPath;
#ifndef _DEBUG
				GetModuleFileName(m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
				sPath.ReleaseBuffer();
				sPath = sPath.Left(sPath.ReverseFind('\\'));
#else
				sPath = _T("c:\\dv");
#endif
				CString sFilepath = sPath + _T("\\sequence.dbf");
				m_SequenceMap.DeleteAll();
				m_SequenceMap.Load(sFilepath);

				// really do the changes
				IsChanged(wkp,TRUE);

				m_SaveThread.Reset();
				// load the new info for next full reset
				m_ArchivInfos.Load(wkp);
				m_SaveThread.StartThread();
			}
			((CFrameWnd*)m_pMainWnd)->RedrawWindow();
			w.StopWatch(_T("FAST RESET"));
			WK_TRACE(_T("FAST RESET ... SUCCESS %s\n"),bChanged ? _T("CHANGED") : _T("UNCHANGED"));
			m_bFastResetting = FALSE;
			m_bResetting = FALSE;
		}
		else
		{
			WK_TRACE(_T("cannot fast reset already in reset or fast reset\n"));
		}
	}
	else
	{
		WK_TRACE(_T("cannot fast reset during backup\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::InformClients(CWK_Profile& wkp)
{
	CArchivInfo* pArchivInfo;
	CCollection* pCollection;

	// first inform about deleted archives
	for (int a=0;a<m_ArchivInfos.GetSize();a++)
	{
		pArchivInfo = m_ArchivInfos.GetAt(a);

		if (pArchivInfo->GetType() != SICHERUNGS_RING_ARCHIV)
		{
			pCollection = m_Archives.GetCollection(pArchivInfo->GetArchivNr());

			if (pCollection == NULL)
			{
				// it's deleted
				m_Clients.DoIndicateDeleteCollection(pArchivInfo->GetArchivNr());
			}
		}
	}

	// then inform about new archives
	for (a=0;a<m_Archives.GetSize();a++)
	{
		pCollection = m_Archives.GetAt(a);
		if (!pCollection->IsPreAlarm())
		{
			CSecID id(SECID_GROUP_ARCHIVE,pCollection->GetNr());
			pArchivInfo = m_ArchivInfos.GetArchivInfo(id);

			if (pArchivInfo == NULL)
			{
				// it's new
				m_Clients.DoIndicateNewCollection(*pCollection);
			}
		}
	}

	// load the new info for next reset
	m_ArchivInfos.Load(wkp);
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::Initialize(CWK_Profile& wkp)
{
	CWK_StopWatch w;

	w.StartWatch();
	m_bNoFrag = wkp.GetInt(REGKEY_STORAGE,_T("NoFrag"),m_bNoFrag);
	m_bTraceOpen   = wkp.GetInt(REGKEY_STORAGE,_T("TraceOpen"),m_bTraceOpen);
	m_bTraceBackup = wkp.GetInt(REGKEY_STORAGE,_T("TraceBackup"),m_bTraceBackup);
	m_bTraceVerify = wkp.GetInt(REGKEY_STORAGE,_T("TraceVerify"),m_bTraceVerify);
	m_bTraceSaveThread = wkp.GetInt(REGKEY_STORAGE,_T("TraceSaveThread"),m_bTraceSaveThread);
	m_bEqualArchiveDistribution = wkp.GetInt(REGKEY_STORAGE,_T("EqualArchiveDistribution"),m_bTraceSaveThread);

	if (m_bNoFrag)
	{
		WK_TRACE(_T("NO FRAG feature is on\n"));
	}
	m_Drives.Init(wkp);
	m_Drives.Initialize();
	w.StopWatch(_T("init drives"));
	m_Drives.Trace();

	w.StartWatch();
	m_Archives.Init(wkp);
	w.StopWatch(_T("init database"));
}
/////////////////////////////////////////////////////////////////////////////
int	CDVStorageApp::GetInValue(LPCTSTR sKey, int nDefault)
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	int nVal = wkp.GetInt(REGKEY_STORAGE, sKey, nDefault);
	wkp.WriteInt(REGKEY_STORAGE, sKey, nVal);
	return nVal;
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::StartThreads()
{
	int i;
	int nDBD = 0;
	int nStorage = 0;
	BOOL bInitThreads = TRUE;
	CIPCDrive* pDrive = NULL;
	CIPCDrive* pBackupDrive = NULL;

	for (i=0;i<m_Drives.GetSize();i++)
	{
		pDrive = m_Drives.GetAtFast(i);
		if (pDrive->IsDatabase())
		{
			nDBD++;
		}
		else if (pDrive->IsWriteBackup())
		{
			pBackupDrive = pDrive;
		}
		if (pDrive->GetType() == DRIVE_FIXED)
		{
			nStorage++;
		}
	}

	// is there any drive ???
	if (   (nDBD == 0) 
		&& (m_Archives.GetSize()>0)
		)
	{
		CString s;
        s.LoadString(IDS_NO_DRIVE);
		CWK_RunTimeError runtimeError(WAI_DATABASE_SERVER, 
									  REL_REBOOT_ERROR, 
									  RTE_CONFIGURATION,
									  s);
		runtimeError.Send();
		bInitThreads = FALSE;
	}

	if (nStorage == 0)
	{
		CString s;
		s.LoadString(IDS_NO_STORAGE_DRIVE);
		CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_HARDDISK_FALURE, s);
		e.Send();
		bInitThreads = FALSE;
	}

	// is there enough space left on all drives
	if (m_Archives.GetMB() > m_Drives.GetAvailableMB())
	{
		CString s;
		s.Format(IDS_NOT_ENOUGH_SPACE,
				 m_Archives.GetMB(),
				 m_Drives.GetAvailableMB());
		CWK_RunTimeError runtimeError(WAI_DATABASE_SERVER, 
									  REL_ERROR, 
									  RTE_CONFIGURATION,
									  s);
		runtimeError.Send();
		bInitThreads = FALSE;
	}

	WK_TRACE(_T("starting threads %d\n"),m_Archives.GetSize());
	CWK_StopWatch w;
	w.StartWatch();
	m_SaveThread.StartThread();
	w.StopWatch(_T("starting threads"));

	if (pBackupDrive)
	{
		m_Archives.ScanBackupDrive(pBackupDrive,FALSE);
	}

	if (m_pCIPCServerControlStorage == NULL)
	{
		m_pCIPCServerControlStorage = new CIPCServerControlStorage();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::InitializeReadOnly()
{
	CString s;

	WK_TRACE(_T("InitializeReadOnly %s\n"),m_sStartDrive);
	CWK_StopWatch w;
	CIPCDrive* pDrive = new CIPCDrive(m_sStartDrive,DVR_READ_BACKUP_DRIVE);
	m_Drives.Add(pDrive);
	w.StopWatch(s);

	w.StartWatch();
	m_Archives.ScanBackupDrive(pDrive,FALSE);
	w.StopWatch(s);

	if (m_pCIPCServerControlStorage == NULL)
	{
		m_pCIPCServerControlStorage = new CIPCServerControlStorage();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::ShutDown(BOOL bWriteSequenceMap)
{
	m_bShuttingDown = TRUE;

	CancelSearchs();
	Sleep(10);
	ExitThreads();
	Sleep(10);
	m_Archives.SafeDeleteAll();
	Sleep(10);
	m_Drives.SafeDeleteAll();

	if (    bWriteSequenceMap
		&& !IsReadOnly())
	{
		m_SequenceMap.Flush();
		m_SequenceMap.Close();
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		wkp.WriteInt(REGKEY_STORAGE,_T("TraceOpen"),m_bTraceOpen);
		wkp.WriteInt(REGKEY_STORAGE,_T("TraceBackup"),m_bTraceBackup);
		wkp.WriteInt(REGKEY_STORAGE,_T("TraceVerify"),m_bTraceVerify);
		wkp.WriteInt(REGKEY_STORAGE,_T("TraceSaveThread"),m_bTraceSaveThread);
	}

	m_bShuttingDown = FALSE;

}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::ExitThreads()
{
	int i;
	for (i=0;i<m_SearchThreads.GetSize();i++)
	{
		m_SearchThreads.GetAt(0)->StopThread();
	}
	m_SearchThreads.SafeDeleteAll();

	m_SaveThread.StopThread();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVStorageApp::OnIdle(LONG lCount) 
{
	if (m_bFullResetting)
	{
		if (   m_pInitializeThread 
			&& !m_pInitializeThread->IsRunning())
		{
			WK_DELETE(m_pInitializeThread);
			FinishFullReset();
		}
	}

	OnConnectionRecord();
	OnRequestBackup();
	OnRequestQuery();

	m_Clients.OnIdle();

	if (!IsReadOnly())
	{
		// check database to server connection
		if (m_pCIPCDatabaseStorage == NULL)
		{
			WK_TRACE(_T("connecting to process\n"));
			m_pCIPCDatabaseStorage = new CIPCDatabaseStorage(NULL,_T("DataBaseToServer"));
		}
		else 
		{
			if (m_pCIPCDatabaseStorage->GetIsMarkedForDelete())
			{
				WK_DELETE(m_pCIPCDatabaseStorage);
			}
		}
	}
	else
	{
		if (!IsResetting())
		{
			if (   !WK_IS_RUNNING(WK_APP_NAME_DVCLIENT)
				&& !m_bClientStarted)
			{
#ifndef _DEBUG
				WinExec("audiounit.exe",SW_SHOWNORMAL);
				WinExec("dvclient.exe",SW_SHOWNORMAL);
#endif			
				m_bClientStarted = TRUE;
			}
		}
	}
	
	OnBackupFinished();
	OnQueryFinished();

	if (m_sArrivedDrives.GetSize())
	{
		CString sRoot = m_sArrivedDrives.GetAt(0);
		m_sArrivedDrives.RemoveAt(0);
		CIPCDrive* pDrive;
		for (int i=0;i<m_Drives.GetSize();i++)
		{
			pDrive = m_Drives.GetAtFast(i);
			if (0==sRoot.CompareNoCase(pDrive->GetRootString()))
			{
				pDrive->CheckSpace();
				WK_TRACE(_T("drive arrived %s %d MB all\n"),pDrive->GetRootString(),pDrive->GetMB());
				if (pDrive->GetMB()>0)
				{
					OnBackupDriveArrived(pDrive);
				}
				break;
			}
		}
	}

	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
int CDVStorageApp::ExitInstance() 
{
	WK_TRACE(_T("EXIT ...\n"));

	WK_DELETE(m_pCIPCServerControlStorage);

	m_Searchs.SafeDeleteAll();
	Sleep(10);
	m_Clients.SafeDeleteAll();
	Sleep(10);
	WK_DELETE(m_pCIPCDatabaseStorage);
	Sleep(10);

	ShutDown(TRUE);

	WK_TRACE(_T("EXIT ... SUCCESS\n"));

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));

	Sleep(10);

	CloseDebugger();
	
	ExitReadOnlyModus();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnError()
{
	if (   WK_IS_WINDOW(m_pMainWnd) 
		&& !IsResetting()
		&& !IsFastResetting()
		&& !IsShuttingDown())
	{
		if (!m_bPosted)
		{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
			m_bPosted = TRUE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::EmergencyExit()
{
	if (   WK_IS_WINDOW(m_pMainWnd) 
		&& !IsResetting()
		&& !IsFastResetting()
		&& !IsShuttingDown())
	{
		m_pMainWnd->PostMessage(WM_COMMAND,ID_APP_EXIT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::AddConnectionRecord(const CConnectionRecord& c)
{
	m_aConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	AfxGetMainWnd()->PostMessage(WM_USER,WPARAM_CONNECTION_RECORD);
}
/////////////////////////////////////////////////////////////////////////////
CString CDVStorageApp::GetSupervisorPIN() const
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#ifdef _UNICODE
	CString s = wkp.GetString(_T("Process"),_T("PIN"),_T(""), TRUE);
#else
	CString s = wkp.GetString(_T("Process"),_T("PIN"),_T(""));
#endif
	if (!s.IsEmpty())
	{
		 CWK_Profile::Decode(s);
		 return s;
	}
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
CString CDVStorageApp::GetOperatorPIN() const
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#ifdef _UNICODE
	CString s = wkp.GetString(_T("Process"),_T("OPIN"),_T(""), TRUE);
#else
	CString s = wkp.GetString(_T("Process"),_T("OPIN"),_T(""));
#endif
	if (!s.IsEmpty())
	{
		 CWK_Profile::Decode(s);
		 return s;
	}
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnConnectionRecord()
{
	if (m_aConnectionRecords.GetCount())
	{
		CConnectionRecord* pCR = m_aConnectionRecords.SafeGetAndRemoveHead();
		CString sPIN;
		CString sSerial;
		BOOL bConnect = TRUE;
		
		if (m_Clients.GetNrOfLicensed() >= 5)
		{
			WK_TRACE_ERROR(_T("max number of clients reached\n"));
			m_pCIPCServerControlStorage->DoIndicateError(0, 
				SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 4);
			bConnect = FALSE;
		}
		else
		{
			if (pCR->GetFieldValue(CRF_SERIAL,sSerial))
			{
				if (CRSA::IsValidSerial(sSerial))
				{
					if (m_Clients.IsConnected(sSerial,pCR->GetSourceHost()))
					{
						WK_TRACE_ERROR(_T("Serial Nr %s already connected\n"),sSerial);
						m_pCIPCServerControlStorage->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 0);
						bConnect = FALSE;
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("invalid Serial Nr %s\n"),sSerial);
					m_pCIPCServerControlStorage->DoIndicateError(0, 
						SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 1);
					bConnect = FALSE;
				}
			}
		}
		
		if (bConnect)
		{
			
			if (!pCR->GetFieldValue(CSD_PIN,sPIN))
			{
				sPIN = pCR->GetPassword();
			}
			
			if (   IsReadOnly()
				|| (sPIN == GetSupervisorPIN())
				|| (sPIN == GetOperatorPIN())	// ML Operator PIN muß ebenfalls überprüft wird.
				)
			{
				WK_TRACE(_T("PIN correct\n"));
				
				CClient* pClient = m_Clients.AddClient();
				
				if (pClient->Connect(*pCR))
				{
					CString shmName = pClient->GetCIPC()->GetShmName();
					m_pCIPCServerControlStorage->DoConfirmDataBaseConnection(pClient->GetID(), 
						shmName,_T(""), SCO_IS_DV
#ifdef _UNICODE
						, MAKELONG(CODEPAGE_UNICODE, 0)
#endif
						);
				}
				else
				{
					m_pCIPCServerControlStorage->DoIndicateError(0, 
						SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
				}
			}
			else
			{
				WK_TRACE(_T("PIN invalid\n"));
				m_pCIPCServerControlStorage->DoIndicateError(0, 
					SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
			}
		}
		
		WK_DELETE(pCR);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnSearch()
{
	CSearch* pSearch = m_Searchs.SafeGetAndRemoveHead();

	CSearchThread* pSearchThread;

	pSearchThread = new CSearchThread(pSearch);
	m_SearchThreads.SafeAdd(pSearchThread);
	pSearchThread->StartThread();
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::CancelSearchs()
{
	m_SearchThreads.Lock();
	for(int i=0;i<m_SearchThreads.GetSize();i++)
	{
		m_SearchThreads.GetAt(i)->Cancel();
	}
	m_SearchThreads.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnFileReset() 
{
	StartFullReset(TRUE);
	m_bPosted = FALSE;
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnDeviceArrived(DEV_BROADCAST_HDR* pHdr)
{
	if (!IsInBackup())
	{
		if (pHdr)
		{
			if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
				COsVersionInfo os;
				if (   os.dwPlatformId == VER_PLATFORM_WIN32_NT
					&& os.dwMajorVersion == 4
					&& os.dwMinorVersion == 0)
				{
					WK_TRACE(_T("OnDeviceArrived %08lx\n"),pHdr->dbch_reserved);
					CIPCDrive* pDrive;
					for (int i=0;i<m_Drives.GetSize();i++)
					{
						pDrive = m_Drives.GetAtFast(i);
						if (   pDrive->IsCDROM() 
							&& (   pDrive->IsReadBackup()
								|| pDrive->IsWriteBackup())
								)
						{
							pDrive->CheckSpace();
							WK_TRACE(_T("drive arrived %s %d MB all\n"),
								pDrive->GetRootString(),pDrive->GetMB());
							if (pDrive->GetMB()>0)
							{
								OnBackupDriveArrived(pDrive);
							}
							break;
						}
					}
				}
				else
				{
					DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
					CString sRoot = _T("a:\\");
					int   nDrive;
					DWORD dwUnitMask = pDBV->dbcv_unitmask;
					for (nDrive=-1; nDrive<32 && dwUnitMask!=0; nDrive++) 
					{
						dwUnitMask >>= 1;
					}
					sRoot.SetAt(0,(char)(_T('a') + nDrive));
					WK_TRACE(_T("OnDeviceArrived %s %d\n"),sRoot,pDBV->dbcv_flags);

					CIPCDrive* pDrive = NULL;
					BOOL bFound = FALSE;
					for (int i=0;i<m_Drives.GetSize() && !bFound;i++)
					{
						pDrive = m_Drives.GetAtFast(i);
						bFound = (0==sRoot.CompareNoCase(pDrive->GetRootString()));
					}
					if (   pDBV->dbcv_flags == 0
						&& !bFound)
					{
						// complete new drive
						pDrive = new CIPCDrive(sRoot,DVR_READ_BACKUP_DRIVE);
						bFound = TRUE;
						m_Drives.Add(pDrive);
					}

					if (   pDrive
						&& bFound)
					{
						pDrive->CheckSpace();
						WK_TRACE(_T("drive arrived %s %d MB all\n"),pDrive->GetRootString(),pDrive->GetMB());
						if (   pDrive->GetMB()>0
							&& !IsResetting())
						{
							OnBackupDriveArrived(pDrive);
						}
						else
						{
							TRACE(_T("adding %s to arrived drives\n"),sRoot);
							BOOL bFound = FALSE;
							for (int j=0;j<m_sArrivedDrives.GetSize()&&!bFound;j++)
							{
								bFound = (0==sRoot.CompareNoCase(m_sArrivedDrives.GetAt(j)));
							}
							m_sArrivedDrives.Add(sRoot);
						}
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnBackupDriveArrived(CIPCDrive* pDrive)
{
	CString sFile;
	sFile = pDrive->GetRootString()+_T("dvb0001");
	if (DoesFileExist(sFile))
	{
		WK_TRACE(_T("new backup drive %s\n"),pDrive->GetRootString());
		
		if (   !pDrive->IsReadBackup()
			|| !pDrive->IsWriteBackup())
		{
			pDrive->SetUsedAs(DVR_READ_BACKUP_DRIVE);
		}
		if (m_pBackupScanThread == NULL)
		{
			m_pBackupScanThread = new CBackupScanThread(pDrive,TRUE);
			m_pBackupScanThread->StartThread();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnBackupDriveRemoved(CIPCDrive* pDrive)
{
	m_Archives.DeleteDatabaseDrive(pDrive->GetRootString());
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		if (m_pMainWnd->IsWindowVisible())
		{
			m_pMainWnd->RedrawWindow();
		}
	}
}
CString GetDriveFromDEV_BROADCAST_VOLUME(DEV_BROADCAST_VOLUME* pDBV)
{
	CString sRoot = _T("a:\\");
	int   nDrive;
	DWORD dwUnitMask = pDBV->dbcv_unitmask;
	for (nDrive=-1; nDrive<32 && dwUnitMask!=0; nDrive++) 
	{
		dwUnitMask >>= 1;
	}
	sRoot.SetAt(0,(char)(_T('a') + nDrive));
	return sRoot;
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnDeviceRemoved(DEV_BROADCAST_HDR* pHdr)
{
	if (pHdr == NULL)
	{
		return;
	}
	if (pHdr->dbch_devicetype != DBT_DEVTYP_VOLUME)
	{
		return;
	}


	COsVersionInfo os;
	if (IsInBackup())
	{
		WK_TRACE(_T("CDVStorageApp::OnDeviceRemoved In Backup\n"));
		// für alle Backups überprüfen, ob das Laufwerk gerade wech ist.
		if (os.IsWinXP())
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			if (pDBV->dbcv_flags == 0)
			{
				CString sRoot = GetDriveFromDEV_BROADCAST_VOLUME(pDBV);
				for (int i=0;i<m_BackupThreads.GetSize();i++)
				{
					CBackupThread* pBackupThread = m_BackupThreads.GetAt(i);
					CBackup* pBackup = pBackupThread->GetBackup();
					CString sDestination = pBackup->GetDestinationPath();
					sDestination = sDestination.Left(sRoot.GetLength());
					
					if (0==sDestination.CompareNoCase(sRoot))
					{
						// Backup Laufwerk ist weg !
						WK_TRACE(_T("cancelling backup by drive loss %s\n"),sDestination);
						pBackupThread->Cancel(FALSE);
					}
				}
			}
		}
	}
	else
	{
		if (   os.dwPlatformId == VER_PLATFORM_WIN32_NT
			&& os.dwMajorVersion == 4
			&& os.dwMinorVersion == 0)
		{
			WK_TRACE(_T("OnDeviceRemoved NT %08lx\n"),pHdr->dbch_reserved);
			CIPCDrive* pDrive;
			for (int i=0;i<m_Drives.GetSize();i++)
			{
				pDrive = m_Drives.GetAtFast(i);
				if (   pDrive->IsCDROM() 
					&& (   pDrive->IsReadBackup()
						|| pDrive->IsWriteBackup())
						)
				{
					pDrive->CheckSpace();
					WK_TRACE(_T("drive removed %s %d MB all\n"),pDrive->GetRootString(),pDrive->GetMB());
					if (pDrive->GetMB() == 0)
					{
						OnBackupDriveRemoved(pDrive);
					}
					break;
				}
			}
		}
		else
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			CString sRoot = GetDriveFromDEV_BROADCAST_VOLUME(pDBV);
			WK_TRACE(_T("OnDeviceRemoved %s %x\n"),sRoot, pDBV->dbcv_flags);
			CIPCDrive* pDrive = NULL;
			for (int i=0;i<m_Drives.GetSize();i++)
			{
				pDrive = m_Drives.GetAtFast(i);
				if (0==sRoot.CompareNoCase(pDrive->GetRootString()))
				{
					BOOL bReset = FALSE;
					if (pDrive->GetType() == DRIVE_FIXED)
					{
						CString sError, sTxt;
						sError.Format(IDS_HDD_FAILURE, pDrive->GetLetter(), pDrive->GetVolumeString());
						if (pDrive->IsSystem())
						{
							sTxt.LoadString(IDS_SYSTEM_DRIVE);
							sError += sTxt + _T(" ");
						}
						else
						{
							sTxt.LoadString(IDS_STORAGE_DRIVE);
							sError += sTxt + _T(" ");
						}
						WK_RunTimeErrorLevel rtel = REL_ERROR;
						CWK_Profile wkp;
						CString sLogPath = wkp.GetString(SECTION_LOG, LOG_LOGPATH, NULL);
						sLogPath.MakeLower();
						if (  !sLogPath.IsEmpty() 
							&& sLogPath.GetAt(0) == sRoot.GetAt(0)) // RKE: hier ist auch das Pagefile
						{
							rtel = REL_REBOOT_ERROR;
						}
						else
						{
							bReset = TRUE;
						}
						CWK_RunTimeError e(WAI_DATABASE_SERVER, rtel, RTE_HARDDISK_FALURE, sError,0,1);
						e.Send();
					}
					else
					{
						pDrive->CheckSpace();
						WK_TRACE(_T("drive removed %s %d MB all\n"),pDrive->GetRootString(),pDrive->GetMB());
						if (pDrive->GetMB() == 0)
						{
							OnBackupDriveRemoved(pDrive);
						}
					}
					if (pDBV->dbcv_flags == 0)
					{
						WK_TRACE(_T("removing drive %s\n"),pDrive->GetRootString());
						m_Drives.RemoveAt(i);
						WK_DELETE(pDrive);
					}
					if (bReset)
					{
						OnFileReset();
					}
					break;
				}
			}
			for (int j=0;j<m_sArrivedDrives.GetSize();j++)
			{
				if (0==sRoot.CompareNoCase(m_sArrivedDrives.GetAt(j)))
				{
					m_sArrivedDrives.RemoveAt(j);
					break;
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::SetCollectionName(DWORD dwClientID, CSecID id, const CString& sName)
{
	m_Archives.Lock();
	CCollection* pCollection = m_Archives.GetCollection(id.GetSubID());
	if (pCollection)
	{
		pCollection->SetName(sName);
		// save the new name 
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		CArchivInfoArray aia;
		aia.Load(wkp);
		CArchivInfo* pAI = aia.GetArchivInfo(id);
		if (pAI)
		{
			pAI->SetName(sName);
			aia.Save(wkp);
		}
        m_Clients.DoIndicateNewCollectionName(dwClientID,*pCollection);
	}
	else
	{
		WK_TRACE_ERROR(_T("no archive for set name\n"));
	}
	m_Archives.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnUpdatePaneClients(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("DBC %d"),m_Clients.GetSize());
	pCmdUI->SetText(s);
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnUpdatePaneProcess(CCmdUI* pCmdUI)
{
	CString s;
    if (   m_pCIPCDatabaseStorage
		&& m_pCIPCDatabaseStorage->GetIPCState()==CIPC_STATE_CONNECTED)
	{
		s.LoadString(IDS_PANE_PROCESS2);
	}
	else
	{
		s.LoadString(IDS_PANE_PROCESS);
	}
	pCmdUI->SetText(s);
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnUpdatePaneFPS(CCmdUI* pCmdUI)
{
	if (   m_pCIPCDatabaseStorage
		&& m_pCIPCDatabaseStorage->GetIPCState()==CIPC_STATE_CONNECTED
		&& m_SaveThread.IsRunning())
	{
		CString s;
		s.Format(_T("%04.02f fps"),m_SaveThread.GetStatistics().GetFPS());
		pCmdUI->SetText(s);
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnFileFastReset() 
{
	if (   (m_Archives.GetSize() > 0)
		&& (m_Drives.GetSize() > 0)
		)
	{
		FastReset();
	}
	else
	{
		StartFullReset(TRUE);
	}
	m_bPosted = FALSE;
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnBackup()
{
	CBackup* pBackup = m_Backups.SafeGetAndRemoveHead();

	CBackupThread* pBackupThread = new CBackupThread(pBackup);
	m_BackupThreads.SafeAdd(pBackupThread);
	pBackupThread->StartThread();
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::CancelBackup(DWORD dwUserData)
{
	for (int i=0;i<m_BackupThreads.GetSize();i++)
	{
		CBackup* pBackup = m_BackupThreads.GetAt(i)->GetBackup();
		if (pBackup->GetUserData()==dwUserData)
		{
			m_BackupThreads.GetAt(i)->Cancel(TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVStorageApp::ReadOldSequenceMap()
{
	CString sPath;
	CMapDWordToSequenceHashEntry map;

#ifdef _DEBUG
	sPath = _T("c:\\dv");
#else
	GetModuleFileName(m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();
	sPath = sPath.Left(sPath.ReverseFind(_T('\\')));
#endif
	CString sFilepath = sPath+_T("\\sequence.dat");

	TRY
	{
		CFile fileMap;
		if (fileMap.Open(sFilepath,CFile::modeRead))
		{
			if (fileMap.GetLength()>0)
			{
				CArchive ar(&fileMap,CArchive::load);
				map.Serialize(ar);
				int c = map.GetCount();
				for (int i=0;i<c;i++)
				{
					CSequenceHashEntry* pOldEntry = new CSequenceHashEntry;
					pOldEntry->Serialize(ar);

					CSystemTime stFirst,stLast;
					stFirst.FromTime(pOldEntry->GetFirst());
					stLast.FromTime(pOldEntry->GetLast());
					m_SequenceMap.SetSequenceHashEntry(pOldEntry->GetArchiveNr(),
													   pOldEntry->GetSequenceNr(),
													   pOldEntry->GetRecords(),
													   stFirst,stLast,_T(""));
					delete pOldEntry;
				}
				WK_TRACE(_T("read %d sequence map entries\n"),c);
			}
		}
	}
	CATCH(CFileException, cfe)
	{
		WK_TRACE_ERROR(_T("cannot read sequence map %s,%s\n"),sFilepath,
			GetLastErrorString(cfe->m_lOsError));
	}
	END_CATCH

//	TRACE(_T("DeleteFile(sequence.dat)\n"));
#ifndef _DEBUG
	DeleteFile(_T("sequence.dat"));
#endif

	return map.GetCount() >0;
}
/////////////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnDateCheck()
{
	// check for a new day
	CSystemTime t;
	t.GetLocalTime();
//	TRACE(_T("checking day %d, new %d\n"),t.GetDay(),m_currentTime.GetDay());
	if (t.GetDay() != m_currentTime.GetDay())
	{
		// it's a new day
		OnNewDay();
	}
	m_currentTime = t;
	m_Archives.CheckBackupDirectories();
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnNewDay() 
{
	WK_TRACE(_T("new day detected, start with new sequences forced\n"));
	m_Archives.Lock();
	for (int i=0;i<m_Archives.GetSize();i++)
	{
		CCollection* pCollection = m_Archives.GetAt(i);
		if (   pCollection
			&& pCollection->IsAlarmList())
		{
			pCollection->SetFirstSequenceAfterRestart();
		}
	}
	m_Archives.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnRequestBackup()
{
	if (m_Backups.GetCount())
	{
		OnBackup();
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnBackupFinished()
{
	// check for finished backup threads
	if (m_BackupThreads.GetSize())
	{
		int i;
		CBackupThread* pBackupThread;
		for (i=0;i<m_BackupThreads.GetSize();i++)
		{
			pBackupThread = m_BackupThreads.GetAt(i);
			if (pBackupThread)
			{
				if (!pBackupThread->IsRunning())
				{
					m_BackupThreads.RemoveAt(i);
					WK_DELETE(pBackupThread);
					break; // next CBackupThread will be removed from next OnIdle call;
				}
				else if (pBackupThread->IsTimedOut())
				{
					pBackupThread->HandleTimeOut();
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnRequestQuery()
{
	if (m_Searchs.GetCount())
	{
		OnSearch();
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnQueryFinished()
{
	// check for finished Search threads
	if (m_SearchThreads.GetSize())
	{
		int i;
		CSearchThread* pSearchThread;
		for (i=0;i<m_SearchThreads.GetSize();i++)
		{
			pSearchThread = m_SearchThreads.GetAt(i);
			if (pSearchThread && !pSearchThread->IsRunning())
			{
				WK_TRACE(_T("removing old search thread\n"));
				m_SearchThreads.RemoveAt(i);
				WK_DELETE(pSearchThread);
				break; // next CSearchThread will be removed from next OnIdle call;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnBackupScanFinished()
{
	WK_DELETE(m_pBackupScanThread);
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::OnVerifyFinished()
{
}
//////////////////////////////////////////////////////////////////////
void CDVStorageApp::PrepareToShutDown_NoArchives()
{
	m_bPrepareToShutDown_NoArchives = TRUE;
}
//////////////////////////////////////////////////////////////////////
