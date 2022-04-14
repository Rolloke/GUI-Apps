// BackupWindow.h: interface for the CBackupWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPWINDOW_H__C49E7F63_5FC7_11D5_8DBF_0050BF554CAA__INCLUDED_)
#define AFX_BACKUPWINDOW_H__C49E7F63_5FC7_11D5_8DBF_0050BF554CAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DisplayWindow.h"
#include "DirectCD.h"
#include "Winioctl.h" //for preventing backup drive removal with recorders eject button

#define MAX_MB  600
#define MIN_MB  595 
#define BACKUP_LAST_RECORD  99

class CMainFrame;
class CCopyReadOnlyVersion;
class CBackupEraseMedium;
class CBackupWindow : public CDisplayWindow  


{
	DECLARE_DYNAMIC(CBackupWindow)

public:
	enum PlayStatus
	{
		PS_STOP										=  0,

		PS_BACKUP_CALCULATE_END__START				=  1,
		PS_BACKUP_CALCULATE_END__MULTI_START		=  2,
		PS_BACKUP_CALCULATE_END__WAIT_FOR			=  3,
		PS_BACKUP_CALCULATE_END__MULTI_WAIT_FOR		=  4,
		PS_BACKUP_CALCULATE_END__SUCCESS			=  5, //Endzeitberechnung: Datenbankende erreicht
		PS_BACKUP_CALCULATE_END__SUCCESS_MIN		=  6, //Endzeitberechnung: Datenbankende erreicht
														  //aber keine Bilder gefunden
		PS_BACKUP_CALCULATE_END__SUCCESS_MAX		=  7, //Endzeitberechnung: max. CD Auslastung erreicht
		PS_BACKUP_CALCULATE_END__FAILED				=  8, //Endzeitberechnung schlug fehl, benutze 
														  //benutzerdef. Endzeitpunkt

		PS_BACKUP_INPUT_CAMERAS				=  20, //zeige Text bei Eingabe der Kameras
		PS_BACKUP_INPUT_START				=  21, //zeige Text bei Eingabe des Startzeitpunktes
		PS_BACKUP_INPUT_MULTI				=  22, //zeige Text bei Eingabe des Endzeitpunktes, wenn dieser
												   //größer als der berechnete ist -> Multibackup
		PS_BACKUP_INPUT_MULTI_IS_RUNNING	=  23, //zeige Text bei der Eingabe des Endzeitpunktes,
												   //wenn die Auslagerung fortgesetzt wird
												   
		PS_BACKUP_INPUT_WAIT_FOR_MEDIUM		=  24, //Backup mit ACDC: warten auf Medium, das in
												   //CD Laufwerk eingelegt werden muss

		PS_BACKUP_TOO_MANY_IMAGES			=   99, 
		PS_BACKUP_BACKUP_ACTIVATED			=  100,
		PS_BACKUP_ACDC_ERASE_MEDIUM			=  101,
		PS_BACKUP_ERASE_EXTERN_MEDIUM		=  102,
		PS_BACKUP_START						=  103, //do the backup...
		PS_BACKUP_DCD_START					=  104,
		PS_BACKUP_DCD_WAIT_EMPTY_CDR		=  105,
		PS_BACKUP_DCD_NO_EMPTY_CDR			=  106,
		PS_BACKUP_DCD_FOUND_NOT_EMPTY_CDR	=  107,
		PS_BACKUP_DCD_WAIT_FORMAT			=  108,
		PS_BACKUP_START_ERASE_EXTERN_MEDIUM =  109, 
		PS_BACKUP_ERASING_EXTERN_MEDIUM		=  110,
		PS_BACKUP_REQUEST					=  111,
		PS_BACKUP_REQUEST_MB				=  112,
		PS_BACKUP_RUNNING					=  113,
		PS_BACKUP_COPY_RO					=  114,
		PS_BACKUP_COPYING_RO				=  115,
		PS_BACKUP_DCD_FINALIZE				=  116,
		PS_BACKUP_DCD_ERROR					=  117,
		PS_BACKUP_ACDC_START				=  118,
		PS_BACKUP_ACDC_WAIT_COMPLETE		=  119,
		PS_BACKUP_ACDC_NOT_ERASED			=  120,
		PS_BACKUP_NOT_ERASED_EXTERN_MEDIUM  =  121,
		PS_BACKUP_ACDC_NO_EMPTY_CD			=  122,
		PS_BACKUP_ACDC_DVD_NOT_ALLOWED		=  123,
		PS_BACKUP_ACDC_CANCEL_BACKUP		=  124, //Benutzer Abbruch des Nero-Brennvorgangs
		PS_BACKUP_ACDC_ERROR				=  125,
		PS_BACKUP_ACDC_COMPLETE				=  126,
		PS_BACKUP_COMPLETE					=  127, //backup complete
		PS_NAVIGATE_FAILED					=  128,
		PS_READ_FAILED						=  129,

