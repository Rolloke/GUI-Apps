// SelectInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SelectInputDlg.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSelectInputDlg dialog


CSelectInputDlg::CSelectInputDlg(CWnd* pParent,CInputList* pInputList)
	: CWK_Dialog(CSelectInputDlg::IDD, pParent)
{
	m_pInputList = pInputList;
	m_pSelectedInput = NULL;
}

void CSelectInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectInputDlg)
	DDX_Control(pDX, IDC_INPUT_LIST, m_Inputs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectInputDlg, CWK_Dialog)
	//{{AFX_MSG_MAP(CSelectInputDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_INPUT_LIST, OnDblclkInputList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectInputDlg message handlers
BOOL CSelectInputDlg::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}
	
	CRect	  rect;
	LV_COLUMN lvc;

	m_Inputs.SetImageList(theApp.GetSmallImage(),LVSIL_SMALL);

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText  = NULL;
	lvc.cx       = rect.Width();
	lvc.fmt      = LVCFMT_LEFT;

	m_Inputs.GetWindowRect(&rect);
	lvc.cx       = rect.Width()-4;
	m_Inputs.InsertColumn(0,&lvc);
	
	
	// Fuer jeden Input in der Inputliste einen Dlg anlegen und mit Daten fuellen
	CInputGroup* pInputGroup = NULL;
	CInput* pInput = NULL;
	CString s;
	int i,c;
	int j,d;

	LV_ITEM lvis;	

	lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvis.iSubItem = 0;

	// Schleife ueber alle Inputgroups
	c = m_pInputList->GetSize();
	for (i=0;i<c;i++)
	{
		pInputGroup = m_pInputList->GetGroupAt(i);
		// Schleife ueber alle Eingaenge der Gruppe

		d = pInputGroup->GetSize();
		for (j=0 ; j<d ; j++)
		{
			// Input nur anzeigen, wenn aktiv ist
			pInput = pInputGroup->GetInput(j);
			if ( pInput && pInput->GetIDActivate() != SECID_ACTIVE_OFF )
			{
				s = pInput->GetName();
				lvis.iItem = m_Inputs.GetItemCount();
				lvis.lParam = (LPARAM)(LPVOID)pInput;
				lvis.pszText = s.GetBuffer(0);
				lvis.cchTextMax = s.GetLength();
				if (pInput->GetIDActivate() == SECID_ACTIVE_OFF) 
				{
					lvis.iImage = IMAGE_NULL;  
				}
				else if (pInput->GetIDActivate() == SECID_ACTIVE_ON) 
				{
					lvis.iImage = IMAGE_INPUT;  
				}
				else if (pInput->GetIDActivate() == SECID_ACTIVE_LOCK) 
				{
					lvis.iImage = IMAGE_LOCK;  
				}
				else
				{
					lvis.iImage = 0;
				}
				m_Inputs.InsertItem(&lvis);
				s.ReleaseBuffer();
			}
		}
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSelectInputDlg::OnDblclkInputList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSelectInputDlg::OnOK() 
{
	int i,c;


	c = m_Inputs.GetItemCount();

	for (i=0;i<c;i++)
	{
		if (m_Inputs.GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
		{
			m_pSelectedInput = (CInput*)m_Inputs.GetItemData(i);
			break;
		}
	}
	
	if (m_pSelectedInput) {
		CWK_Dialog::OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
CInput*	CSelectInputDlg::GetSelectedInput()
{
	return m_pSelectedInput;
}
