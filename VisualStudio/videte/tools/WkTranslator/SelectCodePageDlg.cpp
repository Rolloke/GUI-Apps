// SelectCodePageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wktranslator.h"
#include "SelectCodePageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSelectCodePageDlg dialog
HWND CSelectCodePageDlg::gm_hWndCombo = NULL;

CSelectCodePageDlg::CSelectCodePageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCodePageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectCodePageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_wCodePage = CP_UNICODE;
}


void CSelectCodePageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectCodePageDlg)
	DDX_Control(pDX, IDC_COMBO_CODE_PAGE, m_cCodePages);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectCodePageDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectCodePageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectCodePageDlg message handlers
BOOL CALLBACK CSelectCodePageDlg::EnumCodePagesProc(LPTSTR lpCodePageString)
{
	CPINFOEX cpi;
	GetCPInfoEx (_ttoi(lpCodePageString), 0, &cpi);
	if (cpi.CodePage < 10000)
	{
		int nRet = ::SendMessage(gm_hWndCombo, CB_ADDSTRING, 0, (LPARAM)cpi.CodePageName);
		::SendMessage(gm_hWndCombo, CB_SETITEMDATA, nRet, (LPARAM)cpi.CodePage);
	}
	return 1;
}

BOOL CSelectCodePageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString str;
	int nCurrent;
	str.LoadString(IDS_DEFAULTANSICODEPAGE);
	nCurrent = m_cCodePages.AddString(str);
	m_cCodePages.SetItemData(nCurrent, CP_ACP);

	str.LoadString(IDS_DEFAULTOEMCODEPAGE);
	nCurrent = m_cCodePages.AddString(str);
	m_cCodePages.SetItemData(nCurrent, CP_OEMCP);

	str.LoadString(IDS_UNICODE);
	nCurrent = m_cCodePages.AddString(str);
	m_cCodePages.SetItemData(nCurrent, CP_UNICODE);

	gm_hWndCombo = m_cCodePages.m_hWnd;
	EnumSystemCodePages(EnumCodePagesProc, CP_INSTALLED);
	int i, nCount = m_cCodePages.GetCount();
	for (i=0; i<nCount; i++)
	{
		if (m_cCodePages.GetItemData(i) == m_wCodePage)
		{
			m_cCodePages.SetCurSel(i);
			break;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectCodePageDlg::OnOK() 
{
	m_wCodePage = (WORD)m_cCodePages.GetItemData(m_cCodePages.GetCurSel());

	CDialog::OnOK();
}
