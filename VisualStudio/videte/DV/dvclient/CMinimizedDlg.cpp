/* GlobalReplace: CSkinsDlg --> CPanel */
// CMinimizedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "CMinimizedDlg.h"
#include "CPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg dialog


CMinimizedDlg::CMinimizedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMinimizedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMinimizedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPanel = (CPanel*)pParent;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMinimizedDlg::Create	()
{
	if (!CDialog::Create(IDD))
	{
		return FALSE;
	}

	return TRUE;
}


void CMinimizedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMinimizedDlg)
	DDX_Control(pDX, IDC_MINIMIZED, m_ctrlBitmap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMinimizedDlg, CDialog)
	//{{AFX_MSG_MAP(CMinimizedDlg)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::PostNcDestroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CMinimizedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// OEM Spezifische Einstellungen
	if (m_pPanel->GetOem() == OemDTS)
		m_Bitmap.LoadBitmap(IDB_MINIMIZED);
	else if (m_pPanel->GetOem() == OemAlarmCom)
		m_Bitmap.LoadBitmap(IDB_MINIMIZED_AC);
	else if (m_pPanel->GetOem() == OemDResearch)
		m_Bitmap.LoadBitmap(IDB_MINIMIZED);
	else if (m_pPanel->GetOem() == OemSantec)
		m_Bitmap.LoadBitmap(IDB_MINIMIZED);
	else if (m_pPanel->GetOem() == OemPOne)
		m_Bitmap.LoadBitmap(IDB_MINIMIZED_PO);
	else
		WK_TRACE_ERROR(_T("Unknown OEMCode (%d)\n"), m_pPanel->GetOem());
	
	m_ctrlBitmap.SetBitmap(m_Bitmap);

	MoveToCorner();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnOK() 
{
	m_pPanel->MaximizeDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnCancel() 
{
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMinimizedDlg::MoveToCorner()
{
	HDC hDC			= CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	int nHRes		= GetDeviceCaps(hDC, HORZRES);				
	int nVRes		= GetDeviceCaps(hDC, VERTRES);	
	DeleteDC(hDC);
	CRect rcClient;

	CWnd* pWnd = (CWnd*)theApp.GetMainFrame();
	if (pWnd)
	{
		pWnd->GetWindowRect(rcClient);

		BITMAP bm;
		m_Bitmap.GetBitmap(&bm);
		CRect rect(0,0, bm.bmWidth+4, bm.bmHeight+4);

		int nXPos = min(rcClient.right - rect.Width(), nHRes - rect.Width());
		int nYPos = min(rcClient.bottom - rect.Height(), nVRes - rect.Height());
		SetWindowPos(&CWnd::wndTopMost, nXPos, nYPos, rect.Width(), rect.Height(), 0);
		ShowWindow(SW_SHOW);
	}

	return TRUE;
}
