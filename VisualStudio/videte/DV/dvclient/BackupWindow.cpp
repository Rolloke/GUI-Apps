// BackupWindow.cpp: implementation of the CBackupWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "DisplayWindow.h"
#include "BackupWindow.h"
#include "PlayWindow.h"
#include "Server.h"
#include "IPCDatabaseDVClient.h"
#include "MainFrame.h"
#include "CPanel.h"
#include "CopyReadOnlyVersion.h"
#include "IPCDataCarrierClient.h"
#include "BackupEraseMedium.h"
#include ".\backupwindow.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#define BACKUP_TO_HDD  //zum Testen, Backup auf HDD c:\DV\
////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CBackupWindow,CDisplayWindow)
/////////////////////////////////////////////////////////////////////////////
// CBackupWindow

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBackupWindow::CBackupWindow(CMainFrame* pParent, CServer* pServer)
	: CDisplayWindow(pParent, pServer)
{
	m_PlayStatus			= PS_BACKUP_START_STATUS;
	m_MultiBackupLastPlayStatus = PS_BACKUP_START_STATUS;
	m_LastPlayStatusBeforeBackupActivated = PS_BACKUP_START_STATUS;
	m_CalcMBUserBackupEnd = PS_BACKUP_START_STATUS;
	m_ErrorStatus			= ES_NO_ERROR;
	m_LastPlayStatus		= m_PlayStatus;
	m_i64BytesToBackup		= 0;
	m_iMaxCameraNameLength	= 0;
	m_pBackupDrive			= NULL;
	m_bDeleteExternBackupDrive	= FALSE;
	m_dwRunningTime			= 0;

	//members needed for backup`s on CD-R`s with DirectCD
	m_dwBackupImages	= 0;
	m_i64CDCapacity		= GetCDCapacity();	//CD-R Kapazität (meist 600 MB)

	m_dwWaitLastRecord	= 0;
	m_bDirectCD			= FALSE;
	m_pDirectCD			= NULL;
	m_dwMsgTimeCompleteBackup	= 0;
	m_dwDCDWait					= 0;
	m_dwStartFormat				= 0;
	m_dwBackupFailed			= 0;
	m_dwCurrentSeconds			= 0; 
	m_dwLastCurrentSeconds		= 0;
	m_dwLastNotEnoughMB			= 0;
	m_bStopDBSearch				= FALSE;
	m_i64BytesBackuped			= 0;
	m_dwDCDWaitFinalize			= 0;
	m_dwCountUp					= 0;
	m_dwPercentBackuped			= 0;
	m_dwStartTime				= 0;
	m_dwBytesPerSecond			= 4 * 150 * 1024; 	//angenommene 4 fach Brenngeschwindigkeit
	m_bStopCalculate			= FALSE;
	m_bCloseBackupWindow		= FALSE;
	m_pCopyReadOnlyVersion		= NULL;
	m_bOnlyForwards				= TRUE;
	m_bCalculateError			= FALSE;
	m_wCamBackupEnd				= 0;
	m_bMultiBackup				= FALSE;
	m_bAlarmBackup				= FALSE;
	m_bLastRecordLocalized		= FALSE;
	m_wBackupUserID				= (WORD)GetTickCount();
	m_iACDCProgress				= 0;
	m_iACDCEraseProgress		= 0;
	m_dwACDCSessionID			= 0;
	m_dwMsgTimeNotErasedMedium	= 0;
	m_dwMsgTimeACDCNoEmptyCD	= 0;
	m_dwMsgTimeACDCIndicateError= 0;
	m_dwMsgTimeACDCCancelBackup = 0;
	m_bACDCEraseProgress		= FALSE;
	m_bEraseComplete			= FALSE;
	m_bACDCProgressRuns			= FALSE;
	m_pBackupEraseMedium		= NULL;
	m_bPrepareStopBackup		= FALSE;
	m_bTKRTraceBackup			= theApp.TKR_TraceBackup();
	m_uTimerProgressGeneric		= 0;
	m_uTimerProgressFormat		= 0;
	m_uTimerProgressFinalize	= 0;
}

CBackupWindow::~CBackupWindow()
{
		
	//falls in der Datenbank gerade eine Endzeitberechnung läuft
	//hier der Datenbank Bescheid sagen, dass kein Endzeitpunkt mehr
	//geliefert werden braucht

	if(m_pServer)
	{
		if(    m_PlayStatus == PS_BACKUP_CALCULATE_END__WAIT_FOR
			|| m_PlayStatus == PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR)
		{
			if (m_pServer->IsDatabaseConnected())
			{
				WK_TRACE(_T("~CBackupWindow()::DoRequestCancelBackup(%i)\n"), m_wBackupUserID);
				m_pServer->GetDatabase()->DoRequestCancelBackup(m_wBackupUserID); 

				//release removal of backup drive tray (CD/DVD drive) 
				//when pushing eject button on the recorder
				BOOL bPrevent = FALSE;
				PreventBackupDriveRemoval(bPrevent,(const _TCHAR)m_pBackupDrive->GetLetter());
			}

		}

	}

	WK_DELETE(m_pCopyReadOnlyVersion);
	WK_DELETE(m_pDirectCD);
	WK_DELETE(m_pBackupEraseMedium);
	if (m_bDeleteExternBackupDrive)
	{
		m_bDeleteExternBackupDrive = FALSE;
		WK_DELETE(m_pBackupDrive);
	}
}


BEGIN_MESSAGE_MAP(CBackupWindow, CDisplayWindow)
	//{{AFX_MSG_MAP(CBackupWindow)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CHECK_BACKUP_STATUS, OnBackupCheckStatus)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CSecID CBackupWindow::GetID() const
{
	return SECID_NO_ID;
}

/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::GetName() const
{
	CString sName;
	sName.LoadString(IDS_BACKUP_TITLE);
	return sName;
}

/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::GetTitle(CDC* pDC)
{		
    CString sTitle(_T("Auslagerung auf CD"));
	return sTitle;
}

/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::GetFooter(CByteArray& Array)
{

}

/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::GetDefaultText()
{
	CString s;
	int	iBackupRunning = 0;

	switch(m_PlayStatus)
	{

	case PS_BACKUP_FAILED:
	case PS_BACKUP_ACDC_ERROR:
		s.LoadString(IDS_BACKUP_FAILED);
		break;

	case PS_BACKUP_ACDC_CANCEL_BACKUP:
		if(    m_PlayStatus == PS_BACKUP_RUNNING
			|| m_PlayStatus == PS_BACKUP_COPY_RO
			|| m_PlayStatus == PS_BACKUP_COPYING_RO)
		{
			//Benutzer Abbruch der Zwischenspeicherung
			s.LoadString(IDS_BACKUP_STOP_RUNNING_UP_ACDC);			
		}
		else if(   m_PlayStatus == PS_BACKUP_ACDC_START
		        || m_PlayStatus == PS_BACKUP_ACDC_WAIT_COMPLETE
				|| m_PlayStatus == PS_BACKUP_START_ERASE_EXTERN_MEDIUM
				|| m_PlayStatus == PS_BACKUP_ERASING_EXTERN_MEDIUM)
		{
			//Benutzer Abbruch des ACDC Brennvorgangs
			s.LoadString(IDS_BACKUP_STOP_BACKUP_ACDC);
		}
		else
		{
			s.LoadString(IDS_BACKUP_STOP_BACKUP_ACDC);
		}
		break;

	case PS_BACKUP_INPUT_CAMERAS:
		s.LoadString(IDS_BACKUP_INPUT_CAMERAS);
		break;

	case PS_BACKUP_INPUT_MULTI_IS_RUNNING:
		if(theApp.CanBackupWithACDC())
		{
			//formatiere Kamera String für MultiBackup Anzeige
			m_sBackupCams = _T("");
			CString sText;
			sText.LoadString(IDS_BACKUP_NODATA);
			int iMaxSpaceCount = m_iMaxCameraNameLength + 1;
			for(int i = 0; i < m_waBackupCameras.GetSize(); i++)
			{
				CPlayWindow* pPW = m_pMainFrame->GetPlayWindow(m_waBackupCameras[i]);
				m_sBackupCams += pPW->GetName();

				if(m_waBackupCamerasNoImages.GetAt(i) == 1) //images available
				{							

					m_sBackupCams += _T("\n");		
				}
				else //no images available
				{
					int iSpaceCount = iMaxSpaceCount - (pPW->GetName().GetLength());
					CString sSpace(_T(' '), iSpaceCount);
					CString sString;
					sString.Format(_T("%s%s\n"), sSpace, sText);
					m_sBackupCams += sString;
				}	
			}

			s.Format(IDS_BACKUP_MULTI_IS_RUNNING, m_stMultiBackupStartLast.GetDateTime(),
												  m_stMultiBackupEndLast.GetDateTime(),
												  m_stMultiBackupStartNew.GetDateTime(),
												  m_sBackupCams);

		}
		else
		{
			s.Format(IDS_BACKUP_MULTI_IS_RUNNING, m_stMultiBackupStartLast.GetDateTime(),
												  m_stMultiBackupEndLast.GetDateTime(),
												  m_stMultiBackupStartNew.GetDateTime(),
												  m_sBackupCams);
		}


		break;
		
	case PS_BACKUP_INPUT_START:
		if(m_LastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS_MIN)
		{
			if(m_CalcMBUserBackupEnd == PS_BACKUP_REQUEST_MB)
			{
				//nur hier rein, wenn User nicht berechneten Endzeitpunkt gewählt hat
				//und die anschließende MB Berechung keine MB`s geliefert hat
				if(m_LastPlayStatusBeforeBackupActivated == PS_BACKUP_CALCULATE_END__SUCCESS)
				{
					s.Format(IDS_BACKUP_REQUEST_MB, m_sLastCDPercentUsed,
													m_stBackupStart.GetDateTime(), 
												  	GetCamsToBackupAsString());
				}
				else if(m_LastPlayStatusBeforeBackupActivated == PS_BACKUP_CALCULATE_END__SUCCESS_MAX)
				{
					s.Format(IDS_BACKUP_REQUEST_MB_MAX, m_stBackupStart.GetDateTime(), 
												  		   GetCamsToBackupAsString());
				}
			}
			else
			{
				s.LoadString(IDS_BACKUP_CALCULATE_SUCCESS_MIN);
			}
		}
		else
		{
			s.LoadString(IDS_BACKUP_INPUT_START);
		}

		break;
		
	case PS_BACKUP_INPUT_MULTI:
		if(!m_bMultiBackup)
		{
			s.Format(IDS_BACKUP_MULTI, m_stBackupStart.GetDateTime(), GetCamsToBackupAsString());
		}
		break;

	case PS_BACKUP_INPUT_WAIT_FOR_MEDIUM:
		break;
		
	case PS_BACKUP_CALCULATE_END__START:
	case PS_BACKUP_CALCULATE_END__WAIT_FOR:
		if(!m_bAlarmBackup)
		{
			s.LoadString(IDS_BACKUP_CALCULATE_WAIT);
		}
		break;


	case PS_BACKUP_CALCULATE_END__MULTI_START:
	case PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR:
		s.Format(IDS_BACKUP_CALCULATE_WAIT_MULTI, m_stMultiBackupStartLast.GetDateTime(), 
											 	  m_stMultiBackupEndLast.GetDateTime(),
												  m_stMultiBackupStartNew.GetDateTime(),
											  	  GetCamsToBackupAsString(TRUE));
		break;

	case PS_BACKUP_CALCULATE_END__SUCCESS:
		if(m_bMultiBackup)
		{
			//benutze in Abhängigkeit von m_MultiBackupLastPlayStatus, welche Meldung im BackupWnd
			//bei Multibackup und Eingabe eines größeren Endzeitpunktes, als der berechnete,
			//angezeigt werden soll. Es soll stets der vorherige sein.
			if(m_MultiBackupLastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS_MAX )
			{
				s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS_MAX, m_stMultiBackupStartLast.GetDateTime(), 
																 m_stMultiBackupEndLast.GetDateTime(),
																 m_stMultiBackupStartNew.GetDateTime(),
												  				 GetCamsToBackupAsString());
			}
			else if(m_MultiBackupLastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS)
			{
				s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS, m_stMultiBackupStartLast.GetDateTime(), 
															 m_stMultiBackupEndLast.GetDateTime(),
															 m_stMultiBackupStartNew.GetDateTime(),
											  				GetCamsToBackupAsString(),
															m_sCDPercentUsed);
			}
		}
		else
		{
			if(!m_bAlarmBackup)
			{
				s.Format(IDS_BACKUP_CALCULATE_SUCCESS, m_sCDPercentUsed,
													 m_stBackupStart.GetDateTime(),
														GetCamsToBackupAsString());
			}

		}
		break;

	case PS_BACKUP_CALCULATE_END__SUCCESS_MAX:
		if(m_bMultiBackup)
		{
			s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS_MAX, m_stMultiBackupStartLast.GetDateTime(), 
															 m_stMultiBackupEndLast.GetDateTime(),
															 m_stMultiBackupStartNew.GetDateTime(),
											  				 GetCamsToBackupAsString());
		}
		else
		{
			s.Format(IDS_BACKUP_CALCULATE_SUCCESS_MAX, m_stBackupStart.GetDateTime(), 
											  		   GetCamsToBackupAsString());
		}
		break;
			
	case PS_BACKUP_BACKUP_ACTIVATED:
		{
		if(m_LastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS_MAX)
		{
			if(m_bMultiBackup)
			{
				if(m_MultiBackupLastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS_MAX )
				{
					s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS_MAX, m_stMultiBackupStartLast.GetDateTime(), 
																	 m_stMultiBackupEndLast.GetDateTime(),
																	 m_stMultiBackupStartNew.GetDateTime(),
												  					 GetCamsToBackupAsString());
				}
				else if(m_MultiBackupLastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS)
				{
					s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS, m_stMultiBackupStartLast.GetDateTime(), 
																 m_stMultiBackupEndLast.GetDateTime(),
																 m_stMultiBackupStartNew.GetDateTime(),
											  					 GetCamsToBackupAsString(),
																 m_sCDPercentUsed);
				}
			}
			else
			{
				s.Format(IDS_BACKUP_CALCULATE_SUCCESS_MAX, m_stBackupStart.GetDateTime(), 
												  		   GetCamsToBackupAsString());
			}
		}
		else if(   m_LastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS)
		{
			if(m_bMultiBackup)
			{
				if(m_MultiBackupLastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS_MAX )
				{
					s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS_MAX, m_stMultiBackupStartLast.GetDateTime(), 
																	 m_stMultiBackupEndLast.GetDateTime(),
																	 m_stMultiBackupStartNew.GetDateTime(),
												  					 GetCamsToBackupAsString());
				}
				else if(m_MultiBackupLastPlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS)
				{
					s.Format(IDS_BACKUP_CALCULATE_MULTI_SUCCESS, m_stMultiBackupStartLast.GetDateTime(), 
																 m_stMultiBackupEndLast.GetDateTime(),
																 m_stMultiBackupStartNew.GetDateTime(),
											  					 GetCamsToBackupAsString(),
																 m_sCDPercentUsed);
				}
			}
			else
			{
				s.Format(IDS_BACKUP_CALCULATE_SUCCESS, m_sCDPercentUsed,
													   m_stBackupStart.GetDateTime(),
													   GetCamsToBackupAsString());
			}
		}
		else if(   m_LastPlayStatus == PS_BACKUP_CALCULATE_END__WAIT_FOR
				|| m_LastPlayStatus == PS_BACKUP_CALCULATE_END__FAILED)
		{
			if(!m_bCloseBackupWindow) //wenn TRUE, dann Anzeigetext nicht ändern, da Backupfenster
									  //geschlossen wird
			{
				if(m_bMultiBackup)
				{
					s.Format(IDS_BACKUP_CALCULATE_MULTI_ERROR, m_stMultiBackupStartLast.GetDateTime(), 
															   m_stMultiBackupEndLast.GetDateTime(),
															   m_stMultiBackupStartNew.GetDateTime(),
											  				   GetCamsToBackupAsString());
				}
				else
				{
					s.Format(IDS_BACKUP_CALCULATE_ERROR, m_stBackupStart.GetDateTime(),
															 GetCamsToBackupAsString());
				}
			}
		}
		}
		break;

	case PS_BACKUP_TOO_MANY_IMAGES:
		{
			CString s1, s2;
			s1.LoadString(IDS_BACKUP_TOO_MANY_IMAGES);
			s2.LoadString(IDS_BACKUP_INPUT_START);
			s = s1 + _T("\n\n") + s2;
			break;
		}

	case PS_BACKUP_DCD_START:
	case PS_BACKUP_DCD_WAIT_EMPTY_CDR:
		if(IsAlarm())
		{
			s.Format(IDS_BACKUP_CHECK_CD2, m_stBackupStart.GetDateTime(), 
										   m_stBackupEnd.GetDateTime());	
		}
		else
		{
			m_sBackupCams = _T("");
			CString sText;
			sText.LoadString(IDS_BACKUP_NODATA);
			int iMaxSpaceCount = m_iMaxCameraNameLength + 1;
			for(int i = 0; i < m_waBackupCameras.GetSize(); i++)
			{
				CPlayWindow* pPW = m_pMainFrame->GetPlayWindow(m_waBackupCameras[i]);
				m_sBackupCams += pPW->GetName();

				if(m_waBackupCamerasNoImages.GetAt(i) == 1) //images available
				{							

					m_sBackupCams += _T("\n");		
				}
				else //no images available
				{
					int iSpaceCount = iMaxSpaceCount - (pPW->GetName().GetLength());
					CString sSpace(_T(' '), iSpaceCount);
					CString sString;
					sString.Format(_T("%s%s\n"), sSpace, sText);
					m_sBackupCams += sString;
				}

			}

			s.Format(IDS_BACKUP_CHECK_CD, m_stBackupStart.GetDateTime(), 
										  m_stBackupEnd.GetDateTime(), 
										  m_sBackupCams);

			}
		break;

	case PS_BACKUP_DCD_NO_EMPTY_CDR:
		s.LoadString(IDS_BACKUP_FORMAT_NOCDR);
		break;

	case PS_BACKUP_DCD_FOUND_NOT_EMPTY_CDR:
		s.LoadString(IDS_BACKUP_NO_EMPTY_CD);
		break;

	case PS_BACKUP_DCD_FINALIZE:
		{
			CString sTime;
			sTime.Format(_T("%02d:%02d"), m_dwRunningTime/60, m_dwRunningTime%60);
			if(IsAlarm())
			{
				s.Format(IDS_BACKUP_FINALIZE, sTime);
			}
			else
			{
				s.Format(IDS_BACKUP_FINALIZE2, sTime);
			}
		}
		break;


	case PS_BACKUP_ERASE_EXTERN_MEDIUM:
		s.LoadString(IDS_BACKUP_ERASE_EXTERN_MEDIUM);
		break;

	case PS_BACKUP_ACDC_ERASE_MEDIUM:
		s.LoadString(IDS_BACKUP_ACDC_ERASE_CDRW);
		break;

	case PS_BACKUP_ACDC_START:	
		break;

	case PS_BACKUP_ACDC_WAIT_COMPLETE:
		{
		CString sTime;
		CString sWaitComplete, sErase;

		sTime.Format(_T("%02d:%02d:%02d"),m_dwCountUp/3600,
					 (m_dwCountUp/60)%60,
					 m_dwCountUp%60);
		
		if(m_bACDCEraseProgress)
		{
			//Progress (Prozent) während des Löschens einer CD-RW
			if(m_iACDCEraseProgress < 100)
			{
				m_iACDCEraseProgress = m_iACDCProgress;
				m_iACDCProgress = 0;
			}
			else
			{
				//Progress (Prozent) während des Brennens (nach dem Löschen) einer CD-RW
				m_iACDCEraseProgress = 100;

				if(    m_iACDCProgress > 50 
					&& m_iACDCProgress < 100)
				{
					m_bACDCProgressRuns = TRUE;
				}

				if(    m_iACDCProgress == 100
					&& !m_bEraseComplete
					&& !m_bACDCProgressRuns)
				{
					//hier erster Durchlauf mit 100%, sind aber die 100% des Löschvorganges
					//also den Backup Progress auf 0% zurücksetzen
					m_iACDCProgress = 0;
					m_bEraseComplete = TRUE;
				}
			}

			sErase.LoadString(IDS_BACKUP_ACDC_WAIT_ERASE_COMPLETE);
			sWaitComplete.LoadString(IDS_BACKUP_RUNNING_UP);
			sWaitComplete = sErase + sWaitComplete;

			//nach Beendigung des Brennens anzeigen, dass Brennen beendet wird
			//das dauert immer ein paar Sekunden, besonders beim DVD brennen
			if(    m_bEraseComplete
				&& m_iACDCProgress == 100)
			{
				CString sWaitMoreThan100, sWait;
				sWaitMoreThan100.LoadString(IDS_BACKUP_FINALIZE2);
				sWait.LoadString(IDS_WAIT);
				int iFirst = 0;
				int iSecond = 0;

				iFirst = sWaitMoreThan100.Find(_T("\n"));
				if(iFirst != -1)
				{
					iSecond = sWaitMoreThan100.Find(_T("\n"), iFirst+1);

					if(    iSecond != -1 
						&& iSecond > iFirst)
					{
						sWaitMoreThan100 = sWaitMoreThan100.Mid(iFirst, iSecond-iFirst);
						sWaitMoreThan100 = sWaitMoreThan100 + _T("\n") + sWait;
						sWaitComplete = sWaitComplete + sWaitMoreThan100;
					}
				}
			}
			s.Format(sWaitComplete, m_iACDCEraseProgress, m_iACDCProgress, sTime);
		}
		else
		{
			sWaitComplete.LoadString(IDS_BACKUP_RUNNING_UP);

			//nach Beendigung des Brennens anzeigen, dass Brennen beendet wird
			//das dauert immer ein paar Sekunden, besonders beim DVD brennen
			if(m_iACDCProgress == 100)
			{
				CString sWaitMoreThan100, sWait;
				sWaitMoreThan100.LoadString(IDS_BACKUP_FINALIZE2);
				sWait.LoadString(IDS_WAIT);
				int iFirst = 0;
				int iSecond = 0;

				iFirst = sWaitMoreThan100.Find(_T("\n"));
				if(iFirst != -1)
				{
					iSecond = sWaitMoreThan100.Find(_T("\n"), iFirst+1);

					if(    iSecond != -1 
						&& iSecond > iFirst)
					{
						sWaitMoreThan100 = sWaitMoreThan100.Mid(iFirst, iSecond-iFirst);
						sWaitMoreThan100 = sWaitMoreThan100 + _T("\n") + sWait;
						sWaitComplete = sWaitComplete + sWaitMoreThan100;
					}
				}
			}
			s.Format(sWaitComplete, m_iACDCProgress, sTime);
		}
		
		if(	   m_bPrepareStopBackup 
			&& IsBackupACDC_BurningToDisc())
		{
			//Brennen soll auf Wunsch des Benutzers abgebrochen werden
			//das dauert einen Moment, deshalb Hilfetext ausgeben.
			CString sPrepareCancel;
			sPrepareCancel.LoadString(IDS_BACKUP_PREPARE_TO_STOP_BACKUP_ACDC);
			 
			s += "\n" + sPrepareCancel;
		}
		}
		break;

	case PS_BACKUP_ACDC_NO_EMPTY_CD:
		s.LoadString(IDS_BACKUP_NO_EMPTY_CD);
		break;
		
	case PS_BACKUP_ACDC_DVD_NOT_ALLOWED:
		s.LoadString(IDS_BACKUP_FAILED);
		break;

	case PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM:
		s.LoadString(IDS_BACKUP_EXTERN_MEDIUM_NOT_ERASED);
		break;

	case PS_BACKUP_ACDC_NOT_ERASED:
		s.LoadString(IDS_BACKUP_ACDC_NOT_ERASED);
		break;
		
	case PS_BACKUP_ACDC_COMPLETE:
	case PS_BACKUP_COMPLETE:
		if (IsAlarm())
		{
			s.LoadString(IDS_BACKUP_COMPLETE);
		}
		else
		{
			s.LoadString(IDS_BACKUP_COMPLETE2);
		}
		break;


	case PS_BACKUP_DCD_WAIT_FORMAT:
		{	
			CString sTime;
			sTime.Format(_T("%02d:%02d"), m_dwRunningTime/60, m_dwRunningTime%60);

			if(IsAlarm())
			{
				s.Format(IDS_BACKUP_FORMAT, sTime);
			}
			else
			{
				s.Format(IDS_BACKUP_FORMAT2, sTime);
			}

		}
		break;

	case PS_BACKUP_START_ERASE_EXTERN_MEDIUM:
	case PS_BACKUP_ERASING_EXTERN_MEDIUM:
		//kommt nur vor beim Löschen eines externen Mediums (Memorystick, HDD)
		{
		CString sTime;
		sTime.Format(_T("%02d:%02d:%02d"),m_dwCountUp/3600,
					 (m_dwCountUp/60)%60,
					 m_dwCountUp%60);
		
		s.Format(IDS_BACKUP_ERASING_EXTERN_MEDIUM,sTime);
		}
		break;

	case PS_BACKUP_REQUEST:
	case PS_BACKUP_REQUEST_MB:
		if(!theApp.CanBackupWithACDC())
		{
			s.LoadString(IDS_BACKUP_FORMATTED);
		}
		break;

	case PS_BACKUP_RUNNING:
		{
			CString sTime;
			RecalcBackupCountdown();
			if (m_i64BytesToBackup>0)
			{
				iBackupRunning = (int)m_dwPercentBackuped;
				if (iBackupRunning>100)
				{
					iBackupRunning = 100;
				}
			}

			sTime.Format(_T("%02d:%02d:%02d"),m_dwCountUp/3600,
						 (m_dwCountUp/60)%60,
						 m_dwCountUp%60);
			if (m_pBackupDrive)
			{
				if(m_pBackupDrive->GetType() == DRIVE_REMOVABLE)
				{
					s.Format(IDS_BACKUP_RUNNING_UP,iBackupRunning, sTime);
				}
				else if(theApp.CanBackupWithACDC())
				{
					CString sTempHDD;
					sTempHDD.LoadString(IDS_BACKUP_RUNNING_UP_ACDC);
					s.Format(sTempHDD,iBackupRunning, sTime);
				}
				else
				{
					if (DRIVE_FIXED == m_pBackupDrive->GetType())
					{
						CString sHDD;
						sHDD.LoadString(IDS_BACKUP_RUNNING_UP);
						sHDD = _T("Auslagerung auf HDD nach ") + m_pBackupDrive->GetRootString() + _T("\n\n") + sHDD;
						s.Format(sHDD,iBackupRunning, sTime);
					}
					else
					{
						s.Format(IDS_BACKUP_RUNNING_UP,iBackupRunning, sTime);
					}
				}

			}

#ifdef _DEBUG
			s += _T('\n');
			sTime.Format(_T("%d kB/s"),m_dwBytesPerSecond/1024);
			s += sTime;
#endif

		}
		break;

	case PS_BACKUP_COPY_RO:
	case PS_BACKUP_COPYING_RO:
		s.LoadString(IDS_BACKUP_COPYING);
		break;


	case PS_NAVIGATE_FAILED:
		s.LoadString(IDS_NO_RECORD_FOUND);
		break;
	case PS_READ_FAILED:
		s.LoadString(IDS_READ_FAILED);
		break;

	case PS_BACKUP_START_STATUS:
	case PS_BACKUP_START:
		if(    theApp.CanBackupWithACDC()
			&& m_LastPlayStatus != PS_BACKUP_BACKUP_ACTIVATED
			&& !m_bAlarmBackup)
		{
			//hier nur rein beim warten auf das Einlegen eines Mediums
			//nicht nochmal hier rein, wenn das Brennen beginnt
			s.LoadString(IDS_BACKUP_ACDC_START);
		}
		break;

	default:
		s.LoadString(IDS_WAIT);
		break;

	}
	return GetName() + _T("\n") + s;

}

