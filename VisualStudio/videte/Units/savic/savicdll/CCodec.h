/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CCodec.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/CCodec.h $
// CHECKIN:		$Date: 15.01.03 11:53 $
// VERSION:		$Revision: 41 $
// LAST CHANGE:	$Modtime: 15.01.03 11:27 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CCODEC_H__
#define __CCODEC_H__

#include "WK_RuntimeError.h"
#include "CMyJpeg.h"

class CPCIDevice;
class CPCF8582;
class CPCF8563;
class CBT878;
class CRelay;
class CAlarm;
class CWatchDog;
class CProfile;
class CDlgColor;
class CMDConfigDlg;
class CMDPoints;
class CIPCActivityMask;
class CMotionDetection;
class CVideoInlay;

/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CCodec
{
public:
	virtual BOOL InitPll();
	CCodec(const CString &sAppIniName, CPCIDevice* pDevice);

	virtual ~CCodec();
	// Zerstört das CCodec-Objekt

	virtual BOOL Open();
	// Diese Funktion muß nach dem Anlegen eines CCodec-Objektes aufgerufen werden, um
	// das Objekt korrekt zu initialisieren.
	// Rückgabe: TRUE -> Kein Fehler aufgetreten, ansonsten FALSE

	virtual BOOL Close();
	// Diese Funktion muß vor dem Beenden des Programmes aufgerufen werden, um das CCodec-
	// Objekt zu schließen. Close ruft die Funktion 'SaveConfig' auf.
	// Rückgabe: TRUE -> Kein Fehler aufgetreten, ansonsten FALSE

	virtual BOOL IsValid() const; 
	// Prüft ob das CCodec-Objekt konsistent ist.
	// Rückgabe: TRUE -> Kein Fehler aufgetreten, ansonsten FALSE

	virtual DWORD GetVersion() const;
	// Liefert eine Versionsnummer des SaVic-Treibers

	virtual WORD GetLastError();
	// Liefert einen der folgenden Fehlerkodes zurück.
	//	-	ERR_NO_ERROR				
	//	-	ERR_WRONG_FE_ID			 
	//	-	ERR_WRONG_SOURCE			 
	//	-	ERR_WRONG_EXT_CARD			 
	//	-	ERR_WRONG_SOURCE_TYPE		 
	//	-	ERR_WRONG_FE_VIDEO_FORMAT   
	//	-	ERR_PARM_OUT_OF_RANGE		 	
	//	-	ERR_UNKNOWN_PARAMETER  	 
	//	-	ERR_WRONG_FILTER_TYPE		 
	//	-	ERR_WRONG_ENCODER_FORMAT	 
	//	-	ERR_WRONG_ENCODER_BPF		 
	//	-	DEFAULT_ERROR						
	// Das Lesen bewirkt das Rücksetzen des Fehlerstatuses.

	virtual int GetEncoderState() const;
	// Liefert den aktuellen Status des Encoders (ENCODER_ON/ENCODER_OFF)

	virtual BOOL SetFramerate(WORD wFps);
	// Setzt die Framerate. Die Einhaltung der Framerate wird intern im BT878 durch
	// droppen von Bildern realisiert.

	virtual BOOL GetFramerate(WORD& wFps);

	virtual WORD SetInputSource(SOURCE_SELECT_STRUCT &CurrSrcSel);
	// Schaltet auf die gewünschte Videoquelle um.
	// Das Umschalten auf eine andere Eingangsquelle bewirkt das Setzen der ihr
	// zugeordneten Einstellungen (VideoFormat, Brightness, Contrast, Saturation)
	// SrcSel.wSource		= VIDEO_SOURCE0...VIDEO_SOURCE3, VIDEO_SOURCE_CURRENT
	// SrcSel.wFormat		= ENCODER_RES_HIGH, ENCODER_RES_LOW
	// SrcSel.dwBPF			= Anzahl der Bytes pro JPEG-File
	// SrcSel.dwProzessID	= Eine eindeutige ID, die jedem Bild einem Prozeß zuordnet
	// NextSrcSel			= Ist bei der SaVic-Karte obsolet, da nur ein Frontend vorhanden ist
	// Rückgabe: Bisherige Videoquelle			(VIDEO_SOURCE0...VIDEO_SOURCE3)
	//			 Im Fehlerfall DEFAULT_ERROR
	// Note: Das eigendliche Umschalten muß die Unterklasse erledigen, dazu
	//		 werden die folgenden beiden virtuellen Funktionen von der Basisklasse aufgerufen.
	// 		 'DoAnalogSwitch',
	//		 'SetInputSource' ergänzt einige Einträge der 'SrcSel'-Struktur
	//		 (SrcSel.byPortY)

	virtual WORD GetInputSource();
	// Liefert die aktuell verwendete Videoquelle und Erweiterungskarte
	//	(VIDEO_SOURCE0...VIDEO_SOURCE3)

	BOOL WaitForSourceSwitch(SOURCE_SELECT_STRUCT &SrcSel);
	// Warte bis die Kamera gewechselt wurde.
	// Rückgabe: FALSE->Es kam zu einem Timeout

	BOOL SetCameraToAnalogOut(WORD wSource, const CString& sPort);
	// Schaltet die gewünschte Kamera auf den gewünschten externen Ausgang
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE3, VIDEO_SOURCE_CURRENT)
	// sPort = CSD_PORT0, CSD_PORT1, CSD_PORT2, CSD_PORT3

	virtual BOOL SetVideoFormat(WORD wFeVideoFormat);
	// Setzt das FrontEnd-Videoformat
	// wFeVideoFormat	= VFMT_PAL_CCIR, VFMT_NTSC_CCIR)
	// Rückgabe: TRUE->Kein Fehler erkannt

	virtual WORD GetVideoFormat() const;
	// Liefert das FrontEnd-Videoformat (VFMT_PAL_PAL, VFMT_NTSC_CCIR) 
	
	virtual BOOL DlgLuminance() const;
	// Öffnet einen modless Dialog zur Einstellung von Kontrast, Helligkeit, Farbsättigung und Hue

	virtual BOOL SetBrightness(WORD wSource, WORD wBrightness, BOOL bPassThrough=FALSE);
	// Setzt die Helligkeit
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wBrightness	 = Helligkeitseinstellung (MIN_BRIGHTNESS...MAX_BRIGHTNESS)
	// bPassThrough	 = TRUE->Der Wert wird gleich an den BT879 weitergereicht.
	//				 = FALSE->Der Wert wird erst beim nächsten Umschalten auf die Kamera übertragen
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 Liegt wBrightness außerhalb der gültigen Grenzen, wird diese begrenzt.

	virtual WORD GetBrightness(WORD wSource);
	// Liefert die Helligkeitseinstellung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_BRIGHTNESS...MAX_BRIGHTNESS)

	virtual BOOL SetContrast(WORD wSource, WORD wContrast, BOOL bPassThrough=FALSE);
	// Setzt den Kontrast
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wContrast	 = Kontrasteinstellung (MIN_CONTRAST...MAX_CONTRAST)
	// bPassThrough	 = TRUE->Der Wert wird gleich an den BT879 weitergereicht.
	//				 = FALSE->Der Wert wird erst beim nächsten Umschalten auf die Kamera übertragen
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 liegt wContrast außerhalb der gültigen Grenzen, wird dieser begrenzt.

	virtual WORD GetContrast(WORD wSource);
	// Liefert die Kontrasteinstellung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_CONTRAST...MAX_CONTRAST)

	virtual BOOL SetSaturation(WORD wSource, WORD wSaturation, BOOL bPassThrough=FALSE);
	// Setzt die Farbsättigung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wSaturation	 = Farbsättigung (MIN_SATURATION...MAX_SATURATION)
	// bPassThrough	 = TRUE->Der Wert wird gleich an den BT879 weitergereicht.
	//				 = FALSE->Der Wert wird erst beim nächsten Umschalten auf die Kamera übertragen
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 liegt wSaturation außerhalb der gültigen Grenzen, wird diese begrenzt.

	virtual WORD GetSaturation(WORD wSource);
	// Liefert die Farbsättigung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_SATURATION...MAX_SATURATION)

	virtual BOOL SetHue(WORD wSource, WORD wHue, BOOL bPassThrough=FALSE);
	// Setzt den 'Hue'
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wHue	 = Farbsättigung (MIN_HUE...MAX_HUE)
	// bPassThrough	 = TRUE->Der Wert wird gleich an den BT879 weitergereicht.
	//				 = FALSE->Der Wert wird erst beim nächsten Umschalten auf die Kamera übertragen
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 liegt wHue außerhalb der gültigen Grenzen, wird diese begrenzt.

	virtual WORD GetHue(WORD wSource);
	// Liefert den 'Hue'
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_HUE...MAX_HUE)

	virtual BOOL SetFilter(WORD wFilter);
	// Aktiviert bzw. deaktiviert die beiden Filter des BT829
	// wFilter = LNOTCH_FILTER, LDEC_FILTER
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 LNOTCH_FILTER und LDEC_FILTER könne bitweise geodert werden 

	virtual WORD GetFilter() const;
	// Liefert die augenblickliche Filtereinstellung des BT829

	virtual BOOL SetFeHScale(WORD wHScale);
	// Setzt die horizontale Skalierung im BT829
	// wHScale = horizontaler Skalierungswert (MIN_HSCALE...MAX_HSCALE)
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 Liegt wHScale außerhalb der gültigen Grenzen, wird dieser begrenzt.

	virtual BOOL GetFeHScale(WORD& wHSCale) const;
	// Liefert die aktuelle horizontale Skalierung des BT829
	
	virtual BOOL SetFeVScale(WORD wVScale);
	// Setzt die vertikale Skalierung im BT829
	// wVScale = horizontaler Skalierungswert (MIN_VSCALE...MAX_VSCALE)
	// Rückgabe: Kein Fehler erkannt
	// Notes:	 Liegt wVScale außerhalb der gültigen Grenzen, wird dieser begrenzt.

	virtual BOOL GetFeVScale(WORD& wVScale) const;
	// Liefert die aktuelle vertikale Skalierung des BT829

	virtual BOOL SetHActive(WORD wHActive) const;

	virtual WORD GetCurrentAlarmState();
	// Liefert den aktuellen Inhalt des CurrentAlarmstatus Registers. 
	// Rückgabe: CurrentAlarmstatus: 0000000000000000b...1111111111111111b

	virtual WORD SetAlarmEdge(WORD wEdge);
	// Legt Bitweise die Alarmflanke fest
	// wEdge = 0000000000000000B...1111111111111111B
	// Rückgabe: Die bisherige Einstellung

	virtual WORD GetAlarmEdge();
	// Liefert den aktuellen Inhalt des AlarmEdge-Registers
	// Rückgabe: wEdge = 0000000000000000B...1111111111111111B

	virtual void ResetSavedAlarmState();
	// Löscht den zuletzt gemeldeten Alarmzustand.

	virtual BYTE SetRelais(BYTE byRelais);
	// Setzt die Relaisausgänge.
	// wRelais	= xxxx0000B...xxxx1111B
	// Rückgabe: Bisherige Relaiseinstellung
	
	virtual BYTE GetRelais();
	// Rückgabe: Aktuelle Relaiseinstellung.

	virtual BOOL EncoderStart();
	// Startet den Encoder. Sobald der Encoder läuft, können die Bilder in der
	// der virtuellen Funktion, 'OnReceivedImageData', die überschrieben werden muß,
	// entgegengenommen werden.
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual BOOL EncoderStop();
	// Stopt den Encoder
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual BOOL RestartEncoder();
	// Stopt den Encoder...Initialisiert den PLL neu und starten den Encoder neu.
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual WORD EncoderGetFormat() const;
	// Liefert die gewählte Encoderauflösung (ENCODER_RES_LOW, ENCODER_RES_HIGH)

	virtual DWORD EncoderGetBPF() const;
	// Liefert die aktuell eingestelle 'Byte pro Frame'

	virtual BOOL VideoInlaySetOutputWindow(const CRect &rc);
	// Setzt das Begrenzungsrechteck beim Videoinlay
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.
	// Note: Die Keyfarbe ist RGB(255, 0, 255)
	
	virtual BOOL VideoInlayActivate();
	// Schaltet das Videoinlay ein
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual BOOL VideoInlayDeactivate();
	// Schaltet das Videoinlay aus
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual BOOL MotionDetectionConfiguration(WORD wSource);
	// Öffnet einen Dialog zur konfiguration des Motiondetection
	// wSource = Camera, für die die Masken verändert werden soll

	virtual BOOL ActivateMotionDetection(WORD wSource);
	// Aktiviert für die Kaera wSource die MotionDetection

	virtual BOOL DeactivateMotionDetection(WORD wSource);
	// Deaktiviert für die Kaera wSource die MotionDetection

	virtual BOOL SetMDMaskSensitivity(WORD wSource, const CString &sLevel);
	// Setzt die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll
	// sLevel = High, Normal, Low

	virtual BOOL SetMDAlarmSensitivity(WORD wSource, const CString &sLevel);
	// Setzt die Empfindlichkeit der Motiondetection in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll
	// wLevel = High, Normal, Low

	virtual CString GetMDMaskSensitivity(WORD wSource);
	// Liefert die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll

	virtual CString GetMDAlarmSensitivity(WORD wSource);
	// Liefert die Empfindlichkeit der Motiondetection in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll

	virtual BOOL GetMask(WORD wSource, CIPCActivityMask& mask);
	// Liefert für eine bestimmte Kamera die MD-Maske
	// wSource = Kamera, deren Maske angefordert wird.
	// Der Typ der Maske (permanent/dynamisch) muß in mask vorbelegt sein.

	virtual BOOL SetMask(WORD wSource, const CIPCActivityMask& mask);
	// Setzt für eine bestimmte Kamera die MD-Maske
	// wSource = Kamera, deren Maske gesetzt werden soll..

	virtual HDEVICE GetDevice(){return m_hDevice;}
	// Liefert ein Handle auf den Gerätetreiber.

	virtual DWORD ScanForCameras(DWORD dwScanMask);
	// Scant die Kameraeingänge durch und prüft, ob ein Videosignal vorhanden ist oder nicht.
	// Für die erste detektierte Kamera, die zuvor kein Videosignal geliefert hatte, wird
	// zusätzlich die Videonorm geprüft, und das System evtl. uminitialisiert.
	// dwScanMask = Bitmaske derjenigen Kameraeingänge, die erneunt gescannt werden sollen
	// Rückgabe: Bitmaske aller Kameraeingänge: 1=aktiv, 0=nicht aktiv

	virtual BOOL EnableDoubleFieldMode(WORD wSource, BOOL bEnable);
	// Schaltet für die gewählte Kamera in den Vollbildmodus (2fields) bzw.
	// in den Halbbildmodus
	// Rückgabe: Im Fehlerfall FALSE
	
	virtual BOOL IsDoubleFieldMode(WORD wSource);
	// Liefert darüber Auskunft, ob sich die die Kamera im Vollbild- oder
	// Halbbildmodus befindet.
	
	virtual BOOL SetLEDState(eLEDState ledState);
	// Setzt den Status der beiden externen LEDs
	// LED1: eLED1_On
	//		 eLED1_Blink
	// LED2: eLED2_On
	// LED2: eLED2_Off

	BOOL EnableDIBIndication(WORD wSource);
	// Ist die DIB Indication eingeschaltet, so wird eine unkomprimierte DIB verschickt.

	BOOL DisableDIBIndication(WORD wSource);
	// Schaltet die DIB Indication wieder aus

	virtual WORD EncoderSetFormat(WORD wEncFormat);
	// wEncFormat = ENCODER_RES_LOW, ENCODER_RES_HIGH
	// Rückgabe: Bisherige Einstellung, bzw. DEFAULT_ERROR

	virtual DWORD EncoderSetBPF(DWORD dwBPF);
	// Setzt die Byte pro Frame
	// Der Maximalwert ist JPEG_BUFFER_SIZE
	// Note: Der neue Wert wird erst nach dem Aufruf von 'SetInputSource(...)',
	//		 oder 'StartEncoder(...)' übernommen.

	virtual BOOL EncoderSetMode(WORD wEncFormat, DWORD dwBPF);
	// Setzt Format und Bytes pro Frame
	// wEncFormat = ENCODER_RES_LOW, ENCODER_RES_HIGH
	// dwBPF	  = Setzt die Byte pro Frame
	// Note: Der neue Wert wird erst nach dem Aufruf von 'SetInputSource(...)',
	//		 oder 'StartEncoder(...)' übernommen.

	virtual void DisableWatchdog() const;
	// Sperrt den Watchdogtimer.

	virtual void EnableWatchdog() const;
	// Gibt den Watchdog frei

	virtual BOOL TriggerWatchDog() const;
	// Setzt den Watchdog-Zähler auf den Startwert

	virtual void TriggerPiezo() const;
	// Wird nach erhalt des PiezoEvents aufgerufen.

	virtual void LoadConfig();
	// Lädt die folgenden Einstellungen aus der Registry.
	//		Debug\Trace\..
	//		General\Resolution
	//		FrontEnd\VideoFormat
	//		FrontEnd\UseGlobalColorSetting
	//		FrontEnd\GlobalBrightness
	//		FrontEnd\GlobalContrast
	//		FrontEnd\GlobalSaturation
	//		FrontEnd\GlobalHue
	//		Kameramapping+individuellen Farbeinstellungen.

	virtual BOOL SaveConfig() const;
	// Sichert die folgenden Einstellungen in die Registry
	//		General\Resolution
	//		Frontend\GlobalBrightness
	//		Frontend\GlobalContrast
	//		Frontend\GlobalSaturation
	//		Frontend\GlobalHue
	//		Backend\VideoType
 	//		Kameramapping+individuellen Farbeinstellungen.

	virtual BOOL Diagnostic(const CString &sSN);
	// Führt umfangreiche Diagnosen des SaVic-Boards durch

	virtual void OnReceivedRunTimeError(WK_RunTimeErrorLevel Level,
										WK_RunTimeError RTError,
										const CString &sMsg,
										DWORD dwParam1=0,
										DWORD dwParam2=0,
										WK_RunTimeErrorSubcode errorSubcode=RTES_INVALID) const;	   
	// Diese Funktion sollte überschrieben werden, sofern die Runtimeerrors
	// selber behandelt werden sollen.

	virtual void OnReceivedMotionDIB(WORD wSource, HANDLE hDIB);
	// Liefert eine DIB, die während der MD gewonnen wurde.

	virtual void OnReceivedImageData(const IMAGE* pImage) const;
	// Wird nach erhalt eines Fields aufgerufen.
	// pImage    = Pointer auf eine Struktur, die alle zum Image notwendigen Daten
	//			   enthält.
	// Note: Diese Funtion sollte überschrieben werden.

	virtual void OnReceivedEncoderError(int nErrorCode) const;
	// Wird ein Fehler während des Encodens auf (Timeout etc.) so wird diese
	// Funktion aufgerufen.
	// Note: Diese Funtion sollte überschrieben werden.
	
	virtual void OnReceivedAlarm(WORD wCurrentAlarmState) const;
	// Wird für jeden erkannten Alarm aufgerufen.
	// wCurrentAlarmState = b0000000000000000...b1111111111111111. Binarmaske: 1=Alarm, 0=Kein Alarm
	// Note: Diese Funtion sollte überschrieben werden.

	virtual void OnReceivedCameraStatus(WORD wSource, BOOL bStatus) const;
	// Wird nach jedem Kamerawechsel aufgerufen.
	// wSource  = VIDEO_SOURCE0... VIDEO_SOURCE15
	// bStatus  = TRUE -> Kamerasignal ist ok, ansonstens FALSE;
	// Note: Diese Funtion sollte überschrieben werden.
									  
	virtual void OnReceivedMotionAlarm(WORD wSource, CMDPoints &Points) const;
	// Wird Aufgerufen, wenn eine Bewegung detectiert wurde.
	// wSource  = VIDEO_SOURCE0... VIDEO_SOURCE15
	// Point = Position der größten Bewegung in Promille.
	//		   Point.x: 0=links 1000=rechts
	//		   Point.y: 0=oben  1000=unten
	// Note: Diese Funtion sollte überschrieben werden.

	virtual void OnPowerOffButtonPressed() const;
	// Wird aufgerufen, wenn der externe PowerOff-Button der SaVic gedrückt wurde.

	virtual void OnResetButtonPressed() const;
	// Wird aufgerufen, wenn der externe Reset-Button der SaVic gedrückt wurde.

	virtual BOOL WriteToEEProm(WORD wAddr, BYTE byVal);
	// Schreibt das Byte byVal an die Adresse wAddr des EEProms
	// Rückgabe: Im Fehlerfall FALSE

	virtual BOOL ReadFromEEProm(WORD wAddr, BYTE& byVal);
	// Liest das Byte byVal an der Adresse wAddr des EEProms
	// Rückgabe: Im Fehlerfall FALSE

	virtual BOOL WriteSubsystemID(WORD wSubsystemID);
	// Schreibt eine SubsystemID in das EEProm.

	virtual BOOL ReadSubsystemID(WORD& wSubsystemID);
	// Liest die SubsystemID aus dem EEProm.

	virtual BOOL ClearEEProm();
	// Löscht das EEPRom mit Außnahme der SubSystemID und der SubSystemVendorID!

	virtual BOOL WriteSN(const CString &sSN);
	// Schreibt die Seriennummer in das EEProm

	virtual BOOL ReadSN(CString &sSN);

