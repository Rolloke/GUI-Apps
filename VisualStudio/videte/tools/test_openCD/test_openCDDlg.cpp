// test_openCDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test_openCD.h"
#include "test_openCDDlg.h"
#include "Winioctl.h"


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
// CTest_openCDDlg dialog

CTest_openCDDlg::CTest_openCDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTest_openCDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTest_openCDDlg)
	m_OpenStatus = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest_openCDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTest_openCDDlg)
	DDX_Text(pDX, IDC_OPEN_STATUS, m_OpenStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTest_openCDDlg, CDialog)
	//{{AFX_MSG_MAP(CTest_openCDDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTest_openCDDlg message handlers

BOOL CTest_openCDDlg::OnInitDialog()
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

void CTest_openCDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTest_openCDDlg::OnPaint() 
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
HCURSOR CTest_openCDDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTest_openCDDlg::OnButton1() 
{
	InitDebugger("Test_OpenCD.log",WAI_DV_CLIENT);
	// TODO: Add your control notification handler code here
	CWK_Profile tes;
	
	LockCD();
	CloseDebugger();
}
/////////////////////////////////////////////////////////////////////////////
void CTest_openCDDlg::OnButton2() 
{
	InitDebugger("Test_OpenCD.log",WAI_DV_CLIENT);
	// TODO: Add your control notification handler code here
	CWK_Profile tes;
	
	UnlockCD();
	CloseDebugger();
	
}
/////////////////////////////////////////////////////////////////////////////
void CTest_openCDDlg::LockCD()
{

  DISK_GEOMETRY pdg;            // disk drive geometry structure
  BOOL bResult;                 // generic results flag

  bResult = LockUnlockDrive (&pdg, TRUE, "G");

  if (bResult) 
  {
    SetDlgItemText(IDC_OPEN_STATUS, "CD-Drive G:\\ locked successfully");
  } 
  else 
  {
	  CString sError("CD-Drive G:\\ not locked. -> Error %ld.\n",GetLastError ());
	 SetDlgItemText(IDC_OPEN_STATUS,sError);
  }
}

/////////////////////////////////////////////////////////////////////////////
void CTest_openCDDlg::UnlockCD()
{

  DISK_GEOMETRY pdg;            // disk drive geometry structure
  BOOL bResult;                 // generic results flag

  bResult = LockUnlockDrive (&pdg, FALSE, "G");

  if (bResult) 
  {
    SetDlgItemText(IDC_OPEN_STATUS, "CD-Drive G:\\ unlocked successfully");
  } 
  else 
  {
	  CString sError("CD-Drive G:\\ not unlocked. -> Error %d.\n",GetLastError ());
	 SetDlgItemText(IDC_OPEN_STATUS,sError);
  }
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTest_openCDDlg::LockUnlockDrive(DISK_GEOMETRY *pdg, BOOL bLock, CString sDriveLetter)
{

  HANDLE hDevice;               // handle to the drive to be examined 
  BOOL bResult;                 // results flag
  CString sDrive;
  sDrive = "\\\\.\\" + sDriveLetter + ":";

  hDevice = CreateFile(sDrive, // drive to open
                       GENERIC_READ,
                       0,  // share mode
                       NULL,    // default security attributes
                       OPEN_EXISTING,  // disposition
                       0,//0,       // file attributes
                       0);   // don't copy any file's attributes

  if (hDevice == INVALID_HANDLE_VALUE) // we can't open the drive
  {
    return (FALSE);
  }

	PREVENT_MEDIA_REMOVAL Lock;
	Lock.PreventMediaRemoval = bLock;
	DWORD nInBufferSize = sizeof(Lock);
	DWORD dwBytesReturned = 0;


	bResult = DeviceIoControl(hDevice,
							  IOCTL_STORAGE_MEDIA_REMOVAL,
							  &Lock,
							  nInBufferSize,  
							  NULL,                      
							  0,                           // nOutBufferSize
							  &dwBytesReturned,   // number of bytes returned
							  NULL); // OVERLAPPED structure

  CloseHandle(hDevice);         // we're done with the handle

  return (bResult);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTest_openCDDlg::GetDriveGeometry(DISK_GEOMETRY *pdg)
{

  HANDLE hDevice;               // handle to the drive to be examined 
  BOOL bResult;                 // results flag
  DWORD junk;                   // discard results

  hDevice = CreateFile("\\\\.\\\\.\\G:", // drive to open
                       0,       // don't need any access to the drive
                       FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
                       NULL,    // default security attributes
                       OPEN_EXISTING,  // disposition
                       0,       // file attributes
                       NULL);   // don't copy any file's attributes

  if (hDevice == INVALID_HANDLE_VALUE) // we can't open the drive
  {
    return (FALSE);
  }

  bResult = DeviceIoControl(hDevice,  // device we are querying
      IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
                             NULL, 0, // no input buffer, so pass zero
                            pdg, sizeof(*pdg),  // output buffer
                            &junk, // discard count of bytes returned
                            (LPOVERLAPPED) NULL);  // synchronous I/O

  CloseHandle(hDevice);         // we're done with the handle

  return (bResult);
}



