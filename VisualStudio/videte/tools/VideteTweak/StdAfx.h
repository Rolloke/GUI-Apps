// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9FFF7A79_F122_4BA5_8075_31C643DED546__INCLUDED_)
#define AFX_STDAFX_H__9FFF7A79_F122_4BA5_8075_31C643DED546__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Policies Explorer
#define HK_EXPLORER_POLICIES	   "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\"
#define NODRIVES						"NoDrives"
#define NODRIVEAUTORUN				"NoDriveAutoRun"
#define NOVIEWONDRIVE				"NoViewOnDrive"
#define NODRIVETYPEAUTORUN			"NoDriveTypeAutoRun"
// Policies System
#define HKCU_SYSTEM_POLICIES	   "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\"

// Policies Non Enum Folders
#define HKLM_POLICIES_NON_ENUM	"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\NonEnum\\"
// Folder Attribute Location
#define HKCU_FOLDER_LOCATION		"CLSID\\%s\\ShellFolder"
// Folder IDs
#define MY_DOCUMENTS					"{450D8FBA-AD25-11D0-98A8-0800361B1103}"
#define MY_COMPUTER					"{20D04FE0-3AEA-1069-A2D8-08002B30309D}"
#define ADMINISTRATIVE_TOOLS		"{D20EA4E1-3957-11d2-A40B-0C5020524153}"
#define BRIEFCASE						"{85BBD92O-42A0-1O69-A2E4-08002B30309D}"
#define CONTROL_PANEL				"{21EC2O2O-3AEA-1O69-A2DD-08002b30309d}"
#define FONTS							"{D20EA4E1-3957-11d2-A40B-0C5020524152}"
#define HISTORY						"{FF393560-C2A7-11CF-BFF4-444553540000}"
#define INBOX							"{00020D75-0000-0000-C000-000000000046}"
#define MICROSOFT_NETWORK			"{00028B00-0000-0000-C000-000000000046}"
#define NETWORK_COMPUTERS			"{1f4de370-d627-11d1-ba4f-00a0c91eedba}"
#define NETWORK_CONNECTIONS		"{7007ACC7-3202-11D1-AAD2-00805FC1270E}"
#define PRINTERS_AND_FAXES			"{2227A280-3AEA-1069-A2DE-08002B30309D}"
#define PROGRAMS_FOLDER				"{7be9d83c-a729-4d97-b5a7-1b7313c39e0a}"
#define RECYCLE_BIN					"{645FF040-5081-101B-9F08-00AA002F954E}"
#define SCANNERS_AND_CAMERAS		"{E211B736-43FD-11D1-9EFB-0000F8757FCD}"
#define SCHEDULED_TASKS				"{D6277990-4C6A-11CF-8D87-00AA0060F5BF}"
#define START_MENU_FOLDER			"{48e7caab-b918-4e58-a94d-505519c795dc}"
#define TEMPORARY_INTERNET_FILES "{7BD29E00-76C1-11CF-9DD0-00A0C9034933}"
#define WEB_FOLDERS					"{BDEADF00-C265-11d0-BCED-00A0C90AB50F}"

#define FOLDER_ATTRIBUTES			"Attributes"

/*
// FOLDER_ATTRIBUTES : Flags

	File Features 
	50 01 00 20 - Rename 
	60 01 00 20 - Delete 
	70 01 00 20 - Rename and Delete 

	Edit Features 
	41 01 00 20 - Copy 
	42 01 00 20 - Cut 
	43 01 00 20 - Copy and Cut 
	44 01 00 20 - Paste 
	45 01 00 20 - Copy and Pase 
	46 01 00 20 - Cut and Copy 
	47 01 00 20 - Cut, Copy and Paste 
*/
// System 
#define HKCU_SW_POL_SYSTEM			"Software\\Policies\\Microsoft\\Windows\\System"
#define HKCU_SW_GRP_POL_SYSTEM	"Software\\Microsoft\\Windows\\CurrentVersion\\Group Policy Objects\\LocalUser\\Software\\Policies\\Microsoft\\Windows\\System"
#define DISABLECMD					"DisableCMD"

// Memory Management
#define HKLM_MEMORY_MNGMNT_KEY	"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management\\"
#define DISABLE_KERNEL_PAGING		"DisablePagingExecutive"
#define USE_LARGE_SYST_CACHE		"LargeSystemCache"
#define FILE_CACHE_SIZE				"IoPageLockLimit"
#define CLEAR_PAGEFILE_ASD			"ClearPageFileAtShutdown"
#define SECOND_LEVEL_DATA_CACHE		"SecondLevelDataCache"
#define HKLM_LANMAN_SERV_P_KEY	"SYSTEM\\CurrentControlSet\\Services\\LanmanServer\\Parameters"
#define LANMAN_CACHE_SIZE			"Size"

