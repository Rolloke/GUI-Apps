// SystemInfo.cpp: implementation of the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemInfo.h"
#include "DVProcess.h"
#include "winioctl.h" //for counting hard drives

extern CDVProcessApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemInfo::CSystemInfo()
{

}

CSystemInfo::~CSystemInfo()
{

}

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetSystemSerialNumber 
 Description   : This function returns the serial number of the system (normaly the computer name) 

 Parameters: -- 

 Result type:  returns the serial number of the system (computer name)  (CString) 

 Author: TKR
 created: May, 07 2003
 <TITLE GetSystemSerialNumber >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetSystemSerialNumber>
*********************************************************************************************/
CString CSystemInfo::GetSystemSerialNumber()
{
	CString sSerial(_T("no serial found"));

	CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
	CString sComputerName = prof.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER, _T(""));
	if (sComputerName.IsEmpty())
	{
		sComputerName = prof.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME, _T(""));
	}
	if (!sComputerName.IsEmpty())
	{
		sSerial = sComputerName;
	}
	return sSerial;
}

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetOSType 
 Description   : This function returns the OS 

 Parameters: -- 

 Result type:  returns the Operation System type  (CString) 

 Author: TKR
 created: May, 07 2003
 <TITLE GetOSType >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetOSType>
*********************************************************************************************/
CString CSystemInfo::GetOSVersion()
{
	CString sOsRet(_T("no system"));

	OSVERSIONINFO	osinfo;
	CString			sOSVersion;

	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);
	
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{		
		CString sNr;
		CString sBuild;

		if(osinfo.dwMajorVersion  > 4)
		{
			sNr = _T("WIN XP v");

			if(osinfo.dwMinorVersion == 0)
			{
				sNr = _T("WIN 2000 v");
			}
		}
		else
		{
			sNr = _T("WIN NT v");
		}

		sBuild.Format(_T(" Build: %d"), osinfo.dwBuildNumber);
		sOsRet.Format(_T("%s%d.%d %s"), sNr, 
								 osinfo.dwMajorVersion,
								 osinfo.dwMinorVersion,
								 sBuild);

	}
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if(osinfo.dwMinorVersion == 0)  //Win 95
		{
			sOsRet = _T("WIN 95");
		}
		
		if(osinfo.dwMinorVersion > 0)
		{
			if(LOWORD(osinfo.dwBuildNumber) >= 3000)
			{
				sOsRet = _T("WIN ME");
			}
			else
			{
				sOsRet = _T("WIN 98");
			}
		}
	}
	return sOsRet;
}		

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetAvailableOptions 
 Description   : This function returns all available options

 Parameters: 
    (I): bNet:			if TRUE, Network is available (BOOL)
    (I): bISDN:			if TRUE, ISDN is available (BOOL)
    (I): bBackup:		if TRUE, Backup to CD only is available (BOOL)
    (I): bBackupDVD:	if TRUE, Backup to CD and DVD is available (BOOL)

 Result type:  --

 Author: TKR
 created: May, 07 2003
 <TITLE GetAvailableOptions >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetAvailableOptions>
