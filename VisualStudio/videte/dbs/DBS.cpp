/* GlobalReplace: CDVRDrive --> CIPCDrive */
/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
// DBS.cpp : Defines the class behaviors for the application.
//

// TODO UF 1. Lesen der S Werte für tracking BETA
// TODO UF 2. Dynamische Archive BETA
// TODO UF 3. Schnellreset
// TODO UF 4. 
// TODO UF 5.

#include "stdafx.h"
#include "DBS.h"

#include "MainFrm.h"
#include "DBSDoc.h"
#include "DBSView.h"

#include "CIPCServerControlDBS.h"
#include "ConversionDialog.h"
#include "InitializeThread.h"
#include "RHDDialog.h"
#include ".\dbs.h"


#define BASE_DVRT_KEY            _T("software\\DVRT\\")
#define REGKEY_BASEDVRT_DEFINED  2

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR szSection[] = SECTION_DBS;
static TCHAR BASED_CODE szDontUse[]	= _T("dontuse");

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
/////////////////////////////////////////////////////////////////////////////
// The one and only CDBSApp object
CDBSApp theApp;
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDBSApp, CWinApp)
	//{{AFX_MSG_MAP(CDBSApp)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	ON_COMMAND_RANGE(ID_FILE_ACTUALIZE, ID_FILE_ACTUALIZE, OnFileActualize)
	ON_COMMAND_RANGE(ID_ACTUALIZE_TIMED, ID_ACTUALIZE_TIMED, OnFileActualize)
	ON_COMMAND(ID_FILE_DELETEALL, OnFileDeleteall)
	ON_COMMAND(ID_TRACE_QUEUES, OnTraceQueues)
	ON_UPDATE_COMMAND_UI(ID_TRACE_QUEUES, OnUpdateTraceQueues)
	ON_COMMAND(ID_TRACE_REQUESTS, OnTraceRequests)
	ON_COMMAND(ID_TRACE_SEQUENCES, OnTraceSequences)
	ON_UPDATE_COMMAND_UI(ID_TRACE_SEQUENCES, OnUpdateTraceSequences)
	ON_UPDATE_COMMAND_UI(ID_TRACE_REQUESTS, OnUpdateTraceRequests)
	ON_COMMAND(ID_TRACE_DRIVES, OnTraceDrives)
	ON_UPDATE_COMMAND_UI(ID_TRACE_DRIVES, OnUpdateTraceDrives)
	ON_COMMAND(ID_VIEW_ARCHIVES, OnViewArchives)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ARCHIVES, OnUpdateViewArchives)
	ON_COMMAND(ID_VIEW_CLIENTS, OnViewClients)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLIENTS, OnUpdateViewClients)
	ON_COMMAND(ID_VIEW_DRIVES, OnViewDrives)
	ON_COMMAND(ID_VIEW_THREADS, OnViewThreads)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THREADS, OnUpdateViewThreads)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRIVES, OnUpdateViewDrives)
	ON_COMMAND(ID_TRACE_CODEBASE, OnTraceCodebase)
	ON_UPDATE_COMMAND_UI(ID_TRACE_CODEBASE, OnUpdateTraceCodebase)
	ON_COMMAND(ID_TRACE_QUERY, OnTraceQuery)
	ON_UPDATE_COMMAND_UI(ID_TRACE_QUERY, OnUpdateTraceQuery)
	ON_COMMAND(ID_TRACE_OPEN_CLOSE, OnTraceOpenClose)
	ON_UPDATE_COMMAND_UI(ID_TRACE_OPEN_CLOSE, OnUpdateTraceOpenClose)
	ON_COMMAND(ID_TRACE_DELETE, OnTraceDelete)
	ON_UPDATE_COMMAND_UI(ID_TRACE_DELETE, OnUpdateTraceDelete)
	ON_COMMAND(ID_TRACE_BACKUP, OnTraceBackup)
	ON_UPDATE_COMMAND_UI(ID_TRACE_BACKUP, OnUpdateTraceBackup)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_FAST_RESET, OnFileFastReset)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CDBSApp::CDBSApp()
{
	InitVideteLocaleLanguage();

	m_pInitializeThread = NULL;
	m_pCIPCServerControlDBS = NULL;
	m_pCIPCDatabaseServerToServer = NULL;
	
	m_bFullResetting = FALSE;
	m_bFastResetting = FALSE;
	m_bExiting = FALSE;

	m_bTraceDrives = FALSE;
	m_bTraceQueue = FALSE;
	m_bTraceRequests = FALSE;
	m_bTraceSequence = FALSE;
	m_bTraceCodebase = FALSE;
	m_bTraceQuery = FALSE;
	m_bTraceOpenClose = FALSE;
	m_bTraceBackup = FALSE;
	m_bTraceDelete = FALSE;
	m_bTraceQueryResults = FALSE;
	m_bTracePreMove = FALSE;

	m_bViewDrives = FALSE;
	m_bViewClients = FALSE;
	m_bViewArchivs = FALSE;
	m_bViewThreads = FALSE;
	m_bOffsetCorrected = FALSE;

	m_bReadOnlyModus = FALSE;
	m_bPosted = FALSE;

	m_InternalProductCode = IPC_DTS;
	m_pDebugger = NULL;



	CWK_Profile wkp;
	m_bThreadPoolEnabled = GetIntValue(wkp, _T("EnableThreadPool"), 0);
	EnableThreadPool(m_bThreadPoolEnabled);
	m_bTraceLocks = GetIntValue(wkp, _T("TraceLocks"), FALSE);

#ifdef TRACE_LOCKS
	int nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;

	m_aConnectionRecords.m_nCurrentThread = nCurrentThread;
	m_Backups.m_nCurrentThread = nCurrentThread;
	m_BackupThreads.m_nCurrentThread = nCurrentThread;
	m_Conversions.m_nCurrentThread = nCurrentThread;
	m_Queries.m_nCurrentThread = nCurrentThread;
	m_QueryThreads.m_nCurrentThread = nCurrentThread;

	m_Fields.m_nCurrentThread = nCurrentThread;

	m_Clients.m_nCurrentThread = nCurrentThread;
	m_Archives.m_nCurrentThread = nCurrentThread;
	m_Drives.m_nCurrentThread = nCurrentThread;			
#endif

	m_pCheckDrivesThread = NULL;

	m_bAutoReduceArchiveSize = GetIntValue(wkp, DBS_AUTO_REDUCE_ARCHIVES, 1);
	m_bAllowNetworkStorage = GetIntValue(wkp, DBS_ALLOW_NETWORK_STORAGE, 0);


	m_sDatabaseDir = wkp.GetString(SECTION_COMMON, COMMON_DATA_BASE_DIR, _T("c:\\database"));
}
/////////////////////////////////////////////////////////////////////////////
CDBSApp::~CDBSApp()
{
	ASSERT(1);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::RegisterWindowClass()
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
void CDBSApp::InitDTS()
{
	CArchiv::m_sRootName = _T("dvs");
	CArchiv::m_sBackupName = _T("dvb");
	CArchiv::m_sBackupInfoFilename = _T("autorun.inf");
	CArchiv::m_sDbd	= _T("dat");

	CArchiv::m_sTYP = DVR_TYP;
	CArchiv::m_sDATE = DVR_DATE;
	CArchiv::m_sTIME = DVR_TIME;
	CArchiv::m_sMS = DVR_MS;

	CArchiv::m_sCANR = DVR_CAMERANR;
	CArchiv::m_sRES  = DVR_RESOLUTION;
	CArchiv::m_sCOMP = DVR_COMPRESSION;
	CArchiv::m_sCOTY = DVR_COMPRESSIONTYP;

	CArchiv::m_sDPOS = DVR_DATA_POS;
	CArchiv::m_sDLEN = DVR_DATA_LEN;
	CArchiv::m_sCSUM = DVR_CHECK_SUM;

	CArchiv::m_sMD_X  = DVD_MD_X;
	CArchiv::m_sMD_Y  = DVD_MD_Y;
	CArchiv::m_sMD_X2 = DVD_MD_X2;
	CArchiv::m_sMD_Y2 = DVD_MD_Y2;
	CArchiv::m_sMD_X3 = DVD_MD_X3;
	CArchiv::m_sMD_Y3 = DVD_MD_Y3;
	CArchiv::m_sMD_X4 = DVD_MD_X4;
	CArchiv::m_sMD_Y4 = DVD_MD_Y4;
	CArchiv::m_sMD_X5 = DVD_MD_X5;
	CArchiv::m_sMD_Y5 = DVD_MD_Y5;
	CArchiv::m_sMD_S = DVD_MD_S;

}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InitIDIP()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_DB_SERVER)==FALSE)
	{
		TRACE(_T("second instance\n"));
		return FALSE;
	}

	InitReadOnlyModus();

	if (IsReadOnly())
	{
		InitDebugger(_T("dbsro.log"),WAI_DATABASE_SERVER);
	}
	else
	{
		CString sTitle;
		sTitle = COemGuiApi::GetApplicationName(WAI_DATABASE_SERVER);
		m_pszAppName = _tcsdup(sTitle);    // human readable title
		
		CWK_Dongle dongle;
		if (dongle.IsValid())
		{
			m_InternalProductCode = dongle.GetProductCode();
		}

		InitDebugger(IsIDIP()?_T("dbs.log"):_T("dvs.log"),WAI_DATABASE_SERVER);
	}
	if (IsDTS())
	{
		InitDTS();
	}
	else
	{
		InitIDIP();
	}

	RegisterWindowClass();
	
	WK_TRACE(_T("START DBS %s\n"),COemGuiApi::GetProductCodeName());
	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDBSDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDBSView));
	AddDocTemplate(pDocTemplate);

	// do not touch
#ifndef _DEBUG
	if (m_nCmdShow!=0) // do not touch if already hidden
	{	
		m_nCmdShow = SW_HIDE;
	}
