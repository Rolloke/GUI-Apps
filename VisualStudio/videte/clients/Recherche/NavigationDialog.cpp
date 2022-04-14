// NavigationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "NavigationDialog.h"
#include "ImageWindow.h"
#include "DisplayWindow.h"
#include "Mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavigationDialog dialog


CNavigationDialog::CNavigationDialog(CImageWindow* pImageWindow)
	: CDialog(CNavigationDialog::IDD, pImageWindow)
{
	m_pImageWindow = pImageWindow;
	//{{AFX_DATA_INIT(CNavigationDialog)
	m_sRecordNr = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,pImageWindow);
	CRect rect;
	m_pImageWindow->GetClientRect(rect);
	rect.top = rect.bottom - NAVIGATION_HEIGHT;
	MoveWindow(rect);
	ShowWindow(SW_SHOW);
}


void CNavigationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNavigationDialog)
	DDX_Text(pDX, IDC_NUMBER, m_sRecordNr);
	DDX_Control(pDX, IDC_SLIDER, m_ctrlSlider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNavigationDialog, CDialog)
	//{{AFX_MSG_MAP(CNavigationDialog)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavigationDialog message handlers

void CNavigationDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect cr;
	CRect r;
	GetWindowRect(r);
	int x = r.Width();

	if (x>0)
	{
		if (WK_IS_WINDOW(&m_ctrlSlider))
		{
			m_ctrlSlider.GetWindowRect(cr);
			ScreenToClient(cr);
			cr.right = x - cr.left - 4;
			m_ctrlSlider.MoveWindow(cr);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNavigationDialog::PostNcDestroy() 
{
	m_pImageWindow->m_pNavigationDialog = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNavigationDialog::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// don't call base class implementation 
	// it will eat frame window accelerators

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	pFrameWnd = GetParentFrame();   // start with first parent frame
	while (pFrameWnd != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	if (CDialog::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CNavigationDialog::EnableSlider(BOOL bEnable)
{
	m_ctrlSlider.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CNavigationDialog::Update()
{
	if (WK_IS_WINDOW(m_pImageWindow))
	{
		int i,c;
		i = m_pImageWindow->GetRecNo();
		c = m_pImageWindow->GetRecCount();
#ifdef _DEBUG
		if (m_pImageWindow->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)m_pImageWindow;
			m_sRecordNr.Format(_T("%d,%d/%d"),pDW->GetID().GetSubID(),i,c);
		}
#else
		m_sRecordNr.Format(_T("%d/%d"),i,c);
#endif
		if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
		{
			m_ctrlSlider.ShowWindow(SW_HIDE);
		}
		else
		{
			m_ctrlSlider.SetRange(1,c);
			m_ctrlSlider.SetLineSize(1);
			m_ctrlSlider.SetPos(i);
			m_ctrlSlider.ShowWindow(c>1 ? SW_SHOW : SW_HIDE);
		}

		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNavigationDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (nSBCode==SB_ENDSCROLL)
	{
		// gf todo sollte nicht erstmal ein Play gestoppt werden?!
		m_pImageWindow->Navigate(m_ctrlSlider.GetPos());
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CNavigationDialog::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CNavigationDialog::OnOK() 
{
	DestroyWindow();
}
