// DCClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DCClient.h"
#include "DCClientDlg.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static _TCHAR BASED_CODE szFilter[] = _T("All Files (*.*)|*.*||");
/////////////////////////////////////////////////////////////////////////////
// CDCClientDlg dialog

CDCClientDlg::CDCClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDCClientDlg::IDD, pParent)
{

	m_pClient = NULL;

	//{{AFX_DATA_INIT(CDCClientDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	Create(IDD);		//create not modal dialog
}

void CDCClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDCClientDlg)
	DDX_Control(pDX, IDC_DIR_TO_BURN, m_EditBrowse);
	DDX_Control(pDX, IDC_MESSAGES, m_EdtMessages);
	DDX_Control(pDX, IDC_PROGRESS1, m_prgCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDCClientDlg, CDialog)
	//{{AFX_MSG_MAP(CDCClientDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_CDINFO, OnButtonCdinfo)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BURN, OnButtonBurn)
	ON_BN_CLICKED(IDABORT, OnAbort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCClientDlg message handlers

BOOL CDCClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	CRect lpRect;
	GetClientRect(lpRect);
	SetWindowPos(&wndTop,0,0,lpRect.right, lpRect.bottom, SWP_SHOWWINDOW);

	//init directory to burn
	m_EditBrowse.SetWindowText(_T("C:\\log"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDCClientDlg::OnPaint() 
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
HCURSOR CDCClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDCClientDlg::OnButtonConnect() 
{
	CIPCFetch			fetch;
	CConnectionRecord	cr;
	CIPCFetchResult		fr;

	if(!m_pClient)
	{
		cr.SetDestinationHost(LOCAL_LOOP_BACK);
		fr = fetch.FetchDataCarrier(cr);

		if(fr.IsOkay())
		{
			m_pClient = new CIPCDataCarrierClient(fr.GetShmName());
		}
		else
		{
			TRACE(_T("connect failed %s\n"),fr.GetErrorMessage());
		}
	}

	
}

void CDCClientDlg::OnButtonDisconnect() 
{
	WK_DELETE(m_pClient);
	AppendMessage(_T("CIPC connection canceled\n"));
}

void CDCClientDlg::OnButtonCdinfo() 
{
	if(m_pClient)
	{
		m_pClient->DoRequestVolumeInfos();
	}
}

void CDCClientDlg::OnOK() 
{
	OnButtonDisconnect();
	DestroyWindow();
}

void CDCClientDlg::OnCancel() 
{
	OnButtonDisconnect();
	DestroyWindow();
}

void CDCClientDlg::OnButtonBurn() 
{
	StartBurning();
}

void CDCClientDlg::StartBurning()
{
	if(m_pClient)
	{
		CString sVolumeName(_T("TestCD"));
		CStringArray saPathNames;
		saPathNames.Add(m_sDirToBurn);
//		saPathNames.Add(_T("C:\\windows\\regedit.exe"));
//		saPathNames.Add(_T("C:\\log"));


//TODO tkr hier filenotfound testen 
		int i = saPathNames.GetSize();;
		m_pClient->DoRequestSession(GetTickCount(),
								  sVolumeName,
								  saPathNames,
								  CD_WRITE_ISO);
	}
}


void CDCClientDlg::OnButtonBrowse() 
{

	CDirDialog dlg(TRUE,NULL,_T("trick.dat"),OFN_HIDEREADONLY,szFilter);
	dlg.m_ofn.lpstrInitialDir = m_sDirToBurn.GetBuffer(0);
	dlg.m_sDir = m_sDirToBurn;
	int res = dlg.DoModal();
	if (res==IDOK)
	{
		m_sDirToBurn.ReleaseBuffer(0);
		CString sTemp = dlg.GetPathName();
		int p = sTemp.ReverseFind(_T('\\'));
		m_sDirToBurn = sTemp.Left(p);
	}
	else
	{
	}

	m_EditBrowse.SetWindowText(m_sDirToBurn);
}

void CDCClientDlg::SetProgress(DWORD dwPercent)
{
	if(IsWindow(m_prgCtrl))	
	 m_prgCtrl.SetPos(dwPercent);
}

void CDCClientDlg::OnIdle()
{
	Sleep(20);
	PostMessage(WM_USER);
}

void CDCClientDlg::AppendMessage(CString sMessage)
{
	if(sMessage.Right(1) == _T("\n"))
	{
		sMessage = sMessage.Left(sMessage.GetLength()-1);
	}
	CString strBuffer;
	m_EdtMessages.GetWindowText (strBuffer);
	if (!strBuffer.IsEmpty())
	{
		strBuffer += _T("\r\n");
	}
	
	strBuffer += sMessage;
	m_EdtMessages.SetWindowText (strBuffer);
	m_EdtMessages.LineScroll (m_EdtMessages.GetLineCount(), 0);
}

void CDCClientDlg::DoDisconnect()
{
	OnButtonDisconnect();
}

void CDCClientDlg::OnAbort() 
{
	if(m_pClient)
	{
		m_pClient->DoRequestCancelSession(GetTickCount());
	}
}
