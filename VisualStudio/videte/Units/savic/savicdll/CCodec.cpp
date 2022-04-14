/* GlobalReplace: CPoints --> CMDPoints */
/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CCodec.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/CCodec.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 141 $
// LAST CHANGE:	$Modtime: 20.06.03 11:53 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

/*////////////////////////////////////////////////////////////////////////////
@TOPIC{CCodec Overview|Overview,CCodec}
@RELATED @LINK{members|CCodec},

Die Klasse CCodec ermöglicht den einfachen Zugriff auf die komplette SaVic-Hardware.
Dazu bietet sie einen umfangreichen Satz von einfach einzusetzende Memberfunktionen.

@MLIST @RELATED @LINK{Overview|Overview,CCodec}
*////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CCodec.h"
#include "DlgColor.h"
#include "CMDConfigDlg.h"
#include "CRelay.h"
#include "CAlarm.h"
#include "CMDPoints.h"
#include "resource.h"
#include "SavicProfile.h"
#include "CWatchDog.h"
#include "CPCI.h"
#include "CPCF8563.h"
#include "CPCF8582.h"
#include "CIoMemory.h"
#include "CIoPort.h"
#include "CBT878.h"
#include "CMotionDetection.h"
#include "CVideoInlay.h"
#include "WK_Timer.h"
#include "PictureDef.h"
#include "SystemTime.h"
#include "MemCopy.h"

CWK_Timer theTimer;
LARGE_INTEGER CWK_Timer::m_CyclesPerMicroSecond = {0};

#include "..\\SavicDA\\SavicDirectAccess.h"

int p_fetch;	// this global "anchor" variable helps to fool the compiler’s
				// optimizer into leaving the prefetchcode intact!

static const void inline BLOCK_PREFETCH_4K(void* addr)
{
	int* a = (int*) addr; // cast as INT pointer for speed

	p_fetch += a[0] + a[16] + a[32] + a[48] // Grab every
	+ a[64] + a[80] + a[96] + a[112]		// 64th address,
	+ a[128] + a[144] + a[160] + a[176]		// to hit each
	+ a[192] + a[208] + a[224] + a[240];	// cache line once.
	a += 256; // advance to next 1K block

	p_fetch += a[0] + a[16] + a[32] + a[48] // Grab every
	+ a[64] + a[80] + a[96] + a[112]		// 64th address,
	+ a[128] + a[144] + a[160] + a[176]		// to hit each
	+ a[192] + a[208] + a[224] + a[240];	// cache line once.
	a += 256; // advance to next 1K block

	p_fetch += a[0] + a[16] + a[32] + a[48] // Grab every
	+ a[64] + a[80] + a[96] + a[112]		// 64th address,
	+ a[128] + a[144] + a[160] + a[176]		// to hit each
	+ a[192] + a[208] + a[224] + a[240];	// cache line once.
	a += 256; // advance to next 1K block

	p_fetch += a[0] + a[16] + a[32] + a[48] // Grab every
	+ a[64] + a[80] + a[96] + a[112]		// 64th address,
	+ a[128] + a[144] + a[160] + a[176]		// to hit each
	+ a[192] + a[208] + a[224] + a[240];	// cache line once.
	a += 256; // advance to next 1K block

}

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Buffersize
#define SIZE 255 		

