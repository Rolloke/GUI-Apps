// BackupProcessPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "BackupProcessPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackupProcessPage dialog

CBackupProcessPage::CBackupProcessPage(CSVView* pParent) : CSVPage(CBackupProcessPage::IDD)
{
	m_pParent  = pParent;
	//{{AFX_DATA_INIT(CBackupProcessPage)
	m_sName = _T("");
	m_iArchive = -1;
	m_iDrive = -1;
	m_bDelete = FALSE;
	//}}AFX_DATA_INIT
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pArchivList = pParent->GetDocument()->GetArchivs();
	m_pDrives = pParent->GetDocument()->GetDrives();
	m_pSelectedProcess = NULL;
	m_iSelectedItem = -1;

	Create(IDD,(CWnd*)m_pParent);
}


void CBackupProcessPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackupProcessPage)
	DDX_Control(pDX, IDC_EDIT_BACKUP_NAME, m_editName);
	DDX_Control(pDX, IDC_COMBO_ARCHIV, m_comboArchive);
	DDX_Control(pDX, IDC_COMBO_DRIVE, m_comboDrive);
	DDX_Control(pDX, IDC_COMBO_BU_TIME_SPAN, m_comboTimeSpan);
	DDX_Control(pDX, IDC_LIST_BACKUP_PROCESS, m_listProcesses);
	DDX_Text(pDX, IDC_EDIT_BACKUP_NAME, m_sName);
	DDX_CBIndex(pDX, IDC_COMBO_ARCHIV, m_iArchive);
	DDX_CBIndex(pDX, IDC_COMBO_DRIVE, m_iDrive);
	DDX_Check(pDX, IDC_CHECK_DELETE, m_bDelete);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate)
	{
		GetDlgItem(IDC_CHECK_DELETE)->EnableWindow(m_comboTimeSpan.GetCurSel() == 0);
	}

}


