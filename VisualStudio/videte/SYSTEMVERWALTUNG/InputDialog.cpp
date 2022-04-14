// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "systemverwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#include "InputDialog.h"
#include "InputPage.h"

#include "InformationDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInputDialog dialog

int CInputDialog::gm_nList[3] = {IMAGE_NULL, IMAGE_INPUT, IMAGE_LOCK};

CInputDialog::CInputDialog(CInputPage* pInputPage, CInput* pInput)
	: CWK_Dialog(CInputDialog::IDD)
{
	m_pInputPage = pInputPage;
	m_pInput = pInput;
	m_iTypePos = INPUT_OFF;
/*
   m_hOff = NULL;
	m_hMelder = NULL;
	m_hBlock = NULL;
*/
	//{{AFX_DATA_INIT(CInputDialog)
	m_bEdge = FALSE;
	m_bPushButton = FALSE;
	m_sName = _T("");
	m_sNr = _T("");
	m_bTempDeactivate = TRUE;
	//}}AFX_DATA_INIT
	Create(IDD,pInputPage);
}


void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDialog)
	DDX_Control(pDX, IDC_STATIC_TYPE_INPUT, m_StaticTypeInput);
	DDX_Control(pDX, IDC_SPIN_TYPE_INPUT, m_SpinTypeInput);
	DDX_Control(pDX, IDC_DEACTIVATE, m_btnTempDeactivate);
	DDX_Control(pDX, IDC_INSTRUCTION, m_btnInstruction);
	DDX_Control(pDX, IDC_EDGE, m_btnEdge);
	DDX_Control(pDX, IDC_NAME_INPUT, m_editName);
	DDX_Check(pDX, IDC_EDGE, m_bEdge);
	DDX_Text(pDX, IDC_NAME_INPUT, m_sName);
	DDV_MaxChars(pDX, m_sName, 64);
	DDX_Text(pDX, IDC_NR_INPUT, m_sNr);
	DDX_Check(pDX, IDC_DEACTIVATE, m_bTempDeactivate);
	//}}AFX_DATA_MAP
	if (GetDlgItem(IDC_PUSHBUTTON))
	{
		DDX_Check(pDX, IDC_PUSHBUTTON, m_bPushButton);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputDialog::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CInputDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CInputDialog)
	ON_EN_CHANGE(IDC_NAME_INPUT, OnChangeName)
	ON_BN_CLICKED(IDC_EDGE, OnChange)
	ON_BN_CLICKED(IDC_INSTRUCTION, OnInstruction)
	ON_BN_CLICKED(IDC_DEACTIVATE, OnChange)
	ON_BN_CLICKED(IDC_PUSHBUTTON, OnChange)
	ON_EN_CHANGE(IDC_EDIT_TYPE_INPUT, OnChangeEditTypeInput)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
//	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDialog message handlers

void CInputDialog::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::OnCancel() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

