// DVStarter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DVStarter.h"
#include "DVStarterDlg.h"
#include "CInfoDlg.h"
#include "..\dvclient\enumerations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szLogonNT[]		= _T("Microsoft\\Windows NT\\CurrentVersion");
static const TCHAR szLogon95[]			= _T("Microsoft\\Windows\\CurrentVersion");
static TCHAR BASED_CODE szWinlogon[]	= _T("Winlogon");
static TCHAR BASED_CODE szMsdos[]		= _T("C:\\msdos.sys");



#define DV_DVCLIENT						_T("DV\\DVClient")

#define HKCU_CONTROL_PANEL_DESKTOP		_T("Control Panel\\Desktop\\")
#define WALLPAPER						_T("Wallpaper")
#define WALLPAPERSTYLE					_T("WallpaperStyle")
#define TILEWALLPAPER					_T("TileWallpaper")
#define BACKUP							_T("Backup")

#define HKCU_CONTROL_PANEL_COLORS		_T("Control Panel\\Colors")
#define BACKGROUND						_T("Background")

#define HKCU_EXPLORER_POLICIES			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\")
#define NODRIVES						_T("NoDrives")
#define NOVIEWONDRIVE					_T("NoViewOnDrive")
#define NODRIVEAUTORUN					_T("NoDriveAutoRun")
#define NOTRAYCONTEXTMENU				_T("NoTrayContextMenu")
#define NOVIEWCONTEXTMENU				_T("NoViewContextMenu")
#define NODESKTOP						_T("NoDeskTop")
#define NOLOWDISKSPACECHECKS			_T("NoLowDiskSpaceChecks")

#define HKLM_WIN_SETUP				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup\\")
#define INSTALLATION_SOURCES		_T("Installation Sources")
#define SOURCEPATH					_T("SourcePath")
#define SERVICEPACKSOURCEPATH		_T("ServicePackSourcePath")
#define DRIVERCACHEPATH				_T("DriverCachePath")

#define HKCU_SYSTEM_POLICIES		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\")
#define DISABLETASKMGR				_T("DisableTaskMgr")

#define HKLM_WINLOGON				_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define POWERDOWNAFTERSHUTDOWN		_T("PowerdownAfterShutdown")

#define HKCU_DRIVERSIGNING			_T("Software\\Microsoft\\Driver Signing\\")
#define POLICY						_T("Policy")

#define HKLM_SERVICE_DHCP			_T("SYSTEM\\CurrentControlSet\\Services\\Dhcp")
#define SERVICE_START_TYPE			_T("Start")
#define SERVICE_DEACTIVE			4
#define SERVICE_MANUAL				3
#define SERVICE_AUTOMATIC			2

#define SHELL_BOOT					_T("boot")
#define SHELL_SHELL					_T("Shell")
#define SHELL_SYSTEM_INI			_T("system.ini")
#define AUTORESTARTSHELL			_T("AutoRestartShell")
#define SHELL_OPTIONS				_T("Options")
#define SHELL_LOGO					_T("Logo")
#define SHELL_BOOTKEYS				_T("BootKeys")
#define SHELL_C_LOGO_SYS			_T("c:\\logo.sys")
#define SHELL_C_LOGO_000			_T("c:\\logo.000")
#define SHELL_LOGOW_SYS				_T("\\logow.sys")
#define SHELL_LOGOS_SYS				_T("\\logos.sys")
#define SHELL_LOGOW_000				_T("\\logow.000")
#define SHELL_LOGOS_000				_T("\\logos.000")
#define SHELL_WINNT256_BMP			_T("WinNT256.bmp")
#define SHELL_WINNT256_000			_T("WinNT256.000")
#define SHELL_WIN9X_BMP				_T("\\Win9x.bmp")
#define WIN_NT_BMP					_T("\\WinNT.bmp")
#define BACKSLASH						_T('\\')


/////////////////////////////////////////////////////////////////////////////
// CDVStarterApp

BEGIN_MESSAGE_MAP(CDVStarterApp, CWinApp)
	//{{AFX_MSG_MAP(CDVStarterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVStarterApp construction

