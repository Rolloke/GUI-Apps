// SocketPage.cpp : implementation file
//

#include "stdafx.h"
#include <direct.h>
#include "systemverwaltung.h"
#include "SocketPage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "WK_File.h"
#include "WKClasses/WK_Exec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szModules[]   = _T("SecurityLauncher\\Modules");
static TCHAR BASED_CODE szSocket[] = _T("SocketUnit");
/////////////////////////////////////////////////////////////////////////////
// CSocketPage dialog



CSocketPage::CSocketPage(CSVView* pParent) : CSVPage(CSocketPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CSocketPage)
	m_iBitrate = 1;
	//}}AFX_DATA_INIT
	m_bSocketUnit = FALSE;

	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}
void CSocketPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSocketPage)
	DDX_Control(pDX, IDC_RADIO_64_KB, m_btn64);
	DDX_Control(pDX, IDC_RADIO_128_KB, m_btn128);
	DDX_Control(pDX, IDC_RADIO_256_KB, m_btn256);
	DDX_Control(pDX, IDC_RADIO_512_KB, m_btn512);
	DDX_Control(pDX, IDC_RADIO_1MBIT, m_btn1MBit);
	DDX_Control(pDX, IDC_RADIO_2MBIT, m_btn2MBit);
	DDX_Control(pDX, IDC_RADIO_4MBIT, m_btn4MBit);
	DDX_Control(pDX, IDC_RADIO_8MBIT, m_btn8MBit);
	DDX_Check(pDX, IDC_CHECK_SOCKET, m_bSocketUnit);
	DDX_Radio(pDX, IDC_RADIO_8MBIT, m_iBitrate);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CSocketPage, CSVPage)
	//{{AFX_MSG_MAP(CSocketPage)
	ON_BN_CLICKED(IDC_CHECK_SOCKET, OnCheckSocket)
	ON_BN_CLICKED(IDC_RADIO_64_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_512_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_256_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_128_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_1MBIT, OnChange)
	ON_BN_CLICKED(IDC_RADIO_2MBIT, OnChange)
	ON_BN_CLICKED(IDC_RADIO_4MBIT, OnChange)
	ON_BN_CLICKED(IDC_RADIO_8MBIT, OnChange)
	ON_BN_CLICKED(IDC_DO_LISTEN, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketPage message handlers

BOOL CSocketPage::IsDataValid()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSocketPage::SaveChanges()
{
	UpdateData();

	GetProfile().WriteString(szModules,WK_APP_NAME_SOCKETS,m_bSocketUnit ? _T("SocketUnit.exe") : _T(""));

	int iSelectedBitrate = 256;
	switch (m_iBitrate)
	{
	case 0:
		iSelectedBitrate = 8192;
		break;
	case 1:
		iSelectedBitrate = 4096;
		break;
	case 2:
		iSelectedBitrate = 2048;
		break;
	case 3:
		iSelectedBitrate = 1024;
		break;
	case 4:
		iSelectedBitrate = 512;
		break;
	case 5:
		iSelectedBitrate = 256;
		break;
	case 6:
		iSelectedBitrate = 128;
		break;
	case 7:
		iSelectedBitrate = 64;
		break;
	}
	GetProfile().WriteInt(szSocket,_T("DefaultBitrate"),iSelectedBitrate );
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::CancelChanges()
{
	CString sTemp;

	sTemp = GetProfile().GetString(szModules,WK_APP_NAME_SOCKETS,_T(""));
	m_bSocketUnit = sTemp.CompareNoCase(_T("SocketUnit.exe"))==0; 

	int iSelectedBitrate = GetProfile().GetInt(szSocket,_T("DefaultBitrate"),1024);

	m_iBitrate = 1;
	switch (iSelectedBitrate)
	{
	case 8192:
		m_iBitrate = 0;
		break;
	case 4096:
		m_iBitrate = 1;
		break;
	case 2048:
		m_iBitrate = 2;
		break;
	case 1024:
		m_iBitrate = 3;
		break;
	case 512:
		m_iBitrate = 4;
		break;
	case 256:
		m_iBitrate = 5;
		break;
	case 128:
		m_iBitrate = 6;
		break;
	case 64:
		m_iBitrate = 7;
		break;
	}
	
	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::OnCheckSocket() 
{
	SetModified();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	CancelChanges();
	ShowHide();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::ShowHide()
{
	UpdateData();
	
	m_btn64.EnableWindow(m_bSocketUnit);
	m_btn128.EnableWindow(m_bSocketUnit);
	m_btn256.EnableWindow(m_bSocketUnit);
	m_btn512.EnableWindow(m_bSocketUnit);
	m_btn1MBit.EnableWindow(m_bSocketUnit);
	m_btn2MBit.EnableWindow(m_bSocketUnit);
	m_btn4MBit.EnableWindow(m_bSocketUnit);
	m_btn8MBit.EnableWindow(m_bSocketUnit);
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::OnChange() 
{
	SetModified(TRUE,FALSE);
	ShowHide();
	m_pParent->AddInitApp(WK_APP_NAME_SOCKETS);
}





/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////




