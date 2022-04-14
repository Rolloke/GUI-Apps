// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "UpdateHandler.h"

#include "MainFrm.h"
#include "UpdateDoc.h"
#include "DirectoryView.h"
#include "FileView.h"
#include "ReportView.h"
#include "DirContent.h"
#include "UpdateHandlerInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUpdateHandler theApp;

////////////////////////////////////////////////////////////////
// CMyCoolBar
//
IMPLEMENT_DYNAMIC(CMyCoolBar, CCoolBar)

////////////////
// This is the virtual function you have to override to add bands
//
BOOL CMyCoolBar::OnCreateBands()
{
	// Create tool bar
	if (!m_wndToolBar.Create(this,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC) ||
		 !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
		TRACE(_T("Failed to create toolbar\n"));
		return FALSE; // failed to create
	}
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	// Following is not needed since I'm not using an image list
//	CRebarInfo rbi;
// ... set stuff in rbi...
//	SetBarInfo(&rbi);

	// Get minimum size of bands
	CSize szHorz = m_wndToolBar.CalcDynamicLayout(-1, 0);	      // get min horz size
	CSize szVert = m_wndToolBar.CalcDynamicLayout(-1, LM_HORZ);	// get min vert size

	// Band 1: Add toolbar band
	CRebarBandInfo rbbi;
	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_COLORS;
	rbbi.fStyle = 0;
	rbbi.hwndChild = m_wndToolBar;
	rbbi.cxMinChild = szHorz.cx;
	rbbi.cyMinChild = szVert.cy;
	rbbi.hbmBack = NULL;
	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	if (!InsertBand(0, &rbbi))
		return FALSE;

	return 0; // OK
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_COMMAND(ID_OPTIONS_CLEAR_REPORT, OnOptionsClearReport)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_PROGRESS,
	ID_CONNECTION,
};

static UINT titles[] =
{
	ID_DIRECTORY,
	ID_FILES,
	ID_ACTIONS
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pDirectoryView = NULL;
	m_pFileView = NULL;
	m_pReportView = NULL;
	m_nCountUp = 0;
	m_nLimit = 0;
	m_nCommand = 0;
}

CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create cool bar
	if (!m_wndToolBar.Create(this,
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) {
		TRACE(_T("Failed to create cool bar\n"));
		return -1;      // fail to create
	}

/*	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE(_T("Failed to create toolbar\n"));
		return -1;      // fail to create
	}*/

/*	m_wndUpdateBar.SetResourceID(IDR_UPDATE);
	if (!m_wndUpdateBar.Create(this,
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) {
		TRACE(_T("Failed to create cool bar\n"));
		return -1;      // fail to create
	}*/

/*	if (!m_wndUpdateBar.Create(this) ||
		!m_wndUpdateBar.LoadToolBar(IDR_UPDATE))
	{
		TRACE(_T("Failed to create toolbar\n"));
		return -1;      // fail to create
	}*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE(_T("Failed to create status bar\n"));
		return -1;      // fail to create
	}

	if (!m_wndTitleBar.Create(this,IDD_TITLE_BAR,WS_CHILD | WS_VISIBLE | CBRS_TOP,5))
	{
		TRACE(_T("Failed to create title bar\n"));
		return -1;      // fail to create
	}

	LoadBarState(_T("UpdateHandler"));

	SetWindowText(COemGuiApi::GetApplicationName(WAI_UPDATE_HANDLER));

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.lpszClass = WK_APP_NAME_UPDATE_HANDLER;
	cs.style &= ~FWS_ADDTOTITLE;

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
	case ID_INPUT_CONNECT:
		theApp.OnInputConnect();
		return TRUE;
	case ID_INPUT_DISCONNECT:
		theApp.OnInputDisconnect();
		return TRUE;
	case ID_CONFIRM_FILE_UPDATE:
		{
			CString* pString = (CString*)lParam; 
			CString sName = *pString;
			WK_DELETE(pString);
			theApp.OnConfirmFileUpdate(sName);
		}
		return TRUE;
	case ID_FIRST:
		{
			CRect rect;

			m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_PROGRESS),rect);
			m_wndProgress.Create(WS_VISIBLE|WS_CHILD,rect,&m_wndStatusBar,1);
		}
		return TRUE;
	case ID_CONNECT:
		theApp.Connect();
		return TRUE;
	case ID_TIMEOUT:
		theApp.OnTimeOut();
		return TRUE;
	case ID_DRIVES:
		theApp.OnDrives();
		return TRUE;
	case ID_TIME_ARRIVED:
		theApp.OnTimeArrived();
		return TRUE;
	case ID_DIRECTORY_ARRIVED:
		{
			CDirContent* pDirContent = (CDirContent*)(LPVOID)(lParam);
			if (   m_pReportView && m_pReportView->m_hWnd 
				&& !pDirContent->IsContinued())
			{
				CString sLine = _T("Inhalt von Directory gelesen: ");
				sLine += pDirContent->GetDirectory();
				m_pReportView->AddLine(sLine);
			}
			if (WK_IS_WINDOW(m_pDirectoryView))
			{
				m_pDirectoryView->InsertDirInfo(pDirContent);
			}
			if (WK_IS_WINDOW(m_pFileView))
			{
				m_pFileView->SetDirInfo(pDirContent);
			}
			CString sText;
			sText.LoadString(ID_DIRECTORY);
			sText += _T(' ') + pDirContent->GetDirectory();
			m_wndTitleBar.GetDlgItem(IDC_DIRECTORY)->SetWindowText(sText);
		}
		return TRUE;
	case ID_REG_ENUM_VALUES:
	case ID_REG_GET_VALUE:
		if (WK_IS_WINDOW(m_pFileView))
		{
			m_pFileView->InsertRegValue(theApp.GetInput()->m_RegInfo);
		}
		return TRUE;
	case ID_ERROR:
		theApp.OnError();
		return TRUE;
	case ID_FILE_EXECUTED:
		theApp.OnFileExecuted();
		return TRUE;
	case ID_FILE_DELETED:
		theApp.OnFileDeleted();
		return TRUE;
	case ID_FILE_GOT:
		theApp.OnFileGot();
		return TRUE;
	}

	if (wParam==ID_ADD_FILE)
	{
		CString dummy;
		LPWIN32_FIND_DATA pFFDPost = (LPWIN32_FIND_DATA)lParam;
		m_pFileView->AddFile(dummy,pFFDPost);
		free(pFFDPost);
		return TRUE;
	}
	
	if (wParam==ID_CLEAR_FILE)
	{
		m_pFileView->Clear();
		return TRUE;
	}
	return CFrameWnd::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
    // create a splitter with 1 row, 2 columns
    if (!m_wndSplitter.CreateStatic(this, 1, 3))
    {
        TRACE(_T("Failed to CreateStaticSplitter\n"));
        return FALSE;
    }

    // add the first splitter pane - the default view in column 0
    if (!m_wndSplitter.CreateView(0, 0,
        pContext->m_pNewViewClass, CSize(150, 0), pContext))
    {
        TRACE(_T("Failed to create first pane\n"));
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(0, 1,
        RUNTIME_CLASS(CFileView), CSize(200, 0), pContext))
    {
        TRACE(_T("Failed to create second pane\n"));
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(0, 2,
        RUNTIME_CLASS(CReportView), CSize(100, 0), pContext))
    {
        TRACE(_T("Failed to create third pane\n"));
        return FALSE;
    }

	m_pDirectoryView = (CDirectoryView*)m_wndSplitter.GetPane(0,0);
	m_pFileView = (CFileView*)m_wndSplitter.GetPane(0,1);
	m_pReportView = (CReportView*)m_wndSplitter.GetPane(0,2);

    // activate the input view
    SetActiveView((CView*)m_wndSplitter.GetPane(0, 0));

	return TRUE;
}
//////////////////////////////////////////////////////////////////
void CMainFrame::ActualizeTitleBarPaneSizes(CWnd*pWnd/*=NULL*/)
{
	CRect vr;
	CRect sr;
	int p;

	if (   m_wndTitleBar.m_hWnd
		&& WK_IS_WINDOW(m_pDirectoryView)
		&& WK_IS_WINDOW(m_pFileView)
		&& WK_IS_WINDOW(m_pReportView))
	{
//		if (m_pDirectoryView != pWnd)
		{
			m_pDirectoryView->GetWindowRect(vr);
			m_wndTitleBar.GetDlgItem(IDC_HOST)->GetClientRect(sr);
			sr.left = 3;
			p = sr.right = sr.left + vr.Width()-3;
			sr.top += 5;
			sr.bottom = sr.top + 16;
			m_wndTitleBar.GetDlgItem(IDC_HOST)->MoveWindow(sr);
		}
//		if (m_pFileView != pWnd)
		{
			m_pFileView->GetWindowRect(vr);
			sr.left = p+4;
			p = sr.right =  sr.left + vr.Width()-2;
			m_wndTitleBar.GetDlgItem(IDC_DIRECTORY)->MoveWindow(sr);
		}
//		if (m_pReportView != pWnd)
		{
			m_pReportView->GetWindowRect(vr);
			sr.left = p+5;
			sr.right =  sr.left + vr.Width();
			m_wndTitleBar.GetDlgItem(IDC_REPORT)->MoveWindow(sr);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	
	if (bShow)
	{
		ActualizeTitleBarPaneSizes();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddReportLine(const CString& sLine)
{
	if (WK_IS_WINDOW(m_pReportView))
	{
		m_pReportView->AddLine(sLine);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddDrives(const CString& sDrives)
{
	if (WK_IS_WINDOW(m_pDirectoryView))
	{
		CString sDir;
		CString sT;
		int p,ico;
		TCHAR c;

		sT = sDrives;
		p = sT.Find(_T("->"));
		if (p != -1)
		{
			ico = 7;
			sDir = sT.Left(p);
			sT = sT.Mid(p+2);
			sDir = REG_PATH_ID + sDir + _T("\\");
			HTREEITEM hInsert = m_pDirectoryView->AddDirectory(sDir,ico);
			CStringArray sa;
			SplitString(sT, sa, _T(","));
			ico = 0;
			int i, n = sa.GetCount();
			for (i=0; i<n; i++)
			{
				m_pDirectoryView->AddDirectory(sDir+sa.GetAt(i), ico);
			}
			m_pDirectoryView->GetTreeCtrl().Expand(hInsert, TVE_EXPAND);
			return;
		}
		p = sT.Find(_T(','));
		ico = -1;

		while (p!=-1)
		{
			sDir = sT.Left(p);
			sT = sT.Mid(p+1);
			if (sDir.GetLength()==4)
			{
				c = sDir[3];
				switch (c)
				{
				case _T('F'):
					ico = 1;
					break;
				case _T('H'):
					ico = 2;
					break;
				case _T('N'):
					ico = 3;
					break;
				case _T('C'):
					ico = 4;
					break;
				default:
					ico = -1;
				}
				if (ico != -1)
				{
					sDir = sDir.Left(3);
					m_pDirectoryView->AddDirectory(sDir,ico);
				}
			}
			p = sT.Find(_T(','));
		}

		if (ico != -1)
		{
			m_pDirectoryView->AddDirectory(sDir,ico);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CountDown(int nSek, UINT nCommand)
{
	m_nCommand = nCommand;
	if (nSek>0)
	{
		m_nLimit = nSek;
		m_nCountUp = 0;
		m_wndProgress.SetRange32(0,m_nLimit);
		m_wndProgress.SetPos(m_nCountUp);
		m_nTimerCountUpTimer = SetTimer(1, 1000, NULL);
	}
	else
	{
		KillTimer(m_nTimerCountUpTimer);
		m_nTimerCountUpTimer = 0;
		m_nLimit = 0;
		m_nCountUp = 0;
		m_wndProgress.SetPos(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimerCountUpTimer)
	{
		m_nCountUp++;
		m_wndProgress.SetPos(m_nCountUp);
		if (m_nCountUp==m_nLimit)
		{
			m_nLimit = 0;
			m_nCountUp = 0;
			m_wndProgress.SetPos(0);
			KillTimer(m_nTimerCountUpTimer);
			m_nTimerCountUpTimer = 0;
			if (m_nCommand)
			{
				PostMessage(WM_COMMAND,m_nCommand);
			}
		}
	}
	else if (nIDEvent == m_nIdleTimer)
	{
		theApp.OnIdle(1);
	}
	CFrameWnd::OnTimer(nIDEvent);
}
void CMainFrame::SetIdleTimer(BOOL bSet)
{
	if (bSet)
	{
		if (m_nIdleTimer == 0)
		{
			m_nIdleTimer = SetTimer(2, 200, NULL);
		}
	}
	else
	{
		if (m_nIdleTimer)
		{
			KillTimer(m_nIdleTimer);
			m_nIdleTimer = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// actualize the progress controls window rect
	if (m_wndStatusBar.m_hWnd && IsWindow(m_wndStatusBar.m_hWnd))
	{
		if (m_wndProgress.m_hWnd && IsWindow(m_wndProgress.m_hWnd))
		{
			CRect rect;

			m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_PROGRESS),rect);
			m_wndProgress.MoveWindow(rect);
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnOptionsClearReport() 
{
	if (WK_IS_WINDOW(m_pReportView))
	{
		m_pReportView->Clear();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetFileViewDirectory(const CString& sDir)
{
	if (WK_IS_WINDOW(m_pFileView))
	{
		CUpdateHandlerInput* pUHI = theApp.GetInput();
		if (pUHI)
		{
			POSITION pos;
			m_pFileView->SetDirInfo(pUHI->GetDirContent(sDir, pos));
			CString sText;
			sText.LoadString(ID_DIRECTORY);
			sText += _T(' ') + sDir;
			m_wndTitleBar.GetDlgItem(IDC_DIRECTORY)->SetWindowText(sText);
		}
		else
		{
			m_wndTitleBar.GetDlgItem(IDC_DIRECTORY)->SetWindowText(_T(""));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::RemoveFileFromFileView(const CString& sFile)
{
	if (WK_IS_WINDOW(m_pFileView))
	{
		m_pFileView->DeleteFile(sFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnClose() 
{
	SetIdleTimer(FALSE);
	if (m_nTimerCountUpTimer)
	{
		KillTimer(m_nTimerCountUpTimer);
		m_nTimerCountUpTimer = 0;
	}
	SaveBarState(_T("UpdateHandler"));
	
	CFrameWnd::OnClose();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ClearDirFileView()
{
	if (WK_IS_WINDOW(m_pFileView))
	{
		m_pFileView->Clear();
		SetFileViewDirectory(_T(""));
	}
	if (WK_IS_WINDOW(m_pDirectoryView))
	{
		m_pDirectoryView->Clear();
	}
}
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_UPDATE_HANDLER, 0);

	return 0;
}