		PS_BACKUP_START_STATUS				=  199,
		PS_BACKUP_FAILED					=  200,
	};

	enum ErrorStatus
	{
		ES_NO_ERROR							= 0,
		ES_CANNOT_ACTIVATE_BACKUP_WINDOW	= 1,
		ES_EXTERN_BACKUP_DRIVE_REMOVED		= 2,
	};

	enum DCDErrorStatus
	{
		DCD_NO_ERROR						= 1,
		DCD_ERROR_START_WATCHCD				= 2,
		DCD_ERROR_WAIT_FOR_EMPTY_CDR		= 3,
		DCD_ERROR_FORMAT					= 4,
		DCD_ERROR_CANT_FIND_WATCHCD			= 5,
		DCD_ERROR_WAIT_FOR_FINALIZE_SUCCESS = 6,
	};

	enum BackupStatus
	{
		//Backup with DirectCD
		BS_DCD_NOT_FOUND_EMPTY_CDR	= 200,
		BS_DCD_FOUND_NOT_EMPTY_CDR	= 201,
		BS_DCD_FORMATTING			= 202,
		BS_DCD_START_BACKUP			= 203,
		BS_DCD_COPYING_DATA			= 204,
		BS_DCD_BACKUP_FAILED		= 205,
		BS_DCD_DATA_COPIED			= 206,
		BS_DCD_FINALIZE_SUCCESS		= 207,
		BS_DCD_UNEXPECTED_ERROR		= 208,
		BS_DCD_ERROR				= 299,
	};


// Construction
public:
	CBackupWindow(CMainFrame* pParent, CServer* pServer);
	virtual ~CBackupWindow();



// Attributes
public:

	//override abstract function from CDisplayWindow
	virtual CSecID		GetID()		const;
	virtual CString		GetName()	const;
	virtual CString		GetTitle(CDC* pDC);
	virtual void	    GetFooter(CByteArray& Array);
	virtual CString		GetDefaultText();
	virtual COLORREF	GetOSDColor();
	virtual BOOL		IsShowable();
	virtual BOOL		Request(int iMax=-1);
	virtual int			GetType() const { return DISPLAYWINDOW_BACKUP;} ;


	inline BOOL		IsInCalculate() const;
	inline BOOL		IsBackupFailed() const;
	inline BOOL		IsBackupRunning() const;
	BOOL			IsAlarm() const;

	void ConfirmBackupByTime(WORD wUserData,
							 DWORD dwID,
							 WORD wFlags,
							 CIPCInt64 i64Size,
							 const CSystemTime& stEndLessMaxCDCapacity,
							 const CSystemTime& stEndMoreMaxCDCapacity);

	BOOL ConfirmBackup(DWORD dwID);			//wird nach jeder erfolgreich kopierten Sequenz aufgerufen
	BOOL ConfirmCancelSession(DWORD dwID);	//wird nach cancel des Brennvorganges (acdc) aufgerufen
	BOOL ConfirmCancelBackup(DWORD dwID);	//wird nach cancel der Zwischenspeicherung (acdc) aufgerufen

