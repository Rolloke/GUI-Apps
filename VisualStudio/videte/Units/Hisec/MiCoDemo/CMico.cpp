/////////////////////////////////////////////////////////////////////////////
//	Project:	MiCoISA
//
//	File:		cmico.cpp -implementation file
//
//	Author:		Martin Lück
//
//	Copyright:	w+k Video Communication GmbH & Co.KG
//
/////////////////////////////////////////////////////////////////////////////

/*////////////////////////////////////////////////////////////////////////////
@TOPIC{CMico Overview|Overview,CMiCo}
@RELATED @LINK{members|CMiCo},

Die Klasse CMiCo ermöglicht den einfachen Zugriff auf die komplette MiCo-Hardware.
Dazu bietet sie einen umfangreichen Satz von einfach einzusetzende Memberfunktionen.

@MLIST @RELATED @LINK{Overview|Overview,CMiCo}
*////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mmsystem.h"
#include "DlgColor.h"
#include "MicoReg.h"
#include "CMiCo.h"
#include "CRelay.h"
#include "CAlarm.h"
#include "CBT829.h"
#include "CBT856.h"
#include "CZoran.h"
#include "resource.h"

// Buffersize
#define SIZE 255 		

UINT AlarmPollThread(LPVOID pData)
{
	CMiCo*	pMiCo	 = (CMiCo*)pData;  // Pointer auf's Mico-Objekt

	if (!pMiCo)
	{
		WK_TRACE_ERROR("AlarmPollThread\tpMiCo = NULL\n");
		return 0;
	}

	// Warte bis MainProzeß initialisiert ist.
	while (!pMiCo->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pMiCo->m_bRun)
	{
		Sleep(250);
		pMiCo->PollAlarm();
	}

	WK_TRACE("ExitAlarmPollThread\n");
	return 0;
}

