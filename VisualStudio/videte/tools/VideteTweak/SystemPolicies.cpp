// SystemPolicies.cpp : implementation file
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "SystemPolicies.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemPolicies property page
#define SYSTEM_SAVED "SavedSystem"
#define SYSTEM_IMAGE "ImageSystem"

IMPLEMENT_DYNCREATE(CSystemPolicies, CPropertyPage)

CSystemPolicies::CSystemPolicies() : CPropertyPage(CSystemPolicies::IDD)
{
	//{{AFX_DATA_INIT(CSystemPolicies)
	m_bRemoveMyDocumentsIcon = FALSE;
	m_bDisableCmdPrompt = FALSE;
	m_bRemoveMyComputerIcon = FALSE;
	//}}AFX_DATA_INIT
	m_bChanged = false;
}

CSystemPolicies::~CSystemPolicies()
{
}

void CSystemPolicies::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemPolicies)
	DDX_Control(pDX, IDC_LST_POLICY_EXPLORER, m_List);
	DDX_Check(pDX, IDC_CK_REMOVE_MY_DOCUMENT_ICON, m_bRemoveMyDocumentsIcon);
	DDX_Check(pDX, IDC_CK_DISABLE_COMMAND_PROMPT, m_bDisableCmdPrompt);
	DDX_Check(pDX, IDC_CK_REMOVE_MY_COMPUTER_ICON, m_bRemoveMyComputerIcon);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemPolicies, CPropertyPage)
	//{{AFX_MSG_MAP(CSystemPolicies)
	ON_NOTIFY(NM_CLICK, IDC_LST_POLICY_EXPLORER, OnChange)
	ON_NOTIFY(NM_RCLICK, IDC_LST_POLICY_EXPLORER, OnRclickLstPolicyExplorer)
	ON_BN_CLICKED(IDC_CK_REMOVE_MY_DOCUMENT_ICON, OnChanged)
	ON_BN_CLICKED(IDC_CK_DISABLE_COMMAND_PROMPT, OnChanged)
	ON_BN_CLICKED(IDC_CK_REMOVE_MY_COMPUTER_ICON, OnChanged)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static const RegKeyInfo g_pSystem[] =
{
	{"NoDispAppearancePage",
	 "No Appearance Page in Control Panel",
	 "Removes the ability to change the colors or color scheme on the desktop from Control Panel.", REG_DWORD,  0, 1},
	{"NoDispBackgroundPage",
	 "No Background Page in Control Panel",
	 "Removes the ability to change wallpaper and backround pattern from Control Panel.", REG_DWORD,  0, 1},
	{"NoDispCPL", "No Display Options in Control Panel", "Disables the Display option in Control Panel", REG_DWORD,  0, 1},
	{"NoDispScrSavPage", "No Screensaver Tab in Control Panel",
	 "The Screen Saver tab does not appear in the Display Properties page of Control Panel.", REG_DWORD,  0, 1},
	{"NoDispSettingsPage", "No Display Settings Page in Control Panel", "The Settings and Plus tab do not appear in the Display Properties page of Control Panel", REG_DWORD,  0, 1},
	{"DisableTaskMgr",
	 "Prevent TaskMgr.exe from running",
	 "This entry is only supported from NT 4.0 with SP2 or greater.", REG_DWORD,  0, 1},
	{"DisableLockWorkstation", "Disable Lock Workstation", NULL, REG_DWORD,  0, 1},
	{"DisableChangePassword", "Disable Change Password", NULL, REG_DWORD,  0, 1},
	{"DisableRegistryTools", "Disable Registry Tools", "Disable user's ability to run Regedit.exe or Regedt32.exe.", REG_DWORD,  0, 1},
	{"EnableProfileQuota", "Enable Profile Size Tracking", "Determines whether to track profile space (i.e., whether to run proquota.exe). A value of 1 means track profile space; a value of 0 means don't track it.", REG_DWORD,  0, 1},
	{"IncludeRegInProQuota", "Include Registry in Size Tracking", "Tells Proquota whether to include the size of the Registry file in its profile size calculations. A value of 1 means include the Registry; a value of 0 means omit it.", REG_DWORD,  0, 1},
	{"WarnUser", "Warn User for too much Data in Registry Profile", "Tells Proquota whether to nag the user when profile space is exhausted. If the value is 1, the nagging messages appear. If the value is 0, no nagging messages appear; the user gets the mandatory message only when trying to log off with too much data in the profile.", REG_DWORD,  0, 1},
//	{"", "", "", REG_DWORD,  0, 1},
	{NULL, NULL, NULL, 0,  0, 0}
};


/*
*	{"RunLogonScriptSync",
    "Run logon scripts synchronously",
	 "Determines whether the shell waits for the logon script to complete or not. If the value is 0, the logon script is run during the startup of the shell and allows items in the Startup group to start. If the value is 1, the logon script completes before the shell or any items in the Startup group are started. If this value is also set in the Computer section, the Computer section value takes precedence.",
	 REG_DWORD,  0, 1},

 */
/////////////////////////////////////////////////////////////////////////////
// CSystemPolicies message handlers