	BOOL IndicateACDCError(DWORD dwCmd, //wird bei Backup Fehlern mit ACDC aufgerufen
						   CSecID id, 
						   CIPCError errorCode, 
						   int iUnitCode,
						   const CString &sErrorMsg);	

	inline WORD		GetBackupUserID();		 //liefert UserID beim Zwischenspeicherung mit DVStorage
	inline DWORD	GetBackupACDCSessionID();//liefert SessionID beim Backup mit ACDC (Brennen)
	
	inline BOOL		IsBackupACDC_PreparingToCopy();
	inline BOOL		IsBackupACDC_BurningToDisc();
	inline BOOL		IsBackupACDC_EraseExternMedium();
	

// Operations from Panel
public:
	BOOL StartBackup(const CSystemTime &stStart,
					 const CSystemTime &stEnd,
					 const CWordArray& waCameras,
					 CString sCamNrSeperated);
	BOOL StartAlarmBackup(const CSystemTime &stStart,
						  const CSystemTime &stEnd,
						  const CWordArray& waCameras);
	BOOL CalcBackupEnd(const CWordArray &waCameras, 
					   const CSystemTime &stStart);
	BOOL CalcMBUserBackupEnd(const CWordArray &waCameras,	//fragt MB`s in Datenbank nach
							 const CSystemTime &stStart,	//mit benutzerspezifischem Endzeitpunkt
							 const CSystemTime &stUserEnd); //da der berechnete Endzeitpunkt nicht gewählt wurde
	CIPCInt64 GetCDCapacity();

	//used on backup with ACDC and backup to extern drive
	BOOL  SetMediumCapacityInMB(DWORD dwCapacity); 
	
	//gets the capacity of the reader software in bytes
	DWORD	GetBytesReadOnlySoftware();

	BOOL Activate();
	inline CSystemTime GetCalculatedEnd();
	CString GetCalculatedEndFormatted();	//liefert formatierten Zeitpunkt
	BOOL RequestFirstImageInDB(CString sCameras, CSystemTime& stFirstDBImage);

	//Speichert alle nötigen Members um ein Multibackup (Auslagerung fortsetzen) ausführen zu können
	void SaveMultiBackupMembers();	
	
	//setzt notwendigen members aus Panel heraus bei MultiBackup, da das BackupWindow
	//immer wieder neu angelegt wird und die notwendigen Daten (Start/Endzeitpunkt) fehlen
	void InitMultiBackupMembers(CSystemTime stMultiBackupStartLast, 
							    CSystemTime stMultiBackupEndLast, 
								CSystemTime stMultiBackupStartNew);

	void SetMultiBackup(BOOL bMultiBackup); //bei Multibackup (Auslagerung fortsetzen) == TRUE

	void SetStopCalculate(BOOL bStopCalculate);		//setzt das flag zum Beenden der laufenden Berechnung des 
									//Endzeitpunktes der ersten CD-R 

	void SetCloseBackupWindow(BOOL bCloseBackupWindow, //setzt Flag zum sich selbst beenden
							  BOOL bExternDriveRemoved = FALSE); //== TRUE, wenn USB removed
	void SetPlayStatus(PlayStatus playstatus);

	BOOL OnCalculateEnd_Failed(CSystemTime stStart, CSystemTime stEnd);	//bei Endzeitberechnung gab es ein timeout (vom Panel aus)

	//letztes (neustes) Bild des Backups in datenbank wurde gefunden
	void SetLastRecordLocalized(WORD wLastSequence, DWORD dwLastRecord, BOOL bConfirmOK = TRUE);

	//Fortschritt der Auslagerung von ACDC gemeldet 
	//(CIPCDataCarrierClient::OnIndicateSessionProgress)
	void SessionProgress(DWORD dwSessionID, int iProgress);

	//Auslagerung mit ACDC beendet
	////(CIPCDataCarrierClient::OnConfirmSession)
	void ConfirmSession(DWORD dwSessionID);

