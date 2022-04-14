// RelayPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "RelayPage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "TreeList.h"
#include "Output.h"
#include "OutputGroup.h"
#include "OutputList.h"
#include ".\relaypage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_RELAYS_ON_PAGE	12

/////////////////////////////////////////////////////////////////////////////
// CRelayPage dialog
CRelayPage::CRelayPage(CSVView* pParent,COutputGroup* pOutputGroup)
	: CSVPage(CRelayPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CRelayPage)
	m_iScroll = 0;
	//}}AFX_DATA_INIT
	m_pOutputGroup			= pOutputGroup;
	m_sGroup = pOutputGroup->GetName();
	m_idGroup = pOutputGroup->GetID();
	m_pOutputList	= pParent->GetDocument()->GetOutputs();
	m_pInputList    = pParent->GetDocument()->GetInputs();
	m_iBoard = 0;

	m_nInitToolTips = TOOLTIPS_SIMPLE;
	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelayPage)
	DDX_Control(pDX, IDC_TXT_OUTPUTNR, m_staticOutputNr);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
	DDX_Scroll(pDX, IDC_SCROLLBAR, m_iScroll);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRelayPage, CSVPage)
	//{{AFX_MSG_MAP(CRelayPage)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TYPE_RELAY, OnDeltaposSpinTypeRelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CRelayPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRelayPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::SaveChanges() 
{
	WK_TRACE_USER(_T("Relais-Einstellungen wurden geändert\n"));
	CRelayDialog* pRelayDialog;
	int i,c;

	c = m_RelayDialogs.GetSize();
	for (i=0;i<c;i++)
	{
		pRelayDialog = m_RelayDialogs.GetAt(i);
		if (pRelayDialog)
		{
			pRelayDialog->ControlToOutput();
		}
	}

	m_pOutputList->Save(GetProfile(),IsLocal());

	SetModified(FALSE,FALSE); 
}
/////////////////////////////////////////////////////////////////////////////
// CRelayPage message handlers

