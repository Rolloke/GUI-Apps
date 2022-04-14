// SecAnalyzer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SecAnalyzer.h"

#include "wk.h"
#include "OemGui/OemGuiApi.h"

#include "MainFrm.h"
#include "SecAnalyzerDoc.h"
#include "SecAnalyzerView.h"

#include <afxadv.h>

#include "TimerTestDialog.h"
#include "TestProgressDialog.h"
#include "SecTimer.h"
#include "SecIDArray.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerApp

BEGIN_MESSAGE_MAP(CSecAnalyzerApp, CWinApp)
	//{{AFX_MSG_MAP(CSecAnalyzerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_DELETE_FILE, OnDeleteFile)
	ON_COMMAND(ID_VIEW_WORDPAD, OnViewWordpad)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_DO_TIMER_TEST, OnDoTimerTest)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CSecAnalyzerApp::OnMyFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerApp construction

CSecAnalyzerApp::CSecAnalyzerApp()
{
	m_pDoc=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSecAnalyzerApp object

CSecAnalyzerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerApp initialization

BOOL CSecAnalyzerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER <0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	InitDebugger(_T("SecAnalyzer.log"),WAI_ANALYZER );

	CString sTitle = COemGuiApi::GetApplicationName(WAI_ANALYZER);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

#if 0
	extern void TESTSplitPath();
	TESTSplitPath();
#endif
#if 0
	TestNewFailure();
#endif

#if 0
	CSecIDArray foo;
	CString sIDs=_T("1234");
	foo.FillFromString(sIDs);

	sIDs=_T("678,901");
	foo.FillFromString(sIDs);

	sIDs=_T("abc,123,,,,");
	foo.FillFromString(sIDs);
#endif

	// dongle check via applicationId
	CWK_Dongle dongle(WAI_ANALYZER);
	if (dongle.IsExeOkay()==FALSE) {	// already sends RunTimeError 
		return FALSE;
	}

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// NOT YET check for Tmp in name convert -> .lgz
	// NOT YET check file exists
	if (m_pRecentFileList && m_pRecentFileList->GetSize() )
	{
		m_sDefaultFile = (*m_pRecentFileList)[0];

	}
	else
	{
		m_sDefaultFile = GetLogPath() + _T("\\Server.log");
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSecAnalyzerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSecAnalyzerView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	// cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	GetMain()->m_wndSecAnalyzerBar.CheckDlgButton(IDC_START_DEFAULT,1);
	GetMain()->m_wndSecAnalyzerBar.CheckDlgButton(IDC_SHOW_LEVEL_1,1);
	GetMain()->m_wndSecAnalyzerBar.CheckDlgButton(IDC_SHOW_LEVEL_2,1);

	GetMain()->m_wndSecAnalyzerBar.UpdateGroups();

	//
	/*BOOL bLogDirFound =*/  SetCurrentDirectory(GetLogPath());
	// ignore failure
	return TRUE;
}

// App command to run the dialog
void CSecAnalyzerApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(m_pMainWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerApp commands

BOOL CSecAnalyzerApp::OnIdle(LONG lCount) 
{
	
	CWinApp::OnIdle(lCount);

	// auto update
	if (m_pDoc && GetMain() && 
		IsWindow(GetMain()->m_wndSecAnalyzerBar)
		) {
		if (GetMain()->m_wndSecAnalyzerBar.IsDlgButtonChecked(IDC_AUTO_UPDATE)) {
			DWORD dwDelta = 30;
			CString sDelta;
			CWnd *pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_UPDATE_INTERVALL);
			if (pWnd) {
				pWnd->GetWindowText(sDelta);
			}
			if (sDelta.GetLength()) {
				dwDelta = max(1, _ttoi(sDelta));
			}

			if ( (DWORD)((CTime::GetCurrentTime()-m_pDoc->m_lastUpdateTime).GetTotalSeconds())>dwDelta) {
				m_pDoc->ScanFile();
				m_pDoc->UpdateAllViews(NULL);
			}
		}
	}

	Sleep(50);

	return TRUE;
}

int CSecAnalyzerApp::ExitInstance() 
{
	WK_DELETE(m_pDoc);	// OOPS
	CloseDebugger();
	return CWinApp::ExitInstance();
}

void CSecAnalyzerApp::OnDeleteFile() 
{
	if (m_pDoc) {
		CString sQuestion;
		CString sTemp;
		sTemp.LoadString(IDS_REALLY_DELETE);
		sQuestion.Format(sTemp,m_pDoc->m_sFilename);
		int bResult = AfxMessageBox(sQuestion,MB_YESNO|MB_ICONQUESTION);
		if (bResult==IDYES) {
			CFile::Remove(m_pDoc->m_sFilename);
			m_pDoc->ScanFile();
			m_pDoc->UpdateAllViews(NULL);
		}	
	}
}
					 
void CSecAnalyzerApp::OnViewWordpad() 
{
	if (m_pDoc) {
		CString sCmd;
		sCmd.Format(_T("LogView.exe %s"),m_pDoc->m_sFilename);
//		WinExec(sCmd,SW_SHOW);
		STARTUPINFO si = {0};
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi = {0};
		BOOL bRet = CreateProcess(
								sCmd,	// LPCTSTR lpApplicationName,
								NULL,	// LPTSTR lpCommandLine,
								NULL,	// LPSECURITY_ATTRIBUTES lpProcessAttributes,
								NULL,	// LPSECURITY_ATTRIBUTES lpThreadAttributes,
								FALSE,	// BOOL bInheritHandles,
								0,		// DWORD dwCreationFlags,
								NULL,	// LPVOID lpEnvironment,
								NULL,	// LPCTSTR lpCurrentDirectory,
								&si,	// LPSTARTUPINFO lpStartupInfo,
								&pi		// LPPROCESS_INFORMATION lpProcessInformation
								);
		if (!bRet)
		{
			WK_TRACE(_T("Create process of %s FAILED\n"), sCmd);
		}
	}
}

#include "wkclasses\logziplib\LogZip.h"


static TCHAR szFilter[] = _T("Log Files (*.log,.*.lgz)|*.log;*.lgz|LogFiles(*.log)|*.log|Komprimierte Log Files (*.lgz)|*.lgz||");

afx_msg void CSecAnalyzerApp::OnMyFileOpen()
{
	CString sName;

	// CFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, 
	//				LPCTSTR lpszFileName = NULL, 
	//				DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );

	CFileDialog myDialog(TRUE, _T("*.log"), NULL, 0, szFilter);
	if (myDialog.DoModal()==IDOK) {
		// sName = myDialog.GetPathName();
		sName = myDialog.GetFileName();
		// TODO: Add your specialized code here and/or call the base class
		if (sName.GetLength()) {
			OpenDocumentFile(sName);	// call the overridden functions, which can deal with .lgz files
		}
	}

}

BOOL DecompressLGZFile(const CString & sName, CString &sNewName)
{
	sNewName = sName;;
	int ixSlash = sName.ReverseFind(_T('\\'));
	if (ixSlash != -1) {
		sNewName = sNewName.Left(ixSlash+1)
					+ _T("Tmp")
					+ sNewName.Mid(ixSlash+1);
	} else {
		sNewName = _T("Tmp")+sNewName;
	}
	sNewName= sNewName.Left(sNewName.GetLength()-3);
	sNewName += _T("lgz");
			
	
	if (DoesFileExist(sNewName)) {
		CFile::Remove(sNewName);
	}

	if (CopyFile(sName,sNewName,TRUE)) {
		if (WK_DecompressLogFile(sNewName)) {
			// fine, convert to xeidecompressed name
			sNewName= sNewName.Left(sNewName.GetLength()-3);
			sNewName += _T("log");

			return TRUE;
		} else {
			sNewName=_T("");	// OOPS what about tmp files
			return FALSE;
		}
	} else {
		CString sMsg;
		sMsg.Format(_T("Could not create tmp file (%s)\n"),GetLastErrorString());
		AfxMessageBox(sMsg,MB_OK);
		sNewName=_T("");
		return FALSE;
	}
}

CDocument* CSecAnalyzerApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	if (m_pDoc) {
		m_pDoc->DeleteTmpFile();
	}
	
	BeginWaitCursor();

	CString sName;
	sName = lpszFileName;
	if (sName.Find(_T(".lgz"))!= -1 || sName.Find(_T(".LGZ"))!=-1) {
		CString sNewName(sName);
		int ixSlash = sName.ReverseFind(_T('\\'));
		if (ixSlash != -1) {
			sNewName = sNewName.Left(ixSlash+1)
						+ _T("Tmp")
						+ sNewName.Mid(ixSlash+1);
		} else {
			sNewName = _T("Tmp")+sNewName;
		}
		if (CopyFile(sName,sNewName,TRUE)) {
			sName = sNewName;
			if (WK_DecompressLogFile(sName)) {
				// fine, convert to decompressed name
				sName= sName.Left(sName.GetLength()-3);
				sName += _T("log");
			}
		} else {
			AfxMessageBox(_T("Could not create tmp file\n"),MB_OK);
			sName=_T("");
		}
	}

	EndWaitCursor();

	return CWinApp::OpenDocumentFile(sName);
}

