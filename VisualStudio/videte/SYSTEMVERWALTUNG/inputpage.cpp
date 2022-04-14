/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputpage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputpage.cpp $
// CHECKIN:		$Date: 10.07.06 16:52 $
// VERSION:		$Revision: 30 $
// LAST CHANGE:	$Modtime: 10.07.06 16:43 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "systemverwaltung.h"
#include "globals.h"

#include "SVDoc.h"
#include "SVView.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#include "InputPage.h"
#include ".\inputpage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_INPUTS_ON_PAGE	12

/////////////////////////////////////////////////////////////////////////////
// CInputPage property page
IMPLEMENT_DYNAMIC(CInputPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CInputPage::CInputPage(CSVView* pParent, CInputGroup* pInputGroup) : CSVPage(CInputPage::IDD)
{
	m_pParent = pParent;
	m_sGroup = pInputGroup->GetName();
	//{{AFX_DATA_INIT(CInputPage)
	m_iScroll = 0;
	//}}AFX_DATA_INIT
	m_pInputGroup = pInputGroup;
	m_idGroup	  = m_pInputGroup->GetID();
	m_pInputList  = pParent->GetDocument()->GetInputs();
	m_nInitToolTips = TOOLTIPS_SIMPLE;

	Create(IDD,m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
CInputPage::~CInputPage()
{
	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::ShowHideStuff()
{
	if (!m_pInputGroup)
	{
		m_staticInputNr.ShowWindow(SW_HIDE);
		m_staticActivate.ShowWindow(SW_HIDE);
		m_staticEdge.ShowWindow(SW_HIDE);
		m_staticComment.ShowWindow(SW_HIDE);
	}
	else
	{
		m_staticInputNr.ShowWindow(SW_SHOW);
		m_staticActivate.ShowWindow(SW_SHOW);
		m_staticEdge.ShowWindow(SW_SHOW);
		m_staticComment.ShowWindow(SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::SaveChanges()
{
	WK_TRACE_USER(_T("Melder-Einstellungen wurden geändert\n"));
	if (!m_pInputGroup)
	{
		return;
	}

	CInputDialog* pInputDialog;
	int i,c;

	c = m_InputDialogs.GetSize();
	for (i=0;i<c;i++)
	{
		pInputDialog = m_InputDialogs.GetAtFast(i);
		if (pInputDialog)
		{
			pInputDialog->ControlToInput();
		}
	}
	m_pInputList->Save(GetProfile(),IsLocal());
	SetModified(FALSE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPage)
	DDX_Control(pDX, IDC_TXT_DEACTIVATE, m_staticTempDeactivate);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
	DDX_Control(pDX, IDC_TXTNR, m_staticInputNr);
	DDX_Control(pDX, IDC_TXTACTIVATE, m_staticActivate);
	DDX_Control(pDX, IDC_TXTEDGE, m_staticEdge);
	DDX_Control(pDX, IDC_TXTCOMMENT, m_staticComment);
	DDX_Scroll(pDX, IDC_SCROLLBAR, m_iScroll);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CInputPage, CSVPage)
	//{{AFX_MSG_MAP(CInputPage)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TYPE_INPUT, OnDeltaposSpinTypeInput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CInputPage::OnInitDialog() 
{
	CreateInputDialogs();

	CSVPage::OnInitDialog();
	RemoveFromResize(IDC_TXTNR);
	RemoveFromResize(IDC_TXTACTIVATE); // Workaround für Resizing
	m_staticTempDeactivate.ShowWindow(SW_HIDE);

	// gf todo when must the temp. deactivated text be visible
	if (GetDongle().AllowAlarmOffSpans())
	{
		m_staticTempDeactivate.ShowWindow(SW_SHOW);
	}
	if (m_pInputGroup->GetSize() > MAX_INPUTS_ON_PAGE)
	{
		m_ScrollBar.SetScrollRange(0,m_pInputGroup->GetSize()-1);
		SetPageSize(m_ScrollBar, MAX_INPUTS_ON_PAGE);
	}
	else
	{
		m_ScrollBar.ShowWindow(SW_HIDE);
	}

	SetModified(FALSE,FALSE);
	// Page muss einmal neugezeichnet werden, nicht immer kommt ein OnSize durch!
	Resize();

//	if (theApp.m_bProfessional)
	{
		CWK_Dialog*pDlg = GetSubDlg(0);
		CWnd *pWndChild   = pDlg->GetDlgItem(IDC_SPIN_TYPE_INPUT);
		CWnd *pWndMaster  = GetDlgItem(IDC_SPIN_TYPE_INPUT);
		if (pWndMaster && pWndChild)
		{
			RemoveFromResize(IDC_SPIN_TYPE_INPUT);
			pWndMaster->SendMessage(UDM_SETRANGE, 0, (LPARAM)pWndChild->SendMessage(UDM_GETRANGE));
			int nPos = pWndChild->SendMessage(UDM_GETPOS);
			pWndMaster->SendMessage(UDM_SETPOS, 0, nPos);
			pWndMaster->ShowWindow(SW_SHOW);
		}
	}
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::CreateInputDialogs()
{
	int i,c;
	CInput* pInput;
	CInputDialog* pInputDialog;

	c = m_pInputGroup->GetSize();
	for (i=0 ; i<MAX_INPUTS_ON_PAGE && i<c ; i++)
	{
		pInput = m_pInputGroup->GetInput(i);
		pInputDialog = new CInputDialog(this,pInput);
		m_InputDialogs.Add(pInputDialog);
	}

	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::Reset(BOOL bDirection)
{
	int i,c, iScroll = m_iScroll;
	CInput* pInput;
	CInputDialog* pInputDialog;

	c = m_InputDialogs.GetSize();

	// Es wurde nach oben gescrollt -> Elemente aufsteigend verschieben
	if (bDirection)
	{
		for (i=0 ; i<MAX_INPUTS_ON_PAGE && i<c ; i++)
		{
			pInputDialog = m_InputDialogs.GetAt(i);
			pInputDialog->ControlToInput();
			pInput = m_pInputGroup->GetInput(i+iScroll);
			if (!IsInputAllowed(pInput))
			{
				iScroll++;
				i--;
				continue;
			}
			pInputDialog->SetInput(pInput);
		}
	}
	else
	{
		for (i=MAX_INPUTS_ON_PAGE-1 ; i>=0 ; i--)
		{
			pInputDialog = m_InputDialogs.GetAt(i);
			pInputDialog->ControlToInput();
			pInput = m_pInputGroup->GetInput(i+iScroll);
			if (!IsInputAllowed(pInput))
			{
				iScroll--;
				i++;
				continue;
			}
			pInputDialog->SetInput(pInput);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CInputPage::Resize()
{
	if (m_staticInputNr.m_hWnd)
	{
		CRect rect;
		CInputDialog* pInputDialog;
		int i,c,height,off;
		CRect scrollrect;

		c = m_InputDialogs.GetSize();

		m_staticInputNr.GetWindowRect(rect);
		ScreenToClient(rect);
		off = rect.bottom + 10;

		if (c>0)
		{
			pInputDialog = m_InputDialogs.GetAt(0);
			pInputDialog->GetClientRect(rect);
			rect.top += off;
			rect.bottom += off;
			pInputDialog->MoveWindow(rect);
			pInputDialog->ShowWindow(SW_SHOW);
			height = rect.Height() + 5;
			scrollrect.left = rect.right + 5;
			scrollrect.right = scrollrect.left + GetSystemMetrics(SM_CXVSCROLL);
			scrollrect.top = rect.top;
			
			for (i=1;i<c;i++)
			{
				pInputDialog = m_InputDialogs.GetAt(i);
				pInputDialog->GetClientRect(rect);
				rect.top += height*i+off;
				rect.bottom += height*i+off;
				pInputDialog->MoveWindow(rect);
				pInputDialog->ShowWindow(SW_SHOW);
			}
			scrollrect.bottom = rect.bottom;
			m_ScrollBar.MoveWindow(scrollrect);
		}

		m_ScrollBar.ShowWindow((m_pInputGroup->GetSize()>MAX_INPUTS_ON_PAGE) ? SW_SHOW : SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::CancelChanges() 
{
	m_pInputList->Reset();
	m_pInputList->Load(GetProfile());

	// attention the m_pInputGroup is invalid
	// so get a correct one

	m_pInputGroup = m_pInputList->GetGroupByID(m_idGroup);

	CInputDialog* pInputDialog;
	int i,c,d;

	c = m_InputDialogs.GetSize();
	d = m_pInputGroup->GetSize();
	for (i=0;i<c && i<d;i++)
	{
		pInputDialog = m_InputDialogs.GetAt(i);
		pInputDialog->SetInput(m_pInputGroup->GetInput(m_iScroll+i));
	}
}

/////////////////////////////////////////////////////////////////////////////
void CInputPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldScroll = m_iScroll;
	CSVPage::OnVScroll(m_ScrollBar, nSBCode, nPos, m_pInputGroup->GetSize(), m_iScroll);

	if (oldScroll != m_iScroll)
	{
		UpdateData(FALSE);
		Reset(m_iScroll < oldScroll);
	}
	
	CWK_Dialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CInputPage::OnSize(UINT nType, int cx, int cy) 
{
	CSVPage::OnSize(nType, cx, cy);
	
	Resize();
}
/////////////////////////////////////////////////////////////////////////////
int CInputPage::GetNoOfSubDlgs()
{
	return m_InputDialogs.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
CWK_Dialog *CInputPage::GetSubDlg(int nDlg)
{
	if ((nDlg >=0) && (nDlg < m_InputDialogs.GetSize()))
	{
		return m_InputDialogs.GetAtFast(nDlg);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputPage::IsInputAllowed(CInput *pInput)
{
	if (   m_pInputGroup->GetSMName() == SM_LAUNCHER_INPUT
		 && pInput->GetID().GetSubID() == BIT_VALARM_ALARM_OFF_SPAN)
	{
		if (!GetDongle().AllowAlarmOffSpans())
		{
			return FALSE;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CInputPage::OnDeltaposSpinTypeInput(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int i, c = m_InputDialogs.GetSize(),
		   d = m_pInputGroup->GetSize(),
		   nPos = ::SendMessage(pNMUpDown->hdr.hwndFrom, UDM_GETPOS, 0, 0);
	CInputDialog* pInputDialog;
	if (c)
	{
		pInputDialog = m_InputDialogs.GetAt(0);
		for (i=0; i<m_iScroll; i++)
		{
			pInputDialog->SetInput(m_pInputGroup->GetInput(i));
			pInputDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
			pInputDialog->ControlToInput();
		}
		for (i=m_iScroll+c; i<d; i++)
		{
			pInputDialog->SetInput(m_pInputGroup->GetInput(i));
			pInputDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
			pInputDialog->ControlToInput();
		}

		pInputDialog->SetInput(m_pInputGroup->GetInput(m_iScroll));
	}

	for (i=0;i<c;i++)
	{
		pInputDialog = m_InputDialogs.GetAtFast(i);
		if (pInputDialog)
		{
			pInputDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
		}
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputPage::GetToolTip(UINT nID, CString&sText)
{
	if (nID == IDC_SPIN_TYPE_INPUT)
	{
		return sText.LoadString(IDS_DE_ACTIVATE_ALL);
	}
	return FALSE;
}