//	RemoveFromResize(IDC_FREIGABE);     // Workaround
	RemoveFromResize(IDC_STATIC_TYPE_INPUT);     // Workaround
	RemoveFromResize(IDC_EDIT_TYPE_INPUT);
	RemoveFromResize(IDC_SPIN_TYPE_INPUT);     // Workaround
	RemoveFromResize(IDC_NR_INPUT);     // Workaround für das Resizen
	
	CInputGroup *pInputGroup = m_pInputPage->m_pInputGroup;

	// ML Die Namen der virtuellen Alarme dürfen nicht editierbar sein 
	if (pInputGroup && pInputGroup->GetSMName() == SM_LAUNCHER_INPUT)
		m_editName.EnableWindow(FALSE);

	InitTypeTree();

	// gf todo when must the temporary deactivated box be visible
	if (m_pInputPage->GetDongle().AllowAlarmOffSpans())
	{
		m_btnTempDeactivate.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnTempDeactivate.EnableWindow(FALSE);
		m_btnTempDeactivate.ShowWindow(SW_HIDE);
	}

	InputToControl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::SetInput(CInput* pInput)
{
	m_pInput = pInput;
	InputToControl();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::InputToControl()
{
	m_sNr.Format(_T("%d"),m_pInput->GetID().GetSubID()+1);
	m_sName = m_pInput->GetName();

	CSecID aID = m_pInput->GetIDActivate();
	if (aID==SECID_ACTIVE_ON)
	{
		m_iTypePos = INPUT_MELDER;
	}
	else if (aID==SECID_ACTIVE_LOCK)
	{
		m_iTypePos = INPUT_BLOCK;
	}
	else // SECID_ACTIVE_OFF
	{
		m_iTypePos = INPUT_OFF;
	}
	m_SpinTypeInput.SetPos(m_iTypePos);

	m_bEdge = m_pInput->GetEdge();
	m_bPushButton = m_pInput->GetPushButton();
	m_bTempDeactivate = m_pInput->GetTempDeactivate();

	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::ControlToInput()
{
	UpdateData();

//   HTREEITEM hSelectedItem = m_Type.GetFirstVisibleItem();
	int nPos = m_SpinTypeInput.GetPos();

	m_pInput->SetName(m_sName);
	m_pInput->SetEdge(m_bEdge);
	m_pInput->SetPushButton(m_bPushButton);
	m_pInput->SetTempDeactivate(m_bTempDeactivate);

	if (nPos == INPUT_MELDER)
	{
		m_pInput->SetIDActivate(SECID_ACTIVE_ON);
	}
	else if (nPos == INPUT_BLOCK)
	{
		m_pInput->SetIDActivate(SECID_ACTIVE_LOCK);
	}
	else // INPUT_OFF
	{
		m_pInput->SetIDActivate(SECID_ACTIVE_OFF);
	}
/*
   if (hSelectedItem == m_hMelder)
	{
		m_pInput->SetIDActivate(SECID_ACTIVE_ON);
	}
	else if (hSelectedItem == m_hBlock)
	{
		m_pInput->SetIDActivate(SECID_ACTIVE_LOCK);
	}
	else
	{
		m_pInput->SetIDActivate(SECID_ACTIVE_OFF);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::InitTypeTree()
{
	CInputGroup *pInputGroup = m_pInputPage->m_pInputGroup;
   int nRange = INPUT_MELDER;
	// Virtuelle Alarme sollen kein Blockschloß sein.
   /*
	if (   pInputGroup 
#ifndef _DEBUG	// TODO! RKE: test virtual alarm with switch lock
		&& pInputGroup->GetSMName() != SM_LAUNCHER_INPUT
#endif
		)*/
	{
		// MD Melder sollen kein Blockschloß sein.
		if (pInputGroup && pInputGroup->IsMD())
			nRange = INPUT_MELDER;
		else
			nRange = INPUT_BLOCK;
	}


	m_SpinTypeInput.SetRange32(nRange, INPUT_OFF);
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::ShowHide()
{
	CInputGroup *pInputGroup = m_pInputPage->m_pInputGroup;

//	HTREEITEM hSelectedItem = m_Type.GetFirstVisibleItem();
   int nPos = m_SpinTypeInput.GetPos();
	// Elemente nur enablen, wenn Input nicht AUS geschaltet
//	BOOL bEnable = (hSelectedItem != m_hOff);
	BOOL bEnable = (nPos != INPUT_OFF);
	BOOL bExtraEnableName = FALSE;
	BOOL bExtraEnableEdge = FALSE;
	// Namen nur editierbar, wenn
	if (pInputGroup)
	{
		CString sSMName = pInputGroup->GetSMName();
		CString sAudioInput = SM_AudioUnitInput;
		sAudioInput = sAudioInput.Left(sAudioInput.GetLength() -2);
		bExtraEnableName = (   sSMName != SM_LAUNCHER_INPUT		// kein virtueller Alarm
			                 && sSMName.Find(sAudioInput) == -1);	// kein AudioUnit Melder

		bExtraEnableEdge = (   sSMName != SM_LAUNCHER_INPUT		// kein virtueller Alarm
			                 && sSMName.Find(SM_SDIUNIT_INPUT) == -1);// kein SIDUnit Melder
	}
	// Oeffner nur enablen, wenn kein virtueller Alarm, keine SDI
	
	m_editName.EnableWindow(bEnable && bExtraEnableName);
	m_btnEdge.EnableWindow(bEnable && bExtraEnableEdge);

	m_btnInstruction.EnableWindow(bEnable);

	// are alarm off spans allowed
	if (m_pInputPage->GetDongle().AllowAlarmOffSpans())
	{
		// only SuperVisor is allowed to change the state of temporary deactivation allowed
		m_btnTempDeactivate.EnableWindow(theApp.m_pPermission->IsSuperVisor());
	}

	int iShow = SW_SHOW;	
/*
	if (   pInputGroup->GetSMName() == SM_LAUNCHER_INPUT
		&& m_pInput->GetID().GetSubID() == BIT_VALARM_ALARM_OFF_SPAN)
	{
		if (!m_pInputPage->GetDongle().AllowAlarmOffSpans())
		{
			iShow = SW_HIDE;
		}
	}
*/	
	m_btnEdge.ShowWindow(iShow);
	m_btnInstruction.ShowWindow(iShow);
	m_editName.ShowWindow(iShow);
	m_SpinTypeInput.ShowWindow(iShow);
	m_StaticTypeInput.ShowWindow(iShow);

	CWnd *pWnd = GetDlgItem(IDC_PUSHBUTTON);
	if (pWnd)
	{
		pWnd->ShowWindow(m_pInput->CanPushButton() ? SW_SHOW : SW_HIDE);
		pWnd->EnableWindow(bEnable);
	}
//	m_Type.ShowWindow(iShow);
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::OnChangeName() 
{
	// TODO! RKE: Implement CSkinEdit::SetForbiddenCharacters(CString s);
	m_editName.GetWindowText(m_sName);
	int nLength = m_sName.GetLength();
	if (nLength && m_sName.GetAt(nLength-1) == _T('\\'))
	{
		m_sName.SetAt(nLength-1, 0);
		m_editName.SetWindowText(m_sName);
	}
	m_pInputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::OnChange() 
{
	m_pInputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::OnInstruction() 
{
	CInformationDialog dlg(this);

	if (!m_pInput->m_sEditor.IsEmpty())
	{
		dlg.m_sEditor = m_pInput->m_sEditor;
	}
	else
	{
		dlg.m_sEditor = theApp.m_pUser->GetName();
	}
	dlg.m_sInformation = m_pInput->m_sInformation;
	dlg.m_sObject = m_pInput->GetName();

	if (IDOK == dlg.DoModal())
	{
		m_pInput->m_sEditor		  = dlg.m_sEditor;
		m_pInput->m_sInformation = dlg.m_sInformation;
		m_pInput->m_tInstructionStampNew = CTime::GetCurrentTime();
		m_pInputPage->SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::OnChangeEditTypeInput() 
{
	if (m_SpinTypeInput.m_hWnd == NULL) return;
	int nPos = m_SpinTypeInput.GetPos();

	if (nPos==INPUT_OFF)
	{
		// check the activations !!
		if (m_pInput->GetNumberOfInputToOutputs())
		{
			CString sMsg, sFormat;
			sFormat.LoadString(IDS_DELETE_INPUT);
			sMsg.Format(sFormat, m_pInput->GetName());
			if (IDYES == AfxMessageBox(sMsg, MB_YESNO))
			{
				m_pInput->SetDeleteInputToOutputs(m_pInput->GetID());
			}
			else
			{
				m_SpinTypeInput.SetPos(m_iTypePos);
				nPos = m_SpinTypeInput.GetPos();
			}
		}
	}
	ShowHide();
	if (nPos != m_iTypePos)
	{
		m_iTypePos = nPos;
		m_pInputPage->SetModified();
	}
	m_StaticTypeInput.InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CInputDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDIS) 
{
   if (nIDCtl == IDC_STATIC_TYPE_INPUT)
   {
		int nPos = m_SpinTypeInput.GetPos();
		::SaveDC(pDIS->hDC);
//		::FillRect(pDIS->hDC, &pDIS->rcItem,GetSysColorBrush((nIDCtl == m_nSelectedControl) ? COLOR_INFOBK : COLOR_BTNFACE));
		HBRUSH hbr = (HBRUSH) ::GetWindowLong(m_hWnd, DWL_MSGRESULT);
		if (hbr)
		{
			::FillRect(pDIS->hDC, &pDIS->rcItem, hbr);
		}
		::ImageList_Draw(HIMAGELIST(*((CSVApp*)AfxGetApp())->GetSmallImage()), gm_nList[nPos], pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, ILD_TRANSPARENT);
		CString str;
		if (nPos)
		{
			str.LoadString((nPos == INPUT_MELDER) ? IDS_ON : IDS_LOCK);
			pDIS->rcItem.left+=30;
			::SetBkMode(pDIS->hDC, TRANSPARENT);
			::DrawText(pDIS->hDC, str, str.GetLength(), &pDIS->rcItem, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
		::RestoreDC(pDIS->hDC, -1);
	}
	else
	{
		CWK_Dialog::OnDrawItem(nIDCtl, pDIS);
	}
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CInputDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWK_Dialog::OnCtlColor(pDC, pWnd, nCtlColor);
	::SetWindowLong(m_hWnd, DWL_MSGRESULT, (long)hbr);
	return hbr;
}
