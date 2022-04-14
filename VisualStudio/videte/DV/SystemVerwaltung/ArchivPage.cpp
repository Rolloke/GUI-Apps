/* GlobalReplace: CDVRDrive --> CIPCDrive */
// ArchivPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ArchivPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArchivPage dialog
CArchivPage::CArchivPage(CSVView* pParent) : CSVPage(CArchivPage::IDD)
{
	m_pParent = pParent;
	m_pSelectedArchivInfo = NULL;
	m_iSelectedItem = -1;
	m_pArchivs = pParent->GetDocument()->GetArchivs();
	m_pDrives = pParent->GetDocument()->GetDrives();
	m_pHostArray = pParent->GetDocument()->GetHosts();

	//{{AFX_DATA_INIT(CArchivPage)
	m_sName = _T("");
	m_dwSize = 15;
	m_sMax = _T("");
	m_sUsed = _T("");
	m_sWhole = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::CanNew()
{
	BOOL bReturn =    (theApp.m_pPermission->IsSpecialReceiver() == FALSE)
				   && (m_pDrives->GetAvailableMB() > m_pArchivs->GetSizeMB()
				   );
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::CanDelete()
{
	if (m_Archivs.GetItemCount()>0)
	{
		if (m_pSelectedArchivInfo)
		{
			return (m_pSelectedArchivInfo->GetID().GetSubID() != LOCAL_SEARCH_RESULT_ARCHIV_NR);
		}
		return FALSE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::CopyActualSettingsToNew(CArchivInfo* pAI)
{
	if (pAI && m_pSelectedArchivInfo) {
		pAI->SetSizeMB(m_pSelectedArchivInfo->GetSizeMB());
		pAI->SetTyp(m_pSelectedArchivInfo->GetType());
		pAI->SetSafeRingFor(m_pSelectedArchivInfo->GetSafeRingFor());
		pAI->SetFixedDrive(m_pSelectedArchivInfo->GetFixedDrive());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SaveChanges()
{
	WK_TRACE_USER(_T("Archiv-Einstellungen wurden geändert\n"));
	UpdateData();
	m_pArchivs->Save(GetProfile());
	GetProfile().DeleteSection(_T("ArchivManagment"));
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::CancelChanges()
{
	ClearArchivListBox();
	m_pArchivs->Load(GetProfile());
	FillArchivListBox();
	int	c = m_Archivs.GetItemCount();
	SelectItem((c>0) ? 0 : -1);
	EnableExceptNew();
	if (c>=0)
		m_Archivs.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::IsDataValid()
{
	// check for size != 0
	if (m_dwSize == 0)
	{
		AfxMessageBox(IDP_ARCHIVE_SIZE_ZERO, MB_OK|MB_ICONSTOP);
		m_editSize.SetFocus();
		m_editSize.SetSel(0,-1);
		return FALSE;
	}

	// Check for total archiv size greater than total drive size
	if (m_pArchivs->GetSizeMB() > m_pDrives->GetAvailableMB())
	{
		AfxMessageBox(IDP_ARCHIVS_TOO_BIG,MB_OK|MB_ICONSTOP);
		m_editSize.SetFocus();
		m_editSize.SetSel(0,-1);
		return FALSE;
	}


	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::EnableExceptNew()
{
	BOOL bEnable,bEnable2;
	int c;
	c = m_Archivs.GetItemCount();
	bEnable = (c>0);
	m_Archivs.EnableWindow(bEnable);

	bEnable2 =    m_pSelectedArchivInfo 
				&& (m_pSelectedArchivInfo->GetArchivNr() != LOCAL_SEARCH_RESULT_ARCHIV_NR)
				&& bEnable;

	m_editName.EnableWindow(bEnable2);
	m_editSize.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::UpdateSizeInfo()
{
	if (m_pSelectedArchivInfo) {
		// Ist das Archiv einem Laufwerk fest zuzugeordnet
		CString sRoot = m_pSelectedArchivInfo->GetFixedDrive();
		CIPCDrive* pDrive = NULL;
		if (sRoot.IsEmpty() == FALSE) {
			pDrive = m_pDrives->GetDrive(sRoot+_T('\\'));
		}

		DWORD dwAvailableMBForArchive;
		DWORD dwAvailableMBTotal;
		DWORD dwReservedForOtherArchives;
		if (pDrive) {
			// Wieviel Platz ist auf dem Laufwerk
			dwAvailableMBTotal = pDrive->GetAvailableMB();
			// Wieviel ist denn bereits von anderen Archiven reserviert (alle - das aktuelle)
			dwReservedForOtherArchives = m_pArchivs->GetSizeMBForFixedDrive(sRoot)
											- m_pSelectedArchivInfo->GetSizeMB();
			dwAvailableMBForArchive = dwAvailableMBTotal - dwReservedForOtherArchives;
		}
		else {
			// Wieviel Platz ist auf allen Laufwerken
			dwAvailableMBTotal = m_pDrives->GetAvailableMB();
			// Wieviel ist denn bereits von anderen Archiven reserviert (alle - das aktuelle)
			dwReservedForOtherArchives = m_pArchivs->GetSizeMB()
											- m_pSelectedArchivInfo->GetSizeMB();
			dwAvailableMBForArchive = dwAvailableMBTotal - dwReservedForOtherArchives;
		}

		m_sMax.Format(_T("%d"), dwAvailableMBForArchive);
		m_sUsed.Format(_T("%d"),m_pArchivs->GetSizeMB());
		m_sWhole.Format(_T("%d"),m_pDrives->GetAvailableMB());

		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SelectedItemToControl()
{
	m_sName = m_pSelectedArchivInfo->GetName();
	m_dwSize = m_pSelectedArchivInfo->GetSizeMB();

	UpdateSizeInfo();
	UpdateData(FALSE);
	m_editName.EnableWindow(m_pSelectedArchivInfo->GetArchivNr() != LOCAL_SEARCH_RESULT_ARCHIV_NR);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SelectItem(int i)
{
	if (i>=0)
	{
		m_iSelectedItem = i;
		m_Archivs.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		m_pSelectedArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(m_iSelectedItem);
		SelectedItemToControl();
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedArchivInfo = NULL;
		m_sName.Empty();
		m_dwSize = 0;
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::FillArchivListBox()
{
	int i,c;
	CArchivInfo* pAI;

	c = m_pArchivs->GetSize();
	
	if (c>0)
	{
		pAI = (CArchivInfo*)m_pArchivs->GetAt(0);
		InsertItem(pAI,TRUE);
	}
	for (i=1; i<c; i++)
	{
		pAI = (CArchivInfo*)m_pArchivs->GetAt(i);
		InsertItem(pAI);
	}
}
/////////////////////////////////////////////////////////////////////////////
int	 CArchivPage::InsertItem(CArchivInfo* pAI, BOOL bSelectIt /*= FALSE*/)
{
	LV_ITEM lvis;
	CString sName,sSize;
	int i,c,iImage = 0;

	sName = pAI->GetName();
	sSize.Format(_T("%d"),pAI->GetSizeMB());

	iImage = GetTypImage(pAI->GetType());

	c = m_Archivs.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvis.pszText = sName.GetBuffer(0);
	lvis.cchTextMax = sName.GetLength();
	lvis.lParam = (LPARAM)pAI;
	lvis.iItem = c;
	lvis.iSubItem = 0;
	lvis.iImage = iImage;

	i = m_Archivs.InsertItem(&lvis);
	sName.ReleaseBuffer();
	lvis.iItem = i;

	m_Archivs.SetItemText(i,1,sSize);

	if (bSelectIt)
	{
		lvis.iSubItem = 0;
		lvis.mask = LVIF_STATE;
		lvis.state = LVIS_SELECTED;
		lvis.stateMask = LVIS_SELECTED;
		m_Archivs.SetItem(&lvis);
	}
	
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::ClearArchivListBox()
{
	m_Archivs.DeleteAllItems();
	m_sName.Empty();
	m_iSelectedItem = -1;
	m_pSelectedArchivInfo = NULL;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchivPage)
	DDX_Control(pDX, IDC_EDIT_SIZE, m_editSize);
	DDX_Control(pDX, IDC_NAME_ARCHIV, m_editName);
	DDX_Control(pDX, IDC_ARCHIVE, m_Archivs);
	DDX_Text(pDX, IDC_NAME_ARCHIV, m_sName);
	DDV_MaxChars(pDX, m_sName, 250);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_dwSize);
	DDX_Text(pDX, IDC_STATIC_MAX_MB, m_sMax);
	DDX_Text(pDX, IDC_STATIC_USED_MB, m_sUsed);
	DDX_Text(pDX, IDC_STATIC_WHOLE_MB, m_sWhole);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CArchivPage, CSVPage)
	//{{AFX_MSG_MAP(CArchivPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ARCHIVE, OnItemchangedArchive)
	ON_EN_CHANGE(IDC_NAME_ARCHIV, OnChangeName)
	ON_EN_CHANGE(IDC_EDIT_SIZE, OnChangeEditSize)
	ON_NOTIFY(NM_CLICK, IDC_ARCHIVE, OnClickArchive)
	ON_NOTIFY(NM_DBLCLK, IDC_ARCHIVE, OnDblclkArchive)
	ON_NOTIFY(NM_RCLICK, IDC_ARCHIVE, OnRclickArchive)
	ON_NOTIFY(NM_RDBLCLK, IDC_ARCHIVE, OnRdblclkArchive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CArchivPage message handlers
BOOL CArchivPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	m_Images.Create(IDB_ARCHIVE,20,1,RGB(0,255,255));

	m_Archivs.SetImageList(&m_Images,LVSIL_SMALL);

	CRect			rect;
	LV_COLUMN		lvcolumn;
	int w[] = {75,25};	// CAVET add sum has to be 100
	int i,c;
	c = 2;
	CString s[2];

	m_Archivs.GetWindowRect(&rect);
	s[0].LoadString(IDS_NAME);
	s[1].LoadString(IDS_SIZE_MB);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0; i < c; i++)  // add the columns to the list control
	{
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_Archivs.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}
	
	FillArchivListBox();

	EnableExceptNew();

	SelectItem((m_Archivs.GetItemCount()>0) ? 0 : -1);

	LONG dw = m_Archivs.GetExtendedStyle();
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_Archivs.SetExtendedStyle(dw);

	// Check drives and warn user if necessary
	CIPCDrives* pDrives = m_pParent->GetDocument()->GetDrives();
	if (pDrives && (pDrives->GetAvailableMB() <= 0))
	{
		AfxMessageBox(IDP_CONFIG_DRIVE_FIRST, MB_OK|MB_ICONSTOP);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnNew()
{
	if ( IsDataValid() ) {
		CArchivInfo* pAI = m_pArchivs->AddArchivInfo();
		// copy actual setting from selected archiv
		CopyActualSettingsToNew(pAI);

		CString s;
		s.LoadString(IDS_NEW_ARCHIV);
		pAI->SetName(s);
		m_dwSize = pAI->GetSizeMB();
		InsertItem(pAI,TRUE);
		m_sName = s;
		UpdateData(FALSE);
		SetModified(TRUE,FALSE);
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);

		EnableExceptNew();

		m_editName.SetFocus();
		m_editName.SetSel(0,-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnDelete()
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedArchivInfo==NULL))
	{
		return;
	}

	BOOL bSaveHosts = FALSE;
	CHost* pHost = m_pHostArray->GetLocalHost();
	if (pHost)
	{
		if (pHost->GetAlarmArchive() == m_pSelectedArchivInfo->GetID())
		{
			pHost->SetAlarmArchive(SECID_NO_ID);
			bSaveHosts = TRUE;
		}
		else if (pHost->GetNormalArchive() == m_pSelectedArchivInfo->GetID())
		{
			pHost->SetNormalArchive(SECID_NO_ID);
			bSaveHosts = TRUE;
		}
	}

	for (int j=0;j<m_pHostArray->GetSize();j++)
	{
		pHost = m_pHostArray->GetAt(j);
		if (pHost)
		{
			if (pHost->GetAlarmArchive() == m_pSelectedArchivInfo->GetID())
			{
				pHost->SetAlarmArchive(SECID_NO_ID);
				bSaveHosts = TRUE;
			}
			else if (pHost->GetNormalArchive() == m_pSelectedArchivInfo->GetID())
			{
				pHost->SetNormalArchive(SECID_NO_ID);
				bSaveHosts = TRUE;
			}
			else if (pHost->GetSearchArchive() == m_pSelectedArchivInfo->GetID())
			{
				pHost->SetSearchArchive(SECID_NO_ID);
				bSaveHosts = TRUE;
			}
		}
	}
	if (bSaveHosts)
	{
		m_pHostArray->Save(GetProfile());
	}


	m_pArchivs->DeleteArchivInfo(m_pSelectedArchivInfo);
	m_pSelectedArchivInfo = NULL;
	// calc new selection
	int	newCount = m_Archivs.GetItemCount()-1;	// pre-subtract before DeleteItem
	int i = m_iSelectedItem;	// new selection

	// range check
	if (i > newCount-1 ) {	// outside ?
		i=newCount-1;
	} else if (newCount==0) {	// last one ?
		i = -1;
	}

	if (m_Archivs.DeleteItem(m_iSelectedItem))	// remove from listCtrl
	{
		SelectItem(i);
		SetModified(TRUE,FALSE);
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		EnableExceptNew();
		m_Archivs.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnItemchangedArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem!=m_iSelectedItem)
	{
		m_pSelectedArchivInfo = (CArchivInfo*)pNMListView->lParam;
		m_iSelectedItem = pNMListView->iItem;
		SelectedItemToControl();
		UpdateData(FALSE);
		EnableExceptNew();
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnClickArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnDblclkArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRclickArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRdblclkArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChangeName() 
{
	UpdateData();

	if (m_pSelectedArchivInfo)
	{
		m_pSelectedArchivInfo->SetName(m_sName);
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedArchivInfo->GetName();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = 0;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Archivs.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	else
	{
		//no host selected -> create and select a new one
		CArchivInfo* pAI = m_pArchivs->AddArchivInfo();

		pAI->SetName(m_sName);
		InsertItem(pAI,TRUE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChange()
{
	SetModified(TRUE,FALSE);
	m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChangeEditSize() 
{

	// Verhindert ein leeres Editfeld.
	CString sSize;
	m_editSize.GetWindowText(sSize.GetBuffer(255), 255);
	sSize.ReleaseBuffer(-1);
	if (sSize.IsEmpty())
	{
		m_editSize.SetWindowText(_T("1"));
		m_editSize.SetSel(0,-1);
	}

	UpdateData();
	m_dwSize = min(m_dwSize, 999999999); // Maximal 999 TeraByte
	m_pSelectedArchivInfo->SetSizeMB(m_dwSize);
	CString s;
	s.Format(_T("%d"),m_dwSize);
	m_Archivs.SetItemText(m_iSelectedItem,1,s);
	// Groessen aktualisieren
	UpdateSizeInfo();

	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
CString CArchivPage::GetTypString(ArchivType at)
{
	CString sTyp;
	switch(at)
	{
	case ALARM_ARCHIV:
		sTyp.LoadString(IDS_ALARM_ARCHIV);
		break;
	case RING_ARCHIV:
		sTyp.LoadString(IDS_RING_ARCHIV);
		break;
	case SICHERUNGS_RING_ARCHIV:
		sTyp.LoadString(IDS_VORRING_ARCHIV);
		break;
	case SUCHERGEBNIS_ARCHIV:
		sTyp.LoadString(IDS_RING_ARCHIV);
		break;
	}
	return sTyp;
}
/////////////////////////////////////////////////////////////////////////////
int	CArchivPage::GetTypImage(ArchivType at)
{
	int iImage=0;
	switch(at)
	{
	case ALARM_ARCHIV:
		iImage = 1;
		break;
	case RING_ARCHIV:
		iImage = 0;
		break;
	case SICHERUNGS_RING_ARCHIV:
		iImage = 2;
		break;
	case SUCHERGEBNIS_ARCHIV:
		iImage = 1;
		break;
	}
	return iImage;
}
