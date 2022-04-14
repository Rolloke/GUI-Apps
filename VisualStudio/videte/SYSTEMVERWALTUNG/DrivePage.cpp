/* GlobalReplace: CDVRDrive --> CIPCDrive */
// DrivePage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "DrivePage.h"

#include "DlgMapNetworkDrive.h"
#include <Winnetwk.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrivePage dialog

IMPLEMENT_DYNAMIC(CDrivePage, CSVPage)

CDrivePage::CDrivePage(CSVView* pParent) : CSVPage(CDrivePage::IDD)
{
	m_pParent = pParent;
	m_pDrives = pParent->GetDocument()->GetDrives();
	m_pArchivs = pParent->GetDocument()->GetArchivs();
	m_iSelectedItem = -1;
	m_pSelectedDrive = NULL;

	//{{AFX_DATA_INIT(CDrivePage)
	m_iUsedAs = -1;
	m_dwUse = 0;
	m_sRoot = _T("");
	m_sDrivesMB = _T("");
	m_sArchivsMB = _T("");
	m_bAll = FALSE;
	//}}AFX_DATA_INIT

	if (m_pParent->GetDocument()->IsLocal())
	{
		m_bNetworkFnkOk = theApp.IsNetExeOK() || CIPCDrive::InitNetworkApi();
	}
	else
	{
		m_bNetworkFnkOk = FALSE;
	}

	m_bAllowNetworkStorage = GetProfile().GetInt(SECTION_DBS, DBS_ALLOW_NETWORK_STORAGE, 0);

	m_sDrivesMB.Format(_T("%d"), m_pDrives->GetAvailableMB());
	m_sArchivsMB.Format(_T("%d"), m_pArchivs->GetSizeMB());
	Create(IDD,(CWnd*)m_pParent);
}


void CDrivePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrivePage)
	DDX_Control(pDX, IDC_ALL, m_checkAll);
	DDX_Control(pDX, IDC_EDIT_USE, m_editUse);
	DDX_Control(pDX, IDC_RADIO_SYSTEM, m_radioSystem);
	DDX_Control(pDX, IDC_RADIO_NORMAL, m_radioNormal);
	DDX_Control(pDX, IDC_RADIO_DATABASE, m_radioDatabase);
	DDX_Control(pDX, IDC_RADIO_BACKUP_WRITE_READ, m_radioBackupWrite);
	DDX_Control(pDX, IDC_RADIO_BACKUP_READ, m_radioBackupRead);
	DDX_Control(pDX, IDC_RADIO_REMOVABLE, m_radioRHD);
	DDX_Control(pDX, IDC_DRIVES, m_Drives);
	DDX_Radio(pDX, IDC_RADIO_SYSTEM, m_iUsedAs);
	DDX_Text(pDX, IDC_EDIT_USE, m_dwUse);
	DDX_Text(pDX, IDC_STATIC_DRIVE, m_sRoot);
	DDX_Text(pDX, IDC_STATIC_DRIVES_MB, m_sDrivesMB);
	DDX_Text(pDX, IDC_STATIC_ARCHIVS_MB, m_sArchivsMB);
	DDX_Check(pDX, IDC_ALL, m_bAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrivePage, CSVPage)
	//{{AFX_MSG_MAP(CDrivePage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DRIVES, OnItemchangedDrives)
	ON_EN_CHANGE(IDC_EDIT_USE, OnChangeEditUse)
	ON_BN_CLICKED(IDC_RADIO_BACKUP_READ, OnRadioBackupRead)
	ON_BN_CLICKED(IDC_RADIO_BACKUP_WRITE_READ, OnRadioBackupWriteRead)
	ON_BN_CLICKED(IDC_RADIO_DATABASE, OnRadioDatabase)
	ON_BN_CLICKED(IDC_RADIO_NORMAL, OnRadioNormal)
	ON_BN_CLICKED(IDC_RADIO_SYSTEM, OnRadioSystem)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	ON_NOTIFY(NM_CLICK, IDC_DRIVES, OnClickDrives)
	ON_NOTIFY(NM_DBLCLK, IDC_DRIVES, OnDblclkDrives)
	ON_NOTIFY(NM_RCLICK, IDC_DRIVES, OnRclickDrives)
	ON_NOTIFY(NM_RDBLCLK, IDC_DRIVES, OnRdblclkDrives)
	ON_BN_CLICKED(IDC_RADIO_REMOVABLE, OnRadioRemovable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrivePage message handlers
BOOL CDrivePage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::SaveChanges()
{
	WK_TRACE_USER(_T("Datenbank-Laufwerks-Einstellungen wurden geändert\n"));
	UpdateData();
	m_pDrives->Save(GetProfile());
	if (m_pParent->GetDocument()->IsLocal() == FALSE)
	{
		m_pParent->GetDocument()->SaveChangesRemoteDrives();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::CancelChanges()
{
	// CAVEAT: m_pDrives->Init() will use GetLogicalDrives(),
	// that is a local function!
	if (m_pParent->GetDocument()->IsLocal())
	{
		m_pDrives->Init(GetProfile());
	}
	else
	{
		m_pParent->GetDocument()->CancelChangesRemoteDrives();
	}
	FillDriveListBox();
	SelectItem((m_Drives.GetItemCount()>0) ? 0 : -1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDrivePage::CanNew()
{
	return m_bNetworkFnkOk;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnNew()
{
	CDlgMapNetworkDrive dlg(GetProfile());
	if (m_pSelectedDrive && m_pSelectedDrive->IsNetworkDrive())
	{
		CString sVolume = m_pSelectedDrive->GetVolumeString();
		dlg.SetVolume(sVolume);
	}
	if (dlg.DoModal() == IDOK)
	{
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDrivePage::CanDelete()
{
	if (m_pSelectedDrive && m_pSelectedDrive->IsNetworkDrive())
	{
		return m_bNetworkFnkOk;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnDelete()
{
	if (m_pSelectedDrive && m_pSelectedDrive->IsNetworkDrive())
	{
		if (CIPCDrive::InitNetworkApi())
		{
			BOOL bConnected = m_pSelectedDrive->IsConnected();
			m_pSelectedDrive->SetType(DRIVE_DELETED);
			m_pSelectedDrive->Save(GetProfile());

			if (bConnected)
			{
				bConnected = !m_pSelectedDrive->DisconnectNetworkDrive();
			}
			else
			{
				m_pDrives->Remove(m_pSelectedDrive);
				WK_DELETE(m_pSelectedDrive);
				UpdateDrives();
				bConnected = FALSE;
			}

			m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
			SetModified();
		}
		else
		{
			CString sCmd;
			sCmd.Format(_T("net use %c: /delete"), m_pSelectedDrive->GetLetter());
			ExecuteProgram(sCmd, NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::EnableControls()
{
	BOOL bEnableBase = FALSE;
	BOOL bExtraEnable = FALSE;

	// enable controls only if there are drives and one is selected
	if (m_Drives.GetItemCount() && m_pSelectedDrive) 
	{
		bEnableBase = TRUE;
	}
	// drive list
	bExtraEnable = bEnableBase;
	m_Drives.EnableWindow(bExtraEnable);

	// system only if drive is system
	bExtraEnable = bEnableBase && m_pSelectedDrive->IsSystem();
	m_radioSystem.EnableWindow(bExtraEnable);
	// normal
	bExtraEnable = bEnableBase
					&& !m_pSelectedDrive->IsSystem();
	m_radioNormal.EnableWindow(bExtraEnable);

	CWK_Dongle dongle;
	
	if (dongle.IsReceiver())
	{
		// database only if no cdrom nor network
		bExtraEnable = bEnableBase
						&& !m_pSelectedDrive->IsCDROM()
						&& !m_pSelectedDrive->IsNetworkDrive();
	}
	else // Auf Transmittern soll nicht auf der Systempartition gespeichert werden
	{
		// database only if no cdrom nor network nor system
		BOOL bIsNotAllowedDrive =    bEnableBase
								  && m_pSelectedDrive->IsNetworkDrive();
		if (m_bAllowNetworkStorage)
		{
			bIsNotAllowedDrive = FALSE;
		}
		bExtraEnable = bEnableBase
						&& !m_pSelectedDrive->IsCDROM()
						&& !bIsNotAllowedDrive
						&& !m_pSelectedDrive->IsSystem();
	}
	m_radioDatabase.EnableWindow(bExtraEnable);
	m_radioRHD.EnableWindow(bExtraEnable);
	// backup write only if no system nor cdrom
	bExtraEnable = bEnableBase
					&& !m_pSelectedDrive->IsSystem();
	m_radioBackupWrite.EnableWindow(bExtraEnable);
	// backup read only if no system
	bExtraEnable = bEnableBase
					&& !m_pSelectedDrive->IsSystem();
	m_radioBackupRead.EnableWindow(bExtraEnable);
	// editUse only if not useAll
	bExtraEnable = bEnableBase && !m_bAll;
	m_editUse.EnableWindow(bExtraEnable);
	// useAll
	bExtraEnable = bEnableBase
					&& (m_pSelectedDrive->IsDatabase() || m_pSelectedDrive->IsRemoveableDatabase())
					&& !m_pSelectedDrive->IsSystem();
	m_checkAll.EnableWindow(bExtraEnable);

	// todo TO3000
	// Beim TO_3000 Prototyp erstmal nur Informationen anzeigen
	// es darf nichts verstellt werden können!
	if (dongle.GetProductCode() == IPC_TOBS_TO3K)
	{
		m_radioSystem.EnableWindow(FALSE);
		m_radioNormal.EnableWindow(FALSE);
		m_radioDatabase.EnableWindow(FALSE);
		m_radioBackupWrite.EnableWindow(FALSE);
		m_radioBackupRead.EnableWindow(FALSE);
		m_radioRHD.EnableWindow(FALSE);
		m_editUse.EnableWindow(FALSE);
		m_checkAll.EnableWindow(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDrivePage::IsDataValid()
{
	if (m_pArchivs->GetSizeMB() > m_pDrives->GetAvailableMB())
	{
		DWORD dwOversize = m_pArchivs->GetSizeMB() - m_pDrives->GetAvailableMB();
		CString sMsg;
		sMsg.Format(IDP_ARCHIVS_TOO_BIG, dwOversize);
		AfxMessageBox(sMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	if (FALSE == m_bAll && 0 == m_pSelectedDrive->GetQuotaMB())
	{
		AfxMessageBox(IDP_ARCHIV_SIZE_ZERO,MB_OK|MB_ICONSTOP);
		m_editUse.SetFocus();
		m_editUse.SetSel(0, -1);
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDrivePage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	m_Images.Create(IDB_DRIVES,16,1,RGB(255,0,255));

	m_Drives.SetImageList(&m_Images,LVSIL_SMALL);

	CRect			rect;
	LV_COLUMN		lvcolumn;
	int w[] = {12,20,23,15,15,15};	// CAVET add sum has to be 100
	int i,c;
	c = 6;
	CString s[6];

	m_Drives.GetWindowRect(&rect);
	GetDlgItem(IDC_TXT_DRIVE)->GetWindowText(s[0]);
	s[1].LoadString(IDS_TYP);
	GetDlgItem(IDC_GROUP_USE)->GetWindowText(s[2]);
	s[3].LoadString(IDS_USED);
	s[4].LoadString(IDS_SIZE);
	s[5].LoadString(IDS_QUOTA);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0; i < c; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s[i]);
		lvcolumn.iSubItem = i;
		m_Drives.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}
	
	LONG dw = m_Drives.GetExtendedStyle();
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_Drives.SetExtendedStyle(dw);

	if (!GetDongle().RunCDRWriter())
	{
		m_radioBackupWrite.ShowWindow(SW_HIDE);
		m_radioBackupRead.ShowWindow(SW_HIDE);
	}

	FillDriveListBox();
	SelectItem((m_Drives.GetItemCount()>0) ? 0 : -1);
	
	IsDataValid();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::FillDriveListBox()
{
	int i,c;

	m_Drives.DeleteAllItems();

	c = m_pDrives->GetSize();

	if (c>0)
	{
		InsertItem(m_pDrives->GetAt(0),TRUE);
	}
	for (i=1; i<c; i++)
	{
		InsertItem(m_pDrives->GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
int CDrivePage::InsertItem(CIPCDrive* pD, BOOL bSelectIt /*= FALSE*/)
{
	LV_ITEM lvis;
	CString sRoot,sTyp,sUsedAs,sUsed,sMB,sQuota,sVolume;
	int i,c,iImage = 0;

	sRoot   = pD->GetRootString();
	sVolume = pD->GetVolumeString();
	int nFind = sVolume.Find(_T(";"));
	if (nFind != -1)
	{
		sVolume = sVolume.Left(nFind);
	}
	if (!sVolume.IsEmpty())
	{
		sRoot += _T(" (") + sVolume + _T(")");
	}
	
	iImage = GetTypImage(pD->GetType());
	sTyp = GetTypString(pD->GetType());
	sUsedAs = GetUsedString(pD);
	sUsed.Format(_T("%d"),pD->GetUsedMB());
	sMB.Format(_T("%d"),pD->GetMB());

	if (pD->IsDatabase() || pD->IsRemoveableDatabase())
	{
		if (pD->GetQuotaMB()==0)
		{
			sQuota.LoadString(IDS_ALL);
		}
		else
		{
			sQuota.Format(_T("%d"),pD->GetQuotaMB());
		}
	}
	else
	{
		sMB.Format(_T("%d"),pD->GetMB());
		sQuota = _T("0");
	}

	c = m_Drives.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvis.pszText = sRoot.GetBuffer(0);
	lvis.cchTextMax = sRoot.GetLength();
	lvis.lParam = (LPARAM)pD;
	lvis.iItem = c;
	lvis.iSubItem = 0;
	lvis.iImage = iImage;

	i = m_Drives.InsertItem(&lvis);
	sRoot.ReleaseBuffer();

	m_Drives.SetItemText(i,1,sTyp);
	m_Drives.SetItemText(i,2,sUsedAs);
	m_Drives.SetItemText(i,3,sUsed);
	m_Drives.SetItemText(i,4,sMB);
	m_Drives.SetItemText(i,5,sQuota);

	if (bSelectIt)
	{
		lvis.iItem = i;
		lvis.iSubItem = 0;
		lvis.mask = LVIF_STATE;
		lvis.state = LVIS_SELECTED;
		lvis.stateMask = LVIS_SELECTED;
		m_Drives.SetItem(&lvis);
	}

	EnableControls();
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::SelectItem(int i)
{
	if (i>=0)
	{
		m_iSelectedItem = i;
		m_Drives.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		m_pSelectedDrive = (CIPCDrive*)m_Drives.GetItemData(m_iSelectedItem);
		SelectedItemToControl();
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedDrive = NULL;
		m_dwUse = 0;
	}
	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
CString CDrivePage::GetTypString(UINT dt)
{
	CString ret;

	//GetDriveType
	switch (dt)
	{
	case DRIVE_UNKNOWN:
		// The drive type cannot be determined. 
		break;
	case DRIVE_NO_ROOT_DIR:
		// The root directory does not exist. 
		break;
	case DRIVE_REMOVABLE:
		// The disk can be removed from the drive. 
		ret.LoadString(IDS_DRIVE_FD);
		break;
	case DRIVE_FIXED:
		// The disk cannot be removed from the drive. 
		ret.LoadString(IDS_DRIVE_HD);
		break;
	case DRIVE_REMOTE:
		// The drive is a remote (network) drive. 
		ret.LoadString(IDS_DRIVE_NET);
		break;
	case DRIVE_CDROM:
		// The drive is a CD-ROM drive. 
		ret.LoadString(IDS_DRIVE_CD);
		break;
	case DRIVE_RAMDISK:
		// The drive is a RAM disk
		break;
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
int	CDrivePage::GetTypImage(UINT dt)
{
	//GetDriveType
	switch (dt)
	{
	case DRIVE_UNKNOWN:
		// The drive type cannot be determined. 
		return 0;
	case DRIVE_NO_ROOT_DIR:
		// The root directory does not exist. 
		return 0;
	case DRIVE_REMOVABLE:
		// The disk can be removed from the drive. 
		return 0;
	case DRIVE_FIXED:
		// The disk cannot be removed from the drive. 
		return 1;
	case DRIVE_REMOTE:
		// The drive is a remote (network) drive. 
		return 2;
	case DRIVE_CDROM:
		// The drive is a CD-ROM drive. 
		return 3;
	case DRIVE_RAMDISK:
		// The drive is a RAM disk
		return 0;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CString CDrivePage::GetUsedString(CIPCDrive* pD)
{
	CString ret;

	// CAREFUL: don't change order carelessly, take care of priority
	// database is priority before all others, even system can be database
	if (pD->IsNetworkDrive() && !pD->IsConnected())
	{
		ret.LoadString(IDS_DISCONNECTED_ERROR);
	}
	else if (   pD->IsDatabase())
	{
		ret.LoadString(IDS_USE_DATABASE);
	}
	else if (pD->IsRemoveableDatabase())
	{
		ret.LoadString(IDS_REMOVABLE);
	}
	// system is priority before others than database
	// shall not used for others except database
	else if (pD->IsSystem())
	{
		ret.LoadString(IDS_USE_SYSTEM);
	}
	else if (pD->IsReadBackup())
	{
		ret.LoadString(IDS_USE_BACKUP_READ);
	}
	else if (pD->IsWriteBackup())
	{
		ret.LoadString(IDS_USE_BACKUP_WRITE_READ);
	}
	else
	{
		ret.LoadString(IDS_USE_NORMAL);
	}

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
int CDrivePage::GetUsedRadio(CIPCDrive* pD)
{
	// CAREFUL: don't change order carelessly, take care of priority
	// database is priority before all others, even system can be database
	if (pD->IsDatabase())
	{
		return 2;
	}
	// system is priority before others than database
	// shall not used for others except database
	else if (pD->IsSystem())
	{
		return 0;
	}
	else if (pD->IsReadBackup())
	{
		return 4;
	}
	else if (pD->IsWriteBackup())
	{
		return 3;
	}
	else if (pD->IsRemoveableDatabase())
	{
		return 5;
	}
	// others are normal, not used
	else
	{
		return 1;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::SelectedItemToControl()
{
	m_sRoot = m_pSelectedDrive->GetRootString();
	if (   m_pSelectedDrive->IsDatabase()
		|| m_pSelectedDrive->IsRemoveableDatabase())
	{
		m_dwUse = m_pSelectedDrive->GetQuotaMB();
		if (m_pSelectedDrive->IsSystem())
		{
			// the system drive can obviously not be used complete for database
			m_bAll = FALSE;
		}
		else {
			m_bAll = (m_dwUse == 0);
		}
	}
	else
	{
		m_dwUse = 0;
		m_bAll = TRUE;
	}

	CString sQuota;
	if (   m_pSelectedDrive->IsDatabase()
		|| m_pSelectedDrive->IsRemoveableDatabase())
	{
		if (m_pSelectedDrive->GetQuotaMB()==0)
		{
			sQuota.LoadString(IDS_ALL);
		}
		else
		{
			sQuota.Format(_T("%d"),m_pSelectedDrive->GetQuotaMB());
		}
	}
	else
	{
		sQuota = _T("0");
	}
	m_Drives.SetItemText(m_iSelectedItem,2,GetUsedString(m_pSelectedDrive));
	m_Drives.SetItemText(m_iSelectedItem,5,sQuota);
	m_iUsedAs = GetUsedRadio(m_pSelectedDrive);
	m_sDrivesMB.Format(_T("%d"), m_pDrives->GetAvailableMB());
	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnItemchangedDrives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			m_pSelectedDrive = (CIPCDrive*)pNMListView->lParam;
			m_iSelectedItem = pNMListView->iItem;
			SelectedItemToControl();
		}
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnClickDrives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Drives.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnDblclkDrives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Drives.SetItemState(m_iSelectedItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}	
	if (m_pSelectedDrive && m_pSelectedDrive->IsNetworkDrive() && !m_pSelectedDrive->IsConnected())
	{
		if (!m_pSelectedDrive->ConnectNetworkDrive())
		{
			CString sFmt, sError = GetLastErrorString();
			sFmt.LoadString(AFX_IDS_INVALID_ARG_EXCEPTION);
			sFmt += _T("\n") + sError;
			AfxMessageBox(sFmt, MB_OK|MB_ICONERROR);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRclickDrives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Drives.SetItemState(m_iSelectedItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
	if (m_pSelectedDrive && m_pSelectedDrive->IsNetworkDrive())
	{
		CDlgMapNetworkDrive dlg(GetProfile());
		CString sVolume = m_pSelectedDrive->GetVolumeString();
		CString sRoot   = m_pSelectedDrive->GetRootString();
		dlg.SetVolume(sVolume);
		dlg.SetRoot(sRoot);
		dlg.DoModal();
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRdblclkDrives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Drives.SetItemState(m_iSelectedItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnChangeEditUse() 
{
	UpdateData();

	if (m_dwUse <= m_pSelectedDrive->GetMB()-20)
	{
		m_pSelectedDrive->SetQuota(m_dwUse);
	}
	else
	{
		AfxMessageBox(IDP_ARCHIV_SIZE_TO_BIG,MB_OK|MB_ICONSTOP);
		m_dwUse = m_pSelectedDrive->GetMB();
		m_pSelectedDrive->SetQuota(m_dwUse);
		m_editUse.SetFocus();
		m_editUse.SetSel(0, -1);
	}
	SelectedItemToControl();
	m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDrivePage::SetNewTyp(DvrDriveUse dt)
{
	if (m_pSelectedDrive)
	{
		if (   (m_pSelectedDrive->IsDatabase() || m_pSelectedDrive->IsRemoveableDatabase())
			&& dt != DVR_DB_DRIVE
			&& dt != DVR_REMOVABLE_DB_DRIVE)
		{
			// no longer database drive

			// check for fixed archives
			CString sRoot = m_pSelectedDrive->GetRootString();
			if (sRoot.GetLength() >=2 && sRoot.GetAt(1) == _T(':')) {
				sRoot = sRoot.Left(2);
			}
			if (m_pArchivs->GetSizeMBForFixedDrive(sRoot) > 0)
			{
				// OOPS fixed archive on this drive
				AfxMessageBox(IDP_FIXED_ARCHIVES_ON_DRIVE,MB_OK|MB_ICONSTOP);
				m_iUsedAs = GetUsedRadio(m_pSelectedDrive);
				UpdateData(FALSE);
				return FALSE;
			}

			// check available diskspace for archives
			if (m_pDrives->GetAvailableMB() - m_pSelectedDrive->GetFreeMB() < m_pArchivs->GetSizeMB())
			{
				// OOPS not enough diskspace for archives
				AfxMessageBox(IDS_TOO_LESS_DATABASE_DRIVES,MB_OK|MB_ICONSTOP);
				m_iUsedAs = GetUsedRadio(m_pSelectedDrive);
				UpdateData(FALSE);
				return FALSE;
			}
			m_pSelectedDrive->SetQuota(0);
		}
		if (   !m_pSelectedDrive->IsDatabase() 
			&& !m_pSelectedDrive->IsRemoveableDatabase() 
			&&	(dt == DVR_DB_DRIVE || dt == DVR_REMOVABLE_DB_DRIVE)
			)
		{
			// new database drive
		}
		m_pSelectedDrive->SetUsedAs(dt);
		SelectedItemToControl();
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		SetModified();
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRadioBackupRead() 
{
	SetNewTyp(DVR_READ_BACKUP_DRIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRadioBackupWriteRead() 
{
	SetNewTyp(DVR_WRITE_BACKUP_DRIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRadioDatabase() 
{
	SetNewTyp(DVR_DB_DRIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRadioNormal() 
{
	SetNewTyp(DVR_NORMAL_DRIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRadioSystem() 
{
	SetNewTyp(DVR_SYSTEM_DRIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnRadioRemovable() 
{
	SetNewTyp(DVR_REMOVABLE_DB_DRIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::OnAll() 
{
	UpdateData();
	// useAll is only allowed at database
	if (m_bAll) {
		m_pSelectedDrive->SetQuota(0);
		SelectedItemToControl();
	}
	m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
	SetModified();
	EnableControls();
	if (!m_bAll) {
		m_editUse.SetFocus();
		m_editUse.SetSel(0, -1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDrivePage::UpdateDrives()
{
	CIPCDrives oldDrives;
	int i, nSize = m_pDrives->GetSize();
	for (i=0; i<nSize; i++)
	{
		oldDrives.Add(new CIPCDrive(*m_pDrives->GetAtFast(i)));
	}
	m_pSelectedDrive = NULL;
	m_pDrives->Init(GetProfile());

	for (i=0; i<nSize; i++)
	{
		CIPCDrive*pOldDrive = oldDrives.GetAtFast(i);
		CIPCDrive*pDrive = m_pDrives->GetDrive((char)pOldDrive->GetLetter());
		if (pDrive)
		{
			pDrive->SetUsedAs(pOldDrive->GetUsedAs());
			pDrive->SetQuota(pOldDrive->GetQuotaMB());
		}
	}

	FillDriveListBox();
	SelectItem((m_Drives.GetItemCount()>0) ? 0 : -1);
}