void CSystemPolicies::OnChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CListCtrl*pWnd = (CListCtrl*) CWnd::FromHandle(pNMHDR->hwndFrom);
	CPoint pt;
	UINT   nFlags = 0;
	GetCursorPos(&pt);
	pWnd->ScreenToClient(&pt);
	int nHit = pWnd->HitTest(pt, &nFlags);
	if ((nHit != -1) && (nFlags & LVHT_ONITEMSTATEICON))
	{
		SetModified();	
		m_bChanged = true;
	}
	*pResult = 0;
}

void CSystemPolicies::OnRclickLstPolicyExplorer(NMHDR* pNMHDR, LRESULT* pResult) 
{

   CListCtrl*pWnd = (CListCtrl*) CWnd::FromHandle(pNMHDR->hwndFrom);
	CPoint pt;
	UINT   nFlags = 0;
	GetCursorPos(&pt);
	pWnd->ScreenToClient(&pt);
	int nHit = pWnd->HitTest(pt, &nFlags);
	if (nHit != -1) 
	{
		if (nFlags & LVHT_ONITEMICON )
		{
			LVITEM lvi;
			lvi.mask     = LVIF_IMAGE;
			lvi.iItem    = nHit;
			lvi.iSubItem = 0;
			m_List.GetItem(&lvi);
			lvi.iImage++;
			if (lvi.iImage > 2) lvi.iImage = 0;
			m_List.SetItem(&lvi);
			m_List.Update(nHit);
		}
		else
		{
			if ((nHit >= 0) && (nHit < m_List.GetItemCount()))
			{
				char *psz = ((g_pSystem[nHit].szDescription != NULL) ? g_pSystem[nHit].szDescription : g_pSystem[nHit].szPolicy);
				::MessageBox(m_hWnd, psz, g_pSystem[nHit].szKey, MB_OK|MB_ICONINFORMATION);
			}

		}
	}		

	*pResult = 0;
}

BOOL CSystemPolicies::OnInitDialog() 
{
	CString sHeadline;
	CRect rc;
	int   i;
	HKEY  hSecKey = NULL;
	LONG  lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_POLICIES_NON_ENUM, 0, KEY_READ, &hSecKey);
	DWORD dwValue, dwType, dwCount = sizeof(DWORD);

	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueEx(hSecKey, MY_DOCUMENTS, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			m_bRemoveMyDocumentsIcon = dwValue;
		}
		lResult = RegQueryValueEx(hSecKey, MY_COMPUTER, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			m_bRemoveMyComputerIcon = dwValue;
		}
	}
	REG_CLOSE_KEY(hSecKey);

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, HKCU_SW_POL_SYSTEM, 0, KEY_READ, &hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueEx(hSecKey, DISABLECMD, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			m_bDisableCmdPrompt = dwValue;
		}
	}
	REG_CLOSE_KEY(hSecKey);

	CPropertyPage::OnInitDialog();

	m_ImageList.Create(IDB_LIST_IMAGES, 14, 0, RGB(255, 255, 255));
	m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	
	sHeadline.LoadString(IDS_HL_EXP_POL);
	m_List.GetClientRect(&rc);
	LV_COLUMN lvc = {LVCF_TEXT|LVCF_WIDTH, 0, rc.right, (char*)LPCTSTR(sHeadline), sHeadline.GetLength(), 0, 0, 0};
	m_List.InsertColumn(0, &lvc);
	m_List.SetImageList(&m_ImageList, LVSIL_SMALL);

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, HKCU_SYSTEM_POLICIES, 0, KEY_READ, &hSecKey);
	if (lResult != ERROR_SUCCESS)
	{
		REG_CLOSE_KEY(hSecKey);
	}

	LVITEM lvi;
	lvi.mask     = LVIF_IMAGE;
	lvi.iSubItem = 0;
	for (i=0; g_pSystem[i].szKey != NULL; i++)
	{
		m_List.InsertItem(i, g_pSystem[i].szPolicy);
		lvi.iImage = theApp.GetProfileInt(SYSTEM_IMAGE, g_pSystem[i].szKey, 0);
		lvi.iItem  = i;
		m_List.SetItem(&lvi);

		if (hSecKey)
		{
			lResult = RegQueryValueEx(hSecKey, (LPTSTR)g_pSystem[i].szKey, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		}
		else
		{
			lResult = 1;
		}
		if (lResult == ERROR_SUCCESS)
		{	
			m_List.SetCheck(i, (dwValue != 0) ? TRUE:FALSE);
		}
		else
		{
			m_List.SetCheck(i, (g_pSystem[i].dwDefault != 0) ? TRUE:FALSE);
		}
	}
	REG_CLOSE_KEY(hSecKey);

	int nTest = theApp.GetProfileInt(SYSTEM_SAVED, MY_DOCUMENTS, -1);
	if (nTest==-1)
	{
		OnBtnSave();
	}

	return TRUE;
}