CDVStarterApp::CDVStarterApp()
{
	WK_CheckTempDirectory();
 	m_AppID	= WAI_LAUNCHER;
	m_bRebootAtExit = FALSE;
	m_pInfoDlg = NULL;
	m_bUsePiezo = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDVStarterApp object

CDVStarterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDVStarterApp initialization
#include "CRTErrorDlg.h"

BOOL CDVStarterApp::InitInstance()
{
	if (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER)) 
	{
		AfxMessageBox(_T("Idip® Software is already started!\n\nStarting both modules will cause resource conflicts!"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if (!WK_ALONE(WK_APP_NAME_DVSTARTER))
	{
		return FALSE;
	}

	int nI;
	_TCHAR szPath[_MAX_PATH];
	GetModuleFileName(m_hInstance, szPath, _MAX_PATH);
	m_sHomePath = szPath;
	CString sDongle;
	nI = m_sHomePath.ReverseFind(BACKSLASH);
	if (nI != -1)
	{
		m_sHomePath = m_sHomePath.Left(nI);
#ifdef _DEBUG
//#ifdef _UNICODE
//		m_sHomePath = _T("C:\\DVX");
//#else
		m_sHomePath = _T("C:\\DV");
//#endif
#endif
		sDongle = m_sHomePath + _T("\\DVDongle.dcf");
	}

	CWK_Profile wkp;
	CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));

	int nRunVcd = wkp.GetInt(DV_DVSTARTER, VCD_RUN_MODULE, -1);
	CString sEmbedded = wkpSystem.GetString(REGPATH_HKLM_WINDOWS_EMBEDDED, REGKEY_RUNTIME_PID, NULL);
	if (!sEmbedded.IsEmpty())
	{
		nRunVcd = 1;
	}
	if (nRunVcd == -1)
	{
		wkp.WriteInt(DV_DVSTARTER, VCD_RUN_MODULE, 0);
		nRunVcd = 0;
	}
	if (nRunVcd)
	{
		DWORD dwExitCodeVCD = 0;
		CApplication app;
		CString sCheckDisk = m_sHomePath + _T("\\VideteCheckDisk.exe");
		app.Create(sCheckDisk, WAI_VIDETE_CHECK_DISK, 5);
		app.SetWaitUntilProcessEnd(true);
		app.SetTemporaryCmdLine(_T(" /a"));
		BOOL bResult = app.Load(SW_SHOW, 2000);
		if (bResult)
		{
			do
			{
				app.IsRunning(0);
                dwExitCodeVCD = app.GetExitCode();
				if (dwExitCodeVCD == STILL_ACTIVE)
				{
					DWORD dwResult;
					if (app.SendMessageTimeOut(WM_SELFCHECK, 0, 0, 5000, &dwResult) && dwResult == 0)
					{
						app.Terminate();
						break;
					}
				}
				else if (dwExitCodeVCD == 2)
				{
					m_bRebootAtExit = TRUE;
					return FALSE;
				}
			}while (dwExitCodeVCD == STILL_ACTIVE);
		}
	}

	wkp.DeleteEntry(SECTION_COMMON, COMMON_SHUTDOWN_SETTING);

	// TODO! ML: Use CSkinDialog in stead of CTransparentDialog
	CSkinDialog::SetOEMSettings(StyleBackGroundColorChangeSimple, SKIN_COLOR_BLUE, SKIN_COLOR_GOLD_METALLIC, TRUE);
	CTime StartTime;									 
	CTime CurrentTime = CTime::GetCurrentTime();

	// Die Registrierung soll NICHT sofort beim Systemstart erfolgen, da die Regsitrierung
	// evtl. schon beschädigt ist. Die Sicherung erfolgt um Mitternacht jeden Tages
	wkp.WriteString(DV_DVCLIENT, _T("NextRDiskTime"), _T(""));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	InitDebugger(_T("DVStarter.log"), m_AppID, NULL);
	WK_TRACE(_T("Starting DVStarter...\n"));

	CString sOEM1 = GetOEMString();
#ifdef _UNICODE
	CString sOEM2 = wkp.GetString(DV_DVSTARTER, _T("Oem"), _T(""), TRUE);
#else
	CString sOEM2 = wkp.GetString(DV_DVSTARTER, _T("Oem"), _T(""));
#endif
	wkp.Decode(sOEM2);

	WK_TRACE(_T("%s, %s\n"), sOEM1, sOEM2);
	if ((sOEM2 != _T("11071964")) && (sOEM2 != sOEM1))
	{
		WK_TRACE_WARNING(_T("ProductId wrong\n"));
//		AfxMessageBox(sOEM1 + sOEM2 + _T("ProductId wrong"), MB_OK, 0);
		return FALSE;
	}
	
	if (sOEM2 == _T("11071964"))
	{
		wkp.Encode(sOEM1);
#ifdef _UNICODE
		wkp.WriteString(DV_DVSTARTER, _T("Oem"), sOEM1, TRUE);
#else
		wkp.WriteString(DV_DVSTARTER, _T("Oem"), sOEM1);
#endif
	}

	m_pDongle = new CWK_Dongle(sDongle);

	AdjustPrivileges(SE_SHUTDOWN_NAME);

	CString sComputername = wkpSystem.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,_T(""));
	if (sComputername.GetLength())
	{
		if (_istdigit(sComputername.GetAt(0)))	// if the first character is a decimal digit
		{										// DHCP and DNS will not work
			OEM eOem = (OEM)wkp.GetInt(DV_DVCLIENT, _T("Oem"),  (int)OemDTS);
			CString sOEM(_T("DTS"));
			switch (eOem)
			{
			case OemDTS:
				sOEM = _T("DTS");
				break;
			case OemAlarmCom:
			case OemPOne:
				sOEM = _T("VDR");
				break;
			case OemSantec:
				sOEM = _T("SDR");
				break;
			}

			sComputername = sOEM + sComputername;
			CString sUpperName(sComputername);	// change the computername
			sUpperName.MakeUpper();

			BOOL bReturn =	SetComputerNameEx(ComputerNamePhysicalNetBIOS, sUpperName) &&
							SetComputerNameEx(ComputerNamePhysicalDnsHostname, sComputername);

			if (bReturn)
			{
				wkpSystem.WriteInt(HKLM_SERVICE_DHCP, SERVICE_START_TYPE, SERVICE_AUTOMATIC);
				AfxMessageBox(_T("The computername and DHCP service\nhas been adapted for DHCP ability.\n\nThe system is rebooting now!"), MB_OK|MB_ICONINFORMATION);
				m_bRebootAtExit = TRUE;
				return FALSE;
			}
		}
	}

#if _MFC_VER < 0x700
	Enable3dControls();			// Call this when using MFC in a shared DLL
#endif

	// Alle evtl. von Scandisk angelegten Verzeichnisse und Dateien löschen.
	ScanChk();

	// Sockets initialisieren.
	AfxSocketInit();

	CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON, NULL, _T("Microsoft\\Windows\\CurrentVersion"),_T(""));
	CString sFileFrom		= wkProf.GetString(_T("Run"), _T("PacketCD"), _T(""));
  	CString sFileTo			= _T("");
	CString sPacketCDPath	= _T("");

	if (!sFileFrom.IsEmpty())
	{
		int nIndex = sFileFrom.ReverseFind(BACKSLASH);
		if (nIndex != -1)
		{
			sPacketCDPath = sFileFrom.Left(nIndex);
			
			// Benenne UDFForm.exe in UDFOrig.exe um
			sFileFrom	= sPacketCDPath + _T("\\udfform.exe");
			sFileTo		= sPacketCDPath + _T("\\udforig.exe");
			
			// Ist UDFForm.exe das Original?
			if (GetOriginalFileName(sFileFrom) == _T("udfform.exe"))
				CopyFile(sFileFrom, sFileTo);
		
			// Verschiebt den UDFFormat-Clone in das PacketCD Verzeichnis.
			MoveFileEx(m_sHomePath + _T("\\remotecd.exe"), sFileFrom, MOVEFILE_REPLACE_EXISTING);

			//copy cipc.dll into packetCD folder because RemoteCD also uses cipc.dll
			CString sFileCIPC = _T("cipc.dll");
			CString sPacketCDCIPC = sPacketCDPath + _T("\\")+sFileCIPC;
			if(CopyFile(sFileCIPC, sPacketCDCIPC))
			{
				WK_TRACE(_T("InitInstance(): Cipc.dll copied from %s to %s\n"), m_sHomePath, sPacketCDPath);
			}
			else
			{
				WK_TRACE(_T("InitInstance(): Could not copie cipc.dll from %s to %s\n"), m_sHomePath, sPacketCDPath);
			}

		}
	}

	BOOL bShowStartUpInfo = wkp.GetInt(DV_DVSTARTER, _T("ShowStartUpInfo"), 0);
	wkp.WriteInt(DV_DVSTARTER, _T("ShowStartUpInfo"), bShowStartUpInfo);
	m_bUsePiezo = wkp.GetInt(DV_DVSTARTER, _T("UsePiezo"), m_bUsePiezo);
	wkp.WriteInt(DV_DVSTARTER, _T("UsePiezo"), m_bUsePiezo);
	
	if (bShowStartUpInfo)
	{
		// StartUpInfo-Dialog anlegen
		m_pInfoDlg = new CInfoDlg();
		if (m_pInfoDlg)
			m_pInfoDlg->Create();
	}

	if (IsTransmitter())
	{
		// Festplattencache abschalten gewünscht?
		BOOL bDisableHDCache = wkp.GetInt(DV_DVSTARTER, _T("DisableHDWriteCache"), 0);
		wkp.WriteInt(DV_DVSTARTER, _T("DisableHDWriteCache"), bDisableHDCache);
	
		if (bDisableHDCache)
		{
			// Beim Programmstart den Festplatten Writecache ausschalten...
			UINT uDeviceNum = 0;
			CWinAta* pAta = new CWinAta(uDeviceNum);
			if (pAta)
			{
				if (pAta->GetAtaErrorCode())
				{
					if (pAta->GetAtaErrorCode() == CWinAta::aec_OSNotSupported)
						WK_TRACE_WARNING(_T("Wrong OS...can't activate/deactivate write cache with ATA-cmds\n"));
					else
						WK_TRACE_WARNING(_T("Disk%u communication error (%u)\n"), uDeviceNum, pAta->GetAtaErrorCode());
				}
				else
				{
					if (pAta->DisableWriteCache())
						WK_TRACE(_T("Write cache is now deactivated.\n"));
					else
						WK_TRACE_WARNING(_T("Write cache deactivating failed.\n"));
				}
				WK_DELETE(pAta);
			}
		}
		else
		{
			// Beim Programmstart den Festplatten Writecache einschalten...
			UINT uDeviceNum = 0;
			CWinAta* pAta = new CWinAta(uDeviceNum);
			if (pAta)
			{
				if (pAta->GetAtaErrorCode())
				{
					if (pAta->GetAtaErrorCode() == CWinAta::aec_OSNotSupported)
						WK_TRACE_WARNING(_T("Wrong OS...can't activate/deactivate write cache with ATA-cmds\n"));
					else
						WK_TRACE_WARNING(_T("Disk%u communication error (%u)\n"), uDeviceNum, pAta->GetAtaErrorCode());
				}
				else
				{
					if (pAta->EnableWriteCache())
						WK_TRACE(_T("Write cache is now activated.\n"));
					else
						WK_TRACE_WARNING(_T("Write cache activating failed.\n"));
				}
				WK_DELETE(pAta);
			}
		}

		if (AmIShell())
		{		
			if (IsWinXP())
			{
/* TODO Erst einmal deaktiviert, bis das Partitionieren/Formatieren korrekt funktioniert.
				// Ist ein Laufwerk ohne Partitionen?
				// Es werden nur leere Laufwerke vom Typ FixedMedia partitioniert.
				for (int nDrive = 0; nDrive < 16; nDrive++)
					InitializeDrive(nDrive);

				// Ist eine Partition ohne Dateisystem? dann formatiere es
				FormatAllUnformatedFixedVolumes();
*/
			}
		}
	}

