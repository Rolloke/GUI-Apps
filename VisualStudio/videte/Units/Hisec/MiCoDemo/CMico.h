////////////////////////////////////////////////////////////////////////////
//	Project:	MiCoISA
//
//	File:		cmico.h 
//
//	Description:		
//
//	Start:		30.11.95
//
//	Last change:01.12.95
//
//	Author:		Martin Lück
//
//	Copyright:	w+k Video Communication GmbH & Co.KG
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CMICO_H__
#define __CMICO_H__

#include "MiCoDefs.h"
#include "CIo.h"

class CBT829;
class CBT856;
class CRelay;
class CAlarm;
class CDlgColor;
class CZoran;
class CIo;


/////////////////////////////////////////////////////////////////////////////
// CMiCo
class CMiCo
{
public:
	CMiCo(HWND hWnd, const CString &sAppIniName);
	// Konstrunktor:Das Anlegen eines CMiCo-Objektes bewirkt die komplette Initialisierung
	//				der Hardware.
	// hWnd			= An dieses Fenster verschickt der Treiber Meldungen, wenn nötig.
	// lpIniAppName = Pointer auf den Namen der INI-Datei

	~CMiCo();
	// Der Destruktor gibt alle vom CMiCo-Objekt benutzten Resourcen frei.

	BOOL IsValid();
	// Liefert TRUE, wenn das CMiCo-Objekt fehlerfrei angelegt wurde, ansonsten FALSE.

	BOOL SaveConfig();
	// Sichert alle wichtigen Einstellungen in der INI-Datei

	WORD GetVersion();
	// Liefert eine Versionsnummer des MiCo-Treibers

	WORD GetLastError();
	// Liefert einen der folgenden Fehlerkodes zurück.
	//	-	MICO_ERR_NO_ERRROR				
	//	-	MICO_ERR_WRONG_FE_ID			 
	//	-	MICO_ERR_WRONG_SOURCE			 
	//	-	MICO_ERR_WRONG_EXT_CARD			 
	//	-	MICO_ERR_WRONG_SOURCE_TYPE		 
	//	-	MICO_ERR_WRONG_FE_VIDEO_FORMAT   
	//	-	MICO_ERR_WRONG_BE_VIDEO_FORMAT   
	//	-	MICO_ERR_WRONG_BE_VIDEO_TYPE	 
	//	-	MICO_ERR_PARM_OUT_OF_RANGE		 	
	//	-	MICO_ERR_UNKNOWN_PARAMETER  	 
	//	-	MICO_ERR_WRONG_FILTER_TYPE		 
	//	-	MICO_ERR_WRONG_ENCODER_FORMAT	 
	//	-	MICO_ERR_WRONG_ENCODER_FRAMERATE 
	//	-	MICO_ERR_WRONG_ENCODER_BPF		 
	//	-	MICO_ERROR						
	// Das Lesen bewirkt das Rücksetzen des Fehlerstatuses.

	BOOL CheckHardware();
	// Testet die Hardware
	// Liefert TRUE, wenn kein Fehler erkannt wurde, ansonsten FALSE
	// Note: Not implemented

	BOOL PollAlarm();
	// Pollt auf die Alarmeingänge. Diese Funktion muß regelmäßig aufgerufen werden.
	// Wenn ein Alarm festgestellt wurde verschickt das CMiCo-Objekt eine
	// WM_MICO_ALARM Message an das Fenster, dessen Fensterhandle im Konstruktor
	// von CMiCo angegeben wurde (wParam=Extcard lParam = CurrentAlarmState).
	// Liefert TRUE, wenn wenigstens ein Alarm detektiert wurde.

	BOOL IsVideoPresent(WORD &wExtCard, WORD &wSource);
	// Überprüft ob am aktuell gewählten Eingang ein Videosignal anliegt.
	// wExtCard = MICO_EXTCARD0....MICO_EXTCARD3
	// wSource	= MICO_SOURCE0...MICO_SOURCE7
	// Rückgabe:  TRUE, wenn die aktuelle Kamera ein Videosignal liefert
	// Note:	  wExtCard und wSource enthalten nach dem Aufruf die aktuelle
	//			  Videoquelle/Erweiterungskarte.