	//wird vom Panel gesetzt, wenn eine nicht leere CD-RW nicht gelöscht werden soll
	//oder ein anderes Medium (Memorystick, HDD) nicht mehr genug Speicherplatz frei hat
	//Gesetzt wird die Wartezeit für den Anzeigetext.
	void SetTimeNotErasedMedium();

	//wird vom Panel gesetzt, wenn eine nicht leere CD-R erkannt wurde
	//Gesetzt wird die Wartezeit für den Anzeigetext.
	void SetTimeACDCNoEmptyCD();

	void SetACDCEraseProgress(BOOL bErase);
	void SetDeleteExternBackupDrive(BOOL bDelete);
	
	CString GetACDCTempPath();	//liefert den Temp Pfad beim Backup mit ACDC zurück 
	BOOL	DeleteACDCTempPath();	//löscht das temporäre Verzeichnis des ACDC backups

	static BOOL GetRemovableDrive(CString &sRemovableDrive, int &iDriveBit);
	DWORD GetBytesReadOnlySoftwareFromExternMedium();
	void  SetPrepareStopBackup(BOOL bPrepareStopBackup);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CBackupWindow)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg long OnBackupCheckStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Implementation
protected:

	void ChangePlayStatus(PlayStatus playstatus);
	CString NameOfEnum(PlayStatus ps);
	CString SystemTimeToString(CSystemTime st);
	void IncrementCalculatedEnd();
	BOOL IsNewestDBImageReached(); 
	void RecalcBackupCountdown();	//Berechnung zum Countdown des Backups (time to go)

	BOOL OnCalculateEnd_Start(BOOL bForward);	
	BOOL OnCalculateEnd_WaitFor();
	BOOL OnCalculateEnd_Success();

	BOOL OnBackupBackupActivated();

	BOOL OnBackupTooManyImages();
	BOOL OnBackupStart();
	BOOL OnBackupDCDStart();
	BOOL OnBackupDCDWaitEmptyCDR();
	BOOL OnBackupDCDNoEmptyCDR();
	BOOL OnBackupDCDFoundNotEmptyCDR();
	BOOL OnBackupDCDWaitFormat();
	BOOL OnBackupRequest();
	BOOL OnBackupStartEraseExternMedium();
	BOOL OnBackupErasingExternMedium();
	BOOL OnBackupRequestMB();
	BOOL OnBackupRunning();
	BOOL OnCopyRO();
	BOOL OnCopyingRO();
	BOOL OnBackupDCDFinalize();
	BOOL OnBackupDCDError();
	BOOL OnBackupEraseMedium();
	BOOL OnBackupACDCStart();
	BOOL OnBackupACDCWaitComplete();
	BOOL OnBackupACDCComplete();
	BOOL OnBackupACDCNotErased();
	BOOL OnBackupACDCNoEmptyCD();
	BOOL OnBackupACDCIndicateError();
	BOOL OnBackupACDCCancelBackup();

	BOOL OnBackupFailed();
	BOOL OnBackupComplete();
	BOOL OnStop();

	//liefert Backup Kameras als String für Anzeige im BackupWindow (GetDefaultText)
	CString GetCamsToBackupAsString(BOOL bMultiBackup = FALSE); 

	//fragt Datenbank nach letztem Bild des aktuellen Backups (SequneceNr und RecordNr)
	void RequestLastBackupImageInDB();

	CString GetACDCVolumeName();	//Volumename für Backup mit ACDC als DateTime, 11 Zeichen
									//Beispiel: 300902_1233

	BOOL PreventBackupDriveRemoval(BOOL bPrevent, const _TCHAR cDriveLetter);

	//error handling

