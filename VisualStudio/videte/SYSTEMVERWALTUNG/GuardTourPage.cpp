// GuardTourPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "GuardTourPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuardTourPage dialog
CGuardTourPage::CGuardTourPage(CSVView* pParent)
	: CSVPage(CGuardTourPage::IDD)
{
	m_pParent = pParent;
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pPermissionArray = pParent->GetDocument()->GetPermissions();
	m_pHostArray = pParent->GetDocument()->GetHosts();
	m_pSelectedProcess = NULL;
	m_iSelectedProcessItem = -1;
	m_iSelectedHostItem = -1;
	m_iSelectedCamerasItem = -1;

	//{{AFX_DATA_INIT(CGuardTourPage)
	m_bFullScreen = FALSE;
	m_bSave = FALSE;
	m_dwSwitchTime = 0;
	m_sCameras = _T("");
	m_dwNrOfTries = 0;
	m_sName = _T("");
	m_bExclusive = FALSE;
	m_bDisconnect = TRUE;
	m_iHold = 0;
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGuardTourPage)
	DDX_Control(pDX, IDC_GUARDTOUR_SWITCH_TIME, m_editSwitchTime);
	DDX_Control(pDX, IDC_GUARDTOUR_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_GUARDTOUR_FULLSCREEN, m_btnFullscreen);
	DDX_Control(pDX, IDC_SAVE_FOREVER, m_btnForever);
	DDX_Control(pDX, IDC_GUARDTOUR_HOLD, m_btnHold);
	DDX_Control(pDX, IDC_GUARDTOUR_EXCLUSIVE, m_btnExclusive);
	DDX_Control(pDX, IDC_GUARDTOUR_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_GUARDTOUR_NAME, m_editName);
	DDX_Control(pDX, IDC_GUARDTOUR_PROCESSES, m_listProcesses);
	DDX_Control(pDX, IDC_GUARDTOUR_TRY, m_editNrOfTries);
	DDX_Control(pDX, IDC_GUARDTOUR_CAMERAS, m_editCameras);
	DDX_Control(pDX, IDC_GUARDTOUR_HOSTS, m_listHosts);
	DDX_Control(pDX, IDC_COMBO_PERMISSION_PROCESS, m_cbPermission);
	DDX_Check(pDX, IDC_GUARDTOUR_FULLSCREEN, m_bFullScreen);
	DDX_Check(pDX, IDC_GUARDTOUR_SAVE, m_bSave);
	DDX_Text(pDX, IDC_GUARDTOUR_SWITCH_TIME, m_dwSwitchTime);
	DDV_MinMaxDWord(pDX, m_dwSwitchTime, 0, 3600);
	DDX_Text(pDX, IDC_GUARDTOUR_CAMERAS, m_sCameras);
	DDX_Text(pDX, IDC_GUARDTOUR_TRY, m_dwNrOfTries);
	DDV_MinMaxDWord(pDX, m_dwNrOfTries, 1, 3);
	DDX_Text(pDX, IDC_GUARDTOUR_NAME, m_sName);
	DDX_Check(pDX, IDC_GUARDTOUR_EXCLUSIVE, m_bExclusive);
	DDX_Check(pDX, IDC_GUARDTOUR_DISCONNECT, m_bDisconnect);
	DDX_Radio(pDX, IDC_SAVE_FOREVER, m_iHold);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGuardTourPage, CSVPage)
	//{{AFX_MSG_MAP(CGuardTourPage)
	ON_BN_CLICKED(IDC_GUARDTOUR_FULLSCREEN, OnCheckFullScreen)
	ON_BN_CLICKED(IDC_GUARDTOUR_SAVE, OnCheckSave)
	ON_CBN_SELCHANGE(IDC_COMBO_PERMISSION_PROCESS, OnSelchangePermission)
	ON_EN_CHANGE(IDC_GUARDTOUR_SWITCH_TIME, OnChangeSwitchTime)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GUARDTOUR_PROCESSES, OnItemchangedGuardtourProcesses)
	ON_EN_CHANGE(IDC_GUARDTOUR_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_GUARDTOUR_CAMERAS, OnChangeCameras)
	ON_BN_CLICKED(IDC_GUARDTOUR_EXCLUSIVE, OnCheckExclusive)
	ON_BN_CLICKED(IDC_GUARDTOUR_DISCONNECT, OnCheckDisconnect)
	ON_BN_CLICKED(IDC_SAVE_FOREVER, OnRadioHold)
	ON_EN_CHANGE(IDC_GUARDTOUR_TRY, OnChangeTries)
	ON_LBN_SELCHANGE(IDC_GUARDTOUR_HOSTS, OnSelchangeHosts)
	ON_NOTIFY(NM_CLICK, IDC_GUARDTOUR_PROCESSES, OnClickGuardtourProcesses)
	ON_NOTIFY(NM_DBLCLK, IDC_GUARDTOUR_PROCESSES, OnDblclkGuardtourProcesses)
	ON_NOTIFY(NM_RCLICK, IDC_GUARDTOUR_PROCESSES, OnRclickGuardtourProcesses)
	ON_NOTIFY(NM_RDBLCLK, IDC_GUARDTOUR_PROCESSES, OnRdblclkGuardtourProcesses)
	ON_BN_CLICKED(IDC_GUARDTOUR_HOLD, OnRadioHold)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GTP_HOST_SPIN, OnDeltaposGtpHostSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CGuardTourPage::CanNew()
{
	return (   m_pHostArray
			&& (m_listHosts.GetCount() > 0) // allows if only local host
			&& m_pPermissionArray
			&& (m_pPermissionArray->GetSize() > 0)
			);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGuardTourPage::CanDelete()
{
	return (m_listProcesses.GetItemCount() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnNew() 
{
	ControlToProcess();

	CSecID id(SECID_NO_ID);
	DWORD dwTries = 1,
		   dwSwitchTime = 1;
	BOOL  iHold       = FALSE, 
		   bSave       = FALSE,
			bFullscreen = FALSE,
			bExclusive  = FALSE,
			bDisconnect = TRUE;
	CString sName;

	if (m_pSelectedProcess)
	{
		CWordArray wa;
		CStringArray sa;
		m_pSelectedProcess->GetGuardTourData(sName,
			id,
			dwTries,
			dwSwitchTime,
			iHold,
			bSave,
			bFullscreen, 
			bExclusive,
			bDisconnect,
			wa,
			sa);
 
		m_listProcesses.SetItemState(m_iSelectedProcessItem, 0, LVIS_SELECTED|LVIS_FOCUSED);
	}
	// Clear all temporary data for new process
	m_pSelectedProcess = NULL; // already saved, new one will be selected
	m_iSelectedProcessItem = -1;
	m_iSelectedHostItem = -1;
	m_iSelectedCamerasItem = -1;
	
	CProcess* pProcess = m_pProcessList->AddProcess();
	sName.LoadString(IDS_GUARD_TOUR);
	m_cbPermission.SetCurSel(CB_ERR);
	CWordArray waEmpty;
	CStringArray saEmpty;
	pProcess->SetGuardTour( sName,
							id,
							dwTries, 
							dwSwitchTime,
							iHold,
							bSave,
							bFullscreen,
							bExclusive,
							bDisconnect,
							waEmpty,
							saEmpty);

	int iIndex = InsertProcess(pProcess);
	SetModified();

	SelectProcessItem(iIndex);
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnDelete() 
{
	if ( (m_iSelectedProcessItem==-1) || (m_pSelectedProcess==NULL))
	{
		return;
	}

	if (m_pParent->GetDocument()->CheckActivationsWithProcess(m_pSelectedProcess->GetID()))
	{
		m_pProcessList->DeleteProcess(m_pSelectedProcess);
		m_pSelectedProcess = NULL;
		// calc new selection
		int	newCount = m_listProcesses.GetItemCount()-1;	// pre-subtract before DeleteItem
		int i = m_iSelectedProcessItem;	// new selection

		// range check
		if (i > newCount-1 ) {	// outside or last one?
			i=newCount-1;
		}
		if (m_listProcesses.DeleteItem(m_iSelectedProcessItem))	// remove from listCtrl
		{
			SelectProcessItem(i);
			SetModified();
			EnableExceptNew();
			m_listProcesses.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGuardTourPage::IsDataValid()
{
	// first actualize data from dialog
	ControlToProcess();
	
	CProcess* pProcess = NULL;
	CWordArray waHosts;
	CStringArray saCameras;
	CString sCameras;
	BOOL bValid = TRUE;
	CString sWrong;
	int iIndex = 0;

	for (int i=0 ; i<m_listProcesses.GetItemCount() ; i++)
	{
		pProcess = (CProcess*)m_listProcesses.GetItemData(i);
		// name empty
		if (pProcess->GetName().IsEmpty())
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_NAME_INVALID, MB_ICONSTOP);
			m_editName.SetFocus();
			return FALSE;
		}
		// Permission empty
		if (pProcess->GetPermission() == SECID_NO_ID)
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_GUARDTOUR_PERMISSION_INVALID, MB_ICONSTOP);
			m_cbPermission.SetFocus();
			return FALSE;
		}
		// No of tries will be capture by DoDataExchange
		// switch time will be capture by DoDataExchange
		// Any hosts
		if (pProcess->GetNrOfCall() == 0)
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_GUARDTOUR_NO_HOSTS, MB_ICONSTOP);
			m_listHosts.SetFocus();
			return FALSE;
		}
		// Any camera for host
		pProcess->GetCallArray(waHosts);
		pProcess->GetCameraArray(saCameras);
		if (waHosts.GetSize() == saCameras.GetSize())
		{
			for (int iCamHost=0 ; iCamHost<saCameras.GetSize() ; iCamHost++)
			{
				sCameras = saCameras.GetAt(iCamHost);
				if (sCameras.IsEmpty())
				{
					AfxMessageBox(IDP_NO_CAMERAS_SELECTED, MB_ICONSTOP);
					bValid = FALSE;
				}
				else
				{
					int iRet = IsCameraValid(sCameras, sWrong, iIndex);
					switch (iRet)
					{
					case 0:
						AfxMessageBox(IDP_NO_CAMERAS_SELECTED, MB_ICONSTOP);
						bValid = FALSE;
						break;
					case -1:
						AfxMessageBox(IDP_CAMERAS_INVALID_CHAR, MB_ICONSTOP);
						bValid = FALSE;
						break;
					case -2:
						AfxMessageBox(IDP_CAMERAS_INVALID_CAM, MB_ICONSTOP);
						bValid = FALSE;
						break;
					case -3:
						AfxMessageBox(IDP_CAMERAS_INVALID_RANGE, MB_ICONSTOP);
						bValid = FALSE;
						break;
					default:
						bValid = TRUE;
						break;
					}
				}
				if (!bValid)
				{
					SelectProcessItem(i);
					SelectHostItemBySubID(waHosts.GetAt(iCamHost));
					EnableExceptNew();
					m_editCameras.SetFocus();
					iIndex = m_sCameras.Find(sWrong, iIndex);
					if (iIndex != -1)
					{
						m_editCameras.SetSel(iIndex, iIndex+sWrong.GetLength());
					}
					else
					{
						m_editCameras.SetSel(0, -1);
					}
					return FALSE;
				}
			}
		}
		else
		{	// Error, process invalid
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::SaveChanges()
{
	WK_TRACE_USER(_T("Wächterrundgang-Einstellungen wurden geändert\n"));
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());

	FillProcesses();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::EnableExceptNew()
{
	BOOL bEnableProcesses = (m_listProcesses.GetItemCount() > 0);
	m_listProcesses.EnableWindow(bEnableProcesses);

	BOOL bEnable = bEnableProcesses && (m_pSelectedProcess != NULL);
	m_listHosts.EnableWindow(bEnable);
	m_editName.EnableWindow(bEnable);

	BOOL bEnableCameras = (bEnable && m_listHosts.GetCheck(m_iSelectedHostItem));
	m_editCameras.EnableWindow(bEnableCameras);

	m_cbPermission.EnableWindow(bEnable);
	m_editNrOfTries.EnableWindow(bEnable);
	m_editSwitchTime.EnableWindow(bEnable);
	m_btnForever.EnableWindow(bEnable);
	m_btnHold.EnableWindow(bEnable);
	m_btnSave.EnableWindow(bEnable);
	m_btnFullscreen.EnableWindow(bEnable);
	m_btnExclusive.EnableWindow(bEnable);
	m_btnDisconnect.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGuardTourPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	GetDlgItemText(IDC_TXT_SECONDS, m_sSeconds);
	m_listProcesses.ModifyStyle(LVS_SINGLESEL,0);

	// Init process list control
	InitProcessList();

	FillPermissions();
	FillHosts();

	CancelChanges(); // will fill processes anyway

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::InitProcessList()
{
	LONG dw = ListView_GetExtendedListViewStyle(m_listProcesses.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listProcesses.m_hWnd,dw);

	CRect rect;
	const int nColumns = 9;
	int w[] = {12          ,12                ,10         ,14                 ,20                  ,8            ,10                      ,7             ,7};
	int h[] = {IDC_TXT_NAME,IDC_TXT_PERMISSION,IDC_TXT_TRY,IDC_TXT_SWITCH_TIME,0                   ,0            ,IDC_GUARDTOUR_FULLSCREEN,0             ,IDC_GUARDTOUR_DISCONNECT};
	int ht[]= {0           ,0                 ,0          ,0                  ,IDS_ACTIVATION_STATE,IDS_PROZ_SAVE,0                       ,IDS_EXCLUSIVE,0};
	m_listProcesses.GetWindowRect(rect);
	
	LV_COLUMN	lvcolumn;
	CString s;

	lvcolumn.mask	= LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvcolumn.fmt	= LVCFMT_LEFT;

	for (int i = 0; i < nColumns; i++)  // add the columns to the list control
	{
		lvcolumn.cx = MulDiv(rect.Width(), w[i], 100);
		if (h[i]) 
		{
			GetDlgItemText(h[i], s);
		}
		else if (ht[i]) 
		{
			s.LoadString(ht[i]);
		}
		s.Replace(_T(":"), _T(""));
		s.Replace(_T("&"), _T(""));
		lvcolumn.pszText  = (LPTSTR)LPCTSTR(s);
		lvcolumn.iSubItem = i;

		m_listProcesses.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::FillHosts()
{
	m_listHosts.ResetContent();

	CString sName;
	DWORD dw;
	CHost* pHost;
	int iIndex = -1;

	pHost = m_pHostArray->GetLocalHost();
	sName = pHost->GetName();
	dw = pHost->GetID().GetID();
	iIndex = m_listHosts.AddString(sName);
	m_listHosts.SetItemData(iIndex, dw);

	for (int i=0; i<m_pHostArray->GetSize(); i++)
	{
		pHost = m_pHostArray->GetAt(i);
		if (   (pHost->IsSMS()   == FALSE)
			&& (pHost->IsEMail() == FALSE)
			&& (pHost->IsFAX()   == FALSE)
			)
		{
			sName = pHost->GetName() + _T(" (") + pHost->GetTyp() + _T(")");
			dw = pHost->GetID().GetID();
			iIndex = m_listHosts.AddString(sName);
			m_listHosts.SetItemData(iIndex, dw);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::FillPermissions()
{
	m_cbPermission.ResetContent();

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
		j = m_cbPermission.AddString(s);
		m_cbPermission.SetItemData(j,dw);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::FillProcesses()
{
	m_listProcesses.DeleteAllItems();
	m_pSelectedProcess = NULL;

	CProcess* pProcess;
	for (int i=0 ; i<m_pProcessList->GetSize() ; i++)
	{
		if (m_pProcessList->GetAt(i)->IsGuardTour())
		{
			pProcess = m_pProcessList->GetAt(i);
			InsertProcess(pProcess);
		}
	}
	int iSelect = (m_listProcesses.GetItemCount() > 0) ? 0 : -1;
	SelectProcessItem(iSelect);
}
/////////////////////////////////////////////////////////////////////////////
int CGuardTourPage::InsertProcess(CProcess* pProcess)
{
	int iIndex = -1;
	if (pProcess)
	{
		CString sText;
		LV_ITEM lvis;
		lvis.mask = LVIF_TEXT|LVIF_PARAM;
		int iCount = m_listProcesses.GetItemCount();
		lvis.iItem = iCount;
		lvis.iSubItem = 0;
		lvis.lParam = (long)pProcess;
		lvis.pszText = (LPTSTR)LPCTSTR(pProcess->GetName());
		lvis.cchTextMax = pProcess->GetName().GetLength();
		iIndex = m_listProcesses.InsertItem(&lvis);
		lvis.iItem = iIndex;

		DWORD id = pProcess->GetPermission().GetID();
		int i, n;
		n = m_cbPermission.GetCount();
		for (i=0; i<n; i++)
		{
			if (id == m_cbPermission.GetItemData(i))
			{
				m_cbPermission.GetLBText(i, sText);
				break;
			}
		}
		m_listProcesses.SetItemText(lvis.iItem, 1, sText);

		sText.Format(_T("%d"), pProcess->GetCallTime());
		m_listProcesses.SetItemText(lvis.iItem, 2, sText);

		sText.Format(_T("%d"), pProcess->GetSwitchTime());
		sText += _T(" ") + m_sSeconds;
		m_listProcesses.SetItemText(lvis.iItem, 3, sText);

		if (pProcess->IsHold()) GetDlgItemText(IDC_GUARDTOUR_HOLD, sText);
		else                    GetDlgItemText(IDC_SAVE_FOREVER, sText);
		sText.Replace(_T("&"), _T(""));
		m_listProcesses.SetItemText(lvis.iItem, 4, sText);
		
		sText.LoadString(pProcess->GetSave() ? IDS_YES: IDS_NO);
		m_listProcesses.SetItemText(lvis.iItem, 5, sText);

		sText.LoadString(pProcess->GetFullScreen() ? IDS_YES: IDS_NO);
		m_listProcesses.SetItemText(lvis.iItem, 6, sText);

		sText.LoadString(pProcess->GetExclusive() ? IDS_YES: IDS_NO);
		m_listProcesses.SetItemText(lvis.iItem, 7, sText);

		sText.LoadString(pProcess->GetDisconnect() ? IDS_YES: IDS_NO);
		m_listProcesses.SetItemText(lvis.iItem, 8, sText);
	}
	else
	{
		WK_TRACE_ERROR(_T("CGuardTourPage::InsertProcess pProcess = NULL\n"));
	}
	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::SelectProcessItem(int i)
{
	ControlToProcess();

	m_iSelectedProcessItem = i;
	if (m_iSelectedProcessItem != -1)
	{
		m_pSelectedProcess = (CProcess*)m_listProcesses.GetItemData(m_iSelectedProcessItem);
	}
	else
	{
		m_pSelectedProcess = NULL;
	}

	ProcessToControl();
	m_listProcesses.SetItemState(m_iSelectedProcessItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::SelectHostItem(int i)
{
	m_iSelectedCamerasItem = -1;
	m_sCameras = _T("");
	m_iSelectedHostItem = i;
	m_listHosts.SetCurSel(m_iSelectedHostItem);
	if (m_iSelectedHostItem != -1)
	{
		CSecID idHost(m_listHosts.GetItemData(m_iSelectedHostItem));
		WORD wSubID = idHost.GetSubID();
		for (int i=0; i<m_waHosts.GetSize(); i++)
		{
			if (m_waHosts.GetAt(i) == wSubID)
			{
				m_iSelectedCamerasItem = i;
				m_sCameras = m_saCameras.GetAt(i);
				break;
			}
		}
		CheckHostItemStatus();
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::SelectHostItemBySubID(WORD wSubID)
{
	for (int i=0 ; i<m_listHosts.GetCount() ; i++)
	{
		CSecID idHost(m_listHosts.GetItemData(i));
		if (idHost.GetSubID() == wSubID)
		{
			SelectHostItem(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::CheckHostItemStatus()
{
	if (m_iSelectedHostItem != -1)
	{
		CSecID idHost(m_listHosts.GetItemData(m_iSelectedHostItem));
		BOOL  bChecked = m_listHosts.GetCheck(m_iSelectedHostItem);

		if (bChecked)
		{
			AddHost();
		}
		else
		{
			RemoveHost();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::AddHost()
{
	if (m_iSelectedHostItem != -1)
	{
		BOOL bFound = FALSE;
		CSecID idHost(m_listHosts.GetItemData(m_iSelectedHostItem));
		for (int i=0 ; !bFound && i<m_waHosts.GetSize() ; i++)
		{
			if (m_waHosts.GetAt(i) == idHost.GetSubID())
			{
				bFound = TRUE;
			}
		}
		if (!bFound)
		{
			m_iSelectedCamerasItem = m_waHosts.Add(idHost.GetSubID());
			m_saCameras.Add(m_sCameras);
			SetModified();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::RemoveHost()
{
	if (m_iSelectedHostItem != -1)
	{
		BOOL bFound = FALSE;
		CSecID idHost(m_listHosts.GetItemData(m_iSelectedHostItem));
		for (int i=0 ; !bFound && i<m_waHosts.GetSize() ; i++)
		{
			if (m_waHosts.GetAt(i) == idHost.GetSubID())
			{
				m_waHosts.RemoveAt(i);
				m_iSelectedCamerasItem = i;
				bFound = TRUE;
				break;
			}
		}
		if (bFound)
		{
			m_saCameras.RemoveAt(m_iSelectedCamerasItem);
			m_iSelectedCamerasItem = -1;
			SetModified();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::ProcessToControl()
{
	if (m_pSelectedProcess)
	{
		CSecID idPermission = SECID_NO_ID;
		int i, j, nCount, nSize, nItem;
		m_pSelectedProcess->GetGuardTourData(	m_sName,
												idPermission,
												m_dwNrOfTries,
												m_dwSwitchTime,
												m_iHold,
												m_bSave,
												m_bFullScreen,
												m_bExclusive,
												m_bDisconnect,
												m_waHosts,
												m_saCameras
												);

		m_cbPermission.SetCurSel(-1);
		for (i=0 ; i<m_cbPermission.GetCount() ; i++)
		{
			if (m_cbPermission.GetItemData(i) == idPermission.GetID())
			{
				m_cbPermission.SetCurSel(i);
				break;
			}
		}
		
		nCount = m_listHosts.GetCount();
		for (j=0 ; j<nCount; j++)
		{
			m_listHosts.SetCheck(j, FALSE);
		}
		nSize = m_waHosts.GetSize();
		nItem = 0;
		LB_DATA lb;
		for (i=0; i<nSize; i++)
		{
			nCount = m_listHosts.GetCount();
			for (j=0 ; j<nCount; j++)
			{
				if (LOWORD(m_listHosts.GetItemData(j)) == m_waHosts[i])
				{
					m_listHosts.SetCheck(j, TRUE);
					if (j != nItem)
					{
						RemoveHostItem(j, lb);
						InsertHostItem(nItem, lb);
					}
					nItem++;
					break;
				}
			}
		}
		int iSelect = (m_listHosts.GetCount() > 0) ? 0 : -1;
		SelectHostItem(iSelect);
	}
	else
	{
		for (int i=0 ; i<m_listHosts.GetCount(); i++)
		{
			m_listHosts.SetCheck(i, FALSE);
		}
	}
	UpdateData(FALSE);
	EnableExceptNew();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::ControlToProcess()
{
	if (m_pSelectedProcess)
	{
		UpdateData();

		CSecID idPermission = SECID_NO_ID;
		int nCount, j, i = m_cbPermission.GetCurSel();
		if (i != CB_ERR)
		{
			idPermission.Set(m_cbPermission.GetItemData(i));
		}
		
		CStringArray saCameras;
		CWordArray   waHosts;
		nCount = m_listHosts.GetCount();
		for (i=0; i<nCount; i++)
		{
			CSecID idHost(m_listHosts.GetItemData(i));
			for (j=0; j<m_waHosts.GetSize(); j++)
			{
				if (idHost.GetSubID() == m_waHosts[j])
				{
					waHosts.Add(m_waHosts[j]);
					saCameras.Add(m_saCameras[j]);
					break;
				}
			}
		}

		
		m_pSelectedProcess->SetGuardTour( m_sName,
			idPermission,
			m_dwNrOfTries,
			m_dwSwitchTime,
			(BOOL)m_iHold,
			m_bSave,
			m_bFullScreen,
			m_bExclusive,
			m_bDisconnect,
			waHosts,
			saCameras);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnItemchangedGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != m_iSelectedProcessItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectProcessItem(pNMListView->iItem);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnClickGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}
	BOOL bEnable = (m_listProcesses.GetSelectedCount() > 1) ? FALSE : TRUE;
	GetDlgItem(IDC_GUARDTOUR_HOSTS)->EnableWindow(bEnable);
	GetDlgItem(IDC_GUARDTOUR_NAME)->EnableWindow(bEnable);
	CWnd *pWnd = GetDlgItem(IDC_GUARDTOUR_CAMERAS);
	if (pWnd->IsWindowEnabled() || !bEnable)
	{
		pWnd->EnableWindow(bEnable);
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnDblclkGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnRclickGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnRdblclkGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnSelchangeHosts() 
{
	int iSelected = m_listHosts.GetCurSel();
	if (iSelected != m_iSelectedHostItem)
	{
		SelectHostItem(iSelected);
	}
	else
	{
		CheckHostItemStatus();
	}
	EnableExceptNew();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnChangeName() 
{
	UpdateData();
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 0, m_sName);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnChangeCameras() 
{
	UpdateData();
	if (m_iSelectedCamerasItem != -1)
	{
		CString sCameras(m_sCameras);
		if (sCameras.Find(_T(" ")) != -1)
		{
			sCameras.Replace(_T(" "), _T(""));
		}
		m_saCameras.SetAt(m_iSelectedCamerasItem, sCameras);
	}
	else
	{
		WK_TRACE_ERROR(_T("CGuardTourPage::OnChangeCameras m_iSelectedCamerasItem == -1\n"));
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnSelchangePermission() 
{
	UpdateData();
	CString sText;
	m_cbPermission.GetWindowText(sText);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 1, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		DWORD dwPermission = m_cbPermission.GetItemData(m_cbPermission.GetCurSel());
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetPermission(CSecID(dwPermission));
					m_listProcesses.SetItemText(nPos, 1, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnRadioHold() 
{
	UpdateData();
	CString sText;
	if (m_iHold) GetDlgItemText(IDC_GUARDTOUR_HOLD, sText);
	else         GetDlgItemText(IDC_SAVE_FOREVER, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 4, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetHold(m_iHold);
					m_listProcesses.SetItemText(nPos, 4, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnChangeTries() 
{
	UpdateData();
	CString sText;
	sText.Format(_T("%d"), m_dwNrOfTries);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 2, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetCallTime(m_dwNrOfTries);
					m_listProcesses.SetItemText(nPos, 2, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnChangeSwitchTime() 
{
	UpdateData();
	CString sText;
	sText.Format(_T("%d"), m_dwSwitchTime);
	sText += _T(" ") + m_sSeconds;
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 3, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetSwitchTime(m_dwSwitchTime);
					m_listProcesses.SetItemText(nPos, 3, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnCheckFullScreen() 
{
	UpdateData();
	CString sText;
	sText.LoadString(m_bFullScreen ? IDS_YES: IDS_NO);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 6, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetFullscreen(m_bFullScreen);
					m_listProcesses.SetItemText(nPos, 6, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnCheckSave() 
{
	UpdateData();
	CString sText;
	sText.LoadString(m_bSave ? IDS_YES: IDS_NO);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 5, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetSave(m_bSave);
					m_listProcesses.SetItemText(nPos, 5, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnCheckExclusive() 
{
	UpdateData();
	CString sText;
	sText.LoadString(m_bExclusive ? IDS_YES: IDS_NO);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 7, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetExclusive(m_bExclusive);
					m_listProcesses.SetItemText(nPos, 7, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnCheckDisconnect() 
{
	UpdateData();

	CString sText;
	sText.LoadString(m_bDisconnect ? IDS_YES: IDS_NO);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 8, sText);

	if (m_listProcesses.GetSelectedCount() > 1)
	{
		POSITION pos = m_listProcesses.GetFirstSelectedItemPosition();
		CProcess	*pItem;
		int nPos;
		while (pos)
		{
			nPos = m_listProcesses.GetNextSelectedItem(pos);
			if (nPos != m_iSelectedProcessItem)
			{
				pItem = (CProcess*)m_listProcesses.GetItemData(nPos);
				if (pItem)
				{
					pItem->SetDisconnect(m_bDisconnect);
					m_listProcesses.SetItemText(nPos, 8, sText);
				}
			}
		}
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGuardTourPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::OnDeltaposGtpHostSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int     nSel   = m_listHosts.GetCurSel(),
		     nCount = m_listHosts.GetCount();
	LB_DATA lb;
	if (pNMUpDown->iDelta < 0) // Button Up
	{
		if (nSel > 0)
		{
			RemoveHostItem(nSel-1, lb);
			InsertHostItem(nSel  , lb);
			SetModified();
		}
	}
	else								// Button Down
	{
		if (nSel < nCount-1)
		{
			RemoveHostItem(nSel+1, lb);
			InsertHostItem(nSel  , lb);
			SetModified();
		}
	}
		
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGuardTourPage::RemoveHostItem(int nSel, LB_DATA &lb)
{
	lb.dwData = m_listHosts.GetItemData(nSel);
	lb.bCheck = m_listHosts.GetCheck(nSel);
	m_listHosts.GetText(nSel, lb.sItem);
	m_listHosts.DeleteString(nSel);
}
/////////////////////////////////////////////////////////////////////////////
int CGuardTourPage::InsertHostItem(int nSel, LB_DATA &lb)
{
	nSel = m_listHosts.InsertString(nSel, lb.sItem);
	m_listHosts.SetItemData(nSel, lb.dwData);
	m_listHosts.SetCheck(nSel, lb.bCheck);
	return nSel;
}