*********************************************************************************************/
void CSystemInfo::GetAvailableOptions(BOOL &bLAN, BOOL &bISDN, BOOL &bBackup, BOOL &bBackupDVD)
{
	CWK_Profile prof;

	// Ist Backup auf DVD, CD, Memorystick, etc... im Dongle freigeschaltet
	if (WK_IS_RUNNING(WK_APP_NAME_JACOBUNIT1))
	{
		bLAN		= prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("Net"), bLAN);
		bISDN		= prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("ISDN"), bISDN);
		bBackup		= prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("BackUp"), bBackup);
		bBackupDVD	= prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("BackUpToDVD"), bBackupDVD);
	}
	else if (WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT1))
	{
		bLAN		= prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("Net"), bLAN);
		bISDN		= prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("ISDN"), bISDN);
		bBackup		= prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("BackUp"), bBackup);
		bBackupDVD	= prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("BackUpToDVD"), bBackupDVD);
	}
	else if (WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT1))
	{
		bLAN		= prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("Net"), bLAN);
		bISDN		= prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("ISDN"), bISDN);
		bBackup		= prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("BackUp"), bBackup);
		bBackupDVD	= prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("BackUpToDVD"), bBackupDVD);
	}
	else if (WK_IS_RUNNING(WK_APP_NAME_QUNIT))
	{
		//TODO ML QUnit not yet saves the dongle information in the eeprom, maybe later (05.07.06)
		bLAN		= prof.GetInt(_T("QUnit\\EEProm"), _T("Net"), bLAN);
		bISDN		= prof.GetInt(_T("QUnit\\EEProm"), _T("ISDN"), bISDN);
		bBackup		= prof.GetInt(_T("QUnit\\EEProm"), _T("BackUp"), bBackup);
		bBackupDVD	= prof.GetInt(_T("QUnit\\EEProm"), _T("BackUpToDVD"), bBackupDVD);
	}

	// Wenn das Backup auf DVD erlaubt ist, soll auf jedenfall auch das Backup auf CD erlaubt sein! 
	if (bBackupDVD)
		bBackup = TRUE;
}
/*********************************************************************************************
Class		   : CSystemInfo
Function      : GetAvailableQCams 
Description   : This function returns the number of available cameras from a QUnit

Parameters: -- 


Result type:  returns the number of available Q Unit cameras  (int) 

Author: TKR
created: Jul, 07 2006
<TITLE GetAvailableQCams >
<GROUP CSystemInfo>
<TOPICORDER 0>
<KEYWORDS CSystemInfo, GetAvailableQCams>
*********************************************************************************************/
int CSystemInfo::GetAvailableQCams()
{
	int iNrOfQCams = 4;
	CWK_Profile wkp;
	iNrOfQCams = wkp.GetInt(_T("DV\\Process"),_T("NrOfCameras"),iNrOfQCams);
	return iNrOfQCams;
}
/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetBurnSoftware 
 Description   : This function returns the currently installed burn software type
				 Types: 		
					BURN_PACKETCD = 0,
					BURN_DIRECTCD = 1,
					BURN_NERO	  = 2,

 Parameters: 
    (E): sVersionNr:	the version number of installed burn software (CString)


 Result type:  returns the installed burn software type  (CString) 

 Author: TKR
 created: May, 07 2003
 <TITLE GetBurnSoftware >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetBurnSoftware>
*********************************************************************************************/
CString CSystemInfo::GetBurnSoftware(CString &sVersionNr)
{	
	CString sBurnSoftware(_T("no burn software installed")); 
	CString sDefault(_T("default"));
	CString sPacketCD, sDirectCD, sNero;

	sVersionNr.Empty();
	
	//prüfen ob PacketCD installiert ist
	CWK_Profile profPacketCD(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""),_T(""));
	sPacketCD = profPacketCD.GetString(_T("Software\\MicroSoft\\Windows\\CurrentVersion\\Run"),_T("PacketCD"),sDefault);

	//Prüfen, ob DirectCD installiert ist
	CWK_Profile profDirectCD;
	sDirectCD = profDirectCD.GetString(_T("DV\\DVClient"), _T("DirectCDPath"), sDefault);


	if(sPacketCD != sDefault)
	{
		sBurnSoftware = PACKETCD;
	}

	else if(sDirectCD != sDefault)
	{
		sBurnSoftware = DIRECTCD;
	}
	
	else
	{
		//first check if Nero is installed on the system
		if(IsNeroInstalled())
		{
			CString sNeroVersion;
			BOOL bVersion = GetNeroVersion(sNeroVersion);//installed Nero version

			sBurnSoftware = NERO;

			//has to be at least version 5.5.8.2
			if(bVersion)
			{
				sVersionNr = sNeroVersion;
			}
			else
			{
				sVersionNr = _T("invalid Nero version, must be at least version 5.5.8.2.");
			}		
		}
	}
	return sBurnSoftware;
}

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : IsNeroInstalled 
 Description   : This function checks if Nero burn software is installed.

 Parameters: --

 Result type:  return TRUE if Nero is installed (BOOL)

 Author: TKR
 created: May, 08 2003
 <TITLE IsNeroInstalled >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CProducer, IsNeroInstalled>
