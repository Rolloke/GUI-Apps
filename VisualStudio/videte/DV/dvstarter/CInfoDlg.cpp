// CInfoDlg.cpp: implementation of the CInfoDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CInfoDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//////////////////////////////////////////////////////////////////////
CInfoDlg::~CInfoDlg()
{
	DestroyWindow();
}

//////////////////////////////////////////////////////////////////////
void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Control(pDX, IDC_INFO_DISPLAY_BORDER, m_ctrlBorderInfo);
	DDX_Control(pDX, IDC_INFO_DISPLAY, m_ctrlIInfoText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers

//////////////////////////////////////////////////////////////////////
BOOL CInfoDlg::Create()
{
	if (!CTransparentDialog::Create(IDD))
	{
		return FALSE;
	}

	CDC* pDC = GetDC();
	if (pDC)
	{
		int nHRes		= GetDeviceCaps(pDC->m_hDC, HORZRES);				
		int nVRes		= GetDeviceCaps(pDC->m_hDC, VERTRES);	
		CRect rcClient;
		GetClientRect(rcClient);
		int nX = (nHRes - rcClient.Width()) / 2;
		int nY = (nVRes - rcClient.Height()) - 20;
		SetWindowPos(&wndTopMost,nX, nY, 0, 0, SWP_NOSIZE);
	}
	
	ShowWindow(SW_SHOW);

	return TRUE;
}

void CInfoDlg::ShowInfoText(const CString& sInfo)
{
	m_ctrlIInfoText.SetDisplayText(sInfo);
}

BOOL CInfoDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	m_ctrlIInfoText.OpenStockDisplay(this, Display2);

	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText = RGB(0,0,0);
	DisplayColors.dwFlags = DSP_COL_TEXT;
	m_ctrlIInfoText.SetDisplayColors(DisplayColors);
	m_ctrlIInfoText.SetTextAllignment(DT_CENTER|DT_WORDBREAK);

	m_ctrlBorderInfo.CreateFrame(this);
	m_ctrlBorderInfo.EnableShadow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


