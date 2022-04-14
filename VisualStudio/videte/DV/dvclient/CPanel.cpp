// CPanel.cpp : implementation file
//
#include "stdafx.h"
#include "winspool.h"
#include "DVClient.h"
#include "CPanel.h"
#include "Server.h"
#include "IPCOutputDVClient.h"
#include "LiveWindow.h"
#include "MainFrame.h"
#include "CMinimizedDlg.h"
#include "CKeyboardDlg.h"
#include "CRTEDlg.h"
#include "CExportDlg.h"
#include "CStatisticDlg.h"
#include "AlarmlistDlg.h"
#include "CMDConfigDlg.h"
#include "HostsDlg.h"
#include "IPCDatabaseDVClient.h"			    
#include "IPCDataCarrierClient.h"
#include "Settings.h"
#include "InfoDlg.h"
#include "MaintenanceDlg.h"
#include "BackupEraseMedium.h"
#include "CIPCAudioDVClient.h"
#include <afximpl.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE					   
static char THIS_FILE[] = __FILE__;
#endif


#include "resource.h"
#include ".\cpanel.h"

#define DO_REBOOT 2

#define _TEST_DHCP_SETTINGS		1

#define NET_SHELL_EVENT_START		200
#define NET_SHELL_EVENT_IP_INT		201
#define NET_SHELL_EVENT_CMD_SENT	202
#define NET_SHELL_EVENT_STOP		203

#define INCREMENT +1
#define DECREMENT -1

extern CDVClientApp theApp;


int g_iaCameraFPS[CAMERA_FPS_STEPS]	=  {1,2,3,4,5,6,8,12,25};

#define IDC_CPU_USAGE			100
#define IDC_CPU_AVG_USAGE		101
#define IDC_PERFORMANCE_LEVEL	102

class CSkinProgressEx : public CSkinProgress  
{
public:
	CSkinProgressEx() {};
    virtual ~CSkinProgressEx() {};

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

LRESULT CSkinProgressEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDOWN && GetDlgCtrlID() == IDC_PERFORMANCE_LEVEL)
	{
		int nY = HIWORD(lParam);
		CRect rc;
		GetClientRect(&rc);
		if (nY < rc.bottom >> 1)
		{
			theApp.IncreasePerformanceLevel();
		}
		else
		{
			theApp.DecreasePerformanceLevel();
		}
		SendMessage(PBM_SETPOS, theApp.GetPerformanceLevel());
	}
	return CSkinProgress::WindowProc(message, wParam, lParam);
}


// dynamisches Laden der IpHlpApi.dll
HMODULE hIpHlpApiDll = NULL;
HMODULE hKernel32Dll = NULL;
DWORD (WINAPI *pGetIpAddrTableU)(PMIB_IPADDRTABLE, PULONG, BOOL) = NULL;
DWORD (WINAPI *pGetIpForwardTableU)(PMIB_IPFORWARDTABLE, PULONG, BOOL) = NULL;
DWORD (WINAPI *pGetInterfaceInfo)(PIP_INTERFACE_INFO pIfTable, PULONG dwOutBufLen) = NULL;
DWORD (WINAPI *pGetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen) = NULL;
DWORD (WINAPI *pGetIfEntry)(PMIB_IFROW pIfRow) = NULL;

BOOL  (WINAPI *pGetVolumeNameForVolumeMountPoint)(LPCTSTR,LPTSTR,DWORD) = NULL;

void LoadApiDllFkts()
{
	hIpHlpApiDll = LoadLibrary(_T("iphlpapi.dll"));
	if (hIpHlpApiDll)
	{
		pGetIpAddrTableU    = (DWORD (WINAPI*)(PMIB_IPADDRTABLE, PULONG, BOOL))    GetProcAddress(hIpHlpApiDll, "GetIpAddrTable");
		pGetIpForwardTableU = (DWORD (WINAPI*)(PMIB_IPFORWARDTABLE, PULONG, BOOL)) GetProcAddress(hIpHlpApiDll, "GetIpForwardTable");
		pGetAdaptersInfo    = (DWORD (WINAPI*)(PIP_ADAPTER_INFO, PULONG))          GetProcAddress(hIpHlpApiDll, "GetAdaptersInfo");
		pGetInterfaceInfo   = (DWORD (WINAPI*)(PIP_INTERFACE_INFO, PULONG))        GetProcAddress(hIpHlpApiDll, "GetInterfaceInfo");
		pGetIfEntry		    = (DWORD (WINAPI*)(PMIB_IFROW)) 					   GetProcAddress(hIpHlpApiDll, "GetIfEntry");
	}
	hKernel32Dll = GetModuleHandle(_T("Kernel32.dll"));
	if (hKernel32Dll)
	{
#ifdef _UNICODE
		pGetVolumeNameForVolumeMountPoint = (BOOL (WINAPI*)(LPCTSTR,LPTSTR,DWORD))GetProcAddress(hKernel32Dll, "GetVolumeNameForVolumeMountPointW");
#else
		pGetVolumeNameForVolumeMountPoint = (BOOL (WINAPI*)(LPCTSTR,LPTSTR,DWORD))GetProcAddress(hKernel32Dll, "GetVolumeNameForVolumeMountPointA");
#endif
	}
}
DWORD GetIpAddrTableU(
  PMIB_IPADDRTABLE pIpAddrTable,  // buffer for mapping table 
  PULONG pdwSize,                 // size of buffer 
  BOOL bOrder                     // sort the table 
)
{
   if (pGetIpAddrTableU)
   {
      return pGetIpAddrTableU(pIpAddrTable,pdwSize,bOrder);
   }
   *pdwSize = 0;
   return ERROR_INVALID_FUNCTION;
}

DWORD GetIpForwardTableU(
   PMIB_IPFORWARDTABLE pIpForwardTable, // buffer for routing table
   PULONG pdwSize,                      // size of buffer
   BOOL bOrder                          // sort the table?
)
{
   if (pGetIpForwardTableU)
   {
      pGetIpForwardTableU(pIpForwardTable,pdwSize,bOrder);
   }
   *pdwSize = 0;
   return ERROR_INVALID_FUNCTION;
}

/////////////////////////////////////////////////////////////////////////////
static const TCHAR szDefaultLanguageEnu[] = _T("English");
COLORREF CPanel::m_BaseColorButtons = SKIN_COLOR_BLUE_SHADOW_DARK;
COLORREF CPanel::m_BaseColorBackGrounds = SKIN_COLOR_BLUE_SHADOW;
COLORREF CPanel::m_BaseColorDisplays = SKIN_COLOR_BLUE;
/////////////////////////////////////////////////////////////////////////////
// CPanel dialog
IMPLEMENT_DYNAMIC(CPanel, CTransparentDialog)

extern AFX_DATA_IMPORT AUX_DATA afxData;

/////////////////////////////////////////////////////////////////////////////
CPanel::CPanel() : 
	  CTransparentDialog(CPanel::IDD, NULL)
{
	//{{AFX_DATA_INIT(CPanel)
	//}}AFX_DATA_INIT

	LoadApiDllFkts();   // dynamisches laden der IpHlpApi.dll
	InitMembers();
	LoadConfiguration();
	InitConfiguration();
}

/////////////////////////////////////////////////////////////////////////////
CPanel::~CPanel()
{
	SaveConfiguration();
	WK_DELETE(m_pPrinterInfo);
	WK_DELETE(m_pnPrinterSort);
	WK_DELETE(m_pControlProcess);

	// Nicht notwendig, im Gegenteil: provoziert evtl. GP!
	// DLL wird nach Programmende sowieso automatisch entladen
	// UnloadLanguageDll();
	m_arrayLanguages.DeleteAll();

	if (    theApp.IsReceiver()
		&& !theApp.IsReadOnly())
	{
		// Hostliste löschen
		m_HostsAll.DeleteAll();
		CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		m_HostsAll.Save(prof);
	}


	if (hIpHlpApiDll)
	{
		FreeLibrary(hIpHlpApiDll);
		hIpHlpApiDll        = NULL;
		pGetIpAddrTableU    = NULL;
		pGetIpForwardTableU = NULL;
		pGetInterfaceInfo   = NULL;
		pGetAdaptersInfo    = NULL;

	}
	if (hKernel32Dll)
	{
		FreeLibrary(hKernel32Dll);
		hKernel32Dll        = NULL;
		pGetVolumeNameForVolumeMountPoint = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::StretchElements()
{
	// Border of Dialog has to be set to Resizing!
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::InitMembers()
{
	m_hLanguageDll					= NULL;
	m_bShowPower					= FALSE;
	m_bShowTargetCross				= TRUE;
	m_bAudibleAlarm					= FALSE;
	m_nSystemAGC					= 0;
	m_nSystemVideoformat			= VIDEO_FORMAT_MPEG4;
	m_sSystemVideonorm				= CSD_PAL;
	m_bRealtimePanel				= FALSE;
	m_bNetAllowedByDongle			= FALSE;
	m_bISDNAllowedByDongle			= FALSE;
	m_bBackUpAllowedByDongle		= FALSE;
	m_bBackUpToDVDAllowedByDongle	= FALSE;
	m_bDDMMYY						= TRUE;
	m_sIP							= _T("192168000001");
	m_sMSN							= _T("");
	m_sHostName						= _T("");
	m_sHostAddress					= _T("");
	m_nHostIndex					= 0;
	m_bDHCPenabled					= FALSE;
	m_dwInterfaceIndex				= 0xffffffff;
	DHCPParameters();
	if (!m_sNetworkAdapterName.IsEmpty())
	{
		m_bNetShellOK = GetFileAttributes(theApp.GetWindowsDirectory() + _T("\\System32\\netsh.exe")) != 0xffffffff;
	}
	else
	{
		m_bNetShellOK = FALSE;
	}
	if (m_bNetShellOK)
	{
		if (!GetIP(m_sIP, IPADDRESS))
		{
			m_sIP =	_T("????????????");
		}
		if (!GetIP(m_sSubNetMask, SUBNETMASK))
		{
			m_sSubNetMask = _T("255255255000");
		}
	}

	WK_TRACE(_T("Net Adapter Index: %d\n"), m_dwInterfaceIndex);
	WK_TRACE(_T("%s, %s\n"), m_sNetworkAdapterName, m_sNetworkAdapterKey);
	WK_TRACE(_T("netsh: %d\n"), m_bNetShellOK);
	WK_TRACE(_T("dhcp: %d\n"), m_bDHCPenabled);

	m_nMDAlarmLevel				= MD_SENSITIVITY_NORMAL;
	m_nMDMaskLevel				= MD_SENSITIVITY_NORMAL;
	m_nAlarmCallEvent			= ALARMCALL_EVENT_UNKNOWN;
	m_nAlarmCallEventByCardType = CARD_TYPE_UNKNOWN;

	m_nAudioRecordingMode   = AUDIO_RECORDING_UNKNOWN;
	m_nAudioCameraNumber    = ERROR_VALUE;

	m_nResolution				= RESOLUTION_2CIF;
	m_nCurrentResolution		= m_nResolution;
	m_nCompression				= COMPRESSION_1; //no compression
	m_nCameraFPS				= g_iaCameraFPS[0];
	m_nCurrentCompression		= m_nCompression;
	m_nCurrentCameraFPS			= m_nCameraFPS;
	m_nCameraPTZType			= 0;
	m_nCurrentCameraPTZType		= 0;
	m_dwCameraPTZID				= 0;
	m_nCameraPTZComPort			= 1;
	m_nCurrentCameraPTZComPort	= m_nCameraPTZComPort;
	m_pRTEDlg				= NULL;
	m_pKeyboardDlg			= NULL;
//	m_pExportDlg			= NULL;
	m_pStatisticDlg		= NULL;
	m_pAlarmlistDlg		= NULL;
	m_pInfoDlg			= NULL;
	m_pMaintenanceDlg	= NULL;
	m_pMDConfigDlg		= NULL;
	m_pUpdateThread		= NULL;
	m_pHostsDlg			= NULL;
	m_pControlProcess	= NULL;


	m_pPrinterInfo		= NULL;
	m_nPrinterInfoSize	= 0;
	m_nPrinterCnt		= 0;
	m_nNewDefaultPrt    = 0;
	m_pnPrinterSort		= NULL;
	
	m_pDefaultLanguage	= NULL;
	m_pOSD				= NULL;

	m_nExportDrive			= 1;
	m_nExternBackupDrive	= 1;
	CString sDrive(_T("no"));
	m_bBackupToExternDrive	= CBackupWindow::GetRemovableDrive(sDrive, m_nExternBackupDrive);
	m_dwFreeMBExternalDrive = 0;
	m_dwTotalMBExternalDrive = 0;
	m_bIsExternDriveEmpty = FALSE;
	CheckCapacityOfExternDrive(m_dwFreeMBExternalDrive, m_dwTotalMBExternalDrive);
	if(m_dwFreeMBExternalDrive == m_dwTotalMBExternalDrive)
	{
		m_bIsExternDriveEmpty = TRUE;
	}
	
	WK_TRACE(_T("found removable drive: %s\n"),sDrive);

	m_dwLastUserAction	= GetTickCount();
	m_dwMinimizeTimespan = 15000;
	m_hCursor			= NULL;
	m_iLanguageCnt		= 0;
	m_bDateTime			= 0;
	m_nCamNr			= 0;
	m_nDayOfWeek		= Su;
	m_eViewMode			= View1pX;
	m_sInput			= _T("");
	m_sNewPIN			= _T("");
	m_sSearchDate		= _T("");
	m_sSearchTime		= _T("");
	m_sExportPath		= _T("");
	m_sExportDriveName  = _T("");
	m_eInputState		= InputNo;
	m_bSetValue			= FALSE;
	m_eOem				= OemDTS;
	m_nCameras			= 4;
	m_bTimerOnOffState	= TRUE;
	m_bAlarmOutputOnOffState=TRUE;
	m_nOldBrightess		= 0;
	m_nOldContrast		= 0;
	m_nOldSaturation	= 0;
	m_nOldAudioValue	= 0;
	m_bValueOn			= 0;

	m_nTimeZone			= 0;

	m_bFloppyExportOnlySupervisor = FALSE;
	m_eUserMode = UM_INVALID;

	m_pMinimizedDlg		= NULL;
	m_bCanRectSearch	= FALSE;
	m_bImagesToBackup	= TRUE;
	m_bCalculateError	= FALSE;
	m_bTooManyImages	= FALSE;

    m_bCalculateEndComplete = FALSE;
	m_dwTimeoutCalculateEnd = 0;

	m_wMultiBackupCamLast		= 0;	
	m_wMultiBackupLastSequence	= 0;
	m_dwMultiBackupLastRecord	= 0;

	m_eConfirmingYesNoMode	= ConfirmingModeUnknown;

	m_MedienType		  = MT_NOMEDIUM;

	m_wNextSyncPlayWnd = 0;

	afxData.clrBtnFace = m_BaseColorButtons;
	afxData.clrWindowFrame = m_BaseColorBackGrounds;
	afxData.clrBtnShadow = CSkinButton::ChangeBrightness(afxData.clrBtnFace, -30, TRUE);
	afxData.clrBtnHilite = CSkinButton::ChangeBrightness(afxData.clrBtnFace, 30, TRUE);
	afxData.clrBtnText	= RGB(255,255,255);
	m_nTimerCounter = 0;
	InitCodePageByteArrays();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::InitCodePageByteArrays()
{
	CWK_Profile prof(CWK_Profile::WKP_REGPATH,
					 HKEY_LOCAL_MACHINE,
					 _T("SYSTEM\\CurrentControlSet\\Control\\Nls"),_T(""));
	m_sSystemCodePage = prof.GetString(_T("CodePage"), _T("ACP"), _T("1252"));

	int i;
/*	for (i=0 ; i<256 ; i++)
	{
		m_byCP1252To1250[i] = (BYTE)i;
	}
	m_byCP1252[] = _T('');
*/
	for (i=0 ; i<256 ; i++)
	{
		m_byCP1250To1252[i] = (BYTE)i;
	}
	m_byCP1250To1252[140] = (BYTE)_T('S');
	m_byCP1250To1252[141] = (BYTE)_T('T');
	m_byCP1250To1252[142] = (BYTE)_T('Z');
	m_byCP1250To1252[143] = (BYTE)_T('Z');
	m_byCP1250To1252[156] = (BYTE)_T('s');
	m_byCP1250To1252[157] = (BYTE)_T('t');
	m_byCP1250To1252[158] = (BYTE)_T('z');
	m_byCP1250To1252[159] = (BYTE)_T('z');
	m_byCP1250To1252[163] = (BYTE)_T('L');
	m_byCP1250To1252[165] = (BYTE)_T('A');
	m_byCP1250To1252[170] = (BYTE)_T('S');
	m_byCP1250To1252[175] = (BYTE)_T('Z');
	m_byCP1250To1252[179] = (BYTE)_T('l');
	m_byCP1250To1252[185] = (BYTE)_T('a');
	m_byCP1250To1252[186] = (BYTE)_T('s');
	m_byCP1250To1252[188] = (BYTE)_T('L');
	m_byCP1250To1252[190] = (BYTE)_T('l');
	m_byCP1250To1252[191] = (BYTE)_T('z');
	m_byCP1250To1252[192] = (BYTE)_T('R');
	m_byCP1250To1252[195] = (BYTE)_T('A');
	m_byCP1250To1252[197] = (BYTE)_T('L');
	m_byCP1250To1252[198] = (BYTE)_T('C');
	m_byCP1250To1252[200] = (BYTE)_T('C');
	m_byCP1250To1252[202] = (BYTE)_T('E');
	m_byCP1250To1252[204] = (BYTE)_T('E');
	m_byCP1250To1252[207] = (BYTE)_T('D');
	m_byCP1250To1252[209] = (BYTE)_T('N');
	m_byCP1250To1252[210] = (BYTE)_T('N');
	m_byCP1250To1252[213] = (BYTE)_T('Ö');
	m_byCP1250To1252[216] = (BYTE)_T('R');
	m_byCP1250To1252[217] = (BYTE)_T('U');
	m_byCP1250To1252[219] = (BYTE)_T('Ü');
	m_byCP1250To1252[222] = (BYTE)_T('T');
	m_byCP1250To1252[224] = (BYTE)_T('r');
	m_byCP1250To1252[227] = (BYTE)_T('a');
	m_byCP1250To1252[229] = (BYTE)_T('l');
	m_byCP1250To1252[230] = (BYTE)_T('c');
	m_byCP1250To1252[232] = (BYTE)_T('c');
	m_byCP1250To1252[234] = (BYTE)_T('e');
	m_byCP1250To1252[236] = (BYTE)_T('e');
	m_byCP1250To1252[239] = (BYTE)_T('d');
	m_byCP1250To1252[240] = (BYTE)_T('d');
	m_byCP1250To1252[241] = (BYTE)_T('n');
	m_byCP1250To1252[242] = (BYTE)_T('n');
	m_byCP1250To1252[245] = (BYTE)_T('ö');
	m_byCP1250To1252[248] = (BYTE)_T('r');
	m_byCP1250To1252[249] = (BYTE)_T('u');
	m_byCP1250To1252[251] = (BYTE)_T('ü');
	m_byCP1250To1252[254] = (BYTE)_T('t');
	m_byCP1250To1252[255] = (BYTE)_T('°');
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::InitConfiguration()
{
	// Am Anfang ist der Standardmenübaum aktiv.
	m_pOSD = &m_OSDNormal;

	// Am Anfang auf die gespeicherten Werte setzen
	m_OSDNormal.SetSubMenuPos(IDOSD_CAM1, m_nCamNr);
	m_OSDNormal.SetSubMenuPos(IDOSD_VIEW_FULL, m_eViewMode);
	m_OSDConfig.SetSubMenuPos(IDOSD_LANGUAGE1, max(0, GetDefaultLanguageIndex()));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::LoadLanguageDll()
{
	BOOL bLanguageChange = m_hLanguageDll != NULL ? TRUE : FALSE;

	// unload previous language dll
	if (UnloadLanguageDll())
	{
		Sleep(100);
		// load new language dll
		CString sDll = m_pDefaultLanguage->GetDll();
		TRACE(_T("%s\n"), sDll);
		m_hLanguageDll = AfxLoadLibrary(sDll);
		if (m_hLanguageDll)
		{
			WK_TRACE(_T("Language dll %s loaded\n"), m_pDefaultLanguage->GetDll());
		}
		else
		{
			WK_TRACE_ERROR(_T("No language dll %s ; Error %u ; %s\n"),
							m_pDefaultLanguage->GetDll(), GetLastError(), GetLastErrorString());
		}

		m_sPrevMsg.Empty();
		m_sCurrentLanguage = m_pDefaultLanguage->GetLanguage();
		_TCHAR* szLocale = _tsetlocale(LC_ALL, NULL);
		if (szLocale)
		{
			WK_TRACE(_T("Original locale: %s\n"), szLocale);
		}
		// Try to change code page
		if (theApp.ChangeCodePage())
		{
			CString sValue;
			CWK_Profile prof;
			CString sCodePage = prof.GetString(REGKEY_DVCLIENT,_T("CodePage"),_T(""));
			if (sCodePage.IsEmpty())
			{
				sCodePage = m_pDefaultLanguage->GetCodePage();
			}
			sValue.Format(_T(".%s"), sCodePage);

			szLocale = _tsetlocale(LC_CTYPE, sValue);
			if (szLocale)
			{
				WK_TRACE(_T("Locale set for: %s ; %s\n"), sValue, szLocale);
			}
			else
			{
				WK_TRACE_ERROR(_T("Locale NOT set for: %s\n"), sValue);
			}
		}
		ShowOnlineHelp();

		SaveConfiguration();

		// Update to show OEM name
		CMainFrame* pMainWnd  = theApp.GetMainFrame();
		if (pMainWnd)
		{
			pMainWnd->ClearOEMName();
			pMainWnd->InvalidateRect(NULL);
#ifdef _UNICODE
			DWORD dwCPBits = m_pDefaultLanguage->GetCodePageBits();
			if (dwCPBits)
			{
				SetFontFaceNamesFromCodePageBits(dwCPBits);
				if (CSkinDialog::UseGlobalFonts() && m_hWnd)
				{
					m_ctrlDisplayMenu1.OpenStockDisplay(this, Display2);
					m_ctrlDisplayMenu2.OpenStockDisplay(this, Display2);
					m_ctrlDisplayCamera.OpenStockDisplay(this, Display2);
					m_ctrlDisplayTooltip.OpenStockDisplay(this, Display2);
					m_ctrlDisplayDateTime.OpenStockDisplay(this, Display2);
				}
			}
			else
			{
				CSkinDialog::DoUseGlobalFonts(FALSE);
				SetFontFaceNames(NULL, NULL);
				if (m_hWnd)
				{
					m_ctrlDisplayMenu1.OpenStockDisplay(this, Display2);
					m_ctrlDisplayMenu2.OpenStockDisplay(this, Display2);
					m_ctrlDisplayCamera.OpenStockDisplay(this, Display2);
					m_ctrlDisplayTooltip.OpenStockDisplay(this, Display2);
					m_ctrlDisplayDateTime.OpenStockDisplay(this, Display2);
				}
			}
			if (bLanguageChange)
			{
				pMainWnd->UpdateLogoFont();
				CSkinDialog::SetChildWindowFont(m_hWnd);
				CWnd *pStarter = FindWindow(NULL, _T("DVStarter"));
				if (pStarter)
				{
					UINT uCodePage = 0;
					CString sCodePage = m_pDefaultLanguage->GetCodePage();
					_stscanf(sCodePage, _T("%d"), &uCodePage);
					pStarter->PostMessage(WM_LANGUAGE_CHANGED, uCodePage, dwCPBits);
				}
			}
#endif
		}
	}
}

int CALLBACK CPanel::EnumFontFamExProc(CONST ENUMLOGFONTEX *lpelfe, CONST NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	LOGFONT *plf = (LOGFONT*) lParam;
	if (   (lpntme->ntmFontSig.fsCsb[0] & plf->lfHeight)
		 && ((lpelfe->elfLogFont.lfPitchAndFamily & 0x0f) == plf->lfPitchAndFamily))
	{
		*plf = lpelfe->elfLogFont;
		return 0;
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::UnloadLanguageDll()
{
	BOOL bReturn = TRUE;
	if (m_hLanguageDll)
	{
		if (AfxFreeLibrary(m_hLanguageDll))
		{
			m_hLanguageDll = NULL;
			WK_TRACE(_T("Language dll %s unloaded\n"), m_pDefaultLanguage->GetDll());
		}
		else
		{
			WK_TRACE_ERROR(_T("Language dll %s NOT unloaded\n"), m_pDefaultLanguage->GetDll());
			bReturn = FALSE;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::LoadNormalMenu()
{
	// Beide Menübäume laden
	if (!m_OSDNormal.Open(IDR_MENU_OSD))
	{
		WK_TRACE_ERROR(_T("Can't load OSD-Menu\n"));
#ifdef _UNICODE  //  TODO RKE muß wieder entfernt werden
		AfxMessageBox(_T("Can't load OSD-Menu"));
#endif
	}
	if (theApp.IsReceiver())
	{
		// als Receiver nicht "Gerät sperren" sondern "Verbindung trennen"
		CString sText;
		sText.LoadString(IDS_DISCONNECT);
		m_OSDNormal.SetString(IDOSD_OPTION_LOCK, sText);
	}
	// Kameras-menü anpassen
	for (int nI = m_nCameras; nI < MAX_CAM; nI++)
	{
		m_OSDNormal.RemoveMenu(IDOSD_CAM1+nI);
	}

	// OEM Spezifische Einstellungen
	if ((m_eOem == OemAlarmCom) || (m_eOem == OemPOne))
	{
		if (m_nCameras<=8)
			m_OSDNormal.RemoveMenu(IDOSD_VIEW_XXX);
	}

	//den Menüeintrag "System Info" rausnehmen, wenn Server Version < 2.1.1.589
	if(theApp.IsReadOnly())
	{
		m_OSDNormal.RemoveMenu(IDOSD_OPTION_INFO);
	}
	else
	{
		CServer* pServer = theApp.GetServer();
		if(pServer)
		{
			if(!pServer->CanSystemInfo())
			{
				m_OSDNormal.RemoveMenu(IDOSD_OPTION_INFO);
			}
		}
	}
	
	//die Menüeinträge "Statistik" und "Alarmliste" bei ReadOnly rausnehmen, 
	//da diese beiden Punkte ab Version 2.5.0.603 ins Hauptmenü gewandert sind
	if(theApp.IsReadOnly())
	{
		m_OSDNormal.RemoveMenu(IDOSD_OPTION_STATISTIC);
		m_OSDNormal.RemoveMenu(IDOSD_SHOW_ALARMLIST);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::LoadConfigMenu()
{
	if (!m_OSDConfig.Open(IDR_MENU_CONFIG))
	{
		WK_TRACE_ERROR(_T("Can't load Config-Menu\n"));
	}

	// Nur die vorhandenen Sprachen zeigen
	for (int i=IDOSD_LANGUAGE16 - IDOSD_LANGUAGE1; i>=0 ; i--)
	{
		if (i >= m_iLanguageCnt)
			m_OSDConfig.RemoveMenu(IDOSD_LANGUAGE1+i);
		else
			break;
	}

	// Nur die vorhandenen Drucker zeigen
	EnumeratePrinters();
	for (int nI = MAX_PRINTERS; nI >= 1; nI--)
	{
		if (nI > m_nPrinterCnt)
			m_OSDConfig.RemoveMenu(IDOSD_NO_PRINTER+nI); 
		else
			break;
	}
	if (m_nPrinterCnt == 0)
		m_OSDConfig.RemoveMenu(IDOSD_NO_PRINTER); 
		
	// Ohne Netzwerk, oder in der ReadOnly-Variante keine Netzwerkeinstellungen
	if (!m_bNetAllowedByDongle || theApp.IsReadOnly())
	{
		m_OSDConfig.RemoveMenu(IDOSD_TCPIP);
		m_OSDConfig.RemoveMenu(IDOSD_SUBNETMASK);
		m_OSDConfig.RemoveMenu(IDOSD_NET_NAME);	
		m_OSDConfig.RemoveMenu(IDOSD_GATEWAY);
		m_OSDConfig.RemoveMenu(IDOSD_DNS);
	}
	else
	{
		// OEM Spezifische Einstellungen
		// Aus ">Stationsname:_______" wird eine Oem-abhängige Bezeichnung
		// z.B.">DTS name:_______"
		CString sMsg;
		sMsg.Format(m_OSDConfig.GetSubString(IDOSD_NET_NAME), theApp.GetMainFrame()->GetShortOemName());
		m_OSDConfig.SetSubString(sMsg, IDOSD_NET_NAME);

#ifdef _TEST_DHCP_SETTINGS
		m_OSDConfig.EnableMenuItem(IDOSD_TCPIP,      m_bDHCPenabled ? MF_DISABLED:MF_ENABLED);
		m_OSDConfig.EnableMenuItem(IDOSD_SUBNETMASK, m_bDHCPenabled ? MF_DISABLED:MF_ENABLED);
		if (!theApp.IsWinXP())
		{
			m_OSDConfig.RemoveMenu(IDOSD_DNS);
		}
#else
		m_OSDConfig.RemoveMenu(IDOSD_DNS);
#endif
	}

	// Ohne ISDN keine MSN einstellbar.
	if (!m_bISDNAllowedByDongle || theApp.IsReadOnly())
	{
		m_OSDConfig.RemoveMenu(IDOSD_ISDN_MSN);
	}

	//Ohne ISDN und ohne Netzwerk im Menü "Alarmkonfiguration" die Einträge
	//"Alarmanruf bei:" und "Rufe 1,2,3" entfernen
	if((!m_bISDNAllowedByDongle && !m_bNetAllowedByDongle) || theApp.IsReadOnly())
	{
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALLNUMBER1);
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALLNUMBER2);
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALLNUMBER3);
	}


	// Beim Receiver soll nicht 'Geräte ausschalten' stehen, sondern
	// 'Programm beenden'
	if (theApp.IsReceiver())
	{
		CString sMsg;
		sMsg.LoadString(IDS_EXIT_ENTRY);		
		m_OSDConfig.SetSubString(sMsg, IDOSD_OPTION_EXIT);
	}

	// System-spezifische Anpassungen
	if (theApp.GetServer())
	{
		CServer* pServer = theApp.GetServer();
		if (pServer->GetNrOfVideoOut()<4)
		{
			m_OSDConfig.RemoveMenu(IDOSD_VIDEO_OUT_MODE_PORT_4);
		}
		if (pServer->GetNrOfVideoOut()<3)
		{
			m_OSDConfig.RemoveMenu(IDOSD_VIDEO_OUT_MODE_PORT_3);
		}
		if (pServer->GetNrOfVideoOut()<2)
		{
			m_OSDConfig.RemoveMenu(IDOSD_VIDEO_OUT_MODE_PORT_2);
		}
		if (pServer->GetNrOfVideoOut()<1)
		{
			m_OSDConfig.RemoveMenu(IDOSD_VIDEO_OUT_MODE_PORT_1);
		}
	}


	//nur, wenn 
	// Dem Empfänger fehlt:
	//	- das Bilder-Menü
	//	- das Netzwerk-Menü
//	//	- der Sequenzer					doch rein nach Wunsch Alarmcom
//	//	- das Videoausgang-Menü			doch rein nach Wunsch Alarmcom
	//	- der Laufzeitfehlerdialog
	//	- Softwareupdate
	//  - Backup (incl. Multibackup) auf CD-R
	
	if (theApp.IsReceiver())
	{
		m_OSDConfig.RemoveMenuTitle(IDOSD_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenuTitle(IDOSD_STOP_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_MULTY_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenuTitle(IDOSD_TCPIP);	
//		m_OSDConfig.RemoveMenuTitle(IDOSD_SEQUENCER_DWELLTIME);	
//		m_OSDConfig.RemoveMenuTitle(IDOSD_VIDEO_OUT_MODE_PORT_1);	
		m_OSDConfig.RemoveMenu(IDOSD_OPTION_RTE);	
		m_OSDConfig.RemoveMenu(IDOSD_OPTION_UPDATE);
	}

	//wenn Backup mit ACDC möglich, dann in Abhängigkeit, ob ein Brenner 
	//installiert ist (m_bIsACDCBackupDrive == TRUE) das gesamte Backup Menü disablen
	if (   theApp.CanBackupWithACDC() 
		&& !theApp.IsACDCBackupDrive())
	{
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_MULTY_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_STOP_ACTIVITY_BACKUP);
	}

	// Unter WindowsXP muß die Backupfunktionalität durch die JaCob/SaVic erlaubt sein
	if (theApp.IsWinXP() && !m_bBackUpAllowedByDongle)
	{
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_MULTY_ACTIVITY_BACKUP);
		m_OSDConfig.RemoveMenu(IDOSD_STOP_ACTIVITY_BACKUP);
	}
	

	// Dem Sender fehlt:
//	//	- der Akustische Alarm
	if (theApp.IsTransmitter())
	{
//		m_OSDConfig.RemoveMenu(IDOSD_ACOUSTIC_ALARM);	
	}

	// in Read Only kein Konfig
	if (theApp.IsReadOnly())
	{
		m_OSDNormal.RemoveMenu(IDOSD_CONFIG_PIN);
		m_OSDNormal.RemoveMenu(IDOSD_OPTION_LOCK);
	
	}

	//in ReadOnly, im Empfänger 
	//Import/Export Funktion der Einstellungen entfernen
	if(theApp.IsReadOnly() || theApp.IsReceiver())
	{
		m_OSDConfig.RemoveMenu(IDOSD_IMPORT_SETTINGS);
		m_OSDConfig.RemoveMenu(IDOSD_EXPORT_SETTINGS);
		m_OSDConfig.RemoveMenu(IDOSD_MAINTENANCE);
	}

	//Ist Audio vorhanden und kann es benutzt werden?
	if (!theApp.IsAudioAllowed())
	{
		m_OSDConfig.RemoveMenu(IDOSD_AUDIO_CHANGE_CAMNR);
		m_OSDConfig.RemoveMenu(IDOSD_AUDIO_THRESHOLD);
		m_OSDConfig.RemoveMenu(IDOSD_AUDIO_SENSITIVITY);
		m_OSDConfig.RemoveMenu(IDOSD_AUDIO_VOLUME);
		m_OSDConfig.RemoveMenu(IDOSD_AUDIO_RECORD_MODE);
	}

	// Menüeintrag "Bildrate" nur lokal bei Tasha anzeigen
	//im Receiver nur anzeigen, wenn der Sender eine Tasha hat
/*	if  (theApp.IsTransmitter() 
		 && !(WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT1)))
	{
		m_OSDConfig.RemoveMenu(IDOSD_CAMERA_FPS);
	}
*/
	if (theApp.IsTransmitter()  || theApp.IsReceiver())
	{
		int nCardType = CARD_TYPE_UNKNOWN;
		CServer* pServer = theApp.GetServer();
		if (pServer && pServer->GetCardType(m_nCamNr, nCardType))
		{
			if ((nCardType != CARD_TYPE_TASHA) && (nCardType != CARD_TYPE_Q))			
			{ 
				m_OSDConfig.RemoveMenu(IDOSD_CAMERA_FPS);
			}

			if (nCardType != CARD_TYPE_Q)
			{
				//diese Dinge sind nur bei der Q Unit einstellbar
				m_OSDConfig.RemoveMenu(IDOSD_SYSTEM_AGC);
				m_OSDConfig.RemoveMenu(IDOSD_SYSTEM_VIDEOFORMAT);
				m_OSDConfig.RemoveMenu(IDOSD_SYSTEM_VIDEONORM);
			}
		}
	}

	// in DEBUG geht das dann immer!
#ifndef _DEBUG
	// Kann das Backup fortgesetzt werden?
	if (theApp.GetServer() && !theApp.GetServer()->CanMultiCDExport())
		m_OSDConfig.RemoveMenu(IDOSD_MULTY_ACTIVITY_BACKUP);

		// Kann die permanente Maske editiert werden?
	if (!theApp.CanActivityMask())
		m_OSDConfig.RemoveMenu(IDOSD_MD_DRAW_MASK);


	// Kann der externe Alarmausgang aktiviert/deaktiviert werden?
	if (!theApp.CanAlarmOutput())
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_OUTPUT_ON_OFF);
	
	// Können Alarmanrufe gemacht werden?
	if (!theApp.CanAlarmCall())
	{
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALLNUMBER1);
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALLNUMBER2);
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALLNUMBER3);
		m_OSDConfig.RemoveMenu(IDOSD_ALARM_CALL_EVENT);

	}

	// Besitzt die Softwareversion schon die Freischaltfuktion?
	if (!theApp.HasExpanionCode())
		m_OSDConfig.RemoveMenu(IDOSD_EXPANSION_CODE);

#endif
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::LoadMenus()
{
	LoadNormalMenu();
	LoadConfigMenu();
	// Wurde aus dem Konfigurationsmenu heraus aufgerufen
//ML  16.01.2003	m_pOSD = &m_OSDConfig;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanel)
	DDX_Control(pDX, IDC_BUTTON_PRINT, m_ctrlButtonPrint);
	DDX_Control(pDX, IDC_BUTTON_EXPORT, m_ctrlButtonExport);
	DDX_Control(pDX, IDC_BUTTON_SKIP_FORWARD, m_ctrlButtonSkipForward);
	DDX_Control(pDX, IDC_BUTTON_SKIP_BACK, m_ctrlButtonSkipBack);
	DDX_Control(pDX, IDC_BUTTON_SINGLE_FORWARD, m_ctrlButtonSingleForward);
	DDX_Control(pDX, IDC_BUTTON_SINGLE_BACK, m_ctrlButtonSingleBack);
	DDX_Control(pDX, IDC_BUTTON_PLAY_BACK, m_ctrlButtonPlayBack);
	DDX_Control(pDX, IDC_BUTTON_MINIMIZE, m_ctrlButtonMinimize);
	DDX_Control(pDX, IDC_CROSS_SET, m_ctrlCrossSet);
	DDX_Control(pDX, IDC_CROSS_UP, m_ctrlCrossUp);
	DDX_Control(pDX, IDC_CROSS_RIGHT, m_ctrlCrossRight);
	DDX_Control(pDX, IDC_CROSS_LEFT, m_ctrlCrossLeft);
	DDX_Control(pDX, IDC_CROSS_DOWN, m_ctrlCrossDown);
	DDX_Control(pDX, IDC_BUTTON_EJECT, m_ctrlButtonEject);
	DDX_Control(pDX, IDC_BUTTON_REWIND, m_ctrlButtonFastBack);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_ctrlButtonPlayForward);
	DDX_Control(pDX, IDC_BUTTON_FF, m_ctrlButtonFastForward);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_ctrlButtonSearch);
	DDX_Control(pDX, IDC_BUTTON_OK, m_ctrlButtonOk);
	DDX_Control(pDX, IDC_BUTTON_NUM9, m_ctrlButtonNum9);
	DDX_Control(pDX, IDC_BUTTON_NUM8, m_ctrlButtonNum8);
	DDX_Control(pDX, IDC_BUTTON_NUM7, m_ctrlButtonNum7);
	DDX_Control(pDX, IDC_BUTTON_NUM6, m_ctrlButtonNum6);
	DDX_Control(pDX, IDC_BUTTON_NUM5, m_ctrlButtonNum5);
	DDX_Control(pDX, IDC_BUTTON_NUM4, m_ctrlButtonNum4);
	DDX_Control(pDX, IDC_BUTTON_NUM3, m_ctrlButtonNum3);
	DDX_Control(pDX, IDC_BUTTON_NUM2, m_ctrlButtonNum2);
	DDX_Control(pDX, IDC_BUTTON_NUM1, m_ctrlButtonNum1);
	DDX_Control(pDX, IDC_BUTTON_NUM0, m_ctrlButtonNum0);
	DDX_Control(pDX, IDC_BUTTON_NUMPLUS, m_ctrlButtonPlus);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_ctrlButtonClear);
	DDX_Control(pDX, IDC_DISPLAY_MENU1, m_ctrlDisplayMenu1);
	DDX_Control(pDX, IDC_DISPLAY_MENU2, m_ctrlDisplayMenu2);
	DDX_Control(pDX, IDC_DISPLAY_CAMERA, m_ctrlDisplayCamera);
	DDX_Control(pDX, IDC_DISPLAY_TOOLTIP, m_ctrlDisplayTooltip);
	DDX_Control(pDX, IDC_DISPLAY_TIME, m_ctrlDisplayDateTime);
	DDX_Control(pDX, IDC_BORDER1, m_ctrlBorder1);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPanel, CTransparentDialog)
	//{{AFX_MSG_MAP(CPanel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_NUM1, OnButtonNum1)
	ON_BN_CLICKED(IDC_BUTTON_NUM2, OnButtonNum2)
	ON_BN_CLICKED(IDC_BUTTON_NUM3, OnButtonNum3)
	ON_BN_CLICKED(IDC_BUTTON_NUM4, OnButtonNum4)
	ON_BN_CLICKED(IDC_BUTTON_NUM5, OnButtonNum5)
	ON_BN_CLICKED(IDC_BUTTON_NUM6, OnButtonNum6)
	ON_BN_CLICKED(IDC_BUTTON_NUM7, OnButtonNum7)
	ON_BN_CLICKED(IDC_BUTTON_NUM8, OnButtonNum8)
	ON_BN_CLICKED(IDC_BUTTON_NUM9, OnButtonNum9)
	ON_BN_CLICKED(IDC_BUTTON_FF, OnButtonFF)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_REWIND, OnButtonRewind)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_EJECT, OnButtonEject)
	ON_BN_CLICKED(IDC_BUTTON_NUM0, OnButtonNum0)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_NUMPLUS, OnButtonPlus)
	ON_BN_CLICKED(IDC_CROSS_UP, OnCrossUp)
	ON_BN_CLICKED(IDC_CROSS_DOWN, OnCrossDown)
	ON_BN_CLICKED(IDC_CROSS_LEFT, OnCrossLeft)
	ON_BN_CLICKED(IDC_CROSS_RIGHT, OnCrossRight)
	ON_BN_CLICKED(IDC_CROSS_SET, OnCrossSet)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_MINIMIZE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_FORWARD, OnButtonSingleForward)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_BACK, OnButtonSingleBack)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_BACK, OnButtonPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_SKIP_BACK, OnButtonSkipBack)
	ON_BN_CLICKED(IDC_BUTTON_SKIP_FORWARD, OnButtonSkipForward)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_WM_ACTIVATE()
	ON_MESSAGE(WM_UPDATE_BUTTONS,OnUpdateButtons)
	ON_MESSAGE(WM_UPDATE_MENU,OnUpdateMenu)
	ON_MESSAGE(WM_SET_CAMERA, OnSetCamera)
	ON_MESSAGE(WM_SET_VIEWMODE, OnSetViewMode)
	ON_MESSAGE(WM_TOGGLE_VIEWMODE, OnToggleViewMode)
	ON_MESSAGE(WM_NOTIFY_CONNECT, OnNotifyConnect)
	ON_MESSAGE(WM_NOTIFY_CIPC_CONNECT, OnNotifyCIPCConnect)
	ON_MESSAGE(WM_EXIT, OnExit)
	ON_MESSAGE(WM_NOTIFY_DISCONNECT, OnNotifyDisconnect)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_SHUTDOWN, OnShutDown)
	ON_MESSAGE(WM_REBOOT, OnReboot)
	ON_MESSAGE(WM_POWEROFF_PRESSED, OnPowerOffButtonPressed)
	ON_MESSAGE(WM_RESET_PRESSED, OnResetButtonPressed)
	ON_MESSAGE(WM_ACTIVATE_PANEL, OnActivatePanel)
	ON_MESSAGE(WM_NOTIFY_CALCULATE_END, OnNotifyCalculateEnd)
	ON_MESSAGE(WM_NOTIFY_FOUND_MEDIUM, OnNotifyFoundMedium)
	ON_MESSAGE(WM_NOTIFY_DRIVE, OnNotifyDrive)
	ON_CONTROL(20, IDC_BUTTON_PRINT, OnRclickPrint)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CPanel message handlers
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonOk() 
{
	m_dwLastUserAction = GetTickCount();
	DoShutdown();
}
void CPanel::SetImageList(CSkinButton&btn, int cx, UINT nID)
{
	CImageList *pIL = new CImageList;
	if (pIL->Create(nID, cx, 0, SKIN_COLOR_KEY))
	{
		btn.SetImageList(pIL, true);
	}
	else
	{
		delete pIL;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	{
		CDC* pDC = CDC::FromHandle(::GetDC(m_hWnd));
		::ReleaseDC(m_hWnd, pDC->m_hDC);
	}

	CreateTransparent(StyleBackGroundColorChangeBrushed, m_BaseColorBackGrounds);

	//------------------------------------------------------------
	// OEM Spezifische Einstellungen
	UINT nIDminimize = 0;
	switch (m_eOem)
	{
		case OemDTS:		nIDminimize = IDB_MINIMIZE;		break;
		case OemAlarmCom:	nIDminimize = IDB_MINIMIZE_AC;	break;
		case OemDResearch:	nIDminimize = IDB_MINIMIZE;		break;
		case OemSantec:		nIDminimize = IDB_MINIMIZE;		break;
		case OemPOne:		nIDminimize = IDB_MINIMIZE_PO;	break;
		default:
			nIDminimize = IDB_MINIMIZE;
			WK_TRACE_WARNING(_T("No OEM specified (%d)\n"), m_eOem);
		break;
	}

	SetImageList(m_ctrlButtonEject, 16, IDB_EJECT2);
	SetImageList(m_ctrlButtonSearch, 16, IDB_SEARCH);
	SetImageList(m_ctrlButtonFastBack, 20, IDB_REW2);
	SetImageList(m_ctrlButtonFastForward, 20, IDB_FF2);
	SetImageList(m_ctrlButtonPlayForward, 16, IDB_PLAY2);
	SetImageList(m_ctrlButtonPlayBack, 16, IDB_BACK2);
	SetImageList(m_ctrlButtonSingleForward, 16, IDB_SINGLE_FORWARD2);
	SetImageList(m_ctrlButtonSingleBack, 16, IDB_SINGLE_BACK2);
	SetImageList(m_ctrlButtonSkipForward, 17, IDB_SKIP_FORWARD2);
	SetImageList(m_ctrlButtonSkipBack, 16, IDB_SKIP_BACK2);
	SetImageList(m_ctrlButtonExport, 21, IDB_EXPORT);
	SetImageList(m_ctrlButtonPrint, 24, IDB_PRINT);
	SetImageList(m_ctrlButtonMinimize, 16, nIDminimize);

	m_ctrlButtonSingleForward.EnableAutoRepeat();
	m_ctrlButtonSingleBack.EnableAutoRepeat();
	m_ctrlButtonSkipForward.EnableAutoRepeat();
	m_ctrlButtonSkipBack.EnableAutoRepeat();

	// Diese Tasten rasten ein
	m_ctrlButtonPlayForward.SetStyle(StyleCheck);
	m_ctrlButtonPlayBack.SetStyle(StyleCheck);
	m_ctrlButtonFastForward.SetStyle(StyleCheck);
	m_ctrlButtonFastBack.SetStyle(StyleCheck);

	m_Buttons.Add(&m_ctrlButtonEject);
	m_Buttons.Add(&m_ctrlButtonEject);
	m_Buttons.Add(&m_ctrlButtonSearch);
	m_Buttons.Add(&m_ctrlButtonFastBack);
	m_Buttons.Add(&m_ctrlButtonFastForward);
	m_Buttons.Add(&m_ctrlButtonPlayForward);
	m_Buttons.Add(&m_ctrlButtonPlayBack);
	m_Buttons.Add(&m_ctrlButtonSingleForward);
	m_Buttons.Add(&m_ctrlButtonSingleBack);
	m_Buttons.Add(&m_ctrlButtonSkipForward);
	m_Buttons.Add(&m_ctrlButtonSkipBack);
	m_Buttons.Add(&m_ctrlButtonExport);
	m_Buttons.Add(&m_ctrlButtonPrint);
	m_Buttons.Add(&m_ctrlButtonMinimize);

	//------------------------------------------------------------
	m_ctrlBorder1.CreateFrame(this);
	m_ctrlBorder1.EnableShadow(TRUE);
	
	//------------------------------------------------------------
	m_ctrlButtonNum0.SetShape(ShapeRound);
	m_ctrlButtonNum1.SetShape(ShapeRound);
	m_ctrlButtonNum2.SetShape(ShapeRound);
	m_ctrlButtonNum3.SetShape(ShapeRound);
	m_ctrlButtonNum4.SetShape(ShapeRound);
	m_ctrlButtonNum5.SetShape(ShapeRound);
	m_ctrlButtonNum6.SetShape(ShapeRound);
	m_ctrlButtonNum7.SetShape(ShapeRound);
	m_ctrlButtonNum8.SetShape(ShapeRound);
	m_ctrlButtonNum9.SetShape(ShapeRound);
	m_ctrlButtonClear.SetShape(ShapeRound);
	m_ctrlButtonPlus.SetShape(ShapeRound);

	m_Buttons.Add(&m_ctrlButtonNum0);
	m_Buttons.Add(&m_ctrlButtonNum1);
	m_Buttons.Add(&m_ctrlButtonNum2);
	m_Buttons.Add(&m_ctrlButtonNum3);
	m_Buttons.Add(&m_ctrlButtonNum4);
	m_Buttons.Add(&m_ctrlButtonNum5);
	m_Buttons.Add(&m_ctrlButtonNum6);
	m_Buttons.Add(&m_ctrlButtonNum7);
	m_Buttons.Add(&m_ctrlButtonNum8);
	m_Buttons.Add(&m_ctrlButtonNum9);
	m_Buttons.Add(&m_ctrlButtonClear);
	m_Buttons.Add(&m_ctrlButtonPlus);

	//------------------------------------------------------------
	m_ctrlDisplayMenu1.OpenStockDisplay(this, Display2);
	m_ctrlDisplayMenu2.OpenStockDisplay(this, Display2);
	m_ctrlDisplayCamera.OpenStockDisplay(this, Display2);
	m_ctrlDisplayTooltip.OpenStockDisplay(this, Display2);
	m_ctrlDisplayDateTime.OpenStockDisplay(this, Display2);

	m_ctrlDisplayTooltip.SetScrollDelay(100);

	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText = SKIN_COLOR_BLACK;
	DisplayColors.dwFlags = DSP_COL_TEXT;
	m_ctrlDisplayMenu1.SetDisplayColors(DisplayColors);
	m_ctrlDisplayMenu2.SetDisplayColors(DisplayColors);
	m_ctrlDisplayCamera.SetDisplayColors(DisplayColors);
	m_ctrlDisplayDateTime.SetDisplayColors(DisplayColors);

	DisplayColors.colText = SKIN_COLOR_DARK_RED;
	m_ctrlDisplayTooltip.SetDisplayColors(DisplayColors);

	m_ctrlDisplayMenu1.SetTextAllignment(DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	m_ctrlDisplayTooltip.SetTextAllignment(DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	m_ctrlDisplayCamera.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	m_ctrlDisplayDateTime.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	//------------------------------------------------------------

	SetImageList(m_ctrlCrossUp, 16, IDB_UP);
	SetImageList(m_ctrlCrossDown, 16, IDB_DOWN);
	SetImageList(m_ctrlCrossLeft, 16, IDB_LEFT);
	SetImageList(m_ctrlCrossRight, 16, IDB_RIGHT);
	SetImageList(m_ctrlCrossSet, 16, IDB_ENTER);

	m_Buttons.Add(&m_ctrlCrossUp);
	m_Buttons.Add(&m_ctrlCrossDown);
	m_Buttons.Add(&m_ctrlCrossLeft);
	m_Buttons.Add(&m_ctrlCrossRight);
	m_Buttons.Add(&m_ctrlCrossSet);				      
	
	m_ctrlCrossUp.EnableAutoRepeat();
	m_ctrlCrossDown.EnableAutoRepeat();
	m_ctrlCrossLeft.EnableAutoRepeat();
	m_ctrlCrossRight.EnableAutoRepeat();

	//------------------------------------------------------------
	SetImageList(m_ctrlButtonOk, 16, IDB_POWER2);
	m_ctrlButtonOk.SetBaseColor(m_BaseColorButtons);
	m_ctrlButtonOk.EnableActionOnButtonDown();

#ifndef _DEBUG
	if (!m_bShowPower)
	{
		m_ctrlButtonOk.ShowWindow(SW_HIDE);
	}
#endif

	for (int i=0;i<m_Buttons.GetSize();i++)
	{
		m_Buttons.GetAtFast(i)->EnableActionOnButtonDown();
	}

	m_Buttons.SetTextAllignment(DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);


	SetTimer(TIMER_EVENT_ONE_SECOND, 1000, NULL);

	SetBaseColor(m_BaseColorButtons);
			  
	if (theApp.IsReadOnly())
	{
		m_sHostAddress = LOCAL_LOOP_BACK;
		OnPINEntered(_T("0000"));
	}
	else
	{
		// Beim Sender nach PIN fragen, beim Empfänger nach Gegenstation fragen
		if (theApp.IsTransmitter())
		{
			m_eInputState	= InputPanelPIN;
			// AutoLogin für Testzwecke
			CWK_Profile wkp;
			CString sPin = wkp.GetString(REGKEY_DVCLIENT,_T("AutomaticTestPIN"), _T(""));
			if (   !sPin.IsEmpty()
				&& sPin.GetLength() == 4)
			{
				m_sInput = sPin.Left(4);
			}
		}
		else
		{
			m_eInputState	= InputSelectHost;
		}

		EnableInputMode();
		UpdateMenu();
	}

	// Ohne Drucker keine Druckerauswahl und kein Printbutton
	if (!IsPrinterAvailable())
	{
		m_OSDConfig.RemoveMenuTitle(IDOSD_PRINTER1);
		m_ctrlButtonPrint.ShowWindow(SW_HIDE);
	}												 

	// Ohne Wechselmedium kein Exportbutton
	m_nExportDrive = IsDriveAvailable(FALSE, m_sExportPath, m_sExportDriveName);
	m_bIsExportDrive = m_nExportDrive;
	m_ctrlButtonExport.ShowWindow(m_nExportDrive ? SW_SHOW : SW_HIDE);

	// Falls Panelgröße änderbar, sollen die Buttons auch die Höhe ändern
	StretchButtonHeight(TRUE);

	m_hCursor = theApp.m_hArrow;

	SetPanelPosition(TRUE);
	ShowWindow(SW_SHOW);

	m_ctrlBorder1.SetFocus();

	ChangeDisplayColor();

	if (theApp.ShowCPUusage() & 1)
	{
		CRect rc, rcNum;
		m_ctrlButtonNum1.GetWindowRect(&rcNum);
		ScreenToClient(&rcNum);
		m_ctrlCrossUp.GetWindowRect(&rc);
		ScreenToClient(&rc);
		int nWidth  = (rcNum.left - rc.right - 6) / 3 ;
		rc.left = rc.right + 2;
		rc.right = rc.left + nWidth;

		CSkinProgressEx*pProgress = new CSkinProgressEx;
		pProgress->Create(PBS_VERTICAL|WS_VISIBLE|WS_CHILD, rc, this, IDC_CPU_USAGE);
		pProgress->SetRange(0, 100);
		
		nWidth += 2;
		rc.OffsetRect(nWidth, 0);
		pProgress = new CSkinProgressEx;
		pProgress->Create(PBS_VERTICAL|WS_VISIBLE|WS_CHILD, rc, this, IDC_CPU_AVG_USAGE);
		pProgress->SetRange(0, 100);

		rc.OffsetRect(nWidth, 0);
		pProgress = new CSkinProgressEx;
		pProgress->Create(PBS_VERTICAL|WS_VISIBLE|WS_CHILD, rc, this, IDC_PERFORMANCE_LEVEL);
		pProgress->SetRange(0, MAX_PERFORMANCE_LEVEL);
	}
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetDisplayColor(COLORREF cr)
{
	// Hintergrundfarbe des Paneldisplays setzen
	DSPCOL DisplayColors;
	DisplayColors.colBackGround = cr;
	DisplayColors.colBorder = cr;
	DisplayColors.colShadow = RGB(max(GetRValue(cr)-30,0),max(GetGValue(cr)-30,0),max(GetBValue(cr)-30,0));
	DisplayColors.dwFlags = DSP_COL_BACKGROUNND|FRAME_COL_BORDER | DSP_COL_SHADOW;
	m_ctrlDisplayMenu1.SetDisplayColors(DisplayColors);
	m_ctrlDisplayMenu2.SetDisplayColors(DisplayColors);
	m_ctrlDisplayCamera.SetDisplayColors(DisplayColors);
	m_ctrlDisplayDateTime.SetDisplayColors(DisplayColors);
	m_ctrlDisplayTooltip.SetDisplayColors(DisplayColors);
	FRAMECOL FrameColors;
	FrameColors.dwFlags = FRAME_COL_BACKGROUNND;
	FrameColors.colBackGround = DisplayColors.colBackGround;
	m_ctrlBorder1.SetFrameColors(FrameColors);
	
	// Wenn Submenüeintrag nicht wählbar ist diesen heller anzeigen (Nur den Schatten)
	if ((m_pOSD->GetSubMenuState() & MF_DISABLED) && (m_eInputState == InputNo))
	{
		DSPCOL DisplayColors;
		DisplayColors.colText = cr;
		DisplayColors.dwFlags = DSP_COL_TEXT;
		m_ctrlDisplayMenu2.SetDisplayColors(DisplayColors);
	}
	else
	{
		DSPCOL DisplayColors;
		DisplayColors.colText = SKIN_COLOR_BLACK;
		DisplayColors.dwFlags = DSP_COL_TEXT;
		m_ctrlDisplayMenu2.SetDisplayColors(DisplayColors);
	}

	// Wenn Menüeintrag nicht wählbar ist diesen heller anzeigen (Nur den Schatten)
	if ((m_pOSD->GetMenuState() & MF_DISABLED) && (m_eInputState == InputNo))
	{
		DSPCOL DisplayColors;
		DisplayColors.colText = cr;
		DisplayColors.dwFlags = DSP_COL_TEXT;
		m_ctrlDisplayMenu1.SetDisplayColors(DisplayColors);
	}
	else
	{
		DSPCOL DisplayColors;
		DisplayColors.colText = SKIN_COLOR_BLACK;
		DisplayColors.dwFlags = DSP_COL_TEXT;
		m_ctrlDisplayMenu1.SetDisplayColors(DisplayColors);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetBaseColor(COLORREF cr)
{
	for (int i=0;i<m_Buttons.GetSize();i++)
	{
		m_Buttons.GetAtFast(i)->SetBaseColor(cr);
	}

	if (IsWindow(m_hWnd))
	{
		RedrawWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::DestroyWindow() 
{
	KillTimer(2);
	
	return CTransparentDialog::DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnTimer(UINT nIDEvent) 
{
	if (m_pControlProcess)
	{
		CWK_String *pstr = NULL;
		BOOL bHandled = TRUE;
		switch (nIDEvent)
		{
			case NET_SHELL_EVENT_START:
			if ((pstr = m_pControlProcess->FindString(_T("netsh>"), FALSE, TRUE)) != NULL)
			{
				m_pControlProcess->WriteStdIn(_T("interface ip"), TRUE);
				m_pControlProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, NET_SHELL_EVENT_IP_INT, 0);
			}break;
			case NET_SHELL_EVENT_IP_INT:
			if ((pstr = m_pControlProcess->FindString(_T("netsh interface ip>"), FALSE, TRUE)) != NULL)
			{
				m_pControlProcess->WriteStdIn(m_sNetShellCmd, TRUE);
				m_pControlProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, NET_SHELL_EVENT_CMD_SENT, 0);
			}break;
			case NET_SHELL_EVENT_CMD_SENT:
			if ((pstr = m_pControlProcess->FindString(_T("netsh interface ip>"), FALSE, TRUE)) != NULL)
			{
				WK_DELETE(pstr);
				pstr = m_pControlProcess->FindString(_T("WARNING:"), FALSE, TRUE);
				if (pstr)
				{
					WK_TRACE(_T("%s:\n"), LPCTSTR(*pstr));
				}
				WK_DELETE(pstr);
				CString sMsg = m_sNetShellCmd + _T(": ");
				while ((pstr = m_pControlProcess->GetAndRemoveOutputString()) != NULL)
				{
					sMsg += *pstr;
					delete pstr;
				}
				sMsg.Replace(_T('\r'), _T(' '));
				sMsg.Replace(_T('\n'), _T(' '));
				WK_TRACE(_T("%s:"), sMsg);
				SetTooltipText(sMsg, true);
				m_pControlProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, NET_SHELL_EVENT_STOP, 0);
				m_pControlProcess->WriteStdIn(_T("bye"), TRUE);
			}break;
			case NET_SHELL_EVENT_STOP:
				m_sNetShellCmd.Empty();
				break;

			default:
				bHandled = FALSE;
				break;
		}
		WK_DELETE(pstr);
		if (bHandled)
		{
			return ;
		}
		else if (m_sNetShellCmd.IsEmpty())
		{
			WK_DELETE(m_pControlProcess);
		}
	}
	CTransparentDialog::OnTimer(nIDEvent);
	CPoint CurPoint(0,0);
	static CPoint LastPoint(0,0);

	GetCursorPos(&CurPoint);

	if (CurPoint != LastPoint)
		m_dwLastUserAction = GetTickCount();
	
	LastPoint = CurPoint;

	// Bis zur Endgültigen Lösung, auf die Hostliste pollen
	if (m_eInputState == InputSelectHost)
	{
//		LoadHostList();
//		UpdateMenu();
	}

	if (IsWindowVisible())
	{
		if (GetTimeSpan(m_dwLastUserAction) > m_dwMinimizeTimespan)
		{
			CRect rcPanel(-1,-1,-1,-1);
			CRect rcKeyboardDlg(-1,-1,-1,-1);
			CRect rcRTEDlg(-1,-1,-1,-1);
			CRect rcExportDlg(-1,-1,-1,-1);
			CRect rcStatisticDlg(-1,-1,-1,-1);
			CRect rcAlarmlistDlg(-1,-1,-1,-1);
			CRect rcMDConfigDlg(-1,-1,-1,-1);
			CRect rcHostsDlg(-1,-1,-1,-1);
			CRect rcInfoDlg(-1,-1,-1,-1);
			CRect rcMaintenanceDlg(-1,-1,-1,-1);
			
			GetWindowRect(rcPanel);

			if (m_pKeyboardDlg)
			{
				m_pKeyboardDlg->GetWindowRect(rcKeyboardDlg);
			}
			if (m_pRTEDlg)
			{
				m_pRTEDlg->GetWindowRect(rcRTEDlg);
			}
			if (m_pStatisticDlg)
			{
				m_pStatisticDlg->GetWindowRect(rcStatisticDlg);
			}
			if (m_pAlarmlistDlg)
			{
				m_pAlarmlistDlg->GetWindowRect(rcAlarmlistDlg);
			}
			if (m_pMDConfigDlg)
			{
				m_pMDConfigDlg->GetWindowRect(rcMDConfigDlg);
			}
			if (m_pHostsDlg)
			{
				m_pHostsDlg->GetWindowRect(rcHostsDlg);
			}
			if (m_pInfoDlg)
			{
				m_pInfoDlg->GetWindowRect(rcInfoDlg);
			}
			if (m_pMaintenanceDlg)
			{
				m_pMaintenanceDlg->GetWindowRect(rcMaintenanceDlg);
			}
		
			if (!rcPanel.PtInRect(CurPoint) &&
				!rcKeyboardDlg.PtInRect(CurPoint) &&
				!rcRTEDlg.PtInRect(CurPoint) &&
				!rcExportDlg.PtInRect(CurPoint) &&
				!rcStatisticDlg.PtInRect(CurPoint) &&
				!rcAlarmlistDlg.PtInRect(CurPoint) &&
				!rcMDConfigDlg.PtInRect(CurPoint) &&
				!rcHostsDlg.PtInRect(CurPoint) &&
				!rcInfoDlg.PtInRect(CurPoint)&&
				!rcMaintenanceDlg.PtInRect(CurPoint))
			{
#ifndef _DEBUG
				MinimizeDlg();
#endif
			}
		}

		if (m_bDateTime<10)
		{
			m_bDateTime++;
		}
		else
		{
			m_bDateTime = 0;
		}

		CPlayWindow* pPlayWnd = NULL;
		CMainFrame* pMainWnd  = theApp.GetMainFrame();
		CSystemTime t;
		t.GetLocalTime();	
	
		// Bei Playwindows soll die Aufnahmezeit/datum angezeigt werden.
		if (pMainWnd)
		{
			pMainWnd->PostMessage(WM_TIMER, TIMER_EVENT_ONE_SECOND);

			pMainWnd->LockDisplayWindows(_T(__FUNCTION__));
			pPlayWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
			if (pPlayWnd)
			{
				t = pPlayWnd->GetTimeStamp();
			}
			pMainWnd->UnlockDisplayWindows();
		}
		
		if (m_bDateTime == 0)
		{
			SetDisplayText(m_ctrlDisplayDateTime, t.GetTime(), SCROLL_TEXT, 0);
		}
		else if ((m_bDateTime>0) && (m_bDateTime<7))
		{
			SetDisplayText(m_ctrlDisplayDateTime, t.GetTime(), 0, 0);

		}
		else if (m_bDateTime == 7)
		{
			CString s;
			s.Format(_T("%02d.%02d.%02d"),t.GetDay(),t.GetMonth(),t.GetYear()%100);
			SetDisplayText(m_ctrlDisplayDateTime, s, SCROLL_TEXT, 0);
		}
		else
		{
			CString s;
			s.Format(_T("%02d.%02d.%02d"),t.GetDay(),t.GetMonth(),t.GetYear()%100);
			SetDisplayText(m_ctrlDisplayDateTime, s, 0, 0);
		}
	
		// Kurze Hilfetexte anzeigen
		ShowOnlineHelp();
	}

	theApp.CalcCPUusage();		// Performance Management
	CWnd *pWnd =  CDialog::GetDlgItem(IDC_CPU_USAGE);
	if (pWnd)
	{
		pWnd->SendMessage(PBM_SETPOS, 	theApp.GetCPUusage());

		pWnd =  CDialog::GetDlgItem(IDC_CPU_AVG_USAGE);
		if (pWnd)
		{
			pWnd->SendMessage(PBM_SETPOS, 	theApp.GetAvgCPUusage());
		}
	}

	if (m_nTimerCounter++ >= 5)	// alle 5 Sekunden prüfen
	{
		theApp.CheckPerformanceLevel();
		pWnd =  CDialog::GetDlgItem(IDC_PERFORMANCE_LEVEL);
		if (pWnd)
		{
			pWnd->SendMessage(PBM_SETPOS, theApp.GetPerformanceLevel());
		}
		m_nTimerCounter=0;
	}

	// Minimierte Panel nachführen
	if (IsMinimized() &&  m_pMinimizedDlg)
		m_pMinimizedDlg->MoveToCorner();

	// Panel soll immer den Focus haben
	ActivatePanel();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonClear() 
{
	m_sInput = _T("");
	OnButtonNum(_T(""));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum0() 
{
	OnButtonNum(_T("0"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum1() 
{
	OnButtonNum(_T("1"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum2() 
{
	OnButtonNum(_T("2"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum3() 
{
	OnButtonNum(_T("3"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum4() 
{
	OnButtonNum(_T("4"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum5() 
{
	OnButtonNum(_T("5"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum6() 
{
	OnButtonNum(_T("6"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum7() 
{
	OnButtonNum(_T("7"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum8() 
{
	OnButtonNum(_T("8"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum9() 
{
	OnButtonNum(_T("9"));
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonPlus() 
{
	if (m_eInputState == InputNo)
	{
		m_eInputState = InputCamFirstDigit;
		m_sInput = _T("1");
		OnButtonNum(_T(""));
	}
	else if (m_eInputState == InputBackupCameras)
	{
		if (m_sInput.IsEmpty())
		{
			OnButtonNum(_T("1_"));
		}
		else
		{
			theApp.MessageBeep(DEFAULT_BEEP);
		}
	}
	else if (m_eInputState == InputEditAlarmDialingNumber)
	{
		// Bei der Angabe der Alarmrufnummern, soll die "1.."-Taste
		// als "."-Taste fungieren.
		OnButtonNum(_T("."));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonNum(const CString &sChar)
{
	//WK_TRACE(_T("### TKR OnButtonNum(): Char: %s  InputState: %s\n"), 
	//	sChar, NameOfEnumInputState(m_eInputState));
	CString sMsg = _T("");
	m_dwLastUserAction = GetTickCount();

	m_sInput += sChar;

	if (!m_bSetValue)
	{
		// Eingabe der Kameranummer
		if (m_eInputState == InputNo)
		{
			int nCamNr = _ttoi(sChar);
			if (ExistLiveCamera(nCamNr-1) || ExistPlayCamera(nCamNr-1))
			{
				OnSetCamera(nCamNr-1, PlayLive);
			}
			else
			{
				theApp.MessageBeep(DEFAULT_BEEP); // MB_ICONEXCLAMATION);
			}

			m_sInput = _T("");
		}									 

		// Eingabe der Kameranummern größer 10
		if (m_eInputState == InputCamFirstDigit)
		{
			// Verhindert das Anwählen der Kameras 17, 18, 19 im 16. System
			int nCamNr = _ttoi(m_sInput);
			if (ExistLiveCamera(nCamNr-1) || ExistPlayCamera(nCamNr-1))
			{
				sMsg.LoadString(IDS_CAMERA);
				
				if (SetFormatDisplayText(m_ctrlDisplayCamera, m_sInput, sMsg.Left(3) + _T(".__")) == 2)
				{															 
					if (sChar != _T(""))
					{
						m_eInputState = InputNo;
						OnSetCamera(_ttoi(m_sInput)-1, PlayLive);
						m_sInput = _T("");
					}
				}
			}
			else
			{
				theApp.MessageBeep(DEFAULT_BEEP);
				m_sInput = _T("1");
			}
		}			
	}

	// Eingabe der Pin
	if (m_eInputState == InputPanelPIN)
	{
//		SetDisplayText(m_ctrlDisplayMenu1, _T(""));
		m_sInput = m_sInput.Left(4);
		sMsg.LoadString(IDS_PIN);

		if (SetFormatDisplayText(m_ctrlDisplayMenu2, CString(_T('*'),m_sInput.GetLength()), sMsg) == 4)
			OnCrossSet();
	}

	// Eingabe der PIN für den Configdialog
	if (m_eInputState == InputConfigPIN)
	{
		m_sInput = m_sInput.Left(4);
		sMsg = m_pOSD->GetSubString();
		if (SetFormatDisplayText(m_ctrlDisplayMenu2, CString(_T('*'),m_sInput.GetLength()), sMsg) == 4)
			OnCrossSet();
	}

	// Eingabe der PIN für den Shutdown/Reset
	if ((m_eInputState == InputShutdownPIN) || (m_eInputState == InputResetPIN))
	{
		m_sInput = m_sInput.Left(4);
		sMsg.LoadString(IDS_PIN);
		if (SetFormatDisplayText(m_ctrlDisplayMenu2, CString(_T('*'),m_sInput.GetLength()), sMsg) == 4)
			OnCrossSet();
	}
	
	// Eingabe der alten PIN
	if (m_eInputState == InputOldPIN)
	{
		m_sInput = m_sInput.Left(4);
		sMsg.LoadString(IDS_OLD_PIN);
		if (SetFormatDisplayText(m_ctrlDisplayMenu2, CString(_T('*'),m_sInput.GetLength()), sMsg) == 4)
			OnCrossSet();
	}

	// Eingabe der neuen PIN
	if (m_eInputState == InputNewPIN)
	{
		m_sInput = m_sInput.Left(4);
		sMsg.LoadString(IDS_NEW_PIN);
		if (SetFormatDisplayText(m_ctrlDisplayMenu2, CString(_T('*'),m_sInput.GetLength()), sMsg) == 4)
			OnCrossSet();
	}

	// Neue PIN bestätigen
	if (m_eInputState == InputConfirmPIN)
	{
		m_sInput = m_sInput.Left(4);
		sMsg.LoadString(IDS_CONFIRM_PIN);
		if (SetFormatDisplayText(m_ctrlDisplayMenu2, CString(_T('*'),m_sInput.GetLength()), sMsg) == 4)
			OnCrossSet();
	}

	// Eingabe der PTZ Kamera ID
	if (m_eInputState == InputCameraPTZID)
	{
		if (m_sInput.GetLength() > 16) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe der PTZ Kamera ComPort
	if (m_eInputState == InputCameraPTZComPort)
	{
		if (m_sInput.GetLength() > 2) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe des Suchdatums
	if (m_eInputState == InputSearchDate)
	{
		sMsg.LoadString(IDS_SEARCH_DATE);
		if (m_sInput.IsEmpty())
		{
			CString sFormat;
			sFormat.LoadString(IDS_DATE_FORMAT);
			SetFormatDisplayText(m_ctrlDisplayMenu2, sFormat, sMsg);
			m_sSearchDate.Empty();
			m_sSearchTime.Empty();
		}
		else if (m_sInput.GetLength() > 6)
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, sMsg);
	}

	// Eingabe der Suchzeit
	if (m_eInputState == InputSearchTime)
	{
		sMsg.LoadString(IDS_SEARCH_TIME);
		if (m_sInput.IsEmpty())
		{
			CString sFormat;
			sFormat.LoadString(IDS_TIME_FORMAT_HHMM);
			SetFormatDisplayText(m_ctrlDisplayMenu2, sFormat, sMsg);
			m_sSearchTime.Empty();
		}
		else if (m_sInput.GetLength() > 4) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, sMsg);
	}

	// Eingabe des aktuellen Datums
	if (m_eInputState == InputClockDate)
	{
		if (m_sInput.GetLength() > 6) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());

	}

	// Eingabe der aktuellen Uhrzeit
	if (m_eInputState == InputClockTime)
	{
		if (m_sInput.GetLength() > 6) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe des Wochenkalenders
	if (m_eInputState == InputTimer)
	{
		if (m_sInput.GetLength() > 8) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe der Alarmrufnummer
	if (m_eInputState == InputEditAlarmDialingNumber)
	{
		CString sFormat;
		CString sTemp = m_sInput;
		
		// Wenn ein '.' im Inputstring ist, handelt es sich um eine IP-Adresse
		if (m_sInput.Find(_T(".")) != -1)
		{	
			sFormat.LoadString(IDS_FORMAT_IP);
			sTemp.Remove(_T('.'));
			// IP Adressen bestehen aus 12 Zeichen (Ohne die Punkte)
			if (sTemp.GetLength() > 12)
			{
				m_sInput = sChar;	// Neue Eingabe beginnen
				sTemp	= m_sInput;
				sFormat.LoadString(IDS_FORMAT_TEL);
			}
		}
		else // Telefonnummer
		{
			sFormat.LoadString(IDS_FORMAT_TEL);
			// Telephonnummern bestehen aus max 16 Zeichen
			if (m_sInput.GetLength() > 16) 
			{
				m_sInput = sChar;	// Neue Eingabe beginnen
				sTemp	= m_sInput;
			}	
		}
		
		SetFormatDisplayText(m_ctrlDisplayMenu2, sTemp, m_pOSD->GetSubString()+sFormat);
	}

	// Eingabe der IP-Adresse
	if (m_eInputState == InputIP)
	{
		if (m_sInput.GetLength() > 12) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe der SubnetMask
	if (m_eInputState == InputSubNetMask)
	{
		if (m_sInput.GetLength() > 12) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe der Gateway-Adresse
	if (m_eInputState == InputGateway)
	{
		if (m_sInput.GetLength() > 12) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe der ISDN-MSN
	if (m_eInputState == InputMSN)
	{
		if (m_sInput.GetLength() > 11) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
	}

	// Eingabe StartDatum Backup
	if (m_eInputState == InputBackupStartDateTime)
	{
		if (m_sInput.GetLength() > 12) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, sMsg);
	}

	// Eingabe EndDatum Backup
	if (m_eInputState == InputBackupEndDateTime)
	{
		if (m_sInput.GetLength() > 12) 
			m_sInput = sChar;	// Neue Eingabe beginnen
		sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, sMsg);
	}

	// Eingabe Kamera Nr. Backup
	if (m_eInputState == InputBackupCameras)
	{
		//WK_TRACE(_T("Input <%s>\n"),m_sInput);
		if (m_sInput.IsEmpty())
		{
			m_sBackupCameras.Empty();
			sMsg = _T("Nr.:_");
		}
		else
		{
			if (m_sInput == _T("1_"))
			{
				m_sInput = _T("1");
				sMsg = _T("Nr.:") + m_sBackupCameras.Right(22) + _T("1_");
			}
			else
			{
				WORD wCameraNr = (WORD)(_ttoi(m_sInput)-1);

				BOOL bCamHasDBimages = TRUE;

				if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
				{
					//prüfe, ob es zur gewählten Kamera wirklich DB-Bilder gibt
					CString sCam;
					CSystemTime st;
					st.GetLocalTime();
					CSystemTime stFirstCamImage(st);
//					sCam.Format(_T(",%i"), wCameraNr+1);
					sCam.Format(_T("%i"), wCameraNr+1);

					CMainFrame* pMF = theApp.GetMainFrame();
					CServer* pServer = theApp.GetServer();
					if (pMF && (pServer))
					{		
						CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
						if(   pBackupWnd
						   && !pBackupWnd->RequestFirstImageInDB(sCam,stFirstCamImage))
						{
							bCamHasDBimages = FALSE;
						}
					}
				}
				else
				{
					//WK_TRACE(_T("### TKR kein Server oder CanMultiCDExport = false\n"));
				}

				//WK_TRACE(_T("### TKR bCamHasDBimages: %d  Cam: %d\n"), 
				//	bCamHasDBimages, wCameraNr);
				if (bCamHasDBimages && ExistPlayCamera(wCameraNr))
				{
					// ist die Kamera schon gewählt ?
					BOOL bFound = FALSE;
					CStringArray saCameras;
					WORD wNr;

					SplitString(m_sBackupCameras,saCameras,_T(','));

					for (int i=0;i<saCameras.GetSize();i++)
					{
						wNr = (WORD)(_ttoi(saCameras[i])-1);
						if (wNr == wCameraNr)
						{
							bFound = TRUE;
							break;
						}
					}
					if (!bFound)
					{
						m_sBackupCameras += (m_sInput + _T(","));
					}
					else
					{
						//WK_TRACE(_T("### TKR Cam: %d schon in Liste\n"),wCameraNr);
						theApp.MessageBeep(DEFAULT_BEEP);
					}
				}
				sMsg = _T("Nr.:") + m_sBackupCameras.Right(22) + _T("_");
				m_sInput.Empty();
			}
		}
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);
	}
	else
	{
		//WK_TRACE(_T("### TKR InputState war nicht InputBackupCameras\n"));
	}
}				 
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetPlayStatus(CPlayWindow::PlayStatus playstatus, int nCamNr /* -1*/)
{
	CMainFrame *pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CPlayWindow*	pPlayWnd = NULL;
		BOOL bRectAngleQuery = FALSE;

		if (nCamNr != -1)
			pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)nCamNr);
		else
			pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)m_nCamNr);


		if (pPlayWnd)
		{
			CPlayWindow::PlayStatus currentPlayStatus = pPlayWnd->GetPlayStatus();
	
			//wird selber Button erneut gedrückt, auf STOP schalten
			if (playstatus == CPlayWindow::PS_PLAY_FORWARD)
			{
				if (currentPlayStatus == CPlayWindow::PS_PLAY_FORWARD)
				playstatus = CPlayWindow::PS_STOP;
			}
			else if (playstatus == CPlayWindow::PS_PLAY_BACK)
			{
				if (currentPlayStatus == CPlayWindow::PS_PLAY_BACK)	
					playstatus = CPlayWindow::PS_STOP;
			}
			else if (playstatus == CPlayWindow::PS_FAST_FORWARD)
			{
				if (currentPlayStatus == CPlayWindow::PS_FAST_FORWARD)
					playstatus = CPlayWindow::PS_STOP;
			}
			else if (playstatus == CPlayWindow::PS_FAST_BACK)
			{
				if (currentPlayStatus == CPlayWindow::PS_FAST_BACK)
					playstatus = CPlayWindow::PS_STOP;
			}

//TODO TKR synchrone Wiedergabe
/*
			//start synchrone Wiedergabe
			if(    CanSyncPlay()
				&& !CheckNextSyncPlayWnd(playstatus, currentPlayStatus, pPlayWnd))
			{
				return FALSE;
			}
*/
			if (m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
			{
				CQueryRectTracker* pTracker = pPlayWnd->GetTracker();
				if(pTracker && !pTracker->GetRect()->IsRectEmpty())
				{
					if(currentPlayStatus == CPlayWindow::PS_QUERY)
					{
						//War PlayStatus vor Query gleich dem neuen PlayStatus 
						//(RectStatus ist nicht PS_RECT_STOP) dann Query anhalten.
						//Dieses wird durch setzten des RECT Status vorbereitet und erst nach Beendigung
						//der Query ausgeführt
						//Ist der neue PlayStatus != dem vor der Query, neuen Status zum Setzen vorbereiten

						//Suche mittels Suchbutton gestartet
						if(playstatus == CPlayWindow::PS_QUERY_RECT)
						{
							playstatus = CPlayWindow::PS_STOP;
							pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_QUERY_RECT);
						}
						
						if(playstatus == CPlayWindow::PS_PLAY_FORWARD)
						{
							if(pPlayWnd->GetRectPlayStatus() == CPlayWindow::PS_RECT_PLAY_FORWARD)
							{
								
								playstatus = CPlayWindow::PS_STOP;
							}
							else
							{
								pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_PLAY_FORWARD);
								playstatus = CPlayWindow::PS_QUERY;
							}
						}

						if(playstatus == CPlayWindow::PS_PLAY_BACK)
						{
							if(pPlayWnd->GetRectPlayStatus() == CPlayWindow::PS_RECT_PLAY_BACK)
							{
								//den jeweiligen Abspielvorgang nach erneutem Klick auf selbigen
								//Button beenden
								playstatus = CPlayWindow::PS_STOP;
							}
							else
							{
								pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_PLAY_BACK);
								playstatus = CPlayWindow::PS_QUERY;
							}
						}

						if(playstatus == CPlayWindow::PS_SINGLE_FORWARD)
						{
							if(pPlayWnd->GetRectPlayStatus() == CPlayWindow::PS_RECT_SINGLE_FORWARD)
							{
								//den jeweiligen Abspielvorgang nach erneutem Klick auf selbigen
								//Button beenden
								playstatus = CPlayWindow::PS_STOP;
							}
							else
							{
								pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_SINGLE_FORWARD);
								playstatus = CPlayWindow::PS_QUERY;
							}
						}

						if(playstatus == CPlayWindow::PS_SINGLE_BACK)
						{
							if(pPlayWnd->GetRectPlayStatus() == CPlayWindow::PS_RECT_SINGLE_BACK)
							{
								//den jeweiligen Abspielvorgang nach erneutem Klick auf selbigen
								//Button beenden
								playstatus = CPlayWindow::PS_STOP;
							}
							else
							{
								pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_SINGLE_BACK);
								playstatus = CPlayWindow::PS_QUERY;
							}
						}
					}
					else
					{

						if(playstatus == CPlayWindow::PS_QUERY_RECT)
						{
							pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_QUERY_RECT);
						}
						
						if(playstatus == CPlayWindow::PS_PLAY_FORWARD)
						{
							pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_PLAY_FORWARD);
						}

						if(playstatus == CPlayWindow::PS_PLAY_BACK)
						{
							pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_PLAY_BACK);
						}

						if(playstatus == CPlayWindow::PS_SINGLE_FORWARD)
						{
							pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_SINGLE_FORWARD);
						}

						if(playstatus == CPlayWindow::PS_SINGLE_BACK)
						{
							pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_SINGLE_BACK);
						}
					}
				}
			}
			if (playstatus == CPlayWindow::PS_EJECT)
			{
				pPlayWnd->SetPlayStatus(playstatus);

				// PlayWindow wurde gerade beendet
				// suche nach dem ggf. naechstem PlayWindow
				CPlayWindow* pNextPlayWnd = theApp.GetMainFrame()->GetNextPlayWindow(pPlayWnd);
				if (pNextPlayWnd)
				{
					theApp.GetMainFrame()->SetDisplayActive(pNextPlayWnd);
					WORD wCam = pNextPlayWnd->GetID().GetSubID();		
					SetCamera(wCam, PlayLive);
				}
				else
					SetCamera(m_nCamNr, Live);				
				
			}
			else
			{
				// Fenster wurde gerade sichtbar -> Ans Ende des Archive springen
				if (currentPlayStatus == CPlayWindow::PS_EJECT
					&& !bRectAngleQuery)
				{
					pPlayWnd->NavigateLastPicture(playstatus);
				}
				else
				{
					pPlayWnd->SetPlayStatus(playstatus);
				}
			}

			return TRUE;
		}	
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonFF() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_FAST_FORWARD);
	
//TODO TKR Sync Abspielen
/*
	//bei synchroner Wiedergabe in OnSetPlayStatus prüfen, ob selbe Kamera abgespielt werden kann
	//oder ob Kamera gewechselt werden muss
	if(CanSyncPlay())
	{
		if(!OnSetPlayStatus(CPlayWindow::PS_FAST_FORWARD))
		{
			OnSetPlayStatus(CPlayWindow::PS_FAST_FORWARD, m_wNextSyncPlayWnd);
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_FAST_FORWARD);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonPlay() 
{		
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_PLAY_FORWARD);

//TODO TKR Sync Abspielen
	//bei synchroner Wiedergabe in OnSetPlayStatus prüfen, ob selbe Kamera abgespielt werden kann
	//oder ob Kamera gewechselt werden muss
/*
	if(CanSyncPlay())
	{
		if(!OnSetPlayStatus(CPlayWindow::PS_PLAY_FORWARD))
		{
			OnSetPlayStatus(CPlayWindow::PS_PLAY_FORWARD, m_wNextSyncPlayWnd);
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_PLAY_FORWARD);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonPlayBack() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_PLAY_BACK);

//TODO TKR Sync Abspielen
	//bei synchroner Wiedergabe in OnSetPlayStatus prüfen, ob selbe Kamera abgespielt werden kann
	//oder ob Kamera gewechselt werden muss
/*
	if(CanSyncPlay())
	{
		if(!OnSetPlayStatus(CPlayWindow::PS_PLAY_BACK))
		{
			OnSetPlayStatus(CPlayWindow::PS_PLAY_BACK, m_wNextSyncPlayWnd);
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_PLAY_BACK);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonSingleForward() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_SINGLE_FORWARD);

//TODO TKR Sync Abspielen
	//bei synchroner Wiedergabe in OnSetPlayStatus prüfen, ob selbe Kamera abgespielt werden kann
	//oder ob Kamera gewechselt werden muss
/*
	if(CanSyncPlay())
	{
		if(!OnSetPlayStatus(CPlayWindow::PS_SINGLE_FORWARD))
		{
			OnSetPlayStatus(CPlayWindow::PS_SINGLE_FORWARD, m_wNextSyncPlayWnd);
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_SINGLE_FORWARD);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonSingleBack() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_SINGLE_BACK);

//TODO TKR Sync Abspielen
	//bei synchroner Wiedergabe in OnSetPlayStatus prüfen, ob selbe Kamera abgespielt werden kann
	//oder ob Kamera gewechselt werden muss
/*
	if(CanSyncPlay())
	{
		if(!OnSetPlayStatus(CPlayWindow::PS_SINGLE_BACK))
		{
			OnSetPlayStatus(CPlayWindow::PS_SINGLE_BACK, m_wNextSyncPlayWnd);
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_SINGLE_BACK);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonRewind() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_FAST_BACK);

//TODO TKR Sync Abspielen
	//bei synchroner Wiedergabe in OnSetPlayStatus prüfen, ob selbe Kamera abgespielt werden kann
	//oder ob Kamera gewechselt werden muss
/*
	if(CanSyncPlay())
	{
		if(!OnSetPlayStatus(CPlayWindow::PS_FAST_BACK))
		{
			OnSetPlayStatus(CPlayWindow::PS_FAST_BACK, m_wNextSyncPlayWnd);
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_FAST_BACK);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonSkipBack() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_SKIP_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonSkipForward() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_SKIP_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonStop() 
{
	m_dwLastUserAction = GetTickCount();
	OnSetPlayStatus(CPlayWindow::PS_STOP);
	UpdateButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonEject() 
{
	m_dwLastUserAction = GetTickCount();
	if (   theApp.IsReadOnly()
		|| theApp.IsReceiver())
	{
		//on select remote host from hostlist of input panel pin
		if( theApp.IsReadOnly()  
			|| m_eInputState == InputSelectHost
			|| m_eInputState == InputPanelPIN)
		{
			DoShutdown();
		}
		else
		{
			//if a database window is open, close it
			BOOL bIsDBWindow = FALSE;
			CMainFrame* pMainWnd = theApp.GetMainFrame();
			if (pMainWnd)
			{
				CPlayWindow* pPlayWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
				if (pPlayWnd)
				{
					if (pPlayWnd->IsWindowVisible())
					{
						bIsDBWindow = TRUE;
						OnSetPlayStatus(CPlayWindow::PS_EJECT);
						DisableInputMode();
						OnUpdateMenu(0,0);
					}
				}
			}

			//no database window was open, so exit menu or optionsmenu
			if(!bIsDBWindow)
			{
				//in welchem menü befinde ich mich gerade? dann dieses schließen
				if(IsNormalMenu())
				{
					//disconnect from transmitter
					OnDisconnect();
				}
				else if(IsConfigMenu())
				{
					//switch from config menu to normal menu
					OnEndConfig();
				}
			}
		}
	}
	else
	{
		OnSetPlayStatus(CPlayWindow::PS_EJECT);
		DisableInputMode();
		OnUpdateMenu(0,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonSearch() 
{
	CString sMsg,sFmt;
	EnableInputMode();
	int nDa,nMo,nYe;
	int nHo,nMi,nSe;

	// Menü grafisch als enable darstellen
	DSPCOL DisplayColors;
	DisplayColors.colText = SKIN_COLOR_BLACK;
	DisplayColors.dwFlags = DSP_COL_TEXT;
	m_ctrlDisplayMenu2.SetDisplayColors(DisplayColors);
	m_ctrlDisplayMenu1.SetDisplayColors(DisplayColors);
	m_ctrlCrossSet.EnableWindow(TRUE);

	if (m_bSetValue)
	{
		if (m_eInputState == InputNo)
		{
			sMsg.LoadString(IDC_BUTTON_SEARCH);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputSearchDate;
			sMsg.LoadString(IDS_SEARCH_DATE);
			if (m_sSearchDate.IsEmpty())
			{
				m_sInput = _T("");
				sFmt.LoadString(IDS_DATE_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
			}
			else
			{
				if(theApp.TKR_Trace())
				{
					TRACE(_T("TKR******* OnButtonSearch(): SearchDate = %s\n"), m_sSearchDate);
				}
				m_sInput = m_sSearchDate;
				SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, sMsg);
			}
			UpdateButtons();
		}
		else if (m_eInputState == InputSearchDate)
		{
			m_sSearchDate = m_sInput;	
			if (   m_sSearchDate.IsEmpty() 
				|| (   ExtractAndValidateDate(m_sSearchDate, nDa, nMo, nYe)
					&& !IsDateTimeInFuture(nDa,nMo,nYe,0,0,0))
			   )
			{
				m_eInputState = InputSearchTime;
				sMsg.LoadString(IDS_SEARCH_TIME);
				if (m_sSearchTime.IsEmpty())
				{
					m_sInput = _T("");
					sFmt.LoadString(IDS_TIME_FORMAT_HHMM);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				}
				else
				{
					m_sInput = m_sSearchTime;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, sMsg);
				}
			}
			else
			{
				sMsg.LoadString(IDS_SEARCH_DATE_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_SEARCH_DATE);
				sFmt.LoadString(IDS_DATE_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				m_sInput = _T("");
			}
		}	
		else if (m_eInputState == InputSearchTime)
		{
			m_sSearchTime	= m_sInput;
			ExtractAndValidateDate(m_sSearchDate, nDa, nMo, nYe);
	
			if (   m_sSearchTime.IsEmpty() 
				|| (   ExtractAndValidateTime(m_sSearchTime, nHo, nMi, nSe))
					&& !IsDateTimeInFuture(nDa,nMo,nYe,nHo,nMi,nSe))
			{

				// Datenbanksuche starten
				if (OnStartDatabaseSearch(m_sSearchDate, m_sSearchTime, m_nCamNr))
				{	  
					sMsg.LoadString(IDS_SEARCH_START);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					m_eInputState	= InputNo;
					DisableInputMode();
					OnUpdateMenu(0,0);
				}
				else
				{
					sMsg.LoadString(IDS_SEARCH_ERROR);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadErrorMessage();
					sMsg.LoadString(IDS_SEARCH_DATE);
					sFmt.LoadString(IDS_DATE_FORMAT);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
					m_eInputState	= InputSearchDate;
					m_sInput = _T("");
				}
			}
			else
			{
				sMsg.LoadString(IDS_SEARCH_TIME_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_SEARCH_TIME);
				sFmt.LoadString(IDS_TIME_FORMAT_HHMM);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				m_eInputState	= InputSearchTime;
				m_sInput = _T("");
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::Create	()
{
	if (!CTransparentDialog::Create(IDD))
	{
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnMenuDown()
{
	m_dwLastUserAction = GetTickCount();

	if (m_bSetValue && m_eInputState == InputNo)
	{
		switch (m_pOSD->GetMenuID())
		{
			case IDOSD_COLOR_BRIGHTNESS:
				SetBrightness(m_nOldBrightess);
				break;
			case IDOSD_COLOR_CONTRAST:
				SetContrast(m_nOldContrast);
				break;
			case IDOSD_COLOR_SATURATION:
				SetSaturation(m_nOldSaturation);
				break;
				
			case IDOSD_AUDIO_THRESHOLD:
			case IDOSD_AUDIO_SENSITIVITY:
			case IDOSD_AUDIO_VOLUME:
				OnChangeAudio(m_pOSD->GetMenuID(), 0);
				break;
		}
	}

	DisableInputMode(TRUE);
	
	if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
	{
		//Backup Object löschen
		OnDeleteBackup();
	}

	m_pOSD->DecMenuPos();
	SetDefaultSubMenu();

	OnUpdateMenu(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnMenuUp()
{
	m_dwLastUserAction = GetTickCount();
	
	if (m_bSetValue && m_eInputState == InputNo)
	{			
		switch (m_pOSD->GetMenuID())
		{
			case IDOSD_COLOR_BRIGHTNESS:
				SetBrightness(m_nOldBrightess);
				break;
			case IDOSD_COLOR_CONTRAST:
				SetContrast(m_nOldContrast);
				break;
			case IDOSD_COLOR_SATURATION:
				SetSaturation(m_nOldSaturation);
				break;

			case IDOSD_AUDIO_THRESHOLD:
			case IDOSD_AUDIO_SENSITIVITY:
			case IDOSD_AUDIO_VOLUME:
				OnChangeAudio(m_pOSD->GetMenuID(), 0);
				break;
		}
	}

	DisableInputMode(TRUE);

	if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
	{
		//Backup Object löschen
		OnDeleteBackup();
	}

	m_pOSD->IncMenuPos();
	SetDefaultSubMenu();

	OnUpdateMenu(0,1);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetDefaultSubMenu()
{					  
	switch (m_pOSD->GetMenuID())
	{
		case IDOSD_COLOR_BRIGHTNESS:
		case IDOSD_COLOR_CONTRAST:
		case IDOSD_COLOR_SATURATION:
			m_pOSD->SetSubMenuPos(IDOSD_COLOR_BRIGHTNESS,
								  m_pOSD->FindSubMenuPos(IDOSD_COLOR_BRIGHTNESS));
			break;
		case IDOSD_MD_ALARM_SENSITIVITY:
		case IDOSD_MD_MASK_SENSITIVITY:
			m_pOSD->SetSubMenuPos(IDOSD_MD_ALARM_SENSITIVITY,
								  m_pOSD->FindSubMenuPos(IDOSD_MD_ALARM_SENSITIVITY));
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnItemDown()
{									  
	m_dwLastUserAction = GetTickCount();

	CheckExistingCameras();
	if (!m_bSetValue)
	{
		m_pOSD->IncSubMenuPos();

		//TKR 29.07.03 no longer switch to next LiveCam, because changing CamName and DwellTime
		//has changed in the menu structur
/*
		if (   m_pOSD->GetMenuID() == IDOSD_CHANGE_CAMNAME
			|| m_pOSD->GetMenuID() == IDOSD_SEQUENCER_DWELLTIME
		   )
		{
			OnSwitchToNextLiveCamera();
		}
*/
	}
	else
	{
		if (m_eInputState == InputSelectHost)
		{
			OnIncrementHost();
		}
		else if (    m_eInputState == InputBackupEraseMediumConfirmingYes
				  || m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			//Umschalten zwischen CD-RW löschen "ja" und "nein"
			if(m_eInputState == InputBackupEraseMediumConfirmingYes)
			{
				m_eInputState = InputBackupEraseMediumConfirmingNo;
			}
			else if(m_eInputState == InputBackupEraseMediumConfirmingNo)
			{
				m_eInputState = InputBackupEraseMediumConfirmingYes;
			}
		}
		else
		{
			switch (m_pOSD->GetMenuID())
			{
				case IDOSD_COLOR_BRIGHTNESS:
					OnDecrementBrightness();
					break;
				case IDOSD_COLOR_CONTRAST:
					OnDecrementContrast();
					break;
				case IDOSD_COLOR_SATURATION:
					OnDecrementSaturation();
					break;
				case IDOSD_TIMER_ONOFF:
					OnToggleTimerOnOffState();
					break;
				case IDOSD_ALARM_MODE:
					OnChangeAlarmMode();
					break;
				case IDOSD_ALARM_DELETE:
				case IDOSD_STOP_ACTIVITY_BACKUP:
					OnChangeConfirmingYesNoMode();
					break;
				case IDOSD_MD_ALARM_SENSITIVITY:
					OnDecrementAlarmSensitivity();
					break;
				case IDOSD_MD_MASK_SENSITIVITY:
					OnDecrementMaskSensitivity();
					break;
				case IDOSD_ALARM_CALL_EVENT:
					OnDecrementAlarmCallEvent();
					break;
				case IDOSD_SEQUENCER_DWELLTIME:
					OnDecrementSequencerDwellTime();
					break;
				case IDOSD_ALARM_OUTPUT_ON_OFF:
					OnToggleAlarmOutputOnOffState();
					break;
				case IDOSD_VIDEO_OUT_MODE_PORT_1:
				case IDOSD_VIDEO_OUT_MODE_PORT_2:
				case IDOSD_VIDEO_OUT_MODE_PORT_3:
				case IDOSD_VIDEO_OUT_MODE_PORT_4:
					DecrementVideoOutputMode();
					break;
				case IDOSD_TIME_ZONE:
					OnDecrementTimeZone();
					break;
				case IDOSD_NO_PRINTER:
				case IDOSD_PRINTER1:
				case IDOSD_PRINTER2:
				case IDOSD_PRINTER3:
				case IDOSD_PRINTER4:
				case IDOSD_PRINTER5:
				case IDOSD_PRINTER6:
				case IDOSD_PRINTER7:
				case IDOSD_PRINTER8:
				case IDOSD_PRINTER9:
				case IDOSD_PRINTER10:
				case IDOSD_PRINTER11:
				case IDOSD_PRINTER12:
				case IDOSD_PRINTER13:
				case IDOSD_PRINTER14:
				case IDOSD_PRINTER15:
				case IDOSD_PRINTER16:
					m_pOSD->DecSubMenuPos();
					break;
				case IDOSD_LANGUAGE1:
				case IDOSD_LANGUAGE2:
				case IDOSD_LANGUAGE3:
				case IDOSD_LANGUAGE4:
				case IDOSD_LANGUAGE5:
				case IDOSD_LANGUAGE6:
				case IDOSD_LANGUAGE7:
				case IDOSD_LANGUAGE8:
				case IDOSD_LANGUAGE9:
				case IDOSD_LANGUAGE10:
				case IDOSD_LANGUAGE11:
				case IDOSD_LANGUAGE12:
				case IDOSD_LANGUAGE13:
				case IDOSD_LANGUAGE14:
				case IDOSD_LANGUAGE15:
				case IDOSD_LANGUAGE16:
					m_pOSD->DecSubMenuPos();
					break;
				case IDOSD_OPTION_TARGET_DISPLAY:
					OnChangeTargetDisplay();
					break;
				case IDOSD_ACOUSTIC_ALARM:
					OnChangeAudibleAlarm();
					break;
				case IDOSD_SYSTEM_VIDEOFORMAT:
					OnChangeSystemVideoformat();
					break;
				case IDOSD_SYSTEM_VIDEONORM:
					OnChangeSystemVideonorm();
					break;
				case IDOSD_SYSTEM_AGC:
					OnChangeSystemAGC();
					break;
				case IDOSD_REALTIME:
					OnChangeRealtime();
					break;
				case IDOSD_CAMERA_TERMINATION:
					OnChangeCameraTermination();
					break;
				case IDOSD_CAMERA_RESOLUTION:
					{
						BOOL bIncreaseItem = FALSE;
						OnChangeCameraResolution(bIncreaseItem);
					}
					break;
				case IDOSD_CAMERA_COMPRESSION:
					{					
						BOOL bIncreaseItem = FALSE;
						OnChangeCameraCompression(bIncreaseItem);
					}
					break;
				case IDOSD_CAMERA_FPS:
					{
						BOOL bIncreaseItem = FALSE;
						OnChangeCameraFPS(bIncreaseItem);
					}
					break;
				case IDOSD_CAMERA_PTZ_TYPE:
					OnDecrementCameraPTZType();
					break;
				case IDOSD_CAMERA_PTZ_COMPORT:
					OnDecrementCameraPTZComPort();
					break;

				//Audio Settings
				case IDOSD_AUDIO_CHANGE_CAMNR:
					OnChangeAudioCamNr(DECREMENT);
					break;
				case IDOSD_AUDIO_THRESHOLD:
					OnChangeAudio(IDOSD_AUDIO_THRESHOLD, DECREMENT);
					break;	
				case IDOSD_AUDIO_SENSITIVITY:
					OnChangeAudio(IDOSD_AUDIO_SENSITIVITY, DECREMENT);
					break;
				case IDOSD_AUDIO_VOLUME:
					OnChangeAudio(IDOSD_AUDIO_VOLUME, DECREMENT);
					break;
				case IDOSD_AUDIO_RECORD_MODE:
					OnChangeAudioRecordMode(DECREMENT);
					break;
				case IDOSD_AUDIO_LISTEN_IN:
				case IDOSD_DNS:
					m_bValueOn = !m_bValueOn;
					break;
				default:
					DisableInputMode();
			}
		}
	}
	
	OnUpdateMenu(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnItemUp()
{
	m_dwLastUserAction = GetTickCount();

	CheckExistingCameras();

	if (!m_bSetValue)
	{
		m_pOSD->DecSubMenuPos();

		//TKR 29.07.03 no longer switch to next LiveCam, because changing CamName and DwellTime
		//has changed in the menu structur
/*
		if (   m_pOSD->GetMenuID() == IDOSD_CHANGE_CAMNAME
			|| m_pOSD->GetMenuID() == IDOSD_SEQUENCER_DWELLTIME
			)
		{
			OnSwitchToPrevLiveCamera();
		}
*/
	}
	else if (    m_eInputState == InputBackupEraseMediumConfirmingYes
			  || m_eInputState == InputBackupEraseMediumConfirmingNo)
	{
		//Umschalten zwischen CD-RW löschen "ja" und "nein"
		if(m_eInputState == InputBackupEraseMediumConfirmingYes)
		{
			m_eInputState = InputBackupEraseMediumConfirmingNo;
		}
		else if(m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			m_eInputState = InputBackupEraseMediumConfirmingYes;
		}
	}
 	else
	{
		if (m_eInputState == InputSelectHost)
		{
			OnDecrementHost();
		}
		else
		{
			switch (m_pOSD->GetMenuID())
			{
				case IDOSD_COLOR_BRIGHTNESS:
					OnIncrementBrightness();
					break;
				case IDOSD_COLOR_CONTRAST:
					OnIncrementContrast();
					break;
				case IDOSD_COLOR_SATURATION:
					OnIncrementSaturation();
					break;
				case IDOSD_TIMER_ONOFF:
					OnToggleTimerOnOffState();
					break;
				case IDOSD_ALARM_MODE:
					OnChangeAlarmMode();
					break;
				case IDOSD_ALARM_DELETE:
				case IDOSD_STOP_ACTIVITY_BACKUP:
					OnChangeConfirmingYesNoMode();
					break;
				case IDOSD_MD_ALARM_SENSITIVITY:
					OnIncrementAlarmSensitivity();
					break;
				case IDOSD_MD_MASK_SENSITIVITY:
					OnIncrementMaskSensitivity();
					break;
				case IDOSD_ALARM_CALL_EVENT:
					OnIncrementAlarmCallEvent();
					break;
				case IDOSD_SEQUENCER_DWELLTIME:
					OnIncrementSequencerDwellTime();
					break;
				case IDOSD_ALARM_OUTPUT_ON_OFF:
					OnToggleAlarmOutputOnOffState();
					break;
				case IDOSD_VIDEO_OUT_MODE_PORT_1:
				case IDOSD_VIDEO_OUT_MODE_PORT_2:
				case IDOSD_VIDEO_OUT_MODE_PORT_3:
				case IDOSD_VIDEO_OUT_MODE_PORT_4:
					IncrementVideoOutputMode();
					break;
				case IDOSD_TIME_ZONE:
					OnIncrementTimeZone();
					break;
				case IDOSD_NO_PRINTER:
				case IDOSD_PRINTER1:
				case IDOSD_PRINTER2:
				case IDOSD_PRINTER3:
				case IDOSD_PRINTER4:
				case IDOSD_PRINTER5:
				case IDOSD_PRINTER6:
				case IDOSD_PRINTER7:
				case IDOSD_PRINTER8:
				case IDOSD_PRINTER9:
				case IDOSD_PRINTER10:
				case IDOSD_PRINTER11:
				case IDOSD_PRINTER12:
				case IDOSD_PRINTER13:
				case IDOSD_PRINTER14:
				case IDOSD_PRINTER15:
				case IDOSD_PRINTER16:
					m_pOSD->IncSubMenuPos();
					break;
				case IDOSD_LANGUAGE1:
				case IDOSD_LANGUAGE2:
				case IDOSD_LANGUAGE3:
				case IDOSD_LANGUAGE4:
				case IDOSD_LANGUAGE5:
				case IDOSD_LANGUAGE6:
				case IDOSD_LANGUAGE7:
				case IDOSD_LANGUAGE8:
				case IDOSD_LANGUAGE9:
				case IDOSD_LANGUAGE10:
				case IDOSD_LANGUAGE11:
				case IDOSD_LANGUAGE12:
				case IDOSD_LANGUAGE13:
				case IDOSD_LANGUAGE14:
				case IDOSD_LANGUAGE15:
				case IDOSD_LANGUAGE16:
					m_pOSD->IncSubMenuPos();
					break;
				case IDOSD_OPTION_TARGET_DISPLAY:
					OnChangeTargetDisplay();
					break;
				case IDOSD_ACOUSTIC_ALARM:
					OnChangeAudibleAlarm();
					break;
				case IDOSD_SYSTEM_VIDEOFORMAT:
					OnChangeSystemVideoformat();
					break;
				case IDOSD_SYSTEM_VIDEONORM:
					OnChangeSystemVideonorm();
					break;
				case IDOSD_SYSTEM_AGC:
					OnChangeSystemAGC();
					break;
				case IDOSD_REALTIME:
					OnChangeRealtime();
					break;
				case IDOSD_CAMERA_TERMINATION:
					OnChangeCameraTermination();
					break;
				case IDOSD_CAMERA_RESOLUTION:
					{
						BOOL bIncreaseItem = TRUE;
						OnChangeCameraResolution(bIncreaseItem);
					}
					break;
				case IDOSD_CAMERA_COMPRESSION:
					{					
						BOOL bIncreaseItem = TRUE;
						OnChangeCameraCompression(bIncreaseItem);
					}
					break;
				case IDOSD_CAMERA_FPS:
					{
						BOOL bIncreaseItem = TRUE;
						OnChangeCameraFPS(bIncreaseItem);
					}
					break;
				case IDOSD_CAMERA_PTZ_TYPE:
					OnIncrementCameraPTZType();
					break;
				case IDOSD_CAMERA_PTZ_COMPORT:
					OnIncrementCameraPTZComPort();
					break;

				//Audio Settings
				case IDOSD_AUDIO_CHANGE_CAMNR:
					OnChangeAudioCamNr(INCREMENT);
					break;
				case IDOSD_AUDIO_THRESHOLD:
					OnChangeAudio(IDOSD_AUDIO_THRESHOLD, INCREMENT);
					break;	
				case IDOSD_AUDIO_SENSITIVITY:
					OnChangeAudio(IDOSD_AUDIO_SENSITIVITY, INCREMENT);
					break;
				case IDOSD_AUDIO_VOLUME:
					OnChangeAudio(IDOSD_AUDIO_VOLUME, INCREMENT);
					break;
				case IDOSD_AUDIO_RECORD_MODE:
					OnChangeAudioRecordMode(INCREMENT);
					break;
				case IDOSD_AUDIO_LISTEN_IN:
				case IDOSD_DNS:
					m_bValueOn = !m_bValueOn;
					break;
				default:
					DisableInputMode();
			}
		}
	}	


	OnUpdateMenu(0,1);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnCrossUp() 
{
	// OEM Spezifische Einstellungen
	if (m_eOem == OemSantec)
		OnMenuUp();
	else
		OnItemUp();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnCrossDown() 
{
	// OEM Spezifische Einstellungen
	if (m_eOem == OemSantec)
		OnMenuDown();
	else
		OnItemDown();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnCrossLeft() 
{
	// OEM Spezifische Einstellungen
	if (m_eOem == OemSantec)
		OnItemDown();
	else
		OnMenuDown();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnCrossRight() 
{
	// OEM Spezifische Einstellungen
	if (m_eOem == OemSantec)
		OnItemUp();
	else
		OnMenuUp();	
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnCrossSet() 
{
	CString sMsg, sFmt;
	m_dwLastUserAction = GetTickCount();

	if (m_eInputState == InputSelectHost)
	{
		OnSelectHost();
		if (m_sHostAddress == SPECIAL_HOSTIP)
		{
			OnShowHostListDlg();
			UpdateMenu();
			return;
		}
		else
		{
			m_eInputState = InputPanelPIN;
			EnableInputMode();
			UpdateMenu();
			return;
		}
	}

	if (m_eInputState == InputPanelPIN)
	{
		OnPINEntered(m_sInput);
		return;
	}

	// Shutdown/Reset-PIN eingegeben
	if ((m_eInputState == InputShutdownPIN) || (m_eInputState == InputResetPIN))
	{
		OnShutdownResetPINEntered(m_sInput);
		return;
	}

	if ((m_eInputState == InputSearchDate) || (m_eInputState == InputSearchTime))
	{
		OnButtonSearch();
		return;
	}


	//Eingabe der Start- und Endzeit einer CD-Auslagerung
	if ( (m_eInputState == InputBackupStartDateTime) 
		 || (m_eInputState == InputBackupEndDateTime)
		 || (m_eInputState == InputBackupCameras)
		 || (m_eInputState == InputBackupWaitCalculatedEnd)
		 || (m_eInputState == InputBackupAskForMedium)
		 || (m_eInputState == InputBackupWaitForMedium)
		 || (m_eInputState == InputBackupFoundMedium)
		 || (m_eInputState == InputBackupFoundNoEmptyCD)
		 || (m_eInputState == InputBackupDVDNotAllowed)
		 || (m_eInputState == InputBackupEraseMedium)
		 || (m_eInputState == InputBackupEraseMediumConfirmingYes)
		 || (m_eInputState == InputBackupEraseMediumConfirmingNo)
		 )
	{
		OnBackup(m_bMultiBackup, m_bAlarmBackup);
		return;
	}

	if (m_pOSD->GetSubMenuState() & MF_DISABLED)
		return;
	
	switch (m_pOSD->GetMenuID())
	{
		// normal OSD
		case IDOSD_CONFIG_PIN:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputCamFirstDigit)
				{
					// reset camera >10 input, cam no. display will be corrected anyway
					m_eInputState = InputNo;
				}
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputConfigPIN;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputConfigPIN)
				{
					CServer* pServer = theApp.GetServer();
					if (   pServer
						&& (m_sInput.IsEmpty() == FALSE)
						&& (m_sInput == pServer->GetSupervisorPIN())
						)
					{
						sMsg.LoadString(IDS_PIN_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						
						WaitToReadInfoMessage();
						OnBeginConfig();

						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_PIN_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						
						sMsg = m_pOSD->GetSubString();
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					}
				}
			}
			m_sInput = _T("");
			break;
		case IDOSD_OPTION_LOCK:
			OnDisconnect();
			break;

		case IDOSD_OPTION_INFO:
			OnSystemInfo();
			break;

		case IDOSD_VIEW_FULL:
			OnSetViewMode(ViewFull);
			break;
		case IDOSD_VIEW_XXX:
			OnSetViewMode(ViewXxX);
			break;
		case IDOSD_VIEW_1PX:
			OnSetViewMode(View1pX);
			break;
		case IDOSD_CAM1:
		case IDOSD_CAM2:
		case IDOSD_CAM3:
		case IDOSD_CAM4:
		case IDOSD_CAM5:
		case IDOSD_CAM6:
		case IDOSD_CAM7:
		case IDOSD_CAM8:
		case IDOSD_CAM9:
		case IDOSD_CAM10:
		case IDOSD_CAM11:
		case IDOSD_CAM12:
		case IDOSD_CAM13:
		case IDOSD_CAM14:												   
		case IDOSD_CAM15:
		case IDOSD_CAM16:
			{
				CString sCam;
				sCam.Format(_T("%d"), 1+m_pOSD->GetSubMenuPos());
				// Aufruf von 'OnSetCamera(...) über 'OnButtonNum'
				OnButtonNum(sCam);
				break;
			}
		// config OSD
		case IDOSD_COLOR_BRIGHTNESS:
			ToggleInputMode();
			if (m_bSetValue)
				GetBrightness(m_nOldBrightess);
			break;
		case IDOSD_COLOR_CONTRAST:
			ToggleInputMode();
			if (m_bSetValue)
				GetContrast(m_nOldContrast);
			break;
		case IDOSD_COLOR_SATURATION:
			ToggleInputMode();
			if (m_bSetValue)
				GetSaturation(m_nOldSaturation);
			break;
		case IDOSD_CHANGE_CAMNAME:
			EnableInputMode();
			OnChangeCameraName();
			DisableInputMode();
			break;
		case IDOSD_EXPANSION_CODE:
			EnableInputMode();
			if (OnSetExpansionCode())
			{
				sMsg.LoadString(IDS_EXPANSION_CODE_SUCCESS);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
				WaitToReadInfoMessage();
				SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				
				DisableInputMode();
			}
			else
			{
				sMsg.LoadString(IDS_EXPANSION_CODE_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
				WaitToReadErrorMessage();
				sFmt.LoadString(IDS_TIMER_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
			}
			
			break;
		case IDOSD_TIMER_MO:
		case IDOSD_TIMER_TU:
		case IDOSD_TIMER_WE:
		case IDOSD_TIMER_TH:
		case IDOSD_TIMER_FR:
		case IDOSD_TIMER_SA:
		case IDOSD_TIMER_SU:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputTimer;
					sFmt.LoadString(IDS_TIMER_FORMAT);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputTimer)
				{	
					if (OnSetTimer(m_sInput))
					{
						sMsg.LoadString(IDS_SET_TIMER_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_TIMER_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
						WaitToReadErrorMessage();
						sFmt.LoadString(IDS_TIMER_FORMAT);
						SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}	
			break;		
		case IDOSD_TIMER_ONOFF:
			ToggleInputMode();
			if (!m_bSetValue)
				SetTimerOnOffState();
			break;
		case IDOSD_ALARM_MODE:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				CMainFrame* pMainWnd = theApp.GetMainFrame();
				if (pMainWnd)
				{
					CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if (pLiveWnd)
					{
						if (m_eAlarmMode == UVVKassenConfirmingYes)
						{
							//if UVV is set, change compression of camera to "low" = COMPRESSION_1
							if(pLiveWnd->EnableUVV())
							{
								pLiveWnd->SetCompression(COMPRESSION_1);
							}
						}

						if (m_eAlarmMode == MotionDetectionConfirmingYes)
							pLiveWnd->EnableMD();				
						if (m_eAlarmMode == UVVKassen)
						{
							// Eingabe ist noch nicht beendet. Eingabe muß erst noch bestätigt werden.
							// daher zurück in den Eingabemodus.
							m_eAlarmMode = UVVKassenConfirmingNo;
							EnableInputMode();
						}
						if (m_eAlarmMode == MotionDetection)
						{
							// Eingabe ist noch nicht beendet. Eingabe muß erst noch bestätigt werden.
							// daher zurück in den Eingabemodus.
							m_eAlarmMode = MotionDetectionConfirmingNo;
							EnableInputMode();
						}
					}
				}
			}
			break;
		case IDOSD_MD_ALARM_SENSITIVITY:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				CMainFrame* pMainWnd = theApp.GetMainFrame();
				if (pMainWnd)
				{
					CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if (pLiveWnd)
						pLiveWnd->SetMDAlarmSensitivity(m_nMDAlarmLevel);
				}
			}
			break;
		case IDOSD_MD_MASK_SENSITIVITY:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				CMainFrame* pMainWnd = theApp.GetMainFrame();
				if (pMainWnd)
				{
					CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if (pLiveWnd)
						pLiveWnd->SetMDMaskSensitivity(m_nMDMaskLevel);
				}
			}
			break;
		case IDOSD_ALARM_OUTPUT_ON_OFF:
			ToggleInputMode();
			if (!m_bSetValue)
				SetAlarmOutputOnOffState();
			break;
		case IDOSD_ALARM_CALLNUMBER1:
		case IDOSD_ALARM_CALLNUMBER2:
		case IDOSD_ALARM_CALLNUMBER3:
#if(1)
			// insert also hostnames
			EnableInputMode();
			if (m_bSetValue)
			{
				CString sFormat;
				sFormat.LoadString(IDS_FORMAT_TEL);				
				m_pKeyboardDlg = new CKeyboardDlg(&m_ctrlDisplayMenu2, m_pOSD->GetSubString()+sFormat, _T(""), this);
				if (m_pKeyboardDlg)
				{
					EnablePanel(FALSE);
					m_eInputState = InputEditAlarmDialingNumber;
					m_pKeyboardDlg->m_bIPallowed = TRUE;
					m_pKeyboardDlg->m_sNotAllowedChars = _T("/\\[]:|<>+=;,? ");
					m_pKeyboardDlg->m_bOnlyAsscii = TRUE;
					m_pKeyboardDlg->DoModal();
					if (m_eInputState == InputEditAlarmDialingNumber)
					{	
						int nCallingNumber = m_pOSD->GetMenuID()-IDOSD_ALARM_CALLNUMBER1;
						nCallingNumber = max(min(nCallingNumber, 9), 0);

						if (SetAlarmDialingNumber(nCallingNumber, m_pKeyboardDlg->GetInputString()))
						{
							sMsg.LoadString(IDS_SET_ALARM_DIALING_NUMMER_SUCCESS);
							SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
							WaitToReadInfoMessage();
						}
						else
						{
							sMsg.LoadString(IDS_SET_ALARM_DIALING_NUMMER_FAILED);
							SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
							WaitToReadErrorMessage();
							sFmt.LoadString(IDS_FORMAT_TEL);
							SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString()+sFmt);
						}
					}
					WK_DELETE(m_pKeyboardDlg);
					EnablePanel(TRUE);
					OnUpdateMenu(0,0);
				}
			}
			DisableInputMode();
#else
			// old insert only numbers
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputEditAlarmDialingNumber;
					CString sFormat;
					sFormat.LoadString(IDS_FORMAT_TEL);				
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString()+sFormat);
				}
				else if (m_eInputState == InputEditAlarmDialingNumber)
				{	
					int nCallingNumber = m_pOSD->GetMenuID()-IDOSD_ALARM_CALLNUMBER1;
					nCallingNumber = max(min(nCallingNumber, 9), 0);

					if (SetAlarmDialingNumber(nCallingNumber, m_sInput))
					{
						sMsg.LoadString(IDS_SET_ALARM_DIALING_NUMMER_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
						WaitToReadInfoMessage();
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_ALARM_DIALING_NUMMER_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
						WaitToReadErrorMessage();
						sFmt.LoadString(IDS_FORMAT_TEL);
						SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString()+sFmt);
					}
				}
				m_sInput = _T("");
			}
#endif
			break;		
		case IDOSD_ALARM_CALL_EVENT:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				CMainFrame* pMainWnd = theApp.GetMainFrame();
				if (pMainWnd)
				{
					CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if (pLiveWnd)
						pLiveWnd->SetAlarmCallEvent(m_nAlarmCallEvent);
				}
			}
			break;

		case IDOSD_SEQUENCER_DWELLTIME:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				OnSetSequencerDwellTime();
			}
			break;
		case IDOSD_VIDEO_OUT_MODE_PORT_1:
		case IDOSD_VIDEO_OUT_MODE_PORT_2:
		case IDOSD_VIDEO_OUT_MODE_PORT_3:
		case IDOSD_VIDEO_OUT_MODE_PORT_4:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				SetVideoOutputMode(m_pOSD->GetSubMenuPos());
			}
			break;
		case IDOSD_ACTIVITY_BACKUP:
			OnActivityBackup();
			break;
		case IDOSD_STOP_ACTIVITY_BACKUP:
			OnStopActivityBackup();
			break;
		case IDOSD_MULTY_ACTIVITY_BACKUP:
			OnMultyActivityBackup();
			break;
		case IDOSD_ALARM_BACKUP:
			OnAlarmBackup();
			break;
		case IDOSD_ALARM_DELETE:
			OnAlarmDelete();
			break;
		case IDOSD_SUSPECT_SEARCH:
			OnSuspectSearch();
			break;
		case IDOSD_SUSPECT_DELETE:
			OnSuspectDelete();
			break;
		case IDOSD_DNS:
#ifdef _TEST_DHCP_SETTINGS
			ToggleInputMode();
			if (m_bSetValue)
			{
				m_bValueOn = m_bDHCPenabled;
			}
			else if (m_bValueOn != m_bDHCPenabled)	// something changed?
			{
				BOOL bReturn = DHCPParameters(TRUE, m_bValueOn);
				if (bReturn)
				{
					UINT nState = m_bDHCPenabled ? MF_DISABLED : MF_ENABLED;
					m_OSDConfig.EnableMenuItem(IDOSD_TCPIP     , nState);
					m_OSDConfig.EnableMenuItem(IDOSD_SUBNETMASK, nState);
					if (theApp.IsComputerNameValid() && bReturn == DO_REBOOT)
					{
						DoReboot();
					}
				}
				else
				{
					m_bDHCPenabled = m_bValueOn;	// restore the old value
					DHCPParameters(FALSE);
					sMsg.LoadString(IDS_SET_DHCP_FAILED);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadErrorMessage();
					SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
				}
			}
#endif
			break;
		case IDOSD_TCPIP:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputIP;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputIP)
				{		
					m_sIP = m_sInput;
					if (OnSetIP(m_sInput, IPADDRESS))
					{
						sMsg.LoadString(IDS_SET_IP_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);								
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_IP_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}
			break;
		case IDOSD_SUBNETMASK:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputSubNetMask;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputSubNetMask)
				{			
					m_sSubNetMask = m_sInput;
					if (OnSetIP(m_sInput, SUBNETMASK))
					{
						sMsg.LoadString(IDS_SET_SUBNETMASK_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);								
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_SUBNETMASK_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}
			break;
		case IDOSD_GATEWAY:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputGateway;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputGateway)
				{			
					if (OnSetIP(m_sInput, DEFAULTGATEWAY))
//					if (OnSetDefaultGateway(m_sInput))
					{
						sMsg.LoadString(IDS_SET_GATEWAY_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);								
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_GATEWAY_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}
			break;
		case IDOSD_ISDN_MSN:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputMSN;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else
				{
					if (OnSetMSN(m_sInput))
					{
						sMsg.LoadString(IDS_SET_MSN_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);			
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_MSN_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}
			break;
		case IDOSD_NET_NAME:
			EnableInputMode();
			OnChangeLocalHostName();
			DisableInputMode();
			break;
		case IDOSD_CLOCK_DATE:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputClockDate;
					sFmt.LoadString(IDS_DATE_FORMAT);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputClockDate)
				{	
					if (!m_sInput.IsEmpty() && OnSetClock(m_sInput, _T("")))
					{
						sMsg.LoadString(IDS_SET_CLOCK_DATE_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						DisableInputMode();
					}
					else
					{
						sMsg.LoadString(IDS_SET_CLOCK_DATE_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						sFmt.LoadString(IDS_DATE_FORMAT);
						SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}
			break;
		case IDOSD_CLOCK_TIME:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputClockTime;

					sFmt.LoadString(IDS_TIME_FORMAT_HHMMSS);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputClockTime)
				{
					if (!m_sInput.IsEmpty() && OnSetClock(_T(""), m_sInput))
					{
						sMsg.LoadString(IDS_SET_CLOCK_TIME_SUCCESS);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);				
						WaitToReadInfoMessage();
						SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
						DisableInputMode();
					}
					else
					{	
						sMsg.LoadString(IDS_SET_CLOCK_TIME_FAILED);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);						
						WaitToReadErrorMessage();
						sFmt.LoadString(IDS_TIME_FORMAT_HHMMSS);
						SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, m_pOSD->GetSubString());
					}
				}
				m_sInput = _T("");
			}
			break;
		case IDOSD_TIME_ZONE:
			ToggleInputMode();
			if (m_bSetValue)
				OnGetTimeZones();
			else
			{		
				if (OnSetTimeZone(m_nTimeZone))
				{
					sMsg.LoadString(IDS_SET_TIME_ZONE_SUCCESS);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadInfoMessage();
				}
				else
				{
					sMsg.LoadString(IDS_SET_TIME_ZONE_FAILED);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadErrorMessage();
					sFmt.LoadString(IDS_DATE_FORMAT);
				}
			}
			break;
		case IDOSD_NO_PRINTER:
		case IDOSD_PRINTER1:
		case IDOSD_PRINTER2:
		case IDOSD_PRINTER3:
		case IDOSD_PRINTER4:
		case IDOSD_PRINTER5:
		case IDOSD_PRINTER6:
		case IDOSD_PRINTER7:
		case IDOSD_PRINTER8:
		case IDOSD_PRINTER9:
		case IDOSD_PRINTER10:
		case IDOSD_PRINTER11:
		case IDOSD_PRINTER12:
		case IDOSD_PRINTER13:
		case IDOSD_PRINTER14:
		case IDOSD_PRINTER15:
		case IDOSD_PRINTER16:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				if (SetDefaultPrinter(m_pnPrinterSort[m_pOSD->GetSubMenuPos()]))
				{
					sMsg.LoadString(IDS_DEFAULTPRINTER_SET);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadInfoMessage();
				}
			}
			break;	
		case IDOSD_LANGUAGE1:
		case IDOSD_LANGUAGE2:
		case IDOSD_LANGUAGE3:
		case IDOSD_LANGUAGE4:
		case IDOSD_LANGUAGE5:
		case IDOSD_LANGUAGE6:
		case IDOSD_LANGUAGE7:
		case IDOSD_LANGUAGE8:
		case IDOSD_LANGUAGE9:
		case IDOSD_LANGUAGE10:
		case IDOSD_LANGUAGE11:
		case IDOSD_LANGUAGE12:
		case IDOSD_LANGUAGE13:
		case IDOSD_LANGUAGE14:
		case IDOSD_LANGUAGE15:
		case IDOSD_LANGUAGE16:
			ToggleInputMode();
			if (!m_bSetValue)
			{
				if (SetDefaultLanguage(m_pOSD->GetSubMenuPos()))
				{
					sMsg.LoadString(IDS_DEFAULT_LANGUAGE_SET);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadInfoMessage();
				}
			}
			break;	
		case IDOSD_CHANGE_PIN_OPERATOR:
			OnChangePIN(FALSE);
			break;
		case IDOSD_CHANGE_PIN_SUPERVISOR:
			OnChangePIN(TRUE);
			break;
		case IDOSD_OPTION_RTE:
			OnShowRTErrors();
			break;
		case IDOSD_OPTION_TARGET_DISPLAY:
			ToggleInputMode();
			if (!m_bSetValue)
				SetTargetDisplayState();
			break;
		case IDOSD_OPTION_EXIT:
			DoShutdown();
			break;
		case IDOSD_OPTION_UPDATE:
			StartUpdateThread();
			break;
		case IDOSD_OPTION_STATISTIC:
			OnShowStatistic();
			break;
		case IDOSD_SHOW_ALARMLIST:
			OnShowAlarmlist();
			break;
		case IDOSD_MD_DRAW_MASK:
			OnShowMDConfigDlg();
			break;
		case IDOSD_ACOUSTIC_ALARM:
			ToggleInputMode();
			if (!m_bSetValue)
				theApp.SetAudibleAlarm(m_bAudibleAlarm);
			break;
		case IDOSD_SYSTEM_AGC:
			ToggleInputMode();
			if (!m_bSetValue)
				SetSystemAGC();
			break;
		case IDOSD_SYSTEM_VIDEOFORMAT:
			ToggleInputMode();
			if (!m_bSetValue)
				SetSystemVideoformat();
			break;
		case IDOSD_SYSTEM_VIDEONORM:
			ToggleInputMode();
			if (!m_bSetValue)
				SetSystemVideonorm();
			break;
		case IDOSD_REALTIME:
			ToggleInputMode();
			if (!m_bSetValue)
				theApp.SetRealtime(m_bRealtimePanel);
			break;
		case IDOSD_CAMERA_TERMINATION:
			ToggleInputMode();
			if (!m_bSetValue)
				SetCameraTermination();
			break;
		case IDOSD_CAMERA_RESOLUTION:
			ToggleInputMode();
			if (!m_bSetValue)
				SetResolution(m_nResolution);
			break;
		case IDOSD_CAMERA_COMPRESSION:
			ToggleInputMode();
			if (!m_bSetValue)
				SetCompression(m_nCompression);
			break;
		case IDOSD_CAMERA_FPS:
			ToggleInputMode();
			if (!m_bSetValue)
				SetCameraFPS(m_nCameraFPS);
			break;
		case IDOSD_CAMERA_PTZ_TYPE:
			ToggleInputMode();
			if (!m_bSetValue)
				SetCameraPTZType(m_nCameraPTZType);
			break;
		case IDOSD_CAMERA_PTZ_ID:
			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputCameraPTZID;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputCameraPTZID)
				{			
					SetCameraPTZID(_ttoi(m_sInput));
				}
			}
			break;
		case IDOSD_CAMERA_PTZ_COMPORT:
			ToggleInputMode();
			if (!m_bSetValue)
				SetCameraPTZComPort(m_nCameraPTZComPort);
/*			EnableInputMode();
			if (m_bSetValue)
			{
				if (m_eInputState == InputNo)
				{
					m_eInputState = InputCameraPTZComPort;
					SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
				}
				else if (m_eInputState == InputCameraPTZComPort)
				{			
					SetCameraPTZComPort(_ttoi(m_sInput));
				}
			}
*/			break;
		case IDOSD_EXPORT_SETTINGS:
			OnExportSettings();
			break;
		case IDOSD_IMPORT_SETTINGS:
			OnImportSettings();
			break;
		case IDOSD_MAINTENANCE:
			OnMaintenance();
			break;
		case IDOSD_OPTION_END:
			OnEndConfig();
			break;
			
		//Audio implementation
		case IDOSD_AUDIO_LISTEN_IN:
		case IDOSD_AUDIO_CHANGE_CAMNR:
		case IDOSD_AUDIO_THRESHOLD:
		case IDOSD_AUDIO_SENSITIVITY:
		case IDOSD_AUDIO_VOLUME:
		case IDOSD_AUDIO_RECORD_MODE:
			ToggleInputMode();
			if (theApp.GetAudio())
			{
//TODO tkr, rke Für DTS-IP-Receiver Audio remote auf DTS einstellbar machen,
			//dazu ohne LocalAudio arbeiten
				CIPCAudioDVClient* pAudio = theApp.GetAudio();
				if (m_bSetValue)
				{
					switch (m_pOSD->GetMenuID())
					{//save old value
						case IDOSD_AUDIO_THRESHOLD:
							m_nOldAudioValue = pAudio->GetInputThreshold();
							break;
						case IDOSD_AUDIO_SENSITIVITY:
							m_nOldAudioValue = pAudio->GetInputSensitivity(); 
							break;
						case IDOSD_AUDIO_VOLUME:
							m_nOldAudioValue = pAudio->GetOutputVolume();
							break;
						case IDOSD_AUDIO_LISTEN_IN:
							m_bValueOn = pAudio->IsInputOn();
							break;
					}
				}
				else
				{
					switch (m_pOSD->GetMenuID())
					{
						case IDOSD_AUDIO_CHANGE_CAMNR:
							pAudio->SetCameraNumber(m_nAudioCameraNumber);
							break;
						case IDOSD_AUDIO_RECORD_MODE:
							pAudio->SetRecordingMode(m_nAudioRecordingMode);
							break;	
						case IDOSD_AUDIO_VOLUME:
							if (pAudio->IsOutputOn() && pAudio->GetOutputVolume() <= 0)
							{
								pAudio->SwitchOutput(CSD_OFF);
							}
							else if (!pAudio->IsOutputOn() && pAudio->GetOutputVolume() > 0)
							{
								int iOutputVolume = pAudio->GetOutputVolume();

								if (pAudio->SwitchOutput(CSD_ON) && pAudio->IsOutputOn())
								{
									pAudio->SetOutputVolume(iOutputVolume);
								}
							}
							break;
						case IDOSD_AUDIO_LISTEN_IN:
							pAudio->SwitchInput(m_bValueOn ? CSD_ON : CSD_OFF);
							break;
					}
				}
			}
			break;
	}
	UpdateMenu();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetBrightness(int nBrightness)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->SetBrightness(nBrightness);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetContrast(int nContrast)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->SetContrast(nContrast);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetSaturation(int nSaturation)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->SetSaturation(nSaturation);
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetResolution(int nResolution)
{
	BOOL bResult = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		bResult = pServer->SetResolution(nResolution);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetCompression(int nCompression)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->SetCompression(nCompression);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetCameraFPS(int nCameraFPS)
{
	BOOL bResult = FALSE;
	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->SetCameraFPS(nCameraFPS);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetCameraPTZType(int nCameraPTZType)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->SetCameraPTZType(nCameraPTZType);
		}
	}
	CString sMsg;
	if (bResult)
	{
		m_nCurrentCameraPTZType = nCameraPTZType;
//		sMsg.LoadString(IDS_SET_CAMERA_TYPE_SUCCESS);
// gf todo PTZ
		sMsg = _T("Kamera Typ gesetzt");
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
		WaitToReadInfoMessage();
		DisableInputMode();
	}
	else
	{
//		sMsg.LoadString(IDS_SET_CAMERA_TYPE_FAILED);
// gf todo PTZ
		sMsg = _T("Kamera Typ nicht gesetzt");
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);					
		WaitToReadErrorMessage();
		m_nCameraPTZType = m_nCurrentCameraPTZType;
		SetDisplayText(m_ctrlDisplayMenu2, NameOfEnumPTZRealName((CameraControlType)m_nCurrentCameraPTZType));					
		DisableInputMode();
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetCameraPTZID(DWORD dwCameraPTZID)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->SetCameraPTZID(dwCameraPTZID);
		}
	}
	CString sMsg;
	if (bResult)
	{
		m_dwCameraPTZID = dwCameraPTZID;
	//	sMsg.LoadString(IDS_SET_PTZID_FAILED);
		sMsg = _T("Camera ID gesetzt");
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);								
		WaitToReadInfoMessage();
//		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sCameraPTZID, m_pOSD->GetSubString());
		DisableInputMode();
	}
	else
	{
	//	sMsg.LoadString(IDS_SET_PTZID_FAILED);
		sMsg = _T("Camera ID nicht gesetzt");
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);
		WaitToReadErrorMessage();
		sMsg.Format(_T("%u"), m_dwCameraPTZID);
		SetFormatDisplayText(m_ctrlDisplayMenu2, sMsg, m_pOSD->GetSubString());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetCameraPTZComPort(int nCameraPTZComPort)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->SetCameraPTZComPort(nCameraPTZComPort);
		}
	}
	CString sMsg;
	if (bResult)
	{
		m_nCameraPTZComPort = nCameraPTZComPort;
	//	sMsg.LoadString(IDS_SET_PTZCOMPORT_SUCCESS);
		sMsg = _T("Com Port gesetzt");
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);								
		WaitToReadInfoMessage();
//		SetFormatDisplayText(m_ctrlDisplayMenu2, m_sInput, m_pOSD->GetSubString());
		DisableInputMode();
	}
	else
	{
	//	sMsg.LoadString(IDS_SET_PTZCOMPORT_FAILED);
		sMsg = _T("Com Port nicht gesetzt");
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);
		WaitToReadErrorMessage();
		SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetBrightness(int &nBrightness)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetBrightness(nBrightness);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetContrast(int &nContrast)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetContrast(nContrast);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetSaturation(int &nSaturation)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetSaturation(nSaturation);
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetResolution(int &nResolution)
{
	BOOL bResult = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		bResult = pServer->GetResolution(nResolution);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetCompression(int &nCompression)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetCompression(nCompression);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetCameraFPS(int &nCameraFPS)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetCameraFPS(nCameraFPS);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetCameraPTZType(int &nPTZType)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetCameraPTZType(nPTZType);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetCameraPTZID(DWORD &dwCameraPTZID)
{
	BOOL bResult = FALSE;

	if (theApp.GetMainFrame())
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
			bResult = pLiveWnd->GetCameraPTZID(dwCameraPTZID);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetCameraPTZComPort(int &nPTZComPort)
{
	BOOL bResult = FALSE;
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CLiveWindow* pLiveWnd = pMF->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->GetCameraPTZComPort(nPTZComPort);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnIncrementBrightness()
{
	BOOL bResult = FALSE;
	int nVal = 0;

	if (GetBrightness(nVal))
	{
		nVal = min(nVal+1, 100);
		bResult = SetBrightness(nVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnIncrementContrast()
{
	BOOL bResult = FALSE;
	int nVal = 0;

	if (GetContrast(nVal))
	{
		nVal = min(nVal+1, 100);
		bResult = SetContrast(nVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnIncrementSaturation()
{
	BOOL bResult = FALSE;
	int nVal = 0;

	if (GetSaturation(nVal))
	{
		nVal = min(nVal+1, 100);
		bResult = SetSaturation(nVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeAudio(int nAudioParam, int iValue)
{
	//iValue == +1  increment
	//iValue == -1  decrement
	//iValue == 0   load old value

	int nVal = 0;

	CIPCAudioDVClient* pAudio = NULL;
	pAudio = theApp.GetAudio();
//TODO tkr, rke Für DTS-IP-Receiver Audio remote auf DTS einstellbar machen,
			//dazu ohne LocalAudio arbeiten
	if(pAudio)
	{
		switch (nAudioParam)
		{
		case IDOSD_AUDIO_THRESHOLD:
			{
				if(iValue != 0)
				{
					nVal = pAudio->GetInputThreshold() + iValue;
					if (nVal > 100) nVal =   0;
					if (nVal <   0) nVal = 100;
//					if (IsBetween(nVal, 0, 100))
					{
						if(!pAudio->SetInputThreshold(nVal))
						{
							WK_TRACE_ERROR(_T("Cannot set new audio value: treshold\n"));
						}
					}
				}
				else
				{
					pAudio->SetInputThreshold(m_nOldAudioValue);
				}
			}
			break;

		case IDOSD_AUDIO_SENSITIVITY:
			{
				if(iValue != 0)
				{
					nVal = pAudio->GetInputSensitivity() + iValue;
					if (nVal > 100) nVal =   0;
					if (nVal <   0) nVal = 100;
//					if (IsBetween(nVal, 0, 100))
					{
						if(!pAudio->SetInputSensitivity(nVal))
						{
							WK_TRACE_ERROR(_T("Cannot set new audio value: sensitivity\n"));
						}
					}
				}
				else
				{
					pAudio->SetInputSensitivity(m_nOldAudioValue);
				}
			}
			break;

		case IDOSD_AUDIO_VOLUME:
			{
				if(iValue != 0)
				{
					nVal = pAudio->GetOutputVolume() + iValue;
					if (nVal > 100) nVal =   0;
					if (nVal <   0) nVal = 100;
//					if (IsBetween(nVal, 0, 100))
					{
						if(!pAudio->SetOutputVolume(nVal))
						{
							WK_TRACE_ERROR(_T("Cannot set new audio value: volume\n"));
						}
					}
				}
				else
				{
					pAudio->SetOutputVolume(m_nOldAudioValue);
				}
			}
			break;
			
		default:
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeAudioCamNr(int iValue)
{
	for (int i=0; i<m_nCameras; i++)
	{
		m_nAudioCameraNumber -= iValue;
		if (m_nAudioCameraNumber < 0)
		{
			m_nAudioCameraNumber = m_nCameras;
		}
		else if (m_nAudioCameraNumber > m_nCameras+1)
		{
			m_nAudioCameraNumber = 0;
		}
		if (m_nAudioCameraNumber == 0)
		{
			break;
		}
		else if (ExistLiveCamera(m_nAudioCameraNumber-1))
		{
			CLiveWindow* pLiveCam = ExistLiveCamera(m_nAudioCameraNumber-1);
			if(pLiveCam->IsMD()) //only MD cameras allowed, no UVV
			{
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeAudioRecordMode(int iValue)
{
	m_nAudioRecordingMode -= iValue;
	if (m_nAudioRecordingMode < AUDIO_RECORDING_OFF)
	{
		m_nAudioRecordingMode = AUDIO_RECORDING_ALE;
	}
	else if (m_nAudioRecordingMode > AUDIO_RECORDING_ALE)
	{
		m_nAudioRecordingMode = AUDIO_RECORDING_OFF;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnDecrementBrightness()
{
	BOOL bResult = FALSE;
	int nVal = 0;

	if (GetBrightness(nVal))
	{
		nVal = max(nVal-1, 0);
		bResult = SetBrightness(nVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnDecrementContrast()
{
	BOOL bResult = FALSE;
	int nVal = 0;

	if (GetContrast(nVal))
	{
		nVal = max(nVal-1, 0);
		bResult = SetContrast(nVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnDecrementSaturation()
{
	BOOL bResult = FALSE;
	int nVal = 0;

	if (GetSaturation(nVal))
	{
		nVal = max(nVal-1, 0);
		bResult = SetSaturation(nVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeAlarmMode()
{
	// Umschaltung zwischen 'Modus: UVVkassen' und 'Modus: Motiondetection:
	if (m_eAlarmMode == MotionDetection)
		m_eAlarmMode = UVVKassen;
	else if (m_eAlarmMode == UVVKassen)
		m_eAlarmMode = MotionDetection;
	// Unschaltung zwischen 'MotionDetection: Yes' und 'MotionDetection: No'
	else if (m_eAlarmMode == MotionDetectionConfirmingNo)
		m_eAlarmMode = MotionDetectionConfirmingYes;
	else if (m_eAlarmMode == MotionDetectionConfirmingYes)
		m_eAlarmMode = MotionDetectionConfirmingNo;
	// Unschaltung zwischen 'UVV-Kassen: Yes' und 'UVV-Kassen: No'
	else if (m_eAlarmMode == UVVKassenConfirmingNo)
		m_eAlarmMode = UVVKassenConfirmingYes;
	else if (m_eAlarmMode == UVVKassenConfirmingYes)
		m_eAlarmMode = UVVKassenConfirmingNo;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeConfirmingYesNoMode()
{
	//Umschalten zwischen "ja" und "nein" beim Bestätigen des Löschens von Alarmbildern
	//oder beim Abbruch eines Backups
	if(m_eConfirmingYesNoMode == ConfirmingYes)
	{
		m_eConfirmingYesNoMode = ConfirmingNo;
	}
	else if(m_eConfirmingYesNoMode == ConfirmingNo)
	{
		m_eConfirmingYesNoMode = ConfirmingYes;
	}

}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnStartDatabaseSearch(const CString &sDate, const CString &sTime, int nCamNr)
{
	BOOL bRet = FALSE;
	int nDay, nMonth, nYear, nHour, nMinute, nSecond;
	CSystemTime st;

	
	CMainFrame *pMF = theApp.GetMainFrame();
	st.GetLocalTime();
	if (pMF && sDate.IsEmpty() && sTime.IsEmpty())
	{
		CPlayWindow* pPlayWnd = pMF->GetPlayWindow((WORD)nCamNr);
		if (pPlayWnd)
		{
			if(m_bCanRectSearch && !pMF->IsInBackup())
			{
				CQueryRectTracker* pTracker = pPlayWnd->GetTracker();
				if(pTracker && !pTracker->GetRect()->IsRectEmpty())
				{
					//geh zum ersten Bild der Datenbank, die den Rechtecksuchkriterien entspricht
					CSystemTime stFirst;
					m_dwLastUserAction = GetTickCount();
					pPlayWnd->SetQueryRectSystemTime(stFirst);
					bRet = OnSetPlayStatus(CPlayWindow::PS_QUERY_RECT);
				}
				else
				{
					bRet = pPlayWnd->NavigateFirstPicture();
				}
			}
			else
			{
				bRet = pPlayWnd->NavigateFirstPicture();
			}
		}
	}
	else // (sDate.IsEmpty() && sTime.IsEmpty())
	{
		if (!ExtractAndValidateDate(sDate, nDay, nMonth, nYear))
			return FALSE;
		
		if (!ExtractAndValidateTime(sTime, nHour, nMinute, nSecond))
			return FALSE;
		
		if (IsDateTimeInFuture(nDay, nMonth, nYear, nHour, nMinute, nSecond))
			return FALSE;

		st.wYear = (WORD)nYear;
		st.wMonth = (WORD)nMonth;
		st.wDay = (WORD)nDay;

		st.wHour = (WORD)nHour;
		st.wMinute = (WORD)nMinute;
		st.wSecond = (WORD)nSecond;

		if (pMF)
		{
			CPlayWindow* pPlayWnd = pMF->GetPlayWindow((WORD)nCamNr);
			if (pPlayWnd)
			{
				if(m_bCanRectSearch && !pMF->IsInBackup())
				{
					CQueryRectTracker* pTracker = pPlayWnd->GetTracker();
					if(pTracker && !pTracker->GetRect()->IsRectEmpty())
					{
						m_dwLastUserAction = GetTickCount();
						pPlayWnd->SetQueryRectSystemTime(st);
						bRet = OnSetPlayStatus(CPlayWindow::PS_QUERY_RECT);
					}
					else
					{
						bRet = pPlayWnd->StartDatabaseSearch(st);
					}
				}
				else
				{
					bRet = pPlayWnd->StartDatabaseSearch(st);
				}
			}
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetCamera(int nCamNr, WindowType eType)
{
	PostMessage(WM_SET_CAMERA, nCamNr, eType);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnSetCamera(WPARAM wParam, LPARAM lParam)
{
	//TRACE(_T("##CPanel::OnSetCamera Cam=%d WndType=%d\n"), wParam, lParam);

	int nCamNr = wParam & (MAX_CAM-1);
	CDisplayWindow* pDisplayWindow = NULL;

	if ((nCamNr < 0) || (nCamNr >= MAX_CAM))
	{
		WK_TRACE_ERROR(_T("CPanel::OnSetCamera unknown camera (%d)\n"), nCamNr);
		return 0;
	}
	
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF == NULL)
	{
		return 0;
	}

	pMF->LockDisplayWindows(_T(__FUNCTION__));

	CLiveWindow* pLiveWnd	  = ExistLiveCamera(nCamNr);
	CPlayWindow* pPlayWnd	  = ExistPlayCamera(nCamNr);
	CDebugWindow* pDebugWnd	  = ExistDebugWindow();	//for showing maintenance text

	//					Live	Play	PlayOpen	Backup		Result
	//	---------------------------------------------------------------
	//	Live		|	1		x		x			x			Live
	//	Play		|	x		1		1			x			Play
	//	Backup		|	x		x		x			1			Backup
	//	LivePlay	|	1		x		x			x			Live
	//		"		|	0		1		1			x			Play
	//	PlayLive	|	x		1		1			x			Play
	//		"		|	1		1		0			x			Live
	//		"		|	0		1		0			x			Play
	// ---------------------------------------------------------------
	// 1 = TRUE, 0 = FALSE, x = ignore

	switch ((WindowType)lParam)
	{
		case Debug:
			if(pDebugWnd)
			{
				pDisplayWindow = pDebugWnd;
			}
			break;
		case Live:
			if (pLiveWnd)
			{
				m_nCamNr = nCamNr;
				pDisplayWindow = pLiveWnd;	// Livefenster vorhanden
			}
			break;
		case Play:
			if (pPlayWnd && pPlayWnd->IsShowable())
			{
				m_nCamNr = nCamNr;
				pDisplayWindow = pPlayWnd;	// Wiedergabefenster vorhanden UND geöffnet
			}
			break;
		case Backup:
			if (ExistBackupCamera())
			{
				pDisplayWindow = ExistBackupCamera();	// Backupfenster vorhanden
			}
			break;
		case LivePlay:
			if (pLiveWnd)
			{
				m_nCamNr = nCamNr;
				pDisplayWindow = pLiveWnd;	// Livefenster vorhanden
			}
			else if (pPlayWnd && pPlayWnd->IsShowable())
			{
				m_nCamNr = nCamNr;
				pDisplayWindow = pPlayWnd;	// Kein Livefenster, Wiedergabefenster vorhanden UND geöffnet
			}
			break;
		case PlayLive:
			if (pPlayWnd)					// Achtung das Playwindow wird nicht wieder gelöscht
			{
				pDisplayWindow = NULL;
				m_nCamNr = nCamNr;
				if (pPlayWnd->IsShowable() && !pPlayWnd->IsActive())
				{
					pDisplayWindow = pPlayWnd; // Wiedergabefenster vorhanden UND geöffnet
				}
				else if (pLiveWnd && !pLiveWnd->IsShowable())// Das Livewindow kann ausgeschaltet sein.
				{
					pLiveWnd->SetShowable();	// Das LiveWindow wird hier wieder eingeschaltet
					pDisplayWindow = pLiveWnd;	// Wiedergabefenster vorhanden NICHT geöffnet, Livefenster vorhanden
				}
				else if (pPlayWnd->IsShowable())// Auch das Playwindow kann ausgeschaltet sein.
				{
					pDisplayWindow = pPlayWnd; // Wiedergabefenster vorhanden NICHT geöffnet, Kein Livefenster
				}
			}
			if (pDisplayWindow == NULL && pLiveWnd)
			{
				m_nCamNr = nCamNr;
				pDisplayWindow = pLiveWnd;	   // Playwindow NICHT vorhanden, Live vorhanden.
			}
			break;
	}

	if (pDisplayWindow)
	{
		pMF->SetDisplayActive(pDisplayWindow);
	}
	
	pMF->UnlockDisplayWindows();

	m_OSDNormal.SetSubMenuPos(IDOSD_CAM1, m_nCamNr);
	OnSetViewMode(m_eViewMode);
	OnUpdateMenu(0,0);
	DisableInputMode();

	return 0;				 
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSwitchToNextLiveCamera()
{
	int nTmpCamNr = 0;
	int nCamNr = m_nCamNr;
	nCamNr++;

	nCamNr = nCamNr & (MAX_CAM-1);
	if ((nCamNr >= 0) && (nCamNr < MAX_CAM))
	{
		// Suche die nächste mögliche Livecamera
		for (int nI = nCamNr; nI < nCamNr+MAX_CAM; nI++)
		{
			nTmpCamNr = nI & (MAX_CAM-1);
			if (ExistLiveCamera(nTmpCamNr))
			{
				m_nCamNr = nTmpCamNr;
				break;
			}
		}
		m_OSDNormal.SetSubMenuPos(IDOSD_CAM1, m_nCamNr);
	}
	else
		WK_TRACE_ERROR(_T("CPanel::OnIncLiveCamera unknown camera (%d)\n"), nCamNr);

	OnSetViewMode(m_eViewMode);
	OnUpdateMenu(0,0);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSwitchToPrevLiveCamera()
{
	int nTmpCamNr = 0;

	int nCamNr = m_nCamNr;
	nCamNr--;

	nCamNr = nCamNr & (MAX_CAM-1);
	if ((nCamNr >= 0) && (nCamNr < MAX_CAM))
	{
		// Suche die vorherige mögliche Livecamera
		for (int nI = nCamNr; nI >= nCamNr-(MAX_CAM-1); nI--)
		{
			nTmpCamNr = nI & (MAX_CAM-1);
			if (ExistLiveCamera(nTmpCamNr))
			{
				m_nCamNr = nTmpCamNr;
				break;
			}
		}
		m_OSDNormal.SetSubMenuPos(IDOSD_CAM1, m_nCamNr);
	}
	else
		WK_TRACE_ERROR(_T("CPanel::OnDecCamera unknown camera (%d)\n"), nCamNr);

	OnSetViewMode(m_eViewMode);
	OnUpdateMenu(0,0);
}

/////////////////////////////////////////////////////////////////////////////
// Markiert ein Fenster als aktiv. Es ist immer genau eines der Fenster
// aktiv. Dieses wird in CMainFrame::AutoViewMode groß dargestellt.
void CPanel::SetDisplayActive(int nCamNr)
{
	nCamNr = nCamNr & (MAX_CAM-1);
	if ((nCamNr >= 0) && (nCamNr < MAX_CAM))
	{
		CMainFrame* pMF = theApp.GetMainFrame();
		if (pMF)
		{
			CLiveWindow* pLW = pMF->GetLiveWindow((WORD)nCamNr);
			if (pLW)
			{
				pMF->SetDisplayActive(pLW);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::NotifyCIPCConnect(ServerControlGroup control)
{
	PostMessage(WM_NOTIFY_CIPC_CONNECT,control);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnNotifyCIPCConnect(WPARAM wParam, LPARAM lParam)
{				 
	ServerControlGroup control = (ServerControlGroup)wParam;
	switch (control)
	{
	case SC_INPUT:
		if (theApp.GetServer())
		{
			CString sEnteredPIN;
			sEnteredPIN = theApp.GetServer()->GetEnteredPIN();
			if (sEnteredPIN == theApp.GetServer()->GetSupervisorPIN())
			{
				m_eUserMode = UM_SUPERVISOR;
				WK_TRACE(_T("User mode is Supervisor\n"));
			}
			else if (sEnteredPIN == theApp.GetServer()->GetOperatorPIN())
			{
				m_eUserMode = UM_OPERATOR;
				WK_TRACE(_T("User mode is Operator\n"));
			}
			else
			{
				WK_TRACE_ERROR(_T("unknown User mode\n"));
			}
		}
		break;
	case SC_OUTPUT:
		if (theApp.GetServer())
		{
			CServer* pServer = theApp.GetServer();
			if(pServer)
			{
				CIPCOutputDVClient* pCIPCOutputClient = pServer->GetOutput();
				if(pCIPCOutputClient)
				{
					m_nCameras = pCIPCOutputClient->GetNrOfCameras();

		//			LoadNormalMenu();
					LoadMenus();

					if (pServer->IsAlarm())
					{
						OnSetCamera(pServer->GetAlarmCameraNr(), Live);
					}
				}
			}
		}
		break;
	case SC_DATABASE:
		break;
	default:
		break;
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::NotifyConnect(BOOL bOK, int iErrorCode)
{
	PostMessage(WM_NOTIFY_CONNECT, bOK, iErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnNotifyConnect(WPARAM wParam, LPARAM lParam)
{				 
	BOOL bOK = wParam;
	int iErrorCode = lParam;
	CString sMsg;

	if (   (m_eInputState == InputPanelPIN)
		|| (m_eInputState == InputSelectHost)
		)
	{
		// Dem Nutzer anzeigen, das die Verbindung
		// fehlgeschlagen ist, oder geklappt hat
		if (bOK)
		{
			if (m_eInputState == InputPanelPIN)
			{
				sMsg.LoadString(IDS_PIN_SUCCESS);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadInfoMessage();
			}
			SetPanelPosition(FALSE);	
			m_eInputState = InputNo;
			m_sInput.Empty();
			DisableInputMode();

			// Name der Alarmverbindung holen
			if (theApp.GetServer() && theApp.GetServer()->IsAlarm())
				m_sHostName = theApp.GetServer()->GetHostName();
		}
		else
		{
			// dem Nutzer anzeigen, was er wieder falsch gemacht hat
			sMsg.LoadString(IDS_PIN_FAILED);
			switch (iErrorCode)
			{
			case CIPC_ERROR_INVALID_PASSWORD:
				// prevent compiler error
				sMsg.LoadString(IDS_PIN_FAILED);
				break;
			case CIPC_ERROR_UNABLE_TO_CONNECT:
				sMsg.LoadString(IDS_CONNECT_ERROR);
				break;
			case CIPC_ERROR_INVALID_SERIAL:
 				sMsg.LoadString(IDS_INVALID_SERIAL);
				break;
			case CIPC_ERROR_UNKNOWN:
 				sMsg.LoadString(CIPC_ERROR_UNKNOWN);
				break;

			default:
				// prevent compiler error
				break;
			}
			m_sInput.Empty();
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			WaitToReadErrorMessage();

			// Avoid automatic reconnect
			// Well as long as possible - till the user tries again
			OnDisconnect();
		}
		UpdateMenu();
	}

	// Auch im Readonly-modus muß das Panel nach dem Connect in die rechte untere Ecke verschoben werden
	if (theApp.IsReadOnly())
		SetPanelPosition(FALSE);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::NotifyDisconnect()
{
	PostMessage(WM_NOTIFY_DISCONNECT);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnNotifyDisconnect(WPARAM wParam, LPARAM lParam)
{
	m_eUserMode = UM_INVALID;

	// Offene Dialoge schließen.
	if (m_pRTEDlg)
	{
		m_pRTEDlg->SendMessage(WM_COMMAND , MAKELONG(IDC_RTE_OK, BN_CLICKED), NULL);
	}
	if (m_pKeyboardDlg)
	{
		m_pKeyboardDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	if (m_pStatisticDlg)
	{
		m_pStatisticDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	if (m_pAlarmlistDlg)
	{
		m_pAlarmlistDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	if (m_pMDConfigDlg)
	{
		m_pMDConfigDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	if (m_pHostsDlg)
	{
		m_pHostsDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	if (m_pInfoDlg)
	{
		m_pInfoDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	if (m_pMaintenanceDlg)
	{
		m_pMaintenanceDlg->SendMessage(WM_COMMAND , MAKELONG(IDC_MAINT_BUTTON_OK, BN_CLICKED), NULL);
	}

	// Bei Verbindungsabbau raus aus dem Konfigurationsmenü
	m_pOSD	= &m_OSDNormal;

	// Panel maximieren
	MaximizeDlg();

	if (theApp.IsTransmitter())
	{
		m_eInputState	= InputPanelPIN;
	}
	else
	{
		m_eInputState	= InputSelectHost;
	}

	LoadHostList();
	EnableInputMode();
	UpdateMenu();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSetViewMode(int nViewMode)
{
	//TRACE(_T("##CPanel::OnSetViewMode (ViewMode=%d)\n"), nViewMode);

	m_eViewMode = (ViewMode)nViewMode;
	m_OSDNormal.SetSubMenuPos(IDOSD_VIEW_FULL, m_eViewMode);
	CMainFrame* pMainWnd  = theApp.GetMainFrame();
	if (pMainWnd)
    {
		pMainWnd->SetView((WORD)m_nCamNr,m_eViewMode);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetClock(const CString &sD, const CString &sT)
{
	BOOL bResult = FALSE;

	int nDay, nMonth, nYear, nHour, nMinute, nSecond;
	CString sDate = sD;
	CString sTime = sT;

	CSystemTime t;
	t.GetLocalTime();

	// Fehlende Datumsangabe hinzufügen
	if (sDate.IsEmpty())
	{
		nDay	= t.GetDay();
		nMonth	= t.GetMonth();
		nYear	= t.GetYear()%100;
		if (nYear >= 99)
			nYear += 1900;
		else
			nYear += 2000;
	}
	else
	{
		if (!ExtractAndValidateDate(sDate, nDay, nMonth, nYear))
			return FALSE;
	}
	
	// Fehlende Zeitangabe hinzufügen
	if (sTime.IsEmpty())
	{
		nHour	= t.GetHour();
		nMinute	= t.GetMinute();
		nSecond	= t.GetSecond();
	}
	else
	{
		if (!ExtractAndValidateTime(sTime, nHour, nMinute, nSecond))
			return FALSE;
	}

	CSystemTime timeNew((WORD)nDay, (WORD)nMonth, (WORD)nYear, 
		(WORD)nHour, (WORD)nMinute, (WORD)nSecond);


	// Uhrzeit/Datum setzen
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		bResult = pServer->SetClock(timeNew);
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
// Setzt die TimeZoneInformation mit dem Index 'nTimeZone' des TimeZoneInformationArray
BOOL CPanel::OnSetTimeZone(int nTimeZone)
{
	BOOL bResult = FALSE;

	// Zeitzone setzen
	CServer* pServer = theApp.GetServer();
	if (pServer && (m_TziArray.GetSize() > 0))
	{
		if (pServer->SetTimeZoneInformation(*m_TziArray.GetAtFast(nTimeZone)))
			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// Holt die aktuell eingestellte TimeZoneInformation
BOOL CPanel::OnGetTimeZone(CTimeZoneInformation& TimeZone)
{
	BOOL bResult = FALSE;

	// Zeitzone ermitteln
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->GetTimeZoneInformation(TimeZone))
			bResult = TRUE;
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
// Holt alle verfügbaren TimeZoneInformation
int CPanel::OnGetTimeZones()
{
	// Zeitzonen holen
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		// Liste aller Zeitzonen holen
		if (pServer->GetTimeZoneInformations(m_TziArray))
		{
			// Aktuelle Zeitzone holem
			CTimeZoneInformation TimeZone;
			if (OnGetTimeZone(TimeZone))
			{
				// Index innerhalb der Zeitzonenliste holen.
				int nIndex = m_TziArray.FindByTzi(TimeZone);
				if (nIndex != -1)
					m_nTimeZone = nIndex;
			}
		}
	}

	return m_nTimeZone;
}
/////////////////////////////////////////////////////////////////////////////
// Erhöht den Index auf das TimeZoneInformationArray
BOOL CPanel::OnIncrementTimeZone()
{
	m_nTimeZone++;
	if (m_nTimeZone >= m_TziArray.GetSize())
		m_nTimeZone = 0;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Verringert de Index auf das TimeZoneInformationArray
BOOL CPanel::OnDecrementTimeZone()
{
	m_nTimeZone--;
	if (m_nTimeZone < 0)
		m_nTimeZone = m_TziArray.GetSize()-1;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_dwLastUserAction = GetTickCount();
	CTransparentDialog::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsMDActive(int nCamNr)
{
	BOOL bRet = FALSE;
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CLiveWindow* pLiveWnd = pMF->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bRet = pLiveWnd->IsMD();
		}
	}
	return bRet;	
}
/////////////////////////////////////////////////////////////////////////////
// Liefert True, wenn übergebene Kamera einen UVV-Kassen Alarm hat
BOOL CPanel::IsActiveCamUVVActive()
{
	BOOL bResult = FALSE;
	CMainFrame*		pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)GetActiveCamera());
		if (pLiveWnd)
		{
			bResult = !pLiveWnd->IsMD() && pLiveWnd->IsAlarmActive();	
		}
	}
	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
// Liefert True, wenn mindestens eine Kamera einen UVV-Kassen Alarm hat
BOOL CPanel::IsAnyUVVActive()
{
	BOOL bResult = FALSE;
	CMainFrame*		pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		for (int nI = 0; ((nI < MAX_CAM) && !bResult); nI++)
		{
			CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)nI);
			if (pLiveWnd)
			{
				bResult = !pLiveWnd->IsMD() && pLiveWnd->IsAlarmActive();	
			}
		}	
	}
	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonClose() 
{
	MinimizeDlg();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnCancel()
{									  
	if (m_ctrlButtonEject.IsWindowEnabled())
		OnButtonEject();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::EnablePanel(BOOL bEnable)
{
	OnUpdateButtons(bEnable,0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnUpdateMenu(WPARAM wParam, LPARAM lParam)
{
//	TRACE(_T("OnUpdateMenu\n"));
	CString	sMsg  = _T("");
	CString sTemp = _T("");
   if (wParam == WPARAM_CONFIG_MENU)
	{
		LoadConfigMenu();
		if (m_nNewDefaultPrt)
		{
			SetDefaultPrinter(m_nNewDefaultPrt);
			m_nNewDefaultPrt = 0;
		}
		else
		{
			SetDefaultPrinter(GetDefaultPrinter());
		}
		if (m_pOSD == &m_OSDConfig)
		{
			theApp.GetMainFrame()->InvalidateRect(NULL);
			theApp.GetMainFrame()->UpdateWindow();
		}
//		return 0;
	}
	// Kameramenü aktualisieren und prüfen ob mehr als eine Kamera möglich
	if (CheckExistingCameras() <= 1)
	{
		m_pOSD->EnableMenuItemTitle(IDOSD_VIEW_FULL,		MF_DISABLED);
	}
	else
	{
		m_pOSD->EnableMenuItemTitle(IDOSD_VIEW_FULL,		MF_ENABLED);
	}

	// Die folgenden Menüpunkte überspringen, sofern Kamera nicht vorhanden
	if (!ExistLiveCamera(m_nCamNr))
	{							   
		m_pOSD->EnableMenuItemTitle(IDOSD_COLOR_BRIGHTNESS,		MF_DISABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_MD_MASK_SENSITIVITY,	MF_DISABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_CHANGE_CAMNAME,		MF_DISABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_ALARM_MODE,			MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_MO,					MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_TU,					MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_WE,					MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_TH,					MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_FR,					MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_SA,					MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_SU,					MF_DISABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_SEQUENCER_DWELLTIME,	MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALL_EVENT,			MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALLNUMBER1,			MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALLNUMBER2,			MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALLNUMBER3,			MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_COMPRESSION,		MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_FPS,				MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_PTZ_TYPE,			MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_PTZ_ID,				MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_PTZ_COMPORT,		MF_DISABLED);
	}
	else
	{
		m_pOSD->EnableMenuItemTitle(IDOSD_COLOR_BRIGHTNESS,		MF_ENABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_MD_MASK_SENSITIVITY,	MF_ENABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_CHANGE_CAMNAME,		MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_MO,					MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_TU,					MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_WE,					MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_TH,					MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_FR,					MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_SA,					MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_TIMER_SU,					MF_ENABLED);
		m_pOSD->EnableMenuItemTitle(IDOSD_SEQUENCER_DWELLTIME,	MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_PTZ_TYPE,			MF_ENABLED);

		int nID	 = m_pOSD->GetMenuID();
		int iStateID = MF_DISABLED;
		int iStateComPort = MF_DISABLED;
		int iStateCompression = MF_DISABLED;
		int iStateCameraFPS = MF_DISABLED;

		switch (nID)
		{
		case IDOSD_CAMERA_PTZ_ID:
		case IDOSD_CAMERA_PTZ_COMPORT:
			{
				CMainFrame* pMainWnd = theApp.GetMainFrame();
				if (pMainWnd)
				{
					CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if(pLiveWnd)
					{
						int iCameraPTZType = CCT_UNKNOWN;
						if (GetCameraPTZType(iCameraPTZType))
						{
							if (IsCameraIDRequired((CameraControlType)iCameraPTZType))
							{
								iStateID = MF_ENABLED;
							}
							if (iCameraPTZType != CCT_UNKNOWN)
							{
								iStateComPort = MF_ENABLED;
							}
						}
					}
				}
			}
			break;
		case IDOSD_CAMERA_COMPRESSION:
			{
				//disable camera compression on alarm cameras
				if(IsMDActive(m_nCamNr))
				{
					iStateCompression = MF_ENABLED;
				}
			}
			break;
		case IDOSD_CAMERA_FPS:
			{
				int nCardType = CARD_TYPE_UNKNOWN;
				CMainFrame* pMainWnd = theApp.GetMainFrame();
				if (pMainWnd)
				{
					CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if(pLiveWnd)
					{
						pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
						if (pLiveWnd)
						{
							//check card type of current camera (Jacob, Savic, Tasha, Q)
							nCardType = pLiveWnd->GetAlarmCallEventByCardType();
						}
					}
				}

				if  ((nCardType == CARD_TYPE_TASHA) || (nCardType == CARD_TYPE_Q))
				{
					iStateCameraFPS = MF_ENABLED;
				}
			}
			break;
		}
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_PTZ_ID,				iStateID);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_PTZ_COMPORT,		iStateComPort);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_COMPRESSION,		iStateCompression);
		m_pOSD->EnableMenuItem(IDOSD_CAMERA_FPS,				iStateCameraFPS);

		// aufnahme modus nur ändern, wenn kein
		// Alarmlistendialog offen ist.
		m_pOSD->EnableMenuItemTitle(IDOSD_ALARM_MODE,			MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_SHOW_ALARMLIST,
			(theApp.GetServer() && theApp.GetServer()->CanAlarmList()) ? MF_ENABLED : MF_DISABLED);

		// Kann die permanente Maske editiert werden?
		m_pOSD->EnableMenuItem(IDOSD_MD_DRAW_MASK,
			(theApp.GetServer() && theApp.GetServer()->CanActivityMask()) ? MF_ENABLED : MF_DISABLED);

		// Kann der externe Alarmausgang aktiviert/deaktiviert werden?
		m_pOSD->EnableMenuItem(IDOSD_ALARM_OUTPUT_ON_OFF,
			(theApp.GetServer() && theApp.GetServer()->CanAlarmOutput()) ? MF_ENABLED : MF_DISABLED);
	
		// Können Alarmanrufe gemacht werden?
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALLNUMBER1,
			(theApp.GetServer() && theApp.GetServer()->CanAlarmCall()) ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALLNUMBER2,
			(theApp.GetServer() && theApp.GetServer()->CanAlarmCall()) ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALLNUMBER3,
			(theApp.GetServer() && theApp.GetServer()->CanAlarmCall()) ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_ALARM_CALL_EVENT,
			(theApp.GetServer() && theApp.GetServer()->CanAlarmCall()) ? MF_ENABLED : MF_DISABLED);

		// Gibt es die Möglichkeit eines Freischaltcodes
		m_pOSD->EnableMenuItem(IDOSD_EXPANSION_CODE,
			(theApp.GetServer() && theApp.GetServer()->HasExpanionCode()) ? MF_ENABLED : MF_DISABLED);
	
	}

	CIPCAudioDVClient *pAudio = theApp.GetAudio();
//TODO tkr, rke Für DTS-IP-Receiver Audio remote auf DTS einstellbar machen,
			//dazu ohne LocalAudio arbeiten

	if (pAudio)
	{
		BOOL bRecording   = pAudio->GetCameraNumber() > 0;
		BOOL bListenIn    = pAudio->IsInputOn();

		BOOL bDetectLevel = FALSE;
		switch (pAudio->GetRecordingMode())
		{
			case AUDIO_RECORDING_L:		// alle Modi, die Schwellwerterkennung auswerten
			case AUDIO_RECORDING_LE:
			case AUDIO_RECORDING_AL:
			case AUDIO_RECORDING_ALE:
				bDetectLevel = TRUE;
				break;
		}
		
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_LISTEN_IN   , MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_CHANGE_CAMNR, MF_ENABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_RECORD_MODE , (bRecording                ) ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_SENSITIVITY , (bRecording || bListenIn   ) ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_THRESHOLD   , (bRecording && bDetectLevel) ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_VOLUME      , MF_ENABLED);
	}
	else
	{
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_LISTEN_IN   , MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_CHANGE_CAMNR, MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_VOLUME      , MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_RECORD_MODE , MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_THRESHOLD   , MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_AUDIO_SENSITIVITY , MF_DISABLED);
	}
	
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		BOOL bBackupAll = !pMainWnd->IsInBackup();
		CPlayWindow* pPlayWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
		if (pPlayWnd)
		{
			BOOL bAlarm = pPlayWnd->IsAlarm();
			BOOL bAlarmDelete = FALSE;
			BOOL bAlarmBackup = FALSE;
			BOOL bActivityBackup = FALSE;
			BOOL bStopActivityBackup = FALSE;
			BOOL bShowSuspect = FALSE;
			BOOL bDeleteSuspect = FALSE;
			BOOL bChangeMode = FALSE;

			bChangeMode = pPlayWnd->CanChangeMode();
			if (pPlayWnd->IsWindowVisible())
			{
//TODO tkr wenn gar keine Brennsoftware installiert ist (z.B. unter XP ohne Nero) bedenke RemoteBrennen
//dann darf hier kein einziger Bachup Eintrag im Panel enabled sein
				bAlarmDelete = pPlayWnd->CanPlayStatus(CPlayWindow::PS_DELETE_ALARM);
				bShowSuspect = pPlayWnd->CanPlayStatus(CPlayWindow::PS_SHOW_SUSPECT);
				bDeleteSuspect = pPlayWnd->CanPlayStatus(CPlayWindow::PS_DELETE_SUSPECT);
				if (bAlarm)
				{
					bAlarmBackup = bBackupAll && bAlarmDelete;
				}
				else
				{
				//ob Menüeitrag "Auslagerung auf CD" enabled ist oder nicht, wird weiter unten
				//entschieden 
//					bActivityBackup = bBackupAll;
				}
			}

			CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
			if (pLiveWnd)
			{
				BOOL bNoAlarm = !pLiveWnd->IsAlarmActive();
				bAlarmDelete &= bNoAlarm;
				bAlarmBackup &= bNoAlarm;
			}

			//wenn gerade ein Alarm läuft, dann soll der Aufnahmemodus nicht geändert werden können
			//ebensfalls nicht änderbar, wenn gerade ein Backup läuft
			if (   !bChangeMode				// ACHTUNG: nur disablen, falls nötig, nicht enablen!
				|| IsActiveCamUVVActive()
				|| pMainWnd->GetBackupWindow())
			{
				m_pOSD->EnableMenuItem(IDOSD_ALARM_MODE, MF_DISABLED);
			}
		
			if (bAlarm) // ACHTUNG: nur disablen, falls nötig, nicht enablen!
			{
				m_pOSD->EnableMenuItemTitle(IDOSD_MD_MASK_SENSITIVITY, MF_DISABLED);
			}
			
			m_pOSD->EnableMenuItem(IDOSD_ALARM_DELETE,bAlarmDelete ? MF_ENABLED : MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_ALARM_BACKUP,bAlarmBackup ? MF_ENABLED : MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_ACTIVITY_BACKUP,bActivityBackup ? MF_ENABLED : MF_DISABLED);
		
//TODO TKR hier testen, ob bStopActivityBackup benötigt wird
			m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, bStopActivityBackup ? MF_ENABLED : MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_MULTY_ACTIVITY_BACKUP,	MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_SUSPECT_DELETE,bDeleteSuspect ? MF_ENABLED : MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_SUSPECT_SEARCH,bShowSuspect ? MF_ENABLED : MF_DISABLED);
		}
		else
		{
			// no play window
			m_pOSD->EnableMenuItem(IDOSD_ALARM_DELETE,MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_ALARM_BACKUP,MF_DISABLED);

			m_pOSD->EnableMenuItem(IDOSD_ACTIVITY_BACKUP,MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, MF_DISABLED);

			m_pOSD->EnableMenuItem(IDOSD_MULTY_ACTIVITY_BACKUP,	MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_SUSPECT_DELETE,MF_DISABLED);
			m_pOSD->EnableMenuItem(IDOSD_SUSPECT_SEARCH,MF_DISABLED);
		}

		m_pOSD->EnableMenuItem(IDOSD_OPTION_LOCK, bBackupAll ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_OPTION_EXIT, bBackupAll ? MF_ENABLED : MF_DISABLED);
		m_pOSD->EnableMenuItem(IDOSD_OPTION_UPDATE, bBackupAll ? MF_ENABLED : MF_DISABLED);

		// Läuft ein UVV-Kassen Alarm oder ist die Datenbank noch nicht 
		//connected (z.B. beim Scannen der Datenbank) dann Menüpunkt 'Bilder' disablen
		switch (m_pOSD->GetMenuID())
		{
			case  IDOSD_ALARM_DELETE:
			case  IDOSD_ALARM_BACKUP:
			case  IDOSD_ACTIVITY_BACKUP:
			case  IDOSD_MULTY_ACTIVITY_BACKUP:
			case  IDOSD_SUSPECT_DELETE:
			case  IDOSD_SUSPECT_SEARCH:
			case  IDOSD_STOP_ACTIVITY_BACKUP:

			if (   !theApp.GetServer()->IsDatabaseConnected() 
				|| IsAnyUVVActive())
			{
				m_pOSD->EnableMenuItemTitle(IDOSD_ALARM_DELETE, MF_DISABLED);
			}
			else
			{
				//soll CD-RW gelöscht werden, "ja" oder "nein"
				if (m_eInputState == InputBackupEraseMediumConfirmingYes)
				{
					sMsg.LoadString(IDS_ERASE_MEDIUM);
					sTemp.LoadString(IDS_YES);
					CString	sSubMenu = m_pOSD->GetSubString();
					sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
					SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
				}
				else if (m_eInputState == InputBackupEraseMediumConfirmingNo)
				{
					sMsg.LoadString(IDS_ERASE_MEDIUM);
					sTemp.LoadString(IDS_NO);
					CString	sSubMenu = m_pOSD->GetSubString();
					sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
					SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
				}

				

				CServer* pServer = theApp.GetServer();
				//bei Server Version ab 1.5 ist Multibackup möglich
				if(pServer && pServer->CanMultiCDExport())
				{
					// Prüfe ob Backuplaufwerk vorhanden.
					CIPCDatabaseDVClient* pDatabase = NULL;
					if (pServer->IsDatabaseConnected())
					{
						pDatabase = pServer->GetDatabase();
					}

					BOOL bSetBackupItems = FALSE;

					//enable backup menu
					if (     (   pDatabase->GetBackupDrive() != NULL
						      && pDatabase->GetBackupDrive()->GetType() != DRIVE_REMOVABLE)
						  || theApp.CanBackupWithACDC()
						  || m_bBackupToExternDrive)
					{
						//backup to extern, removable drive
						if(m_bBackupToExternDrive)
						{
							bSetBackupItems = TRUE;
						}
						//backup with ACDC
						else if(theApp.CanBackupWithACDC())
						{
							if(    theApp.IsACDCBackupDrive()
								|| m_bBackupToExternDrive)
							{
								bSetBackupItems = TRUE;
							}
						}
						//backup with directCD, only if backup drive is not removable
						else if(    pDatabase->GetBackupDrive() != NULL
							     && pDatabase->GetBackupDrive()->GetType() != DRIVE_REMOVABLE)
						{
							bSetBackupItems = TRUE;
						}
	
					}
					else // kein BackupDrive (also kein DirectCD,
						 //kein BackupWithACDC und kein MemoryStick
						 //also auch AlarmBackup disablen
					{
						m_pOSD->EnableMenuItem(IDOSD_ALARM_BACKUP,MF_DISABLED);

						//TODO TKR prüfen, ob das hier auch disabled werden muss. Ist nämlich sowieso schon
						//gleube ich, bei keinem Backupdrive
						m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, MF_DISABLED);
					}

					if(bSetBackupItems)
					{
						CBackupWindow* pBackupWnd = pMainWnd->GetBackupWindow();
						if(pBackupWnd)
						{
							//auf jeden Fall disablen wenn BackupWindow vorhanden, da immer nur
							//ein einziges Backup durchgeführt werden kann
							m_pOSD->EnableMenuItem(IDOSD_ALARM_DELETE, MF_DISABLED);
							m_pOSD->EnableMenuItem(IDOSD_ALARM_BACKUP, MF_DISABLED);
							
							if(pBackupWnd->IsBackupRunning() || pBackupWnd->IsBackupFailed())
							{
								//backup copies data or failed
								m_pOSD->EnableMenuItem(IDOSD_ACTIVITY_BACKUP,MF_DISABLED);
								m_pOSD->EnableMenuItem(IDOSD_MULTY_ACTIVITY_BACKUP,MF_DISABLED);
								
								//Nur bei Zwischenspeicherung der Bilder oder Brennen mit ACDC
								//oder löschen eines externen mediums (Memory Stick)
								//"Auslagerung abbrechen" enablen
								if(    pBackupWnd->IsBackupACDC_PreparingToCopy()
									|| pBackupWnd->IsBackupACDC_BurningToDisc()
									|| pBackupWnd->IsBackupACDC_EraseExternMedium())
								{
									m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, MF_ENABLED);
								}

							}
							else 
							{
								//backup not yet started
								m_pOSD->EnableMenuItem(IDOSD_ACTIVITY_BACKUP,MF_ENABLED);
								m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, MF_DISABLED);

								if(m_pOSD->GetMenuID() == IDOSD_MULTY_ACTIVITY_BACKUP)
								{
									m_pOSD->EnableMenuItem(IDOSD_MULTY_ACTIVITY_BACKUP,
											CanMultiBackup() ? MF_ENABLED : MF_DISABLED);
								}
							}
						}			 
						else
						{
							//backup not yet started
							m_pOSD->EnableMenuItem(IDOSD_ACTIVITY_BACKUP,MF_ENABLED);
							m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, MF_DISABLED);
							
							if(m_pOSD->GetMenuID() == IDOSD_MULTY_ACTIVITY_BACKUP)
							{
								m_pOSD->EnableMenuItem(IDOSD_MULTY_ACTIVITY_BACKUP,
										CanMultiBackup() ? MF_ENABLED : MF_DISABLED);
							}

							//backup to extern, removable drive
							//keine Alarmbackup, kein Multibackup erlauben
					//TODO TKR wenn externe HDD unterstützt werden sollen, dann reicht der Platz
					//aus um ein Alarmbackup durchzuführen, auf Memorystick reicht es nicht aus
							if(m_bBackupToExternDrive)
							{
								m_pOSD->EnableMenuItem(IDOSD_ALARM_BACKUP, MF_DISABLED);
								m_pOSD->EnableMenuItem(IDOSD_MULTY_ACTIVITY_BACKUP,MF_DISABLED);
							}
						}					
					}
					else
					{
						m_pOSD->EnableMenuItem(IDOSD_ALARM_BACKUP,MF_DISABLED);

						//TODO TKR prüfen, ob das hier auch disabled werden muss. Ist nämlich sowieso schon
						//glaube ich, bei keinem Backupdrive
						m_pOSD->EnableMenuItem(IDOSD_STOP_ACTIVITY_BACKUP, MF_DISABLED);
					}
				}
			}
		}

		// Wenn kein Exportmedium vorhanden, dann Exp/Import funktion sperren
		m_OSDConfig.EnableMenuItem(IDOSD_IMPORT_SETTINGS, m_bIsExportDrive ? MF_ENABLED : MF_DISABLED);
		m_OSDConfig.EnableMenuItem(IDOSD_EXPORT_SETTINGS, m_bIsExportDrive ? MF_ENABLED : MF_DISABLED);
	}

	// alle Menu Stati gesetzt, jetzt sind die Knöpfe dran
	OnUpdateButtons(TRUE,0);

	if (m_eInputState != InputCamFirstDigit)
	{
		// Aktuelle Kamera anzeigen
		sMsg.LoadString(IDS_CAMERA);
		sTemp.Format(_T("%s.%d"), sMsg.Left(3), m_nCamNr+1);
		SetDisplayText(m_ctrlDisplayCamera, sTemp, 0, 0);
	}

	if (m_eInputState == InputSelectHost)
	{
		// Aktuelle Gegenstation zeigen
		sTemp.LoadString(IDS_SELECT_HOST);
		// OEM Spezifische Einstellungen
		sMsg.Format(sTemp, theApp.GetMainFrame()->GetShortOemName());
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		if (m_HostsAll.GetSize() > 0)
		{
			m_nHostIndex = max(min(m_nHostIndex, m_HostsAll.GetSize()-1), 0);
			CHost* pHost = (CHost*)m_HostsAll.GetAtFast(m_nHostIndex);
			SetDisplayText(m_ctrlDisplayMenu2, pHost->GetName());
			if (!m_ctrlDisplayMenu2.IsBlinking())
				m_ctrlDisplayMenu2.EnableBlinking(500);
		}
		else
		{
			sTemp.LoadString(IDS_HOSTLIST_EMPTY);
			// OEM Spezifische Einstellungen
			sMsg.Format(sTemp, theApp.GetMainFrame()->GetShortOemName());
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			m_ctrlDisplayMenu2.DisableBlinking();
		}
	}

	if (m_eInputState == InputPanelPIN)
	{
		if (theApp.IsReceiver())
			SetDisplayText(m_ctrlDisplayMenu1, m_sHostName);
		else
			SetDisplayText(m_ctrlDisplayMenu1, _T(""));

		OnButtonNum(_T(""));
	}

	// Eingabe der Shutdown PIN
	if (m_eInputState == InputShutdownPIN)
	{
		sMsg.LoadString(IDS_POWEROFF_PRESSED);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);

		OnButtonNum(_T(""));
	}

	// Eingabe der Reset PIN
	if (m_eInputState == InputResetPIN)
	{
		sMsg.LoadString(IDS_RESET_PRESSED);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);

		OnButtonNum(_T(""));
	}


	// Bei den folgenden Inputstates soll das Menü nicht aktualisiert werden
	if (m_eInputState != InputNo)
		return 0;									

	CLiveWindow* pLiveWnd = NULL;
	CPlayWindow* pPlayWnd = NULL;
	CSystemTime t;

	CString	sSubMenu = m_pOSD->GetSubString();
	int nID	 = m_pOSD->GetMenuID();
	int	nVal = 0;

	switch (nID)
	{
		case IDOSD_CAM1:
		case IDOSD_CAM2:
		case IDOSD_CAM3:
		case IDOSD_CAM4:
		case IDOSD_CAM5:
		case IDOSD_CAM6:
		case IDOSD_CAM7:
		case IDOSD_CAM8:
		case IDOSD_CAM9:
		case IDOSD_CAM10:
		case IDOSD_CAM11:
		case IDOSD_CAM12:
		case IDOSD_CAM13:
		case IDOSD_CAM14:
		case IDOSD_CAM15:
		case IDOSD_CAM16:
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_pOSD->GetSubMenuPos());
				if (pLiveWnd)
				{
					sSubMenu.Format(_T(">(%d)%s"), m_pOSD->GetSubMenuPos()+1, pLiveWnd->GetName());
					SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
				}
				else
				{
					pPlayWnd = pMainWnd->GetPlayWindow((WORD)m_pOSD->GetSubMenuPos());
					if (pPlayWnd)
					{
						sSubMenu.Format(_T(">(%d)%s"), m_pOSD->GetSubMenuPos()+1, pPlayWnd->GetName());
						SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
					}
					else
					{
						sSubMenu.Format(_T(">(%d)%s"), m_pOSD->GetSubMenuPos()+1, _T("?????"));
						SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
					}
				}
			}
			break;
//		case IDOSD_CONFIG_PIN:	see at IDOSD_CHANGE_PIN_
		case IDOSD_COLOR_BRIGHTNESS:
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				if (pLiveWnd && pLiveWnd->GetBrightness(nVal))
					sSubMenu.Format(_T("%s: %d%%"), m_pOSD->GetSubString(), nVal);
				else
					sSubMenu.Format(_T("%s: ??%%"), m_pOSD->GetSubString());
				SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);			
			}
			break;
		case IDOSD_COLOR_CONTRAST:
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				if (pLiveWnd && pLiveWnd->GetContrast(nVal))
					sSubMenu.Format(_T("%s: %d%%"), m_pOSD->GetSubString(), nVal);
				else
					sSubMenu.Format(_T("%s: ??%%"), m_pOSD->GetSubString());	
				SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			}
			break;
		case IDOSD_COLOR_SATURATION:
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				if (pLiveWnd && pLiveWnd->GetSaturation(nVal))
					sSubMenu.Format(_T("%s: %d%%"), m_pOSD->GetSubString(), nVal);
				else
					sSubMenu.Format(_T("%s: ??%%"), m_pOSD->GetSubString());
				SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			}
			break;
		case IDOSD_CAMERA_COMPRESSION:
			OnUpdateMenuCameraCompression();
			break;
		case IDOSD_CAMERA_FPS:
			OnUpdateMenuCameraFPS();
			break;
		case IDOSD_CHANGE_CAMNAME:
			if (m_eInputState != InputChangeCamName)
			{
				if (pMainWnd)
				{
					CString sTemp = _T(">");
					pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					sMsg.LoadString(IDS_CAMNAME);
					sMsg = sTemp + sMsg;
					if (pLiveWnd)
						sSubMenu.Format(_T("%s %s"), sMsg, pLiveWnd->GetName());
					else
						sSubMenu.Format(_T("%s ?????????"), sMsg);
					
					SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
				}
			}
			break;
		case IDOSD_TIMER_MO:
 		case IDOSD_TIMER_TU:
		case IDOSD_TIMER_WE:
		case IDOSD_TIMER_TH:
		case IDOSD_TIMER_FR:
		case IDOSD_TIMER_SA:
		case IDOSD_TIMER_SU:
			if (m_eInputState != InputTimer)
			{
				if (pMainWnd)
				{
					pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
					if (pLiveWnd)
					{
						m_nDayOfWeek = m_pOSD->GetSubMenuPos();
						sTemp = pLiveWnd->GetTimer(m_nDayOfWeek);
					}
					else
						sTemp = _T("????????");

					SetFormatDisplayText(m_ctrlDisplayMenu2, sTemp, m_pOSD->GetSubString());
				}
			}
			break;
		case IDOSD_TIMER_ONOFF:
		{
			BOOL bTimerOnOffState = FALSE;
			sMsg = _T("???");
			if (GetTimerOnOffState(bTimerOnOffState))
			{
				if (!m_bSetValue)
					m_bTimerOnOffState = bTimerOnOffState;

				if (m_bTimerOnOffState)
					sMsg.LoadString(IDS_TIMER_ON);
				else
					sMsg.LoadString(IDS_TIMER_OFF);
			}

			// Actual value?
			if (   m_bSetValue
				&& (m_bTimerOnOffState == bTimerOnOffState)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_ALARM_MODE:
		{

			AlarmMode eAlarmMode = AlarmModeUnknown;

			if (IsMDActive(m_nCamNr))
			{
				eAlarmMode = MotionDetection;
			}
			else 
			{
				eAlarmMode = UVVKassen;
			}

			if (!m_bSetValue)
			{
				m_eAlarmMode = eAlarmMode;
			}
			
			if (m_eAlarmMode == MotionDetection)
				sMsg.LoadString(IDS_MD);
			else if (m_eAlarmMode == UVVKassen)
				sMsg.LoadString(IDS_UVV);
			else if (m_eAlarmMode == MotionDetectionConfirmingYes)
			{
				sMsg.LoadString(IDS_MD);
				sTemp.LoadString(IDS_YES);
				sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
			}
			else if (m_eAlarmMode == MotionDetectionConfirmingNo)
			{
				sMsg.LoadString(IDS_MD);
				sTemp.LoadString(IDS_NO);
				sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
			}
			else if (m_eAlarmMode == UVVKassenConfirmingYes)
			{
				sMsg.LoadString(IDS_UVV);
				sTemp.LoadString(IDS_YES);
				sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
			}
			else if (m_eAlarmMode == UVVKassenConfirmingNo)
			{
				sMsg.LoadString(IDS_UVV);
				sTemp.LoadString(IDS_NO);
				sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
			}
			else
				sMsg = _T("???");

			if ((m_eAlarmMode == UVVKassen) || (m_eAlarmMode == MotionDetection))
			{
				// Actual value?
				if (m_bSetValue && (m_eAlarmMode == eAlarmMode))
				{
					sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
				}
				else
				{
					sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
				}
			}

			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);

			break;
		}
		case IDOSD_MD_ALARM_SENSITIVITY:
		{
			int nMDAlarmLevelActual = MD_SENSITIVITY_UNKNOWN;
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				if (pLiveWnd)
				{
					nMDAlarmLevelActual = pLiveWnd->GetMDAlarmSensitivity();
				}
			}
			if (!m_bSetValue)
			{
				m_nMDAlarmLevel = nMDAlarmLevelActual;
			}
			if (m_nMDAlarmLevel == MD_SENSITIVITY_LOW)
				sMsg.LoadString(IDS_LOW);
			else if (m_nMDAlarmLevel == MD_SENSITIVITY_NORMAL)
				sMsg.LoadString(IDS_NORMAL);
			else if (m_nMDAlarmLevel == MD_SENSITIVITY_HIGH)
				sMsg.LoadString(IDS_HIGH);
			else
				sMsg = _T("???");

			// Actual value?
			if (   m_bSetValue
				&& (m_nMDAlarmLevel == nMDAlarmLevelActual)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_MD_MASK_SENSITIVITY:
		{
			int nMDMaskLevelActual = MD_SENSITIVITY_UNKNOWN;
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				if (pLiveWnd)
				{
					nMDMaskLevelActual = pLiveWnd->GetMDMaskSensitivity();
				}
			}
			if (!m_bSetValue)
			{
				m_nMDMaskLevel = nMDMaskLevelActual;
			}
			if (m_nMDMaskLevel == MD_SENSITIVITY_OFF)
				sMsg.LoadString(IDS_OFF);
			else if (m_nMDMaskLevel == MD_SENSITIVITY_LOW)
				sMsg.LoadString(IDS_LOW);
			else if (m_nMDMaskLevel == MD_SENSITIVITY_NORMAL)
				sMsg.LoadString(IDS_NORMAL);
			else if (m_nMDMaskLevel == MD_SENSITIVITY_HIGH)
				sMsg.LoadString(IDS_HIGH);
			else
				sMsg = _T("???");

			// Actual value?
			if (   m_bSetValue
				&& (m_nMDMaskLevel == nMDMaskLevelActual)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_ALARM_OUTPUT_ON_OFF:
		{
			BOOL bAlarmOutputOnOffState = FALSE;
			sMsg = _T("???");
			if (GetAlarmOutputOnOffState(bAlarmOutputOnOffState))
			{
				if (!m_bSetValue)
					m_bAlarmOutputOnOffState = bAlarmOutputOnOffState;

				if (m_bAlarmOutputOnOffState)
					sMsg.LoadString(IDS_ON);
				else
					sMsg.LoadString(IDS_OFF);
			}

			// Actual value?
			if (m_bSetValue && (m_bAlarmOutputOnOffState == bAlarmOutputOnOffState))
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_ALARM_CALLNUMBER1:
		case IDOSD_ALARM_CALLNUMBER2:
		case IDOSD_ALARM_CALLNUMBER3:
		{
			sMsg = _T("???");
			int nCallingNumber = m_pOSD->GetMenuID() - IDOSD_ALARM_CALLNUMBER1;
			nCallingNumber = max(min(nCallingNumber, 9), 0);

			GetAlarmDialingNumber(nCallingNumber, sMsg);

			CString sFormat;
			if (sMsg.Find(_T(".")) != -1)
			{	
				sMsg.Remove(_T('.'));
				sFormat.LoadString(IDS_FORMAT_IP);
			}
			else
				sFormat.LoadString(IDS_FORMAT_TEL);
			
			SetFormatDisplayText(m_ctrlDisplayMenu2, sMsg, m_pOSD->GetSubString()+sFormat);
			break;
		}
		case IDOSD_ALARM_CALL_EVENT:
		{
			int nAlarmCallEvent				= ALARMCALL_EVENT_UNKNOWN;
			int nAlarmCallEventByCardType	= CARD_TYPE_UNKNOWN;
			if (pMainWnd)
			{
				pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				if (pLiveWnd)
				{
					nAlarmCallEvent = pLiveWnd->GetAlarmCallEvent();
					
					//check card type of current camera (Jacob, Savic, Tasha)
					nAlarmCallEventByCardType = pLiveWnd->GetAlarmCallEventByCardType();
				}
			}
			
			if (!m_bSetValue)
			{
				m_nAlarmCallEvent			= nAlarmCallEvent;
				m_nAlarmCallEventByCardType = nAlarmCallEventByCardType;
			}
			
			if (m_nAlarmCallEvent == ALARMCALL_EVENT_OFF)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_OFF);
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_A)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_A);		
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_A1)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_A1);
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_A2)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_A2);
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_A12)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_A12);
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_1)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_1);
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_2)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_2);
			else if (m_nAlarmCallEvent == ALARMCALL_EVENT_12)
				sMsg.LoadString(IDS_ALARMCALL_EVENT_12);
			else
				sMsg = _T("???");


			// Actual value?
			if (   m_bSetValue
				&& (m_nAlarmCallEvent == nAlarmCallEvent)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_SEQUENCER_DWELLTIME:
			OnUpdateMenuSequencerDwellTime();
			break;

		case IDOSD_VIDEO_OUT_MODE_PORT_1:
		case IDOSD_VIDEO_OUT_MODE_PORT_2:
		case IDOSD_VIDEO_OUT_MODE_PORT_3:
		case IDOSD_VIDEO_OUT_MODE_PORT_4:
			OnUpdateMenuVideoOutputMode(m_pOSD->GetSubMenuPos());
			break;
		case IDOSD_DNS:
			if (m_bSetValue)
			{
				sMsg.LoadString(m_bValueOn ? IDS_ON : IDS_OFF);
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sMsg.LoadString(m_bDHCPenabled ? IDS_ON : IDS_OFF);
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		case IDOSD_TCPIP:
			if (m_eInputState != InputIP)
			{
				CString sIP;
				if (!GetIP(sIP, IPADDRESS))
				{
					if (m_bNetShellOK && !m_sNetworkAdapterName.IsEmpty())
					{
						sIP = m_sIP;
					}
					else
					{
						sIP = _T("????????????");
					}
				}
				SetFormatDisplayText(m_ctrlDisplayMenu2, sIP, m_pOSD->GetSubString());
			}
			break;
		case IDOSD_SUBNETMASK:
			if (m_eInputState != InputSubNetMask)
			{
				CString sSubMask;
				if (!GetIP(sSubMask, SUBNETMASK))
				{
					if (m_bNetShellOK&& !m_sNetworkAdapterName.IsEmpty())
					{
						sSubMask = m_sSubNetMask;
					}
					else
					{
						sSubMask = _T("????????????");
					}
				}
				SetFormatDisplayText(m_ctrlDisplayMenu2, sSubMask, m_pOSD->GetSubString());
			}
			break;
		case IDOSD_GATEWAY:
			if (m_eInputState != InputGateway)
			{
				CString sGateway;
				if (!GetIP(sGateway, DEFAULTGATEWAY))
					sGateway = _T("????????????");			
//				if (!GetDefaultGateway(sGateway))
//					sGateway = _T("????????????");			
				SetFormatDisplayText(m_ctrlDisplayMenu2, sGateway, m_pOSD->GetSubString());
			}
			break;
		case IDOSD_ISDN_MSN:
			if (m_eInputState != InputMSN)
				SetFormatDisplayText(m_ctrlDisplayMenu2, m_sMSN, m_pOSD->GetSubString());
			break;
		case IDOSD_NET_NAME:
			if (m_eInputState != InputChangeNetName)
			{
				CString sLocalHostName;
				sLocalHostName = GetLocalHostName();
				sSubMenu = sLocalHostName + _T("              ");
				SetFormatDisplayText(m_ctrlDisplayMenu2, sSubMenu, m_pOSD->GetSubString());
			}
			break;
		case IDOSD_CLOCK_DATE:
			if (m_eInputState != InputClockDate)
			{
				t.GetLocalTime();
				// Uhrzeit/Datum holen
				CServer* pServer = theApp.GetServer();
				if (pServer && pServer->GetClock(t))
					sTemp.Format(_T("%02d%02d%02d"),t.GetDay(),t.GetMonth(),t.GetYear()%100);
				else
					sTemp = _T("??????");

				SetFormatDisplayText(m_ctrlDisplayMenu2, sTemp, m_pOSD->GetSubString());
			}
			break;
		case IDOSD_CLOCK_TIME:
			if (m_eInputState != InputClockTime)
			{
				t.GetLocalTime();
				// Uhrzeit/Datum holen
				CServer* pServer = theApp.GetServer();
				if (pServer && pServer->GetClock(t))
					sTemp.Format(_T("%02d%02d%02d"),t.GetHour(),t.GetMinute(),t.GetSecond());
				else
					sTemp = _T("??????");

				SetFormatDisplayText(m_ctrlDisplayMenu2, sTemp, m_pOSD->GetSubString());
			}
			break;
		case IDOSD_TIME_ZONE:
			{
				sTemp = _T("");
				CString	sTimeZone;
				if (!m_bSetValue)
				{
					CTimeZoneInformation CTzi;
					if (OnGetTimeZone(CTzi))
						sTemp = CTzi.GetDisplayName();
					else
						sTemp = _T("???");			
				}
				else
				{						
					if ((m_TziArray.GetSize() > 0) && (m_nTimeZone < m_TziArray.GetSize()))
					{
						CTimeZoneInformation CTzi;
						if (OnGetTimeZone(CTzi))
						{
							int nCurrentTimeZone = m_TziArray.FindByStandardName(CTzi.StandardName);
							if ((nCurrentTimeZone != -1) && (nCurrentTimeZone == m_nTimeZone))
								sTemp = _T("*");
						}
						sTemp += m_TziArray.GetAtFast(m_nTimeZone)->GetDisplayName();
					}
					else
						sTemp = _T("???");			
				}
				sTimeZone.Format(_T("%s%s"), m_pOSD->GetSubString(), sTemp); 
				SetDisplayText(m_ctrlDisplayMenu2, sTimeZone);
			}
			break;
		case IDOSD_NO_PRINTER:
		case IDOSD_PRINTER1:
		case IDOSD_PRINTER2:
		case IDOSD_PRINTER3:
		case IDOSD_PRINTER4:
		case IDOSD_PRINTER5:
		case IDOSD_PRINTER6:
		case IDOSD_PRINTER7:
		case IDOSD_PRINTER8:
		case IDOSD_PRINTER9:
		case IDOSD_PRINTER10:
		case IDOSD_PRINTER11:
		case IDOSD_PRINTER12:
		case IDOSD_PRINTER13:
		case IDOSD_PRINTER14:
		case IDOSD_PRINTER15:
		case IDOSD_PRINTER16:
			if (!m_bSetValue)
			{
				nVal = min(max(GetDefaultPrinter(), 0), MAX_PRINTERS);
				for (int i=0; i<m_nPrinterCnt; i++)
				{
					if (nVal == m_pnPrinterSort[i])
					{
						nVal = i;
						break;
					}
				}
				m_pOSD->SetSubMenuPos(IDOSD_PRINTER1, nVal);
			}
			else
			{
				nVal = m_pOSD->GetSubMenuPos();
			}

			if (nVal == 0)
			{
				sMsg = m_pOSD->GetSubString();
			}
			else
			{
				sMsg = m_pPrinterInfo[m_pnPrinterSort[nVal]].pPrinterName;
			}

			if (   m_bSetValue
				&& (m_pnPrinterSort[nVal] == GetDefaultPrinter())
				)
			{
				sSubMenu.Format(_T(">*%s"), sMsg);
			}
			else
			{
				sSubMenu.Format(_T(">%s"), sMsg);
			}

			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		case IDOSD_LANGUAGE1:
		case IDOSD_LANGUAGE2:
		case IDOSD_LANGUAGE3:
		case IDOSD_LANGUAGE4:
		case IDOSD_LANGUAGE5:
		case IDOSD_LANGUAGE6:
		case IDOSD_LANGUAGE7:
		case IDOSD_LANGUAGE8:
		case IDOSD_LANGUAGE9:
		case IDOSD_LANGUAGE10:
		case IDOSD_LANGUAGE11:
		case IDOSD_LANGUAGE12:
		case IDOSD_LANGUAGE13:
		case IDOSD_LANGUAGE14:
		case IDOSD_LANGUAGE15:
		case IDOSD_LANGUAGE16:
		{
			int iIndex = GetDefaultLanguageIndex();
			if (!m_bSetValue)
			{
				nVal = min(max(iIndex, 0), m_iLanguageCnt);
				m_pOSD->SetSubMenuPos(IDOSD_LANGUAGE1, nVal);
			}
			else
			{
				nVal = m_pOSD->GetSubMenuPos();
			}

			CLangDllInfo* pDllInfo = m_arrayLanguages.GetAtFast(nVal);
			if (   m_bSetValue
				&& (nVal == iIndex)
				)
			{
				sSubMenu.Format(_T(">*%s"), pDllInfo->GetLanguage());
			}
			else
			{
				sSubMenu.Format(_T(">%s"), pDllInfo->GetLanguage());
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_CHANGE_PIN_OPERATOR:
		case IDOSD_CHANGE_PIN_SUPERVISOR:
		case IDOSD_CONFIG_PIN:
			if (m_eInputState != InputConfigPIN)
				SetFormatDisplayText(m_ctrlDisplayMenu2, _T("____"), m_pOSD->GetSubString());
			break;
		case IDOSD_OPTION_TARGET_DISPLAY:
		{
			BOOL bShowTargetCrossActual = CDisplayWindow::GetShowTargetCross();
			if (!m_bSetValue)
			{
				m_bShowTargetCross = bShowTargetCrossActual;
			}
			if (m_bShowTargetCross)
				sMsg.LoadString(IDS_ON);
			else
				sMsg.LoadString(IDS_OFF);

			// Actual value?
			if (   m_bSetValue
				&& (bShowTargetCrossActual == m_bShowTargetCross)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		}
		case IDOSD_ACOUSTIC_ALARM:
		{
			OnUpdateMenuAudibleAlarm();
			break;
		}
		case IDOSD_SYSTEM_VIDEOFORMAT:
		{
			OnUpdateMenuSystemVideoformat();
			break;
		}
		case IDOSD_SYSTEM_VIDEONORM:
		{
			OnUpdateMenuSystemVideonorm();
			break;
		}
		case IDOSD_SYSTEM_AGC:
		{
			OnUpdateMenuSystemAGC();
			break;
		}
		case IDOSD_REALTIME:
		{
			OnUpdateMenuRealtime();
			break;
		}
		case IDOSD_CAMERA_TERMINATION:
		{
			OnUpdateCameraTermination();
			break;
		}
		case IDOSD_CAMERA_RESOLUTION:
		{			
			OnUpdateMenuCameraResolution();
			break;
		}
		case IDOSD_ALARM_DELETE:
		case IDOSD_STOP_ACTIVITY_BACKUP:
		//Löschen von Alarmbildern und Abbruch eines Backups erst bestätigen lassen
		{
			if (!m_bSetValue)
			{
				m_eConfirmingYesNoMode = ConfirmingModeUnknown;
			}

			if(m_eConfirmingYesNoMode == ConfirmingNo)
			{
				sMsg = m_pOSD->GetSubString();
				sTemp.LoadString(IDS_NO);
				sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
			}
			else if(m_eConfirmingYesNoMode == ConfirmingYes)
			{
				sMsg = m_pOSD->GetSubString();
				sTemp.LoadString(IDS_YES);
				sSubMenu.Format(_T("%s: %s"), sMsg, sTemp);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);	
			break;
		}
		case IDOSD_CAMERA_PTZ_TYPE:
			OnUpdateMenuCameraPTZType();
			break;
		case IDOSD_CAMERA_PTZ_ID:
			OnUpdateMenuCameraPTZID();
			break;
		case IDOSD_CAMERA_PTZ_COMPORT:
			OnUpdateMenuCameraPTZComPort();
			break;

		// Audio Settings
		case IDOSD_AUDIO_THRESHOLD:
		case IDOSD_AUDIO_SENSITIVITY:
		case IDOSD_AUDIO_VOLUME:
		{
			nVal = ERROR_VALUE;
			if(pAudio)
			{
				if      (nID == IDOSD_AUDIO_THRESHOLD) 
				{
					nVal = pAudio->GetInputThreshold();
				}
				else if (nID == IDOSD_AUDIO_SENSITIVITY) 
				{
					nVal = pAudio->GetInputSensitivity();
				}
				else if (nID == IDOSD_AUDIO_VOLUME)
				{
					nVal = pAudio->GetOutputVolume();
				}
			}
			
			if (   nVal == ERROR_VALUE
				 || (nID == IDOSD_AUDIO_VOLUME && nVal == 0))
			{
				sMsg.LoadString(IDS_OFF);
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %d%%"), m_pOSD->GetSubString(), nVal);
			}

			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
		} 
		break;
		case IDOSD_AUDIO_CHANGE_CAMNR:
		{
			int nAudioCameraNumber = ERROR_VALUE;
			if(pAudio)
			{
				nAudioCameraNumber = pAudio->GetCameraNumber();
			}
			if (!m_bSetValue)
			{
				m_nAudioCameraNumber = nAudioCameraNumber;
			}
			
			if (m_nAudioCameraNumber == 0)
			{
				sMsg.LoadString(IDS_OFF);
			}
			else if (ExistLiveCamera(m_nAudioCameraNumber-1))
			{
				CLiveWindow* pLiveCam = ExistLiveCamera(m_nAudioCameraNumber-1);
				if(pLiveCam->IsMD()) //only MD cameras allowed, no UVV
				{
					sSubMenu.LoadString(IDS_CAMERA);
					sSubMenu.Replace(_T(":"), _T(""));
					sMsg.Format(_T("%s %d"), sSubMenu, m_nAudioCameraNumber);
				}
				else
				{
					sMsg.LoadString(IDS_OFF);
				}
			}
			else if (ExistPlayCamera(m_nAudioCameraNumber-1))
			{
				CPlayWindow* pPlayCam = ExistPlayCamera(m_nAudioCameraNumber-1);
				if(pPlayCam->IsMD()) //only MD cameras allowed, no UVV
				{
					sSubMenu.LoadString(IDS_CAMERA);
					sSubMenu.Replace(_T(":"), _T(""));
					sMsg.Format(_T("%s %d"), sSubMenu, m_nAudioCameraNumber);
				}
				else
				{
					sMsg.LoadString(IDS_OFF);
				}
			}			
			else
			{
				sMsg.LoadString(IDS_OFF);
			}

			if (   m_bSetValue
				&& (m_nAudioCameraNumber == nAudioCameraNumber)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
		}break;
		case IDOSD_AUDIO_RECORD_MODE:
		{
			int nAudioRecordingMode = AUDIO_RECORDING_UNKNOWN;
			if(pAudio)
			{
				nAudioRecordingMode = pAudio->GetRecordingMode();
			}
			if (!m_bSetValue)
			{
				m_nAudioRecordingMode = nAudioRecordingMode;
			}
			
			if (m_nAudioRecordingMode == AUDIO_RECORDING_OFF)
			{
				sMsg.LoadString(IDS_OFF);
			}
			else if (IsBetween(m_nAudioRecordingMode, AUDIO_RECORDING_A, AUDIO_RECORDING_ALE))
			{
				sMsg.LoadString(IDS_AUDIO_RECORDING_A + m_nAudioRecordingMode - AUDIO_RECORDING_A);
			}
			else
			{
				sMsg.LoadString(IDS_OFF);
//				sMsg = _T("???");
			}

			if (   m_bSetValue
				&& (m_nAudioRecordingMode == nAudioRecordingMode)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
		}break;
		case IDOSD_AUDIO_LISTEN_IN:
			if (!m_bSetValue && pAudio)
			{
				m_bValueOn = pAudio->IsInputOn();
			}
			sMsg.LoadString(pAudio && m_bValueOn ? IDS_ON : IDS_OFF);
			if (   m_bSetValue
				 && pAudio
				 && pAudio->IsInputOn() == m_bValueOn)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
			break;
		default:
			SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
	}
	
	SetDisplayText(m_ctrlDisplayMenu1, m_pOSD->GetTitleString(), 0, 0);
	ChangeDisplayColor();

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::UpdateMenu()
{
    if (IsWindow(m_hWnd))
	{
		PostMessage(WM_UPDATE_MENU,1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::UpdateButtons()
{
    if (IsWindow(m_hWnd))
	{
		PostMessage(WM_UPDATE_BUTTONS,1);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetTimer(const CString &sTimer)
{
	BOOL bResult = FALSE;

	if (ValidateTimer(sTimer))
	{
		CMainFrame*pMF = theApp.GetMainFrame();
		if (pMF)
		{
			CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
			if (pLiveWnd)
			{
				bResult = pLiveWnd->SetTimer(m_nDayOfWeek, sTimer);
			}
		}
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnIncrementMaskSensitivity()
{
	m_nMDMaskLevel++;
	if (m_nMDMaskLevel > MD_SENSITIVITY_HIGH)
		m_nMDMaskLevel = MD_SENSITIVITY_OFF;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnIncrementAlarmSensitivity()
{
	m_nMDAlarmLevel++;
	if (m_nMDAlarmLevel > MD_SENSITIVITY_HIGH)
		m_nMDAlarmLevel = MD_SENSITIVITY_LOW;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDecrementMaskSensitivity()
{
	m_nMDMaskLevel--;
	if (m_nMDMaskLevel < MD_SENSITIVITY_OFF)
		m_nMDMaskLevel = MD_SENSITIVITY_HIGH;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDecrementAlarmSensitivity()
{
	m_nMDAlarmLevel--;
	if (m_nMDAlarmLevel < MD_SENSITIVITY_LOW)
		m_nMDAlarmLevel = MD_SENSITIVITY_HIGH;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnIncrementAlarmCallEvent()
{
	BOOL bIsMD = TRUE;
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CLiveWindow* pLiveWnd = NULL;
		pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bIsMD = pLiveWnd->IsMD();
		}
	}

	m_nAlarmCallEvent++;

	//bei Alarmkameras nur die externen Alarme anzeigen
	if(    !bIsMD
		&& m_nAlarmCallEvent >= ALARMCALL_EVENT_A
		&& m_nAlarmCallEvent <= ALARMCALL_EVENT_A12)
	{
		m_nAlarmCallEvent = ALARMCALL_EVENT_1;
	}

	if (m_nAlarmCallEvent > ALARMCALL_EVENT_12)
		m_nAlarmCallEvent = ALARMCALL_EVENT_OFF;
	
	//bei Kameras, die zu einer Savic gehören, gibt es den Ext2 - Alarmeingang nicht
	if(    m_nAlarmCallEventByCardType == CARD_TYPE_SAVIC
		|| m_nAlarmCallEventByCardType == CARD_TYPE_TASHA)
	{
		if(    m_nAlarmCallEvent == ALARMCALL_EVENT_A2
		   ||  m_nAlarmCallEvent == ALARMCALL_EVENT_A12)

		{
			m_nAlarmCallEvent = ALARMCALL_EVENT_1;
		}	

		if(    m_nAlarmCallEvent == ALARMCALL_EVENT_2
		   ||  m_nAlarmCallEvent == ALARMCALL_EVENT_12)
		{
			m_nAlarmCallEvent = ALARMCALL_EVENT_OFF;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDecrementAlarmCallEvent()
{	
	BOOL bIsMD = TRUE;
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CLiveWindow* pLiveWnd = NULL;
		pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bIsMD = pLiveWnd->IsMD();
		}
	}

	m_nAlarmCallEvent--;
	
	//bei Alarmkameras nur die externen Alarme anzeigen
	if(    !bIsMD
		&& m_nAlarmCallEvent >= ALARMCALL_EVENT_A
		&& m_nAlarmCallEvent <= ALARMCALL_EVENT_A12)
	{
		m_nAlarmCallEvent = ALARMCALL_EVENT_OFF;
	}

	if (m_nAlarmCallEvent < ALARMCALL_EVENT_OFF)
		m_nAlarmCallEvent = ALARMCALL_EVENT_12;

	//bei Kameras, die zu einer Savic gehören, gibt es den Ext2 - Alarmeingang nicht
	if (   m_nAlarmCallEventByCardType == CARD_TYPE_SAVIC
		|| m_nAlarmCallEventByCardType == CARD_TYPE_TASHA)
	{
		if(    m_nAlarmCallEvent == ALARMCALL_EVENT_A2
		   ||  m_nAlarmCallEvent == ALARMCALL_EVENT_A12
		   ||  m_nAlarmCallEvent == ALARMCALL_EVENT_1)

		{
			m_nAlarmCallEvent = ALARMCALL_EVENT_A1;
		}	

		if(    m_nAlarmCallEvent == ALARMCALL_EVENT_2
		   ||  m_nAlarmCallEvent == ALARMCALL_EVENT_12)
		{
			m_nAlarmCallEvent = ALARMCALL_EVENT_1;
		}

		if (m_nAlarmCallEvent < ALARMCALL_EVENT_OFF)
			m_nAlarmCallEvent = ALARMCALL_EVENT_1;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuSequencerDwellTime()
{
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CString sValue = _T("???");
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if (!m_bSetValue)
		{
			if (pLiveWnd && pLiveWnd->GetSequencerDwellTime(m_iSequencerDwellTime))
			{
				if (m_iSequencerDwellTime == 0)
				{
					sValue.LoadString(IDS_OFF);
				}
				else
				{
					sValue.Format(_T("%d"), m_iSequencerDwellTime);
				}
			}
			else
			{
				sValue = _T("???");
			}
		}
		else
		{
			int iDwellTime;
			if (pLiveWnd && pLiveWnd->GetSequencerDwellTime(iDwellTime))
			{
				if (   (m_iSequencerDwellTime < 0)
					|| (m_iSequencerDwellTime > MAX_SEQUENCER_DWELL_TIME)
					)
				{
					sValue = _T("???");
				}
				else
				{
					if (m_iSequencerDwellTime == 0)
					{
						sValue.LoadString(IDS_OFF);
					}
					else
					{
						sValue.Format(_T("%d"), m_iSequencerDwellTime);
					}
					if (iDwellTime == m_iSequencerDwellTime)
					{
						sValue = _T("*") + sValue;
					}
				}
			}
		}
		CString sSubMenu;
		sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sValue);
		SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnIncrementSequencerDwellTime()
{
	m_iSequencerDwellTime++;
	if (m_iSequencerDwellTime > MAX_SEQUENCER_DWELL_TIME)
		m_iSequencerDwellTime = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDecrementSequencerDwellTime()
{
	m_iSequencerDwellTime--;
	if (m_iSequencerDwellTime < 0)
		m_iSequencerDwellTime = MAX_SEQUENCER_DWELL_TIME;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnGetSequencerDwellTime(int& iSeconds)
{
	BOOL bResult = FALSE;

	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CLiveWindow* pLiveWnd = pMF->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->GetSequencerDwellTime(iSeconds);
		}
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetSequencerDwellTime()
{
	BOOL bResult = FALSE;

	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			bResult = pLiveWnd->SetSequencerDwellTime(m_iSequencerDwellTime);
		}
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuVideoOutputMode(int iIndex)
{
	CString sMode = _T("???");
	if (!m_bSetValue)
	{
		if (GetVideoOutputMode(iIndex, m_eVideoOutputMode))
		{
			sMode = GetVideoOutputModeName(m_eVideoOutputMode);
		}
	}
	else
	{						
		enumVideoOutputMode eVideoOutputMode;
		if (GetVideoOutputMode(iIndex, eVideoOutputMode))
		{
			if (   (eVideoOutputMode != VOUT_UNKNOWN)
				&& (eVideoOutputMode == m_eVideoOutputMode)
				)
			{
				sMode = _T("*") + GetVideoOutputModeName(m_eVideoOutputMode);
			}
			else
			{
				sMode = GetVideoOutputModeName(m_eVideoOutputMode);
			}
		}
	}
	CString sSubMenu;
	sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMode); 
	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::IncrementVideoOutputMode()
{
	if (m_eVideoOutputMode == VOUT_ACTIVITY)
	{
		m_eVideoOutputMode = VOUT_CLIENT;
	}
	else if (m_eVideoOutputMode == VOUT_CLIENT)
	{
		m_eVideoOutputMode = VOUT_SEQUENCE;
	}
	else if (m_eVideoOutputMode == VOUT_SEQUENCE)
	{
		m_eVideoOutputMode = VOUT_ACTIVITY;
	}
	else
	{
		m_eVideoOutputMode = VOUT_ACTIVITY;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::DecrementVideoOutputMode()
{
	if (m_eVideoOutputMode == VOUT_SEQUENCE)
	{
		m_eVideoOutputMode = VOUT_CLIENT;
	}
	else if (m_eVideoOutputMode == VOUT_CLIENT)
	{
		m_eVideoOutputMode = VOUT_ACTIVITY;
	}
	else if (m_eVideoOutputMode == VOUT_ACTIVITY)
	{
		m_eVideoOutputMode = VOUT_SEQUENCE;
	}
	else
	{
		m_eVideoOutputMode = VOUT_SEQUENCE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetVideoOutputMode(int iIndex, enumVideoOutputMode& eVideoOutputMode)
{
	BOOL bResult = FALSE;

	CString sCSDPort;
	switch (iIndex)
	{
		case 0:	sCSDPort = CSD_PORT0;	break;
		case 1:	sCSDPort = CSD_PORT1;	break;
		case 2:	sCSDPort = CSD_PORT2;	break;
		case 3:	sCSDPort = CSD_PORT3;	break;
		default:						break;
	}
	if (!sCSDPort.IsEmpty())
	{
		CServer* pServer = theApp.GetServer();
		if (pServer)
		{
			bResult = pServer->GetVideoOutputMode(sCSDPort, eVideoOutputMode);
		}
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetVideoOutputMode(int iIndex)
{
	BOOL bResult = FALSE;

	CString sCSDPort;
	switch (iIndex)
	{
		case 0:	sCSDPort = CSD_PORT0;	break;
		case 1:	sCSDPort = CSD_PORT1;	break;
		case 2:	sCSDPort = CSD_PORT2;	break;
		case 3:	sCSDPort = CSD_PORT3;	break;
		default:						break;
	}
	if (!sCSDPort.IsEmpty())
	{
		CServer* pServer = theApp.GetServer();
		if (pServer)
		{
			bResult = pServer->SetVideoOutputMode(sCSDPort, m_eVideoOutputMode);
		}
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
CString CPanel::GetVideoOutputModeName(enumVideoOutputMode eVideoOutputMode)
{
	CString sName;
	switch (eVideoOutputMode)
	{
		case VOUT_ACTIVITY:	sName.LoadString(IDS_VOUT_MODE_ACTIVITY);	break;
		case VOUT_CLIENT:	sName.LoadString(IDS_VOUT_MODE_CLIENT);		break;
		case VOUT_SEQUENCE:	sName.LoadString(IDS_VOUT_MODE_SEQUENCER);	break;
		default:			sName = _T("???");								break;
	}
	return sName;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuAudibleAlarm()
{
	CString sMsg;
	BOOL bAudibleAlarmActual = theApp.AudibleAlarm();
	if (!m_bSetValue)
	{
		m_bAudibleAlarm = bAudibleAlarmActual;
	}
	if (m_bAudibleAlarm)
		sMsg.LoadString(IDS_ON);
	else
		sMsg.LoadString(IDS_OFF);

	CString sSubMenu;
	// Actual value?
	if (   m_bSetValue
		&& (bAudibleAlarmActual == m_bAudibleAlarm)
		)
	{
		sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
	}
	else
	{
		sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
	}
	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuRealtime()
{
	CString sMsg;
	BOOL bRealtimeActual = theApp.CanRealtime();
	if (!m_bSetValue)
	{
		m_bRealtimePanel = bRealtimeActual;
	}
	if (m_bRealtimePanel)
		sMsg.LoadString(IDS_ON);
	else
		sMsg.LoadString(IDS_OFF);

	CString sSubMenu;
	// Actual value?
	if (   m_bSetValue
		&& (bRealtimeActual == m_bRealtimePanel)
		)
	{
		sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
	}
	else
	{
		sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
	}
	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuCameraResolution()
{
	CString sMsg;
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		int nResolution = m_nResolution;
		BOOL bResult = FALSE;
		if(!m_bSetValue)
		{
			bResult = pServer->GetResolution(nResolution);
		}

		if (!m_bSetValue && bResult)
		{
			m_nResolution = nResolution;
			m_nCurrentResolution = nResolution;
		}

		CMainFrame*	pMainWnd = theApp.GetMainFrame();
		int nCardType = CARD_TYPE_UNKNOWN;
		if (pMainWnd)
		{
			CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
			if (pLiveWnd)
			{
				nCardType = pLiveWnd->GetAlarmCallEventByCardType();
			}
		}

		if ((nCardType == CARD_TYPE_Q) || (nCardType == CARD_TYPE_TASHA))
		{
			if (m_nResolution == RESOLUTION_4CIF)
			{
				sMsg.LoadString(IDS_HIGH);
				sMsg += _T(" (4CIF)");
			}
			else if(m_nResolution == RESOLUTION_2CIF)
			{
				sMsg.LoadString(IDS_NORMAL);
				sMsg += _T(" (2CIF)");
			}
			else if(m_nResolution == RESOLUTION_CIF)
			{
				sMsg.LoadString(IDS_LOW);
				sMsg += _T(" (CIF)");
			}
			else
				sMsg.LoadString(IDS_ERROR);
		}
		else
		{
			if (   m_nResolution == RESOLUTION_4CIF
				|| m_nResolution == RESOLUTION_2CIF)
				sMsg.LoadString(IDS_HIGH);
			else if(   m_nResolution == RESOLUTION_CIF
				    || m_nResolution == RESOLUTION_QCIF)
				sMsg.LoadString(IDS_LOW);
			else
				sMsg.LoadString(IDS_ERROR);
		}

		CString sSubMenu;
		// Actual value?
		if (   m_bSetValue
			&& (nResolution == m_nCurrentResolution)
			)
		{
			sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
		}
		else
		{
			sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
		}

		SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuCameraCompression()
{
	CString sMsg;
			
	CMainFrame* pMainWnd = NULL;
	pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CString sSubMenu;
		CLiveWindow* pLiveWnd = NULL;

		pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if(pLiveWnd)
		{
			int nCompression = m_nCompression;
			BOOL bResult = FALSE;
			if(!m_bSetValue)
			{
				bResult = GetCompression(nCompression);	
			}

			if (!m_bSetValue && bResult)
			{
				m_nCompression = nCompression;
				m_nCurrentCompression = nCompression;
			}
			if (m_nCompression == COMPRESSION_1)
				sMsg.LoadString(IDS_LOW);
			else if (   m_nCompression == COMPRESSION_2)
				sMsg.LoadString(IDS_NORMAL);
			else if (m_nCompression == COMPRESSION_3)
				sMsg.LoadString(IDS_HIGH);
			else
				sMsg.LoadString(IDS_ERROR);

			// Actual value?
			if (   m_bSetValue
				&& (nCompression == m_nCurrentCompression)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
		}
		else
		{
			sSubMenu.Format(_T("%s: ??"), m_pOSD->GetSubString());
		}

		SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);	
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuCameraFPS()
{
	CString sMsg;

	CMainFrame* pMainWnd = NULL;
	pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CString sSubMenu;
		CLiveWindow* pLiveWnd = NULL;

		pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if(pLiveWnd)
		{
			int nCameraFPS = m_nCameraFPS;
			BOOL bResult = FALSE;
			if(!m_bSetValue)
			{
				bResult = GetCameraFPS(nCameraFPS);	
			}

			if (!m_bSetValue && bResult)
			{
				m_nCameraFPS = nCameraFPS;
				m_nCurrentCameraFPS = nCameraFPS;
			}

			
			int i = 0;

			for (i = 0; i < CAMERA_FPS_STEPS; i++)
			{
				if(m_nCameraFPS == g_iaCameraFPS[i])
				{
					sMsg.Format(_T("%d"), g_iaCameraFPS[i]);
					break;
				}
			}

			if(sMsg.IsEmpty())
			{
				m_nCameraFPS = g_iaCameraFPS[0]; //init with 1 fps
				sMsg.LoadString(IDS_ERROR);
			}

			// Actual value?
			if (   m_bSetValue
				&& (nCameraFPS == m_nCurrentCameraFPS)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
		}
		else
		{
			sSubMenu.Format(_T("%s: ??"), m_pOSD->GetSubString());
		}

		SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);	
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuCameraPTZType()
{
	CMainFrame* pMainWnd = NULL;
	pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CString	sSubMenu = m_pOSD->GetSubString();
		CString sMsg = _T("???");
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if(pLiveWnd)
		{
			int nCameraPTZType = m_nCameraPTZType;
			if(!m_bSetValue)
			{
				if (GetCameraPTZType(nCameraPTZType))
				{
					m_nCameraPTZType = nCameraPTZType;
					m_nCurrentCameraPTZType = m_nCameraPTZType;
					sMsg = NameOfEnumPTZRealName((CameraControlType)(m_nCameraPTZType));
				}
				else
				{
					sMsg = _T("???");
				}
			}
			else
			{
				sMsg = NameOfEnumPTZRealName((CameraControlType)(m_nCameraPTZType));
			}
			if (sMsg.IsEmpty())
			{
				sMsg.LoadString(IDS_CAMERA_FIX);
			}
			// Actual value?
			if (   m_bSetValue
				&& (nCameraPTZType == m_nCurrentCameraPTZType)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
		}
		else
		{
			sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
		}
		SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);	
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuCameraPTZID()
{
	// PTZ ID ist abhängig vom PTZ Typ
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CString sMsg;
		CString	sSubMenu = m_pOSD->GetSubString();
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if(pLiveWnd)
		{
			if (m_eInputState != InputCameraPTZID)
			{
				sMsg = _T("???");
				DWORD dwCameraPTZID = 0;
				if (GetCameraPTZID(dwCameraPTZID))
				{
					m_dwCameraPTZID = dwCameraPTZID;
					sMsg.Format(_T("%u"), m_dwCameraPTZID);
				}
			}
			else
			{
				sMsg.Format(_T("%u"), m_dwCameraPTZID);
			}
		}
		SetFormatDisplayText(m_ctrlDisplayMenu2, sMsg, m_pOSD->GetSubString());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuCameraPTZComPort()
{
	// PTZ ComPort ist abhängig vom Typ
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CString sMsg;
		CString	sSubMenu = m_pOSD->GetSubString();
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if(pLiveWnd)
		{
			sMsg = _T("???");
			int iCameraPTZComPort = 0;
			if(!m_bSetValue)
			{
				if (GetCameraPTZComPort(iCameraPTZComPort))
				{
					m_nCameraPTZComPort = iCameraPTZComPort;
					m_nCurrentCameraPTZComPort = m_nCameraPTZComPort;
					sMsg.Format(_T("%d"), m_nCameraPTZComPort);
				}
			}
			else
			{
				sMsg.Format(_T("%d"), m_nCameraPTZComPort);
			}
			// Actual value?
			if (   m_bSetValue
				&& (m_nCameraPTZComPort == m_nCurrentCameraPTZComPort)
				)
			{
				sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
			}
			else
			{
				sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
			}
		}
		else
		{
			sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
		}
		SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);	
//		SetFormatDisplayText(m_ctrlDisplayMenu2, sMsg, m_pOSD->GetSubString());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeAudibleAlarm()
{
 	m_bAudibleAlarm = !m_bAudibleAlarm;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeRealtime()
{
 	m_bRealtimePanel = !m_bRealtimePanel;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeCameraResolution(BOOL bIncreaseItem)
{
	CMainFrame*	pMainWnd = theApp.GetMainFrame();
	int nCardType = CARD_TYPE_UNKNOWN;
	if (pMainWnd)
	{
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			nCardType = pLiveWnd->GetAlarmCallEventByCardType();
		}
	}

	if ((nCardType == CARD_TYPE_Q) || (nCardType == CARD_TYPE_TASHA))
	{
		if(bIncreaseItem)
		{
			if (m_nResolution == RESOLUTION_4CIF)
				m_nResolution = RESOLUTION_CIF;
			else if(m_nResolution == RESOLUTION_2CIF)
				m_nResolution = RESOLUTION_4CIF;
			else if(m_nResolution == RESOLUTION_CIF)
				m_nResolution = RESOLUTION_2CIF;
			else
				m_nResolution = RESOLUTION_2CIF;
		}
		else
		{
			if (m_nResolution == RESOLUTION_4CIF)
				m_nResolution = RESOLUTION_2CIF;
			else if(m_nResolution == RESOLUTION_2CIF)
				m_nResolution = RESOLUTION_CIF;
			else if(m_nResolution == RESOLUTION_CIF)
				m_nResolution = RESOLUTION_4CIF;
			else
				m_nResolution = RESOLUTION_2CIF;
		}
	}
	else
	{
		//alles ausser Q-Unit
		m_nResolution =  m_nResolution == RESOLUTION_2CIF ? RESOLUTION_QCIF : RESOLUTION_2CIF;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeCameraCompression(BOOL bIncreaseItem)
{

	if(bIncreaseItem)
	{
		if(m_nCompression == COMPRESSION_1)
		{
			m_nCompression = COMPRESSION_2;
		}
		else if(m_nCompression == COMPRESSION_2)
		{
			m_nCompression = COMPRESSION_3;
		}
		else if(m_nCompression == COMPRESSION_3)
		{
			m_nCompression = COMPRESSION_1;
		}
		else
		{
			m_nCompression = COMPRESSION_1;
		}
	}
	else
	{
		if(m_nCompression == COMPRESSION_1)
		{
			m_nCompression = COMPRESSION_3;
		}
		else if(m_nCompression == COMPRESSION_2)
		{
			m_nCompression = COMPRESSION_1;
		}
		else if(m_nCompression == COMPRESSION_3)
		{
			m_nCompression = COMPRESSION_2;
		}
		else
		{
			m_nCompression = COMPRESSION_1;
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeCameraFPS(BOOL bIncreaseItem)
{
	int i = 0;
	if(bIncreaseItem)
	{
		for (i = 0; i < CAMERA_FPS_STEPS; i++)
		{
			if(m_nCameraFPS == g_iaCameraFPS[i])
			{
				if(i==CAMERA_FPS_STEPS-1)
				{
					m_nCameraFPS = g_iaCameraFPS[0];
				}
				else
				{
					m_nCameraFPS = g_iaCameraFPS[i+1];
				}
				break;
			}
		}
	}
	else
	{
		for (i = 0; i < CAMERA_FPS_STEPS; i++)
		{
			if(m_nCameraFPS == g_iaCameraFPS[i])
			{
				if(i==0)
				{
					m_nCameraFPS = g_iaCameraFPS[CAMERA_FPS_STEPS-1];
				}
				else
				{
					m_nCameraFPS = g_iaCameraFPS[i-1];
				}
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDecrementCameraPTZType()
{
	m_nCameraPTZType--;
	if (m_nCameraPTZType < CCT_UNKNOWN)
	{
		m_nCameraPTZType = GetLastCameraControlType(FALSE);
	}
	else if(m_nCameraPTZType == CCT_RELAY_PTZ)
	{
		m_nCameraPTZType--;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnIncrementCameraPTZType()
{
	m_nCameraPTZType++;
	if (m_nCameraPTZType > GetLastCameraControlType(FALSE))
	{
		m_nCameraPTZType = CCT_UNKNOWN;
	}
	else if(m_nCameraPTZType == CCT_RELAY_PTZ)
	{
		m_nCameraPTZType++;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDecrementCameraPTZComPort()
{
	DWORD dwComBit = dwComBit=1<<(m_nCameraPTZComPort-1),
		  dwMask   = GetCommPortInfo();
	if (dwMask)
	{
		for (m_nCameraPTZComPort--; ; m_nCameraPTZComPort--)
		{
			if (m_nCameraPTZComPort<1)
			{
				m_nCameraPTZComPort = 33;
			}
			else
			{
				dwComBit=1<<(m_nCameraPTZComPort-1);
				if (dwMask&dwComBit)
				{
					break;
				}
			}
		}
	}
	else
	{
		m_nCameraPTZComPort = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnIncrementCameraPTZComPort()
{
	DWORD dwComBit = dwComBit=1<<(m_nCameraPTZComPort-1),
		  dwMask   = GetCommPortInfo();
	if (dwMask)
	{
		for (m_nCameraPTZComPort++; ;m_nCameraPTZComPort++)
		{
			if (m_nCameraPTZComPort>32)
			{
				m_nCameraPTZComPort = 0;
			}
			else
			{
				dwComBit=1<<(m_nCameraPTZComPort-1);
				if (dwMask&dwComBit)
				{
					break;
				}
			}
		}
	}
	else
	{
		m_nCameraPTZComPort = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangePIN(BOOL bSupervisor)
{
	EnableInputMode();
	if (m_bSetValue)
	{
		CString sMsg;
		if (m_eInputState == InputNo)
		{
			m_eInputState = InputOldPIN;
			sMsg.LoadString(IDS_OLD_PIN);
			SetFormatDisplayText(m_ctrlDisplayMenu2, _T("____"), sMsg);
		}
		else if (m_eInputState ==  InputOldPIN)
		{
			CServer* pServer = theApp.GetServer();
			BOOL bPinOK = FALSE;
			if (pServer)
			{
				if (bSupervisor)
				{
					bPinOK = (   (m_sInput.IsEmpty() == FALSE)
							  && (m_sInput == pServer->GetSupervisorPIN())
							  );
				}
				else
				{
					bPinOK = (   (m_sInput.IsEmpty() == FALSE)
							  && (m_sInput == pServer->GetOperatorPIN())
							  );
				}
			}
			if (bPinOK)
			{
				m_eInputState = InputNewPIN;
				sMsg.LoadString(IDS_NEW_PIN);
				SetFormatDisplayText(m_ctrlDisplayMenu2, _T("____"), sMsg);
			}
			else
			{
				sMsg.LoadString(IDS_PIN_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_OLD_PIN);
				SetFormatDisplayText(m_ctrlDisplayMenu2, _T("____"), sMsg);
			}
		}
		else if (m_eInputState == InputNewPIN)
		{
			if (m_sInput.GetLength() == 4)
			{
				m_eInputState = InputConfirmPIN;
				sMsg.LoadString(IDS_CONFIRM_PIN);
				SetFormatDisplayText(m_ctrlDisplayMenu2, _T("____"), sMsg);
				m_sNewPIN = m_sInput;
			}
			else
			{
				sMsg.LoadString(IDS_PIN_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_NEW_PIN);
				SetFormatDisplayText(m_ctrlDisplayMenu2, _T("____"), sMsg);
			}
		}
		else if (m_eInputState == InputConfirmPIN)
		{
			BOOL bPinChanged = FALSE;
			if (m_sInput == m_sNewPIN)
			{
				if (bSupervisor)
				{
					bPinChanged = (OnChangeSupervisorPIN(m_sNewPIN));
				}
				else
				{
					bPinChanged = (OnChangeOperatorPIN(m_sNewPIN));
				}
			}
			if (bPinChanged)
			{
				sMsg.LoadString(IDS_PIN_CHANGED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);				
				WaitToReadInfoMessage();
				sMsg = m_pOSD->GetSubString();
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);

				DisableInputMode();
			}
			else
			{
				sMsg.LoadString(IDS_PIN_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);			
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_CONFIRM_PIN);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			}
		}
		m_sInput = _T("");
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnChangeSupervisorPIN(const CString &sPIN)
{
	BOOL bResult = FALSE;

	// die PIN im DVProcess ändern
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->SetSupervisorPIN(sPIN))
		{
			bResult = TRUE;
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnChangeOperatorPIN(const CString &sPIN)
{
	BOOL bResult = FALSE;

	// die PIN im DVProcess ändern
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->SetOperatorPIN(sPIN))
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBeginConfig()
{
	// Dem Server mitteilen, das das Configmenü mit Supervisorrechten betreten wurde.
	CServer* pServer = theApp.GetServer();
	if (pServer)
		pServer->OnBeginConfig();

	m_pOSD = &m_OSDConfig;
	m_pOSD->SetMenuPos(IDOSD_COLOR_BRIGHTNESS);
	SetDefaultSubMenu();

	OnUpdateMenu(0,0);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnEndConfig()
{
	// Dem Server mitteilen, das das Configmenü verlassen wurde.
	CServer* pServer = theApp.GetServer();
	if (pServer)
		pServer->OnEndConfig();

	CString sMsg;
	sMsg.LoadString(IDS_END_OPTION);
	SetDisplayText(m_ctrlDisplayMenu2, sMsg, 0, 0);
	WaitToReadInfoMessage();
	m_pOSD = &m_OSDNormal;

	OnUpdateMenu(0,0);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnPINEntered(const CString &sPIN)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		DWORD dwPIN = 0x0000ffff;
 		if (sPIN.GetLength() == 4)
		{
			_stscanf(sPIN,_T("%hu"),&dwPIN);
		}
		pMF->OnPINEntered((WORD)dwPIN);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnShutdownResetPINEntered(const CString &sPIN)
{
	if ((m_eInputState == InputShutdownPIN) || ((m_eInputState == InputResetPIN)))
	{
		CString sMsg;
		CServer* pServer = theApp.GetServer();
		if (pServer && (m_sInput == pServer->GetSupervisorPIN()))
		{
			sMsg.LoadString(IDS_PIN_SUCCESS);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			
			WaitToReadInfoMessage();
			if (m_eInputState == InputShutdownPIN)
			{
				WK_TRACE(_T("CPanel::OnShutdownResetPINEntered\n"));
				DoShutdown();
			}
			if (m_eInputState == InputResetPIN)
			{
				DoReboot();
			}
			DisableInputMode();	
		}
		else
		{
			sMsg.LoadString(IDS_PIN_FAILED);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			WaitToReadErrorMessage();
			
			sMsg.LoadString(IDS_PIN);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			m_sInput = _T("");
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CPanel::LoadConfiguration()
{
	if (!theApp.IsReadOnly())
	{
		CWK_Profile prof;

		// Power-Button anzeigen?
		m_bShowPower = prof.GetInt(REGKEY_DVCLIENT, _T("ShowPower"), m_bShowPower);

		// Default Kamera laden
		m_nCamNr = prof.GetInt(REGKEY_DVCLIENT, REGKEY_DEFAULTCAM, m_nCamNr);

		// Default Viewmode laden
		m_eViewMode = (ViewMode)prof.GetInt(REGKEY_DVCLIENT, _T("DefaultViewMode"), m_eViewMode);
		if (   (m_eViewMode != ViewFull)
			&& (m_eViewMode != View1pX)
			&& (m_eViewMode != ViewXxX)
			)
		{
			m_eViewMode = View1pX;
		}
		
		// MSN laden
		m_sMSN = GetMSN();

		// Ist Netzwerk vorhanden und im Dongle freigeschaltet?
		m_bNetAllowedByDongle	= prof.GetInt(REGKEY_DVSTARTER, _T("Net"),  m_bNetAllowedByDongle);
		
		// Ist ISDN vorhanden und im Dongle freigeschaltet?
		m_bISDNAllowedByDongle = prof.GetInt(REGKEY_DVSTARTER, _T("ISDN"), m_bISDNAllowedByDongle);
		
		//beim Reciever muss Netzwerk immer freigeschaltet sein, da ansonsten die Rufnummern für einen
		//Alarmfall auf der Gegenstation nicht eingetragen werden können.
		//Grund: Im IP-Receiver fehlen die Registry-Einträge "Net" und "ISDN" im DVStarter.
		if(theApp.IsReceiver())
		{
			m_bNetAllowedByDongle = TRUE;	
		}

		// Ist Backup auf DVD, CD, Memorystick, etc... im Dongle freigeschaltet
		if (WK_IS_RUNNING(WK_APP_NAME_JACOBUNIT1))
		{
			m_bBackUpAllowedByDongle		= prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("BackUp"), m_bBackUpAllowedByDongle);
			m_bBackUpToDVDAllowedByDongle	= prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("BackUpToDVD"), m_bBackUpToDVDAllowedByDongle);
		}
		else if (WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT1))
		{
			m_bBackUpAllowedByDongle		= prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("BackUp"), m_bBackUpAllowedByDongle);
			m_bBackUpToDVDAllowedByDongle	= prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("BackUpToDVD"), m_bBackUpToDVDAllowedByDongle);
		}		
		else if (WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT1))
		{
			m_bBackUpAllowedByDongle		= prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("BackUp"), m_bBackUpAllowedByDongle);
			m_bBackUpToDVDAllowedByDongle	= prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("BackUpToDVD"), m_bBackUpToDVDAllowedByDongle);
		}		
		else if (WK_IS_RUNNING(WK_APP_NAME_QUNIT))
		{
			m_bBackUpAllowedByDongle		= prof.GetInt(_T("QUnit\\EEProm"), _T("BackUp"), m_bBackUpAllowedByDongle);
			m_bBackUpToDVDAllowedByDongle	= prof.GetInt(_T("QUnit\\EEProm"), _T("BackUpToDVD"), m_bBackUpToDVDAllowedByDongle);
		}	

		// Wenn das Backup auf DVD erlaubt ist, soll auf jedenfall auch das Backup auf CD erlaubt sein! 
		if (m_bBackUpToDVDAllowedByDongle)
			m_bBackUpAllowedByDongle = TRUE;

		// OEM einlesen
		m_eOem = (OEM)prof.GetInt(REGKEY_DVCLIENT, REGKEY_OEM,  (int)m_eOem);

		m_nCameras = prof.GetInt(_T("DV\\Process"), _T("NrOfCameras"), m_nCameras);
	
		// Soll das Fadenkreuz angezeigt werden?
		m_bShowTargetCross = prof.GetInt(REGKEY_DVCLIENT, _T("ShowCross"), m_bShowTargetCross);
		SetTargetDisplayState();


		m_bFloppyExportOnlySupervisor = prof.GetInt(REGKEY_DVCLIENT,_T("FloppyExportOnlySupervisor"),m_bFloppyExportOnlySupervisor);
		
		// Sprache laden
		// CAVEAT: LoadLanguageDLL speichert die Einstellungen!
		EnumerateLanguages();
		CString sDefaultLanguage = prof.GetString(REGKEY_DVCLIENT, _T("Language"), szDefaultLanguageEnu);
		LoadDefaultLanguageDll(sDefaultLanguage);

		LoadHostList();

		// Defaultwerte speichern.
		SaveConfiguration();
	}
	else
	{
		/* Read only. Einstellungen aus der Autorun.inf lesen  */
		CString sAutorun;
		GetModuleFileName(theApp.m_hInstance,sAutorun.GetBuffer(_MAX_PATH),_MAX_PATH);
		sAutorun.ReleaseBuffer();
		int p = sAutorun.ReverseFind(_T('\\'));
		sAutorun = sAutorun.Left(p+1) + _T("autorun.inf");
		
		m_eOem = (OEM)GetPrivateProfileInt(REGKEY_SETTINGS,REGKEY_OEM,(int)m_eOem,sAutorun);
		m_nCameras = GetPrivateProfileInt(REGKEY_SETTINGS,_T("NrOfCam"),(int)m_nCameras,sAutorun);

		// Sprache laden
		EnumerateLanguages();
		CString sDefaultLanguage; 
		GetPrivateProfileString(REGKEY_SETTINGS, _T("Language"), szDefaultLanguageEnu,
								sDefaultLanguage.GetBuffer(_MAX_PATH),_MAX_PATH,sAutorun);
		sDefaultLanguage.ReleaseBuffer();
		LoadDefaultLanguageDll(sDefaultLanguage);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SaveConfiguration()
{
	if (!theApp.IsReadOnly())
	{
		CWK_Profile prof;

		prof.WriteInt(REGKEY_DVCLIENT, REGKEY_DEFAULTCAM, m_nCamNr);
		prof.WriteInt(REGKEY_DVCLIENT, _T("DefaultViewMode"), m_eViewMode);
		prof.WriteInt(REGKEY_DVCLIENT, _T("ShowPower"), m_bShowPower);
		prof.WriteInt(REGKEY_DVCLIENT, REGKEY_OEM,  (int)m_eOem);
		prof.WriteInt(REGKEY_DVCLIENT, _T("ShowCross"), m_bShowTargetCross);
		prof.WriteString(REGKEY_DVCLIENT, _T("Language"), m_pDefaultLanguage->GetLanguage());
		prof.WriteInt(REGKEY_DVCLIENT,_T("FloppyExportOnlySupervisor"),m_bFloppyExportOnlySupervisor);

		
		CString sMSN = m_sMSN;
		if (sMSN.IsEmpty())
			sMSN = _T("A");

		CWK_Profile::Encode(sMSN);
#ifdef _UNICODE
		prof.WriteString(REGKEY_ISDNUNIT, REGKEY_OWNNUMBER, sMSN,TRUE);	
		prof.WriteString(REGKEY_ISDNUNIT2, REGKEY_OWNNUMBER, sMSN,TRUE);	
#else
		prof.WriteString(REGKEY_ISDNUNIT, REGKEY_OWNNUMBER, sMSN);	
		prof.WriteString(REGKEY_ISDNUNIT2, REGKEY_OWNNUMBER, sMSN);	
#endif

		WK_TRACE(_T("Save Configuration\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::ExtractAndValidateDate(const CString &sD, int &nDay, int &nMonth, int &nYear)
{
	WORD nDaysPerMonth[4][13] =	{ 
									{0,31,29,31,30,31,30,31,31,30,31,30,31},
									{0,31,28,31,30,31,30,31,31,30,31,30,31},
									{0,31,28,31,30,31,30,31,31,30,31,30,31},
									{0,31,28,31,30,31,30,31,31,30,31,30,31}
								};

	CString sDate = sD;
	if (sDate.IsEmpty())
	{
		CSystemTime t;
		t.GetLocalTime();
		sDate.Format(_T("%02d%02d%02d"), t.GetDay(), t.GetMonth(), t.GetYear() % 100);
		
		if (theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
		{
			return FALSE;
		}
	}
	else if (sDate.GetLength() < 6)
	{
		WK_TRACE_WARNING(_T("Date too short\n"));
		return FALSE;
	}

	if (m_bDDMMYY)
	{
		nDay	= _ttoi(sDate.Mid(0,2));
		nMonth	= _ttoi(sDate.Mid(2,2));
		nYear	= _ttoi(sDate.Mid(4,2));
	}
	else
	{
		nMonth	= _ttoi(sDate.Mid(0,2));
		nDay	= _ttoi(sDate.Mid(2,2));
		nYear	= _ttoi(sDate.Mid(4,2));
	}

	if (nYear >= 99)
		nYear += 1900;
	else
		nYear += 2000;

	if ((nDay	> 31)	|| (nDay	<= 0)	||
		(nMonth	> 12)	|| (nMonth	<= 0)	||
		(nYear	> 2038)|| (nYear	<= 1969))
	{
		WK_TRACE_WARNING(_T("Wrong Date\n"));
		return FALSE;
	}
	
	// Nur die einfache Schaltjahrregel!
	if (nDay > nDaysPerMonth[nYear%4][nMonth])
	{
		WK_TRACE_WARNING(_T("Month:%d has only %d days\n"), nMonth, nDaysPerMonth[nYear%4][nMonth]);
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::ExtractAndValidateTime(const CString sTi, int &nHour, int &nMinute, int &nSecond)
{
	CString sTime = sTi;

	
	if (sTime.IsEmpty())
	{
		CSystemTime t;
		t.GetLocalTime();
		sTime.Format(_T("%02d%02d%02d"), t.GetHour(), t.GetMinute(), 0);

		if (theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
		{
			return FALSE;
		}
	}
	else if (sTime.GetLength() == 4)
	{
		// Sekunden als 00 annehmen
		sTime += _T("00");
	}
	else if (sTime.GetLength() < 6)
	{
		WK_TRACE_WARNING(_T("Time too short\n"));
		return FALSE;
	}
	nHour	= _ttoi(sTime.Mid(0,2));
	nMinute	= _ttoi(sTime.Mid(2,2));
	nSecond	= _ttoi(sTime.Mid(4,2));

	if ((nHour		< 0) || (nHour > 23)	||
		(nMinute	< 0) || (nMinute > 59)	||
		(nSecond	< 0) || (nSecond > 59))
	{
		WK_TRACE_WARNING(_T("Wrong Time\n"));
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsDateTimeInFuture(int iDay,int iMonth,int iYear,
								int iHour,int iMinute,int iSeconds)
{
	BOOL bRet = FALSE;
//	if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
//	{
		CSystemTime stNow;
		//liegt Datum in der Zukunft
		CSystemTime stStart((WORD)iDay,(WORD)iMonth,(WORD)iYear,
							(WORD)iHour,(WORD)iMinute,(WORD)iSeconds);

		stNow.GetLocalTime();
		if(stStart > stNow)
		{
			bRet = TRUE;
		}
//	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsDateTimeBeforeFirstImage(int iDay,int iMonth,int iYear,
										int iHour,int iMinute,int iSeconds)
{
	BOOL bRet = FALSE;
	if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
	{
		//liegt Datum in der Zukunft
		CSystemTime stStart((WORD)iDay,(WORD)iMonth,(WORD)iYear,
							(WORD)iHour,(WORD)iMinute,(WORD)iSeconds,(WORD)999);

		if(stStart < m_stFirstDBImage)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::ValidateIP(const CString &sIP)
{
	_TCHAR	szRest[1024]  = {0};
	int		a,b,c,d;
	BOOL	bRet = FALSE;

	if (sIP.IsEmpty() || (sIP.GetLength() != 12))
		return FALSE;

	a=-1;
	b=-1;
	c=-1;
	d=-1;
	_stscanf(sIP, _T("%03d%03d%03d%03d%s"), &a,&b,&c,&d,&szRest[0]);
	if ((a<0) || (a>255) ||
		(b<0) || (b>255) ||
		(c<0) || (c>255) ||
		(d<0) || (d>255))
	{
		//xxx.xxx.xxx.000 bzw. 255 sind nicht zugelassen
		return FALSE;
	}
	else
	{
		if (((CString)szRest).IsEmpty())
			bRet =(sIP.SpanIncluding(_T("1234567890.")) == sIP);
		else 
			bRet = FALSE;
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::ValidateSubNetMask(const CString &sMask)
{
	char	sRest[1024]  = {0};
	int		a,b,c,d;
	BOOL	bRet = FALSE;

	if (sMask.IsEmpty() || (sMask.GetLength() != 12))
		return FALSE;
	a=-1;
	b=-1;
	c=-1;
	d=-1;
	_stscanf(sMask, _T("%03d%03d%03d%03d%s"), &a,&b,&c,&d,sRest);
	if ((a<0) || (a>255) ||
		(b<0) || (b>255) ||
		(c<0) || (c>255) ||
		(d<0) || (d>255))
	{
		return FALSE;
	}
	else
	{
		if (((CString)sRest).IsEmpty())
			bRet =(sMask.SpanIncluding(_T("1234567890.")) == sMask);
		else 
			bRet = FALSE;
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::ValidateMSN(const CString &sMSN)
{
/*	if (_ttoi(sMSN) > 9)
		return FALSE;
	else
*/		return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::ValidateTimer(const CString &sTimer)
{
	int		a,b,c,d;

	if (sTimer.IsEmpty() || (sTimer.GetLength() != 8))
		return FALSE;

	a=-1;	   
	b=-1;
	c=-1;
	d=-1;
	_stscanf(sTimer, _T("%02d%02d%02d%02d"), &a,&b,&c,&d);
	if ((a<0) || (a>23) ||
		(b<0) || (b>59) ||
		(c<0) || (c>23) ||
		(d<0) || (d>59))
	{
		return FALSE;
	}

// timespan over midnight allowed e.g. MO:12:00-DI:10:00
//	return (a*60+b <= c*60+d);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CTransparentDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_UP:
		if (m_ctrlCrossUp.IsWindowEnabled())
			OnCrossUp();
		return;
	case VK_DOWN:
		if (m_ctrlCrossDown.IsWindowEnabled())
			OnCrossDown();
		return;
	case VK_LEFT:
		if (m_ctrlCrossLeft.IsWindowEnabled())
			OnCrossLeft();
		return;
	case VK_RIGHT:
		if (m_ctrlCrossRight.IsWindowEnabled())
			OnCrossRight();
		return;
	case VK_RETURN:
		if (m_ctrlCrossSet.IsWindowEnabled())
			OnCrossSet();
		return;
	case VK_DELETE:
	case VK_DECIMAL:
		if (m_ctrlButtonClear.IsWindowEnabled())
			OnButtonClear();
		return;
	case VK_ADD:
		if (m_ctrlButtonPlus.IsWindowEnabled())
			OnButtonPlus();
		return;
	case VK_F1:
		if (m_ctrlButtonSkipBack.IsWindowEnabled())
			OnButtonSkipBack();
		return;
	case VK_F2:
		if (m_ctrlButtonFastBack.IsWindowEnabled())
			OnButtonRewind();
		return;
	case VK_F3:
		if (m_ctrlButtonPlayBack.IsWindowEnabled())
			OnButtonPlayBack();
		return;
	case VK_F4:
		if (m_ctrlButtonSingleBack.IsWindowEnabled())
			OnButtonSingleBack();
		return;
	case VK_F8:
		if (m_ctrlButtonSkipForward.IsWindowEnabled())
			OnButtonSkipForward();
		return;
	case VK_F7:
		if (m_ctrlButtonFastForward.IsWindowEnabled())
			OnButtonFF();
		return;
	case VK_F6:
		if (m_ctrlButtonPlayForward.IsWindowEnabled())
			OnButtonPlay();
		return;
	case VK_F5:
		if (m_ctrlButtonSingleForward.IsWindowEnabled())
			OnButtonSingleForward();
		return;
	case VK_F9:
		if (m_ctrlButtonSearch.IsWindowEnabled())
			OnButtonSearch();
		return;
	case VK_F10:
		if (m_ctrlButtonExport.IsWindowEnabled())
			OnButtonExport();
		return;
	case VK_F11:
		if (IsMinimized())
		{
			MaximizeDlg();
		}
		else
		{
			if (m_ctrlButtonMinimize.IsWindowEnabled())
				OnButtonClose();
		}
		return;
	case VK_F12:
		if (m_ctrlButtonPrint.IsWindowEnabled())
			OnButtonPrint();
		return;
	}
	CTransparentDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnUpdateButtons(WPARAM wParam, LPARAM lParam)
{
//	TRACE(_T("OnUpdateButtons\n"));
	BOOL bEnablePanel	= wParam;
	BOOL bPlayWnd		= FALSE;
	BOOL bLiveWnd		= FALSE;
	BOOL bForward		= TRUE;
	BOOL bFastForward	= TRUE;
	BOOL bBack			= TRUE;
	BOOL bFastBack		= TRUE;
	BOOL bStop			= TRUE;
	BOOL bEject			= theApp.IsReadOnly() || theApp.IsReceiver();
	BOOL bTrackBack		= TRUE;
	BOOL bTrackForward	= TRUE;
	BOOL bSearch		= FALSE;
	BOOL bExportAndPrint= TRUE;

	// Wenn ein modaler Dialog offen ist, sollen die Buttons bleiben wie sie sind!! 
	if (m_pMaintenanceDlg || m_pInfoDlg || m_pStatisticDlg || m_pMDConfigDlg || m_pRTEDlg || m_pHostsDlg)
		return 0;

	CPlayWindow::PlayStatus ps = CPlayWindow::PS_STOP;
	CPlayWindow::RectPlayStatus rps = CPlayWindow::PS_RECT_NO_RECT;
	
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CPlayWindow* pPlayWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
		if (pPlayWnd)
		{
			bPlayWnd = TRUE;
			bSearch = pPlayWnd->CanPlayStatus(CPlayWindow::PS_QUERY);
			if (pPlayWnd->IsWindowVisible())
			{
				bEject = pPlayWnd->CanPlayStatus(CPlayWindow::PS_EJECT);
				if (!bEject)
				{
					bExportAndPrint = FALSE;
				}
				bForward		= pPlayWnd->CanPlayStatus(CPlayWindow::PS_PLAY_FORWARD);
				bBack			= pPlayWnd->CanPlayStatus(CPlayWindow::PS_PLAY_BACK);
				bFastForward	= pPlayWnd->CanPlayStatus(CPlayWindow::PS_FAST_FORWARD);
				bFastBack		= pPlayWnd->CanPlayStatus(CPlayWindow::PS_FAST_BACK);

				ps = pPlayWnd->GetPlayStatus();
				rps = pPlayWnd->GetRectPlayStatus();

				bStop =    (ps == CPlayWindow::PS_FAST_FORWARD)
					    || (ps == CPlayWindow::PS_PLAY_FORWARD)
					    || (ps == CPlayWindow::PS_FAST_BACK)
					    || (ps == CPlayWindow::PS_PLAY_BACK);
				bTrackBack = pPlayWnd->CanPlayStatus(CPlayWindow::PS_SKIP_BACK);
				bTrackForward = pPlayWnd->CanPlayStatus(CPlayWindow::PS_SKIP_FORWARD);
			}
			else
			{
				if (pPlayWnd->IsAlarm())
				{
					bTrackBack		= FALSE;
					bTrackForward	= FALSE;
				}
			}
		}

		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			if (pLiveWnd->IsWindowVisible())
			{
				if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
				{
					if(!pLiveWnd->GetTracker()->GetRectPromilleImage()->IsRectEmpty())
					{
						bTrackForward	= FALSE;
						bTrackBack		= FALSE;
						bFastForward	= FALSE;
						bFastBack		= FALSE;
					}
				}
				bLiveWnd = TRUE;
			}
		}
	
		//Suchbutton während des Backups deaktivieren
		CBackupWindow* pBackupWnd = pMainWnd->GetBackupWindow();
		if (pBackupWnd)
		{
			bSearch = FALSE;
		}
	}

	m_ctrlButtonSkipBack.EnableWindow(bEnablePanel && bPlayWnd && bBack && bTrackBack);
	m_ctrlButtonSingleBack.EnableWindow(bEnablePanel && bPlayWnd && bBack);
	m_ctrlButtonPlayBack.EnableWindow(bEnablePanel && bPlayWnd && bBack);
	m_ctrlButtonFastBack.EnableWindow(bEnablePanel && bPlayWnd && bFastBack);

	m_ctrlButtonSkipForward.EnableWindow(bEnablePanel && bPlayWnd && bForward && bTrackForward);
	m_ctrlButtonSingleForward.EnableWindow(bEnablePanel && bPlayWnd && bForward);
	m_ctrlButtonPlayForward.EnableWindow(bEnablePanel && bPlayWnd && bForward);
	m_ctrlButtonFastForward.EnableWindow(bEnablePanel && bPlayWnd && bFastForward);

	// Taste gedrückt
//TODO TKR hier bei SyncPlay die Buttons enablen
/*
	BOOL bSync = CanSyncPlay();
	if(bSync)
	{
		CPlayWindow* pOldestPW = theApp.GetMainFrame()->GetOldestPlayWindow();
		if(pOldestPW)
		{
			ps = pOldestPW->GetPlayStatus();	
		}
	}
*/
	BOOL bSetCheckPlayForward = (   (ps == CPlayWindow::PS_PLAY_FORWARD) 
								 || (rps == CPlayWindow::PS_RECT_PLAY_FORWARD)
								);
	BOOL bSetCheckPlayBack = (      (ps == CPlayWindow::PS_PLAY_BACK) 
								 || (rps == CPlayWindow::PS_RECT_PLAY_BACK)
								);
	//während einer Rechtecksuche (Abspielen vorwärts/rückwärts) wurden diese Buttons
	//immer kurz disabled, da sich bei der Rechtecksuche der PlayStatus zeitweise auf
	//PS_QUERY wechselt. Ist die Suche beendet wird der PlayStatus wieder zurück
	//auf PS_PLAY_FORWARD bzw. PS_PLAY_BACK gesetzt.

	m_ctrlButtonPlayForward.SetCheck(bSetCheckPlayForward);
	m_ctrlButtonPlayBack.SetCheck(bSetCheckPlayBack);
	
	m_ctrlButtonFastForward.SetCheck(ps == CPlayWindow::PS_FAST_FORWARD);
	m_ctrlButtonFastBack.SetCheck(ps == CPlayWindow::PS_FAST_BACK);
	
	m_ctrlButtonSearch.EnableWindow(bEnablePanel && bPlayWnd && bSearch);

	BOOL bExport = bEnablePanel &&
				   bExportAndPrint &&
				   (m_eInputState != InputUpdateSystem) &&
				   (bPlayWnd || bLiveWnd);

	if (   m_bFloppyExportOnlySupervisor
		&& bExport)
	{
		if (IsSupervisor())
		{
			// OK
		}
		else if (IsOperator())
		{
			// no Export
			bExport = FALSE;
		}
	}
	if (m_sExportDriveName.IsEmpty())
	{
		bExport = FALSE;
	}

	m_ctrlButtonExport.ShowWindow(bExport ? SW_SHOW : SW_HIDE);
	m_ctrlButtonExport.EnableWindow(bExport);

	m_ctrlButtonEject.EnableWindow(bEnablePanel && bEject);

	BOOL bPlus = FALSE;
	// Darf der Button '>10' versteckt werden?
	if (m_eInputState != InputEditAlarmDialingNumber)
	{
		for (int nI = 9; nI < MAX_CAM; nI++)
		{
			if (ExistLiveCamera(nI) || ExistPlayCamera(nI))
			{
				bPlus = TRUE;
				break;
			}
		}
		m_ctrlButtonPlus.SetWindowText(_T("1.."));
	}
	else
	{
		// Bei der Eingabe der Alarmrufnummern wird diese Taste zur Eingabe
		// des Punktes benötigt.
		m_ctrlButtonPlus.SetWindowText(_T("."));
		bPlus = TRUE;
	}

	// Ist kein Defaultdrucker gewählt oder ist der Drucker 'Kein Drucker' gewählt?
	int nDefaultPrinter = GetDefaultPrinter(); 
	if ((nDefaultPrinter == -1) || (nDefaultPrinter == 0))
	{
		m_ctrlButtonPrint.EnableWindow(FALSE);
	}
	else
	{
		if (!m_ctrlButtonPrint.IsWindowVisible())
		{
			m_ctrlButtonPrint.ShowWindow(SW_SHOW);
		}
		m_ctrlButtonPrint.EnableWindow(    bEnablePanel && bExportAndPrint 
										&& (bPlayWnd || bLiveWnd));
	}

	if (m_eInputState == InputSelectHost)
	{
		m_ctrlCrossSet.EnableWindow((m_HostsAll.GetSize() > 0));

		if (m_eOem == OemSantec)
		{
			m_ctrlCrossUp.EnableWindow(FALSE);
			m_ctrlCrossDown.EnableWindow(FALSE);
			m_ctrlCrossLeft.EnableWindow(TRUE);
			m_ctrlCrossRight.EnableWindow(TRUE);
		}
		else
		{
			m_ctrlCrossUp.EnableWindow(TRUE);
			m_ctrlCrossDown.EnableWindow(TRUE);
			m_ctrlCrossLeft.EnableWindow(FALSE);
			m_ctrlCrossRight.EnableWindow(FALSE);
		}

		m_ctrlButtonMinimize.EnableWindow(FALSE);
		m_ctrlButtonNum0.EnableWindow(FALSE);
		m_ctrlButtonNum1.EnableWindow(FALSE);
		m_ctrlButtonNum2.EnableWindow(FALSE);
		m_ctrlButtonNum3.EnableWindow(FALSE);
		m_ctrlButtonNum4.EnableWindow(FALSE);
		m_ctrlButtonNum5.EnableWindow(FALSE);
		m_ctrlButtonNum6.EnableWindow(FALSE);
		m_ctrlButtonNum7.EnableWindow(FALSE);
		m_ctrlButtonNum8.EnableWindow(FALSE);
		m_ctrlButtonNum9.EnableWindow(FALSE);
		m_ctrlButtonPlus.EnableWindow(FALSE);
		m_ctrlButtonClear.EnableWindow(FALSE);
	}
	else if (m_eInputState == InputPanelPIN)
	{
		m_ctrlCrossSet.EnableWindow(FALSE);
		m_ctrlCrossUp.EnableWindow(FALSE);
		m_ctrlCrossDown.EnableWindow(FALSE);
		m_ctrlCrossLeft.EnableWindow(FALSE);
		m_ctrlCrossRight.EnableWindow(FALSE);
		m_ctrlButtonMinimize.EnableWindow(FALSE);
		m_ctrlButtonNum0.EnableWindow(TRUE);
		m_ctrlButtonNum1.EnableWindow(TRUE);
		m_ctrlButtonNum2.EnableWindow(TRUE);
		m_ctrlButtonNum3.EnableWindow(TRUE);
		m_ctrlButtonNum4.EnableWindow(TRUE);
		m_ctrlButtonNum5.EnableWindow(TRUE);
		m_ctrlButtonNum6.EnableWindow(TRUE);
		m_ctrlButtonNum7.EnableWindow(TRUE);
		m_ctrlButtonNum8.EnableWindow(TRUE);
		m_ctrlButtonNum9.EnableWindow(TRUE);
		m_ctrlButtonPlus.EnableWindow(FALSE);
		m_ctrlButtonClear.EnableWindow(TRUE);
	}
	else if ((m_eInputState == InputShutdownPIN) || (m_eInputState == InputResetPIN))
	{
		m_ctrlCrossSet.EnableWindow(TRUE);
		m_ctrlCrossUp.EnableWindow(TRUE);
		m_ctrlCrossDown.EnableWindow(TRUE);
		m_ctrlCrossLeft.EnableWindow(TRUE);
		m_ctrlCrossRight.EnableWindow(TRUE);
		m_ctrlButtonMinimize.EnableWindow(FALSE);
		m_ctrlButtonNum0.EnableWindow(TRUE);
		m_ctrlButtonNum1.EnableWindow(TRUE);
		m_ctrlButtonNum2.EnableWindow(TRUE);
		m_ctrlButtonNum3.EnableWindow(TRUE);
		m_ctrlButtonNum4.EnableWindow(TRUE);
		m_ctrlButtonNum5.EnableWindow(TRUE);
		m_ctrlButtonNum6.EnableWindow(TRUE);
		m_ctrlButtonNum7.EnableWindow(TRUE);
		m_ctrlButtonNum8.EnableWindow(TRUE);
		m_ctrlButtonNum9.EnableWindow(TRUE);
		m_ctrlButtonPlus.EnableWindow(FALSE);
		m_ctrlButtonClear.EnableWindow(TRUE);
	}
	else
	{
		// Wenn Submenüeintrag nicht wählbar ist den Set-Button ebenfalls sperren
		if ((m_pOSD->GetSubMenuState() & MF_DISABLED) && (m_eInputState == InputNo)
			|| (m_eInputState == InputBackupWaitCalculatedEnd))
			m_ctrlCrossSet.EnableWindow(FALSE);
		else
			m_ctrlCrossSet.EnableWindow(bEnablePanel);

		m_ctrlCrossUp.EnableWindow(bEnablePanel);
		m_ctrlCrossDown.EnableWindow(bEnablePanel);
		m_ctrlCrossLeft.EnableWindow(bEnablePanel);
		m_ctrlCrossRight.EnableWindow(bEnablePanel);
		m_ctrlButtonMinimize.EnableWindow(bEnablePanel);
		m_ctrlButtonNum0.EnableWindow(bEnablePanel);
		m_ctrlButtonNum1.EnableWindow(bEnablePanel);
		m_ctrlButtonNum2.EnableWindow(bEnablePanel);
		m_ctrlButtonNum3.EnableWindow(bEnablePanel);
		m_ctrlButtonNum4.EnableWindow(bEnablePanel);
		m_ctrlButtonNum5.EnableWindow(bEnablePanel);
		m_ctrlButtonNum6.EnableWindow(bEnablePanel);
		m_ctrlButtonNum7.EnableWindow(bEnablePanel);
		m_ctrlButtonNum8.EnableWindow(bEnablePanel);
		m_ctrlButtonNum9.EnableWindow(bEnablePanel);
		m_ctrlButtonPlus.EnableWindow(bEnablePanel && bPlus);
		m_ctrlButtonClear.EnableWindow(bEnablePanel);
	}
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
CLiveWindow* CPanel::ExistLiveCamera(int nCam)
{
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetLiveWindow((WORD)nCam);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDebugWindow* CPanel::ExistDebugWindow()
{
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetDebugWindow();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CPlayWindow* CPanel::ExistPlayCamera(int nCam)
{
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetPlayWindow((WORD)nCam);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CBackupWindow* CPanel::ExistBackupCamera()
{
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetBackupWindow();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
int CPanel::CheckExistingCameras()
{
	int nCounter = 0;
	for (int nI = 0; nI < MAX_CAM; nI++)
	{
		if (ExistLiveCamera(nI) || ExistPlayCamera(nI))
		{
			nCounter++;
			m_OSDNormal.EnableMenuItem(IDOSD_CAM1+nI, MF_ENABLED);
		}
		else
		{
			m_OSDNormal.EnableMenuItem(IDOSD_CAM1+nI, MF_DISABLED);
		}
	}
	
	return nCounter;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::DoShutdown()
{
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (pMainWnd->IsInBackup())
		{
			// in backup now
			return;	
		}
	}
	// Beim Disconnect muß der Alarmlistendialog geschlossen werden
	if (m_pAlarmlistDlg)
	{
		m_pAlarmlistDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
	}
	
	theApp.DoDisconnect();
	SetPanelPosition(TRUE);
	PostMessage(WM_SHUTDOWN);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::DoReboot()
{
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (pMainWnd->IsInBackup())
		{
			// in backup now
			return;	
		}
	}
	PostMessage(WM_REBOOT);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnShutDown(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	CWnd* pWnd = FindWindow(NULL, _T("DVStarter"));
	if (pWnd)
	{
		WK_TRACE(_T("CPanel::OnShutDown\n"));
		CWK_Profile wkp;
		wkp.WriteInt(REGKEY_DVSTARTER, _T("DoReboot"), 0);
   		pWnd->PostMessage(DVSTARTER_EXIT, 0, 0);
		CString sMsg;
		sMsg.LoadString(IDS_SHUTDOWN);
		SetDisplayText(m_ctrlDisplayMenu2, sMsg, 0, 0);
		Sleep(500);
	}
	else
		WK_TRACE_WARNING(_T("Can't find DVStarter window\n"));
#endif

	DestroyWindow();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnReboot(WPARAM wParam, LPARAM lParam)
{
	CWnd* pWnd = FindWindow(NULL, _T("DVStarter"));
	if (pWnd)
	{
		WK_TRACE(_T("CPanel::OnReboot\n"));
		CWK_Profile wkp;
		wkp.WriteInt(REGKEY_DVSTARTER, _T("DoReboot"), 1);
   		pWnd->PostMessage(DVSTARTER_EXIT, 0, 0);
		CString sMsg;
		sMsg.LoadString(IDS_SHUTDOWN);
		SetDisplayText(m_ctrlDisplayMenu2, sMsg, 0, 0);
		Sleep(500);
	}
	DestroyWindow();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnPowerOffButtonPressed(WPARAM wParam, LPARAM lParam)
{	
	if (theApp.IsTransmitter())
	{
		if ((m_eInputState != InputShutdownPIN) && (m_eInputState != InputResetPIN)&& (m_eInputState != InputPanelPIN))
		{
			WK_TRACE(_T("PowerOff-Button pressed\n"));
			
			// Offene Dialoge schließen.
			if (m_pRTEDlg)
			{
				m_pRTEDlg->SendMessage(WM_COMMAND , MAKELONG(IDC_RTE_OK, BN_CLICKED), NULL);
			}
			if (m_pKeyboardDlg)
			{
				m_pKeyboardDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pStatisticDlg)
			{
				m_pStatisticDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pAlarmlistDlg)
			{
				m_pAlarmlistDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pMDConfigDlg)
			{
				m_pMDConfigDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pHostsDlg)
			{
				m_pHostsDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pInfoDlg)
			{
				m_pInfoDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pMaintenanceDlg)
			{
				m_pMaintenanceDlg->SendMessage(WM_COMMAND , MAKELONG(IDC_MAINT_BUTTON_OK, BN_CLICKED), NULL);
			}

			// Raus aus dem Konfigurationsmenü
			m_pOSD = &m_OSDNormal;

			// Panel maximieren
			MaximizeDlg();

			SetPanelPosition(TRUE);
			m_eInputState = InputShutdownPIN;
			EnableInputMode();
			UpdateMenu();	
			Beep(440, 500);
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnResetButtonPressed(WPARAM wParam, LPARAM lParam)
{
	if (theApp.IsTransmitter())
	{
		if ((m_eInputState != InputShutdownPIN) && (m_eInputState != InputResetPIN)&& (m_eInputState != InputPanelPIN))
		{
			WK_TRACE(_T("Reset-Button pressed\n"));

			// Offene Dialoge schließen.
			if (m_pRTEDlg)
			{
				m_pRTEDlg->SendMessage(WM_COMMAND , MAKELONG(IDC_RTE_OK, BN_CLICKED), NULL);
			}
			if (m_pKeyboardDlg)
			{
				m_pKeyboardDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pStatisticDlg)
			{
				m_pStatisticDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pAlarmlistDlg)
			{
				m_pAlarmlistDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pMDConfigDlg)
			{
				m_pMDConfigDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pHostsDlg)
			{
				m_pHostsDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pInfoDlg)
			{
				m_pInfoDlg->SendMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			if (m_pMaintenanceDlg)
			{
				m_pMaintenanceDlg->SendMessage(WM_COMMAND , MAKELONG(IDC_MAINT_BUTTON_OK, BN_CLICKED), NULL);
			}

			// Raus aus dem Konfigurationsmenü
			m_pOSD = &m_OSDNormal;

			// Panel maximieren
			MaximizeDlg();

			SetPanelPosition(TRUE);
			m_eInputState = InputResetPIN;
			EnableInputMode();
			UpdateMenu();	
			Beep(440, 500);
		}
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDisconnect()
{
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (!pMainWnd->IsInBackup())
		{
			// Beim Disconnect muß der Alarmlistendialog geschlossen werden
			if (m_pAlarmlistDlg)
			{
				m_pAlarmlistDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
			}
			
			theApp.DoDisconnect();
			m_sHostAddress.Empty();
			m_sHostName.Empty();
			SetPanelPosition(TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSystemInfo()
{
	CString sInfo;
	CServer *pServer = theApp.GetServer();
	if(pServer)
	{
		sInfo =	pServer->GetInfo();
		TRACE(_T("\n%s\n"), sInfo);
	}

	EnablePanel(FALSE);
	m_pInfoDlg = new CInfoDlg(sInfo, this);

	if (m_pInfoDlg)
	{
		m_pInfoDlg->DoModal();
		WK_DELETE(m_pInfoDlg);
	}
	EnablePanel(TRUE);

}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonExport() 
{
	BOOL bResult = FALSE;
	CString sMsg;

	m_ctrlButtonExport.EnableWindow(FALSE);
	if (IsDriveAvailable(m_nExportDrive, m_sExportPath, m_sExportDriveName))
	{
//		if ((m_sExportPath == _T("a:\\")) || (m_sExportPath == _T("b:\\")))
		if (!m_sExportPath.IsEmpty())
		{
			CMainFrame* pMainWnd = theApp.GetMainFrame();
			if (pMainWnd)
			{		 
				CDisplayWindow *pWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
				if (pWnd == NULL || !pWnd->IsActive())
				{
					pWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
				}

				if (pWnd)
				{
					m_ctrlDisplayTooltip.StopScrolling();
					sMsg.LoadString(IDS_EXPORT_RUNNING);
					SetDisplayText(m_ctrlDisplayTooltip, sMsg);
					
					if (!m_sExportPath.IsEmpty())
					{
						bResult = pWnd->ExportImage(m_sExportPath);					
					}
					else
					{
						WK_TRACE_ERROR(_T("export no path\n"));
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("export no cam window\n"));
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("export no main window\n"));
			}
		}
		else
		{
			
			WK_TRACE_ERROR(_T("wrong export path %s\n"),m_sExportPath);
		}
	}
	else
	{
		WK_TRACE(_T("no drive available for export\n"));
	}

	if (bResult)
	{
		sMsg.LoadString(IDS_EXPORT_SUCCESS);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadInfoMessage();
	}
	else
	{
		sMsg.LoadString(IDS_EXPORT_FAILED);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadErrorMessage();
	}

	// Da das Speichern recht lange dauert, insbesondere
	// wenn mehrmals auf den Save-button gedrückt wurde,
	// wird nach jedem save, einmal confirmiert.
	ForceConfirmSelfcheck();

	m_ctrlButtonExport.EnableWindow(TRUE);

	WK_TRACE(_T("export %s\n"),sMsg);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnButtonPrint() 
{
	BOOL bResult = FALSE;
	CString sMsg, sPrinterStatus;
	DWORD   dwPrinterStatus = 0;

	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CDisplayWindow *pWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
		if (pWnd == NULL || !pWnd->IsActive())
		{
			pWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		}

		if (pWnd)
		{
			sMsg.LoadString(IDS_PRINTJOB_RUNNING);
			m_ctrlDisplayTooltip.StopScrolling();
			SetDisplayText(m_ctrlDisplayTooltip, sMsg);
			
			bResult = pWnd->PrintImage() && theApp.GetPrinterStatus(dwPrinterStatus, sPrinterStatus) && (dwPrinterStatus == 0);
		}
	}

	if (bResult)
	{
		sMsg.LoadString(IDS_PRINTJOB_SUCCESS);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadInfoMessage();
	}
	else
	{
		m_ctrlDisplayTooltip.StopScrolling();
		sMsg.LoadString(IDS_PRINTJOB_FAILED);
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadErrorMessage();

		// Zusätzliche Informationen verfügbar?
		if (!sPrinterStatus.IsEmpty())
			SetDisplayText(m_ctrlDisplayTooltip, sPrinterStatus, SCROLL_TEXT_NO_WRAP, 0);
	}
}

unsigned int CALLBACK CPanel::PrintSetupHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == uiMsg)
	{
		::EnableWindow(::GetDlgItem(hdlg, 0x470), false);	// Druckerauswahl disablen
		::EnableWindow(::GetDlgItem(hdlg, 0x420), false);	// SeitenLayout disablen
		::EnableWindow(::GetDlgItem(hdlg, 0x421), false);
		return TRUE;
	}
	return FALSE;
}

void CPanel::OnRclickPrint()
{
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	int nSetup = prof.GetInt(_T("DVClient"), _T("PrinterSetup"), 0);
	if (nSetup)
	{
		EnablePanel(FALSE);
		CPrintDialog dlg(FALSE);
		dlg.m_pd.Flags &= ~PD_RETURNDC;
		dlg.m_pd.Flags |= PD_HIDEPRINTTOFILE;
		dlg.m_pd.Flags |=	PD_PRINTSETUP;
		dlg.m_pd.Flags |=	PD_NONETWORKBUTTON;
		dlg.m_pd.Flags |=	PD_ENABLESETUPHOOK;
		dlg.m_pd.lpfnSetupHook = PrintSetupHookProc;
		dlg.DoModal();
		EnablePanel(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::MaximizeDlg()
{
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow(SW_SHOWNORMAL);

	if (m_pMinimizedDlg)
	{
		m_pMinimizedDlg->DestroyWindow();
		m_pMinimizedDlg = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::MinimizeDlg()
{
	if (   m_eInputState != InputPanelPIN
		&& m_eInputState != InputShutdownPIN
		&& m_eInputState != InputResetPIN
		&& m_eInputState != InputUpdateSystem
		&& m_eInputState != InputSelectHost
		&& m_eInputState != InputEditHostList
		&& m_eInputState != InputSetExpansionCode)
	{
		m_dwLastUserAction = GetTickCount();
		DisableInputMode(TRUE);

		if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
		{
			//Backup Object löschen
			OnDeleteBackup();
		}

		// Offene Dialoge schließen.
		if (m_pRTEDlg)
		{
			m_pRTEDlg->PostMessage(WM_COMMAND , MAKELONG(IDC_RTE_OK, BN_CLICKED), NULL);
		}
		if (m_pKeyboardDlg)
		{
			m_pKeyboardDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
		}
		if (m_pStatisticDlg)
		{
			m_pStatisticDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
		}
		if (m_pAlarmlistDlg)
		{
			m_pAlarmlistDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
		}
		if (m_pMDConfigDlg)
		{
			m_pMDConfigDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
		}
		if (m_pHostsDlg)
		{
			m_pHostsDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
		}
		if (m_pInfoDlg)
		{
			m_pInfoDlg->PostMessage(WM_COMMAND , MAKELONG(IDOK, BN_CLICKED), NULL);
		}
		if (m_pMaintenanceDlg)
		{
			m_pMaintenanceDlg->PostMessage(WM_COMMAND , MAKELONG(IDC_MAINT_BUTTON_OK, BN_CLICKED), NULL);
		}
		ShowWindow(SW_HIDE);
		OnEndConfig();
		m_pMinimizedDlg = new CMinimizedDlg(this);
		m_pMinimizedDlg->Create();
		
		// Das Panel soll aber weiterhin den Focus haben.
		ActivatePanel();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnChangeCameraName()
{
	BOOL bResult = FALSE;
	CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
	if (pLiveWnd)
	{
		m_pKeyboardDlg = new CKeyboardDlg(&m_ctrlDisplayMenu2, m_pOSD->GetSubString(), pLiveWnd->GetName(), this);
		if (m_pKeyboardDlg)
		{
			EnablePanel(FALSE);
			m_eInputState = InputChangeCamName;
			m_pKeyboardDlg->DoModal();
			
			// Der InputStatus könnte evtl. auf gewechselt haben.
			if (m_eInputState == InputChangeCamName)
			{
				m_eInputState = InputNo;
				bResult = pLiveWnd->SetName(m_pKeyboardDlg->GetInputString());
				WK_DELETE(m_pKeyboardDlg);
				EnablePanel(TRUE);
				OnUpdateMenu(0,0);
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetExpansionCode()
{
	BOOL bResult = FALSE;
	m_pKeyboardDlg = new CKeyboardDlg(&m_ctrlDisplayMenu2, m_pOSD->GetSubString(), _T(""), this);
	if (m_pKeyboardDlg)
	{
		EnablePanel(FALSE);
		m_eInputState = InputSetExpansionCode;
		m_pKeyboardDlg->DoModal();
		m_eInputState = InputNo;
		
		CString sExpCode = m_pKeyboardDlg->GetInputString();
		WK_DELETE(m_pKeyboardDlg);

		CServer* pServer = theApp.GetServer();
		if (pServer)
		{
			bResult = pServer->SetExpansionCode(sExpCode);
		}
		
		EnablePanel(TRUE);
		OnUpdateMenu(0,0);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
/*	not used
BOOL CPanel::OnOpenExportDlg()
{
	BOOL bResult		= FALSE;
	CString	sStart		= _T("");
	CString	sEnd		= _T("");
	DWORD	dwCamMask	= 0;

	m_pExportDlg = new CExportDlg(&m_ctrlDisplayMenu2, this);
	if (m_pExportDlg)
	{
		EnablePanel(FALSE);
		m_eInputState = InputExportDlgOpen;
		if (m_pExportDlg->DoModal() == IDOK)
		{
			m_pExportDlg->GetExportData(sStart, sEnd, dwCamMask);
		}
		m_eInputState = InputNo;

		WK_DELETE(m_pExportDlg);
		EnablePanel(TRUE);
		OnUpdateMenu(0,0);
	}

	return bResult;
}
*/
/////////////////////////////////////////////////////////////////////////////
void CPanel::ToggleInputMode()
{
	m_bSetValue = !m_bSetValue;
	if (m_bSetValue)
		m_ctrlDisplayMenu2.EnableBlinking(500);	
	else
		m_ctrlDisplayMenu2.DisableBlinking();
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetViewMode(ViewMode vm, WORD wCam)
{
	PostMessage(WM_SET_VIEWMODE, wCam, vm);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnSetViewMode(WPARAM wParam, LPARAM lParam)
{
	WORD wCam = (WORD)wParam;

	if (m_nCamNr != wCam)
	{
		OnSetCamera(wCam, LivePlay);
	}
	OnSetViewMode(lParam);
	return 0;				 
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::ToggleViewMode(WORD wCam)
{
	PostMessage(WM_TOGGLE_VIEWMODE, wCam);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnToggleViewMode(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("##CPanel::OnToggleViewMode (Cam=%d)\n"), wParam);
	WORD wCam = (WORD)wParam;

	int iResourceID = IDOSD_VIEW_FULL;
	m_OSDNormal.IncSubMenuPos(iResourceID);
	
	// Aktuelle Auswahl des Viewmodes ermitteln
	int nMenuPos	= m_OSDNormal.FindMenuPos(iResourceID);
	int nSubMenuPos	= m_OSDNormal.GetSubMenuPos(iResourceID);

	switch (m_OSDNormal.GetMenuID(nMenuPos, nSubMenuPos))
	{
		case IDOSD_VIEW_FULL:
			OnSetViewMode(ViewFull);
			break;
		case IDOSD_VIEW_XXX:
			OnSetViewMode(ViewXxX);
			break;
		case IDOSD_VIEW_1PX:
			OnSetViewMode(View1pX);
			break;
	}

	m_nCamNr = 	(wCam & 0xff);

	return 0;				 
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::EnableInputMode()
{
	m_bSetValue = TRUE;
	m_ctrlDisplayMenu2.EnableBlinking(500);	
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::DisableInputMode(BOOL bForceDisable /* = FALSE */)
{

	if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
	{
		//im Backup (Eingabe Kameras, Start- und Endzeit) wird nach Umschalten auf eine andere
		//Live-Kamera bzw. Datanbankbild das Menü im Panel nicht aktualisiert, damit der aktuelle
		//Eingabestatus erhalten bleibt
		if(bForceDisable)
		{
			m_eInputState = InputNo;
			m_bSetValue = FALSE;
			m_sInput	= _T("");
			m_ctrlDisplayMenu2.DisableBlinking();
		}
		else
		{
			// Bei den folgenden Inputstatis den Inputstate NICHT auf InputNo zurücksetzen
			if(	m_eInputState != InputBackupStartDateTime
				&& m_eInputState != InputBackupWaitCalculatedEnd
				&& m_eInputState != InputBackupEndDateTime
				&& m_eInputState != InputBackupCameras
				&& m_eInputState != InputBackupAskForMedium
				&& m_eInputState != InputBackupWaitForMedium
				&& m_eInputState != InputBackupFoundMedium
				&& m_eInputState != InputBackupFoundNoEmptyCD
				&& m_eInputState != InputBackupDVDNotAllowed
				&& m_eInputState != InputBackupEraseMedium
				&& m_eInputState != InputBackupEraseMediumConfirmingYes
				&& m_eInputState != InputBackupEraseMediumConfirmingNo
				&& m_eInputState != InputShutdownPIN
				&& m_eInputState != InputResetPIN
				&& m_eInputState != InputPanelPIN
				&& m_eInputState != InputSelectHost)
			{
				m_eInputState = InputNo;
				m_bSetValue = FALSE;
				m_sInput	= _T("");
				m_ctrlDisplayMenu2.DisableBlinking();
			}
		}
	}
	else
	{
		// Bei den folgenden Inputstatis den Inputstate NICHT auf InputNo zurücksetzen
		if( m_eInputState != InputShutdownPIN
			&& m_eInputState != InputResetPIN
			&& m_eInputState != InputPanelPIN
			&& m_eInputState != InputSelectHost)
		{
			m_eInputState = InputNo;
			m_bSetValue = FALSE;
			m_sInput	= _T("");
			m_ctrlDisplayMenu2.DisableBlinking();	
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsPrinterAvailable()
{					
	return (m_nPrinterCnt > 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsDriveAvailable(BOOL bCheckDisk, 
							  CString &sDrivePath, 
							  CString &sDriveName, 
							  UINT nUnitMask /*=0*/)
{
	// Wechselmedium vorhanden.
	DWORD dwMask, dwDrives = GetLogicalDrives();
	int nCount = 0;

	if(!bCheckDisk)
	{
		sDrivePath.Empty();
		sDriveName.Empty();
	}

	for (int nI = 0; nI <= 31; nI++)
	{
		// Drive C: überspringen. Bei CF ist dies ja removeable!
		if (nI == 2)
			continue;

		dwMask = 1<<nI;
		if (dwDrives & dwMask)
		{
			CString sTemp;
			sTemp.Format(_T("%c:\\"), (char)(_T('a')+nI));
			if (GetDriveType(sTemp) == DRIVE_REMOVABLE)
			{
				nCount++;
				if (bCheckDisk != 0)
				{
					DWORD dwCheck = 1 << (bCheckDisk-1);
					if ((dwCheck & dwMask) && IsDiskAvailable(sTemp))
					{
						sDrivePath = sTemp;
						break;
					}
				}
				else if (nUnitMask != 0)
				{
					if (nUnitMask & dwMask)
					{
						sDrivePath = sTemp;
						break;
					}
				}
				else
				{
					sDrivePath = sTemp;
					break;
				}
			}
		}
	}
	if (nCount)
	{
		CString sName  = GetDriveName(sDrivePath);
		if (sName.IsEmpty())
		{
			sDriveName = sDrivePath;
		}
		else
		{
			sDriveName = sName;
		}
		return nI+1;
	}
	return 0;
//	return (!sDrivePath.IsEmpty());
}

BOOL CPanel::IsDiskAvailable(const CString &sDrive)
{
	_TCHAR	szVolumeNameBuffer[_MAX_PATH];
	_TCHAR	szFileSystemNameBuffer[_MAX_PATH];
	DWORD 	dwSerial = 0;
	DWORD	dwMaximumComponentLength = 0;
	DWORD	dwlpFileSystemFlags	= 0;
	BOOL b1 = GetVolumeInformation(
				sDrive,						// address of root directory of the file system
				szVolumeNameBuffer,			// address of name of the volume
				_MAX_PATH,					// length of lpVolumeNameBuffer
				&dwSerial,					// address of volume serial number
				&dwMaximumComponentLength,	// address of system's maximum filename length
				&dwlpFileSystemFlags,	    // address of file system flags
				szFileSystemNameBuffer,		// address of name of file system
				_MAX_PATH);					// length of lpFileSystemNameBuffer

	if (b1)
	{
		DWORD	dwSectorsPerCluster		=	0;
		DWORD	dwBytesPerSector		=	0;
		DWORD	dwNumberOfFreeClusters	=	0;
		DWORD	dwTotalNumberOfClusters	=	0;

		if (GetDiskFreeSpace( sDrive, 
							  &dwSectorsPerCluster, 
							  &dwBytesPerSector, 
							  &dwNumberOfFreeClusters, 
							  &dwTotalNumberOfClusters ))
		{
			DWORD dwFreeBytes = dwBytesPerSector * dwSectorsPerCluster * dwNumberOfFreeClusters;
			b1 = dwFreeBytes > (100*1024);
		}
		else
		{
			b1 = FALSE;
		}
	}

	return b1; 
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnExit(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
//	TRACE(_T("CPanel exit\n"));
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (pMainWnd->IsInBackup())
		{
			// in backup now
			return 0;	
		}
	}
	DestroyWindow();	
	return 0;	
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	TRACE(_T("CPanel::OnClose\n"));
	// do nothing, disable ALT+F4
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnShowRTErrors()
{
	EnablePanel(FALSE);
	m_pRTEDlg	= new CRTEDlg(this);
	if (m_pRTEDlg)
	{
		m_pRTEDlg->DoModal();
		WK_DELETE(m_pRTEDlg);
	}
	EnablePanel(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnShowStatistic()
{
	EnablePanel(FALSE);
	m_pStatisticDlg	= new CStatisticDlg(this);
	if (m_pStatisticDlg)
	{
		m_pStatisticDlg->DoModal();
		WK_DELETE(m_pStatisticDlg);
	}
	EnablePanel(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnShowAlarmlist()
{
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (m_pAlarmlistDlg == NULL)
		{
			// CAlarmlistDlg is modeless, so it deletes itself
			// and sets pointer back to NULL
			m_pAlarmlistDlg	= new CAlarmlistDlg(m_sHostAddress, 
												pServer->GetSupervisorPIN(), 
												this);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnShowMDConfigDlg()
{
	CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
	if (pLiveWnd)
	{
		EnablePanel(FALSE);
		m_pMDConfigDlg	= new CMDConfigDlg(pLiveWnd->GetID(), this);
		if (m_pMDConfigDlg)
		{
			m_pMDConfigDlg->DoModal();
			WK_DELETE(m_pMDConfigDlg);
		}
		EnablePanel(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnAlarmDelete()
{
	EnableInputMode();

	if(m_eConfirmingYesNoMode == ConfirmingModeUnknown)
	{
		m_eConfirmingYesNoMode = ConfirmingNo;
	}
	
	else if(m_eConfirmingYesNoMode == ConfirmingNo)
	{
		m_eConfirmingYesNoMode = ConfirmingModeUnknown;
		DisableInputMode();
		
	}
	else if(m_eConfirmingYesNoMode == ConfirmingYes)
	{
		DisableInputMode();
		CPlayWindow* pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)m_nCamNr);
		if (pPlayWnd && !pPlayWnd->IsMD())	
		{
			pPlayWnd->SetPlayStatus(CPlayWindow::PS_DELETE_ALARM);
		}	

	}

}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnAlarmBackup()
{
	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (!pMainWnd->IsInBackup())
		{
			m_bAlarmBackup = TRUE;
			OnBackup(m_bMultiBackup, m_bAlarmBackup);
		}
		else
		{
			WK_TRACE(_T("OnActivityBackup() cannot backup PlayStatus not allowed\n"));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSuspectSearch()
{
	CPlayWindow* pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)m_nCamNr);
	if (pPlayWnd && !pPlayWnd->IsMD())	
	{
		pPlayWnd->SetPlayStatus(CPlayWindow::PS_SHOW_SUSPECT);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSuspectDelete()
{
	CPlayWindow* pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)m_nCamNr);
	if (pPlayWnd && !pPlayWnd->IsMD())	
	{
		pPlayWnd->SetPlayStatus(CPlayWindow::PS_DELETE_SUSPECT);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CPanel::GetPrinterStatus(PRINTER_INFO_2 *ppI, CString *pstr)
{
  	if(ppI->Attributes&PRINTER_ATTRIBUTE_WORK_OFFLINE)
	{
		if (pstr) *pstr = _T("offline");
//		if (pstr) pstr->LoadString(IDS_ERROR);
	}
	else
	{
		if(ppI->Status==0)
		{
			if (pstr) *pstr = _T("ready");
//			if (pstr) pstr->LoadString(IDS_OK);
			return true;
		}
		else
		{
			if (pstr) *pstr = _T("not ready");
//			if (pstr) pstr->LoadString(IDS_ERROR);
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::EnumeratePrinters()
{
	DWORD	dwSize		= 0;
	DWORD	dwNeeded	= 0;
	DWORD	dwResult	= 0;
	BOOL	bResult		= FALSE;

	m_nPrinterCnt	= 0;

	// Check for required space
	PRINTER_INFO_2 dummy;
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,	// types of printer objects to enumerate
				  NULL,				// name of printer object
				  2,				// specifies type of printer info structure
				  (LPBYTE)&dummy,	// pointer to buffer to receive printer info structures
				  sizeof(dummy),	// size, in bytes, of the buffer
				  &dwNeeded,		// pointer to variable with no. of bytes copied (or required)
				  &dwResult			// pointer to variable with no. of printer info. structures copied
				  );

	// we need an additional dummy PRINTER_INFO_2 struct for the NO Printer
	dwSize = dwNeeded + sizeof(PRINTER_INFO_2) + 100;
//	dwSize = dwNeeded + sizeof(PRINTER_INFO_2)*2;
	PRINTER_INFO_2 *pPrinterInfoOld		= m_pPrinterInfo;
	int				nPrinterInfoOldSize	= m_nPrinterInfoSize;

	WK_DELETE(m_pnPrinterSort);
	m_pPrinterInfo = (PRINTER_INFO_2*)(new BYTE[dwSize]);

	// CAVEAT: Buffer starts at 2nd element [1], so size is only dwNeeded!
	if (EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,	// types of printer objects to enumerate
					  NULL,							// name of printer object
					  2,							// specifies type of printer info structure
					  (LPBYTE)&m_pPrinterInfo[1],	// pointer to buffer to receive printer info structures
													// and any strings or data the struct members point to!
					  dwNeeded,						// size, in bytes, of the buffer
					  &dwNeeded,	// pointer to variable with no. of bytes copied (or required)
					  &dwResult))	// pointer to variable with no. of printer info. structures copied
	{
		int nOldPrinterCnt	= m_nPrinterCnt;
		m_nPrinterInfoSize	= dwResult;
		m_nPrinterCnt			= dwResult;
		
		int nI;
		int nPrinterCount = 1;
		if (m_nPrinterInfoSize > 0)
		{
			m_pnPrinterSort = new int[m_nPrinterInfoSize+1];
			ZeroMemory(m_pnPrinterSort, sizeof(int)*m_nPrinterInfoSize+1);
			for (nI=1; nI<=m_nPrinterInfoSize; nI++)
			{
				CString sStatus;
				if (GetPrinterStatus(&m_pPrinterInfo[nI], &sStatus))// use only "ready" printers
				{
					m_pnPrinterSort[nPrinterCount++] = nI;
				}
				WK_TRACE(_T("Installed Printers:%s (%s)\n"), m_pPrinterInfo[nI].pPrinterName, sStatus);
			}

			for (nI=nPrinterCount; nI <= m_nPrinterInfoSize; nI++)// others are not ready
			{
				m_pnPrinterSort[nI] = m_pnPrinterSort[1];
			}

			m_nPrinterCnt = min(nPrinterCount-1, MAX_PRINTERS);	// Menu can contain only (16) MAX_PRINTERS

			m_nNewDefaultPrt = 0;
			if ((m_nPrinterCnt > nOldPrinterCnt) &&					// new printer
				 (pPrinterInfoOld != NULL))								// to compare with old ones
			{
				int nFound;
				for (nI=1; nI<=nPrinterInfoOldSize; nI++)				// enum old printers
				{
					if (!GetPrinterStatus(&pPrinterInfoOld[nI], NULL))// printer in old PI-array "not ready" or "offline"
					{																// find printer in new PI-array
						nFound = FindPrinter(m_pPrinterInfo, m_nPrinterInfoSize, pPrinterInfoOld[nI].pPrinterName);
						if ((nFound != -1) && GetPrinterStatus(&m_pPrinterInfo[nFound], NULL))
						{															// found "ready" printer
							m_nNewDefaultPrt = nFound;						// set to new default printer
							break;
						}
					}
				}
				if (!m_nNewDefaultPrt)										// no Printer changed means something completley different
				{
					for (nI=1; nI<=m_nPrinterInfoSize; nI++)			// enum new printers
					{
						if (GetPrinterStatus(&m_pPrinterInfo[nI], NULL))// printer in new PI-array "ready"
						{															// find printer in old PI-array
							nFound = FindPrinter(pPrinterInfoOld, nPrinterInfoOldSize, m_pPrinterInfo[nI].pPrinterName);
							if (nFound == -1)
							{														// not found in old PI-array: must be a new one
								m_nNewDefaultPrt = nI;						// set to new default printer
								break;
							}
						}
					}
				}
			}
		}		
		bResult = TRUE;
	}
	else
	{
		WK_TRACE(_T("Enumerate Printers FAILED\n\t\tSize is %u, needed %u\n\t\tError:%u, %s\n"),
										dwSize, dwNeeded, GetLastError(), GetLastErrorString()); 
	}
	WK_DELETE_ARRAY(pPrinterInfoOld);									// alte Drucker löschen

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetDefaultPrinter(int nIndex)
{
	CWK_Profile prof;

	if (nIndex < 0)
		return FALSE;

	// Realer Drucker oder der Drucker: 'Kein Drucker'?
	if (nIndex > 0)
	{
		// Setzen des Defaultprinters siehe Article ID: Q135387 	
		CString sDefault;
  		if (!GetPrinterStatus(&m_pPrinterInfo[nIndex], NULL))
		{
			prof.WriteInt(REGKEY_DVCLIENT, REGKEY_ENABLE_PRINTER, FALSE);
			UpdateButtons();
			return FALSE;
		}

		sDefault.Format(_T("%s,%s,%s"), (const char*)m_pPrinterInfo[nIndex].pPrinterName,
									(const char*)m_pPrinterInfo[nIndex].pDriverName,
									(const char*)m_pPrinterInfo[nIndex].pPortName);

		WriteProfileString(_T("windows"), _T("device"), sDefault);

		if (theApp.IsNT())
			SendMessageTimeout(HWND_BROADCAST, WM_WININICHANGE, 0L, 0L, SMTO_NORMAL, 1000, NULL);
		else
			SendMessageTimeout(HWND_BROADCAST, WM_WININICHANGE, 0L, (LPARAM)(LPCTSTR)_T("windows"), SMTO_NORMAL, 1000, NULL);

		prof.WriteInt(REGKEY_DVCLIENT, REGKEY_ENABLE_PRINTER, TRUE);
	}
	else
	{
		prof.WriteInt(REGKEY_DVCLIENT, REGKEY_ENABLE_PRINTER, FALSE);
	}

	UpdateButtons();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CPanel::GetDefaultPrinter()
{
	CWK_Profile prof;
	
	// Drucker deaktiviert?
	if (   theApp.IsReadOnly()
		|| prof.GetInt(REGKEY_DVCLIENT, REGKEY_ENABLE_PRINTER, FALSE))
	{
		// Defaultprinter ermitteln
		_TCHAR szbuffer[255];
		GetProfileString(_T("windows"), _T("device"), _T(""), szbuffer, 255);
		CString sDefaultPrinter = szbuffer;
		if (!sDefaultPrinter.IsEmpty())
		{
			int nIndex = sDefaultPrinter.Find(_T(","), 0);
			if (nIndex != -1)
				sDefaultPrinter = sDefaultPrinter.Left(nIndex);
		}

		return FindPrinter(m_pPrinterInfo, m_nPrinterInfoSize, sDefaultPrinter);
	}
	return 0; // 0 -> Drucker: 'Kein Drucker'.
}
/////////////////////////////////////////////////////////////////////////////
int CPanel::FindPrinter(PRINTER_INFO_2*ppi, int nSize, CString sPrinterName)
{
	for (int nI = 1; nI <= nSize; nI++)
	{
		if (ppi[nI].pPrinterName == sPrinterName)
			return nI;
	}
	return  -1;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::EnumerateLanguages()
{
	BOOL bResult	= FALSE;
	// look for language dlls
	CStringArray result;
	CString sSearch = szLangPrefix;
	sSearch += _T("???");	// Do NOT translate, wildcard for chars
#ifdef _DEBUG
	sSearch += szLangDebug;
#endif
	sSearch += szLangSuffix;
	int iFound = WK_SearchFiles(result, theApp.GetHomeDir(), sSearch, FALSE);
	if (iFound)
	{
		// Limit to 16 languages
		iFound = min(iFound, IDOSD_LANGUAGE16-IDOSD_LANGUAGE1+1);
		CString sPath, sFilename, sFilePath;
		CLangDllInfo* pDllInfo = NULL;
		for (int i = 0; i < iFound; i++)
		{								    
			sFilePath = result.GetAt(i);
			WK_SplitPath(sFilePath, sPath, sFilename);
			if (sPath.IsEmpty())
			{
				sFilePath = theApp.GetHomeDir() + _T("\\") + sFilename;
			}
			pDllInfo = GetDllLanguageInfo(sFilePath);
			if (pDllInfo)
			{
				m_arrayLanguages.Add(pDllInfo);
				WK_TRACE(_T("Installed Language: %s\n"), pDllInfo->GetLanguage()); 
				m_iLanguageCnt++;
			}
		}
		bResult = TRUE;
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
CLangDllInfo* CPanel::GetDllLanguageInfo(const CString& sDll)
{
	CLangDllInfo* pDllInfo = NULL;

	HINSTANCE hInstance = LoadLibraryEx(sDll,NULL,0);
	TRACE(_T("%s\n"), sDll);

	if (hInstance)
	{
		CString sLanguage, sAbbreviation, sCodePage;
		DWORD   dwBits = 0;
      FARPROC theProc = ::GetProcAddress(hInstance, "GetLanguageName");
		if (theProc)
		{
			sLanguage = ((GET_LANGUAGE_NAME_PTR)theProc)();
		}
      theProc = ::GetProcAddress(hInstance, "GetLanguageAbbreviation");
		if (theProc)
		{
			sAbbreviation = ((GET_LANGUAGE_ABBREVIATION_PTR)theProc)();
		}
      theProc = ::GetProcAddress(hInstance, "GetCodePage");
		if (theProc)
		{
			sCodePage = ((GET_CODE_PAGE_PTR)theProc)();
		}
      theProc = ::GetProcAddress(hInstance, "GetCodePageBits");
		if (theProc)
		{
			dwBits = ((GET_CODE_PAGE_BITS_PTR)theProc)();
		}
		if (   !sLanguage.IsEmpty()
			&& !sAbbreviation.IsEmpty()
			&& !sCodePage.IsEmpty()
			)
		{
			pDllInfo = new CLangDllInfo(sDll, sLanguage, sAbbreviation, sCodePage, dwBits);
		}
		FreeLibrary(hInstance);
	}
	return pDllInfo;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetDefaultLanguage(int iIndex)
{
	BOOL bFound = FALSE;
	if ((0 <= iIndex) && (iIndex < min(m_iLanguageCnt, m_arrayLanguages.GetSize())))
	{
		bFound = TRUE;
		m_pDefaultLanguage = m_arrayLanguages.GetAtFast(iIndex);
		LoadLanguageDll();
		LoadMenus();
		theApp.GetMainFrame()->InvalidateRect(NULL);
		theApp.GetMainFrame()->UpdateWindow();
	}
	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::LoadDefaultLanguageDll(CString sDefaultLanguage)
{
	BOOL bFound = FALSE;
#ifdef _DEBUG
	if (sDefaultLanguage.Find(szLangDebug) == -1)
	{
		sDefaultLanguage += _T("_");
		sDefaultLanguage += szLangDebug;
	}
#else
	int iIndex = sDefaultLanguage.Find(szLangDebug);
	if (iIndex != -1)
	{
		sDefaultLanguage = sDefaultLanguage.Left(iIndex-1);
	}
#endif

	
	// search for default language dll
	CLangDllInfo* pDllInfo = NULL;
	for (int i=0 ; !bFound && i<min(m_iLanguageCnt, m_arrayLanguages.GetSize()) ; i++)
	{
		pDllInfo = m_arrayLanguages.GetAtFast(i);
		if (pDllInfo->GetLanguage() == sDefaultLanguage)
		{
			m_pDefaultLanguage = pDllInfo;
			bFound = TRUE;
		}
	}
	// not found? Then search for english language dll
	if (!bFound && sDefaultLanguage != szDefaultLanguageEnu)
	{
		for (i=0 ; !bFound && i<min(m_iLanguageCnt, m_arrayLanguages.GetSize()) ; i++)
		{
			pDllInfo = m_arrayLanguages.GetAtFast(i);
			if (pDllInfo->GetLanguage() == szDefaultLanguageEnu)
			{
				m_pDefaultLanguage = pDllInfo;
				bFound = TRUE;
			}
		}
	}
	// not found? Then take the first one in array
	if (!bFound && i>0)
	{
		m_pDefaultLanguage = m_arrayLanguages.GetAtFast(0);
		//m_pDefaultLanguage = m_arrayLanguages.GetAt(1);
		bFound = TRUE;
	}

	if (bFound)
	{
		LoadLanguageDll();
		LoadMenus();
	}
	else
	{
		// not found? There is no dll!!! Should never be possible!!!
		WK_TRACE_ERROR(_T("No language dll found!!!\n"));
	}
	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
int CPanel::GetDefaultLanguageIndex()
{
	int iReturn = -1;
	// Default language
	for (int i = 0 ; i < min(m_iLanguageCnt, m_arrayLanguages.GetSize()) ; i++)
	{
		if (m_arrayLanguages.GetAtFast(i) == m_pDefaultLanguage)
		{
			iReturn = i;
		}
	}
	return iReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}
							
/////////////////////////////////////////////////////////////////////////////
int CPanel::GetActiveCamera()
{
	return m_nCamNr;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::ShowOnlineHelp()
{
	CString sMsg;

	switch (m_eInputState)
	{
		case InputSearchDate:
			sMsg.LoadString(IDS_HELP_INPUT_SEARCHDATE);
			break;
		case InputSearchTime:
			sMsg.LoadString(IDS_HELP_INPUT_SEARCHTIME);
			break;
		case InputPanelPIN:
			sMsg.LoadString(IDS_HELP_INPUT_PIN);
			break;
		case InputShutdownPIN:
			sMsg.LoadString(IDS_HELP_INPUT_PIN);
			break;
		case InputResetPIN:
			sMsg.LoadString(IDS_HELP_INPUT_PIN);
			break;
		case InputOldPIN:
			sMsg.LoadString(IDS_HELP_INPUT_OLD_PIN);
			break;
		case InputNewPIN:
			sMsg.LoadString(IDS_HELP_INPUT_NEW_PIN);
			break;
		case InputConfirmPIN:
			sMsg.LoadString(IDS_HELP_INPUT_CONFIRM_PIN);
			break;
		case InputConfigPIN:
			sMsg.LoadString(IDS_HELP_INPUT_PIN);
			break;
		case InputClockDate:
			sMsg.LoadString(IDS_HELP_INPUT_CLOCK_DATE);
			break;
		case InputClockTime:
			sMsg.LoadString(IDS_HELP_INPUT_CLOCK_TIME);
			break;
		case InputTimer:
			sMsg.LoadString(IDS_HELP_INPUT_TIMER);
			break;
		case InputIP:
			sMsg.LoadString(IDS_HELP_INPUT_TCPIP);
			break;
		case InputSubNetMask:
			sMsg.LoadString(IDS_HELP_INPUT_SUBNETMASK);
			break;
		case InputMSN:
			sMsg.LoadString(IDS_HELP_INPUT_MSN);
			break;
		case InputChangeCamName:
			sMsg.LoadString(IDS_HELP_INPUT_CAMERANAME);
			break;
		case InputBackupStartDateTime:
			sMsg.LoadString(IDS_HELP_INPUT_EXPORT_START);
			break;
		case InputBackupEndDateTime:
		case InputBackupWaitCalculatedEnd:
			sMsg.LoadString(IDS_HELP_INPUT_EXPORT_END);
			break;
		case InputBackupCameras:
			sMsg.LoadString(IDS_HELP_INPUT_EXPORT_CAMERAS);
			break;

		case InputBackupFoundNoEmptyCD:
		case InputBackupDVDNotAllowed:
			break;

		case InputBackupAskForMedium:
		case InputBackupWaitForMedium:
		case InputBackupFoundMedium:
			sMsg.LoadString(IDS_HELP_INPUT_WAIT_FOR_MEDIUM);
			break;

		case InputBackupEraseMedium:
		case InputBackupEraseMediumConfirmingYes:
		case InputBackupEraseMediumConfirmingNo:
			sMsg.LoadString(IDS_HELP_INPUT_ERASE_MEDIUM);
			break;

		case InputUpdateSystem:
			sMsg.LoadString(IDS_HELP_UPDATE_SYSTEM);
			break;
		case InputSelectHost:
			{
				CString sTemp;
				OnSelectHost();

				// Dies ist der 'erweitert' Eintrag
				if (m_sHostAddress == SPECIAL_HOSTIP)
				{
					CString sTemp;
					sTemp.LoadString(IDS_HELP_EXTEND);
					sMsg.Format(sTemp, theApp.GetMainFrame()->GetShortOemName());
				}
				else
				{
					CString sTemp;
					sTemp.LoadString(IDS_HELP_SELECT_HOST);
					sMsg.Format(sTemp, theApp.GetMainFrame()->GetShortOemName(),
									   theApp.GetMainFrame()->GetShortOemName());
				}
				CPoint CurPoint(0,0);
				GetCursorPos(&CurPoint);
				CWnd* pWnd = CWnd::WindowFromPoint(CurPoint);
				if (pWnd)
				{
					int nID = pWnd->GetDlgCtrlID();
					if (theApp.IsReceiver() &&
						(m_eInputState == InputSelectHost) && 
						(nID == IDC_BUTTON_EJECT))
					{
						nID = IDC_BUTTON_OK;
						sMsg.LoadString(nID);
					}
				}
			}
			break;
		case InputEditHostList:
			{
				CPoint CurPoint(0,0);
				GetCursorPos(&CurPoint);
				CWnd* pWnd = CWnd::WindowFromPoint(CurPoint);
				if (pWnd)
				{
					CString sTemp;
					sTemp.LoadString(pWnd->GetDlgCtrlID());
					// ACHTUNG: Alle Hilfetexte im Hosteingabe Dialog enthalten genau ein '%s', der
					// durch den 'ShortOemName' ersetzt wird.
					sMsg.Format(sTemp,	theApp.GetMainFrame()->GetShortOemName());
				}
			}
			break;
		case InputNo:
			{
			CPoint CurPoint(0,0);
			GetCursorPos(&CurPoint);
			CWnd* pWnd = CWnd::WindowFromPoint(CurPoint);
			if (   pWnd
//				&& WK_IS_WINDOW(this)	// CAVEAT: must be checked before IsChild
//				&& IsChild(pWnd)		// Tooltips of other Dlgs will not be displayed otherwise
			   )
			{
				int nID = pWnd->GetDlgCtrlID();
				if ((nID == IDC_DISPLAY_MENU1) || (nID == IDC_DISPLAY_MENU2))
					nID	 = m_pOSD->GetMenuID();
				if (theApp.IsReadOnly()	&& nID == IDC_BUTTON_EJECT)
				{
					nID = IDC_BUTTON_OK;
				}
				sMsg.LoadString(nID);

				if (nID == IDC_BUTTON_PRINT && m_pPrinterInfo)
				{
					CString sTmp;
					sTmp.Format(sMsg, m_pPrinterInfo[GetDefaultPrinter()].pPrinterName);
					sMsg = sTmp;
				}
				if (nID == IDC_BUTTON_EXPORT)
				{
					sMsg += _T(" ");
					sMsg += m_sExportDriveName;
				}
			}

			if (m_bSetValue && (m_pOSD && m_pOSD->GetMenuID() == IDOSD_TIME_ZONE))
			{
				if ((m_TziArray.GetSize() > 0) && (m_nTimeZone < m_TziArray.GetSize()))
					sMsg.Format(_T("%s"), m_TziArray.GetAtFast(m_nTimeZone)->GetDisplayName()); 
			}
			else if (m_bSetValue && (m_pOSD && m_pOSD->GetMenuID() == IDOSD_ALARM_MODE))
			{
				if ((m_eAlarmMode ==  UVVKassenConfirmingYes) ||
					(m_eAlarmMode ==  UVVKassenConfirmingNo) ||
					(m_eAlarmMode ==  MotionDetectionConfirmingYes) ||
					(m_eAlarmMode == MotionDetectionConfirmingNo))
					sMsg.LoadString(IDS_CONFIRM_RECORDINGMODE_CHANGING);
			}

			if (m_pOSD && m_pOSD->GetMenuID() == IDOSD_NET_NAME)
			{
				CString sTemp;
				sTemp.LoadString(IDOSD_NET_NAME);
				sMsg.Format(sTemp, theApp.GetMainFrame()->GetShortOemName(), theApp.GetMainFrame()->GetShortOemName());
			}

			if (m_pOSD && m_pOSD->GetMenuID() == IDOSD_OPTION_INFO)
			{
				sMsg.LoadString(IDOSD_OPTION_INFO);
			}

			if (sMsg.IsEmpty())
			{
				m_ctrlDisplayTooltip.StopScrolling();
				if (theApp.IsReceiver())
				{
					SetDisplayText(m_ctrlDisplayTooltip, m_sHostName, 0, 0);
				}
				else
				{
					SetDisplayText(m_ctrlDisplayTooltip, _T(""), 0, 0);
				}
			}
			}
			break;
		case InputCameraPTZID:
			break;
		case InputCameraPTZComPort:
			break;
	}

	//if IsReceiver, show different online helps for button "eject" in case of 
	//"normal menu" --> Disconnect from transmitter
	//"config menu" --> close config menu
	//is IsReadOnly --> Exit program
	if(    theApp.IsReceiver() 
		|| theApp.IsReadOnly())
	{
		CPoint CurPoint(0,0);
		GetCursorPos(&CurPoint);
		CWnd* pWnd = CWnd::WindowFromPoint(CurPoint);
		if (pWnd)
		{
			int nID = pWnd->GetDlgCtrlID();

			//only show different online help for button "eject" 
			//if not in menu "choosing host from list"
			//if not in menu "input pin"
			//else show "Exit prigram"
			if (nID == IDC_BUTTON_EJECT)
			{
				nID = IDC_BUTTON_OK; //Programm beenden
				sMsg.LoadString(nID);

				if(	theApp.IsReceiver()
					&& m_eInputState != InputSelectHost
					&& m_eInputState != InputPanelPIN)
				{

					//if a database window is open, close it
					BOOL bIsDBWindow = FALSE;
					CMainFrame* pMainWnd = theApp.GetMainFrame();
					if (pMainWnd)
					{
						CPlayWindow* pPlayWnd = pMainWnd->GetPlayWindow((WORD)m_nCamNr);
						if (pPlayWnd)
						{
							if (pPlayWnd->IsWindowVisible())
							{
								bIsDBWindow = TRUE;
							}
						}
					}
					if(bIsDBWindow)
					{
						nID = IDC_BUTTON_EJECT ; //Datenbankfenster schließen
						sMsg.LoadString(nID);
					}
					else
					{
						if(IsNormalMenu())
						{
							sMsg.LoadString(IDS_DISCONNECT); //Verbindung trennen
						}
						else if(IsConfigMenu())
						{
							sMsg.LoadString(IDS_END_OPTION);  //Optionsmenü verlassen
						}
					}

				}
			}
		}
	}

	if (!sMsg.IsEmpty())
	{
		int nIndex = sMsg.Find(_T("|"));
		if (nIndex != -1)
		{
			if (!m_bSetValue)
			{
				sMsg = sMsg.Left(nIndex);
			}
			else
			{
				sMsg = sMsg.Mid(nIndex+1);
			}
		}
		if (m_sPrevMsg != sMsg)
		{
			m_ctrlDisplayTooltip.StopScrolling();
			SetDisplayText(m_ctrlDisplayTooltip, _T(""), 0, 0);
			m_sPrevMsg = sMsg;
		}
	} 
	SetTooltipText(sMsg);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::SetTooltipText(const CString &sMsg, bool bForce /*=false*/)
{
	if (!sMsg.IsEmpty())
	{
		if (bForce)
		{
			m_ctrlDisplayTooltip.StopScrolling();
			SetDisplayText(m_ctrlDisplayTooltip, sMsg, SCROLL_TEXT_NO_WRAP|SCROLL_TEXT_WAIT, 0);
		}
		else
		{
			SetDisplayText(m_ctrlDisplayTooltip, sMsg, SCROLL_TEXT_NO_WRAP, 0);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CPanel::GetMSN()
{
	CWK_Profile prof;
#ifdef _UNICODE
	m_sMSN = prof.GetString(REGKEY_ISDNUNIT, REGKEY_OWNNUMBER, m_sMSN, TRUE);
#else
	m_sMSN = prof.GetString(REGKEY_ISDNUNIT, REGKEY_OWNNUMBER, m_sMSN);
#endif
	CWK_Profile::Decode(m_sMSN);
	m_sMSN.TrimRight();
	if (m_sMSN == _T("A"))
		m_sMSN = _T("");

	return m_sMSN;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetMSN(const CString &sMSN)
{
	if (!ValidateMSN(sMSN))
		return FALSE;
	
	m_sMSN = sMSN;
	
	SaveConfiguration();

	CConnectionRecord c1;
	CIPCFetch fetch1;
	c1.SetDestinationHost(WK_APP_NAME_ISDN);
	fetch1.FetchServerReset(c1);

	CConnectionRecord c2;
	CIPCFetch fetch2;
	c2.SetDestinationHost(WK_APP_NAME_ISDN2);
	fetch2.FetchServerReset(c2);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetIP(const CString &sIPAddress, CString sParam, BOOL bForceReboot/*=TRUE*/)
{
	BOOL bResult = FALSE;
	CString sNetCard;
	CString	sIP;
	CStringArray sKeyList;

	if (!ValidateIP(sIPAddress))
		return FALSE;

	// Geht derzeit nur unter WinNT
	if (!theApp.IsNT())
		return FALSE;

	// Konnte bisherige IP-Adresse korrekt gelesen werden?
	if (!GetIP(sIP, sParam, &sKeyList))
		return FALSE;
	
	// Hat sich die IP-Adresse geändert?

	if (sIPAddress == sIP)
		return TRUE;

	sIP = ConvertIP(sIPAddress, TRUE);
	if (sIP.IsEmpty())
	{
		return FALSE;
	}

	
	if(sParam == DEFAULTGATEWAY)
	{
		//soll Gateway Adresse gelöscht werden (IP = 000.000.000.000) dann
		if (sIP == _T("0.0.0.0"))
		{
			// den Eintrag auf _T("leer") setzen
			sIP.Empty();
			m_sNetShellCmd.Format(_T("set address name=\"%s\" gateway=none"), m_sNetworkAdapterName, sIP);
		}
		else
		{
			m_sNetShellCmd.Format(_T("set address name=\"%s\" gateway=%s gwmetric=255.255.255.0"), m_sNetworkAdapterName, sIP);
		}
	}
	else if (sParam == SUBNETMASK)
	{
		CString sAdr, sSub(sIP);
		if (!GetIP(sAdr, IPADDRESS))
		{
			sAdr = m_sIP;
			if (!ValidateIP(sAdr))
			{
				sAdr.Empty();
			}
		}
		if (!sAdr.IsEmpty())
		{
			sAdr = ConvertIP(sAdr, TRUE);
			m_sNetShellCmd.Format(_T("set address \"%s\" static %s %s"), m_sNetworkAdapterName, sAdr, sSub);
		}
	}
	else if (sParam == IPADDRESS)
	{
		CString sSub;
		if (!GetIP(sSub, SUBNETMASK))
		{
			sSub = m_sSubNetMask;
			if (!ValidateIP(sSub))
			{
				sSub.Empty();
			}
		}
		if (!sSub.IsEmpty())
		{
			sSub = ConvertIP(sSub, TRUE);
			m_sNetShellCmd.Format(_T("set address \"%s\" static %s %s"), m_sNetworkAdapterName, sIP, sSub);
		}
	}

	if (!m_sNetShellCmd.IsEmpty() && SendNetShellCmd())
	{
		return TRUE;
	}

	HKEY	hSecKey	= NULL;
	LONG	lResult	= 0;
	BOOL	bMulti	= FALSE;
	int		nKeys	= sKeyList.GetSize(), i;
	if (nKeys == 0)
	{
		WK_TRACE(_T("Error no IP parameter entries\n"));
	}
	else if (nKeys > 1)
	{
		WK_TRACE(_T("Found Multiple Parameter Entries\n"));
		bMulti = TRUE;
	}

	for (i=0; i<nKeys; i++)
	{
		const CString &sKey = sKeyList.GetAt(i);
		if (bMulti)
		{
			WK_TRACE(_T("%d: %s\n"), i+1, sKey);
		}
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_WRITE, &hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			int iLen = (sIP.GetLength()+1) * sizeof(TCHAR);
			lResult = RegSetValueEx(hSecKey, sParam, 0, REG_MULTI_SZ, (const BYTE*)(LPCTSTR)sIP, iLen);
			if (lResult == ERROR_SUCCESS)
			{
				bResult = TRUE;
				if (!m_bDHCPenabled && sParam != DEFAULTGATEWAY)
				{
					sIP = _T("0.0.0.0");
					iLen = (sIP.GetLength()+1) * sizeof(TCHAR);
					RegSetValueEx(hSecKey, _T("Dhcp") IPADDRESS, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sIP), iLen);
					sIP = _T("255.0.0.0");
					iLen = (sIP.GetLength()+1) * sizeof(TCHAR);
					RegSetValueEx(hSecKey, _T("Dhcp") SUBNETMASK, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sIP), iLen);
				}
			}
		}
		REG_CLOSE_KEY(hSecKey);
	}
	// Neustart ist notwendig.
	if (bResult && bForceReboot)
		DoReboot();

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
CString CPanel::FormatIP(DWORD dwIP)
{
	CString sIP;
	sIP.Format(_T("%03d%03d%03d%03d"), (dwIP & 0x000000ff) >> 0,
									(dwIP & 0x0000ff00) >> 8,						
									(dwIP & 0x00ff0000) >> 16,					
									(dwIP & 0xff000000) >> 24);				
	return sIP;
}
/////////////////////////////////////////////////////////////////////////////
CString CPanel::ConvertIP(const CString& sIPin, BOOL bWantPoints)
{
	int n1=0, n2=0, n3=0, n4=0;
	BOOL bPoints = TRUE;
	CString sIP;

	if (sIPin.Find(_T(".")) == -1)
	{
		bPoints = FALSE;
	}
	if (bPoints == bWantPoints)
	{
		return sIPin;
	}
	if (bPoints)
	{
		if (4==_stscanf(sIPin, _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4))
		{
			sIP.Format(_T("%03d%03d%03d%03d"), n1, n2, n3, n4);
		}
	}
	else
	{
		if (4==_stscanf(sIPin, _T("%03d%03d%03d%03d"), &n1, &n2, &n3, &n4))
		{
			sIP.Format(_T("%d.%d.%d.%d"), n1, n2, n3, n4);
		}
	}
	return sIP;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetIP(CString& sIP, CString sParam, CStringArray* psKeyList)
{
	BOOL	bResult = FALSE;
	HKEY	hKey     = NULL;
	HKEY  hSecKey  = NULL;
	DWORD	dwType   = 0,
			dwIndex,
			dwLen    = 0;
	LONG	lResult  = 0;
	_TCHAR  szName[MAX_PATH];
	CString sNetCard;

	if (sParam == DEFAULTGATEWAY)
	{
		bResult = GetDefaultGateway(sIP);
	}
	else
	{
		PMIB_IPADDRTABLE pIpAddrTable = NULL;
		DWORD dwDummy, dwSize = sizeof(DWORD);
		GetIpAddrTableU((PMIB_IPADDRTABLE)&dwDummy, &dwSize, FALSE);
		DWORD dwIP = 0;
		if (dwSize > 0)
		{		 
			pIpAddrTable = (PMIB_IPADDRTABLE) new BYTE[dwSize];
			
			// Hole die erste IP Adresse, die nicht 127.0.0.1 lautet
			if (pIpAddrTable && (GetIpAddrTableU(pIpAddrTable, &dwSize, FALSE) == NO_ERROR))
			{
				for (DWORD dwI = 0; dwI < pIpAddrTable->dwNumEntries; dwI++)
				{
					dwIP = 0;
					if (m_dwInterfaceIndex != 0xffffffff)
					{
						if (m_dwInterfaceIndex != pIpAddrTable->table[dwI].dwIndex)
						{
							continue;
						}
					}
					if (sParam == IPADDRESS)
					{
						dwIP = pIpAddrTable->table[dwI].dwAddr;
					}
					else if (sParam == SUBNETMASK)
					{
						dwIP = pIpAddrTable->table[dwI].dwMask;
					}
					
					if (   pIpAddrTable->table[dwI].dwAddr != 0x0100007f 
						&& pIpAddrTable->table[dwI].dwAddr != 0)
					{
						sIP = FormatIP(dwIP);
						bResult = TRUE;
						break;
					}
				}
			}
			WK_DELETE(pIpAddrTable);
		}
		if (dwIP == 0)
		{
			bResult = FALSE;
		}
	}

	if (bResult && psKeyList == NULL)
	{
		return bResult;					// ready, entry found
	}
	// if the keylist is not NULL determine the IP-adapter path

	if (m_bDHCPenabled && sParam != DEFAULTGATEWAY)
	{
		sParam = _T("Dhcp") + sParam;
	}

	if (!m_sNetworkAdapterKey.IsEmpty())
	{
		if (psKeyList) psKeyList->Add(m_sNetworkAdapterKey);
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_sNetworkAdapterKey, 0, KEY_READ, &hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			dwLen = MAX_PATH-1;
			lResult = RegQueryValueEx(hSecKey, sParam, NULL, &dwType, (LPBYTE)szName, &dwLen);
			if ((lResult == ERROR_SUCCESS) && (dwType == REG_MULTI_SZ))
			{
				sIP = ConvertIP(szName, FALSE);
				bResult = TRUE;
			}
		}
		REG_CLOSE_KEY(hSecKey);
	}
	else
	{
		try
		{
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NETWORKCARD_NT, 0, KEY_READ, &hKey);
			if (lResult != ERROR_SUCCESS)			throw (int) 1;

			for (dwIndex=0; ; dwIndex++)
			{
				dwLen   = MAX_PATH;												// Key ermitteln
				lResult = RegEnumKeyEx(hKey, dwIndex, szName, &dwLen, NULL, NULL, 0, NULL);
				if (lResult != ERROR_SUCCESS)		throw (int) 0;			// beenden, wenn kein weiterer existiert
																						// Oeffnen
				lResult = RegOpenKeyEx(hKey, szName, 0, KEY_READ, &hSecKey);
				if (lResult != ERROR_SUCCESS)		throw (int) 2;			// 

				lResult = RegQueryValueEx(hSecKey, SERVICENAME, NULL, &dwType, NULL,&dwLen);
				if (lResult != ERROR_SUCCESS)	throw (int) 3;
				
				lResult = RegQueryValueEx(hSecKey, SERVICENAME, NULL, &dwType, (LPBYTE)sNetCard.GetBuffer(dwLen/sizeof(TCHAR)), &dwLen);
				sNetCard.ReleaseBuffer();
				REG_CLOSE_KEY(hSecKey);
				if (lResult != ERROR_SUCCESS)	continue;

				CString sKey;
				if (((CDVClientApp*)AfxGetApp())->IsWinXP())
				{
					sKey.Format(_T("%s\\Tcpip\\Parameters\\Interfaces\\%s\\"), SERVICES, sNetCard);
				}
				else
				{
					sKey.Format(_T("%s\\%s\\Parameters\\tcpip"), SERVICES, sNetCard);
				}

				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_READ, &hSecKey);
				if (lResult != ERROR_SUCCESS)	continue;
				dwLen = MAX_PATH-1;
				lResult = RegQueryValueEx(hSecKey, sParam, NULL, &dwType, (LPBYTE)szName, &dwLen);
				if ((lResult == ERROR_SUCCESS) && (dwType == REG_MULTI_SZ))
				{
					if (psKeyList) psKeyList->Add(sKey);
					WK_TRACE(_T("Adresses-> %s : %s\n"), szName, sIP);
					if (sIP == _T("000000000000"))
					{
						CString str;
						str.LoadString(IDS_CONNECT_ERROR);
						SetTooltipText(str, true);
						bResult = FALSE;
					}

					if (bResult == FALSE)
					{
						sIP = ConvertIP(szName, FALSE);
					}
					bResult = TRUE;
				}
				REG_CLOSE_KEY(hSecKey);
			}
		}
		catch (int nError)
		{
			if (nError)
			{
				WK_TRACE(_T("Error OnGetIP(%s): %d, %d"), sParam, nError, GetLastError());
			}
		}

		REG_CLOSE_KEY(hKey);
		REG_CLOSE_KEY(hSecKey);
	}
	return bResult;
}

BOOL CPanel::DHCPParameters(BOOL bSet/*=FALSE*/, BOOL bDHCP/*=-1*/)
{
	BOOL bReturn = FALSE;
	BOOL bGotAI = FALSE;
	if (pGetAdaptersInfo && pGetIfEntry)
	{
		DWORD dwRetVal = 0;
		ULONG ulOutBufLen = sizeof(DWORD);
		// Declare and initialize variables
//		PIP_INTERFACE_INFO pInfo = NULL;
		IP_ADAPTER_INFO *pAI = NULL, *pAInext;
		MIB_IFROW ifrow;
		ZERO_INIT(ifrow);

		dwRetVal = pGetAdaptersInfo(NULL, &ulOutBufLen);
		if (dwRetVal == ERROR_BUFFER_OVERFLOW)
		{
			pAI = (IP_ADAPTER_INFO*) new BYTE[ulOutBufLen];
		}
		dwRetVal = pGetAdaptersInfo(pAI, &ulOutBufLen);
		if (dwRetVal == NO_ERROR)
		{
			CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
			int nAdapter = 0;
			int nUsedAdapter = theApp.GetIntValue(_T("UsedNetworkAdapter"));
			BOOL bInitNetworkAdapter = m_sNetworkAdapterKey.IsEmpty();
			if (bInitNetworkAdapter)
			{
				WK_TRACE(_T("Used Adapter: %d\n"), nUsedAdapter);
			}
			pAInext = pAI;
			while (pAInext)
			{
				CString sAdapterName(pAInext->AdapterName), sKey, sName;
				ifrow.dwIndex = pAInext->Index;
				dwRetVal = pGetIfEntry(&ifrow);
				if (   dwRetVal == NO_ERROR 
					&& ifrow.dwAdminStatus == MIB_IF_ADMIN_STATUS_UP
//					&& ifrow.dwOperStatus == MIB_IF_OPER_STATUS_OPERATIONAL
					&& nAdapter == nUsedAdapter)
				{
					if (bInitNetworkAdapter)
					{
						sKey.Format(_T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), sAdapterName);
						WK_TRACE(_T("Network location: %s\n"), sKey);
						m_sNetworkAdapterName = wkpSystem.GetString(sKey, _T("Name"), NULL);
						if (theApp.IsWinXP())
						{
							m_sNetworkAdapterKey.Format(_T("%s\\Tcpip\\Parameters\\Interfaces\\%s"), SERVICES, sAdapterName);
						}
						else
						{
							m_sNetworkAdapterKey.Format(_T("%s\\%s\\Parameters\\tcpip"), SERVICES, sAdapterName);
						}
					}
					m_bDHCPenabled = pAInext->DhcpEnabled;
					m_dwInterfaceIndex = pAInext->Index;
					bGotAI = TRUE;
				}
				if (bInitNetworkAdapter)
				{
					WK_TRACE(_T("AdapterName(%d): %s\n"), nAdapter, sAdapterName);
					WK_TRACE(_T("Return value: %d\n"), dwRetVal);
					WK_TRACE(_T("Index: %d\n"), pAInext->Index);
					WK_TRACE(_T("AdminStatus: %d\n"), ifrow.dwAdminStatus);
					WK_TRACE(_T("OperStatus: %d\n"), ifrow.dwOperStatus);
				}
				nAdapter++;
				pAInext = pAInext->Next;
			};
		}
		WK_DELETE(pAI);
	}
	if (bSet)
	{
		if (bDHCP)
		{
			m_sNetShellCmd.Format(_T("set address name=\"%s\" dhcp"), m_sNetworkAdapterName);
		}
		else
		{
			CString sIP, sSub;
			GetIP(sIP, IPADDRESS);
			sIP = ConvertIP(sIP, TRUE);
			GetIP(sSub, SUBNETMASK);
			sSub = ConvertIP(sSub, TRUE);
			m_sNetShellCmd.Format(_T("set address \"%s\" static %s %s"), m_sNetworkAdapterName, sIP, sSub);
		}
		if (SendNetShellCmd())
		{
			m_bDHCPenabled = bDHCP;
			return TRUE;
		}
	}
	else if (bGotAI)
	{
		return TRUE;
	}

	if (m_sNetworkAdapterKey.IsEmpty())
	{
		CStringArray sKeyList;
		CString sIP;

		// Runs only under WinNT
		if (!theApp.IsNT()) return bReturn;

		// could we read an ip address
		if (GetIP(sIP, IPADDRESS, &sKeyList) && sKeyList.GetSize())
		{
			m_sNetworkAdapterKey = sKeyList.GetAt(0);
		}
		else
		{
			return bReturn;
		}
	}

	if (!m_sNetworkAdapterKey.IsEmpty())
	{
		HKEY hSecKey = NULL;
		LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_sNetworkAdapterKey, 0, bSet ? KEY_READ|KEY_WRITE: KEY_READ, &hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			if (bSet)
			{
				lResult = RegSetValueEx(hSecKey, ENABLEDHCP, 0, REG_DWORD, (BYTE*)&bDHCP, sizeof(BOOL));
				if (lResult == ERROR_SUCCESS)
				{
					m_bDHCPenabled = bDHCP;
					bReturn = DO_REBOOT;
				}
				if (bReturn)
				{
					if (m_bDHCPenabled)
					{
						CString sAddress = _T("0.0.0.0");
						RegSetValueEx(hSecKey, IPADDRESS, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
						RegSetValueEx(hSecKey, SUBNETMASK, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
					}
					else
					{
						CString sAddress;
						if (GetIP(sAddress, IPADDRESS))
						{
							sAddress = ConvertIP(sAddress, TRUE);
							RegSetValueEx(hSecKey, IPADDRESS, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
						}
						if (GetIP(sAddress, SUBNETMASK))
						{
							sAddress = ConvertIP(sAddress, TRUE);
							RegSetValueEx(hSecKey, SUBNETMASK, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
						}
						sAddress = _T("0.0.0.0");
						RegSetValueEx(hSecKey, _T("Dhcp") IPADDRESS, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
						sAddress = _T("255.0.0.0");
						RegSetValueEx(hSecKey, _T("Dhcp") SUBNETMASK, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
					}
				}
			}
			else
			{
				DWORD dwLen = sizeof(m_bDHCPenabled), dwType, dwValue;
				lResult = RegQueryValueEx(hSecKey, ENABLEDHCP, NULL, &dwType, (LPBYTE)&dwValue, &dwLen);
				if (lResult == ERROR_SUCCESS)
				{
					m_bDHCPenabled = dwValue;
					bReturn = TRUE;
				}
			}
		}
		REG_CLOSE_KEY(hSecKey);
		if (!bReturn)
		{
			WK_TRACE(_T("DHCPParameters %sValue failed: %d"), bSet ? _T("Set") : _T("Get"), lResult);
		}
	}
	else
	{
		WK_TRACE(_T("No Registrypath for tcpip adapter"));
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SendNetShellCmd()
{
	if (m_bNetShellOK && !m_sNetworkAdapterName.IsEmpty())
	{
		DWORD dwFlags = WKCPF_INPUT|WKCPF_OUTPUT|WKCPF_OUTPUT_CHECK|WKCPF_TERMINATE;
		m_pControlProcess = new CWKControlledProcess(_T("netsh.exe"), NULL, NULL, dwFlags);
		m_pControlProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, NET_SHELL_EVENT_START, 0);
		m_pControlProcess->StartThread();
		return TRUE;
	}
	m_sNetShellCmd.Empty();
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetSubNetMask(const CString &sSubNetMask)
{
	BOOL bResult = FALSE;
    HKEY	hKey;
	CString sNetCard;
	CString sMask;
	DWORD	dwType = 0;
	DWORD	dwLen  = 0;
	DWORD	dwResult = 0;

	if (!ValidateSubNetMask(sSubNetMask))
		return FALSE;

	// Geht derzeit nur unter WinNT
	if (!theApp.IsNT())
		return FALSE;

	// Konnte Subnetmask korrekt gelesen werden?
	if (!GetSubNetMask(sMask))
		return FALSE;

	// Hat sich die Subnetmask geändert.
	if (sSubNetMask == sMask)
		return TRUE;

	sMask = ConvertIP(sSubNetMask, TRUE);
	if (sMask.IsEmpty())
	{
		return FALSE;
	}

	CString sIP;
	GetIP(sIP, IPADDRESS);
	sIP = ConvertIP(sIP, TRUE);
	m_sNetShellCmd.Format(_T("set address \"%s\" static %s %s"), m_sNetworkAdapterName, sIP, sMask);
	if (SendNetShellCmd())
	{
		return TRUE;
	}

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NETWORKCARD_NT, 0, KEY_QUERY_VALUE, &hKey);
	if (dwResult == ERROR_SUCCESS)
	{
		dwResult = RegQueryValueEx(hKey, _T("ServiceName"), NULL, &dwType, NULL,&dwLen);
		if (dwResult == ERROR_SUCCESS)
		{
			dwResult = RegQueryValueEx(hKey, _T("ServiceName"), NULL, &dwType, (LPBYTE)sNetCard.GetBuffer(dwLen/sizeof(TCHAR)), &dwLen);
			sNetCard.ReleaseBuffer();
			if (dwResult == ERROR_SUCCESS)
			{
				CString sKey;
				sKey.Format(_T("%s\\%s\\%s"), SERVICES, sNetCard, _T("Parameters\\tcpip"));
				
				RegCloseKey(hKey);
				dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_WRITE, &hKey);								
				if (dwResult == ERROR_SUCCESS)
				{
					int iLen = (sMask.GetLength()+1) * sizeof(TCHAR);
					dwResult = RegSetValueEx(hKey, _T("SubnetMask"), 0, REG_MULTI_SZ, (const BYTE*)(LPCTSTR)sMask, iLen);
					if (dwResult == ERROR_SUCCESS)
					{
						bResult = DO_REBOOT;
					}
				}		
			}
		}
		RegCloseKey(hKey);
	}

	if (bResult == DO_REBOOT)
		DoReboot();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetSubNetMask(CString& sMask)
{
	BOOL	bResult = FALSE;

	PMIB_IPADDRTABLE pIpAddrTable = NULL;
	MIB_IPADDRTABLE IpAddrTable;
	DWORD	dwSize = sizeof(IpAddrTable);
	GetIpAddrTableU(&IpAddrTable, &dwSize, FALSE);
	if (dwSize > 0)
	{
		pIpAddrTable = (PMIB_IPADDRTABLE) new BYTE[dwSize];

		DWORD dwRet = GetIpAddrTableU(pIpAddrTable, &dwSize, FALSE);
		
		// Hole Standard Gateway
		if (pIpAddrTable && (dwRet == NO_ERROR))
		{
			for (DWORD dwI = 0; dwI < pIpAddrTable->dwNumEntries; dwI++)
			{
				if (pIpAddrTable->table[dwI].dwAddr != 0x0100007f)
				{
					DWORD dwMask = pIpAddrTable->table[dwI].dwMask;
					sMask.Format(_T("%03d%03d%03d%03d"), (dwMask & 0x000000ff) >> 0,
												     (dwMask & 0x0000ff00) >> 8,						
												     (dwMask & 0x00ff0000) >> 16,					
												     (dwMask & 0xff000000) >> 24);				
					bResult = TRUE;
					break;
				}
			}
		}
		else
			WK_TRACE_ERROR(_T("GetIpAddrTable failed (0x%s"), dwRet);
		WK_DELETE(pIpAddrTable);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSetDefaultGateway(const CString &sNewGateway)
{
	BOOL bResult = FALSE;
    HKEY	hKey;
	CString sNetCard;
	CString	sGateway;
	DWORD	dwType = 0;
	DWORD	dwLen  = 0;
	DWORD	dwResult = 0;

	if (!ValidateIP(sNewGateway))
		return FALSE;

	// Geht derzeit nur unter WinNT
	if (!theApp.IsNT())
		return FALSE;

	// Konnte bisherige IP-Adresse korrekt gelesen werden?
//	if (!GetDefaultGateway(sGateway))
//		return FALSE;
	GetDefaultGateway(sGateway);
	
	// Hat sich die IP-Adresse geändert?
	if (sNewGateway == sGateway)
		return TRUE;

	sGateway.Format(_T("%s.%s.%s.%s"),	sNewGateway.Mid(0, 3),
									sNewGateway.Mid(3, 3),
									sNewGateway.Mid(6, 3),
									sNewGateway.Mid(9, 3));

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NETWORKCARD_NT, 0, KEY_QUERY_VALUE, &hKey);
	if (dwResult == ERROR_SUCCESS)
	{
		dwResult = RegQueryValueEx(hKey, _T("ServiceName"), NULL, &dwType, NULL,&dwLen);
		if (dwResult == ERROR_SUCCESS)
		{
			dwResult = RegQueryValueEx(hKey, _T("ServiceName"), NULL, &dwType, (LPBYTE)sNetCard.GetBuffer(dwLen/sizeof(TCHAR)), &dwLen);
			sNetCard.ReleaseBuffer();
			if (dwResult == ERROR_SUCCESS)
			{
				CString sKey;
				sKey.Format(_T("%s\\%s\\%s"), SERVICES, sNetCard, _T("Parameters\\tcpip"));
				
				RegCloseKey(hKey);
				dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_WRITE, &hKey);								
				if (dwResult == ERROR_SUCCESS)
				{
					int iLen = (sGateway.GetLength()+1) * sizeof(TCHAR);
					dwResult = RegSetValueEx(hKey, DEFAULTGATEWAY, 0, REG_MULTI_SZ, (const BYTE*)LPCTSTR(sGateway), iLen);
					if (dwResult == ERROR_SUCCESS)
					{
						bResult = TRUE;
					}
				}		
			}
		}
		RegCloseKey(hKey);
	}


	// Neustart ist notwendig.
	if (bResult)
		DoReboot();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetDefaultGateway(CString& sGateway)
{
	BOOL	bResult = FALSE;

	PMIB_IPFORWARDTABLE pIPForwardTbl = NULL;
	MIB_IPFORWARDTABLE IPForwardTbl;
	DWORD dwSize = sizeof(IPForwardTbl);
	GetIpForwardTableU(&IPForwardTbl, &dwSize, FALSE);
	if (dwSize > 0)
	{
		pIPForwardTbl = (PMIB_IPFORWARDTABLE) new BYTE[dwSize];

		if (pIPForwardTbl && GetIpForwardTableU(pIPForwardTbl, &dwSize, FALSE) == NO_ERROR)
		{
			for (DWORD dwI = 0; dwI < pIPForwardTbl->dwNumEntries; dwI++)
			{
				MIB_IPFORWARDROW    IPForwardRow = pIPForwardTbl->table[dwI];
				if ((IPForwardRow.dwForwardDest == 0)	 &&
					(IPForwardRow.dwForwardMask == 0))
				{
					DWORD dwGateway = IPForwardRow.dwForwardNextHop;
					if (dwGateway != 0)
					{
						sGateway.Format(_T("%03d%03d%03d%03d"), (dwGateway & 0x000000ff) >> 0,
															(dwGateway & 0x0000ff00) >> 8,						
															(dwGateway & 0x00ff0000) >> 16,					
															(dwGateway & 0xff000000) >> 24);				
						bResult = TRUE;
					}						
					break;
				}
			}
		}
		WK_DELETE(pIPForwardTbl);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnChangeLocalHostName()
{
	BOOL bResult = FALSE;
	m_pKeyboardDlg = new CKeyboardDlg(&m_ctrlDisplayMenu2, m_pOSD->GetSubString(), GetLocalHostName(), this);
	if (m_pKeyboardDlg)
	{
		EnablePanel(FALSE);
		m_eInputState = InputChangeNetName;
		m_pKeyboardDlg->m_sNotAllowedChars = _T("/\\[]:|<>+=;,? ");
		m_pKeyboardDlg->m_bOnlyAsscii = TRUE;
		int nResult = m_pKeyboardDlg->DoModal();
		m_eInputState = InputNo;
		if (nResult == IDOK)
		{
			bResult = SetLocalHostName(m_pKeyboardDlg->GetInputString());
#ifdef _TEST_DHCP_SETTINGS
			if (m_bDHCPenabled && bResult)
			{
				DoReboot();
			}
#endif
		}
		WK_DELETE(m_pKeyboardDlg);
		EnablePanel(TRUE);
		OnUpdateMenu(0,0);
		SaveConfiguration();
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
CString CPanel::GetLocalHostName()
{
#ifdef _TEST_DHCP_SETTINGS
	if (m_bDHCPenabled && theApp.IsWinXP() && theApp.GetIntValue(CAN_SET_COMPUTERNAME, FALSE, FALSE))
	{
		_TCHAR szCompName[MAX_COMPUTERNAME_LENGTH];
		DWORD dwSize = MAX_COMPUTERNAME_LENGTH;
		if (GetComputerNameEx(ComputerNameDnsHostname, szCompName, &dwSize))
		{
			return CString(szCompName);
		}
	}
#endif

	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	CHostArray		Hosts;
	Hosts.Load(prof);
	return Hosts.GetLocalHostName();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetLocalHostName(const CString& sLocalHostName, BOOL bImportSettings /*= FALSE */)
{
	BOOL bReturn = TRUE;
#ifdef _TEST_DHCP_SETTINGS
	if (m_bDHCPenabled)
	{
		if (theApp.IsWinXP() && theApp.GetIntValue(CAN_SET_COMPUTERNAME, FALSE, FALSE))
		{
			CString sUpperName(sLocalHostName);		// to change the computername
			sUpperName.MakeUpper();

			bReturn = SetComputerNameEx(ComputerNamePhysicalNetBIOS, sUpperName) &&
					  SetComputerNameEx(ComputerNamePhysicalDnsHostname, sLocalHostName);
			if (bReturn)							// if it worked
			{
				theApp.m_bComputerNameValid = TRUE;
				CWK_Profile system(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""),_T(""));
				CString sComputerName = system.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER, _T(""));
				if (sComputerName.IsEmpty())		// lookup for computername saved befor
				{									// save the old computername once for update and
													// service purpose
					system.WriteString(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER, theApp.GetComputerName());
				}
			}
			else
			{
				CString sMsg;
				sMsg.LoadString(IDS_SET_COMPUTERNAME_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				SetFormatDisplayText(m_ctrlDisplayMenu2, _T(""), m_pOSD->GetSubString());
			}
		}
		else
		{
			bReturn = FALSE;
		}
	}
#endif

	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	CHostArray		Hosts;
	Hosts.Load(prof);
	Hosts.SetLocalHostName(sLocalHostName);
	Hosts.Save(prof);

	// Damit im Destruktor von CPanel der LocalHostname nicht wieder durch
	// m_HostsAll.Save(prof); überschrieben wird.
	m_HostsAll.SetLocalHostName(sLocalHostName);

	if(bImportSettings)
	{
		return TRUE; 
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::ForceConfirmSelfcheck()
{
	CWnd* pWnd = FindWindow(NULL, _T("DVStarter"));
	if (WK_IS_WINDOW(pWnd))
	{
		DoConfirmSelfcheck((WPARAM)pWnd->m_hWnd,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_DV_CLIENT, 0);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnActivityBackup()
{
	BOOL bResult = FALSE;

	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (!pMainWnd->IsInBackup())
		{
			m_bMultiBackup = FALSE;
			m_bAlarmBackup = FALSE;
			bResult = OnBackup();
		}
		else
		{
			WK_TRACE(_T("OnActivityBackup() cannot backup PlayStatus not allowed\n"));
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnStopActivityBackup()
{
	//Backup kann nur beim Backup mit Nero abgebrochen werden, nicht beim backup mit DirectCD
	BOOL bResult = FALSE;
	CServer* pServer = theApp.GetServer();

	EnableInputMode();

	if(m_eConfirmingYesNoMode == ConfirmingModeUnknown)
	{
		m_eConfirmingYesNoMode = ConfirmingNo;
	}
	
	else if(m_eConfirmingYesNoMode == ConfirmingNo)
	{
		m_eConfirmingYesNoMode = ConfirmingModeUnknown;
		DisableInputMode();
		
	}
	else if(m_eConfirmingYesNoMode == ConfirmingYes)
	{
		DisableInputMode();
		if (pServer)
		{
			if (pServer->IsDatabaseConnected())
			{

				CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
				if (pBackupWnd)
				{
					pBackupWnd->SetPrepareStopBackup(TRUE);

					//Zwischenspeicherung geschieht im Zustand: PS_BACKUP_RUNNING
					//Zwischenspeichern der RO-Software: PS_BACKUP_COPY_RO -> PS_BACKUP_COPYING_RO 
					//mit ACDC und Nero gebrannt wird bei: PS_BACKUP_ACDC_START -> PS_BACKUP_ACDC_WAIT_COMPLETE
					//Brennen ist beendet bei : PS_BACKUP_ACDC_COMPLETE -> PS_BACKUP_COMPLETE

					BOOL bPreparingToCopy = pBackupWnd->IsBackupACDC_PreparingToCopy();
					BOOL bBurningToDisc = pBackupWnd->IsBackupACDC_BurningToDisc();
					BOOL bDeletingExternMedium = pBackupWnd->IsBackupACDC_EraseExternMedium();

					//Zwischenspeicherung läuft
					if(bPreparingToCopy)
					{
						WORD wBackupUserID = pBackupWnd->GetBackupUserID();
						WK_TRACE(_T("CPanel::OnStopActivityBackup(%i) Copy to temp\n"), wBackupUserID);
						CIPCDatabaseDVClient* pDatabase = pServer->GetDatabase();
						if(pDatabase)
						{
							pDatabase->DoRequestCancelBackup(wBackupUserID); 
						}
					}			

					//es wird gerade mit ACDC gebrannt
					else if(bBurningToDisc)
					{
						WORD wBackupUserID = pBackupWnd->GetBackupUserID();
						WK_TRACE(_T("CPanel::OnStopActivityBackup(%i) Burning\n"), wBackupUserID);

						CIPCDataCarrierClient* pDataCarrier = pServer->GetDataCarrier();
						if(pDataCarrier)
						{
							DWORD wBackupACDCSessioID = pBackupWnd->GetBackupACDCSessionID();
							pDataCarrier->DoRequestCancelSession(wBackupACDCSessioID); 
						}
					}

					//es wird gerade ein externe Medium (Memory Stick) gelöscht
					else if(bDeletingExternMedium)
					{

						// todo TKR das Löschen eines Memory Sticks ist nicht abbrechbar
	/*					CBackupEraseMedium* pBackupEraseMedium = pBackupWnd->GetBackupEraseMedium();
						if(pBackupEraseMedium)
						{
							pBackupEraseMedium->StopThread();
							
						}
	*/				}
				}
			}
		}	

	}


	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnMultyActivityBackup()
{
	BOOL bResult = FALSE;

	CMainFrame* pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		if (!pMainWnd->IsInBackup())
		{
			m_bMultiBackup = TRUE;
			m_bAlarmBackup = FALSE;
			bResult = OnBackup(m_bMultiBackup, m_bAlarmBackup);
		}
		else
		{
			WK_TRACE(_T("OnMultyActivityBackup() cannot backup PlayStatus not allowed\n"));
		}
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackup(BOOL bMultiBackup15 /* FALSE */, BOOL bAlarmBackup /* FALSE */)
{
	BOOL bRet = FALSE;

	if (theApp.GetServer() && !theApp.GetServer()->CanMultiCDExport())
	{
		bRet = OnBackup14();
	}
	else
	{
		// Backup auf removeable Drive hat immer Vorrang!
		if (m_bBackupToExternDrive)
		{
			if(bMultiBackup15)
			{
				if(m_bIsExternDriveEmpty)
				{
					bRet = OnBackup15Multi();
				}
				else
				{
//					bRet = OnBackupExternMediumMulti();
				}
			}
			else
			{
				m_bIsExternDriveEmpty = FALSE;
				CheckCapacityOfExternDrive(m_dwFreeMBExternalDrive, m_dwTotalMBExternalDrive);
				if(    m_dwFreeMBExternalDrive != 0 
					&& m_dwFreeMBExternalDrive == m_dwTotalMBExternalDrive)
				{
					m_bIsExternDriveEmpty = TRUE;
				}

				if(m_bIsExternDriveEmpty)
				{
					bRet = OnBackup15();
				}
				else
				{
					bRet = OnBackupNotEmptyExternMedium();
				}

			}

		}
		else if(theApp.CanBackupWithACDC())
		{
			if(bAlarmBackup)
			{
				bRet = OnBackupACDCAlarm();
			}
			else
			{
				if(bMultiBackup15)
				{
					bRet = OnBackupACDCMulti();
				}
				else
				{
					bRet = OnBackupACDC();
				}
			}


		}
		else
		{
			if(bAlarmBackup)
			{
				bRet = OnBackupAlarm();
			}
			else
			{
				if(bMultiBackup15)
				{
					bRet = OnBackup15Multi();
				}
				else
				{
					bRet = OnBackup15();
				}
			}

		}


	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackup14()
{
	CString sMsg,sFmt;
	EnableInputMode();
	if (m_bSetValue)
	{

		int iDay,iMonth,iYear,iHour,iMinute,iSeconds;
		CString sDate,sTime;

		if (m_eInputState == InputNo)
		{
			// start datum
			sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupStartDateTime;
			sFmt.LoadString(IDS_DATE_FORMAT);
			sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
			sFmt += sMsg;
			sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
			SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
			UpdateButtons();
		}
		else if (m_eInputState == InputBackupStartDateTime)
		{
			if (m_sInput.GetLength() == 12)
			{
				sDate = m_sInput.Left(6);
				sTime = m_sInput.Right(6);
			}
			else
			{
				sTime = _T("999999"); // Ungültige Zeit, damit ExtractAndValidateTime FALSE liefert!
				sDate = _T("999999");
			}
			
			iDay = iMonth = iYear = iHour = iMinute = iSeconds = 0;
			if (   ExtractAndValidateDate(sDate,iDay,iMonth,iYear)
				&& ExtractAndValidateTime(sTime,iHour,iMinute,iSeconds)
				)
			{
				m_stBackupStart.wDay   = (WORD)iDay;
				m_stBackupStart.wMonth = (WORD)iMonth;
				m_stBackupStart.wYear  = (WORD)(iYear);
				m_stBackupStart.wHour  = (WORD)iHour;
				m_stBackupStart.wMinute= (WORD)iMinute;
				m_stBackupStart.wSecond = (WORD)iSeconds;

				TRACE(_T("backup start time is %s\n"),m_stBackupStart.GetDateTime());

				// ende datum
				sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
				SetDisplayText(m_ctrlDisplayMenu1, sMsg);
				m_eInputState = InputBackupEndDateTime;
				sFmt.LoadString(IDS_DATE_FORMAT);
				sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
				sFmt += sMsg;
				sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				UpdateButtons();
			}
			else
			{
				// start time invalid
				sMsg.LoadString(IDS_EXPORT_TIME_BAD);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
				SetDisplayText(m_ctrlDisplayMenu1, sMsg);
				m_eInputState = InputBackupStartDateTime;
				sFmt.LoadString(IDS_DATE_FORMAT);
				sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
				sFmt += sMsg;
				sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				UpdateButtons();
			}
			m_sInput.Empty();
		}
		else if (m_eInputState == InputBackupEndDateTime)
		{
			if (m_sInput.GetLength() == 12)
			{
				sDate = m_sInput.Left(6);
				sTime = m_sInput.Right(6);
			}
			else
			{
				sTime = _T("999999"); // Ungültige Zeit, damit ExtractAndValidateTime FALSE liefert!
				sDate = _T("999999");
			}
	
			iDay = iMonth = iYear = iHour = iMinute = iSeconds = 0;
			if (   ExtractAndValidateDate(sDate,iDay,iMonth,iYear)
				&& ExtractAndValidateTime(sTime,iHour,iMinute,iSeconds)
				)
			{
				m_stBackupEnd.wDay   = (WORD)iDay;
				m_stBackupEnd.wMonth = (WORD)iMonth;
				m_stBackupEnd.wYear  = (WORD)(iYear);
				m_stBackupEnd.wHour  = (WORD)iHour;
				m_stBackupEnd.wMinute= (WORD)iMinute;
				m_stBackupEnd.wSecond = (WORD)iSeconds;
				TRACE(_T("backup end time is %s\n"),m_stBackupEnd.GetDateTime());

				if (m_stBackupStart < m_stBackupEnd)
				{
					// camera nr.
					m_sBackupCameras.Empty();
					sMsg.LoadString(IDS_EXPORT_CAMERAS);
					SetDisplayText(m_ctrlDisplayMenu1, sMsg);
					m_eInputState = InputBackupCameras;
					SetDisplayText(m_ctrlDisplayMenu2, _T("Nr.:_"));
					UpdateButtons();
				}
				else
				{
					// start time later than end time
					sMsg.LoadString(IDS_EXPORT_TIME_BAD);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadErrorMessage();
					sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
					SetDisplayText(m_ctrlDisplayMenu1, sMsg);
					m_eInputState = InputBackupEndDateTime;
					sFmt.LoadString(IDS_DATE_FORMAT);
					sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
					sFmt += sMsg;
					sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
					UpdateButtons();
				}
			}
			else
			{
				// end time invalid
				sMsg.LoadString(IDS_EXPORT_TIME_BAD);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
				SetDisplayText(m_ctrlDisplayMenu1, sMsg);
				m_eInputState = InputBackupEndDateTime;
				sFmt.LoadString(IDS_DATE_FORMAT);
				sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
				sFmt += sMsg;
				sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				UpdateButtons();
			}

			m_sInput.Empty();
		}
		else if (m_eInputState == InputBackupCameras)
		{
			// now do the backup
			m_sBackupCameras = m_sBackupCameras.Right(m_sBackupCameras.GetLength()-1);
			m_sInput.Empty();
			WK_TRACE(_T("backup cam nr. %s from %s - %s\n"),
				m_sBackupCameras,
				m_stBackupStart.GetDateTime(),
				m_stBackupEnd.GetDateTime());
			if (OnStartBackup())
			{
				// backup calc success
				sMsg.LoadString(IDS_EXPORT_RUNNING);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadInfoMessage();
				m_eInputState	= InputNo;
				DisableInputMode();
				OnUpdateMenu(0,0);
			}
			else
			{
				// backup calc failed
				sMsg.LoadString(IDS_EXPORT_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				m_eInputState	= InputNo;
				DisableInputMode();
				OnUpdateMenu(0,0);
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackup15()
{
	CString sMsg,sFmt;
	EnableInputMode();

	if (m_bSetValue)
	{
		CString sDate,sTime;

		//geänderte Eingabereihenfolge
		if (m_eInputState == InputNo)
		{
			// camera nr.
			m_sBackupCameras.Empty();
			sMsg.LoadString(IDS_EXPORT_CAMERAS);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupCameras;
			SetDisplayText(m_ctrlDisplayMenu2, _T("Nr.:_"));
			UpdateButtons();

			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackup: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);
			CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
			if (pBackupWnd)
			{
				pBackupWnd->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_CAMERAS);
				pBackupWnd->SetMultiBackup(FALSE);
			}

			//bei backup auf memorystick oder HDD hier die Kapazität setzen
			//das externe Medium wird nicht gelöscht, da es schon leer ist
			if(    m_bBackupToExternDrive
				&& pBackupWnd)
			{
				WK_TRACE(_T("OnBackup15(): extern medium: capacity = %d\n"), m_dwFreeMBExternalDrive);
				pBackupWnd->SetMediumCapacityInMB(m_dwFreeMBExternalDrive);
			}
		}

		else if (m_eInputState == InputBackupCameras)
		{
			OnBackup15_CheckBackupCameras();
		}
		else if (m_eInputState == InputBackupStartDateTime)
		{
			OnBackup15_CheckStartDateTime();
		}
		else if( m_eInputState == InputBackupWaitCalculatedEnd)
		{
			OnBackup15_WaitForCalculatedEnd();
		}
		else if (m_eInputState == InputBackupEndDateTime)
		{
			OnBackup15_CheckEndDateTime();
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackupNotEmptyExternMedium()
{
	CString sMsg,sFmt;
	EnableInputMode();

	if (m_bSetValue)
	{
		CString sDate,sTime;

		if (m_eInputState == InputNo)
		{
			//frage Benutzer, ob externes Medium gelöscht werden soll
			sMsg.LoadString(IDS_EXPORT_ERASE_MEDIUM); //IDS_EXPORT_ERASE_EXTERN_MEDIUM
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupEraseMediumConfirmingNo;
			UpdateMenu();
			
			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackupNotEmptyExternMedium: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);
			CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
			if (pBackupWnd)
			{
				pBackupWnd->SetPlayStatus(CBackupWindow::PS_BACKUP_ERASE_EXTERN_MEDIUM);
				pBackupWnd->SetMultiBackup(FALSE);
			}

			//bei backup auf memorystick oder HDD hier die Kapazität setzen
			if(pBackupWnd)
			{
				pBackupWnd->SetMediumCapacityInMB(m_dwTotalMBExternalDrive);
			}
		}
		else if(    m_eInputState == InputBackupEraseMediumConfirmingYes
		         || m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			OnBackup_EraseExternMedium();
		}

		else if (m_eInputState == InputBackupFoundMedium)
		{
			//externes Medium gefunden, beginne Backup
			OnBackup_FoundExternMedium();
		}
		else if (m_eInputState == InputBackupCameras)
		{
			OnBackup15_CheckBackupCameras();
		}
		else if (m_eInputState == InputBackupStartDateTime)
		{
			OnBackup15_CheckStartDateTime();
		}
		else if( m_eInputState == InputBackupWaitCalculatedEnd)
		{
			OnBackup15_WaitForCalculatedEnd();
		}
		else if (m_eInputState == InputBackupEndDateTime)
		{
			OnBackup15_CheckEndDateTime();
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::CheckCapacityOfExternDrive(DWORD &dwFreeMB, DWORD &dwTotalMB)
{
	CString sDrive, sRootExtended;
	int nDriveBit;

	if (CBackupWindow::GetRemovableDrive(sDrive, nDriveBit))
	{
		CIPCDrive removableDrive(sDrive, DVR_WRITE_BACKUP_DRIVE);
		
		removableDrive.CheckSpace();
		dwFreeMB = removableDrive.GetFreeMB();
		dwTotalMB = removableDrive.GetMB();
	}
	else
	{
		dwFreeMB = 0;
		dwTotalMB = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackupAlarm()
{

	TRACE(_T("OnBackupAlarm: Start Alarm Backup\n"));
	CString sMsg;
	EnableInputMode();

	if (m_bSetValue)
	{
		CString sDate,sTime;

		if (m_eInputState == InputNo)
		{
			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackupAlarm: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);
			
			if(OnStartBackup())
			{
				// alarm backup started
				sMsg.LoadString(IDS_EXPORT_RUNNING);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadInfoMessage();
				m_eInputState	= InputNo;
				DisableInputMode();
				OnUpdateMenu(0,0);
			}
			else
			{
				//cannot aktivate backup window
				sMsg.LoadString(IDS_EXPORT_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				m_eInputState	= InputNo;
				TRACE(_T("** disable input mode\n"));
				DisableInputMode();
				OnUpdateMenu(0,0);
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackup15_CheckBackupCameras()
{
	CString sMsg,sFmt;

	if(m_sBackupCameras.IsEmpty())
	{
		// backup calc failed
		sMsg.LoadString(IDS_EXPORT_NO_CAMERA);
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);
		WaitToReadErrorMessage();
		sMsg.LoadString(IDS_EXPORT_CAMERAS);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		m_eInputState = InputBackupCameras;
		SetDisplayText(m_ctrlDisplayMenu2, _T("Nr.:_"));
		UpdateButtons();
	}
	else
	{
		// start datum
		sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		m_eInputState = InputBackupStartDateTime;
		sFmt.LoadString(IDS_DATE_FORMAT);
		sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
		sFmt += sMsg;
		sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);

		m_sFirstDBImage.Empty();

		//Berechnung des ältesten Bildes der Datenbank
		//Ergebnis wird als StartDatum im Panel angezeigt
		CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
		if (pBackupWnd)
		{	
			pBackupWnd->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_START);

			DWORD dwTime = GetTickCount();
			CSystemTime st;
			st.GetLocalTime();
			m_stFirstDBImage = st;
			if(pBackupWnd->RequestFirstImageInDB(m_sBackupCameras, m_stFirstDBImage))
			{

				CString s, sYear;
				sYear.Format(_T("%d"), m_stFirstDBImage.GetYear());
				sYear = sYear.Right(sYear.GetLength()-2);
				s.Format(_T("%02d%02d%s%02d%02d%02d"),m_stFirstDBImage.GetDay(), 
												  m_stFirstDBImage.GetMonth(), 
												  sYear,
												  m_stFirstDBImage.GetHour(), 
												  m_stFirstDBImage.GetMinute(), 
												  m_stFirstDBImage.GetSecond(),
												  m_stFirstDBImage.GetMilliseconds());
				sFmt = m_sInput = m_sFirstDBImage = s;
				WK_TRACE(_T("TKR -- Zeit zum Bestimmen des ältesten Bildes: %d ms\n"), GetTickCount()-dwTime);
			}
			else
			{
				WK_TRACE(_T("TKR -- No pBackupWnd CPanel::OnBackup15() Could not detect oldest image in database!\n"));

			}
		}
		else
		{
			WK_TRACE(_T("TKR -- CPanel::OnBackup15() Could not detect oldest image in database!\n"));
		}

		SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
		UpdateButtons();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackup15_CheckStartDateTime()
{
	CString sDate,sTime,sMsg,sFmt;
	int iDay,iMonth,iYear,iHour,iMinute,iSeconds;

	if (m_sInput.GetLength() == 12)
	{
		sDate = m_sInput.Left(6);
		sTime = m_sInput.Right(6);
	}
	else
	{
		sTime = _T("999999"); // Ungültige Zeit, damit ExtractAndValidateTime FALSE liefert!
		sDate = _T("999999");
	}

	iDay = iMonth = iYear = iHour = iMinute = iSeconds = 0;
	
	//gibt an, ob Bilder zum backup gefunden wurden
	m_bImagesToBackup = TRUE;

	//gibt an, ob die Berechnung erfolgreich war (bei timeout = true)
	m_bCalculateError = FALSE;

	m_sLastCalcDBImage.Empty();
	
	if (   ExtractAndValidateDate(sDate,iDay,iMonth,iYear)
		&& ExtractAndValidateTime(sTime,iHour,iMinute,iSeconds)
		&& !IsDateTimeInFuture(iDay,iMonth,iYear,iHour,iMinute,iSeconds)
		&& (m_bMultiBackup || !IsDateTimeBeforeFirstImage(iDay,iMonth,iYear,iHour,iMinute,iSeconds))
		)
	{
		m_stBackupStart.wDay   = (WORD)iDay;
		m_stBackupStart.wMonth = (WORD)iMonth;
		m_stBackupStart.wYear  = (WORD)(iYear);
		m_stBackupStart.wHour  = (WORD)iHour;
		m_stBackupStart.wMinute= (WORD)iMinute;
		m_stBackupStart.wSecond = (WORD)iSeconds;

		WK_TRACE(_T("backup start time is %s\n"),m_stBackupStart.GetDateTime());

		// ende datum
		sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);

		m_eInputState = InputBackupWaitCalculatedEnd;

		sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
		//hole berechnetes Enddatum
		if (theApp.GetMainFrame())
		{
			CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
			if (pBackupWnd)
			{
				// convert camera nr.
				// from 1,3,6 to word array
				CStringArray saCameras;
				CWordArray waCameras;
				WORD wCameraNr;

				SplitString(m_sBackupCameras,saCameras,_T(','));

				for (int i=0;i<saCameras.GetSize();i++)
				{
					wCameraNr = (WORD)(_ttoi(saCameras[i])-1);
					if (ExistPlayCamera(wCameraNr))
					{
						waCameras.Add(wCameraNr);
					}
				}
				m_dwTimeoutCalculateEnd = GetTickCount();
				m_bCalculateEndComplete = FALSE;
				pBackupWnd->CalcBackupEnd(waCameras, m_stBackupStart);
				sMsg.LoadString(IDS_WAIT);
			}
		}
		SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
		UpdateButtons();

	}
	else
	{
		// start time invalid
		sMsg.LoadString(IDS_EXPORT_TIME_BAD);
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);
		WaitToReadErrorMessage();
		sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		m_eInputState = InputBackupStartDateTime;
		sFmt = m_sInput = m_sFirstDBImage;
		sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
		SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
		UpdateButtons();
	}
	WK_TRACE(_T("Startzeitpunkt: %s\n"), m_sInput);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackup15_WaitForCalculatedEnd()
{
	DisableInputMode();
	CString sMsg, sFmt;
	
	//hole berechnetes Enddatum
	if (theApp.GetMainFrame())
	{
		CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
		if (pBackupWnd)
		{
			if(m_bCalculateEndComplete)
			{
				m_eInputState = InputBackupEndDateTime;

				if(m_bImagesToBackup)
				{
					//hole berechneten Endzeitpunkt (MB`s passen auf eine CD-R)
					m_stBackupEnd = pBackupWnd->GetCalculatedEnd();
					m_sLastCalcDBImage = pBackupWnd->GetCalculatedEndFormatted();
					
					WK_TRACE(_T("TKR Zeit für Calculate in ms: %d\n"), GetTickCount()-m_dwTimeoutCalculateEnd);
					m_sInput = 	sFmt = m_sLastCalcDBImage;
				}
				else
				{
					WK_TRACE(_T("TKR ---- keine Bytes to Backup gefunden\n"));

					m_eInputState = InputBackupStartDateTime;
					sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
					SetDisplayText(m_ctrlDisplayMenu1, sMsg);
					WaitToReadErrorMessage();
					sFmt = m_sInput = m_sFirstDBImage;
					sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
					SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
					UpdateButtons();
					return;
				}
			}

			if(GetTimeSpan(m_dwTimeoutCalculateEnd) > 60*1000)
			{
				//timeout der Berechnung wurde erreicht
				m_bCalculateError = TRUE;  
				pBackupWnd->SetStopCalculate(TRUE);	

				sMsg.LoadString(IDS_BACKUP_CALCULATE_ERROR_PANEL);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				WK_TRACE_WARNING(_T("calculate backup end timeout reached after %d ms\n"),GetTickCount()-m_dwTimeoutCalculateEnd);
				m_sInput.Empty();
				sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
				SetDisplayText(m_ctrlDisplayMenu1, sMsg);
				sFmt = m_sInput;
				sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
				SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
				UpdateButtons();
			}
		}
	}

	sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
	SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
	UpdateButtons();
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackup15_CheckEndDateTime()
{
	CSystemTime stCalculatedEnd, stCurrentDateTime;
	CString sMsg,sFmt,sDate,sTime;
	int iDay,iMonth,iYear,iHour,iMinute,iSeconds;

	if(!m_bCalculateError)
	{
		stCalculatedEnd = theApp.GetMainFrame()->GetBackupWindow()->GetCalculatedEnd();
				TRACE(_T("TKR ###### CalcEnd: %s\n"), stCalculatedEnd.GetDateTime());
				TRACE(_T("TKR calculated backup end time is  : %s\n"),m_stBackupEnd.GetDateTime());
	}

	if (m_sInput.GetLength() == 12)
	{
		sDate = m_sInput.Left(6);
		sTime = m_sInput.Right(6);
	}
	else
	{
		sTime = _T("999999"); // Ungültige Zeit, damit ExtractAndValidateTime FALSE liefert!
		sDate = _T("999999");
	}

	iDay = iMonth = iYear = iHour = iMinute = iSeconds = 0;
	if (   ExtractAndValidateDate(sDate,iDay,iMonth,iYear)
		&& ExtractAndValidateTime(sTime,iHour,iMinute,iSeconds)
		&& !IsDateTimeInFuture(iDay,iMonth,iYear,iHour,iMinute,iSeconds)
		)
	{
		m_stBackupEnd.wDay   = (WORD)iDay;
		m_stBackupEnd.wMonth = (WORD)iMonth;
		m_stBackupEnd.wYear  = (WORD)(iYear);
		m_stBackupEnd.wHour  = (WORD)iHour;
		m_stBackupEnd.wMinute= (WORD)iMinute;
		m_stBackupEnd.wSecond = (WORD)iSeconds;
//				WK_TRACE(_T("TKR ### OnBackup15 backup end time is %s\n"),m_stBackupEnd.GetDateTime());

		if (m_stBackupStart < m_stBackupEnd)
		{
			//schlug Endzeitberechnung fehl, das Backup mit benutzerdefiniertem Endzeitpunkt
			//starten, sofern die `MB`s passen

			if(m_bCalculateError)  //no calculated end available
			{	
				CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
				if(pBW)
				{

					DWORD dwTick = GetTickCount();
					m_evCalculateEnd.ResetEvent();
					pBW->OnCalculateEnd_Failed(m_stBackupStart, m_stBackupEnd);	
					DWORD dwRet = WaitForSingleObject(m_evCalculateEnd, 5*1000);
					
					if(dwRet == WAIT_OBJECT_0)
					{
						if(m_bImagesToBackup)
						{
							if(m_bTooManyImages)
							{
								//es wurden beim benutzerspezifischen Endzeitpunkt
								//zu viele Bilder gefunden, erneute Eingabe erforderlich
								m_bTooManyImages = FALSE;
								pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_TOO_MANY_IMAGES);										

								//Endzeitpunkt zu groß
								sMsg.LoadString(IDS_EXPORT_TIME_BAD);
								SetDisplayText(m_ctrlDisplayMenu2, sMsg);
								WaitToReadErrorMessage();
								WK_TRACE_WARNING(_T("too many images for backup\n"));

								//Zurück zum Startzeitpunkt
								sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
								SetDisplayText(m_ctrlDisplayMenu1, sMsg);
								m_eInputState = InputBackupStartDateTime;
								sFmt.LoadString(IDS_DATE_FORMAT);
								sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
								sFmt += sMsg;
								sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
								sFmt = m_sInput = m_sFirstDBImage;
								SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
								UpdateButtons();

							}
							else
							{
								WK_TRACE(_T("TKR #### Bilder passen: Zeit in ms: %d\n"), GetTickCount()-dwTick);
							
								// now do the backup
								m_sBackupCameras = m_sBackupCameras.Right(m_sBackupCameras.GetLength()-1);
								m_sInput.Empty();
								WK_TRACE(_T("backup cam nr. %s from %s - %s\n"),
									m_sBackupCameras,
									m_stBackupStart.GetDateTime(),
									m_stBackupEnd.GetDateTime());
								if (OnStartBackup())
								{
									// backup window aktivated and backup started
									sMsg.LoadString(IDS_EXPORT_RUNNING);
									SetDisplayText(m_ctrlDisplayMenu2, sMsg);
									WaitToReadInfoMessage();
									m_eInputState	= InputNo;
									DisableInputMode();
									OnUpdateMenu(0,0);
								}
								else
								{
									//cannot aktivate backup window
									sMsg.LoadString(IDS_EXPORT_FAILED);
									SetDisplayText(m_ctrlDisplayMenu2, sMsg);
									WaitToReadErrorMessage();
									m_eInputState	= InputNo;
									TRACE(_T("** disable input mode\n"));
									DisableInputMode();
									OnUpdateMenu(0,0);
								}
							}
							
						}
						else
						{
							WK_TRACE_WARNING(_T("TKR ---- nach timeout: keine Bytes to Backup gefunden\n"));
							
							pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_CALCULATE_END__SUCCESS_MIN);
							
							sMsg.LoadString(IDS_EXPORT_TIME_BAD);
							SetDisplayText(m_ctrlDisplayMenu2, sMsg);
							WaitToReadErrorMessage();
							m_eInputState = InputBackupStartDateTime;
			
							sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
							SetDisplayText(m_ctrlDisplayMenu1, sMsg);

							sFmt.LoadString(IDS_DATE_FORMAT);
							sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
							sFmt += sMsg;
							sFmt = m_sInput = m_sFirstDBImage;
							sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
							SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
							UpdateButtons();
							return;
						}
					}
					else if(dwRet == WAIT_TIMEOUT)
					{
						WK_TRACE_WARNING(_T("TKR ---- zweites timeout: keine Bytes to Backup gefunden\n"));
						
						pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_START);
						
						sMsg.LoadString(IDS_EXPORT_TIME_BAD);
						SetDisplayText(m_ctrlDisplayMenu2, sMsg);
						WaitToReadErrorMessage();
						m_eInputState = InputBackupStartDateTime;
		
						sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
						SetDisplayText(m_ctrlDisplayMenu1, sMsg);

						sFmt.LoadString(IDS_DATE_FORMAT);
						sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
						sFmt += sMsg;
						sFmt = m_sInput = m_sFirstDBImage;
						sMsg.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
						SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
						UpdateButtons();
						return;
					}

				}
			}
			else  //check calculated end
			{
				CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
				if (pBW)
				{
					// convert camera nr.
					// from 1,3,6 to word array
					CStringArray saCameras;
					SplitString(m_sBackupCameras,saCameras,_T(','));				

					//Benutzer Eingabe größer als berechneter Endzeitpunkt
					if(m_stBackupEnd > stCalculatedEnd)
					{
						//Backup Zeitraum zu groß
						//Hinweis im BackupWindow, dass zuerst bis zum berechneten Endzeitpunkt 
						//gebrannt wird und anschließend mit "Auslagerung fortsetzen" 
						//weitergemacht werden kann

						m_sInput = sFmt = m_sMultiBackupStart = m_sLastCalcDBImage;

						if(m_bMultiBackup)
						{
							pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_CALCULATE_END__SUCCESS);
						}
						else
						{
							pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_MULTI);
						}

						sMsg.LoadString(IDS_EXPORT_TIME_BAD);
						SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
						sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
						SetDisplayText(m_ctrlDisplayMenu1, sMsg);
						WaitToReadErrorMessage();
						sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
						SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
						UpdateButtons();
					}
					else 
					{
						BOOL bRunBackup = TRUE;
						if(m_stBackupEnd < stCalculatedEnd)
						{
							//Benutzer Eingabe kleiner als berechneter Endzeitpunkt
							//berechne MB`s für das backup neu
							CWordArray waCameras;
							WORD wCameraNr;

							for (int i=0;i<saCameras.GetSize();i++)
							{
								wCameraNr = (WORD)(_ttoi(saCameras[i])-1);
								if (ExistPlayCamera(wCameraNr))
								{
									waCameras.Add(wCameraNr);
								}
							}
							
							DWORD dwTick = GetTickCount();
							HANDLE theEvents[2];

						
							theEvents[0] = m_evCalculateMBUserBackupEndOK;
							theEvents[1] = m_evCalculateMBUserBackupEndFALSE;
						
							m_evCalculateMBUserBackupEndOK.ResetEvent();
							m_evCalculateMBUserBackupEndFALSE.ResetEvent();

							pBW->CalcMBUserBackupEnd(waCameras, m_stBackupStart, m_stBackupEnd);
					
							DWORD dwRet = WaitForMultipleObjects(2, theEvents, FALSE, 5000);
							DWORD ix = dwRet - WAIT_OBJECT_0;

							if (dwRet == WAIT_TIMEOUT)
							{
								bRunBackup = FALSE;
								WK_TRACE_WARNING(_T("CalcMBUserBackupEnd() timeout. No confirm\n"));
							}
							else if(theEvents[ix] == m_evCalculateMBUserBackupEndOK)
							{
								WK_TRACE(_T("tkr ###### MB`s berechnet, alles OK, Zeit: %d\n"), GetTimeSpan(dwTick, GetTickCount()));
							}
							else if(theEvents[ix] == m_evCalculateMBUserBackupEndFALSE)
							{
								bRunBackup = FALSE;
								WK_TRACE(_T("tkr ###### keine MB`s gefunden, Zeit: %d\n"), GetTimeSpan(dwTick, GetTickCount()));
							}
						}

						if(bRunBackup)
						{
							// now do the backup
							m_sInput.Empty();
							WK_TRACE(_T("backup cam nr. %s from %s - %s\n"),
								m_sBackupCameras,
								m_stBackupStart.GetDateTime(),
								m_stBackupEnd.GetDateTime());
							if (OnStartBackup())
							{
								// backup window aktivated and backup started
								sMsg.LoadString(IDS_EXPORT_RUNNING);
								SetDisplayText(m_ctrlDisplayMenu2, sMsg);
								WaitToReadInfoMessage();
								m_eInputState	= InputNo;
								DisableInputMode();
								OnUpdateMenu(0,0);
							}
							else
							{
								//cannot aktivate backup window
								sMsg.LoadString(IDS_EXPORT_FAILED);
								SetDisplayText(m_ctrlDisplayMenu2, sMsg);
								WaitToReadErrorMessage();
								m_eInputState	= InputNo;
								TRACE(_T("** disable input mode\n"));
								DisableInputMode();
								OnUpdateMenu(0,0);
							}
						}
						else
						{
							WK_TRACE_WARNING(_T("TKR ---- User Endzeitpunkt: keine Bytes to Backup gefunden\n"));
							
							pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_CALCULATE_END__SUCCESS_MIN);
							
							sMsg.LoadString(IDS_EXPORT_NO_IMAGES);
							SetDisplayText(m_ctrlDisplayMenu2, sMsg);
							WaitToReadErrorMessage();
							m_eInputState = InputBackupEndDateTime;
			
							sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
							SetDisplayText(m_ctrlDisplayMenu1, sMsg);

							sFmt.LoadString(IDS_DATE_FORMAT);
							sMsg.LoadString(IDS_TIME_FORMAT_HHMMSS);
							sFmt += sMsg;

							CString s, sYear;
							sYear.Format(_T("%d"), stCalculatedEnd.GetYear());
							sYear = sYear.Right(sYear.GetLength()-2);
							s.Format(_T("%02d%02d%s%02d%02d%02d"),stCalculatedEnd.GetDay(), 
															  stCalculatedEnd.GetMonth(), 
															  sYear,
															  stCalculatedEnd.GetHour(), 
															  stCalculatedEnd.GetMinute(), 
															  stCalculatedEnd.GetSecond(),
															  stCalculatedEnd.GetMilliseconds());
							sFmt = m_sInput = s;
							sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
							SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
							UpdateButtons();
						}
					}
				}
				else
				{
					//no MainWnd available or backup window not activated
					sMsg.LoadString(IDS_EXPORT_FAILED);
					SetDisplayText(m_ctrlDisplayMenu2, sMsg);
					WaitToReadErrorMessage();
					m_eInputState	= InputNo;
					DisableInputMode();
					OnUpdateMenu(0,0);
				}
			}
		}
		else
		{
			// start time later or equal than end time
			sMsg.LoadString(IDS_EXPORT_TIME_BAD);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			WaitToReadErrorMessage();
			sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupEndDateTime;
			sFmt = m_sInput = m_sLastCalcDBImage;
			sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
			SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
			UpdateButtons();
		}
	}
	else
	{
		// end time invalid
		sMsg.LoadString(IDS_EXPORT_TIME_BAD);
		SetDisplayText(m_ctrlDisplayMenu2, sMsg);
		WaitToReadErrorMessage();
		sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		m_eInputState = InputBackupEndDateTime;
		sFmt = m_sInput = m_sLastCalcDBImage;
		sMsg.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
		SetFormatDisplayText(m_ctrlDisplayMenu2, sFmt, sMsg);
		UpdateButtons();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackup15Multi()
{
	CString sMsg,sFmt;
	EnableInputMode();
	if (m_bSetValue)
	{
		//Eingabe nur Endzeitpunkt (incl. vorherige Berechnung), 
		//da weiterführendes Backup (Auslagerung fortsetzen)
		if (m_eInputState == InputNo)
		{

			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackup: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);
			CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
			if (pBackupWnd)
			{
				//Buttons aktualisieren, da Suchbutton (beim Öffnen des Backup Windows)
				//sofort disabled werden soll
				OnUpdateButtons(TRUE, 0);
				pBackupWnd->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_MULTI_IS_RUNNING);
				pBackupWnd->SetMultiBackup(TRUE);
				pBackupWnd->InitMultiBackupMembers(m_stMultiBackupStartLast,
												   m_stMultiBackupEndLast,
												   m_stMultiBackupStartNew);
			}

			//initialisiere die members des "normalen" backup mit werten des multi backups
			m_eInputState = InputBackupStartDateTime;

			CString s, sYear;
			sYear.Format(_T("%d"), m_stMultiBackupStartNew.GetYear());
			sYear = sYear.Right(sYear.GetLength()-2);
			s.Format(_T("%02d%02d%s%02d%02d%02d"),m_stMultiBackupStartNew.GetDay(), 
										  	  m_stMultiBackupStartNew.GetMonth(), 
											  sYear,
											  m_stMultiBackupStartNew.GetHour(), 
											  m_stMultiBackupStartNew.GetMinute(), 
											  m_stMultiBackupStartNew.GetSecond());


			m_sInput = s;
			m_stBackupStart  = m_stMultiBackupStartNew;
			m_sBackupCameras = m_sMultiBackupCamsSeperated;
			
			OnBackup15_CheckStartDateTime();
		}
		else if( m_eInputState == InputBackupWaitCalculatedEnd)
		{
			OnBackup15_WaitForCalculatedEnd();
		}
		else if(m_eInputState == InputBackupEndDateTime)
		{
			OnBackup15_CheckEndDateTime();
		}


	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackupACDC()
{
	CString sMsg,sFmt;
	EnableInputMode();

	if (m_bSetValue)
	{
		CString sDate,sTime;

		if (m_eInputState == InputNo)
		{
			// wait for medium
			//reset MediaType
			m_MedienType = MT_NOMEDIUM;
			
			sMsg.LoadString(IDS_EXPORT_INSERT_MEDIUM);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupAskForMedium;
			sMsg.LoadString(IDS_EXPORT_INPUT_WAIT_FOR_MEDIUM);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			UpdateButtons();

			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackup: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);
			CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
			if (pBackupWnd)
			{
				pBackupWnd->SetMultiBackup(FALSE);
			}
			OnCrossSet();
		}

		else if (m_eInputState == InputBackupAskForMedium)
		{
			//frage ACDC nach Medium
			OnBackupACDC_AskForMedium();
		}
		else if (m_eInputState == InputBackupWaitForMedium)
		{
			//warte auf Medium, noch keines im Laufwerk gefunden
			OnBackupACDC_WaitForMedium();
		}
		else if (m_eInputState == InputBackupFoundMedium)
		{
			//Medium in CD-Laufwerk gefunden, beginne Backup
			OnBackupACDC_FoundMedium();
		}
		else if (m_eInputState == InputBackupFoundNoEmptyCD)
		{
			//nicht beschreibbares Medium (CD-R) gefunden, beende backup
			OnBackupACDC_FoundNoEmptyCD();
		}
		else if (m_eInputState == InputBackupDVDNotAllowed)
		{
			//DVD gefunden, DVD Backup ist aber nicht freigeschaltet durch dongle
			OnBackupACDC_DVDNotAllowed();
		}
		else if (    m_eInputState == InputBackupEraseMedium
				  || m_eInputState == InputBackupEraseMediumConfirmingYes
				  || m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			//nicht leere CD-RW gefunden, frage nach, ob sie gelöscht werden soll
			OnBackupACDC_EraseCDRW();
		}
		else if (m_eInputState == InputBackupCameras)
		{
			OnBackupACDC_CheckBackupCameras();
		}
		else if (m_eInputState == InputBackupStartDateTime)
		{
			OnBackupACDC_CheckStartDateTime();
		}
		else if( m_eInputState == InputBackupWaitCalculatedEnd)
		{
			OnBackupACDC_WaitForCalculatedEnd();
		}
		else if (m_eInputState == InputBackupEndDateTime)
		{
			OnBackupACDC_CheckEndDateTime();
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackupACDCAlarm()
{
	CString sMsg,sFmt;
	EnableInputMode();

	if (m_bSetValue)
	{
		CString sDate,sTime;

		if (m_eInputState == InputNo)
		{
			// wait for medium
			//reset MediaType
			m_MedienType = MT_NOMEDIUM;
			
			sMsg.LoadString(IDS_EXPORT_INSERT_MEDIUM);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupAskForMedium;
			sMsg.LoadString(IDS_EXPORT_INPUT_WAIT_FOR_MEDIUM);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			UpdateButtons();

			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackupACDCAlarm: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);

			OnCrossSet();
		}

		else if (m_eInputState == InputBackupAskForMedium)
		{
			//frage ACDC nach Medium
			OnBackupACDC_AskForMedium();
		}
		else if (m_eInputState == InputBackupWaitForMedium)
		{
			//warte auf Medium, noch keines im Laufwerk gefunden
			OnBackupACDC_WaitForMedium();
		}
		else if (m_eInputState == InputBackupFoundMedium)
		{
			//Medium in CD-Laufwerk gefunden, beginne Backup
			OnBackupACDC_FoundMedium();
		}
		else if (m_eInputState == InputBackupFoundNoEmptyCD)
		{
			//nicht beschreibbares Medium (CD-R) gefunden, beende backup
			OnBackupACDC_FoundNoEmptyCD();
		}
		else if (m_eInputState == InputBackupDVDNotAllowed)
		{
			//DVD gefunden, DVD Backup ist aber nicht freigeschaltet durch dongle
			OnBackupACDC_DVDNotAllowed();
		}
		else if (    m_eInputState == InputBackupEraseMedium
				  || m_eInputState == InputBackupEraseMediumConfirmingYes
				  || m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			//nicht leere CD-RW gefunden, frage nach, ob sie gelöscht werden soll
			OnBackupACDC_EraseCDRW();
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_AskForMedium()
{
	CIPCDataCarrierClient* pDataCarrier = theApp.GetServer()->GetDataCarrier();
	if(pDataCarrier)
	{
		pDataCarrier->DoRequestVolumeInfos();
		m_eInputState = InputBackupWaitForMedium;

		//zur Auswertung des neuen Zustandes, OnCrossSet() aufrufen
		OnCrossSet();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_WaitForMedium()
{
	WK_TRACE(_T("wait for medium\n"));
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_FoundMedium()
{
	WK_TRACE(_T("found medium in drive\n"));
	CString sMsg;

	//setze die wirkliche Kapazität des Mediums
	CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
	CIPCDataCarrierClient* pDataCarrier = theApp.GetServer()->GetDataCarrier();
	
	if(pBW && pDataCarrier)
	{
		CIPCDrive* pBackupDrive = pDataCarrier->GetBackupDrive();
		if(pBackupDrive)
		{
			DWORD dwTotalFreeMB = pBackupDrive->GetMB();
			pBW->SetMediumCapacityInMB(dwTotalFreeMB);
		}
	}

	//prüfen, was für ein Medium im laufwerk gefunden wurde
	switch(m_MedienType)
	{
	case MT_CD_WRITABLE:

		if(m_bAlarmBackup)
		{
			TRACE(_T("OnBackupACDC_FoundMedium: Start Alarm Backup\n"));
					CString sMsg;
					
			if(OnStartBackup())
			{
				// alarm backup started
				sMsg.LoadString(IDS_EXPORT_RUNNING);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadInfoMessage();
				m_eInputState	= InputNo;
				DisableInputMode();
				OnUpdateMenu(0,0);
			}
			else
			{
				//cannot aktivate backup window
				sMsg.LoadString(IDS_EXPORT_FAILED);
				SetDisplayText(m_ctrlDisplayMenu2, sMsg);
				WaitToReadErrorMessage();
				m_eInputState	= InputNo;
				TRACE(_T("** disable input mode\n"));
				DisableInputMode();
				OnUpdateMenu(0,0);
			}
		}
		//soll ein multi backup durchgeführt werden (Auslagerung fortsetzen)
		else if(m_bMultiBackup)
		{
			m_eInputState = InputBackupStartDateTime;
			OnCrossSet();
		}
		else
		{
			// input camera nr.
			m_sBackupCameras.Empty();
			sMsg.LoadString(IDS_EXPORT_CAMERAS);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupCameras;
			SetDisplayText(m_ctrlDisplayMenu2, _T("Nr.:_"));
			UpdateButtons();
			if(pBW)
			{
				//setze Mode für die Eingabe der kameras
				pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_CAMERAS);
			}
		}

		break;

	case MT_CDRW_ERASE:
		//frage Benutzer, ob CD/RW gelöscht werden soll
		sMsg.LoadString(IDS_EXPORT_ERASE_MEDIUM);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		m_eInputState = InputBackupEraseMediumConfirmingNo;
		UpdateMenu();
		if(pBW)
		{
			pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_ACDC_ERASE_MEDIUM);
		}
		break;

	case MT_MEDIUM_ARRIVED:
		//prüfe das ins Laufwerk eingelegte Medium
		m_eInputState = InputBackupAskForMedium;

		//zur Auswertung des neuen Zustandes, OnCrossSet() aufrufen
		OnCrossSet();		
		break;

	case MT_DVD_NOT_ALLOWED:
		//DVD gefunden, DVD Backup ist aber nicht freigeschaltet durch dongle
		m_eInputState = InputBackupDVDNotAllowed;
		
		//zur Auswertung des neuen Zustandes, OnCrossSet() aufrufen
		OnCrossSet();	
		break;
	case MT_NOMEDIUM:
		//nicht beschreibbares Medium (CD-R) gefunden
		m_eInputState = InputBackupFoundNoEmptyCD;
		
		//zur Auswertung des neuen Zustandes, OnCrossSet() aufrufen
		OnCrossSet();
		break;
	default:
		break;
	}



}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackup_FoundExternMedium()
{
	CString sMsg;

	if(m_bAlarmBackup)
	{
		TRACE(_T("OnBackup_FoundExternMedium: Start Alarm Backup\n"));
				
		if(OnStartBackup())
		{
			// alarm backup started
			sMsg.LoadString(IDS_EXPORT_RUNNING);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			WaitToReadInfoMessage();
			m_eInputState	= InputNo;
			DisableInputMode();
			OnUpdateMenu(0,0);
		}
		else
		{
			//cannot aktivate backup window
			sMsg.LoadString(IDS_EXPORT_FAILED);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			WaitToReadErrorMessage();
			m_eInputState	= InputNo;
			TRACE(_T("** disable input mode\n"));
			DisableInputMode();
			OnUpdateMenu(0,0);
		}
	}
	//soll ein multi backup durchgeführt werden (Auslagerung fortsetzen)
	else if(m_bMultiBackup)
	{
		m_eInputState = InputBackupStartDateTime;
		OnCrossSet();
	}
	else
	{
		CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();

		// input camera nr.
		m_sBackupCameras.Empty();
		sMsg.LoadString(IDS_EXPORT_CAMERAS);
		SetDisplayText(m_ctrlDisplayMenu1, sMsg);
		m_eInputState = InputBackupCameras;
		SetDisplayText(m_ctrlDisplayMenu2, _T("Nr.:_"));
		UpdateButtons();
		if(pBW)
		{
			//setze Mode für die Eingabe der kameras
			pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_CAMERAS);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_FoundNoEmptyCD()
{
	ToggleInputMode();
	if (!m_bSetValue)
	{
		m_eInputState = InputNo;
		CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
		CIPCDataCarrierClient* pDataCarrier = theApp.GetServer()->GetDataCarrier();
		if(pBW)
		{
			pBW->SetTimeACDCNoEmptyCD();  //time to show the text in backup window

			//setze Mode für nicht beschreibbares Medium
			pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_ACDC_NO_EMPTY_CD);

			if(pDataCarrier)
			{
				//eject medium
				pDataCarrier->DoRequestGetValue(SECID_NO_ID,_T("Eject"),0);
			}
		}

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_DVDNotAllowed()
{
	ToggleInputMode();
	if (!m_bSetValue)
	{
		m_eInputState = InputNo;
		CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
		CIPCDataCarrierClient* pDataCarrier = theApp.GetServer()->GetDataCarrier();
		if(pBW)
		{
			pBW->SetTimeACDCNoEmptyCD();  //time to show the text in backup window

			//setze Mode für nicht beschreibbares Medium
			pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_ACDC_DVD_NOT_ALLOWED);

			if(pDataCarrier)
			{
				//eject medium
				pDataCarrier->DoRequestGetValue(SECID_NO_ID,_T("Eject DVD"),0);
			}
		}

	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_EraseCDRW()
{
	//frage nach, ob CD-RW gelöscht werden soll oder nicht
	ToggleInputMode();
	if (!m_bSetValue)
	{
		if(m_eInputState == InputBackupEraseMedium)
		{
			// Eingabe ist noch nicht beendet. Eingabe muß erst noch bestätigt werden.
			// daher zurück in den Eingabemodus.
			m_eInputState = InputBackupEraseMediumConfirmingNo;
			EnableInputMode();
		}

		if(m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			m_eInputState = InputNo;
	
			CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
			CIPCDataCarrierClient* pDataCarrier = theApp.GetServer()->GetDataCarrier();
			if (pBW)
			{
				pBW->SetTimeNotErasedMedium();  //time to show the text in backup window
				pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_ACDC_NOT_ERASED);

				if(pDataCarrier)
				{
					//eject medium
					pDataCarrier->DoRequestGetValue(SECID_NO_ID,_T("Eject"),0);
				}
			}
		}

		if(m_eInputState == InputBackupEraseMediumConfirmingYes)
		{
			CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();

			m_eInputState = InputBackupFoundMedium;

			//gefundene CD-RW als beschreibbar kennzeichnen, da sie vor dem Backup
			//automatisch gelöscht werden soll
			m_MedienType = MT_CD_WRITABLE;

			if(pBW)
			{
				//setze BOOL für späteres Anzeigen des Löschvorgangs
				pBW->SetACDCEraseProgress(TRUE);
				
				if(m_bAlarmBackup)
				{
					m_eInputState = InputBackupFoundMedium;
					
				}
				//soll gerade ein multi backup durchgeführt werden (Auslagerung fortsetzen)
				else if(m_bMultiBackup)
				{
					m_eInputState = InputBackupStartDateTime;

					//setze Mode für das multi backup
					pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_MULTI_IS_RUNNING);

				}
				else
				{
					//setze Mode für die Eingabe der kameras
					pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_CAMERAS);
				}
			}
			
			//Panel aktualisieren
			OnCrossSet();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackup_EraseExternMedium()
{
	CString sMsg;
	//frage nach, ob externes medium (memory stick, hdd..)  gelöscht werden soll oder nicht
	ToggleInputMode();
	if (!m_bSetValue)
	{
		CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
		
		if(m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			//Medium soll nicht gelöscht werden (Benutzerwahl = NO)
			//benutze noch freien Speicherplatz darauf
			DWORD dwROSoftwareInMB = 10;
			DWORD dwOneMB = 1024 * 1024;

			if(pBW)
			{
				CheckCapacityOfExternDrive(m_dwFreeMBExternalDrive, m_dwTotalMBExternalDrive);
	
				//hole Gesamtgröße (aufgerundet in MB) der zu kopierenden Readersoftware 
				dwROSoftwareInMB = (pBW->GetBytesReadOnlySoftware()+dwOneMB)/dwOneMB;

				//prüfe, ob Dateien der RO Software (aufgerundet in MB) schon drauf sind
				DWORD dwExternROInMB = 0;
				if(pBW->GetBytesReadOnlySoftwareFromExternMedium() > 0)
				{
					dwExternROInMB = (pBW->GetBytesReadOnlySoftwareFromExternMedium()+dwOneMB)/dwOneMB;
				}

				//Größe in MB die auf externem Medium mindesten frei sein muss
				DWORD dwMinFreeMediumCapacity = dwROSoftwareInMB - dwExternROInMB + 1;
				if(dwMinFreeMediumCapacity == 0)
				{
					dwMinFreeMediumCapacity = 1;
				}
				
				//prüfe, ob noch genug freier Speicherplatz vorhanden ist
				if(m_dwFreeMBExternalDrive > dwMinFreeMediumCapacity)
				{
					//setze zu Verfügung stehenden Speicherplatz
					//vorhandener freier Platz + Platz der Dateien, die sowieso überschrieben werden
					//Größe der ReaderSoftware und 1 MB Reserve werden
					//beim Setzen automatisch wieder mit abgezogen
					BOOL bMediumCapacityOK = pBW->SetMediumCapacityInMB(m_dwFreeMBExternalDrive 
																		- dwMinFreeMediumCapacity 
																		+ dwExternROInMB
																		+ 1);
						
					if(!bMediumCapacityOK)
					{
						//Speicherplatz auf Medium == 0 MB
						m_eInputState = InputNo;
						//time to show the text in backup window
						pBW->SetTimeNotErasedMedium();  
						pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM);						
					}
					else
					{
						//genug Speicherplatz vorhanden, starte Backup
						m_eInputState = InputBackupFoundMedium;

						pBW->SetDeleteExternBackupDrive(FALSE);

						//setze Mode für die Eingabe der kameras
						pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_CAMERAS);
					}
				}
				else
				{
					//nicht genug Speicherplatz vorhanden, beende Backup
					m_eInputState = InputNo;

					//time to show the text in backup window
					pBW->SetTimeNotErasedMedium();  
					pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM);
				}
			}
		}

		if(m_eInputState == InputBackupEraseMediumConfirmingYes)
		{
			if(pBW)
			{
				if(    m_dwFreeMBExternalDrive == 0
					&& m_dwTotalMBExternalDrive == 0)
				{
					//Fehler, externes Medium wurde nicht korrekt erkannt
					//evtl. ist Memory Stick defekt

					//Speicherplatz auf Medium == 0 MB
					m_eInputState = InputNo;
					//time to show the text in backup window
					pBW->SetTimeNotErasedMedium();  
					pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM);					
				}
				else
				{
	//TODO TKR multibackup und alarmbackup auf Memorystick soll vorerst nicht realisiert werden, 
	//da bei Alarmbackup Speicherplatz zu klein sein kann
	//bei späterer Unterstützung von Backups auf externe HDD soll dieses aber dann möglich sein (Alarmbackup)
					if(m_bAlarmBackup)
					{
	//					m_eInputState = InputBackupFoundMedium;
						
					}
					//soll gerade ein multi backup durchgeführt werden (Auslagerung fortsetzen)
					else if(m_bMultiBackup)
					{
	//					m_eInputState = InputBackupStartDateTime;

						//setze Mode für das multi backup
	//					pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_MULTI_IS_RUNNING);

					}
					else
					{
						m_eInputState = InputBackupFoundMedium;

						pBW->SetDeleteExternBackupDrive(TRUE);

						//setze Mode für die Eingabe der kameras
						pBW->SetPlayStatus(CBackupWindow::PS_BACKUP_INPUT_CAMERAS);
					}
				}


			}
		}
		
		//Panel aktualisieren
		OnCrossSet();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_CheckBackupCameras()
{
	OnBackup15_CheckBackupCameras();
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_CheckStartDateTime()
{
	//setze vor dem Begin der Endzeitberechnung die wirkliche, 
	//auf dem Medium erkannte Kapazität
	//ansonsten ist sie standardmäßig auf 600 MB 

	CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
	CIPCDrive* pBackupDrive = theApp.GetServer()->GetDataCarrier()->GetBackupDrive();

	if (   pBackupWnd
		&& pBackupDrive)
	{
		DWORD dwTotalFreeMB = pBackupDrive->GetMB();
		WK_TRACE(_T("#### CD Capacity in MB: %d\n"), dwTotalFreeMB);
		pBackupWnd->SetMediumCapacityInMB(dwTotalFreeMB);
	}

	OnBackup15_CheckStartDateTime();
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_WaitForCalculatedEnd()
{
	OnBackup15_WaitForCalculatedEnd();
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnBackupACDC_CheckEndDateTime()
{
	OnBackup15_CheckEndDateTime();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnBackupACDCMulti()
{
	CString sMsg,sFmt;
	EnableInputMode();

	if (m_bSetValue)
	{
		CString sDate,sTime;

		if (m_eInputState == InputNo)
		{
			// wait for medium
			//reset MediaType
			m_MedienType = MT_NOMEDIUM;
			
			sMsg.LoadString(IDS_EXPORT_INSERT_MEDIUM);
			SetDisplayText(m_ctrlDisplayMenu1, sMsg);
			m_eInputState = InputBackupAskForMedium;
			sMsg.LoadString(IDS_EXPORT_INPUT_WAIT_FOR_MEDIUM);
			SetDisplayText(m_ctrlDisplayMenu2, sMsg);
			UpdateButtons();

			//creates BackupWindow in CMainFrame
			DWORD dwTick = GetTickCount();
			CreateBackupWindow();
			WK_TRACE(_T("CPanel::OnBackup: BackupWindow created after %d ms\n"), GetTickCount()-dwTick);
			CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
			if (pBackupWnd)
			{
				pBackupWnd->SetMultiBackup(TRUE);
				pBackupWnd->InitMultiBackupMembers(m_stMultiBackupStartLast,
												   m_stMultiBackupEndLast,
												   m_stMultiBackupStartNew);

			}
			OnCrossSet();
			//initialisiere die members des "normalen" backup mit werten des multi backups
			CString s, sYear;
			sYear.Format(_T("%d"), m_stMultiBackupStartNew.GetYear());
			sYear = sYear.Right(sYear.GetLength()-2);
			s.Format(_T("%02d%02d%s%02d%02d%02d"),m_stMultiBackupStartNew.GetDay(), 
										  	  m_stMultiBackupStartNew.GetMonth(), 
											  sYear,
											  m_stMultiBackupStartNew.GetHour(), 
											  m_stMultiBackupStartNew.GetMinute(), 
											  m_stMultiBackupStartNew.GetSecond());


			m_sInput = s;
			m_stBackupStart  = m_stMultiBackupStartNew;
			m_sBackupCameras = m_sMultiBackupCamsSeperated;
		}

		else if (m_eInputState == InputBackupAskForMedium)
		{
			//frage ACDC nach Medium
			OnBackupACDC_AskForMedium();
		}
		else if (m_eInputState == InputBackupWaitForMedium)
		{
			//warte auf Medium, noch keines im Laufwerk gefunden
			OnBackupACDC_WaitForMedium();
		}
		else if (m_eInputState == InputBackupFoundMedium)
		{
			//Medium in CD-Laufwerk gefunden, beginne Backup
			OnBackupACDC_FoundMedium();
		}
		else if (m_eInputState == InputBackupFoundNoEmptyCD)
		{
			//nicht beschreibbares Medium (CD-R) gefunden, beende backup
			OnBackupACDC_FoundNoEmptyCD();
		}
		else if (m_eInputState == InputBackupDVDNotAllowed)
		{
			//DVD gefunden, DVD Backup ist aber nicht freigeschaltet durch dongle
			OnBackupACDC_DVDNotAllowed();
		}
		else if (    m_eInputState == InputBackupEraseMedium
				  || m_eInputState == InputBackupEraseMediumConfirmingYes
				  || m_eInputState == InputBackupEraseMediumConfirmingNo)
		{
			//nicht leere CD-RW gefunden, frage nach, ob sie gelöscht werden soll
			OnBackupACDC_EraseCDRW();
		}
		else if (m_eInputState == InputBackupCameras)
		{
			OnBackupACDC_CheckBackupCameras();
		}
		else if (m_eInputState == InputBackupStartDateTime)
		{
			OnBackupACDC_CheckStartDateTime();
		}
		else if( m_eInputState == InputBackupWaitCalculatedEnd)
		{
			OnBackupACDC_WaitForCalculatedEnd();
		}
		else if (m_eInputState == InputBackupEndDateTime)
		{
			OnBackupACDC_CheckEndDateTime();
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnStartBackup()
{
	BOOL bRet = FALSE;
	m_bCalculateError = FALSE; //wieder zurücksetzen, da nun das Backup vollzogen wird und
							   //erst beim nächsten backup wieder ein timeout error im Calculate
							   //auftreten kann
	if (theApp.GetMainFrame())
	{
		CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
		if (pBW)
		{
			if(m_bAlarmBackup)
			{
				CPlayWindow* pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)m_nCamNr);
				if (   pPlayWnd
					&& !pPlayWnd->IsMD())	
				{
					// Auslagerung eines UVV Alarmes
					// Start und Endzeit des Alarmes bestimmen
					// dann mit einem Backup Fenster fortfahren.
					if (pPlayWnd->GetAlarmStartandEndTime(m_stBackupStart,m_stBackupEnd))
					{
						CreateBackupWindow();
						CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
						if (pBW)
						{
							CWordArray waCameras;
							waCameras.Add(pPlayWnd->GetID().GetSubID());
							bRet = pBW->StartAlarmBackup(m_stBackupStart,m_stBackupEnd, waCameras);
						}
					}
					else
					{
					}
				}

			}
			else
			{
				TRACE(_T("TKR ### OnStartBackup: backup end time is %s\n"),m_stBackupEnd.GetDateTime());
				CStringArray saCameras;
				CWordArray waCameras;
				WORD wCameraNr;
				CPlayWindow* pPW;

				SplitString(m_sBackupCameras,saCameras,_T(','));
				
				for (int i=0;i<saCameras.GetSize();i++)
				{
					wCameraNr = (WORD)(_ttoi(saCameras[i])-1);
					pPW = ExistPlayCamera(wCameraNr);

					if (pPW)
					{
						waCameras.Add(pPW->GetID().GetSubID());
					}
				}

				bRet = pBW->StartBackup(m_stBackupStart,m_stBackupEnd, waCameras, m_sBackupCameras);
			}


		}
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::ActivatePanel()
{
// Da die durch die Message aufgerufene Funktion nur in Debug etwas tut,
// kann die Message in Release eingespart werden
// Ggf ändern, wenn OnActivatePanel() geändert wurde
#ifdef _DEBUG	
 	PostMessage(WM_ACTIVATE_PANEL, 0, 0);
#endif
}
 
/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnActivatePanel(WPARAM wParam, LPARAM lParam)
{
// Da diese durch Message aufgerufene Funktion nur in Debug etwas tut,
// wurde auch die Message in ActivatePanel() gekapselt (=eingespart!)
// Ggf in ActivatePanel() ändern, wenn hier geändert wurde
	// Im Shellmodus und als Receiver soll der Focus NICHT ständig
	// auf das Panel gelegt werden!
//	CWK_Profile prof;
//	if ((prof.GetInt(REGKEY_DVSTARTER, _T("AsShell"), 0) == 0) && theApp.IsTransmitter())
#ifdef _DEBUG	
//	SetFocus();
#endif

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetPanelPosition(BOOL bCenter)
{
	int nHRes = 0;
	int nVRes = 0;
	int nXPos = 0;
	int nYPos = 0;
	CRect rcMainWnd(0,0,0,0);
	CRect rectPanel(0,0,0,0);
	CRect rcLogo(0,0,0,0);

	CMainFrame* pWnd = (CMainFrame*)theApp.GetMainFrame();
	if (pWnd)
	{
		// Bildschirmauflösung ermitteln
		HDC hDC		= CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
		nHRes		= GetDeviceCaps(hDC, HORZRES);				
		nVRes		= GetDeviceCaps(hDC, VERTRES);	
		DeleteDC(hDC);
	
		// Größe des Panels
		GetWindowRect(rectPanel);
		
		// Position und Größe des Mainframes
		pWnd->GetWindowRect(rcMainWnd);

		if (bCenter)
		{
			if (pWnd->GetLogoPosition(rcLogo))
			{
				// zentriere unter dem Logo, aber Panel muss sichtbar sein
				rcLogo.OffsetRect(CPoint(rcMainWnd.left, rcMainWnd.top+28));
				nXPos = max(min(rcLogo.left  , nHRes - rectPanel.Width()), 0);
				nYPos = max(min(rcLogo.bottom, nVRes - rectPanel.Height()), 0);
			}
			else
			{
				// zentriere im Fenster
				nXPos = max(min(rcMainWnd.left + (rcMainWnd.Width()-rectPanel.Width()  )/2, nHRes-rectPanel.Width()), 0);
				nYPos = max(min(rcMainWnd.top  + (rcMainWnd.Height()-rectPanel.Height())/2, nVRes-rectPanel.Height()), 0);
			}
		}
		else
		{
			// untere rechte Ecke
			nXPos = max(min(rcMainWnd.left + (rcMainWnd.Width() - rectPanel.Width()), nHRes-rectPanel.Width()), 0);
			nYPos = max(min(rcMainWnd.top  + (rcMainWnd.Height()-rectPanel.Height()), nVRes-rectPanel.Height()) , 0);
		}
		SetWindowPos(&CWnd::wndTopMost, nXPos, nYPos, 0, 0, SWP_NOSIZE);
//		SetCursorPos(nXPos+rectPanel.Width()/2,nYPos+rectPanel.Height()/2);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeTargetDisplay()
{
 	m_bShowTargetCross = !m_bShowTargetCross;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetTargetDisplayState()
{
	// Den TargetDisplayState Status setzen
	CDisplayWindow::SetShowTargetCross(m_bShowTargetCross);
	if (theApp.GetMainFrame())
	{
		// Alle Playwindows neu anfordern
		CPlayWindow* pPlayWndFirst = theApp.GetMainFrame()->GetNextPlayWindow(NULL);
		if (pPlayWndFirst)
		{
			pPlayWndFirst->Invalidate();
			CPlayWindow* pPlayWnd = theApp.GetMainFrame()->GetNextPlayWindow(pPlayWndFirst);
			while (pPlayWnd && pPlayWnd != pPlayWndFirst)
			{
				pPlayWnd->Invalidate();
				pPlayWnd = theApp.GetMainFrame()->GetNextPlayWindow(pPlayWnd);
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnToggleTimerOnOffState()
{
	m_bTimerOnOffState = !m_bTimerOnOffState;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetTimerOnOffState()
{
	BOOL bResult = FALSE;

	// Den TimerOnOff Status setzen
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->SetTimerOnOffState(m_bTimerOnOffState))
			bResult = TRUE;
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetTimerOnOffState(BOOL &bTimerOnOffState)
{
	BOOL bResult = FALSE;

	// Den TimerOnOff Status holen
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->GetTimerOnOffState(bTimerOnOffState))
		{
// Wenn schon Funktion, dann doch wohl nicht gleich den Member setzen!
// Dies muss in OnUpdateMenu gehandelt werden
//			m_bTimerOnOffState = bTimerOnOffState;
			bResult = TRUE;
		}	
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnToggleAlarmOutputOnOffState()
{
	m_bAlarmOutputOnOffState = !m_bAlarmOutputOnOffState;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetAlarmOutputOnOffState()
{
	BOOL bResult = FALSE;

	// Den TimerOnOff Status setzen
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->SetAlarmOutputOnOffState(m_bAlarmOutputOnOffState))
			bResult = TRUE;
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetAlarmOutputOnOffState(BOOL &bAlarmOutputOnOffState)
{
	BOOL bResult = FALSE;

	// Den TimerOnOff Status holen
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->GetAlarmOutputOnOffState(bAlarmOutputOnOffState))
		{
// Wenn schon Funktion, dann doch wohl nicht gleich den Member setzen!
// Dies muss in OnUpdateMenu gehandelt werden
//			m_bAlarmOutputOnOffState = bAlarmOutputOnOffState;
			bResult = TRUE;
		}	
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::GetAlarmDialingNumber(int nCallingNumber, CString& sCalingNumber)
{
	BOOL bResult = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->GetAlarmDialingNumber(nCallingNumber, sCalingNumber))
			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetAlarmDialingNumber(int nCallingNumber, const CString& sNumber)
{
	BOOL bResult	= FALSE;
	CString sCall	= sNumber;

	// Definition: Ein Punkt in der Rufnummer besagt, daß es sich um eine IP
	//			   Adresse handelt.
	if (sCall.Find(_T(".")) != -1)
	{
		CString sTemp = sCall;
		sTemp.Remove(_T('.'));
		// IP-Adresse überprüfen. (Format: www.xxx.yyy.zzz)
		if (!ValidateIP(sTemp))
			return FALSE;
	   
		// Die 4 Punkte wieder reinfriemeln (Braucht der Server so)
		sCall.Format(_T("%s.%s.%s.%s"), sTemp.Mid(0,3), sTemp.Mid(3,3), sTemp.Mid(6,3), sTemp.Mid(9,3));
	}

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->SetAlarmDialingNumber(nCallingNumber, sCall))
			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CPanel::ChangeDisplayColor()
{
	COLORREF col;
	switch (m_pOSD->GetMenuID())
	{
		// Camera specific items color *********************************
		case IDOSD_COLOR_BRIGHTNESS:
		case IDOSD_COLOR_CONTRAST:
		case IDOSD_COLOR_SATURATION:
		// _FallThrough_ -------------------------
		case IDOSD_CHANGE_CAMNAME:	
		case IDOSD_CAMERA_COMPRESSION:
		case IDOSD_CAMERA_FPS:
		case IDOSD_CAMERA_PTZ_TYPE:
		case IDOSD_CAMERA_PTZ_ID:
		case IDOSD_CAMERA_PTZ_COMPORT:
		// _FallThrough_ -------------------------
		case IDOSD_CAMERA_RESOLUTION:
		case IDOSD_ACOUSTIC_ALARM:
		case IDOSD_EXPORT_SETTINGS:
		case IDOSD_IMPORT_SETTINGS:
		case IDOSD_MAINTENANCE:
		case IDOSD_SYSTEM_VIDEOFORMAT:
		case IDOSD_SYSTEM_VIDEONORM:
		case IDOSD_SYSTEM_AGC:
		// _FallThrough_ -------------------------
		case IDOSD_TIMER_MO:
		case IDOSD_TIMER_TU:
		case IDOSD_TIMER_WE:
		case IDOSD_TIMER_TH:
		case IDOSD_TIMER_FR:
		case IDOSD_TIMER_SA:
		case IDOSD_TIMER_SU:
		case IDOSD_TIMER_ONOFF:
		// _FallThrough_ -------------------------
		case IDOSD_ALARM_MODE:
		// _FallThrough_ -------------------------
		case IDOSD_MD_ALARM_SENSITIVITY:
		case IDOSD_MD_MASK_SENSITIVITY:
		case IDOSD_MD_DRAW_MASK:
		// _FallThrough_ -------------------------
		case IDOSD_ALARM_OUTPUT_ON_OFF:
		case IDOSD_ALARM_CALLNUMBER1:
		case IDOSD_ALARM_CALLNUMBER2:
		case IDOSD_ALARM_CALLNUMBER3:
		case IDOSD_ALARM_CALL_EVENT:
		// _FallThrough_ -------------------------
		case IDOSD_ALARM_DELETE:
		case IDOSD_ALARM_BACKUP:
		case IDOSD_SUSPECT_SEARCH:
		case IDOSD_SUSPECT_DELETE:
		case IDOSD_ACTIVITY_BACKUP:
		case IDOSD_MULTY_ACTIVITY_BACKUP:
		case IDOSD_STOP_ACTIVITY_BACKUP:
		case IDOSD_CAMERA_TERMINATION:
			col = RGB(200, 200, 200); // Background Grey Color
			break;

		// System specific items color *********************************

		case IDOSD_TCPIP:
		case IDOSD_SUBNETMASK:
		case IDOSD_GATEWAY:
		case IDOSD_ISDN_MSN:
		case IDOSD_NET_NAME:
		case IDOSD_DNS:
		// _FallThrough_ -------------------------
		case IDOSD_AUDIO_CHANGE_CAMNR:
		case IDOSD_AUDIO_THRESHOLD:
		case IDOSD_AUDIO_SENSITIVITY:
		case IDOSD_AUDIO_VOLUME:
		case IDOSD_AUDIO_RECORD_MODE:
		case IDOSD_AUDIO_LISTEN_IN:
		// _FallThrough_ -------------------------
		case IDOSD_VIDEO_OUT_MODE_PORT_1:
		case IDOSD_VIDEO_OUT_MODE_PORT_2:
		case IDOSD_VIDEO_OUT_MODE_PORT_3:
		case IDOSD_VIDEO_OUT_MODE_PORT_4:
		case IDOSD_SEQUENCER_DWELLTIME:
		// _FallThrough_ -------------------------
		case IDOSD_CLOCK_DATE:
		case IDOSD_CLOCK_TIME:
		case IDOSD_TIME_ZONE:
		// _FallThrough_ -------------------------
		case IDOSD_NO_PRINTER:
		case IDOSD_PRINTER1:
		case IDOSD_PRINTER2:
		case IDOSD_PRINTER3:
		case IDOSD_PRINTER4:
		case IDOSD_PRINTER5:
		case IDOSD_PRINTER6:
		case IDOSD_PRINTER7:
		case IDOSD_PRINTER8:
		case IDOSD_PRINTER9:
		case IDOSD_PRINTER10:
		case IDOSD_PRINTER11:
		case IDOSD_PRINTER12:
		case IDOSD_PRINTER13:
		case IDOSD_PRINTER14:
		case IDOSD_PRINTER15:
		case IDOSD_PRINTER16:
		// _FallThrough_ -------------------------
		case IDOSD_LANGUAGE1:
		case IDOSD_LANGUAGE2:
		case IDOSD_LANGUAGE3:
		case IDOSD_LANGUAGE4:
		case IDOSD_LANGUAGE5:
		case IDOSD_LANGUAGE6:
		case IDOSD_LANGUAGE7:
		case IDOSD_LANGUAGE8:
		case IDOSD_LANGUAGE9:
		case IDOSD_LANGUAGE10:
		case IDOSD_LANGUAGE11:
		case IDOSD_LANGUAGE12:
		case IDOSD_LANGUAGE13:
		case IDOSD_LANGUAGE14:
		case IDOSD_LANGUAGE15:
		case IDOSD_LANGUAGE16:
		// _FallThrough_ -------------------------
		case IDOSD_CHANGE_PIN_OPERATOR:
		case IDOSD_CHANGE_PIN_SUPERVISOR:
		// _FallThrough_ -------------------------
		case IDOSD_OPTION_RTE:
		case IDOSD_OPTION_EXIT:
		case IDOSD_OPTION_UPDATE:
		case IDOSD_EXPANSION_CODE:
		// _FallThrough_ -------------------------
		case IDOSD_OPTION_END:
			col = SKIN_COLOR_BLUE; // Background Blue Color ***********
			break;

		// Standard color **********************************************
		default:
			col = CPanel::m_BaseColorDisplays; // Background Green Color
			break;
	}

	// OEM Spezifische Einstellungen
	// Nur Alarmcom und ProtectionOne besitzt unterschiedliche Displayhintergrundfarben
	if ((m_eOem == OemAlarmCom) || (m_eOem == OemPOne))
		SetDisplayColor(col);
	else
		SetDisplayColor(CPanel::m_BaseColorDisplays); // Background Green Color

	return col;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::StartUpdateThread()
{
	if (m_pUpdateThread == NULL)
	{
		m_pUpdateThread = AfxBeginThread(DoUpdateThread, this);
		if (m_pUpdateThread)
			m_pUpdateThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	}
}

/////////////////////////////////////////////////////////////////////////////
UINT CPanel::DoUpdateThread(LPVOID pData)
{
	BOOL bError	 = FALSE;
	CString sTmp, sMsg;
	CPanel* pThis =(CPanel*)pData;
	CWK_Profile wkp;

	XTIB::SetThreadName(_T("DoUpdateThread"));

	// Update frisch gestartet?
	if (pThis->m_eInputState == InputNo)
	{
		if (pThis->m_Update.StartUpdate(pThis->m_eOem))
		{
			pThis->m_eInputState = InputUpdateSystem;
			sTmp.LoadString(IDS_INPUT_DISK);
			sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
			pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
			pThis->EnableInputMode();
			pThis->m_eInputState = InputUpdateSystem;
		}
	}
	else if (pThis->m_eInputState == InputUpdateSystem)
	{
		pThis->m_ctrlDisplayMenu2.DisableBlinking();	
		sMsg.LoadString(IDS_UPDATE_RUNNING);
		pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);

		BOOL bRunning = TRUE;

		while (bRunning)
		{
			switch (pThis->m_Update.ContinueUpdate())
			{
			case UPDATE_WRONG_OEM:
				sMsg.LoadString(IDS_UPDATE_WRONG_OEM);
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				sTmp.LoadString(IDS_INPUT_DISK);
				sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->EnableInputMode();
				bRunning = FALSE;
				break;	
			case UPDATE_NO_UPDATE_DISK:
				sMsg.LoadString(IDS_UPDATE_NO_UPDATE_DISK);
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				sTmp.LoadString(IDS_INPUT_DISK);
				sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->EnableInputMode();
				bRunning = FALSE;
				break;
			case UPDATE_WRONG_DISK:
				sMsg.LoadString(IDS_UPDATE_WRONG_DISK);
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				sTmp.LoadString(IDS_INPUT_DISK);
				sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->EnableInputMode();
				bRunning = FALSE;
				break;
			case UPDATE_WRONG_BUILD:
				sMsg.LoadString(IDS_UPDATE_WRONG_BUILD);
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				sTmp.LoadString(IDS_INPUT_DISK);
				sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->EnableInputMode();
				bRunning = FALSE;
				break;
			case UPDATE_DISK_READ_FAILED:
				sMsg.LoadString(IDS_UPDATE_FAILED);	
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				sTmp.LoadString(IDS_INPUT_DISK);
				sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->EnableInputMode();
				bRunning = FALSE;
				break;
			case UPDATE_WRONG_CRC:			
				sMsg.LoadString(IDS_UPDATE_WRONG_CRC);
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				sTmp.LoadString(IDS_INPUT_DISK);
				sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->EnableInputMode();
				bRunning = FALSE;
				break;
			case UPDATE_COPY_FAILED:
				sMsg.LoadString(IDS_UPDATE_FAILED);	
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadErrorMessage();
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, pThis->m_pOSD->GetSubString());
				pThis->DisableInputMode();
				bRunning = FALSE;
				break;
			case UPDATE_NEXT_DISK:
				if (pThis->m_Update.IsFDDUpdate())
				{
					sTmp.LoadString(IDS_INPUT_DISK);
					sMsg.Format(sTmp, pThis->m_Update.GetDiskNumber()); 
					pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
					pThis->EnableInputMode();
					bRunning = FALSE;
				}
				else if (pThis->m_Update.IsCDUpdate())
				{
					bRunning = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("unsupported update type\n"));
					bRunning = FALSE;
				}
				break;
			case UPDATE_LAST_DISK:
				sMsg.LoadString(IDS_UPDATE_SUCCESS);
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, sMsg);
				pThis->WaitToReadInfoMessage();
				pThis->SetDisplayText(pThis->m_ctrlDisplayMenu2, pThis->m_pOSD->GetSubString());
				
				// Wurde Update abgebrochen?
				if (pThis->m_eInputState == InputUpdateSystem)	
				{
					pThis->DisableInputMode();
					// Das Kopieren war erfolgreich. UpdateAufforderung in Registry notieren
					// und Software runterfahren.
					wkp.WriteInt(REGKEY_DVSTARTER, _T("DoUpdate"), 1);
					pThis->DoReboot();
				}
				bRunning = FALSE;
				break;
			default:
				bRunning = FALSE;
				WK_TRACE_WARNING(_T("Unknown update state\n"));
				break;
			}
		}
	}

	pThis->m_pUpdateThread = NULL;
	return !bError;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::WaitToReadErrorMessage()
{
	Beep(250, 250);
	Beep(440, 250);

	Sleep(1500);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::WaitToReadInfoMessage()
{
	Sleep(1000);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (zDelta > 0)
	{
		if (nFlags & MK_MBUTTON)
			OnButtonSkipForward();
		else
			OnButtonSingleForward();
	}
	else if (zDelta < 0)
	{
		if (nFlags & MK_MBUTTON)
			OnButtonSkipBack();
		else
			OnButtonSingleBack();
	}
	return CTransparentDialog::OnMouseWheel(nFlags, zDelta, pt);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CTransparentDialog::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::LoadHostList()
{
	// Save actual Host name
	CString sHostName;
	CHost* pHost = NULL;
	m_nHostIndex = min(m_nHostIndex, m_HostsAll.GetSize());
	if (m_HostsAll.GetSize())
	{
		pHost = (CHost*)m_HostsAll.GetAtFast(m_nHostIndex);
		if (pHost)
		{
			sHostName		= pHost->GetName();
		}
	}

	// IP-Book Host-Liste
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	// IP-Book Host-Liste sichern, um sie am Ende löschen zu können
//	m_HostsIPBook.Load(prof);
	// IP-Book Host-Liste ist aber auch Grundstock für die allgemeine Host-Liste
	m_HostsAll.Load(prof);

	// Die zusätzliche Host-Liste laden und die Objekte in die allgemeine Host-Liste KOPIEREN!
	CHostArray Hosts;
	CWK_Profile prof2(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV\\AdditionalHosts"),_T(""));
	Hosts.Load(prof2);
	for (int i=0 ; i<Hosts.GetSize(); i++)
	{
		pHost = m_HostsAll.AddHost();
		pHost->SetName(Hosts.GetAtFast(i)->GetName());
		pHost->SetNumber(Hosts.GetAtFast(i)->GetNumber());
	}
	m_HostsAll.Sort();

	// Der letzte Eintrag besitzt eine Sonderstellung. Wird dieser ausgewählt,
	// so soll sich ein Dialog öffnen, in dem manuell Gegenstellen eingetragen
	// werden.
	CString sExtend;
	sExtend.LoadString(IDS_EXTEND);
	m_HostsAll.Add(new CHost(sExtend, SPECIAL_HOSTIP));

	// actualize m_nHostIndex
	// start at zero
	m_nHostIndex = 0;
	if (sHostName.IsEmpty() == FALSE)
	{
		for (int i=0 ; i<m_HostsAll.GetSize() ; i++)
		{
			pHost = m_HostsAll.GetAtFast(i);
			if (   pHost
				&& (pHost->GetName() == sHostName)
				)
			{
				m_nHostIndex = i;
				break;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnSelectHost()
{
	if (m_HostsAll.GetSize() > 0)
	{
		m_nHostIndex = min(m_nHostIndex, m_HostsAll.GetSize());

		CHost* pHost = (CHost*)m_HostsAll.GetAtFast(m_nHostIndex);
		if (pHost)
		{
			// gf todo change for DUN
			m_sHostAddress	= pHost->GetNumber();
			m_sHostName		= pHost->GetName();
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnIncrementHost()
{
	LoadHostList();
	m_nHostIndex++;
	if (m_nHostIndex >= m_HostsAll.GetSize())
		m_nHostIndex = 0;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnDecrementHost()
{
	LoadHostList();
	m_nHostIndex--;
	if (m_nHostIndex < 0)
		m_nHostIndex = (max(m_HostsAll.GetSize()-1, 0));
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetDisplayText(CDisplay& display, CString sText, int iType/*=0*/, int iTime/*=0*/)
{
	AdjustCodePageDifferences(sText);
	display.SetDisplayText(sText, iType, iTime);
}
/////////////////////////////////////////////////////////////////////////////
int CPanel::SetFormatDisplayText(CDisplay& display, CString sFormat, CString sText)
{
	AdjustCodePageDifferences(sFormat);
	AdjustCodePageDifferences(sText);
	return display.SetFormatDisplayText(sFormat, sText);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::AdjustCodePageDifferences(CString& sText)
{
	if (m_sSystemCodePage != m_pDefaultLanguage->GetCodePage())
	{
		if (m_sSystemCodePage == _T("1252"))
		{
			if (m_pDefaultLanguage->GetCodePage() == _T("1250"))
			{
				TBYTE byte;
				for (int i=0 ; i<sText.GetLength() ; i++)
				{
					byte = sText.GetAt(i);
					if (byte != m_byCP1250To1252[byte])
					{
						sText.SetAt(i, m_byCP1250To1252[byte]);
					}
				}
			}
			else
			{	// gf todo, not yet implemented
			}	// other DLL codepages
		}
		else
		{	// gf todo, not yet implemented
		}	// other system codepages
	}
	else
	{}	// ok, same codepage
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::CreateBackupWindow()
{
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		CMainFrame* pMF = theApp.GetMainFrame();
		if (pMF)
		{
			pMF->AddBackupWindow(pServer);
		}
		else
		{
			WK_TRACE_ERROR(_T("CPanel::CreateBackupWindow(): Couldn`t get MainFrame\n"));
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("CPanel::CreateBackupWindow(): Couldn`t get Server\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetCalculateEndComplete()
{
	//Berechnung für die MB`s für die erste Backup CD-R ist beendet
	WK_TRACE(_T("TKR ----- CPanel::SetCalculateEndComplete()\n"));
	if (m_eInputState == InputBackupWaitCalculatedEnd)
	{	
		m_bCalculateEndComplete = TRUE;
		PostMessage(WM_NOTIFY_CALCULATE_END, 0, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetEventCalculateMBUserBackupEnd(BOOL bOK)
{
	//Berechnung für die MB`s für benutzerspezifischen Endzeitpunkt beendet
	if(bOK)
	{
		m_evCalculateMBUserBackupEndOK.SetEvent();
	}
	else
	{
		m_evCalculateMBUserBackupEndFALSE.SetEvent();
	}

}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetImagesToBackup(BOOL bImages)
{
	m_bImagesToBackup = bImages;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::SetTooManyImages(BOOL bToManyImages)
{
	m_bTooManyImages = bToManyImages;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDeleteBackup()
{
	CBackupWindow* pBackupWnd = theApp.GetMainFrame()->GetBackupWindow();
	if(pBackupWnd)
	{
		if(!pBackupWnd->IsBackupRunning())
		{
			if(pBackupWnd->IsInCalculate())
			{
				//Falls gerade die Endzeitberechnung läuft, kein weiteres
				//DoRequestBackupByTime() aufrufen, sondern Backupfenster 
				//(von sich selbst aus) schließen
				pBackupWnd->SetCloseBackupWindow(TRUE);
			}
			else
			{
				theApp.GetMainFrame()->DeleteBackupWindow();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnShowHostListDlg()
{
	BOOL bReturn = FALSE;
	m_pHostsDlg	= new CHostsDlg(this);
	if (m_pHostsDlg)
	{
		m_eInputState = InputEditHostList;
		EnablePanel(FALSE);
		m_pHostsDlg->DoModal();
		EnablePanel(TRUE);
		WK_DELETE(m_pHostsDlg);
		LoadHostList();
		bReturn = TRUE;
		m_eInputState = InputSelectHost;
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::SetCanRectSearch(BOOL bRectSearch)
{
	m_bCanRectSearch = bRectSearch;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::SetMultiBackupMembers( CSystemTime stBackupStartLast,
								    CSystemTime stBackupEndLast,
									WORD wBackupCamLast,
									CString sBackupCamsSeperated,
									WORD wLastSequence,
									DWORD dwLastRecord)
{
	m_stMultiBackupStartLast	= stBackupStartLast;
	m_stMultiBackupEndLast		= stBackupEndLast;
	m_stMultiBackupStartNew		= stBackupEndLast;
	m_wMultiBackupCamLast		= wBackupCamLast;
	m_wMultiBackupLastSequence	= wLastSequence;
	m_dwMultiBackupLastRecord	= dwLastRecord;
	m_sMultiBackupCamsSeperated	= sBackupCamsSeperated;
}
/////////////////////////////////////////////////////////////////////////////
//Sieht in PlayWindow nach (Kamera dessen Endzeitpunkt beim letzten Backup benutzt wurde)
//ob die Datenbank zum letzen Endzeitpunkt noch das gewünschte Image enthält
BOOL CPanel::CanMultiBackup()
{
	BOOL bFirstImageOK	= FALSE;
	BOOL bLastImageOK	= FALSE;

	//nur, wenn das erste Bild des letzten backups noch in der datenbank vorhanden ist
	//und gleichzeitig mindestens ein Bild hinter dem letzten Bild des letzten Backups
	//vorhanden ist, ´kann die Auslagerung fortgesetzt werden

	//nur, wenn ein vorheriges Backup durchgeführt wurde, kann überhaupt ein MultiBackup
	//erlaubt werden

	if(m_wMultiBackupCamLast != 0)
	{
		CPlayWindow* pPW = theApp.GetMainFrame()->GetPlayWindow(m_wMultiBackupCamLast);

		if (pPW)
		{
			CIPCSequenceRecord* pFirstSeq = pPW->GetFirstNotBackupSequence();
			
			if(pFirstSeq)
			{
				if(m_stMultiBackupEndLast >= pFirstSeq->GetTime())
				{
					//Bild in Datenbank vorhanden
					bFirstImageOK = TRUE;
				}
				else
				{
					WK_TRACE(_T("CPanel::CanMultiBackup() image not longer in database\n"));
				}
			}
			else
			{
				WK_TRACE(_T("CPanel::CanMultiBackup() cannot get first sequence\n"));
			}

			if(bFirstImageOK)
			{
				TRACE(_T("TKR ######### CanMultiBackup()\n"));
				CServer* pServer = theApp.GetServer();
				if(pServer)
				{
					CIPCFields fields;
					CSystemTime stNextImage = m_stMultiBackupStartNew;
					stNextImage.IncrementTime(0, 0, 0, 0, 1);

					CString sDate = stNextImage.GetDBDate();
					CString sTime = stNextImage.GetDBTime();
					char op = _T('}'); //vorwärts suchen

					fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,op));
					fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,op));
								
					//Hole letztes Image der letzen Sequence der letzten Backup Kamera
					DWORD dwUserID = PANEL_LAST_RECORD;
					int iNumArchives = 1;
					WORD w[1];
					w[0] = m_wMultiBackupCamLast;
					
					DWORD dwTick = GetTickCount();
					HANDLE theEvents[2];

					
					theEvents[0] = m_evLastBackupRecordOK;
					theEvents[1] = m_evLastBackupRecordFALSE;
					
					m_evLastBackupRecordOK.ResetEvent();
					m_evLastBackupRecordFALSE.ResetEvent();

					pServer->GetDatabase()->DoRequestNewQuery(dwUserID,
																iNumArchives,
																w,
																fields,
																1);
				
					DWORD dwRet = WaitForMultipleObjects(2, theEvents, FALSE, 2000);
					DWORD ix = dwRet - WAIT_OBJECT_0;


					if (dwRet == WAIT_TIMEOUT)
					{
						WK_TRACE_WARNING(_T("GetLastBackupImage timeout. No confirm %d\n"),GetTimeSpan(dwTick, GetTickCount()));
					}
					else if(theEvents[ix] == m_evLastBackupRecordOK)
					{
						TRACE(_T("tkr ###### Bild in DB vorhanden. Zeit: %d\n"), GetTimeSpan(dwTick, GetTickCount()));
						bLastImageOK = TRUE;
					}
					else if(theEvents[ix] == m_evLastBackupRecordFALSE)
					{
						TRACE(_T("tkr ###### Bild nicht in DB vorhanden. Zeit: %d\n"), GetTimeSpan(dwTick, GetTickCount()));
					}
				}
			}
		}
		else
		{
			WK_TRACE(_T("CPanel::CanMultiBackup() cannot access PlayWindows\n"));
		}
	}

	return bFirstImageOK && bLastImageOK;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::SetEventLastBackupRecord(BOOL bOK)
{
	if(bOK)
	{
		m_evLastBackupRecordOK.SetEvent();
	}
	else
	{
		m_evLastBackupRecordFALSE.SetEvent();
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnNotifyCalculateEnd(WPARAM wParam, LPARAM lParam)
{
	//sobald die Endzeitberechnung erfolgt ist, wird dem Panel eine Nachricht geschickt.
	//erst dann hier OnCrossSet() aufrufen um aus dem Panelthread heraus den Paneltext
	//ändern
	OnCrossSet();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPanel::OnNotifyFoundMedium(WPARAM wParam, LPARAM lParam)
{
	//sobald mit Hilfe von ACDC ein eingelegtes Medium erkannt wurde,
	//wird dem Panel eine Nachricht geschickt.
	//wurde ein neues Medium mittels OnDeviceChange() erkannt, wird das ebenfalls hierhin
	//gemeldet
	//Erst dann hier OnCrossSet() aufrufen um aus dem Panelthread heraus den Paneltext
	//ändern. 
	//VORSICHT: Das Obige nur ausführen, wenn auf das Einlegen eines Mediums gewartet wird

	if(m_eInputState == InputBackupWaitForMedium)
	{
		//Eingabestatus setzen, Medium gefunden, bzw kein Medium im Laufwerk	
		m_eInputState = InputBackupFoundMedium;

		//   lParam  |  wParam
		//----------------------
		//		0	 |	   0		-> OK,    found writable medium
		//      1    |     0		-> Error, found not empty CD-RW/DVD+RW
		//		0	 |	   1		-> OK,    new medium arrived, check medium with ACDC
		//		1	 |	   1		-> Error, no writable medium in drive
		//		1	 |	   2		-> Error, found DVD but DVD is not allowed
		//		2	 |	   2		-> Error, cd drive failed to get mediums capacity

		if (wParam == 0 && lParam == 0)
		{
			m_MedienType = MT_CD_WRITABLE;
		}
		else if (wParam == 1 && lParam == 0)
		{
			m_MedienType = MT_CDRW_ERASE;
		}
		else if(wParam == 0 && lParam == 1)
		{
			m_MedienType = MT_MEDIUM_ARRIVED;
		}
		else if(wParam == 1 && lParam == 2)
		{
			m_MedienType = MT_DVD_NOT_ALLOWED;
		}
		else if(wParam == 2 && lParam == 2)
		{
			m_MedienType = MT_NOMEDIUM;
		}
		else
		{
			m_MedienType = MT_NOMEDIUM;
		}

		OnCrossSet();
	}

	return 0;
}

/*********************************************************************************************
 Class      : CPanel
 Function   : OnNotifyDrive
 Description: Notification for arrival or removal of hotplug drives.

 Parameters:   
  wParam   : (E): LOWORD: Drive type, HIWORD: Device plugged (true, false)  (WPARAM)
  lUnitMask: (E): Unit Bit Mask for the drive letter  (LPARAM)

 Result type: Return zero (long)
 Author: Rolf Kary-Ehlers
 <TITLE OnNotifyDrive>
*********************************************************************************************/
LRESULT CPanel::OnNotifyDrive(WPARAM wParam, LPARAM lUnitMask)
{
	int  nDriveType = (int) LOWORD(wParam);
	BOOL bPlugged   = (BOOL)HIWORD(wParam);
	int iCurrentMask = lUnitMask; 

	if (   (nDriveType == DRIVE_REMOVABLE)						// Floppy, Memorystick
//		 || (nDriveType == DRIVE_FIXED)							// Harddisk ?
		 )
	{																		// Export of single Images
		BOOL nExportDrive = IsDriveAvailable(FALSE, m_sExportPath, m_sExportDriveName, lUnitMask);
		m_bIsExportDrive = m_nExportDrive = nExportDrive;
		WK_TRACE(_T("Export to: %s, %s\n"), m_sExportPath, m_sExportDriveName);
	}
	
	if (((nDriveType == DRIVE_REMOVABLE) && (lUnitMask > 8))// Memorystick
//		 || (nDriveType == DRIVE_FIXED)							// Harddisk: Später
		 )
	{
		//Image export to Extern Drive, not to CD/DVD
		m_bBackupToExternDrive = TRUE;
		m_dwFreeMBExternalDrive  = 0;
		m_dwTotalMBExternalDrive = 0;
		CheckCapacityOfExternDrive(m_dwFreeMBExternalDrive, m_dwTotalMBExternalDrive);
		if(m_dwFreeMBExternalDrive == m_dwTotalMBExternalDrive)
		{
			m_bIsExternDriveEmpty = TRUE;
		}
		else
		{
			m_bIsExternDriveEmpty = FALSE;
		}
		m_nExternBackupDrive = iCurrentMask;
		
	}

	if (nDriveType == DRIVE_CDROM)
	{
		//Backup to external CDROM is handled in ACDC.exe
	}

	if (   (nDriveType == DRIVE_NO_ROOT_DIR)					// on removal
		 || (!bPlugged))
	{
		//externes Disketten Laufwerk entfernt
		LPARAM nOldUnitMask = 1 << (m_nExportDrive-1);			// Exportdrive removed ?
		if (nOldUnitMask == lUnitMask)
		{
			BOOL nExportDrive = IsDriveAvailable(FALSE, m_sExportPath, m_sExportDriveName);
			m_bIsExportDrive = m_nExportDrive = nExportDrive;
		}
		WK_TRACE(_T("Export to: %s, %s\n"), m_sExportPath, m_sExportDriveName);

		//externer backup Laufwerk (Memorystick, später auch HDD) entfernt
		if(m_nExternBackupDrive == iCurrentMask)
		{
			if(!m_nExportDrive)
			{
				m_bIsExportDrive = FALSE;
			}

			m_bBackupToExternDrive = FALSE;

			//wird gerade die Endzeit berechnet, Backupfenster schließen
			CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
			if(pBW)
			{
				if(pBW->IsInCalculate())
				{
					pBW->SetCloseBackupWindow(TRUE);
					m_eInputState = InputNo;
				}
				//wenn gerade im backup, dann liefert die Datenbank ein CopyError, wenn 
				//das externe drive entfernt wurde. Wird dann im BackupWindow gehandled
			}
		}
	}
  
	UpdateMenu();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CString CPanel::GetDriveName(LPCTSTR sPath)
{
	CString sDevice;
	if (pGetVolumeNameForVolumeMountPoint)
	{
		_TCHAR szVolumeName[MAX_PATH];
		if (pGetVolumeNameForVolumeMountPoint(sPath, szVolumeName, MAX_PATH))
		{
			CWK_Profile prof(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
			BYTE *pData = NULL;
			UINT i, nBytes = 0;
			szVolumeName[1] = _T('?');
			szVolumeName[_tcsclen(szVolumeName)-1] = 0;
			
			prof.GetBinary(_T("SYSTEM\\MountedDevices\\"), szVolumeName, &pData, &nBytes);
			if (pData)
			{
				CString sName;
				nBytes /= sizeof(WORD);
				LPTSTR szName = sName.GetBufferSetLength(nBytes+1);
				for (i=0; i<nBytes; i++)
				{
					szName[i] = (char) ((LPCWSTR)pData)[i];
				}
				delete[] pData;
				szName[i] = 0;
				sName.Replace(_T('#'), _T('\\'));
				i = sName.Find(_T('{'));
				if (i!=-1)
				{
					sName	= _T("SYSTEM\\CurrentControlSet\\Enum\\") + sName.Mid(4, i-5);
					sDevice = prof.GetString(sName, _T("FriendlyName"), NULL);
					if (sDevice.IsEmpty())
					{
						sDevice = prof.GetString(sName, _T("DeviceDesc"), NULL);
					}
				}
			}
		}
	}
	return sDevice;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnExportSettings()
{
	BOOL	bResult = FALSE;

	CString sMsg;

	if (IsDriveAvailable(m_nExportDrive, m_sExportPath, m_sExportDriveName))
	{
		if (!m_sExportPath.IsEmpty())
		{
			sMsg.LoadString(IDS_EXPORT_RUNNING);
			m_ctrlDisplayTooltip.StopScrolling();
			SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);

			CSettings Settings(m_sExportPath);
			if (Settings.Export())
				bResult = TRUE;
		}
	}

	if (bResult)
	{
		sMsg.LoadString(IDS_EXPORT_SUCCESS);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadInfoMessage();
	}
	else
	{
		sMsg.LoadString(IDS_EXPORT_FAILED);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadErrorMessage();
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnImportSettings()
{
	BOOL	bResult = FALSE;

	CString sMsg;
	
	if (IsDriveAvailable(m_nExportDrive, m_sExportPath, m_sExportDriveName))
	{
		if (!m_sExportPath.IsEmpty())
		{
			sMsg.LoadString(IDS_IMPORT_RUNNING);
			m_ctrlDisplayTooltip.StopScrolling();
			SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);

			CSettings Settings(m_sExportPath);
			if (Settings.Import())
				bResult = TRUE;
		}
	}
	if (bResult)
	{
		sMsg.LoadString(IDS_IMPORT_SUCCESS);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadInfoMessage();
	}
	else
	{
		sMsg.LoadString(IDS_IMPORT_FAILED);
		m_ctrlDisplayTooltip.StopScrolling();
		SetDisplayText(m_ctrlDisplayTooltip, sMsg, 0, 2);
		WaitToReadErrorMessage();
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::OnMaintenance()
{
	BOOL	bResult = FALSE;
	EnablePanel(FALSE);
	m_pMaintenanceDlg = new CMaintenanceDlg(this);

	if (m_pMaintenanceDlg)
	{
		m_pMaintenanceDlg->DoModal();
		WK_DELETE(m_pMaintenanceDlg);
	}
	EnablePanel(TRUE);
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsDVDBackupAllowedByDongle()
{
	return m_bBackUpToDVDAllowedByDongle;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsNetAllowedByDongle()
{
	return m_bNetAllowedByDongle;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsISDNAllowedByDongle()
{
	return m_bISDNAllowedByDongle;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::IsBackUpAllowedByDongle()
{
	return m_bBackUpAllowedByDongle;
}
CString	CPanel::GetCurrentLanguageAbbr() const
{
	if (m_pDefaultLanguage)
	{
		return m_pDefaultLanguage->GetAbbreviation();
	}
	return _T("");
}

//TODO TKR für Sync Abspielen
/*
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::CanSyncPlay()
{
	BOOL bRet = FALSE;
//TODO TKR synchrones Abspielen wird später im PanelMenü gesetzt und nicht in Registry
	//zurzeit (Test) durch Schalter in Registry freischaltbar
	CWK_Profile prof;
	bRet = prof.GetInt(REGKEY_DVCLIENT, _T("Sync"),  bRet);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::CheckNextSyncPlayWnd(CPlayWindow::PlayStatus& playstatus, 
							      CPlayWindow::PlayStatus& currentPlayStatus, 
							      CPlayWindow* pPlayWnd)
{


	TRACE(_T("############### CheckNextSyncPlayWnd()\n"));
	BOOL bRet = TRUE;

	if(	   WK_IS_WINDOW(pPlayWnd)
		&& playstatus != CPlayWindow::PS_EJECT
		&& pPlayWnd->IsWindowVisible())
	{
		BOOL bForward = FALSE;
		WORD wPlayingCam = 0;
		CSystemTime st;

		//bestimmen die aktuelle Abspielrichtung 
		//ist wichtig für das Suchen des jeweils nächsten Bildes
		//forward  = das nächste Bild muss das Älteste sein
		//backward = das nächste Bild muss das Neuste sein
		if(    playstatus == CPlayWindow::PS_SINGLE_FORWARD
			|| playstatus == CPlayWindow::PS_PLAY_FORWARD
			|| playstatus == CPlayWindow::PS_FAST_FORWARD
			|| (    playstatus == CPlayWindow::PS_STOP
				 && (   currentPlayStatus == CPlayWindow::PS_SINGLE_FORWARD
					 || currentPlayStatus == CPlayWindow::PS_PLAY_FORWARD
					 || currentPlayStatus == CPlayWindow::PS_FAST_FORWARD)
			   ))

		{
			bForward = TRUE;
		}

		if(   (    playstatus == CPlayWindow::PS_PLAY_BACK
			    || playstatus == CPlayWindow::PS_FAST_BACK)
			&& 
			  (    currentPlayStatus == CPlayWindow::PS_PLAY_FORWARD
			    || currentPlayStatus == CPlayWindow::PS_FAST_FORWARD))
		{
			//alles auf STOP setzen
			pPlayWnd->SetPlayStatus(CPlayWindow::PS_STOP);
			TRACE(_T("######### alles auf STOP\n"));
			return TRUE;
		}

		WORD wOldestCameraNr = 0;
		WORD wCurrentCamNr   = 0;
		CPlayWindow::PlayStatus psSyncPlaying = CPlayWindow::PS_STOP;

		//finde das älteste (forward) bzw. neuste (backward) Bild
		CDisplayWindow* pDW = theApp.GetMainFrame()->FindOldestDisplayWindow(st, bForward, wPlayingCam);
		

	
		if(WK_IS_WINDOW(pDW))
		{
			//hole die Kamera ID`s und den PlayStutus des ältesten bzw. neusten Bildes
			wOldestCameraNr	= pDW->GetID().GetSubID();
			wCurrentCamNr	= pPlayWnd->GetID().GetSubID();
			psSyncPlaying	= ((CPlayWindow*)pDW)->GetPlayStatus();
		}

		//Sonderfall: spielt z.B. Kam 4 (klein) vorwärts ab und Kam 1 ist (groß) bei 1plus im
		//Panel die aktuelle Kamera, dann ist beim Wechsel nach rückwärts (Buttonklick) Kam 1
		//die nächste abzuspielende Kamera (da im Bezug auf Kam 4 nun die Neuste). In diesem Fall
		//wird Kam 1 durch Buttonklick rückwärts abgespielt und Kam 4 NICHT gestoppt, da
		//wCurrentCamNr == wOldestCameraNr. Deshalb beim Buttonklick alle abspielenden Kameras
		//(nur eine Kamera spielt zurzeit ab) stoppen, wenn sie nicht die aktuelle oder die 
		//älteste (bzw. neuste) ist.


		if(wCurrentCamNr != wOldestCameraNr)
		{
			//wechsel auf andere Kamera
			//gerade abspielender Button wurde erneut gedrückt, Abspielen stoppen
			if(psSyncPlaying == playstatus)
			{
				TRACE(_T("++++ psSyncPlaying == playstatus == %d\n"), psSyncPlaying);
				((CPlayWindow*)pDW)->SetPlayStatus(CPlayWindow::PS_STOP);
				playstatus = CPlayWindow::PS_STOP;
			}
			else
			{	
				//spiele andere als aktuelle Kamera ab
				CPlayWindow::PlayStatus CurrStatus = pPlayWnd->GetPlayStatus();
				if(CurrStatus != CPlayWindow::PS_STOP)
				{
					//Abspielen der spielenden Kamera stoppen
					TRACE(_T("+++ current auf STOP gesetzt\n"));
					pPlayWnd->SetPlayStatus(CPlayWindow::PS_STOP);
				}
				
				//speichere die als nächstes abzuspielende Kamera
				m_wNextSyncPlayWnd = wOldestCameraNr;
				m_nCamNr = wOldestCameraNr & (MAX_CAM-1);

				TRACE(_T("++ current Cam: %d\n"), wCurrentCamNr & (MAX_CAM-1));
				TRACE(_T("++ oldest Cam:  %d ->change\n"), wOldestCameraNr & (MAX_CAM-1));

				//FALSE erzwingt erneuten Aufruf von CPanel::OnSetPlayStatus() zum
				//Abspielen der nächsten ältesten bzw. neusten Kamera
				bRet = FALSE;
			}
		}
		else
		{
			if(wCurrentCamNr != wPlayingCam)
			{
				//stopp gerade spielende Kamera
				CMainFrame* pMainWnd  = theApp.GetMainFrame();
				if(WK_IS_WINDOW(pMainWnd))
				{
					CPlayWindow* pCurrPlayWnd = pMainWnd->GetPlayWindow((WORD)wPlayingCam);
					pCurrPlayWnd->SetPlayStatus(CPlayWindow::PS_STOP);
				}
			}
			//kein Wechsel der Kamera nötig, spiele selbe Kamera ab
			TRACE(_T("++ current Cam = oldest Cam\n"));
			TRACE(_T("++ spiele Kamera %d\n"), wCurrentCamNr & (MAX_CAM-1));
			m_wNextSyncPlayWnd = wOldestCameraNr;
			m_nCamNr = wOldestCameraNr  & (MAX_CAM-1);
		}
	}
	else
	{
		TRACE(_T("################# else:  IsWnd: %d  !=EJECT: %d  IsVisible: %d\n"), WK_IS_WINDOW(pPlayWnd),
			playstatus != CPlayWindow::PS_EJECT, pPlayWnd->IsWindowVisible());
	}

	return bRet;
}
*/

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateCameraTermination()
{
	BOOL bResult = FALSE;

	BOOL bCameraTermination = TRUE;
	CString sMsg;
	CString sSubMenu;

	CMainFrame*		pMainWnd = theApp.GetMainFrame();
	if (pMainWnd)
	{
		CLiveWindow* pLiveWnd = pMainWnd->GetLiveWindow((WORD)m_nCamNr);
		if (pLiveWnd)
		{
			//check card type of current camera (Jacob, Savic, Tasha, Q)
			switch (pLiveWnd->GetAlarmCallEventByCardType())
			{
				case CARD_TYPE_SAVIC:
					m_pOSD->EnableMenuItem(IDOSD_CAMERA_TERMINATION, MF_DISABLED);
					break;
				case CARD_TYPE_JACOB:
					m_pOSD->EnableMenuItem(IDOSD_CAMERA_TERMINATION, MF_ENABLED);
					break;
				case CARD_TYPE_TASHA:
					m_pOSD->EnableMenuItem(IDOSD_CAMERA_TERMINATION, MF_ENABLED);
					break;
				case CARD_TYPE_Q:
					m_pOSD->EnableMenuItem(IDOSD_CAMERA_TERMINATION, MF_DISABLED);
					break;
				default:
					m_pOSD->EnableMenuItem(IDOSD_CAMERA_TERMINATION, MF_DISABLED);
			}

			bResult = pLiveWnd->GetCameraTermination(bCameraTermination);
		}
	}

	if (!m_bSetValue)
	{
		m_bCameraTermination = bCameraTermination;
	}

	if (m_bCameraTermination)
		sMsg.LoadString(IDS_ON);
	else
		sMsg.LoadString(IDS_OFF);

	if (bResult)
	{
		if (m_bSetValue && (m_bCameraTermination == bCameraTermination))
			sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
		else
			sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
	}
	else
			sSubMenu.Format(_T("%s: ??"), m_pOSD->GetSubString());

	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeCameraTermination()
{
	m_bCameraTermination = !m_bCameraTermination;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetCameraTermination()
{
	BOOL bResult = FALSE;

	if (!m_bSetValue)
	{
		if (theApp.GetMainFrame())
		{
			CLiveWindow* pLiveWnd = theApp.GetMainFrame()->GetLiveWindow((WORD)m_nCamNr);
			if (pLiveWnd)
				bResult = pLiveWnd->SetCameraTermination(m_bCameraTermination);
		}
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeSystemAGC()
{
	m_nSystemAGC == VIDEO_AGC_ON ? m_nSystemAGC = VIDEO_AGC_OFF : m_nSystemAGC = VIDEO_AGC_ON;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuSystemAGC()
{
	CString sMsg;
	BOOL nSystemAGC = VIDEO_AGC_ON;
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		//hole eingestellten wert von q unit
		//AGC Einstellung
		//nSystemAGC == 0 -> AGC is ON -> VIDEO_AGC_ON (Q Unit value: 0)
		//nSystemAGC == 1 -> AGC is OFF -> VIDEO_AGC_OFF (Q Unit value: 3)
		pServer->GetSystemAGC(nSystemAGC, m_nCamNr);
	}

	if (!m_bSetValue)
	{
		m_nSystemAGC = nSystemAGC;
	}
	if (m_nSystemAGC == VIDEO_AGC_ON)
		sMsg.LoadString(IDS_ON);
	else if (m_nSystemAGC == VIDEO_AGC_OFF)
		sMsg.LoadString(IDS_OFF);

	CString sSubMenu;
	// Actual value?
	if (   m_bSetValue
		&& (nSystemAGC == m_nSystemAGC)
		)
	{
		sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
	}
	else
	{
		sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
	}
	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetSystemAGC()
{
	BOOL bResult = FALSE;
	if (!m_bSetValue)
	{
		CServer* pServer = theApp.GetServer();
		if (pServer)
		{
			bResult = pServer->SetSystemAGC(m_nSystemAGC, m_nCamNr);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeSystemVideoformat()
{
	m_nSystemVideoformat == VIDEO_FORMAT_MPEG4 ? 
		m_nSystemVideoformat = VIDEO_FORMAT_JPEG : m_nSystemVideoformat = VIDEO_FORMAT_MPEG4;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuSystemVideoformat()
{
	CString sMsg;
	int nSystemVideoformat = VIDEO_FORMAT_MPEG4;
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		//hole eingestellten wert von q unit
		pServer->GetSystemVideoformat(nSystemVideoformat, m_nCamNr);
	}

	if (!m_bSetValue)
	{
		m_nSystemVideoformat = nSystemVideoformat;
	}
	m_nSystemVideoformat == VIDEO_FORMAT_MPEG4 ? 
	    sMsg.LoadString(IDS_SYSTEM_MPEG4) : sMsg.LoadString(IDS_SYSTEM_JPEG);

	CString sSubMenu;
	// Actual value?
	if (   m_bSetValue
		&& (nSystemVideoformat == m_nSystemVideoformat)
		)
	{
		sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
	}
	else
	{
		sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
	}
	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetSystemVideoformat()
{
	BOOL bResult = FALSE;
	if (!m_bSetValue)
	{
		CServer* pServer = theApp.GetServer();
		if (pServer)
		{
			bResult = pServer->SetSystemVideoformat(m_nSystemVideoformat, m_nCamNr);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnChangeSystemVideonorm()
{
	m_sSystemVideonorm == CSD_PAL ? 
		m_sSystemVideonorm = CSD_NTSC : m_sSystemVideonorm = CSD_PAL;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnUpdateMenuSystemVideonorm()
{
	CString sMsg;
	CString sSystemVideonorm = CSD_PAL;
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		//hole eingestellten wert von q unit
		pServer->GetSystemVideonorm(sSystemVideonorm, m_nCamNr);
	}

	if (!m_bSetValue)
	{
		m_sSystemVideonorm = sSystemVideonorm;
	}
	m_sSystemVideonorm == CSD_PAL ? 
		sMsg.LoadString(IDS_SYSTEM_PAL) : sMsg.LoadString(IDS_SYSTEM_NTSC);

	CString sSubMenu;
	// Actual value?
	if (   m_bSetValue
		&& (sSystemVideonorm == m_sSystemVideonorm)
		)
	{
		sSubMenu.Format(_T("%s: *%s"), m_pOSD->GetSubString(), sMsg);
	}
	else
	{
		sSubMenu.Format(_T("%s: %s"), m_pOSD->GetSubString(), sMsg);
	}
	SetDisplayText(m_ctrlDisplayMenu2, sSubMenu, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::SetSystemVideonorm()
{
	BOOL bResult = FALSE;
	if (!m_bSetValue)
	{
		CServer* pServer = theApp.GetServer();
		if (pServer)
		{
			//nur die Videonorm ändern, wenn ein neuer Wert eingestellt werden soll,
			//da die Q Unit sich selbst dann ausschaltet (reset)
			CString sSystemVideonorm = CSD_PAL;
			pServer->GetSystemVideonorm(sSystemVideonorm, m_nCamNr);
			if(sSystemVideonorm != m_sSystemVideonorm)
			{
				bResult = pServer->SetSystemVideonorm(m_sSystemVideonorm, m_nCamNr);
			}
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnDestroy()
{
	CWnd *pWnd = GetDlgItem(IDC_CPU_USAGE);
	if (pWnd)
	{
		delete pWnd;
	}
	pWnd = GetDlgItem(IDC_CPU_AVG_USAGE);
	if (pWnd)
	{
		delete pWnd;
	}
	pWnd = GetDlgItem(IDC_PERFORMANCE_LEVEL);
	if (pWnd)
	{
		delete pWnd;
	}
	CTransparentDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
CString CPanel::NameOfEnumInputState(INP_STATE Inp)
{

	switch (Inp)
	{
	case InputNo:						return _T("InputNo");
	case InputCamFirstDigit:			return _T("InputCamFirstDigit");
	case InputPanelPIN:					return _T("InputPanelPIN");
	case InputOldPIN:					return _T("InputOldPIN");
	case InputNewPIN:					return _T("InputNewPIN");
	case InputConfirmPIN:				return _T("InputConfirmPIN");
	case InputConfigPIN:				return _T("InputConfigPIN");
	case InputSearchDate:				return _T("InputSearchDate");
	case InputSearchTime:				return _T("InputSearchTime");
	case InputTimer:					return _T("InputTimer");
	case InputClockDate:				return _T("InputClockDate");
	case InputClockTime:				return _T("InputClockTime");
	case InputIP:						return _T("InputIP");
	case InputSubNetMask:				return _T("InputSubNetMask");
	case InputMSN:						return _T("InputMSN");
	case InputGateway:					return _T("InputGateway");
	case InputChangeCamName:			return _T("InputChangeCamName");
	case InputExportDlgOpen:			return _T("InputExportDlgOpen");
	case InputBackupStartDateTime:		return _T("InputBackupStartDateTime");
	case InputBackupWaitCalculatedEnd:	return _T("InputBackupWaitCalculatedEnd");
	case InputBackupEndDateTime:		return _T("InputBackupEndDateTime");
	case InputBackupCameras:			return _T("InputBackupCameras");
	case InputBackupAskForMedium:		return _T("InputBackupAskForMedium");
	case InputBackupWaitForMedium:		return _T("InputBackupWaitForMedium");
	case InputBackupFoundMedium:		return _T("InputBackupFoundMedium");
	case InputBackupFoundNoEmptyCD:		return _T("InputBackupFoundNoEmptyCD");
	case InputBackupDVDNotAllowed:		return _T("InputBackupDVDNotAllowed");
	case InputBackupEraseMedium:		return _T("InputBackupEraseMedium");
	case InputBackupEraseMediumConfirmingYes:	return _T("InputBackupEraseMediumConfirmingYes");
	case InputBackupEraseMediumConfirmingNo:	return _T("InputBackupEraseMediumConfirmingNo");
	case InputUpdateSystem:				return _T("InputUpdateSystem");
	case InputChangeNetName:			return _T("InputChangeNetName");
	case InputSelectHost:				return _T("InputSelectHost");
	case InputEditHostList:				return _T("InputEditHostList");
	case InputEditAlarmDialingNumber:	return _T("InputEditAlarmDialingNumber");
	case InputSetExpansionCode:			return _T("InputSetExpansionCode");
	case InputShutdownPIN:				return _T("InputShutdownPIN");
	case InputResetPIN:					return _T("InputResetPIN");
	case InputPrinterPIN:				return _T("InputPrinterPIN");
	case InputCameraPTZID:				return _T("InputCameraPTZID");
	case InputCameraPTZComPort:			return _T("InputCameraPTZComPort");
	default:							return _T("unknown InputStatus");
	}
}