/////////////////////////////////////////////////////////////////////////////
UINT AlarmPollThread(LPVOID pData)
{
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pCodec)
	{
		ML_TRACE_ERROR(_T("AlarmPollThread\tpCodec = NULL\n"));
		return 0;
	}

	// Warte bis MainProzeß initialisiert ist.
	while (!pCodec->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun)
	{
		Sleep(100);
		pCodec->PollAlarm();
	}

	ML_TRACE(_T("Exit AlarmPollThread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT ExternEventThread(LPVOID pData)
{	 
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pCodec)
	{
		ML_TRACE_ERROR(_T("ExternEventThread\tpCodec = NULL\n"));
		return 0;
	}

	// Warte bis MainProzeß initialisiert ist.
	while (!pCodec->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun)
	{
		pCodec->WaitForExternEvent();
	}

	ML_TRACE(_T("Exit ExternEventThread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT DriverDebugEventTread(LPVOID pData)
{
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pCodec)
	{
		ML_TRACE_ERROR(_T("DriverDebugEventTread\tpCodec = NULL\n"));
		return 0;
	}

	// Warte bis MainProzeß initialisiert ist.
	while (!pCodec->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun)
	{
		pCodec->WaitForDriverDebugEvent();
	}

	ML_TRACE(_T("Exit DriverDebugEventTread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT EncoderDataThread(LPVOID pData)
{
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pCodec)
	{
		ML_TRACE_ERROR(_T("PollEncoderDataThread\tpCodec = NULL\n"));
		return 0;
	}

	// Warte bis MainProzeß initialisiert ist.
	while (!pCodec->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun)
	{
		Sleep(1);
		pCodec->ReadData();
	}

	ML_TRACE(_T("Exit PollEncoderDataThread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CCodec::CCodec(const CString &sAppIniName, CPCIDevice* pDevice)
{
	// Alle Pointer initialisieren
	m_pDevice			= pDevice;
	m_pRelay			= NULL;
	m_pAlarm			= NULL;
	m_pPCF8582			= NULL;
	m_pPCF8563			= NULL;
	m_pBT878			= NULL;
	m_pPerf1			= NULL;
	m_pPerf2			= NULL;
	m_pPerf3			= NULL;

	// Defaultinitialisierung der Member
	m_hDevice			= NULL;					// Handle auf Device
	m_wLastError		= ERR_NO_ERROR;			// Status des Objektes. TRUE=OK
	m_wSource			= VIDEO_SOURCE0;		// Aktuelle Videoquelle (VIDEO_SOURCE0...VIDEO_SOURCE7)
	m_wFormat			= ENCODER_RES_HIGH;		// Bildformat											
	m_dwBPF				= 20000;				// Bytes pro Frame
	m_dwLastLinAddr		= 0;					// Rückgabewert von DAMapPhysToLin();
	m_hFieldReadyEvent	= NULL;				  	// Event-Handle
	m_hSourceSwitchEvent= NULL;					// Event-Handle
	m_hWatchDogTriggerEvent = NULL;				// Event-Handle
	m_hPiezoEvent		= NULL;					// Event-Handle
	m_hShutDownEvent	= NULL;					// Shutdowneventhandle
	m_nEncoderState		= ENCODER_OFF;			// Encoder ist aus
	m_bUseTVCard		= FALSE;				// TRUE->Benutze TV-Karte als SaVic Ersatz
	m_bDiagnosticIsRunning = FALSE;				// Die Diagnosefunktion läuft gerade.

	m_bUseGlobalColorSettings = FALSE;			// Eine Einstellung für alle Kameras
	m_wGlobalBrightness	= (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 2;	// Globale Helligkeitseinstellung
	m_wGlobalContrast	= (MAX_CONTRAST - MIN_CONTRAST) / 2;		// Globale Kontrasteinstellung
	m_wGlobalSaturation	= (MAX_SATURATION - MIN_SATURATION) / 2;	// Globale Farbsättigungseinstellung
	m_wGlobalHue		= (MAX_HUE - MIN_HUE) / 2;					// Globale Hueeinstellung
	m_wFPS				= 25;

	m_pDlgColor			= NULL;					// Pointer auf Dialog
	m_pMDConfigDlg		= NULL;

	m_pAlarmPollThread	= NULL;					// PollAramThread
	m_pDriverDebugThread= NULL;					// DebugThread
	m_pExternEventThread= NULL;					// Warte auf Watchdog trigger
	m_pEncoderDataThread= NULL;					// Holt die Daten vom Encoder
	m_sAppIniName		= sAppIniName;			// INI-Filename
	m_pCWatchDog		= NULL;					// Watchdog-Objekt
	m_nTemperature		= 0;					// Aktuelle Temperatur.
	m_dwCurrCamMask		= 0;					// Maske der vorhandenen Kameras

	// Statistik
	m_nStatisticSwitchCounter		= 0;			 // Zählt die Kamerawechsel
	m_nStatisticHitCounter			= 0;			 // Zählt die korrekten Umschaltvorhersagen
	m_dwStatisticTotalBytes			= 0;			 // Zählt die Ankommende Datenmenge in Bytes
	m_dwStatisticFieldCnt			= 0;			 // Zählt die ankommenden Bilder.
	m_dwStatisticTC					= GetTickCount();// Zeiterfassung für Statistik
	m_dwLastPowerButtonPollTimeTC	= GetTickCount();// Letzter Zeitpunkt der Powertastenabfrage

	// Tracemeldungen (An=TRUE / Aus=FALSE)
	m_bTraceStartStopEncoder	= FALSE;
	m_bTraceStartStopDecoder	= FALSE;
	m_bTraceEncoderQueue		= FALSE;
	m_bTraceSetBPF				= FALSE;
	m_bTraceSetFormat			= FALSE;
	m_bTraceSetVideoFormat		= FALSE;
	m_bTraceSetBrightness		= FALSE;
	m_bTraceSetContrast			= FALSE;
	m_bTraceSetSaturation		= FALSE;
	m_bTraceSetHue				= FALSE;
	m_bTraceSetFilter			= FALSE;
	m_bTraceSetInputSource		= FALSE;
	m_bTraceSetRelais			= FALSE;
	m_bTraceSetAlarmEdge		= FALSE;
	m_bTraceAlarmIndication		= FALSE;
	m_bTraceDriverInfo			= TRUE;
	m_bTraceDriverWarning		= FALSE;
	m_bTraceDriverError			= TRUE;
	m_bTraceSwitchPrediction	= FALSE;
	m_bTraceFrameRate			= FALSE;
	m_bTraceDataRate			= FALSE;
	m_bTraceFiFoEmptyFlag		= FALSE;
	m_bTraceFiFoHalfFlag		= FALSE;
	m_bTraceFiFoFullFlag		= FALSE;
	m_bTraceAnalogOutSwitch		= FALSE;
	m_bTraceWaitForImageTimeout	= FALSE;
	m_bNewCameraScanMethod		= FALSE;
	m_bRun						= FALSE;

	// Buffer zur Aufnahme der Bilddaten, die an die Unit verschickt werden sollen.
	// Dieser Buffer wird in 'CompressImageToJpeg' bzw. 'CreateDIB' immer wieder recycled.
	// Die Größe ist auf 768x288+sizeof(sizeof(BITMAPINFO) ausgelegt...
	m_Image.pImageData	= new BYTE[FIELD_BUFFER_SIZE+1024];
	m_Image.dwImageLen	= FIELD_BUFFER_SIZE;

	m_bNewCameraScanMethod =  ReadConfigurationInt(_T("General"), _T("NewCameraScanMethod"), m_bNewCameraScanMethod, m_sAppIniName);

	ZeroMemory(&m_PredSrcSel, sizeof(SOURCE_SELECT_STRUCT));													  
	m_PredSrcSel.wSource		= VIDEO_SOURCE_NO;

	m_wAlarmState = 0;

	// Dieses Event veranlaßt das Beenden der einzelnen Threads
	m_hShutDownEvent = CreateEvent(0, TRUE, FALSE, NULL);
	if (!m_hShutDownEvent)
		ML_TRACE_ERROR(_T("Create ShutDownEvent failed\n"));

	// Dieses Event teilt der SaVicUnit mit, daß Bilddaten abgeholt werden können
	m_hFieldReadyEvent = CreateEvent(0, FALSE, FALSE, NULL);
	if (!m_hFieldReadyEvent)
		ML_TRACE_ERROR(_T("Create FieldReadyEvent failed\n"));

	// Dieses Event teilt der SaVicUnit mit, daß digital umgeschaltet wurde
	m_hSourceSwitchEvent = CreateEvent(0, FALSE, FALSE, NULL);
	if (!m_hSourceSwitchEvent)
		ML_TRACE_ERROR(_T("Create SourceSwitchEvent failed\n"));

	// Dieses Event teilt der SaViCUnit mit, daß eine Message vom Treiber kommt
	m_hDebugEvent = CreateEvent(0, FALSE, FALSE, NULL);
	if (!m_hDebugEvent)
		ML_TRACE_ERROR(_T("Create DebugEvent failed\n"));

	// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
	m_hWatchDogTriggerEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,EV_SAVIC_TRIGGER_WATCHDOG);
	if (m_hWatchDogTriggerEvent == NULL)
	{
		m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_SAVIC_TRIGGER_WATCHDOG);
	}
	else
	{
		ML_TRACE(_T("Watchdog Trigger Event already open\n"));
	}
	if (m_hShutDownEvent==NULL) 
	{
		ML_TRACE_ERROR(_T("Create WatchDogTriggerEvent failed\n"));
	}

	// Mit Hilfe diese Event läßt sich der Störmelder von Außen triggern
	m_hPiezoEvent = CreateEvent(NULL, FALSE, FALSE, EV_SAVIC_PIEZO);
	if (m_hPiezoEvent==NULL) 
	{
		ML_TRACE_ERROR(_T("Create PiezoOnOffEvent failed\n"));
	}

	if (m_pDevice)
	{
		ML_TRACE(_T("Compressionboard detected VendorID=0x%04x DeviceID=0x%04x SubSystemID=0x%04x SubSystemVendorID=0x%04x\n"),
					m_pDevice->GetVendorID(), m_pDevice->GetDeviceID(), m_pDevice->GetSubsystemID(), m_pDevice->GetSubsystemVendorID());
	
		m_pDevice->WriteByte(0x0d,127); // Set Master Latency Timer	to 127
	
	
		BYTE byVal = m_pDevice->GetDeviceControl();
		byVal = CLRBIT(byVal, 2);	// TEST: VIA/SIS PCI Controler compatibility mode.
		m_pDevice->SetDeviceControl(byVal);

	}
	else
	{
		ML_TRACE_ERROR(_T("No Compressionboard detected!\n"));
		return;
	}

	// BT878-Objekt anlegen
	m_pBT878 = new CBT878(this, GetBaseAddress(), sAppIniName);
   
	// Die Variationen ausgeben.
	DWORD dwVariationMask = GetBoardVariations();
	if (TSTBIT(dwVariationMask, VARIATION_ALARM_SWAP_BIT))
		ML_TRACE(_T("Variation: SaVic Bugfix: swap Alarminputs\n"));
	if (TSTBIT(dwVariationMask, VARIATION_VIDEO_SWAP_BIT))
		ML_TRACE(_T("Variation: SaVic: swap Videoinputs\n"));
	if (TSTBIT(dwVariationMask, VARIATION_ALARM4_PRESENT))
		ML_TRACE(_T("Variation: SaVic: 5 Alarminputs present\n"));
	
	for (WORD wSrc = VIDEO_SOURCE0; wSrc <= VIDEO_SOURCE0 + VIDEO_MAX_SOURCE; wSrc++)
	{
		m_Codec.Source[wSrc].wPortYC		= wSrc - VIDEO_SOURCE0;
		m_Codec.Source[wSrc].wBrightness	= (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 2;
		m_Codec.Source[wSrc].wContrast		= (MAX_CONTRAST   - MIN_CONTRAST)   / 2;
		m_Codec.Source[wSrc].wSaturation	= (MAX_SATURATION - MIN_SATURATION) / 2;
		m_Codec.Source[wSrc].wHue			= (MAX_HUE		  - MIN_HUE)        / 2;
		m_Codec.Source[wSrc].bDoubleField	= FALSE;

		m_nQuality[wSrc] = 75;
	}

	// Defaultmäßig sind Netz und ISDN und Backup sperrren
	WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("NET"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 0, m_sAppIniName);
	WriteConfigurationString(_T("EEProm"), _T("SN"), _T(""), m_sAppIniName);

	// Objekt zum Zugriff auf das EEProm
	m_pPCF8582 = new CPCF8582(GetBaseAddress());

	// TV-karten besitzen keine Relays, Alarmeingänge, WatchdogEEProm
	m_bUseTVCard = ReadConfigurationInt(_T("General"), _T("UseTVCard"), m_bUseTVCard, m_sAppIniName);
	if (!m_bUseTVCard)
	{
		m_pRelay = new CRelay(GetBaseAddress());

		// Alarmobjekt anlegen
		m_pAlarm = new CAlarm(GetBaseAddress(), GetBoardVariations());

		// Objekt zum Zugriff auf den Watchdog
		m_pCWatchDog= new CWatchDog(GetBaseAddress());

		// Objekt zum Zugriff auf die Uhr
		m_pPCF8563 = new CPCF8563(GetBaseAddress());

		WORD  wDongle	= 0;
		DWORD dwHeader	= 0;
		CString sSN		= _T("");

		if (m_pPCF8582 && m_pPCF8582->ReadMagicHeader(dwHeader))
		{
			if (dwHeader == MAGIC_HEADER)
			{
				// Wenn Dongleinformationen in Registryeintrag _T("Raw") stehen, diese in
				// das EEProm übertragen und den Eintrag löschen.
				int nRaw = ReadConfigurationInt(_T("EEProm"), _T("RawBits"), -1, m_sAppIniName);
				DeleteEntry(_T("EEProm"), _T("RawBits"), m_sAppIniName);
				if (nRaw != -1)
				{
					// Wenn nRawBits=0 ist, sollen die Dongleinformationen im EProm gelöscht werden.
					if (nRaw == 0)
						m_pPCF8582->WriteDongleInformation(0);
					else
					{
						// Ansonsten die Dongleinformationen verodern
						if (m_pPCF8582->ReadDongleInformation(wDongle))
							m_pPCF8582->WriteDongleInformation(wDongle | (WORD)nRaw);
					}
				}

				// Dongleinformationen aus EEProm lesen und in die Registry übertragen.
				if (m_pPCF8582->ReadDongleInformation(wDongle))
				{
					if (TSTBIT(wDongle, EE_DONGLE_BIT_ISDN))
					{
						ML_TRACE(_T("SaViC Dongleinformation: ISDN allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 1, m_sAppIniName);
					}
					else
					{
						ML_TRACE(_T("SaViC Dongleinformation: ISDN not allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 0, m_sAppIniName);
					}
					if (TSTBIT(wDongle, EE_DONGLE_BIT_TCP))
					{
						ML_TRACE(_T("SaViC Dongleinformation: TCP/IP allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("NET"), 1, m_sAppIniName);
					}
					else
					{
						ML_TRACE(_T("SaViC Dongleinformation: TCP/IP not allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("NET"), 0, m_sAppIniName);
					}
					if (TSTBIT(wDongle, EE_DONGLE_BIT_BACKUP))
					{
						ML_TRACE(_T("SaViC Dongleinformation: Backup allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 1, m_sAppIniName);
					}
					else
					{
						ML_TRACE(_T("SaViC Dongleinformation: Backup not allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 0, m_sAppIniName);
					}
					if (TSTBIT(wDongle, EE_DONGLE_BIT_BACKUP_DVD))
					{
						ML_TRACE(_T("SaViC Dongleinformation: Backup to DVD allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 1, m_sAppIniName);
					}
					else
					{
						ML_TRACE(_T("SaViC Dongleinformation: Backup to DVD not allowed\n"));
						WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 0, m_sAppIniName);
					}
				}
				// Seriennummer in die Registry übertragen.
				if (m_pPCF8582->ReadSN(sSN))
				{
					ML_TRACE(_T("SaViC SN=<%s>\n"), sSN);
					WriteConfigurationString(_T("EEProm"), _T("SN"), sSN, m_sAppIniName);
				}
			}
/*			else // Kein Magic Header vorhanden.
			{
				if (m_pPCF8582)
				{
					m_pPCF8582->ClearEEProm();
					m_pPCF8582->WriteSN(_T("1234567890123456"));
					m_pPCF8582->WriteSubsystemVendorID(SAVIC_SUBSYSTEM_ID1);
				}
			}
*/		}
		if (m_pPCF8582)
			m_pPCF8582->DumpEEProm();
	}
	else // TV-Karten besitzen keinen Syncdetektor, daher wird der Kamerascan über den BT878 realisiert
		m_bNewCameraScanMethod = FALSE;

	if (m_bNewCameraScanMethod)
		ML_TRACE(_T("Using new camera scan methode (Syncdetector)\n"));
	else
		ML_TRACE(_T("Using traditional camera scan methode (BT878)\n"));
}

/////////////////////////////////////////////////////////////////////////////
CCodec::~CCodec()
{
	ML_TRACE(_T("CCodec::~CCodec()\n"));

	WK_DELETE(m_pPCF8582);

	WK_DELETE(m_pPCF8563);

	WK_DELETE(m_pCWatchDog);

	WK_DELETE(m_pRelay);
	WK_DELETE(m_pAlarm);
	
	WK_DELETE(m_pBT878);

	if (m_hFieldReadyEvent)
	{
		CloseHandle(m_hFieldReadyEvent);
		m_hFieldReadyEvent = NULL;
	}

	if (m_hSourceSwitchEvent)
	{
		CloseHandle(m_hSourceSwitchEvent);
		m_hSourceSwitchEvent = NULL;
	}

	if (m_hDebugEvent)
	{
		CloseHandle(m_hDebugEvent);
		m_hDebugEvent = NULL;
	}

	if (m_hWatchDogTriggerEvent)
	{
		CloseHandle(m_hWatchDogTriggerEvent);
		m_hWatchDogTriggerEvent = NULL;
	}

	if (m_hPiezoEvent)
	{
		CloseHandle(m_hPiezoEvent);
		m_hPiezoEvent = NULL;
	}

	if (m_hShutDownEvent)
	{
		CloseHandle(m_hShutDownEvent);
		m_hShutDownEvent = NULL;
	}

	// Gerätetreiber schließen
	CloseSaVicDevice();

	WK_DELETE(m_Image.pImageData);
}

//////////////////////////////////////////////////////////////////////////////
BOOL CCodec::IsValid() const
{
	// Ist der Gerätetreiber geladen?
	if (!DAIsValid())
	{
		CString sError;
		sError.LoadString(IDS_RTE_DRIVER_NOT_LOADED);
		OnReceivedRunTimeError(REL_CANNOT_RUN, RTE_CONFIGURATION, sError);
		return FALSE;
	}

	// Stimmen die Versionen überein?
	DWORD dwDllVersion = DAGetSaVicDADllVersion();
	if (dwDllVersion != SAVIC_UNIT_VERSION)
	{
		CString sError, sMsg;
		sMsg.LoadString(IDS_RTE_DLL_WRONG_VERSION);
		sError.Format(sMsg, dwDllVersion, SAVIC_UNIT_VERSION);
		OnReceivedRunTimeError(REL_CANNOT_RUN, RTE_CONFIGURATION, sError);
		return FALSE;
	}

	// Eine TV-Karte besitzt keine Alarme oder Relais.
	if (m_bUseTVCard)
	{
		if (!GetBT878Object()  || !GetBT878Object()->IsValid() || !m_pDevice)
			return FALSE;
	}
	else
	{

		if( (!GetRelayObject()  || !GetRelayObject()->IsValid()) ||
			(!GetAlarmObject()  || !GetAlarmObject()->IsValid()) ||
			(!GetBT878Object()  || !GetBT878Object()->IsValid()) ||
			(!m_pPCF8563 || !m_pPCF8582 || !m_pCWatchDog) ||
			(!m_pDevice))
					return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::ReadDebugConfiguration()
{
	// Informationen der zu protokollierenden Funktionen einlesen
	m_bTraceStartStopEncoder	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("StartStopEncoder"),	m_bTraceStartStopEncoder,	m_sAppIniName);
	m_bTraceStartStopDecoder	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("StartStopDecoder"),	m_bTraceStartStopDecoder,	m_sAppIniName);
	m_bTraceSetBPF				= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetBPF"),  			m_bTraceSetBPF,				m_sAppIniName);
	m_bTraceSetFormat			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetFormat"),			m_bTraceSetFormat,			m_sAppIniName);
	m_bTraceSetVideoFormat		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetVideoFormat"), 		m_bTraceSetVideoFormat,		m_sAppIniName);
	m_bTraceSetBrightness		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetBrightness"), 		m_bTraceSetBrightness,		m_sAppIniName);
	m_bTraceSetContrast			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetContrast"), 		m_bTraceSetContrast,		m_sAppIniName);
	m_bTraceSetSaturation		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetSaturation"), 		m_bTraceSetSaturation,		m_sAppIniName);
	m_bTraceSetHue				= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetHue"), 				m_bTraceSetHue,				m_sAppIniName);
	m_bTraceSetFilter			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetFilter"), 			m_bTraceSetFilter,			m_sAppIniName);
	m_bTraceSetInputSource		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetInputSource"), 		m_bTraceSetInputSource,		m_sAppIniName);
	m_bTraceSetRelais			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetRelais"), 			m_bTraceSetRelais,			m_sAppIniName);
	m_bTraceSetAlarmEdge		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetAlarmEdge"),		m_bTraceSetAlarmEdge,		m_sAppIniName);
	m_bTraceAlarmIndication		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("AlarmIndication"),		m_bTraceAlarmIndication,	m_sAppIniName);
	m_bTraceEncoderQueue		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("EncoderQueue"),		m_bTraceEncoderQueue,		m_sAppIniName);
	m_bTraceDriverInfo			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("DriverInfo"),			m_bTraceDriverInfo,			m_sAppIniName);
	m_bTraceDriverWarning		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("DriverWarning"),		m_bTraceDriverWarning,		m_sAppIniName);
	m_bTraceDriverError			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("DriverError"),			m_bTraceDriverError,		m_sAppIniName);
	m_bTraceSwitchPrediction	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SwitchPrediction"),	m_bTraceSwitchPrediction,	m_sAppIniName);
	m_bTraceFrameRate			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("FrameRate"),			m_bTraceFrameRate,			m_sAppIniName);
	m_bTraceDataRate			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("DataRate"),			m_bTraceDataRate,			m_sAppIniName);
	m_bTraceFiFoEmptyFlag		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("FiFoEmpty"),			m_bTraceFiFoEmptyFlag,		m_sAppIniName);
	m_bTraceFiFoHalfFlag		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("FiFoHalfFull"),		m_bTraceFiFoHalfFlag,		m_sAppIniName);
	m_bTraceFiFoFullFlag		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("FiFoFull"),			m_bTraceFiFoFullFlag,		m_sAppIniName);
	m_bTraceAnalogOutSwitch		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("AnalogOutSwitch"),		m_bTraceAnalogOutSwitch,	m_sAppIniName);
	m_bTraceWaitForImageTimeout	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("WaitForImageTimeout"),	m_bTraceWaitForImageTimeout,m_sAppIniName);
	
	// Und im Logfile protokollieren
	ML_TRACE(_T("TraceStart/StopEncoder=%s\n"),		(m_bTraceStartStopEncoder	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceStart/StopDecoder=%s\n"),		(m_bTraceStartStopDecoder	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetBPF=%s\n"),				(m_bTraceSetBPF				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetFormat=%s\n"),				(m_bTraceSetFormat			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetVideoFormat=%s\n"),		(m_bTraceSetVideoFormat		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetBrighness=%s\n"),			(m_bTraceSetBrightness		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetContrast=%s\n"),			(m_bTraceSetContrast		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetSaturation=%s\n"),			(m_bTraceSetSaturation		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetHue=%s\n"),				(m_bTraceSetHue				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetFilter=%s\n"),				(m_bTraceSetFilter			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetInputSource=%s\n"),		(m_bTraceSetInputSource		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetRelais=%s\n"),				(m_bTraceSetRelais			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetAlarmEdge=%s\n"),			(m_bTraceSetAlarmEdge		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceAlarmIndication=%s\n"),		(m_bTraceAlarmIndication	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceEncoderQueue=%s\n"),			(m_bTraceEncoderQueue		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceDriverInfo=%s\n"),			(m_bTraceDriverInfo			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceDriverWarning=%s\n"),			(m_bTraceDriverWarning		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceDriverError=%s\n"),			(m_bTraceDriverError		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSwitchPrediction=%s\n"),		(m_bTraceSwitchPrediction	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceFrameRate=%s\n"),				(m_bTraceFrameRate			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceDataRate=%s\n"),				(m_bTraceDataRate			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceFifoEmptyFlag=%s\n"),			(m_bTraceFiFoEmptyFlag		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceFifoHalfFullFlag=%s\n"),		(m_bTraceFiFoHalfFlag		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceFifoFullFlag=%s\n"),			(m_bTraceFiFoFullFlag		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceAnalogOutSwitch=%s\n"),		(m_bTraceAnalogOutSwitch	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceWaitForImageTimeout=%s\n"),	(m_bTraceWaitForImageTimeout? _T("On") : _T("Off")));
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::Open()
{
	// IOBasisadressen und IRQ protokollieren					 
	ML_TRACE(_T("IRQ=%u\n"),		GetIRQNumber());
	ML_TRACE(_T("IOBase=0x%lx\n"),	GetBaseAddress());

	// Konfigurationsstruktur anlegen...
	SETTINGS Settings;

	// ...und füllen
	Settings.dwIOBase			= GetBaseAddress();
	Settings.wIRQ				= GetIRQNumber();
	Settings.hFieldReadyEvent	= (DWORD)DACreateRing0Event(m_hFieldReadyEvent);
	Settings.hSourceSwitchEvent	= (DWORD)DACreateRing0Event(m_hSourceSwitchEvent);
	Settings.hDebugEvent		= (DWORD)DACreateRing0Event(m_hDebugEvent);
	Settings.wCameraType		= ReadConfigurationInt(_T("General"), _T("CameraType"), 0, m_sAppIniName);
	Settings.wField				= ReadConfigurationInt(_T("General"), _T("StoreField"), 0, m_sAppIniName);
	Settings.wSkipFields	 	= ReadConfigurationInt(_T("General"), _T("SkipFields"), 1, m_sAppIniName);
	Settings.wCaptureSizeH		= 720; // Größe eines Halbbildes im Inlay
	Settings.wCaptureSizeV		= 288; // Dies ist auch das Format des erhaltenen Bildes
	Settings.wCaptureSizeH		= ReadConfigurationInt(_T("FrontEnd"), _T("CaptureSizeH"), 720, m_sAppIniName);

	// Gerätetreiber laden
	if (!OpenSaVicDevice(Settings))
		return FALSE;
	
	if (Settings.wCameraType == CAMTYPE_ASYNC)
		ML_TRACE(_T("CameraType = Async\n"));
	else if (Settings.wCameraType == CAMTYPE_SYNC)
		ML_TRACE(_T("CameraType = Sync\n"));
	else
 		ML_TRACE(_T("No Cameratyp specified\n"));

	if (Settings.wField == EVEN_FIELD)
		ML_TRACE(_T("Store even fields\n"));
	else if (Settings.wField == ODD_FIELD)
		ML_TRACE(_T("Store odd fields\n"));
	else
		ML_TRACE(_T("Store both fields\n"));
			  
	// Initialisiere den BT878
	if (!GetBT878Object()->Init(m_sAppIniName, _T("FrontEnd\\BT878Registers")))
	{
		ML_TRACE_ERROR(_T("CCodec::Init\tCBT878-Objekt konnte nicht initialisiert werden\n"));
		return FALSE;
	}

	// Einstellungen aus der Registry lesen.
	LoadConfig();

	// Den Scaler entsprechend der gewünschten Bildgröße setzen.
	HandleScaler(Settings.wCaptureSizeH, 2*Settings.wCaptureSizeV); 

	// Schaltet auf den ersten Eingang an dem ein Videosignal detektiert wurde.
	SOURCE_SELECT_STRUCT SrcSel = {0};
	SrcSel.wSource				= VIDEO_SOURCE0;
	SrcSel.wFormat				= m_wFormat;
	SrcSel.dwBPF				= m_dwBPF;
	SrcSel.dwProzessID			= 0;
	SrcSel.dwPics				= (DWORD)-1;
	SrcSel.wCompressionType		= COMPRESSION_JPEG;

	DWORD dwCamMask = ScanForCameras(0xffff);
	
	WORD  wFirstSource = VIDEO_SOURCE_NO;
	for (WORD wSource = 0; wSource < MAX_CAMERAS; wSource++)
	{
		if (TSTBIT(dwCamMask, wSource))
		{
			wFirstSource = wSource;
			break;
		}
	}	  
	if (wFirstSource == VIDEO_SOURCE_NO)
	{
		ML_TRACE_WARNING(_T("No camerasignal detected! Using first videoport\n"));
		wFirstSource = VIDEO_SOURCE0;		
	}
	else
	{
		ML_TRACE(_T("First camerasignal detected on videoport %d\n"), wFirstSource+1);

		SrcSel.wSource = wFirstSource;
		// Kamera _T('wFirstSource') ans 1. Frontend legen
		SetInputSource(SrcSel);	
	}

	// Dialog zur Farbeinstellung anlegen.
	m_pDlgColor = new CDlgColor(this);

	// Alarmpollthread anlegen.
	m_pAlarmPollThread = AfxBeginThread(AlarmPollThread, this);
	if (m_pAlarmPollThread)
	{
		m_pAlarmPollThread->m_bAutoDelete = FALSE;
		m_pAlarmPollThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	}

	// Darf diese SaVicUnit auf externe Events lauschen?
	if (ReadConfigurationInt(_T("General"), _T("ListenToExtEvent"), 0, m_sAppIniName) == 1)
	{
		// ExternEventThread anlegen.
		m_pExternEventThread = AfxBeginThread(ExternEventThread, this);
		if (m_pExternEventThread)
		{
			m_pExternEventThread->m_bAutoDelete = FALSE;
			m_pExternEventThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		}
	}

	// Debugthread anlegen.
	m_pDriverDebugThread = AfxBeginThread(DriverDebugEventTread, this);
	if (m_pDriverDebugThread)
	{
		m_pDriverDebugThread->m_bAutoDelete = FALSE;
		m_pDriverDebugThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	}

	// EncoderData Thread anlegen.
	m_pEncoderDataThread = AfxBeginThread(EncoderDataThread, this);
	if (m_pEncoderDataThread)
	{
		m_pEncoderDataThread->m_bAutoDelete = FALSE;
		m_pEncoderDataThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	}

	// LED1 einschalten
	SetLEDState(eLED1_On);

	// LED2 ausschalten
	SetLEDState(eLED2_Off);

	// Zum Test die AGC aus!
	if (m_pBT878)
	{
		if (ReadConfigurationInt(_T("FrontEnd"), _T("AGC"), 1, m_sAppIniName) == 1)
			m_pBT878->EnableAGC(TRUE);
		else
			m_pBT878->EnableAGC(FALSE);
	}

	m_bRun= TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::Close()
{
	// Nur Wenn CCodec::Open erfolgreich war.
	if (!m_bRun)
		return FALSE;

	ML_TRACE(_T("CCodec::Close\n"));
	SaveConfig();

	m_bRun = FALSE;
	
	DisableWatchdog();
	EncoderStop();

	SetEvent(m_hShutDownEvent);

    // Warte bis _T('AlarmPollThread') beendet ist.
	if (m_pAlarmPollThread)
		WaitForSingleObject(m_pAlarmPollThread->m_hThread, 2000);
	WK_DELETE(m_pAlarmPollThread); //Autodelete = FALSE;

    // Warte bis 'EcoderDataThread' beendet ist.
	if (m_pEncoderDataThread)
		WaitForSingleObject(m_pEncoderDataThread->m_hThread, 2000);
	WK_DELETE(m_pEncoderDataThread); //Autodelete = FALSE;

	// Warte bis 'ExternEventThread' beendet ist
	if (m_pExternEventThread)
		WaitForSingleObject(m_pExternEventThread->m_hThread, 2000);
	WK_DELETE(m_pExternEventThread); //Autodelete = FALSE;
	
	WK_DELETE(m_pDlgColor);

   // Warte bis 'DebugThread' beendet ist.
	if (m_pDriverDebugThread)
		WaitForSingleObject(m_pDriverDebugThread->m_hThread, 2000);
	WK_DELETE(m_pDriverDebugThread); //Autodelete = FALSE;

	// Warte bis der letzte Interrupt Bearbeitet wurde.
	Sleep(100);

 	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OpenSaVicDevice(const SETTINGS &Settings)
{
	m_csCodec.Lock();
	DWORD dwDriverVersion	= 0;

	// Versionsnummer des Gerätetreibers holen
	dwDriverVersion = DAGetDriverVersion();

	ML_TRACE(_T("DeviceDriver Version=0x%08x\n"), dwDriverVersion);  
	
	if (dwDriverVersion != SAVIC_UNIT_VERSION)
	{
		CString sError, sMsg;
		sMsg.LoadString(IDS_RTE_DRIVER_WRONG_VERSION);
		sError.Format(sMsg, dwDriverVersion, SAVIC_UNIT_VERSION);
		OnReceivedRunTimeError(REL_CANNOT_RUN, RTE_CONFIGURATION, sError);

		m_csCodec.Unlock();
		return FALSE;
	}

	// Gerätetreiber öffnen
	m_hDevice = DAOpenDriver(Settings);

	if (!m_hDevice)
	{
		CString sError;
		sError  = GetLastErrorString();

		OnReceivedRunTimeError(REL_CANNOT_RUN, RTE_CONFIGURATION, sError);

		m_csCodec.Unlock();
		return FALSE;
	}

	m_csCodec.Unlock();

	return TRUE;
}
	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CloseSaVicDevice()
{
	ML_TRACE(_T("CCodec::CloseSaVicDevice\n"));

	// Gerätetreiber schließen
	if (m_hDevice && !DACloseDriver(m_hDevice))
	{
		DWORD dwLastError = GetLastError();
        ML_TRACE_ERROR(_T("CCodec::Close\tDevice does not support the requested API\t(Fehlercode=%lu)\n"), dwLastError);
  		return FALSE;
	}
	
	m_hDevice = NULL;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCodec::MapPhysToLin(DWORD dwPhysAddr, DWORD dwLen)
{
	// Nur der erste Aufruf von 'MapPhysToLin' wird an den
	// Gerätetreiber weitergeleitet, da unter WinNT jedes Mapping
	// rückgängig gemacht werden muß. Dies erfolgt derzeit beim
	// schließen des Treibers innerhalb von 'DAClose', also genau
	// einmal!
	if (!m_dwLastLinAddr)
		m_dwLastLinAddr	= DAMapPhysToLin(dwPhysAddr, dwLen);
	
	return m_dwLastLinAddr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SaveConfig() const
{
	_TCHAR szBuffer[SIZE];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	
	_TCHAR szSourceType[SIZE];
	ZeroMemory(szSourceType, sizeof(szSourceType));

	_TCHAR szVideoFormat[SIZE];
	ZeroMemory(szVideoFormat, sizeof(szVideoFormat));

	_TCHAR szSource[SIZE];
	ZeroMemory(szSource, sizeof(szSource));

	if (EncoderGetFormat() == ENCODER_RES_HIGH)
		WriteConfigurationString(_T("General"), _T("Resolution"), _T("High"), m_sAppIniName); 
	if (EncoderGetFormat() == ENCODER_RES_LOW)
		WriteConfigurationString(_T("General"), _T("Resolution"), _T("Low"), m_sAppIniName); 
	
	// *************************************************
	// Video Front End
	// *************************************************

	// Aktuelle FrontEnd Bildeinstellungen sichern
	WriteConfigurationInt(_T("FrontEnd"), _T("GlobalBrightness"), m_wGlobalBrightness, m_sAppIniName);
	WriteConfigurationInt(_T("FrontEnd"), _T("GlobalContrast"),	 m_wGlobalContrast, m_sAppIniName);
	WriteConfigurationInt(_T("FrontEnd"), _T("GlobalSaturation"), m_wGlobalSaturation, m_sAppIniName);
	WriteConfigurationInt(_T("FrontEnd"), _T("GlobalHue"),		 m_wGlobalHue, m_sAppIniName);

	for (WORD wSrc = VIDEO_SOURCE0; wSrc <= VIDEO_SOURCE0 + VIDEO_MAX_SOURCE; wSrc++)
	{
		wsprintf(szBuffer, _T("Mapping\\Source%02u"), wSrc);

		// Jede Kamera hat ihre eigenen Einstellungen
		WriteConfigurationInt(szBuffer, _T("Brightness"), m_Codec.Source[wSrc].wBrightness, m_sAppIniName);
		WriteConfigurationInt(szBuffer, _T("Contrast"),   m_Codec.Source[wSrc].wContrast, m_sAppIniName);
		WriteConfigurationInt(szBuffer, _T("Saturation"), m_Codec.Source[wSrc].wSaturation, m_sAppIniName);
		WriteConfigurationInt(szBuffer, _T("Hue"),		  m_Codec.Source[wSrc].wHue, m_sAppIniName);
		WriteConfigurationInt(szBuffer, _T("DoubleField"),m_Codec.Source[wSrc].bDoubleField, m_sAppIniName);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::LoadConfig()
{
	_TCHAR szBuffer[SIZE];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	_TCHAR szSourceType[SIZE];
	ZeroMemory(szSourceType, sizeof(szSourceType));

	_TCHAR szVideoFormat[SIZE];
	ZeroMemory(szVideoFormat, sizeof(szVideoFormat));

	_TCHAR szFilter[SIZE];
	ZeroMemory(szFilter, sizeof(szFilter));

	// Angaben über zu protokollierene Funktionen aus der Registry
	ReadDebugConfiguration();

	// Auflösung
	ReadConfigurationString(_T("General"), _T("Resolution"), _T("High"), szBuffer,  SIZE, m_sAppIniName); 
	if (_tcsicmp(szBuffer,_T("High")) == 0)
		EncoderSetFormat(ENCODER_RES_HIGH);
	else if (_tcsicmp(szBuffer,_T("Low")) == 0)
		EncoderSetFormat(ENCODER_RES_LOW);

	// Die Videonorm wird automatisch anhand der Videonorm
	// der ersten detektierten Kamera ermittelt. Dies erfolgt zu einem späteren Zeitpunkt
	// in 'Codec::Open():
	SetVideoFormat(VFMT_PAL_CCIR);

	m_bUseGlobalColorSettings = ReadConfigurationInt(_T("FrontEnd"), _T("UseGlobalColorSetting"),	m_bUseGlobalColorSettings, m_sAppIniName);
	ML_TRACE(_T("UseGlobalColorSettings=%s\n"), (m_bUseGlobalColorSettings	? _T("On") : _T("Off")));

	// Globale Einstellungen gelten für alle Kameras
	m_wGlobalBrightness = ReadConfigurationInt(_T("FrontEnd"), _T("GlobalBrightness"),	m_wGlobalBrightness, m_sAppIniName);
	m_wGlobalContrast   = ReadConfigurationInt(_T("FrontEnd"), _T("GlobalContrast"),	m_wGlobalContrast, m_sAppIniName);
	m_wGlobalSaturation = ReadConfigurationInt(_T("FrontEnd"), _T("GlobalSaturation"),	m_wGlobalSaturation, m_sAppIniName);
	m_wGlobalHue	    = ReadConfigurationInt(_T("FrontEnd"), _T("GlobalHue"),			m_wGlobalHue, m_sAppIniName);
	m_wFPS				= ReadConfigurationInt(_T("FrontEnd"), _T("Fps"),				m_wFPS, m_sAppIniName);
	m_wFPS				= max(min(m_wFPS, 25), 0);
	
	// Und protokollieren
	ML_TRACE(_T("GlobalBrightness=%u\n"),	m_wGlobalBrightness);
	ML_TRACE(_T("GlobalContrast=%u\n"),		m_wGlobalContrast);
	ML_TRACE(_T("GlobalSaturation=%u\n"),	m_wGlobalSaturation);
	ML_TRACE(_T("GlobalHue=%u\n"),			m_wGlobalHue);
	ML_TRACE(_T("Framerate=%u\n"),			m_wFPS);

	// Den physikalischen Portanschluß aller Kameras einlesen
	for (WORD wSrc = VIDEO_SOURCE0; wSrc <= VIDEO_SOURCE0 + VIDEO_MAX_SOURCE; wSrc++)
	{
		wsprintf(szBuffer, _T("Mapping\\Source%02u"), wSrc);
		
		m_Codec.Source[wSrc].wPortYC = ReadConfigurationInt(szBuffer, _T("PortYC"), m_Codec.Source[wSrc].wPortYC, m_sAppIniName);
		
		// Sollen die Videoeingänge getauscht werden?
		if (TSTBIT(GetBoardVariations(), VARIATION_VIDEO_SWAP_BIT))
			m_Codec.Source[wSrc].wPortYC		= VIDEO_SOURCE3 - m_Codec.Source[wSrc].wPortYC;
		
		// Jede Kamera hat ihre eigenen Einstellungen
		m_Codec.Source[wSrc].wBrightness = ReadConfigurationInt(szBuffer, _T("Brightness"), m_Codec.Source[wSrc].wBrightness, m_sAppIniName);
		m_Codec.Source[wSrc].wContrast   = ReadConfigurationInt(szBuffer, _T("Contrast"),   m_Codec.Source[wSrc].wContrast, m_sAppIniName);
		m_Codec.Source[wSrc].wSaturation = ReadConfigurationInt(szBuffer, _T("Saturation"), m_Codec.Source[wSrc].wSaturation, m_sAppIniName);
		m_Codec.Source[wSrc].wHue		 = ReadConfigurationInt(szBuffer, _T("Hue"),        m_Codec.Source[wSrc].wHue, m_sAppIniName);		
		
		ReadConfigurationString(szBuffer, _T("NotchFilter"), _T("Auto"), szFilter,  SIZE, m_sAppIniName); 
		if (_tcsicmp(szFilter,_T("Auto")) == 0)
			m_Codec.Source[wSrc].wFilter = NOTCH_FILTER_AUTO;
		else if (_tcsicmp(szFilter,_T("Yes")) == 0)
			m_Codec.Source[wSrc].wFilter = NOTCH_FILTER_ON;
		else if (_tcsicmp(szFilter,_T("No")) == 0)
			m_Codec.Source[wSrc].wFilter = NOTCH_FILTER_OFF;

		m_Codec.Source[wSrc].bDoubleField= ReadConfigurationInt(szBuffer, _T("DoubleField"),m_Codec.Source[wSrc].bDoubleField, m_sAppIniName);		

		// Und alles schön protokollieren
		ML_TRACE(_T("Cam%02d Y=%d Br=%d Co=%d Sa=%d Hu=%d DF=%d\n"), wSrc+1,
																m_Codec.Source[wSrc].wPortYC,
																m_Codec.Source[wSrc].wBrightness,
																m_Codec.Source[wSrc].wContrast,
																m_Codec.Source[wSrc].wSaturation,
																m_Codec.Source[wSrc].wHue,
																m_Codec.Source[wSrc].bDoubleField);
	}

	if (m_bUseGlobalColorSettings)
	{
		// Frontend-Bildeinstellungen setzen
		SetBrightness(VIDEO_SOURCE_CURRENT,	m_wGlobalBrightness, TRUE);
		SetContrast(VIDEO_SOURCE_CURRENT,	m_wGlobalContrast, TRUE); 
		SetSaturation(VIDEO_SOURCE_CURRENT,	m_wGlobalSaturation, TRUE); 
		SetHue(VIDEO_SOURCE_CURRENT,		m_wGlobalHue, TRUE);
	}
	else
	{
		// Frontend-Bildeinstellungen setzen
		SetBrightness(VIDEO_SOURCE_CURRENT,	m_Codec.Source[m_wSource].wBrightness, TRUE);
		SetContrast(VIDEO_SOURCE_CURRENT,	m_Codec.Source[m_wSource].wContrast, TRUE); 
		SetSaturation(VIDEO_SOURCE_CURRENT,	m_Codec.Source[m_wSource].wSaturation, TRUE); 
		SetHue(VIDEO_SOURCE_CURRENT,		m_Codec.Source[m_wSource].wHue, TRUE);
	}

	// Framerate setzten
	SetFramerate(m_wFPS);

	ML_TRACE(_T("\n"));

}

/////////////////////////////////////////////////////////////////////////////
DWORD CCodec::GetVersion() const
{
	return SAVIC_UNIT_VERSION;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetLastError()
{
	m_csCodec.Lock();

	WORD wTemp = m_wLastError;
	m_wLastError = ERR_NO_ERROR;

	m_csCodec.Unlock();

	return wTemp;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::IsVideoPresent(BOOL& bPresent)
{
 	m_csCodec.Lock();

	BOOL bResult = FALSE;

	if (GetBT878Object())
		bResult = GetBT878Object()->IsVideoPresent(bPresent);

 	m_csCodec.Unlock();			    

	return bResult;
}
						   
/////////////////////////////////////////////////////////////////////////////
WORD CCodec::SetInputSource(SOURCE_SELECT_STRUCT &CurrSrcSel)
{
	m_csCodec.Lock();

//	WK_TRACE(_T("SetInputSource to %d (Next=%d)\n"), CurrSrcSel.wSource+1, NextSrcSel.wSource+1);

	// Bisherige Videoquelle
	WORD	wOldSource		= m_wSource;

	if (!CheckSource(CurrSrcSel.wSource))
	{
	 	m_csCodec.Unlock();
		return DEFAULT_ERROR;
	}
														 
	if (m_bTraceSetInputSource)
		ML_TRACE(_T("SetInputSource %u on SaVic\n"), CurrSrcSel.wSource + 1);

	// Bildgröße und Format setzen
	EncoderSetBPF(CurrSrcSel.dwBPF);
	EncoderSetFormat(CurrSrcSel.wFormat);

	// SourceSelect-Struktur um einige Einträge ergänzen
	CurrSrcSel.wPortYC		= m_Codec.Source[CurrSrcSel.wSource].wPortYC;
	CurrSrcSel.bDoubleField	= m_Codec.Source[CurrSrcSel.wSource].bDoubleField;
	
	// Die Farbeinstellungen der Kamera werden unmittelbar nach dem Umschaltung
	// im Treiber gesetzt.
	if (m_bUseGlobalColorSettings)
	{
		CurrSrcSel.wBrightness	= m_wGlobalBrightness;
		CurrSrcSel.wContrast	= m_wGlobalContrast;
		CurrSrcSel.wSaturation	= m_wGlobalSaturation;
		CurrSrcSel.wHue			= m_wGlobalHue;
	}
	else
	{
		CurrSrcSel.wBrightness	= m_Codec.Source[CurrSrcSel.wSource].wBrightness;
		CurrSrcSel.wContrast	= m_Codec.Source[CurrSrcSel.wSource].wContrast;
		CurrSrcSel.wSaturation	= m_Codec.Source[CurrSrcSel.wSource].wSaturation;
		CurrSrcSel.wHue			= m_Codec.Source[CurrSrcSel.wSource].wHue;
	}

	// Auf die gewünschte Kamera schalten.
	DoAnalogSwitch(CurrSrcSel);	

	// Im Standalone sollen zu Testzwecken die externen Videoausgänge geschaltet werden.
	if (ReadConfigurationInt(_T("Debug"), _T("Mode"),  0, m_sAppIniName))
	{
		SetCameraToAnalogOut(CurrSrcSel.wPortYC,  CSD_PORT0);
		SetCameraToAnalogOut(CurrSrcSel.wPortYC,  CSD_PORT1);
	}

	// Aktuelle Kamera merken.
	m_wSource  = CurrSrcSel.wSource;

	// Überprüft, ob die neue Kamera ein Videosignal liefert,
	BOOL bPresent = FALSE;
	if (IsVideoPresent(bPresent))
	{
		if (bPresent)
			OnReceivedCameraStatus(m_wSource, TRUE);
		else
			OnReceivedCameraStatus(m_wSource, FALSE);
	}

	m_nStatisticSwitchCounter++;
	
	m_csCodec.Unlock();

	return wOldSource;
}						    

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::WaitForExternEvent() const
{
	HANDLE hEvent[3];
	
	hEvent[0] = m_hWatchDogTriggerEvent;
	hEvent[1] = m_hPiezoEvent;
	hEvent[2] = m_hShutDownEvent;

	// Warte auf externe Events
	switch (WaitForMultipleObjects(3, hEvent, FALSE, 1000))
	{
		case WAIT_TIMEOUT:
		case WAIT_FAILED:
			return FALSE;			
		case WAIT_OBJECT_0:
			TriggerWatchDog();
			return TRUE;
		case WAIT_OBJECT_0+1:
			TriggerPiezo();
			return TRUE;
		case WAIT_OBJECT_0+2:		
			return FALSE;	
		default:
			return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetInputSource()
{
	return m_wSource;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetVideoFormat() const
{
	WORD wVideoFormat = VFMT_PAL_CCIR;

	if (GetBT878Object())
		wVideoFormat = GetBT878Object()->GetVideoFormat();

	return wVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetVideoFormat(WORD wVideoFormat)
{
 	m_csCodec.Lock();
	
	BOOL bResult = FALSE;
	
	if (CheckVideoFormat(wVideoFormat))
	{
		// Soll das Videoformat überhaupt geändert werden?
		if (wVideoFormat != GetVideoFormat())
		{
			// Frontend einstellen
			if (GetBT878Object())
			{
				if (GetBT878Object()->SetVideoFormat(wVideoFormat))
				{
					// VideoOverlay bzw Motiondetection uminitialisieren
					if (ReadConfigurationInt(_T("MotionDetection"), _T("Activate"), 1, m_sAppIniName) == 1)
						bResult = MotionDetectionInit();
					else
						bResult = VideoInlayInit();
				}
			}
		}
	}

 	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetBrightness(WORD wSource, WORD wBrightness, BOOL bPassThrough/*=FALSE*/)
{
	m_csCodec.Lock();
	BOOL bResult = FALSE;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		CheckRange(&wBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

		if (m_bTraceSetBrightness)
			ML_TRACE(_T("SetBrightness for Camera %u to %u\n"), wSource+1, wBrightness);

		if (GetBT878Object())
		{
			if (!bPassThrough || GetBT878Object()->SetBrightness(wBrightness))
			{
				// Die einstellungen in der Registrierung setzen
				if (m_bUseGlobalColorSettings)
				{
					if (wBrightness != m_wGlobalBrightness)
					{
						m_wGlobalBrightness = wBrightness;					
						WriteConfigurationInt(_T("FrontEnd"), _T("GlobalBrightness"), wBrightness, m_sAppIniName);					
					}
				}
				else
				{
					if (wBrightness != m_Codec.Source[wSource].wBrightness)
					{
						m_Codec.Source[wSource].wBrightness = wBrightness;
						_TCHAR szBuffer[255];
						wsprintf(szBuffer, _T("Mapping\\Source%02u"), wSource);
						WriteConfigurationInt(szBuffer, _T("Brightness"), wBrightness, m_sAppIniName);
					}
				}
				bResult = TRUE;
			}
		}
	}

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetBrightness(WORD wSource)
{
	WORD wBrightness = (MAX_BRIGHTNESS	- MIN_BRIGHTNESS)	/ 2;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		// Existiert eine globale Einstellung für alle Kameras?
		if (m_bUseGlobalColorSettings)
			wBrightness = m_wGlobalBrightness;
		else
			wBrightness = m_Codec.Source[wSource].wBrightness;
	}

	return wBrightness;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetContrast(WORD wSource, WORD wContrast, BOOL bPassThrough/*=FALSE*/)
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		CheckRange(&wContrast, MIN_CONTRAST, MAX_CONTRAST);

		if (m_bTraceSetContrast)
			ML_TRACE(_T("SetContrast for Camera %u to %u\n"), wSource+1, wContrast);

		if (GetBT878Object())
		{
			if (!bPassThrough || GetBT878Object()->SetContrast(wContrast))
			{
				if (m_bUseGlobalColorSettings)
				{
					if (wContrast != m_wGlobalContrast)
					{
						m_wGlobalContrast = wContrast;
						WriteConfigurationInt(_T("FrontEnd"), _T("GlobalContrast"), wContrast, m_sAppIniName);					
					}
				}
				else
				{
					if (wContrast != m_Codec.Source[wSource].wContrast)
					{
						m_Codec.Source[wSource].wContrast = wContrast;
						_TCHAR szBuffer[255];
						wsprintf(szBuffer, _T("Mapping\\Source%02u"), wSource);
						WriteConfigurationInt(szBuffer, _T("Contrast"), wContrast, m_sAppIniName);
					}
				}
				bResult = TRUE;
			}
		}
	}

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetContrast(WORD wSource)
{
	WORD wContrast = (MAX_CONTRAST - MIN_CONTRAST) / 2;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		// Existiert eine globale Einstellung für alle Kameras?
		if (m_bUseGlobalColorSettings)
			wContrast = m_wGlobalContrast;
		else
			wContrast = m_Codec.Source[wSource].wContrast;
	}

	return wContrast;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetSaturation(WORD wSource, WORD wSaturation, BOOL bPassThrough/*=FALSE*/)
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		CheckRange(&wSaturation, MIN_SATURATION, MAX_SATURATION);
						    
		if (m_bTraceSetSaturation)
			ML_TRACE(_T("SetSaturation for Camera %u to %u\n"), wSource+1, wSaturation);

		if (GetBT878Object())
		{
			if (!bPassThrough || GetBT878Object()->SetSaturation(wSaturation))
			{
				if (m_bUseGlobalColorSettings)
				{
					if (wSaturation != m_wGlobalSaturation)
					{
						m_wGlobalSaturation = wSaturation;
						WriteConfigurationInt(_T("FrontEnd"), _T("GlobalSaturation"), wSaturation, m_sAppIniName);					
					}
				}
				else
				{
					if (wSaturation != m_Codec.Source[wSource].wSaturation)
					{
						m_Codec.Source[wSource].wSaturation = wSaturation;
						_TCHAR szBuffer[255];
						wsprintf(szBuffer, _T("Mapping\\Source%02u"), wSource);
						WriteConfigurationInt(szBuffer, _T("Saturation"), wSaturation, m_sAppIniName);				
					}
				}
				bResult = TRUE;
			}
		}
	}

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetSaturation(WORD wSource)
{
	WORD wSaturation = (MAX_SATURATION - MIN_SATURATION) / 2;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		// Existiert eine globale Einstellung für alle Kameras?
		if (m_bUseGlobalColorSettings)
			wSaturation = m_wGlobalSaturation;
		else
			wSaturation = m_Codec.Source[wSource].wSaturation;
	}

	return wSaturation;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetHue(WORD wSource, WORD wHue, BOOL bPassThrough/*=FALSE*/)
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		CheckRange(&wHue, MIN_HUE, MAX_HUE);

		if (m_bTraceSetHue)
			ML_TRACE(_T("SetHue for Camera %u to %u\n"), wSource+1, wHue);

		if (GetBT878Object())
		{
			if (!bPassThrough || GetBT878Object()->SetHue(wHue))
			{
				if (m_bUseGlobalColorSettings)
				{
					if (wHue != m_wGlobalHue)
					{
						m_wGlobalHue = wHue;
						WriteConfigurationInt(_T("FrontEnd"), _T("GlobalHue"), wHue, m_sAppIniName);					
					}
				}
				else
				{
					if (wHue != m_Codec.Source[wSource].wHue)
					{
						m_Codec.Source[wSource].wHue = wHue;
						_TCHAR szBuffer[255];
						wsprintf(szBuffer, _T("Mapping\\Source%02u"), wSource);
						WriteConfigurationInt(szBuffer, _T("Hue"), wHue, m_sAppIniName);
					}
				}
				bResult = TRUE;
			}
		}
	}

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetHue(WORD wSource)
{
	WORD wHue = (MAX_HUE	- MIN_HUE)	/ 2;

	// Quelle ok?
	if (CheckSource(wSource))
	{
		// Existiert eine globale Einstellung für alle Kameras?
		if (m_bUseGlobalColorSettings)
			wHue = m_wGlobalHue;
		else
			wHue = m_Codec.Source[wSource].wHue;
	}

	return wHue;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetFilter(WORD wFilter)
{
 	m_csCodec.Lock();

	BOOL bResult = FALSE;

	if(CheckFeFilter(wFilter))
	{
		// Beide Frontends einstellen
		if (GetBT878Object())
		{
			if (GetBT878Object()->SetFilter(wFilter))
			{			
				bResult = TRUE;
				if (m_bTraceSetFilter)
					ML_TRACE(_T("SetFilter Notch=%s\tLDec=%s\n"),  wFilter & LNOTCH_FILTER ? _T("Off") : _T("On"),
													wFilter & LDEC_FILTER ? _T("Off") : _T("On"));
			}
		}
	}
	else
	{
		if (m_bTraceSetFilter)
			ML_TRACE(_T("SetFilter Unknown FeFilter\n"));

	}

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetFilter() const
{
	WORD wFilter = NOTCH_FILTER_ON;

	if (GetBT878Object())
		wFilter = GetBT878Object()->GetFilter();

	return wFilter;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetFeHScale(WORD wHScale)
{
 	m_csCodec.Lock();

	BOOL bResult = FALSE;

	CheckRange(&wHScale, MIN_HSCALE, MAX_HSCALE);

	if (GetBT878Object())
	{
		// Zunächst ist die Skalierungen für Odd und Even-field gleich
		if (GetBT878Object()->SetHScaleE(wHScale) && (GetBT878Object()->SetHScaleO(wHScale)))
		{
			bResult = TRUE;
		}
	}
				
 	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::GetFeHScale(WORD& wHSCale) const
{
	BOOL bResult = FALSE;
	
	// Da uunächst die Skalierung für Odd und Even-field gleich sind, wird
	// hier stellvertretend die Sklalierung des Even-fields geliefert.
	if (GetBT878Object())
		bResult = GetBT878Object()->GetHScaleE(wHSCale);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetFeVScale(WORD wVScale)
{
 	m_csCodec.Lock();

	BOOL bResult = FALSE;

	CheckRange(&wVScale, MIN_VSCALE, MAX_VSCALE);

	// Zunächst ist die Skalierungen für Odd und Even-field gleich
	if (GetBT878Object())
	{
		if (GetBT878Object()->SetVScaleE(wVScale) && GetBT878Object()->SetVScaleO(wVScale))
		{
			bResult = TRUE;
		}
	}

 	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::GetFeVScale(WORD& wVScale) const
{
	BOOL bResult = FALSE;

	// Da uunächst die Skalierung für Odd und Even-field gleich sind, wird
	// hier stellvertretend die Sklalierung des Even-fields geliefert.
	if (GetBT878Object())
		bResult = GetBT878Object()->GetVScaleE(wVScale);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetHActive(WORD wHActive) const
{
	BOOL bResult = FALSE;
	
	if (GetBT878Object())
		bResult = GetBT878Object()->SetHActiveE(wHActive) && GetBT878Object()->SetHActiveO(wHActive);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DlgLuminance() const
{
	BOOL bRet = FALSE;

	if (m_pDlgColor)
		m_pDlgColor->ShowWindow(SW_SHOW);
	
	if (m_pDlgColor)
		bRet = m_pDlgColor->IsValid();

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::PollAlarm()
{
	WORD wCurrentAlarmState		= 0;
	BOOL  bRetFlag				= FALSE;

	// Es darf nur auf die Alarmeingänge gepollt werden, wenn die Diagnosefunktion nicht
	// läuft, da ansonsten das SaVicDiagnostic Programm den Alarmeingang zum Relaytest nicht
	// sicher abfragen kann. 
	if (!m_bDiagnosticIsRunning)
	{
		// CurrentAlarmstatusregister lesen.
		wCurrentAlarmState = GetCurrentAlarmState();
												
		if (wCurrentAlarmState != m_wAlarmState)
		{	
			if (m_bTraceAlarmIndication)						
				ML_TRACE(_T("Alarmstate changed from 0x%x to 0x%x\n"), m_wAlarmState, wCurrentAlarmState);

			m_wAlarmState = wCurrentAlarmState;
			
			OnReceivedAlarm(wCurrentAlarmState);

			bRetFlag = TRUE;
		}

		// Zweimal in der Sekunde den Status des PowerOffButton/ResetButton und das FiFoFull-Flags abfragen.
		if (GetTimeSpan(m_dwLastPowerButtonPollTimeTC) > 500)
		{
			m_dwLastPowerButtonPollTimeTC = GetTickCount();

			// Den PowerOff/Reset-Button abfragen
			// Auf den PowerOff-Button pollen
			if (GetPowerOffButtonState())
				OnPowerOffButtonPressed();

			// Auf den Reset-Button pollen
			if (GetResetButtonState())
				OnResetButtonPressed();
		}
	#if (0)
		// Alle 10 Sekunden die Kameraeingänge scannen.
		static DWORD dwTC2 = 0;
		if (GetTimeSpan(dwTC2) > 10000)
		{
			dwTC2 = GetTickCount();
			DWORD dwCamMask = ScanForCameras(0xffff);
		}
	#endif
	}

	return bRetFlag;

}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedAlarm(WORD /*wCurrentAlarmState*/)	const
{
	// Dummy funktion
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedCameraStatus(WORD /*wSource*/, BOOL /*bStatus*/) const
{
	// Dummy funktion
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedMotionAlarm(WORD /*wSource*/, CMDPoints &Points/*Point*/) const
{
	// Dummy funktion
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnPowerOffButtonPressed() const
{
	// Dummy funktion
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnResetButtonPressed() const
{
	// Dummy funktion
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedMotionDIB(WORD wSource, HANDLE hDIB)
{
	// Dummy funktion
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetCurrentAlarmState()
{
	m_csCodec.Lock();

	WORD wRet = 0;

	if (GetAlarmObject())
		wRet = GetAlarmObject()->GetState();

	m_csCodec.Unlock();
	
	return wRet;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::SetAlarmEdge(WORD wEdge)
{
 	m_csCodec.Lock();

	WORD wOldEdge = 0;

	wOldEdge = GetAlarmEdge();

	if (GetAlarmObject())
		GetAlarmObject()->SetEdge(wEdge);

	if (m_bTraceSetAlarmEdge)
		ML_TRACE(_T("SetAlarmEdge (Mask=0x%x)\n"), wEdge); 
	
	m_csCodec.Unlock();

	return wOldEdge;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetAlarmEdge()
{
 	m_csCodec.Lock();

	WORD wRet = 0;

	if (GetAlarmObject())
		wRet = GetAlarmObject()->GetEdge();

	m_csCodec.Unlock();

	return wRet;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::ResetSavedAlarmState()
{
 	m_csCodec.Lock();

	m_wAlarmState = 0;

	m_csCodec.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BYTE CCodec::GetRelais()
{
 	m_csCodec.Lock();

	BYTE byRet = 0;

	if (GetRelayObject())
		byRet = GetRelayObject()->GetRelay();

	m_csCodec.Unlock();

	return byRet;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CCodec::SetRelais(BYTE byRelais)
{
 	m_csCodec.Lock();

	BYTE byOldRelais = 0; 

	byOldRelais = GetRelais(); 
	if (GetRelayObject())
		GetRelayObject()->SetRelay(byRelais);

	if (m_bTraceSetRelais)
		ML_TRACE(_T("SetRelais (Mask=0x%x)\n"), (WORD)byRelais);

  	m_csCodec.Unlock();

	return byOldRelais;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TriggerWatchDog() const
{ 
	int nTi = ReadConfigurationInt(_T("General"), _T("WatchDogTimeout"), -1, m_sAppIniName);

	BOOL bResult = FALSE;

	if (!m_pCWatchDog)
		return FALSE;
	
	if (nTi != -1)
		bResult = m_pCWatchDog->TriggerWatchDog(nTi);
	else	// Watchdog ausschalten
		bResult = m_pCWatchDog->DisableWatchDog(); 
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::DisableWatchdog() const
{
	if (m_pCWatchDog)
		m_pCWatchDog->DisableWatchDog(); 
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::EnableWatchdog() const
{
	if (m_pCWatchDog)
		m_pCWatchDog->EnableWatchDog(); 
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::TriggerPiezo() const
{
	int nState = ReadConfigurationInt(_T("General"), _T("PiezoState"), -1, m_sAppIniName);

	CRelay* pRelay = GetRelayObject();

	switch (nState)
	{
		case 0:
			if (pRelay)
				pRelay->BeeperOff();
			break;
		case 1:
			if (pRelay)
				pRelay->BeeperOn();
			break;
		default:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EncoderStart()
{
	BOOL bResult = FALSE;
	
	m_csCodec.Lock();

	m_pPerf1 = new CWK_PerfMon(_T("Compressing"));
	m_pPerf2 = new CWK_PerfMon(_T("Scaling"));
	m_pPerf3 = new CWK_PerfMon(_T("CopyDIB"));

	TraceStatistics(NULL, TRUE);

	if (m_bTraceStartStopEncoder)
		ML_TRACE(_T("CCodec::EncoderStart\n"));

	if (m_pBT878)
	{
		if (m_pBT878->EncoderStart())
		{
			m_nEncoderState = ENCODER_ON;
			bResult = TRUE;
		}
	}

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EncoderStop()
{
	BOOL bResult = FALSE;

	m_csCodec.Lock();

	if (m_bTraceStartStopEncoder)
		ML_TRACE(_T("CCodec::EncoderStop\n"));
	
	if (m_pBT878)
	{
		if (m_pBT878->EncoderStop())
		{
			m_nEncoderState = ENCODER_OFF;
			bResult = TRUE;
		}
	}


	WK_DELETE(m_pPerf1);
	WK_DELETE(m_pPerf2);
	WK_DELETE(m_pPerf3);

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::RestartEncoder()
{
	BOOL bResult = FALSE;

	m_csCodec.Lock();

	if (m_pBT878 && (m_nEncoderState == ENCODER_ON))
		bResult = m_pBT878->RestartEncoder();

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::EncoderSetFormat(WORD wFormat)
{
	m_csCodec.Lock();
	
	WORD wOldFormat = DEFAULT_ERROR;
	
	if (CheckEncoderFormat(wFormat))
	{
		wOldFormat = EncoderGetFormat();

		// ToDo: Muß für SaVic noch implementiert werden

		m_wFormat = wFormat;
	}

  	m_csCodec.Unlock();

	return wOldFormat;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::EncoderGetFormat() const
{
	return 	m_wFormat;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCodec::EncoderSetBPF(DWORD dwBPF)
{
	m_csCodec.Lock();

	DWORD dwOldBPF = DEFAULT_ERROR;

	if (CheckEncoderBPF(dwBPF))
	{
		if (m_bTraceSetBPF)
			ML_TRACE(_T("EncoderSetBPF=%d\n"), dwBPF);

		dwOldBPF = EncoderGetBPF();
		m_dwBPF = dwBPF;
	}

	m_csCodec.Unlock();

	return dwOldBPF;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCodec::EncoderGetBPF() const
{
	return m_dwBPF;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EncoderSetMode(WORD wFormat, DWORD dwBPF)
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;

	if (CheckEncoderFormat(wFormat))
	{
		if (CheckEncoderBPF(dwBPF))
		{
			// ToDo: Muß für SaVic noch implementiert werden
		
			bResult		= TRUE;
			m_wFormat	= wFormat;
			m_dwBPF		= dwBPF;
		}
	}
	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
int CCodec::GetEncoderState() const
{
	return m_nEncoderState;
}							 

/////////////////////////////////////////////////////////////////////////////
void CCodec::ReadData()
{
	IMAGE	*pImage	= NULL;
	HANDLE	hDIB	= NULL;
	BOOL	bOk		= FALSE;
	
	HANDLE hEvent[2];
	
	hEvent[0] = m_hFieldReadyEvent;
	hEvent[1] = m_hShutDownEvent;
	static DWORD dwLastFieldID = 0;
	static CSystemTime LastTime; 

	// Es waren keine neuen Bilddaten verfügbar, also dann drauf warten
	switch (WaitForMultipleObjects(2, hEvent, FALSE, 500))
	{
		case WAIT_TIMEOUT:
			if (GetEncoderState() == ENCODER_ON)
			{
				if (m_bTraceWaitForImageTimeout)
				{
					CString sText;
					sText.Format(_T("Wait for image timeout (Current Source=%d)"), m_wSource+1);
					ML_STAT_PEAK(_T("Reset"), 1, sText);
				}
				OnReceivedEncoderError(ERR_TIMEOUT);
			}
			break;
		case WAIT_FAILED:
			ML_TRACE_ERROR(_T("CCodec::ReadData\tWAIT_FAILED\n"));
			OnReceivedEncoderError(ERR_WAITFAILED);
			break;
		case WAIT_OBJECT_0:			  
			do
			{
				TraceEncoderQueue();
				CAPTURE_STRUCT Capture = {0};
				if ((bOk=DACaptureFrame(m_hDevice, Capture)))
				{
					if (m_nEncoderState == ENCODER_ON)
					{
						if (Capture.wCompressionType == COMPRESSION_JPEG)
							pImage = CompressImageToJpeg(Capture);
						else if (Capture.wCompressionType == COMPRESSION_YUV_422)
							pImage = CreateDIB(Capture);
						else
						{
							ML_TRACE_ERROR(_T("CCodec::ReadData: Unknown requested compressiontype (%d) assume Jpeg\n"), Capture.wCompressionType);
							Capture.wCompressionType = COMPRESSION_JPEG;
							pImage = CompressImageToJpeg(Capture);
						}
					}
					MotionDetection(Capture);
					DAReleaseQueueBuffer(m_hDevice);
					if (pImage)
					{
						if (pImage->dwFieldID < dwLastFieldID)
							ML_TRACE_WARNING(_T("### Wrong FieldID (%lu < %lu)\n"), pImage->dwFieldID, dwLastFieldID);
						dwLastFieldID = pImage->dwFieldID;
						CSystemTime CurrTime(pImage->TimeStamp.wDay,
											 pImage->TimeStamp.wMonth,
											 pImage->TimeStamp.wYear,
											 pImage->TimeStamp.wHour,
											 pImage->TimeStamp.wMinute,
											 pImage->TimeStamp.wSecond,
											 pImage->TimeStamp.wMSecond);
										 							
						if (CurrTime < LastTime)
						{
							CString sCurrTime = CurrTime.GetDBTime();
							CString sLastTime = LastTime.GetDBTime();
							ML_TRACE_WARNING(_T("Currtime < LastTime (%s < %s)\n"), sCurrTime, sLastTime);
						}
						LastTime = CurrTime;


						OnReceivedImageData(pImage);
					}
				}
			}while(bOk);
			break;
		case WAIT_OBJECT_0+1:			   
			break;
		default:
			ML_TRACE_WARNING(_T("CZoranISA::ReadData\tUnknown returnvalue\n"));
			OnReceivedEncoderError(DEFAULT_ERROR);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedEncoderError(int /*nErrorCode*/)	const
{

}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedImageData(const IMAGE* /*pImage*/) const
{
//	TraceStatistics(pImage);
//	TraceEncoderQueue();
}


/////////////////////////////////////////////////////////////////////////////
void CCodec::OnReceivedRunTimeError(WK_RunTimeErrorLevel /*Level*/,
									WK_RunTimeError /*RTError*/,
									const CString& /*sError*/,
								   DWORD /*dwParam1*/,
								   DWORD /*dwParam2*/,
								   WK_RunTimeErrorSubcode /*errorSubcode*/) const
{
	ML_TRACE(_T("CCodec::OnReceivedRunTimeError not overridden\n"));
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckSource(WORD &wSource)
{
	switch(wSource)
	{
		case VIDEO_SOURCE_CURRENT:
			wSource = m_wSource;
			// FALLTHROUGH
		case VIDEO_SOURCE0: 
			// FALLTHROUGH
		case VIDEO_SOURCE1: 
			// FALLTHROUGH
		case VIDEO_SOURCE2: 
			// FALLTHROUGH
		case VIDEO_SOURCE3: 
			// FALLTHROUGH
			return TRUE;
		default:
			m_wLastError = ERR_WRONG_SOURCE;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckVideoFormat(WORD wVideoFormat)
{
	switch(wVideoFormat)
	{
		case VFMT_PAL_CCIR: 
			if (m_bTraceSetVideoFormat)
				ML_TRACE(_T("SetVideoType PAL CCIR\n"));
        	return TRUE;

		case VFMT_NTSC_CCIR:
			if (m_bTraceSetVideoFormat)
				ML_TRACE(_T("SetVideoType NTSC CCIR\n"));
        	return TRUE;

		default:
			if (m_bTraceSetVideoFormat)
				ML_TRACE(_T("SetVideoType Unknown FeVideoFormat\n"));
			m_wLastError = ERR_WRONG_FE_VIDEO_FORMAT;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckFeFilter(WORD wFilter)
{
	if (wFilter <= (LNOTCH_FILTER | LDEC_FILTER))
	{
		return TRUE;
	}
	else
	{
		m_wLastError = ERR_WRONG_FILTER_TYPE;
		return FALSE;
	}
}
 
/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckEncoderFormat(WORD wFormat)
{
	switch(wFormat)
	{
		case ENCODER_RES_HIGH: 
			if (m_bTraceSetFormat)
				ML_TRACE(_T("EncoderSetFormat High\n"));
        	return TRUE;		
		
		case ENCODER_RES_LOW: 
			if (m_bTraceSetFormat)
				ML_TRACE(_T("EncoderSetFormat Low\n"));
			return TRUE;

		default:
			if (m_bTraceSetFormat)
				ML_TRACE(_T("EncoderSetFormat unknown Format\n"));
			m_wLastError = ERR_WRONG_ENCODER_FORMAT;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckEncoderBPF(DWORD dwBPF)
{
	if ((dwBPF >= MIN_BPF) && (dwBPF <= MAX_BPF))
		return (TRUE);
	else
	{
		m_wLastError = ERR_WRONG_ENCODER_BPF;
		return FALSE;
	}		
} 
 
/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckIRQNumber(BYTE byIRQ) const
{
	return (BOOL)((byIRQ >=3) && (byIRQ <= 15));
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CheckRange(LPWORD lpwValue, long lMin, long lMax)
{
	if (!lpwValue)
	{
		ML_TRACE_ERROR(_T("CCodec::CheckRange\tlpwValue = Null\n"));
		return FALSE;
	}

	if (*lpwValue < (WORD)lMin)
	{
		m_wLastError = ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMin;
		return FALSE;
	}
	else if (*lpwValue > (WORD)lMax)
	{
		m_wLastError = ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMax;
		return FALSE;
	
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::VideoInlaySetOutputWindow(const CRect &rc)
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;
	
	if (GetBT878Object())
	{
//		bResult = GetBT878Object()->VideoInlaySetOutputWindow(CRect(56,82, rc.right, 344));
		bResult = GetBT878Object()->VideoInlaySetOutputWindow(rc);
	}
	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::VideoInlayActivate()
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;
	
	if (GetBT878Object())
		bResult = GetBT878Object()->VideoInlayActivate();

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::VideoInlayDeactivate()
{
	m_csCodec.Lock();

	BOOL bResult = FALSE;
	
	if (GetBT878Object())
		bResult = GetBT878Object()->VideoInlayDeactivate();

	m_csCodec.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EnableDIBIndication(WORD wSource)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->EnableDIBIndication(wSource);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DisableDIBIndication(WORD wSource)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->DisableDIBIndication(wSource);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::MotionDetection(CAPTURE_STRUCT& Capture)
{
	m_csCodec.Lock();

	CMDPoints Points;

	// Überprüft, ob Bewegung detectiert wurde.
	if (MotionCheck(Capture, Points))
		OnReceivedMotionAlarm(Capture.wSource, Points);

	m_csCodec.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points)
{
	BOOL bResult = FALSE;

	if (CheckSource(Capture.wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->MotionCheck(Capture, Points);
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::MotionDetectionConfiguration(WORD wSource)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->MotionDetectionConfiguration(wSource);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::ActivateMotionDetection(WORD wSource)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->ActivateMotionDetection(wSource);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DeactivateMotionDetection(WORD wSource)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->DeactivateMotionDetection(wSource);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetMDMaskSensitivity(WORD wSource, const CString &sLevel)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->SetMDMaskSensitivity(wSource, sLevel);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetMDAlarmSensitivity(WORD wSource, const CString &sLevel)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->SetMDAlarmSensitivity(wSource, sLevel);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
CString CCodec::GetMDMaskSensitivity(WORD wSource)
{
	CString sLevel = _T("Unknown");

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			sLevel = GetBT878Object()->GetMDMaskSensitivity(wSource);
	}

	return sLevel;
}

/////////////////////////////////////////////////////////////////////////////
CString CCodec::GetMDAlarmSensitivity(WORD wSource)
{
	CString sLevel = _T("Unknown");

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			sLevel = GetBT878Object()->GetMDAlarmSensitivity(wSource);
	}

	return sLevel;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::GetMask(WORD wSource, CIPCActivityMask& mask)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->GetMask(wSource, mask);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetMask(WORD wSource, const CIPCActivityMask& mask)
{
	BOOL bResult = FALSE;

	if (CheckSource(wSource))
	{
		if (GetBT878Object())
			bResult = GetBT878Object()->SetMask(wSource, mask);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::WaitForDriverDebugEvent() const 
{
	EVENT_PACKET EventPacket;
	HANDLE	hEvent[2];
	CString	sMsg;

	// Liegt vielleicht schon ein Eventpacket vor?
	DAReadEventPacket(m_hDevice, EventPacket);

	// Packet gültig?
	if (!EventPacket.bValid)
	{	
		hEvent[0] = m_hDebugEvent;
		hEvent[1] = m_hShutDownEvent;

		switch (WaitForMultipleObjects(2, hEvent, FALSE, 1000))
		{
			case WAIT_OBJECT_0:
				DAReadEventPacket(m_hDevice, EventPacket);		
				break;
			case WAIT_OBJECT_0+1:
				return TRUE;
			case WAIT_TIMEOUT:
			case WAIT_FAILED:
				return FALSE;
			default:
				ML_TRACE_WARNING(_T("Unbekantes TracePacket Format\n"));
				return FALSE;
		}			 

		// Packet gültig?
		if (!EventPacket.bValid)
			return FALSE;
	}

	switch (EventPacket.dwNumParam)
	{
		case 0:
			sMsg = EventPacket.Message; 
			break;
		case 1:
			sMsg.Format(EventPacket.Message, EventPacket.dwParam1); 
			break;
		case 2:
			sMsg.Format(EventPacket.Message,
						EventPacket.dwParam1,
						EventPacket.dwParam2); 
			break;
		case 3:
			sMsg.Format(EventPacket.Message,
						EventPacket.dwParam1,
						EventPacket.dwParam2, 
						EventPacket.dwParam3); 
			break;
		case 4:
			sMsg.Format(EventPacket.Message,
						EventPacket.dwParam1,
						EventPacket.dwParam2, 
						EventPacket.dwParam3, 
						EventPacket.dwParam4); 
	}	

	sMsg = _T("Driver:") + sMsg;
	
	if (EventPacket.wEventType == EVENT_INFO)
	{
		if (m_bTraceDriverInfo)
			ML_TRACE(sMsg);
	}
	else if (EventPacket.wEventType == EVENT_WARNING)
	{
		if (m_bTraceDriverWarning)
			ML_TRACE_WARNING(sMsg);
	}
	else if (EventPacket.wEventType == EVENT_ERROR)
	{														 
		if (m_bTraceDriverError)
			ML_TRACE_ERROR(sMsg);
	}
	else if (EventPacket.wEventType == EVENT_STAT_PEAK_INFO)
	{
		if (m_bTraceDriverInfo)
			ML_STAT_PEAK(_T("Reset"), 1, sMsg);
	}
	else if (EventPacket.wEventType == EVENT_STAT_PEAK_WARNING)
	{
		if (m_bTraceDriverWarning)
			ML_STAT_PEAK(_T("Reset"), 1, sMsg);
	}
	else if (EventPacket.wEventType == EVENT_STAT_PEAK_ERROR)
	{
		if (m_bTraceDriverError)
			ML_STAT_PEAK(_T("Reset"), 1, sMsg);
	}
	else
		ML_TRACE_WARNING(_T("Unknown EventPacket-Type\n"));

	// Packet freigeben.
	EventPacket.bValid = FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::TraceEncoderQueue()	const
{
	WORD		wI			= 0;
	CString		s			= _T("");
	CString		s2			= _T("");
	CString		sSource[]	= {_T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8")};
	QUEUE_INF	QInf;

	// Soll der Queuestatus ausgegeben werden?
	if (!m_bTraceEncoderQueue)
		return;

	// Queuestatus holen.
	if (!DAReadQueueState(m_hDevice, QInf))
		return;

	// Ist die Queue leer?
//	if (QInf.wWriteIndex == QInf.wReadIndex)
//		return;

	// Writeindex zeichnen
	s=_T("");
	for (wI = 0; wI < QInf.wWriteIndex; wI++)
		s=s+_T(" ");
	s=s+_T("W");
	ML_TRACE(_T("%s\n"), s);
				 
	// Queue zeichnen
	s=_T("");
	for (wI = 0; wI < MAX_FIELD_BUFFER; wI++)
	{
		if (QInf.wQueueState[wI] != VIDEO_SOURCE_NO)
			s=s+sSource[QInf.wQueueState[wI]];
		else
			s=s+_T("0");
	}

	ML_TRACE(_T("%s\n"), s);

	// Readindex zeichnen
	s=_T("");
	for (wI = 0; wI < QInf.wReadIndex; wI++)
		s=s+_T(" ");
	s=s+_T("R");
	ML_TRACE(_T("%s\n"), s);
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::TraceStatistics(const IMAGE* pImage, BOOL bCLear)
{
	DWORD  dwTimeDiff			= 0;

	if (bCLear)
	{
		m_dwStatisticTotalBytes  = 0;	// Statistic zurücksetzen
		m_dwStatisticFieldCnt	 = 0;
		m_dwStatisticTC		 = GetTickCount();
	}
	else if (pImage)
	{
		m_dwStatisticTotalBytes += pImage->dwImageLen;
		m_dwStatisticFieldCnt++;
		dwTimeDiff = GetTimeSpan(m_dwStatisticTC);

		if (dwTimeDiff > 60000)
		{
			m_dwStatisticTC = GetTickCount();
			if (m_bTraceDataRate)
				ML_STAT_LOG(_T("Reset"), m_dwStatisticTotalBytes / 1024 * 1000 / dwTimeDiff, _T("KByte/Sec"));
			if (m_bTraceFrameRate)
				ML_STAT_LOG(_T("Reset"), (m_dwStatisticFieldCnt+15) * 1000 / dwTimeDiff, _T("Fps"));
			m_dwStatisticTotalBytes = 0;			
			m_dwStatisticFieldCnt	= 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingFrontends() const
{
	return  FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingRelais() const
{
	if (!GetRelayObject())
		return FALSE;

	return GetRelayObject()->Diagnostic(GetBoardVariations());
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingRelais2() const
{
	if (!GetRelayObject())
		return FALSE;

	return GetRelayObject()->Diagnostic2(GetBoardVariations());
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingWatchdog() const
{
	if (!m_pCWatchDog)
		return FALSE;

	return m_pCWatchDog->Diagnostic();
}

//////////////////////////////////////////////////////////////////////
// Liefert eine Bitmaske, die wiederspiegelt, welche Kameras ein Videosignal liefern.
// dabei werden nur diejenigen Kameras geprüft, deren zugehöriges Bit in der ScanMask
// gesetzt ist.
#pragma	optimize("", off)
DWORD CCodec::ScanForCameras(DWORD dwScanMask)
{
	DWORD	dwCamMask	= 0;

	m_csCodec.Lock();

	if (m_bNewCameraScanMethod)
	{
		// Alle zu überprüfenden Kameras analog am inaktiven Frontend durchschalten
		// und testen, ob ein Videosignal vorhanden ist.
		for (int nI = 0; nI < MAX_CAMERAS; nI++)
		{
			if (TSTBIT(dwScanMask, nI))
			{															 
				// Kamera auswählen
				GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[nI].wPortYC, VS_MUX);
				DWORD dwCnt = 0;
				WK_Sleep(20); // Nach der Umschaltung muß gewartet werden!!

				// 250 ms lang auf eine fallende oder steigende Flanke pollen
				BOOL b0 = GetBT878Object()->ReadGPIOPin(BT878_GPIO_VS_ACTIVE_BIT);
				BOOL b1 = 0;

				dwCnt = 0;
				do
				{
					WK_Sleep(11); // Nach der Umschaltung muß gewartet werden!!
					//Sleep(11); // ACHTUNG: Ein Sleep(10) dauert zwischen 1ms und 9ms!!! und das ist zu kurz
					b1 = GetBT878Object()->ReadGPIOPin(BT878_GPIO_VS_ACTIVE_BIT);
				}while ((b0 == b1) && (dwCnt++ < 7));

				// Flanke?
				if ((b0 != b1))
					dwCamMask = SETBIT(dwCamMask, nI); // Signal vorhanden
				else
					dwCamMask = CLRBIT(dwCamMask, nI); // Kein Signal vorhanden.
			}
		}
		if (m_dwCurrCamMask != dwCamMask)
		{
			ML_TRACE(_T("CameraMask changed from 0x%x to 0x%x\n"), (WORD)m_dwCurrCamMask, (WORD)dwCamMask);
			m_dwCurrCamMask = dwCamMask;
			MessageBeep(0);
		}
	}
	else // Use old Scanmethod
	{
		BOOL	bVideoLost	= FALSE;
		SOURCE_SELECT_STRUCT SrcSel = {0};

		Sleep(100); // Warte bis alle Umschaltungen aus der IRQ erfolgt sind

		WORD wCurrentSource = GetInputSource();
		
		// Alle zu überprüfenden Kameras analog am inaktiven Frontend durchschalten
		// und testen, ob ein Videosignal vorhanden ist.
		for (int nI = VIDEO_MAX_SOURCE; nI >= 0; nI--)
		{
			if (TSTBIT(dwScanMask, nI))
			{															 
				SrcSel.wSource			= nI;
				SrcSel.wPortYC			= m_Codec.Source[nI].wPortYC;
				SrcSel.wCompressionType = COMPRESSION_JPEG;
				SrcSel.wBrightness		= m_Codec.Source[nI].wBrightness;
				SrcSel.wContrast		= m_Codec.Source[nI].wContrast;
				SrcSel.wSaturation		= m_Codec.Source[nI].wSaturation;
				SrcSel.wHue				= m_Codec.Source[nI].wHue;
	//			ML_TRACE(_T("ScanForCameras Camera %d\n"), nI+1);

				DoAnalogSwitch(SrcSel);
				
				Sleep(20);					// Warte mindestens 31 Zeilen (31*64ys=2ms)
			
				BOOL bPresent = FALSE;
				if (GetBT878Object()->IsVideoPresent(bPresent))
				{
					if (bPresent)
						dwCamMask = SETBIT(dwCamMask, nI); // Signal vorhanden
					else
					{
						dwCamMask = CLRBIT(dwCamMask, nI); // Kein Signal vorhanden.
						bVideoLost= TRUE;			
					}
				}
			}
		}
		// Wieder auf die ursprüngliche Kamera zurückschalten
		SrcSel.wSource			= wCurrentSource;
		SrcSel.wPortYC			= m_Codec.Source[wCurrentSource].wPortYC;
		SrcSel.wCompressionType = COMPRESSION_JPEG;
		SrcSel.wBrightness		= m_Codec.Source[wCurrentSource].wBrightness;
		SrcSel.wContrast		= m_Codec.Source[wCurrentSource].wContrast;
		SrcSel.wSaturation		= m_Codec.Source[wCurrentSource].wSaturation;
		SrcSel.wHue				= m_Codec.Source[wCurrentSource].wHue;
		DoAnalogSwitch(SrcSel);	

		if (m_dwCurrCamMask != dwCamMask)
		{
			ML_TRACE(_T("CameraMask changed from 0x%x to 0x%x\n"), (WORD)m_dwCurrCamMask, (WORD)dwCamMask);
			m_dwCurrCamMask = dwCamMask;
			MessageBeep(0);
		}
	//	WK_TRACE(_T("Scan for cameras result:0x%x\n"), (WORD)dwCamMask);
	}

	m_csCodec.Unlock();

	return dwCamMask;
}
#pragma	optimize("", on)

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EnableDoubleFieldMode(WORD wSource, BOOL bEnable)
{
	m_Codec.Source[wSource].bDoubleField = bEnable;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::IsDoubleFieldMode(WORD wSource)
{
	return m_Codec.Source[wSource].bDoubleField;
}

//////////////////////////////////////////////////////////////////////
BYTE CCodec::ReadVariationRegister() const
{
	BYTE byVal = 0;

	if (m_pBT878)
	{
		// Variationregister auslesen
		DWORD dwData;
		if (m_pBT878->BT878In(BT878_GPIO_DATA_CTL, dwData))
			byVal = (dwData>>21) & 0x07;
	}

	return byVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CCodec::GetBoardVariations() const
{
	DWORD dwResult = 0;
	
	BYTE byBoardVariations = ReadVariationRegister();
	
	switch (byBoardVariations)
	{
		case 0:	// SaVic V0.1 Proto
			dwResult = SETBIT(dwResult, VARIATION_ALARM_SWAP_BIT);	// Alarme sind gedreht
			dwResult = CLRBIT(dwResult, VARIATION_VIDEO_SWAP_BIT);
			dwResult = CLRBIT(dwResult, VARIATION_ALARM4_PRESENT);	// Nur 4 Alarmeingänge			
			break;
		case 1:	// SaVic V1.0 Serie null
			dwResult = CLRBIT(dwResult, VARIATION_ALARM_SWAP_BIT);
			dwResult = SETBIT(dwResult, VARIATION_VIDEO_SWAP_BIT);  // Videoeingänge sind gedreht.
			dwResult = SETBIT(dwResult, VARIATION_ALARM4_PRESENT);	// 5 Alarmeingang vorhanden			
			break;
		default:
			dwResult = 0;
			ML_TRACE(_T("Unknown card variation (%d)\n"), (WORD)byBoardVariations);
			break;
	}

	return dwResult;
}

//////////////////////////////////////////////////////////////////////
DWORD CCodec::GetBaseAddress()
{
	DWORD dwIOBase = 0;
	if (m_pDevice)
	{
		dwIOBase = m_pDevice->GetMemBase(0) & 0xfffff000;  // Die Bits 0...11 sind ReadOnly auf 8
		WriteConfigurationInt(_T("System"), _T("MemoryIOBase"), dwIOBase, m_sAppIniName);
		dwIOBase = MapPhysToLin(dwIOBase, 4096);
	}
	
	return dwIOBase;
}

//////////////////////////////////////////////////////////////////////
WORD CCodec::GetIRQNumber() const
{
	BYTE byIRQ = 0;

	if (m_pDevice)
	{
		byIRQ = m_pDevice->GetIRQLine();
		
		if (CheckIRQNumber(byIRQ))
			WriteConfigurationInt(_T("System"), _T("IRQ"), m_pDevice->GetIRQLine(), m_sAppIniName);
		else
			WriteConfigurationInt(_T("System"), _T("IRQ"), 0, m_sAppIniName);
	}
	return (WORD)byIRQ;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoAnalogSwitch(SOURCE_SELECT_STRUCT  &SrcSel)
{
	// Im Interrupt umschalten
	if (!WaitForSourceSwitch(SrcSel))
	{
		if (GetBT878Object())
			GetBT878Object()->SetVideoSource(SrcSel);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::WaitForSourceSwitch(SOURCE_SELECT_STRUCT &SrcSel)
{
	HANDLE	hEvent[2];		  
	BOOL	bTimeOut = FALSE;
	WORD	wCnt	 = 0;

	hEvent[0] = m_hSourceSwitchEvent;
	hEvent[1] = m_hShutDownEvent;

	do
	{
		// Der Interruptroutine den Wunsch des Umschaltens mitteilen
		DARequestSourceSwitch(m_hDevice, SrcSel);
		if (GetEncoderState() == ENCODER_OFF)
			return FALSE;
		else
		{
			switch (WaitForMultipleObjects(2, hEvent, FALSE, 500))
			{
				case WAIT_OBJECT_0:
	//				ML_STAT_PEAK(_T("Reset"), 1, _T("Wait Object 0"));
				case WAIT_OBJECT_0+1:
					break;
				case WAIT_TIMEOUT:
				
					// Switch Request löschen   
					DAClearSourceSwitchRequest(m_hDevice);
					
					bTimeOut = TRUE;
					break;
			}
		}
	}while (bTimeOut && (wCnt++ < 4)); // 5 Versuche

	if (bTimeOut)
		ML_TRACE(_T("WaitForSourceSwitch 5 Timeouts!\n"));		

	return (bTimeOut ? FALSE : TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetCameraToAnalogOut(WORD wSource,  const CString& sPort)
{
//	TRACE(_T("Cam=%d to Port:%s\n"), wSource, sPort);
	BOOL bResult = FALSE;

	if (m_bTraceAnalogOutSwitch)
		ML_TRACE(_T("SetCameraToAnalogOut Source=%d sPort=%s\n"), wSource+1, sPort);

	if (!CheckSource(wSource))
		return FALSE;
	
	if (GetBT878Object())
	{
		// Irgendwo ist die Zuordnung der Videoausgänge gedreht. Dies wird hier korrigiert
		// CSD_PORT0 <- 2. Videoausgang laut Panel
		// CSD_PORT1 <- 1. Videoausgang laut Panel
		if (sPort == CSD_PORT0)
			bResult = GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[wSource].wPortYC, AA2_MUX);
		else if (sPort == CSD_PORT1)
			bResult = GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[wSource].wPortYC, AA1_MUX);
		else
			return FALSE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::Diagnostic(const CString &sSN)
{
	BOOL			bRet		= FALSE;
	
	if (!m_pDevice)
		return FALSE;

	ML_TRACE_DIAGNOSTIC(_T("  VendorID:         0x%04x\n"), m_pDevice->GetVendorID());
	ML_TRACE_DIAGNOSTIC(_T("  DeviceID:         0x%04x\n"), m_pDevice->GetDeviceID());
	ML_TRACE_DIAGNOSTIC(_T("  SubSystemID:      0x%04x\n"), m_pDevice->GetSubsystemID());
	ML_TRACE_DIAGNOSTIC(_T("  SubSystemVendorID:0x%04x\n"), m_pDevice->GetSubsystemVendorID());
	
	ML_TRACE_DIAGNOSTIC(_T(""));
	ML_TRACE_DIAGNOSTIC(_T(""));
	ML_TRACE_DIAGNOSTIC(_T("SaVicVariation: 0x%x\n"), ReadVariationRegister());
	ML_TRACE_DIAGNOSTIC(_T(""));
	
	BOOL bUsingVideoAdapter = ReadConfigurationInt(_T(""), _T("UsingVideoAdapter"), 1, _T("SaVicDiagnostic"));
	BOOL bUsingRelayAdapter = ReadConfigurationInt(_T(""), _T("UsingRelayAdapter"), 1, _T("SaVicDiagnostic"));

	m_bDiagnosticIsRunning = TRUE;
	bRet = TestingPCIBridge();
	bRet = TestingWatchdog() && bRet;
	bRet = TestingRealTimeClock() && bRet;
	bRet = TestingEEProm(sSN) && bRet;
	
	if (!bUsingRelayAdapter)
		bRet = TestingRelais() && bRet;
	else	
		bRet = TestingRelais2() && bRet;		    

	if (bUsingVideoAdapter)
		bRet = TestingSyncSeperators() && bRet;
	else
	{
		ML_TRACE_DIAGNOSTIC(_T("Skipping SyncseperatorTest"));
		ML_TRACE_DIAGNOSTIC(_T(""));
	}
	m_bDiagnosticIsRunning = FALSE;

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingPCIBridge() const
{
	BOOL	bErr = FALSE;
	DWORD	dwTestPattern[] = {0x55555555, 0xaaaaaaaa, 0xffffffff, 0x00000000};
	DWORD	dwBackUp = 0;

	ML_TRACE_DIAGNOSTIC(_T("\nTesting PCI-Bridge "));


	// Zum Test ob der Zugriff auf die PCI-Bridge funktioniert, werden in
	// 3 ausgewählten Registern nacheinander jeweils 4 Testpattern hineingeschrieben
	// und wieder gelesen.
	for (int nJ = 0; nJ < 16; nJ++)
	{
		for (int nI = 0; nI < 4; nI++)
		{
			// Schreiblesetest auf MASKTOPBASE-Register

			// Schreiblesetest auf MASKBOTBASE-Register
		
			// Schreiblesetest auf CODMEMBASE-Register
		
			// Schreiblesetest auf I_STAT_COM-Register
		}
		Sleep(250);
		ML_TRACE_DIAGNOSTIC(_T("@+."));
	}
	
	if (bErr)
		ML_TRACE_DIAGNOSTIC(_T("@-Testing PCI-Bridge: failed"));
	else
		ML_TRACE_DIAGNOSTIC(_T("@-Testing PCI-Bridge: passed"));

	return !bErr;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingRealTimeClock()
{
	CTime time1;
	CTime time2;
	BOOL bResult = FALSE;

	time1 = CTime::GetCurrentTime();

	ML_TRACE_DIAGNOSTIC(_T("\nTesting RealtimeClock"));

	if (SetTime(time1))
	{
		for (int nI = 0; nI < 5; nI++)
		{
			ML_TRACE_DIAGNOSTIC(_T("@+."));
			Sleep(1000);
		}
		
		if (GetTime(time2))
		{
			CTimeSpan span = time2 - time1;

			int nSec = 3600*span.GetTotalHours() + 60*span.GetTotalMinutes() + span.GetTotalSeconds();
			bResult = (abs(nSec-5) <= 1); 
		}
	}

	if (bResult)
		ML_TRACE_DIAGNOSTIC(_T("@-Testing RealtimeClock: passed"));
	else
		ML_TRACE_DIAGNOSTIC(_T("@-Testing RealtimeClock: failed"));

	return bResult;					 
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingEEProm(const CString &sSN)
{
	BOOL bResult = TRUE;
	BYTE bySaved[256];
	BYTE byVal = 0;

	ML_TRACE_DIAGNOSTIC(_T("\nTesting EEProm"));

	WORD	wI = 0;
	DWORD	dwMagicHeader = 0;
	
	if (!m_pPCF8582)
		return FALSE;
	
	m_pPCF8582->ReadMagicHeader(dwMagicHeader);

	// Magic Header Ok?
	if (dwMagicHeader == MAGIC_HEADER)
	{
		// EEProm auslesen und sichern
		for (wI = 0; wI <= 255; wI++)
			ReadFromEEProm(wI, bySaved[wI]);
	}

	// 1. Bitmuster schreiben
	for (wI = 0; wI <= 255; wI++)
	{
		WriteToEEProm(wI, 0xff);
		Sleep(15);
		if ((wI % 32) == 0)
			ML_TRACE_DIAGNOSTIC(_T("@+."));
	}											 

	// 1. Bitmuster wieder auslesen
	for (wI = 0; wI <= 255; wI++)
	{
		ReadFromEEProm(wI, byVal);
		if (byVal != 0xff)
			bResult = FALSE;
	}

	// 2. Bitmuster schreiben
	for (wI = 0; wI <= 255; wI++)
	{
		WriteToEEProm(wI, 0x00);
		Sleep(15);
		if ((wI % 32) == 0)
			ML_TRACE_DIAGNOSTIC(_T("@+."));
	}	     

	// 2. Bitmuster lesen.
	for (wI = 0; wI <= 255; wI++)
	{
		ReadFromEEProm(wI, byVal);
		if (byVal != 0x00)
			bResult = FALSE;
	}

	if (dwMagicHeader == MAGIC_HEADER)
	{
		// Original EEProm-Inhalt wieder herstellen
		for (wI = 0; wI <= 255; wI++)
		{
			WriteToEEProm(wI, bySaved[wI]);
			Sleep(15);
			if ((wI % 32) == 0)
				ML_TRACE_DIAGNOSTIC(_T("@+."));
		}
	}

	// Ergebnis ausgeben
	if (bResult)
		ML_TRACE_DIAGNOSTIC(_T("@-Testing EEProm: passed"));
	else
		ML_TRACE_DIAGNOSTIC(_T("@-Testing EEProm: failed"));

	if (bResult)
	{
		// Sofern kein Fehler erkannt und der Header Ok ist, die
		// Seriennummer ausgeben.
		if (dwMagicHeader == MAGIC_HEADER)
		{
			ML_TRACE_DIAGNOSTIC(_T("EEProm-Header Ok"));
			CString sCurrSN;
			m_pPCF8582->ReadSN(sCurrSN);
			if (sCurrSN == sSN)
				ML_TRACE_DIAGNOSTIC(_T("SN=%s"), sCurrSN);
			else
			{
				ML_TRACE_DIAGNOSTIC(_T("EEProm-SN and new SN don't fit"));
				ML_TRACE_DIAGNOSTIC(_T("EEProm-SN=%s New-SN=%s"), sCurrSN, sSN);
			}
		}
		else
		{
			// Ansonsten Seriennummer und SubSystemID schreiben.
			ML_TRACE_DIAGNOSTIC(_T("Writing SN to EEProm"));
			m_pPCF8582->WriteSN(sSN);
			m_pPCF8582->WriteSubsystemID(SAVIC_SUBSYSTEM_ID1);
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::TestingSyncSeperators()
{
	BOOL bSignal1	= FALSE;
	BOOL bSignal2	= FALSE;

	ML_TRACE_DIAGNOSTIC(_T("\nTesting Syncseperator"));

	// -------------------------------------------------------
	// Den Syncseperator MUX auf den ersten Eingang schalten. Hier sollte ein aufgrund des
	// Testadapteraufbaues EIN Videosignal detektiert werden.
	// signal vorhanden sein
	ML_TRACE(_T("m_Codec.Source[0].wPortYC=%d\n"), m_Codec.Source[0].wPortYC);
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[0].wPortYC, VS_MUX);
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[0].wPortYC, AA1_MUX);
	WK_Sleep(100); // Nach der Umschaltung muß gewartet werden!!

	// 250 ms lang auf eine fallende oder steigende Flanke pollen
	BOOL b0 = GetBT878Object()->ReadGPIOPin(BT878_GPIO_VS_ACTIVE_BIT);
	BOOL b1 = 0;

	DWORD dwCnt = 0;
	do						     
	{
		WK_Sleep(11); // Nach der Umschaltung muß gewartet werden!!
		b1 = GetBT878Object()->ReadGPIOPin(BT878_GPIO_VS_ACTIVE_BIT);
		ML_TRACE_DIAGNOSTIC(_T("@+."));

	}while ((b0 == b1) && (dwCnt++ < 25));

	// Flanke?
	if ((b0 != b1))
		bSignal1 = TRUE; // Signal vorhanden
	else		   
		bSignal1 = FALSE; // Kein Signal vorhanden.

	// -------------------------------------------------------
	// Den Syncseperator MUX auf den zweiten Eingang schalten.
	// Den Videoausgang von MUX1 auf den 2. Ausgang schalten
	// Hier sollte ein aufgrund des	Testadapteraufbaues KEIN Videosignal detektiert werden.
	// signal vorhanden sein
	ML_TRACE(_T("m_Codec.Source[1].wPortYC=%d\n"), m_Codec.Source[1].wPortYC);
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[1].wPortYC, VS_MUX);
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[1].wPortYC, AA1_MUX);

	WK_Sleep(100); // Nach der Umschaltung muß gewartet werden!!

	// 250 ms lang auf eine fallende oder steigende Flanke pollen
	b0 = GetBT878Object()->ReadGPIOPin(BT878_GPIO_VS_ACTIVE_BIT);
	b1 = 0;

	dwCnt = 0;
	do
	{
		WK_Sleep(11); // Nach der Umschaltung muß gewartet werden!!
		b1 = GetBT878Object()->ReadGPIOPin(BT878_GPIO_VS_ACTIVE_BIT);
		ML_TRACE_DIAGNOSTIC(_T("@+."));
	}while ((b0 == b1) && (dwCnt++ < 25));

	// Flanke?
	if ((b0 != b1))
		bSignal2 = TRUE; // Signal vorhanden
	else		   
		bSignal2 = FALSE; // Kein Signal vorhanden.

	// Ergebnis ausgeben
	if (bSignal1 && !bSignal2)
		ML_TRACE_DIAGNOSTIC(_T("@-Testing Synseperator: passed"));
	else
	{
		if (!bSignal1)
		{
			ML_TRACE_DIAGNOSTIC(_T("@-Testing Synseperator (With Signal): failed"));
			ML_TRACE_DIAGNOSTIC(_T("  (No Videosignal detected)"));
		}
		if (bSignal2)
		{
			ML_TRACE_DIAGNOSTIC(_T("@-Testing Synseperator (Without Signal): failed"));
			ML_TRACE_DIAGNOSTIC(_T("  (Videosignal detected)"));
		}
	}	
	
	// Die MUXer wieder auf ihren Uhrsprungszustand setzen
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[0].wPortYC, VS_MUX);
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[0].wPortYC, AA1_MUX);
	GetBT878Object()->SetCameraToAnalogOut(m_Codec.Source[0].wPortYC, AA2_MUX);
	
	return (bSignal1 && !bSignal2);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::GetTime(CTime& Time) const
{
	BOOL bResult = FALSE;
	Time = CTime::GetCurrentTime();

	if (m_pPCF8563)
		bResult = m_pPCF8563->GetTime(Time);

	return	bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SetTime(const CTime &time)
{
	if (!m_pPCF8563)
		return FALSE;

	return m_pPCF8563->SetTime(time);
}


/////////////////////////////////////////////////////////////////////
BOOL CCodec::WriteToEEProm(WORD wAddr, BYTE byVal)
{
	if (!m_pPCF8582)
		return FALSE;

	return m_pPCF8582->WriteToEEProm(wAddr, byVal);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::ReadFromEEProm(WORD wAddr, BYTE& byVal)
{
	if (!m_pPCF8582)
		return FALSE;

	return m_pPCF8582->ReadFromEEProm(wAddr, byVal);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::GetPowerOffButtonState() const
{
	BOOL bState = FALSE;

	if (GetBT878Object())
		bState = !GetBT878Object()->ReadGPIOPin(BT878_GPIO_POWEROFF_BIT);

	return bState;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::GetResetButtonState() const
{
	BOOL bState = FALSE;

	if (GetBT878Object())
		bState = !GetBT878Object()->ReadGPIOPin(BT878_GPIO_RESET_BIT);

	return bState;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetLEDState(eLEDState ledState)
{
	BOOL bResult = FALSE;
	if (GetBT878Object())
	{
		switch (ledState)
		{
			case eLED1_On:
				GetBT878Object()->SetGPIOPin(BT878_GPIO_LED1_BIT);
				bResult = TRUE;
				break;
			case eLED1_Blink:
				GetBT878Object()->ClrGPIOPin(BT878_GPIO_LED1_BIT);
				bResult = TRUE;
				break;
			case eLED2_On:
				GetBT878Object()->SetGPIOPin(BT878_GPIO_LED2_BIT);
				bResult = TRUE;
				break;
			case eLED2_Off:
				GetBT878Object()->ClrGPIOPin(BT878_GPIO_LED2_BIT);
				bResult = TRUE;
				break;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::MotionDetectionInit()
{
	BOOL bResult = FALSE;

	if (GetBT878Object())
		bResult = GetBT878Object()->MotionDetectionInit();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::VideoInlayInit()
{
	BOOL bResult = FALSE;

	if (GetBT878Object())
		bResult = GetBT878Object()->VideoInlayInit();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
IMAGE* CCodec::CompressImageToJpeg(CAPTURE_STRUCT& Capture)
{
	int nDstWidth			= 0;
	int nDstHeight			= 0;

	if (CheckSource(Capture.wSource))
	{
		// Die Qualität entsprechend der gewünschten Dateigröße anpassen.
		m_Jpeg.SetQuality(m_nQuality[Capture.wSource]);

		// RES_LOW bedeutet halbe horizontale Auflösung.
		if (m_wFormat == ENCODER_RES_LOW)
		{
			nDstWidth	= Capture.wSizeH / 2;
			nDstHeight	= Capture.wSizeV;;
		}
		else
		{
			nDstWidth	= Capture.wSizeH;
			nDstHeight	= Capture.wSizeV;
		}

		m_Image.dwImageLen = FIELD_BUFFER_SIZE;
		if (m_Jpeg.EncodeJpegToMemory(Capture.wSizeH, Capture.wSizeV, nDstWidth, nDstHeight, Capture.wSourceType, (BYTE*)Capture.Buffer.pLinAddr, m_Image.pImageData, m_Image.dwImageLen))
		{
			m_Image.bValid			= Capture.bValid;
			m_Image.TimeStamp		= Capture.TimeStamp;
			m_Image.dwFieldID		= Capture.dwFieldID;
			m_Image.dwProzessID		= Capture.dwProzessID;
			m_Image.wSource			= Capture.wSource;
			m_Image.wSizeH			= Capture.wSizeH;
			m_Image.wSizeV			= Capture.wSizeV;
			m_Image.wCompressionType= COMPRESSION_JPEG;

			// Qualität anpassen
			if (m_Image.dwImageLen > 1.05*(float)m_dwBPF)
				m_nQuality[Capture.wSource] -= 1;
			else if (m_Image.dwImageLen < 0.95*(float)m_dwBPF)
				m_nQuality[Capture.wSource] += 1;

			m_nQuality[Capture.wSource] = max(min(m_nQuality[Capture.wSource], 90),10);
		}
		else
		{
			ML_TRACE_ERROR(_T("CCodec::CompressImageToJpeg: EncodeJpegToMemory failed\n"));
			return NULL;
		}
	}
	else
		ML_TRACE_ERROR(_T("CCodec::CompressImageToJpeg unknown Videoinput (%d)\n"), Capture.wSource);

	return &m_Image;
}

/////////////////////////////////////////////////////////////////////////////
IMAGE* CCodec::CreateDIB(CAPTURE_STRUCT& Capture)
{
	DWORD   dwImageLen		= 0;
	int	nSrcWidth			= Capture.wSizeH;
	int nSrcHeight			= Capture.wSizeV;
	int nDstWidth			= nSrcWidth;
	int nDstHeight			= nSrcHeight;
	YUV422* pDIBData		= (YUV422*)Capture.Buffer.pLinAddr;

	if (CheckSource(Capture.wSource))
	{
		// RES_LOW bedeutet halbe horizontale Auflösung. Dann muß skaliert werden.
		if (m_wFormat == ENCODER_RES_LOW)
		{
			nDstWidth /= 2;
			ScaleDownYUV422(CSize(nSrcWidth, nSrcHeight), CSize(nDstWidth, nDstHeight), pDIBData);
		}

		dwImageLen		= Capture.dwLen; //2 * nDstWidth*nDstHeight;

		LPBITMAPINFO lpBMI = (LPBITMAPINFO)m_Image.pImageData;
		if (lpBMI)
		{
			int nS = sizeof(BITMAPINFO);
			memset(lpBMI, 0, sizeof(BITMAPINFO));
			lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
			lpBMI->bmiHeader.biWidth		= nDstWidth;
			lpBMI->bmiHeader.biHeight		= nDstHeight;
			lpBMI->bmiHeader.biPlanes		= 1;
			lpBMI->bmiHeader.biBitCount		= 16; 
			lpBMI->bmiHeader.biCompression	= mmioFOURCC('Y','4','2','2');

			lpBMI->bmiHeader.biSizeImage	= dwImageLen;

			if (m_pPerf3)
				m_pPerf3->Start();
			
			CMemCopy cpy;
			cpy.FastCopy(lpBMI->bmiColors, pDIBData, dwImageLen);

			if (m_pPerf3)
				m_pPerf3->Stop();

			m_Image.bValid				= Capture.bValid;
			m_Image.TimeStamp			= Capture.TimeStamp;
			m_Image.dwFieldID			= Capture.dwFieldID;
			m_Image.pImageData			= (BYTE*)lpBMI;
			m_Image.dwImageLen			= dwImageLen;
			m_Image.dwProzessID			= Capture.dwProzessID;
			m_Image.wSource				= Capture.wSource;
			m_Image.wSizeH				= nDstWidth;
			m_Image.wSizeV				= nDstHeight;
			m_Image.wCompressionType	= COMPRESSION_YUV_422;

		}
		else
		{
			ML_TRACE_ERROR(_T("CMotionDetection::CreateDIB GlobalAlloc failed\n")); 
			return NULL;
		}
	}
	
	return &m_Image;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::ScaleDown(const CSize& siSrc, const CSize& siDst, BGR24* pSrc)
{
	if (m_pPerf2)
		m_pPerf2->Start();

	BGR24* pDst = new BGR24[siDst.cx * siDst.cy];

	if (!pSrc || !pDst)
		return FALSE;

	// Scale down: 
	int		nVScale	= siSrc.cx * siSrc.cy / siDst.cy;
	int		nHScale = (int)((1<<16)*((float)siSrc.cx / (float)siDst.cx));
	int		nSrcIndex[768];

	// Indextabelle erzeugen
	for (int nX=0; nX<siDst.cx; nX++)
		nSrcIndex[nX] = (int)(nX * nHScale) / (1<<16);

	for (int nY=0; nY<siDst.cy; nY++)
	{
		BGR24* pSrcBase = pSrc + nY * nVScale;
		BGR24* pDstBase = pDst + nY * siDst.cx;
		for (int nX=0; nX<siDst.cx; nX++)
		{	
			pDstBase[nX].bB = pSrcBase[nSrcIndex[nX]].bB;
			pDstBase[nX].bR = pSrcBase[nSrcIndex[nX]].bR;
			pDstBase[nX].bG = pSrcBase[nSrcIndex[nX]].bG;
		}
	}

	CMemCopy cpy;
	cpy.FastCopy(pSrc, pDst, siDst.cx * siDst.cy * sizeof(BGR24));
	
	WK_DELETE(pDst);

	if (m_pPerf2)
		m_pPerf2->Stop();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!
BOOL CCodec::ScaleDownYUV422(const CSize& siSrc, const CSize& siDst, YUV422* pSrc)
{
	if (m_pPerf1)
		m_pPerf1->Start();

	YUV422* pDst = (YUV422*)new BYTE[siDst.cx * siDst.cy * 2];

	if (!pSrc || !pDst)
		return FALSE;

	int 	nHScale = (int)((1<<16)*((float)siSrc.cx / (float)siDst.cx));
	int		nVScale	= siSrc.cx * siSrc.cy / siDst.cy;
	int		nSrcIndex[768];

	// Indextabelle erzeugen
	for (int nX=0; nX<siDst.cx; nX++)
		nSrcIndex[nX] = (int)(nX * nHScale) / (1<<16);

	for (int nY=0; nY<siDst.cy; nY++)
	{
		YUV422* pSrcBase = pSrc + nY * nVScale / 2;
		YUV422* pDstBase = pDst + nY * siDst.cx / 2;
//		BLOCK_PREFETCH_4K(pSrcBase);
		for (int nX=0; nX<siDst.cx/2; nX++)
		{	
			pDstBase[nX].bY1 = pSrcBase[nSrcIndex[nX]].bY1;
			pDstBase[nX].bY2 = pSrcBase[nSrcIndex[nX]].bY2;
		 	pDstBase[nX].bU  = pSrcBase[nSrcIndex[nX]].bU;
			pDstBase[nX].bV  = pSrcBase[nSrcIndex[nX]].bV;
		}
	}

	CMemCopy cpy;
	cpy.FastCopy(pSrc, pDst, siDst.cx * siDst.cy * 2);

	WK_DELETE(pDst);

	if (m_pPerf1)
		m_pPerf1->Stop();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::HandleScaler(int nH, int nV)
{
	if (!IsValid())
		return FALSE;

	// Das Bild skalieren.
	m_pBT878->SetHActiveE(nH);
	m_pBT878->SetHActiveO(nH);
	if (m_pBT878->GetVideoFormat() == VFMT_PAL_CCIR)
	{
		m_pBT878->SetHScaleE((int)(((922.0/(float)nH) - 1.0) * 4096));
		m_pBT878->SetHScaleO((int)(((922.0/(float)nH) - 1.0) * 4096));
		m_pBT878->SetHDelayE((WORD)(186.0/922.0*(float)nH) & 0x3FE);
		m_pBT878->SetHDelayO((WORD)(186.0/922.0*(float)nH) & 0x3FE);
		m_pBT878->SetVScaleE((WORD)((0x10000 - (576.0/(float)nV-1.0)*512.0)) & 0x1fff);
		m_pBT878->SetVScaleO((WORD)((0x10000 - (576.0/(float)nV-1.0)*512.0)) & 0x1fff);
	}
	else if (m_pBT878->GetVideoFormat() == VFMT_NTSC_CCIR)
	{
		m_pBT878->SetHScaleE((int)(((754.0/(float)nH) - 1.0) * 4096));
		m_pBT878->SetHScaleO((int)(((754.0/(float)nH) - 1.0) * 4096));
		m_pBT878->SetHDelayE((WORD)(135.0/754.0*(float)nH) & 0x3FE);
		m_pBT878->SetHDelayO((WORD)(135.0/754.0*(float)nH) & 0x3FE);
		m_pBT878->SetVScaleE((WORD)((0x10000 - (480.0/(float)nV-1.0)*512.0)) & 0x1fff);
		m_pBT878->SetVScaleO((WORD)((0x10000 - (480.0/(float)nV-1.0)*512.0)) & 0x1fff);
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SetFramerate(WORD wFps)
{
#if (1)
	BOOL bResult = FALSE;
	CBT878* pBT878 = GetBT878Object();
	if (pBT878 && (wFps <=25))
	{
		pBT878->BT878Out(BT878_TDEC, 0x00);
		pBT878->BT878Out(BT878_TDEC, 0x00 | 2*(max(25-wFps, 0)));
		
		bResult = TRUE;
	}

	return bResult;
#else
	BOOL bResult = FALSE;
	CBT878* pBT878 = GetBT878Object();
	if (pBT878 && (wFps <=50))
	{
		pBT878->BT878Out(BT878_TDEC, 0x00);
		pBT878->BT878Out(BT878_TDEC, 0x80 | 1*(max(50-wFps, 0)));
		
		bResult = TRUE;
	}

	return bResult;
#endif
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::GetFramerate(WORD& wFps)
{
	BOOL bResult = FALSE;
	CBT878* pBT878 = GetBT878Object();
	if (pBT878)
	{
		WORD wVal = 0;
		if (pBT878->BT878In(BT878_TDEC, wVal))
		{
			wFps = min(max(25 - (wVal / 2), 0), 25);
			bResult = TRUE;
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::InitPll()
{
	BOOL bResult = FALSE;
	
	if (GetBT878Object())
	{
		GetBT878Object()->InitPLL();
		bResult = TRUE;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::WriteSubsystemID(WORD wSubsystemID)
{
	BOOL bResult = FALSE;

	if (m_pPCF8582)
		bResult = m_pPCF8582->WriteSubsystemID(wSubsystemID);		

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::ReadSubsystemID(WORD& wSubsystemID)
{
	BOOL bResult = FALSE;

	if (m_pPCF8582)
		bResult = m_pPCF8582->ReadSubsystemID(wSubsystemID);		

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::ClearEEProm()
{
	BOOL bResult = FALSE;

	if (m_pPCF8582)
		bResult = m_pPCF8582->ClearEEProm();		

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::WriteSN(const CString &sSN)
{
	BOOL bResult = FALSE;

	if (m_pPCF8582)
		bResult = m_pPCF8582->WriteSN(sSN);		

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::ReadSN(CString &sSN)
{
	BOOL bResult = FALSE;

	if (m_pPCF8582)
		bResult = m_pPCF8582->ReadSN(sSN);		

	return bResult;
}