	int GetEncoderState();
	// Liefert den aktuellen Status des Encoders (MICO_ENCODER_ON/MICO_ENCODER_OFF)

	int GetDecoderState();
	// Liefert den aktuellen Status des Decoders (MICO_DECODER_ON/MICO_DECODER_OFF)

	DWORD GetHardwareState();
	// Gibt Auskunft darüber welche Karten (MiCo's, ExtCard's) im System eingebunden sind
	// Rückgabe:	Bit 0...3  -> 	MiCo, CoVi1...CoVi3
	// Gesetztes Bit heißt Karte in INI-Datei eingetragen

	DWORD SetInputSource(WORD wExtCard, WORD wSource, WORD wFormat, DWORD dwBPF, DWORD dwProzessID);
	// Schaltet auf die gewünschte Videoquelle um.
	// Das Umschalten auf eine andere Eingangsquelle bewirkt das Setzen der ihr
	// zugeordneten Einstellungen (VideoFormat)
	// wExtCard		= MICO_EXTCARD0....MICO_EXTCARD3
	// wSource		= MICO_SOURCE0...MICO_SOURCE7
	// wFormat		= MICO_ENCODER_HIGH, MICO_ENCODER_LOW
	// dwBPF		= Anzahl der Bytes pro JPEG-File
	// dwProzessID	= Eine eindeutige ID, die jedes Bild einem Prozeß zuordnet
	// Rückgabe: LOWWORD->Bisherige Videoquelle			(MICO_SOURCE0...MICO_SOURCE7)
	//			 HIWORD ->Bisherige Erweiterungskarte	(MICO_EXTCARD0....MICO_EXTCARD3)
	//			 Im Fehlerfall MICO_ERROR

	void GetInputSource(WORD &wExtCard, WORD &wSource);
	// Liefert die aktuelle Videoquelle und Erweiterungskarte
	// Rückgabe: LOWWORD->Aktuelle Videoquelle			(MICO_SOURCE0...MICO_SOURCE7)
	//			 HIWORD ->Aktuelle Erweiterungskarte	(MICO_EXTCARD0....MICO_EXTCARD3)

	WORD SetFeSourceType(WORD wSourceType);
	// Setzt den SourceType der aktuellen Videoquelle
	// wSourceType	= MICO_FBAS, MICO_SVHS
	// Rückgabe: Bisherigen SourceType

	WORD GetFeSourceType();
	// Liefert den SourceType der aktuell gewählten Videoquelle (MICO_FBAS, MICO_SVHS)
	
	WORD SetFeVideoFormat(WORD wFeVideoFormat);
	// Setzt das FrontEnd-Videoformat
	// wFeVideoFormat	= MICO_PAL_SQUARE, MICO_NTSC_SQUARE, MICO_PAL_CCIR, MICO_NTSC_CCIR)
	// Rückgabe: Bisheriges FrontEnd-Videoformat

	WORD GetFeVideoFormat();
	// Liefert das FrontEnd-Videoformat (MICO_PAL, MICO_NTSC) 
	
	WORD SetBeVideoFormat(WORD wBeVideoFormat);
	// Setzt das BackEnd-Videoformat
	// wBeVideoFormat = MICO_PAL, MICO_NTSC 
	// Rückgabe: Bisheriges	BackEnd-Videoformat.

	WORD GetBeVideoFormat();
	// Liefert das aktuell gewählte BackEnd-Videoformat. (MICO_PAL, MICO_NTSC)

	WORD SetBeVideoType(WORD wBeVideoType);
	// Setzt die BackEnd-Videoausgabe Art
	// wBeVideoFormat = MICO_COMPOSITE, MICO_RGB 
	// Rückgabe: Bisherige	BackEnd-Videoausgabeart.

	WORD GetBeVideoType();
	// Liefert die aktuell gewählte BackEnd-Videoausgabeart. (MICO_COMPOSITE, MICO_RGB)

	BOOL DlgLuminance();
	// Öffnet einen modless Dialog zur Einstellung von Kontrast, Helligkeit, Farbsättigung und Hue

