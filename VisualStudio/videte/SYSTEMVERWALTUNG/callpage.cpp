/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: callpage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/callpage.cpp $
// CHECKIN:		$Date: 4.05.06 15:40 $
// VERSION:		$Revision: 30 $
// LAST CHANGE:	$Modtime: 4.05.06 15:39 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "systemverwaltung.h"

#include "SVPage.h"

#include "ProcessList.h"
#include "callpage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCallPage dialog
IMPLEMENT_DYNAMIC(CCallPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CCallPage::CCallPage(CSVView* pParent) : CSVPage(CCallPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CCallPage)
	m_sName = _T("");
	m_bHold = TRUE;
	m_bInterval = FALSE;
	m_bForever = FALSE;
	m_sCallTime = _T("0");
	m_sMainTime = _T("30");
	m_sPauseTime = _T("0");
	m_dwConnectionTime = 0;
	//}}AFX_DATA_INIT
	m_pSelectedProcess = NULL;
	m_iSelectedItem = -1;

	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pHostArray = pParent->GetDocument()->GetHosts();
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pPermissionArray = pParent->GetDocument()->GetPermissions();

	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
CCallPage::~CCallPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::ProcessToControl(CProcess* pProcess)
{
	if (pProcess)
	{
		// data from new selection to controls
		int i,j,r,c;
		WORD wHostID;
		CSecID idHost;
		DWORD dw;
		CString s;

		idHost.SetGroupID(SECID_GROUP_HOST);
		wHostID = 0;
		ResetHostList();

		c = pProcess->GetNrOfCall();
		
		for (i=0;i<c && wHostID!=SECID_NO_SUBID;i++)
		{
			wHostID = pProcess->GetCall(i);
			idHost.SetSubID(wHostID);
			// now find the id in m_lbAllHost and move it to m_lbToCallHost
			for (j=0;j<m_lbAllHost.GetCount();j++)
			{
				dw = m_lbAllHost.GetItemData(j);
				if (idHost==dw)
				{
					// found it, move it to m_lbToCallHost
					m_lbAllHost.GetText(j,s);
					m_lbAllHost.DeleteString(j);
					r = m_lbToCallHost.AddString(s);
					if (r!=LB_ERR)
					{
						m_lbToCallHost.SetItemData(r, dw);
					}
					break;
				}
			}
		}

		m_sName		= pProcess->GetName();
		m_bHold		= pProcess->IsHold();
		m_bInterval	= pProcess->IsInterval();
		m_dwConnectionTime = pProcess->GetConnectionTime();

		m_sCallTime.Format(_T("%d"),pProcess->GetCallTime()/1000);
		m_sPauseTime.Format(_T("%d"),pProcess->GetPause()/1000);
		m_sMainTime.Format(_T("%d"),pProcess->GetDuration()/1000);


		if (m_bInterval && m_sMainTime==_T("0")) 
		{
			m_bForever = TRUE;
		}
		else 
		{
			m_bForever	= FALSE;
		}

		dw = pProcess->GetPermission().GetID();
		m_cbPermissions.SetCurSel(-1);
		c = m_cbPermissions.GetCount();
		for (i=0;i<c;i++)
		{
			if (m_cbPermissions.GetItemData(i)==dw)
			{
				m_cbPermissions.SetCurSel(i);
				break;
			}
		}
	}
	else 
	{
		// empty all
		m_sName.Empty();
		m_sCallTime = _T("0");
		m_bHold = FALSE;
		m_bInterval = FALSE;
		m_sPauseTime = _T("0");
		m_sMainTime = _T("0");
		m_bForever = FALSE;
		m_cbPermissions.SetCurSel(-1);
		ResetHostList();
	}
	UpdateData(FALSE);

	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::ControlToProcess(CProcess* pProcess)
{
	if (pProcess)
	{
		// data from controls to pProcess
		UpdateData();
		if ( m_bInterval && !m_bHold && !m_bForever && m_sMainTime==_T("0")) 
		{
			AfxMessageBox(IDP_INTERVAL_NO_FOREVER_NOR_MAINTIME, MB_OK);
			m_bForever = TRUE;
			UpdateData(FALSE);
			ShowHide();
		}

		int i;
		CSecID idHost;
		pProcess->ClearCalls();

		// get hosts from m_lbToCallHost and copy to pProcess
		for (i=0; i<m_lbToCallHost.GetCount();i++)
		{
			idHost = (DWORD)m_lbToCallHost.GetItemData(i);
			pProcess->AddCall(idHost.GetSubID());
		}

		i = m_cbPermissions.GetCurSel();
		if (i!=CB_ERR)
		{
			DWORD dw = m_cbPermissions.GetItemData(i);
			pProcess->SetCall(m_sName,
							  m_bForever ? 0L : 1000 * _ttoi(m_sMainTime),
							  1000 * _ttoi(m_sPauseTime),
							  1000 * _ttoi(m_sCallTime),
							  m_bHold,
							  m_bInterval,
							  dw,
							  m_dwConnectionTime);
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::ShowHide() 
{
	if ( m_pSelectedProcess ) 
	{
		m_editName.EnableWindow(TRUE);
		m_lbProcess.EnableWindow(TRUE);
		m_editCallTime.EnableWindow(TRUE);
		m_btnHold.EnableWindow(TRUE);
		m_btnInterval.EnableWindow(TRUE);
		m_lbAllHost.EnableWindow(TRUE);
		m_lbToCallHost.EnableWindow(TRUE);
		m_cbPermissions.EnableWindow(TRUE);
		m_editMCT.EnableWindow(TRUE);
	}
	else 
	{
		m_bInterval = FALSE;
		m_editName.EnableWindow(FALSE);
		m_lbProcess.EnableWindow(FALSE);
		m_editCallTime.EnableWindow(FALSE);
		m_btnHold.EnableWindow(FALSE);
		m_btnInterval.EnableWindow(FALSE);
		m_lbAllHost.EnableWindow(FALSE);
		m_lbToCallHost.EnableWindow(FALSE);
		m_cbPermissions.EnableWindow(FALSE);
		m_editMCT.EnableWindow(FALSE);
	}

	if (m_bInterval) 
	{
		m_sSec1.EnableWindow(TRUE);
		m_sSec2.EnableWindow(TRUE);
		m_btnMainTime.EnableWindow(TRUE);
		m_btnIntervalGrp.EnableWindow(TRUE);
		m_editPauseTime.EnableWindow(TRUE);
		m_editMainTime.EnableWindow(TRUE);
		m_sTxtPause.EnableWindow(TRUE);
		m_sTxtNotForever.EnableWindow(TRUE);
		m_btnForever.EnableWindow(!m_bHold);

		m_btnHold.EnableWindow(!m_bForever);
		m_editMainTime.EnableWindow(!m_bForever);
		m_sTxtNotForever.EnableWindow(!m_bForever);
	}
	else 
	{
		m_sPauseTime = _T("0");
		m_bForever = FALSE;
		m_sMainTime = _T("0");
		m_sSec1.EnableWindow(FALSE);
		m_sSec2.EnableWindow(FALSE);
		m_btnForever.EnableWindow(FALSE);
		m_btnMainTime.EnableWindow(FALSE);
		m_btnIntervalGrp.EnableWindow(FALSE);
		m_editPauseTime.EnableWindow(FALSE);
		m_editMainTime.EnableWindow(FALSE);
		m_sTxtPause.EnableWindow(FALSE);
		m_sTxtNotForever.EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallPage::IsDataValid()
{
	if (m_pSelectedProcess) 
	{
		UpdateData();
		if (m_bHold && m_bInterval) 
		{
			if (m_bForever) 
			{
				AfxMessageBox(IDP_HOLD_INTERVAL_FOREVER, MB_OK);
				return FALSE;
			}
			else if (m_sMainTime.IsEmpty()) 
			{
				AfxMessageBox(IDP_HOLD_INTERVAL_NOTIME, MB_OK);
				return FALSE;
			}
		}

		int nSelected = m_lbToCallHost.GetCount();
		if (nSelected<1 || nSelected>MAX_SELECTED_HOSTS) 
		{
			CString sMsg;
			sMsg.Format(IDP_WRONG_SELECTED_HOSTS, MAX_SELECTED_HOSTS);
			AfxMessageBox(sMsg, MB_OK);
			m_lbToCallHost.SetFocus();
			return FALSE;
		}
		if (CB_ERR==m_cbPermissions.GetCurSel())
		{
			AfxMessageBox(IDP_GUARDTOUR_PERMISSION_INVALID);
			m_cbPermissions.SetFocus();
			return FALSE;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::ClearListBoxProcess() 
{
	m_lbProcess.ResetContent();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::FillListBoxProcess()
{
	ClearListBoxProcess();
	CProcess* pProcess;
	int iIndex = -1;
	for (int i=0;i<m_pProcessList->GetSize();i++) 
	{
		pProcess = m_pProcessList->GetAt(i);
		if (pProcess && pProcess->IsCall()) 
		{
			iIndex = m_lbProcess.AddString(pProcess->GetName());
			if (iIndex != -1) 
			{
				m_lbProcess.SetItemDataPtr(iIndex, (void*)pProcess);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CCallPage::InsertHost(CHost* pHost)
{
	CString	sHostName;
	CString sHostNumber;
	int		iIndex =0;

	sHostName = pHost->GetName();
	sHostNumber = pHost->GetNumber();
	if (pHost->GetID()!=SECID_LOCAL_HOST)
	{
		sHostName += _T(" ") + pHost->GetTyp();
	}
	else
	{
		CString l;
		l.LoadString(IDS_LOCAL_HOST);
		sHostName = l + _T(" ") + sHostName;
	}

	iIndex = m_lbAllHost.AddString(sHostName);
	if ( iIndex != LB_ERR ) 
	{
		m_lbAllHost.SetItemData(iIndex, pHost->GetID().GetID());
	}
	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::ResetHostList()
{
	m_lbAllHost.ResetContent();
	CHost*	pHost;
	InsertHost(m_pHostArray->GetLocalHost());
	for (int i=0; i<m_pHostArray->GetSize(); i++)
	{
		pHost = (CHost*)m_pHostArray->GetAt(i);
		if (pHost) 
		{
			InsertHost(pHost);
		}
	}
	m_lbToCallHost.ResetContent();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::SetStandardEntriesForHostToCall(int iIndex)
{
	if (iIndex < m_lbToCallHost.GetCount() ) {
		CSecID id = (CSecID)m_lbToCallHost.GetItemData(iIndex);
		CHost*	pHost = m_pHostArray->GetHost(id);
		if (pHost) {
			// Standard-Vorgaben fuer SMS
			if (pHost->GetTyp() == _T("SMS")) {
				m_bHold = TRUE;
				m_bInterval = TRUE;
				m_bForever = FALSE;
				m_sCallTime = _T("70");
				m_sMainTime = _T("490");
				m_sPauseTime = _T("10");
			}
			UpdateData(FALSE);
			ShowHide();
		}
	}
	else {
		WK_TRACE_ERROR(_T("%s index %i > ToCallHost members\n"), __FILE__, iIndex);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallPage)
	DDX_Control(pDX, IDC_EDIT_MCT, m_editMCT);
	DDX_Control(pDX, IDC_COMBO_PERMISSION_ISDN, m_cbPermissions);
	DDX_Control(pDX, IDC_ISDN_LIST_HOST_ALL, m_lbAllHost);
	DDX_Control(pDX, IDC_ISDN_INTERVAL, m_btnInterval);
	DDX_Control(pDX, IDC_ISDN_HOLD, m_btnHold);
	DDX_Control(pDX, IDC_CALLTIME, m_editCallTime);
	DDX_Control(pDX, IDC_TXTPAUSE, m_sTxtPause);
	DDX_Control(pDX, IDC_ISDN_PAUSETIME, m_editPauseTime);
	DDX_Control(pDX, IDC_ISDN_LIST_HOST_CALL, m_lbToCallHost);
	DDX_Control(pDX, IDC_INTERVALGRP, m_btnIntervalGrp);
	DDX_Control(pDX, IDC_GESTIMEGRP, m_btnMainTime);
	DDX_Control(pDX, IDC_ISDN_FOREVER, m_btnForever);
	DDX_Control(pDX, IDC_TXT_NOTFOREVER, m_sTxtNotForever);
	DDX_Control(pDX, IDC_ISDN_MAINTIME, m_editMainTime);
	DDX_Control(pDX, IDC_TXT_SEC2, m_sSec2);
	DDX_Control(pDX, IDC_TXT_SEC1, m_sSec1);
	DDX_Control(pDX, IDC_ISDN_NAME, m_editName);
	DDX_Control(pDX, IDC_ISDN_LIST_PROCESS, m_lbProcess);
	DDX_Text(pDX, IDC_ISDN_NAME, m_sName);
	DDX_Check(pDX, IDC_ISDN_HOLD, m_bHold);
	DDX_Check(pDX, IDC_ISDN_INTERVAL, m_bInterval);
	DDX_Check(pDX, IDC_ISDN_FOREVER, m_bForever);
	DDX_Text(pDX, IDC_CALLTIME, m_sCallTime);
	DDX_Text(pDX, IDC_ISDN_MAINTIME, m_sMainTime);
	DDX_Text(pDX, IDC_ISDN_PAUSETIME, m_sPauseTime);
	DDX_Text(pDX, IDC_EDIT_MCT, m_dwConnectionTime);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCallPage, CSVPage)
	//{{AFX_MSG_MAP(CCallPage)
	ON_LBN_SELCHANGE(IDC_ISDN_LIST_PROCESS, OnSelchangeListproc)
	ON_BN_CLICKED(IDC_ISDN_INTERVAL, OnInterval)
	ON_BN_CLICKED(IDC_ISDN_FOREVER, OnForever)
	ON_EN_CHANGE(IDC_CALLTIME, OnChangeCalltime)
	ON_BN_CLICKED(IDC_ISDN_HOLD, OnHold)
	ON_EN_CHANGE(IDC_ISDN_PAUSETIME, OnChangePausetime)
	ON_EN_CHANGE(IDC_ISDN_MAINTIME, OnChangeMaintime)
	ON_EN_CHANGE(IDC_ISDN_NAME, OnChangeName)
	ON_LBN_DBLCLK(IDC_ISDN_LIST_HOST_CALL, OnDblclkIsdnListHostCall)
	ON_LBN_DBLCLK(IDC_ISDN_LIST_HOST_ALL, OnDblclkIsdnListHostAll)
	ON_CBN_SELCHANGE(IDC_COMBO_PERMISSION_ISDN, OnSelchangeComboPermission)
	ON_LBN_SELCHANGE(IDC_ISDN_LIST_HOST_ALL, OnSelchangeListHostAll)
	ON_EN_CHANGE(IDC_EDIT_MCT, OnChangeEditMct)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCallPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CCallPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	ResetHostList();
	FillListBoxProcess();
	FillPermissions();
	if (m_lbProcess.GetCount()>0)
	{
		m_iSelectedItem = m_lbProcess.SetCurSel(0);
		m_pSelectedProcess = (CProcess*)m_lbProcess.GetItemDataPtr(m_iSelectedItem);
		ProcessToControl(m_pSelectedProcess);
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedProcess = NULL;
		ProcessToControl(NULL);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::FillPermissions() 
{
	m_cbPermissions.ResetContent();

	int i,j,c;
	CString s;
	DWORD dw;
	CPermission* pPermission;

	c = m_pPermissionArray->GetSize();
	
	for (i=0; i<c; i++)
	{
		pPermission = (CPermission*)m_pPermissionArray->GetAt(i);
		s = pPermission->GetName();
		dw = pPermission->GetID().GetID();
		j = m_cbPermissions.AddString(s);
		m_cbPermissions.SetItemData(j,dw);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallPage::CanNew()
{
	return (m_pInputList->GetNrOfActiveInputs() >0 );
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallPage::CanDelete()
{
	return m_lbProcess.GetCount()>0;
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnNew() 
{
	if (m_pSelectedProcess)
	{
		if (IsDataValid())
		{
			ControlToProcess(m_pSelectedProcess);
		}
		else
		{
			m_lbProcess.SetCurSel(m_iSelectedItem);
			return;
		}
	}
	m_pSelectedProcess = m_pProcessList->AddProcess();
	CString s;

	s.LoadString(IDS_NEW);
	m_pSelectedProcess->SetCall(s,0,0,30000,TRUE,FALSE,SECID_SUPERVISOR_PERMISSION,0);
	m_sName = s;
	UpdateData(FALSE);
	SetModified();
	m_iSelectedItem = m_lbProcess.AddString(m_sName);
	m_lbProcess.SetCurSel(m_iSelectedItem);
	ProcessToControl(m_pSelectedProcess);
	ShowHide();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnDelete() 
{
	if (m_pSelectedProcess)
	{
		// first check wether there is any Activation (CInputToOutput) with or 
		// process macro
		CInputGroup* pInputGroup;
		CInput* pInput;
		CInputToOutput* pInputToOutput;
		int i,j,k,c,d,e;
		BOOL bShowMB = TRUE;

		c = m_pInputList->GetSize();
		for (i=0;i<c;i++)
		{
			pInputGroup = m_pInputList->GetGroupAt(i);
			d = pInputGroup->GetSize();
			for (j=0;j<d;j++)
			{
				pInput = pInputGroup->GetInput(j);
				e = pInput->GetNumberOfInputToOutputs();
				for (k=e-1;k>=0;k--)
				{
					pInputToOutput = pInput->GetInputToOutput(k);
					if (pInputToOutput->GetProcessID() == m_pSelectedProcess->GetID())
					{
						// we still have one
						if (bShowMB)
						{
							if (IDYES==AfxMessageBox(IDP_DELETE_PROCESS_WITH_ACTIVATION,MB_YESNO))
							{
								bShowMB = FALSE;
							}
							else
							{
								return;
							}
						}
						pInput->DeleteInputToOutput(pInputToOutput);
					}
				} // inputstooutputs
			} // inputs
		}//groups

		if (bShowMB==FALSE)
		{
			m_pInputList->Save(GetProfile(),FALSE);
		}

		m_pProcessList->DeleteProcess(m_pSelectedProcess);
		m_pSelectedProcess = NULL;
		m_lbProcess.DeleteString(m_iSelectedItem);

		int iCount = m_lbProcess.GetCount();
		if (iCount>0)
		{
			if (m_iSelectedItem==iCount) 
			{
				m_iSelectedItem = m_lbProcess.SetCurSel(m_iSelectedItem-1);
			}
			else 
			{
				m_iSelectedItem = m_lbProcess.SetCurSel(m_iSelectedItem);
			}
			m_pSelectedProcess = (CProcess*)m_lbProcess.GetItemDataPtr(m_iSelectedItem);
			ProcessToControl(m_pSelectedProcess);
		}
		else
		{
			ProcessToControl(NULL);
		}
		SetModified();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnSelchangeListproc() 
{
	int iCurSel = m_lbProcess.GetCurSel();
	if (iCurSel!=m_iSelectedItem)
	{
		if (IsDataValid())
		{
			ControlToProcess(m_pSelectedProcess);
			m_iSelectedItem = iCurSel;
			m_pSelectedProcess = (CProcess*)m_lbProcess.GetItemDataPtr(m_iSelectedItem);
			ProcessToControl(m_pSelectedProcess);

			// Change ML: Wenn der unterste Entrag in der Liste gewählt wird,
			//  dann die Liste automatisch um einen Eintrag nach oben scrollen.
			CRect ItemRect, ListRect;
			m_lbProcess.GetItemRect(0, ItemRect);
			m_lbProcess.GetClientRect(ListRect);
			int nVisibleEntries = ListRect.Height() / ItemRect.Height();
			if (iCurSel - m_lbProcess.GetTopIndex() >= nVisibleEntries -1)
			{
				if (m_lbProcess.GetTopIndex() + nVisibleEntries < m_lbProcess.GetCount())
				{
					m_lbProcess.SetTopIndex(m_lbProcess.GetTopIndex( ) + 1 ); 	
				}
			}	
		}
		else
		{
			m_lbProcess.SetCurSel(m_iSelectedItem);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnChangeName() 
{
	SetModified();
	if( m_pSelectedProcess && UpdateData()) 
	{
		if ( m_sName.GetLength() ) 
		{
			m_lbProcess.DeleteString(m_iSelectedItem);
			m_lbProcess.InsertString(m_iSelectedItem, m_sName);
			m_lbProcess.SetItemDataPtr(m_iSelectedItem, m_pSelectedProcess);
			m_iSelectedItem = m_lbProcess.SetCurSel(m_iSelectedItem);
		}
		else 
		{
			// gf todo
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnChangeCalltime() 
{
	UpdateData();
	CString sCallTime = m_sCallTime;
	int iCurSel = CheckDigits(sCallTime);
	if (iCurSel>=0)
	{
		m_sCallTime = sCallTime;
		UpdateData(FALSE);
		m_editCallTime.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnHold() 
{
	UpdateData();
	ShowHide();
	SetModified();
	if (!m_bHold && m_bInterval && !m_bForever && m_sMainTime==_T("0")) 
	{
		m_editMainTime.SetFocus();
		m_editMainTime.SetSel(0,-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnInterval() 
{
	UpdateData();
	ShowHide();
	SetModified();
	if (!m_bInterval && m_bHold && !m_bForever && m_sMainTime==_T("0")) 
	{
		m_editMainTime.SetFocus();
		m_editMainTime.SetSel(0,-1);
	}
	else 
	{
		m_bHold = FALSE;
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnChangePausetime() 
{
	UpdateData();
	CString sPauseTime = m_sPauseTime;
	int iCurSel = CheckDigits(sPauseTime);
	if (iCurSel>=0)
	{
		m_sPauseTime = sPauseTime;
		UpdateData(FALSE);
		m_editPauseTime.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnForever() 
{
	UpdateData();
	ShowHide();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnChangeMaintime() 
{
	UpdateData();
	CString sMainTime = m_sMainTime;
	int iCurSel = CheckDigits(sMainTime);
	if (iCurSel>=0)
	{
		m_sMainTime = sMainTime;
		UpdateData(FALSE);
		m_editMainTime.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::SaveChanges() 
{
	WK_TRACE_USER(_T("Anruf-Prozess-Einstellungen wurden geändert\n"));
	UpdateData();
	ControlToProcess(m_pSelectedProcess);
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::CancelChanges() 
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());
	ClearListBoxProcess();
	FillListBoxProcess();
	int iCount = m_lbProcess.GetCount();
	if (iCount>0)
	{
		if (m_iSelectedItem==iCount) 
		{
			m_iSelectedItem = m_lbProcess.SetCurSel(m_iSelectedItem-1);
		}
		else 
		{
			m_iSelectedItem = m_lbProcess.SetCurSel(m_iSelectedItem);
		}
		m_pSelectedProcess = (CProcess*)m_lbProcess.GetItemDataPtr(m_iSelectedItem);
		ProcessToControl(m_pSelectedProcess);
	}
	else
	{
		m_pSelectedProcess = NULL;
		m_iSelectedItem = -1;
		ProcessToControl(NULL);
	}
	SetModified(FALSE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnDblclkIsdnListHostCall() 
{
	int i;
	DWORD dw;
	CString s;
	
	i = m_lbToCallHost.GetCurSel();
	if (i!=LB_ERR)
	{
		dw = m_lbToCallHost.GetItemData(i);
		m_lbToCallHost.GetText(i,s);
		m_lbToCallHost.DeleteString(i);
		i = m_lbAllHost.AddString(s);
		if (i!=LB_ERR)
		{
			m_lbAllHost.SetItemData(i, dw);
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnDblclkIsdnListHostAll() 
{
	int i/*,c*/;
	DWORD dw;
	CString s;
	
	i = m_lbAllHost.GetCurSel();
	if (i!=LB_ERR)
	{
//		c = m_lbToCallHost.GetCount();
//		if (c==0)
//		{
			dw = m_lbAllHost.GetItemData(i);
			m_lbAllHost.GetText(i,s);
			m_lbAllHost.DeleteString(i);
			i = m_lbToCallHost.AddString(s);
			if (i!=LB_ERR)
			{
				m_lbToCallHost.SetItemData(i, dw);
				SetStandardEntriesForHostToCall(i);
			}
			SetModified();
//		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallPage::OnSelchangeComboPermission() 
{
	if (m_pSelectedProcess)
	{
		int i = m_cbPermissions.GetCurSel();
		if (i!=CB_ERR)
		{
			DWORD dw = m_cbPermissions.GetItemData(i);
			m_pSelectedProcess->SetPermission(dw);
			SetModified();
		}
	}
}

void CCallPage::OnSelchangeListHostAll() 
{
	// Change ML: Wenn der unterste Entrag in der Liste gewählt wird,
	//  dann die Liste automatisch um einen Eintrag nach oben scrollen.
	CRect ItemRect, ListRect;
	m_lbAllHost.GetItemRect(0, ItemRect);
	m_lbAllHost.GetClientRect(ListRect);
	int nVisibleEntries = ListRect.Height() / ItemRect.Height();

	if (m_lbAllHost.GetCaretIndex() - m_lbAllHost.GetTopIndex( ) >= nVisibleEntries -1 )
	{
		if (m_lbAllHost.GetTopIndex() + nVisibleEntries < m_lbAllHost.GetCount())
		{
			m_lbAllHost.SetTopIndex(m_lbAllHost.GetTopIndex( ) + 1 ); 
		}
	}
}

void CCallPage::OnChangeEditMct() 
{
	SetModified();
}
