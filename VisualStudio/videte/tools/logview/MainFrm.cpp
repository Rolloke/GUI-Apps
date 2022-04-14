/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: MainFrm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/LogView/MainFrm.cpp $
// CHECKIN:		$Date: 28.02.06 10:22 $
// VERSION:		$Revision: 67 $
// LAST CHANGE:	$Modtime: 28.02.06 10:18 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "User.h"
#include "Permission.h"
#include <oemgui\oemlogindialog.h>
#include <direct.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

#include "oemgui\oemgui.h"
#include "oemgui\OemFileDialog.h"

#include "LogView.h"
#include "LogViewView.h"
#include "LogViewDoc.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include "DlgLoadCompressed.h"

#define NO_LOGIN 1
#ifdef NO_LOGIN
  #include "wk_dongle.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_CBN_DROPDOWN(IDC_COMBO_LOG_FILES, OnDropDownComboLogFiles)
	ON_CBN_SELENDOK(IDC_COMBO_LOG_FILES, OnOkComboLogFiles)
	ON_CBN_DROPDOWN(IDC_COMBO_HTM_FILES, OnDropDownComboHtmFiles)
	ON_CBN_SELENDOK(IDC_COMBO_HTM_FILES, OnOkComboHtmFiles)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_COMMAND(IDC_COMBO_LOG_FILES, OnComboLogFilesViaAcc)
	ON_MESSAGE(WM_KEYBOARD_CHANGED, OnKeyboardChanged)
	ON_MESSAGE(WM_OPEN_FILE, OnOpenFile)
	ON_COMMAND(ID_VIEW_SYNC_TIME, OnViewSyncTime)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SYNC_TIME, OnUpdateViewSyncTime)
	ON_COMMAND(ID_FILE_SAVE_LOGFILES, OnFileSaveLogfiles)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_LOGFILES, OnUpdateFileSaveLogfiles)
	ON_COMMAND(ID_FILE_CHANGE_LOG_PATH, OnFileChangeLogPath)
	ON_UPDATE_COMMAND_UI(ID_FILE_CHANGE_LOG_PATH, OnUpdateFileChangeLogPath)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_UPDATE_AUTOMATICALLY, OnViewUpdateAutomatically)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_AUTOMATICALLY, OnUpdateViewUpdateAutomatically)
	ON_COMMAND(ID_VIEW_JUMP_TO_END, OnViewJumpToEnd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_JUMP_TO_END, OnUpdateViewJumpToEnd)
	ON_COMMAND(ID_FILE_DELETE_LOGFILES, OnFileDeleteLogfiles)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETE_LOGFILES, OnUpdateFileDeleteLogfiles)
	ON_COMMAND(ID_WINDOW_SYNC_COMPRESSED, OnWindowSyncCompressed)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SYNC_COMPRESSED, OnUpdateWindowSyncCompressed)
	ON_COMMAND(ID_FILE_SAVE_PROTOCOLLS, OnFileSaveProtocolls)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROTOCOLLS, OnUpdateFileSaveProtocolls)
	ON_COMMAND(ID_FILE_DELETE_PROTOCOLLS, OnFileDeleteProtocolls)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETE_PROTOCOLLS, OnUpdateFileDeleteProtocolls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_EXT,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

