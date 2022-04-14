/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: HostPage.cpp $
// ARCHIVE:		$Archive: /DV/SystemVerwaltung/HostPage.cpp $
// CHECKIN:		$Date: 4.03.04 15:50 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 4.03.04 15:31 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "MainFrm.h"
#include "HostPage.h"

#include "SVDoc.h"
#include "SVView.h"
#include "CipcServerControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHostPage property page
IMPLEMENT_DYNAMIC(CHostPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CHostPage::CHostPage(CSVView* pParent) : CSVPage(CHostPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CHostPage)
	m_Name = _T("");
	m_Number = _T("");
	m_sStationsName = _T("");
	//}}AFX_DATA_INIT
	
	// data structures
	m_pHostArray = pParent->GetDocument()->GetHosts();
	m_pArchivInfoArray = pParent->GetDocument()->GetArchivs();
	m_pDrives = pParent->GetDocument()->GetDrives();

	m_iSelectedItem = -1;
	m_pSelectedHost = NULL;

	m_iISDNIndex = -2;
	m_iTCPIPIndex = -2;
	
	m_bISDN = GetDongle().RunISDNUnit();
	m_bTCPIP = GetDongle().RunSocketUnit();

	Create(IDD,(CWnd*)m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
CHostPage::~CHostPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::EnableExceptNew()
{
	BOOL bEnable;
	int c;
	c = m_Hosts.GetItemCount();
	bEnable = (c>0);
	m_editName.EnableWindow(bEnable);
	m_editNumber.EnableWindow(bEnable);
	m_Hosts.EnableWindow(bEnable);
	m_cbTyp.EnableWindow(bEnable);

	bEnable = (   bEnable			// generally enabled
			   && m_pSelectedHost	// host selected
			   && (   m_pSelectedHost->GetAlarmArchive() == SECID_NO_ID
				   || m_pSelectedHost->GetNormalArchive() == SECID_NO_ID
				   || m_pSelectedHost->GetSearchArchive() == SECID_NO_ID
				  )	// any archieve not assigned
			  );
	m_btnHostArchives.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::ClearHostListBox()
{
	m_Hosts.DeleteAllItems();
	m_Name.Empty();
	m_Number.Empty();
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
	DDX_Control(pDX, IDC_HOST_ARCHIVES, m_btnHostArchives);
	DDX_Control(pDX, IDC_ADDRESS_HOST, m_editNumber);
	DDX_Control(pDX, IDC_NAME_HOST, m_editName);
	DDX_Control(pDX, IDC_COMBO_TYPE_HOST, m_cbTyp);
	DDX_Control(pDX, IDC_LIST_HOST, m_Hosts);
	DDX_Text(pDX, IDC_NAME_HOST, m_Name);
	DDX_Text(pDX, IDC_ADDRESS_HOST, m_Number);
	DDX_Text(pDX, IDC_STATIONS_NAME, m_sStationsName);
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CRect			rect;
	LV_COLUMN		lvcolumn;
	int w[] = {10,30,30,30};	// CAVET add sum has to be 100
	CString s[4];
	int i;

	m_Hosts.GetWindowRect(&rect);
	s[0].LoadString(IDS_NR);
	s[1].LoadString(IDS_HOST);
	s[2].LoadString(IDS_NUMBER);
	s[3].LoadString(IDS_TYP);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0; i < 4; i++)  // add the columns to the list control
	{
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_Hosts.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}

	if (m_bISDN)
	{
		m_iISDNIndex = m_cbTyp.AddString(_T("ISDN"));
	}
	if (m_bTCPIP)
	{
		m_iTCPIPIndex = m_cbTyp.AddString(_T("TCP/IP"));
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

	// Check drives and warn user if necessary
	CIPCDrives* pDrives = m_pParent->GetDocument()->GetDrives();
	if (pDrives && (pDrives->GetAvailableMB() <= 0))
	{
		AfxMessageBox(IDP_CONFIG_DRIVE_FIRST, MB_OK|MB_ICONSTOP);
	}

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
		m_Name = m_pSelectedHost->GetName();
		sTyp = m_pSelectedHost->GetTyp();
		m_Number = m_pSelectedHost->GetClearNumber();

		int iSel = m_cbTyp.FindStringExact(0,m_pSelectedHost->GetTyp());
		m_cbTyp.SetCurSel(iSel);
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedHost = NULL;
		m_Name.Empty();
		m_Number.Empty();
		m_cbTyp.SetCurSel(-1);
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CanNew()
{
	return 	   m_bISDN 
		    || m_bTCPIP;
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
	InsertItem(pHost,TRUE);
	m_Name = pHost->GetName();
	m_Number = pHost->GetNumber();
	s = pHost->GetTyp();
	int iIndex = m_cbTyp.FindString(-1, s);
	m_cbTyp.SetCurSel(iIndex);
	UpdateData(FALSE);
	SetModified(TRUE,FALSE);

	EnableExceptNew();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::CheckDeleteArchivesForHost(CHost* pHost)
{
	BOOL bShowMB = TRUE;

	for (int i=0;i<m_pArchivInfoArray->GetSize();i++)
	{
		CArchivInfo* pArchivInfo = m_pArchivInfoArray->GetAt(i);

		if (pArchivInfo)
		{
			if (pArchivInfo->GetID()==pHost->GetSearchArchive())
			{
				if (!bShowMB)
				{
					m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo);
				}
				else
				{
					if (IDYES==AfxMessageBox(IDS_DELETE_HOST_ARCHIV,MB_YESNO))
					{
						m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo);
						bShowMB = FALSE;
					}
					else
					{
						return FALSE;
					}
				}
			}
			if (pArchivInfo->GetID()==pHost->GetNormalArchive())
			{
				if (!bShowMB)
				{
					m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo);
				}
				else
				{
					if (IDYES==AfxMessageBox(IDS_DELETE_HOST_ARCHIV,MB_YESNO))
					{
						m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo);
						bShowMB = FALSE;
					}
					else
					{
						return FALSE;
					}
				}
			}
			if (pArchivInfo->GetID()==pHost->GetAlarmArchive())
			{
				if (!bShowMB)
				{
					m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo);
				}
				else
				{
					if (IDYES==AfxMessageBox(IDS_DELETE_HOST_ARCHIV,MB_YESNO))
					{
						m_pArchivInfoArray->DeleteArchivInfo(pArchivInfo);
						bShowMB = FALSE;
					}
					else
					{
						return FALSE;
					}
				}
			}
		}
	}
	if (bShowMB==FALSE)
	{
		m_pArchivInfoArray->Save(GetProfile());
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnDelete() 
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedHost==NULL))
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
	if (i > newCount-1 ) {	// outside ?
		i=newCount-1;
	} else if (newCount==0) {	// last one ?
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
		m_pSelectedHost->SetName(m_Name);
		m_Hosts.SetItemText(m_iSelectedItem,1,m_Name);
	}
	else
	{
		//no host selected -> create and select a new one
		CHost* pHost = m_pHostArray->AddHost();

		pHost->SetName(m_Name);
		InsertItem(pHost,TRUE);
	}
	SetModified(TRUE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHostPage::OnChangeNumber() 
{
	UpdateData();

	CString s = m_Number;
	CString sTyp;
	int iCurSel = m_cbTyp.GetCurSel();

	if (-1==s.Find(_T('#')))
	{
		if (iCurSel==m_iTCPIPIndex)
		{
			if (m_Number.Find(_T("tcp:"))!=0)
			{
				s = _T("tcp:") + m_Number;
			}
			s = TrimLeadingZerosIP(s);
		}
		else
		{
			s = m_Number;
		}
	}


	if (m_pSelectedHost)
	{
		m_pSelectedHost->SetNumber(s);
		sTyp = m_pSelectedHost->GetTyp();

		m_Hosts.SetItemText(m_iSelectedItem,2,m_Number);
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
		CString s;
		UpdateData(TRUE);
		m_pSelectedHost = (CHost*)pNMListView->lParam;
		m_iSelectedItem = pNMListView->iItem;
		m_Name = m_pSelectedHost->GetName();
		m_Number = m_pSelectedHost->GetClearNumber();
		int iSel = m_cbTyp.FindStringExact(0,m_pSelectedHost->GetTyp());
		m_cbTyp.SetCurSel(iSel);
		UpdateData(FALSE);
		EnableExceptNew();
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
/////////////////////////////////////////////////////////////////////////////
void CHostPage::SaveChanges()
{
	if (theApp.m_pUser)
	{
		WK_TRACE_USER(_T("%s Gegenstationen-Einstellungen wurden geändert\n"),
			theApp.m_pUser->GetName());
	}
	UpdateData();
	m_pHostArray->SetLocalHostName(m_sStationsName);
	m_pHostArray->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostPage::IsDataValid()
{
	CString sName1,sName2;
	CString sNumber1,sNumber2;
	int i,j,c;

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

	// we have to check for valid own host name
	UpdateData();
	if (m_sStationsName.IsEmpty()
		|| m_sStationsName == CHostArray::GetLocalHostNameStandard())
	{
		AfxMessageBox(IDP_NO_LOCAL_HOSTNAME,MB_ICONSTOP);
		return FALSE;
	}

	// and now ask for creating archives
	// for hosts, which have none
/*	not neccessary, DBS will generate automatically
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
	if (d != m_pArchivInfoArray->GetSize())
	{
		m_pArchivInfoArray->Save(GetProfile());
		SetModified(TRUE,FALSE);
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
	}
*/

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
		aID = pHost->GetNormalArchive();
		if (NULL==ai.GetArchivInfo(aID))
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
		aID	= pHost->GetSearchArchive();
		if (   (!pHost->IsB3())
			&& (!pHost->IsB6())
			&& (pHost->GetID()!=SECID_LOCAL_HOST))
		{
			if (NULL==ai.GetArchivInfo(aID))
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
	CString sF,sM;

	sF.LoadString(nQuestion);
	CString sHostName = pHost->GetName();
	if (pHost->GetID() == SECID_LOCAL_HOST)
	{
		sHostName.LoadString(IDS_LOCAL_HOST);
	}
	sM.Format(sF,sHostName);

	if (r!=IDYESALL)
	{
		r = COemGuiApi::MessageBox(sM,0,MB_YESNO|MB_YESALL|MB_NOALL|MB_ICONQUESTION);
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
			CString sF,sM;
			WK_TRACE_ERROR(_T("Cannot automatically create %s archive for <%s>\n"),
				sExt,
				pHost->GetName());
			sF.LoadString(IDP_NO_AUTOMATIC_ARCHIV);
			sM.Format(sF,sExt,pHost->GetName());
			AfxMessageBox(sM,MB_OK|MB_ICONSTOP);
			r = IDNOALL; // stop asking for new archives
		}
	}
	return r;
}
////////////////////////////////////////////////////////////////////////////
void CHostPage::CancelChanges()
{
	ClearHostListBox();
	m_pHostArray->Load(GetProfile());
	FillHostListBox();
	int	c = m_Hosts.GetItemCount();
	SelectItem((c>0) ? 0 : -1);
	EnableExceptNew();
	if (c>=0)
		m_Hosts.SetFocus();
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
			m_pArchivInfoArray->Save(GetProfile());
			SetModified(TRUE,FALSE);
			m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		}
	}
}