BOOL CSystemPolicies::OnApply() 
{
	if (m_bChanged)
	{
		DWORD dwValue, dw, i;
		DWORD dwCount = sizeof(DWORD);
		HKEY  hSecKey = NULL;
		LONG  lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HKCU_SYSTEM_POLICIES, 0, REG_NONE,
								REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);

		if (lResult == ERROR_SUCCESS)
		{
			for (i=0; g_pSystem[i].szKey != NULL; i++)
			{
				if (m_List.GetCheck(i))
				{
					dwValue = g_pSystem[i].dwSet;
				}
				else
				{
					dwValue = 0;
				}
				if (dwValue != g_pSystem[i].dwDefault)
				{
					lResult = RegSetValueEx(hSecKey, g_pSystem[i].szKey, NULL, g_pSystem[i].dwType, (BYTE*)&dwValue, dwCount);
				}
				else
				{
					RegDeleteValue(hSecKey, g_pSystem[i].szKey);
				}
			}
		}
		REG_CLOSE_KEY(hSecKey);

		lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, HKLM_POLICIES_NON_ENUM, 0, REG_NONE,
										REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
		if (lResult == ERROR_SUCCESS)
		{
			if (m_bRemoveMyDocumentsIcon)
			{
				dwValue = m_bRemoveMyDocumentsIcon;
				lResult = RegSetValueEx(hSecKey, MY_DOCUMENTS, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
			}
			else
			{
				RegDeleteValue(hSecKey, MY_DOCUMENTS);
			}
			if (m_bRemoveMyComputerIcon)
			{
				dwValue = m_bRemoveMyComputerIcon;
				lResult = RegSetValueEx(hSecKey, MY_COMPUTER, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
			}
			else
			{
				RegDeleteValue(hSecKey, MY_COMPUTER);
			}
		}
		REG_CLOSE_KEY(hSecKey);

		lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, HKCU_SW_POL_SYSTEM, 0, REG_NONE,
										REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
		if (lResult == ERROR_SUCCESS)
		{
			if (m_bDisableCmdPrompt)
			{
				dwValue = m_bDisableCmdPrompt;
				lResult = RegSetValueEx(hSecKey, DISABLECMD, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
			}
			else
			{
				RegDeleteValue(hSecKey, DISABLECMD);
			}
		}
		REG_CLOSE_KEY(hSecKey);
	
		SetModified(FALSE);
		m_bChanged = false;
	}
	return CPropertyPage::OnApply();
}

void CSystemPolicies::OnChanged() 
{
	SetModified(TRUE);
	m_bChanged = true;
}

void CSystemPolicies::OnBtnRestore() 
{
	LVITEM lvi;
	lvi.mask     = LVIF_IMAGE;
	lvi.iSubItem = 0;
	for (lvi.iItem=0; g_pSystem[lvi.iItem].szKey != NULL; lvi.iItem++)
	{
		m_List.SetCheck(lvi.iItem, theApp.GetProfileInt(SYSTEM_SAVED, g_pSystem[lvi.iItem].szKey, 0));
	}	

	m_bRemoveMyDocumentsIcon = theApp.GetProfileInt(SYSTEM_SAVED, MY_DOCUMENTS, 0);
	m_bRemoveMyComputerIcon  = theApp.GetProfileInt(SYSTEM_SAVED, MY_COMPUTER, 0);
	m_bDisableCmdPrompt      = theApp.GetProfileInt(SYSTEM_SAVED, DISABLECMD, 0);
	UpdateData(FALSE);
	OnChanged();
}

void CSystemPolicies::OnBtnSave() 
{
	if (UpdateData())
	{
		LVITEM lvi;
		lvi.mask     = LVIF_IMAGE;
		lvi.iSubItem = 0;
		for (lvi.iItem=0; g_pSystem[lvi.iItem].szKey != NULL; lvi.iItem++)
		{
			m_List.GetItem(&lvi);
			theApp.WriteProfileInt(SYSTEM_IMAGE, g_pSystem[lvi.iItem].szKey, lvi.iImage);
			theApp.WriteProfileInt(SYSTEM_SAVED, g_pSystem[lvi.iItem].szKey, m_List.GetCheck(lvi.iItem));
		}	

		theApp.WriteProfileInt(SYSTEM_SAVED, MY_DOCUMENTS, m_bRemoveMyDocumentsIcon);
		theApp.WriteProfileInt(SYSTEM_SAVED, MY_COMPUTER, m_bRemoveMyComputerIcon);
		theApp.WriteProfileInt(SYSTEM_SAVED, DISABLECMD, m_bDisableCmdPrompt);
	}
}

LRESULT CSystemPolicies::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;
	switch (pHI->iCtrlId)
	{
		case IDC_CK_REMOVE_MY_DOCUMENT_ICON: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_POLICIES_NON_ENUM, MY_DOCUMENTS); break;
		case IDC_CK_REMOVE_MY_COMPUTER_ICON: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_POLICIES_NON_ENUM, MY_COMPUTER); break;
		case IDC_CK_DISABLE_COMMAND_PROMPT: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKCU_SW_POL_SYSTEM, DISABLECMD); break;
		case IDC_LST_POLICY_EXPLORER: str.Format("HKCU\\%s:\nCheckmarks for System Policies", HKCU_SYSTEM_POLICIES); break;
		default:break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}
