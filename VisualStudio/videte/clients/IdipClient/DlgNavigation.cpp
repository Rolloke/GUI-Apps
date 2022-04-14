// NavigationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgNavigation.h"
#include "WndImage.h"
#include "WndPlay.h"
#include "Mainfrm.h"
#include ".\dlgnavigation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNavigation dialog


CDlgNavigation::CDlgNavigation(CWndImageRecherche* pImageWindow)
	: CSkinDialog(CDlgNavigation::IDD, pImageWindow)
{
	m_pImageWindow = pImageWindow;
	//{{AFX_DATA_INIT(CDlgNavigation)
	m_sRecordNr = _T("");
	//}}AFX_DATA_INIT
	
	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		MoveOnClientAreaClick(false);
	}
	Create(IDD,pImageWindow);
	CRect rect;
	m_pImageWindow->GetClientRect(rect);
	rect.top = rect.bottom - NAVIGATION_HEIGHT;
	MoveWindow(rect);
	ShowWindow(SW_SHOW);
}


void CDlgNavigation::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNavigation)
	DDX_Text(pDX, IDC_NUMBER, m_sRecordNr);
	DDX_Control(pDX, IDC_SLIDER, m_ctrlSlider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNavigation, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgNavigation)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNavigation message handlers

void CDlgNavigation::OnSize(UINT nType, int cx, int cy) 
{
	CSkinDialog::OnSize(nType, cx, cy);

	if (cx>0)
	{
		CWnd *pWnd = GetDlgItem(IDC_NUMBER);
		CRect rcNumber, rcSlider;
		if (pWnd)
		{
			pWnd->GetWindowRect(rcNumber);
		}
		if (m_ctrlSlider.m_hWnd)
		{
			m_ctrlSlider.GetWindowRect(rcSlider);
			ScreenToClient(rcSlider);
			rcSlider.left = rcNumber.Width()+5;
			rcSlider.right = cx - 5;
			m_ctrlSlider.MoveWindow(rcSlider);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgNavigation::PostNcDestroy() 
{
	m_pImageWindow->m_pDlgNavigation = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgNavigation::PreTranslateMessage(MSG* pMsg) 
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

	if (CSkinDialog::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgNavigation::EnableSlider(BOOL bEnable)
{
	m_ctrlSlider.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgNavigation::Update()
{
	if (m_pImageWindow)
	{
		int i,c;
		i = m_pImageWindow->GetRecNo();
		c = m_pImageWindow->GetRecCount();
#ifdef _DEBUG
		if (m_pImageWindow->IsWndPlay())
		{
			CWndPlay* pDW = (CWndPlay*)m_pImageWindow;
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
void CDlgNavigation::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	BOOL bIsLowBandwidth = FALSE;
	CServer* pServer = m_pImageWindow->GetServer();

	if (pServer)
	{
		bIsLowBandwidth = pServer->IsLowBandwidth();
	}

	if (   nSBCode==SB_ENDSCROLL
		|| (nSBCode==SB_THUMBTRACK && !bIsLowBandwidth))
	{
		// gf todo sollte nicht erstmal ein Play gestoppt werden?!
		m_pImageWindow->Navigate(m_ctrlSlider.GetPos(),-1); //muss mit -1 übergeben werden, da dann
															//im WndPlay m_bCorrectRequestRecordNr = TRUE
															//gesetzt wird, um bei Mpeg4 Geisterbilder
															//zu verhindern
	}
	else if (nSBCode==SB_THUMBPOSITION)
	{
		if (m_pImageWindow->IsKindOf(RUNTIME_CLASS(CWndPlay)))
		{
			CWndPlay* pWndPlay = (CWndPlay*)m_pImageWindow;
			DWORD dwCurrentRecord = pWndPlay->GetCurrentRecordNr();
			m_pImageWindow->Navigate(dwCurrentRecord,nPos-dwCurrentRecord); //muss mit -1 übergeben werden, da dann
		}
	}
	CSkinDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgNavigation::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgNavigation::OnOK() 
{
	DestroyWindow();
}

BOOL CDlgNavigation::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();

	// TODO! RKE: in stead of tick marks a tooltip info may be shown
	m_ctrlSlider.ModifyStyle(TBS_AUTOTICKS, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
}