protected:
	virtual BOOL PollAlarm();
	// Pollt auf die Alarmeingänge
	// Wenn ein Alarm festgestellt wurde ruft das CCodec-Objekt 'OnReceivedAlarm()'
	// auf. 
	// Rückgabe: TRUE, wenn wenigstens ein Alarm detektiert wurde.

	virtual BOOL WaitForExternEvent() const;
	// Wartet darauf, daß der Launcher ein WatchDogTriggerEvent oder ein PiezoEvent
	// verschickt.
	// Rückgabe: TRUE, wenn wenigstens ein Event detektiert wurde.

	virtual BOOL WaitForDriverDebugEvent() const;
	// Wartet auf ein DebugEvent des Treibers, und protokoliert bei Erhalt
	// eine Nachricht ins Logfile.
	// Rückgabe: TRUE, wenn wenigstens ein Event detektiert wurde.

	virtual void ReadData();
	// Liest die Bilddaten aus der ImageQueue

	virtual void MotionDetection(CAPTURE_STRUCT& Capture);
	// Test ob Bewegung im aktuellen Videobild ist,
	// Wird eine Bewegung im aktuellen Kamerabild erkannt, so wird dies über
	// 'OnReceivedMotionAlarm(...)' gemeldet.

	virtual BOOL MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points);
	// Liefert TRUE, wenn eine Bewegung im Kamerabild detectiert wurde

	virtual BOOL IsVideoPresent(BOOL& bPresent);
	// Überprüft ob am aktuell gewählten Eingang ein Videosignal anliegt.
	// bPresent = TRUE, wenn die aktuelle Kamera ein Videosignal liefert

	BYTE ReadVariationRegister() const;

	virtual BOOL TestingPCIBridge() const;

	virtual BOOL TestingRealTimeClock();

	virtual BOOL TestingEEProm(const CString &sSN);

	virtual BOOL TestingSyncSeperators();

	virtual DWORD MapPhysToLin(DWORD dwAddr, DWORD dwLen);
	// Wandelt eine physikalische Adresse in eine lineare Adresse.

	virtual BOOL OpenSaVicDevice(const SETTINGS &Settings);
	// - Öffnet den Gerätetreiber

	virtual BOOL CloseSaVicDevice();
	// Schließt den den Gerätetreiber

	virtual DWORD GetBaseAddress();
	// Liefert die Basisadresse der Karte
	// Note: Diese Funktion ist ein Dummy und MUß von der Unterklasse überschrieben werden.

	virtual WORD GetIRQNumber() const;
	// Liefert die IRQ-Nummer
	// Note: Diese Funktion ist ein Dummy und MUß von der Unterklasse überschrieben werden.

	virtual BOOL DoAnalogSwitch(SOURCE_SELECT_STRUCT &SrcSel);
	// Führt die analoge Umschaltung der Videoquelle durch.
	// SrcSel = Enthält alle zur Umschaltung notwendigen Parameter.
	// Note: Diese Funktion ist ein Dummy und MUß von der Unterklasse überschrieben werden.

	virtual BOOL CheckSource(WORD &wSource);
	// Liefert TRUE, wenn wSource zwischen VIDEO_SOURCE0 und VIDEO_SOURCE7 liegt, oder
	// VIDEO_SOURCE_CURRENT ist. Bei wSource = VIDEO_SOURCE_CURRENT wird die aktuelle
	// Kameranummer zurückgeliefert.

	virtual BOOL CheckVideoFormat(WORD wVideoFormat);
	// Liefert TRUE, wenn wVideoFormat entweder VFMT_PAL_CCIR, VFMT_NTSC_CCIR ist.

	virtual BOOL CheckFeFilter(WORD wFilter);
	// Liefert TRUE, wenn wFilter entweder LNOTCH_FILTER, LDEC_FILTER,
	// oder LNOTCH_FILTER | LDEC_FILTER ist.
	
	virtual BOOL CheckEncoderFormat(WORD wFormat);
	// Liefert TRUE, wenn wFormat entweder ENCODER_RES_HIGH, oder ENCODER_RES_LOW ist.

	virtual BOOL CheckEncoderBPF(DWORD dwBPF);
	// Liefert TRUE, wenn dwBPF zwischen  MIN_BPF und MAX_BPF liegt.
	
	virtual BOOL CheckRange(LPWORD lpwValue, long lMin, long lMax);
	// Liefert TRUE, wenn lpwValue zwischen  lMin und lMax liegt.
	
	virtual BOOL CheckIRQNumber(BYTE byIRQ) const;
	// Liefert TRUE, wenn byIRQ zwischen 3 und 15 liegt.

	virtual void ReadDebugConfiguration();
	// Liest die Einstellungen unter \Debug\Trace aus der Registry.

	virtual CRelay* GetRelayObject() const {return m_pRelay;}; 
	// Liefert ein Pointer auf ein CRelay-Objekt, über den die Basisklasse 
	// auf das Objekt zugreifen kann.

	virtual CAlarm* GetAlarmObject() const {return m_pAlarm;};
	// Liefert ein Pointer auf ein CAlarm-Objekt, über den die Basisklasse 
	// auf das Objekt zugreifen kann.

	virtual CBT878* GetBT878Object() const {return m_pBT878;};
	// Liefert ein Pointer auf ein CBT878-Objekt, über den die Basisklasse 
	// auf das Objekt zugreifen kann.

	virtual BOOL GetPowerOffButtonState() const;
	// Liefert TRUE, wenn der Poverbutton an der SaVic gedrückt wird.
	
	virtual BOOL TestingFrontends() const;
	
	virtual BOOL TestingRelais() const; 
	// Akustische Überprüfung der Relays

	virtual BOOL TestingRelais2() const; 
	// Überprüfung der Relays mit Hilfe eines Testadapters, der den Relaystatus auf den
	// ersten Alarmeingang legt.

	virtual BOOL TestingWatchdog() const;

	virtual DWORD GetBoardVariations() const;
	// Liefert die Bestückungsvaranten Bitcodiert.

	virtual BOOL GetResetButtonState() const;
	// Liefert TRUE, wenn der Resetbutton an der SaVic gedrückt wird.

	virtual BOOL GetTime(CTime& Time) const;

	virtual BOOL SetTime(const CTime &time);

	virtual BOOL MotionDetectionInit();

	virtual BOOL VideoInlayInit();

	IMAGE* CompressImageToJpeg(CAPTURE_STRUCT& Capture);
	// Komprimiert das von der SaVic-Hardware gelieferte RGB-Bild nach JPeg
	
	IMAGE* CreateDIB(CAPTURE_STRUCT& Capture);
	// Erzeugt aus den unkomprimierten Bilddaten eine DIB
	
	BOOL ScaleDown(const CSize& siSrc, const CSize& siDst, BGR24* pSrc);
	// Skaliert das unkomprimierte Bild herunter. Diese Funktion wird in CreateDIB dazu benutzt
	// mein RES_LOW die DIB zu verkleinern.

	BOOL ScaleDownYUV422(const CSize& siSrc, const CSize& siDst, YUV422* pSrc);
	// Skaliert das unkomprimierte Bild herunter. Diese Funktion wird in CreateDIB dazu benutzt
	// mein RES_LOW die DIB zu verkleinern.
	// (Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!)


	BOOL HandleScaler(int nH, int nV);

	CMyJpeg	m_Jpeg;

	// Tracemeldungen (An=TRUE / Aus=FALSE)
	BOOL	m_bTraceStartStopEncoder;
	BOOL	m_bTraceStartStopDecoder;
	BOOL	m_bTraceSetBPF;
	BOOL	m_bTraceSetFormat;
	BOOL	m_bTraceSetVideoFormat;
	BOOL	m_bTraceSetBrightness;
	BOOL	m_bTraceSetContrast;
	BOOL	m_bTraceSetSaturation;
	BOOL	m_bTraceSetHue;
	BOOL	m_bTraceSetFilter;
	BOOL	m_bTraceSetInputSource;
	BOOL	m_bTraceSetRelais;
	BOOL	m_bTraceSetAlarmEdge;
	BOOL	m_bTraceAlarmIndication;
	BOOL	m_bTraceEncoderQueue;
	BOOL	m_bTraceDriverInfo;
	BOOL	m_bTraceDriverWarning;
	BOOL	m_bTraceDriverError;
	BOOL	m_bTraceSwitchPrediction;
	BOOL	m_bTraceFrameRate;
	BOOL	m_bTraceDataRate;
	BOOL	m_bTraceFiFoEmptyFlag;
	BOOL	m_bTraceFiFoHalfFlag;
	BOOL	m_bTraceFiFoFullFlag;
	BOOL	m_bTraceAnalogOutSwitch;
	BOOL	m_bTraceWaitForImageTimeout;

	CWatchDog*		m_pCWatchDog;					// Pointer auf ein Watchdog-Objekt	

