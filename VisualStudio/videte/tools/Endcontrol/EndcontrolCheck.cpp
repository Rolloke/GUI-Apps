// EndcontrolCheck.cpp: implementation of the CEndcontrolCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Endcontrol.h"
#include "EndcontrolCheck.h"
#include "EndcontrolDlg.h"
#include <user.h>  // für CUserArray
#include <afxtempl.h>
#include <afxmt.h>
#include "afx.h"  // für Cfile::remove


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEndcontrolCheck::CEndcontrolCheck(CEndcontrolDlg* eDlg)
{
	m_pClassDlg = eDlg;
}
//////////////////////////////////////////////////////////////////////
CEndcontrolCheck::~CEndcontrolCheck()
{

}
//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolApp::InitInstance
BOOL CEndcontrolCheck::CheckSecurityLauncher()
{

	CWnd* fenster;
	BOOL bCheckSecurityLauncher = true;

	// Läuft Video Save noch ?
	if((fenster = m_pClassDlg->FindWindow(NULL, "Video Save Starter")) != NULL)
	{
		if(m_pClassDlg->MessageBox("EndControl kann nicht ausgeführt werden, solange Video Safe aktiv ist.\n\n\rVideo Safe beenden?",
			"Video Safe noch aktiv !",
			MB_OKCANCEL|MB_ICONINFORMATION) != IDOK)
		{
			bCheckSecurityLauncher = false;
		}
		else
		{
			// Erstes Login und Passwort aus Registry holen
			GetLogin();
			
			// Video Save zum Runterfahren bewegen durch schicken einer Message
			fenster->PostMessage(WM_COMMAND,ID_APP_EXIT);
			Sleep(1000); //zum Warten, bis sich Login Dialog öffnet
			
			// Login Dialog mit Autologin beenden
			ClimbWindow(fenster->GetWindow(GW_HWNDFIRST));
			Sleep(500);   //da sonst das System-Fenster nicht gefunden wird
			
			// für Version 3.6x: Ebenfalls das System-Fenster schliessen
			if((fenster = m_pClassDlg->FindWindow(NULL, "Video Save Starter System")) != NULL)
			{
				fenster->PostMessage(WM_COMMAND, IDOK, BN_CLICKED);
			}

			// solange warten, bis Video Save Starter beendet
			fenster = m_pClassDlg->FindWindow(NULL, "Video Save Starter");
			while (fenster)
			{
				fenster = m_pClassDlg->FindWindow(NULL, "Video Save Starter");
			}
		}	
	}
	return bCheckSecurityLauncher;
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolApp::CheckDrivesAndRegistry
CString CEndcontrolCheck::RenameHarddiskDrives()
{

	CString sAllHarddrives = "";
	DWORD dwLength;
	CString strLength;
	
	dwLength = GetLogicalDriveStrings(0,
									 NULL); // dwLength enthält die Buffergröße des 
											// zurückgelieferten Strings

	LPTSTR  pBuffer = new TCHAR[dwLength]; // neue Buffergröße anlegen

	dwLength = GetLogicalDriveStrings(dwLength,
 									  pBuffer);

	
	UINT i;								// Schleifenzähler
	UINT iNrPartition = 1;						
	UINT iNrFestplatte = 1;
	CString cDiskName;			// Name einer Partition

	for (i=0; i<= dwLength-1; i+=4)		// extrahiere alle Laufwerke
	{
		// nur Festplattenlaufwerke beachten
		if(GetDriveType(pBuffer) == DRIVE_FIXED)
		{
			sAllHarddrives = sAllHarddrives+pBuffer[0];
			cDiskName.Format("DISK%i_%i",iNrFestplatte,iNrPartition); //int in String konvertieren
//TODO Laufwerksbenennung bleibt draussen, solange Zuordnung zu Festplatten nicht geklärt
//			SetVolumeLabel(pBuffer, cDiskName);
			iNrPartition++;
		}

		pBuffer += 4;
	}

	return sAllHarddrives;
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolApp::CheckDrivesAndRegistry
BOOL CEndcontrolCheck::CheckHarddiskDrives(CString cDriveLetter)
{
	BOOL iCheckOk = true;
	int iNumberOfDrives;
	int i;
	CString cSubKey;
	CString sCurrentDriveLetter;
	HKEY hCheckDrivesKey;

	//m_sDrivesChanged.Empty();  // beinhaltet die Laufwerke, die nicht am Testlauf beteiligt waren
	
	cSubKey = "SOFTWARE\\DVRT\\Drives";
	iNumberOfDrives = cDriveLetter.GetLength();
	
	// Registry öffnen
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					cSubKey,
					0,
					KEY_READ,
					&hCheckDrivesKey) == ERROR_SUCCESS)

	{
		DWORD dwType;
		DWORD dwSize;
		DWORD dwValue;
		dwSize = sizeof(dwValue);

		// alle Laufwerke nacheinander prüfen
		for(i = 0; i < iNumberOfDrives; i++)
		{
			sCurrentDriveLetter = cDriveLetter[i];
			sCurrentDriveLetter.MakeUpper();

			// Inhalt für jeden Laufwerksbuchstaben holen (nur HardDisks)
			if(RegQueryValueEx(hCheckDrivesKey,
							   sCurrentDriveLetter,
							   NULL,
							   &dwType,
							   (PBYTE)&dwValue,
							   &dwSize) == ERROR_SUCCESS)

			{				
				// Laufwerk "C" muß Wert 5 haben
				if((sCurrentDriveLetter == "C") && (dwValue != 5 ))
				{
					iCheckOk = false;

					dwValue = 5;
					dwSize = sizeof(dwValue);

					if(RegSetValueEx(hCheckDrivesKey,
									 sCurrentDriveLetter,
									 NULL,
									 dwType,
									 (PBYTE)&dwValue,
									 dwSize) == ERROR_SUCCESS)
					{
						m_sDrivesChanged += sCurrentDriveLetter;
					}

				}
				
				// alle anderen Laufwerke müssen Wert 1 haben
				if((sCurrentDriveLetter != "C") && (dwValue != 1 ))
				{
					iCheckOk = false;
					dwValue = 1;
					dwSize = sizeof(dwValue);

					if(RegSetValueEx(hCheckDrivesKey,
									 sCurrentDriveLetter,
									 NULL,
									 dwType,
									 (PBYTE)&dwValue,
									 dwSize) == ERROR_SUCCESS)
					{
						m_sDrivesChanged += sCurrentDriveLetter;
					}
				}
			}
		}

		RegCloseKey(hCheckDrivesKey);
	}
	else
	{	
		iCheckOk = false;
	}
	return iCheckOk;
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolCheck::CheckSecurityLauncher
void CEndcontrolCheck::GetLogin()
{
	CUserArray	UserArray;
	CWK_Profile prof;
	CUser*		pUser = NULL;

	UserArray.Load(prof);

	// Namen und Passwort des ersten Eintrages in der Registry für Videosafe holen
	pUser = UserArray.GetAt(0);
	if(pUser)
	{
		m_sLoginName = pUser->GetName();
		m_sLoginPassword = pUser->GetPassword();
	}
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolCheck::CheckSecurityLauncher
void CEndcontrolCheck::ClimbWindow(CWnd *pWnd)
{
	CString	sTitle;
	CWnd*	pChild;  
	char	szClassName[256];
	BOOL	bFindFirst = FALSE;
	
	if (m_sLoginName.IsEmpty())
		return;

	while (pWnd)
	{ 
		// Logindialog suchen
		pWnd->GetWindowText(sTitle);
		
		if ((sTitle.Find(" - Login")!= -1) ||
			(sTitle.Find(" - Legitimation") != -1) ||
			(sTitle.Find("*** Systemfehler ***")!= -1))
		{
			pChild = pWnd->GetWindow(GW_CHILD);
			
			// Suche die Editfelder
			while (pChild)
			{
				GetClassName(pChild->m_hWnd, szClassName, sizeof(szClassName)-1);
				if ((CString)szClassName == "Edit")
				{
					if (!bFindFirst)	
					{
						bFindFirst = TRUE;
						// Erstmal den Inhalt markieren
						pChild->PostMessage(EM_SETSEL, 0, -1);
						// Login-Name ins Editfeld kopieren
						for (int i = 0; i < m_sLoginName.GetLength(); i++)
							pChild->PostMessage(WM_CHAR, m_sLoginName[i]);
					}
					else
					{
						// Erstmal den Inhalt markieren
						pChild->PostMessage(EM_SETSEL, 0, -1);
						// Login-Passwort ins Editfeld kopieren
						for (int i = 0; i < m_sLoginPassword.GetLength(); i++)
							pChild->PostMessage(WM_CHAR, m_sLoginPassword[i]);
					}
				}
				pChild = pChild->GetNextWindow();
			}
			
			// OK-Button suchen
			pChild = pWnd->GetWindow(GW_CHILD);
			while (pChild)
			{
				GetClassName(pChild->m_hWnd, szClassName, sizeof(szClassName)-1);
				if ((CString)szClassName == "Button")
				{
					pWnd->PostMessage(WM_COMMAND, MAKELONG(IDOK, BN_CLICKED), (LPARAM)pChild->m_hWnd);
					pChild = NULL;
				}
				else
				{
					pChild = pChild->GetNextWindow();
				}
			}
		}	
		pWnd = pWnd->GetNextWindow();
	}
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolApp::CheckDrivesAndRegistry
BOOL CEndcontrolCheck::ImportEndcontrolRegistry()
{

	BOOL bImportRegOK = true;
	CWnd* fenster;

	//Dialog zum auswählen des zu importierenden RegFiles festlegen
	CFileDialog LoadReg(TRUE,
						NULL,
						NULL,
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						"REG Files (*.reg)|*.reg||",
						NULL );

	// Dateidialog initialisieren
	LoadReg.m_ofn.lpstrTitle = "Öffnen der Endcontrol - Registry";
	LoadReg.m_ofn.lpstrInitialDir = "C:\\";

	//Dialog auf Bildschirm ausgeben
	if(LoadReg.DoModal() == IDOK)
	{
		CString sRegFileName;
		CString sRegPathName;
		CString sRegEdit;
		
		sRegFileName = LoadReg.GetFileName();
		sRegPathName = LoadReg.GetPathName();
		sRegEdit = "C:\\windows\\regedit.exe "+sRegPathName;

		CString sRegPathToDelete;
		CString sRegSubKeyToDelete; 


		//**** Registry Verzeichnis ...\DVRT\User löschen ****
		// wird später durch Endcontrol-Reg wieder ersetzt
		
		sRegPathToDelete = "SOFTWARE\\DVRT";
		sRegSubKeyToDelete = "User";
		if(!DeleteCriticalRegPath(sRegPathToDelete, sRegSubKeyToDelete))
		{
			bImportRegOK = false;
		}



		// **** RegKeys sichern aus Pfad ...\DRVT\Drives und ...\DVRT\Version ****
		// sichern des Pfades ...\DVRT\Drives da dieser alle aktuell aktiven
		// Harddisk-Laufwerke beinhaltet, bei Version ebenfalls
		// zum Erweitern: #define neu 3; in SaveRegKey "case neu" einfügen

		CString sRegPathToSave;
		sRegPathToSave = "SOFTWARE\\DVRT\\Drives";
		if(!SaveRegKey(sRegPathToSave, DRIVES))
		{
			bImportRegOK = false;
		}

		sRegPathToSave = "SOFTWARE\\DVRT\\Version";
		if(!SaveRegKey(sRegPathToSave, VERSION))
		{
			bImportRegOK = false;			
		}


		//**** Registry Verzeichnis ...\DVRT\Drives und ...\DVRT\Version löschen ****
		// festlegen des zu löschenden RegVerzeichnisses unter HKEY_LOCAL_MACHINE
		
		sRegPathToDelete = "SOFTWARE\\DVRT";
		sRegSubKeyToDelete = "Drives";
		if(!DeleteCriticalRegPath(sRegPathToDelete, sRegSubKeyToDelete))
		{
			bImportRegOK = false;
		}

		sRegSubKeyToDelete = "Version";
		if(!DeleteCriticalRegPath(sRegPathToDelete, sRegSubKeyToDelete))
		{
			bImportRegOK = false;
		}
/*
//TODO regedit runterfahren
		// falls RegEdit.exe läuft, dieses schliessen
	//	fenster = pDlg->FindWindow("RegEdit_RegEdit", "Registrierungseditor");
	//	fenster->PostMessage(WM_COMMAND, WM_QUIT);
	//	if(fenster->DestroyWindow())
		{
			TRACE("destroyed\n");
		}
		while(fenster)
		{	
			fenster = pDlg->FindWindow("RegEdit_RegEdit", "Registrierungseditor");
			TRACE("RegEdit offen\n");
		}
*/
		//Gewählte Datei in Registry eintragen
		if(!(WinExec(sRegEdit, SW_SHOW) > 31))
		{
			m_pClassDlg->MessageBox("Eintragen der Datei: \n\n\r"
					   +sRegFileName
					   +"\n\nin die Registry nicht möglich.\n\n\r"
					   "Mögliche Gründe:\n\r"
					   "  - Programm RegEdit.exe liegt nicht im Windows-Hauptverzeichnis\n\r"
					   "  - keine Zugriffsrechte auf Registry",   
					   "Registry wurde nicht verändert !",
					   MB_OK|MB_ICONSTOP);
			bImportRegOK = false;
		}

		Sleep(300);

		// erst weiter, wenn Dialogbox des Reg-Editors geschlossen
		fenster = m_pClassDlg->FindWindow(NULL, "Registrierungseditor");
		while(fenster)
		{	
			fenster = m_pClassDlg->FindWindow(NULL, "Registrierungseditor");
			TRACE("schleife\n");
		}
	
		//**** gesichertes Registry Verzeichnis ...\DVRT\Drives ****
		// und ...\DVRT\Version laden 
		// zum Erweitern: in LoadRegKey "case neu" einfügen

		CString sRegPathToLoad = "SOFTWARE\\DVRT\\Drives";
		if(!LoadRegKey(sRegPathToLoad, DRIVES))
		{
			bImportRegOK = false;
			
		}

		sRegPathToLoad = "SOFTWARE\\DVRT\\Version";
		if(!LoadRegKey(sRegPathToLoad, VERSION))
		{
			bImportRegOK = false;
		}

	}
	else
	{
		bImportRegOK = false;
	}
	return bImportRegOK;
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolCheck::ImportEndcontrolRegistry
BOOL CEndcontrolCheck::DeleteCriticalRegPath(CString sRegPathToDelete, CString sRegSubKeyToDelete)
{
	BOOL bPathDeleted = true;
	HKEY hPathToDelete;

	// Registry öffnen
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					sRegPathToDelete,
					0,
					KEY_ALL_ACCESS,
					&hPathToDelete) == ERROR_SUCCESS)

	{
		RegDeleteKey(hPathToDelete,
					 sRegSubKeyToDelete);

		RegCloseKey(hPathToDelete);
	}
	else
	{
		bPathDeleted = false;
	}

	return bPathDeleted;
}
//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolCheck::ImportEndcontrolRegistry
BOOL CEndcontrolCheck::SaveRegKey(CString sRegPathToSave, int flag)
{

	BOOL bSaveRegKeyOK = true;
	HKEY hPathToSave;
//	CEndcontrolRegKeySave ListSave;

	
	// Registry Pfad öffnen
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		sRegPathToSave,
		0,
		KEY_ALL_ACCESS,
		&hPathToSave) == ERROR_SUCCESS)
	{
		DWORD dwSubKeys;
		DWORD dwMaxSubKeyLen;
		DWORD dwMaxClassNameLen;
		DWORD dwValues;
		DWORD dwMaxValueNameLen;
		DWORD dwMaxValueLen;
		FILETIME ftLastWriteTime;
		
		// aus geöffnetem Pfad Eigenschaften der Schlüsselwerte lesen
		if(RegQueryInfoKey(hPathToSave,
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
			
			DWORD	dwIndex;		// durchzählen aller Schlüsselwerte
			LPSTR	pcValueName;	// Schlüsselnamen
			DWORD	dwType;			// Typ des Schlüsselwertes
			LPBYTE	pbData;			// Inhalt des Schlüsselwertes
			DWORD	dwDataLen;		// Länge des Inhaltes
			
			// alle Schlüsselwerte nacheinander auslesen und in Liste sichern
			for(dwIndex = 0; dwIndex <= dwValues-1; dwIndex++)
			{
				DWORD	dwValueNameLen;				// Länge des Schlüsselnamens
				pcValueName = new CHAR(sizeof(dwMaxValueNameLen+1));
				dwValueNameLen = dwMaxValueNameLen+1;// Länge des Schlüsselnamens
				// auf maximal vorkommende
				// Länge setzen
				pbData = new BYTE(sizeof(dwMaxValueLen+1));
				dwDataLen = dwMaxValueLen+1;
				
				if(RegEnumValue(hPathToSave,
								dwIndex,
								pcValueName,
								&dwValueNameLen,
								NULL,
								&dwType,
								pbData,
								&dwDataLen) == ERROR_SUCCESS)
				{
					// alle Schlüsselwerte sichern 
/*					ListSave.SetRegPathToSave(sRegPathToSave);
					ListSave.SetValueName(pcValueName);
					ListSave.SetValueNameLen(dwValueNameLen);
					ListSave.SetType(dwType);
					ListSave.SetData(pbData);
					ListSave.SetDataLen(dwDataLen);
*/
					m_ListSave.m_sRegPathToSave = sRegPathToSave;
					m_ListSave.m_pcValueName = pcValueName;
					m_ListSave.m_dwValueNameLen = dwValueNameLen;
					m_ListSave.m_dwType = dwType;
					m_ListSave.m_pbData = pbData;
					m_ListSave.m_dwDataLen = dwDataLen;


					
					
					// jeden Schlüsselwert mit seinen Eigenschaften in Liste
					// speichern, in Abh. des Pfades (DVRT\Drives oder DVRT\Version)

					switch(flag)
					{
					case DRIVES:
						m_ListDrives.AddTail(m_ListSave);
						break;

					case VERSION:
						m_ListVersion.AddTail(m_ListSave);
					}

				}
			}
		}
		RegCloseKey(hPathToSave);
	}
	else
	{ 
		// kein Zugriff auf Registry, Schlüssel konnte nicht geöffnet werden
		bSaveRegKeyOK = false;
	}
	return bSaveRegKeyOK;
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolCheck::ImportEndcontrolRegistry
BOOL CEndcontrolCheck::LoadRegKey(CString sRegPathToLoad, int flag)
{
	
	BOOL bLoadRegKeyOK = true;
	POSITION position = NULL; 
	HKEY hNewKey;
	DWORD dwKeyCreated;
	BOOL bListEmpty = false;
	CEndcontrolRegKeySave	load;
	//Liste darf nicht leer sein

	switch (flag)
	{
	case DRIVES:
		bListEmpty = m_ListDrives.IsEmpty();
		break;
	case VERSION:
		bListEmpty = m_ListVersion.IsEmpty();
	}

	
	if(!bListEmpty)
	{
		// in Abh. des wieder herzustellenden Pfades, richtige Liste auswählen
		switch (flag)
		{
		case DRIVES:
			position = m_ListDrives.GetHeadPosition();
			break;
		case VERSION:
			position = m_ListVersion.GetHeadPosition();
		}
	
		// gewünschten Schlüssel wieder anlegen
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
			sRegPathToLoad,
			NULL,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hNewKey,
			&dwKeyCreated) == ERROR_SUCCESS)
		{
			do
			{
				// in Abh. des Schlüssels richtige Liste wählen und durchgehen
				switch (flag)
				{
				case DRIVES:
					load = m_ListDrives.GetNext(position);
					break;
				case VERSION:
					load = m_ListVersion.GetNext(position);
				}
				
				//Schlüsselwerte wieder in Registry eintragen
				if(RegSetValueEx(hNewKey,
					load.m_pcValueName,
					NULL,
					load.m_dwType,
					load.m_pbData,
					load.m_dwDataLen) != ERROR_SUCCESS)
				{
					bLoadRegKeyOK = false;
				}
				
				// solange noch ein Listenelement vorhanden ist 
			}while(position) ;
			RegCloseKey(hNewKey);
			
			// in Abh. des Schlüssels richtige Liste wieder löschen
			switch (flag)
			{
			case DRIVES:
				m_ListDrives.RemoveAll();
				break;
			case VERSION:
				m_ListVersion.RemoveAll();
			}
		}
	}
	
	return bLoadRegKeyOK;
}

