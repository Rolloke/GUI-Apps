// CRTErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DVStarter.h"
#include "CRTErrorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDVStarterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRTErrorDlg dialog


/////////////////////////////////////////////////////////////////////////////
CRTErrorDlg::CRTErrorDlg(CWK_RunTimeError* pLastError)
	: CTransparentDialog(CRTErrorDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CRTErrorDlg)
	//}}AFX_DATA_INIT
	m_sErrorText		= _T("");
	m_pCountdownThread	= NULL;
	m_bRun				= FALSE;
	m_bEnableCountdown	= TRUE;
	m_pLastError		= pLastError;
	m_hCursor			= NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CRTErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTErrorDlg)
	DDX_Control(pDX, IDC_DISPLAY_BORDER, m_ctrlBorderRTError);
	DDX_Control(pDX, IDOK, m_ctrlButtonOk);
	DDX_Control(pDX, IDC_RTERROR_DISPLAY, m_ctrlDisplayRTError);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRTErrorDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CRTErrorDlg)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTErrorDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CRTErrorDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();

	// 29.04.05 ML: In OpenStockTransparent wird eine transparente Region erzeugt/geladen. Dies
	// ist bei einigen Grafikkarten extrem langsam und bremst den ganzen Rechner aus. Deshalb
	// wird nun anstatt des transparenten Schattens der Farbverlauf des Pannels (in grün)
	// verwendet.
#if (0)
	OpenStockTransparent(BackGround3);
#else	
	CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREEN);