/*/////////////////////////////////////////////////////////////////////////////
@MHEAD{constructor:}
@MD 
@CW{Parameter:}
	@CW{hWnd=}
	Fensterhandle der Applikation. An dieses Fenster verschickt der Treiber
	Meldungen, wenn nötig.	
	@CW{sAppIniName=}
	Name der Ini-Datei, die zur Konfiguration des Mico-Boards notwendig ist.
@HRULE
@CW{Rückgabe:}
	Keine
@HRULE
@CW{Funktion:}
	Das Anlegen eines MiCo-Objektes bewirkt, die komplette Initialisierung der
	Micohardware. Dabei werden die Einstellung aus der durch sAppIniName spezifizierten
	Ini-Datei gelesen und das Videofrontend/backend sowie der Zoran initialisiert.
	Im Konstruktor werden einige Hilfsobjekte angelegt, über die die einzelnen
	Komponenten auf der MiCo angesprochen werden. (Relays, Alarm, BT829, BT856,
	PCT8584, Zoran)
@HRULE
@RELATED
	@ITEM@MLINK{CMiCo::~CMiCo}
*/////////////////////////////////////////////////////////////////////////////
CMiCo::CMiCo(HWND hWnd, const CString &sAppIniName)
{
	m_csMico.Lock();
	m_bOK					= FALSE;

	// Default-Initialisierung
	m_bRun					= FALSE;
	m_hWndClient			= hWnd;
	m_wLastError			= MICO_ERR_NO_ERRROR;
	m_pDlgColor				= NULL;
	m_wSource				= MICO_SOURCE_NO;
	m_wExtCard				= MICO_EXTCARD_NO;
	m_wFrontEnd				= MICO_FE0;
	m_wBeVideoType			= MICO_COMPOSITE;
	m_wFormat				= MICO_ENCODER_HIGH;
	m_wFramerate			= 25;
	m_dwBPF					= 20000;
	m_dwHardwareState		= 0;
	m_pBT829[MICO_FE0]		= NULL;
	m_pBT829[MICO_FE1]		= NULL;
	m_pBT856				= NULL;
	m_pZoran				= NULL;
	m_pAlarmPollThread		= NULL;
	m_byMicoID				= 0;	//	0x22 -> 58MHz, 0x30 -> 59MHz
	m_sAppIniName			= sAppIniName;

	// Informationen der zu protokollierenden Funktionen einlesen
	m_bTraceStartStopEncoder = (BOOL)GetPrivateProfileInt("TRACE", "StartStopEncoder",  	0, "MICOUNIT.INI");
	m_bTraceStartStopDecoder = (BOOL)GetPrivateProfileInt("TRACE", "StartStopDecoder",  	0, "MICOUNIT.INI");
	m_bTraceSetBPF			 = (BOOL)GetPrivateProfileInt("TRACE", "SetBPF",  				0, "MICOUNIT.INI");
	m_bTraceSetFormat		 = (BOOL)GetPrivateProfileInt("TRACE", "SetFormat",				0, "MICOUNIT.INI");
	m_bTraceSetFeSourceType	 = (BOOL)GetPrivateProfileInt("TRACE", "SetFeSourceType", 		0, "MICOUNIT.INI");
	m_bTraceSetFeVideoFormat = (BOOL)GetPrivateProfileInt("TRACE", "SetFeVideoFormat", 		0, "MICOUNIT.INI");
	m_bTraceSetBeVideoType	 = (BOOL)GetPrivateProfileInt("TRACE", "SetBeVideoType", 		0, "MICOUNIT.INI");
	m_bTraceSetBeVideoFormat = (BOOL)GetPrivateProfileInt("TRACE", "SetBeVideoFormat", 		0, "MICOUNIT.INI");
	m_bTraceSetFeBrightness	 = (BOOL)GetPrivateProfileInt("TRACE", "SetFeBrightness", 		0, "MICOUNIT.INI");
	m_bTraceSetFeContrast	 = (BOOL)GetPrivateProfileInt("TRACE", "SetFeContrast", 		0, "MICOUNIT.INI");
	m_bTraceSetFeSaturation	 = (BOOL)GetPrivateProfileInt("TRACE", "SetFeSaturation", 		0, "MICOUNIT.INI");
	m_bTraceSetFeHue		 = (BOOL)GetPrivateProfileInt("TRACE", "SetFeHue", 				0, "MICOUNIT.INI");
	m_bTraceSetFeFilter		 = (BOOL)GetPrivateProfileInt("TRACE", "SetFeFilter", 			0, "MICOUNIT.INI");
	m_bTraceSetInputSource	 = (BOOL)GetPrivateProfileInt("TRACE", "SetInputSource", 		0, "MICOUNIT.INI");
	m_bTraceSetRelais		 = (BOOL)GetPrivateProfileInt("TRACE", "SetRelais", 			0, "MICOUNIT.INI");
	m_bTraceSetAlarmEdge	 = (BOOL)GetPrivateProfileInt("TRACE", "SetAlarmEdge",			0, "MICOUNIT.INI");
	m_bTraceSetAlarmAck		 = (BOOL)GetPrivateProfileInt("TRACE", "SetAlarmAck",			0, "MICOUNIT.INI");
	m_bTraceAlarmIndication	 = (BOOL)GetPrivateProfileInt("TRACE", "AlarmIndication",		0, "MICOUNIT.INI");

	// Und im Logfile protokollieren
	WK_TRACE("TraceStart/StopEncoder=%u\n", m_bTraceStartStopEncoder);
	WK_TRACE("TraceStart/StopDecoder=%u\n", m_bTraceStartStopDecoder);
	WK_TRACE("TraceSetBPF=%u\n",			m_bTraceSetBPF);
	WK_TRACE("TraceSetFormat=%u\n",			m_bTraceSetFormat);
	WK_TRACE("TraceSetFeSourceType=%u\n",	m_bTraceSetFeSourceType);
	WK_TRACE("TraceSetFeVideoFormat=%u\n",	m_bTraceSetFeVideoFormat);
	WK_TRACE("TraceSetBeVideoType=%u\n",	m_bTraceSetBeVideoType);
	WK_TRACE("TraceSetBeVideoFormat=%u\n",	m_bTraceSetBeVideoFormat);
	WK_TRACE("TraceSetFeBrighness=%u\n",	m_bTraceSetFeBrightness);
	WK_TRACE("TraceSetFeContrast=%u\n",		m_bTraceSetFeContrast);
	WK_TRACE("TraceSetFeSaturation=%u\n",	m_bTraceSetFeSaturation);
	WK_TRACE("TraceSetFeHue=%u\n",			m_bTraceSetFeHue);
	WK_TRACE("TraceSetFeFilter=%u\n",		m_bTraceSetFeFilter);
	WK_TRACE("TraceSetInputSource=%u\n",	m_bTraceSetInputSource);
	WK_TRACE("TraceSetRelais=%u\n",			m_bTraceSetRelais);
	WK_TRACE("TraceSetAlarmEdge=%u\n",		m_bTraceSetAlarmEdge);
	WK_TRACE("TraceSetAlarmAck=%u\n",		m_bTraceSetAlarmAck);
	WK_TRACE("TraceAlarmIndication=%u\n",	m_bTraceAlarmIndication);

	// Parameter initialisieren.
	for (WORD wCrd = MICO_EXTCARD0; wCrd <= MICO_EXTCARD0   + MICO_MAX_EXTCARD; wCrd++)
	{
		m_byAlarmState[wCrd] = 0;
		m_pRelay[wCrd] = NULL;
		m_pAlarm[wCrd] = NULL;

		for (WORD wSrc = MICO_SOURCE0; wSrc <= MICO_SOURCE0 + MICO_MAX_SOURCE; wSrc++)
		{
			m_MiCoISA.ExtCard[wCrd].Source[wSrc].nAVPortY		= wSrc - MICO_SOURCE0;
			m_MiCoISA.ExtCard[wCrd].Source[wSrc].nAVPortC		= wSrc - MICO_SOURCE0;
			m_MiCoISA.ExtCard[wCrd].Source[wSrc].wFeSourceType	= MICO_FBAS;
			m_MiCoISA.ExtCard[wCrd].Source[wSrc].bPresent		= FALSE;	
		}
	}

	// Ohne INI-Datei geht gar nichts
	if (sAppIniName.IsEmpty())
	{
		WK_TRACE_WARNING("Keine Ini-Datei angegeben\n");
		m_csMico.Unlock();
		return;
	}

	// Pfad und Name der INI-Datei sichern
	m_sAppIniName = sAppIniName;

	// Configstruktur initialisieren
	m_Config.wIOBase[MICO_EXTCARD0] = GetPrivateProfileInt("MICOISA", "MiCoIOBase",		0x00, m_sAppIniName);
	m_Config.wIOBase[MICO_EXTCARD1] = GetPrivateProfileInt("MICOISA", "ExtCard1IOBase", 0x00, m_sAppIniName);
	m_Config.wIOBase[MICO_EXTCARD2] = GetPrivateProfileInt("MICOISA", "ExtCard2IOBase", 0x00, m_sAppIniName);
	m_Config.wIOBase[MICO_EXTCARD3] = GetPrivateProfileInt("MICOISA", "ExtCard3IOBase", 0x00, m_sAppIniName);
	m_Config.wIRQ			= GetPrivateProfileInt("MICOISA", "IRQ",  0x10, m_sAppIniName); 
	m_Config.hWnd			= hWnd;
	m_Config.wBPF			= 20000;
	m_Config.wMinLen		= 0;
	m_Config.wMaxLen		= 0;
	m_Config.wSkipFields	= 0;
	m_Config.bHardwareSkip	= FALSE;
	m_Config.bSwitchDelay	= TRUE;
	m_Config.wField			= EVEN_FIELD;
	m_Config.dwReqProzessID	= 0;
	m_Config.wReqSource		= MICO_SOURCE0;
	m_Config.wReqExtCard	= MICO_EXTCARD0;
	m_Config.dwProzessID	= 0;
	m_Config.wSource		= MICO_SOURCE0;
	m_Config.wExtCard		= MICO_EXTCARD0;
	m_Config.bSwitch		= FALSE;
	m_Config.hEvent			= NULL;
	m_Config.wCSVDIN		= 0;

	// Jpegstruktur initialisieren
	for (int nI = 0; nI	< MAX_JPEG_BUFFER; nI++)
	{
		m_Config.Jpeg[nI].dwProzessID	= 0;			// ProzeßID = 0;
		m_Config.Jpeg[nI].wSource		= 0;			// Videoquelle zu den die Bilddaten gehören
		m_Config.Jpeg[nI].wExtCard		= 0;			// CoVi zu den die Bilddaten gehören
		m_Config.Jpeg[nI].lpBuffer		= NULL;			// Adresse an der die Bilddaten liegen
		m_Config.Jpeg[nI].dwLen			= 0;			// Länge der Bilddaten.
		m_Config.Jpeg[nI].dwTime		= 0;			// YYYYYYMMMMDDDDDhhhhhmmmmmmssssss
		m_Config.Jpeg[nI].dwFieldID		= 0;			// Jedes Bild hat seine Nummer
		m_Config.Jpeg[nI].wField		= EVEN_FIELD;	// Even Field oder Odd Field
		m_Config.Jpeg[nI].bValid		= FALSE;		// TRUE -> Field valid
		m_Config.Jpeg[nI].byFCNT		= 0;			// For debug only
		m_Config.Jpeg[nI].byPage		= 0;			// For debug only
	}

	// Und ab ins Logfile
	WK_TRACE("Mico  IRQ=%u\n", m_Config.wIRQ);
	WK_TRACE("Mico  IOBase=0x%x\n", m_Config.wIOBase[MICO_EXTCARD0]);
	WK_TRACE("CoVi0 IOBase=0x%x\n", m_Config.wIOBase[MICO_EXTCARD1]);
	WK_TRACE("CoVi1 IOBase=0x%x\n", m_Config.wIOBase[MICO_EXTCARD2]);
	WK_TRACE("CoVi2 IOBase=0x%x\n", m_Config.wIOBase[MICO_EXTCARD3]);

	// Standardwerte für sychrone und asynchrone Kameras.
	if (GetPrivateProfileInt("MICOISA", "CameraType",   -1, m_sAppIniName) == 0)
	{
		WK_TRACE("CameraType = Async\n");
		// Anzahl der Halbbilder die nach der Umschaltung mindestens verworfen werden 
		m_Config.wSkipFields	= 1;	// Mindestens ein Halbbild verwerfen   

		// Hardwareskip ein/aus
		m_Config.bHardwareSkip	= 0;	// Hardwareskip aus	

		// Zusätlicher Delay von 20ms zwischen analoger und digitaler Umschaltung
		m_Config.bSwitchDelay	= 1;	// Umschaltverzögerung ein

	}
	else if (GetPrivateProfileInt("MICOISA", "CameraType",   -1, m_sAppIniName) == 1)
	{
		WK_TRACE("CameraType = Sync\n");

		// Anzahl der Halbbilder die nach der Umschaltung mindestens verworfen werden 
		m_Config.wSkipFields	= 1;	// Kein Halbbild verwerfen   

		// Hardwareskip ein/aus
		m_Config.bHardwareSkip	= 0;	// Hardwareskip aus	

		// Zusätlicher Delay von 20ms zwischen analoger und digitaler Umschaltung
		m_Config.bSwitchDelay	= 0;	// Umschaltverzögerung aus
	}
	else
 		WK_TRACE("No Cameratyp specified\n");

	// Anzahl der Halbbilder die nach der Umschaltung mindestens verworfen werden 
	if (GetPrivateProfileInt("MICOISA", "SkipFields",  -1, m_sAppIniName) != -1)
		m_Config.wSkipFields   = GetPrivateProfileInt("MICOISA", "SkipFields",  0, m_sAppIniName);

	// Hardwareskip ein/aus
	if (GetPrivateProfileInt("MICOISA", "HardwareSkip", -1, m_sAppIniName) != -1)
		m_Config.bHardwareSkip = (BOOL)GetPrivateProfileInt("MICOISA", "HardwareSkip", 0, m_sAppIniName);

	// Zusätlicher Delay von 20ms zwischen analoger und digitaler Umschaltung
	if (GetPrivateProfileInt("MICOISA", "SwitchDelay",  -1, m_sAppIniName) != -1)
		m_Config.bSwitchDelay = GetPrivateProfileInt("MICOISA", "SwitchDelay",  1, m_sAppIniName);

	// Legt fest welches Field gespeichert werden soll
	if (GetPrivateProfileInt("MICOISA", "StoreField",  -1, m_sAppIniName) != -1)
		m_Config.wField = GetPrivateProfileInt("MICOISA", "StoreField",  0, m_sAppIniName);
	
	// Einstellungen protokollieren
	WK_TRACE("SkipFields=%u\n", m_Config.wSkipFields);
	if (m_Config.bHardwareSkip)
		WK_TRACE("Hardwareskip on\n");
	else
		WK_TRACE("Hardwareskip off\n");
	if (m_Config.bSwitchDelay)
		WK_TRACE("Switchdelay on\n");
	else
		WK_TRACE("Switchdelay off\n");
	if (m_Config.wField == 0)
		WK_TRACE("Store even fields\n");
	else if (m_Config.wField == 1)
		WK_TRACE("Store odd fields\n");
	else
		WK_TRACE("Store both fields\n");

	// Identifier der Mico auslesen: 0x22 -> 58MHz (Alt), 0x30 -> 59MHz (Neu)
	m_byMicoID = m_Io.Input(m_Config.wIOBase[MICO_EXTCARD0] | MICO_ID_OFFSET);

	// Ist das Relayregister Readbackfähig?
	BYTE bReg = m_Io.Input(m_Config.wIOBase[MICO_EXTCARD0] | RELAY_OFFSET);
	bReg = SETBIT(bReg, SPARE6_BIT);
	m_Io.Output(m_Config.wIOBase[MICO_EXTCARD0] | RELAY_OFFSET, bReg);
	bReg = m_Io.Input(m_Config.wIOBase[MICO_EXTCARD0] | RELAY_OFFSET);
	if (!TSTBIT(bReg, SPARE6_BIT)) 
	{
	 	AfxMessageBox(IDP_OLD_MICO, MB_ICONSTOP);
		WK_TRACE_WARNING("MiCo Hardware alt!!! (ID=0x%x)\n", m_byMicoID);
		return;
	}

	// Hardwarestatusbits anhand der Basisadresseneinträge setzen
	if (m_Config.wIOBase[MICO_EXTCARD0] != 0)
		m_dwHardwareState = SETBIT(m_dwHardwareState, 0);
	if (m_Config.wIOBase[MICO_EXTCARD1] != 0)
		m_dwHardwareState = SETBIT(m_dwHardwareState, 1);
	if (m_Config.wIOBase[MICO_EXTCARD2] != 0)
		m_dwHardwareState = SETBIT(m_dwHardwareState, 2);
	if (m_Config.wIOBase[MICO_EXTCARD3] != 0)
		m_dwHardwareState = SETBIT(m_dwHardwareState, 3);

	// Das Videointerface des MiCo resetn
	ResetVideoInterface();

	// Jede Erweiterungskarte bekommt ein Relayobjekt
	m_pRelay[MICO_EXTCARD0] = (CRelay*)new CRelay(m_Config.wIOBase[MICO_EXTCARD0], MICO_EXTCARD0);
	m_pRelay[MICO_EXTCARD1] = (CRelay*)new CRelay(m_Config.wIOBase[MICO_EXTCARD1], MICO_EXTCARD1);
	m_pRelay[MICO_EXTCARD2] = (CRelay*)new CRelay(m_Config.wIOBase[MICO_EXTCARD2], MICO_EXTCARD2);
	m_pRelay[MICO_EXTCARD3] = (CRelay*)new CRelay(m_Config.wIOBase[MICO_EXTCARD3], MICO_EXTCARD3);
	
	// Jede Erweiterungskarte bekommt ein Alarmobjekt
	m_pAlarm[MICO_EXTCARD0] = (CAlarm*)new CAlarm(m_Config.wIOBase[MICO_EXTCARD0], MICO_EXTCARD0);
	m_pAlarm[MICO_EXTCARD1] = (CAlarm*)new CAlarm(m_Config.wIOBase[MICO_EXTCARD1], MICO_EXTCARD1);
	m_pAlarm[MICO_EXTCARD2] = (CAlarm*)new CAlarm(m_Config.wIOBase[MICO_EXTCARD2], MICO_EXTCARD2);
	m_pAlarm[MICO_EXTCARD3] = (CAlarm*)new CAlarm(m_Config.wIOBase[MICO_EXTCARD3], MICO_EXTCARD3);
   
	// Jedes Videofrontend bekommt ein CBT829 Objekt
	for (WORD wI = MICO_FE0; wI <= MICO_FE1; wI++)
	{
		// Ein BT829 Objekt anlegen
		m_pBT829[wI] = (CBT829*)new CBT829(m_Config.wIOBase[MICO_EXTCARD0], wI); // ExtCard 0 ist die MiCo
   
		if ((!m_pBT829[wI]) || (!m_pBT829[wI]->IsValid()))
		{
			WK_TRACE_ERROR("CMainWindow::CMainWindow\tCBT829-Objekt konnte nicht angelegt werden\n");
			m_csMico.Unlock();
			return;
		}	
		if (m_byMicoID == 0x22)			// Alte Hardware
		{
			WK_TRACE("Mico with 58MHz\n");
			// Initialisiere den BT829
			if (!m_pBT829[wI]->Init("MICOUNIT.INI", "BT829"))
			{
				WK_TRACE_ERROR("CMainWindow::CMainWindow\tCBT829-Objekt konnte nicht initialisiert werden\n");
				m_csMico.Unlock();
				return;
			}
		}
		else if (m_byMicoID == 0x30)	// Neue Hardware
		{
			WK_TRACE("Mico with 59MHz\n");
			// Initialisiere den BT829
			if (!m_pBT829[wI]->Init("MICOUNIT.INI", "BT829_59MHZ"))
			{
				WK_TRACE_ERROR("CMainWindow::CMainWindow\tCBT829-Objekt konnte nicht initialisiert werden\n");
				m_csMico.Unlock();
				return;
			}
		}
	}
	
	// Ein BT856 Objekt anlegen
	m_pBT856 = (CBT856*)new CBT856(m_Config.wIOBase[MICO_EXTCARD0]); // ExtCard 0 ist die MiCo 
   
	if ((!m_pBT856) || (!m_pBT856->IsValid()))
	{
		WK_TRACE_ERROR("CMainWindow::CMainWindow\tCBT856-Objekt konnte nicht angelegt werden\n");
		m_csMico.Unlock();
		return;
	}	

	if (m_byMicoID == 0x22)			// Alte Hardware			
	{
		// Initialisiere den BT856
		if (!m_pBT856->Init("MICOUNIT.INI", "BT856"))
		{
			WK_TRACE_ERROR("CMainWindow::CMainWindow\tCBT856-Objekt konnte nicht initialisiert werden\n");
			m_csMico.Unlock();
			return;
		}
	}
	else if (m_byMicoID == 0x30)	// Neue Hardware
	{
		// Initialisiere den BT856
		if (!m_pBT856->Init("MICOUNIT.INI", "BT856_59MHZ"))
		{
			WK_TRACE_ERROR("CMainWindow::CMainWindow\tCBT856-Objekt konnte nicht initialisiert werden\n");
			m_csMico.Unlock();
			return;
		}
	}

	// Zoranobjekt anlegen
	m_pZoran = (CZoran*)new CZoran(sAppIniName);

	if ((!m_pZoran)	|| (!m_pZoran->IsValid()))
	{
		WK_TRACE_ERROR("CMainWindow::CMainWindow\tCZoran-Objekt konnte nicht angelegt werden\n");
		m_csMico.Unlock();
		return;
	}	

	if (!m_pZoran->Open(m_Config))
	{
		WK_TRACE_ERROR("CMainWindow::CMainWindow\tCZoran-Objekt konnte nicht geöffnet werden\n");
		m_csMico.Unlock();
		return;
	}

	// Initialisiere Objekt anhand der INI-Datei
	Init();

	// Schaltet auf die 1. Kamera
	SetInputSource(MICO_EXTCARD0, MICO_SOURCE0, m_wFormat, m_dwBPF, 0);

	// Dialog zur Farbeinstellung anlegen.
	m_pDlgColor = new CDlgColor(this, m_hWndClient);
	
	if (!m_pDlgColor)
	{
		m_csMico.Unlock();
		return;
	}

	if (!m_pDlgColor->IsValid())
	{
		m_csMico.Unlock();
		return;
	}

	// Alarmpollthread anlegen.
	m_pAlarmPollThread = AfxBeginThread(AlarmPollThread, this);
	if (m_pAlarmPollThread)
		m_pAlarmPollThread->m_bAutoDelete = FALSE;
	
	if (m_pAlarmPollThread)
		m_pAlarmPollThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);

	m_bOK = TRUE;  
	m_bRun= TRUE;

	m_csMico.Unlock();
}