/////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	CWK_Profile prof;

	m_sLogPath	= GetLogPath();
	m_sHtmPath	= prof.GetString(SECTION_LOG, LOG_PROTOCOLPATH, _T("c:\\protocol"));
	m_nCurSelLogFile = -1;
	m_nCurSelHtmFile = -1;

	m_uUpdateTimerID		= 0;
	m_bScrollToEndAtRefresh = TRUE;

	m_bOk = FALSE;
	m_bInOnTimer = FALSE;
	m_uTimerID = 0;
	m_pSyncWindowsThread = NULL;
	m_nDay = 0;
	m_nSec = 0;
	m_bSyncCompressed = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	if (m_pSyncWindowsThread)
	{
		WaitForSingleObject(m_pSyncWindowsThread->m_hThread, 5000);
	}
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
	m_wndReBar.SetBarStyle(m_wndReBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	OnKeyboardChanged(0, 0);
	if (!Login())
		return -1;

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	// Wenn das Verzeichnis 'Protocols' nicht existiert, dann die HTML-Combobox nicht anzeigen.
	if (!DoesFileExist(m_sHtmPath))
	{
		WK_TRACE(_T("%s does not exist\n"), m_sHtmPath);
		GetComboBoxHtm()->ShowWindow(FALSE);
		(CWnd*)m_wndDlgBar.GetDlgItem(IDC_STATIC_HTML)->ShowWindow(FALSE);

	}

	FillComboBoxLog(_T("*.log"));
	FillComboBoxLog(_T("*.lgz"));
	FillComboBoxHtm(_T("*.htm"));

	m_uTimerID = SetTimer(1, 60 * 1000, NULL);
	if (m_uTimerID == 0) 
	{
		WK_TRACE_ERROR(_T("Timer failed\n"));
	}
	
	m_bOk = TRUE;
	CSkinDialog::SetChildWindowFont(m_hWnd);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_MAXIMIZE;
	cs.lpszName = theApp.m_pszAppName;
	cs.lpszClass = WK_APP_NAME_LOG_VIEW;

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ResetComboBoxLog()
{
	// delete simply everything 
	// it's fast it's simple
	CComboBox* pCombo = GetComboBoxLog();
	pCombo->ResetContent();
	
	FillComboBoxLog(_T("*.log"));
	FillComboBoxLog(_T("*.lgz"));
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::FillComboBoxLog(const CString &sExt)
{
	CString sLogName;
	CComboBox* pCombo = GetComboBoxLog();

	_tfinddata_t fileinfo;
	int	lNext		= 0;

	CString sLogFileName = m_sLogPath + _T("\\") + sExt;
	long  hFile = _tfindfirst(sLogFileName.GetBuffer(MAX_PATH), &fileinfo );
	sLogFileName.ReleaseBuffer();
	if (hFile == -1)
		return FALSE;

	do
	{
		sLogName = fileinfo.name;
		
		// Temporäre 'TmpXXX'-Dateien vom Log-Dateien vom Logzip nicht anzeigen
		if (sLogName.Find(_T("Tmp")) == -1)
		{
			// Ist der Eintrag schon in der Liste?
			// UF removed dauert einfach zu lange
			// die liste wir jetzt vorher komplett gelöscht
//			if (pCombo->FindStringExact(0, sLogName) == CB_ERR)
				pCombo->AddString(sLogName);
		}
		lNext = _tfindnext(hFile, &fileinfo);
		
	}while (lNext == 0);

	_findclose(hFile);

	// Aktuelle Auswahl wieder anzeigen
	if (m_nCurSelLogFile != -1)
		GetComboBoxLog()->SetCurSel(m_nCurSelLogFile);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ResetComboBoxHtm()
{
	// delete simply everything 
	// it's fast it's simple
	CComboBox* pCombo = GetComboBoxHtm();
	pCombo->ResetContent();
	
	FillComboBoxHtm(_T("*.htm"));
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::FillComboBoxHtm(const CString &sExt)
{
	CString sHtmlName;
	CComboBox* pCombo = GetComboBoxHtm();

	_tfinddata_t fileinfo;
	int	lNext		= 0;

	CString sLogFileName = m_sHtmPath + _T("\\") + sExt;
	long  hFile = _tfindfirst(sLogFileName.GetBuffer(MAX_PATH), &fileinfo );
	sLogFileName.ReleaseBuffer();
	if (hFile == -1)
		return FALSE;

	do
	{
		sHtmlName = fileinfo.name;
		
		// Ist der Eintrag schon in der Liste?
		// UF removed dauert einfach zu lange
		// die liste wir jetzt vorher komplett gelöscht
//		if (pCombo->FindStringExact(0, sLogName) == CB_ERR)
		pCombo->AddString(sHtmlName);
		lNext = _tfindnext(hFile, &fileinfo);
		
	}while (lNext == 0);

	_findclose(hFile);

	// Aktuelle Auswahl wieder anzeigen
	if (m_nCurSelHtmFile != -1)
		GetComboBoxHtm()->SetCurSel(m_nCurSelHtmFile);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDropDownComboLogFiles() 
{
	ResetComboBoxLog();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnOkComboLogFiles() 
{
	CString sText;
	CString sPathFile;

	CComboBox* pCombo = GetComboBoxLog();

	int nSel = pCombo->GetCurSel();	

	if (nSel != CB_ERR )
	{
		pCombo->GetLBText(nSel, sText);
		m_nCurSelLogFile = nSel;
		TRACE(_T("<%s> %d %d\n"), (LPCTSTR)sText, nSel, pCombo->GetCount());

		sPathFile = m_sLogPath + _T("\\") + sText;
		theApp.OpenDocumentFile(sPathFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDropDownComboHtmFiles() 
{
	ResetComboBoxHtm();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnOkComboHtmFiles() 
{
	CString sText;
	CString sPathFile;

	CComboBox* pCombo = GetComboBoxHtm();

	int nSel = pCombo->GetCurSel();

	if (nSel != CB_ERR )
	{
		pCombo->GetLBText(nSel, sText);
		m_nCurSelHtmFile = nSel;
		TRACE(_T("<%s> %d %d\n"), (LPCTSTR)sText, nSel, pCombo->GetCount());

		sPathFile = m_sHtmPath + _T("\\") + sText;
		theApp.OpenDocumentFile(sPathFile);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (m_bInOnTimer)
	{
		return;
	}
	m_bInOnTimer = TRUE;
	if (m_uTimerID == nIDEvent)
	{
		ResetComboBoxLog();
		ResetComboBoxHtm();
	}

	if (m_uUpdateTimerID == nIDEvent)
	{
		//m_hWndMDIClient - undocumented
		CFrameWnd *pAF =GetActiveFrame();
		if (pAF)
		{
			CView *pActiveView = pAF->GetActiveView();
			if (pActiveView && pActiveView->IsKindOf(RUNTIME_CLASS(CLogViewView)))
			{
				pActiveView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_REFRESH,0), NULL);
			}

			HWND hwnd = ::GetWindow(m_hWndMDIClient, GW_CHILD);
			while (hwnd)
			{
				if (hwnd != pAF->m_hWnd)
				{
					CFrameWnd*pFrame = (CFrameWnd*)CWnd::FromHandle(hwnd);
					if (pAF != pFrame
						&& pFrame->IsWindowVisible())
					{
						pActiveView = pFrame->GetActiveView();
						if (pActiveView && pActiveView->IsWindowVisible() && pActiveView->IsKindOf(RUNTIME_CLASS(CLogViewView)))
						{
							pActiveView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_REFRESH,0), NULL);
						}
					}
				}
				hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
			}

		}
	}
	m_bInOnTimer = FALSE;
	CMDIFrameWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
CComboBox* CMainFrame::GetComboBoxLog()
{
	return (CComboBox*)m_wndDlgBar.GetDlgItem(IDC_COMBO_LOG_FILES);
}

/////////////////////////////////////////////////////////////////////////////
CComboBox* CMainFrame::GetComboBoxHtm()
{
	return (CComboBox*)m_wndDlgBar.GetDlgItem(IDC_COMBO_HTM_FILES);
}
/////////////////////////////////////////////////////////////////////////////
CDateTimeCtrl* CMainFrame::GetDateCtrl()
{
	return (CDateTimeCtrl*)m_wndDlgBar.GetDlgItem(IDC_SEARCH_DATE);
}
/////////////////////////////////////////////////////////////////////////////
CDateTimeCtrl* CMainFrame::GetTimeCtrl()
{
	return (CDateTimeCtrl*)m_wndDlgBar.GetDlgItem(IDC_SEARCH_TIME);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::Login()
{
	WK_TRACE_WARNING(_T("Test Login Start\n"));
	BOOL bRet = FALSE;			 

#ifdef NO_LOGIN
	WK_TRACE_WARNING(_T("Test NO_LOGIN\n"));
	bRet = TRUE;
#else
	CWK_Profile wkp;
	CUserArray ua;
	ua.Load(wkp);
	if (ua.GetSize()>0)
	{
		CSecID idUser(SECID_NO_ID);
		if (m_saCmdParam.GetCount() >= 2)
		{
			idUser = ua.IsValid(m_saCmdParam[0], m_saCmdParam[1]);
		}
		CWK_Dongle dongle;
		InternalProductCode ipc = dongle.GetProductCode();
		if (   (ipc != IPC_TOBS_RECEIVER)
			&& (ipc != IPC_DTS_RECEIVER)
			&& (idUser == SECID_NO_ID)
			)
		{
			COEMLoginDialog dlg(this, WK_ALLOW_LOGVIEW);
			if (IDOK==dlg.DoModal()) {
				bRet = TRUE;		 
			}
			else
				WK_TRACE_ERROR(_T("Login Failed\n"));
		}
		else
		{
			bRet = TRUE;
		}
	}
	else
	{
		bRet = TRUE;
	}
#endif
	
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetStatusText(const CString &sText)
{
//	if (theApp.m_nThreadID == GetCurrentThreadId())
	{
		m_wndStatusBar.SetWindowText(sText);
	}
}
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_LOG_VIEW, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnComboLogFilesViaAcc()
{
	CComboBox* pComboBox = GetComboBoxLog();

	pComboBox->ShowDropDown();
	pComboBox->SetFocus();
}
//////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnKeyboardChanged(WPARAM wParam, LPARAM lParam)
{
	int i, n = sizeof(indicators) / sizeof(indicators[0]);
	for (i=n-1; i>=0; i--)
	{
		if (ID_INDICATOR_EXT == indicators[i])
			break;
	}
	if (wParam)
	{
		CKBInput*pActive = (CKBInput*)wParam;
		GetStatusBar()->SetPaneText(i, pActive->GetShortName());
	}
	else
	{
		GetStatusBar()->SetPaneText(i, _T(""));
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnOpenFile(WPARAM, LPARAM)
{
	CCommandLineInfo cmdInfo;
	GetWindowText(cmdInfo.m_strFileName);
	SetLogPath(cmdInfo.m_strFileName);
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;

	theApp.ProcessShellCommand(cmdInfo);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileSaveLogfiles()
{
	CComboBox*pBox = GetComboBoxLog();
	OnSaveFiles(pBox, m_sLogPath, IDS_SAVE_LOG_FILES, ID_FILE_SAVE_LOGFILES);
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnSaveFiles(CComboBox*pBox, CString&sPath, UINT nIDHeadline, UINT nIDHeadline2)
{
	if (pBox && pBox->GetCount())
	{
		int i, nCount = pBox->GetCount();
		CString sSource;
		CStringArray FileNames;
		for (i=0; i<nCount; i++)
		{
			pBox->GetLBText(i, sSource);
			FileNames.Add(sSource);
		}
		SortStrings(FileNames, TRUE);
		CDlgLoadCompressed dlgLC;
		dlgLC.m_pFiles = &FileNames;
		dlgLC.m_sHeadLine.LoadString(nIDHeadline);
		dlgLC.DoModal();

		nCount = FileNames.GetCount();
		if (nCount)
		{
			COemFileDialog dlg(this);
			CString sOpen, sName, sExt, sOpenBtn;
			sName.LoadString(IDS_DIRECTORY);
			sExt = _T(".");

			sOpen.LoadString(nIDHeadline2);
			sOpenBtn.LoadString(IDS_SAVE);
			dlg.SetProperties(TRUE,sOpen,sOpenBtn);
			dlg.SetInitialDirectory(_T("C:"));
			dlg.AddFilter(sName, sExt); 
			if (dlg.DoModal() == IDOK)
			{
				CString str = dlg.GetPathname();
				CString sSource, sDest;
				CStringArray NotCopied;
				for (i=0; i<nCount; i++)
				{
					sSource = FileNames.GetAt(i);
					sDest = str + sSource;
					sSource = sPath + _T("\\") + sSource;
					if (!CopyFile(sSource, sDest, FALSE))
					{
						NotCopied.Add(FileNames.GetAt(i));
					}
				}
				if (NotCopied.GetCount())
				{
					dlgLC.m_pFiles = &NotCopied;
					dlgLC.m_sHeadLine.FormatMessage(AFX_IDP_FAILED_IO_ERROR_WRITE, _T(""));
					dlgLC.DoModal();
				}
			}
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileSaveLogfiles(CCmdUI *pCmdUI)
{
	CComboBox*pBox = GetComboBoxLog();
	if (pBox)
	{
		pCmdUI->Enable(pBox->GetCount() ? TRUE : FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileDeleteLogfiles()
{
	CComboBox*pBox = GetComboBoxLog();
	if (OnDeleteFiles(pBox, m_sLogPath, IDS_DELETE_LOGFILES, IDS_NOT_DELETED_LOGFILES))
	{
		ResetComboBoxLog();
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnDeleteFiles(CComboBox*pBox, CString&sPath, UINT nIDHeadline, UINT nIDNotDeleted)
{
	if (pBox && pBox->GetCount())
	{
		int i, nCount = pBox->GetCount();
		CString sSource;
		CStringArray FileNames;
		for (i=0; i<nCount; i++)
		{
			pBox->GetLBText(i, sSource);
			FileNames.Add(sSource);
		}
		SortStrings(FileNames, TRUE);
		CDlgLoadCompressed dlgLC;
		dlgLC.m_pFiles = &FileNames;
		dlgLC.m_sHeadLine.LoadString(nIDHeadline);
		dlgLC.DoModal();

		nCount = FileNames.GetCount();
		CStringArray NotDeleted;
		for (i=0; i<nCount; i++)
		{
			if (!DeleteFile(sPath + _T("\\") + FileNames.GetAt(i)))
			{
				NotDeleted.Add(FileNames.GetAt(i));
			}
		}
		if (NotDeleted.GetCount())
		{
			dlgLC.m_pFiles = &NotDeleted;
			dlgLC.m_sHeadLine.LoadString(nIDNotDeleted);
			dlgLC.DoModal();
		}
		if (nCount != 0 && nCount != NotDeleted.GetCount())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileDeleteLogfiles(CCmdUI *pCmdUI)
{
	// if there are any file to save,
	// there are also files to delete
	OnUpdateFileSaveLogfiles(pCmdUI);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileChangeLogPath()
{
	COemFileDialog dlg(this);
	CString sOpen, sName, sExt, sOpenBtn;
	sName.LoadString(IDS_DIRECTORY);
	sExt = _T(".");

	sOpen.LoadString(ID_FILE_CHANGE_LOG_PATH);
	sOpenBtn.LoadString(IDS_SAVE);
	dlg.SetProperties(TRUE,sOpen,sOpenBtn);
	dlg.SetInitialDirectory(m_sLogPath);
	dlg.AddFilter(sName, sExt); 
	if (dlg.DoModal() == IDOK)
	{
		m_sLogPath = dlg.GetPathname();
		if (m_sLogPath.GetAt(m_sLogPath.GetLength()-1) == _T('\\'))
		{
			m_sLogPath = m_sLogPath.Left(m_sLogPath.GetLength()-1);
		}
		ResetComboBoxLog();
	}
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileChangeLogPath(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::SetLogPath(CString&sLogPath)
{
	int nFind;
	m_sLogPath = sLogPath;
	nFind = m_sLogPath.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		m_sLogPath = m_sLogPath.Left(nFind);
	}
	nFind = m_sLogPath.Find(_T('\"'));
	if (nFind == 0)
	{
		m_sLogPath = m_sLogPath.Mid(nFind+1);
	}
	nFind = m_sLogPath.ReverseFind(_T('\"'));
	if (nFind != -1)
	{
		m_sLogPath = m_sLogPath.Left(nFind);
	}
	ResetComboBoxLog();
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewSyncTime()
{
	CFrameWnd *pAF =GetActiveFrame();
	if (pAF)
	{
		CView*pView = pAF->GetActiveView();
		if (pView && pView->IsKindOf(RUNTIME_CLASS(CLogViewView)))
		{
			CString sSyncLine;
			m_nDay = 0;
			m_nSec = 0;
			((CLogViewView*)pView)->OnViewSyncTime(m_nDay, m_nSec);
			if (m_nDay != 0 || m_nSec != 0)
			{
				HWND hwnd = ::GetWindow(m_hWndMDIClient, GW_CHILD);
				while (hwnd)
				{
					if (hwnd != pAF->m_hWnd)
					{
						pView = ((CFrameWnd*)CWnd::FromHandle(hwnd))->GetActiveView();
						if (pView && pView->IsKindOf(RUNTIME_CLASS(CLogViewView)))
						{
							if (m_bSyncCompressed)
							{
								if (((CLogViewView*)pView)->GetDocument()->SelectCompressedFileByTime((CLogViewView*)pView, m_nDay, m_nSec))
								{
									m_SyncViews.AddTail(pView);
								}
							}
							else if (((CLogViewView*)pView)->IsTimeAvailable(m_nDay, m_nSec))
							{
								m_SyncViews.AddTail(pView);
							}

						}
					}
					hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
				}
				if (m_SyncViews.GetCount())
				{
					m_pSyncWindowsThread = AfxBeginThread(SyncWindowThread, (LPVOID)this);
					m_pSyncWindowsThread->m_bAutoDelete = TRUE;
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
UINT CMainFrame::SyncWindowThread(LPVOID pParam )
{
	CMainFrame*pThis = (CMainFrame*) pParam;
	POSITION pos = pThis->m_SyncViews.GetHeadPosition();
	CView*pView;
	while (pos)
	{
		pView = (CView*)pThis->m_SyncViews.GetNext(pos);
		((CLogViewView*)pView)->OnViewSyncTime(pThis->m_nDay, pThis->m_nSec);
		if (pThis->IsSyncWindowThreadRunning() == 2)
		{
			break;
		}
	}
	pThis->m_SyncViews.RemoveAll();
	pThis->m_pSyncWindowsThread = NULL;
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewSyncTime(CCmdUI *pCmdUI)
{
	CFrameWnd *pAF =GetActiveFrame();
	if (pAF)
	{
		CView*pView = pAF->GetActiveView();
		if (pView && pView->IsKindOf(RUNTIME_CLASS(CLogViewView)))
		{
			pCmdUI->Enable(!IsSyncWindowThreadRunning());
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::IsSyncWindowThreadRunning()
{
	if (m_pSyncWindowsThread)
	{
		return m_nDay ? TRUE : 2;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::StopSyncWindowThread()
{
	m_nDay = 0;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewUpdateAutomatically(CCmdUI *pCmdUI)
{
	OnUpdateViewSyncTime(pCmdUI);	// is a logview window open
	pCmdUI->SetCheck(m_uUpdateTimerID != 0);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewJumpToEnd(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_bScrollToEndAtRefresh);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy()
{
	if (m_uTimerID != 0) 
	{
		KillTimer(m_uTimerID);
	}
	if (m_uUpdateTimerID!= 0) 
	{
		KillTimer(m_uUpdateTimerID);
	}
	CMDIFrameWnd::OnDestroy();
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewUpdateAutomatically()
{
	if (m_uUpdateTimerID == 0)
	{
		m_uUpdateTimerID = SetTimer(2, 2000, NULL);
	}
	else
	{
		KillTimer(m_uUpdateTimerID);
		m_uUpdateTimerID = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewJumpToEnd()
{
	m_bScrollToEndAtRefresh = !m_bScrollToEndAtRefresh;
}

long CMainFrame::toDays(const CTime& date)
{
    return (date.GetYear() - 2000) * 365 + (date.GetMonth() - 1) * 31 + date.GetDay();
}

void CMainFrame::fromDays(long days, CTime& date)
{
    int year = days / 365;
    days -= year * 365;
    year += 2000;
    int month = days / 31;
    days  -= month * 31;
    month += 1;
    CTime t(year, month, days, 0, 0, 0);
    date = t;
}

long CMainFrame::toSeconds(const CTime& time)
{
    return time.GetHour() * 3600000 + time.GetMinute() * 60000 + time.GetSecond()*1000;
}

void CMainFrame::fromSeconds(long seconds, CTime& time)
{
    int hours = seconds / 3600000;
    seconds -= hours * 3600000;
    int minutes = seconds / 60000;
    seconds -= minutes * 60000;
    CTime date = CTime::GetCurrentTime();
    CTime t(date.GetYear(), date.GetMonth(), date.GetDay(), hours, minutes, seconds/1000);
    time = t;
}

//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditSearchTimeValue(CView*pView)
{
	CDateTimeCtrl*pDateCtrl = GetDateCtrl();
	CDateTimeCtrl*pTimeCtrl	= GetTimeCtrl();
	if (pDateCtrl && pTimeCtrl)
	{
		CTime date, time;
		pDateCtrl->GetTime(date);
		m_nDay = toDays(date);

		pTimeCtrl->GetTime(time);
        m_nSec = toSeconds(time);

		m_SyncViews.AddTail(pView);
		m_pSyncWindowsThread = AfxBeginThread(SyncWindowThread, (LPVOID)this);
		m_pSyncWindowsThread->m_bAutoDelete = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnWindowSyncCompressed()
{
	m_bSyncCompressed = !m_bSyncCompressed;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateWindowSyncCompressed(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_bSyncCompressed);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileSaveProtocolls()
{
	CComboBox*pBox = GetComboBoxHtm();
	OnSaveFiles(pBox, m_sHtmPath, IDS_SAVE_PROTOCOLLS, ID_FILE_SAVE_PROTOCOLLS);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileSaveProtocolls(CCmdUI *pCmdUI)
{
	CComboBox*pBox = GetComboBoxHtm();
	if (pBox)
	{
		pCmdUI->Enable(pBox->GetCount() ? TRUE : FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileDeleteProtocolls()
{
	CComboBox*pBox = GetComboBoxHtm();
	if (OnDeleteFiles(pBox, m_sHtmPath, IDS_DELETE_PROTOCOLLS, IDS_NOT_DELETED_PROTOCOLLS))
	{
		ResetComboBoxHtm();
	}
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileDeleteProtocolls(CCmdUI *pCmdUI)
{
	// if there are any file to save,
	// there are also files to delete
	OnUpdateFileSaveProtocolls(pCmdUI);
}
