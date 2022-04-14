// DirectCDStart.cpp: implementation of the CDirectCDStart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WatchCD.h"
#include "DirectCDStart.h"
#include "WatchCDDlg.h"
#include "DirectCDError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CDirectCDStart* m_pThis;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectCDStart::CDirectCDStart(CWatchCDDlg* pWatchCD, CDirectCDError* pDCDError)

{

	m_pWatchCD				= pWatchCD;
	m_pDCDError				= pDCDError;

	m_pThis					= this;
	m_DCDStartStatus		= START_EXEC_DIRECTCD;
	m_hDCDWatchDog			= NULL;
	m_hDCDWelcomeDlg		= NULL;
	m_hDCDDriveInfoDlg		= NULL;
	m_hNextButton			= NULL;
	m_hDeviceField			= NULL;
	m_hFinalizeFinishButton = NULL;
	
	//welche DirectCD Version ist installiert
	m_bDirectCDVersion5		= pWatchCD->IsDirectCDVersion5();

	if(!m_bDirectCDVersion5)
	{
		m_sPathToDirectCD = m_pWatchCD->GetPathToDirectCD();
		InitLanguageStrings();	

		m_bDeviceIsLocked			= FALSE;
		m_dwTick					= GetTickCount();
		m_dwTick_WaitForEmptyCDR	= GetTickCount();

		//ID next-button on Welcome and DriveInfo dialogs
		m_dwIDButtonNext	= 0x0045E;

		//ID Device Status Field on DriveInfo dialog
		m_dwIDDeviceField	= 0x003FB;


		//ID finish-button on Finalize dialog
		switch(m_pWatchCD->GetDirectCDVersion())
		{
		case DCD_VERSION_26:
			m_dwIDFinalizeFinishButton = 0x0004;
			break;

		case DCD_VERSION_30:
			m_dwIDFinalizeFinishButton = 0x00B1;
			break;

		default:
			m_dwIDFinalizeFinishButton = 0xFFFF;
			break;
		}
	}
	else
	{
		//DirectCD Version 5
		m_sPathToDirectCDVersion5	= m_pWatchCD->GetPathToDirectCDVersion5();
		m_dwTick					= GetTickCount();
		m_dwTick_WaitForEmptyCDR	= GetTickCount();
		m_hFormatCDDialogVersion5	= NULL;
		m_hCDReadyDialogVersion5	= NULL;
		m_sWndTitleWithDriveVersion5 = m_pWatchCD->GetDCDWndTitleWithDriveVersion5() + " ";
		m_hCDReadyDialogTextVersion5 = NULL;
		m_dwCDReadyDialogTextVersion5 = 0x0456;		//static text "CD Ready"
	}
}

CDirectCDStart::~CDirectCDStart()
{

}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::InitLanguageStrings()
{
	//in der Datei DirectCD.loc steht jeweils die nicht englische Variante aller in
	//DirectCD angezeigten Texte, wenn DirectCD in iner anderen Sprache als englisch
	//installiert wurde.
	//Alle englischen Texte befinden sich in DirectCD.exe selbst.
	//Da DirectCD im Hintergrund läuft, und egal welche Sprache gewählt wurde, sich die
	//englische Variante IMMER in DirectCD.exe befindet, reicht es aus, die Daten aus
	//DirectCD.exe selbst zu holen

	CString sLoc(_T("directcd.loc"));
	CString sExe(_T("directcd.exe"));


	if(m_pWatchCD->FileExists(m_sPathToDirectCD, sLoc))
	{
		m_sDeviceNotReady = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
															_T("directcd.loc"), 410);
		m_sDeviceIsReady = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
															_T("directcd.loc"), 401);

		m_sDeviceIsReady.Replace(_T("%s"), m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
														 _T("directcd.loc"), 106));

		m_sEjecting = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													_T("directcd.loc"), 131);
		
		m_sUnableToLock = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													_T("directcd.loc"), 637);

		m_sDirectCDDisc = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													_T("directcd.loc"), 404);
	}
	else
	{
		m_sDeviceNotReady = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
														_T("directcd.exe"), 410);
		
		m_sDeviceIsReady = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
														_T("directcd.exe"), 401);

		m_sDeviceIsReady.Replace(_T("%s"), m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													 _T("directcd.exe"), 106));

		m_sEjecting = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													_T("directcd.exe"), 131);
		
		m_sUnableToLock = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													_T("directcd.exe"), 637);

		m_sDirectCDDisc = m_pWatchCD->GetStringFromTable(m_sPathToDirectCD,
													_T("directcd.exe"), 404);
	}
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::ResetDCDStartStatus()
{
	DirectCDStatus LastDCDStartStatus = m_DCDStartStatus;
	m_DCDStartStatus = START_EXEC_DIRECTCD;

	WK_TRACE(_T("CDirectCDStart() ResetDCDStartStatus: from last %s to new %s\n")
			,NameOfEnumDirectCD(LastDCDStartStatus)
			,NameOfEnumDirectCD(m_DCDStartStatus));
}


