// PINReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PINReader.h"
#include "PINReaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPINReaderDlg dialog

CPINReaderDlg::CPINReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPINReaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPINReaderDlg)
	m_sOPIN = _T("");
	m_sSPIN = _T("");
	m_sSerial = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPINReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPINReaderDlg)
	DDX_Text(pDX, IDC_TXT_OPIN, m_sOPIN);
	DDX_Text(pDX, IDC_TXT_SPIN, m_sSPIN);
	DDX_Text(pDX, IDC_TXT_SERIAL, m_sSerial);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPINReaderDlg, CDialog)
	//{{AFX_MSG_MAP(CPINReaderDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static char szSection[] = "Process";

/////////////////////////////////////////////////////////////////////////////
// CPINReaderDlg message handlers

BOOL CPINReaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,"DVRT\\DV","");


	//Auslesen der Pins in Abhängigkeit des zum Test hardcodierten Computernamens

	CString shostname = GetKey("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName","ComputerName", REG_SZ);
	m_sSerial = shostname;

//TODO TKR geänderte Version zum Auslesen der Pin nur eines einzigen bestimmten System
	if(    shostname == "2205187-1"
		|| shostname == "2205187-2"
		|| shostname == "2205187-3")
	{
		CString s = wkp.GetString(szSection,"PIN","");
		if (!s.IsEmpty())
		{
			 wkp.Decode(s);
			 m_sSPIN = s;
		}
		s = wkp.GetString(szSection,"OPIN","");
		if (!s.IsEmpty())
		{
			 wkp.Decode(s);
			 m_sOPIN = s;
		}
	}
	else
	{
		m_sSPIN = "wrong version";
		m_sOPIN = "wrong version";
	}


//wieder rein bei allgemeingültigkeit
/*
	CString s = wkp.GetString(szSection,"PIN","");
	if (!s.IsEmpty())
	{
		 wkp.Decode(s);
		 m_sSPIN = s;
	}
	s = wkp.GetString(szSection,"OPIN","");
	if (!s.IsEmpty())
	{
		 wkp.Decode(s);
		 m_sOPIN = s;
	}
*/	
//TODO TKR ende
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPINReaderDlg::OnPaint() 
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
HCURSOR CPINReaderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CString CPINReaderDlg::GetKey(CString KeyAdress, CString KeyName, DWORD Regtype)
{
	HKEY phkResult;
	DWORD dwType,dwLen;
	LONG rvalue;
	CString strValue;
	LPBYTE test=NULL;
	BYTE value;
	
	
	//Schlüssel öffnen
	rvalue = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,         // handle to open key
		KeyAdress,  // address of name of subkey to open
		0,							// reserved
		KEY_QUERY_VALUE, // security access mask
		&phkResult    // address of handle to open key
		);
	//Länge des Inhaltes holen
	if(rvalue == ERROR_SUCCESS)
	{
		rvalue = RegQueryValueEx(phkResult, 
			KeyName, 
			NULL, 
			&dwType, 
			NULL,
			&dwLen);
		//Inhalt (OEM) holen;
		if(rvalue == ERROR_SUCCESS)
		{
			if (Regtype==REG_SZ)
			{
				rvalue = RegQueryValueEx(phkResult, 
					KeyName, 
					NULL, 
					&dwType,
					(LPBYTE)strValue.GetBuffer(dwLen/sizeof(TCHAR)),&dwLen);
				strValue.ReleaseBuffer();
				RegCloseKey(phkResult);
				return strValue;
			}
			
			else if(Regtype == REG_DWORD)
			{
				rvalue = RegQueryValueEx(phkResult, 
					KeyName, 
					NULL, 
					&dwType,
					&value, 
					&dwLen);
				
				RegCloseKey(phkResult);
				strValue.Format("%i",value);
				return strValue;
			}
		}
	}
	
	RegCloseKey(phkResult);
	return "0";
}