#ifdef _DEBUG
	RGBQUAD Color0;
	memset (&Color0, 0, sizeof(Color0));
	CFile file;
	CString sOriginal;
	if (file.Open(m_sHomePath + WIN_NT_BMP, CFile::modeRead))
	{
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		int              nColors = 0;

		file.Read(&bmfh, sizeof(BITMAPFILEHEADER));
		if (bmfh.bfType==0x4d42)
		{
			file.Read(&bmih, sizeof(BITMAPINFOHEADER));
			if (bmih.biBitCount<9) nColors = 1 << bmih.biBitCount;
			if (bmih.biClrUsed!=0) nColors = bmih.biClrUsed;
			RGBQUAD *pCol = NULL;
			DWORD   *pdwCol =(DWORD*) &Color0;
			if (nColors)                                                // read in the Colortable
			{
				pCol = new RGBQUAD[nColors];
				file.Read(pCol, nColors * sizeof(RGBQUAD));
			}
			file.Seek(bmfh.bfOffBits, CFile::begin);
			file.Read(&Color0, sizeof(RGBQUAD));

			switch(bmih.biBitCount)
			{
				case  4: Color0 = pCol[((*pdwCol)&0x000f)]; break;
				case  8:
					{
						Color0 = pCol[Color0.rgbBlue]; 
						break;
					}
				case 16:
				{
					RGBQUAD Color;
					Color.rgbRed   = (BYTE)(*pdwCol &  0x001f);
					Color.rgbGreen = (BYTE)((*pdwCol & 0x0370)>>5);
					Color.rgbBlue  = (BYTE)((*pdwCol & 0x7c00)>>10);
					break;
				}
				case 24: case 32:
					break;
			} 
			for (nI=0; nI<nColors; nI++)
			{
				sOriginal.Format(_T("%d, %d, %d"), pCol[nI].rgbRed, pCol[nI].rgbGreen, pCol[nI].rgbBlue);
				TRACE(_T("%d:%s\n"), nI, sOriginal);
			}
			WK_DELETE(pCol);
		}
	}
	sOriginal.Format(_T("%d, %d, %d"), Color0.rgbRed, Color0.rgbGreen, Color0.rgbBlue);
#endif
	// MainFrame anlegen
	CDVStarterDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