/*/////////////////////////////////////////////////////////////////////////////
@MHEAD{destructor:}
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	Keine
@HRULE
@CW{Funktion:}
	Gibt alle vom CMiCo-Objekt benötigten Resourcen frei, meldet sich beim
	MiCo.vxd ab.
@HRULE
@RELATED
	@ITEM@MLINK{CMiCo::CMiCo}
*/////////////////////////////////////////////////////////////////////////////
CMiCo::~CMiCo()
{
	SaveConfig();

	// AlarmPollThread beenden.
	m_bRun = FALSE;

	// Warte bis 'AlarmPollThread' beendet ist.
	if (m_pAlarmPollThread)
		WaitForSingleObject(m_pAlarmPollThread->m_hThread, 5000);

	WK_DELETE(m_pAlarmPollThread); //Autodelete = FALSE;

	WK_DELETE(m_pDlgColor);

	// CBT829 Objekte zerstören
	for (WORD wI = MICO_FE0; wI <= MICO_FE1; wI++)
		WK_DELETE(m_pBT829[wI]);
	
	// CBT856 Objekte zerstören
	WK_DELETE(m_pBT856);

	// CRelay und CAlarm Objekte zerstören
	for (wI = MICO_EXTCARD0; wI <= MICO_EXTCARD3; wI++)
	{
		WK_DELETE(m_pRelay[wI]);
		WK_DELETE(m_pAlarm[wI]);
	}

	// VXD Entladen
	if (m_pZoran)
		m_pZoran->Close();
	
	// CZoran Objekt zerstören
	WK_DELETE(m_pZoran);

	m_bOK  = FALSE;
}
	  
/*/////////////////////////////////////////////////////////////////////////////
@MHEAD{initialisiations:}
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	Keine
@HRULE
@CW{Funktion:}
	Führt ein Reset des I2C-Bus Controllers (PCT8584) durch
*/////////////////////////////////////////////////////////////////////////////
void CMiCo::ResetVideoInterface()
{
	m_csMico.Lock();

    BYTE byReset;

	// I2C-Bus Controller reseten...
	byReset = m_Io.ReadFromLatch(m_Config.wIOBase[MICO_EXTCARD0] | RESET_OFFSET);

	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_VIF_BIT);

	m_Io.WriteToLatch(m_Config.wIOBase[MICO_EXTCARD0] | RESET_OFFSET, byReset);

	Sleep(10);
	
	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_VIF_BIT);

	m_Io.WriteToLatch(m_Config.wIOBase[MICO_EXTCARD0] | RESET_OFFSET, byReset);

	Sleep(10);

	m_csMico.Unlock();
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	Keiner
@HRULE
@CW{Funktion:}
	Liest weitere Einstellungen aus der im Konstruktor @MLINK{CMiCo::CMiCo}
	angegebenen Ini-Datei und setzt die folgenden Parameter:
	BytesPerFrame, FramesPerSecond, Resolution, VideoFormat (Frontend),
	Brightness, Contrast, Saturation, HUE, VideoFormat (Backend),
	Das Mapping der physikalischen Ports zu den Videoquellen,
	SourceType und NotchFilter jeder Videoquelle
@HRULE
@RELATED
	@ITEM@MLINK{CMiCo::CMiCo}