	WORD SetFeBrightness(WORD wBrightness);
	// Setzt die Helligkeit
	// wBrightness	 = Helligkeitseinstellung (MICO_MIN_BRIGHTNESS...MICO_MAX_BRIGHTNESS)
	// Rückgabe: Bisheriges Helligkeitseinstellung
	// Notes:	 liegt wBrightness außerhalb der gültigen Grenzen, wird diese begrenzt.

	WORD GetFeBrightness();
	// Liefert die Helligkeitseinstellung
	// Der Wert liegt innerhalb der Grenzen (MICO_MIN_BRIGHTNESS...MICO_MAX_BRIGHTNESS)

	WORD SetFeContrast(WORD wContrast);
	// Setzt den Kontrast
	// wContrast	 = Kontrasteinstellung (MICO_MIN_CONTRAST...MICO_MAX_CONTRAST)
	// Rückgabe: Bisherige Kontrasteinstellung
	// Notes:	 liegt wContrast außerhalb der gültigen Grenzen, wird dieser begrenzt.

	WORD GetFeContrast();
	// Liefert die Kontrasteinstellung
	// Der Wert liegt innerhalb der Grenzen (MICO_MIN_CONTRAST...MICO_MAX_CONTRAST)

	WORD SetFeSaturation(WORD wSaturation);
	// Setzt die Farbsättigung
	// wSaturation	 = Farbsättigung (MICO_MIN_SATURATION...MICO_MAX_SATURATION)
	// Rückgabe: Bisherige Farbsättigung
	// Notes:	 liegt wSaturation außerhalb der gültigen Grenzen, wird diese begrenzt.

	WORD GetFeSaturation();
	// Liefert die Farbsättigung
	// Der Wert liegt innerhalb der Grenzen (MICO_MIN_SATURATION...MICO_MAX_SATURATION)

	WORD SetFeHue(WORD wHue);
	// Setzt den 'Hue'
	// wHue	 = Farbsättigung (MICO_MIN_HUE...MICO_MAX_HUE)
	// Rückgabe: Bisherige Farbsättigung
	// Notes:	 liegt wHue außerhalb der gültigen Grenzen, wird diese begrenzt.

	WORD GetFeHue();
	// Liefert den 'Hue'
	// Der Wert liegt innerhalb der Grenzen (MICO_MIN_HUE...MICO_MAX_HUE)

	WORD SetFeHScale(WORD wHScale);
	// Setzt die horizontale Skalierung im BT829
	// wHScale = horizontaler Skalierungswert (MICO_MIN_HSCALE...MICO_MAX_HSCALE)
	// Rückgabe bisherige Skalierung
	// Notes:	 liegt wHue außerhalb der gültigen Grenzen, wird dieser begrenzt.

	WORD GetFeHScale();
	// Liefert die aktuelle horizontale Skalierung des BT829
		
	WORD SetFeVScale(WORD wVScale);
	// Setzt die vertikale Skalierung im BT829
	// wVScale = horizontaler Skalierungswert (MICO_MIN_VSCALE...MICO_MAX_VSCALE)
	// Rückgabe: Bisherige Skalierung

	WORD GetFeVScale();
	// Liefert die aktuelle vertikale Skalierung des BT829

	WORD SetFeFilter(WORD wFilter);
	// Aktiviert bzw. deaktiviert die beiden Filter des BT829
	// wFilter = MICO_LNOTCH_ON, MICO_LNOTCH_OFF
	//			 MICO_LDEC_ON, MICO_LDEC_OFF
	// Rückgabe: Bisherige Filtereinstellung.

	WORD GetFeFilter();
	// Liefert die augenblickliche Filtereinstellung des BT829

	BYTE GetCurrentAlarmState(WORD wExtCard);
	// Liefert den aktuellen Inhalt des CurrentAlarmstatus Registers, bzw. im 
	// Fehlerfall MICO_ERROR
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// Rückgabe: CurrentAlarmstatus: 00000000...11111111

	BYTE GetAlarmState(WORD wExtCard);
	// Liefert den aktuellen Inhalt des Alarmstatus Registers, bzw. im
	// Fehlerfall MICO_ERROR.
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// Rückgabe: Alarmstatus: 00000000...11111111
	
	BYTE SetAlarmEdge(WORD wExtCard, BYTE byEdge);
	// Legt Bitweise die Alarmflanke fest
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// byEdge = 00000000B...11111111B
	// Rückgabe: Die bisherige Einstellung

