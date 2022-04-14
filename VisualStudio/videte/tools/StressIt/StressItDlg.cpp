/////////////////////////////////////////////////////////////////////////////
// PROJECT:		StressIt
// FILE:		$Workfile: StressItDlg.cpp $
// ARCHIVE:		$Archive: /Project/Tools/StressIt/StressItDlg.cpp $
// CHECKIN:		$Date: 5.03.04 8:56 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 5.03.04 8:45 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressIt.h"
#include "StressItDlg.h"
#include "math.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT StressSleepThread(LPVOID pData);
UINT StressWriteThread(LPVOID pData);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStressItDlg dialog

CStressItDlg::CStressItDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStressItDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStressItDlg)
	m_sAvailPhysMemory	= _T("");
	m_sAvailPageMemory	= _T("");
	m_sUsedPhysMemory	= _T("");
	m_sUsedPageMemory	= _T("");
	m_sMemoryLoad		= _T("");
	m_sAllocMemory		= _T("");
	m_sAvailSumme = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_dwAllocMemory		= 0;
	m_nSleepSliderPos	= 0;
	m_nWriteSliderPos	= 0;
	m_pStressSleepThread= NULL;
	m_pStressWriteThread= NULL;

	// Sleep Thread anlegen
	m_pStressSleepThread = AfxBeginThread(StressSleepThread, this);
	if (m_pStressSleepThread)
	if (m_pStressSleepThread)
		m_pStressSleepThread->m_bAutoDelete = FALSE;
	
	if (m_pStressSleepThread)
		m_pStressSleepThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);


	// Write Thread anlegen
	m_pStressWriteThread = AfxBeginThread(StressWriteThread, this);
	if (m_pStressWriteThread)
	if (m_pStressWriteThread)
		m_pStressWriteThread->m_bAutoDelete = FALSE;
	
	if (m_pStressWriteThread)
		m_pStressWriteThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
}

void CStressItDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStressItDlg)
	DDX_Control(pDX, IDC_SLIDER_WRITE, m_WriteSlider);
	DDX_Control(pDX, IDC_SLIDER_SLEEP, m_SleepSlider);
	DDX_Text(pDX, IDC_AVAIL_PHYSICAL,	m_sAvailPhysMemory);
	DDX_Text(pDX, IDC_AVAIL_PAGEFILE,	m_sAvailPageMemory);
	DDX_Text(pDX, IDC_USED_PHYSICAL,	m_sUsedPhysMemory);
	DDX_Text(pDX, IDC_USED_PAGEFILE,	m_sUsedPageMemory);
	DDX_Text(pDX, IDC_MEMORY_LOAD,		m_sMemoryLoad);
	DDX_Text(pDX, IDC_ALLOC_MEMORY,		m_sAllocMemory);
	DDX_Text(pDX, IDC_AVAIL_SUMME, m_sAvailSumme);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStressItDlg, CDialog)
	//{{AFX_MSG_MAP(CStressItDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_01MB, OnAllocate01MB)
	ON_BN_CLICKED(IDC_100MB, OnAllocate100MB)
	ON_BN_CLICKED(IDC_10MB, OnAllocate10MB)
	ON_BN_CLICKED(IDC_1MB, OnAllocate1MB)
	ON_BN_CLICKED(IDC_50MB, OnAllocate50MB)
	ON_BN_CLICKED(IDC_RELEASE_ALL, OnReleaseAll)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_THREAD_PRIORITY1, OnSetThreadPriorityHigh)
	ON_BN_CLICKED(IDC_THREAD_PRIORITY2, OnSetThreadPriorityMid)
	ON_BN_CLICKED(IDC_THREAD_PRIORITY3, OnSetThreadPriorityLow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStressItDlg message handlers

BOOL CStressItDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_SleepSlider.SetLineSize(25);
	m_SleepSlider.SetPageSize(250);
	m_SleepSlider.SetRangeMin(0, TRUE);
	m_SleepSlider.SetRangeMax(2500, TRUE);

	m_WriteSlider.SetLineSize(1000);
	m_WriteSlider.SetPageSize(10000);
	m_WriteSlider.SetRangeMin(0, TRUE);
	m_WriteSlider.SetRangeMax(100000, TRUE);
	
	SetWindowPos(&wndTopMost,0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	CheckRadioButton(IDC_THREAD_PRIORITY1, IDC_THREAD_PRIORITY3, IDC_THREAD_PRIORITY2);

	UpdateDialog();
	SetTimer(1, 500, NULL);
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStressItDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStressItDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStressItDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////////////////////////
BOOL CStressItDlg::DestroyWindow() 
{
	m_bRun = FALSE;

	// Warte bis 'StressSleepThread' beendet ist
	if (m_pStressSleepThread)
		WaitForSingleObject(m_pStressSleepThread->m_hThread, 5000);
	delete m_pStressSleepThread; //Autodelete = FALSE;
	m_pStressSleepThread = NULL;

	// Warte bis 'StressWriteThread' beendet ist
	if (m_pStressWriteThread)
		WaitForSingleObject(m_pStressWriteThread->m_hThread, 5000);
	delete m_pStressWriteThread; //Autodelete = FALSE;
	m_pStressWriteThread = NULL;
	
	KillTimer(1);
	
	OnReleaseAll();
	
	return CDialog::DestroyWindow();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnTimer(UINT nIDEvent) 
{
	UpdateDialog();
	
	CDialog::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnAllocate01MB() 
{
	if (AllocateIt(1024 * 1024 / 10))
	{
		UpdateDialog();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnAllocate1MB() 
{
	if (AllocateIt(1024 * 1024))
	{
		UpdateDialog();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnAllocate10MB() 
{
	if (AllocateIt(1024 * 1024 * 10))
	{
		UpdateDialog();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnAllocate50MB() 
{
	if (AllocateIt(1024 * 1024 * 50))
	{
		UpdateDialog();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnAllocate100MB() 
{
	if (AllocateIt(1024 * 1024 * 100))
	{
		UpdateDialog();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
BOOL CStressItDlg::AllocateIt(DWORD dwSize)
{
	char *pMem;

	pMem = new char[dwSize];
	if (!pMem)
		return FALSE;

	memset(pMem, 65, dwSize);

	m_List.AddHead(pMem);
	m_dwAllocMemory	 += dwSize;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnReleaseAll() 
{
	for(POSITION pos = m_List.GetHeadPosition(); pos != NULL; )
	{
		delete m_List.GetNext(pos);
		UpdateDialog();
	}
	m_List.RemoveAll();

	m_dwAllocMemory = 0;
	UpdateDialog();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CStressItDlg::UpdateDialog()
{
	GlobalMemoryStatus(&m_Mem);

	DWORD dwFree = m_Mem.dwAvailPageFile + m_Mem.dwAvailPhys;

	if (dwFree > 1024 * 1024 * 100)
		GetDlgItem(IDC_100MB)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_100MB)->EnableWindow(FALSE);

	if (dwFree > 1024 * 1024 * 50)
		GetDlgItem(IDC_50MB)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_50MB)->EnableWindow(FALSE);

	if (dwFree > 1024 * 1024 * 10)
		GetDlgItem(IDC_10MB)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_10MB)->EnableWindow(FALSE);
	
	if (dwFree > 1024 * 1024 * 1)
		GetDlgItem(IDC_1MB)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_1MB)->EnableWindow(FALSE);
	
	if (dwFree > 1024 * 1024 / 10)
		GetDlgItem(IDC_01MB)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_01MB)->EnableWindow(FALSE);
	
	
	if (m_dwAllocMemory	!= 0)
		GetDlgItem(IDC_RELEASE_ALL)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_RELEASE_ALL)->EnableWindow(FALSE);
	
	m_sAllocMemory.Format(_T("%lu KBytes"), m_dwAllocMemory / 1024);

	m_sAvailPhysMemory.Format(_T("%lu KBytes"), m_Mem.dwAvailPhys	  / 1024);
	m_sAvailPageMemory.Format(_T("%lu KBytes"), m_Mem.dwAvailPageFile / 1024);
	m_sAvailSumme.Format(_T("%lu KBytes"), dwFree / 1024);

	m_sUsedPhysMemory.Format(_T("%lu KBytes"), (m_Mem.dwTotalPhys		- m_Mem.dwAvailPhys)	 / 1024);
	m_sUsedPageMemory.Format(_T("%lu KBytes"), (m_Mem.dwTotalPageFile	- m_Mem.dwAvailPageFile) / 1024);

	m_sMemoryLoad.Format(_T("%lu %%"), m_Mem.dwMemoryLoad);

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (pScrollBar->GetDlgCtrlID())
	{
		case IDC_SLIDER_SLEEP:
			m_nSleepSliderPos = m_SleepSlider.GetPos();
			break;

		case IDC_SLIDER_WRITE:
			m_nWriteSliderPos = m_WriteSlider.GetPos();
			break;
		default:
			break;
	
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


/////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnSetThreadPriorityHigh() 
{
	if (m_pStressSleepThread)
		m_pStressSleepThread->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
}

/////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnSetThreadPriorityMid() 
{
	if (m_pStressSleepThread)
		m_pStressSleepThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
}

/////////////////////////////////////////////////////////////////////////////
void CStressItDlg::OnSetThreadPriorityLow() 
{
	if (m_pStressSleepThread)
		m_pStressSleepThread->SetThreadPriority(THREAD_PRIORITY_LOWEST);
}

/////////////////////////////////////////////////////////////////////////////
UINT StressSleepThread(LPVOID pData)
{
	CStressItDlg*	pDlg	 = (CStressItDlg*)pData;  // Pointer auf's MainDialog

	if (!pDlg)
		return 0;

	// Warte bis MainProzeß initialisiert ist.
	while (!pDlg->m_bRun)
	{
		Sleep(10);
	}
	
	double fRes1 = 0.0;
	double fRes2 = 0.0;
	double fRes3 = 0.0;

	// Poll bis Mainprozeß terminiert werden soll.
	while (pDlg->m_bRun)
	{

		for (int nI = 0; nI < pDlg->m_nSleepSliderPos*20; nI++)
		{
			for (int nJ = 0; nJ < 750; nJ++)
			{
				fRes1 += sin((double)(nJ+nI))*cos((double)(nJ*nI))*sin((double)(nJ+nI))*cos((double)(nJ*nI));
				fRes2 += sin((double)(nJ-nI))*cos((double)(nJ*nJ))*sin((double)(nJ+nI))*cos((double)(nJ*nJ));
				fRes3 += sin((double)(nJ-nI))*cos((double)(nI*nI))*sin((double)(nJ-nI))*cos((double)(nI*nI));			
			}
		}
		Sleep(1);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT StressWriteThread(LPVOID pData)
{
	CStressItDlg* pDlg  = (CStressItDlg*)pData;  // Pointer auf's MainDialog
	char*	pBuffer		= NULL;;

	if (!pDlg)
		return 0;

	// Warte bis MainProzeß initialisiert ist.
	while (!pDlg->m_bRun)
	{
		Sleep(10);
	}

	CFile File(_T("Data.tmp"), CFile::modeCreate |CFile::modeWrite);
	pBuffer = new char[1000000];

	// Poll bis Mainprozeß terminiert werden soll.
	while (pDlg->m_bRun)
	{
		if (pDlg->m_nWriteSliderPos > 0)
		{
			File.Write(pBuffer, pDlg->m_nWriteSliderPos);
			File.Flush();
			File.SeekToBegin();
		}
		Sleep(50);
	}

	delete pBuffer;
	pBuffer = NULL;

	return 0;
}