*********************************************************************************************/
BOOL CSystemInfo::IsNeroInstalled()
{
	BOOL bRet = FALSE;
	CString sNeroPath;
	CString sNeroApi(_T("NeroApi.dll"));
	CString sNeroExe(_T("Nero.exe"));
	CString sNeroApiVersion;

	// open registry path "SOFTWARE\\Ahead\\Shared", find key "NeroAPI" 
	//this key holds the installation path of Nero.exe		
	CFile file;
	HKEY namekey;
	CString sStrValue;
	LONG lResult;
	DWORD dwType, dwCount;

	//get Nero installation path from Nero`s registry values
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
							_T("SOFTWARE\\Ahead\\Shared"),
							0,
							KEY_QUERY_VALUE,
							&namekey);
	if(lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueEx(namekey, 
								_T("NeroApi"), 
								NULL, 
								&dwType, 
								NULL,
								&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(namekey, 
									  _T("NeroApi"), 
									  NULL, 
									  &dwType,
									  (LPBYTE)sStrValue.GetBuffer(dwCount/sizeof(TCHAR)), 
									  &dwCount);
			//sStrValue holds the path to the NeroApi file

		}
		RegCloseKey(namekey);

		sNeroPath.Format(_T("%s"), sStrValue);
		sStrValue.ReleaseBuffer();
		
		if(lResult == ERROR_SUCCESS)
		{
			//nero registry path not found (version to old), nero installation path is unknown
			//search for Nero.exe and get version from Nero.exe
			if(!sNeroPath.IsEmpty())
			{
				if(sNeroPath.Right(3) == _T("exe"))
				{
					//find Nero.exe in folder from registry
					//is Nero Version smaller than 5.5.8.2 the NeroAPI.dll is included in the Nero.exe
					//so get the file version from the Nero.exe
					if(file.Open(sNeroPath, CFile::modeRead))
					{
						file.Close();
						m_sNeroVersion = WK_GetFileVersion(sNeroPath);
						bRet = TRUE;
					}
				}
				else
				{
					sNeroPath = sNeroPath + _T("\\") + sNeroApi;
					//find NeroAPI.dll in folder from registry 
					//is Nero Version equal than or greater 5.5.8.2 get version from NeroAPI.dll 
					if(file.Open(sNeroPath, CFile::modeRead))
					{
						file.Close();
						m_sNeroVersion = WK_GetFileVersion(sNeroPath);
						bRet = TRUE;
					}
				}
			}
		}
	}
	return bRet;
}

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetNeroVersion 
 Description   : This function returns the software version number (e.g.: 2.1.0.568)
				 Types: 		
					BURN_PACKETCD = 0,
					BURN_DIRECTCD = 1,
					BURN_NERO	  = 2,

 Parameters: 
	(O): Nero`s version number (CString)

 Result type:  if TRUE, the installed Nero version is valid (BOOL) 

 Author: TKR
 created: May, 08 2003
 <TITLE GetNeroVersion >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetNeroVersion>
*********************************************************************************************/
BOOL CSystemInfo::GetNeroVersion(CString &sVersion)
{
	BOOL bRet = FALSE;
	CString s = m_sNeroVersion;
	DWORD dw1, dw2, dw3, dw4;

	dw1 = dw2 = dw3 = dw4 = 0;

	//Nero muss mind. Version 5.5.8.2 sein
	_stscanf(s,_T("%d,%d,%d,%d"),&dw1,&dw2,&dw3,&dw4);

	if(dw1 == 5)
	{
		if(dw2 == 5)
		{
			if(dw3 == 8)
			{
				if(dw4 >= 2)
				{
					bRet = TRUE;
				}
			}
			else if(dw3 > 8)
			{
				bRet = TRUE;
			}
		}
		else if(dw2 > 5)
		{
			bRet = TRUE;
		}
	}
	else if(dw1 > 5)
	{
		bRet = TRUE;
	}
	
	s.Format(_T("%d.%d.%d.%d"), dw1, dw2, dw3, dw4);
	sVersion = s;
	return bRet;
}
/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetSoftwareVersion 
 Description   : This function returns the software version number (e.g.: 2.1.0.568)
				 Types: 		
					BURN_PACKETCD = 0,
					BURN_DIRECTCD = 1,
					BURN_NERO	  = 2,

 Parameters: --

 Result type:  the installed software version number (CString) 

 Author: TKR
 created: May, 07 2003
 <TITLE GetSoftwareVersion >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetSoftwareVersion>
*********************************************************************************************/
CString CSystemInfo::GetDTSSoftwareVersion()
{
	CString sSoftwareVersion;

	return sSoftwareVersion;
}

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetInfoHarddisks
 Description   : This function returns the whole diskspace in megabyte of all 
			     installed harddrives

 Parameters: --

 Result type:  the whole diskspace in megabyte of all installed harddrives (CString) 

 Author: TKR
 created: May, 09 2003
 <TITLE GetInfoHarddisks >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetInfoHarddisks>
*********************************************************************************************/
CString CSystemInfo::GetInfoHarddisks()
{
	CString sWholeDiskSpace(_T("0"));
	int index=0;
	HANDLE hDevice;
	CString HardDisk;
	DWORD junk;
	DISK_GEOMETRY pdg;
	ULONGLONG ullWholeDiskSpace = 0;
	CString sSizeInGB;

	HardDisk.Format(_T("\\\\.\\PhysicalDrive%d"),index);

	hDevice=CreateFile(HardDisk,0,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,0,NULL);

	if (hDevice==INVALID_HANDLE_VALUE)
		sSizeInGB==_T("Error: Can not Query Hard Disk Drives.");

	while (hDevice!=INVALID_HANDLE_VALUE)
	{
		BOOL bResult=DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY,
									NULL,0,
									&pdg,sizeof(DISK_GEOMETRY),
									&junk,
									(LPOVERLAPPED) NULL);
		if (bResult)
		{
			ULONGLONG DiskSize=pdg.Cylinders.QuadPart * (ULONG) pdg.TracksPerCylinder * 
				(ULONG) pdg.SectorsPerTrack * (ULONG) pdg.BytesPerSector;

			ullWholeDiskSpace += DiskSize;
			

		}

		sSizeInGB.Format(_T("%I64d GB"),ullWholeDiskSpace/1000/1000/1000);
		
		CloseHandle(hDevice);

		index++;
		HardDisk.Format(_T("\\\\.\\PhysicalDrive%d"),index);
		hDevice=CreateFile(HardDisk,0,FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,0,NULL);
	}

	sWholeDiskSpace = sSizeInGB;
 
	return sWholeDiskSpace;
}

/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetOEMVersion 
 Description   : This function returns the oem version of the installed software

 Parameters: --

 Result type:  oem version (CString) 

 Author: TKR
 created: May, 07 2003
 <TITLE GetVersionInfo >
 <GROUP GetOEMVersion>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetOEMVersion>
*********************************************************************************************/
CString CSystemInfo::GetOEMVersion()
{
	CString sOEMName;

	// OEM Spezifische Einstellungen
	OEM eOem = OemAlarmCom;
	int nNrOfCameras = 8;

	CWK_Profile prof;

 	// Welcher OEM?
	eOem = (OEM)prof.GetInt(_T("DV\\DVClient"), _T("Oem"),  (int)OemDTS);
	nNrOfCameras = prof.GetInt(_T("DV\\Process"), _T("NrOfCameras"), 4);

	// hole Versionsnummer von DVProcess
	CString sOEM, sVersion;
	GetModuleFileName(theApp.m_hInstance,sOEM.GetBuffer(_MAX_PATH),_MAX_PATH);
	sOEM.ReleaseBuffer();
	int p = sOEM.ReverseFind(_T('\\'));
	sOEM = sOEM.Left(p+1) + _T("\\DVProcess.exe");
	sVersion = WK_GetFileVersion(sOEM);

	// OEM Spezifische Einstellungen
	CString sTemp, sVersionText;
//	sVersionText.LoadString(IDS_VERSION);
	if (eOem == OemDTS)
	{
		sTemp = _T("videte IT® DTS %d00 ") + sVersionText + _T(" %s");
		sOEMName.Format(sTemp, nNrOfCameras, sVersion);
	}
	else if (eOem == OemAlarmCom)
	{
		sOEMName = _T("Siemens VDR ") + sVersionText + _T(" ") + sVersion;
	}
	else if (eOem == OemPOne)
	{
		sOEMName = _T("Protection One VDR ")+ sVersionText + _T(" ") + sVersion;
	}
	else if (eOem == OemDResearch)
	{
		sTemp = _T("DResearch Vidia 500 - %d ") + sVersionText + _T(" %s");
		sOEMName.Format(sTemp, nNrOfCameras, sVersion);
	}
	else if (eOem == OemSantec)
	{
		sTemp = _T("Santec SDR-%d ") + sVersionText + _T(" %s");
		sOEMName.Format(sTemp, nNrOfCameras, sVersion);
	}
	return sOEMName;
}
/*********************************************************************************************
 Class		   : CSystemInfo
 Function      : GetSystemInformations 
 Description   : This function returns all system infos	 

 Parameters: --

 Result type:  all system infos (CString) 

 Author: TKR
 created: May, 07 2003
 <TITLE GetSystemInformations >
 <GROUP CSystemInfo>
 <TOPICORDER 0>
 <KEYWORDS CSystemInfo, GetSystemInformations>
*********************************************************************************************/
CString CSystemInfo::GetSystemInformations()
{
	CString sRet, sDummy;
	CString sBurnSoftwareVersionNr;
	CString sSerial, sBurnSoftware, sOsType, sOptions, sOEM, sDisk;
	BOOL bLAN, bISDN, bCD, bDVD;
	bLAN = bISDN = bCD = bDVD = FALSE;


	
	//get system serial number (computer name)
	sSerial.Format(_T("Serial:%s"), GetSystemSerialNumber());
	
	//get installed burn software
	sDummy	= GetBurnSoftware(sBurnSoftwareVersionNr);
	sBurnSoftware.Format(_T("Backup Software:%s"),sDummy);
	if(!sBurnSoftwareVersionNr.IsEmpty())
	{
		sBurnSoftware = sBurnSoftware + _T(" (") + sBurnSoftwareVersionNr + _T(")");	
	}


	//***** get OS type *****
	sOsType.Format(_T("Operating system:%s"),GetOSVersion());


	//***** get activated options *****
	GetAvailableOptions(bLAN, bISDN, bCD, bDVD);



	CString sLAN, sISDN, sCD, sDVD;
	sLAN		= _T("LAN");
	sISDN		= _T("ISDN");
	sCD			= _T("CD");
	sDVD		= _T("DVD");

	//is DirectCD or PacketCD installed, option _T("CD") is available
	if(sDummy == DIRECTCD || sDummy == PACKETCD)
	{
		bCD = TRUE;
	}

	sDummy.Empty();
	sOptions = _T("Options:");

	bLAN  ? sDummy = sLAN				  : sDummy;
	bISDN ? sDummy = sDummy + _T(",") + sISDN : sDummy;
	bCD	  ? sDummy = sDummy + _T(",") + sCD	  : sDummy;
	bDVD  ? sDummy = sDummy + _T(",") + sDVD  : sDummy;


	CBoards boards;
	CString sNrOfQCams;
	int iNrOfQCams = 4; //minimum nr of QUnit cameras
	BOOL bQUnit = boards.GetNumbersOfQs() > 0;
	if(bQUnit)
	{
		iNrOfQCams = GetAvailableQCams();
		sNrOfQCams.Format(_T("(%d Cam.)"), iNrOfQCams); 
		sDummy = sDummy + _T(",") + sNrOfQCams;
	}

	sDummy.TrimLeft(_T(','));

	//no options available
	if(sDummy.IsEmpty())
	{
		sDummy = _T("%");
	}
	sOptions.Format(_T("Options:%s"), sDummy);
	sDummy.Empty();


	//***** get DTS version/OEM number *****
	sDummy = GetOEMVersion();
	sOEM.Format(_T("Version:%s"),sDummy);
	sDummy.Empty();
	

	//***** get hard disk informations *****
	sDummy = GetInfoHarddisks();
	sDisk.Format(_T("Disk space:%s"), sDummy);
	
	sRet.Format(_T("%s;%s;%s;%s;%s;%s"), 
		sSerial, sBurnSoftware, sOsType, sOptions, sOEM, sDisk);

	return sRet;
}