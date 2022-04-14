// IdipCleanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vipcleannt.h"
#include "IdipCleanDlg.h"
#include "CleanNTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _IDIP


#define WM_UPDATE_WND  WM_USER +1

/////////////////////////////////////////////////////////////////////////////
// CIdipCleanDlg dialog
CIdipCleanDlg::CIdipCleanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIdipCleanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdipCleanDlg)
	m_sBuild = _T("");
	m_sOS = _T("");
	m_sSR = _T("");
	m_sDrives = _T("");
	m_sScreen = _T("");
	m_sAlleArchive = _T("");
	m_sTemp = _T("");
	m_sInfoDelDir = _T("");
	m_sInfoNotDelDir = _T("");
	m_sZone = _T("");
	m_sTime = _T("");
	m_sDate = _T("");
	m_sHz = _T("");
	//}}AFX_DATA_INIT

	m_pNTDlg				= new CCleanNTDlg(this);
	m_StartTime				= GetCurrentTime();
	m_bAenderungenClicked	= FALSE;
	m_bVerzeichnisseClicked	= FALSE;
	m_pDateTime = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CIdipCleanDlg::~CIdipCleanDlg()
{
	WK_DELETE(m_pNTDlg);
	WK_DELETE(m_pDateTime);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdipCleanDlg)
	DDX_Control(pDX, IDC_ZONE, m_cZone);
	DDX_Control(pDX, IDC_DATE, m_cDate);
	DDX_Control(pDX, IDC_TIME, m_cTime);
	DDX_Control(pDX, IDOK2, m_cOK);
	DDX_Control(pDX, IDC_INFO_NOTDELDIR, m_cInfoNotDelDir);
	DDX_Control(pDX, IDC_INFO_DELDIR, m_cInfoDelDir);
	DDX_Control(pDX, IDC_CHECK_ARCHIVE, m_cDelArchives);
	DDX_Control(pDX, IDC_CHECK_LOG, m_cDelLog);
	DDX_Control(pDX, IDC_CHECK_TEMP, m_cDelTemp);
	DDX_Text(pDX, IDC_BUILD, m_sBuild);
	DDX_Text(pDX, IDC_OS, m_sOS);
	DDX_Text(pDX, IDC_SR, m_sSR);
	DDX_Text(pDX, IDC_DRIVES, m_sDrives);
	DDX_Text(pDX, IDC_SCREEN, m_sScreen);
	DDX_Text(pDX, IDC_ALLE_ARCHIVE, m_sAlleArchive);
	DDX_Text(pDX, IDC_TEMP, m_sTemp);
	DDX_Text(pDX, IDC_INFO_DELDIR, m_sInfoDelDir);
	DDX_Text(pDX, IDC_INFO_NOTDELDIR, m_sInfoNotDelDir);
	DDX_Text(pDX, IDC_ZONE, m_sZone);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_DATE, m_sDate);
	DDX_Text(pDX, IDC_HZ, m_sHz);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIdipCleanDlg, CDialog)
	//{{AFX_MSG_MAP(CIdipCleanDlg)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DELDIR, OnDeldir)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, OnChangeDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER3, OnChangeTime)
	ON_BN_CLICKED(IDC_DATETIMEZONE, OnDatetimezone)
	ON_BN_CLICKED(IDOK2, OnBeenden)
	ON_BN_CLICKED(IDC_60Hz, On60Hz)
	ON_BN_CLICKED(IDC_75HZ, On75hz)
	ON_BN_CLICKED(IDC_BTN_AUDIO_VOLUMES, OnBtnAudioVolumes)
	ON_BN_CLICKED(IDC_CK_AUDIO_TEST, OnCkAudioTest)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_WND, OnMessageUpdateWnd)
	ON_BN_CLICKED(IDC_TWEAK_TOOL, OnBnClickedTweakTool)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CIdipCleanDlg message handlers
BOOL CIdipCleanDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pDateTime = new CWK_CPLApplet(m_hWnd, _T("timedate.cpl"));
	InitDateTime();
	InitTimezone();
	CheckHarddiskDrives();
	m_OsType = GetOSVersion();
	m_sScreen = GetDisplaySettings();
	m_sAlleArchive = GetDrivesForArchivDelete();
	m_sTemp = GetTempStatus();
	m_sInfoDelDir = _T("Verzeichnisse geleert!");
	m_sInfoNotDelDir = _T("Kein Verzeichnis geleert!");
	m_cDelArchives.SetCheck(1);
	m_cDelLog.SetCheck(1);
	m_cDelTemp.SetCheck(1);
	UpdateData(FALSE);
	InitAllDlgs();

	//hole alle Monitoreinstellungen
	GetMonitorConfigurations();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
