/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: userpage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/userpage.cpp $
// CHECKIN:		$Date: 24.01.05 14:04 $
// VERSION:		$Revision: 47 $
// LAST CHANGE:	$Modtime: 24.01.05 14:03 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"

#include "globals.h"
#include "systemverwaltung.h"
#include "OutputList.h"
#include "OutputGroup.h"
#include "userpage.h"

#include "oemgui\OEMPassworddialog.h"
#include "PasswordCheckDialog.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSecurityLevel[]	= _T("SecurityLevel");

/////////////////////////////////////////////////////////////////////////////
// CUserPage property page
IMPLEMENT_DYNAMIC(CUserPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CUserPage::CUserPage(CSVView* pParent) : CSVPage(CUserPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CUserPage)
	m_Name = _T("");
	m_bDualLogin = FALSE;
	//}}AFX_DATA_INIT
	m_pSelectedUser = NULL;
	m_iSelectedItem = -1;
	m_bDualLogin	= FALSE;
	m_pUserArray = pParent->GetDocument()->GetUsers();
	m_pPermissionArray = pParent->GetDocument()->GetPermissions();
	// m_sPassword;


	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
CUserPage::~CUserPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::EnableExceptNew()
{
	BOOL bEnable = (m_Users.GetItemCount() > 0);
	BOOL bExtraEnable = FALSE;
	BOOL bEnableAlarm = FALSE;
	if (m_pSelectedUser) 
	{
		CSecID idPermission = m_pSelectedUser->GetPermissionID();
		CPermission* pSelectedPermission = m_pPermissionArray->GetPermission(idPermission);
		if (pSelectedPermission)
		{
			bExtraEnable = (   bEnable
							&& (theApp.m_pPermission->GetPriority() <= pSelectedPermission->GetPriority())
							);
			bEnableAlarm = (   bExtraEnable
							&& (m_pSelectedUser->IsUserAlarm() == FALSE)
							);
		}
		else
		{
			bExtraEnable = TRUE;
			bEnableAlarm = TRUE;
		}
	}

	m_Users.EnableWindow(bEnable);
	m_Permissions.EnableWindow(bExtraEnable);

	m_editName.EnableWindow(bEnableAlarm);
	m_btnPassword.EnableWindow(bEnableAlarm);

	m_btnDualLogin.EnableWindow(bEnableAlarm);
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserPage)
	DDX_Control(pDX, IDC_CHECK_DOPPEL_LOGIN, m_btnDualLogin);
	DDX_Control(pDX, IDC_PASSWORD, m_btnPassword);
	DDX_Control(pDX, IDC_NAME_USER, m_editName);
	DDX_Control(pDX, IDC_COMBO_PERMISSION, m_Permissions);
	DDX_Control(pDX, IDC_LIST_USER, m_Users);
	DDX_Text(pDX, IDC_NAME_USER, m_Name);
	DDX_Check(pDX, IDC_CHECK_DOPPEL_LOGIN, m_bDualLogin);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CUserPage, CSVPage)
	//{{AFX_MSG_MAP(CUserPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USER, OnItemchangedListUser)
	ON_EN_CHANGE(IDC_NAME_USER, OnChangeName)
	ON_BN_CLICKED(IDC_PASSWORD, OnPassword)
	ON_CBN_SELCHANGE(IDC_COMBO_PERMISSION, OnSelchangeComboPermission)
	ON_BN_CLICKED(IDC_BUTTON_ALARM, OnButtonAlarm)
	ON_BN_CLICKED(IDC_CHECK_DOPPEL_LOGIN, OnCheckDoppelLogin)
	ON_NOTIFY(NM_CLICK, IDC_LIST_USER, OnClickListUser)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_USER, OnDblclkListUser)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_USER, OnRclickListUser)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_USER, OnRdblclkListUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CUserPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[2];
	int i;

	m_Users.GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_NAME)->GetWindowText(s[0]);
	GetDlgItem(IDC_STATIC_PERMISSION)->GetWindowText(s[1]);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = rect.Width() / 2 - 3;

	for (i = 0; i < 2; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s[i]);
		lvcolumn.iSubItem = i;
		m_Users.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}

	Initialize();

	LONG dw = ListView_GetExtendedListViewStyle(m_Users.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_Users.m_hWnd,dw);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::Initialize() 
{
	m_pUserArray->Load(GetProfile());

	FillPermissions();
	FillUsers();

	EnableExceptNew();
	int	c = m_Users.GetItemCount();
	SelectUser((c>0) ? 0 : -1);
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::ClearUsers()
{
	m_Users.DeleteAllItems();
	ClearActualUser();
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::ClearActualUser()
{
	m_Users.DeleteAllItems();
	m_Name.Empty();
	m_sPassword.Empty();
	m_bDualLogin = FALSE;
	m_iSelectedItem = -1;
	m_pSelectedUser = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::FillUsers() 
{
	ClearUsers();
	int i,c;
	CUser* pUser;

	c = m_pUserArray->GetSize();
	
	if (c>0)
	{
		pUser = (CUser*)m_pUserArray->GetAt(0);
		InsertUser(pUser,TRUE);
	}
	for (i=1; i<c; i++)
	{
		pUser = (CUser*)m_pUserArray->GetAt(i);
		InsertUser(pUser);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::SelectUser(int i, BOOL bSetSel)
{
	if (IsBetween(i, 0, m_Users.GetItemCount()-1))
	{
		m_iSelectedItem = i;
		if (bSetSel)
		{
			m_Users.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		}
		m_pSelectedUser = (CUser*)m_Users.GetItemData(m_iSelectedItem);
		if (m_pSelectedUser)
		{
			m_Name = m_pSelectedUser->GetName();
			m_sPassword = m_pSelectedUser->GetPassword();

			DWORD dw;
			int i,c;
			dw = m_pSelectedUser->GetPermissionID().GetID();
			m_Permissions.SetCurSel(-1);
			c = m_Permissions.GetCount();
			for (i=0;i<c;i++)
			{
				if (m_Permissions.GetItemData(i)==dw)
				{
					m_Permissions.SetCurSel(i);
					break;
				}
			}
			m_bDualLogin = m_pSelectedUser->GetDualLogin();
		}
		else
		{
			ClearActualUser();
		}
	}
	else
	{
		ClearActualUser();
	}

	UpdateData(FALSE);
	EnableExceptNew();
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::FillPermissions()
{
	m_Permissions.ResetContent();

	int i,c;
	CPermission* pUserPermission = theApp.m_pPermission;
	CPermission* pPermission;

	c = m_pPermissionArray->GetSize();
	for (i=0; i<c; i++)
	{
		pPermission = (CPermission*)m_pPermissionArray->GetAt(i);
		if (pUserPermission->GetPriority() <= pPermission->GetPriority() ) 
		{
			InsertPermission(pPermission);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CUserPage::InsertUser(CUser* pUser, BOOL bSelectIt)
{
	if (pUser && pUser->IsIdipServiceUser())
	{	// do not insert special user 'idipservice'
		return (int)(-1);
	}

	LV_ITEM lvis;
	CSecID idP;
	CPermission* pP;
	CString s;
	int i,c;

	s = pUser->GetName();

	c = m_Users.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)pUser;
	lvis.iItem = c;
	lvis.iSubItem = 0;

	i = m_Users.InsertItem(&lvis);
	s.ReleaseBuffer();

	idP = pUser->GetPermissionID();
	pP = m_pPermissionArray->GetPermission(idP);

	if (pP)
		s = pP->GetName();
	else
		s.Empty();

	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = i;
	lvis.iSubItem = 1;
	m_Users.SetItem(&lvis);
	s.ReleaseBuffer();

	if (bSelectIt)
	{
		SelectUser(i, TRUE);
	}

	return i;
}
/////////////////////////////////////////////////////////////////////////////
int CUserPage::InsertPermission(CPermission* pPermission)
{
	CString s = pPermission->GetName();
	DWORD dw = pPermission->GetID().GetID();
	int i;

	i = m_Permissions.AddString(s);
	m_Permissions.SetItemData(i,dw);
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnDelete() 
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedUser==NULL))
	{
		return;
	}

	m_pUserArray->DeleteUser(m_pSelectedUser);
	m_pSelectedUser = NULL;
	// calc new selection
	int	newCount = m_Users.GetItemCount()-1;	// pre-subtract before DeleteItem
	int i = m_iSelectedItem;	// new selection

	// range check
	if (i > newCount-1 ) {	// outside ?
		i=newCount-1;
	} else if (newCount==0) {	// last one ?
		i = -1;
	}

	if (m_Users.DeleteItem(m_iSelectedItem))	// remove from listCtrl
	{							    
		SelectUser(i);
		SetModified();
		EnableExceptNew();
		m_Permissions.SetFocus();
	}					     
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnNew()       
{			      	    	      
	CString s;	   		 
	CUser* pUser = m_pUserArray->AddUser();
									  
	s.LoadString(IDS_NEWUSER);						 
	pUser->SetName(s);

	InsertUser(pUser,TRUE);
	m_editName.SetFocus();
	m_editName.SetSel(0,-1);

	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnItemchangedListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectUser(pNMListView->iItem, FALSE);
		}
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnClickListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Users.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Users.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnRclickListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Users.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnRdblclkListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Users.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnChangeName() 
{
	UpdateData();

	if (m_pSelectedUser)
	{
		m_pSelectedUser->SetName(m_Name);

		if (m_Name.Remove(_T('\\')) == 0)
		{
			LV_ITEM lvis;
			CString s;

			s = m_pSelectedUser->GetName();
			lvis.mask = LVIF_TEXT;
			lvis.iItem = m_iSelectedItem;
			lvis.iSubItem = 0;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();

			m_Users.SetItem(&lvis);
			s.ReleaseBuffer();
		}
		else			  
		{
			int nCurSel = m_Name.GetLength();
			m_Name = m_Name.Left(nCurSel);
			UpdateData(FALSE);
			m_editName.SetSel(nCurSel,nCurSel);

			MessageBeep(0);
		}
	}
	else
	{
		//no User selected -> create and select a new one
		CUser* pUser = m_pUserArray->AddUser();

		pUser->SetName(m_Name);
		InsertUser(pUser,TRUE);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnPassword() 
{
	if (m_pSelectedUser) {
		if (m_pSelectedUser->GetPermissionID()==SECID_NO_ID)
		{
			CString sL,sM;
			sL.LoadString(IDP_USER_WITHOUT_PERMISSION);
			sM.Format(sL,m_pSelectedUser->GetName());
			AfxMessageBox(sM,MB_ICONSTOP);
			return;
		}
		if (!m_sPassword.IsEmpty())
		{
			CPasswordCheckDialog dlg(this);
			if (IDOK==dlg.DoModal())
			{
				if (dlg.m_Password != m_sPassword)
				{	
					AfxMessageBox(IDP_PASSWORD_INCORRECT);
					return;
				}		    
			}							 
			else						 
			{
				return;
			}
		}

		BOOL bIsSupervisor = (m_pSelectedUser->GetPermissionID()==SECID_SUPERVISOR_PERMISSION);
		COEMPasswordDialog dlg(this, m_Name, TRUE, !bIsSupervisor);

		if (IDOK==dlg.DoModal())
		{
			m_sPassword = dlg.GetPassword1();
			m_pSelectedUser->SetPassword(m_sPassword, dlg.ForceToChangePassword());
			SetModified();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnSelchangeComboPermission() 
{
	if (m_pSelectedUser)
	{
		int i = m_Permissions.GetCurSel();
		if (i!=CB_ERR)
		{
			DWORD dw = m_Permissions.GetItemData(i);
			CString s;

			m_Permissions.GetLBText(i,s);
			
			m_pSelectedUser->SetPermissionID(dw);
			LV_ITEM lvis;

			lvis.mask = LVIF_TEXT;
			lvis.iItem = m_iSelectedItem;
			lvis.iSubItem = 1;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();
			m_Users.SetItem(&lvis);
			s.ReleaseBuffer();
			SetModified();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::CancelChanges()
{
	if (IsModified()) 
	{
		m_pUserArray->Load(GetProfile());
		FillUsers();
		int	c = m_Users.GetItemCount();
		SelectUser((c>0) ? 0 : -1);
		EnableExceptNew();
		if (c>=0)
			m_Users.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUserPage::IsDataValid()
{
	// get minimum password length
	DWORD dwMinPasswordLength = GetProfile().GetInt(szSecurityLevel,_T("MinPasswordLength"),0);
	// get last security modified time
	CSystemTime stSecurityModifiedTime;
	stSecurityModifiedTime.GetLocalTime();
	CString sSecurityModifiedTime = GetProfile().GetString(szSecurityLevel,
												   _T("ModifiedTime"),
												   stSecurityModifiedTime.ToString());
	stSecurityModifiedTime.FromString(sSecurityModifiedTime);

	// check data
	CString sName1,sName2, sTemp;
	CUser* pUser = NULL;
	CUser* pUserWithDualLogin = NULL;
	int iCountSuperVisors = 0;
	int iCountUsersWithoutAlarm = 0;
	int j;
	int c = m_pUserArray->GetSize();
	for (int i=0;i<c;i++)
	{
		pUser = m_pUserArray->GetAt(i);
		sName1 = pUser->GetName();

		// check for empty user name
		sTemp = sName1;
		sTemp.TrimLeft();
		sTemp.TrimRight();
		if (sTemp.IsEmpty()) {
			if (IDNO == AfxMessageBox(IDP_USER_NO_NAME, MB_YESNO|MB_ICONQUESTION)) {
				return FALSE;
			}
		}

		// check for correct user Alarm
		if (   (sName1 == szUserAlarm)
			&& (pUser->IsUserAlarm() == FALSE)
			)
		{
			AfxMessageBox(IDP_USER_ALRM_INVALID, MB_ICONSTOP);
			return FALSE;
		}

		// check for correct idip service user
		if (   (sName1 == szIdipServiceUser)
			&& (pUser->IsIdipServiceUser() == FALSE)
			)
		{
			AfxMessageBox(IDP_USER_IDIP_SERVICE_RESERVED, MB_ICONSTOP);
			return FALSE;
		}

		// check for double users
		for (j=i+1;j<c;j++)
		{
			sName2 = m_pUserArray->GetAt(j)->GetName();
			if (sName1 == sName2)
			{
				CString sMsg;
				sMsg.Format(IDP_DOUBLE_USER, sName1);
				AfxMessageBox(sMsg,MB_ICONSTOP);
				return FALSE;
			}
		}

		// check for no permission
		if (pUser->GetPermissionID()==SECID_NO_ID)
		{
			CString sMsg;
			sMsg.Format(IDP_USER_WITHOUT_PERMISSION, pUser->GetName());
			AfxMessageBox(sMsg,MB_ICONSTOP);
			return FALSE;
		}

		// count users (without Alarm and idipservice) and "real" Supervisor permissions,one must be left at all
		if (   sName1 != szUserAlarm
			&& pUser->IsIdipServiceUser() == FALSE)
		{
			iCountUsersWithoutAlarm++;
			if (pUser->GetPermissionID() == SECID_SUPERVISOR_PERMISSION)
			{
				iCountSuperVisors++;
			}
		}		
		if (   (pUserWithDualLogin == NULL)
			&& pUser->GetDualLogin())
		{
			pUserWithDualLogin = pUser;
		}		
		// Ein Paßwort ist zwingend, aber nur, wenn auch eine Mindestlaenge vorgeschrieben!
		if (   (dwMinPasswordLength > 0) 
			&& (pUser->GetPassword().IsEmpty()))
		{
			// Der Nutzer muß neuer sein, als die
			// Sicherheitsvorschriften, damit wir meckern können!
			// Ist er älter, so ist alles i.O. ?!
			if (stSecurityModifiedTime < pUser->GetCreationTime())
			{
				CString sM;
				sM.Format(IDP_USER_WITHOUT_PASSWORD,pUser->GetName());
				AfxMessageBox(sM,MB_ICONSTOP);
				return FALSE;
			}
		}

	}
	if (iCountSuperVisors == 0)
	{
		AfxMessageBox(IDP_NOMORE_SUPERVISOR);
		return FALSE;
	}
	if (   (pUserWithDualLogin != NULL)
		&& (iCountUsersWithoutAlarm < 2)
		)
	{
		CString sMsg;
		sMsg.Format(IDP_NOT_ENOUGH_USERS_FOR_DUAL_LOGIN, pUserWithDualLogin->GetName());
		AfxMessageBox(sMsg, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::SaveChanges()
{
	WK_TRACE_USER(_T("Nutzer-Einstellungen wurden geändert\n"));
#ifdef _UNICODE
	if (m_pParent->GetDocument()->IsLocal())
	{
		m_pUserArray->m_bUseBinaryFormat = TRUE;
	}
	else
	{
		m_pUserArray->m_bUseBinaryFormat = m_pParent->GetDocument()->UseBinaryFormat();
	}
#endif
	m_pUserArray->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUserPage::CanNew()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUserPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUserPage::CanDelete()
{
	BOOL bReturn = FALSE;
	if (m_Users.GetItemCount()==0)
	{
		bReturn = FALSE;
	}
	if (m_pSelectedUser==NULL)
	{
		bReturn = FALSE;
	}
	else 
	{
		CSecID idPermission = m_pSelectedUser->GetPermissionID();
		CPermission* pSelectedPermission = m_pPermissionArray->GetPermission(idPermission);

		if (pSelectedPermission && theApp.m_pPermission)
		{
			bReturn = ( theApp.m_pPermission->GetPriority() <= pSelectedPermission->GetPriority());
		}
		else
		{
			bReturn = FALSE;
		}

	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnButtonAlarm() 
{
	int i,c;
	CUser* pUser = NULL;

	c = m_pUserArray->GetSize();
	for (i=0;i<c;i++)
	{
		pUser = m_pUserArray->GetAt(i);
		if (pUser && pUser->GetName()==szUserAlarm)
		{
			return;
		}
	}

	pUser = m_pUserArray->AddUser();
	CString s;

	CString sUserAlarm(szUserAlarm);
	pUser->SetName(sUserAlarm);
	pUser->SetPermissionID(SECID_SUPERVISOR_PERMISSION);
	s.Format(_T("%08lx"),GetTickCount());
	pUser->SetPassword(s);
	InsertUser(pUser,TRUE);
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CUserPage::OnCheckDoppelLogin() 
{
	if (m_pSelectedUser)
	{
		if (m_pSelectedUser->GetDualLogin())
			m_pSelectedUser->SetDualLogin(FALSE);
		else
			m_pSelectedUser->SetDualLogin(TRUE);
	}
	SetModified();
}