//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolApp::DeleteArchivsCheckFolders
BOOL CEndcontrolCheck::DelArchive(CString sAllHarddrives)
{

	BOOL bDelArchive = true;
	CString sCurrentDriveLetter;
	int iNumberOfDrives; 
	int i;
	CString sFolderToDelete;
	CString sFolderDatabase = ":\\database";
	CString sFolderDbs		= ":\\dbs";
	
	iNumberOfDrives = sAllHarddrives.GetLength();

	for(i = 0; i < iNumberOfDrives; i++)
	{
		sCurrentDriveLetter = sAllHarddrives[i];
		sCurrentDriveLetter.MakeUpper();
		
		if(sCurrentDriveLetter != "C")
		{

			// Verzeichnis "Database" bei VideoSave 3.x
			WinExec("deltree /Y "+sCurrentDriveLetter+sFolderDatabase, SW_HIDE);
			// Verzeichnis "Dbs" ab VideoSave 4.x
			WinExec("deltree /Y "+sCurrentDriveLetter+sFolderDbs, SW_HIDE);

			// wurden die Directory :\Database gelöscht?
			if((BOOL)CreateDirectory(sCurrentDriveLetter+sFolderDatabase,0))
			{ 
				RemoveDirectory(sCurrentDriveLetter+sFolderDatabase);
			}
			else
			{
				bDelArchive = false;
			}
			
			// wurden die Directory :\Dbs gelöscht?
			if((BOOL)CreateDirectory(sCurrentDriveLetter+sFolderDbs,0))
			{ 
				RemoveDirectory(sCurrentDriveLetter+sFolderDbs);
			}
			else
			{
				bDelArchive = false;
			}

		}
	}	

	return bDelArchive;
}
//////////////////////////////////////////////////////////////////////
// Aufruf erfolgt aus: CEndcontrolApp::DeleteArchivsCheckFolders
BOOL CEndcontrolCheck::CheckFolders()
{

/*
	//***** Verzeichnisse prüfen ****

	CString sCheckFolders = "\r\nVerzeichnis Check\r\n*****************\r\n\r\n";

	CString sDirWinst95 = "C:\\WINST95";
	CString sDirInstall = "C:\\INSTALL";
	CString sDirOut = "C:\\OUT";
	CString sDirlog = "C:\\log";

	
	// folder "C:\WINST95" exists ??
	if((BOOL)CreateDirectory(sDirWinst95,0))
	{ 
		sCheckFolders += "FEHLER: "+sDirWinst95+" nicht vorhanden.\r\n";
		RemoveDirectory(sDirWinst95);

		GetDlgItem(IDC_F1_NOT)->ShowWindow(SW_SHOW);
	}
	else
	{
		sCheckFolders += "OK: "+sDirWinst95+" vorhanden.\r\n";
		GetDlgItem(IDC_F1_OK)->ShowWindow(SW_SHOW);
	}
	
	
	// folder "C:\INSTALL" exists ??
	if((BOOL)CreateDirectory(sDirInstall,0))
	{
		sCheckFolders += "FEHLER: "+sDirInstall+" nicht vorhanden.\r\n";
		RemoveDirectory(sDirInstall);
		GetDlgItem(IDC_F2_NOT)->ShowWindow(SW_SHOW);
	}
	else
	{
		sCheckFolders += "OK: "+sDirInstall+" vorhanden.\r\n";
		GetDlgItem(IDC_F2_OK)->ShowWindow(SW_SHOW);
	}
	
	

//	CFile jpgfile("C:\\OUT\\test.jpg", CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
//	CFile htmfile("C:\\OUT\\test.htm", CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	
	// folder "C:\OUT" exists ??
	if((BOOL)CreateDirectory(sDirOut,0))
	{
		sCheckFolders += "FEHLER: "+sDirOut+" nicht vorhanden.\r\n";
		RemoveDirectory(sDirInstall);
		GetDlgItem(IDC_F3_NOT)->ShowWindow(SW_SHOW);
	}
	else
	{
		sCheckFolders += "OK: "+sDirOut+"     vorhanden.\r\n";
		BOOL m_bJpgHtmExists = true;

		CFileFind filesearch;
	
		if(filesearch.FindFile("C:\\out\\*.htm", 0) != 0)
		{
			sCheckFolders += "      -- OK: In ""C:\\OUT"" mindestens eine "
							 """*.htm"" -  Datei vorhanden.\r\n";
		}
		else
		{
			sCheckFolders += "      -- FEHLER: In ""C:\\OUT"" keine "
							 """*.htm"" - Datei vorhanden.\r\n";
			m_bJpgHtmExists = false;
		};

		if(filesearch.FindFile("C:\\out\\*.jpg", 0) != 0)
		{
			sCheckFolders += "      -- OK: In ""C:\\OUT"" mindestens eine "
							 """*.jpg"" - Datei vorhanden.\r\n";
		}
		else
		{
			sCheckFolders += "      -- FEHLER: In ""C:\\OUT"" keine "
							 """*.jpg"" - Datei vorhanden.\r\n";
			m_bJpgHtmExists = false;
		}

		if(m_bJpgHtmExists)
		{
			GetDlgItem(IDC_F3_OK)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_F3_NOT)->ShowWindow(SW_SHOW);
		}
	}
	
	
	// delete folder "C:\log"
	if((BOOL)RemoveDirectory(sDirlog))
	{
		sCheckFolders += "OK: "+sDirlog+" gelöscht.\r\n";
		GetDlgItem(IDC_F4_OK)->ShowWindow(SW_SHOW);
	}
	else
	{
		sCheckFolders += "FEHLER: "+sDirlog+" war nicht vorhanden.\r\n";
		GetDlgItem(IDC_F4_NOT)->ShowWindow(SW_SHOW);
	};
	
	
	// write LogText to LogFile

	// create LogFile "EndControl.txt"
	CString sLogFile = "C:\\EndControl.txt";

	TRY
	{
		CFile fp(sLogFile, CFile::modeCreate | CFile::modeWrite); 
		fp.Write(sCheckFolders, sCheckFolders.GetLength());
		fp.Close();
	}
	CATCH(CFileException, e)
	{
#ifdef _DEBUG      
		afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
	}
	END_CATCH
		
*/
return 0;		
}
//////////////////////////////////////////////////////////////////////
BOOL CEndcontrolCheck::GetSecurityLauncherOK()
{
	return m_bSecurityLauncherOK;
}
//////////////////////////////////////////////////////////////////////
CString CEndcontrolCheck::GetDrivesChanged()
{
	return m_sDrivesChanged;
}