int CDVStarterApp::ExitInstance() 
{
	BOOL bIsTransmitter = IsTransmitter();

	// Im Moment soll der Plattenschreibcache IMMER aus sein. Es muß überprüft
	// werden, ob es evtl. ausreicht den Plattenschreibcache vor dem herunterfahren
	// abzuschalten.
#if (0) 
	if (IsTransmitter())
	{
		// Festplattencache abschalten gewünscht?
		// Festplattencache abschalten gewünscht?
		BOOL bDisableHDCache = prof.GetInt(DV_DVSTARTER, _T("DisableHDWriteCache"), 0);
		prof.WriteInt(DV_DVSTARTER, _T("DisableHDWriteCache"), bDisableHDCache);
	
		if (bDisableHDCache)
		{
			// Vor dem herunterfahren den Festplatten Writecache abschalten....
			// um zu verhindern, das die Registrierung beschädigt wird, wenn NT den
			// Rechner abschaltet, bevor die Festplatte den Writecache gesichert hat.
			UINT uDeviceNum = 0;
			CWinAta* pAta = new CWinAta(uDeviceNum);
			if (pAta)
			{
				if (pAta->GetAtaErrorCode())
				{
					if (pAta->GetAtaErrorCode() == CWinAta::aec_OSNotSupported)
						WK_TRACE_WARNING(_T("Wrong OS...can't activate/deactivate write cache with ATA-cmds\n"));
					else
						WK_TRACE_WARNING(_T("Disk%u communication error (%u)\n"), uDeviceNum, pAta->GetAtaErrorCode());
				}
				else
				{
					if (pAta->DisableWriteCache())
						WK_TRACE(_T("Write cache wurde deaktiviert."));
					else
						WK_TRACE(_T("Write cache konnte nicht deaktiviert werden."));
				}
				WK_DELETE(pAta);
			}
		}
	}
#endif

	WK_DELETE(m_pDongle);

	WK_DELETE(m_pInfoDlg);
	if (m_bRebootAtExit)
	{
		ExitWindowsEx(EWX_REBOOT,0);
	}

	if (AmIShell())
	{
		CWK_Profile wkp;
		if (wkp.GetInt(DV_DVSTARTER, _T("AsShell"), 0) == 0)
		{
			EnableCtrlAltDel();
			DisableAutologin();
			RebootNotAsShell();
			ExitWindowsEx(EWX_REBOOT,0);
		}
		else
		{
			if (wkp.GetInt(DV_DVSTARTER, _T("DoUpdate"), 0) == 1)
			{
				DoUpdate();
			}
			else
			{
				if (   bIsTransmitter
					&& IsNT40())
				{
					Rdisk(TRUE);
				}
				// Führe ein Reboot durch. Es wurde die IP-Adresse oder
				// Subnetmask geändert.
				if (wkp.GetInt(DV_DVSTARTER, _T("DoReboot"), 0) == 1)
				{
					wkp.DeleteEntry(DV_DVSTARTER, _T("DoReboot"));
					WK_TRACE(_T("EWX_REBOOT"));
					ExitWindowsEx(EWX_REBOOT,0);
				}
				else
				{
					CWK_Profile wkpNTL(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
					CString s = wkpNTL.GetString(szWinlogon,POWERDOWNAFTERSHUTDOWN,_T("0"));

					if (wkp.GetInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, 0))
					{
						wkp.WriteInt(SECTION_COMMON, COMMON_SHUTDOWN_SETTING, RD_ES_SHUTDOWN);
					}
					WK_TRACE(_T("%s=%s"), POWERDOWNAFTERSHUTDOWN, s);
					if (s==_T("1"))
					{
						WK_TRACE(_T("EWX_POWEROFF"));
						ExitWindowsEx(EWX_POWEROFF,0);
					}
					else
					{
						WK_TRACE(_T("EWX_SHUTDOWN"));
						ExitWindowsEx(EWX_SHUTDOWN,0);
					}
				}
			}
		}
	}
	else if (!AmIShell())
	{					   
		CWK_Profile wkp;  
// #ifdef _DEBUG
//		EnableCtrlAltDel();
// #endif
		if (wkp.GetInt(DV_DVSTARTER, _T("AsShell"), 0) == 1)
		{
			DisableCtrlAltDel();
			EnableAutologin();
			RebootAsShell();
			ExitWindowsEx(EWX_REBOOT,0);
		}
		else
		{
			if (wkp.GetInt(DV_DVSTARTER, _T("DoUpdate"), 0) == 1)
				DoUpdate();
		}
	}
	
	WK_TRACE(_T("DVStarter Software normal closed\n"));

	CloseDebugger();	

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::MakeMeShell()
{
	CString sPath;

	GetModuleFileName(m_hInstance, sPath.GetBuffer(MAX_PATH), _MAX_PATH);
	sPath.ReleaseBuffer();
	if (IsWin95())
	{
		WritePrivateProfileString(SHELL_BOOT, SHELL_SHELL,sPath,SHELL_SYSTEM_INI);
	}
	else if (IsNT())
	{
#ifndef _DEBUG
		CWK_Profile wkp(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
		wkp.WriteString(szWinlogon, SHELL_SHELL, sPath);
		wkp.WriteInt(szWinlogon, AUTORESTARTSHELL, 1);
#endif
	}

	if (IsWin95())
	{
		DWORD dwAt = GetFileAttributes(szMsdos);
		SetFileAttributes(szMsdos,FILE_ATTRIBUTE_NORMAL);
		Sleep(100);
		WritePrivateProfileString(SHELL_OPTIONS, SHELL_LOGO, _T("0"), szMsdos);
		WritePrivateProfileString(SHELL_OPTIONS, SHELL_BOOTKEYS, _T("0"), szMsdos);
		WritePrivateProfileString(NULL,NULL,NULL,szMsdos);
		Sleep(100);
		SetFileAttributes(szMsdos,dwAt);
	}

	if (IsWin95())
	{
		CopyFile(SHELL_C_LOGO_SYS, SHELL_C_LOGO_000);
		CopyFile(GetWindowsDirectory() + SHELL_LOGOW_SYS,   GetWindowsDirectory() + SHELL_LOGOW_000);
		CopyFile(GetWindowsDirectory() + SHELL_LOGOS_SYS,   GetWindowsDirectory() + SHELL_LOGOS_000);
		
		CopyFile(m_sHomePath + SHELL_WIN9X_BMP, SHELL_C_LOGO_SYS);
		CopyFile(m_sHomePath + SHELL_WIN9X_BMP, GetWindowsDirectory() + SHELL_LOGOW_SYS);
		CopyFile(m_sHomePath + SHELL_WIN9X_BMP, GetWindowsDirectory() + SHELL_LOGOS_SYS);
	}
	else if (IsNT())
	{
		CopyFile(GetWindowsDirectory() + BACKSLASH + SHELL_WINNT256_BMP, GetWindowsDirectory() + BACKSLASH + SHELL_WINNT256_000);
		CopyFile(m_sHomePath + WIN_NT_BMP, GetWindowsDirectory() + BACKSLASH + SHELL_WINNT256_BMP);

		CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, HKEY_CURRENT_USER,_T(""),_T(""));
		CWK_Profile wkp2(CWK_Profile::WKP_ABSOLUT, HKEY_USERS,_T(""),_T(""));
		CString sBmpPath     = m_sHomePath + WIN_NT_BMP,
				  sKey      = WALLPAPER;
		CString sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_DESKTOP, sKey, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey+BACKUP, sOriginal); 
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey, sBmpPath);
		CString sSection = _T(".DEFAULT\\");
		sSection += HKCU_CONTROL_PANEL_DESKTOP;
		wkp2.WriteString(sSection, sKey, sBmpPath);

		sKey      = WALLPAPERSTYLE;
		sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_DESKTOP, sKey, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP , sKey+BACKUP, sOriginal);
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey, _T("0"));
		wkp2.WriteString(sSection, sKey, _T("0"));

		sKey      = TILEWALLPAPER;
		sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_DESKTOP, sKey, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP , sKey+BACKUP, sOriginal);
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey, _T("0"));
		wkp2.WriteString(sSection, sKey, _T("0"));

		sSection = _T(".DEFAULT\\");
		sSection += HKCU_CONTROL_PANEL_COLORS;
		sKey      = BACKGROUND;
		sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_COLORS, sKey, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_COLORS , sKey+BACKUP, sOriginal);

		RGBQUAD Color0 = {0,0,0,0};
		CFile file;
		if (file.Open(sBmpPath, CFile::modeRead))						// WinNt.Bmp öffnen
		{
			BITMAPFILEHEADER bmfh;
			BITMAPINFOHEADER bmih;
			int              nColors = 0;

			file.Read(&bmfh, sizeof(BITMAPFILEHEADER));
			if (bmfh.bfType==0x4d42)										// ist es eine Bitmap ?
			{
				file.Read(&bmih, sizeof(BITMAPINFOHEADER));			// Header lesen
				if (bmih.biBitCount<9) nColors = 1 << bmih.biBitCount;
				if (bmih.biClrUsed!=0) nColors = bmih.biClrUsed;
				RGBQUAD *pCol = NULL;
				DWORD   *pdwCol =(DWORD*) &Color0;
				if (nColors)                                       // Farbtabelle lesen, wenn vorhanden
				{
					pCol = new RGBQUAD[nColors];
					file.Read(pCol, nColors * sizeof(RGBQUAD));
				}
				file.Seek(bmfh.bfOffBits, CFile::begin);				// erstes Pixel lesen
				file.Read(&Color0, sizeof(RGBQUAD));					// hat hoffentlich die Hintergrundfarbe

				switch(bmih.biBitCount)
				{
					case  4: Color0 = pCol[((*pdwCol)&0x0000000f)]; break;
					case  8: Color0 = pCol[((*pdwCol)&0x000000ff)]; break;
					case 16:
					{
						Color0.rgbRed   = (BYTE)(*pdwCol &  0x001f);
						Color0.rgbGreen = (BYTE)((*pdwCol & 0x0370)>>5);
						Color0.rgbBlue  = (BYTE)((*pdwCol & 0x7c00)>>10);
						break;
					}
					case 24: case 32:
						break;
				}
				WK_DELETE(pCol);
			}
		}
		if (Color0.rgbBlue || Color0.rgbGreen || Color0.rgbRed)	// Hintergrundfarbe aus der Bitmapdatei
		{
			sOriginal.Format(_T("%d, %d, %d"), Color0.rgbRed, Color0.rgbGreen, Color0.rgbBlue);
		}
		else																		// sonst aus der OEM Version ermitteln
		{
			CWK_Profile wkpOem;
			OEM eOem = (OEM)wkpOem.GetInt(DV_DVCLIENT, _T("Oem"),  0);
			if (eOem == OemDTS)
			{
				sOriginal =  _T("0 0 41");									// dunkelblau
			}
			else // if ((eOem == OemAlarmCom) || (eOem == OemDResearch) || (eOem == OemSantec) || (eOem == OemPOne))
			{
				sOriginal =  _T("255 255 255");							// weiß
			}
		}
		wkp.WriteString(HKCU_CONTROL_PANEL_COLORS , sKey, sOriginal);
		wkp2.WriteString(sSection , sKey, sOriginal);

		if (IsWinXP())
		{
			CWK_Profile wkpLM(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
			DWORD dwAwailableDrives = GetLogicalDrives(),
				   dwMask, i,
				   dwHide      = 0, 
					dwNoAutoRun = 0;
			_TCHAR  szDrive[]   = _T("A:");
			CString strWin = GetWindowsDirectory() + _T("\\");
			CString strPath;
			CStringArray saInstallationSources;
			for (i=0; i<32; i++)
			{
				dwMask = (1<<i);
				szDrive[0] = (_TCHAR)(_T('A')+i);
				if (dwMask & dwAwailableDrives)
				{
					switch (GetDriveType(szDrive))
					{
						case DRIVE_UNKNOWN:	// unknown
						case DRIVE_NO_ROOT_DIR:// ?
						case DRIVE_FIXED:		// HDD
						case DRIVE_REMOTE:	// Network
						case DRIVE_RAMDISK:	// RAM?
							dwHide |= dwMask;
							break;
						case DRIVE_CDROM: 
						case DRIVE_REMOVABLE:
							saInstallationSources.Add(szDrive);
							dwNoAutoRun |= dwMask;
							break;
					}
				}		
				if (szDrive[0] == _T('Z')) break;
			}
			wkpLM.WriteMultiString(HKLM_WIN_SETUP, INSTALLATION_SOURCES, saInstallationSources);

			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NODRIVES          , dwHide);			// Hides the drive
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NOVIEWONDRIVE		 , dwHide);			// Disables drive access
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NODRIVEAUTORUN    , dwNoAutoRun);	// Disables auto Run
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NOTRAYCONTEXTMENU , 1);				// Disables the context menu for Tray Icons
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NOVIEWCONTEXTMENU , 1);				// Disables the context menu in system list view (Explorer)
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NODESKTOP			 , 1);				// Disables all DesktopItems
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, DISABLETASKMGR    , 1);				// Disables the Taskmanager
			wkp.WriteInt(HKCU_SYSTEM_POLICIES  , DISABLETASKMGR    , 1);				// Disables the Taskmanager
			wkpLM.WriteString(HKLM_WINLOGON, POWERDOWNAFTERSHUTDOWN, _T("1"));				// Sets Powerdown after Shutdown Flag
			wkp.WriteInt(HKCU_DRIVERSIGNING, POLICY, 1);										// Warns for drivers without signing
			wkp.WriteInt(HKCU_EXPLORER_POLICIES, NOLOWDISKSPACECHECKS, 1);				// Disable Warning Low Disk Space

			strPath = strWin + _T("i386\\");
			if (::GetFileAttributes(strPath) == 0xffffffff)
			{
				strPath = strWin + _T("Driver Cache\\i386\\");
				if (::GetFileAttributes(strPath) == 0xffffffff)
				{
					strPath = strWin + _T("SYSTEM32\\");
				}
			}
			wkpLM.WriteString(HKLM_WIN_SETUP, SOURCEPATH, strPath);
			wkpLM.WriteString(HKLM_WIN_SETUP, DRIVERCACHEPATH, strPath);
			wkpLM.WriteString(HKLM_WIN_SETUP, SERVICEPACKSOURCEPATH, strPath);
