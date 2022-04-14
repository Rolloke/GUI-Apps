// SystemTimeOuts.cpp : implementation file
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "SystemTimeOuts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemTimeOuts property page
#define TIMEOUTS_SAVED		"SavedTimeOuts"

#define MILISECONDS			0x00000001
#define SECONDS				0x00000002
#define MINUTES				0x00000004
#define HOURS				0x00000008

#define SHOW_DIFFERENT		0x00000f00
#define SHOW_MILISECONDS	0x00000100
#define SHOW_SECONDS		0x00000200
#define SHOW_MINUTES		0x00000400
#define SHOW_HOURS			0x00000800

#define ENUM_SUBKEYS		0x00010000
	
static RegKeyInfoEx g_pSystemTimeOuts[] =
{
	{{"Control Panel\\Accessibility\\TimeOut\\", "TimeToWait", "Control Panel Accessibility Timeout", REG_SZ, 300000, 1}, HKEY_CURRENT_USER, MILISECONDS|SHOW_SECONDS},
	{{"Control Panel\\Desktop\\", "ActiveWndTrkTimeout", "Active Window Mouse Track Timeout", REG_DWORD, 0, 0}, HKEY_CURRENT_USER, MILISECONDS},
	{{"Control Panel\\Desktop\\", "ClickLockTime", "Click Lock Time for Double Click", REG_DWORD, 2200, 0}, HKEY_CURRENT_USER, MILISECONDS},
	{{"Control Panel\\Desktop\\", "ForegroundLockTimeout", "Application Foreground Lock Timeout", REG_DWORD, 200000, 0}, HKEY_CURRENT_USER, MILISECONDS|SHOW_SECONDS},
	{{"Control Panel\\Desktop\\", "WaitToKillAppTimeout", "Wait to kill Applications", REG_SZ, 20000, 0}, HKEY_CURRENT_USER, MILISECONDS|SHOW_SECONDS},
	{{"Control Panel\\Desktop\\", "HungAppTimeout", "Wait to kill hung Applications", REG_SZ, 25000, 0}, HKEY_CURRENT_USER, MILISECONDS|SHOW_SECONDS},
	{{"Control Panel\\Desktop\\", "ScreenSaveTimeOut", "Screen Saver Time Out", REG_SZ, 1200, 0}, HKEY_CURRENT_USER, SECONDS|SHOW_MINUTES},
	{{"Control Panel\\Desktop\\", "LowPowerTimeOut", "Low Power Time Out", REG_SZ, 0, 0}, HKEY_CURRENT_USER, SECONDS},
	{{"Control Panel\\Desktop\\", "PowerOffTimeOut", "Power Off Time Out", REG_SZ, 0, 0}, HKEY_CURRENT_USER, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\", "WaitToKillServiceTimeout", "Wait to kill Service Timeout", REG_SZ, 20000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "DaemonResponseTimeout", "CiDaemon.EXE indexing a corrupt file Timeout", REG_DWORD, 5, 0}, HKEY_LOCAL_MACHINE, MINUTES},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "FilterIdleTimeout", "CiDaemon.exe Filter Release Timeout", REG_DWORD, 900000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_MINUTES},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "RequestTimeout", "Indexing Service Request Timeout", REG_DWORD, 10000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "MasterMergeTime", "Master Merge Time in Min. after Midnight", REG_DWORD,  60, 0}, HKEY_LOCAL_MACHINE, MINUTES},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "MaxQueryTimeslice", "Max Query Timeslice of CPU time", REG_DWORD,  50, 0}, HKEY_LOCAL_MACHINE, MILISECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "MaxWebhitsCpuTime", "Max Webhits Cpu Time (WEBHITS.dll)", REG_DWORD,  30, 0}, HKEY_LOCAL_MACHINE, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "MinClientIdleTime", "Min Client Idle Time", REG_DWORD,  600, 0}, HKEY_LOCAL_MACHINE, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "StartupDelay", "Startup Delay", REG_DWORD,  480000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_MINUTES},
	{{"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\", "CriticalSectionTimeout", "Critical Section", REG_DWORD, 2592000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_MINUTES},
	{{"SYSTEM\\CurrentControlSet\\Services\\NetBT\\Parameters\\", "BcastQueryTimeout", "NetBIOS Broadcast Query", REG_DWORD, 750, 0}, HKEY_LOCAL_MACHINE, MILISECONDS},
	{{"SYSTEM\\CurrentControlSet\\Services\\NetBT\\Parameters\\", "CacheTimeout", "NetBIOS Cache Timeout", REG_DWORD, 600000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_MINUTES},
	{{"SYSTEM\\CurrentControlSet\\Services\\NetBT\\Parameters\\", "NameSrvQueryTimeout", "NetBIOS Name Server Query", REG_DWORD, 1500, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Services\\NtmsSvc\\Parameters\\", "ShutdownTimeout", "Nt Svc Shutdown", REG_DWORD, 30000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_SECONDS},
	{{"Control Panel\\Accessibility\\Keyboard Response\\", "BounceTime", "Keyboard Response Bounce Time", REG_SZ, 0, 0}, HKEY_CURRENT_USER, MILISECONDS},
	{{"Control Panel\\Accessibility\\Keyboard Response\\", "DelayBeforeAcceptance", "Keyboard Response Delay Before Acceptance", REG_SZ, 1000, 0}, HKEY_CURRENT_USER, MILISECONDS},
	{{"Control Panel\\Accessibility\\MouseKeys", "TimeToMaximumSpeed", "Time to maximum Speed of MouseKeys ", REG_SZ, 3000, 0}, HKEY_CURRENT_USER, MILISECONDS|SHOW_SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Services\\NtmsSvc\\Parameters\\", "ShutdownTimeout", "Shutdown Timeout", REG_DWORD,  30000, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_SECONDS},

/*
	{{"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers\\DCI\\", "Timeout", "Graphics Drivers", REG_DWORD, 7, 0}, HKEY_LOCAL_MACHINE, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Services\\WebClient\\Parameters\\", "ServerNotFoundCacheLifeTimeInSec", "Server Not Found Cache Life Time", REG_DWORD,  30, 0}, HKEY_LOCAL_MACHINE, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\WOW\\", "SharedWowTimeout", "Shared WOW Timeout", REG_DWORD,  3600, 0}, HKEY_LOCAL_MACHINE, MILISECONDS|SHOW_SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\PrintServer\\Ports\\PrintServer\\", "NetTimeout", "Print Server Net Timeout", REG_DWORD,  50, 0}, HKEY_LOCAL_MACHINE, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\WOW\\", "LPT_timeout", "LPT Timeout", REG_SZ,  15, 0}, HKEY_LOCAL_MACHINE, SECONDS},
	{{"SYSTEM\\CurrentControlSet\\Control\\ContentIndex\\", "MinMergeIdleTime", "Min Merge Idle Time", REG_DWORD,  90, 0}, HKEY_LOCAL_MACHINE, PERCENT},
	{{"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}\\", "TcpConnectTimeout", "TCP Connect Timeout", REG_DWORD, 30, 0}, HKEY_LOCAL_MACHINE, SECONDS|ENUM_SUBKEYS},
	{{"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}\\", "TcpDisconnectTimeout", "TCP Disonnect Timeout", REG_DWORD, 30, 0}, HKEY_LOCAL_MACHINE, SECONDS|ENUM_SUBKEYS},
*/
	{{NULL, NULL, NULL, 0,  0, 0}, NULL, 0}
};

IMPLEMENT_DYNCREATE(CSystemTimeOuts, CPropertyPage)

CSystemTimeOuts::CSystemTimeOuts() : CPropertyPage(CSystemTimeOuts::IDD)
{
	//{{AFX_DATA_INIT(CSystemTimeOuts)
	//}}AFX_DATA_INIT
	m_bChanged = false;
}

CSystemTimeOuts::~CSystemTimeOuts()
{
}

void CSystemTimeOuts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemTimeOuts)
	DDX_Control(pDX, IDC_LST_TIMEOUT, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemTimeOuts, CPropertyPage)
	//{{AFX_MSG_MAP(CSystemTimeOuts)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_TIMEOUT, OnGetdispinfoLstTimeout)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LST_TIMEOUT, OnEndlabeleditLstTimeout)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_NOTIFY(NM_RCLICK, IDC_LST_TIMEOUT, OnRclickLstTimeout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemTimeOuts message handlers

BOOL CSystemTimeOuts::OnApply() 
{
	if (m_bChanged)
	{
		DWORD dwValue, dw;
		DWORD dwCount = sizeof(DWORD);
		HKEY  hSecKey = NULL;
		LONG  lResult, nItem;
		char  szText[MAX_PATH];

		for (nItem=0; g_pSystemTimeOuts[nItem].ki.szKey != NULL; nItem++)
		{
			lResult = RegCreateKeyEx(g_pSystemTimeOuts[nItem].hKey, g_pSystemTimeOuts[nItem].ki.szKey, 0, REG_NONE,
									REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);

			if (g_pSystemTimeOuts[nItem].ki.dwType == REG_DWORD)
			{
				dwCount = sizeof(DWORD);
				dwValue = g_pSystemTimeOuts[nItem].ki.dwSet;
				lResult = RegSetValueEx(hSecKey, g_pSystemTimeOuts[nItem].ki.szPolicy, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
			}
			else if (g_pSystemTimeOuts[nItem].ki.dwType == REG_SZ)
			{
				wsprintf(szText, "%d", g_pSystemTimeOuts[nItem].ki.dwSet);
				dwCount = strlen(szText)+1;
				lResult = RegSetValueEx(hSecKey, g_pSystemTimeOuts[nItem].ki.szPolicy, NULL, REG_SZ, (BYTE*)szText, dwCount);
			}

			if (lResult == ERROR_SUCCESS)
			{
				REG_CLOSE_KEY(hSecKey);
			}
		}
		m_bChanged = false;
		SetModified(FALSE);
	}
	return CPropertyPage::OnApply();
}

BOOL CSystemTimeOuts::OnInitDialog() 
{
	CRect rc;
	int   pnOrder[2] = {1, 0};
	HKEY  hSecKey = NULL;
	LONG  lResult;
	DWORD dwValue, dwType, dwCount = sizeof(DWORD);
	char  szText[MAX_PATH];
	CPropertyPage::OnInitDialog();
	
	m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_List.GetClientRect(&rc);
	m_List.InsertColumn(0, "Time", LVCFMT_LEFT, rc.right>>2);
	m_List.InsertColumn(1, "Description", LVCFMT_LEFT, (rc.right>>2)*3);

	m_List.SetColumnOrderArray(2, pnOrder);
	int nItem;
	for (nItem=0; g_pSystemTimeOuts[nItem].ki.szKey != NULL; nItem++)
	{
		lResult = RegOpenKeyEx(g_pSystemTimeOuts[nItem].hKey, g_pSystemTimeOuts[nItem].ki.szKey, 0, KEY_READ, &hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			if (g_pSystemTimeOuts[nItem].ki.dwType == REG_DWORD)
			{
				dwCount = sizeof(DWORD);
				lResult = RegQueryValueEx(hSecKey, g_pSystemTimeOuts[nItem].ki.szPolicy, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
				if ((lResult == ERROR_SUCCESS) && (dwType == REG_DWORD))
				{
					g_pSystemTimeOuts[nItem].ki.dwSet = dwValue;
				}
			}
			else if (g_pSystemTimeOuts[nItem].ki.dwType == REG_SZ)
			{
				dwCount = MAX_PATH-1;
				lResult = RegQueryValueEx(hSecKey, g_pSystemTimeOuts[nItem].ki.szPolicy, NULL, &dwType, (LPBYTE)szText, &dwCount);
				if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ))
				{
					g_pSystemTimeOuts[nItem].ki.dwSet = atoi(szText);
				}
			}
			REG_CLOSE_KEY(hSecKey);
		}
		
	}

	m_List.SetItemCount(nItem);

	CString str;
	str = g_pSystemTimeOuts[0].ki.szKey;
	str += g_pSystemTimeOuts[0].ki.szPolicy;
	str.Replace("\\", "_");
	str.Replace(" ", "_");
	int nTest = theApp.GetProfileInt(TIMEOUTS_SAVED, str, -1);
	if (nTest==-1)
	{
		OnBtnSave();
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSystemTimeOuts::OnGetdispinfoLstTimeout(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	//pDispInfo->item.
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
			case 0:
			{
				int nTime = g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet;
				if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_DIFFERENT)
				{
					if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & MILISECONDS)
					{
						if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_SECONDS)
							nTime = g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet / 1000;
						else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_MINUTES)
							nTime = g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet / 60000;
						else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_HOURS)
							nTime = g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet / 3600000;
					}
					else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SECONDS)
					{
						if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_MINUTES)
							nTime = g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet / 60;
						else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_HOURS)
							nTime = g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet / 3600;
					}
					wsprintf(pDispInfo->item.pszText, "%d ", nTime);
					if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_SECONDS)
						strncat(pDispInfo->item.pszText, "sec", pDispInfo->item.cchTextMax);
					else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_MINUTES)
						strncat(pDispInfo->item.pszText, "min", pDispInfo->item.cchTextMax);
					else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_HOURS)
						strncat(pDispInfo->item.pszText, "h", pDispInfo->item.cchTextMax);
				}
				else
				{
					wsprintf(pDispInfo->item.pszText, "%d ", nTime);
					if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & MILISECONDS)
						strncat(pDispInfo->item.pszText, "ms", pDispInfo->item.cchTextMax);
					else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SECONDS)
						strncat(pDispInfo->item.pszText, "sec", pDispInfo->item.cchTextMax);
					else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & MINUTES)
						strncat(pDispInfo->item.pszText, "min", pDispInfo->item.cchTextMax);
					else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & HOURS)
						strncat(pDispInfo->item.pszText, "h", pDispInfo->item.cchTextMax);
				}
			}break;
			case 1:
			{
				strncpy(pDispInfo->item.pszText, g_pSystemTimeOuts[pDispInfo->item.iItem].ki.szDescription, pDispInfo->item.cchTextMax);
			}break;
		}
	}
	
	*pResult = 0;
}