//data members
private:

	PlayStatus	m_PlayStatus;
	PlayStatus	m_LastPlayStatus;
	PlayStatus  m_MultiBackupLastPlayStatus;	//sichert einen Status beim Multibackup über mehrere
												//Statiwechsel, da diese Variable nicht immer 
												//aktualisiert wird. Wird für Hilfetextanzeige benötigt

	PlayStatus  m_CalcMBUserBackupEnd;			//sichert einen Status beim Backup, wenn der User
												//nicht den berechneten Endzeitpunkt benutzt. Dann
												//erfolgt neue Berechnung der MB`s zum Backup. Werden
												//keine MB`s gefunden, steuert dieser gesicherte
												//Status die Hilfetextanzeige

	PlayStatus  m_LastPlayStatusBeforeBackupActivated; //sichert den Status, der vor dem Status
													   //PS_BACKUP_BACKUP_ACTIVATED aktiv war und
													   //besagt, ob die Endzeitberechnung eine volle
													   //CD Auslastung ergab oder nur eine prozentuelle
													   //Wird benötigt für Hilfetextanzeige, wenn 
													   //zwischen Start und benutzerspezifischem Ende
													   //keine MB`s gefunden wurde und das Ende erneut
													   //eingegeben werden muss
		
	ErrorStatus m_ErrorStatus;
	__int64		m_i64BytesToBackup;
	CWordArray	m_waBackupCameras;			//enthält CamIDs der Kameras die ge-backup-t werden sollen
	CUIntArray	m_waBackupCamerasNoImages;	//enthält CamIDs für Kameras, die im Backupzeitraum Bilder haben
