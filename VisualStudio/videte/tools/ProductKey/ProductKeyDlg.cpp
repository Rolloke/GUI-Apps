// ProductKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProductKey.h"
#include "ProductKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProductKeyDlg dialog

CProductKeyDlg::CProductKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProductKeyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProductKeyDlg)
	m_sSerialNr = _T("");
	m_sRSAn = _T("");
	m_sRSAe = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	char szVolume[255];
	DWORD dwVolumeSerialNumber = 0;
	DWORD dwMaximumComponentLength = 0; 
	DWORD dwFileSystemFlags = 0; 
	char szFileSystemNameBuffer[255]; 

	if (GetVolumeInformation("c:\\",
							 szVolume,sizeof(szVolume),
							 &dwVolumeSerialNumber, 
							 &dwMaximumComponentLength,
							 &dwFileSystemFlags,
							 szFileSystemNameBuffer,sizeof(szFileSystemNameBuffer)))
	{
		WORD wH,wL;
		wH = HIWORD(dwVolumeSerialNumber);
		wL = LOWORD(dwVolumeSerialNumber);
		m_sSerialNr.Format("%04x-%04x",wH,wL);
		m_sRSAn.Format("%u",m_RSA.n());
		m_sRSAe.Format("%u",m_RSA.e());
	}
}

void CProductKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProductKeyDlg)
	DDX_Text(pDX, IDC_TXT_SERIALNR, m_sSerialNr);
	DDX_Text(pDX, IDC_RSA_N, m_sRSAn);
	DDX_Text(pDX, IDC_RSA_E, m_sRSAe);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProductKeyDlg, CDialog)
	//{{AFX_MSG_MAP(CProductKeyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ENCODE, OnEncode)
	ON_BN_CLICKED(IDC_DECODE, OnDecode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProductKeyDlg message handlers

BOOL CProductKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	if (!m_RSA.IsValid())
	{
		GetDlgItem(IDC_ENCODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DECODE)->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProductKeyDlg::OnPaint() 
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
HCURSOR CProductKeyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProductKeyDlg::OnEncode() 
{
	CString sFilter;
	CString sName,sPathName;
	sFilter.LoadString(IDS_ALL_FILTER);
	CFileDialog dlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY , sFilter);

	dlg.m_ofn.lpstrInitialDir = m_sPath.IsEmpty() ? "c:\\" : m_sPath;
	if (dlg.DoModal()==IDOK) 
	{
		CString s = dlg.GetPathName();
		int p;
		p = s.ReverseFind('\\');
		m_sPath = s.Left(p+1);

		if (m_RSA.Encode(s,s))
		{
			AfxMessageBox(IDS_ENCODE_SUCCESS);
		}
		else
		{
			AfxMessageBox(IDS_ENCODE_FAIL);
		}
	}
}

void CProductKeyDlg::OnDecode() 
{
	CString sFilter;
	CString sName,sPathName;
	sFilter.LoadString(IDS_ALL_FILTER);
	CFileDialog dlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY , sFilter);

	dlg.m_ofn.lpstrInitialDir = m_sPath.IsEmpty() ? "c:\\" : m_sPath;
	if (dlg.DoModal()==IDOK) 
	{
		CString s = dlg.GetPathName();
		int p;
		p = s.ReverseFind('\\');
		m_sPath = s.Left(p+1);

		if (m_RSA.Decode(s,s))
		{
			AfxMessageBox(IDS_DECODE_SUCCESS);
		}
		else
		{
			AfxMessageBox(IDS_DECODE_FAIL);
		}
	}
}
