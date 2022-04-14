// SerialEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SerialEdit.h"
#include "SerialEditDlg.h"

#include "WK_File.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialEditDlg dialog

void CSerialEditDlg::TryDirectory(const CString &sDir)
{
	// check for directory first
	if (DoesFileExist(sDir)) {
		CString sFullPath(sDir);
		sFullPath += "\\serial.snt";
		if (DoesFileExist(sFullPath)) {
			m_sFullPath = sFullPath;
			// file found, read the first line
			CStdioFile serialFile;
			if (serialFile.Open(m_sFullPath,  CFile::modeRead)) {
				if (serialFile.ReadString(m_sSerialText)) {
					// fine
				} else {
					m_sSerialText = "Read Error";
				}
				serialFile.Close();
			} else {
				// failed to open
				m_sFullPath="";
			}
		} else {
		}
	} else {
		// directory not found
	}
}


CSerialEditDlg::CSerialEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSerialEditDlg)
	m_sSerialText = _T("");
	m_sFullPath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// hardcoded order is 
	// C:\Security
	// C:\GarnyHQ
	// .
	if (m_sFullPath.IsEmpty()) {
		TryDirectory("C:\\Security");
	}
	if (m_sFullPath.IsEmpty()) {
		TryDirectory("C:\\GarnyHQ");
	}
	if (m_sFullPath.IsEmpty()) {
		TryDirectory(".");
	}

	if (m_sFullPath.IsEmpty()) {
		m_sSerialText = "File not found";
	}
}

void CSerialEditDlg::WriteToFile()
{
	if (m_sFullPath.GetLength()) {
		CStdioFile serialFile;
		if (serialFile.Open(m_sFullPath,  CFile::modeCreate | CFile::modeWrite)) {
			// NOT YET FEX
			serialFile.WriteString(m_sSerialText);
			serialFile.Close();
		} else {
			// failed to open
			m_sFullPath="";
		}
	}
}

void CSerialEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialEditDlg)
	DDX_Text(pDX, IDC_SERIAL_TEXT, m_sSerialText);
	DDX_Text(pDX, IDC_FULL_PATH, m_sFullPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSerialEditDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialEditDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialEditDlg message handlers

BOOL CSerialEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialEditDlg::OnPaint() 
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
HCURSOR CSerialEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
