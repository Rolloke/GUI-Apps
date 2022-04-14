// DlgStatistic.cpp : implementation file
//

#include "stdafx.h"
#include "SocketUnit.h"
#include "DlgStatistic.h"
#include ".\dlgstatistic.h"
#include <wkclasses/OScopeCtrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStatistic dialog

IMPLEMENT_DYNAMIC(CDlgStatistic, CDialog)
CDlgStatistic::CDlgStatistic(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStatistic::IDD, pParent)
{
	m_pScopeCtrl = NULL;
}

CDlgStatistic::~CDlgStatistic()
{
}

void CDlgStatistic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStatistic, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgStatistic message handlers

BOOL CDlgStatistic::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	CString sLoad;
	GetClientRect(&rect);
	rect.DeflateRect(5, 5);
	m_pScopeCtrl= new COScopeCtrl(3);
	m_pScopeCtrl->Create(WS_CHILD|WS_VISIBLE, rect, this, AFX_IDW_PANE_FIRST);
	sLoad.Format(_T("t [%d ms]"), m_nTimeStep);
	m_pScopeCtrl->SetXUnits(sLoad);

	sLoad = _T("[MBit/s]");
	m_pScopeCtrl->SetYUnits(sLoad);
	CWK_Profile wkp;
	m_pScopeCtrl->SetRange(0.0, wkp.GetInt(SOCKET_UNIT_STATISTICS, STAT_MAXIMUM, 100), 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStatistic::AddBitRate(double dSent, double dReceived, int iSendQueue)
{
	if (m_pScopeCtrl)
	{
		m_pScopeCtrl->AppendPoint(dSent, 0, FALSE);
		m_pScopeCtrl->AppendPoint(dReceived, 1, FALSE);
		m_pScopeCtrl->AppendPoint((double)iSendQueue,2,TRUE);
	}
}

void CDlgStatistic::OnDestroy()
{
	CDialog::OnDestroy();
	WK_DELETE(m_pScopeCtrl);
}

void CDlgStatistic::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}
