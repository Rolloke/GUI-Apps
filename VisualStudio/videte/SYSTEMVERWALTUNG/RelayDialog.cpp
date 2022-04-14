// RelayDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"

#include "Output.h"
#include "OutputGroup.h"
#include "OutputList.h"

#include "RelayDialog.h"
#include "RelayPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRelayDialog dialog

int CRelayDialog::gm_nList[2] = {IMAGE_NULL, IMAGE_RELAY};
#define   RELAY_OFF  0
#define   RELAY_ON   1

CRelayDialog::CRelayDialog(CRelayPage* pRelayPage, COutput* pOutput)
	: CWK_Dialog(CRelayDialog::IDD)
{
	m_pRelayPage = pRelayPage;
	m_pOutput = pOutput;
	//{{AFX_DATA_INIT(CRelayDialog)
	m_sNr = _T("1");
	m_sName = _T("");
	m_sClose = _T("0");
	m_bClientBreak = FALSE;
	m_bClientKeep = FALSE;
	m_iEdge = -1;
	//}}AFX_DATA_INIT
   m_bSetPos = false;
	Create(IDD,m_pRelayPage);
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::PostNcDestroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnOK()
{
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnCancel()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRelayDialog::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelayDialog)
	DDX_Control(pDX, IDC_STATIC_TYPE_RELAY, m_StaticTypeRelay);
	DDX_Control(pDX, IDC_SPIN_TYPE_RELAY, m_SpinTypeRelay);
	DDX_Control(pDX, IDC_CHECK_CLIENT_KEEP, m_btnClientKeep);
	DDX_Control(pDX, IDC_CHECK_CLIENT_BREAK, m_btnClientBreak);
	DDX_Control(pDX, IDC_COMBO_CLIENT_EDGE, m_cbEdge);
	DDX_Control(pDX, IDC_EDIT_CLOSE_RELAY, m_editClose);
	DDX_Control(pDX, IDC_NAME_RELAY, m_editName);
	DDX_Text(pDX, IDC_NR_RELAY, m_sNr);
	DDX_Text(pDX, IDC_NAME_RELAY, m_sName);
	DDX_Text(pDX, IDC_EDIT_CLOSE_RELAY, m_sClose);
	DDX_Check(pDX, IDC_CHECK_CLIENT_BREAK, m_bClientBreak);
	DDX_Check(pDX, IDC_CHECK_CLIENT_KEEP, m_bClientKeep);
	DDX_CBIndex(pDX, IDC_COMBO_CLIENT_EDGE, m_iEdge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRelayDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CRelayDialog)
	ON_BN_CLICKED(IDC_CHECK_CLIENT_BREAK, OnCheckClientBreak)
	ON_BN_CLICKED(IDC_CHECK_CLIENT_KEEP, OnCheckClientKeep)
	ON_CBN_SELCHANGE(IDC_COMBO_CLIENT_EDGE, OnSelchangeComboEdge)
	ON_EN_CHANGE(IDC_EDIT_CLOSE_RELAY, OnChangeEditCloseRelay)
	ON_EN_CHANGE(IDC_NAME_RELAY, OnChangeKommentar)
	ON_EN_CHANGE(IDC_EDIT_TYPE_RELAY, OnChangeEditTypeRelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelayDialog message handlers
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::SetOutput(COutput* pOutput)
{
	m_pOutput = pOutput;
	OutputToControl();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OutputToControl()
{
	m_sNr.Format(_T("%d"),m_pOutput->GetID().GetSubID()+1);
	if (m_pOutput->IsSTM())
	{
		m_sName.LoadString(IDS_STM);
		m_editName.EnableWindow(FALSE);
		m_SpinTypeRelay.EnableWindow(FALSE);
		m_StaticTypeRelay.EnableWindow(FALSE);
		m_editClose.EnableWindow(FALSE);
		m_btnClientKeep.EnableWindow(FALSE);
		m_btnClientBreak.EnableWindow(FALSE);
		m_cbEdge.EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
	{
		m_editName.EnableWindow(TRUE);
		m_SpinTypeRelay.EnableWindow(TRUE);
		m_StaticTypeRelay.EnableWindow(TRUE);
		m_editClose.EnableWindow(TRUE);
		m_btnClientKeep.EnableWindow(TRUE);
		m_btnClientBreak.EnableWindow(TRUE);
		m_cbEdge.EnableWindow(TRUE);

		m_sName = m_pOutput->GetName();
		m_bClientKeep = m_pOutput->GetKeep();
		m_bClientBreak = m_pOutput->GetClient();
		m_iEdge = m_pOutput->GetEdge();
		m_sClose.Format(_T("%d"),m_pOutput->GetHoldClose());
		int nPos = RELAY_OFF;
		if (m_pOutput->GetExOutputType() == EX_OUTPUT_OFF)
		{
			nPos = RELAY_OFF;
		}
		else if (m_pOutput->GetExOutputType() == EX_OUTPUT_RELAY)
		{
			nPos = RELAY_ON;
		}
		m_bSetPos = true;
		m_SpinTypeRelay.SetPos(nPos);
		if (nPos == 0)
		{
			OnChangeEditTypeRelay();
		}
		m_bSetPos = false;

		UpdateData(FALSE);
		ShowHide();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::ControlToOutput()
{
	UpdateData();
	int nPos = m_SpinTypeRelay.GetPos();

	m_pOutput->SetName(m_sName);
	if (nPos == RELAY_OFF)
	{
		m_pOutput->SetExOutputType(EX_OUTPUT_OFF);
	}
	else if (nPos == RELAY_ON)
	{
		m_pOutput->SetExOutputType(EX_OUTPUT_RELAY);
		m_pOutput->SetKeep(m_bClientKeep);
		m_pOutput->SetClient(m_bClientBreak);
		m_pOutput->SetEdge(m_iEdge);
		m_pOutput->SetHoldClose(_ttoi(m_sClose));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::InitControls()
{
	CString sL;

	m_StaticTypeRelay.ModifyStyle(0, SS_ICON, 0);
	m_SpinTypeRelay.SetRange32(1, 0);

	sL.LoadString(IDS_EDGE);
	m_cbEdge.AddString(sL);
	sL.LoadString(IDS_LEVEL);
	m_cbEdge.AddString(sL);
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::ShowHide()
{
//	UpdateData();
	BOOL bEnable = (m_SpinTypeRelay.GetPos() != RELAY_OFF) ? TRUE : FALSE;
//	HTREEITEM hSelectedItem = m_Type.GetFirstVisibleItem();

	m_editName.EnableWindow(bEnable);
	m_editClose.EnableWindow(bEnable);
	m_btnClientKeep.EnableWindow(bEnable);
	m_cbEdge.EnableWindow(bEnable);


	m_btnClientBreak.EnableWindow(bEnable && !m_bClientKeep && !m_iEdge);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRelayDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	RemoveFromResize(IDC_NR_RELAY);
	RemoveFromResize(IDC_EDIT_TYPE_RELAY);
	RemoveFromResize(IDC_SPIN_TYPE_RELAY);
	RemoveFromResize(IDC_STATIC_TYPE_RELAY);
	
	InitControls();

	OutputToControl();
	
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnCheckClientBreak() 
{
	m_pRelayPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnCheckClientKeep() 
{
	m_pRelayPage->SetModified();
	UpdateData();
	if (m_bClientKeep)
	{
		// wenn  Halten, dann auch Unterbrechen
		m_bClientBreak = TRUE;
	}
	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnSelchangeComboEdge() 
{
	m_pRelayPage->SetModified();
	UpdateData();
	if (m_iEdge==1)
	{
		// also Pegel
		m_bClientBreak = TRUE;
	}
	UpdateData(FALSE);
	
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnChangeEditCloseRelay() 
{
	UpdateData();
	CString sClose = m_sClose;
	int iCurSel = CheckDigits(sClose);
	if (iCurSel>=0)
	{
		m_sClose = sClose;
		UpdateData(FALSE);
		m_editClose.SetSel(iCurSel,iCurSel);
	}
	m_pRelayPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnChangeKommentar() 
{
	// TODO! RKE: Implement CSkinEdit::SetForbiddenCharacters(CString s);
	m_editName.GetWindowText(m_sName);
	int nLength = m_sName.GetLength();
	if (nLength && m_sName.GetAt(nLength-1) == _T('\\'))
	{
		m_sName.SetAt(nLength-1, 0);
		m_editName.SetWindowText(m_sName);
	}
	m_pRelayPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CRelayDialog::OnChangeEditTypeRelay() 
{
	if (m_SpinTypeRelay.m_hWnd == NULL) return; 

	int nPos = m_SpinTypeRelay.GetPos();
	if (nPos >= RELAY_OFF && nPos <= RELAY_ON)
	{
		HICON hIcon = ::ImageList_GetIcon(HIMAGELIST(*((CSVApp*)AfxGetApp())->GetSmallImage()), gm_nList[nPos], ILD_TRANSPARENT);
		m_StaticTypeRelay.SetIcon(hIcon);
		if (nPos == RELAY_OFF)
		{
			if (!m_pRelayPage->CheckActivationsWithOutput(m_pOutput->GetID(), 0, SVP_CO_CHECK_ONLY))
			{
				if (IDYES == AfxMessageBox(IDP_RELAIS_WITH_ACTIVATION, MB_YESNO))
				{
					if (m_pOutput)
					{
						m_pOutput->SetDeleteActivation();
					}
				}
				else
				{
					UpdateData(FALSE);
				}
			}
		}

		ShowHide();
		if (!m_bSetPos)
		{
			m_pRelayPage->SetModified();
		}
	}
}
