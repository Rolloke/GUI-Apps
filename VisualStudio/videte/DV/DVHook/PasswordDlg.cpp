// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvhook.h"
#include "PasswordDlg.h"
#include "..\LangDll\resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
	m_pTempMain = NULL;
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Text(pDX, IDC_EDT_PASSWORD, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_0, IDC_BTN_9, OnBtn)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void CPasswordDlg::OnOK() 
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, _T("DVRT\\DV"),_T(""));
#ifdef _UNICODE
	CString sPin = wkp.GetString(_T("Process"), _T("PIN"), _T(""), TRUE);
#else
	CString sPin = wkp.GetString(_T("Process"), _T("PIN"), _T(""));
#endif
	if (!sPin.IsEmpty())
	{
		 wkp.Decode(sPin);
	}
	else
	{
#ifdef _DEBUG
		sPin = _T("0000");
#endif
	}
	if (m_strPassword == sPin)
	{
		CDialog::OnOK();
	}
	else
	{
		CString sText, sCaption;
		sCaption.LoadString(IDS_ERROR);
		if(sCaption.IsEmpty())
		{
			sCaption = _T("Error");
		}
		sText.LoadString(IDS_PIN_FAILED);
		if (sText.IsEmpty())
		{
			sText    = _T("PIN");
		}
		::MessageBox(m_hWnd, sText, sCaption, MB_OK|MB_ICONERROR);
		m_strPassword.Empty();
		UpdateData(false);
	}
}

BOOL CPasswordDlg::OnInitDialog() 
{
	CString sText;
	CDialog::OnInitDialog();
	CDVHookApp *pApp = ((CDVHookApp*)AfxGetApp());
	if (m_pTempMain)
	{
		pApp->m_pMainWnd = m_pTempMain;
	}

	pApp->GetLanguageDll();

	if (sText.LoadString(IDS_FILE_SYSTEM_ACCESS))
		SetDlgItemText(IDC_TXT_ENTER_PASSWORD, sText);
	if (sText.LoadString(IDS_OK))
		SetDlgItemText(IDOK, sText);
	if (sText.LoadString(IDS_CANCEL))
		SetDlgItemText(IDCANCEL, sText);

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::OnTimer(UINT nIDEvent) 
{
//	SetForegroundWindow();
	CDialog::OnTimer(nIDEvent);
//	KillTimer(nIDEvent);
}

void CPasswordDlg::OnBtn(UINT nID) 
{
   _TCHAR nNo = (_TCHAR)(nID - IDC_BTN_0);
	if (nNo >= 0 && nNo <= 9)
	{
		nNo += '0';
		m_strPassword += nNo;
		UpdateData(false);
	}
}

void CPasswordDlg::OnBtnDel() 
{
	m_strPassword.Empty();
	UpdateData(false);
}