//	CDWordArray	m_dwaSequencesToBackup;
	int			m_iMaxCameraNameLength;
	CIPCDrive*	m_pBackupDrive;
	BOOL		m_bDeleteExternBackupDrive;
	CEvent		m_evCalculateBackupEnd;
	DWORD		m_dwRunningTime;	//zum Berechnen der Anzeige der verstrichenen Zeit beim Formatieren
									//Brennen und Finalisieren der CD-R
	__int64		m_i64BytesBackuped; //schon auf CD geschriebene Bytes
	
	CSystemTime	m_stBackupStart;	//Startzeitpunkt des Backups
	CSystemTime m_stCalculatedEnd;	//berechneter Endzeitpunkt, MB`s passen auf eine einzige CD-R
	CSystemTime	m_stBackupEnd;		//Endzeitpunkt bis zu dem gebrannt wird	
	CSystemTime m_stBackupAlarmEnd;	//Endzeitpunkt des Alarm der gebrannt werden soll

	//Members für MultiBackup
	CSystemTime m_stMultiBackupStartLast;	//Startzeitpunkt des vorhergegangenen Backups
	CSystemTime m_stMultiBackupEndLast;		//Endzeitpunkt des vorhergegangenen Backups
	CSystemTime m_stMultiBackupStartNew;	//Startzeitpunkt des fortzusetzenden Backups
	CSystemTime m_stMultiBackupEndNew;		//Endzeitpunkt des fortzusetzenden Backups
	CString		m_sMultiBackupCamsSeperated;//Kameras des letzten Backups, getrennt durch ','
	BOOL		m_bMultiBackup;				//TRUE, wenn Auslagerung fortgesetzt werden soll
	BOOL		m_bAlarmBackup;				//TRUE, wenn ein Alarmbackup ausgeführt wird
	BOOL		m_bLastRecordLocalized;		//TRUE, wenn neustes Image aller Bilder des Backups
											//in datenbank gefunden und geliefert wurden



	WORD		m_wCamBackupEnd;				//Kamera zum Endzeitpunkt m_stBackupEnd 
	WORD		m_wLastSequenceBackupEnd;		//Sequence des Endzeitpunktes
	DWORD		m_dwLastRecordBackupEnd;		//letztes Record des Endzeitpunktes

	CSystemTime m_stLastCalculatedEnd;	//letzter berechnete Endzeitpunkt
	CSystemTime	m_stNewestDBImage;	//aktuelle Zeit beim Start der Berechnung des Endzeitpunktes
									//des Backups, ist die Zeit des aktuellen Bildes

	CString		m_sBackupCams;		//
	DWORD		m_dwBackupImages;
	CIPCInt64	m_i64CDCapacity;			//max. Kapazität der CD-R
	CString		m_sCDPercentUsed;			//zu wieviel % die CD-R ausgenutzt wird
	CString		m_sLastCDPercentUsed;		//jeweils zuletzt berechnete CD-R Ausnutzung, falls größer 0
	DWORD		m_dwWaitLastRecord;			//timeout für das Liefern des aktuellen Backupbildes
	DWORD		m_dwDCDWaitFinalize;		//timeout für das Finalisieren
	DWORD		m_dwStartTime;				//Startzeit des Backups, wird für Berechnung der restlich
											//verbleibenden Zeit
	DWORD		m_dwCountUp;				//verstrichene Zeit beim Backup
	DWORD		m_dwPercentBackuped;		//Prozentangabe der schon gebrannten Bilder
	DWORD		m_dwBytesPerSecond;			//angenommene Brenngeschwindigkeit 4 fach (4 * 150 * 1024)
	BOOL		m_bDirectCD;
	CDirectCD*	m_pDirectCD;
	DWORD		m_dwMsgTimeCompleteBackup;  //timedelay to show BackupComplete Msg
	DWORD		m_dwDCDWait;
	DWORD		m_dwStartFormat;
	DWORD		m_dwBackupFailed;
	DWORD		m_dwCurrentSeconds;		//Anzahl Sekunden zwischen Backup Start und End
	DWORD		m_dwLastCurrentSeconds; //Anzahl der Sekunden der vorherigen Suche
	CEvent		m_evRequestBackupByTime;
	BOOL		m_bOnlyForwards;		//wenn FALSE war bei OnCalculateEnd_Start
										//die Richtung der Datenbank-Suchen mind. einmal rückwärts
	DWORD		m_dwLastNotEnoughMB;	//das letzte kalkulierte BackupEnde (in Sekunden ab BackupStart)
										//bei dem noch nicht genug MB`s gefunden wurden
	BOOL		m_bStopDBSearch;		//wenn TRUE, wird Datenbank-Suche nach nächstem 
										//ConfirmBackupByTime() gestoppt
	BOOL		m_bStopCalculate;		//wenn TRUE, ist timeout für die Berechnung des Endzeitpunktes
										//der ersten CD-R (aus dem Panel heraus) überschritten
										//oder vom Panel aus sollte das Backupfenster während
										//einer Endzeitpunkt-Berechnung geschlossen werden
	BOOL		m_bCloseBackupWindow;	//wenn TRUE, schließt sich das backupfenster von selbst,
										//wenn der Status "PS_BACKUP_BACKUP_ACTIVATED erreicht wird

	BOOL		m_bUserEndBiggerCalcEnd;//ist TRUE, wenn vom Benutzer eingegebener Endzeitpunkt größer
										//als der berechnete ist

	CCopyReadOnlyVersion*	m_pCopyReadOnlyVersion; 
	
	BOOL		m_bPrepareStopBackup;	//if true will Benutzer das Backup abbrechen
	DWORD		m_dwStartFinalize;		//enthält Zeitspanne, die für das Finalisieren benötigt wurde
	BOOL		m_bCalculateError;		//true, wenn timeout bei CalculeteEnd erreicht wurde
	
	WORD		m_wBackupUserID;		//individuelle UserID eines Backup (Session mit DVStorage)
										//an GetTickCount() gekoppelt
	DWORD		m_dwACDCSessionID;		//Session ID für das Backup mit ACDC
	CString		m_sACDCTempPath;		//Temp Verueichnis in das die mit ACDC zu brennenden
										//Daten zunächst kopiert werden
	int			m_iACDCProgress;		//Progress Fortschritt beim Brennen mit ACDC
	int			m_iACDCEraseProgress;	//Progress Fortschritt beim Löschen einer CD-RW
	DWORD		m_dwMsgTimeNotErasedMedium; //timedelay to show Msg after not erasing medium
	DWORD		m_dwMsgTimeACDCNoEmptyCD; //timedelay to show msg on not empty cd-r`s
	DWORD		m_dwMsgTimeACDCIndicateError; //timedelay to show Msg after backup error
	DWORD		m_dwMsgTimeACDCCancelBackup;  //timedelay to show Msg after user canceled backup
	BOOL		m_bACDCEraseProgress;	//ist TRUE, wenn eine nicht leere CD-RW gelöscht wird
										//ist wichitg für die Progressanzeige (Prozent) einmal
										//beim CD-RW Löschen und anschließend beim Brennen	
	BOOL		m_bEraseComplete;		//ist TRUE, wenn das Löschen einer CD-RW beendet wurde
										//wird benötigt, um anschließend mit dem Progress 
										//des Backups weiter zu machen
	BOOL		m_bACDCProgressRuns;	//ist TRUE, wenn das Brennen auf CD-RW begonnen hat 
										//und einen Wert zwischen 50% und 99% erreicht hat.
										//wird benötigt, da teilweise zu Beginn des Brennens
										//von Nero der Wert 100% geliefert wird
	CBackupEraseMedium*	m_pBackupEraseMedium; //zum Löschen eines externen Mediums einen eigenen
											  //Thread benutzen
	BOOL		m_bTKRTraceBackup;
	UINT		m_uTimerProgressGeneric;
	UINT		m_uTimerProgressFormat;
	UINT		m_uTimerProgressFinalize;

	DISK_GEOMETRY m_pDiscGeometry;            // disk drive geometry structure