#endif

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	if (!IsReadOnly())
	{
		CWK_Profile wkp;
		m_bTraceQueue = GetIntValue(wkp, _T("TraceQueue"),FALSE);
		m_bTraceDrives = GetIntValue(wkp, _T("TraceDrives"),FALSE);
		m_bTraceSequence = GetIntValue(wkp, _T("TraceSequence"),FALSE);
		m_bTraceRequests = GetIntValue(wkp, _T("TraceRequests"),FALSE);
		m_bTraceCodebase = GetIntValue(wkp, _T("TraceCodebase"),FALSE);
		m_bTraceOpenClose = GetIntValue(wkp, _T("TraceOpenClose"),FALSE);
		m_bTraceBackup = GetIntValue(wkp, _T("TraceBackup"),FALSE);
		m_bTraceDelete = GetIntValue(wkp, _T("TraceDelete"),FALSE);
		m_bTraceQueryResults = GetIntValue(wkp, _T("TraceQueryResults"),FALSE);
		m_bTracePreMove = GetIntValue(wkp, _T("TracePreMove"),FALSE);
		m_bTraceQuery = GetIntValue(wkp, _T("TraceQuery"),FALSE);
		m_bViewDrives = GetIntValue(wkp, _T("ViewDrives"),FALSE);
		m_bViewClients = GetIntValue(wkp, _T("ViewClients"),FALSE);
		m_bViewArchivs = GetIntValue(wkp, _T("ViewArchivs"),FALSE);
		m_bViewThreads = GetIntValue(wkp, _T("ViewThreads"),FALSE);
		m_bOffsetCorrected = GetIntValue(wkp, _T("OffsetCorrected"),m_bOffsetCorrected);
		BOOL bDebugger = GetIntValue(wkp, _T("EnableDebugger"),FALSE);
		if (bDebugger)
		{
			m_pDebugger = new CWK_Debugger();
			WK_TRACE(_T("WK_DEBUGGER enabled\n"));
		}
	}

	// The one and only window has been initialized, so show and update it.