OS_TYPE CIdipCleanDlg::GetOSVersion()
{
	OS_TYPE OsRet(OS_NOT);
	OSVERSIONINFO	osinfo;
	CString			sOSVersion;

	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);
	
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{		
		CString sNr;
		CString s;

		if(osinfo.dwMajorVersion  > 4)
		{
			sNr = _T("WIN XP, Version: ");
			OsRet = OS_XP;

			if(osinfo.dwMinorVersion == 0)
			{
				sNr = _T("WIN 2000, Version: ");
				OsRet = OS_2K;
			}
		}
		else
		{
			sNr = _T("WIN NT, Version: ");
			OsRet = OS_NT;
		}

		GetDlgItem(IDC_BUILD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SR)->ShowWindow(SW_SHOW);
		s.Format(_T("Build: %d"), osinfo.dwBuildNumber);
		m_sBuild = s;
		s.Format(_T("%d"), osinfo.szCSDVersion);
		m_sSR = s;


		m_sSR.Format(_T("%s"), osinfo.szCSDVersion);
		m_sOS.Format(_T("%s %d.%d"), sNr, 
								 osinfo.dwMajorVersion,
								 osinfo.dwMinorVersion);
	}
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if(osinfo.dwMinorVersion == 0)  //Win 95
		{
			m_sOS = _T("WIN 95");
			OsRet = OS_95;
		}
		
		if(osinfo.dwMinorVersion > 0)
		{
			if(LOWORD(osinfo.dwBuildNumber) >= 3000)
			{
				m_sOS = _T("WIN ME");
				OsRet = OS_ME;
			}
			else
			{
				m_sOS = _T("WIN 98");
				OsRet = OS_98;
			}
		}
	}
	
	UpdateData(FALSE);
	return OsRet;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::InitAllDlgs()
{
	switch(m_OsType)
	{
	case OS_NOT:
		break;

	case OS_95:
		break;

	case OS_98:
		m_p98Dlg = new CClean98Dlg;
		break;

	case OS_ME:
		break;

	case OS_NT:
		if(m_pNTDlg)
		{
			m_pNTDlg->Start();
			m_pNTDlg->Create(IDD_CLEANNT);
			m_pNTDlg->m_sOSInUse = _T("NT Einstellungen");
			m_pNTDlg->m_ctrlStaticOEM.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlStaticOEMMiddle.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ControlOEM1.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ControlOEM2.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ControlOEM3.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlUebernehmen.EnableWindow();

			m_pNTDlg->SetForegroundWindow();
			m_pNTDlg->ShowWindow(SW_SHOW);
		}
		break;
	case OS_XP:

		if(m_pNTDlg)
		{
			m_pNTDlg->Start();
			m_pNTDlg->Create(IDD_CLEANNT);
			m_pNTDlg->m_sOSInUse = _T("XP Einstellungen");
			m_pNTDlg->m_ctrlEditXPE.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlStaticXPE.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlStaticNero.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlNero1.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlNero2.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlNero3.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlNero4.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlNero5.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlNero6.ShowWindow(SW_SHOW);
			m_pNTDlg->SetForegroundWindow();
			m_pNTDlg->ShowWindow(SW_SHOW);
			
			//mach alle Einträge unter "Erstellen von" und NT-Loader unsichtbar
			m_pNTDlg->m_ctrlCreate.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlSeconds.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlLoader.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlLoaderTimeout.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlStaticList.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlStaticOrdner.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlStaticSys.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ControlDeskFolder.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlFileList.ShowWindow(SW_HIDE);
			m_pNTDlg->m_ctrlSysOverview.ShowWindow(SW_HIDE);
			m_pNTDlg->m_CheckFileList.ShowWindow(SW_HIDE);
			m_pNTDlg->m_CheckDeskFolder.ShowWindow(SW_HIDE);
			m_pNTDlg->m_CheckSystemOverview.ShowWindow(SW_HIDE);

			//HDD ID sichtbar machen
			m_pNTDlg->m_ctrlStaticHDD.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlHDD.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlHDD2.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlHDDLine.ShowWindow(SW_SHOW);
			m_pNTDlg->m_ctrlSaveHDDID.ShowWindow(SW_SHOW);

			CString m_sNewHDDID = GetHDDID();
			m_pNTDlg->m_sHDD = m_sNewHDDID.Left(4);
			m_pNTDlg->m_sHDD2 = m_sNewHDDID.Right(4);
	
				//XPe Lizenz holen
//	m_sXPeLicense = m_Reg.GetKey(_T("SOFTWARE\\dvrt\\Version"),_T("XPeLicense"));
		}
		break;

	case OS_2K:
		break;

	default:
		break;
	};

}
/////////////////////////////////////////////////////////////////////////////
//sStartDir = ""  ==> alle lokalen Festplatten werden durchsucht, oder "C:" eingeben
//sStartDir = "C:\test" ==> Verzeichnis C:\test (incl. Unterverz.) werden durchsucht
//sResultFile => enthält Fileübersicht der Suche, nur File angeben zB.:"c:\result.txt",
//keinen Pfad angeben
//bDelete = TRUE  ==> löscht das File, falls es schon vorhanden war
//bDelete = FALSE ==> schreibet neue Dateiliste ans Ende der vorhandenen Datei
BOOL CIdipCleanDlg::PrintTreeRecursiv(CString sStartDir, 
						  			CString sResultFile,
									BOOL bDelete)
{
	CFile TreeFile;
	CString sDrives;

	//lege File an
	if(!DoesFileExist((LPCTSTR) sResultFile))
	{
		BOOL bSuccess = TreeFile.Open(sResultFile, CFile::modeCreate | CFile::modeReadWrite   | CFile::modeNoInherit);
		if(bSuccess)
		{
			int iMax = 60;
			int i;
			CString s, t;
			CString sDot(_T("*"));
			for(i=0; i<iMax; i++)
			{	
				t+=sDot;
			}
			SYSTEMTIME sysTime;
			CFileStatus Status;
			CString sTime;
			CTime cTime;
			cTime = CTime::GetCurrentTime();
			cTime.GetAsSystemTime(sysTime);
			sTime.Format(_T("Dateiliste vom %2d.%2d.%4d  %2d:%2d\r\n"), sysTime.wDay, 
																	sysTime.wMonth,
																	sysTime.wYear,
																	sysTime.wHour,
																	sysTime.wMinute);
			TreeFile.Write(sTime, sTime.GetLength());

			TreeFile.Write(t, t.GetLength());
			s = _T("\r\n Datei-Übersicht aller auf Laufwerk 'C' vorhandenen Dateien\r\n");
			TreeFile.Write(s, s.GetLength());
			t+=_T("\r\n\r\n");
			TreeFile.Write(t, t.GetLength());
			TreeFile.Close();
		}
	}
	else
	{
		if(bDelete)
		{
			CFileStatus Status;
			TreeFile.GetStatus(sResultFile, Status);
			SYSTEMTIME	sysFileTime, sysPrgmTime;
			FILETIME	fFileTime, fPrgmTime;

			m_StartTime.GetAsSystemTime(sysFileTime);
			Status.m_ctime.GetAsSystemTime(sysPrgmTime);


			SystemTimeToFileTime(&sysFileTime,&fFileTime);
			SystemTimeToFileTime(&sysPrgmTime,&fPrgmTime);
			
			//Vergleich Create-File-Zeit, Programmstart-Zeit
			LONG lRet = CompareFileTime(&fFileTime,&fPrgmTime);

			if(lRet == -1)
			{
				//File zum Programmstart schon vorhanden, ist also altes File.
				//File löschen!
				DeleteFile(sResultFile);
				PrintTreeRecursiv(sStartDir, sResultFile, 0);
				return FALSE;
			}
		}


	}

	if (sStartDir.IsEmpty())
	{
		// alle lokalen Festplatten
		DWORD dwDrives = GetLogicalDrives();
		DWORD dwDrive = 4; // _T('C')
		CString sRoot;
		_TCHAR c;

		for (dwDrive=4,c=_T('c');dwDrive!=0;dwDrive<<=1,c++)
		{
			if (dwDrive & dwDrives)
			{
				sRoot.Format(_T("%c:\\"),c);
				if (DRIVE_FIXED == GetDriveType(sRoot))
				{
					if (PrintTreeRecursiv(sRoot, sResultFile, 0))
					{
						return TRUE;
					}
				}
			}
		}
		return FALSE;
	}
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSearch;
	CString sPath;
	CString sFile;

	sPath = sStartDir;
	if (sPath.GetLength()==0) {
		sPath += _T(".\\");
	} else if (sPath[sPath.GetLength()-1]!=_T('\\')) {
		sPath += _T('\\');
	}
	sSearch = sPath + _T("*.*");

	hF = FindFirstFile((LPCTSTR)sSearch,&FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		if ( (_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
			 (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sFile = sPath+FindFileData.cFileName;
				if (PrintTreeRecursiv(sFile, sResultFile, 0))
				{
					return TRUE;
				}
			}
			else
			{
				CString sText;
				sText = sPath + FindFileData.cFileName + _T("\r\n");
				TreeFile.Open(sResultFile, CFile::modeReadWrite | CFile::modeNoInherit);
				TreeFile.SeekToEnd();
				TreeFile.Write(sText, sText.GetLength());
				TreeFile.Close();
			}
		} 
		while (FindNextFile(hF,&FindFileData))
		{
			if ( (_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
				 (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					sFile = sPath+FindFileData.cFileName;
					if (PrintTreeRecursiv(sFile, sResultFile, 0))
					{
						return TRUE;
					}
				}
				else
				{
					CString sText;
					sText = sPath + FindFileData.cFileName + _T("\r\n");
					TreeFile.Open(sResultFile, CFile::modeReadWrite | CFile::modeNoInherit);
					TreeFile.SeekToEnd();
					TreeFile.Write(sText, sText.GetLength());
					TreeFile.Close();

				}
			}
		}
		FindClose(hF);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipCleanDlg::DoesFileExist(LPCTSTR szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipCleanDlg::CheckHarddiskDrives()
{
	BOOL iCheckOk = TRUE;
	int i;
	CString cSubKey;
	CString sCurrentDriveLetter;
	CString sDriveInfo;
	CString sAllHarddrives;
	CString sMaxBytes;
	CString sIdipVerwendung;
	HKEY hCheckDrivesKey = NULL;

	DWORD dwLength;
	dwLength = GetLogicalDriveStrings(0, NULL); 

	LPTSTR pBuffer = new TCHAR[dwLength]; 
	LPTSTR pStart = pBuffer;

	dwLength = GetLogicalDriveStrings(dwLength,
 									  pBuffer);
	// Registry öffnen
	cSubKey = _T("SOFTWARE\\DVRT\\Drives");
	//Schlüssel öffnen
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, cSubKey, 0, KEY_QUERY_VALUE, &hCheckDrivesKey);
	if (hCheckDrivesKey)
	{
		for (i=0; i<= (int)dwLength-1; i+=4)		// extrahiere alle Laufwerke
		{
			// nur Festplattenlaufwerke beachten
			if(GetDriveType(pBuffer) == DRIVE_FIXED)
			{
				m_sDriveLetters += pBuffer[0];
				sAllHarddrives = sAllHarddrives+pBuffer[0];
				sDriveInfo = pBuffer;
				DWORD dwType;
				DWORD dwSize;
				DWORD dwValue;
				dwSize = sizeof(dwValue);
				sCurrentDriveLetter = pBuffer[0];
				sCurrentDriveLetter.MakeUpper();

				// Inhalt für den Laufwerksbuchstaben holen 
				if(RegQueryValueEx(hCheckDrivesKey,
								   sCurrentDriveLetter,
								   NULL,
								   &dwType,
								   (PBYTE)&dwValue,
								   &dwSize) == ERROR_SUCCESS)
				{
					CString sVolumeNameBuffer;    // volume name buffer
					DWORD dwVolumeSerialNumber; // volume serial number
					DWORD dwMaximumComponentLength; // maximum file name length
					DWORD dwFileSystemFlags;    // file system options
					CString sFileSystemNameBuffer;// file system name buffer

					if(GetVolumeInformation(pBuffer, 
										 sVolumeNameBuffer.GetBuffer(_MAX_PATH),
										 _MAX_PATH,
										 &dwVolumeSerialNumber,
										 &dwMaximumComponentLength,
										 &dwFileSystemFlags,
										 sFileSystemNameBuffer.GetBuffer(_MAX_PATH),
										 _MAX_PATH))
					{
						sVolumeNameBuffer.ReleaseBuffer();
						sFileSystemNameBuffer.ReleaseBuffer();
						
						DWORD dwSectorsPerCluster = 0;
						DWORD dwBytesPerSector = 0;
						DWORD dwNumberOfFreeClusters = 0;
						DWORD dwTotalNumberOfClusters = 0;

						if (GetDiskFreeSpace( pBuffer, 
											  &dwSectorsPerCluster, 
											  &dwBytesPerSector, 
											  &dwNumberOfFreeClusters, 
											  &dwTotalNumberOfClusters ))
						{
							
							// total
							__int64 iTemp = (__int64)dwSectorsPerCluster * (__int64)dwBytesPerSector;
							__int64 iTotalNumberOfBytes =  ((__int64)dwTotalNumberOfClusters * iTemp)/1000; 
							
							sMaxBytes = InsertDezPoints(iTotalNumberOfBytes)+_T(" KB");

							switch(dwValue)
							{
							case 1:
								sIdipVerwendung = _T("Speicherung");
								break;
							case 2:
								sIdipVerwendung = _T("Backup schreiben & lesen");
								break;
							case 3:
								sIdipVerwendung = _T("Backup lesen");
								break;
							case 5:
								sIdipVerwendung = _T("Betriebssystem");
								break;
							case 6:
								sIdipVerwendung = _T("keine Verwendung");
								break;
							default:
								sIdipVerwendung = _T("FEHLER!");
								break;
							}
							sDriveInfo.Format(_T("%s (%s) %s  Gesamt: %s  Idip: %s"),
														  pBuffer, 
														  sFileSystemNameBuffer,
														  sVolumeNameBuffer,
														  sMaxBytes,
														  sIdipVerwendung);
						}
					} //Ende Inhalt für den Laufwerksbuchstaben holen 
				}
				m_saDrives.Add(sDriveInfo);	
			}
			else if(GetDriveType(pBuffer) == DRIVE_CDROM)
			{
				sAllHarddrives = sAllHarddrives+pBuffer[0];
				sDriveInfo = pBuffer;
				sDriveInfo.Format(_T("%s CD-ROM"), pBuffer);
				m_saDrives.Add(sDriveInfo);	
			}
			pBuffer += 4;
		}
		RegCloseKey(hCheckDrivesKey);
	}
	pBuffer = pStart;
	delete pBuffer;

	for(i=0; i<m_saDrives.GetSize(); i++)
	{
		if((m_saDrives.GetSize()-1)==i)
		{
			m_sDrives+=m_saDrives[i];
		}
		else
		{
			m_sDrives+=m_saDrives[i]+_T("\r\n");
		}
	}

	CString sReport, sCom;
	LPCOMMCONFIG pConfig;
	DWORD dwLen = 2*sizeof(COMMCONFIG);
	DWORD dwCommPortMask = 0;
	DWORD dwBitMask   = 1;

	pConfig = (LPCOMMCONFIG)malloc(dwLen);
	m_sDrives += _T("\r\nPorts:\r\n");
	for (i=1; i<=32; i++,dwBitMask<<=1)
	{
		sCom.Format(_T("COM%d"),i);
		if (GetDefaultCommConfig(sCom,(LPCOMMCONFIG)pConfig,&dwLen))
		{
			dwCommPortMask |= dwBitMask;
			m_sDrives += sCom;
			sCom.Format(_T(" baud=%d parity=%d data=%d stop=%d\r\n"), pConfig->dcb.BaudRate, pConfig->dcb.fParity, pConfig->dcb.ByteSize, pConfig->dcb.StopBits);
			m_sDrives += sCom;
		}
	}
	free(pConfig);
	if (dwCommPortMask == 0)
	{
		m_sDrives += _T("\r\nNo Ports!\r\n");
	}
	else
	{
	}

	UpdateData(FALSE);
	return iCheckOk;
}
/////////////////////////////////////////////////////////////////////////////
CString CIdipCleanDlg::InsertDezPoints(__int64 iValue)
{
	CString sRet;
	CString sDummy;
	__int64 iRest, iKilo, iMega, iGiga, iTerra, iDummy;
	__int64 iMilliard = 1000*1000*1000;
	__int64 iMillion = 1000*1000;
	iRest=iKilo=iMega=iGiga=iTerra=iDummy=0;

	if(iValue/(iMilliard) >= 1000)  //Terra
	{
		iDummy = iValue/iMilliard;
		iTerra = iDummy/1000;
		iGiga = iDummy%1000;
		iMega = (iValue/iMillion)%1000;
		sRet.Format(_T("%d.%03d.%03d."), (int)iTerra, (int)iGiga, (int)iMega);
	}
	else if(iValue/iMillion >= 1000)  //Giga
	{
		iGiga = iValue/iMilliard;
		iMega = (iValue/iMillion)%1000;
		sRet.Format(_T("%d.%03d."), (int)iGiga, (int)iMega);
	}
	else if(iValue/1000 >= 1000)   //Mega
	{
		iMega = iValue/iMillion;
		sRet.Format(_T("%d."), (int)iMega);
	}

	iKilo = (iValue/1000)%1000;
	iRest = iValue%1000;
	sDummy.Format(_T("%03d.%03d"), (int)iKilo, (int)iRest);
	sRet += sDummy;
	TRACE(_T("%s\n"), sRet);
	
	return sRet;
}
//////////////////////////////////////////////////////////////////////
CString CIdipCleanDlg::GetDisplaySettings()
{
	CString sRet;
	DWORD dwHeigth, dwWidth, dwColour, dwHZ;
	if(m_Reg.GetDisplaySettings(dwHeigth, dwWidth, dwColour, dwHZ))
	{
		sRet.Format(_T("%d x %d,  %d Bit Farbtiefe,  %d Hz"), dwWidth, 
														  dwHeigth, 
														  dwColour, 
														  dwHZ);
	}
	else
	{
		sRet = _T("aktuelle Einstellungen / Registry Einträge sind nicht identisch");  
	}
	return sRet;
}
//////////////////////////////////////////////////////////////////////
CString CIdipCleanDlg::GetDrivesForArchivDelete()
{
	CString sRet(_T("Laufwerk:  "));
	if(m_sDriveLetters.GetLength() > 1)
	{
		sRet = _T("Laufwerke: ");
	}

	for(int i = 0; i < m_sDriveLetters.GetLength(); i++)
	{
		 sRet += m_sDriveLetters.GetAt(i);
		 sRet += _T(":\\  ");
	}
	return sRet;
}
//////////////////////////////////////////////////////////////////////
CString CIdipCleanDlg::GetTempStatus()
{
	CString sRet;
	if(!m_Reg.CheckTemp())
	{
		m_Reg.CreateTempDir();
		sRet = _T("wurde angelegt");
	}
	else
	{
		sRet = _T("ist vorhanden");
	}
	return sRet;
} 
//////////////////////////////////////////////////////////////////////
CString CIdipCleanDlg::GetTimeZone()
{
	CString sRet;
	TIME_ZONE_INFORMATION pTimeZoneInformation;

	GetTimeZoneInformation(&pTimeZoneInformation);
	sRet = pTimeZoneInformation.StandardName;

	return sRet;
}
//////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::InitTimezone()
{
	CString sSubTimezone = GetTimeZone();
	HKEY hTimeZone;
	CString sSubKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\");
	CString sSubKeyStd = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\W. Europe Standard Time");
	
	//Öffnen des TimeZone Schlüssels und den Key "Std" bei "W. Europe Standard Time" auslesen
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		sSubKeyStd,
		0,
		KEY_READ | KEY_QUERY_VALUE,
		&hTimeZone) == ERROR_SUCCESS)
	{
		CString sValueStd(_T("Std"));
		DWORD dwSize;
		//die Größe des Wertes beim Schlüssel "Std" feststellen
		if(RegQueryValueEx(hTimeZone,
			(LPCTSTR)sValueStd,
			NULL,
			NULL,
			NULL,
			&dwSize) == ERROR_SUCCESS)
		{
			
		}
	}
	RegCloseKey(hTimeZone);

	//alle Subkey unter TimeZones durchsuchen und in jedem 
	//Subkey den Wert unter "Std" mit sSubTimezone vergleichen
	//Stimmen sie überein, dann den Wert "Display" auslesen.

	//Öffnen des TimeZone Schlüssels
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					sSubKey,
					0,
					KEY_READ | KEY_QUERY_VALUE,
					&hTimeZone) == ERROR_SUCCESS)
	{

		DWORD dwSubKeys = 0;
		DWORD dwMaxSubKeyLen;
		DWORD dwMaxClassNameLen;
		DWORD dwValues;
		DWORD dwMaxValueNameLen;
		DWORD dwMaxValueLen;
		FILETIME ftLastWriteTime;

		// aus geöffnetem TimeZone-Pfad den längsten Subkey-Namen finden
		if(RegQueryInfoKey(hTimeZone,
							NULL,
							NULL,
							NULL,
							&dwSubKeys,
							&dwMaxSubKeyLen,
							&dwMaxClassNameLen,
							&dwValues,
							&dwMaxValueNameLen,
							&dwMaxValueLen,
							NULL,
							&ftLastWriteTime) == ERROR_SUCCESS)
		{
			TCHAR cSubKeyName[_MAX_PATH];
			DWORD dwName = dwMaxSubKeyLen+1;

			//jeden Subkey durchgehen, den Schlüsselwert "Std" suchen, diesen mit
			// dem Wert von "sSubTimezone" vergleichen
			for (DWORD dw= 0; dw < dwSubKeys; dw++)
			{
				LONG lResult = RegEnumKeyEx(hTimeZone,       // handle to key to enumerate
								  dw,		     // subkey index
								  cSubKeyName,        // subkey name
								  &dwName,       // size of subkey buffer
								  NULL,			 // reserved
								  NULL,           // class string buffer
								  NULL,           // size of class string buffer
								  &ftLastWriteTime);
				if(lResult == ERROR_SUCCESS)
				{
					HKEY hSubTimeZone;
					CString sSubSubKey = sSubKey+cSubKeyName;

					//Subkey öffnen
					if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
								sSubSubKey,
								0,
								KEY_READ | KEY_QUERY_VALUE,
								&hSubTimeZone) == ERROR_SUCCESS)
					{
						CString sValueName(_T("Std"));
						DWORD dwSize;

						//die Größe des Wertes beim Schlüssel "Std" feststellen
						if(RegQueryValueEx(hSubTimeZone,
										   (LPCTSTR)sValueName,
										   NULL,
										   NULL,
										   NULL,
										   &dwSize) == ERROR_SUCCESS)
						{
							CString sValue;

							//mit festgestellter Größenvorgabe des Schlüsselwertes
							//den Inhalt des "Std"-Schlüssels auslesen
							if(RegQueryValueEx(hSubTimeZone,
											   (LPCTSTR)sValueName,
											   NULL,
											   NULL,
											   (LPBYTE)sValue.GetBuffer(dwSize),
											   &dwSize) == ERROR_SUCCESS)
							{
								sValue.ReleaseBuffer();
								sValueName = _T("Display");

								//stimmt "Std" Wert mit "sSubTimezone" überein
								if(sValue == sSubTimezone)
								{
									CString sDisplay;

									//die Größe des Wertes beim Schlüssel "Display" feststellen
									if(RegQueryValueEx(hSubTimeZone,
													   (LPCTSTR)sValueName,
													   NULL,
													   NULL,
													   NULL,
													   &dwSize) == ERROR_SUCCESS)
									{
										//mit festgestellter Größe den Inhalt des
										//Schlüssels "Display" auslesen
										if(RegQueryValueEx(hSubTimeZone,
													   (LPCTSTR)sValueName,
													   NULL,
													   NULL,
													   (LPBYTE)sDisplay.GetBuffer(dwSize),
													   &dwSize) == ERROR_SUCCESS)
										{	
										 	sDisplay.ReleaseBuffer();

											//Schlüsselwert von "Display" speichern
											//dieser wird im HauptDlg bei 
											//"Zeitzone" angezeigt
											m_sZone = sDisplay;
											TRACE(_T("Name: %s\n"), sDisplay);
											break;
										}
									}
								}
								
							}
							
						}
						RegCloseKey(hSubTimeZone);
					}
				}
				dwName = dwMaxSubKeyLen+1;
			}
		}
	}
	RegCloseKey(hTimeZone);
	UpdateData(FALSE);
	UpdateWindow();
}
//////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnDestroy() 
{
	WK_DELETE(m_pDateTime);
	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_pNTDlg && IsWindow(m_pNTDlg->m_hWnd))
	{
		m_pNTDlg->m_ControlOEM1.SetFocus();
		m_pNTDlg->UpdateData(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnDeldir() 
{
	int iValue = IDYES;
	m_cInfoDelDir.ShowWindow(SW_HIDE);
	m_cInfoNotDelDir.ShowWindow(SW_HIDE);

	if(!m_cDelLog.GetCheck() || !m_cDelTemp.GetCheck() || !m_cDelArchives.GetCheck())
	{
		CString sText = _T("ACHTUNG:\r\n\r\nEs werden nicht alle Verzeichnisse geleert.\r\nTrotzdem fortfahren ?");
		iValue = MessageBox(sText, _T("Verzeichnisse leeren"), MB_YESNO|MB_ICONSTOP);

	}

	CVipCleanReg reg;

	BOOL iShow = FALSE;
	//leere die markierten Verzeichnisse
	if(m_cDelLog.GetCheck() && iValue == IDYES)
	{
		CString sPath = reg.GetKey(SECTION_DVRT SECTION_LOG, LOG_LOGPATH);
		if (sPath.GetAt(0) == _T('0'))
		{
			sPath = _T("c:\\log");
		}
		m_Reg.DelFiles(sPath);
		iShow = TRUE;
	}

	if(m_cDelTemp.GetCheck() && iValue == IDYES)
	{
		m_Reg.DelFiles(_T("C:\\temp"));
		m_Reg.DelFiles(_T("C:\\tmp"));
		CString sPath = reg.GetKey(HKLM_ENV_TEMP, ENV_TEMP);
		if (sPath.GetAt(0) != _T('0'))
		{
			m_Reg.DelFiles(sPath);
		}

		iShow = TRUE;
	}

	if(m_cDelArchives.GetCheck() && iValue == IDYES)
	{
		m_Reg.DelFiles(_T("c:\\dv\\mdmask1"));
		m_Reg.DelFiles(_T("c:\\dv\\mdmask2"));
		RemoveDirectory(_T("c:\\dv\\mdmask1"));
		RemoveDirectory(_T("c:\\dv\\mdmask2"));

		m_Reg.DelFiles(_T("c:\\Security\\mdmask1"));
		m_Reg.DelFiles(_T("c:\\Security\\mdmask2"));
		RemoveDirectory(_T("c:\\Security\\mdmask1"));
		RemoveDirectory(_T("c:\\Security\\mdmask2"));
		CString sPath = reg.GetKey(SECTION_DVRT SECTION_COMMON, COMMON_RUNTIME_ERRORS);
		if (sPath.GetAt(0) == _T('0'))
		{
			sPath = _T("c:\\dv\\rterrors.dat");
		}
		DeleteFile(sPath);
		
		_TCHAR drive;
		CString path_dvs, path_dbs;
		/* If we can switch to the drive, if exists. */
		for( drive = _T('c'); drive <= _T('z'); drive++ )
		{
			path_dvs.Format(_T("%c:\\dvs"),drive);
			path_dbs.Format(_T("%c:\\dbs"),drive);
			m_Reg.DelFiles(path_dvs);
			RemoveDirectory(path_dvs);
			m_Reg.DelFiles(path_dbs);
			RemoveDirectory(path_dbs);
		}
		iShow = TRUE;
	}

	if(iShow)
	{
		m_cInfoDelDir.ShowWindow(SW_SHOW);
		UpdateData();
	}
	else
	{
		m_cInfoNotDelDir.ShowWindow(SW_SHOW);
	}

	m_bVerzeichnisseClicked = TRUE;
}
/////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnOK() 
{
	//prüfe, ob die Button "Verzeichnisse leeren" und "Änderungen übernehmen"
	//geklickt wurden, wenn nicht, MessageBox anzeigen

	CString sText;
	int iResult = 0;
	if(!m_bAenderungenClicked && !m_bVerzeichnisseClicked)
	{
		sText = _T("ACHTUNG:\r\nFolgende Funktionen wurden noch nicht ausgeführt:\r\n\r\n");
		sText += _T("- Änderungen übernehmen\r\n- Verzeichnisse leeren\r\n\r\n");
		sText += _T("Soll das Programm trotzdem beendet werden ?");
		iResult = MessageBox(sText, _T("Programm beenden ?"), MB_YESNO | MB_ICONSTOP);
	}
	else
	{
		sText = _T("ACHTUNG:\r\nFolgende Funktion wurden noch nicht ausgeführt:\r\n\r\n");
		if(!m_bAenderungenClicked)
		{
			sText += _T("- Änderungen übernehmen\r\n\r\n");
			sText += _T("Soll das Programm trotzdem beendet werden ?");
			iResult = MessageBox(sText, _T("Programm beenden ?"), MB_YESNO | MB_ICONSTOP);
		}

		if(!m_bVerzeichnisseClicked)
		{
			sText += _T("- Verzeichnisse leeren\r\n\r\n");
			sText += _T("Soll das Programm trotzdem beendet werden ?");
			iResult = MessageBox(sText, _T("Programm beenden ?"), MB_YESNO | MB_ICONSTOP);
		}

	}

	if(iResult == IDYES || !iResult)
	{
		CDialog::OnOK();
	}
	

}
/////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnChangeDate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	ChangeDateTime();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnChangeTime(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnChangeTimezone() 
{

}
/////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::InitDateTime()
{
	GetTimeZoneInformation(&m_pTimeZoneInformation);
	GetLocalTime(&m_SystemDateTime);

	CTime SysTime(m_SystemDateTime);
	m_sDate = SysTime.Format(_T("%d.%m.%y"));
	m_sTime = SysTime.Format(_T("%H:%M:%S"));
	
	m_cDate.UpdateData();
	m_cTime.UpdateData();

	UpdateData(FALSE);
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////
BOOL CIdipCleanDlg::ChangeDateTime()
{
	return m_pDateTime->Invoke(m_hWnd, 0);
}
////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnDatetimezone() 
{
	m_pDateTime->Invoke(m_hWnd, 0);
	PostMessage(WM_UPDATE_WND, 0, 0);
}
////////////////////////////////////////////////////////////////////
long CIdipCleanDlg::OnMessageUpdateWnd(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	InitDateTime();
	InitTimezone();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnBeenden() 
{
	OnOK();
	
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::GetMonitorConfigurations()
{
	EnumDisplaySettings(NULL,
						ENUM_CURRENT_SETTINGS,
						&m_lpDevMode
						);
	
	m_sHz.Format(_T("%d"), m_lpDevMode.dmDisplayFrequency);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::On60Hz() 
{
	int iHz = 60;
	int iResult = 0;
	m_lpDevMode.dmDisplayFrequency = iHz;

	CString sText;
	sText.Format(_T("Soll die aktuelle Frequenz wirklich auf %d Hz geändert werden?"), iHz);
	iResult = MessageBox(sText, _T("Bildschirmfrequenz ändern"), MB_OKCANCEL | MB_ICONSTOP);
	if(iResult == IDOK)
	{
		ChangeDisplaySettings(&m_lpDevMode,CDS_UPDATEREGISTRY);
		m_sHz.Format(_T("%d"), m_lpDevMode.dmDisplayFrequency);
		m_sScreen = GetDisplaySettings();
		UpdateData(FALSE);	
	}

	
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::On75hz() 
{
	int iHz = 75;
	int iResult = 0;
	m_lpDevMode.dmDisplayFrequency = iHz;
	CString sText;
	sText.Format(_T("Soll die aktuelle Frequenz wirklich auf %d Hz geändert werden?"), iHz);
	iResult = MessageBox(sText, _T("Bildschirmfrequenz ändern"), MB_OKCANCEL | MB_ICONSTOP);
	if(iResult == IDOK)
	{
		ChangeDisplaySettings(&m_lpDevMode,CDS_UPDATEREGISTRY);
		m_sHz.Format(_T("%d"), m_lpDevMode.dmDisplayFrequency);
		m_sScreen = GetDisplaySettings();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CIdipCleanDlg::GetHDDID()
{
	_TCHAR c1[255];
	_TCHAR c2[255];
	DWORD dw1 = 255;
	DWORD dw2 = 255;
	DWORD dw3 = 255;

  LPCTSTR lpRootPathName = _T("C:\\");         // root directory
  LPTSTR lpVolumeNameBuffer;        // volume name buffer
  DWORD nVolumeNameSize = 255;            // length of name buffer
  LPDWORD lpVolumeSerialNumber;     // volume serial number
  LPDWORD lpMaximumComponentLength; // maximum file name length
  LPDWORD lpFileSystemFlags;        // file system options
  LPTSTR lpFileSystemNameBuffer;    // file system name buffer
  DWORD nFileSystemNameSize  = 255; 

	lpVolumeNameBuffer = c1;
	lpVolumeSerialNumber = &dw1;
	lpMaximumComponentLength = &dw2;
	lpFileSystemFlags = &dw3;
	lpFileSystemNameBuffer = c2;

	GetVolumeInformation(lpRootPathName,
						 lpVolumeNameBuffer,
						 nVolumeNameSize,
						 lpVolumeSerialNumber,
						 lpMaximumComponentLength,
						 lpFileSystemFlags,
						 lpFileSystemNameBuffer,
						 nFileSystemNameSize);
	WORD w2 = LOWORD(dw1);
	WORD w1 = HIWORD(dw1);
	CString sHDDID;
	sHDDID.Format(_T("%x - %x"),w1, w2);

	return sHDDID;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnBtnAudioVolumes() 
{
	theApp.ShellExecute(_T("SNDVOL32.EXE"));
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnCkAudioTest() 
{
	theApp.ShellExecute(_T("AudioTest.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CIdipCleanDlg::OnBnClickedTweakTool()
{
	theApp.ShellExecute(_T("VideteTweak.exe"));
}
/////////////////////////////////////////////////////////////////////////////
#endif // _IDIP