/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::GetCamsToBackupAsString(BOOL bMultiBackup /* FALSE */)
{
	CString sCams;
	
	CString sText;
	sText.LoadString(IDS_BACKUP_NODATA);
	int iMaxSpaceCount = m_iMaxCameraNameLength + 1;
	
	for(int i = 0; i < m_waBackupCameras.GetSize(); i++)
	{
		CPlayWindow* pPW = m_pMainFrame->GetPlayWindow(m_waBackupCameras[i]);
		sCams += pPW->GetName();
		if(bMultiBackup)
		{
			sCams += _T("\n");
		}
		else
		{
			if(!m_bAlarmBackup)
			{
				if(m_waBackupCamerasNoImages.GetAt(i) == 1) //images available
				{							
					sCams += _T("\n");		
				}
				else //no images available
				{
					int iSpaceCount = iMaxSpaceCount - (pPW->GetName().GetLength());
					CString sSpace(_T(' '), iSpaceCount);
					CString sString;
					sString.Format(_T("%s%s\n"), sSpace, sText);
					sCams += sString;
				}
			}

		}
	}
	return sCams;
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CBackupWindow::GetOSDColor()
{
	COLORREF col;

	if (IsActive())
		col = RGB(0,0,192);
	else
		col = RGB(0,0,128);
	return col;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::IsShowable()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::Request(int iMax/*=-1*/)
{
	BOOL bRet = FALSE;
	switch(m_PlayStatus)
	{

	case PS_BACKUP_START_STATUS:
		break;

	case PS_STOP:
		bRet = OnStop();
		break;

	case PS_BACKUP_INPUT_CAMERAS:
	case PS_BACKUP_INPUT_START:
	case PS_BACKUP_INPUT_MULTI:
	case PS_BACKUP_INPUT_MULTI_IS_RUNNING:
		break;

	case PS_BACKUP_INPUT_WAIT_FOR_MEDIUM:
		break;

	case PS_BACKUP_CALCULATE_END__START:
	case PS_BACKUP_CALCULATE_END__MULTI_START:
		bRet = OnCalculateEnd_Start(TRUE);
		break;

	case PS_BACKUP_CALCULATE_END__WAIT_FOR:
	case PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR:
		bRet = OnCalculateEnd_WaitFor();
		break;

	case PS_BACKUP_CALCULATE_END__SUCCESS:
	case PS_BACKUP_CALCULATE_END__SUCCESS_MAX:
	case PS_BACKUP_CALCULATE_END__SUCCESS_MIN:
	case PS_BACKUP_CALCULATE_END__FAILED:
		bRet = OnCalculateEnd_Success();
		break;

	case PS_BACKUP_TOO_MANY_IMAGES:
		bRet = OnBackupTooManyImages();
		break;

	case PS_BACKUP_BACKUP_ACTIVATED:
		bRet = OnBackupBackupActivated();
		break;

	case PS_BACKUP_START:
		bRet = OnBackupStart();
		break;

	case PS_BACKUP_START_ERASE_EXTERN_MEDIUM:
		bRet = OnBackupStartEraseExternMedium();
		break;

	case PS_BACKUP_ERASING_EXTERN_MEDIUM:
		bRet = OnBackupErasingExternMedium();
		break;

	case PS_BACKUP_REQUEST:
		bRet = OnBackupRequest();
		break;

	case PS_BACKUP_REQUEST_MB:
		bRet = OnBackupRequestMB();
		break;

	case PS_BACKUP_DCD_START:
		bRet = OnBackupDCDStart();
		break;

	case PS_BACKUP_RUNNING:
		bRet = OnBackupRunning();
		break;

	case PS_BACKUP_DCD_ERROR:
		bRet = OnBackupDCDError();
		break;

	case PS_BACKUP_DCD_WAIT_EMPTY_CDR:
		bRet = OnBackupDCDWaitEmptyCDR();
		break;

	case PS_BACKUP_DCD_NO_EMPTY_CDR:
		bRet = OnBackupDCDNoEmptyCDR();
		break;

	case PS_BACKUP_DCD_FOUND_NOT_EMPTY_CDR:
		bRet = OnBackupDCDFoundNotEmptyCDR();
		break;
		
	case PS_BACKUP_ERASE_EXTERN_MEDIUM:
	case PS_BACKUP_ACDC_ERASE_MEDIUM:
		bRet = OnBackupEraseMedium();
		break;

	case PS_BACKUP_ACDC_START:
		bRet = OnBackupACDCStart();
		break;
		
	case PS_BACKUP_ACDC_WAIT_COMPLETE:
		bRet = OnBackupACDCWaitComplete();
		break;

	case PS_BACKUP_ACDC_COMPLETE:
		bRet = OnBackupACDCComplete();
		break;

	case PS_BACKUP_ACDC_NO_EMPTY_CD:
	case PS_BACKUP_ACDC_DVD_NOT_ALLOWED:
		bRet = OnBackupACDCNoEmptyCD();
		break;

	case PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM:
	case PS_BACKUP_ACDC_NOT_ERASED:
		bRet = OnBackupACDCNotErased();
		break;

	case PS_BACKUP_ACDC_ERROR: 
		bRet = OnBackupACDCIndicateError();
		break;

	case PS_BACKUP_ACDC_CANCEL_BACKUP:
		bRet = OnBackupACDCCancelBackup();
		break;
		
	case PS_BACKUP_COMPLETE:
		bRet = OnBackupComplete();
		break;

	case PS_BACKUP_FAILED:
		bRet = OnBackupFailed();
		break;

	case PS_BACKUP_DCD_WAIT_FORMAT:
		bRet = OnBackupDCDWaitFormat();
		break;

	case PS_BACKUP_COPY_RO:
		bRet = OnCopyRO();
		break;

	case PS_BACKUP_COPYING_RO:
		bRet = OnCopyingRO();
		break;
		
	case PS_BACKUP_DCD_FINALIZE:
		bRet = OnBackupDCDFinalize();
		break;

	default:
		TRACE(_T("TKR***** play status not handled in Request %s\n"),NameOfEnum(m_PlayStatus));
		break;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::StartBackup(const CSystemTime &stStart,
							    const CSystemTime &stEnd,
							    const CWordArray& waCameras,
								CString sCamsSeperated)
{
	BOOL bRet = FALSE;

	m_stBackupEnd				= stEnd;
	m_stBackupStart				= stStart;
	
 	m_waBackupCameras.RemoveAll();
	m_waBackupCameras.Append(waCameras);

	m_sMultiBackupCamsSeperated	= sCamsSeperated;
	
	if (Activate())
	{
		m_LastPlayStatus = m_PlayStatus;

		//reset variablen
		m_MultiBackupLastPlayStatus				= PS_BACKUP_START_STATUS;
		m_CalcMBUserBackupEnd					= PS_BACKUP_START_STATUS;
		m_LastPlayStatusBeforeBackupActivated	= PS_BACKUP_START_STATUS;

		ChangePlayStatus(PS_BACKUP_START);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** CBackupWindow::StartBackup(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
		bRet = TRUE;		
	}
	else
	{
		m_LastPlayStatus = m_PlayStatus;
		m_ErrorStatus = ES_CANNOT_ACTIVATE_BACKUP_WINDOW;
		m_dwBackupFailed = 0;
		ChangePlayStatus(PS_BACKUP_FAILED);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** CBackupWindow::StartBackup(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}

	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::StartAlarmBackup(const CSystemTime &stStart,
							    const CSystemTime &stEnd,
							    const CWordArray& waCameras)
{
	BOOL bRet = FALSE;

	m_bAlarmBackup = TRUE;
	m_stBackupAlarmEnd			= stEnd;
	m_stBackupEnd				= stEnd;
	m_stBackupStart				= stStart;
	
 	m_waBackupCameras.RemoveAll();
	m_waBackupCameras.Append(waCameras);

	
	if (Activate())
	{
		m_LastPlayStatus = m_PlayStatus;

		//die Berechnung einmal starten, da ansonsten gesamte MB`s für das Alarmbackup
		//nicht zur Verfügung stehen
		ChangePlayStatus(PS_BACKUP_CALCULATE_END__START);
//		ChangePlayStatus(PS_BACKUP_START);
		if(m_bTKRTraceBackup)
		{		
			WK_TRACE(_T("TKR***** CBackupWindow::StartBackup(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}

		bRet = TRUE;		
	}
	else
	{
		m_LastPlayStatus = m_PlayStatus;
		m_ErrorStatus = ES_CANNOT_ACTIVATE_BACKUP_WINDOW;
		m_dwBackupFailed = 0;
		ChangePlayStatus(PS_BACKUP_FAILED);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** CBackupWindow::StartBackup(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::CalcBackupEnd(const CWordArray &waCameras, 
								  const CSystemTime &stBackupStart)

{
	BOOL bRet = FALSE;
	CString sStart, sEnd;
	CPlayWindow* pPlayWnd = NULL;
	WORD wCamID = 0;

	m_stBackupStart = stBackupStart;
	m_stCalculatedEnd = m_stBackupStart;
	m_stCalculatedEnd.wMilliseconds = 999;
	m_waBackupCameras.RemoveAll();
	m_waBackupCamerasNoImages.RemoveAll();
	m_iMaxCameraNameLength = 0;
	CSystemTime st;
	st.GetLocalTime();
	m_stNewestDBImage.wYear			= st.GetYear();
	m_stNewestDBImage.wMonth		= st.GetMonth();
	m_stNewestDBImage.wDay			= st.GetDay();
	m_stNewestDBImage.wHour			= st.GetHour();
	m_stNewestDBImage.wMinute		= st.GetMinute();
	m_stNewestDBImage.wSecond		= st.GetSecond();
	m_stNewestDBImage.wMilliseconds = st.GetMilliseconds();
	
	//hole ID`s (z.B. 0x2004) aller für das Backup angegebenen Kameras	
	for(int k=0; k<waCameras.GetSize(); k++)
	{
		pPlayWnd = theApp.GetMainFrame()->GetPlayWindow(waCameras.GetAt(k));
		if(pPlayWnd)
		{
			wCamID = pPlayWnd->GetID().GetSubID();
			if(wCamID)
			{
				m_waBackupCameras.Add(wCamID);
				m_waBackupCamerasNoImages.Add(0);	//fülle array auf, 
													//im ConfirmBackupByTime werden später diejenigen
													//Einträge auf "1" gesetzt, für dessen Kameras Bilder
													//für das Backup gefunden wurden
				if(m_iMaxCameraNameLength < pPlayWnd->GetName().GetLength())
				{
					m_iMaxCameraNameLength = pPlayWnd->GetName().GetLength();
				}
			}
		}
	}

	CString d,s;
	for (int i=0;i<m_waBackupCameras.GetSize();i++)
	{
		d.Format(_T("%02x,"),m_waBackupCameras[i]);
		s += d;
	}
	m_sBackupCams = s.Left(s.GetLength()-1);
	
	m_LastPlayStatus = m_PlayStatus;
	m_bStopDBSearch = FALSE;
	m_dwCurrentSeconds = 0;
	m_bStopCalculate = FALSE;
	m_bCloseBackupWindow = FALSE;
	m_bOnlyForwards = TRUE;

	if(m_bMultiBackup)
	{
		ChangePlayStatus(PS_BACKUP_CALCULATE_END__MULTI_START);
	}
	else
	{
		ChangePlayStatus(PS_BACKUP_CALCULATE_END__START);
	}

	WK_TRACE(_T("CBackupWindow::CalcBackupEnd(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

	return bRet;

}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::CalcMBUserBackupEnd(const CWordArray &waCameras,
										const CSystemTime &stStart,	
										const CSystemTime &stUserEnd)
{
	int iNrOfCams = waCameras.GetSize();
	DWORD* pCams = NULL;
	pCams = new DWORD[iNrOfCams];
	for(int i=0; i<iNrOfCams; i++)
	{
		pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
	}

	WK_TRACE(_T("CBackupWindow::StartBackup() Start: %s ber. Endzeit: %s,  benutzer Endzeit: %s CDCapacity: %d\n"),
				stStart.GetDateTime(),
				m_stCalculatedEnd.GetDateTime(), 
				stUserEnd.GetDateTime(),
				m_i64CDCapacity.GetInMB());	

	//hole MB`s für Backup zur korrekten Prozent-Berechnung des Backupfortschritts
	m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,_T(""),_T(""),
					  iNrOfCams,
					  pCams,
					  stStart,
					  stUserEnd,
					  BBT_CALCULATE,
					  m_i64CDCapacity.GetInMB());
	WK_DELETE_ARRAY(pCams);

	if(m_bTKRTraceBackup)
	{
		WK_TRACE(_T("TKR ----- CBackupWindow::CalcMBUserBackupEnd() UserID: %i \n"), m_wBackupUserID);
	}

	m_LastPlayStatus = m_PlayStatus; 
	ChangePlayStatus(PS_BACKUP_REQUEST_MB);

	WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
		NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnCalculateEnd_Start(BOOL bForward)
{
	WORD wFlags = BBT_CALCULATE;
	DWORD dwCapacity = 0;
	BOOL bRet = FALSE;
	DWORD* pCams = NULL;
	DWORD dwDummy = 0;
	int iNrOfCams = m_waBackupCameras.GetSize();
	pCams = new DWORD[iNrOfCams];
	
	//speicher letzten kalkulierten Backup Endzeitpunkt
	m_stLastCalculatedEnd = m_stCalculatedEnd;
	
	if(m_dwCurrentSeconds == 0)
	{
		//erster Aufruf, m_dwCurrentSeconds == 0 (Stunde)
		wFlags = BBT_CALCULATE_END;
		dwCapacity = m_i64CDCapacity.GetInMB();
		//TRACE(_T("TKR OnCalculateEnd_Start() CurrentSecond == 0\n")); 
	}
	else
	{
		dwDummy = m_dwCurrentSeconds;
		if(bForward)
		{
			if(!m_bOnlyForwards)
			{
				m_dwCurrentSeconds = m_dwCurrentSeconds+(DWORD)(abs((long)((m_dwCurrentSeconds-m_dwLastCurrentSeconds))/2));
				m_stCalculatedEnd = m_stBackupStart;
				IncrementCalculatedEnd();
			}
		}
		else
		{
			m_bOnlyForwards = FALSE;
			if(!IsNewestDBImageReached())
			{
				m_dwCurrentSeconds = m_dwCurrentSeconds-(DWORD)(abs((long)((m_dwCurrentSeconds-m_dwLastCurrentSeconds))/2));
				if(m_dwCurrentSeconds == dwDummy)
				{
					//kalkulierte Endzeit ändert sich nicht aber MB`s noch zu groß
					if(m_bTKRTraceBackup)
					{
						WK_TRACE(_T("kalkulierte Endzeit ändert sich nicht aber MB`s noch zu groß\n"));
					}
					if(m_dwLastNotEnoughMB != 0)
					{
						m_dwCurrentSeconds = m_dwLastNotEnoughMB;
					}
					else
					{
						if(m_dwCurrentSeconds != 0)
						{
							m_dwCurrentSeconds -= 1;
						}
						else
						{
							m_LastPlayStatus = m_PlayStatus;
							ChangePlayStatus(PS_BACKUP_CALCULATE_END__FAILED);
							WK_TRACE(_T("CBackupWindow::OnCalculateEnd_Start(): playstatus: from %s -> to: %s\n"),
									NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
						}


					}


					m_bStopDBSearch = TRUE;

				}
				m_stCalculatedEnd = m_stBackupStart;
				IncrementCalculatedEnd();
			}
			else
			{
/*
					Berechnungsschritte in Stunden							|
				0 1 2  4    8        16               32			   z.B. 54		   64
 BackupStart->	|-|-|--|----|--------|----------------|---------------------|----------|
																			|
																			|
					Berechnete MB der zu auf CD-R zu brennenden Bilder		|
																			|
				0MB					435MB			 597MB				  720MB		 720MB
				|--------------------|----------------|---------------------| . . . . .|
																			|
		 																	|
											aktuelle Systemzeit	ist	  ----> |
										Datenbank-Ende (letztes Bild)		|


				Da am Datenbank-Ende (54 Stunden hinter Backup-Startzeit)
				zu viele Bilder gefunden wurden (max. 600 MB erlaubt)
				muss nun zwischen Datenbank-Ende (54 h) und letztem
				Berechnungsschritt (32 h) die Intervallberechnung beginnen.
				
				Nächstes Intervall muss nun bei 32h + (54h - 32h)/2 = 43h liegen.
*/
				LARGE_INTEGER liNewCalculatedEnd, liResultSec;
				FILETIME ftNewCalculatedEnd;

				//subtrahiere die Startzeit (0h) von der kalkulierten Endzeit (54h)
				CSystemTime stDiffEndStart =  m_stCalculatedEnd - m_stBackupStart;

				//ziehe von diesem Ergebnis den letzten Berechnungsschritt ab (54h - 32h = 22h)
				CSystemTime stCurrectHour(m_dwLastCurrentSeconds/(60*60));	// 32h
				CSystemTime stNewCalculatedEnd(stDiffEndStart-stCurrectHour); // 22h

				//berechneten Wert (22h) in FileTime umrechnen um ihn dann durch 2 teilen zu können
				SystemTimeToFileTime(&stNewCalculatedEnd,&ftNewCalculatedEnd);
				liNewCalculatedEnd.HighPart =  ftNewCalculatedEnd.dwHighDateTime;
				liNewCalculatedEnd.LowPart = ftNewCalculatedEnd.dwLowDateTime;
				liResultSec.QuadPart = (liNewCalculatedEnd.QuadPart/2);

				//Ergebnis von Zeitschritt des LARGE_INTEGER 100 Nanosekunden
				//in Zeitschritt 1 Sekunde umrechnen
				liResultSec.QuadPart = liResultSec.QuadPart/(10*1000*1000);

				//neue Sekundenzahl berechnen und ab dem BackupStart Inkrementieren
				m_dwCurrentSeconds = (DWORD)liResultSec.QuadPart + m_dwLastCurrentSeconds;
				m_stCalculatedEnd = m_stBackupStart;
				
				//Dummy auf LastCurrectSeconds setzen, da sonst mit einem "falschen" Wert gerechnet wird
				//das verursachte einen Deadlock
				dwDummy = m_dwLastCurrentSeconds;
				IncrementCalculatedEnd();
			}
		}
	}
	//sichere aktuelle Stundenzahl zwischen Start und End
	m_dwLastCurrentSeconds = dwDummy;	
	CString sCamDummy1, sCamDummy2;
	for(int i=0; i<iNrOfCams; i++)
	{
		pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
		sCamDummy2.Format(_T(",%d"), m_waBackupCameras.GetAt(i));
		sCamDummy1 += sCamDummy2;
	}	

	if (   m_pServer 
		&& m_pServer->IsDatabaseConnected())
	{

		if(m_bCloseBackupWindow)
		{
			WK_TRACE(_T("Kein weiteres DoRequestBackupByTime(), da Berechnung vom Panel aus beendet wurde\n"));
			m_LastPlayStatus = m_PlayStatus;
			ChangePlayStatus(PS_BACKUP_CALCULATE_END__FAILED);

			if(m_LastPlayStatus != m_PlayStatus)
			{
				WK_TRACE(_T("CBackupWindow::OnCalculateEnd_Start(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
		else
		{
			m_bCalculateError = FALSE;

			if(m_bAlarmBackup)
			{
				m_stCalculatedEnd = m_stBackupAlarmEnd;
			}

			//berechne mb zwischen Start und Endzeit für die ausgewählten Kameras
			m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,_T(""),_T(""),
								  iNrOfCams,
								  pCams,
								  m_stBackupStart,
								  m_stCalculatedEnd,
								  wFlags,
								  dwCapacity);
		}

	}
	WK_DELETE_ARRAY(pCams);

	TRACE(_T("CBackupWindow::OnCalculateEnd_Start() Request abgeschickt Start: %s  CalcEnd: %s  Cams: %s UserID: %i Capacity: %d\n"),
					m_stBackupStart.GetDateTime(), m_stCalculatedEnd.GetDateTime(), sCamDummy1, m_wBackupUserID, dwCapacity);

	if(m_PlayStatus != PS_BACKUP_CALCULATE_END__FAILED)
	{
		m_LastPlayStatus = m_PlayStatus;
		//warten auf das Ende der Endzeitberechnung
		if(m_bMultiBackup)
		{
			ChangePlayStatus(PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR);
		}
		else
		{
			ChangePlayStatus(PS_BACKUP_CALCULATE_END__WAIT_FOR);
		}

		if(m_LastPlayStatus != m_PlayStatus)
		{
			WK_TRACE(_T("CBackupWindow::OnCalculateEnd_Start(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}


	return bRet;
} 

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnCalculateEnd_WaitFor()
{
	if(m_bTKRTraceBackup)
	{
		WK_TRACE(_T("TKR----- OnCalculateEnd_WaitFor()\n"));
	}

	//falls das Berechnen des Endzeitpunktes für das Brennen der ersten CD-R zu lange dauert
	//oder wenn vom Panel aus das Backupfenster geschlossen werden soll 
	//dann das Berechnen hier abbrechen
 
	if(m_bStopCalculate)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_CALCULATE_END__FAILED);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** CBackupWindow::OnCalculateEnd_Success(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnCalculateEnd_Success()
{
	BOOL bRet = FALSE;
	theApp.GetPanel()->SetCalculateEndComplete();
	
	//wurden keine Bilder im Backupzeitraum gefunden, dies dem Panel mitteilen
	if(m_i64BytesToBackup == 0)
	{
		if(m_PlayStatus != PS_BACKUP_CALCULATE_END__FAILED)
		{
			m_LastPlayStatus = m_PlayStatus;
			theApp.GetPanel()->SetImagesToBackup(FALSE);
			ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS_MIN);
			if(m_bTKRTraceBackup)
			{
				WK_TRACE(_T("TKR***** CBackupWindow::OnCalculateEnd_Success(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
	}

	m_stBackupEnd = m_stCalculatedEnd;

	if(m_PlayStatus == PS_BACKUP_CALCULATE_END__SUCCESS_MIN)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_INPUT_START);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** CBackupWindow::OnCalculateEnd_Success(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	else
	{
		m_LastPlayStatus = m_PlayStatus;

		if(m_bMultiBackup && m_MultiBackupLastPlayStatus == PS_BACKUP_START_STATUS)
		{
			//nur hier rein, wenn Multibackup UND variable m_MultiBackupLastPlayStatus  im StartZustand
			m_MultiBackupLastPlayStatus = m_PlayStatus;
		}
		//wird benötigt, um bei Ausgabe der Hilfetexte zu wissen, ob CD voll 
		//ausgelastet war oder nur zu ein paar Prozent
		m_LastPlayStatusBeforeBackupActivated = m_PlayStatus; 

		if(m_bAlarmBackup)
		{
			ChangePlayStatus(PS_BACKUP_START);
			if(m_bTKRTraceBackup)
			{
				WK_TRACE(_T("TKR***** CBackupWindow::OnCalculateEnd_Success(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
		else
		{
			ChangePlayStatus(PS_BACKUP_BACKUP_ACTIVATED);
			if(m_bTKRTraceBackup)
			{
				WK_TRACE(_T("TKR***** CBackupWindow::OnCalculateEnd_Success(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::RequestLastBackupImageInDB()
{
	if(m_bTKRTraceBackup)
	{
		WK_TRACE(_T("TKR ######### RequestLastBackupImageInDB()\n"));
	}

	m_stMultiBackupStartNew = m_stBackupEnd;

	CIPCFields fields;
	CSystemTime stBackupEndLessOneMilliSec = m_stBackupEnd;
	stBackupEndLessOneMilliSec.DecrementTime(0, 0, 0, 0, 1);
	CString sDate = stBackupEndLessOneMilliSec.GetDBDate();
	CString sTime = stBackupEndLessOneMilliSec.GetDBTime();
	char op = '}'; //vorwärts suchen

	fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,op));
	fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,op));
				
	//Hole letztes Image der letzen Sequence der letzten Backup Kamera
	DWORD dwUserID = BACKUP_LAST_RECORD;
	int iNumArchives = 1;
	WORD w[1];
	w[0] = m_wCamBackupEnd;

	m_pServer->GetDatabase()->DoRequestNewQuery(dwUserID,
												iNumArchives,
												w,
												fields,
												1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnCalculateEnd_Failed(CSystemTime stStart, CSystemTime stEnd)
{

	BOOL bRet = TRUE;
	DWORD* pCams = NULL;
	int iNrOfCams = m_waBackupCameras.GetSize();
	pCams = new DWORD[iNrOfCams];
	
	for(int i=0; i<iNrOfCams; i++)
	{
		pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
	}

	if (m_pServer && m_pServer->IsDatabaseConnected())
	{
		m_bCalculateError = TRUE;

		//berechne mb zwischen Start und Endzeit für die ausgewählten Kameras
		m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,_T(""),_T(""),
							  iNrOfCams,
							  pCams,
							  stStart,
							  stEnd,
							  BBT_CALCULATE);
		TRACE(_T("CBackupWindow::OnCalculateEnd_Start() Request abgeschickt Start: %s  End: %s UserID: %i\n"),
											stStart.GetDateTime(), stEnd.GetDateTime(),m_wBackupUserID);
	}
	
	WK_DELETE_ARRAY(pCams);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupTooManyImages()
{
	BOOL bRet = TRUE;
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupBackupActivated()
{
	BOOL bRet = TRUE;
	
	if(m_bCloseBackupWindow)
	{
		//Kein weiteres DoRequestBackupByTime(), Berechnung vom Panel aus beendet
		//nun das backup Fenster schließen
		OnStop();
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupStart()
{
	BOOL bRet = TRUE;

	m_i64BytesBackuped = 0;
	
	if (m_pServer->IsDatabaseConnected())
	{
		m_bPrepareStopBackup = FALSE;
		
		CString sRemovableDrive;
		int iDriveBit;
		if(GetRemovableDrive(sRemovableDrive, iDriveBit))
		{
			//do backup to extern drive (memory stick, HDD, ...)
			CString sRootExtended;
			sRootExtended.Format(_T("%s"),sRemovableDrive);
	
			m_pBackupDrive = new CIPCDrive(sRootExtended, DVR_CDR_BACKUP_DRIVE);

			if(m_pBackupDrive)
			{
				if(m_bDeleteExternBackupDrive)
				{
					WK_TRACE(_T("***** Erasing extern medium *****\n"));
					//zunächst removabler Medium löschen	
					m_LastPlayStatus = m_PlayStatus;
					ChangePlayStatus(PS_BACKUP_START_ERASE_EXTERN_MEDIUM);
					if(m_bTKRTraceBackup)
					{
						WK_TRACE(_T("TKR***** CBackupWindow::OnBackupStart(): playstatus: from %s -> to: %s\n"),
							NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
					}
				}
				else
				{
					m_pBackupDrive->CheckSpace();

					WK_TRACE(_T("***** Start Backup to removable drive *****\n"));

					//Hole SequenceNr und RecordNr des letzten Bildes des aktuellen Backups
					//wird dann gespeichert um im mutlibackup an dieses Bild anzusetzen
					RequestLastBackupImageInDB();
					ChangePlayStatus(PS_BACKUP_REQUEST);
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("OnBackupStart no backup drive\n"));
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_STOP);
			}
		}
		else if(theApp.CanBackupWithACDC())
		{
			m_pBackupDrive = m_pServer->GetDataCarrier()->GetBackupDrive();

			if(m_pBackupDrive)
			{	
				//do backup with ACDC
				WK_TRACE(_T("***** Start Backup with ACDC *****\n"));

			
				//prevent removal of backup drive try (CD/DVD drive) 
				//when pushing eject button on the recorder
				BOOL bPrevent = TRUE;
				PreventBackupDriveRemoval(bPrevent,(const _TCHAR)m_pBackupDrive->GetLetter());

				//Hole SequenceNr und RecordNr des letzten Bildes des aktuellen Backups
				//wird dann gespeichert um im utlibackup an dieses Bild anzusetzen
				RequestLastBackupImageInDB();

				WK_TRACE(_T("***** Start Backup mit ACDC in Tempverzeichnis *****\n"));
				ChangePlayStatus(PS_BACKUP_REQUEST);
			}
			else
			{
				WK_TRACE_ERROR(_T("OnBackupStart no backup drive\n"));
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_STOP);
			}
		}
		else
		{
			CIPCDatabaseDVClient* pDC = m_pServer->GetDatabase();
			m_pBackupDrive = pDC->GetBackupDrive();

			//Backup mit DirectCD
			if (m_pBackupDrive)
			{
				//Hole SequenceNr und RecordNr des letzten Bildes des aktuellen Backups
				RequestLastBackupImageInDB();

				//nicht auf CD, sondern auf HDD auslagern
				if (DRIVE_FIXED == m_pBackupDrive->GetType())
				{
					WK_TRACE(_T("***** Start Test-Backup auf Festplatte *****\n"));
					ChangePlayStatus(PS_BACKUP_REQUEST);
				}
				else if (m_pBackupDrive->IsCDROM())
				{
					FS_Producer Producer = (FS_Producer)m_pBackupDrive->GetFSProducer();

					if (Producer == FSP_UDF_ADAPTEC_DIRECTCD)
					{
						WK_TRACE(_T("***** Start Backup with DirectCD *****\n"));

						m_bDirectCD = TRUE;
						m_pDirectCD = new CDirectCD;
						m_LastPlayStatus = m_PlayStatus;
						ChangePlayStatus(PS_BACKUP_DCD_START);
						if(m_bTKRTraceBackup)
						{
							WK_TRACE(_T("TKR***** CBackupWindow::OnBackupStart(): playstatus: from %s -> to: %s\n"),
								NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
						}
					}
					else
					{
//TODO TKR hier evtl. noch Abwärtskompatibilität zu PacketCD herstellen ?

						WK_TRACE_ERROR(_T("no Direct CD installed no backup possible\n"));
						m_LastPlayStatus = m_PlayStatus;
						ChangePlayStatus(PS_STOP);
						if(m_bTKRTraceBackup)
						{
							WK_TRACE(_T("TKR***** CBackupWindow::OnBackupStart(): playstatus: from %s -> to: %s\n"),
								NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
						}
					}
				}
				else
				{
					WK_TRACE(_T("***** Backup with DirectCD kann nicht gestartet werden *****\n"));
				}
			}
			else
			{
				WK_TRACE(_T("***** Backup with DirectCD kann nicht gestartet werden *****\n"));
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnBackupStart database not connected\n"));
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_STOP);
	}

	return bRet;

} 


/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDStart()
{

	if(m_pDirectCD->ExecuteWatchCD(m_pBackupDrive->GetRootString()))
	{
		m_LastPlayStatus = m_PlayStatus;
		m_dwDCDWait = GetTickCount();
		ChangePlayStatus(PS_BACKUP_DCD_WAIT_EMPTY_CDR);
		
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	else
	{
		m_pDirectCD->ChangeDCDErrorStatus((CDirectCD::DCDErrorStatus)DCD_ERROR_START_WATCHCD);
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_DCD_ERROR);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDWaitEmptyCDR()
{
	if(GetTimeSpan(m_dwDCDWait) > 70*1000)
	{
		//Timeout, if no message (no "empty CDR found" or "error")
		//are received from WatchCD.exe. after 60 sec. That means an error occoured
		m_pDirectCD->ChangeDCDErrorStatus((CDirectCD::DCDErrorStatus)DCD_ERROR_WAIT_FOR_EMPTY_CDR);
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_DCD_ERROR);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupDCDError(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
		m_dwStartFormat = 0;
	}
return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDNoEmptyCDR()
{
	if(GetTimeSpan(m_dwDCDWait) > 15*1000)
	{
		m_dwMsgTimeCompleteBackup = GetTickCount();
		m_LastPlayStatus = m_PlayStatus;

		ChangePlayStatus(PS_STOP);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupDCDNoEmptyCDR(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDFoundNotEmptyCDR()
{
	if(GetTimeSpan(m_dwDCDWait) > 15*1000)
	{
		m_dwMsgTimeCompleteBackup = GetTickCount();
		m_LastPlayStatus = m_PlayStatus;

		ChangePlayStatus(PS_STOP);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupDCDNoEmptyCDR(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDError()
{
	CString sDCDError = m_pDirectCD->GetErrorString();
	if(m_bTKRTraceBackup)
	{
		WK_TRACE_ERROR(_T("TKR***** CBackupWindow::OnBackupDCDError(): %s\n"), sDCDError);
	}
	m_LastPlayStatus = m_PlayStatus;
	m_dwBackupFailed = 0;
	ChangePlayStatus(PS_BACKUP_FAILED);

	if(m_bTKRTraceBackup)
	{
		WK_TRACE(_T("TKR***** OnBackupDCDError(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupComplete()
{
	BOOL bRet = FALSE;

	if(m_bDirectCD)
	{
		WK_DELETE(m_pDirectCD);
		m_bDirectCD = FALSE;
	}

	// Time Out Backup Complete
	if (GetTimeSpan(m_dwMsgTimeCompleteBackup) > 15*1000)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_STOP);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupComplete(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
		
		//hat Backup geklappt, Daten für das Multibackup merken	
		//aber nur bei NICHT Alarm Backup
		if(!m_bAlarmBackup)
		{
			SaveMultiBackupMembers();
		}

		m_bAlarmBackup = FALSE;

	}
	if (m_bDeleteExternBackupDrive)
	{
		m_bDeleteExternBackupDrive = FALSE;
		WK_DELETE(m_pBackupDrive);
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupFailed()
{

	//reset alarm backup
	m_bAlarmBackup = FALSE;

	//evtl. angelegtes Tempverzeichnis zum Brennen mit ACDC wieder löschen
	DeleteACDCTempPath();


	if (m_dwBackupFailed == 0)
	{
		m_dwBackupFailed = GetTickCount();
		HWND hWatchCD = ::FindWindow(NULL, _T("WatchCD"));
		if(hWatchCD && IsWindow(hWatchCD))
		{
			::PostMessage(hWatchCD, WM_CHECK_BACKUP_STATUS, BS_DCD_BACKUP_FAILED, 0);
			WK_TRACE(_T("CPlayWindow::OnBackupFailed(): Backup failed sended to WatchCD\n"));
		}

		switch(m_ErrorStatus)
		{

		case ES_CANNOT_ACTIVATE_BACKUP_WINDOW:
			WK_TRACE(_T("CBackupWindow::OnBackupFailed(): Couldn't activate BackupWindow\n"));
			break;

		case ES_NO_ERROR:
			break;

		case ES_EXTERN_BACKUP_DRIVE_REMOVED:
			WK_TRACE(_T("CBackupWindow::OnBackupFailed(): Extern backup drive removed\n"));
			break;
		default:
			WK_TRACE(_T("CBackupWindow::OnBackupFailed() was called without an error\n"));
			break;
		}
	}
	else
	{
		if (GetTimeSpan(m_dwBackupFailed) > 15*1000)
		{
			m_LastPlayStatus = m_PlayStatus;
			ChangePlayStatus(PS_STOP);

			if(m_bTKRTraceBackup)
			{
				WK_TRACE(_T("TKR***** OnBackupFailed(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
			m_dwBackupFailed = 0;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnStop()
{
	BOOL bRet = FALSE;
	CMainFrame* pMainWnd  = theApp.GetMainFrame();
	if (pMainWnd)
	{
		pMainWnd->DeleteBackupWindow();
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDWaitFormat()
{
	if(m_dwStartFormat == 0)
	{
		m_dwStartFormat = GetTickCount();
	}
	else
	{
		if(GetTimeSpan(m_dwStartFormat) > 2*60*1000)
		{
			m_pDirectCD->ChangeDCDErrorStatus((CDirectCD::DCDErrorStatus)DCD_ERROR_FORMAT);
			m_LastPlayStatus = m_PlayStatus;
			ChangePlayStatus(PS_BACKUP_DCD_ERROR);

			WK_TRACE(_T("OnBackupDCDError(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			m_dwStartFormat = 0;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupStartEraseExternMedium()
{
	CString sRemovableDrive;
	int iDriveBit;
	if(GetRemovableDrive(sRemovableDrive, iDriveBit))
	{
		m_pBackupEraseMedium = new CBackupEraseMedium(sRemovableDrive);
	
		//erase extern extern drive (memory stick, HDD, ...)
		if(m_pBackupEraseMedium)
		{
			//reset timer
			m_dwCountUp = 0;
			if (m_uTimerProgressGeneric)
			{
				KillTimer(m_uTimerProgressGeneric);
			}
			m_uTimerProgressGeneric = SetTimer(1,1000,NULL);

			m_pBackupEraseMedium->StartThread();
			
			m_LastPlayStatus = m_PlayStatus; 
			ChangePlayStatus(PS_BACKUP_ERASING_EXTERN_MEDIUM);
			WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupErasingExternMedium()
{

	if(m_pBackupEraseMedium)
	{
		if(!m_pBackupEraseMedium->IsRunning())
		{
			WK_TRACE(_T("erasing extern drive complete\n"));
			WK_DELETE(m_pBackupEraseMedium);
	
			if(m_pBackupDrive)
			{
				m_pBackupDrive->CheckSpace();

				WK_TRACE(_T("***** Start Backup to removable drive *****\n"));

				//Hole SequenceNr und RecordNr des letzten Bildes des aktuellen Backups
				//wird dann gespeichert um im mutlibackup an dieses Bild anzusetzen
				RequestLastBackupImageInDB();
				
				m_LastPlayStatus = m_PlayStatus; 
				ChangePlayStatus(PS_BACKUP_REQUEST);

				WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupRequest()
{
	TRACE(_T("***** Beginn von OnBackupRequest()\n"));

	if (m_pBackupDrive->GetType() == DRIVE_REMOVABLE)
	{
		DWORD* pCams = NULL;
		int iNrOfCams = m_waBackupCameras.GetSize();
		pCams = new DWORD[iNrOfCams];
		for(int i=0; i<iNrOfCams; i++)
		{
			pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
		}

		CString sPath;
		sPath = m_pBackupDrive->GetRootString();
		m_bCalculateError = FALSE;	
		

		m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,sPath,_T(""),
							  iNrOfCams,
							  pCams,
							  m_stBackupStart,
							  m_stBackupEnd,
							  BBT_EXECUTE|BBT_PROGRESS,
							  m_i64CDCapacity.GetInMB());
		WK_DELETE_ARRAY(pCams);
		
		//setzen des Startzeitpunktes des folgenden Backups, ist Grundlage für die
		//Berechnung der verbleibenden Backup Zeit
		m_dwStartTime = GetTickCount();
		RecalcBackupCountdown();

		//in Sekundenschritten den Fortschritt des Backups anzeigen
		if (m_uTimerProgressGeneric)
		{
			KillTimer(m_uTimerProgressGeneric);
		}
		m_uTimerProgressGeneric = SetTimer(1, 1000, NULL);	


		m_LastPlayStatus = m_PlayStatus; 
		ChangePlayStatus(PS_BACKUP_RUNNING);

		WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	}
	else if (theApp.CanBackupWithACDC())
	{
		//do backup with ACDC
		m_pBackupDrive = m_pServer->GetDataCarrier()->GetBackupDrive();
		if(m_pBackupDrive)
		{
			if(m_i64CDCapacity > 0)
			{

				WK_TRACE(_T("OnBackupRequest(): Start backup images with ACDC\n"));
			
				DWORD* pCams = NULL;
				int iNrOfCams = m_waBackupCameras.GetSize();
				pCams = new DWORD[iNrOfCams];
				for(int i=0; i<iNrOfCams; i++)
				{
					pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
				}
				
				//Prozent der CD-R Auslastung zurücksetzen
				m_sCDPercentUsed = _T("0,0");
				
				//lösche altes Temp Verzeichnis, falls es noch existiert
				DeleteACDCTempPath();

				m_sACDCTempPath.Empty();
			
				//Lege neuen Ordner im Tempverzeichnis an
				DWORD dwTick = GetTickCount();
				CString sPath;
				CString sTempLocation, sKey;
				CString sSection = REGKEY_DVCLIENT;
				CWK_Profile prof;

				LPTSTR sBuffer = sTempLocation.GetBufferSetLength(MAX_PATH);
				GetTempPath(MAX_PATH, sBuffer);
				GetLongPathName(sBuffer, sBuffer, MAX_PATH);
				sTempLocation.ReleaseBuffer();
				sKey = _T("BackupTempLocation");
				CString sTemp;
				sTemp = prof.GetString(sSection, sKey, sTempLocation);
				if(sTemp != sTempLocation)
				{
					sTempLocation = sTemp;
					prof.WriteString(sSection, sKey, sTempLocation);
				}
				prof.WriteString(sSection, sKey, sTempLocation);

				sPath.Format(_T("%sACDCtmp%d"), sTempLocation, dwTick);

				if(CreateDirectory(sPath,NULL))
				{
					m_sACDCTempPath = sPath;
				}
				else
				{
					WK_TRACE_ERROR(_T("Failed to create ACDCTemp1 Dir: %s\n"), GetLastErrorString());
					m_LastPlayStatus = m_PlayStatus; 
					ChangePlayStatus(PS_STOP);

					WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
					return FALSE;
				}


				//ACDC.exe mittelen, dass das Backup begonnen wird
				//ab jetzt, bis backup complete,  darf ACDC.exe nicht auf USB (CD/DVD) Drives 
				//reagieren und neu starten, wenn USB drive erkannt bzw entfernt wird.
				CIPCDataCarrierClient* pDataCarrier = theApp.GetServer()->GetDataCarrier();
				if(pDataCarrier)
				{
					//Backup with ACDC started
					pDataCarrier->DoRequestGetValue(SECID_NO_ID,_T("Backup start"),0);
				}

				WK_TRACE(_T("Backup in Temp: %s  FreeMB: %d \n"), m_sACDCTempPath, m_i64CDCapacity.GetInMB());	
				m_bCalculateError = FALSE;
				
				if(m_bAlarmBackup)
				{
					m_stBackupEnd = m_stBackupAlarmEnd;
				}

				m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,m_sACDCTempPath+_T("\\"),_T(""),
									  iNrOfCams,
									  pCams,
									  m_stBackupStart,
									  m_stBackupEnd,
									  BBT_EXECUTE|BBT_PROGRESS,
									  m_i64CDCapacity.GetInMB());
				WK_DELETE_ARRAY(pCams);
				
				//setzen des Startzeitpunktes des folgenden Backups, ist Grundlage für die
				//Berechnung der verbleibenden Backup Zeit
				m_dwStartTime = GetTickCount();
				RecalcBackupCountdown();
				
				if (m_uTimerProgressGeneric)
				{
					KillTimer(m_uTimerProgressGeneric);
				}
				m_uTimerProgressGeneric = SetTimer(1,1000,NULL);

				m_LastPlayStatus = m_PlayStatus; 
				ChangePlayStatus(PS_BACKUP_RUNNING);

				WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
			else
			{
				WK_TRACE(_T("OnBackupRequest(): Backup failed, free space on CD = 0 MB\n"));				m_dwBackupFailed = 0;
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_BACKUP_FAILED);

				WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
		else
		{
			// no backup drive
			WK_TRACE(_T("***** Backup with DirectCD impossible, no BackupDrive\n"));			
		}
	}
	else
	{
		m_pBackupDrive = m_pServer->GetDatabase()->GetBackupDrive();
		if (m_pBackupDrive)
		{
			if (DRIVE_FIXED == m_pBackupDrive->GetType())
			{
				DWORD* pCams = NULL;
				int iNrOfCams = m_waBackupCameras.GetSize();
				pCams = new DWORD[iNrOfCams];
				for(int i=0; i<iNrOfCams; i++)
				{
					pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
				}

				CString sPath;
				sPath = m_pBackupDrive->GetRootString();
				m_bCalculateError = FALSE;	
				

				m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,sPath,_T(""),
									  iNrOfCams,
									  pCams,
									  m_stBackupStart,
									  m_stBackupEnd,
									  BBT_EXECUTE|BBT_PROGRESS,
									  m_i64CDCapacity.GetInMB());
				WK_DELETE_ARRAY(pCams);
				
				//setzen des Startzeitpunktes des folgenden Backups, ist Grundlage für die
				//Berechnung der verbleibenden Backup Zeit
				m_dwStartTime = GetTickCount();
				RecalcBackupCountdown();

				//in Sekundenschritten den Fortschritt des Backups anzeigen
				if (m_uTimerProgressGeneric)
				{
					KillTimer(m_uTimerProgressGeneric);
				}
				m_uTimerProgressGeneric = SetTimer(1,1000,NULL);	

				m_LastPlayStatus = m_PlayStatus; 
				ChangePlayStatus(PS_BACKUP_RUNNING);

				WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
			else if (m_pBackupDrive->IsCDROM())
			{
				m_pBackupDrive->CheckSpace();
				WK_TRACE(_T("OnBackupRequest(): free space on %s: %i MB\n"), 
						m_pBackupDrive->GetRootString(),
						m_pBackupDrive->GetFreeMB());

				if(m_pBackupDrive->GetFreeMB() == 0)
				{
					WK_TRACE(_T("OnBackupRequest(): Backup failed, free space on CD: %i\n"),
								m_pBackupDrive->GetFreeMB());
					m_dwBackupFailed = 0;
					m_LastPlayStatus = m_PlayStatus;
					ChangePlayStatus(PS_BACKUP_FAILED);

					WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
				}
				else
				{
					WK_TRACE(_T("OnBackupRequest(): Start backup images\n"));

				
					DWORD* pCams = NULL;
					int iNrOfCams = m_waBackupCameras.GetSize();
					pCams = new DWORD[iNrOfCams];
					for(int i=0; i<iNrOfCams; i++)
					{
						pCams[i] = MAKELONG(0,m_waBackupCameras.GetAt(i));
					}
					
					//Prozent der CD-R Auslastung zurücksetzen
					m_sCDPercentUsed = _T("0,0");
					
					CString sPath = m_pBackupDrive->GetRootString();
					m_bCalculateError = FALSE;
					m_pServer->GetDatabase()->DoRequestBackupByTime(m_wBackupUserID,sPath,_T(""),
										  iNrOfCams,
										  pCams,
										  m_stBackupStart,
										  m_stBackupEnd,
										  BBT_EXECUTE|BBT_PROGRESS,
										  m_i64CDCapacity.GetInMB());
					WK_DELETE_ARRAY(pCams);
					
					//setzen des Startzeitpunktes des folgenden Backups, ist Grundlage für die
					//Berechnung der verbleibenden Backup Zeit
					m_dwStartTime = GetTickCount();
					RecalcBackupCountdown();
					if (m_uTimerProgressGeneric)
					{
						KillTimer(m_uTimerProgressGeneric);
					}
					m_uTimerProgressGeneric = SetTimer(1,1000,NULL);

					m_LastPlayStatus = m_PlayStatus; 
					ChangePlayStatus(PS_BACKUP_RUNNING);

					WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
				}
			}
			else
			{
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_BACKUP_RUNNING);

				WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
		else
		{
			// no backup drive
			WK_TRACE(_T("***** Backup with DirectCD impossible, no BackupDrive\n"));
		}

	}


	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupRequestMB()
{
	return FALSE;

}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupRunning()
{
	BOOL bRet = FALSE;
	return bRet;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::ConfirmBackup(DWORD dwID)
{
	BOOL bRet = FALSE;

	if (dwID == 0)
	{
		//letzte Sequenz kopiert
		TRACE(_T("last sequence going to PS_BACKUP_COPY_RO\n"));
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_COPY_RO);
		WK_TRACE(_T("ConfirmBackup(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	}

	DoRedraw();

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
//wird bei Backup Fehlern mit ACDC aufgerufen
BOOL CBackupWindow::IndicateACDCError(DWORD dwCmd, 
									  CSecID id, 
									  CIPCError errorCode, 
									  int iUnitCode,
									  const CString &sErrorMsg)
{
	//lösche temporäres Verzeichnis vom Backup mit ACDC
	DeleteACDCTempPath();

	//release removal of backup drive try (CD/DVD drive) 
	//when pushing eject button on the recorder
	BOOL bPrevent = FALSE;
	PreventBackupDriveRemoval(bPrevent,(const _TCHAR)m_pBackupDrive->GetLetter());

	WK_TRACE_ERROR(_T("CBackupWindow::IndicateACDCError(): %s\n"), sErrorMsg);

	m_dwMsgTimeACDCIndicateError = GetTickCount();
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_BACKUP_ACDC_ERROR);

	if(m_bTKRTraceBackup)
	{
		WK_TRACE(_T("TKR***** OnBackupComplete(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//wird bei Benutzer Abbruch des Brennvorganges mit ACDC aufgerufen
BOOL CBackupWindow::ConfirmCancelSession(DWORD dwID)
{
	BOOL bRet = FALSE;

	//das Backup (Brennvorgang) mit ACDC wurde durch Benutzer abgebrochen
	TRACE(_T("user canceled backup (Zwischenspeicherung) going to PS_BACKUP_ACDC_CANCEL_BACKUP\n"));
	WK_TRACE(_T("Backup (burning process) canceled by user\n"));

	m_dwMsgTimeACDCCancelBackup = GetTickCount();
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_BACKUP_ACDC_CANCEL_BACKUP);
	WK_TRACE(_T("ConfirmBackup(): playstatus: from %s -> to: %s\n"),
		NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

	DoRedraw();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
//wird bei Benutzer Abbruch der Zwischenspeicherung mit ACDC aufgerufen
BOOL CBackupWindow::ConfirmCancelBackup(DWORD dwID)
{
	BOOL bRet = FALSE;

	//angelegtes Tempverzeichnis für die Zwischenspeicherung (ACDC) wieder löschen
	DeleteACDCTempPath();
	
	//das Zwischenspeichern beim Backup mit ACDC wurde durch Benutzer abgebrochen
	TRACE(_T("user canceled backup (Zwischenspeicherung) going to PS_BACKUP_ACDC_CANCEL_BACKUP\n"));
	WK_TRACE(_T("Backup (copy files to temp) canceled by user\n"));

	m_dwMsgTimeACDCCancelBackup = GetTickCount();
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_BACKUP_ACDC_CANCEL_BACKUP);
	WK_TRACE(_T("ConfirmBackup(): playstatus: from %s -> to: %s\n"),
		NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

	DoRedraw();
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnCopyRO()
{
	BOOL bRet = FALSE;
	CString sPath;

	KillTimer(1);

	if (m_pBackupDrive)
	{
		TRACE(_T("CopyReadOnlyVersion\n"));
		int iOem = OemDTS;
		CWK_Profile wkp;
		int nCameras = wkp.GetInt(_T("DV\\Process"), _T("NrOfCameras"), 4);
		CString sLanguage;
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			iOem = pPanel->GetOem();
			sLanguage = pPanel->GetCurrentLanguage();
		}
		if (   theApp.CanBackupWithACDC()
			&& m_pBackupDrive->GetType()!=DRIVE_REMOVABLE
			&& m_pBackupDrive->GetType()!=DRIVE_FIXED)
		{
			sPath = m_sACDCTempPath + _T("\\");
			m_pCopyReadOnlyVersion = new CCopyReadOnlyVersion(sPath,iOem,nCameras,sLanguage,TRUE);
			
			if (m_pCopyReadOnlyVersion->IsAlreadyDone(sPath))
			{
				WK_DELETE(m_pCopyReadOnlyVersion);
				
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_BACKUP_ACDC_START);

				WK_TRACE(_T("OnCopyRO(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
			else
			{
				m_pCopyReadOnlyVersion->StartThread();
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_BACKUP_COPYING_RO);

				WK_TRACE(_T("OnCopyRO(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
		else //Backup without ACDC
		{
			sPath = m_pBackupDrive->GetRootString();
			m_pCopyReadOnlyVersion = new CCopyReadOnlyVersion(sPath,iOem,nCameras,sLanguage,!m_pBackupDrive->IsCDROM());

			if (m_pCopyReadOnlyVersion->IsAlreadyDone(sPath))
			{
				WK_DELETE(m_pCopyReadOnlyVersion);
				
				if(m_pBackupDrive->IsCDROM())
				{
					WK_TRACE(_T("Copying alarm-archive successfull.\n"));
					m_dwStartFinalize = GetTickCount();
					m_LastPlayStatus = m_PlayStatus;
					m_dwRunningTime = 0;
					if (m_uTimerProgressFinalize)
					{
						KillTimer(m_uTimerProgressFinalize);
					}
					m_uTimerProgressFinalize = SetTimer(3, 1000, NULL);

					//*** Backup with DirectCD ***
					if(m_bDirectCD)
					{
						m_LastPlayStatus = m_PlayStatus;
						m_dwDCDWaitFinalize = 0;
						ChangePlayStatus(PS_BACKUP_DCD_FINALIZE);
					}
					else
					{
						WK_TRACE(_T("***** Finalizing CD impossible *****\n"));
						m_LastPlayStatus = m_PlayStatus;
						ChangePlayStatus(PS_STOP);
					}
					
					if(m_bTKRTraceBackup)
					{
						WK_TRACE(_T("TKR***** CBackupWindow::OnBackupStart(): playstatus: from %s -> to: %s\n"),
							NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
					}
				}
				else
				{
					m_dwMsgTimeCompleteBackup = GetTickCount();
					m_LastPlayStatus = m_PlayStatus;
					ChangePlayStatus(PS_BACKUP_COMPLETE);

					WK_TRACE(_T("OnCopyRO(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
				}
			}
			else
			{
				m_pCopyReadOnlyVersion->StartThread();
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_BACKUP_COPYING_RO);

				WK_TRACE(_T("OnCopyRO(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}

		}
	}
	else
	{
		TRACE(_T("no backup drive\n"));
		m_dwMsgTimeCompleteBackup = GetTickCount();
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_COMPLETE);

		WK_TRACE(_T("OnCopyRO(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnCopyingRO()
{
	BOOL bRet = FALSE;

	if (m_pCopyReadOnlyVersion)
	{
		if (!m_pCopyReadOnlyVersion->IsRunning())
		{
			WK_TRACE(_T("CopyReadOnlyVersion complete\n"));
			WK_DELETE(m_pCopyReadOnlyVersion);

			if(    theApp.CanBackupWithACDC()
				&& m_pBackupDrive->GetType()!=DRIVE_REMOVABLE
			    && m_pBackupDrive->GetType()!=DRIVE_FIXED)
			{
				m_LastPlayStatus = m_PlayStatus;
				ChangePlayStatus(PS_BACKUP_ACDC_START);

				WK_TRACE(_T("OnCopyingRO(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));				
			}
			else
			{
				if (m_pBackupDrive->IsCDROM())
				{
					m_LastPlayStatus = m_PlayStatus;
					m_dwStartFinalize = GetTickCount();
					m_dwRunningTime = 0;
					if (m_uTimerProgressFinalize)
					{
						KillTimer(m_uTimerProgressFinalize);
					}
					m_uTimerProgressFinalize = SetTimer(3, 1000, NULL);

					//*** Backup with DirectCD ***
					if(m_bDirectCD)
					{
						m_LastPlayStatus = m_PlayStatus;
						m_dwDCDWaitFinalize = 0;
						ChangePlayStatus(PS_BACKUP_DCD_FINALIZE);
					}
					else
					{
						WK_TRACE(_T("***** Finalizing CD impossible *****  kein m_bDirectCD\n"));
						m_LastPlayStatus = m_PlayStatus;
						ChangePlayStatus(PS_STOP);
					}

					WK_TRACE(_T("OnCopyingRO(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

				}
				else
				{
					m_dwMsgTimeCompleteBackup = GetTickCount();
					m_LastPlayStatus = m_PlayStatus;
					ChangePlayStatus(PS_BACKUP_COMPLETE);

					WK_TRACE(_T("OnCopyingRO(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
				}
			}

		}
	}
	else
	{
		WK_TRACE(_T("***** Copying ReadOnly impossible ***** kein m_pCopyReadOnlyVersion\n"));
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupDCDFinalize()
{
	if(m_dwDCDWaitFinalize == 0)
	{
		HWND hWnd = ::FindWindow(NULL, _T("WatchCD"));
		if(IsWindow(hWnd))
		{
			::PostMessage(hWnd, WM_CHECK_BACKUP_STATUS, BS_DCD_DATA_COPIED, 0);
			m_dwDCDWaitFinalize = GetTickCount();
		}
		else
		{
			m_pDirectCD->ChangeDCDErrorStatus((CDirectCD::DCDErrorStatus)DCD_ERROR_CANT_FIND_WATCHCD);
			m_LastPlayStatus = m_PlayStatus;
			ChangePlayStatus(PS_BACKUP_DCD_ERROR);

			WK_TRACE(_T("OnBackupDCDError(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			m_dwDCDWaitFinalize = 0;
		}
	}
	else
	{
		if(GetTimeSpan(m_dwDCDWaitFinalize) > 10*60*1000)
		{
			//Timeout, if no message (no "finalize success" or "error")
			//are received from WatchCD.exe. That means an error occoured
			m_pDirectCD->ChangeDCDErrorStatus((CDirectCD::DCDErrorStatus)DCD_ERROR_WAIT_FOR_FINALIZE_SUCCESS);
			m_LastPlayStatus = m_PlayStatus;
			ChangePlayStatus(PS_BACKUP_DCD_ERROR);

			WK_TRACE(_T("OnBackupDCDError(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupEraseMedium()
{
	//warte auf Eingabe: soll medium gelöscht werden oder nicht
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCStart()
{
	BOOL bRet = FALSE;
	
	CIPCDataCarrierClient* pDataCarrier = m_pServer->GetDataCarrier();
	
	//reset Sekunden Zähler
	m_dwCountUp = 0;

	//reset Erase Progress (Löschen von CD-RW`s) und
	//Fortschritt der Auslagerung mit ACDC
	m_iACDCProgress = 0;
	m_iACDCEraseProgress = 0;

	//init session id
	m_dwACDCSessionID = GetTickCount();

	if(pDataCarrier)
	{
		// search all files and dirs in temp dir
		CString sVolumeName = GetACDCVolumeName();
		CStringArray saFilesAndDirs;
		WK_SearchFiles(saFilesAndDirs, m_sACDCTempPath, _T("*.*"), TRUE);
		CString sFileOrDir;
		CString sCompleteFilePath;
		CStringArray saToCopy;
		int iLen = 0;
		for (int i=0 ; i<saFilesAndDirs.GetSize() ; i++)
		{
			sFileOrDir = saFilesAndDirs.GetAt(i);
			iLen = _tcsclen (m_sACDCTempPath) + _tcsclen (sFileOrDir) + 1;
			if (iLen > _MAX_PATH)
			{
				// error, path will be too long!
				WK_TRACE_ERROR(_T("Backup FAILED, file path to long (%i) for %s and %s\n"),
													iLen, m_sACDCTempPath, sFileOrDir);
				break;
			}
			sCompleteFilePath = m_sACDCTempPath + _T('\\') + sFileOrDir;
			saToCopy.Add(sCompleteFilePath);
		}
		WK_TRACE(_T("Requesting DataCarrier backup %08lx\n"), m_dwACDCSessionID);

		for(int j = 0; j < saToCopy.GetSize(); j++)
		{
			WK_TRACE(_T("File: %s\n"), saToCopy.GetAt(j));
		}
		pDataCarrier->DoRequestSession(m_dwACDCSessionID,
									   sVolumeName,
									   saToCopy,
									   CD_WRITE_ISO);
	}
	
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_BACKUP_ACDC_WAIT_COMPLETE);

	WK_TRACE(_T("OnBackupACDCStart(): playstatus: from %s -> to: %s\n"),
		NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	
	if (m_uTimerProgressGeneric)
	{
		KillTimer(m_uTimerProgressGeneric);
	}
	m_uTimerProgressGeneric = SetTimer(1,1000,NULL);

	return bRet;	
}
/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::GetACDCVolumeName()
{
	CSystemTime sysTime;
	CString sDateTime;
	sysTime.GetLocalTime();
	WORD wYear = sysTime.GetYear();
	CString sYear;
	sYear.Format(_T("%4d"), wYear);
	sYear = sYear.Right(2);

	//sDateTime maximal 11 zeichen lang
	sDateTime.Format(_T("%02d%02d%s_%02d%02d"),
			sysTime.GetDay(),
			sysTime.GetMonth(),
			sYear,
			sysTime.GetHour(),
			sysTime.GetMinute(),
			sysTime.GetSecond()
			);

	return sDateTime;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCWaitComplete()
{
	//waiting for completing current ACDC function
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SessionProgress(DWORD dwSessionID, int iProgress)
{

	if(dwSessionID == m_dwACDCSessionID)
	{
		m_iACDCProgress = iProgress;
	}
	else
	{
		WK_TRACE_ERROR(_T("Session Progress with wrong SessionID: %d\n"), dwSessionID);
	}




}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::ConfirmSession(DWORD dwSessionID)
{
	KillTimer(1);

	if(dwSessionID == m_dwACDCSessionID)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_ACDC_COMPLETE);

		WK_TRACE(_T("ConfirmSession(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
	}
	else
	{
		WK_TRACE_ERROR(_T("Confirm Session with wrong SessionID: %d\n"), dwSessionID);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCComplete()
{
	BOOL bRet = FALSE;

	//reset Session ID
	m_dwACDCSessionID = 0;

	//reset progress (Prozent) für zu löschende CD-RW`s
	SetACDCEraseProgress(FALSE);

	//reset CD-RW Löschen/Brennen Progress;
	m_bEraseComplete = FALSE;
	m_bACDCProgressRuns = FALSE;

	m_dwMsgTimeCompleteBackup = GetTickCount();
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_BACKUP_COMPLETE);

	WK_TRACE(_T("OnBackupACDCComplete(): playstatus: from %s -> to: %s\n"),
		NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

	//lösche Temp Pfad, der für das Brennen mit ACDC angelegt wurde
	DeleteACDCTempPath();

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCNoEmptyCD()
{
	// Time Out für Anzeigetext nach einem Nichtlöschen einer nicht leeren CD-RW
	if (GetTimeSpan(m_dwMsgTimeACDCNoEmptyCD) > 15*1000)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_STOP);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupACDCNoEmptyCD(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCIndicateError()
{
	// Time Out für Anzeigetext bei einem Backup Error
	if (GetTimeSpan(m_dwMsgTimeACDCIndicateError) > 15*1000)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_STOP);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupACDCIndicateError(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCCancelBackup()
{
	// Time Out für Anzeigetext nach Benutzer Abbruch des Backups
	if (GetTimeSpan(m_dwMsgTimeACDCCancelBackup) > 15*1000)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_STOP);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupACDCCancelBackup(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::OnBackupACDCNotErased()
{
	// Time Out für Anzeigetext nach einem Nichtlöschen einer nicht leeren CD-RW
	//bzw. auf medium (MemoryStick, HDD...) ist nicht mehr genug Speicherplatz
	if (GetTimeSpan(m_dwMsgTimeNotErasedMedium) > 15*1000)
	{
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_STOP);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupACDCNotErased(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::Activate()
{
	CMainFrame* pMainFrame = theApp.GetMainFrame();
	if (pMainFrame == NULL)
		return FALSE;

	CPanel *pPanel = theApp.GetPanel();
	if (pPanel == NULL)
		 return FALSE;

	BOOL	bRet = FALSE;
	int		nViewMode	= m_pMainFrame->GetViewMode();

	if ((nViewMode == View1pX) ||
		(nViewMode == ViewXxX) ||
		(nViewMode == ViewFull))
	{
		if (   !IsWindowVisible()
			&& m_PlayStatus == PS_BACKUP_START_STATUS)
		{
			ShowWindow(SW_SHOW);
			m_pMainFrame->SetDisplayActive(this);
			m_pMainFrame->DoAutoViewMode();
		}
		
		bRet = TRUE;
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
long CBackupWindow::OnBackupCheckStatus(WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	switch (wParam)
	{

	case BS_DCD_NOT_FOUND_EMPTY_CDR:
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** Msg from WatchCD: No empty CD-R found in drive.\n"));
		}
		m_LastPlayStatus = m_PlayStatus;
		m_dwDCDWait = GetTickCount();
		ChangePlayStatus(PS_BACKUP_DCD_NO_EMPTY_CDR);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
		break;

	case BS_DCD_FOUND_NOT_EMPTY_CDR:
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** Msg from WatchCD: Found not empty CD-R in drive.\n"));
		}
		m_LastPlayStatus = m_PlayStatus;
		m_dwDCDWait = GetTickCount();
		ChangePlayStatus(PS_BACKUP_DCD_FOUND_NOT_EMPTY_CDR);

		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
		break;

	case BS_DCD_FINALIZE_SUCCESS:
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** Msg from WatchCD: CD-R successfully finalized .\n"));
		}
		m_dwMsgTimeCompleteBackup = GetTickCount();
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_COMPLETE);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}
		break;

	
	case BS_DCD_UNEXPECTED_ERROR:
		WK_TRACE(_T("Msg from WatchCD: Finalizing CD-R failed.\n"));
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_FAILED);
		WK_TRACE(_T("OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		break;

	case BS_DCD_FORMATTING:
		WK_TRACE(_T("Msg from WatchCD: Formatting CD-R started.\n"));
		m_LastPlayStatus = m_PlayStatus;
		m_dwRunningTime = 0;
		if (m_uTimerProgressFormat)
		{
			KillTimer(m_uTimerProgressFormat);
		}
		m_uTimerProgressFormat = SetTimer(2, 1000, NULL);
		m_dwStartFormat = 0;
		ChangePlayStatus(PS_BACKUP_DCD_WAIT_FORMAT);

		WK_TRACE(_T("OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		break;

	case BS_DCD_START_BACKUP:
		WK_TRACE(_T("Msg from WatchCD: CD-R successfully formatted .\n"));
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_REQUEST);
		WK_TRACE(_T("OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

		break;

	case BS_DCD_COPYING_DATA:
		//WatchCD fragt alle 60 sec. nach ob DVClient noch Daten auf CD-R kopiert
		//falls das nicht mehr der Fall sein sollte (Absturz von DVClient oder 
		//fehlerhafte CD-R) befindet sich DVClient nicht mehr in einem der
		//Backup Stati und sendet auch nichts zurück. Nach 30 sec. Timeout gibt es dann
		//in WatchCD ein Error. 
		if(m_PlayStatus >= PS_BACKUP_RUNNING && m_PlayStatus <= PS_BACKUP_COPYING_RO)
		{
			::PostMessage((HWND)lParam, WM_CHECK_BACKUP_STATUS, wParam, 0);
		}
		break;

	case BS_DCD_ERROR:
		WK_TRACE(_T("Msg from WatchCD: An error occured during backup.\n"));
		m_LastPlayStatus = m_PlayStatus;
		ChangePlayStatus(PS_BACKUP_FAILED);
		WK_TRACE(_T("OnBackupCheckStatus(): playstatus: from %s -> to: %s\n"),
			NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		break;
	default:
		break;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::SetMediumCapacityInMB(DWORD dwCapacityInMB)
{
	BOOL bRet = FALSE;
	DWORD dwCopyROFile = GetBytesReadOnlySoftware();
	DWORD dwReservedMB = 1024 * 1024;
	ULONGLONG ullTotalMB = 0;
	if((dwCapacityInMB * 1024 * 1024) > (dwCopyROFile + dwReservedMB))
	{
		bRet = TRUE;
		ullTotalMB = dwCapacityInMB;
		ullTotalMB = (ullTotalMB * 1024 * 1024) - dwCopyROFile - dwReservedMB;
		
		//um Rundungsfehler zu beseitigen, kurze vor- und zurück Rechnung
		ullTotalMB = (ullTotalMB / (1024 * 1024)) *1024 * 1024;
	}

	m_i64CDCapacity = ullTotalMB;
	TRACE(_T("SetMediumCapacityInMB() Capacity: %d MB\n"), m_i64CDCapacity.GetInMB());

	return bRet;

}
/////////////////////////////////////////////////////////////////////////////
DWORD CBackupWindow::GetBytesReadOnlySoftware()
{
	CString sLanguage;
	int iOem = OemDTS;
	int nCameras = 0;
	CPanel* pPanel = theApp.GetPanel();

	if (pPanel)
	{
		iOem = pPanel->GetOem();
		sLanguage = pPanel->GetCurrentLanguage();
	}
	
	DWORD dwCopyROFile = 10 * 1024 * 1024;
	CCopyReadOnlyVersion CopyRO(_T(""),iOem,nCameras,sLanguage,TRUE);
	dwCopyROFile = CopyRO.GetBytesReadOnlySoftware();

	return dwCopyROFile;
}
/////////////////////////////////////////////////////////////////////////////
CIPCInt64 CBackupWindow::GetCDCapacity()
{
	__int64 iRet = 600 * 1024 * 1024;
	CWK_Profile prof;

	int iCDCapacity = prof.GetInt(REGKEY_DVCLIENT, _T("CDCapacity"), 0);
	if(iCDCapacity > 0 && iCDCapacity <= 700)
	{
		iRet = iCDCapacity * 1024 * 1024;
	}

	return iRet;
}
/////////////////////////////////////////////////////////////////////////////
//Coverts SystemTime to Date Time String without millisec.
CString CBackupWindow::SystemTimeToString(CSystemTime st)
{
	
	CString sYear, sDate, sTime, s;
	sYear.Format(_T("%04d"), st.GetYear());
	sYear = sYear.Right(2);
	sDate.Format(_T("%02d%02d"), st.GetDay(), st.GetMonth());
	sDate += sYear;

	sTime.Format(_T("%02d%02d%02d"), st.GetHour(),
								 st.GetMinute(),
								 st.GetSecond());
	return (sDate + sTime);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::ChangePlayStatus(PlayStatus playstatus)
{
	if (m_PlayStatus != playstatus)
	{
		m_PlayStatus = playstatus;
		CPanel* pPanel = theApp.GetPanel();
		
		if (WK_IS_WINDOW(this))
		{
			DoRedraw();
		}

		if(pPanel)
		{
			pPanel->UpdateMenu();
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetPlayStatus(PlayStatus playstatus)
{
	m_LastPlayStatus = m_PlayStatus;

	ChangePlayStatus(playstatus);
	WK_TRACE(_T("CBackupWindow::SetPlayStatus(): playstatus: from %s -> to: %s\n"),
		NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::RecalcBackupCountdown()
{
	DWORD dwElapsed = GetTimeSpan(m_dwStartTime);

	if (dwElapsed>0)
	{
		if (m_i64BytesBackuped>0)
		{
			m_dwBytesPerSecond = (DWORD)((m_i64BytesBackuped*1000) / dwElapsed);
		}
	}
	if(m_i64BytesToBackup > 0)
	{
		m_dwPercentBackuped = (DWORD)(100 * m_i64BytesBackuped / m_i64BytesToBackup);
	}
		
//	WK_TRACE(_T("RecalcBackupCountdown Bytes Backuped : %d\n"), m_i64BytesBackuped);
//	WK_TRACE(_T("RecalcBackupCountdown Bytes to Backup: %d\n"), m_i64BytesToBackup);
//	WK_TRACE(_T("RecalcBackupCountdown Time Elapsed   : %d\n"), dwElapsed);
}

/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::ConfirmBackupByTime(WORD wUserData, DWORD dwID, WORD wFlags, 
										CIPCInt64 i64Size, 
										const CSystemTime& stStartLessMaxCDCapacity,
										const CSystemTime& stEndMoreMaxCDCapacity)
{
	if(m_bCalculateError)
	{
		if(dwID != 0)
		{
			m_i64BytesToBackup = i64Size.GetInt64();
			//zurückgeliefertes Archiv enthält Bilder im Backup-Zeitraum	
			WORD wArchivID = HIWORD(dwID);
			for(int i=0; i < m_waBackupCameras.GetSize(); i++)
			{
				if(m_waBackupCameras.GetAt(i) == wArchivID)
				{
					if(m_bTKRTraceBackup)
					{
						WK_TRACE(_T("TKR ########### ERROR Bilder vorhanden: %d  ArrayNr: %i\n"), wArchivID,i);
					}
					m_waBackupCamerasNoImages.SetAt(i, 1);
					break;
				}
			}
			CString sCam, sMB;
			sCam.Format(_T("%d"), wArchivID);
			sMB = i64Size.Format(TRUE);
			if(m_bTKRTraceBackup)
			{
				WK_TRACE(_T("TKR########## sCam: %s  MB`s: %s\n"),sCam,sMB);
				WK_TRACE(_T("TKR----- Calc timeout 64Size: %s\n"), i64Size.Format(TRUE));
				WK_TRACE(_T("TKR------ Calc timeout m_i64CDCapacity: %s\n"), m_i64CDCapacity.Format(TRUE));
			}
			double flPercent = ((double)(i64Size.GetInt64()) * 100) / (double)m_i64CDCapacity.GetInt64();
			
			//2 Nachkommastellen ausgeben bei weniger als 1 promille CD Ausnutzng
			if(flPercent < 0.1)
			{
				m_sCDPercentUsed.Format(_T("%.02f"), flPercent);
			}
			else
			{
				m_sCDPercentUsed.Format(_T("%.01f"), flPercent);
			}
			
			if(i64Size.GetInt64() < m_i64CDCapacity.GetInt64())
			{
				//Bilder (MB`s) passen auf CD, Backup kann gestartet werden
				m_LastPlayStatus = m_PlayStatus;
				theApp.GetPanel()->SetTooManyImages(FALSE);
				ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS);
				WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
			else
			{
				//Bilder (MB`s) passen nicht auf CD, erneute Eingabe Endzeitpunkt durch Benutzer veranlassen
				theApp.GetPanel()->SetTooManyImages(TRUE);
				theApp.GetPanel()->SetCalculateEndComplete();
			}
		}
		else
		{
			m_i64BytesToBackup = i64Size.GetInt64();
			if(m_i64BytesToBackup == 0)
			{
				//keine Bilder (MB`s) hinter Startzeitpunkt gefunden
				m_LastPlayStatus = m_PlayStatus;
				theApp.GetPanel()->SetTooManyImages(FALSE);
				ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS_MIN);
				WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
			}
		}
	}
	else
	{
		// no m_bCalculateError
		if (   wFlags == BBT_CALCULATE
			|| wFlags == BBT_CALCULATE_END)
		{
			m_i64BytesToBackup = i64Size.GetInt64();
			if (dwID == 0)
			{	
				//Ende der gesamten Berechnung
				//führe die Intervallschachtelung fort

				//nur beim ersten Aufruf hier rein, da hier die zurückgelieferte Berechnung 
				//sofort über 600 MB liegt, bzw. das Ende der Datenbank erreicht wurde
				if(wFlags == BBT_CALCULATE_END)
				{

					FILETIME ftDiff_StartLess_BackupStart, ftDiff_EndMore_BackupStart;
					LARGE_INTEGER liDiff_StartLess_BackupStart, liDiff_EndMore_BackupStart;
					CSystemTime stDiff_StartLess_BackupStart;
					CSystemTime stDiff_EndMore_BackupStart;

					//Abstand zwischen Zeitpunkt BackupStart und dem jeweiligen berechnetem Ende,
					//einmal vor und einmal nach 600MB, berechnen

					//der kleinere zurückgelieferte Zeitpunkt darf nicht vor dem Start des 
					//Backups liegen
					if(m_stBackupStart >= stStartLessMaxCDCapacity)
					{
						//BackupStart liegt innerhalb der letzten Sequence und 
						//stDiff_StartLess_BackupStart ist der Sequenceanfang
						CSystemTime st;
						stDiff_StartLess_BackupStart = st;
					}
					else
					{
						stDiff_StartLess_BackupStart = stStartLessMaxCDCapacity - m_stBackupStart;
					}

					//der größere zurückgelieferte Zeitpunkt darf nicht hinter dem Datenbankende
					//liegen, falls doch, dann auf Datenbankende setzen
					if(m_stNewestDBImage <= stEndMoreMaxCDCapacity)
					{
						stDiff_EndMore_BackupStart = m_stNewestDBImage - m_stBackupStart;
						m_stCalculatedEnd = m_stNewestDBImage;
					}
					else
					{
						stDiff_EndMore_BackupStart = stEndMoreMaxCDCapacity - m_stBackupStart;

						m_stCalculatedEnd = stEndMoreMaxCDCapacity;
					}
					

					//den jeweils berechneten Abstand in Sekunden bestimmen
					SystemTimeToFileTime(&stDiff_StartLess_BackupStart, &ftDiff_StartLess_BackupStart);
					SystemTimeToFileTime(&stDiff_EndMore_BackupStart, &ftDiff_EndMore_BackupStart);
					
					liDiff_StartLess_BackupStart.HighPart	= ftDiff_StartLess_BackupStart.dwHighDateTime;
					liDiff_StartLess_BackupStart.LowPart	= ftDiff_StartLess_BackupStart.dwLowDateTime;

					liDiff_EndMore_BackupStart.HighPart		= ftDiff_EndMore_BackupStart.dwHighDateTime;
					liDiff_EndMore_BackupStart.LowPart		= ftDiff_EndMore_BackupStart.dwLowDateTime;

					//Ergebnis von Zeitschritt des LARGE_INTEGER 100 Nanosekunden
					//in Zeitschritt 1 Sekunde umrechnen
					liDiff_StartLess_BackupStart.QuadPart	 = liDiff_StartLess_BackupStart.QuadPart/(10*1000*1000);
					liDiff_EndMore_BackupStart.QuadPart		 = liDiff_EndMore_BackupStart.QuadPart/(10*1000*1000);

					//jeweils berechneten Abstand zum BackupStart setzen
					m_dwLastCurrentSeconds = (DWORD)liDiff_StartLess_BackupStart.QuadPart;
					m_dwCurrentSeconds = (DWORD)liDiff_EndMore_BackupStart.QuadPart;

				}
				if(m_bTKRTraceBackup)
				{
					WK_TRACE(_T("TKR----- 64Size: %s\n"), i64Size.Format(TRUE));
					WK_TRACE(_T("TKR------ m_i64CDCapacity: %s\n"), m_i64CDCapacity.Format(TRUE));
				}
				double flPercent = ((double)(i64Size.GetInt64()) * 100) / (double)m_i64CDCapacity.GetInt64();
				
				//2 Nachkommastellen ausgeben bei weniger als 1 promille CD Ausnutzng
				if(flPercent < 0.1)
				{
					m_sCDPercentUsed.Format(_T("%.02f"), flPercent);
				}
				else
				{
					m_sCDPercentUsed.Format(_T("%.01f"), flPercent);
				}
				
				if(flPercent > 0)
				{
					m_sLastCDPercentUsed = m_sCDPercentUsed;
				}

				if(m_bTKRTraceBackup)
				{
					WK_TRACE(_T("TKR----- calculated bytes : %s\n"), i64Size.Format(TRUE));
					WK_TRACE(_T("TKR----- free Bytes on disc: %s\n"), m_i64CDCapacity.Format(TRUE));
					WK_TRACE(_T("TKR----- Prozent CD Auslastung: %s\n"), m_sCDPercentUsed);
				}

				//Beginn erneute DB-Anfrage nach MB`s mit benutzerspezifischem Endzeitpunkt, 
				//nicht mit berechnetem. (wichtig für korrekte Prozentanzeige im Backup Fenster)
				if(m_PlayStatus == PS_BACKUP_REQUEST_MB)
				{
					//Benutzer hat nicht vorgeschlagenen Endzeitpunkt gewählt, sondern
					//einen anderen. Sichere berechnete MB`s für Prozentanzeige beim Backup und
					//teile dem Panel mit, ob MB`s gefunden wurden
					m_LastPlayStatus = m_PlayStatus; 
					m_CalcMBUserBackupEnd = m_PlayStatus;

					ChangePlayStatus(PS_BACKUP_BACKUP_ACTIVATED);

					WK_TRACE(_T("OnBackupRequest(): playstatus: from %s -> to: %s\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));

					if(m_i64BytesToBackup == 0)
					{
						theApp.GetPanel()->SetEventCalculateMBUserBackupEnd(FALSE);
					}
					else
					{
						theApp.GetPanel()->SetEventCalculateMBUserBackupEnd(TRUE);
					}
				}

				//Ende erneute DB-Anfrage nach MB`s mit benutzerspezifischem Endzeitpunkt, 
				else
				{
					if(m_PlayStatus != PS_BACKUP_BACKUP_ACTIVATED)
					{
						if((i64Size.GetInt64() < m_i64CDCapacity.GetInt64())) 
						{
							if(i64Size.GetInt64() == 0)
							{
								//keine MB`s gefunden
								if(!IsNewestDBImageReached())
								{
									//Suche mit größerem Datum fortsetzen
									if(m_bTKRTraceBackup)
									{
										WK_TRACE(_T("TKR############# Keine MB`s gefunden, Suche mit größerem Datum weiter\n"));
									}
									OnCalculateEnd_Start(TRUE);
								}
								else
								{
									m_LastPlayStatus = m_PlayStatus;
									if(m_bTKRTraceBackup)
									{
										WK_TRACE(_T("TKR############# Keine MB`s gefunden, gleichzeitig DB Ende erreicht\n"));
									}
									//Suche beenden, Datenbank-Ende (aktueller Zeitpunkt) erreicht
									ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS_MIN);
									WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
										NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
								}

							}
							else
							{
								if(!m_bStopDBSearch)
								{
									if(i64Size.GetInt64() > m_i64CDCapacity.GetInt64())
									{
										m_LastPlayStatus = m_PlayStatus;
										//maximale Kapazität der CD-R erreicht, Suche beenden
										if(m_bTKRTraceBackup)
										{
											WK_TRACE(_T("TKR############# Ende Berechnung, CD Kapazität auf 0,5 %% erreicht\n"));
										}
										ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS_MAX);
										WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
											NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
									}
									else
									{

										if(!IsNewestDBImageReached())
										{
											if(m_stCalculatedEnd == m_stLastCalculatedEnd)
											{
												m_LastPlayStatus = m_PlayStatus;
												m_i64BytesToBackup = i64Size.GetInt64();
												//kalkulierter Endzeitpunkt ändert sich nicht mehr, Suche beenden
												if(m_bTKRTraceBackup)
												{
													WK_TRACE(_T("TKR############# Endzeitpunkt bleibt gleich, Suche beenden\n"));
												}
												ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS_MAX);
												WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
													NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
											}
											else
											{
												//noch nicht genug MB`s gefunden, Suche mit größerem Datum fortsetzen
												if(m_bTKRTraceBackup)
												{
													WK_TRACE(_T("TKR############# nicht genug MB, weiter suchen, letzte CurrentSec sichern\n"));
												}
												m_dwLastNotEnoughMB = m_dwCurrentSeconds;
												OnCalculateEnd_Start(TRUE);
											}
										}
										else
										{
											m_LastPlayStatus = m_PlayStatus;
											if(m_bTKRTraceBackup)
											{
												m_i64BytesToBackup = i64Size.GetInt64();
												TRACE(_T("TKR --------- BytesToBackup: %d\n"), i64Size.GetInt64());
												//Suche beenden, Datenbank-Ende (aktueller Zeitpunkt) erreicht
												WK_TRACE(_T("TKR############# vorzeitiges Ende Berechnung, da DB-Ende erreicht\n"));		
											}
											ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS);
											WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
												NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
										}
									}
									

								}
								else
								{
									m_LastPlayStatus = m_PlayStatus;
									if(m_bTKRTraceBackup)
									{
										m_i64BytesToBackup = i64Size.GetInt64();
										//Suche beenden, da kalkulierte Endzeit nicht mehr durch 2 teilbar
										//und nun die letzte Endzeit benutzt wird, bei der noch nicht
										//genug MB`s gefunden wurden
										WK_TRACE(_T("TKR############# Berechnung beendet, letzte zulässige Sekunde benutzen\n"));
									}
									ChangePlayStatus(PS_BACKUP_CALCULATE_END__SUCCESS_MAX);
									WK_TRACE(_T("CBackupWindow::ConfirmBackupByTime(): playstatus: from %s -> to: %s\n"),
										NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
								}
							}
						}
						else
						{
							if(IsNewestDBImageReached())
							{
								if(m_bTKRTraceBackup)
								{
									WK_TRACE(_T("TKR########## zuviele MB`s und gleichzeitig Datenbank Ende erreicht\n"));
								}
								//Ist nächster Berechnungsschritt (zB von 32h auf 64h) größer als
								//das Datenbank Ende ( > m_stNewestDBImage) dann das Datenbank Ende als nächsten
								//Berechnungsschritt wählen.
								//Ist das anschließende Ergebnis größer als 600MB dann den Schritt zwischen
								//32h und Datenbank Ende als nächsten Berechnugsschritt nehmen

								OnCalculateEnd_Start(FALSE);

							}
							else
							{
								//zuviele MB`s gefunden, suche mit halber Schrittweite weiter
								if(m_bTKRTraceBackup)
								{
									WK_TRACE(_T("TKR########## zuviele MB`s gefunden, mit halber Schrittweite zurück\n"));
								}
								OnCalculateEnd_Start(FALSE);
							}

						}
					}
				}
			}
			else  // dwID != 0
			{
				// zurückgeliefertes Archiv enthält Bilder im Backup-Zeitraum	
				WORD wArchivID = HIWORD(dwID);
				m_wCamBackupEnd = wArchivID;
				if(i64Size.GetInt64() > 0)
				{
					for(int i=0; i < m_waBackupCameras.GetSize(); i++)
					{
						if(m_waBackupCameras.GetAt(i) == wArchivID)
						{
							//TRACE(_T("TKR ########### Bilder vorhanden: %d  ArrayNr: %i\n"), wArchivID,i);
							if(m_waBackupCamerasNoImages.GetSize() > i)
							{
								m_waBackupCamerasNoImages.SetAt(i, 1);
							}

							break;
						}
					}
				}
 
				CString sCam, sMB;
				sCam.Format(_T("%d"), wArchivID);
				sMB = i64Size.Format(TRUE);
				//TRACE(_T("TKR########## sCam: %s  MB`s: %s\n"),sCam,sMB);
			}
		}
		else
		{
			// keine Berechnung, echtes Backup
			// Backup-Daten werden kopiert
			m_i64BytesBackuped = i64Size.GetInt64();
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
//fragt jede übergebene Kamera nach dem ältesten Bild ab
BOOL CBackupWindow::RequestFirstImageInDB(CString sCameras, CSystemTime& stLatestDBImage)
{
	BOOL bRet = FALSE;
	CStringArray saCameras;
	CWordArray waCameras;
	CWordArray waCamNr;
	CIPCSequenceRecord* pFirstSequence = NULL;
	WORD wCameraNr;

	CIPCDatabaseDVClient* pDatabase = NULL;
	
//	sCameras = sCameras.Right(sCameras.GetLength()-1);

	SplitString(sCameras,saCameras,',');
	
	if(m_pServer)
	{
		pDatabase = m_pServer->GetDatabase();
		
		if(pDatabase && m_pMainFrame)
		{
			for (int i=0;i<saCameras.GetSize();i++)
			{
				CString s = saCameras.GetAt(i);
				wCameraNr = (WORD)(_ttoi(saCameras[i])-1);
				CPlayWindow* pPW = m_pMainFrame->GetPlayWindow((WORD)wCameraNr);

				if(pPW)
				{
					pFirstSequence = pPW->GetFirstNotBackupSequence();
					if (pFirstSequence)
					{
						CSystemTime stCurrentSequence(pFirstSequence->GetTime());
						if(stLatestDBImage > stCurrentSequence)
						{
							stLatestDBImage.wYear	= stCurrentSequence.GetYear();
							stLatestDBImage.wMonth	= stCurrentSequence.GetMonth();
							stLatestDBImage.wDay	= stCurrentSequence.GetDay();
							stLatestDBImage.wHour	= stCurrentSequence.GetHour();
							stLatestDBImage.wMinute	= stCurrentSequence.GetMinute();
							stLatestDBImage.wSecond	= stCurrentSequence.GetSecond();
							stLatestDBImage.wMilliseconds = stCurrentSequence.GetMilliseconds();
							bRet = TRUE;
						}
					}
				}
				else
				{
					WK_TRACE(_T("TKR -- Camera to backup does not exists\n"));
				}
			}
		}
		else
		{
			WK_TRACE(_T("TKR -- No Database or Mainframe exists\n"));
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::InitMultiBackupMembers(CSystemTime stMultiBackupStartLast, 
										   CSystemTime stMultiBackupEndLast, 
										   CSystemTime stMultiBackupStartNew)
{
	m_stMultiBackupStartLast	= stMultiBackupStartLast;
	m_stMultiBackupEndLast		= stMultiBackupEndLast;
	m_stMultiBackupStartNew		= stMultiBackupStartNew;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetMultiBackup(BOOL bMultiBackup)
{
	m_bMultiBackup = bMultiBackup;

}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetLastRecordLocalized(WORD wLastSequence, 
										   DWORD dwLastRecord, 
										   BOOL bConfirmOK /* = TRUE */)
{
	//TRACE(_T("tkr #### SetLastRecordLocalized\n"));
	//wenn bConfirmOK == FALSE kam Aufruf aus OnConfirmQuery und bedeutet, 
	//dass kein Bild gefunden wurde
	if(bConfirmOK)  
	{
		m_wLastSequenceBackupEnd = wLastSequence;
		m_dwLastRecordBackupEnd = dwLastRecord;
		m_bLastRecordLocalized = TRUE;
	}

}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SaveMultiBackupMembers()
{
	CPanel* pPanel = theApp.GetPanel();

	if(pPanel)
	{
		pPanel->SetMultiBackupMembers(m_stBackupStart,
									  m_stBackupEnd,
									  m_wCamBackupEnd,
									  m_sMultiBackupCamsSeperated,
									  m_wLastSequenceBackupEnd,
									  m_dwLastRecordBackupEnd);
	}
}
/////////////////////////////////////////////////////////////////////////////
//vergrößere m_stCalculatedEnd um m_dwCurrentSeconds
//Zeitpunkt m_stCalculatedEnd darf anschließend nicht größer als
//neustes Datenbank-Bild (m_stNewestDBImage) 
void CBackupWindow::IncrementCalculatedEnd()
{
	m_stCalculatedEnd.IncrementTime(0,0,0,m_dwCurrentSeconds,0);

	if(m_stCalculatedEnd >= m_stNewestDBImage)
	{
		m_stCalculatedEnd.wYear = m_stNewestDBImage.GetYear();
		m_stCalculatedEnd.wMonth = m_stNewestDBImage.GetMonth();
		m_stCalculatedEnd.wDay = m_stNewestDBImage.GetDay();
		m_stCalculatedEnd.wHour = m_stNewestDBImage.GetHour();
		m_stCalculatedEnd.wMinute = m_stNewestDBImage.GetMinute();
		m_stCalculatedEnd.wSecond = m_stNewestDBImage.GetSecond();
		m_stCalculatedEnd.wMilliseconds = m_stNewestDBImage.GetMilliseconds();
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR############ Calculated End größer gleich Datenbank Ende\n"));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//vergleicht Zeitpunkt CalculatedEnd mit Zeitpunkt NewestDBImage
BOOL CBackupWindow::IsNewestDBImageReached()
{
	if(m_stCalculatedEnd >= m_stNewestDBImage)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
//setzt das flag zum Beenden der laufenden Berechnung des Endzeitpunktes der ersten CD-R 
void CBackupWindow::SetStopCalculate(BOOL bStopCalculate)
{
	m_bStopCalculate = bStopCalculate;
}

/////////////////////////////////////////////////////////////////////////////
//setzt das flag zum schließen des Backupfensters,
//wenn m_bCloseBackupWindow == TRUE, dann wird kein neues DoRequestBackupByTime() aufgerufen
void CBackupWindow::SetCloseBackupWindow(BOOL bCloseBackupWindow, 
										 BOOL bExternDriveRemoved /* = FALSE */)
{
	m_bCloseBackupWindow = bCloseBackupWindow;

	//setze ErrorStatus um später auszugeben, welcher Error auftrat
	if(bExternDriveRemoved)
	{
		m_LastPlayStatus = m_PlayStatus;
		m_ErrorStatus = ES_EXTERN_BACKUP_DRIVE_REMOVED;
		m_dwBackupFailed = 0;
		ChangePlayStatus(PS_BACKUP_FAILED);
		if(m_bTKRTraceBackup)
		{
			WK_TRACE(_T("TKR***** CBackupWindow::StartBackup(): playstatus: from %s -> to: %s\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus));
		}		
	}
}

/////////////////////////////////////////////////////////////////////////////
//liefert den berechneten Endzeitpunkt formatiert zurück
CString CBackupWindow::GetCalculatedEndFormatted()
{
	CString sYear, sDate, sTime;
	sYear.Format(_T("%04d"), m_stCalculatedEnd.GetYear());
	sYear = sYear.Right(2);
	sDate.Format(_T("%02d%02d"), m_stCalculatedEnd.GetDay(),
							 m_stCalculatedEnd.GetMonth());
	sDate += sYear;
	sTime.Format(_T("%02d%02d%02d"), m_stCalculatedEnd.GetHour(),
								 m_stCalculatedEnd.GetMinute(),
								 m_stCalculatedEnd.GetSecond());
	return sDate + sTime;
}

/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetTimeACDCNoEmptyCD()
{
	m_dwMsgTimeACDCNoEmptyCD = GetTickCount();
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetTimeNotErasedMedium()
{
	m_dwMsgTimeNotErasedMedium = GetTickCount();
}

/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetACDCEraseProgress(BOOL bErase)
{
	m_bACDCEraseProgress = bErase;
}

/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::GetACDCTempPath()
{
	return m_sACDCTempPath;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::DeleteACDCTempPath()
{
	BOOL bRet = FALSE;

	if(!m_sACDCTempPath.IsEmpty())
	{
		if(DoesFileExist(m_sACDCTempPath))
		{
			if(!DeleteDirRecursiv(m_sACDCTempPath))
			{
				WK_TRACE_ERROR(_T("DeleteACDCTempPath: Could not delete ACDC Temp Path: %s\n"), m_sACDCTempPath);	
			}
			else
			{
				WK_TRACE(_T("DeleteACDCTempPath: ACDC Temp Path deleted successfully\n"));
				bRet = TRUE;
			}
		}
	}

	//sind noch alte, ungelöschte ACDCtmp Folder vorhanden, dann löschen
	CFileFind finder;

	m_sACDCTempPath.Empty();



	CString sPath;
	CString sTempLocation;
	CString sFind;
	CString sKey = _T("BackupTempLocation");
	CString sSection = REGKEY_DVCLIENT;
	CWK_Profile prof;

	//LPTSTR sBuffer = sTempLocation.GetBufferSetLength(MAX_PATH);
	//GetTempPath(MAX_PATH, sBuffer);
	//GetLongPathName(sBuffer, sBuffer, MAX_PATH);
	//sTempLocation.ReleaseBuffer();

	sTempLocation = prof.GetString(sSection, sKey, sTempLocation);
	//prof.WriteString(sSection, sKey, sTempLocation);

	sFind.Format(_T("%sACDCtmp*.*"), sTempLocation);

	BOOL bFound = finder.FindFile(sFind);
    while (bFound)
	{
	   bFound = finder.FindNextFile();

	   // skip . and .. files; otherwise, we'd
	   // recur infinitely!
	   if (finder.IsDots())
 		 continue;

	   if (finder.IsDirectory())
	   {
	 	  CString str = finder.GetFilePath();
		  if(!str.IsEmpty())
		  {
			 DeleteDirRecursiv(str);
		  }
	   }
	}


	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::IsAlarm() const
{
	BOOL bRet = FALSE;
	CPanel* pPanel = theApp.GetPanel();

	//ein Alarm Backup liegt vor, wenn m_bAlarmBackup == TRUE ist
	//und das Alarmarchiv (Playwindow) geöffnet und als aktuelles Fenster (Cam Nr im Panel)
	//angezeigt wird
	if (pPanel)
	{
		int iActiveCam = pPanel->GetActiveCamera();

		CPlayWindow* pPlayWnd = theApp.GetMainFrame()->GetPlayWindow((WORD)iActiveCam);
		if (   pPlayWnd
			&& !pPlayWnd->IsMD())	
		{
			bRet = pPlayWnd->IsAlarm();
		}
	}
	return m_bAlarmBackup && bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBackupWindow::GetRemovableDrive(CString &sRemovableDrive, int &iDriveBit)
{
	DWORD dwLD = GetLogicalDrives();
	CString sDrive;
	TCHAR  cDrive = _T('A');
	DWORD dwBit = 8;
	int nCount = 3;
	
	while (dwBit!=0)
	{
		if (dwLD & dwBit)
		{
			cDrive = (_TCHAR)(_T('A') + nCount);
			sDrive.Format(_T("%c:\\"), cDrive);
			
			if(GetDriveType(sDrive) == DRIVE_REMOVABLE)
			{
				sRemovableDrive = sDrive;
				sRemovableDrive.MakeLower();
				iDriveBit = dwBit;
				return TRUE;
			}
		}
			
		dwBit <<= 1;
		nCount++;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CBackupWindow::GetBytesReadOnlySoftwareFromExternMedium()
{
	DWORD dwMBROFromMedium = 0;
	CString sLanguage;
	CStringArray saReadOnlyFiles;
	int iOem = OemDTS;
	int nCameras = 0;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		iOem = pPanel->GetOem();
		sLanguage = pPanel->GetCurrentLanguage();
	}

	CCopyReadOnlyVersion CopyRO(_T(""),iOem,nCameras,sLanguage,TRUE);

	CopyRO.GetReadOnlyFiles(saReadOnlyFiles);
	for(int i=0; i < saReadOnlyFiles.GetSize(); i++)
	{
		CString sFile = saReadOnlyFiles.GetAt(i);
		CString sRemovableDrive;
		int iDriveBit;
		if(GetRemovableDrive(sRemovableDrive, iDriveBit))
		{
			CString sFilePath;
			sFilePath.Format(_T("%s%s"), sRemovableDrive, sFile);
			if(DoesFileExist(sFilePath))
			{
				WK_TRACE(_T("ROFiles on extern Medium: %s\n"), sFilePath);
				CFile ReadOnlyFile(sFilePath, CFile::modeRead);
				dwMBROFromMedium += (DWORD)ReadOnlyFile.GetLength();
			}
		}
	}

	return dwMBROFromMedium;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetDeleteExternBackupDrive(BOOL bDelete)
{
	m_bDeleteExternBackupDrive = bDelete;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::SetPrepareStopBackup(BOOL bPrepareStopBackup)
{
	m_bPrepareStopBackup = bPrepareStopBackup;

	if(bPrepareStopBackup)
	{
		//release removal of backup drive tray (CD/DVD drive) 
		//when pushing eject button on the recorder
		BOOL bPrevent = FALSE;
		PreventBackupDriveRemoval(bPrevent,(const _TCHAR)m_pBackupDrive->GetLetter());
	}
}

/////////////////////////////////////////////////////////////////////////////
//prevents or alouds ejecting bachup drives tray while pushing recorders eject button
BOOL CBackupWindow::PreventBackupDriveRemoval(BOOL bPrevent, const _TCHAR cDriveLetter)
{
	BOOL bResult = FALSE;               
	HANDLE hDevice;               // handle to the drive to be examined 
	CString sDrive = cDriveLetter;
	CString sError;

	sDrive = _T("\\\\.\\") + sDrive + _T(":");


	//create file to get handle from specified drive
	hDevice = CreateFile(sDrive,					// drive to open
					     GENERIC_READ,				// access mode
					     0,							// share mode
					     NULL,						// default security attributes
					     OPEN_EXISTING,				// disposition
					     0,							// file attributes
					     0);						// don't copy any file's attributes

	if (hDevice != INVALID_HANDLE_VALUE) // we can't open the drive
	{

		PREVENT_MEDIA_REMOVAL Lock;
		Lock.PreventMediaRemoval = (BOOLEAN)bPrevent;
		DWORD nInBufferSize = sizeof(Lock);
		DWORD dwBytesReturned = 0;

		//prevent eject drives tray
		bResult = DeviceIoControl(hDevice,						// handle to device
								  IOCTL_STORAGE_MEDIA_REMOVAL,	// Control code for the operation
								  &Lock,						// input buffer 
								  nInBufferSize,				// size of input buffer 
								  NULL,							// output buffer
								  0,							// output buffer size
								  &dwBytesReturned,				// number of bytes returned
								  NULL);						// OVERLAPPED structure
   
		CloseHandle(hDevice);         // we're done with the handle
	}

	
	bPrevent ? sError = _T("Lock ") : sError = _T("Unlock ");
	if(bResult)
	{	

		WK_TRACE(_T("Prevent removal: ") + sError + _T("backup drive was successful\n"));
	}
	else
	{
		CString sErrorNr = GetLastErrorString();
		WK_TRACE_WARNING(_T("Prevent removal: ") + sError + _T("backup drive failed: %s"),sErrorNr);
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::OnTimer(UINT nIDEvent) 
{

	if (nIDEvent == m_uTimerProgressGeneric)
	{
		if (    m_PlayStatus == PS_BACKUP_RUNNING
			||  m_PlayStatus == PS_BACKUP_ACDC_WAIT_COMPLETE
			||  m_PlayStatus == PS_BACKUP_START_ERASE_EXTERN_MEDIUM
			||  m_PlayStatus == PS_BACKUP_ERASING_EXTERN_MEDIUM)
		{
			if (m_dwCountUp>=0)
			{
				m_dwCountUp++;
				DoRedraw();
			}
		}
	}
	else if (nIDEvent == m_uTimerProgressFormat)
	{
		// time count up for formatting
		if(m_PlayStatus == PS_BACKUP_DCD_WAIT_FORMAT)
		{
			m_dwRunningTime++;
			DoRedraw();
		}
	}
	else if (nIDEvent == m_uTimerProgressFinalize)
	{
		// time count up for finalizing
		if(m_PlayStatus == PS_BACKUP_DCD_FINALIZE)
		{
			m_dwRunningTime++;
			DoRedraw();		
		}
	}
	else
	{
		WK_TRACE(_T("CPlayWindow()::OnTimer(): this method was called without a definied case\n"));
	}
	CDisplayWindow::OnTimer(nIDEvent);

}
/////////////////////////////////////////////////////////////////////////////
CString CBackupWindow::NameOfEnum(PlayStatus ps)
{
	switch (ps)
	{

	case PS_BACKUP_START_STATUS:			return _T("PS_BACKUP_START_STATUS");
	case PS_STOP:							return _T("PS_STOP");
	case PS_BACKUP_INPUT_CAMERAS:			return _T("PS_BACKUP_INPUT_CAMERAS");
	case PS_BACKUP_INPUT_START:				return _T("PS_BACKUP_INPUT_START");
	case PS_BACKUP_INPUT_MULTI:				return _T("PS_BACKUP_INPUT_MULTI");
	case PS_BACKUP_INPUT_MULTI_IS_RUNNING:	return _T("PS_BACKUP_INPUT_MULTI_IS_RUNNING");
	case PS_BACKUP_INPUT_WAIT_FOR_MEDIUM:	return _T("PS_BACKUP_INPUT_WAIT_FOR_MEDIUM");


	case PS_BACKUP_CALCULATE_END__START:	return _T("PS_BACKUP_CALCULATE_END__START");
	case PS_BACKUP_CALCULATE_END__MULTI_START:	return _T("PS_BACKUP_CALCULATE_END__MULTI_START");
	case PS_BACKUP_CALCULATE_END__WAIT_FOR:	return _T("PS_BACKUP_CALCULATE_END__WAIT_FOR");
	case PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR:	return _T("PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR");
	case PS_BACKUP_CALCULATE_END__SUCCESS:	return _T("PS_BACKUP_CALCULATE_END__SUCCESS");

	case PS_BACKUP_CALCULATE_END__SUCCESS_MAX:
											return _T("PS_BACKUP_CALCULATE_END__SUCCESS_MAX");
	case PS_BACKUP_CALCULATE_END__SUCCESS_MIN:
											return _T("PS_BACKUP_CALCULATE_END__SUCCESS_MIN");
	case PS_BACKUP_CALCULATE_END__FAILED:	return _T("PS_BACKUP_CALCULATE_END__FAILED");

	case PS_BACKUP_TOO_MANY_IMAGES:			return _T("PS_BACKUP_TOO_MANY_IMAGES");
	case PS_BACKUP_BACKUP_ACTIVATED:		return _T("PS_BACKUP_BACKUP_ACTIVATED");

	case PS_BACKUP_START:					return _T("PS_BACKUP_START");
	case PS_BACKUP_DCD_START:				return _T("PS_BACKUP_DCD_START");
	case PS_BACKUP_DCD_WAIT_EMPTY_CDR:		return _T("PS_BACKUP_DCD_WAIT_EMPTY_CDR");
	case PS_BACKUP_DCD_NO_EMPTY_CDR:		return _T("PS_BACKUP_DCD_NO_EMPTY_CDR");
	case PS_BACKUP_DCD_FOUND_NOT_EMPTY_CDR:	return _T("PS_BACKUP_DCD_FOUND_NOT_EMPTY_CDR");
	case PS_BACKUP_DCD_WAIT_FORMAT:			return _T("PS_BACKUP_DCD_WAIT_FORMAT");
	case PS_BACKUP_REQUEST:					return _T("PS_BACKUP_REQUEST");
	case PS_BACKUP_REQUEST_MB:				return _T("PS_BACKUP_REQUEST_MB");
	case PS_BACKUP_RUNNING:					return _T("PS_BACKUP_RUNNING");
	case PS_BACKUP_COPY_RO:					return _T("PS_BACKUP_COPY_RO");
	case PS_BACKUP_COPYING_RO:				return _T("PS_BACKUP_COPYING_RO");
	case PS_BACKUP_DCD_FINALIZE:			return _T("PS_BACKUP_DCD_FINALIZE");
	case PS_BACKUP_DCD_ERROR:				return _T("PS_BACKUP_DCD_ERROR");
	case PS_BACKUP_COMPLETE:				return _T("PS_BACKUP_COMPLETE");
	case PS_BACKUP_FAILED:					return _T("PS_BACKUP_FAILED");
	case PS_NAVIGATE_FAILED:				return _T("PS_NAVIGATE_FAILED");
	case PS_READ_FAILED:					return _T("PS_READ_FAILED");
	case PS_BACKUP_ACDC_ERASE_MEDIUM:		return _T("PS_BACKUP_ACDC_ERASE_MEDIUM");
	case PS_BACKUP_ACDC_START:				return _T("PS_BACKUP_ACDC_START");
	case PS_BACKUP_ACDC_WAIT_COMPLETE:		return _T("PS_BACKUP_ACDC_WAIT_COMPLETE");
	case PS_BACKUP_ACDC_COMPLETE:			return _T("PS_BACKUP_ACDC_COMPLETE");
	case PS_BACKUP_ACDC_NOT_ERASED:			return _T("PS_BACKUP_ACDC_NOT_ERASED");
	case PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM:return _T("PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM");
	case PS_BACKUP_ACDC_NO_EMPTY_CD:		return _T("PS_BACKUP_ACDC_NO_EMPTY_CD");
	case PS_BACKUP_ACDC_DVD_NOT_ALLOWED:	return _T("PS_BACKUP_ACDC_DVD_NOT_ALLOWED");
	case PS_BACKUP_ACDC_ERROR:				return _T("PS_BACKUP_ACDC_ERROR");
	case PS_BACKUP_ACDC_CANCEL_BACKUP:		return _T("PS_BACKUP_ACDC_CANCEL_BACKUP");
	case PS_BACKUP_ERASE_EXTERN_MEDIUM:		return _T("PS_BACKUP_ERASE_EXTERN_MEDIUM");
	case PS_BACKUP_START_ERASE_EXTERN_MEDIUM:
											return _T("PS_BACKUP_START_ERASE_EXTERN_MEDIUM");
	case PS_BACKUP_ERASING_EXTERN_MEDIUM:	return _T("PS_BACKUP_ERASING_EXTERN_MEDIUM");

	default:								return _T("unknown playstatus");
	}

}
/////////////////////////////////////////////////////////////////////////////
void CBackupWindow::OnDestroy()
{
	if (m_uTimerProgressGeneric)
	{
		KillTimer(m_uTimerProgressGeneric);
	}
	if (m_uTimerProgressFormat)
	{
		KillTimer(m_uTimerProgressFormat);
	}
	if (m_uTimerProgressFinalize)
	{
		KillTimer(m_uTimerProgressFinalize);
	}
	CDisplayWindow::OnDestroy();
}
