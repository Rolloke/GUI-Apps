// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideteCheckDisk.h"
#include "InfoDlg.h"

#include "videtecheckdiskdlg.h"
#include ".\infodlg.h"


////////////////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog
IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)
CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	m_colBkGnd   = RGB(134,186,133);
	m_colText    = RGB(0,0,0);
	m_colLight   = RGB(168,232,166);	// Button like
	m_colShaddow = RGB(100, 140, 100);
//	m_colLight   = RGB(100, 140, 100);	// pressed Button like
//	m_colShaddow = RGB(168,232,166);
	m_nTimer = 0;
}
////////////////////////////////////////////////////////////////////////////////////////
CInfoDlg::~CInfoDlg()
{
}
////////////////////////////////////////////////////////////////////////////////////////
void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXT_INFO, m_txtInfo);
}
////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers
void CInfoDlg::Create(CWnd* pParent)
{
	if (m_hWnd)
	{
		ShowWindow(SW_SHOW);
	}
	else
	{
		CDialog::Create(IDD, pParent);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
BOOL CInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_FixedFont.CreatePointFont(12*10, _T("Courier New"));
	m_txtInfo.SetFont(&m_FixedFont);
	m_BkGndBrush.CreateSolidBrush(m_colBkGnd);

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
////////////////////////////////////////////////////////////////////////////////////////
BOOL CInfoDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	CDialog::OnEraseBkgnd(pDC);
	pDC->Draw3dRect(&rc, m_colLight, m_colShaddow);
	rc.DeflateRect(1,1);
	pDC->Draw3dRect(&rc, m_colLight, m_colShaddow);
	rc.DeflateRect(1,1);
	pDC->Draw3dRect(&rc, m_colLight, m_colShaddow);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////
void CInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
}
////////////////////////////////////////////////////////////////////////////////////////
HBRUSH CInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(m_colText);
		pDC->SetBkColor(m_colBkGnd);
	}

	return m_BkGndBrush;
}
////////////////////////////////////////////////////////////////////////////////////////
void CInfoDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nTimer)
	{
		KillTimer(nIDEvent);
		if (m_saMessages.GetCount())
		{
			m_txtInfo.SetWindowText(m_saMessages.GetAt(0));
			m_nTimer = SetTimer(100, m_dwaTimeouts.GetAt(0), NULL);
			m_saMessages.RemoveAt(0);
			m_dwaTimeouts.RemoveAt(0);
		}
		else
		{
			m_nTimer = 0;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
////////////////////////////////////////////////////////////////////////////////////////
void CInfoDlg::SetInfoText(LPCTSTR sInfo, int nTimeout)
{
	if (m_nTimer)
	{
		m_saMessages.Add(sInfo);
		m_dwaTimeouts.Add(nTimeout);
	}
	else
	{
		m_txtInfo.SetWindowText(sInfo);
		m_nTimer = SetTimer(100, nTimeout, NULL);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
BOOL CInfoDlg::PendingMessages()
{
	return m_nTimer != 0 ? TRUE : FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////
void CInfoDlg::OnOK()
{
//	CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////////////
void CInfoDlg::OnCancel()
{
	if (AfxMessageBox(IDS_REQUEST_CANCEL, MB_ICONQUESTION|MB_YESNO) == IDYES)
	{
		GetParent()->PostMessage(WM_COMMAND, IDOK);
		CDialog::OnCancel();
	}
}