public:
	afx_msg void OnDestroy();
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CBackupWindow::IsBackupRunning() const
{
	return    (PS_BACKUP_START <= m_PlayStatus) 
		   && (m_PlayStatus <= PS_BACKUP_COMPLETE);
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL CBackupWindow::IsInCalculate() const
{
	return    (PS_BACKUP_CALCULATE_END__START <= m_PlayStatus) 
		   && (m_PlayStatus <= PS_BACKUP_CALCULATE_END__FAILED);
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CBackupWindow::IsBackupFailed() const
{
	return    (m_PlayStatus == PS_BACKUP_FAILED);
}
/////////////////////////////////////////////////////////////////////////////
inline CSystemTime CBackupWindow::GetCalculatedEnd()
{
	return m_stCalculatedEnd;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CBackupWindow::GetBackupUserID()
{
	return m_wBackupUserID;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CBackupWindow::GetBackupACDCSessionID()
{
	return m_dwACDCSessionID;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL CBackupWindow::IsBackupACDC_PreparingToCopy()
{
	//Zwischenspeicherung geschieht im Zustand: PS_BACKUP_RUNNING
	//Zwischenspeichern der RO-Software: PS_BACKUP_COPY_RO -> PS_BACKUP_COPYING_RO 
	
	return    (    m_PlayStatus == PS_BACKUP_RUNNING
				|| m_PlayStatus == PS_BACKUP_COPY_RO
				|| m_PlayStatus == PS_BACKUP_COPYING_RO);

}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CBackupWindow::IsBackupACDC_BurningToDisc()
{
	//mit ACDC und Nero gebrannt wird bei: PS_BACKUP_ACDC_START -> PS_BACKUP_ACDC_WAIT_COMPLETE

	return    (    m_PlayStatus == PS_BACKUP_ACDC_START
		        || m_PlayStatus == PS_BACKUP_ACDC_WAIT_COMPLETE);
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CBackupWindow::IsBackupACDC_EraseExternMedium()
{
	//mit ACDC und Nero werden exerne medien gelöscht bei: 
	//PS_BACKUP_START_ERASE_EXTERN_MEDIUM -> PS_BACKUP_ERASING_EXTERN_MEDIUM

	return    (    m_PlayStatus == PS_BACKUP_START_ERASE_EXTERN_MEDIUM
				|| m_PlayStatus == PS_BACKUP_ERASING_EXTERN_MEDIUM);
}
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BACKUPWINDOW_H__C49E7F63_5FC7_11D5_8DBF_0050BF554CAA__INCLUDED_)