#endif
	m_ctrlDisplayRTError.OpenStockDisplay(this, Display2);

	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText = RGB(0,0,0);
	DisplayColors.dwFlags = DSP_COL_TEXT;
	m_ctrlDisplayRTError.SetDisplayColors(DisplayColors);

	m_ctrlBorderRTError.CreateFrame(this);
	m_ctrlBorderRTError.EnableShadow(TRUE);

	m_ctrlDisplayRTError.SetTextAllignment(DT_LEFT|DT_WORDBREAK);
	m_ctrlDisplayRTError.SetDisplayText(m_sErrorText);

	m_ctrlButtonOk.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
	m_ctrlButtonOk.SetSurface(Surface3DPlaneEdge5);

	m_ilConfirm.Create(IDB_CONFIRM, 16, 0, RGB(255,0,255));
	m_ctrlButtonOk.SetImageList(&m_ilConfirm);

	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE);

	if(    m_pLastError  
		&& m_pLastError->GetError() == RTE_HARDDISK_FALURE
		&& m_pLastError->GetLevel() == REL_REBOOT_ERROR)
	{
		//harddisk with pagefile failed
		BOOL bConfirmed = FALSE; //save as not confirmes RTE

		CString sNextDriveToSaveRTEFile = theApp.GetNextAvailableFixedDrive(); // e.g.: "e:"
		//Ordner "HDDfailed" anlegen
		CString sHDDFailedPath = sNextDriveToSaveRTEFile+_T("\\")+RTE_HDDFAILED_PATH;
		CreateDirectory(sHDDFailedPath, NULL);

		//save RTEFile on next fixed HDD to make sure that the RTE is definitly saved
		AddRTEToRTEFile(bConfirmed, sHDDFailedPath);
		//Sleep(100);
		//Beep(2000, 1000);
	}

	if (m_bEnableCountdown)
		StartCountdownThread();

	m_hCursor = theApp.LoadCursor(IDC_ARROW4);
	if (!m_hCursor)
		WK_TRACE_ERROR(_T("Can't load cursor\n"));

	AfxGetMainWnd()->PostMessage(WM_CHECK_RTE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CRTErrorDlg::OnOK() 
{
	m_bRun = FALSE;
	
	if (m_pCountdownThread)
		WaitForSingleObject(m_pCountdownThread->m_hThread, 1000);

	BOOL bConfirmed = TRUE;
	AddRTEToRTEFile(bConfirmed);
	DestroyWindow();
	
	AfxGetMainWnd()->PostMessage(WM_CHECK_RTE);
}

/////////////////////////////////////////////////////////////////////////////
void CRTErrorDlg::OnCancel() 
{
	m_bRun = FALSE;
	
	if (m_pCountdownThread)
		WaitForSingleObject(m_pCountdownThread->m_hThread, 1000);

	BOOL bConfirmed = FALSE;
	AddRTEToRTEFile(bConfirmed);
	DestroyWindow();

	AfxGetMainWnd()->PostMessage(WM_CHECK_RTE);
}	
/////////////////////////////////////////////////////////////////////////////
void CRTErrorDlg::AddRTEToRTEFile(BOOL bConfirmed, CString sPathToSaveRTEFile /* = "" */)
{

	if (m_pLastError)
	{
		CString sTimeStamp	= m_pLastError->GetFormattedTimeStamp();
		CString sErrorText	= m_pLastError->GetErrorText();

		if(bConfirmed)
		{
			// Runtimeerror quittieren.
			m_pLastError->SetErrorText(sErrorText + RTE_CONFIRMED);
			WK_TRACE(_T("Runtimeerror <%s> was confirmed\n"), sTimeStamp + _T(" ") + sErrorText);
		}
		else
		{
			WK_TRACE(_T("Runtimeerror <%s> was not confirmed\n"), sTimeStamp + _T(" ") + sErrorText);
		}


		CWK_RunTimeErrors rtErrors;

		//maybe we have to save the RTE into a RTEFile on a different HDD
		//in case of HDD including the pagefile failed
		//if not, save to the default path (theApp.GetHomeDir())
		if(sPathToSaveRTEFile.IsEmpty())
		{
			sPathToSaveRTEFile = theApp.GetHomeDir();
		}

		rtErrors.LoadFromFile(sPathToSaveRTEFile+_T("\\")+RTE_FILENAME);
		rtErrors.SafeAdd(m_pLastError);

		// maximal MAX_RTE Runtimeerrors zulassen.
		int nSize = rtErrors.GetSize();
		if (rtErrors.GetSize() > MAX_RTE)
		{
			for (int nI = 0; nI < nSize - MAX_RTE; nI++)
				rtErrors.RemoveAt(0);
		}		
		
		//save the RTE to RTEFile
		rtErrors.WriteToFile(sPathToSaveRTEFile+_T("\\")+RTE_FILENAME);
		rtErrors.SafeDeleteAll();
	}

}
/////////////////////////////////////////////////////////////////////////////
void CRTErrorDlg::StartCountdownThread()
{
	if (m_pCountdownThread == NULL)
	{
		m_pCountdownThread = AfxBeginThread(CountdownThread, this);
		if (m_pCountdownThread)
		{
			m_pCountdownThread->m_bAutoDelete = TRUE;
			m_pCountdownThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			m_bRun = TRUE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
UINT CRTErrorDlg::CountdownThread(LPVOID pData)
{
	CRTErrorDlg* pThis = (CRTErrorDlg*)pData;

	DWORD dwTimeOut = pThis->m_sErrorText.GetLength()*1000/2;
	DWORD dwTC		= GetTickCount();
	do
	{
		Sleep(100);
	}while ((GetTimeSpan(dwTC) < dwTimeOut) && pThis->m_bRun);

	if (pThis->m_bRun)
	{
		pThis->PostMessage(WM_COMMAND, MAKEWORD(IDCANCEL, BN_CLICKED), NULL); 
	}

	pThis->m_pCountdownThread = NULL;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRTErrorDlg::Create()
{
	if (!m_pLastError)
		return FALSE;
	
	CString sTimeStamp	= m_pLastError->GetFormattedTimeStamp();
	CString sErrorText	= m_pLastError->GetErrorText();

	m_sErrorText = sTimeStamp + _T("\n") + sErrorText;

	if (!CTransparentDialog::Create(IDD))
	{
		return FALSE;
	}
	ShowWindow(SW_SHOW);
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CRTErrorDlg::PostNcDestroy() 
{
	TRACE(_T("deleting this = %08lx in PostNcDestroy\n"),(DWORD)this);
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRTErrorDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
		else
			WK_TRACE_ERROR(_T("SetCursur failed\n"));
	}	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}