//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::ExecDirectCD()
{
	BOOL bRet = FALSE;


	BOOL bProcess;
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sPrgToStart;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_HIDE;
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sExe = m_sPathToDirectCD + _T("\\directcd.exe");
	WK_TRACE(_T("Path to Direct CD: %s\n"), sExe);
	bProcess = CreateProcess(
				(LPCTSTR)sExe,				// pointer to name of executable module 
				(LPTSTR)sExe.GetBuffer(0),		// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);
	sExe.ReleaseBuffer();	

	if (bProcess && prozessInformation.hProcess != NULL)
	{
		DWORD dwWFII;
		dwWFII = WaitForInputIdle(prozessInformation.hProcess,15*1000);
		if(dwWFII == WAIT_TIMEOUT || dwWFII == -1)
		{
			return bRet;
		}
		bRet = TRUE;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::ExecDirectCDVersion5()
{
	BOOL bRet = FALSE;


	BOOL bProcess;
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sPrgToStart;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_HIDE;
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sExe = m_sPathToDirectCDVersion5 + _T("\\directcd.exe");
	WK_TRACE(_T("Path to Direct CD Version5: %s\n"), sExe);
	bProcess = CreateProcess(
				(LPCTSTR)sExe,				// pointer to name of executable module 
				(LPTSTR)sExe.GetBuffer(0),		// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);
	sExe.ReleaseBuffer();	

	if (bProcess && prozessInformation.hProcess != NULL)
	{
		DWORD dwWFII;
		dwWFII = WaitForInputIdle(prozessInformation.hProcess,15*1000);
		if(dwWFII == WAIT_TIMEOUT || dwWFII == -1)
		{
			return bRet;
		}
		bRet = TRUE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::IsStarted()
{
	BOOL bRet = FALSE;
	Sleep(20);
	switch(m_DCDStartStatus)
	{

	case START_EXEC_DIRECTCD:
		//start directCD 
		OnStartExecDirectCD();
		break;

	case START_FIND_WELCOME_DLG:
		OnStartFindWelcomeDlg();
		break;

	case START_FIND_DRIVEINFO_DLG:
		OnStartFindDriveInfoDlg();
		break;

	case START_IS_STARTED:
		m_dwTick_WaitForEmptyCDR = GetTickCount();
		ChangeDCDStartStatus(START_WAIT_FOR_EMPTY_CDR);
		//weiter mit START_WAIT_FOR_EMPTY_CDR in IsEmptyCD()
		bRet = TRUE;
		break;

	case START_ERROR:
		OnStartError();
		break;

	default:
		WK_TRACE(_T("CDirectCDStart::IsStarted(): undefined status\n"));
		ChangeDCDStartStatus(START_ERROR);
		break;

	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartExecDirectCD()
{
	if(!m_bDirectCDVersion5)
	{
		m_hDCDWatchDog = FindWindow(NULL, _T("Adaptec DirectCD Wizard"));

		if(IsWindow(m_hDCDWatchDog))
		{
			if(ExecDirectCD())
			{
				m_dwTick = GetTickCount();
				ChangeDCDStartStatus(START_FIND_WELCOME_DLG);
			}
			else
			{
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_DCD_NOT_STARTED);
			}
		}
		else
		{
			if(GetTimeSpan(m_dwTick) > 10*1000)
			{
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_DCD_WATCHDOG_NOT_RUNNING);
			}
		}	
	}
	else
	{
		//DirectCD Version5
		m_hDCDWatchDog = FindWindow(NULL, _T("DirectCD"));

		if(IsWindow(m_hDCDWatchDog))
		{
			if(ExecDirectCDVersion5())
			{
				m_dwTick = GetTickCount();
				ChangeDCDStartStatus(START_IS_STARTED);
			}
			else
			{
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_DCD_NOT_STARTED);
			}
		}
		else
		{
			if(GetTimeSpan(m_dwTick) > 10*1000)
			{
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_DCD_WATCHDOG_NOT_RUNNING);
			}
		}
	}

}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartFindWelcomeDlg()
{
	if(FindWelcomeDlg())
	{
		//find next-button
		LPARAM lParam = 0;
		if(!EnumChildWindows(m_hDCDWelcomeDlg, (WNDENUMPROC)EnumChildProc, lParam))
		{
			//click button "&Next >" in Welcome-Dialog
			::PostMessage(m_hDCDWelcomeDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwIDButtonNext,BN_CLICKED),
					(LPARAM)m_hNextButton);	

			m_dwTick = GetTickCount();
			ChangeDCDStartStatus(START_FIND_DRIVEINFO_DLG);
		}
	}	
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartFindDriveInfoDlg()
{
	if(FindDriveInfoDlg())
	{
		//find Device Status field
		LPARAM lParam = 1;
		if(!EnumChildWindows(m_hDCDDriveInfoDlg, (WNDENUMPROC)EnumChildProc, lParam))
		{
			ChangeDCDStartStatus(START_IS_STARTED);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartOpenFormatDlg()
{
	if(!m_bDirectCDVersion5)
	{
		if(OpenNextDlg())
		{
			ChangeDCDStartStatus(START_IS_EMPTY_CDR);
		}

	}
	else
	{
		//DirectCD Version 5 
		
	}

}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::OpenNextDlg()
{
	BOOL bRet = FALSE;

	//find next-button
	LPARAM lParam = 0;
	if(!EnumChildWindows(m_hDCDDriveInfoDlg, (WNDENUMPROC)EnumChildProc, lParam))
	{
		//click button "&Next >" in Drive-Info-Dialog
		::PostMessage(m_hDCDDriveInfoDlg,WM_COMMAND,
				(WPARAM)MAKELONG(m_dwIDButtonNext,BN_CLICKED),
				(LPARAM)m_hNextButton);	
		bRet = TRUE;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartError()
{
	m_pDCDError->SetError(TRUE);
}
//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::IsEmptyCDR()
{
	BOOL bRet = FALSE;
	Sleep(20);
	
	switch(m_DCDStartStatus)
	{

	case START_WAIT_FOR_EMPTY_CDR:
		OnStartWaitForEmptyCDR();
		break;

//	case START_WAIT_FOR_CDR_RELEASE:
//		OnStartWaitForCDRRelease();
//		break;

	case START_OPEN_FORMAT_DLG:
		OnStartOpenFormatDlg();
		break;

	case START_CLOSE_CDREADY_DLG_VERSION5:
		OnStartCloseCDReadyDlgVersion5();
		break;

	case START_IS_EMPTY_CDR:
		ResetDCDStartStatus();
		bRet = TRUE;
		break;

	case START_ERROR:
		OnStartError();
		break;

	default: 
		WK_TRACE(_T("CDirectCDStart::IsEmptyCDR(): undefined status\n"));
		ChangeDCDStartStatus(START_ERROR);
		break;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartWaitForEmptyCDR()
{
	if(!m_bDirectCDVersion5)
	{
		if(FindEmptyCDR())
		{
			ChangeDCDStartStatus(START_OPEN_FORMAT_DLG);
		}
	}
	else
	{
		//DirectCD Version5
		BOOL bFoundCDReadyDialogVersion5 = FALSE;
		if(FindEmptyCDRVersion5(bFoundCDReadyDialogVersion5))
		{
			TRACE(_T("Format Dialog DirectCD Version5 gefunden\n"));
			ChangeDCDStartStatus(START_IS_EMPTY_CDR);
		}
		else
		{
			if(!bFoundCDReadyDialogVersion5)
			{
//TODO TKR wenn der Format Dialog nach gewisser Zeit nicht gefunden wurde, wurde entweder
			//1. gar keine CD eingelegt
			//2. eine schon formatierte (CD Ready) CD liegt schon im LW
			//3. eine Read Only CD wurde eingelegt
			//4. eine Read Only CD war schon eingelegt

			//bei  1. ist auch der Eject Button deaktiviert
			//bei 2. bis 4. kann die nicht leere eingelegte CD wieder ausgeworfen werden

				TRACE(_T("weitersuchen\n"));
			}
			else
			{
				TRACE(_T("CD Ready Dialog DirectCD Version5 gefunden\n"));
				ChangeDCDStartStatus(START_CLOSE_CDREADY_DLG_VERSION5);
			}

		}
	}

}
//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartCloseCDReadyDlgVersion5()
{
	//TODO TKR hier weiter
	//CD Ready Dialog schließen und cd auswerfen
	::PostMessage(m_hCDReadyDialogVersion5, WM_CLOSE, 0, 0);
	Sleep(1000);
	TRACE(_T("CD Ready Dialog wieder geschlossen\n"));
	
}
//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OnStartWaitForCDRRelease()
{
	//dieser Zustand wurde zu Testzwecken für Yamaha Brenner benötigt.
	//Es kam mit einem solchen Brenner vor, daß nach Einlegen einer schon beschrieben
	//CD-R DirectCD beendet wurde, DirectCD selbst aber das CD-R Laufwerk gelockt hielt,
	//sodaß es per Auswurfknopfdruck nicht geöffnet werden konnte.
	//Als Lösungsansatz wurde in einem solchen Fall die CD-R "per Software" ausgeworfen.
	//DirectCD wurde erst danach geschlossen. Allerdings befand sich beim Erneuten Backup
	//DitectCD immer noch im Status "z.B. Read only CD ist eingelegt", sodaß der Backup-
	//Prozess sofort mit Meldung "nicht leerer Rohling eingelegt" beendet wird, obwohl gar
	//kein Rohling im Laufwerk liegt. 
	//Lösungsversuch hier, nach "softwaremäßigem" CD-R Auswurf warten, bis DirectCD neuen
	//Status feststellt. Klappte meist, aber nicht immer, sodaß nach einem Timeout
	//DirectCD trotzdem beendet wurde. Beim nächsten Backup wurde dann wieder ein 
	//nicht leere Rohling von DirectCD erkannt, obwohl keiner drin liegt.
	CString sText;
	if (m_hDeviceField && IsWindow(m_hDeviceField))
	{
		GetWindowText(m_hDeviceField,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
		
	//	OpenCDRDrive();
		ChangeDCDStartStatus(START_ERROR);
		m_pDCDError->ChangeErrorStatus(ERROR_FOUND_NOT_EMPTY_CDR);
	/*	
		if(sText.Left(19) == m_sDeviceNotReady)
		{
			//warte max. 10 sek. bis DirectCD erkannt hat, daß nicht leerer Rohling wieder
			//ausgeworfen wurde. Erst dann die Fehlermeldung generieren, daß eingelegte CD-R
			//nicht leer war
			WK_TRACE(_T("CDirectCDStart::OnStartWaitForCDRRelease(): Status = %s after %d sec.\n"), m_sDeviceIsReady, (GetTickCount()-m_dwTick));

		}
		else
		{
			if((GetTimeSpan(m_dwTick) >= 10*1000)
			{
				//timeout reached
						WK_TRACE(_T("CDirectCDStart::OnStartWaitForCDRRelease(): Timeout reached\n"));
	//			ChangeDCDStartStatus(START_ERROR);
	//			m_pDCDError->ChangeErrorStatus(ERROR_FOUND_NOT_EMPTY_CDR);
			}
		}
	*/
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::FindEmptyCDR()
{
	Sleep(2000);
	
	BOOL bRet = FALSE;
	CString sText;

	if (   m_hDeviceField
		&& IsWindow(m_hDeviceField))
	{
		GetWindowText(m_hDeviceField,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();

		if (sText.Left(m_sDeviceIsReady.GetLength()) == m_sDeviceIsReady)
		{
			WK_TRACE(_T("FindEmptyCDR device is ready\n"));
			bRet = TRUE;
		}
		else
		{
			if(sText.Left(15) != m_sDeviceNotReady.Left(15))
			{
				WK_TRACE(_T("CDirectCDStart::FindEmptyCDR(): TitleText: %s  ResoucreText: %s\n"),
											sText.Left(15), m_sDeviceNotReady.Left(15));
				if(sText.Left(15) == m_sDirectCDDisc.Left(15))
				{
					WK_TRACE(_T("CDirectCDStart::FindEmptyCDR(): Found DirectCD disk, Locked = TRUE\n"));
					m_bDeviceIsLocked = TRUE;
				}

				//not empty CD-R was inserted
				WK_TRACE(_T("CDirectCDStart::FindEmptyCDR(): Found not empty CDR\n"));
				m_dwTick = GetTickCount();
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_FOUND_NOT_EMPTY_CDR);

			}

			if(GetTimeSpan(m_dwTick_WaitForEmptyCDR) > 60*1000)
			{
				//timeout waiting for empty CD-R
				WK_TRACE(_T("CDirectCDStart::IsEmptyCDR(): Not found empty CDR\n"));
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_EMPTY_CDR);
			}

			//falls DVClient während des Wartens auf eine leere CDR abgeschaltet wird,
			//muß WatchCD.exe, wie auch DirectCD.exe beendet werden

			if(!m_pWatchCD->IsDVClient())
			{
				//Close WatchCD.exe
				WK_TRACE(_T("DVClient wurde beendet. Schließe DirectCD und WatchCD\n"));
				ChangeDCDStartStatus(START_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_DVCLIENT_NOT_RUNNING);
			}
		}
	}
	else
	{
		WK_TRACE(_T("FindEmptyCDR no window\n"));
	}
	return bRet;

}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::FindEmptyCDRVersion5(BOOL &bFoundCDReadyDialogVersion5)
{
	BOOL bRet = FALSE;
	bFoundCDReadyDialogVersion5 = FALSE; //wird TRUE, wenn CDReady Dialog gefunden wurde
	Sleep(2000);
	
	if(    m_hDCDWatchDog
	    && IsWindow(m_hDCDWatchDog))
	{
		//Finde FormatCD Dialog von DirectCD Version 5
		LPARAM lParam = 3; 
		if(!EnumDesktopWindows(NULL, (WNDENUMPROC)EnumChildProc, lParam))
		{
			TRACE(_T("TODO TKR: Setze Label im Format Dialog\n"));
			bRet = TRUE;
		}	
		else
		{
			TRACE(_T("Format Dialog noch nicht gefunden\n"));

			//sende zum Öffnen des Format Dialogs "Alt + F" an DirectCD Version 5 Hauptfenster
			::PostMessage(m_hDCDWatchDog,WM_COMMAND,
					(WPARAM)0x00018003,
					(LPARAM)0x00000000);

			//falls eine nicht abgeschlossene CD-R eingelegt wurde, wird der Dialog "CD Ready" angezeigt
			//finde "CD Ready" Dialog von DirectCD Version 5
			LPARAM lParam = 4; 
			if(!EnumDesktopWindows(NULL, (WNDENUMPROC)EnumChildProc, lParam))
			{
				//finde im CD Ready Dialog des static text "CD Ready" (ID: 0x0456) als
				//Sicherheit, dass es wirklich der CD Ready Dialog ist, denn im
				//EnumChildProc wird mit lParam = 4 nur nach dem WindowText "DirectCD 5.02c (Drive G:) "
				//gesucht. Diesen Titel tragen aber auch andere DirectCD Fenster
				TCHAR sText[128];
				::GetDlgItemText(m_hCDReadyDialogVersion5,
								 m_dwCDReadyDialogTextVersion5, 
								 sText, 
								 sizeof(sText));
				CString sCDReady(sText);
				if(sCDReady == _T("CD Ready"))
				{
					bFoundCDReadyDialogVersion5 = TRUE;
				}
				
			}
		}
	}
	else
	{
		WK_TRACE(_T("FindEmptyCDRVersion5 no window\n"));
	}
	
		
		//TODO TKR hier weiter
	//1. Prüfen ob DirectCD5 Hauptfenster da
	//2. Tastenkürzel alt+f an DirectCD Hauptfenster schicken (in Schleife alle paar Sekunden)
	//2.1. Vor jedem erneuten Tastenkürzel-Schicken prüfen, ob der FormatDLG da
	//2.2. Ist FormatDLG da den CDNamen dort eintragen

	//3. Vor dem erneuten Tastaturkürzel-Schicken prüfen, ob der CDReady DLG da
	//3.1. wenn ja, dann ist nicht leere CD-R eingelegt
	//3.2. bei nicht leerer CD-R Tastenkürzel alt+e schicken 
	//3.3. auf den DLG CD ejected warten
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::FindDriveInfoDlg()
{
	BOOL bRet = FALSE;

	HWND hWnd = ::FindWindow(NULL, m_pThis->m_pWatchCD->GetDCDWndTitleWithoutDrive());
	if (hWnd && IsWindow(hWnd))
	{
		CString sText;
		GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
		if(    (hWnd != m_hDCDWelcomeDlg)
			&& (sText == m_pWatchCD->GetDCDWndTitleWithoutDrive()))
		{
			m_hDCDDriveInfoDlg = hWnd;
			WK_TRACE(_T("Drive Info Dlg found\n"));
			bRet = TRUE;
		}
	}
		
/*
	LPARAM lParam = 0;
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
	TRACE(_T("Handle mit this: 0x%x\n"), m_pThis->m_hDCDDriveInfoDlg);
	TRACE(_T("Handle: 0x%x\n"), m_hDCDDriveInfoDlg);

	if(IsWindow(m_pThis->m_hDCDDriveInfoDlg))
	{
		TRACE(_T("IsWindow = TRUE\n"));
		bRet = TRUE;
	}
*/
	else
	{
		if((GetTimeSpan(m_dwTick)) > 30*1000)
		{
			//Drive Info Dialog not opened
			ChangeDCDStartStatus(START_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_DRIVEINFO_DLG);
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDirectCDStart::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{

	BOOL bRet = TRUE;
	
	switch(lParam)
	{
	case 0:
		//find Drive Information dialog
		if (hWnd && IsWindow(hWnd))
		{
			CString sText;
			GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
			sText.ReleaseBuffer();

			if(    (hWnd != m_pThis->m_hDCDWelcomeDlg)
				&& (sText == m_pThis->m_pWatchCD->GetDCDWndTitleWithoutDrive()))
			{

				m_pThis->m_hDCDDriveInfoDlg = hWnd;
				TRACE(_T("Drive Info Dlg gefunden: Handle: 0x%x\n"), m_pThis->m_hDCDDriveInfoDlg);
				bRet = FALSE;
			}
		}
		break;

	default:
		break;
	
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDirectCDStart::EnumChildProc(HWND hWnd, LPARAM lParam)
{
	BOOL bRet = TRUE;

	switch(lParam)
	{
	case 0:
		{
		//find next-button on Welcome-dialog
		if ((DWORD)::GetDlgCtrlID(hWnd) == m_pThis->m_dwIDButtonNext)
		{
			m_pThis->m_hNextButton = hWnd; 
			bRet = FALSE;
		}
		}
		break;

	case 1:
		{
		//find Device Status field in Drive-Information-dialog
		if((DWORD)::GetDlgCtrlID(hWnd) == m_pThis->m_dwIDDeviceField)
		{
			m_pThis->m_hDeviceField = hWnd;
			bRet = FALSE;
		}
		}
		break;

	case 2:
		//find Finish-Button on Finalize-Dialog
		{
		
		DWORD dwFinalizeButton = m_pThis->m_dwIDFinalizeFinishButton;
		if(dwFinalizeButton != 0xFFFF && (DWORD)::GetDlgCtrlID(hWnd) == dwFinalizeButton)
		{
			m_pThis->m_hFinalizeFinishButton = hWnd;
			bRet = FALSE;
		}
		}
		break;

	case 3:
		//finde Format Dialog von DirectCD Version 5
		{
		CWnd FormatDlg;
		CWnd *Window = FormatDlg.FromHandle(hWnd);
		CString sText;
		Window->GetWindowText(sText);

		if(sText == m_pThis->m_sWndTitleWithDriveVersion5 + _T("Format"))
		{
			m_pThis->m_hFormatCDDialogVersion5 = hWnd; 
			bRet = FALSE;
		}
		}
		break;

	case 4:
		//finde CD Ready Dialog von DirectCD Version 5
		{
		CWnd FormatDlg;
		CWnd *Window = FormatDlg.FromHandle(hWnd);
		CString sText;
		Window->GetWindowText(sText);

		if(sText == m_pThis->m_sWndTitleWithDriveVersion5)
		{
			m_pThis->m_hCDReadyDialogVersion5 = hWnd; 
			bRet = FALSE;
		}
		}
		break;
/*
	case 5:
		{
		//finde im CD Ready Dialog das Feld ID: 0x0456
		//dort muss "CD Ready" drin stehen
		if((DWORD)::GetDlgCtrlID(hWnd) == m_pThis->m_dwCDReadyDialogTextVersion5)
		{
			if()
			m_pThis->m_hCDReadyDialogTextVersion5 = hWnd;
			bRet = FALSE;
		}
		}
		break;
*/
	default:
		break;
	}

	return bRet;

}


//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::FindWelcomeDlg()
{
	BOOL bRet = FALSE;

	//get handle from DirectCD Welcome Dialog
	m_hDCDWelcomeDlg = FindWindow(_T("#32770"), m_pWatchCD->GetDCDWndTitleWithoutDrive());
	if (   m_hDCDWelcomeDlg 
	    && IsWindow(m_hDCDWelcomeDlg))
	{
		WK_TRACE(_T("found WelcomeDlg: %s\n"), m_pWatchCD->GetDCDWndTitleWithoutDrive());
		bRet = TRUE;
	}
	else
	{
		if (GetTimeSpan(m_dwTick) > 30*1000)
		{
			WK_TRACE_ERROR(_T("not found WelcomeDlg after timeout: %d\n"), GetTimeSpan(m_dwTick));
			//Welcome Dialog not opened
			ChangeDCDStartStatus(START_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_WELCOME_DLG);
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDStart::FindWelcomeDlgVersion5()
{
	BOOL bRet = FALSE;

	//get handle from DirectCD5 Main Dialog
	m_hDCDWelcomeDlg = FindWindow(_T("#32770"), m_pWatchCD->GetDCDWndTitleWithDriveVersion5());
	if (   m_hDCDWelcomeDlg 
	    && IsWindow(m_hDCDWelcomeDlg))
	{
		WK_TRACE(_T("found DirectCD5 Format Dialog: %s\n"), m_pWatchCD->GetDCDWndTitleWithDriveVersion5());
		bRet = TRUE;
	}
	else
	{
		if (GetTimeSpan(m_dwTick) > 10*1000)
		{
			WK_TRACE_ERROR(_T("not found DirectCD5 Format Dialog after timeout: %d\n"), GetTimeSpan(m_dwTick));
			//Welcome Dialog not opened
			ChangeDCDStartStatus(START_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_WELCOME_DLG);
		}
	}

	return bRet;
}

////////////////////////////////////////////////////////////////////////////
void CDirectCDStart::ChangeDCDStartStatus(DirectCDStatus DCDStartStatus)
{
	DirectCDStatus LastDCDStartStatus = m_DCDStartStatus;
	m_DCDStartStatus = DCDStartStatus;

	WK_TRACE(_T("CDirectCDStart() DCDStartStatus: from last %s to new %s\n")
			,NameOfEnumDirectCD(LastDCDStartStatus)
			,NameOfEnumDirectCD(m_DCDStartStatus));

}


//////////////////////////////////////////////////////////////////////
void CDirectCDStart::CloseStartDialogs()
{
	if(!m_bDirectCDVersion5)
	{
		if(IsWindow(m_hDCDWelcomeDlg))
		{
			::PostMessage(m_hDCDWelcomeDlg, WM_CLOSE, 0,0);
			WK_TRACE(_T("CDirectCDStart::CloseStartDialogs(): WelcomeDlg closed.\n"), m_hDCDWelcomeDlg);
		}

		if(IsWindow(m_hDCDDriveInfoDlg))
		{
			::PostMessage(m_hDCDDriveInfoDlg, WM_CLOSE, 0,0); 
			WK_TRACE(_T("CDirectCDStart::CloseStartDialogs(): DriveInfoDlg closed.\n"), m_hDCDDriveInfoDlg);
		}
	}
	else
	{
		if(IsWindow(m_hDCDWatchDog))
		{
			//schließt das hauptfensetr von DirectCD Version5
			::PostMessage(m_hDCDWatchDog, WM_CLOSE, 0,0);
			WK_TRACE(_T("CDirectCDStart::CloseStartDialogs(): Close DirectCD5. 0x%x\n"), m_hDCDWatchDog);
		}
	}

}


//////////////////////////////////////////////////////////////////////
DWORD CDirectCDStart::GetIDFinalizeFinishButton()
{
	return m_dwIDFinalizeFinishButton;
}

//////////////////////////////////////////////////////////////////////
HWND CDirectCDStart::GetHdlFinalizeFinishButton()
{
	return m_hFinalizeFinishButton;
}

//////////////////////////////////////////////////////////////////////
HWND CDirectCDStart::GetHdlDeviceField()
{
	return m_hDeviceField;
}

//////////////////////////////////////////////////////////////////////
HWND CDirectCDStart::GetHdlDriveInfoDlg()
{
	return m_hDCDDriveInfoDlg;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDStart::OpenCDRDrive()
{
	//open CD-R device
	CWK_Profile wkp;
	
	CString sDrive = m_pWatchCD->GetBackupDrive();
	TCHAR cDrive = sDrive.GetAt(0);
	CIPCDrive Drive(cDrive, wkp);

	if(Drive.OpenCD())
	{
		WK_TRACE(_T("CDirectCDStart::FindEmptyCDR(): Drive opened sucessfully\n"));
	}
	else
	{
		WK_TRACE(_T("CDirectCDStart::FindEmptyCDR(): Drive not opened\n"));
	}
}