void CSecAnalyzerApp::OnFileNew() 
{
	CWinApp::OnFileNew();
}

void CSecAnalyzerApp::OnDoTimerTest() 
{
	CTimerTestDialog timerTestDialog;
	if (timerTestDialog.DoModal()!=IDOK) {
		return;	// <<< EXIT>>>
	}
	
	BeginWaitCursor();

	TestProgressDialog progressDialog;

	progressDialog.Create(IDD_TEST_PROGRESS_DIALOG);
	progressDialog.CenterWindow();
	progressDialog.ShowWindow(SW_NORMAL);
	progressDialog.UpdateWindow();

	CTime now = timerTestDialog.m_testTime; 
	BOOL bShowMids = timerTestDialog.m_bShowMids;

	// try to start the week on mondays
	CTime startTime = CTime( now.GetYear(),
						now.GetMonth(),
						now.GetDay(),
						0,0, 0);; // OPTIONAL CTime::GetCurrentTime();

	if (timerTestDialog.m_iTestIntervall>0){	// more than one day ?
		// move backwards in time, up to monday
		while (startTime.GetDayOfWeek()!=2) {	// 2==Monday
			startTime -= CTimeSpan(1,0,0,0);
		}
	}

	CTime endTime(startTime);

	//  constructor is CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs
	switch (timerTestDialog.m_iTestIntervall) {
		case 0:	// one day
			endTime += CTimeSpan(1,0,0,0);
		break;
		case 1:	// one week
			endTime += CTimeSpan(7,0,0,0);
		break;
		case 2:	// one month
			endTime += CTimeSpan(31,0,0,0);
		break;
		case 3:	// one year
			endTime += CTimeSpan(365,0,0,0);
		break;
		case 4:	// five years
			endTime += CTimeSpan(5*365,0,0,0);
		break;

		default:
			WK_TRACE_ERROR(_T("Invalid test intervall selction %d\n"),timerTestDialog.m_iTestIntervall);
			endTime += CTimeSpan(1,0,0,0);
	}

	// simulate the server and print the timer state changes in statlog format

#if 1
	CStdioFile outputFile;

	// OOPS hardcoded path
	outputFile.Open(GetLogPath() + _T("\\TimerTest.log"), 
		CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite);	// OOPS no check

	CWK_Profile wkp;
	CSecTimerArray timers;
	timers.SetAutoDelete(TRUE);
	timers.Load(startTime,wkp);
	timers.CalcTotalSpans(timers);	// give the root timer for complete calculation

	CTime ct(startTime);

	CString sInfo = startTime.Format(_T("%d %b %Y:%A,%H:%M:%S"));
	TRACE(_T("Start Time is %s\n"),sInfo);
	sInfo = endTime.Format(_T("%d %b:%A %Y,%H:%M:%S"));
	TRACE(_T("End   Time is %s\n"),sInfo);

	// init the progress control
	CTimeSpan overallTime = endTime-startTime;
	progressDialog.m_ctlProgress.SetRange32(0,overallTime.GetTotalMinutes());

	CSecTimerArray droppedTimers;
	int i=0;
	for (i=timers.GetSize()-1;i>=0;i--) {
		CSecTimer *pTimer = timers[i];
		if (timerTestDialog.m_timers.GetTimerByID(pTimer->GetID())==NULL) {
			TRACE(_T("Unused timer %s\n"),pTimer->GetName());
			droppedTimers.Add(timers[i]);
			// CAVEAT do not delete or remove in timers to keep the tree structure
		}
	}

	TRACE(_T("Have %d active timers %d are dropped\n"),timers.GetSize(),droppedTimers.GetSize());
	timers.UpdateTimers(ct);


	while (ct<=endTime) {
		for (i=0;i<timers.GetSize();i++) {
			CSecTimer *pTimer = timers[i];
			if (droppedTimers.GetTimerByID(pTimer->GetID())==NULL) {
				if (pTimer->GetNextModeChange() <= ct) {
#if 0
					CString sNext = pTimer->GetNextModeChange().Format(_T("%d %b:%A %Y,%H:%M:%S"));
					CString sCT = ct.Format(_T("%d %b:%A,%H:%M:%S"));
					TRACE(_T("[%s]Next change is %s, now it's %s\n"),
						(const char *)pTimer->GetName(), 
						(const char *)sNext,
						(const char *)sCT
						);
#endif

					int iValue;
					if (pTimer->IsIncluded(ct)) {
						iValue=1;
					} else {
						iValue=0;
					}

					// have to fake WK_STAT_LOG, 
					// since there is a loop over the current time
					{
						CString sMsg;
						sMsg.Format(_T("TimerLog@%02d.%02d.%4d,%02d:%02d:%02d@%d@Timer|%s\n"),
						ct.GetDay(),ct.GetMonth(), ct.GetYear(),
						ct.GetHour(), ct.GetMinute(), ct.GetSecond(),
						iValue,
						pTimer->GetName()
						);
						outputFile.WriteString(sMsg);

					}
					// NOT HERE pTimer->UpdateNextModeChange(ct+TimeSpan(0,0,0,1))// OOPS adjust to time advance of loop
				}
			} else {
				// a droppped timer
			}
		}	// end of loop over timers

		if (bShowMids) {
			CString sMidMsg;
			if (ct.GetHour()==0 && ct.GetMinute()==0 && ct.GetSecond()==0) {
				sMidMsg.Format(_T("TimerLog@%02d.%02d.%4d,%02d:%02d:%02d@~3@Timer|%s\n"),
				ct.GetDay(),ct.GetMonth(),ct.GetYear(),
				ct.GetHour(), ct.GetMinute(), ct.GetSecond(),
				_T("Mitternacht/Mittag")
				);
				outputFile.WriteString(sMidMsg);
			}

			// special peaks for noon, midnight
			if (ct.GetHour()==12 && ct.GetMinute()==0 && ct.GetSecond()==0) {
				sMidMsg.Format(_T("TimerLog@%02d.%02d.%4d,%02d:%02d:%02d@~1@Timer|%s\n"),
				ct.GetDay(),ct.GetMonth(),ct.GetYear(),
				ct.GetHour(), ct.GetMinute(), ct.GetSecond(),
				_T("Mitternacht/Mittag")
				);
				outputFile.WriteString(sMidMsg);
			}
		}

		CTime lastCt(ct);
		CTimeSpan deltaT;

		CTime nextChange(2038,1,1,1,1,1);
		for (i=0;i<timers.GetSize();i++) {
			CSecTimer *pTimer = timers[i];
			if (pTimer->GetNextModeChange() <= nextChange) {
					nextChange = pTimer->GetNextModeChange();
			}
		}

		CTimeSpan diffTime = (nextChange-lastCt);
		if (diffTime.GetTotalHours()) {
			deltaT = CTimeSpan(0,1,0,0);	// one hour steps
		} else {
			deltaT = CTimeSpan(0,0,1,0);	// minute steps
		}

		BOOL bUpdateString=FALSE;
		switch (timerTestDialog.m_iTestIntervall) {
			case 0:	// one day
			case 1:	// one week
				bUpdateString = (ct.GetMinute()==0 && ct.GetSecond()==0);
			break;
			case 2:	// one month
				bUpdateString = ((ct.GetHour()==0 || (ct.GetHour()%6)==0) && ct.GetMinute()==0 && ct.GetSecond()==0);
			break;
			case 3:	// one year
			case 4:	// five years
				bUpdateString = (ct.GetDay()==1 && ct.GetHour()==0 && ct.GetMinute()==0 && ct.GetSecond()==0);
			break;
		}

		
		if (bUpdateString) {
			progressDialog.m_sTestTime.Format(_T("%02d.%02d.%04d,%02d:%02d"),
				ct.GetDay(),ct.GetMonth(),ct.GetYear(),
				ct.GetHour(), ct.GetMinute()
				);
			progressDialog.UpdateData(FALSE);
		}


		ct += deltaT;

		// update the progress control
		progressDialog.m_ctlProgress.SetStep(deltaT.GetTotalMinutes());
		progressDialog.m_ctlProgress.StepIt();



		// change of weekday ?
		if (ct.GetDayOfWeek()!=lastCt.GetDayOfWeek()) {
			timers.UpdateTimers(lastCt);
		} else {
			// CAVEAT use a seperate loop to update the timers
			for (i=0;i<timers.GetSize();i++) {
				CSecTimer *pTimer = timers[i];
				if (pTimer->GetNextModeChange() <= lastCt) {
					pTimer->UpdateNextModeChange(ct);	// OOPS adjust to time advance of loop
			}
		}	// end of second loop over timers


		}

	}	// end of loop over ct
	outputFile.Close();
	EndWaitCursor();

	progressDialog.DestroyWindow();

	// now read the created file
	OpenDocumentFile(GetLogPath() + _T("\\TimerTest.log"));
	// OOPS does not always update the display

#endif
}