	BYTE GetAlarmEdge(WORD wExtCard);
	// Liefert die augenblicklich eingestellte AlarmEdge der gewünschten Karte
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// Rückgabe: wEdge = 00000000B...11111111B

	BYTE SetAlarmAck(WORD wExtCard, BYTE byAck);
	// Sperrt Bitweise die Alarmeingänge
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// byAck	= 00000000B...11111111B
	// Rückgabe: Die bisherige Einstellung

	BYTE GetAlarmAck(WORD wExtCard);
	// Liefert den augenblicklich eingestellten AlarmAcknoledge der gewünschten Karte
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// Rückgabe: wAck = 00000000B...11111111B

	BYTE SetRelais(WORD wExtCard, BYTE byRelais);
	// Setzt die Relaisausgänge der gewünschten Erweiterungskarte
	// wExtCard = MICO_EXTCARD0...MICO_EXTCARD3
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// wRelais	= xxxx0000B...xxxx1111B
	// Rückgabe: Bisherige Relaiseinstellung
	
	BYTE GetRelais(WORD wExtCard);
	// wExtCard = MICO_EXTCARD0... MICO_EXTCARD3, MICO_EXTCARD_CURRENT)
	// Rückgabe: Aktuelle Relaiseinstellung	der gewünschten Karte.

	BOOL EncoderStart();
	// Startet den Encoder. Sobald der Encoder läuft, können die Bilddaten mit Hilfe
	// der Funktion 'ReadData()' gelesen werden.

	BOOL EncoderStop();
	// Stopt den Encoder

	WORD EncoderSetFormat(WORD wEncFormat);
	// wEncFormat = MICO_ENCODER_LOW, MICO_ENCODER_HIGH
	// Rückgabe: Bisherige Einstellung, bzw. MICO_ERROR

	WORD EncoderGetFormat();
	// Liefert die gewählte Encoderauflösung

	WORD EncoderSetFramerate(WORD wFramerate); 
	// wFramerate = Frames / Second
	// Rückgabe: Bisherige Einstellung, bzw. MICO_ERROR
	// Wird nicht ausgewertet, die MiCoUnit liefert immer 25fps

	WORD EncoderGetFramerate(); 
	// Liefert die gewählte Framerate
	
	DWORD EncoderSetBPF(DWORD dwBPF);
	// Setzt die Byte pro Frame

	DWORD EncoderGetBPF();
	// Liefert die Byte pro Frame

	BOOL EncoderSetMode(WORD wEncFormat, WORD wFramerate, DWORD dwBPF);
	// Setzt Format, Framerate und Bytes pro Frame
	// wEncFormat = MICO_ENCODER_LOW, MICO_ENCODER_HIGH
	// wFramerate = Frames / Second
	// dwBPF	  = Setzt die Byte pro Frame

	BOOL DecoderStart();
	// Startet den Decoder, sobald der Decoder gestartet ist, können Daten mit Hilfe
	// der Funktion	'WriteData()' an den Decoder verschickt werden.

	BOOL DecoderStop();	
	// Stopt den Decoder

	JPEG* ReadData(int &nErrorCode);
	// Liest Bilddaten aus dem SharedMemory und stellt diese in Form einer JPEG-Struktur
	// zur Verfügung. Die Jpegstruktur enthält neben den eigendlichen JPeg-Bilddaten z.B.
	// noch die Kameranummer.
	// Die Funktion kehrt erst nach erhalt eines Bildes zurück, spätestens aber nach einer
	// in der INI-Datei spezifizierten Zeit. ([MICOISA]	TimeOut=1000) Typischerweise
	// 1000ms.
	// Rückgabe: Pointer auf eine Jpegstruktur oder im Fehlerfall NULL;

	DWORD WriteData(LPCSTR pData, DWORD dwDataLen);
	// Überträgt ein Jpeg-Bild an den Decoder
	// pData		= Pointer auf die Bilddaten
	// dwDataLen	= Größe des Jpegbildes in Bytes.
	// Rückgabe: Anzahl der übertragenen Bytes.