BOOL CRelayPage::OnInitDialog() 
{
	CreateOutputDialogs();

	CSVPage::OnInitDialog();

	RemoveFromResize(IDC_TXT_OUTPUTNR);
	RemoveFromResize(IDC_TXT_OUTPUTTYP); // Workaround für Resizing
	
	if (m_pOutputGroup->GetSize()>MAX_RELAYS_ON_PAGE)
	{
		m_ScrollBar.SetScrollRange(0,m_pOutputGroup->GetSize()-1);
		SetPageSize(m_ScrollBar, MAX_RELAYS_ON_PAGE);
	}
	else
	{
		m_ScrollBar.ShowWindow(SW_HIDE);
	}

	SetModified(FALSE,FALSE);

//	if (theApp.m_bProfessional)
	{
		CRelayDialog*pDlg = m_RelayDialogs.GetAtFast(0);
		CWnd *pWndChild   = pDlg->GetDlgItem(IDC_SPIN_TYPE_RELAY);
		CWnd *pWndMaster  = GetDlgItem(IDC_SPIN_TYPE_RELAY);
		if (pWndMaster && pWndChild)
		{
			RemoveFromResize(IDC_SPIN_TYPE_RELAY);
			pWndMaster->SendMessage(UDM_SETRANGE, 0, (LPARAM)pWndChild->SendMessage(UDM_GETRANGE));
			pWndMaster->ShowWindow(SW_SHOW);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::CreateOutputDialogs()
{
	int i,c;
	COutput* pOutput;
	CRelayDialog* pRelayDialog;

	c = m_pOutputGroup->GetSize();
	for (i=0;i<MAX_RELAYS_ON_PAGE && i<c;i++)
	{
		pOutput = m_pOutputGroup->GetOutput(i);
		pRelayDialog = new CRelayDialog(this,pOutput);
		m_RelayDialogs.Add(pRelayDialog);
	}

	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::Reset(BOOL bDirection)
{
	int i,c;
	COutput* pOutput;
	CRelayDialog* pRelayDialog;

	c = m_RelayDialogs.GetSize();
	
	// Es wurde nach oben gescrollt -> Elemente aufsteigend verschieben
	if (bDirection)
	{
		for (i=0;i<MAX_RELAYS_ON_PAGE && i<c;i++)
		{
			pRelayDialog = m_RelayDialogs.GetAt(i);
			pRelayDialog->ControlToOutput();
			pOutput = m_pOutputGroup->GetOutput(i+m_iScroll);
			pRelayDialog->SetOutput(pOutput);
		}
	}
	else // Es wurde nach unten gescrollt -> Elemente absteigend verschieben
	{
		for (i=MAX_RELAYS_ON_PAGE-1;i>=0;i--)
		{
			pRelayDialog = m_RelayDialogs.GetAt(i);
			pRelayDialog->ControlToOutput();
			pOutput = m_pOutputGroup->GetOutput(i+m_iScroll);
			pRelayDialog->SetOutput(pOutput);
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::Resize()
{
	if (m_staticOutputNr.m_hWnd)
	{
		CRect rect;
		CRelayDialog* pRelayDialog;
		int i,c,height,off;
		CRect scrollrect;

		c = m_RelayDialogs.GetSize();

		m_staticOutputNr.GetWindowRect(rect);
		ScreenToClient(rect);
		off = rect.bottom + 10;

		if (c>0)
		{
			pRelayDialog = m_RelayDialogs.GetAt(0);
			pRelayDialog->GetClientRect(rect);
			rect.top += off;
			rect.bottom += off;
			pRelayDialog->MoveWindow(rect);
			pRelayDialog->ShowWindow(SW_SHOW);
			height = rect.Height() + 5;
			scrollrect.left = rect.right + 5;
			scrollrect.right = scrollrect.left + GetSystemMetrics(SM_CXVSCROLL);
			scrollrect.top = rect.top;
			
			for (i=1;i<c;i++)
			{
				pRelayDialog = m_RelayDialogs.GetAt(i);
				pRelayDialog->GetClientRect(rect);
				rect.top += height*i+off;
				rect.bottom += height*i+off;
				pRelayDialog->MoveWindow(rect);
				pRelayDialog->ShowWindow(SW_SHOW);
			}
			scrollrect.bottom = rect.bottom;
			m_ScrollBar.MoveWindow(scrollrect);
		}

		m_ScrollBar.ShowWindow((m_pOutputGroup->GetSize()>MAX_RELAYS_ON_PAGE) ? SW_SHOW : SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::CancelChanges() 
{
	m_pOutputList->Reset();
	m_pOutputList->Load(GetProfile());

	// attention the m_pOutputGroup is invalid
	// so get a correct one

	m_pOutputGroup = m_pOutputList->GetGroupByID(m_idGroup);

	CRelayDialog* pRelayDialog;
	int i,c;

	c = m_RelayDialogs.GetSize();
	for (i=0;i<c;i++)
	{
		pRelayDialog = m_RelayDialogs.GetAt(i);
		pRelayDialog->SetOutput(m_pOutputGroup->GetOutput(m_iScroll+i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int		oldScroll	= m_iScroll;
	CSVPage::OnVScroll(m_ScrollBar, nSBCode, nPos, m_pOutputGroup->GetSize(), m_iScroll);

	if (oldScroll != m_iScroll)
	{
		UpdateData(FALSE);
		Reset(m_iScroll < oldScroll);
	}
	
	CWK_Dialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::OnSize(UINT nType, int cx, int cy) 
{
	CSVPage::OnSize(nType, cx, cy);
	
	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayPage::OnDeltaposSpinTypeRelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int i, c = m_RelayDialogs.GetSize(),
		   d = m_pOutputGroup->GetSize(),
		nPos = ::SendMessage(pNMUpDown->hdr.hwndFrom, UDM_GETPOS, 0, 0);

	CRelayDialog* pRelayDialog;
	if (c)
	{
		pRelayDialog = m_RelayDialogs.GetAt(0);
		for (i=0; i<m_iScroll; i++)
		{
			pRelayDialog->SetOutput(m_pOutputGroup->GetOutput(i));
			pRelayDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
			pRelayDialog->ControlToOutput();
		}
		for (i=m_iScroll+c; i<d; i++)
		{
			pRelayDialog->SetOutput(m_pOutputGroup->GetOutput(i));
			pRelayDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
			pRelayDialog->ControlToOutput();
		}

		pRelayDialog->SetOutput(m_pOutputGroup->GetOutput(m_iScroll));
	}

	for (i=0;i<c;i++)
	{
		pRelayDialog = m_RelayDialogs.GetAtFast(i);
		if (pRelayDialog)
		{
			pRelayDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRelayPage::GetToolTip(UINT nID, CString&sText)
{
	if (nID == IDC_SPIN_TYPE_RELAY)
	{
		return sText.LoadString(IDS_DE_ACTIVATE_ALL);
	}
	return FALSE;
}
