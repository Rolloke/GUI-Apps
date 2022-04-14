/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: HostPage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/HostPage.cpp $
// CHECKIN:		$Date: 8.08.06 12:13 $
// VERSION:		$Revision: 82 $
// LAST CHANGE:	$Modtime: 8.08.06 12:11 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "MainFrm.h"
#include "HostPage.h"

#include "SVDoc.h"
#include "SVView.h"
#include "CipcServerControl.h"
#include ".\hostpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CHostPage property page
IMPLEMENT_DYNAMIC(CHostPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CHostPage::CHostPage(CSVView* pParent) : CSVPage(CHostPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CHostPage)
	m_sName = _T("");
	m_sNumber = _T("");
	m_sStationsName = _T("");
	m_bPINRequired = FALSE;
	m_sPIN = _T("");
	m_dwDisconnect = 0;
	//}}AFX_DATA_INIT
	
	// data structures
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_bProcessesChanged = FALSE;
	m_pArchivInfoArray = pParent->GetDocument()->GetArchivs();
	m_bArchivesChanged = FALSE;
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_bInputsChanged = FALSE;
	m_pDrives = pParent->GetDocument()->GetDrives();

	m_pHostArray = pParent->GetDocument()->GetHosts();
	m_pSelectedHost = NULL;
	m_iSelectedItem = -1;
	m_iSortColumn	= 0;
	m_iMaxHosts = GetDongle().GetNrOfHosts();

	m_iISDNIndex = -2;
	m_iTCPIPIndex = -2;
	m_iVCSIndex = -2;
	m_iPresIndex = -2;
	m_iSMSIndex = -2;
	m_iEMailIndex = -2;
	m_iFAXIndex = -2;
	m_iTOBSIndex = -2;
	
	m_bISDN = GetDongle().RunISDNUnit();
	m_bTCPIP = GetDongle().RunSocketUnit();
	m_bVCS = GetDongle().RunVCSUnit();
	m_bPres = GetDongle().RunPTUnit();
	m_bSMS = GetDongle().RunSMSUnit();
	m_bEMail = GetDongle().RunEMailUnit();
	m_bFAX = GetDongle().RunFAXUnit();
	m_bTOBS = GetDongle().RunTOBSUnit();

	InternalProductCode ipc = GetDongle().GetProductCode();

	m_bPINAccessable = (   (ipc == IPC_INSPECTUS)
						|| (ipc == IPC_INSPICIO)
						|| (ipc == IPC_IDIP_PROJECT)
						|| (ipc == IPC_IDIP_4)
						|| (ipc == IPC_IDIP_8)
						|| (ipc == IPC_IDIP_16)
						|| (ipc == IPC_IDIP_32)
						|| (ipc == IPC_IDIP_XL_4)
						|| (ipc == IPC_IDIP_XL_8)
						|| (ipc == IPC_IDIP_XL_16)
						|| (ipc == IPC_IDIP_XL_24)
						|| (ipc == IPC_IDIP_XL_32)
						|| (ipc == IPC_IDIP_ML)
						|| (ipc == IPC_IDIP_IP)
						);

	Create(IDD,(CWnd*)m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
CHostPage::~CHostPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::EnableExceptNew()
{
	BOOL bEnable, bEnableItem;
	int c;
	c = m_Hosts.GetItemCount();
	bEnable = (c>0);
	m_editName.EnableWindow(bEnable);
	m_editNumber.EnableWindow(bEnable);
	m_Hosts.EnableWindow(bEnable);
	m_cbTyp.EnableWindow(bEnable);

	bEnableItem = (   bEnable			// generally enabled
				   && m_pSelectedHost	// host selected
				   && (   m_pSelectedHost->GetAlarmArchive() == SECID_NO_ID
					   || m_pSelectedHost->GetNormalArchive() == SECID_NO_ID
					   || m_pSelectedHost->GetSearchArchive() == SECID_NO_ID
					  )	// any archieve not assigned
				  );
	m_btnHostArchives.EnableWindow(bEnableItem);

	m_editDisconnect.EnableWindow(   m_pSelectedHost
								  && (   m_pSelectedHost->IsB3()
								      || m_pSelectedHost->IsB6()
									  || m_pSelectedHost->IsTCPIP()
									  || m_pSelectedHost->IsISDN()
									  || m_pSelectedHost->IsDUN()
									  || m_pSelectedHost->IsTOBS())
								  );

	if (m_bPINAccessable)
	{
		int nSel = m_cbTyp.GetCurSel();
		int nCmdShow = (   nSel == m_iEMailIndex
						|| nSel == m_iFAXIndex
						|| nSel == m_iSMSIndex) ? SW_HIDE : SW_SHOW;
		m_editPIN.ShowWindow(nCmdShow);
		m_btnPINRequired.ShowWindow(nCmdShow);
		bEnableItem = bEnable;
		m_btnPINRequired.EnableWindow(bEnableItem);
		bEnableItem = m_btnPINRequired.IsWindowEnabled() && m_bPINRequired;
		m_editPIN.EnableWindow(bEnableItem);
	}
	else
	{
		m_btnPINRequired.ShowWindow(SW_HIDE);
		m_editPIN.ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::ClearHostListBox()
{
	m_Hosts.DeleteAllItems();
	m_sName.Empty();
	m_sNumber.Empty();
	m_iSelectedItem = -1;
	m_pSelectedHost = NULL;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CHostPage::FillHostListBox()
{
	int i,c;
	CHost* pHost;

	c = m_pHostArray->GetSize();
	if (m_iMaxHosts != 0 && c > m_iMaxHosts)
	{
		c = m_iMaxHosts;
	}
	
	if (c>0)
	{
		pHost = (CHost*)m_pHostArray->GetAt(0);
		InsertItem(pHost,TRUE);
	}
	for (i=1; i<c; i++)
	{
		pHost = (CHost*)m_pHostArray->GetAt(i);
		InsertItem(pHost);
	}
}

/////////////////////////////////////////////////////////////////////////////
int CHostPage::InsertItem(CHost* pHost, BOOL bSelectIt)
{
	LV_ITEM lvis;
	CString sName;
	int i,c;

	sName = pHost->GetName();
	CString sNr;
	sNr.Format(_T("%03d"),pHost->GetID().GetSubID()+1);

	c = m_Hosts.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = sNr.GetBuffer(0);
	lvis.cchTextMax = sNr.GetLength();
	lvis.lParam = (LPARAM)pHost;
	lvis.iItem = c;
	lvis.iSubItem = 0;
	
	i = m_Hosts.InsertItem(&lvis);
	sNr.ReleaseBuffer();

	m_Hosts.SetItemText(i,1,sName);
	m_Hosts.SetItemText(i,2,pHost->GetClearNumber());
	m_Hosts.SetItemText(i,3,pHost->GetTyp());
	if (m_bPINAccessable)
	{
		m_Hosts.SetItemText(i,4,(pHost->IsPINRequired()?_T("X"):_T("")));
		m_Hosts.SetItemText(i,5,pHost->GetPIN());
	}
	CString s;
	s.Format(_T("%d"),pHost->GetAutoDisconnectTime());
	m_Hosts.SetItemText(i,6,s);

	if (bSelectIt)
	{
		lvis.iItem = i;
		lvis.iSubItem = 0;
		lvis.mask = LVIF_STATE;
		lvis.state = LVIS_SELECTED;
		lvis.stateMask = LVIS_SELECTED;
		m_Hosts.SetItem(&lvis);
	}
	
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostPage)
	DDX_Control(pDX, IDC_EDIT_DISCONNECT, m_editDisconnect);
	DDX_Control(pDX, IDC_HOST_PIN, m_editPIN);
	DDX_Control(pDX, IDC_HOST_HAS_PIN, m_btnPINRequired);
	DDX_Control(pDX, IDC_HOST_ARCHIVES, m_btnHostArchives);
	DDX_Control(pDX, IDC_ADDRESS_HOST, m_editNumber);
	DDX_Control(pDX, IDC_NAME_HOST, m_editName);
	DDX_Control(pDX, IDC_COMBO_TYPE_HOST, m_cbTyp);
	DDX_Control(pDX, IDC_LIST_HOST, m_Hosts);
	DDX_Text(pDX, IDC_NAME_HOST, m_sName);
	DDX_Text(pDX, IDC_ADDRESS_HOST, m_sNumber);
	DDX_Text(pDX, IDC_STATIONS_NAME, m_sStationsName);
	DDX_Check(pDX, IDC_HOST_HAS_PIN, m_bPINRequired);
	DDX_Text(pDX, IDC_HOST_PIN, m_sPIN);
	DDV_MaxChars(pDX, m_sPIN, 4);
	DDX_Text(pDX, IDC_EDIT_DISCONNECT, m_dwDisconnect);
	//}}AFX_DATA_MAP

}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHostPage, CSVPage)
	//{{AFX_MSG_MAP(CHostPage)
	ON_EN_CHANGE(IDC_NAME_HOST, OnChangeName)
	ON_EN_CHANGE(IDC_ADDRESS_HOST, OnChangeNumber)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_HOST, OnItemchangedListHost)
	ON_EN_CHANGE(IDC_STATIONS_NAME, OnChangeStationsName)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE_HOST, OnSelchangeComboTyp)
	ON_NOTIFY(NM_CLICK, IDC_LIST_HOST, OnClickListHost)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_HOST, OnDblclkListHost)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_HOST, OnRclickListHost)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_HOST, OnRdblclkListHost)
	ON_BN_CLICKED(IDC_HOST_ARCHIVES, OnHostArchives)
	ON_BN_CLICKED(IDC_HOST_HAS_PIN, OnHostHasPin)
	ON_EN_CHANGE(IDC_HOST_PIN, OnChangeHostPin)
	ON_EN_CHANGE(IDC_EDIT_DISCONNECT, OnChangeEditDisconnect)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickListHost)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CRect rect;
	m_Hosts.GetWindowRect(&rect);

	int iColumns = 5;

	CWordArray waWidth;	// CAVEAT add sum has to be 100
	waWidth.SetSize(iColumns);
	waWidth[0] = 10;
	waWidth[1] = 30;
	waWidth[2] = 30;
	waWidth[3] = 20;
	waWidth[4] = 10;

	CStringArray saHeader;
	saHeader.SetSize(iColumns);
	saHeader[0].LoadString(IDS_NR);
	GetDlgItem(IDC_STATIC_NAME)->GetWindowText(saHeader[1]);
	GetDlgItem(IDC_STATIC_NUMBER)->GetWindowText(saHeader[2]);
	GetDlgItem(IDC_TXT_TYP)->GetWindowText(saHeader[3]);
	GetDlgItem(IDC_TXT_DISCONNECT)->GetWindowText(saHeader[4]);

	if (m_bPINAccessable)
	{
		iColumns = 7;
		waWidth.SetSize(iColumns);
		waWidth[0] = 6;
		waWidth[1] = 25;
		waWidth[2] = 25;
		waWidth[3] = 15;
		waWidth[4] = 5;
		waWidth[5] = 6;
		waWidth[6] = 19;
		saHeader.SetSize(iColumns);
		GetDlgItem(IDC_HOST_HAS_PIN)->GetWindowText(saHeader[4]);
		saHeader[5] = saHeader[4];
		GetDlgItem(IDC_TXT_DISCONNECT)->GetWindowText(saHeader[6]);
	}

	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	CString s;
	for (int i = 0; i < iColumns; i++)  // add the columns to the list control
	{
		s = saHeader.GetAt(i);
		s.Replace(_T(":"), _T(""));
		s.Replace(_T("&"), _T(""));
		lvcolumn.cx = (rect.Width() * waWidth[i])/100;
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s);
		lvcolumn.iSubItem = i;
		m_Hosts.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}

	if (m_bISDN)
	{
		m_iISDNIndex = m_cbTyp.AddString(_T("ISDN"));
	}
	if (m_bTCPIP)
	{
		m_iTCPIPIndex = m_cbTyp.AddString(_T("TCP/IP"));
	}
	if (m_bVCS)
	{
		m_iVCSIndex = m_cbTyp.AddString(_T("MIDI B6"));
	}
	if (m_bPres)
	{
		m_iPresIndex = m_cbTyp.AddString(_T("MINI B3"));
	}
	if (m_bSMS)
	{
		m_iSMSIndex = m_cbTyp.AddString(_T("SMS"));
	}
	if (m_bEMail)
	{
		m_iEMailIndex= m_cbTyp.AddString(_T("E-Mail"));
	}
	if (m_bFAX)
	{
		m_iFAXIndex= m_cbTyp.AddString(_T("FAX"));
	}
	if (m_bTOBS)
	{
		m_iTOBSIndex= m_cbTyp.AddString(_T("TeleObserver"));
	}

	FillHostListBox();

	EnableExceptNew();

	int	c = m_Hosts.GetItemCount();
	SelectItem((c>0) ? 0 : -1);

	m_sStationsName = m_pHostArray->GetLocalHostName();
	UpdateData(FALSE);

	LONG dw = ListView_GetExtendedListViewStyle(m_Hosts.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_Hosts.m_hWnd,dw);

	return FALSE;  // return TRUE unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::SelectItem(int i)
{
	if (i>=0)
	{
		CString sTyp;
		m_iSelectedItem = i;
		m_Hosts.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		m_pSelectedHost = (CHost*)m_Hosts.GetItemData(m_iSelectedItem);
		m_sName = m_pSelectedHost->GetName();
		m_sNumber = m_pSelectedHost->GetClearNumber();
		sTyp = m_pSelectedHost->GetTyp();
		m_bPINRequired = m_pSelectedHost->IsPINRequired();
		m_sPIN = m_pSelectedHost->GetPIN();
		m_dwDisconnect = m_pSelectedHost->GetAutoDisconnectTime();

		int iSel = m_cbTyp.FindStringExact(0,m_pSelectedHost->GetTyp());
		m_cbTyp.SetCurSel(iSel);
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedHost = NULL;
		m_sName.Empty();
		m_sNumber.Empty();
		m_cbTyp.SetCurSel(-1);
		m_bPINRequired = FALSE;
		m_sPIN.Empty();
		m_dwDisconnect = 0;
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CanNew()
{
	BOOL bCanNew = TRUE;
	if (m_iMaxHosts != 0 && m_Hosts.GetItemCount() >= m_iMaxHosts)
	{
		bCanNew = FALSE;
	}

	return 	  bCanNew 
			&&(m_bISDN 
		    || m_bTCPIP 
			|| m_bVCS 
			|| m_bPres 
			|| m_bSMS 
			|| m_bEMail
			|| m_bFAX
			|| m_bTOBS);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CanDelete()
{
	return (m_Hosts.GetItemCount()>0) && (m_pSelectedHost != NULL);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CheckNewArchivesForHost(CHost* pHost)
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnNew() 
{							  
	CHost* pHost = m_pHostArray->AddHost();
	CString s;

	s.LoadString(IDS_NEWHOST);
	pHost->SetName(s);
	int nItem = InsertItem(pHost,TRUE);
	SelectItem(nItem);
/*
	m_pSelectedHost = pHost;
	m_sName = pHost->GetName();
	m_sNumber = pHost->GetNumber();
	s = pHost->GetTyp();
	int iIndex = m_cbTyp.FindString(-1, s);
	m_cbTyp.SetCurSel(iIndex);
	m_bPINRequired = FALSE;
	m_sPIN.Empty();
	m_dwDisconnect = 0;
	UpdateData(FALSE);
*/
	SetModified(TRUE,FALSE);

	EnableExceptNew();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CheckProcessesWithHost(CSecID hostID)
{
	CProcess* pProcess;
	BOOL bRemoveFromProcessAchknowledged = FALSE;
	BOOL bRemoveCompleteProcessAchknowledged = FALSE;
	BOOL bNoChangesOrAllowed = TRUE;
	BOOL bFound = FALSE;

	for (int i=0 ; bNoChangesOrAllowed && i<m_pProcessList->GetSize() ; i++)
	{
		pProcess = m_pProcessList->GetAt(i);
		if (pProcess && (pProcess->IsCall()))
		{
			// first search for host at process
			bFound = FALSE;
			for (int j=0 ; !bFound && j<pProcess->GetNrOfCall() ; j++)
			{
				if (pProcess->GetCall(j)==hostID.GetSubID())
				{
					bFound = TRUE;
					// if found, ask for remove/delete
					if (pProcess->GetNrOfCall()>1)
					{
						// delete Host from Process
						if (!bRemoveFromProcessAchknowledged)
						{
							if (IDYES==AfxMessageBox(IDP_DELETE_HOST_FROM_PROCESS,MB_YESNO))
							{
								bRemoveFromProcessAchknowledged = TRUE;
							}
							else
							{
								bNoChangesOrAllowed = FALSE;
							}
						}
						if (bRemoveFromProcessAchknowledged)
						{
							pProcess->ClearCall(hostID.GetSubID());
							m_bProcessesChanged = TRUE;
						}
					}
					else
					{
						// delete Host and Process activation ?
						if (!bRemoveCompleteProcessAchknowledged)
						{
							if (IDYES==AfxMessageBox(IDP_DELETE_HOST_PROCESS,MB_YESNO))
							{
								bRemoveCompleteProcessAchknowledged = TRUE;
							}
							else
							{
								bNoChangesOrAllowed = FALSE;
							}
						}
						if (bRemoveCompleteProcessAchknowledged)
						{
							m_pInputList->DeleteActivation(pProcess->GetID());
							m_bInputsChanged = TRUE;
							m_pProcessList->DeleteProcess(pProcess);
							m_bProcessesChanged = TRUE;
						}
					}
				}
			}
		}
	}
	if (   m_bInputsChanged
		|| m_bProcessesChanged
		)
	{
		SetModified(TRUE,FALSE);
	}
	return bNoChangesOrAllowed;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CheckDeleteArchivesForHost(CHost* pHost)
{
	BOOL bNoChangesOrAllowed = FALSE;
	CArchivInfo* pArchivInfo = NULL;

	// m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo) will remove it from the array,
	// so first collect them and delete them afterwards, if not denied
	CPtrArray arrayArchivesToDelete;

	for (int i=0;i<m_pArchivInfoArray->GetSize();i++)
	{
		pArchivInfo = m_pArchivInfoArray->GetAt(i);

		if (pArchivInfo)
		{
			if (   (pArchivInfo->GetID()==pHost->GetSearchArchive())
				|| (pArchivInfo->GetID()==pHost->GetNormalArchive())
				|| (pArchivInfo->GetID()==pHost->GetAlarmArchive())
				)
			{
				if (!bNoChangesOrAllowed)
				{
					if (IDYES == AfxMessageBox(IDS_DELETE_HOST_ARCHIV,MB_YESNO))
					{
						bNoChangesOrAllowed = TRUE;
					}
					else
					{
						break;
					}
				}
				if (bNoChangesOrAllowed)
				{
					arrayArchivesToDelete.Add(pArchivInfo);
				}
			}
		}
	}
	if (arrayArchivesToDelete.GetSize())
	{
		while (arrayArchivesToDelete.GetSize())
		{
			m_pArchivInfoArray->DeleteArchivInfo((CArchivInfo*)arrayArchivesToDelete.GetAt(0));
			arrayArchivesToDelete.RemoveAt(0);
		}
		m_bArchivesChanged = TRUE;
		SetModified(TRUE,FALSE);
	}
	else
	{	// nothing changed, so return TRUE, it can be deleted
		bNoChangesOrAllowed = TRUE;
	}
	return bNoChangesOrAllowed;
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnDelete() 
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedHost==NULL))
	{
		return;
	}

	// check/ask for archives, temporary html files
	// and processes

	// processes
	if (!CheckProcessesWithHost(m_pSelectedHost->GetID()))
	{
		return;
	}
	if (!CheckDeleteArchivesForHost(m_pSelectedHost))
	{
		return;
	}

	m_pHostArray->DeleteHost(m_pSelectedHost);
	m_pSelectedHost = NULL;
	
	// calc new selection
	int	newCount = m_Hosts.GetItemCount()-1;	// pre-subtract before DeleteItem
	int i = m_iSelectedItem;	// new selection

	// range check
	if (i > newCount-1 ) 
	{	// outside ?
		i=newCount-1;
	}
	else if (newCount==0)
	{	// last one ?
		i = -1;
	}

	if (m_Hosts.DeleteItem(m_iSelectedItem))	// remove from listCtrl
	{
		SelectItem(i);
		SetModified(TRUE,FALSE);
		EnableExceptNew();
		m_Hosts.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnChangeName() 
{
	UpdateData();

	if (m_pSelectedHost)
	{
		m_pSelectedHost->SetName(m_sName);
		m_Hosts.SetItemText(m_iSelectedItem,1,m_sName);
	}
	else
	{
		//no host selected -> create and select a new one
		CHost* pHost = m_pHostArray->AddHost();

		pHost->SetName(m_sName);
		InsertItem(pHost,TRUE);
	}
	SetModified(TRUE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnChangeNumber() 
{
	UpdateData();

	CString s = m_sNumber;
	CString sTyp;
	int iCurSel = m_cbTyp.GetCurSel();

	if (-1==s.Find(_T('#')))
	{
		if (iCurSel==m_iTCPIPIndex)
		{
			if (m_sNumber.Find(_T("tcp:"))!=0)
			{
				s = _T("tcp:") + m_sNumber;
			}
			s = TrimLeadingZerosIP(s);
		}
		else if (iCurSel==m_iVCSIndex)
		{
			if (m_sNumber.Find(_T("vcs:"))!=0)
			{
				s = _T("vcs:") + m_sNumber;
			}
		}
		else if (iCurSel==m_iPresIndex)
		{
			if (m_sNumber.Find(_T("pres:"))!=0)
			{
				s = _T("pres:") + m_sNumber;
			}
		}
		else if (iCurSel==m_iSMSIndex)
		{
			if (m_sNumber.Find(_T("sms:"))!=0)
			{
				s = _T("sms:") + m_sNumber;
			}
		}
		else if (iCurSel==m_iEMailIndex)
		{
			if (m_sNumber.Find(_T("mailto:"))!=0)
			{
				s = _T("mailto:") + m_sNumber;
			}
		}
		else if (iCurSel==m_iFAXIndex)
		{
			if (m_sNumber.Find(_T("fax:"))!=0)
			{
				s = _T("fax:") + m_sNumber;
			}
		}
		else if (iCurSel==m_iTOBSIndex)
		{
			if (m_sNumber.Find(_T("tobs:"))!=0)
			{
				s = _T("tobs:") + m_sNumber;
			}
		}
		else
		{
			s = m_sNumber;
		}
	}


	if (m_pSelectedHost)
	{
		m_pSelectedHost->SetNumber(s);
		sTyp = m_pSelectedHost->GetTyp();

		m_Hosts.SetItemText(m_iSelectedItem,2,m_sNumber);
		m_Hosts.SetItemText(m_iSelectedItem,3,sTyp);

		int iSel = m_cbTyp.FindString(0,sTyp);
		if (iSel!=CB_ERR)
		{
			m_cbTyp.SetCurSel(iSel);
		}
	}
	else
	{
		//no host selected -> create and select a new one
		LV_ITEM lvis;
		int i;
		UpdateData();

		CHost* pHost = m_pHostArray->AddHost();
		CString sN;

		sN.LoadString(IDS_NEWHOST);
		pHost->SetName(sN);
		pHost->SetNumber(s);
		i = InsertItem(pHost);
		lvis.mask = LVIF_TEXT|LVIF_STATE;
		lvis.iItem = i;
		lvis.iSubItem = 0;
		lvis.pszText = sN.GetBuffer(0);
		lvis.cchTextMax = sN.GetLength();
		lvis.state = LVIS_SELECTED|LVIS_FOCUSED;
		lvis.stateMask = LVIS_SELECTED|LVIS_FOCUSED;
		
		m_Hosts.SetItem(&lvis);

		sN.ReleaseBuffer();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = 2;
		lvis.pszText = sTyp.GetBuffer(0);
		lvis.cchTextMax = sTyp.GetLength();

		m_Hosts.SetItem(&lvis);
		sTyp.ReleaseBuffer();
	}
	SetModified(TRUE,FALSE);
	UpdateData(FALSE);
	EnableExceptNew();
}
////////////////////////////////////////////////////////////////////////////
CString CHostPage::TrimLeadingZerosIP(const CString& s)
{
	CString sRet;
	CString sTCP = _T("tcp:");
	CString sTemp= s;

	int iTCP = s.Find(sTCP);

	if (iTCP != -1)
	{
		sTCP  = s.Left(iTCP+4);
		sTemp = s.Mid(iTCP+4);
	}
	else
	{
		sTCP = _T("");
	}

	int iA=0, iB=0, iC=0, iD=0;
	CString sFormat(_T("%d.%d.%d.%d"));
	if (_stscanf(sTemp, sFormat, &iA, &iB, &iC, &iD) == 4)
	{
		// all fields set, trim leading zeros in each field
		sTemp.Format(sFormat, iA, iB, iC, iD);
		sRet = sTCP + sTemp;
	}
	else
	{
		// not all fields set, keep original
		sRet = s;
	}
	return sRet;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnSelchangeComboTyp() 
{
	OnChangeNumber();
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnItemchangedListHost(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			CString s;
			UpdateData(TRUE);
			m_pSelectedHost = (CHost*)pNMListView->lParam;
			m_iSelectedItem = pNMListView->iItem;
			m_sName = m_pSelectedHost->GetName();
			m_sNumber = m_pSelectedHost->GetClearNumber();
			int iSel = m_cbTyp.FindStringExact(0,m_pSelectedHost->GetTyp());
			m_cbTyp.SetCurSel(iSel);
			m_bPINRequired = m_pSelectedHost->IsPINRequired();
			m_sPIN = m_pSelectedHost->GetPIN();
			m_dwDisconnect = m_pSelectedHost->GetAutoDisconnectTime();
			if (!m_bPINRequired)
			{
				m_sPIN = _T("");
			}
			UpdateData(FALSE);
			EnableExceptNew();
		}
	}
	
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnClickListHost(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Hosts.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnDblclkListHost(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Hosts.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnRclickListHost(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Hosts.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnRdblclkListHost(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Hosts.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::CancelChanges()
{
	if (m_bProcessesChanged)
	{
		m_pProcessList->Reset();
		m_pProcessList->Load(GetProfile());
		m_bProcessesChanged = FALSE;
	}
	if (m_bArchivesChanged)
	{
		m_pArchivInfoArray->DeleteAll();
		m_pArchivInfoArray->Load(GetProfile());
		m_bArchivesChanged = FALSE;
	}
	if (m_bInputsChanged)
	{
		m_pInputList->Reset();
		m_pInputList->Load(GetProfile());
		m_bInputsChanged = FALSE;
	}

	ClearHostListBox();
	m_pHostArray->Load(GetProfile());
	FillHostListBox();
	int	c = m_Hosts.GetItemCount();
	SelectItem((c>0) ? 0 : -1);
	EnableExceptNew();
	if (c>=0)
	{
		m_Hosts.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::SaveChanges()
{
	if (theApp.m_bUpdateLanguageDependencies)
	{
		int i, c, r = IDNO;
		CHost *pHost;
		c = m_pHostArray->GetSize();
		for (i=0;i<c;i++)
		{
			pHost = m_pHostArray->GetAt(i);
			CheckArchiveForHost(*m_pArchivInfoArray,r,pHost,m_pDrives->GetAvailableMB());
		}
		
		pHost = m_pHostArray->GetLocalHost();
		CheckArchiveForHost(*m_pArchivInfoArray,r,pHost,m_pDrives->GetAvailableMB());
		m_bArchivesChanged = TRUE;
	}

	if (theApp.m_pUser)
	{
		WK_TRACE_USER(_T("%s Gegenstationen-Einstellungen wurden geändert\n"),
			theApp.m_pUser->GetName());
	}
#ifdef _UNICODE
	if (m_pParent->GetDocument()->IsLocal())
	{
		m_pHostArray->m_bUseBinaryFormat = TRUE;
	}
	else
	{
		m_pHostArray->m_bUseBinaryFormat = m_pParent->GetDocument()->UseBinaryFormat();
	}
#endif
	m_pHostArray->Save(GetProfile());

	if (m_bProcessesChanged)
	{
		m_pProcessList->Save(GetProfile());
		m_bProcessesChanged = FALSE;
		SetModified(TRUE,TRUE);
	}
	if (m_bArchivesChanged)
	{
		m_pArchivInfoArray->Save(GetProfile());
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		m_bArchivesChanged = FALSE;
		SetModified(TRUE,TRUE);
	}
	if (m_bInputsChanged)
	{
		m_pInputList->Save(GetProfile(),FALSE);
		m_bInputsChanged = FALSE;
		SetModified(TRUE,TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::IsDataValid()
{
	CString sName1,sName2;
	CString sNumber1,sNumber2;
	int i,j,c;

	if (!UpdateData())
	{
		return FALSE;
	}

	// we have to check for valid own host name
	if (m_sStationsName.IsEmpty()
		|| m_sStationsName == CHostArray::GetLocalHostNameStandard())
	{
		AfxMessageBox(IDP_NO_LOCAL_HOSTNAME,MB_ICONSTOP);
		return FALSE;
	}

	m_pHostArray->SetLocalHostName(m_sStationsName);

	c = m_pHostArray->GetSize();

	// we have to check for double hosts
	for (i=0;i<c;i++)
	{
		sName1 = m_pHostArray->GetAt(i)->GetName();
		sNumber1 = m_pHostArray->GetAt(i)->GetNumber();
		for (j=i+1;j<c;j++)
		{
			sName2 = m_pHostArray->GetAt(j)->GetName();
			sNumber2 = m_pHostArray->GetAt(j)->GetNumber();
			if ((sName1 == sName2) && (sNumber1 == sNumber2))
			{
				CString sL,sM;
				sL.LoadString(IDP_DOUBLE_HOST);
				sM.Format(sL,sName1);
				AfxMessageBox(sM,MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	CHost* pHost;
	CString sNum;

	// we have to check for valid host number
	for (i=0;i<c;i++)
	{
		pHost = m_pHostArray->GetAt(i);
		sNum = pHost->GetNumber();
		if (sNum.IsEmpty())
		{
			CString sL,sM;
			sL.LoadString(IDP_HOST_WITHOUT_NUMBER);
			sM.Format(sL,pHost->GetName());
			AfxMessageBox(sM,MB_ICONSTOP);
			return FALSE;
		}

		if (!pHost->IsValid())
		{
			if (pHost->IsRouting())
			{
				CString sL,sM;
				sL.LoadString(IDP_HOST_ILLEGAL_ROUTING);
				sM.Format(sL,pHost->GetName());
				AfxMessageBox(sM,MB_ICONSTOP);
				return FALSE;
			}
			else
			{
				CString sL,sM;
				sL.LoadString(IDP_HOST_ILLEGAL_ADDRESS);
				sM.Format(sL,pHost->GetName());
				AfxMessageBox(sM,MB_ICONSTOP);
				return FALSE;
			}
		}
	}


	// and now ask for creating archives
	// for hosts, which have none
	int r = IDYES;
	int d = m_pArchivInfoArray->GetSize();

	c = m_pHostArray->GetSize();
	for (i=0;i<c;i++)
	{
		pHost = m_pHostArray->GetAt(i);
		r = CheckArchiveForHost(*m_pArchivInfoArray,r,pHost,m_pDrives->GetAvailableMB());
		if (r==IDNOALL)
		{
			break;
		}
	}
	
	if (r!=IDNOALL)
	{
		pHost = m_pHostArray->GetLocalHost();
		r = CheckArchiveForHost(*m_pArchivInfoArray,r,pHost,m_pDrives->GetAvailableMB());
	}

	if (d != m_pArchivInfoArray->GetSize())
	{
		m_bArchivesChanged = TRUE;
		SetModified(TRUE,FALSE);
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
int CHostPage::CheckArchiveForHost(CArchivInfoArray& ai,
								   int r, 
								   CHost* pHost,
								   DWORD dwAvailableMB)
{
	int lr = r;
	CSecID aID;
	
	aID = SECID_NO_ID;

	if (   pHost 
		&& !pHost->IsSMS()
		&& !pHost->IsEMail()
		&& !pHost->IsFAX()
		)
	{
		aID = pHost->GetAlarmArchive();
		CArchivInfo*pAi = ai.GetArchivInfo(aID);
		if (NULL==pAi)
		{
			pHost->SetAlarmArchive(SECID_NO_ID);
			lr = AskForAutomaticArchive(ai,lr,IDP_CREATE_ALARM_ARCHIVE_FOR_HOST,
										IDS_HOST_ALARM_ARCHIVE,
										pHost,aID,dwAvailableMB);
			if (lr==IDNOALL)
			{
				return lr;
			}
			pHost->SetAlarmArchive(aID);
		}
		else
		{
			CString sExt, sOld = pAi->GetName();
			sExt.LoadString(IDS_HOST_ALARM_ARCHIVE);
			pAi->SetName(pHost->GetName() + _T(" ") + sExt);
			if (sOld != pAi->GetName())
			{
				m_bArchivesChanged = TRUE;
			}
		}
		aID = pHost->GetNormalArchive();
		pAi = ai.GetArchivInfo(aID);
		if (NULL==pAi)
		{
			pHost->SetNormalArchive(SECID_NO_ID);
			lr = AskForAutomaticArchive(ai,lr,IDP_CREATE_NORMAL_ARCHIVE_FOR_HOST,
										IDS_HOST_NORMAL_ARCHIVE,
										pHost,aID,dwAvailableMB);
			if (lr==IDNOALL)
			{
				return lr;
			}
			pHost->SetNormalArchive(aID);
		}
		else
		{
			CString sExt, sOld = pAi->GetName();
			sExt.LoadString(IDS_HOST_NORMAL_ARCHIVE);
			pAi->SetName(pHost->GetName() + _T(" ") + sExt);
			if (sOld != pAi->GetName())
			{
				m_bArchivesChanged = TRUE;
			}
		}

		aID	= pHost->GetSearchArchive();
		if (   (!pHost->IsB3())
			&& (!pHost->IsB6())
			&& (pHost->GetID()!=SECID_LOCAL_HOST))
		{
			pAi = ai.GetArchivInfo(aID);
			if (NULL==pAi)
			{
				pHost->SetSearchArchive(SECID_NO_ID);
				lr = AskForAutomaticArchive(ai,lr,IDP_CREATE_SEARCH_ARCHIVE_FOR_HOST,
											IDS_HOST_SEARCH_ARCHIVE,
											pHost,aID,dwAvailableMB);
				if (lr==IDNOALL)
				{
					return lr;
				}
				pHost->SetSearchArchive(aID);
			}
			else
			{
				CString sExt, sOld = pAi->GetName();
				sExt.LoadString(IDS_HOST_SEARCH_ARCHIVE);
				pAi->SetName(pHost->GetName() + _T(" ") + sExt);
				if (sOld != pAi->GetName())
				{
					m_bArchivesChanged = TRUE;
				}
			}

		}
	}
	return lr;
}
////////////////////////////////////////////////////////////////////////////
int CHostPage::AskForAutomaticArchive(CArchivInfoArray& ai,
									  int r, 
									  UINT nQuestion, 
									  UINT nExtension,
									  CHost* pHost,
									  CSecID& id,
									  DWORD dwAvailableMB)
{
	CString sHostNameType;
	if (pHost->GetID() == SECID_LOCAL_HOST)
	{
		sHostNameType.LoadString(IDS_LOCAL_HOST);
	}
	else
	{
		sHostNameType.Format(_T("%s [%s]"), pHost->GetName(), pHost->GetTyp());
	}

	CString sFormat,sMsg;
	sFormat.LoadString(nQuestion);
	sMsg.Format(sFormat, sHostNameType);

	if (r!=IDYESALL && !theApp.m_bUpdateLanguageDependencies)
	{
		r = COemGuiApi::MessageBox(sMsg,0,MB_YESNO|MB_YESALL|MB_NOALL|MB_ICONQUESTION);
	}
	if ((r==IDYES) || (r==IDYESALL))
	{
		CString sName,sExt;
		sExt.LoadString(nExtension);

		// create an alarm archive for host if possible
		if (ai.GetSizeMB() + START_SIZE_MB <= dwAvailableMB)
		{
			// can create
			CArchivInfo* pAI = ai.AddArchivInfo();
			sName = pHost->GetName() + _T(" ") + sExt; 
			pAI->SetName(sName);
			id = pAI->GetID();
			WK_TRACE_USER(_T("automatically created %s archive for <%s>\n"),
						sExt,
						pHost->GetName());
		}
		else
		{
			WK_TRACE_ERROR(_T("Cannot automatically create %s archive for <%s>\n"),
				sExt,
				pHost->GetName());
			sFormat.LoadString(IDP_NO_AUTOMATIC_ARCHIV);
			sMsg.Format(sFormat, sExt, pHost->GetName());
			AfxMessageBox(sMsg, MB_OK|MB_ICONSTOP);
			r = IDNOALL; // stop asking for new archives
		}
	}
	return r;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnChangeStationsName() 
{
	SetModified();
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnHostArchives() 
{
	// TODO: Add your control notification handler code here
	if (m_pSelectedHost)	// actual host selected
	{
		int d = m_pArchivInfoArray->GetSize();
		CheckArchiveForHost(*m_pArchivInfoArray,IDNO,m_pSelectedHost,m_pDrives->GetAvailableMB());

		if (d != m_pArchivInfoArray->GetSize())
		{
			m_bArchivesChanged = TRUE;
			SetModified(TRUE,FALSE);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnHostHasPin() 
{
	if (m_pSelectedHost)
	{
		UpdateData();
		EnableExceptNew();
		m_pSelectedHost->SetPINRequired(m_bPINRequired);
		m_Hosts.SetItemText(m_iSelectedItem, 4, (m_bPINRequired ? _T("X") : _T("")));
		SetModified(TRUE,FALSE);
		if (m_bPINRequired)
		{
			m_editPIN.SetFocus();
		}
		else
		{
			m_sPIN.Empty();
			UpdateData(FALSE);
			OnChangeHostPin();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnChangeHostPin() 
{
	if (m_pSelectedHost)
	{
		UpdateData();
		m_pSelectedHost->SetPIN(m_sPIN);
		m_Hosts.SetItemText(m_iSelectedItem,5,m_sPIN);
		SetModified(TRUE,FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnChangeEditDisconnect() 
{
	if (m_pSelectedHost)
	{
		UpdateData();
		m_pSelectedHost->SetAutoDisconnectTime(m_dwDisconnect);
		CString s;
		s.Format(_T("%d"),m_dwDisconnect);
		m_Hosts.SetItemText(m_iSelectedItem,6,s);
		SetModified(TRUE,FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////
int CALLBACK CHostPage::HostCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CHostPage *pThis = (CHostPage*)lParamSort;
	CHost *pH1 = (CHost*)lParam1;
	CHost *pH2 = (CHost*)lParam2;
	int iRet = 0;
	switch(pThis->m_iSortColumn & 0x0000ffff)
	{
		case 0:
			if		(pH1->GetID().GetSubID() > pH2->GetID().GetSubID())	iRet =  1;
			else if (pH1->GetID().GetSubID() < pH2->GetID().GetSubID())	iRet = -1;
			else														iRet =  0;
			break;
		case 1: iRet = _tcscmp(pH1->GetName(), pH2->GetName()); break;
		case 2: iRet = _tcscmp(pH1->GetClearNumber(), pH2->GetClearNumber()); break;
		case 3: iRet = _tcscmp(pH1->GetTyp(), pH2->GetTyp()); break;
		case 4: 
			if		(pH1->IsPINRequired() > pH2->IsPINRequired())	iRet =  1;
			else if (pH1->IsPINRequired() < pH2->IsPINRequired())	iRet = -1;
			else													iRet =  0;
			break;
		case 5: iRet = _tcscmp(pH1->GetPIN(), pH2->GetPIN()); break;
		case 6: 
			if		(pH1->GetAutoDisconnectTime() > pH2->GetAutoDisconnectTime())	iRet =  1;
			else if (pH1->GetAutoDisconnectTime() < pH2->GetAutoDisconnectTime())	iRet = -1;
			else																	iRet =  0;
			break;
	}
	if (pThis->m_iSortColumn & 0x80000000)
	{
		iRet = -iRet;
	}
	return iRet;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::OnHdnItemclickListHost(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if (phdr->iItem != m_iSortColumn)
	{
		m_iSortColumn = phdr->iItem;
	}
	else
	{
		m_iSortColumn |= 0x80000000;
	}
	m_Hosts.SortItems(HostCompareProc, (LPARAM)this);
	for (int i=0; i<m_Hosts.GetItemCount(); i++)
	{
		if (m_Hosts.GetItemData(i) == (DWORD)m_pSelectedHost)
		{
			m_iSelectedItem = i;
			break;
		}
	}

	*pResult = 0;
}