*/////////////////////////////////////////////////////////////////////////////
void CMiCo::Init()
{
	m_csMico.Lock();

	static char szBuffer[SIZE];
	memset(szBuffer, 0, sizeof(szBuffer));

	static char szSourceType[SIZE];
	memset(szSourceType, 0, sizeof(szSourceType));

	static char szVideoFormat[SIZE];
	memset(szVideoFormat, 0, sizeof(szVideoFormat));

	static char szFilter[SIZE];
	memset(szFilter, 0, sizeof(szFilter));

	// Bytes pro Frame einlesen
	GetPrivateProfileString("MICOISA", "BytesPerFrame", "20000", szBuffer,  SIZE, m_sAppIniName); 
	EncoderSetBPF(atol(szBuffer));
	
	// Frames pro Sekunde einlesen		
	GetPrivateProfileString("MICOISA", "FramesPerSecond", "10", szBuffer,  SIZE, m_sAppIniName); 
	EncoderSetFramerate(atoi(szBuffer));
	
	// Auflösung
	GetPrivateProfileString("MICOISA", "Resolution", "High", szBuffer,  SIZE, m_sAppIniName); 
	if (stricmp(szBuffer,"High") == 0)
		EncoderSetFormat(MICO_ENCODER_HIGH);
	else if (stricmp(szBuffer,"Low") == 0)
		EncoderSetFormat(MICO_ENCODER_LOW);

	// Aktuelle FrontEnd Videoformat (PAL_CCIR, NTSC_CCIR, PAL_SQUARE, NTSC_SQUARE)
	GetPrivateProfileString("FrontEnd", "VideoFormat", "PAL_SQUARE", szVideoFormat,  SIZE, m_sAppIniName); 
	if (stricmp(szVideoFormat,"PAL_SQUARE") == 0)
			SetFeVideoFormat(MICO_PAL_SQUARE);
	else if (stricmp(szVideoFormat,"NTSC_SQUARE") == 0)
			SetFeVideoFormat(MICO_NTSC_SQUARE);
	else if (stricmp(szVideoFormat,"PAL_CCIR") == 0)
			SetFeVideoFormat(MICO_PAL_CCIR);
	else if (stricmp(szVideoFormat,"NTSC_CCIR") == 0)
			SetFeVideoFormat(MICO_NTSC_CCIR);

	// Frontend-Bildeinstellungen laden
	SetFeBrightness(GetPrivateProfileInt("FrontEnd", "Brightness", (MICO_MAX_BRIGHTNESS - MICO_MIN_BRIGHTNESS) / 2, m_sAppIniName));
	SetFeContrast(GetPrivateProfileInt("FrontEnd", "Contrast",     (MICO_MAX_CONTRAST - MICO_MIN_CONTRAST) / 2, m_sAppIniName)); 
	SetFeSaturation(GetPrivateProfileInt("FrontEnd", "Saturation", (MICO_MAX_SATURATION - MICO_MIN_SATURATION) / 2, m_sAppIniName)); 
	SetFeHue(GetPrivateProfileInt("FrontEnd", "Hue",		       (MICO_MAX_HUE - MICO_MIN_HUE) / 2, m_sAppIniName));
	
	// Aktuelles BackEnd Videoformat(PAL_CCIR, NTSC_CCIR, PAL_SQUARE, NTSC_SQUARE)
	GetPrivateProfileString("BackEnd", "VideoFormat", "PAL_SQUARE", szVideoFormat,  SIZE, m_sAppIniName); 
	if (stricmp(szVideoFormat,"PAL_SQUARE") == 0)
			SetBeVideoFormat(MICO_PAL_SQUARE);
	else if (stricmp(szVideoFormat,"NTSC_SQUARE") == 0)
			SetBeVideoFormat(MICO_NTSC_SQUARE);
	else if (stricmp(szVideoFormat,"PAL_CCIR") == 0)
			SetBeVideoFormat(MICO_PAL_CCIR);
	else if (stricmp(szVideoFormat,"NTSC_CCIR") == 0)
			SetBeVideoFormat(MICO_NTSC_CCIR);

	// Aktuelle BackEnd Videoausgabeart (COMPOSITE, RGB)
	GetPrivateProfileString("BackEnd", "VideoType", "COMPOSITE", szSourceType,  SIZE, m_sAppIniName); 
	if (stricmp(szSourceType,"COMPOSITE") == 0)
		SetBeVideoType(MICO_COMPOSITE);
	else if (stricmp(szSourceType,"RGB") == 0)
		SetBeVideoType(MICO_RGB);

	// Den physikalischen Portanschluß aller Kameras einlesen
	for (WORD wCrd = MICO_EXTCARD0; wCrd <= MICO_EXTCARD0   + MICO_MAX_EXTCARD; wCrd++)
	{
		if (!TSTBIT(GetHardwareState(), wCrd))
			continue;
	
		for (WORD wSrc = MICO_SOURCE0; wSrc <= MICO_SOURCE0 + MICO_MAX_SOURCE; wSrc++)
		{
			wsprintf(szBuffer, "Source%u-%u", wCrd, wSrc);

			// Test ob Eintrag vorhanden ist
			if (GetPrivateProfileInt(szBuffer, "AVPortY",    -1, m_sAppIniName) != -1)
			{
				m_MiCoISA.ExtCard[wCrd].Source[wSrc].nAVPortY = GetPrivateProfileInt(szBuffer, "AVPortY", wSrc - MICO_SOURCE0, m_sAppIniName);
				m_MiCoISA.ExtCard[wCrd].Source[wSrc].nAVPortC = GetPrivateProfileInt(szBuffer, "AVPortC", wSrc - MICO_SOURCE0, m_sAppIniName);
				m_MiCoISA.ExtCard[wCrd].Source[wSrc].bPresent = TRUE;
				GetPrivateProfileString(szBuffer, "SourceType", "FBAS", szSourceType,  SIZE, m_sAppIniName); 
				if (stricmp(szSourceType,"FBAS") == 0)
					m_MiCoISA.ExtCard[wCrd].Source[wSrc].wFeSourceType = MICO_FBAS;
				else if (stricmp(szSourceType,"SVHS") == 0)
					m_MiCoISA.ExtCard[wCrd].Source[wSrc].wFeSourceType = MICO_SVHS;

				GetPrivateProfileString(szBuffer, "NotchFilter", "Auto", szFilter,  SIZE, m_sAppIniName); 
				if (stricmp(szFilter,"Auto") == 0)
					m_MiCoISA.ExtCard[wCrd].Source[wSrc].wFeFilter = MICO_NOTCH_FILTER_AUTO;
				else if (stricmp(szFilter,"Yes") == 0)
					m_MiCoISA.ExtCard[wCrd].Source[wSrc].wFeFilter = MICO_NOTCH_FILTER_ON;
				else if (stricmp(szFilter,"No") == 0)
					m_MiCoISA.ExtCard[wCrd].Source[wSrc].wFeFilter = MICO_NOTCH_FILTER_OFF;
			}
			else
				m_MiCoISA.ExtCard[wCrd].Source[wSrc].bPresent = FALSE;

		}
	}	

	m_csMico.Unlock();

	return;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	TRUE, wenn kein Fehler aufgetreten ist, ansonsten FALSE
@HRULE
@CW{Funktion:}
	Speichert folgende Parameter in die im Konstruktor @MLINK{CMiCo::CMiCo}
	angegebene Ini-Datei: Resolution, VideoFormat (Frontend), Brightness, Contrast, Saturation, HUE,
	VideoFormat (Backend), VideoType.
@HRULE
@RELATED
	@ITEM@MLINK{CMiCo::CMiCo}
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::SaveConfig()
{
	m_csMico.Lock();

	if (!IsValid())
	{
		m_csMico.Unlock();
		return FALSE;
	}

	static char szBuffer[SIZE];
	memset(szBuffer, 0, sizeof(szBuffer));

	static char szSourceType[SIZE];
	memset(szSourceType, 0, sizeof(szSourceType));

	static char szVideoFormat[SIZE];
	memset(szVideoFormat, 0, sizeof(szVideoFormat));

	static char szSource[SIZE];
	memset(szSource, 0, sizeof(szSource));

	if (EncoderGetFormat() == MICO_ENCODER_HIGH)
		WritePrivateProfileString("MICOISA", "Resolution", "High", m_sAppIniName); 
	if (EncoderGetFormat() == MICO_ENCODER_LOW)
		WritePrivateProfileString("MICOISA", "Resolution", "Low", m_sAppIniName); 
	
	// *************************************************
	// Video Front End
	// *************************************************

	// Aktuelle FrontEnd Videoformat (PAL_CCIR, PCAL_SQUARE, NTSC_CCIR, NTSC_SUARE) sichern
	if (GetFeVideoFormat() == MICO_PAL_SQUARE)
		WritePrivateProfileString("FrontEnd", "VideoFormat", "PAL_SQUARE", m_sAppIniName); 
	else if (GetFeVideoFormat() == MICO_PAL_CCIR)
		WritePrivateProfileString("FrontEnd", "VideoFormat", "PAL_CCIR", m_sAppIniName); 
	else if (GetFeVideoFormat() == MICO_NTSC_SQUARE)
		WritePrivateProfileString("FrontEnd", "VideoFormat", "NTSC_SQUARE", m_sAppIniName); 
	else if (GetFeVideoFormat() == MICO_NTSC_CCIR)
		WritePrivateProfileString("FrontEnd", "VideoFormat", "NTSC_CCIR", m_sAppIniName); 

	// Aktuelle FrontEnd Bildeinstellungen sichern
	itoa(GetFeBrightness(), szBuffer, 10);
	WritePrivateProfileString("FrontEnd", "Brightness", szBuffer, m_sAppIniName);

	itoa(GetFeContrast(), szBuffer, 10);
	WritePrivateProfileString("FrontEnd", "Contrast", szBuffer, m_sAppIniName);
	
	itoa(GetFeSaturation(), szBuffer, 10);
	WritePrivateProfileString("FrontEnd", "Saturation", szBuffer, m_sAppIniName);

	itoa(GetFeHue(), szBuffer, 10);
	WritePrivateProfileString("FrontEnd", "Hue", szBuffer, m_sAppIniName);

	// Aktuelle BackEnd Videoformat (PAL_CCIR, PCAL_SQUARE, NTSC_CCIR, NTSC_SUARE) sichern
	if (GetBeVideoFormat() == MICO_PAL_SQUARE)
		WritePrivateProfileString("BackEnd", "VideoFormat", "PAL_SQUARE", m_sAppIniName); 
	else if (GetBeVideoFormat() == MICO_PAL_CCIR)
		WritePrivateProfileString("BackEnd", "VideoFormat", "PAL_CCIR", m_sAppIniName); 
	else if (GetBeVideoFormat() == MICO_NTSC_SQUARE)
		WritePrivateProfileString("BackEnd", "VideoFormat", "NTSC_SQUARE", m_sAppIniName); 
	else if (GetBeVideoFormat() == MICO_NTSC_CCIR)
		WritePrivateProfileString("BackEnd", "VideoFormat", "NTSC_CCIR", m_sAppIniName); 

	// Aktuelle BackEnd Videoausgabeart (COMPOSITE, RGB) sichern
	if (GetBeVideoType() == MICO_COMPOSITE)
		WritePrivateProfileString(szBuffer, "VideoType", "COMPOSITE", m_sAppIniName); 
	else if (GetBeVideoType() == MICO_RGB)
		WritePrivateProfileString(szBuffer, "VideoType", "RGB", m_sAppIniName); 

	m_csMico.Unlock();

	return TRUE;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{diagnostics:}
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	TRUE, wenn das CMiCo-Objekt korrekt angelegt werden konnte, ansonsten FALSE
@HRULE
@CW{Funktion:}
	s.o. 
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::IsValid()
{
	return m_bOK;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	Liefert eine Versionsnummer der MiCoUnit zurück (z.Z 0x0001) 
@HRULE
@CW{Funktion:}
	s.o.
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetVersion()
{
	return MICO_UNIT_VERSION;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	Liefert einen der folgenden Fehlerkodes zurück.
		@ITEM@CW{MICO_ERR_NO_ERRROR					= 0}
		@ITEM@CW{MICO_ERR_WRONG_FE_ID				= 1}
		@ITEM@CW{MICO_ERR_WRONG_SOURCE				= 2}
		@ITEM@CW{MICO_ERR_WRONG_EXT_CARD			= 3}
		@ITEM@CW{MICO_ERR_WRONG_SOURCE_TYPE			= 4}
		@ITEM@CW{MICO_ERR_WRONG_FE_VIDEO_FORMAT		= 5}
		@ITEM@CW{MICO_ERR_WRONG_BE_VIDEO_FORMAT		= 6}
		@ITEM@CW{MICO_ERR_PARM_OUT_OF_RANGE			= 8}
		@ITEM@CW{MICO_ERR_UNKNOWN_PARAMETER			= 9}
		@ITEM@CW{MICO_ERR_WRONG_FILTER_TYPE			= 10}
		@ITEM@CW{MICO_ERR_WRONG_ENCODER_FORMAT		= 11}
		@ITEM@CW{MICO_ERR_WRONG_ENCODER_FRAMERATE	= 12}
		@ITEM@CW{MICO_ERR_WRONG_ENCODER_BPF			= 13}
		@ITEM@CW{MICO_ERROR							= 0xffff}
	Das Lesen bewirkt das Rücksetzen des Fehlerstatuses.
@HRULE
@CW{Funktion:}
	Nach dem Auftreten eines Fehlers, kann durch den Aufruf dieser Funktion
	die Ursache näher ermittelt werden.
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetLastError()
{
	m_csMico.Lock();

	WORD wTemp = m_wLastError;
	m_wLastError = MICO_ERR_NO_ERRROR;

	m_csMico.Unlock();

	return wTemp;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	TRUE, wenn die MiCo-Hardware korrekt arbeitet, ansonsten FALSE 
@HRULE
@CW{Funktion:}
	Nicht implementiert
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckHardware()
{
	return TRUE;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	Keine
@HRULE
@CW{Rückgabe:}
	Der Rückgabewert gibt Bitweise darüber Auskunft welche Boards installiert
	sind.
	@ITEM@CW{Bit0 = 1 MiCo  vorhanden, 0 MiCo nicht vorhanden}
	@ITEM@CW{Bit1 = 1 CoVi0 vorhanden, 0 CoVi0 nicht vorhanden}
	@ITEM@CW{Bit2 = 1 CoVi1 vorhanden, 0 CoVi1 nicht vorhanden}
	@ITEM@CW{Bit3 = 1 CoVi2 vorhanden, 0 CoVi2 nicht vorhanden}
@HRULE
@CW{Funktion:}
	Gibt darüber Auskunft ob ein MiCo-Board und wieviele CoVis installiert sind.
*/////////////////////////////////////////////////////////////////////////////
DWORD CMiCo::GetHardwareState()
{
	return m_dwHardwareState;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	@ITEM@CW{WORD &wExtCard}=Referenz auf eine WORD-Variable, die nach dem 
	Funktionsaufruf die aktuell ausgewählte CoVi-Nummer enthält. 
	@ITEM@CW{WORD &wSource}	=Referenz auf eine WORD-Variable, die nach dem 
	Funktionsaufruf die aktuell ausgewählte Kameranummer enthält.
@HRULE
@CW{Rückgabe:}
	TRUE->Videosignal vorhanden, ansonsten FALSE
@HRULE
@CW{Funktion:}
	Liefert für die aktuell ausgewählte Videoquelle die Information
	darüber, ob ein Videosignal anliegt, oder nicht.
	Die beiden Referenzen enthalten nach dem Aufruf die aktuell
	ausgewählte Videoquelle
	Die Überprüfung findet nur für die Kameras statt, die einen AVPortY-Eintrag
	in der MiCoUnit.ini besitzen. Fehlt dieser Eintrag liefert die Funktion TRUE
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::IsVideoPresent(WORD &wExtCard, WORD &wSource)
{
 	m_csMico.Lock();

	BOOL bRet = TRUE;

	wExtCard = m_wExtCard;
	wSource  = m_wSource;

	// Nur die Videoquellen Überwachen denen ein AVPort zugewiesen ist.
	if (CheckSourceInstalled(wSource, wExtCard))
	{
		if (m_wFrontEnd == MICO_FE0)
		{
			if (m_pBT829[MICO_FE1])
				bRet = m_pBT829[MICO_FE1]->IsVideoPresent();
		}
		else
		{
			if (m_pBT829[MICO_FE0])
				bRet = m_pBT829[MICO_FE0]->IsVideoPresent();
		}
	}

 	m_csMico.Unlock();

	return bRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{source selection:}
@CW{Parameter:}
	@ITEM@CW{WORD wExtCard}=CoVi-Nummer	    (MICO_EXTCARD0...MICO_EXTCARD3)
	@ITEM@CW{WORD wSource}	= Videoquellennummer (MICO_SOURCE0...MICO_SOURCE7)
	@ITEM@CW{WORD wFormat}	= Bildgröße			(MICO_ENCODER_LOW, MICO_ENCODER_HIGH)
	@ITEM@CW{DWORD dwBPF}	= Größe eines JPegbildes (5000...45000 Bytes)
	@ITEM@CW{DWORD dwProzessID} = Eine eindeutige ProzeßID, die jedes Jpegbild
								  einen Prozeß zuordnet.
  @HRULE
@CW{Rückgabe:}
	Die bisherige Videoquelle wird wie folgt kodiert zurückgeliefert: 0xYYYYZZZZ
	YYYY = Videoquellennummer (MICO_SOURCE0...MICO_SOURCE7)
	ZZZZ = CoVi-Nummer (MICO_EXTCARD0...MICO_EXTCARD3)	
	Im Fehlerfall wird CW{MICO_ERROR} zurückgeliefert und nicht auf die
	geforderte Videoquelle umgeschaltet,
@HRULE
@CW{Funktion:}
	Die Funktion schaltet auf den geforderten Videoeingang, sofern die Parameter sich
	in gültigen Grenzen bewegen.
	Bei jedem erfolgreichen Kameraumschaltvorgang wird das Video-Frontend gewechselt.
@HRULE
@RELATED
	@ITEM@MLINK{CMiCo::GetInputSource}
*/////////////////////////////////////////////////////////////////////////////
DWORD CMiCo::SetInputSource(WORD wExtCard, WORD wSource, WORD wFormat, DWORD dwBPF, DWORD dwProzessID)
{
 	m_csMico.Lock();

	DWORD	dwOldSource		= MAKELONG(m_wSource, m_wExtCard);
	BYTE	byPortY			= 0;
	BYTE	byPortC			= 0;
	DWORD	dwVidSel		= 0;

	if (!CheckExtCard(wExtCard))
	{
	 	m_csMico.Unlock();
		return MICO_ERROR;
	}

	if(!CheckSource(wSource))
	{
	 	m_csMico.Unlock();
		return MICO_ERROR;
	}
	
	if (m_bTraceSetInputSource)
	{
		if (wExtCard == MICO_EXTCARD0)
			WK_TRACE("SetInputSource %u on MiCo\n", wSource + 1);
		else
			WK_TRACE("SetInputSource %u on CoVi%u\n", wSource + 1, wExtCard);
	}

	// Aktuelle Kamera merken.
	m_wExtCard = wExtCard;		
	m_wSource  = wSource;

	// Bildgröße und Format setzen
	EncoderSetBPF(dwBPF);
	EncoderSetFormat(wFormat);

	// Das Frontend auf den Sourcetype der gewüschten Kamera schalten
	if (m_pBT829[m_wFrontEnd]){
		m_pBT829[m_wFrontEnd]->SetFeSourceType(m_MiCoISA.ExtCard[wExtCard].Source[wSource].wFeSourceType,
										       m_MiCoISA.ExtCard[wExtCard].Source[wSource].wFeFilter);
	}

	// Kamera umschalten
	byPortY  = (BYTE)m_MiCoISA.ExtCard[wExtCard].Source[wSource].nAVPortY;
	byPortC |= (BYTE)m_MiCoISA.ExtCard[wExtCard].Source[wSource].nAVPortC;
	if (m_pZoran)
		m_pZoran->SetInputSource(m_wFrontEnd, wExtCard, wSource, byPortY, byPortC, dwProzessID);

	// Die Frontends werden im Wechsel angesprochen
	if (m_wFrontEnd == MICO_FE0)
		m_wFrontEnd = MICO_FE1;
	else if (m_wFrontEnd == MICO_FE1)
		m_wFrontEnd = MICO_FE0;

	m_csMico.Unlock();

	return dwOldSource;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	@ITEM@CW{WORD &wExtCard}=Referenz auf eine WORD-Variable, die nach dem 
	Funktionsaufruf die aktuell ausgewählte CoVi-Nummer enthält. 
	@ITEM@CW{WORD &wSource}	=Referenz auf eine WORD-Variable, die nach dem 
	Funktionsaufruf die aktuell ausgewählte Kameranummer enthält.
@HRULE
@CW{Rückgabe:}
	Keine
@HRULE
@CW{Funktion:}
	Liefert über die beiden Referenzen den aktuell eingestellten Videoeingang.
	@ITEM wExtCard = (MICO_EXTCARD0...MICO_EXTCARD3)
	@ITEM wSource  = (MICO_SOURCE0...MICO_SOURCE7)
@HRULE
@RELATED
	@ITEM@MLINK{CMiCo::SetInputSource}
*/////////////////////////////////////////////////////////////////////////////
void CMiCo::GetInputSource(WORD &wExtCard, WORD &wSource)
{
 	m_csMico.Lock();

	wSource		= m_wSource;
	wExtCard	= m_wExtCard;
 	
	m_csMico.Unlock();
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{video frontend:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeSourceType(WORD wSourceType)
{
 	m_csMico.Lock();

	WORD wOldSourceType = GetFeSourceType();

	if (CheckSourceType(wSourceType))
	{
		// Beide Frontends einstellen
		if (m_pBT829[MICO_FE0])
			m_pBT829[MICO_FE0]->SetFeSourceType(wSourceType, m_MiCoISA.ExtCard[m_wExtCard].Source[m_wSource].wFeFilter);
		
		if (m_pBT829[MICO_FE1])
			m_pBT829[MICO_FE1]->SetFeSourceType(wSourceType, m_MiCoISA.ExtCard[m_wExtCard].Source[m_wSource].wFeFilter);
		
		m_MiCoISA.ExtCard[m_wExtCard].Source[m_wSource].wFeSourceType = wSourceType;
	}

	m_csMico.Unlock();

	return wOldSourceType;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeSourceType()
{
	WORD wFeSourceType = MICO_FBAS;

	if (m_pBT829[MICO_FE0])
		wFeSourceType = m_pBT829[MICO_FE0]->GetFeSourceType();

	return wFeSourceType;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeVideoFormat(WORD wFeVideoFormat)
{
 	m_csMico.Lock();
	
	WORD wOldFeVideoFormat = GetFeVideoFormat();

	if (CheckFeVideoFormat(wFeVideoFormat))
	{
		// MiCo auf SQUARE/CCIR stellen
		BYTE byParam = m_Io.ReadFromLatch(m_Config.wIOBase[MICO_EXTCARD0] | PARAM_OFFSET);
		switch (wFeVideoFormat)
		{
			case MICO_PAL_SQUARE:
			case MICO_NTSC_SQUARE:
				byParam = SETBIT(byParam, PARAM_SQUARE_BIT);
				byParam = SETBIT(byParam, PARAM_SQUARE2_BIT);
				break;					  
			case MICO_PAL_CCIR:
			case MICO_NTSC_CCIR:
				byParam = CLRBIT(byParam, PARAM_SQUARE_BIT);
				byParam = CLRBIT(byParam, PARAM_SQUARE2_BIT); 			
				break;
		}
		m_Io.WriteToLatch(m_Config.wIOBase[MICO_EXTCARD0] | PARAM_OFFSET, byParam);
		
		// Beide Frontends einstellen
		if (m_pBT829[MICO_FE0])
			m_pBT829[MICO_FE0]->SetFeVideoFormat(wFeVideoFormat);
		
		if (m_pBT829[MICO_FE1])
			m_pBT829[MICO_FE1]->SetFeVideoFormat(wFeVideoFormat);

		if (m_pZoran)
			m_pZoran->SetFeVideoFormat(wFeVideoFormat);
	}

 	m_csMico.Unlock();

	return wOldFeVideoFormat;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeVideoFormat()
{
	WORD wFeVideoFormat = MICO_PAL_SQUARE;

	if (m_pBT829[MICO_FE0])
		wFeVideoFormat = m_pBT829[MICO_FE0]->GetFeVideoFormat();

	return wFeVideoFormat;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeBrightness(WORD wBrightness)
{
 	m_csMico.Lock();

	WORD wOldBrightness = GetFeBrightness();
	
	CheckRange(&wBrightness, MICO_MIN_BRIGHTNESS, MICO_MAX_BRIGHTNESS);

	if (m_bTraceSetFeBrightness)
		WK_TRACE("SetFeBrightness=%u\n", wBrightness);

	// Beide Frontends einstellen
	if (m_pBT829[MICO_FE0])
		m_pBT829[MICO_FE0]->SetFeBrightness(wBrightness);
	
	if (m_pBT829[MICO_FE1])
		m_pBT829[MICO_FE1]->SetFeBrightness(wBrightness);

 	m_csMico.Unlock();

	return wOldBrightness;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeBrightness()
{
	WORD wFeBrightness = (MICO_MAX_BRIGHTNESS	- MICO_MIN_BRIGHTNESS)	/ 2;

	if (m_pBT829[MICO_FE0])
		wFeBrightness = m_pBT829[MICO_FE0]->GetFeBrightness();

	return wFeBrightness;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeContrast(WORD wContrast)
{
 	m_csMico.Lock();

	WORD wOldContrast = GetFeContrast();

	CheckRange(&wContrast, MICO_MIN_CONTRAST, MICO_MAX_CONTRAST);

	if (m_bTraceSetFeContrast)
		WK_TRACE("SetFeContrast=%u\n", wContrast);

	// Beide Frontends einstellen
	if (m_pBT829[MICO_FE0])
		m_pBT829[MICO_FE0]->SetFeContrast(wContrast);
	
	if (m_pBT829[MICO_FE1])
		m_pBT829[MICO_FE1]->SetFeContrast(wContrast);

 	m_csMico.Unlock();

	return wOldContrast;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeContrast()
{
	WORD wFeContrast = (MICO_MAX_CONTRAST	- MICO_MIN_CONTRAST) / 2;

	if (m_pBT829[MICO_FE0])
		wFeContrast = m_pBT829[MICO_FE0]->GetFeContrast();

	return wFeContrast;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeSaturation(WORD wSaturation)
{
 	m_csMico.Lock();

	WORD wOldSaturation = GetFeSaturation();

	CheckRange(&wSaturation, MICO_MIN_SATURATION, MICO_MAX_SATURATION);

	if (m_bTraceSetFeSaturation)
		WK_TRACE("SetFeSaturation=%u\n", wSaturation);

	// Beide Frontends einstellen
	if (m_pBT829[MICO_FE0])
		m_pBT829[MICO_FE0]->SetFeSaturation(wSaturation);
	
	if (m_pBT829[MICO_FE1])
		m_pBT829[MICO_FE1]->SetFeSaturation(wSaturation);

 	m_csMico.Unlock();

	return wOldSaturation;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeSaturation()
{
	WORD wFeSaturation = (MICO_MAX_SATURATION - MICO_MIN_SATURATION) / 2;

	if (m_pBT829[MICO_FE0])
		wFeSaturation =  m_pBT829[MICO_FE0]->GetFeSaturation();

	return wFeSaturation;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeHue(WORD wHue)
{
 	m_csMico.Lock();

	WORD wOldHue = GetFeHue();

	CheckRange(&wHue, MICO_MIN_HUE, MICO_MAX_HUE);

	if (m_bTraceSetFeHue)
		WK_TRACE("SetFeHue=%u\n", wHue);

	// Beide Frontends einstellen
	if (m_pBT829[MICO_FE0])
		m_pBT829[MICO_FE0]->SetFeHue(wHue);
	
	if (m_pBT829[MICO_FE1])
		m_pBT829[MICO_FE1]->SetFeHue(wHue);

 	m_csMico.Unlock();

	return wOldHue;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeHue()
{
	WORD wFeHue = (MICO_MAX_HUE	- MICO_MIN_HUE)	/ 2;

	if (m_pBT829[MICO_FE0])
		wFeHue = m_pBT829[MICO_FE0]->GetFeHue();

	return wFeHue;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeHScale(WORD wHScale)
{
 	m_csMico.Lock();

	WORD wOldHScale = GetFeHScale();

	CheckRange(&wHScale, MICO_MIN_HSCALE, MICO_MAX_HSCALE);

	// Beide Frontends einstellen
	if (m_pBT829[MICO_FE0])
		m_pBT829[MICO_FE0]->SetFeHScale(wHScale);
	
	if (m_pBT829[MICO_FE1])
		m_pBT829[MICO_FE1]->SetFeHScale(wHScale);
	
 	m_csMico.Unlock();

	return wOldHScale;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeHScale()
{
	WORD wFeHScale = (MICO_MAX_HSCALE - MICO_MIN_HSCALE) / 2;

	if (m_pBT829[MICO_FE0])
		wFeHScale = m_pBT829[MICO_FE0]->GetFeHScale();

	return wFeHScale;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeVScale(WORD wVScale)
{
 	m_csMico.Lock();

	WORD wOldVScale = GetFeVScale();

	CheckRange(&wVScale, MICO_MIN_VSCALE, MICO_MAX_VSCALE);

	// Beide Frontends einstellen
	if (m_pBT829[MICO_FE0])
		m_pBT829[MICO_FE0]->SetFeVScale(wVScale);
	
	if (m_pBT829[MICO_FE1])
		m_pBT829[MICO_FE1]->SetFeVScale(wVScale);

 	m_csMico.Unlock();

	return wOldVScale;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeVScale()
{
	WORD wFeVScale = (MICO_MAX_VSCALE - MICO_MIN_VSCALE) / 2;

	if (m_pBT829[MICO_FE0])
		wFeVScale = m_pBT829[MICO_FE0]->GetFeVScale();

	return wFeVScale;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetFeFilter(WORD wFilter)
{
 	m_csMico.Lock();

	WORD wOldFilter = GetFeFilter();

	if(CheckFeFilter(wFilter))
	{
		// Beide Frontends einstellen
		if (m_pBT829[MICO_FE0])
			m_pBT829[MICO_FE0]->SetFeFilter(wFilter);
		
		if (m_pBT829[MICO_FE1])
			m_pBT829[MICO_FE1]->SetFeFilter(wFilter);
	}

 	m_csMico.Unlock();

	return wOldFilter;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetFeFilter()
{
	WORD wFeFilter = MICO_NOTCH_FILTER_ON;

	if (m_pBT829[MICO_FE0])
		wFeFilter = m_pBT829[MICO_FE0]->GetFeFilter();

	return wFeFilter;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{video backend:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetBeVideoFormat(WORD wBeVideoFormat)
{
 	m_csMico.Lock();
	
	WORD wOldBeVideoFormat = GetBeVideoFormat();

	if(CheckBeVideoFormat(wBeVideoFormat))
	{
		if (m_pBT856)
			m_pBT856->SetBeVideoFormat(wBeVideoFormat);

		if (m_pZoran)
			m_pZoran->SetBeVideoFormat(wBeVideoFormat);
	}

	m_csMico.Unlock();

	return wOldBeVideoFormat;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetBeVideoFormat()
{
	WORD wBeVideoFormat = MICO_PAL_CCIR;

	if (m_pBT856)
		wBeVideoFormat = m_pBT856->GetBeVideoFormat();

	return wBeVideoFormat;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::SetBeVideoType(WORD wBeVideoType)
{
 	m_csMico.Lock();

	WORD wOldBeVideoType = GetBeVideoType();
	if(CheckBeVideoType(wBeVideoType))
	{
		BYTE byVal;
		// Den Ausgang des MiCo's auf RGB bzw. FBAS stellen
		byVal = m_Io.Input(m_Config.wIOBase[MICO_EXTCARD0] | RELAY_OFFSET);
		if (wBeVideoType == MICO_COMPOSITE)
		   byVal = CLRBIT(byVal, RGBOUT_BIT);
		else
		   byVal = SETBIT(byVal, RGBOUT_BIT);
		m_Io.Output(m_Config.wIOBase[MICO_EXTCARD0] | RELAY_OFFSET, byVal);

		m_wBeVideoType = wBeVideoType;
	}
	
 	m_csMico.Unlock();

	return wOldBeVideoType;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::GetBeVideoType()
{
	return  m_wBeVideoType; 
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::DlgLuminance()
{
	BOOL bRet = FALSE;

	if (m_pDlgColor)
		m_pDlgColor->ShowWindow(SW_SHOW);
	
	if (m_pDlgColor)
		bRet = m_pDlgColor->IsValid();

	return bRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{alarm handling:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::PollAlarm()
{
	m_csMico.Lock();

	BYTE byAck					= 0;
	BYTE byFree					= 0;
	BYTE byChange				= 0;
	BYTE byCurrentAlarmState	= 0;
	BOOL  bRetFlag				= FALSE;
	DWORD dwStartTime;

	for (WORD wExtCard = MICO_EXTCARD0; wExtCard <= MICO_EXTCARD3; wExtCard++)
	{
		// Karte vorhanden
		if (!TSTBIT(GetHardwareState(), wExtCard))
			continue;

		// CurrentAlarmstatusregister lesen.
		byCurrentAlarmState = GetCurrentAlarmState(wExtCard);
		
		if (byCurrentAlarmState != m_byAlarmState[wExtCard])
		{
			byAck 		= GetAlarmAck(wExtCard);  	
			byChange 	= byCurrentAlarmState ^ m_byAlarmState[wExtCard];
			byFree		= ~byChange & byAck;

			m_byAlarmState[wExtCard] = byCurrentAlarmState;
												
			// Alarm löschen
			SetAlarmAck(wExtCard, byAck); 
			// Alarm freigeben
			SetAlarmAck(wExtCard, byFree); 

   			// Test ob das Signal über einen längeren Zeitpunkt nicht ändert.
			dwStartTime = GetTickCount();
			while (GetTickCount() < dwStartTime + 50)
            {
				// CurrentAlarmstatusregister lesen und 'aufundieren'.
				byCurrentAlarmState = byCurrentAlarmState & GetCurrentAlarmState(wExtCard);
				if (byCurrentAlarmState == 0)
					break;
				Sleep(0);
			}             

			PostMessage(m_hWndClient, WM_MICO_ALARM, wExtCard, byCurrentAlarmState);
			if (m_bTraceAlarmIndication)
				WK_TRACE("AlarmIndication (Mask=0x%x on ExtCard %u)\n", (WORD)byCurrentAlarmState, wExtCard);

			bRetFlag = TRUE;
		}
	}
	
	m_csMico.Unlock();

	return bRetFlag;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::GetCurrentAlarmState(WORD wExtCard)
{
 	m_csMico.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wExtCard))
	{
		if (m_pAlarm[wExtCard])
			byRet = m_pAlarm[wExtCard]->GetCurrentState();
	}
	m_csMico.Unlock();
	
	return byRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::GetAlarmState(WORD wExtCard)
{
 	m_csMico.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wExtCard))
	{
		if (m_pAlarm[wExtCard])
			byRet = m_pAlarm[wExtCard]->GetState(); 
	}
	
	m_csMico.Unlock();

	return byRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::SetAlarmEdge(WORD wExtCard, BYTE byEdge)
{
 	m_csMico.Lock();

	BYTE byOldEdge = 0;

	if (CheckExtCard(wExtCard))
	{
		byOldEdge = GetAlarmEdge(wExtCard);

		if (m_pAlarm[wExtCard])
			m_pAlarm[wExtCard]->SetEdge(byEdge);
	
		if (m_bTraceSetAlarmEdge)
			WK_TRACE("SetAlarmEdge (Mask=0x%x on ExtCard %u)\n", (WORD)byEdge, wExtCard); 
	}
	
	m_csMico.Unlock();

	return byOldEdge;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::GetAlarmEdge(WORD wExtCard)
{
 	m_csMico.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wExtCard))
	{
		if (m_pAlarm[wExtCard])
			byRet = m_pAlarm[wExtCard]->GetEdge();
	}
	m_csMico.Unlock();

	return byRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::SetAlarmAck(WORD wExtCard, BYTE byAck)
{
 	m_csMico.Lock();

	BYTE byOldAck = 0; 

	if (CheckExtCard(wExtCard))
	{
		byOldAck = GetAlarmAck(wExtCard); 

		if (m_pAlarm[wExtCard])
			m_pAlarm[wExtCard]->SetAck(byAck);

		if (m_bTraceSetAlarmAck)
			WK_TRACE("SetAlarmAck (Mask=0x%x on ExtCard %u)\n", (WORD)byAck, wExtCard); 
	}

	m_csMico.Unlock();

	return byOldAck;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::GetAlarmAck(WORD wExtCard)
{
 	m_csMico.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wExtCard))
	{
		if (m_pAlarm[wExtCard])
			byRet = m_pAlarm[wExtCard]->GetAck();
	}
	
	m_csMico.Unlock();

	return byRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{relay handling:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::GetRelais(WORD wExtCard)
{
 	m_csMico.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wExtCard))
	{
		if (m_pRelay[wExtCard])
			byRet = m_pRelay[wExtCard]->GetRelay();
	}

	m_csMico.Unlock();

	return byRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BYTE CMiCo::SetRelais(WORD wExtCard, BYTE byRelais)
{
 	m_csMico.Lock();

	BYTE byOldRelais = 0; 

	if (CheckExtCard(wExtCard))
	{
		byOldRelais = GetRelais(wExtCard); 
		if (m_pRelay[wExtCard])
			m_pRelay[wExtCard]->SetRelay(byRelais);

		if (m_bTraceSetRelais)
			WK_TRACE("SetRelais (Mask=0x%x on ExtCard=%u)\n", (WORD)byRelais, wExtCard);
	}

  	m_csMico.Unlock();

	return byOldRelais;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{video encoding:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::EncoderStart()
{
	m_csMico.Lock();

	BOOL bRet = FALSE;

	if (m_bTraceStartStopEncoder)
		WK_TRACE("CMiCo::EncoderStart\n");

	if (m_pZoran)
		bRet = m_pZoran->StartEncoding();

	m_csMico.Unlock();

	return bRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::EncoderStop()
{
	m_csMico.Lock();
	
	BOOL bRet = FALSE;

	if (m_bTraceStartStopEncoder)
		WK_TRACE("CMiCo::EncoderStop\n");
	
	if (m_pZoran)
		bRet = m_pZoran->StopEncoding();

	m_csMico.Unlock();

	return bRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::EncoderSetFormat(WORD wFormat)
{
	m_csMico.Lock();
	
	WORD wOldFormat = MICO_ERROR;
	
	if (CheckEncoderFormat(wFormat))
	{
		wOldFormat = EncoderGetFormat();

		if (m_pZoran)
			m_pZoran->SetFormat(wFormat);

		m_wFormat = wFormat;
	}

  	m_csMico.Unlock();

	return wOldFormat;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::EncoderGetFormat()
{
	return 	m_wFormat;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::EncoderSetFramerate(WORD wFramerate)
{
	m_csMico.Lock();

	WORD wOldFramerate = MICO_ERROR;

	if (CheckEncoderFramerate(wFramerate))
	{
		wOldFramerate = EncoderGetFramerate();
		
		if (m_pZoran)
			m_pZoran->SetFramerate(wFramerate);

		m_wFramerate = wFramerate;
	}

  	m_csMico.Unlock();

	return wOldFramerate;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
WORD CMiCo::EncoderGetFramerate()
{
	return m_wFramerate;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
DWORD CMiCo::EncoderSetBPF(DWORD dwBPF)
{
	m_csMico.Lock();

	DWORD dwOldBPF = MICO_ERROR;

	if (CheckEncoderBPF(dwBPF))
	{
		if (m_bTraceSetBPF)
			WK_TRACE("EncoderSetBPF=%d\n", dwBPF);

		dwOldBPF = EncoderGetBPF();

		if (m_pZoran)
			m_pZoran->SetBPF(dwBPF);

		m_dwBPF = dwBPF;
	}

	m_csMico.Unlock();

	return dwOldBPF;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
DWORD CMiCo::EncoderGetBPF()
{
	return m_dwBPF;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::EncoderSetMode(WORD wFormat, WORD wFramerate, DWORD dwBPF)
{
	m_csMico.Lock();

	if (!CheckEncoderFormat(wFormat))
	{
		m_csMico.Unlock();
		return MICO_ERROR;
	}

	if (!CheckEncoderFramerate(wFramerate))
	{
		m_csMico.Unlock();
		return MICO_ERROR;
	}

	if (!CheckEncoderBPF(dwBPF))
	{
		m_csMico.Unlock();
		return MICO_ERROR;
	}
	
	if (m_pZoran)
		m_pZoran->SetFormat(wFormat);

	if (m_pZoran)
		m_pZoran->SetFramerate(wFramerate);
	
	if (m_pZoran)
		m_pZoran->SetBPF(dwBPF);

	m_wFormat		= wFormat;
 	m_wFramerate	= wFramerate;
	m_dwBPF			= dwBPF;

	m_csMico.Unlock();

	return TRUE;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
int CMiCo::GetEncoderState()
{
	
	m_csMico.Lock();
	int nRet = MICO_ENCODER_OFF;

	if (m_pZoran)
		nRet = m_pZoran->GetEncoderState();
	
	m_csMico.Unlock();

	return nRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
JPEG* CMiCo::ReadData(int &nErrorCode)
{
	JPEG* pJpeg = NULL;
	
	if (m_pZoran)
		pJpeg = m_pZoran->ReadData(nErrorCode);
	
	return pJpeg;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{video decoding:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::DecoderStart()
{
	m_csMico.Lock();

	if (m_bTraceStartStopDecoder)
		WK_TRACE("CMiCo::DecoderStart\n");
	
	BOOL bRet = FALSE;
	
	if (m_pZoran)
		bRet = m_pZoran->StartDecoding();

	m_csMico.Unlock();

	return bRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::DecoderStop()	
{
	m_csMico.Lock();

	if (m_bTraceStartStopDecoder)
		WK_TRACE("CMiCo::DecoderStop\n");

	BOOL bRet = FALSE;
	
	if (m_pZoran)
		bRet = m_pZoran->StopDecoding();

	m_csMico.Unlock();

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
int CMiCo::GetDecoderState()
{
	m_csMico.Lock();

	int nRet = MICO_DECODER_OFF;
	
	if (m_pZoran)
		nRet = m_pZoran->GetDecoderState();

	m_csMico.Unlock();

	return nRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
DWORD CMiCo::WriteData(LPCSTR pData, DWORD dwDataLen)
{
	m_csMico.Lock();
	DWORD dwRet = 0L;
	
	if (m_pZoran)
		dwRet = m_pZoran->WriteData(pData, dwDataLen);

	m_csMico.Unlock();
	
	return dwRet;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{parameter check:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckSource(WORD wSource)
{
	switch(wSource)
	{
		case MICO_SOURCE0: 
		case MICO_SOURCE1: 
		case MICO_SOURCE2: 
		case MICO_SOURCE3: 
		case MICO_SOURCE4: 
		case MICO_SOURCE5: 
		case MICO_SOURCE6: 
		case MICO_SOURCE7:
			return TRUE;
		default:
			m_wLastError = MICO_ERR_WRONG_SOURCE;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckExtCard(WORD wExtCard)
{
	switch(wExtCard)
	{
		case MICO_EXTCARD0: 
		case MICO_EXTCARD1: 
		case MICO_EXTCARD2: 
		case MICO_EXTCARD3:
			return TRUE;
		default:
			m_wLastError = MICO_ERR_WRONG_EXT_CARD;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@MHEAD{parameter check:}
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckSourceInstalled(WORD wSource, WORD wExtCard)
{
	if (!CheckSource(wSource) || !CheckExtCard(wExtCard))
		return FALSE;

	return m_MiCoISA.ExtCard[wExtCard].Source[wSource].bPresent;
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckFeID(WORD wFeID)
{
	switch(wFeID)
	{
		case MICO_FE0: 
		case MICO_FE1:
        	return TRUE;
		default:
			m_wLastError = MICO_ERR_WRONG_FE_ID;
			return FALSE;
	}		
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckSourceType(WORD wSourceType)
{
	switch(wSourceType)
	{
		case MICO_FBAS: 
			if (m_bTraceSetFeSourceType)
				WK_TRACE("SetFeSourcetype FBAS\n");
        	return TRUE;
		
		case MICO_SVHS: 
			if (m_bTraceSetFeSourceType)
				WK_TRACE("SetFeSourcetype SVHS\n");
        	return TRUE;

		default:
			if (m_bTraceSetFeSourceType)
				WK_TRACE("SetFeSourcetype Unknown FeSourceType\n");
			m_wLastError = MICO_ERR_WRONG_SOURCE_TYPE;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckFeVideoFormat(WORD wFeVideoFormat)
{
	switch(wFeVideoFormat)
	{
		case MICO_PAL_SQUARE: 
			if (m_bTraceSetFeVideoFormat)
				WK_TRACE("SetFeVideoType PAL Square\n");
        	return TRUE;

		case MICO_NTSC_SQUARE: 
			if (m_bTraceSetFeVideoFormat)
				WK_TRACE("SetFeVideoType NTSC Square\n");
        	return TRUE;

		case MICO_PAL_CCIR: 
			if (m_bTraceSetFeVideoFormat)
				WK_TRACE("SetFeVideoType PAL CCIR\n");
        	return TRUE;

		case MICO_NTSC_CCIR:
			if (m_bTraceSetFeVideoFormat)
				WK_TRACE("SetFeVideoType NTSC CCIR\n");
        	return TRUE;

		default:
			if (m_bTraceSetFeVideoFormat)
				WK_TRACE("SetFeVideoType Unknown FeVideoFormat\n");
			m_wLastError = MICO_ERR_WRONG_FE_VIDEO_FORMAT;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckBeVideoFormat(WORD wBeVideoFormat)
{
	switch(wBeVideoFormat)
	{
		case MICO_PAL_SQUARE: 
			if (m_bTraceSetBeVideoFormat)
				WK_TRACE("SetBeVideoFormat PAL Square\n");
			return TRUE;

		case MICO_NTSC_SQUARE: 
			if (m_bTraceSetBeVideoFormat)
				WK_TRACE("SetBeVideoFormat NTSC Square\n");
			return TRUE;
		
		case MICO_PAL_CCIR: 
			if (m_bTraceSetBeVideoFormat)
				WK_TRACE("SetBeVideoFormat PAL CCIR\n");
			return TRUE;
		
		case MICO_NTSC_CCIR:
			if (m_bTraceSetBeVideoFormat)
				WK_TRACE("SetBeVideoFormat NTSC CCIR\n");
			return TRUE;

		default:
			if (m_bTraceSetBeVideoFormat)
				WK_TRACE("SetBeVideoFormat Unknown BeVideoFormat\n");
			m_wLastError = MICO_ERR_WRONG_BE_VIDEO_FORMAT;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckBeVideoType(WORD wBeVideoType)
{
	switch(wBeVideoType)
	{
		case MICO_COMPOSITE: 
			if (m_bTraceSetBeVideoType)
				WK_TRACE("SetBeVideoType Composite\n");
	       	return TRUE;

		case MICO_RGB: 
			if (m_bTraceSetBeVideoType)
				WK_TRACE("SetBeVideoType RGB\n");
	       	return TRUE;

		default:
			if (m_bTraceSetBeVideoType)
				WK_TRACE("SetBeVideoType Unknown BeVideoType\n");
			m_wLastError = MICO_ERR_WRONG_BE_VIDEO_TYPE;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckFeFilter(WORD wFilter)
{
	if (wFilter <= (MICO_LNOTCH | MICO_LDEC))
	{
		if (m_bTraceSetFeFilter)
			WK_TRACE("SetFeFilter Notch and LDec\n");
		return TRUE;
	}
	else
	{
		if (m_bTraceSetFeFilter)
			WK_TRACE("SetFeFilter Unknown FeFilter\n");
		m_wLastError = MICO_ERR_WRONG_FILTER_TYPE;
		return FALSE;
	}
}
 
/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckEncoderFormat(WORD wFormat)
{
	switch(wFormat)
	{
		case MICO_ENCODER_HIGH: 
			if (m_bTraceSetFormat)
				WK_TRACE("EncoderSetFormat High\n");
        	return TRUE;		
		
		case MICO_ENCODER_LOW: 
			if (m_bTraceSetFormat)
				WK_TRACE("EncoderSetFormat Low\n");
			return TRUE;

		default:
			if (m_bTraceSetFormat)
				WK_TRACE("EncoderSetFormat unknown Format\n");
			m_wLastError = MICO_ERR_WRONG_ENCODER_FORMAT;
			return FALSE;
	}		
} 

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckEncoderFramerate(WORD wFramerate)
{
	if ((wFramerate >= MICO_MIN_FPS) && (wFramerate <= MICO_MAX_FPS))
		return TRUE;

	m_wLastError = MICO_ERR_WRONG_ENCODER_FRAMERATE;
	return FALSE;
}

/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckEncoderBPF(DWORD dwBPF)
{
	if ((dwBPF >= MICO_MIN_BPF) && (dwBPF <= MICO_MAX_BPF))
		return (TRUE);
	else
	{
		m_wLastError = MICO_ERR_WRONG_ENCODER_BPF;
		return FALSE;
	}		
} 
 
/*/////////////////////////////////////////////////////////////////////////////
@MD 
@CW{Parameter:}
	NYD
@HRULE
@CW{Rückgabe:}
	NYD
@HRULE
@CW{Funktion:}
	NYD
*/////////////////////////////////////////////////////////////////////////////
BOOL CMiCo::CheckRange(LPWORD lpwValue, long lMin, long lMax)
{
	if (!lpwValue)
	{
		WK_TRACE_ERROR("CMiCo::CheckRange\tlpwValue = Null\n");
		return FALSE;
	}

	if (*lpwValue < (WORD)lMin)
	{
		m_wLastError = MICO_ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMin;
		return FALSE;
	}
	else if (*lpwValue > (WORD)lMax)
	{
		m_wLastError = MICO_ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMax;
		return FALSE;
	
	}
	return TRUE;
}