	BOOL  m_bRun; // TRUE -> solange der 'PollAlarm'-Thread läuft.

private:
	void ResetVideoInterface();
	// Führt einen Hardwarereset des Mico Videofrontends durch.

	void Init();
	// Lädt Initialisierungseinstellungen aus der INI Datei und Konfiguriert die MiCo

	// Ein Satz von Member, die die Übergabeparameter überprüfen.
	BOOL CheckSource(WORD wSource);
	BOOL CheckExtCard(WORD wExtCard);
	BOOL CheckSourceInstalled(WORD wSource, WORD wExtCard);

	BOOL CheckFeID(WORD wFeID);
	BOOL CheckSourceType(WORD wSourceType);
	BOOL CheckFeVideoFormat(WORD wFeVideoFormat);
	BOOL CheckBeVideoFormat(WORD wBeVideoFormat);
	BOOL CheckBeVideoType(WORD wBeVideoType);
	BOOL CheckFeFilter(WORD wFilter);
	BOOL CheckEncoderFormat(WORD wFormat);
	BOOL CheckEncoderFramerate(WORD wFramerate);
	BOOL CheckEncoderBPF(DWORD dwBPF);
	BOOL CheckRange(LPWORD lpwValue, long lMin, long lMax);
	
	BOOL  m_bOK;
	WORD  m_wLastError;						// Status des Objektes. TRUE=OK
	WORD  m_wSource;						// Aktuelle Videoquelle (MICO_SOURCE0...MICO_SOURCE7)
	WORD  m_wExtCard;						// Aktueller Erweiterungskarte (MICO_EXTCARD0...MICO_EXTCARD3)
	WORD  m_wFrontEnd;						// Aktuelles Frontend
	WORD  m_wFormat;						// Bildformat											
	WORD  m_wFramerate;						// Bildrate
	WORD  m_wBeVideoType;					// ´FBAS/RGB
	WORD  m_dwHardwareState;				// Gibt über verfügbare Hardware auskunft.
	BYTE  m_byAlarmState[MICO_MAX_EXTCARD];	// Enthält den letzten Alarmstatus
	BYTE  m_byMicoID;						// Mico Identifier
	DWORD m_dwBPF;							// Bytes pro Frame

	MICOISASTRUCT	m_MiCoISA;
	CDlgColor*		m_pDlgColor;			// Pointer auf Dialog
	CWinThread*		m_pAlarmPollThread;		// PollAramThread
	HWND	m_hWndClient;					// Fensterhandle der Anwendung
	CString m_sAppIniName;					// INI-Filename
	CRelay* m_pRelay[MICO_MAX_EXTCARD+1];		// Pointer auf Relay-Objekte
	CAlarm* m_pAlarm[MICO_MAX_EXTCARD+1];		// Pointer auf Alarm-Objekte
	CBT829* m_pBT829[MICO_MAX_FE];			// Pointer auf BT829-Objekte
	CBT856* m_pBT856;						// Pointer auf BT856-Objekt
	CZoran* m_pZoran;						// Pointer auf Zoran-Objekt.
	CIo		m_Io;							// IO-Access
	CONFIG	m_Config;						// Konfigurationsparameter
	CCriticalSection	m_csMico;			// Synchronisationsobjekt

	// Tracemeldungen (An=TRUE / Aus=FALSE)
	BOOL	m_bTraceStartStopEncoder;
	BOOL	m_bTraceStartStopDecoder;
	BOOL	m_bTraceSetBPF;
	BOOL	m_bTraceSetFormat;
	BOOL	m_bTraceSetFeSourceType;
	BOOL	m_bTraceSetFeVideoFormat;
	BOOL	m_bTraceSetBeVideoType;
	BOOL	m_bTraceSetBeVideoFormat;
	BOOL	m_bTraceSetFeBrightness;
	BOOL	m_bTraceSetFeContrast;
	BOOL	m_bTraceSetFeSaturation;
	BOOL	m_bTraceSetFeHue;
	BOOL	m_bTraceSetFeFilter;
	BOOL	m_bTraceSetInputSource;
	BOOL	m_bTraceSetRelais;
	BOOL	m_bTraceSetAlarmEdge;
	BOOL	m_bTraceSetAlarmAck;
	BOOL	m_bTraceAlarmIndication;
};

#endif // __CMICO_H__