private:
	friend UINT AlarmPollThread(LPVOID pData);
	friend UINT ExternEventThread(LPVOID pData);
	friend UINT DriverDebugEventTread(LPVOID pData);
	friend UINT EncoderDataThread(LPVOID pData);

	friend void ML_TRACE(LPCSTR lpszFormat, ...);
	friend void ML_TRACE_WARNING(LPCSTR lpszFormat, ...);
	friend void ML_TRACE_ERROR(LPCSTR lpszFormat, ...);
	friend void ML_STAT_LOG(const char *szTopic,int iValue,const char *sName);
	friend void ML_STAT_PEAK(const char *szTopic,int iValue,const char *sName);

	void TraceEncoderQueue() const;
	void TraceStatistics(const IMAGE* Image, BOOL bClear = FALSE);	


	BOOL		    m_bRun;
	BOOL			m_bNewCameraScanMethod;			// TRUE=kamerascan über Syncdetector
	BOOL			m_bDiagnosticIsRunning;			// TRUE=Die Diagnosefunktion wird ausgeführt
	WORD			m_wLastError;					// Status des Objektes. TRUE=OK
	WORD			m_wSource;						// Aktuelle Videoquelle (VIDEO_SOURCE0...VIDEO_SOURCE7)
	WORD			m_wFormat;						// Bildformat											
	WORD			m_wAlarmState;					// Enthält den letzten Alarmstatus
	DWORD			m_dwBPF;						// Bytes pro Frame
	DWORD			m_dwLastLinAddr;				// Rückgabewert von DAMapPhysToLin();
	
	HDEVICE			m_hDevice;						// Identifiziert das Device
	HANDLE			m_hFieldReadyEvent;				// Field Encoded Event
	HANDLE			m_hSourceSwitchEvent;			// Source Switched
	HANDLE			m_hShutDownEvent;				// ShutdownEvent
	HANDLE			m_hDebugEvent;					// DebugEvent
	HANDLE			m_hWatchDogTriggerEvent;		// WatchdogTriggerEvent.
	HANDLE			m_hPiezoEvent;					// Störmelder an/aus
	
	CString			m_sAppIniName;					// INI-Filename
	CODECSTRUCT		m_Codec;
	CDlgColor*		m_pDlgColor;					// Pointer auf Settings-Dialog
	CMDConfigDlg*	m_pMDConfigDlg;					// Pointer auf MDConfig-Dialog;
	CWinThread*		m_pAlarmPollThread;				// PollAlarmThread
	CWinThread*		m_pDriverDebugThread;			// Wait for Debuginfo
	CWinThread*		m_pExternEventThread;			// Wait for extern Event
	CWinThread*		m_pEncoderDataThread;			// Holt Daten vom Encoder
	BOOL			m_bUseGlobalColorSettings;		// TRUE -> Eine Einstellung für alla
	WORD			m_wGlobalBrightness;			// Globale Helligkeitseinstellung
	WORD			m_wGlobalContrast;				// Globale Kontrasteinstellung
	WORD			m_wGlobalSaturation;			// Globale Farbsättigungseinstellung
	WORD			m_wGlobalHue;					// Globale Hueeinstellung
	WORD			m_wFPS;							// Framerate

	CCriticalSection	m_csCodec;					// Synchronisationsobjekt

	// Statistik
	int m_nStatisticSwitchCounter;					// Zählt die Kamerawechsel
	int m_nStatisticHitCounter;						// Zählt die korrekten Umschaltvorhersagen
	int m_nTemperature;								// Aktuelle Temperatur
	
	DWORD m_dwStatisticTotalBytes;					// Zählt die Ankommende Datenmenge in Bytes
	DWORD m_dwStatisticFieldCnt;					// Zählt die erhaltenen Bilder.
	DWORD m_dwStatisticTC;							// Zeiterfassung für Statistik
	DWORD m_dwLastPowerButtonPollTimeTC;			// Letzter Zeitpunkt der Powertastenabfrage
	SOURCE_SELECT_STRUCT m_PredSrcSel;				// Kameraumschaltvorhersage

	DWORD	m_dwCurrCamMask;						// Spiegelt die detektierten Kameras wieder.
													// Bit0=Cam0, Bit1=Cam1....

	CPCIDevice*			m_pDevice;					// Device-Objekt des SaVic-Boards
	CBT878*				m_pBT878;					// Pointer auf BT878-Objekt
	CRelay*				m_pRelay;					// Pointer auf Relay-Objekt
	CAlarm*				m_pAlarm;					// Pointer auf Alarm-Objekt
	CPCF8582*			m_pPCF8582;					// PCF8582-Objekt (EEprom)
	CPCF8563*			m_pPCF8563;					// PCF8663-Objekt (Uhr)

	int					m_nEncoderState;			// ENCODER_ON, ENCODER_OFF
	int					m_nQuality[MAX_CAMERAS];	// Qualitätseinstellung für die Bitratenregelung

	CWK_PerfMon*		m_pPerf1;
	CWK_PerfMon*		m_pPerf2;
	CWK_PerfMon*		m_pPerf3;

	BOOL				m_bUseTVCard;				// TRUE->Benutze TV-Karte als SaVic Ersatz

	IMAGE				m_Image;					// Enthält die Bilddaten, die an die Unit verschickt werden.
};
#endif // __CCODEC_H__