void CSystemTimeOuts::OnEndlabeleditLstTimeout(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
		int nTime = atoi(pDispInfo->item.pszText);
		if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_DIFFERENT)
		{
			if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & MILISECONDS)
			{
				if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_SECONDS)
					nTime *= 1000;
				else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_MINUTES)
					nTime *= 60000;
				else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_HOURS)
					nTime *= 3600000;
			}
			else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SECONDS)
			{
				if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_MINUTES)
					nTime *= 60;
				else if (g_pSystemTimeOuts[pDispInfo->item.iItem].dwFlags & SHOW_HOURS)
					nTime *= 3600;
			}
		}

		g_pSystemTimeOuts[pDispInfo->item.iItem].ki.dwSet = nTime;
		m_List.Update(pDispInfo->item.iItem);
		SetModified();	
		m_bChanged = true;
	}	
	*pResult = 0;
}

void CSystemTimeOuts::OnBtnRestore() 
{
	int nItem;
	CString str;
	for (nItem=0; g_pSystemTimeOuts[nItem].ki.szKey != NULL; nItem++)
	{
		str = g_pSystemTimeOuts[nItem].ki.szKey;
		str += g_pSystemTimeOuts[nItem].ki.szPolicy;
		str.Replace("\\", "_");
		str.Replace(" ", "_");
		g_pSystemTimeOuts[nItem].ki.dwSet = theApp.GetProfileInt(TIMEOUTS_SAVED, str, g_pSystemTimeOuts[nItem].ki.dwDefault);
	}
	m_List.InvalidateRect(NULL);
	SetModified();	
	m_bChanged = true;
}

