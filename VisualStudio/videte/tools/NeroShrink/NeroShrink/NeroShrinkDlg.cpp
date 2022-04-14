// NeroShrinkDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "NeroShrink.h"
#include "NeroShrinkDlg.h"
#include ".\neroshrinkdlg.h"
#include "direct.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CNeroShrinkDlg Dialogfeld



CNeroShrinkDlg::CNeroShrinkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeroShrinkDlg::IDD, pParent)
	, m_sErrorText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNeroShrinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ERRORTXT, m_sErrorText);
}

BEGIN_MESSAGE_MAP(CNeroShrinkDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CNeroShrinkDlg Meldungshandler

BOOL CNeroShrinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CNeroShrinkDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CNeroShrinkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNeroShrinkDlg::OnBnClickedButton1()
{
	if(CopyNecessaryNeroFiles())
	{
		
	}
}
CString CNeroShrinkDlg::GetNeroPath(CString sToFind)
{
	BOOL bRet = FALSE;
	CString sNeroPath;
	CString sNeroApi(_T("NeroApi.dll"));
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
			sToFind, 
			NULL, 
			&dwType, 
			NULL,
			&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(namekey, 
				sToFind, 
				NULL, 
				&dwType,
				(LPBYTE)sStrValue.GetBuffer(dwCount/sizeof(TCHAR)), 
				&dwCount);
			//sStrValue holds the path to the NeroApi file

		}
		RegCloseKey(namekey);

		sNeroPath.Format(_T("%s"), sStrValue);
		sStrValue.ReleaseBuffer();
	}
	return sNeroPath;
}
BOOL CNeroShrinkDlg::DoesFileExist(LPCTSTR szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
BOOL CNeroShrinkDlg::CopyNecessaryNeroFiles()
{
	//m_sErrorText = _T("alles ok\n");
	CString sTempPath;
	CString sFindApi = _T("NeroApi");
	CString sFindLib = _T("Lib");
	CString sNeroPath = GetNeroPath(sFindApi);
	CString sLibPath = GetNeroPath(sFindLib);
	CString sLibFile1, sLibFile2;
	CString sNeroPathRename;
	CString sNeroLibRename;
	BOOL bSuccess = TRUE;

	if(sLibPath.IsEmpty())
	{	
		CString sDriveLocker(_T("DriveLocker"));
		sLibPath = GetNeroPath(sDriveLocker);
		if(sLibPath.IsEmpty())
		{
			//fehler, Nero Version zu alt
			MessageBox("Nero Version zu alt. Mind. Version 6.0.0.19. ist erforderlich.\n\nNeroShrink kann nicht ausgeführt werden.",
				NULL,
				MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
		else
		{
			sLibPath= sLibPath.Left(sLibPath.ReverseFind('\\'));
		}

	}

	CStringArray files;

	files.Add(_T("NeroAPI.dll"));
	files.Add(_T("NeroErr.dll"));
	files.Add(_T("NeroSCSI.dll"));
	files.Add(_T("newtrf.dll"));
	files.Add(_T("NeRSDB.dll"));
	files.Add(_T("WNASPI32.dll"));
	files.Add(_T("image.dll"));
	files.Add(_T("MMC.dll"));
	files.Add(_T("Geniso.dll"));
	files.Add(_T("Nero.txt"));
	files.Add(_T("NeroHistory.log"));
	files.Add(_T("Nerodeu.txt"));
	files.Add(_T("CDROM.dll"));
	files.Add(_T("CDROM.CFG"));

	sLibFile1 = _T("DriveLocker.dll");
	sLibFile2 = _T("AdvrCntr.dll");

	if(!sNeroPath.IsEmpty() && !sLibPath.IsEmpty())
	{
		//lege Temp-Verzeichnis der Art "Programme\Ahead1" an
		sTempPath = sNeroPath.Left(sNeroPath.ReverseFind('\\'));
		sTempPath = sTempPath.Left(sTempPath.ReverseFind('\\'));
		sTempPath = sTempPath + _T("\\Ahead1");
		sNeroPathRename = sTempPath;
	
		if(!DoesFileExist(sTempPath))
		{
			bSuccess = CreateDirectory(sTempPath,NULL);
		}

		if(bSuccess)
		{
			CString sFile;
			sTempPath = sTempPath + _T("\\Nero");
			if(!DoesFileExist(sTempPath))
			{
				bSuccess = CreateDirectory(sTempPath,NULL);
			}

			if(bSuccess)
			{	
				//kopiere für Nero minimal notwendige Dateien
				for(int i=0; bSuccess && i<files.GetSize(); i++)
				{
					sFile = files[i];
					bSuccess = CopyFile(sNeroPath + _T("\\") + sFile, sTempPath + _T("\\") + sFile,FALSE);
					if(!bSuccess)
					{
						if(0==sFile.CompareNoCase(_T("Nerodeu.txt"))) //nerodeu.txt darf bei engl. version fehlen
						{
							m_sErrorText.Format(_T("CopyFile %s\\%s -> %s\\%s failed: %s\n"), sNeroPath, sFile, sTempPath, sFile, GetLastErrorString());
						}
					}
				}
			}
			else
			{
				m_sErrorText.Format(_T("CreateDirectory %s failed: %s\n"), sTempPath, GetLastErrorString());
			}


			//lege temp Verzeichnis der Art "C:\Programme\Gemeinsame Dateien\Ahead1" an
			sTempPath = sLibPath.Left(sLibPath.ReverseFind('\\'));
			sTempPath = sTempPath.Left(sTempPath.ReverseFind('\\'));
			sTempPath = sTempPath + _T("\\Ahead1");
			sNeroLibRename = sTempPath;

			if(!DoesFileExist(sTempPath))
			{
				bSuccess = CreateDirectory(sTempPath,NULL);
				if(!bSuccess)
				{
					m_sErrorText.Format(_T("CreateDirectory %s failed: %s\n"), sTempPath, GetLastErrorString());
				}
			}
		
			sTempPath = sTempPath + _T("\\Lib");
			if(!DoesFileExist(sTempPath))
			{
				bSuccess = CreateDirectory(sTempPath,NULL);
				if(!bSuccess)
				{
					m_sErrorText.Format(_T("CreateDirectory %s failed: %s\n"), sTempPath, GetLastErrorString());
				}
			}

			for (int i=1; bSuccess && i<3; i++)
			{
				switch(i)
				{
				case 1: sFile = sLibFile1;break;
				case 2: sFile = sLibFile2;break;
				default: sFile = _T("");break;
				}
				bSuccess = CopyFile(sLibPath + _T("\\") + sFile, sTempPath + _T("\\") + sFile,FALSE);
				if(!bSuccess)
				{
					m_sErrorText.Format(_T("CopyFile %s\\%s -> %s\\%s failed: %s\n"), sLibPath, sFile, sTempPath, sFile, GetLastErrorString());
				}
			}
		}
		else
		{
			m_sErrorText.Format(_T("CreateDirectory %s failed: %s\n"), sTempPath, GetLastErrorString());
		}
	}
	BOOL bRet = TRUE;
	if(bSuccess)
	{
		CString sError;
		CString sLibPathToDelete = sLibPath.Left(sLibPath.ReverseFind('\\'));
		CString sNeroPathToDelete = sNeroPath.Left(sNeroPath.ReverseFind('\\'));

//		sError.Format(_T("1 DelFiles %s failed: %s\n"), sLibPathToDelete, GetLastErrorString());
//		m_sErrorText += sError;
//		UpdateData(FALSE);
		if(!DelFiles(sLibPathToDelete))
		{
//			sError.Format(_T("DelFiles %s failed: %s\n"), sLibPathToDelete, GetLastErrorString());
//			m_sErrorText += sError;
			bRet = FALSE;
		}

		if(!MoveFile(sLibPathToDelete+_T("1\\Lib"), sLibPathToDelete + _T("\\Lib")))
		{
//			sError.Format(_T("1 MoveFile %s1\\Lib -> %s\\Lib failed: %s\n"), sLibPathToDelete, sLibPathToDelete, GetLastErrorString());
//			m_sErrorText += sError;
			UpdateData(FALSE);
			bRet = FALSE;
		}

//		sError.Format(_T("1 RemoveDirectory %s1 failed: %s\n"), sLibPathToDelete, GetLastErrorString());
//		m_sErrorText += sError;
		if(!RemoveDirectory(sLibPathToDelete+_T("1")))
		{
//			sError.Format(_T("RemoveDirectory %s1 failed: %s\n"), sLibPathToDelete, GetLastErrorString());
//			m_sErrorText += sError;
			bRet = FALSE;
		}

//		sError.Format(_T("2 DelFiles %s failed: %s\n"), sNeroPathToDelete, GetLastErrorString());
//		m_sErrorText += sError;
		if(!DelFiles(sNeroPathToDelete))
		{
//			sError.Format(_T("DelFiles %s failed: %s\n"), sNeroPathToDelete, GetLastErrorString());
//			m_sErrorText += sError;
			bRet = FALSE;
		}

		if(!MoveFile(sNeroPathToDelete+_T("1\\Nero"), sNeroPathToDelete + _T("\\Nero")))
		{
//			sError.Format(_T("2 MoveFile %s1\\Nero -> %s\\Nero failed: %s\n"), sNeroPathToDelete, sNeroPathToDelete, GetLastErrorString());
//			m_sErrorText += sError;
			bRet = FALSE;
		}
		sError.Format(_T("RemoveDirectory %s1 failed: %s\n"), sNeroPathToDelete, GetLastErrorString());
		m_sErrorText += sError;
		if(!RemoveDirectory(sNeroPathToDelete+_T("1")))
		{
//			sError.Format(_T("RemoveDirectory %s1 failed: %s\n"), sNeroPathToDelete, GetLastErrorString());
//			m_sErrorText += sError;
			bRet = FALSE;
		}
	}
	
	//UpdateData(FALSE);
	if(bSuccess && bRet)
	{
		//alles OK
		MessageBox("NeroShrink erfolgreich ausgeführt.",
			NULL,
			MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		//fehler
		MessageBox("NeroShrink nicht erfolgreich ausgeführt.",
			NULL,
			MB_OK|MB_ICONINFORMATION);
	}
	return bSuccess;
}
BOOL CNeroShrinkDlg::DelFiles(CString sdir)
{
	CFileFind test;
	CString name;
	BOOL bIsDot, bIsDir;

	BOOL ok=test.FindFile(sdir+_T("\\*.*"));

	while(ok)
	{
		ok=test.FindNextFile();
		name=test.GetFileName();
		TRACE(_T("%s\n"),name);

		bIsDot=test.IsDots();
		bIsDir=test.IsDirectory();

		if(!bIsDot  && !bIsDir)
		{
			SetFileAttributes(sdir+_T("\\")+name,FILE_ATTRIBUTE_NORMAL);
			BOOL bDeleted = FALSE;
			bDeleted = DeleteFile(sdir+_T("\\")+name);
		}
		if((!test.IsDots())&&(test.IsDirectory()))
		{
			DelFiles(sdir+_T("\\")+name);
			_tchdir(sdir);
			_trmdir(name);
		}

	}


	return TRUE;
}
