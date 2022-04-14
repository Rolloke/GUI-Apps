// IPCDataCarrierClient.cpp: implementation of the CIPCDataCarrierClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCDataCarrierClient.h"
#include "DVClient.h"
#include "CPanel.h"
#include "MainFrame.h"
#include "PlayRealTime.h"
#include "Server.h"

extern CDVClientApp theApp;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDataCarrierClient::CIPCDataCarrierClient(LPCTSTR shmName, CServer* pServer)
 :CIPCDataCarrier(shmName, FALSE)
{
	m_pServer		= pServer;
#ifdef _UNICODE
	m_wCodePage = pServer->GetDataCarrierCodePage();
#endif
	m_pBackupDrive	= NULL;
	StartThread();
}

CIPCDataCarrierClient::~CIPCDataCarrierClient()
{
	WK_DELETE(m_pBackupDrive);
	StopThread();
}


//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnReadChannelFound()
{
	DoRequestConnection();
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmConnection()
{
	WK_TRACE(_T("Connection to ACDC established\n"));
	
	//ask for installed burn software
//TODO tkr _T("Burn Burnsoftware") noch als ENUM deklarieren in CIPCstringdefs.h
	DoRequestGetValue(SECID_NO_ID,_T("Burn Software"),0);
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnRequestDisconnect()
{
	WK_TRACE(_T("Connection to ACDC canceled\n"));
	theApp.SetACDCBackupDrive(FALSE);
	DelayedDelete();
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmVolumeInfos(int iNumDrives, const CIPCDrive drives[])
{
	//this function is called only when a CD/DVD drive exists physically in the system
	//if not, OnIndicateError is called 
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel == NULL)
	{
		return;
	}

	CString sMsg;
	//found medium in CD/DVD drive

//TODO TKR hier noch handlen, was passieren soll, wenn mehrere Drives gefunden werden
	//Test mit 2 Laufwerken (Brennern) durchführen
	if(iNumDrives == 1)
	{	
		m_pBackupDrive = new CIPCDrive(drives[0]);

		DWORD dwType = drives[0].GetType();
		DWORD dwTotalFreeMB = drives[0].GetMB();
		DWORD dwUsedMB = drives[0].GetUsedBytes().GetInMB();
		BOOL bDoBackup = TRUE;

		//ist erkanntes Medium eine DVD
		if(   dwType == DRIVE_DVD_RW
		   || dwType == DRIVE_DVD_R)
		{
			
			//ist DVD Backup freigeschaltet
			if(pPanel->IsDVDBackupAllowedByDongle())
			{
				//alle ok
			}
			else
			{
				WK_TRACE(_T("Backup to DVD is not allowed by dongle\n"));
				bDoBackup = FALSE;
			}
		}
		
		if(bDoBackup)
		{
			if(dwTotalFreeMB == 0)
			{
				//medium in not writable
				WK_TRACE(_T("Backup: Error1 - no writable medium in drive\n"));
				//lParam == 1 und wParam == 1 -> Error
				pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 1);
			}
			else
			{

				//den freien Speicherplatz auf dem Backup Zwischenspeicher laufwerk
				//prüfen. Ist dieser kleiner als Kapazität des Backup Mediums (CD,DVD) 
				//dann die Kapazität für Zwischenspeicherung anpassen

				CString sTempLocation;
				CString sTempDrive;
				LPTSTR sBuffer = sTempLocation.GetBufferSetLength(MAX_PATH);
				GetTempPath(MAX_PATH, sBuffer);
				GetLongPathName(sBuffer, sBuffer, MAX_PATH);
				sTempLocation.ReleaseBuffer();
				
				if(!sTempLocation.IsEmpty())
				{
					sTempDrive = sTempLocation.Left(3);
				}
				else
				{
					sTempDrive = _T("C:\\");
					WK_TRACE(_T("### kein Temp Pfad verfügbar, C:\\ als default gesetzt\n"));
				}
				CIPCDrive DriveForTempCopy(sTempDrive, DVR_SYSTEM_DRIVE);
				//CIPCDrive DriveForTempCopy(_T("C:\\"), DVR_SYSTEM_DRIVE);
				DWORD dwReserveMB = 300; //300 MB Reserveplatz auf Backup Zwischenspeicher LW garantieren
				DriveForTempCopy.CheckSpace();
				DWORD dwMBC = 0;
				dwMBC = DriveForTempCopy.GetFreeMB();
				
				if(dwMBC > dwReserveMB)
				{
					//ist Kapazität von Drive Backup Zwischenspeicher LW
					//größer als freie Kapazität auf Backup Medium
					if((dwMBC - dwReserveMB) < dwTotalFreeMB)
					{
						ULONGLONG		ullMediumFreeCapacity = 0;
						ULARGE_INTEGER  uliTotalBytes;
						ULONGLONG ullNewSize = dwMBC - dwReserveMB;
						ullMediumFreeCapacity = ullNewSize * 1024 * 1024;
						uliTotalBytes.QuadPart	= ullMediumFreeCapacity;
						
						//setzte an Drive Backup Zwischenspeicher LWangepasste, 
						//verkleinerte freie Kapazität des Mediums
						m_pBackupDrive->SetTotalNumberOfBytes(uliTotalBytes);
						WK_TRACE(_T("### CD Capacity begrenzt auf %d MB, da auf %s nur noch %d MB frei.\n"), (dwMBC - dwReserveMB), sTempDrive, dwMBC);
					}
				}
				else
				{
					//drive Backup Zwiscjenspeicherung has no space left
					WK_TRACE(_T("Backup: Error4 - no free space left for backup on drive %sn"),sTempDrive);
					//lParam == 1 und wParam == 1 -> Error
					pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 1);
					bDoBackup = FALSE;
				}
				
				if(bDoBackup)
				{
					if(dwUsedMB == 0)
					{	
						WK_TRACE(_T("Backup: Writable medium, start input cameras\n"));
						//lParam und wParam == 0 -> CD is writable
						pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 0, 0);					
					}
					else
					{
						if(dwType == DRIVE_CD_RW)
						{
							WK_TRACE(_T("Backup: CD-RW is not empty, Erase CD-RW"));
							//lParam == 1 und wParam == 0 -> not empty CD/RW
							pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 0);
						}
						else if(dwType == DRIVE_DVD_RW)
						{
							WK_TRACE(_T("Backup: DVD+RW is not empty, Erase DVD+RW"));
							//lParam == 1 und wParam == 0 -> not empty CD/RW
							pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 0);
						}
						else
						{	
							//medium in not writable
							WK_TRACE(_T("Backup: Error2 - no writable medium in drive\n"));
							//lParam == 1 und wParam == 1 -> Error
							pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 1);
						}
					}
				}
			}
		}
		else
		{
			//medium in not writable
			WK_TRACE(_T("Backup: Error5 - DVD backup not allowed\n"));
			//lParam == 1 und wParam == 2 -> Error, DVD not allowed
			pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 2);
		}
	}
	else
	{	
		//medium in not writable
		WK_TRACE(_T("Backup: Error3 - no writable medium in drive\n"));
		//lParam == 1 und wParam == 1 -> Error
		pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 1, 1);
	}	
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CBackupWindow* pBW = pMF->GetBackupWindow();
		if(pBW)
		{
			pBW->SessionProgress(dwSessionID, iProgress);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmSession(DWORD dwSessionID)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CBackupWindow* pBW = pMF->GetBackupWindow();
		if(pBW)
		{
			pBW->ConfirmSession(dwSessionID);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnIndicateError(DWORD dwCmd, 
											CSecID id, 
											CIPCError errorCode, 
											int iUnitCode,
											const CString &sErrorMsg)
{
	if(iUnitCode == DC_ERROR_NO_MEDIUM) //Medium not found, Error obtaining CD info
						 //kann igroriert werden, da auf das Einlegen eines Mediums 
						 //gewartet wird
	{

	}
	else	//error Meldung auswerten und melden
	{
		if(iUnitCode == DC_ERROR_DRIVE_NOT_FOUND) //Drive was not found, Error obtaining CD info
		{
			theApp.SetACDCBackupDrive(FALSE);
		}
		CMainFrame *pMF = theApp.GetMainFrame();
		if (pMF)
		{
			CBackupWindow* pBW = pMF->GetBackupWindow();
			if(pBW)
			{
				if(iUnitCode == DC_ERROR_BURN_PROCESS_FAILED) //Drive failed to get mediums capacity, stop backup
				{
					WK_TRACE(_T("Backup: Error0 - Drive failed to get mediums capacity.\n"));
					//lParam == 2 und wParam == 2 -> Error, Drive failed to get mediums capacity
					CPanel *pPanel = theApp.GetPanel();
					if (pPanel)
					{
						pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 2, 2);				
					}
				}
				pBW->IndicateACDCError(dwCmd, id, errorCode, iUnitCode, sErrorMsg);
			}
		}
	}		
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmCancelSession(DWORD dwSessionID)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CBackupWindow* pBW = pMF->GetBackupWindow();
		if(pBW)
		{
			pBW->ConfirmCancelSession(dwSessionID);
		}	
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmDeleteVolume(const CString& sVolume)
{
	
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmGetValue(CSecID id, 
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData)
{
	//ist ein Backup mit ACDC möglich (ist Brennsoftware vorhanden)
	if(sValue == _T("BP_NOPRODUCER"))
	{
		TRACE(_T("OnConfirmGetValue(): Cannot backup with ACDC.exe\n"));
		theApp.SetBackupWithACDC(FALSE);
		theApp.SetACDCBackupDrive(FALSE);
	}
	else
	{
		theApp.SetBackupWithACDC(TRUE);

		//ist ein brenner angeschlossen
		if(sValue == _T("NO_DRIVE"))
		{
			theApp.SetACDCBackupDrive(FALSE);
		}
		else
		{
			theApp.SetACDCBackupDrive(TRUE);
			//frage ACDC nach Infos über den vorhandenen Brenner
			DoRequestVolumeInfos();
		}
	}
}

//////////////////////////////////////////////////////////////////////
CIPCDrive* CIPCDataCarrierClient::GetBackupDrive()
{
	return m_pBackupDrive;
}