/*
			ni386 = saInstallationSources.Add(_T(strWin + _T("i386\\")));
			saInstallationSources.Add(_T(strWin));
			saInstallationSources.Add(_T(strWin + _T("INF\\")));
			saInstallationSources.Add(_T(strWin + _T("SYSTEM32\\")));
			saInstallationSources.Add(_T(strWin + _T("SYSTEM32\\DRIVERS\\")));

			wkpLM.WriteString(HKLM_WIN_SETUP, SOURCEPATH, saInstallationSources.GetAt(ni386));
*/
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::MakeExplorerShell()
{
	if (IsWin95())
	{
		WritePrivateProfileString(SHELL_BOOT,SHELL_SHELL,_T("Explorer.exe"),SHELL_SYSTEM_INI);
	}
	else if (IsNT())
	{
		CWK_Profile wkp(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
		wkp.WriteString(szWinlogon,SHELL_SHELL,_T("Explorer.exe"));
		wkp.WriteInt(szWinlogon,AUTORESTARTSHELL, 1);
	}

	if (IsWin95())
	{
		DWORD dwAt = GetFileAttributes(szMsdos);
		SetFileAttributes(szMsdos,FILE_ATTRIBUTE_NORMAL);
		Sleep(100);
		WritePrivateProfileString(SHELL_OPTIONS,SHELL_LOGO,_T("1"),szMsdos);
		WritePrivateProfileString(SHELL_OPTIONS,SHELL_BOOTKEYS,_T("1"),szMsdos);
		WritePrivateProfileString(NULL,NULL,NULL,szMsdos);
		Sleep(100);
		SetFileAttributes(szMsdos,dwAt);
	}

	if (IsWin95())
	{
		CopyFile(SHELL_C_LOGO_000, SHELL_C_LOGO_SYS);
		CopyFile(GetWindowsDirectory() + SHELL_LOGOW_000, GetWindowsDirectory() + SHELL_LOGOW_SYS);
		CopyFile(GetWindowsDirectory() + SHELL_LOGOS_000, GetWindowsDirectory() + SHELL_LOGOS_SYS);
	}
	else if (IsNT())
	{
		CopyFile(GetWindowsDirectory() + BACKSLASH + SHELL_WINNT256_000, GetWindowsDirectory() + BACKSLASH + SHELL_WINNT256_BMP);
		CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, HKEY_CURRENT_USER,_T(""),_T(""));

		CString sKey      = WALLPAPER;
		CString sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_DESKTOP, sKey+BACKUP, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey, sOriginal);
		sKey      = BACKGROUND;
		sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_COLORS, sKey+BACKUP, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_COLORS, sKey, sOriginal);
		sKey      = WALLPAPERSTYLE;
		sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_DESKTOP, sKey+BACKUP, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey, sOriginal);
		sKey      = TILEWALLPAPER;
		sOriginal = wkp.GetString(HKCU_CONTROL_PANEL_DESKTOP, sKey+BACKUP, _T(""));
		wkp.WriteString(HKCU_CONTROL_PANEL_DESKTOP, sKey, sOriginal);
		
		if (IsWinXP())
		{
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NODRIVES);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NOVIEWONDRIVE);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NODRIVEAUTORUN);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NOTRAYCONTEXTMENU);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NOVIEWCONTEXTMENU);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NODESKTOP);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, DISABLETASKMGR);
			wkp.DeleteEntry(HKCU_SYSTEM_POLICIES  , DISABLETASKMGR);
			wkp.DeleteEntry(HKCU_EXPLORER_POLICIES, NOLOWDISKSPACECHECKS);
			// allows drivers without signing, but warns
			wkp.WriteInt(HKCU_DRIVERSIGNING, POLICY, 1);	

		}	
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::AmIShell()
{
	CString sPath;
	CString sShell;
	CWK_Profile wkp;

	GetModuleFileName(m_hInstance, sPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	sPath.ReleaseBuffer();

	if (IsWin95())
	{
		GetPrivateProfileString(SHELL_BOOT,SHELL_SHELL, _T(""), sShell.GetBuffer(MAX_PATH), _MAX_PATH, SHELL_SYSTEM_INI);
		sShell.ReleaseBuffer();
	}
	else if (IsNT())
	{
		CWK_Profile wkp(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
		sShell = wkp.GetString(szWinlogon,SHELL_SHELL,_T(""));
	}

	return (0==sPath.CompareNoCase(sShell));
}
/////////////////////////////////////////////////////////////////////////////
CString CDVStarterApp::GetWindowsDirectory()
{
	_TCHAR szPath[MAX_PATH] = {0};

	::GetWindowsDirectory(szPath, _MAX_PATH);
	return (CString)szPath;
}

/////////////////////////////////////////////////////////////////////////////
CString CDVStarterApp::GetSystemDirectory()
{
	_TCHAR szPath[MAX_PATH] = {0};
	
	::GetSystemDirectory(szPath,_MAX_PATH);
	return (CString)szPath;
}

/////////////////////////////////////////////////////////////////////////////
CString CDVStarterApp::GetTempDirectory()
{
	_TCHAR szPath[MAX_PATH] = {0};
	
	::GetTempPath(_MAX_PATH, szPath);
	CString sPath(szPath);

	int nIndex = sPath.ReverseFind(BACKSLASH);
	if (nIndex != -1)
		sPath = sPath.Left(nIndex);

	return sPath;
}

BOOL CDVStarterApp::AdjustPrivileges(LPCTSTR privilege,	DWORD dwAccess /*= SE_PRIVILEGE_ENABLED*/)
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;        // ptr. to token structure 
	
	BOOL fResult;                  // system shutdown flag 
	
	// 
	// Get the current process token handle 
	// so we can get debug privilege. 
	
	
	OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;
	
	// Get the LUID for debug privilege. 
	
	LookupPrivilegeValue(NULL, privilege, 
		&tkp.Privileges[0].Luid); 
	
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = dwAccess; 
	
	// Get shutdown privilege for this process. 
	
	fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES) NULL, 0); 
	
	
	
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::RebootAsShell()
{
	if (!AmIShell())
	{
		WK_TRACE(_T("Reboot as Shell\n"));
		MakeMeShell();
		m_bRebootAtExit = TRUE;
	}

	return m_bRebootAtExit;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::RebootNotAsShell()
{
	if (AmIShell())
	{
		WK_TRACE(_T("Reboot NOT as Shell\n"));
		MakeExplorerShell();
		m_bRebootAtExit = TRUE;
		CWK_Profile prof;
		prof.WriteInt(DV_DVSTARTER, _T("AsShell"), 0);
	}
	
	return m_bRebootAtExit;
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::EnableCtrlAltDel()	     
{
	if (IsNT())
		UnInstallKeyboardDriver();
	else if (IsWin95())
		SystemParametersInfo( SPI_SETSCREENSAVERRUNNING, 0, &m_bSystemParametersInfo, 0); 
}						  				        

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::DisableCtrlAltDel()
{
	if (IsNT())
		InstallKeyboardDriver();
	else if (IsWin95())
		SystemParametersInfo( SPI_SETSCREENSAVERRUNNING, 1, &m_bSystemParametersInfo, 0); 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::CopyFile(const CString &sFrom, const CString &sTo)
{
	BOOL bResult = FALSE;
	DWORD dwAttribute = GetFileAttributes(sTo);
	if (dwAttribute != -1)
		SetFileAttributes(sTo, FILE_ATTRIBUTE_NORMAL);
	bResult = ::CopyFile(sFrom, sTo, FALSE);
	if (dwAttribute != -1)
		SetFileAttributes(sTo, dwAttribute);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::MoveFile(const CString &sFrom, const CString &sTo)
{
	BOOL bResult = FALSE;
	DWORD dwAttribute = GetFileAttributes(sTo);
	SetFileAttributes(sTo, FILE_ATTRIBUTE_NORMAL);
	bResult = ::MoveFile(sFrom, sTo);
	SetFileAttributes(sTo, dwAttribute);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::InstallKeyboardDriver()
{
	BOOL bResult = FALSE;

    HKEY	hKey;
    DWORD	value, type, length;
	_TCHAR	filters[MAX_PATH];

   // Create the driver Registry key.
   if (RegCreateKey( HKEY_LOCAL_MACHINE, KEYHOOK_DRIVER_KEY, &hKey ) == ERROR_SUCCESS)
	{
		// Add the appropriate values.
		value = 1;
		if (RegSetValueEx( hKey, _T("Type"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value)) == ERROR_SUCCESS)
		{
			value = 1;
			if (RegSetValueEx( hKey, _T("ErrorControl"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value)) == ERROR_SUCCESS)
			{
				if (theApp.IsNT40())
				{
					value = 1;
					RegSetValueEx( hKey, _T("Start"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
					RegSetValueEx( hKey, _T("Group"), 0, REG_SZ, (const BYTE*)_T("KeyboardClass"), (_tcslen(_T("KeyboardClass"))+1)*sizeof(_TCHAR));
					RegCloseKey( hKey );
					bResult = TRUE;
				}
				else if (theApp.IsWinXP())
				{
					value = 3;
					RegSetValueEx( hKey, _T("Start"), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
					RegCloseKey(hKey);
					if (RegOpenKey(HKEY_LOCAL_MACHINE, KEYHOOK_FILTER_KEY, &hKey) == ERROR_SUCCESS)
					{
						length = sizeof( filters );
						if (RegQueryValueEx(hKey, _T("UpperFilters"), 0, &type, (BYTE*)filters, &length) == ERROR_SUCCESS)
						{
							// Append keyhook to the end.
							length /= sizeof(_TCHAR);
							if (!SearchMultiSz(filters, length/sizeof(_TCHAR), KEYHOOK))
							{
								_tcscpy(&filters[length-1], KEYHOOK);
								length = length + _tcslen(KEYHOOK);
								filters[ length ] = 0;

								if (RegSetValueEx( hKey, _T("UpperFilters"), 0, type, (const BYTE*)filters, (length + 1) * sizeof(_TCHAR)) == ERROR_SUCCESS)
									bResult = TRUE;
							}
							else // Keyhook ist schon eingetragen.
								bResult = TRUE;
						}
						RegCloseKey( hKey );
					}
				}
			}
		}
	}

	if (bResult)
		WK_TRACE(_T("Installation of KeyHook.sys successful\n"));
	else
	{
		WK_TRACE(_T("Installation of KeyHook.sys failed\n"));
		UnInstallKeyboardDriver();	// Alle evtl. gemachten registry einträge wieder löschen
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::UnInstallKeyboardDriver()
{
	BOOL	bResult = FALSE;
    HKEY	hKey;
	_TCHAR	filters[MAX_PATH], *ptr = {0};
	DWORD	type, length;

    // Delete the driver Registry key.
	RegDeleteKey( HKEY_LOCAL_MACHINE, CString(KEYHOOK_DRIVER_KEY) + _T("\\Security"));
	RegDeleteKey( HKEY_LOCAL_MACHINE, CString(KEYHOOK_DRIVER_KEY) + _T("\\Enum"));
	RegDeleteKey( HKEY_LOCAL_MACHINE, KEYHOOK_DRIVER_KEY);

	if (theApp.IsNT40())
		bResult = TRUE;	// 
	else if (theApp.IsWinXP())	// Unter WinNT5 sollte der Filtertreiber ausgetragen werden.
	{
  		if (RegOpenKey( HKEY_LOCAL_MACHINE,	KEYHOOK_FILTER_KEY, &hKey ) == ERROR_SUCCESS)
		{
			length = sizeof( filters );
			if (RegQueryValueEx(hKey, _T("UpperFilters"), 0, &type, (BYTE*)filters, &length ) == ERROR_SUCCESS)
			{
				length /= sizeof(_TCHAR);
				ptr = SearchMultiSz(filters, length, KEYHOOK);
				if (ptr)
				{
					// Zap it.
					memcpy( ptr, ptr + _tcslen(KEYHOOK)+1, (length - (ptr-filters) - _tcslen(KEYHOOK) -1) * sizeof(_TCHAR));
					length -= _tcslen(KEYHOOK)+1;

					if (RegSetValueEx(hKey, _T("UpperFilters"), 0, type, (const BYTE*)filters, length * sizeof(_TCHAR)) == ERROR_SUCCESS)
						bResult = TRUE;
				}
				else
					bResult = TRUE;
			}
			RegCloseKey( hKey );
		}
	}

	if (bResult)
		WK_TRACE(_T("Uninstall KeyHook successful\n"));
	else
		WK_TRACE(_T("Uninstall KeyHook failed\n"));

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::EnableAutologin()
{
	CWK_Profile wkProfDV;

	// Autologin Einstellungen laden
	CString sUser		= wkProfDV.GetString(DV_DVSTARTER_AUTO_LOGIN, _T("User"), _T(""));
	CString sDomain	= wkProfDV.GetString(DV_DVSTARTER_AUTO_LOGIN, _T("Domain"), _T(""));
	CString sPassword	= wkProfDV.GetString(DV_DVSTARTER_AUTO_LOGIN, _T("Password"), _T(""));

	// Defaultwerte setzen
	wkProfDV.WriteString(DV_DVSTARTER_AUTO_LOGIN, _T("User"), sUser);
	wkProfDV.WriteString(DV_DVSTARTER_AUTO_LOGIN, _T("Domain"), sDomain);
	wkProfDV.WriteString(DV_DVSTARTER_AUTO_LOGIN, _T("Password"), sPassword);

	if (IsNT())
	{
		CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
		if (!sUser.IsEmpty() && !sPassword.IsEmpty())
		{
			wkProf.WriteString(szWinlogon,_T("AutoAdminLogon"), _T("1"));
			wkProf.WriteString(szWinlogon,_T("DefaultUserName"), sUser);
			wkProf.WriteString(szWinlogon,_T("DefaultDomainName"), sDomain);
			wkProf.WriteString(szWinlogon,_T("DefaultPassword"), sPassword);
		}
		else
			wkProf.WriteString(szWinlogon,_T("AutoAdminLogon"), _T("0"));
	}
	else
	{
		CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON,NULL,szLogon95,_T(""));
		if (!sUser.IsEmpty() && !sPassword.IsEmpty())
		{
			wkProf.WriteString(szWinlogon,_T("AutoAdminLogon"), _T("1"));
			wkProf.WriteString(szWinlogon,_T("DefaultUserName"), sUser);
			wkProf.WriteString(szWinlogon,_T("DefaultPassword"), sPassword);
			wkProf.WriteString(szWinlogon,_T("DontDisplayLastUserName"), _T("0"));
		}
		else
			wkProf.WriteString(szWinlogon,_T("AutoAdminLogon"), NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::DisableAutologin()
{
	CWK_Profile wkProfDV;

	if (IsNT())
	{
		CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
		wkProf.WriteString(szWinlogon,_T("AutoAdminLogon"),		_T("0"));
		wkProf.WriteString(szWinlogon,_T("DefaultUserName"),	_T(""));
		wkProf.WriteString(szWinlogon,_T("DefaultDomainName"),	_T(""));
		wkProf.WriteString(szWinlogon,_T("DefaultPassword"),	_T(""));
	}
	else
	{
		CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON,NULL,szLogon95,_T(""));
		wkProf.WriteString(szWinlogon,_T("AutoAdminLogon"), _T("0"));
		wkProf.WriteString(szWinlogon,_T("DefaultUserName"), _T(""));
		wkProf.WriteString(szWinlogon,_T("DefaultPassword"), _T(""));
		wkProf.WriteString(szWinlogon,_T("DontDisplayLastUserName"), NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CDVStarterApp::GetOEMString()
{
	
	HKEY	hKey	= NULL;
	DWORD	dwType	= 0;
	DWORD	dwLen	= 0;
	LONG	lResult	= 0;
	CString sValue	= _T("");
	CString	sKey	= _T("");
	CString sEntry	= _T("ProductId");

	if (IsNT())
		sKey	= _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	else if (IsWin95())
		sKey	= _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion");

	else
		WK_TRACE_ERROR(_T("Unknown Operating system\n"));

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, sKey, 0, KEY_QUERY_VALUE, &hKey);
	if(lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueEx(hKey, sEntry, NULL, &dwType, NULL, &dwLen);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(hKey, sEntry, NULL, &dwType, (LPBYTE)sValue.GetBuffer(dwLen/sizeof(TCHAR)),&dwLen);
			sValue.ReleaseBuffer();
		}
	}
	
	RegCloseKey(hKey);

	return sValue;
}

/////////////////////////////////////////////////////////////////////////////
CString CDVStarterApp::GetOriginalFileName(const CString &sFileName)
{
	// first try in current directory
	CString sOriginalName = _T("");
	CString sFile = sFileName;

	struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	if (DoesFileExist(sFile))
	{
		const	int MAX_INFO_LEN=5000;
		BYTE	pData[MAX_INFO_LEN+1];
		_TCHAR  sValue[1000];
		void	 *pValueData=NULL;
		_TCHAR *pUnconst = sFile.GetBuffer(0);
		UINT	dwValueLen;
		DWORD	dwDummy;
		DWORD	dwLen = GetFileVersionInfoSize(pUnconst,&dwDummy);

		if (dwLen > MAX_INFO_LEN)
		{
			WK_TRACE_ERROR(_T("CDVStarterApp::GetOriginalFileName internal overflow %d !?\n"),dwLen);
			sFile.ReleaseBuffer();
			return _T(""); // <<<EXIT >>>
		}

		BOOL bOkay = GetFileVersionInfo(pUnconst, 0, MAX_INFO_LEN, pData);	
		sFile.ReleaseBuffer();

		if (bOkay)
		{
			if (VerQueryValue(pData, TEXT("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate, &dwValueLen))
			{
				for (DWORD dwI=0; dwI < (dwValueLen/(DWORD)sizeof(struct LANGANDCODEPAGE)); dwI++)
				{
					wsprintf(sValue, TEXT("\\StringFileInfo\\%04x%04x\\OriginalFilename"),
								lpTranslate[dwI].wLanguage,
								lpTranslate[dwI].wCodePage);
					if (VerQueryValue(pData,			// address of buffer for version resource
								sValue,			// address of value to retrieve
								&pValueData,	// address of buffer for version pointer
								&dwValueLen))		// address of version-value length buffer
					{
						sOriginalName = (const char*)pValueData;
						break;
					}
				}
			}
		}
	}
	sOriginalName.MakeLower();

	return sOriginalName;
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::ShowInfoText(const CString &sText)
{
	if (m_pInfoDlg)
	{
		m_pInfoDlg->ShowWindow(SW_SHOW);
		m_pInfoDlg->ShowInfoText(sText);
		m_pInfoDlg->UpdateWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::HideInfoText()
{
	if (m_pInfoDlg)
	{
		m_pInfoDlg->ShowWindow(SW_HIDE);
		m_pInfoDlg->UpdateWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::DoUpdate()
{
	BOOL bResult = FALSE;
	CWK_Profile wkp;
	CString sTempDir;
	CString sSetupFile;
 
	::GetTempPath(_MAX_PATH,sTempDir.GetBuffer(_MAX_PATH));
	sTempDir.ReleaseBuffer();

	sSetupFile = sTempDir + _T("Setup.exe");
	
	if (DoesFileExist(sSetupFile))
	{
		WK_TRACE(_T("Start update from %s\n"), sSetupFile);
		MessageBeep(0);
		CWK_String str = sTempDir+_T("Setup.exe -auto");
		if (WinExec(str, SW_HIDE) > 31)
		{
			wkp.WriteInt(DV_DVSTARTER, _T("DoUpdate"), 0);
			bResult = TRUE;
		}
	}

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::IsReceiver()
{
	BOOL bResult = FALSE;

	if (m_pDongle)
		bResult = (m_pDongle->GetProductCode() == IPC_DTS_IP_RECEIVER);

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::UsePiezo()
{
	return m_bUsePiezo;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::IsTransmitter()
{
	BOOL bResult = FALSE;

	if (m_pDongle)
		bResult = (m_pDongle->GetProductCode() == IPC_DTS);

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::OnPiezoUseChanged()
{
	CWK_Profile prof;
	m_bUsePiezo = prof.GetInt(DV_DVSTARTER, _T("UsePiezo"), m_bUsePiezo);
}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::Rdisk(BOOL bWait)
{
	BOOL				bRet;
	CStartupInfo		StartUpInfo;
	CProcessInformation ProzessInformation;
	CString sSystem;
	CString sCommandline,sExe;

	::GetSystemDirectory(sSystem.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sSystem.ReleaseBuffer();

	sExe = sSystem + _T("\\rdisk.exe");
	sCommandline = sExe + _T(" /s-");
	StartUpInfo.dwFlags = STARTF_USESHOWWINDOW|STARTF_USEPOSITION;
	StartUpInfo.wShowWindow = SW_HIDE;
	StartUpInfo.dwX = 0;
	StartUpInfo.dwY = 0;
	StartUpInfo.dwXSize = 0;
	StartUpInfo.dwYSize = 0;

	bRet = CreateProcess(sExe,	// pointer to name of executable module 
					 sCommandline.GetBuffer(0),	// pointer to command line string
					 (LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
					 (LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
					 FALSE,	// handle inheritance flag 
					 IDLE_PRIORITY_CLASS,//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
					 (LPVOID)NULL,	// pointer to new environment block 
					 (LPCTSTR)sSystem,	// pointer to current directory name 
					 (LPSTARTUPINFO)&StartUpInfo,	// pointer to STARTUPINFO 
					 (LPPROCESS_INFORMATION)&ProzessInformation 	// pointer to PROCESS_INFORMATION  
							);
	
	if (bRet)
	{
		WK_TRACE(_T("rdisk started\n"));
	}
	else
	{
		WK_TRACE(_T("cannot start rdisk %s\n"),GetLastErrorString());
	}
	sCommandline.ReleaseBuffer();

	if (bRet && bWait)
	{
		DWORD dwTick = GetTickCount();
		WaitForInputIdle(ProzessInformation.hProcess,INFINITE);
		WK_TRACE(_T("WaitForInputIdle rdisk %d ms\n"),GetTimeSpan(dwTick));
		WaitForSingleObject(ProzessInformation.hProcess,INFINITE);
		WK_TRACE(_T("WaitForProcess rdisk %d ms\n"),GetTimeSpan(dwTick));
	}
}

//----------------------------------------------------------------------
//
// SearchMultiSz
//
// Finds the specified string in a multiSz.
//
//----------------------------------------------------------------------
_TCHAR* CDVStarterApp::SearchMultiSz(_TCHAR *Value, DWORD ValueLength, _TCHAR *String)
{
	DWORD	len;

	if (ValueLength < _tcslen(String))
		return NULL;

	len = ValueLength - _tcslen(String);
	do
	{
		if (!_tcsicmp( &Value[len], String))
			return &Value[len];
	} while(len--);

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::SetInstallationSourcePaths()
{
	CWK_Profile wkpLM(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
	DWORD dwMask, i, dwAwailableDrives = GetLogicalDrives();
	_TCHAR  szDrive[]   = _T("A:");
	CStringArray saInstallationSources;
	for (i=0; i<32; i++)
	{
		dwMask = (1<<i);
		szDrive[0] = (_TCHAR)(_T('A')+i);
		if (dwMask & dwAwailableDrives)
		{
			switch (GetDriveType(szDrive))
			{
				case DRIVE_CDROM: 
				case DRIVE_REMOVABLE:
					saInstallationSources.Add(szDrive);
					break;
			}
		}		
		if (szDrive[0] == _T('Z')) break;
	}
	wkpLM.WriteMultiString(HKLM_WIN_SETUP, INSTALLATION_SOURCES, saInstallationSources);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterApp::InitializeDrive(int nDrive)
{
	BOOL bResult = FALSE;

	CDrive Drive(nDrive);

	// Diskgeometrie informationejn holen und ausgeben.
	DISK_GEOMETRY_EX DiskGeometry;
	if (Drive.GetDriveGeometry(DiskGeometry))
		Drive.TraceDriveGeometry(DiskGeometry);

	// Partitionsinformationen holen und ausgeben
	Drive.TraceAllPartitionInfos();

	// Partitioniere laufwerk mit einer Partition.
	bResult = Drive.PartitionDrive();

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::FormatAllUnformatedFixedVolumes()
{
	TCHAR Volumes[100];

	DWORD dwSysFlags;            // flags that describe the file system
	TCHAR FileSysNameBuf[FILESYSNAMEBUFSIZE];
	GetLogicalDriveStrings(sizeof(Volumes), Volumes);
	
	int nI		= 0;
	int nVolume = 0;

	while (Volumes[nI] != '\0')
	{
		CString sVolume(&Volumes[nI+=4]);
		if (!sVolume.IsEmpty())
		{
			if (GetDriveType(sVolume) == DRIVE_FIXED)
			{
				// Hat diese Volume ein gültiges Filesystem?
				if (GetVolumeInformation(sVolume, NULL, 0, NULL, NULL, &dwSysFlags, FileSysNameBuf, FILESYSNAMEBUFSIZE))
					WK_TRACE(_T("Volume:%d Volumename:%s Filesystem=%s\n"), nVolume++, sVolume, FileSysNameBuf);
				else
				{
					if (GetLastError() == ERROR_UNRECOGNIZED_VOLUME)
					{
						WK_TRACE(_T("Volume: %s has no valid filesystem\n"), sVolume);
						CDrive::Format(sVolume);
					}
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::Reboot()
{
	m_bRebootAtExit = TRUE;
	m_pMainWnd->PostMessage(DVSTARTER_EXIT, 0, 0);
}
///////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::AddApplication(CApplication*pApp)
{
	BOOL bFound = FALSE;
	int i, nc = m_Applications.GetSize();
	for (i=0; i<nc; i++)
	{
		if (m_Applications.GetAtFast(i) == pApp)
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		m_Applications.Add(pApp);
	}
}
///////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::RemoveApplication(CApplication*pApp)
{
	int i, nc = m_Applications.GetSize();
	for (i=0; i<nc; i++)
	{
		if (m_Applications.GetAtFast(i) == pApp)
		{
			m_Applications.RemoveAt(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CDVStarterApp::GetNextAvailableFixedDrive()
{
	CString sRet;
	DWORD dwAwailableDrives = GetLogicalDrives();
	DWORD dwMask, i;
	_TCHAR  szDrive[]   = _T("A:");
	CString strPath;
	for (i=0; i<32; i++)
	{
		dwMask = (1<<i);
		szDrive[0] = (_TCHAR)(_T('A')+i);
		if (dwMask & dwAwailableDrives)
		{
			switch (GetDriveType(szDrive))
			{
			case DRIVE_FIXED:		// HDD
				sRet.Format(_T("%s"), szDrive);
				break;
			}
			if(!sRet.IsEmpty())
			{
				break;
			}
		}		
		if (szDrive[0] == _T('Z')) break;
	}

	return sRet;
}
//////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CDVStarterApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	if (uCodePage)
	{
		InitVideteLocaleLanguage();
		CWK_String::SetCodePage(uCodePage);
		CWK_Profile wkp;
		wkp.SetCodePage(CWK_String::GetCodePage());
	}

	return 0;
}
#endif