void CSystemTimeOuts::OnBtnSave() 
{
	int nItem;
	CString str;
	for (nItem=0; g_pSystemTimeOuts[nItem].ki.szKey != NULL; nItem++)
	{
		str = g_pSystemTimeOuts[nItem].ki.szKey;
		str += g_pSystemTimeOuts[nItem].ki.szPolicy;
		str.Replace("\\", "_");
		str.Replace(" ", "_");
		theApp.WriteProfileInt(TIMEOUTS_SAVED, str, g_pSystemTimeOuts[nItem].ki.dwSet);
	}
}

LRESULT CSystemTimeOuts::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;
	switch (pHI->iCtrlId)
	{

		case 1:
		default:break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}

void CSystemTimeOuts::OnRclickLstTimeout(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CListCtrl*pWnd = (CListCtrl*) CWnd::FromHandle(pNMHDR->hwndFrom);
	CPoint pt;
	UINT   nFlags = 0;
	GetCursorPos(&pt);
	pWnd->ScreenToClient(&pt);
	int nHit = pWnd->HitTest(pt, &nFlags);
	if (nHit != -1) 
	{
		if ((nHit >= 0) && (nHit < m_List.GetItemCount()))
		{
			CString str;
			char *pszType = "";
			char *pszKey  = "";
			switch (g_pSystemTimeOuts[nHit].ki.dwType)
			{
			   case REG_SZ: pszType = "REG_SZ"; break;
			   case REG_DWORD: pszType = "REG_DWORD"; break;
			   case REG_BINARY: pszType = "REG_BINARY"; break;
			}
			switch ((DWORD)g_pSystemTimeOuts[nHit].hKey)
			{
			   case ((DWORD)HKEY_CURRENT_USER): pszKey = "HKEY_CURRENT_USER"; break;
			   case ((DWORD)HKEY_LOCAL_MACHINE): pszKey = "HKEY_LOCAL_MACHINE"; break;
			}
			str.Format("%s\\%s:%s\n\n%s\nDefault:%d, %x, (%s)", pszKey, g_pSystemTimeOuts[nHit].ki.szKey, g_pSystemTimeOuts[nHit].ki.szPolicy, g_pSystemTimeOuts[nHit].ki.szDescription, g_pSystemTimeOuts[nHit].ki.dwDefault, g_pSystemTimeOuts[nHit].ki.dwDefault, pszType);
		
			::MessageBox(m_hWnd, str, g_pSystemTimeOuts[nHit].ki.szPolicy, MB_OK|MB_ICONINFORMATION);
		}
	}		
	
	*pResult = 0;
}