BEGIN_MESSAGE_MAP(CBackupProcessPage, CSVPage)
	//{{AFX_MSG_MAP(CBackupProcessPage)
	ON_BN_CLICKED(IDC_CHECK_DELETE, OnCheckDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_ARCHIV, OnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_DRIVE, OnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_BU_TIME_SPAN, OnSelchangeCombo)
	ON_EN_CHANGE(IDC_EDIT_BACKUP_NAME, OnChangeEditName)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BACKUP_PROCESS, OnItemchangedListBackupProcess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupProcessPage::CanNew()
{
	return (m_pArchivList->GetSize()>0) &&
		   (m_pInputList->GetNrOfActiveInputs()) &&
		   (m_comboDrive.GetCount()>0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupProcessPage::CanDelete()
{
	return m_listProcesses.GetItemCount()>0;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::SaveChanges()
{
	WK_TRACE_USER(_T("Backup-Prozess-Einstellungen wurden geändert\n"));
	ControlToProcess();
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());

	InsertProcesses();
	
	EnableExceptNew();

	int	c = m_listProcesses.GetItemCount();
	SelectItem((c>0) ? 0 : -1);
	
	SetModified(FALSE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupProcessPage::IsDataValid()
{
	BOOL bReturn = TRUE;
	const CProcess* pProcess = NULL;
	CSecID idArchive;

	// check the activations one by one
	int iCount = m_listProcesses.GetItemCount();
	for (int i=0 ; bReturn && i<iCount ; i++) {
		// first: is there really an process behind
		pProcess = (CProcess*)m_listProcesses.GetItemData(i);
		if (pProcess==NULL) {
			// no one behind, next one please
			continue;
		}
		// second: check for archive
		else if (pProcess->GetBackupArchiv().IsArchiveID() == FALSE) {
			AfxMessageBox(IDP_BACKUP_PROCESS_NO_ARCHIVE);
			SelectItem(i);
			m_listProcesses.SetFocus();
			bReturn = FALSE;
		}
		// third: check for drive
		else if (pProcess->GetDestination().IsEmpty()) {
			AfxMessageBox(IDP_BACKUP_PROCESS_NO_DRIVE);
			SelectItem(i);
			m_listProcesses.SetFocus();
			bReturn = FALSE;
		}
	} // for
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::ControlToProcess()
{
	UpdateData();
	if (m_pSelectedProcess)
	{
		CString sDelete,sArchiv, sTimeSpan;
		m_pSelectedProcess->SetName(m_sName);
		DWORD dwArchiveID = m_comboArchive.GetItemData(m_iArchive);
		CString sDrive;
		if (m_iDrive != CB_ERR) 
		{
			m_comboDrive.GetLBText(m_iDrive,sDrive);
		}
		DWORD dwBackupTimeSpan = m_comboTimeSpan.GetCurSel();
		if (dwBackupTimeSpan == CB_ERR)
		{
			dwBackupTimeSpan = 0;
		}
		if (dwBackupTimeSpan != 0 && m_bDelete)
		{
			m_bDelete = FALSE;
			UpdateData(FALSE);
		}

		m_pSelectedProcess->SetBackup(m_sName, sDrive,
									  CSecID(dwArchiveID),
									  m_bDelete, dwBackupTimeSpan);

		m_listProcesses.SetItemText(m_iSelectedItem,0,m_sName);
		CArchivInfo* pArchiv = m_pArchivList->GetArchivInfo(m_pSelectedProcess->GetBackupArchiv());
		if (pArchiv)
		{
			sArchiv = pArchiv->GetName();
		}
		m_listProcesses.SetItemText(m_iSelectedItem,1,sArchiv);
		m_listProcesses.SetItemText(m_iSelectedItem,2,m_pSelectedProcess->GetDestination());

		sDelete.LoadString(m_pSelectedProcess->GetDeleteAtSuccess() ? IDS_YES : IDS_NO);
		m_listProcesses.SetItemText(m_iSelectedItem, 3, sDelete);

		m_comboTimeSpan.GetLBText(m_pSelectedProcess->GetBackupTimeSpan(), sTimeSpan);
		m_listProcesses.SetItemText(m_iSelectedItem, 4, sTimeSpan);
		GetDlgItem(IDC_CHECK_DELETE)->EnableWindow(dwBackupTimeSpan == 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::OnNew()
{
	if (m_pSelectedProcess)
	{
		ControlToProcess();
	}
	m_pSelectedProcess = m_pProcessList->AddProcess();

	CString s;
	s.LoadString(IDS_NEW);
	m_pSelectedProcess->SetName(s);
	int i = InsertProcess(m_pSelectedProcess);
	SelectItem(i);
	SetModified(TRUE,FALSE);

	EnableExceptNew();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::OnDelete()
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedProcess==NULL))
	{
		return;
	}

	if (m_pParent->GetDocument()->CheckActivationsWithProcess(m_pSelectedProcess->GetID()))
	{
		m_pProcessList->DeleteProcess(m_pSelectedProcess);
		m_pSelectedProcess = NULL;
		// calc new selection
		int	newCount = m_listProcesses.GetItemCount()-1;	// pre-subtract before DeleteItem
		int i = m_iSelectedItem;	// new selection

		// range check
		if (i > newCount-1 ) {	// outside or last one?
			i=newCount-1;
		}
		if (m_listProcesses.DeleteItem(m_iSelectedItem))	// remove from listCtrl
		{
			SelectItem(i);
			SetModified();
			EnableExceptNew();
			m_listProcesses.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupProcessPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CBackupProcessPage message handlers
BOOL CBackupProcessPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CString str;
	str.LoadString(IDS_ALL);
	m_comboTimeSpan.AddString(str);
	str.LoadString(IDS_HOURS);
	str = _T("24 ") + str;
	m_comboTimeSpan.AddString(str);

	InitProcessList();

	InsertProcesses();
	InsertArchives();
	InsertDrives();
	
	EnableExceptNew();

	int	c = m_listProcesses.GetItemCount();
	SelectItem((c>0) ? 0 : -1);

	LONG dw = ListView_GetExtendedListViewStyle(m_listProcesses.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listProcesses.m_hWnd,dw);


	SetModified(FALSE,FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::InitProcessList()
{
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[5];
	int i;

	m_listProcesses.GetWindowRect(&rect);
	GetDlgItem(IDC_NAME_BACKUP)->GetWindowText(s[0]);
	s[1].LoadString(IDS_ARCHIV);
	s[2].LoadString(IDS_DRIVE);
	GetDlgItem(IDC_CHECK_DELETE)->GetWindowText(s[3]);
	GetDlgItem(IDC_STATIC_BU_TIME_SPAN)->GetWindowText(s[4]);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = (rect.Width() -3)/ 5;

	for (i = 0; i < 5; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s[i]);
		lvcolumn.iSubItem = i;
		m_listProcesses.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::InsertProcesses()
{
	CProcess* pProcess;

	m_listProcesses.DeleteAllItems();
	for (int i=0;i<m_pProcessList->GetSize();i++) 
	{
		pProcess = m_pProcessList->GetAt(i);
		if (pProcess && pProcess->IsBackup()) 
		{
			InsertProcess(pProcess);
		}
	}

	if (m_listProcesses.GetItemCount()>0)
	{
		SelectItem(0);
	}
	else
	{
		SelectItem(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CBackupProcessPage::InsertProcess(CProcess* pProcess)
{
	LV_ITEM lvis;
	CString sName,sArchiv,sDelete,sTimeSpan;
	int i,c;

	sName = pProcess->GetName();

	c = m_listProcesses.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = sName.GetBuffer(0);
	lvis.cchTextMax = sName.GetLength();
	lvis.lParam = (LPARAM)pProcess;
	lvis.iItem = c;
	lvis.iSubItem = 0;

	i = m_listProcesses.InsertItem(&lvis);
	sName.ReleaseBuffer();

	CArchivInfo* pArchiv = m_pArchivList->GetArchivInfo(pProcess->GetBackupArchiv());
	if (pArchiv)
	{
		sArchiv = pArchiv->GetName();
	}
	m_listProcesses.SetItemText(i,1,sArchiv);
	m_listProcesses.SetItemText(i,2,pProcess->GetDestination());
	if (pProcess->GetDeleteAtSuccess())
	{
		sDelete.LoadString(IDS_YES);
	}
	else
	{
		sDelete.LoadString(IDS_NO);
	}
	m_listProcesses.SetItemText(i,3,sDelete);

	m_comboTimeSpan.GetLBText(pProcess->GetBackupTimeSpan(), sTimeSpan);
	m_listProcesses.SetItemText(i, 4, sTimeSpan);
	
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::InsertDrives()
{
	m_comboDrive.ResetContent();
	int iCount = m_pDrives->GetSize();
	for (int j=0 ; j<iCount ; j++) 
	{
		CIPCDrive* pDrive = m_pDrives->GetAt(j);
		// exclude Suchergebn.
		if (   pDrive 
			&& pDrive->IsWriteBackup()
			&& (!pDrive->IsCDROM())) 
		{	
			m_comboDrive.AddString(pDrive->GetRootString().Left(2));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::InsertArchives()
{
	m_comboArchive.ResetContent();
	int iCount = m_pArchivList->GetSize();
	for (int j=0 ; j<iCount ; j++) 
	{
		CArchivInfo *pArchive = m_pArchivList->GetAt(j);
		// exclude Suchergebnis
		// Warum soll das Suchergebnisarchive nicht auch gesichert werden können?
		// ML 04.10.99	if (pArchive->GetID().GetSubID()!=LOCAL_SEARCH_RESULT_ARCHIV_NR) 
		{	
			// Archive von Hosts nicht zeigen
			// WARUM NICHT? gf 990928 siehe email von SB am 990927

			// ML Ich finde es spricht nichts dagegen die Hostarchive bei den Backupprozessen
			// ebenfalls zuzulassen 04.10.99
			// CHostArray* m_pHostArray = m_pParent->GetDocument()->GetHosts();
			// if (NULL==m_pHostArray->GetHostForArchive(pArchive->GetID()))
			{
				int ix = m_comboArchive.AddString(pArchive->GetName());
				m_comboArchive.SetItemData(ix,pArchive->GetID().GetID());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::EnableExceptNew()
{
	EnableAll(m_listProcesses.GetItemCount()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::SelectItem(int i)
{
	if (i>=0)
	{
		CString sTyp;
		m_iSelectedItem = i;
		m_pSelectedProcess = (CProcess*)m_listProcesses.GetItemData(m_iSelectedItem);
		m_sName = m_pSelectedProcess->GetName();
		m_bDelete = m_pSelectedProcess->GetDeleteAtSuccess();
		DWORD dwBackupTimeSpan = m_pSelectedProcess->GetBackupTimeSpan();

		CArchivInfo* pArchiv = m_pArchivList->GetArchivInfo(m_pSelectedProcess->GetBackupArchiv());
		if (pArchiv)
		{
			CString sArchiv;
			sArchiv = pArchiv->GetName();
			m_iArchive = m_comboArchive.SelectString(0,sArchiv);
		}
		m_iDrive = m_comboDrive.SelectString(0,m_pSelectedProcess->GetDestination());
		if (m_comboTimeSpan.SetCurSel(dwBackupTimeSpan) == CB_ERR)
		{
			m_comboTimeSpan.SetCurSel(0);
		}
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedProcess = NULL;
		m_sName = _T("");
		m_iArchive = -1;
		m_iDrive = -1;
		m_bDelete = FALSE;
	}
	UpdateData(FALSE);
	m_listProcesses.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::OnCheckDelete() 
{
	ControlToProcess();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::OnSelchangeCombo() 
{
	ControlToProcess();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::OnChangeEditName() 
{
	ControlToProcess();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CBackupProcessPage::OnItemchangedListBackupProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectItem(pNMListView->iItem);
		}
	}
	*pResult = 0;
}