// Explore settings
#define HKLM_EXPLORER				"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
#define UNLOAD_DLL_ALWAYS			"AlwaysUnloadDll"

// File System Settings
#define HKLM_FILESYSTEM				"SYSTEM\\CurrentControlSet\\Control\\FileSystem\\"
#define NTFS_MFT_SIZE				"NtfsMftZoneReservation"
#define NTFS_DISABLE_8DOT3_FN		"NtfsDisable8dot3NameCreation"
#define NTFS_DISABLE_L_A_UPDT		"NtfsDisableLastAccessUpdate"

/// Atapi Settings 
#define HKLM_ATAPI_PARAMS			"System\\CurrentControlSet\\Services\\Atapi\\Parameters\\"
#define ENABLEBIGLBA					"EnableBigLba"

// WBEM/CIMON/
#define HKLM_WBEM_CIMOM				"SOFTWARE\\Microsoft\\WBEM\\CIMOM\\"
#define ENABLE_EVENTS				"EnableEvents"

// Encrypted File System
#define HKLM_EFS						"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\EFS\\"
#define KEY_CACHE_VAL_PER			"KeyCacheValidationPeriod"

// Registered Version
#define HKLM_CURRENTVERSION		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"
#define REGISTEREDORGANIZATION	"RegisteredOrganization"
#define REGISTEREDOWNER				"RegisteredOwner"
#define PRODUCTID						"ProductId"
#define REGDONE						"RegDone"

// CD File System
#define HKLM_CONTROL_CDFS			"SYSTEM\\CurrentControlSet\\Control\\CDFS\\"
#define CACHE_SIZE					"CacheSize"
#define PREFETCH_TAIL				"PrefetchTail"
#define PREFETCH						"Prefetch"

// Control Windows
#define HKLM_CONTROL_WINDOWS		"SYSTEM\\CurrentControlSet\\Control\\Windows\\"
#define NOPOPUPSONBOOT				"NoPopUpsOnBoot"
#define NOINTERACTIVESERVICES		"NoInteractiveServices"

// Local Environment Variables
#define HKLM_LOCAL_ENVIRONMENT	"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment\\"

// Logon Logoff Settings
#define HKLM_WINLOGON				"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\"
#define DEFAULTDOMAINNAME			"DefaultDomainName"
#define DEFAULTPASSWORD				"DefaultPassword"
#define DEFAULTUSERNAME				"DefaultUserName"
#define AUTOADMINLOGON				"AutoAdminLogon"
#define DONTDISPLAYLASTUSERNAME	"DontDisplayLastUserName"
#define DELETEROAMINGCACHE			"DeleteRoamingCache"
#define CACHEDLOGONSCOUNT			"CachedLogonsCount"
#define POWERDOWNAFTERSHUTDOWN	"PowerDownAfterShutdown"
#define SLOWLINKTIMEOUT				"SlowLinkTimeOut"
#define SLOWLINKDETECTENABLED		"SlowLinkDetectEnabled"

// Driver Signing
#define HKCU_DRIVERSIGNING			"Software\\Microsoft\\Driver Signing\\"
#define POLICY							"Policy"

// Desktop Settings
#define HKCU_DESKTOP					"Control Panel\\Desktop\\"
#define HKU_DEFAULT					".DEFAULT"
#define MENU_SHOW_DELAY				"MenuShowDelay"
#define MUILANGUAGEPENDING			"MUILanguagePending"
#define MULTIUILANGUAGEID			"MultiUILanguageId"

// MUI Interface
#define HKLM_MUILANGUAGES			"SYSTEM\\CurrentControlSet\\Control\\Nls\\MUILanguages"

// Hardware Details
#define HKLM_SESSION_MANAGER		"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"
#define SHOW_DETAILS_DEVMGR		"DEVMGR_SHOW_DETAILS"

#define HKLM_SERVICES            "SYSTEM\\CurrentControlSet\\Services"
#define SERVICE_DISP_NAME			"DisplayName"
#define SERVICE_START_TYPE			"Start"
#define SERVICE_DESCRIPTIONTXT	"Description"

struct RegKeyInfo
{
	char *szKey;
	char *szPolicy;
	char *szDescription;
	DWORD dwType;
	DWORD dwDefault;
	DWORD dwSet;
};

struct RegKeyInfoEx
{
	RegKeyInfo	ki;
	HKEY			hKey;
	DWORD       dwFlags;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9FFF7A79_F122_4BA5_8075_31C643DED546__INCLUDED_)