//	m_pMainWnd->ShowWindow(m_nCmdShow);
//	m_pMainWnd->UpdateWindow();

	StartFullReset();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InitFieldsRO()
{
	CString sOemIni;
	CString s;
	CIPCField* pField = NULL;
	m_Fields.SafeDeleteAll();

	CAutoRun ar;
	if (ar.IsValid())
	{
		sOemIni = ar.GetPath();
	}

	const DWORD dwSizeKeys = 1000;
	LPTSTR szKeysBuffer = new TCHAR[dwSizeKeys];
	DWORD dw = GetPrivateProfileString(_T("Fields"), NULL, NULL, szKeysBuffer, dwSizeKeys, sOemIni);
	if (dw == dwSizeKeys-1)
	{
		WK_TRACE_WARNING(_T("InitFields Buffer for keys maybe too small\n"));
	}
	LPTSTR pszKeys = szKeysBuffer;
	const DWORD dwSizeKey = 100;
	CString sKey, sValue;
	sKey = pszKeys;
	while (sKey.GetLength() > 0)
	{
		dw = GetPrivateProfileString(_T("Fields"), sKey, szDontUse, sValue.GetBuffer(dwSizeKey), dwSizeKey, sOemIni);
		sValue.ReleaseBuffer();
		if (dw == dwSizeKey-1)
		{
			WK_TRACE_WARNING(_T("InitFields Buffer for key maybe too small\n"));
		}
		pField = new CIPCField(sKey, sValue, 'C');
		m_Fields.Add(pField);
		pszKeys += sKey.GetLength() + 1;
		sKey = pszKeys;
	}
	WK_DELETE(szKeysBuffer);

	s.LoadString(IDS_DBD_STNM);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,s,64,'C'));
	s.LoadString(IDS_DBD_CANM);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfCaNm,s,64,'C'));
	s.LoadString(IDS_DBD_INNM);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfInNm,s,64,'C'));
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InitFields(CWK_Profile& wkp)
{
	m_Fields.SafeDeleteAll();

	CWK_Dongle dongle;
	CString s;

	if (   dongle.RunGAUnit()
		|| dongle.RunSDIUnit()
		|| dongle.IsReceiver())
	{
		WK_TRACE(_T("loading database fields data\n"));
		CString sWatch;	
		sWatch.LoadString(IDS_INIT_DATABASE_FIELDS);
		CWK_StopWatch watch;
		watch.StartWatch();

		// CAVEAT: FIRST load fields from registry...
		m_Fields.Load(SECTION_DATABASE_FIELDS, wkp);
		// check for old style before 4.6.2
		if (m_Fields.GetSize() == 0)
		{
			// 4.0
			s.LoadString(IDS_DBD_TIME);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfTime,s,6,'T'));
			s.LoadString(IDS_DBD_DATE);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfDate,s,8,'D'));
			s.LoadString(IDS_DBD_GANR);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfGaNr,s,6,'C'));
			s.LoadString(IDS_DBD_TANR);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfTaNr,s,4,'C'));
			s.LoadString(IDS_DBD_KTNR);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfKtNr,s,16,'C'));
			s.LoadString(IDS_DBD_BCNR);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfBcNr,s,16,'C'));
			s.LoadString(IDS_DBD_AMNT);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfAmnt,s,8,'C'));
			s.LoadString(IDS_DBD_CURR);
			m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfCurr,s,3,'C'));
		}

		watch.StopWatch(sWatch);
	}
	// CAVEAT: ... append fields AFTER load from registry
	s.LoadString(IDS_DBD_STNM);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,s,64,'C'));
	s.LoadString(IDS_DBD_CANM);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfCaNm,s,64,'C'));
	s.LoadString(IDS_DBD_INNM);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfInNm,s,64,'C'));

	CIPCField* pField = NULL;
	for (int i=0 ; i<m_Fields.GetSize() ; i++)
	{
		pField = m_Fields.GetAtFast(i);
		WK_TRACE(_T("%-8s %-20s %03u %c\n"),
					pField->GetName(), pField->GetValue(), pField->GetMaxLength(), pField->GetType());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::Clear()
{
	m_VerifyThread.StopThread();
	m_MoveThread.StopThread();
	m_SaveThread.StopThread();

	CancelQueries();

	if (LVClear())
	{
		CString s;
		s.LoadString(IDS_INIT);
		LVAddString(s);
	}

	Sleep(10);
	ExitThreads();
	Sleep(10);

	m_Archives.Finalize(TRUE);
	m_Archives.DeleteAll();
	
	Sleep(10);
	m_Drives.DeleteAll();
	m_Hosts.DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::Initialize()
{
	CWK_Profile* pWKP = NULL;
	CString sSection;
	if (IsDTS())
	{
		pWKP = new 	CWK_Profile(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		sSection = _T("DVStorage");
	}
	else
	{
		pWKP = new CWK_Profile();
		sSection = _T("dbs");
	}

	CString sPath;
	GetModuleFileName(m_hInstance, sPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	sPath.ReleaseBuffer();
	sPath = sPath.Left(sPath.ReverseFind(_T('\\')));
#ifdef _DEBUG
	if (IsDTS())
	{
		sPath = _T("c:\\dv");
	}
	else
	{
		sPath = _T("c:\\security");
	}
#endif
	CString sMapPath = pWKP->GetString(sSection, DBS_MAP_PATH, NULL);
	if (sMapPath.IsEmpty())
	{
		m_sMapDirectory = sPath;
	}
	else
	{
		m_sMapDirectory = sMapPath;
	}
	
	CString s;
	int i, j;
	s.LoadString(IDS_INIT_DRIVES);

	// sequence map and hosts 
	ReloadSequenceMap(TRUE);
	m_Hosts.Load(*pWKP);

	// drives
	CWK_StopWatch w;
	m_Drives.Init(*pWKP);
	m_Drives.Trace();
	w.StopWatch(s);
	const CString& sFailedDrives = m_Drives.GetFailedDrives();
	if (!sFailedDrives.IsEmpty())
	{
		m_sDisconnectedDrives += sFailedDrives;
		for (i=0; i<sFailedDrives.GetLength(); i++)
		{
			CString sError, sTxt;
			DWORD dwParam1 = sFailedDrives.GetAt(i);
			DWORD dwParam2 = 0x01|0x02; // Alarm an, MessageBox
			sTxt.LoadString(AFX_IDS_UNTITLED);
			sError.Format(IDS_HDD_FAILURE, sFailedDrives.GetAt(i), sTxt);
			sTxt.LoadString(IDS_STORAGE_DRIVE);
			sError += sTxt;
			CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_HARDDISK_FALURE, sError, dwParam1, dwParam2);
			e.Send();
		}
	}

	for (i=0;i<m_Drives.GetSize();i++)
	{
		CIPCDrive* pDrive = m_Drives.GetAtFast(i);
		if (m_sDisconnectedDrives.Find(pDrive->GetLetter()) != -1)
		{
			continue;
		}

		if (   pDrive->IsRemoveableDatabase()
			&& CDrives::IsRO(pDrive->GetRootString()))
		{
			{
				CDrives::DoROToRW(pDrive);
			}
		}
		//check for backup network drives, if not connected, try to connect
		if (   pDrive->IsNetworkDrive() 
			&& (   pDrive->IsReadBackup() 
			    || pDrive->IsWriteBackup() 
				|| m_bAllowNetworkStorage && pDrive->IsDatabase() )
			&& !pDrive->IsConnected())
		{
			BOOL bConnect = pDrive->ConnectNetworkDrive();
			if(!bConnect)
			{
				//send RTE because network backup drive is not connected
				CString sError = GetLastErrorString();
				CString sFmt, sVolume = pDrive->GetVolumeString();
				int nFind = sVolume.Find(_T(";"));
				if (nFind != -1) sVolume = sVolume.Left(nFind);
				sFmt.FormatMessage(AFX_IDP_FILE_NOT_FOUND, pDrive->GetRootString()
					               + _T(" -> ") 
								   + sVolume);
				sError = sFmt + _T("\n") + sError;
				CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_STORAGE, sError,0,0);
				e.Send();
				m_sDisconnectedDrives += pDrive->GetLetter();
			}
		}
	}

	if (m_sDisconnectedDrives.GetLength())
	{
		TRACE(_T("Disconnected drives: %s\n"), m_sDisconnectedDrives);
	}

	// variable fields
	InitFields(*pWKP);

	// archives scan
	s.LoadString(IDS_INIT_ARCHIVS);
	w.StartWatch();
	m_Archives.Init(*pWKP);

	CString sFixed, sRoot;
	CArchiv *pA;
	for (i=0; i<m_sDisconnectedDrives.GetLength(); i++)
	{
		CIPCDrive* pDrive = m_Drives.GetDriveEx(m_sDisconnectedDrives.GetAt(i));
		if (pDrive)
		{
			sRoot = pDrive->GetRootString();
			for (j=m_Archives.GetSize()-1; j>=0; j--)
			{
				pA = m_Archives.GetAtFast(j);
				sFixed = pA->GetFixedDrive();
				if (!sFixed.IsEmpty())
				{
					sFixed.MakeLower();
					if (sRoot.Find(sFixed) != -1)
					{
						WK_TRACE(_T("archive without drive: %s\n"), pA->GetName());
						m_Archives.RemoveAt(j);
						// TODO! RKE: Notify Server
						WK_DELETE(pA);
					}
				}
			}
			m_Drives.Delete(pDrive);
		}
	}

	m_Archives.ScanBackup();
	w.StopWatch(s);

	WK_DELETE(pWKP);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::Exit()
{
	m_bExiting = TRUE;
	WK_TRACE(_T("CLOSING ...\n"));

	m_VerifyThread.StopThread();
	m_MoveThread.StopThread();
	BOOL bSaveThreadOK = m_SaveThread.StopThread();

	CWK_StopWatch w;
	WK_DELETE(m_pCIPCServerControlDBS);

	m_Queries.SafeDeleteAll();
	Sleep(10);
	m_Clients.SafeDeleteAll();
	Sleep(10);
	WK_DELETE(m_pCIPCDatabaseServerToServer);
	w.StopWatch(_T("deleted objects"));
	Sleep(10);
	
	w.StartWatch();
	
	m_Archives.Finalize(TRUE);

	w.StopWatch(_T("finalized archives"));
	Sleep(10);

	w.StartWatch();
	ExitThreads();
	w.StopWatch(_T("exit threads"));
	Sleep(100);

	m_SequenceMap.Close();
	for (int i=0;i<m_Drives.GetSize();i++)
	{
		CIPCDrive* pDrive = m_Drives.GetAtFast(i);

		if (   pDrive->IsRemoveableDatabase()
			&& CDrives::IsRW(pDrive->GetRootString()))
		{
/*
			CRHDDialog dlg(FALSE,pDrive);
			dlg.DoModal();
			if (dlg.GetRWToRO())
*/
			{
				CDrives::DoRWToRO(pDrive);
			}
		}
	}
	m_SequenceMap.DeleteAll();

	m_Archives.SafeDeleteAll();
	m_Conversions.DeleteAll();
	m_Drives.DeleteAll();
	m_sDisconnectedDrives.Empty();

	WK_TRACE(_T("CLOSING ... COMPLETE\n"));

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	WK_TRACE(_T("END DBS 4.0\n"));

	if (!IsReadOnly())
	{
		CWK_Profile wkp;
		
		wkp.WriteInt(szSection,_T("TraceQueue"),m_bTraceQueue);
		wkp.WriteInt(szSection,_T("TraceDrives"),m_bTraceDrives);
		wkp.WriteInt(szSection,_T("TraceSequence"),m_bTraceSequence);
		wkp.WriteInt(szSection,_T("TraceRequests"),m_bTraceRequests);
		wkp.WriteInt(szSection,_T("TraceCodebase"),m_bTraceCodebase);
		wkp.WriteInt(szSection,_T("TraceQuery"),m_bTraceQuery);
		wkp.WriteInt(szSection,_T("TraceOpenClose"),m_bTraceOpenClose);
		wkp.WriteInt(szSection,_T("TraceBackup"),m_bTraceBackup);
		wkp.WriteInt(szSection,_T("TraceDelete"),m_bTraceDelete);
		wkp.WriteInt(szSection,_T("TraceQueryResults"),m_bTraceQueryResults);
		wkp.WriteInt(szSection,_T("TracePreMove"),m_bTracePreMove);
		wkp.WriteInt(szSection,_T("OffsetCorrected"),m_bOffsetCorrected);

		wkp.WriteInt(szSection,_T("ViewDrives"),m_bViewDrives);
		wkp.WriteInt(szSection,_T("ViewClients"),m_bViewClients);
		wkp.WriteInt(szSection,_T("ViewArchivs"),m_bViewArchivs);
		wkp.WriteInt(szSection,_T("ViewThreads"),m_bViewThreads);
		wkp.WriteInt(szSection,_T("EnableDebugger"),(m_pDebugger!=NULL));
	}
	else
	{
		CWK_Profile wkpAbs(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
		CString str = BASE_DVRT_KEY;
		if (!(m_bReadOnlyModus & REGKEY_BASEDVRT_DEFINED))
		{
			wkpAbs.DeleteSection(str);
		}
	}

	Sleep(10);

	WK_DELETE(m_pDebugger);
	
	CloseDebugger();

	ExitReadOnlyModus();

	return bSaveThreadOK;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::StartThreads()
{
	int i;
	BOOL bInitThreads = TRUE;
	CIPCDrive* pDrive = NULL;
	int nDBD = 0;
	int nStorage = 0;

	for (i=0;i<m_Drives.GetSize();i++)
	{
		pDrive = m_Drives.GetAtFast(i);
		if (   pDrive->IsDatabase()
			|| pDrive->IsRemoveableDatabase())
		{
			nDBD++;
		}
		
		if (   pDrive->GetType() == DRIVE_FIXED
			|| (   pDrive->IsNetworkDrive() 
				&& pDrive->IsDatabase()
			    && pDrive->IsConnected()
				)
			)
		{
			nStorage++;
		}
	}

	// is there any drive ???
	if (   (nDBD==0) 
		&& (m_Archives.GetSize()>0)
		)
	{
		CString s;
		s.LoadString(IDS_NO_DATABASE_DRIVE);
		CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_CONFIGURATION, s);
		e.Send();
		bInitThreads = FALSE;
	}

	if (nStorage == 0)
	{
		CString s;
		s.LoadString(IDS_NO_STORAGE_DRIVE);
		CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_HARDDISK_FALURE, s, 0, 1);
		e.Send();
		bInitThreads = FALSE;
	}

	// is there enough space left on all drives
	DWORD dwTotalSize = m_Archives.GetMB();
	DWORD dwAvailable = m_Drives.GetAvailableMB();
	if (dwTotalSize > dwAvailable)
	{
		CString s;
		WK_RunTimeErrorLevel rel = REL_ERROR;
		dwTotalSize = m_Archives.GetMB(FALSE);
		if (   m_bAutoReduceArchiveSize 
			&& dwAvailable > 0
			&& dwTotalSize > 0)
		{
			int i, n = m_Archives.GetSize();
			for (i=0; i<n; i++)
			{
				CArchiv *pArchiv = m_Archives.GetAtFast(i);
				if (pArchiv->GetFixedDrive().IsEmpty())	// only reduce not fixed
				{
					DWORD dwSizeMB = pArchiv->GetSizeMB();
					if (dwSizeMB)
					{
						dwSizeMB = MulDiv(dwSizeMB, dwAvailable, dwTotalSize);
						pArchiv->SetSizeInMB(dwSizeMB);
					}
				}
			}
			rel = REL_WARNING;
			s.Format(IDS_ADAPT_ARCHIVE_SIZE, MulDiv(100, dwAvailable, dwTotalSize));
			bInitThreads = TRUE;
		}
		else
		{
			s.LoadString(IDS_ARCHIV_LESS_THAN_DRIVES);
			bInitThreads = FALSE;
		}

		CWK_RunTimeError runtimeError(	WAI_DATABASE_SERVER, 
										rel, 
										RTE_CONFIGURATION, 
										s);
		runtimeError.Send();
	}

	if (m_Conversions.GetSize())
	{
		// Convert old DB
		Convert();
	}
	else
	{
		if (bInitThreads)
		{
			CString s;
			CWK_StopWatch w;
			s.LoadString(IDS_INIT_THREADS);
			LVAddString(s);
			w.StartWatch();
			m_SaveThread.StartThread();
			w.StopWatch(s);
			m_MoveThread.StartThread();
			m_VerifyThread.StartThread();
		}
		if (LVClear())
		{
			LVDrives();
			LVArchivs();
			LVClients();
			LVThreads();
		}

		if (m_pCIPCServerControlDBS == NULL)
		{
			m_pCIPCServerControlDBS = new CIPCServerControlDBS();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InitializeReadOnly()
{
	CString s;

	WK_TRACE(_T("InitializeReadOnly %s\n"),m_sStartDrive);
	s.LoadString(IDS_INIT_DRIVES);
	LVAddString(s);
	CWK_StopWatch w;
	CIPCDrive* pDrive = new CIPCDrive(m_sStartDrive,DVR_READ_BACKUP_DRIVE);
	m_Drives.Add(pDrive);
	w.StopWatch(s);

	InitFieldsRO();

	s.LoadString(IDS_INIT_ARCHIVS);
	LVAddString(s);
	w.StartWatch();
	m_Archives.ScanBackupDrive(pDrive);
	w.StopWatch(s);

	if (LVClear())
	{
		LVDrives();
		LVArchivs();
		LVClients();
		LVThreads();
	}
	if (m_pCIPCServerControlDBS == NULL)
	{
		m_pCIPCServerControlDBS = new CIPCServerControlDBS();
	}

	WinExec("audiounit.exe",SW_SHOWNORMAL);
#if _MFC_VER < 0x0700
	WinExec("recherche.exe",SW_SHOWNORMAL);
#else
	if (IsIDIP())
	{
		WinExec("idipclient.exe",SW_SHOWNORMAL);
	}
	else
	{
		WinExec("dvclient.exe",SW_SHOWNORMAL);
	}
#endif

}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::StartFullReset()
{
	m_bFullResetting = TRUE;

	WK_TRACE(_T("RESETTING ...\n"));

	// clear all
	Clear();
	// clear complete

	if (IsReadOnly())
	{
		InitializeReadOnly();
		WK_TRACE(_T("RESETTING ... COMPLETE\n"));
		m_bFullResetting = FALSE;
	}
	else
	{
		m_pInitializeThread = new CInitializeThread(TRUE);
		m_pInitializeThread->StartThread();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::FinishFullReset()
{
	StartThreads();

	InformClients();

	// load the new archive info for next reset
	CWK_Profile wkp;
	m_ArchivInfos.Load(wkp);

	WK_TRACE(_T("RESETTING ... COMPLETE\n"));
	m_bFullResetting = FALSE;

	if (WK_IS_WINDOW(m_pMainWnd))
	{
		PostMessage(m_pMainWnd->m_hWnd,WM_COMMAND,ID_FILE_ACTUALIZE,0);
	}

	SendAlarmFuel();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::IsChanged(CWK_Profile& wkp, BOOL bDoChanges)
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
	CArchiv* pArchiv;
	CArchivInfoArray newArchives;

	newArchives.Load(wkp);

	// first find out deleted archives
	for (int c = m_Archives.GetSize()-1;c>=0;c--)
	{
		pArchiv = m_Archives.GetAt(c);
		WORD wArchivNr = pArchiv->GetNr();
		CSecID id(SECID_GROUP_ARCHIVE,wArchivNr);

		//			TRACE(_T("checking existing %s %04lx\n"),pArchiv->GetName(),wArchivNr);
		pArchivInfo = newArchives.GetArchivInfo(id);

		if (pArchivInfo == NULL)
		{
			// it's deleted, so delete the Archiv
			if (bDoChanges)
			{
				CString s = pArchiv->GetName();
				WK_TRACE(_T("deleting %s %d\n"),s,wArchivNr);
				if (m_Archives.DeleteArchive(wArchivNr))
				{
					WK_TRACE(_T("deleted %s %d\n"),s,wArchivNr);
				}
				else
				{
					WK_TRACE(_T("cannot delete %s %d\n"),s,wArchivNr);
				}
			}
			pArchiv = NULL;
			bArchivesChanged = TRUE;
		}
		else
		{
			// new size/name ?
			if (pArchiv->GetName() != pArchivInfo->GetName())
			{
				if (bDoChanges)
				{
					WK_TRACE(_T("new name %s -> %s\n"),pArchivInfo->GetName(),pArchiv->GetName());
					pArchiv->SetName(pArchivInfo->GetName());
					m_Clients.DoIndicateNewArchivName((DWORD)-1,*pArchiv);
				}
				bArchivesChanged = TRUE;
			}
			if (pArchiv->GetSizeMB() != pArchivInfo->GetSizeMB())
			{
				if (bDoChanges)
				{
					WK_TRACE(_T("new size %d -> %d\n"),
						pArchiv->GetSizeMB(),
						pArchivInfo->GetSizeMB());
					pArchiv->SetSize(pArchivInfo->GetSizeMB());
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
		WORD wArchivNr = pArchivInfo->GetArchivNr();
		pArchiv = m_Archives.GetArchiv(wArchivNr);

		if (pArchiv == NULL)
		{
			// it's a new Archiv
			if (bDoChanges)
			{
				WK_TRACE(_T("creating %s %d\n"),pArchivInfo->GetName(),wArchivNr);
				pArchiv = new CArchiv(*pArchivInfo);
				m_Archives.SafeAdd(pArchiv);
				m_Clients.DoIndicateNewArchiv(*pArchiv);
			}
			bArchivesChanged = TRUE;
		}
		else
		{
			// gibt's schon oben!
			//				TRACE(_T("already handled %s\n"),pArchiv->GetName());
		}
	}

	if (m_Archives.GetSize() != newArchives.GetSize())
	{
		WK_TRACE(_T("internal error in fast reset\n"));
	}

	return bDrivesChanged || bArchivesChanged;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::FastReset()
{
	if (m_BackupThreads.GetSize()==0)
	{
		if (   !m_bFastResetting
			&& !m_bFullResetting)
		{
			m_bFastResetting = TRUE;
			((CFrameWnd*)m_pMainWnd)->RedrawWindow();

			CWK_StopWatch w;
			CWK_Profile* pWKP = NULL;
			if (IsDTS())
			{
				pWKP = new 	CWK_Profile(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
			}
			else
			{
				pWKP = new CWK_Profile();
			}
			
			WK_TRACE(_T("FAST RESET ...\n"));
			BOOL bChanged = IsChanged(*pWKP,FALSE);

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
				if (IsDTS())
				{
					sPath = _T("c:\\dv");
				}
				else
				{
					sPath = _T("c:\\security");
				}

#endif
				CString sFilepath = sPath + _T("\\sequence.dbf");
				m_SequenceMap.DeleteAll();
				m_SequenceMap.Load(sFilepath);

				// really do the changes
				IsChanged(*pWKP,TRUE);

				m_SaveThread.Reset();
				// load the new info for next full reset
				m_ArchivInfos.Load(*pWKP);
				m_SaveThread.StartThread();
			}
			if (WK_IS_WINDOW(m_pMainWnd))
			{
				PostMessage(m_pMainWnd->m_hWnd,WM_COMMAND,ID_FILE_ACTUALIZE,0);
			}
			w.StopWatch(_T("FAST RESET"));
			WK_TRACE(_T("FAST RESET ... SUCCESS %s\n"),bChanged ? _T("CHANGED") : _T("UNCHANGED"));
			m_bFastResetting = FALSE;
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
void CDBSApp::InformClients()
{
	InformClientsAboutDrives();
	InformClientsAboutArchives();
	InformClientsAboutDatabaseFields();
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InformClientsAboutDrives()
{
	// inform about drives, they may have been changed
	CClient* pClient;
	CAutoCritSec acs(&m_Clients.m_cs);
	for (int i=0;i<m_Clients.GetSize();i++)
	{
		pClient = m_Clients.GetAtFast(i);
		if (   pClient 
			&& pClient->IsConnected()
			&& pClient->GetCIPCDatabaseServer()->GetDriveRequests())
		{
			pClient->GetCIPCDatabaseServer()->DoConfirmDrives(m_Drives);
		}
	}
	acs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InformClientsAboutArchives()
{
	CArchivInfo* pArchivInfo;
	CArchiv* pArchiv;
	CClient* pClient;

	// first inform about deleted archives
	for (int a=0;a<m_ArchivInfos.GetSize();a++)
	{
		pArchivInfo = m_ArchivInfos.GetAtFast(a);
		pArchiv = m_Archives.GetArchiv(pArchivInfo->GetArchivNr());

		if (pArchiv == NULL)
		{
			// it's deleted
			CAutoCritSec acs(&m_Clients.m_cs);
			for (int i=0;i<m_Clients.GetSize();i++)
			{
				pClient = m_Clients.GetAtFast(i);
				if (   pClient 
					&& pClient->IsConnected()
					&& pClient->GetCIPCDatabaseServer()->GetFieldRequests())
				{
					pClient->GetCIPCDatabaseServer()->DoIndicateRemoveArchiv(pArchivInfo->GetArchivNr());
				}
			}
			acs.Unlock();
		}
	}

	// then inform about new archives
	for (a=0;a<m_Archives.GetSize();a++)
	{
		pArchiv = m_Archives.GetAtFast(a);
		CSecID id(SECID_GROUP_ARCHIVE,pArchiv->GetOriginalNr());
		pArchivInfo = m_ArchivInfos.GetArchivInfo(id);

		if (pArchivInfo == NULL)
		{
			// it's new
			CAutoCritSec acs(&m_Clients.m_cs);
			for (int i=0;i<m_Clients.GetSize();i++)
			{
				pClient = m_Clients.GetAtFast(i);
				if (pClient && pClient->IsConnected())
				{
					CIPCArchivRecord data;
					pClient->GetCIPCDatabaseServer()->ArchiveToArchiveRecord(*pArchiv,data);
					pClient->GetCIPCDatabaseServer()->DoIndicateNewArchiv(data);
				}
			}
			acs.Unlock();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::InformClientsAboutDatabaseFields()
{
	// inform about drives, they may have been changed
	CClient* pClient;
	CAutoCritSec acs(&m_Clients.m_cs);
	for (int i=0;i<m_Clients.GetSize();i++)
	{
		pClient = m_Clients.GetAtFast(i);
		if (pClient && pClient->IsConnected())
		{
			pClient->GetCIPCDatabaseServer()->DoConfirmFieldInfo(theApp.m_Fields);
		}
	}
	acs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::Convert()
{
	CConversionDialog dlg(m_pMainWnd);

	if (IDOK==dlg.DoModal())
	{
		OnConversionComplete();
		WK_TRACE(_T("conversion complete force reset\n"));
		theApp.DoReset();
	}
	else
	{
		m_pMainWnd->PostMessage(WM_COMMAND,ID_APP_EXIT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnConversionComplete()
{
	if (DoesFileExist(m_sDatabaseDir))
	{
		DeleteDirRecursiv(m_sDatabaseDir);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::CheckInitializeThreadFinished()
{
	if (   m_pInitializeThread 
		&& !m_pInitializeThread->IsRunning())
	{
		BOOL bInit = m_pInitializeThread->GetInit();
		WK_DELETE(m_pInitializeThread);
		if (bInit)
		{
			FinishFullReset();
			if (WK_IS_WINDOW(m_pMainWnd))
			{
				m_pMainWnd->RedrawWindow();
				m_pMainWnd->UpdateWindow();
			}
		}
		else
		{
			CWinApp::OnAppExit();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::CheckDrives(BOOL bCheck)
{
	int i, nSize = m_sDisconnectedDrives.GetLength();

	if (bCheck)
	{
		CWK_Profile wkp;
		for (i=nSize-1; i>=0; i--)
		{
			if (m_bExiting || m_bFullResetting)
			{
				return FALSE;
			}
			CIPCDrive drive(m_sDisconnectedDrives.GetAt(i), wkp);
			if (drive.IsNetworkDrive())
			{
//				drive.ConnectNetworkDrive();
//				pDrive->UpdateNetworkState();
				if (drive.IsConnected())
				{
					theApp.PostDriveChanged(FALSE, drive.GetLetter());
					return TRUE;
				}
			}
			else if (drive.IsRemoveableDatabase())
			{
				BOOL bRet = DoesFileExist(drive.GetRootString() + _T("*.*"));
				if (bRet)
				{
					theApp.PostDriveChanged(FALSE, drive.GetLetter());
					return TRUE;
				}
			}
		}
	}

	CIPCDrive* pDrive;
	for (i=m_Drives.GetSize()-1; i>=0; i--)
	{
		if (m_bExiting || m_bFullResetting)
		{
			return FALSE;
		}
		pDrive = m_Drives.GetAtFast(i);
		
		if (   pDrive->IsDatabase()			// database drives or
			|| (   pDrive->IsNetworkDrive()	// only network drives that are used
			    && (   pDrive->IsReadBackup() || pDrive->IsWriteBackup() ) ) )
		{
			nSize++;
			if (!bCheck)
			{
				continue;
			}
			//check for any file on network backup drive
			//could be last a little time, because of network timeout
			BOOL bRet = DoesFileExist(pDrive->GetRootString() + _T("*.*"));
			if (bRet == FALSE)
			{
				//if network backup drive is missing, send RTE
				CString sError = GetLastErrorString();
				CString sFmt;
				CString sVolume = pDrive->GetVolumeString();
				int nFind = sVolume.Find(_T(";"));
				if (nFind != -1)
				{
					sVolume = sVolume.Left(nFind);
				}
				sFmt.FormatMessage(AFX_IDP_FILE_NOT_FOUND, pDrive->GetRootString()
					+ _T(" -> ") 
					+ sVolume);
				sError = sFmt + _T("\n") + sError;
				WK_TRACE(_T("%s\n"), sError);
				CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_STORAGE, sError,0,0);
				e.Send();

				theApp.PostDriveChanged(TRUE, pDrive->GetLetter());
			}
		}
	}

	return nSize;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::CheckRemoveNetworkDrive(CString sDrive)
{
	if (   !m_bFullResetting 
		&& !m_bExiting
		&& !sDrive.IsEmpty())
	{
		sDrive.MakeLower();
		CIPCDrive*pD = m_Drives.GetDrive((char)sDrive.GetAt(0));
		if (pD && pD->IsNetworkDrive())
		{
			theApp.PostDriveChanged(TRUE, sDrive.GetAt(0));
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CDBSApp::HandleFileException(CFileException*pFE, BOOL bThrow)
{
	CString sLastError;
	signed short nDBError = 0;
	if (LOWORD(pFE->m_lOsError) == 0 && HIWORD(pFE->m_lOsError) != 0)
	{
		nDBError = (signed short)HIWORD(pFE->m_lOsError);
		sLastError.Format(_T("DB Error:%d"), nDBError);
	}
	else
	{
		sLastError = GetLastErrorString(pFE->m_lOsError);
	}
	WK_TRACE(_T("%s: %s, %s, Cause:%d, Throw:%d\n"),_T(__FUNCTION__), pFE->m_strFileName, sLastError, pFE->m_cause, bThrow);
	CUnhandledException::TraceCallStack(NULL);
	if (IsNetworkStorageAllowed())
	{
		switch (LOWORD(pFE->m_lOsError))
		{
			// Network
			case ERROR_BAD_NETPATH:
			case ERROR_BAD_NET_NAME:
			case ERROR_BAD_NET_RESP:
			case ERROR_NET_WRITE_FAULT:
			case ERROR_DEV_NOT_EXIST:
			case ERROR_NETNAME_DELETED:
			case ERROR_NETWORK_ACCESS_DENIED:
			case ERROR_NETWORK_NOT_AVAILABLE:
			case ERROR_NETWORK_UNREACHABLE:
			case ERROR_NO_NET_OR_BAD_PATH:
				if (bThrow)
				{
					AfxThrowFileException(pFE->m_cause, pFE->m_lOsError, pFE->m_strFileName);
				}
				return TRUE;
				break;
		}
	}

	if (pFE->m_cause == CFileException::generic && nDBError)
	{
		pFE->m_lOsError = TranslateDBError(nDBError);
	}

	switch (LOWORD(pFE->m_lOsError))
	{
		// Local
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
		case ERROR_BAD_PATHNAME:
		case ERROR_INVALID_DRIVE:
//		case ERROR_SECTOR_NOT_FOUND:
//		case ERROR_BAD_UNIT:
//		case ERROR_WRITE_FAULT:
//		case ERROR_NOT_READY:
			if (bThrow)
			{
				AfxThrowFileException(pFE->m_cause, pFE->m_lOsError, pFE->m_strFileName);
			}
			return TRUE;
			break;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CDBSApp::TranslateDBError(int nDBError)
{
	DWORD dwError = 0;
	switch (nDBError)
	{
		case r4noCreate:	// Could not create file
		case e4create:
		case e4close:
			dwError = ERROR_BAD_PATHNAME;
			break;
		case r4noExist:		// File does not exist
		case r4noOpen:		// Could not open file
		case e4open:
		case e4fileFind:
		case e4read:
		case e4write:
//		case e4append:
//		case e4seek:
			dwError = ERROR_FILE_NOT_FOUND;
			break;
		case 0: break;
		default:
			dwError = 0;
			break;
	}
	return dwError;
}
//////////////////////////////////////////////////////////////////////
BOOL CDBSApp::HandleDriveError(DWORD dwLastError, CString sDrive)
{
	CString s, sError = GetLastErrorString(dwLastError);
	BOOL bRemoved = TRUE;
	WK_TRACE(_T("Drive Error occurred: %s, %s\n"), sDrive, sError);

	s.FormatMessage(AFX_IDP_FILE_NOT_FOUND, sDrive);
	sError = s + _T("\n") + sError;
	CWK_RunTimeError e(WAI_DATABASE_SERVER, REL_ERROR, RTE_STORAGE, sError,0,0);
	e.Send();

	sDrive.MakeLower();
	s = sDrive + _T("*.*");
	if (DoesFileExist(s) == FALSE)
	{
		PostDriveChanged(bRemoved, sDrive.GetAt(0));
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::PostDriveChanged(BOOL bRemoved, _TCHAR cRoot)
{
	if (m_pMainWnd)
	{
		return m_pMainWnd->PostMessage(WM_USER, MAKELONG(WPARAM_DRIVE_CHANGED, bRemoved), (LPARAM)cRoot);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnDriveChanged(BOOL bRemoved, _TCHAR cRoot)
{
	if (m_bExiting)
	{
		return;
	}

	BOOL bDoHandle = TRUE;
	CIPCDrive* pDrive = NULL;
	if (bRemoved)
	{
		pDrive = m_Drives.GetDrive((char)cRoot);
		if (pDrive)
		{
			pDrive->Enable(FALSE);
			if (m_sDisconnectedDrives.Find(cRoot) != -1)
			{
				bDoHandle = FALSE;	// must be already handled
			}
			else
			{
				m_sDisconnectedDrives += cRoot;
			}
			if (bDoHandle)
			{
				WK_RunTimeErrorLevel rtel = REL_ERROR;
				CString sLogPath = GetLogPath();
				sLogPath.MakeLower();
				CString sError, sTxt;
				DWORD dwParam1 = cRoot;
				DWORD dwParam2 = 0x01|0x02; // Alarm an, MessageBox
				sError.Format(IDS_HDD_FAILURE, cRoot, pDrive->GetVolumeString());
				sTxt.LoadString(pDrive->GetType());
				sError += sTxt;
				if (sLogPath.GetAt(0) == cRoot)
				{
					rtel = REL_REBOOT_ERROR;
					DeleteLogPath();
					WK_TRACE(_T("HDD %c:\\ with Pagefile removed Sending RTE to reboot\n"), cRoot);
				}
				CWK_RunTimeError e(WAI_DATABASE_SERVER, rtel, RTE_HARDDISK_FALURE, sError, dwParam1, dwParam2);
				e.Send();
			}
		}
	}
	else
	{
		if (m_sDisconnectedDrives.Find(cRoot) != -1)
		{
			pDrive = m_Drives.GetDriveEx(cRoot);
			if (pDrive)
			{
				if (pDrive->IsEnabled())
				{
					bDoHandle = FALSE; // must be already handled
				}
				else
				{
					pDrive->Enable(TRUE);
				}
			}
			else
			{
				CWK_Profile wkp;
				m_Drives.Add(new CIPCDrive(cRoot, wkp));
			}
			CString sRoot(cRoot); 
			m_sDisconnectedDrives.Replace(sRoot, _T(""));
		}
	}

	TRACE(_T("OnDriveChanged(%d, %c:\\), handled:%d, resetting:%d\n"), bRemoved, cRoot, bDoHandle, m_bFullResetting);

	if (   bDoHandle 
		&& !m_bFullResetting
		&& pDrive)
	{
		if (pDrive->IsDatabase())
		{
			DoReset();
		}
		else if (bRemoved)
		{
			OnBackupDriveRemoved(pDrive);
		}
		else
		{
			OnBackupDriveArrived(pDrive);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::OnIdle(LONG lCount) 
{
	if (!m_bExiting)
	{
		CheckInitializeThreadFinished();
		OnConnectionRecord();
		OnRequestQuery();
		OnRequestBackup();

		m_Clients.OnIdle();

		if (!IsReadOnly())
		{
			// check database to server connection
			if (m_pCIPCDatabaseServerToServer == NULL)
			{
				CPermissionArray pa;
				CWK_Profile wkp;
				pa.Load(wkp);
				m_pCIPCDatabaseServerToServer = new CIPCDatabaseServer(NULL, _T("DataBaseToServer"),pa.GetSuperVisor());
				// It is an internal connection, both objects have the same codepage as default
			}
			else 
			{
				if (m_pCIPCDatabaseServerToServer->GetIsMarkedForDelete())
				{
					WK_DELETE(m_pCIPCDatabaseServerToServer);
				}
			}
		}

		OnQueryFinished();
		OnBackupFinished();

		if (m_sArrivedDrives.GetLength())
		{
			CString sRoot;
			sRoot.Format(_T("%c:\\"), m_sArrivedDrives.GetAt(0));
			m_sArrivedDrives = m_sArrivedDrives.Mid(1); // remove first entry
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
		if (m_pMainWnd)
		{
			ASSERT_KINDOF(CMainFrame, m_pMainWnd);
			((CMainFrame*)m_pMainWnd)->OnIdle();
		}
		// sleep a while for lower processor duty
		Sleep(0);
	}
	
	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::AddConnectionRecord(const CConnectionRecord& c)
{
	m_aConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	AfxGetMainWnd()->PostMessage(WM_USER,WPARAM_CONNECTION_RECORD);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnConnectionRecord()
{
	if (m_aConnectionRecords.GetCount())
	{
		CConnectionRecord* pCR = m_aConnectionRecords.SafeGetAndRemoveHead();
		
		CClient* pClient = m_Clients.AddClient();
		CIPCError result = pClient->Connect(*pCR);
		if (result == CIPC_ERROR_OKAY)
		{
			CString shmName = pClient->GetCIPCDatabaseServer()->GetShmName();
			m_pCIPCServerControlDBS->DoConfirmDataBaseConnection(pClient->GetID(),
																 shmName,
																 _T(""),
																 0
#ifdef _UNICODE
																 , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																 );
		}
		else
		{
			m_pCIPCServerControlDBS->DoIndicateError(0, 
				SECID_NO_ID, result, 0);
//				SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
		}
		
		WK_DELETE(pCR);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnRequestQuery()
{
	if (m_Queries.GetCount())
	{
		CQuery* pQuery = m_Queries.SafeGetAndRemoveHead();

		CQueryThread* pQueryThread;

		pQueryThread = new CQueryThread(pQuery);
		m_QueryThreads.SafeAdd(pQueryThread);
		pQueryThread->StartThread();
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnQueryFinished()
{
	// check for finished Search threads
	if (m_QueryThreads.GetSize())
	{
		int i;
		CQueryThread* pQueryThread;
		for (i=0;i<m_QueryThreads.GetSize();i++)
		{
			pQueryThread = m_QueryThreads.GetAtFast(i);
			if (pQueryThread && !pQueryThread->IsRunning())
			{
				WK_TRACE(_T("removing old Query thread\n"));
				m_QueryThreads.RemoveAt(i);
				WK_DELETE(pQueryThread);
				break; // next CQueryThread will be removed from next OnIdle call;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::CancelQueries()
{
	CAutoCritSec acs(&m_QueryThreads.m_cs);
	for(int i=0;i<m_QueryThreads.GetSize();i++)
	{
		m_QueryThreads.GetAtFast(i)->Cancel();
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnRequestBackup()
{
	if (m_Backups.GetCount())
	{
		CBackup* pBackup = m_Backups.SafeGetAndRemoveHead();

		CBackupThread* pBackupThread;

		pBackupThread = new CBackupThread(pBackup);
		m_BackupThreads.SafeAdd(pBackupThread);
		pBackupThread->StartThread();
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnBackupFinished()
{
	// check for finished backup threads
	if (m_BackupThreads.GetSize())
	{
		TRACE(_T("OnBackupFinished\n"));
		int i;
		CBackupThread* pBackupThread;
		for (i=0;i<m_BackupThreads.GetSize();i++)
		{
			pBackupThread = m_BackupThreads.GetAtFast(i);
			if (pBackupThread)
			{
				if (!pBackupThread->IsRunning())
				{
					CString sBackupDrive = pBackupThread->GetBackup()->GetDestinationPath();
					m_BackupThreads.RemoveAt(i);
					WK_DELETE(pBackupThread);

					if (!sBackupDrive.IsEmpty())
					{
						CIPCDrive* pDrive = m_Drives.GetDrive(sBackupDrive);
						if (pDrive)
						{
							m_Archives.ScanBackupDrive(pDrive);
						}
					}
					break; // next CBackupThread will be removed from OnBackupFinished;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::CancelBackup(DWORD dwUserData)
{
	for (int i=0;i<m_BackupThreads.GetSize();i++)
	{
		CBackup* pBackup = m_BackupThreads.GetAtFast(i)->GetBackup();
		if (pBackup->GetUserData()==dwUserData)
		{
			m_BackupThreads.GetAtFast(i)->Cancel();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::ExitThreads()
{
	CQueryThread* pQueryThread;

	while (m_QueryThreads.GetSize())
	{
		pQueryThread = m_QueryThreads.GetAtFast(0);
		pQueryThread->StopThread();
		m_QueryThreads.RemoveAt(0);
		WK_DELETE(pQueryThread);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::LVClear()
{
	if (m_pMainWnd)
	{
		CDBSDoc* pDoc = (CDBSDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
		if (pDoc)
		{
			CDBSView* pView = pDoc->GetView();
			if (pView && pView->IsWindowVisible())
			{
				pView->Clear();
				return TRUE;
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::LVAddString(const CString& s)
{
	if (m_pMainWnd)
	{
		CDBSDoc* pDoc = (CDBSDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
		if (pDoc)
		{
			CDBSView* pView = pDoc->GetView();
			if (pView)
			{
				pView->Add(s);
				m_pMainWnd->UpdateWindow();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::LVArchivs()
{
	if (m_bViewArchivs)
	{
		int i;

		CString sWhole,sLoad,sAlarm;
		DWORD dw1,dw2;
		CIPCInt64 i1;
		i1 = m_Archives.GetUsedBytes();
		dw1 = i1.GetInMB();
		dw2 = m_Archives.GetMB();

		sLoad.LoadString(IDS_ARCHIVES_WHOLE);
		sWhole.Format(sLoad,i1.GetInt64(),
					  dw1,dw2,m_Archives.GetNrOfSequences());

		CString s;
		if (dw2>0)
		{
			s.Format(_T(" = %3.1f%%"),((double)((double)dw1*100.0)/(double)dw2));
		}
		sWhole += s;
		
		LVAddString(sWhole);

		sLoad.LoadString(IDS_ARCHIVES_ALARM);
		sAlarm.Format(sLoad,m_Archives.GetAlarmPercent());

		LVAddString(sAlarm);

		for (i=0;i<m_Archives.GetSize();i++)
		{
			LVAddString(_T("    ") + m_Archives.GetAtFast(i)->Format());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::LVDrives()
{
	if (m_bViewDrives)
	{
		int i;
		CIPCDrive* pDrive;

		CString sWhole,sLoad;
		CIPCInt64 i1;
		DWORD dw1,dw2;
		i1 = m_Drives.GetUsedBytes();
		dw1 = m_Drives.GetUsedMB();
		dw2 = m_Drives.GetMB();

		sLoad.LoadString(IDS_DRIVES_WHOLE);
		sWhole.Format(sLoad,i1.GetInt64(),dw1,dw2);
		
		CString s;
		if (dw2>0)
		{
			s.Format(_T(" = %3.1f%%"),((double)((double)dw1*100.0)/(double)dw2));
		}
		sWhole += s;
		LVAddString(sWhole);

		for (i=0;i<m_Drives.GetSize();i++)
		{
			pDrive = m_Drives.GetAtFast(i);
			if (   pDrive->IsDatabase() 
				|| pDrive->IsWriteBackup()
				|| pDrive->IsRemoveableDatabase())
			{
				LVAddString(_T("    ") + pDrive->Format());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::LVClients()
{
	if (m_bViewClients)
	{
		int i,c;
		CClient* pClient;

		CAutoCritSec acs(&m_Clients.m_cs);
		c = m_Clients.GetSize();

		LVAddString(_T("Clients"));

		for (i=0;i<c;i++)
		{
			pClient = m_Clients.GetAtFast(i);
			LVAddString(_T("    ") + pClient->GetName());
		}

		if (m_pCIPCDatabaseServerToServer && 
			m_pCIPCDatabaseServerToServer->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			LVAddString(_T("    Server"));
		}

		acs.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::LVThreads()
{
	if (m_bViewThreads)
	{
		LVAddString(_T("Threads"));
		int i=0; 
		DWORD dwID;
		CString sName, sFmt;
		while (GetThreadFromPool(i++, dwID, sName))
		{
			sFmt.Format(_T("  %s: %08x"), sName, dwID);
			LVAddString(sFmt);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::DoReset()
{
	if (   WK_IS_WINDOW(m_pMainWnd) 
		&& !IsResetting())
	{
		if (!m_bPosted)
		{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
			m_bPosted = TRUE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnFileReset() 
{
	StartFullReset();
	m_bPosted = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnFileFastReset()
{
	if (   (m_Archives.GetSize() > 0)
		&& (m_Drives.GetSize() > 0)
		)
	{
		FastReset();
	}
	else
	{
		StartFullReset();
	}
	m_bPosted = FALSE;
}
static DWORD s_dwTC = GetTickCount();
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnFileActualize(UINT nID) 
{
	if (!m_bFullResetting)
	{
		if (m_pMainWnd)
		{
			CDBSDoc* pDoc = (CDBSDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
			CDBSView* pView = pDoc->GetView();
			if (pView && !pView->IsWindowVisible())
			{
				return ;
			}
		}
		DWORD dwTickCount = GetTickCount();
		DWORD dwTimeSpan  = dwTickCount - s_dwTC;
		if (   nID == ID_FILE_ACTUALIZE  && dwTimeSpan > 100
			|| nID == ID_ACTUALIZE_TIMED && dwTimeSpan > 5000)
		{
			BOOL bClear = LVClear();
			LVAddString(IsIDIP()?_T("run as idip"):_T("run as DTS"));
			CString sMBpers;
			if (m_SaveThread.IsRunning())
			{
				sMBpers.Format(_T("%.3f MB / s %.3f FPS"),
					m_SaveThread.GetStatistics().GetMBperSecond(),
					m_SaveThread.GetStatistics().GetFPS());
				LVAddString(sMBpers);
			}
			m_Drives.CheckSpace();

			if (bClear)
			{
				LVDrives();
				LVArchivs();
				LVClients();
				LVThreads();
			}
			s_dwTC = dwTickCount;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnFileDeleteall() 
{
	int i;
	for (i=0;i<m_Archives.GetSize();i++)
	{
		m_Archives.GetAtFast(i)->DeleteAll();
	}
	OnFileActualize(ID_FILE_ACTUALIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceQueues() 
{
	m_bTraceQueue = !m_bTraceQueue;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceQueues(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceQueue);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceRequests() 
{
	m_bTraceRequests = !m_bTraceRequests;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceSequences() 
{
	m_bTraceSequence = !m_bTraceSequence;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceSequences(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceSequence);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceRequests(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceRequests);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceDrives() 
{
	m_bTraceDrives = !m_bTraceDrives;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceDrives(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceDrives);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnViewArchives() 
{
	m_bViewArchivs = !m_bViewArchivs;
	m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_ACTUALIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateViewArchives(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewArchivs);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnViewClients() 
{
	m_bViewClients = !m_bViewClients;
	m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_ACTUALIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateViewClients(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewClients);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnViewDrives() 
{
	m_bViewDrives = !m_bViewDrives;
	m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_ACTUALIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnViewThreads() 
{
	m_bViewThreads = !m_bViewThreads;
	m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_ACTUALIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateViewThreads(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bThreadPoolEnabled);
	pCmdUI->SetCheck(m_bViewThreads);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateViewDrives(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewDrives);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceCodebase() 
{
	m_bTraceCodebase = !m_bTraceCodebase;
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceCodebase(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceCodebase);
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::DoSendAlarmFuel()
{
	if (m_pMainWnd && m_pMainWnd->m_hWnd)
	{
		m_pMainWnd->PostMessage(WM_USER,WPARAM_SEND_ALARM_FUEL);
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::SendAlarmFuel()
{
	int iPercent = m_Archives.GetAlarmPercent();
	CWK_RunTimeError e(WAI_DATABASE_SERVER, 
					   REL_MESSAGE, 
					   RTE_ARCHIVE_FUEL, 
					   NULL,0,iPercent);
	e.Send();
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::InitReadOnlyModus()
{
	TCHAR szBuffer[_MAX_PATH];
	CString sFile;
	CString sTest1,sTest2;
	int p;

	GetModuleFileName(m_hInstance, szBuffer, sizeof(szBuffer));
	sFile = szBuffer;
#ifdef _DEBUG
	m_sStartDrive = sFile.Left(3);
//	m_sStartDrive = _T("e:\\");
#else
	m_sStartDrive = sFile.Left(3);
#endif

	p = sFile.ReverseFind(_T('\\'));
	CString sExe = sFile.Mid(p+1);

	if (0==sExe.CompareNoCase(_T("dbs.exe")))
	{
		// irgendein idip code
		m_InternalProductCode = IPC_IDIP_16;
	}
	else
	{
		// DTS
		m_InternalProductCode = IPC_DTS;
	}

	if (CDrives::IsRO(m_sStartDrive))
	{
		m_bReadOnlyModus = TRUE;
		HKEY hKey = NULL;
		CString str = BASE_DVRT_KEY;
		LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			m_bReadOnlyModus |= REGKEY_BASEDVRT_DEFINED;
		}
	}

	if (m_bReadOnlyModus)
	{
		WK_ALONE(WK_DB_SERVER_READ_ONLY);
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::ExitReadOnlyModus()
{
	// clean up something
	if (!m_bReadOnlyModus)
	{
		return;
	}

	DeleteFile(GetLogPath() + _T("\\dbsro.log"));
	DeleteFile(GetLogPath() + _T("\\recherchero.log"));
	DeleteFile(GetLogPath() + _T("\\audiounit1ro.log"));
	
	CString sAudio = GetLogPath() + _T("\\audiounit");
	CString sROLog = _T("ro.log");
	for (int i = 1; i <=4; i++)
	{
		CString sToDelete;
		sToDelete.Format(_T("%s%d%s"), sAudio, i, sROLog);
		if(DoesFileExist(sToDelete))
		{
			DeleteFile(sToDelete);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL DbsCopyFile(const CString& source, const CString& dest, BOOL bFailIfExist)
{
	BOOL bRet = TRUE;
	if (!CopyFile(source,dest,bFailIfExist))
	{
		WK_TRACE_ERROR(_T("copy FAILED %s -> %s, %s\n"),
			source,dest,GetLastErrorString());
		bRet = FALSE;
	}
	else
	{
		WK_TRACE(_T("copy SUCCESS %s -> %s\n"),source,dest);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnDeviceArrived(DEV_BROADCAST_HDR* pHdr)
{
	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			// Win NT has no USB and therefor all drives are predetermined
			COsVersionInfo os;
			if (   os.dwPlatformId == VER_PLATFORM_WIN32_NT
				&& os.dwMajorVersion == 4
				&& os.dwMinorVersion == 0)
			{
				WK_TRACE(_T("OnDeviceArrived %08lx\n"),pHdr->dbch_reserved);
				CIPCDrive* pDrive;
				for (int i=0 ; i<m_Drives.GetSize() && !IsResetting() ; i++)
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
			// With USB a total new unknown drive could arrive
			else
			{
				DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
				CString sRoot = GetDriveFromDEV_BROADCAST_VOLUME(pDBV);
				WK_TRACE(_T("OnDeviceArrived %s %d\n"),sRoot, pDBV->dbcv_flags);
				sRoot.MakeLower();
				CIPCDrive* pDrive = m_Drives.GetDriveEx(sRoot.GetAt(0));
				BOOL bFound = pDrive != NULL ? TRUE : FALSE;
				if (m_sDisconnectedDrives.Find(sRoot.GetAt(0)) != -1)
				{
					CString sDrive(sRoot.GetAt(0)); 
					m_sDisconnectedDrives.Replace(sDrive, _T(""));
				}

				if (   pDBV->dbcv_flags == 0
					&& !bFound)
				{
					if (pDrive == NULL)
					{	// complete new drive
						CWK_Profile wkp;
						pDrive = new CIPCDrive(sRoot.GetAt(0), wkp);
						if (pDrive->GetUsedAs() == DVR_NORMAL_DRIVE)
						{
							pDrive->SetUsedAs(DVR_READ_BACKUP_DRIVE);
						}
						m_Drives.Add(pDrive);
					}
					else
					{
						pDrive->Enable(TRUE);
						if (pDrive->IsNetworkDrive())
						{
							pDrive->UpdateNetworkState();
						}
					}
					bFound = TRUE;
					InformClientsAboutDrives();
				}

				if (pDrive && bFound)
				{
					if (!IsResetting())
					{
						pDrive->CheckSpace();
						WK_TRACE(_T("drive arrived %s %d MB all\n"),pDrive->GetRootString(),pDrive->GetMB());
						if (pDrive->IsDatabase())
						{
							DoReset();
						}
						else if (pDrive->GetMB()>0)
						{
							OnBackupDriveArrived(pDrive);
						}
					}
					else
					{
						if (m_sArrivedDrives.Find(sRoot.GetAt(0)) == -1)
						{
							m_sArrivedDrives += sRoot.GetAt(0);
						}
					}
				}
			}
		}
	}
	OnFileActualize(ID_FILE_ACTUALIZE);
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnBackupDriveArrived(CIPCDrive* pDrive)
{
	WK_TRACE(_T("new backup CD-ROM %s\n"),pDrive->GetRootString());
	m_Archives.ScanBackupDrive(pDrive);
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnDeviceRemoved(DEV_BROADCAST_HDR* pHdr)
{
	if (pHdr == NULL)
	{
		return;
	}
	if (pHdr->dbch_devicetype != DBT_DEVTYP_VOLUME)
	{
		return;
	}
	WK_TRACE(_T("device removed\n"));
	COsVersionInfo os;
	if (   os.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& os.dwMajorVersion == 4
		&& os.dwMinorVersion == 0)
	{
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
				if (pDrive->GetMB()==0)
				{
					OnBackupDriveRemoved(pDrive);
				}
			}
		}
	}
	else
	{
		DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
		CString sRoot = GetDriveFromDEV_BROADCAST_VOLUME(pDBV);
		WK_TRACE(_T("OnDeviceRemoved %s %d\n"),sRoot,pDBV->dbcv_flags);
		CIPCDrive* pDrive = NULL;
		for (int i=0;i<m_Drives.GetSize();i++)
		{
			pDrive = m_Drives.GetAtFast(i);
			if (0==sRoot.CompareNoCase(pDrive->GetRootString()))
			{
				WK_TRACE(_T("drive removed %s %d MB all\n"),pDrive->GetRootString(),pDrive->GetMB());
				if (   pDrive->GetType() == DRIVE_FIXED
					|| pDrive->IsDatabase())
				{
					CString sError, sTxt;
					sError.Format(IDS_HDD_FAILURE, pDrive->GetLetter(), pDrive->GetVolumeString());
					if (pDrive->IsSystem())
					{
						sTxt.LoadString(IDS_SYSTEM_DRIVE);
						sError += sTxt + _T(" ");
					}
					if (pDrive->IsDatabase())
					{
						sTxt.LoadString(IDS_STORAGE_DRIVE);
						sError += sTxt + _T(" ");
						pDrive->Enable(FALSE);
					}
					WK_RunTimeErrorLevel rtel = REL_ERROR;
					CString sLogPath = GetLogPath();
					sLogPath.MakeLower();
					sRoot.MakeLower();

					DWORD dwParam1 = sRoot.GetAt(0);
					DWORD dwParam2 = 0x01|0x02; // Alarm an, MessageBox
					if (sLogPath.GetAt(0) == sRoot.GetAt(0)) // RKE: hier ist auch das Pagefile
					{
						rtel = REL_REBOOT_ERROR;
						DeleteLogPath();
						WK_TRACE(_T("HDD %s with Pagefile removed Sending RTE to reboot\n"), sRoot);
					}

					CWK_RunTimeError e(WAI_DATABASE_SERVER, rtel, RTE_HARDDISK_FALURE, sError, dwParam1, dwParam2);
					e.Send();
					if (pDrive->GetType() == DRIVE_REMOVABLE)
					{
						OnDriveChanged(TRUE, sRoot.GetAt(0));
					}

					if (rtel != REL_REBOOT_ERROR)	// no reboot, 
					{								// do reset instead
						WK_TRACE(_T("HDD %s removed Sending RTE and reset DB\n"), sRoot);
						DoReset();
					}
				}
				else 
				{
					pDrive->CheckSpace();
					if (  !pDrive->IsDatabase() 
						&& pDrive->GetMB() == 0)
					{
						OnBackupDriveRemoved(pDrive);
						if (pDBV->dbcv_flags == 0)
						{
							WK_TRACE(_T("removing drive %s\n"), m_Drives.GetAtFast(i)->GetRootString());
							if (pDrive->GetType() == DRIVE_REMOVABLE)
							{
								m_Drives.RemoveAt(i);
								WK_DELETE(pDrive);
							}
							else
							{
								pDrive->Enable(FALSE);
								m_sDisconnectedDrives += m_Drives.GetAtFast(i)->GetLetter();
							}
							InformClientsAboutDrives();
						}
					}
				}
				break;
			}
		}
		m_sArrivedDrives.Replace(sRoot.Left(1), _T(""));
	}
	OnFileActualize(ID_FILE_ACTUALIZE);
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnBackupDriveRemoved(CIPCDrive* pDrive)
{
	CString sType = _T("CD-ROM");
	if (pDrive->IsDatabase())
	{
		return;
	}
	switch (pDrive->GetType())
	{
		case DRIVE_REMOTE:    sType = _T("Network"); break;
		case DRIVE_REMOVABLE: sType = _T("Removeable"); break;
		default: break;
	}
	WK_TRACE(_T("%s backup drive %s removed\n"), sType, pDrive->GetRootString());

	int i,c;
	c = m_Archives.GetSize();
	for (i=c-1;i>0;i--)
	{
		CArchiv* pArchive = m_Archives.GetAtFast(i);

		if (pArchive->IsBackup())
		{
			if (0==pArchive->GetFixedDrive().CompareNoCase(
				pDrive->GetRootString().Left(2)))
			{
				WK_TRACE(_T("removing backup archiv %s\n"),pArchive->GetName());
				m_Clients.DoIndicateDeleteArchiv(pArchive->GetNr());
				CAutoCritSec acs(&m_Archives.m_cs);
				m_Archives.RemoveAt(i);
				acs.Unlock();
				WK_DELETE(pArchive);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceQuery() 
{
	m_bTraceQuery = !m_bTraceQuery;
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceQuery(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceQuery);
}
////////////////////////////////////////////////////////////////////////////
void CDBSApp::LoadHosts()
{
	CWK_Profile wkp;
	m_Hosts.Load(wkp);
}
////////////////////////////////////////////////////////////////////////////
CHostArray& CDBSApp::GetHosts()
{
	return m_Hosts;
}
////////////////////////////////////////////////////////////////////////////
BOOL CDBSApp::CreateArchiveForHost(CSecID idHost,BOOL bAlarm,BOOL bSearch)
{
	CAutoCritSec acs(&m_csHosts);
	CWK_Profile wkp;
	m_Hosts.Load(wkp);
	m_Drives.CheckSpace();
	int iFreeMB = m_Drives.GetAvailableMB() - m_ArchivInfos.GetSizeMB();
    if (iFreeMB>0)
	{
		// is there any space left on drives
		CHost* pHost = m_Hosts.GetHost(idHost);

		if (pHost)
		{
			CArchivInfo* pAI = m_ArchivInfos.AddArchivInfo();

			if (pAI->GetSizeMB() > (DWORD)iFreeMB)
			{
				pAI->SetSizeMB(iFreeMB);
			}
			else
			{
				DWORD dwCalcSize = max(pAI->GetSizeMB(),(DWORD)min(150,iFreeMB/50));
				pAI->SetSizeMB(dwCalcSize);
			}

			CString s;

			if (bAlarm)
			{
				s.LoadString(IDS_ALARM);
				pHost->SetAlarmArchive(pAI->GetID());
			}
			else if (bSearch)
			{
				s.LoadString(IDS_SEARCH);
				pHost->SetSearchArchive(pAI->GetID());
			}
			else
			{
				s.LoadString(IDS_NORMAL);
				pHost->SetNormalArchive(pAI->GetID());
			}
			pAI->SetName(pHost->GetName() + _T(" ") + s);

			// now really create the archive
			CArchiv* pArchiv = new CArchiv(*pAI); 

			// add the archive to a thread
			m_Archives.SafeAdd(pArchiv);
			m_Hosts.Save(wkp);
			m_ArchivInfos.Save(wkp);
			m_Clients.DoIndicateNewArchiv(*pArchiv);
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceOpenClose() 
{
	m_bTraceOpenClose = !m_bTraceOpenClose;
	
}
/////////////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceOpenClose(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceOpenClose);
	
}
/////////////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceDelete() 
{
	m_bTraceDelete = !m_bTraceDelete;
}
/////////////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceDelete);
}
/////////////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTraceBackup() 
{
	m_bTraceBackup = !m_bTraceBackup;
}
/////////////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnUpdateTraceBackup(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceBackup);
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::OnTimer()
{
	if (!IsResetting())
	{
		if (   m_pCheckDrivesThread == NULL 
			&& CheckDrives(FALSE) > 0)
		{
			m_pCheckDrivesThread = new CCheckDriveThread();
		}

		if (m_pCheckDrivesThread)
		{
			m_pCheckDrivesThread->DoCheck();
		}

		//check backup directories, because the user could have deleted one or more backup folders
		//on the backup drive
		m_Archives.CheckBackupDirectories();
		
#ifndef _DEBUG
		if (m_SaveThread.IsRunning())
		{
			DWORD dwTick = GetTickCount();
			BOOL bExit = FALSE;
			if (GetTimeSpan(m_SaveThread.GetRunTick(),dwTick) > 20*1000)
			{
				WK_TRACE_ERROR(_T("save thread does not answer\n"));
				if (m_pDebugger)
				{
					m_pDebugger->Trace(0);
				}
				bExit = TRUE;
			}
			
			if (bExit)
			{
				m_pMainWnd->PostMessage(WM_COMMAND,ID_APP_EXIT);
			}
		}
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDBSApp::ReloadSequenceMap(BOOL bFromReset)
{
	if (!IsResetting() || bFromReset)
	{
		m_SequenceMap.Close();
		m_SequenceMap.DeleteAll();

		CString sPathname;

		sPathname = m_sMapDirectory + _T("\\sequence.dbf");
		m_SequenceMap.Load(sPathname);
	}
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OnAppExit() 
{
	BOOL bOneDriveWriteRemovable = FALSE;

	for (int i=0;i<m_Drives.GetSize();i++)
	{
		CIPCDrive* pDrive = m_Drives.GetAtFast(i);

		if (   pDrive->IsRemoveableDatabase()
			&& CDrives::IsRW(pDrive->GetRootString()))
		{
			bOneDriveWriteRemovable = TRUE;
		}
	}

	if (bOneDriveWriteRemovable)
	{
		m_pInitializeThread = new CInitializeThread(FALSE);
		m_pInitializeThread->StartThread();
	}
	else
	{
		if (Exit())
		{
			CWinApp::OnAppExit();
		}
		else
		{
			ExitProcess(0);
		}
	}
	
}
//////////////////////////////////////////////////////////////////////
void CDBSApp::OffsetCorrected()
{
	m_bOffsetCorrected = TRUE;
}
//////////////////////////////////////////////////////////////////////
int	CDBSApp::GetIntValue(CWK_Profile &wkp, LPCTSTR szValue, int nDefault)
{
	int nValue = wkp.GetInt(szSection, szValue, -1);
	if (nValue == -1)
	{
		nValue = nDefault;
		wkp.WriteInt(szSection, szValue, nValue);
	}
	return nValue;
}
//////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CDBSApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	else
	{
		COemGuiApi::ChangeLanguageResource();
		InitVideteLocaleLanguage();
	}

//  Set the CodePage for MBCS conversion, if necessary
	CWK_String::SetCodePage(uCodePage);
	CWK_Profile wkp;
	wkp.SetCodePage(CWK_String::GetCodePage());

	return 0;
}
#endif
