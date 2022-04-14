// TestUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUnit.h"
#include "TestUnitDlg.h"
#include "..\HAccDll\DirectAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CTestUnitDlg dialog

CTestUnitDlg::CTestUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestUnitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestUnitDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CTestUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ScanPCIBus, OnScanPCIBus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestUnitDlg message handlers

BOOL CTestUnitDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestUnitDlg::OnPaint() 
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
HCURSOR CTestUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#define	TASHA_VENDOR_ID			0x11D4
#define TASHA_DEVICE_ID			0x1535
#define TASHA_SUBSYSTEM_ID1		0x1700

void CTestUnitDlg::OnScanPCIBus() 
{
	CString sText;
	DWORD dwTC		= 0;
	DWORD dwLen		= 50*1024;
	DWORD dwCount	= 1000;

	CPCIBus pci;	
	CDeviceList* pDeviceList = pci.ScanBus();

	GetDlgItem(IDC_ScanPCIBus)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	if (pDeviceList)
	{
		pDeviceList->Trace();

		CPCIDevice*	 pDevice	= pDeviceList->GetDevice(TASHA_VENDOR_ID, TASHA_DEVICE_ID);
		if (pDevice)
		{
			DWORD dwMemBasePhys	= pDevice->GetMemBase(0) & 0xfffffff0;
			if (dwMemBasePhys)
			{
				DWORD dwMemBaseLin = HACCMapPhysToLin(dwMemBasePhys, dwLen);
				if (dwMemBaseLin)
				{
					BYTE* pBuffer = new BYTE[dwLen];

					if (pBuffer)
					{
						CIoMemory IoMem(dwMemBaseLin);
	
						// Transfertest!

						for (DWORD dwI = 0; dwI < dwLen; dwI++)
							pBuffer[dwI] = 0xaa; //(BYTE)dwI;
						
						//........................................
						dwTC = GetTickCount();
						for (DWORD dwJ=0; dwJ < dwCount; dwJ++)
							IoMem.OutputX(pBuffer, 0x100, dwLen);

						sText.Format("%lu KByte/s", dwLen*dwCount/(GetTickCount() - dwTC)*1000/1024);
						GetDlgItem(IDC_STATIC_RATE_PCTOBF535)->SetWindowText(sText);
						
						//........................................
						ZeroMemory(pBuffer, dwLen);					
						dwTC = GetTickCount();
						for (DWORD dwK=0; dwK < dwCount; dwK++)
							IoMem.InputX(pBuffer, 0x100, dwLen);

						sText.Format("%lu KByte/s", dwLen*dwCount/(GetTickCount() - dwTC)*1000/1024);
						GetDlgItem(IDC_STATIC_RATE_BF535TOPC)->SetWindowText(sText);
						
						delete pBuffer;
						pBuffer = NULL;
					}

					if (!HACCUnmapPhysToLin(dwMemBaseLin, dwLen))
						TRACE("HACCUnmapPhysToLin failed\n");
				}
			}
		}
	}
	GetDlgItem(IDC_ScanPCIBus)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
}
