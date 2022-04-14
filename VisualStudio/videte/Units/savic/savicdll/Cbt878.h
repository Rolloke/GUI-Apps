/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Cbt878.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Cbt878.h $
// CHECKIN:		$Date: 27.01.03 14:19 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 27.01.03 13:54 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
	   
#ifndef __CBT828_H__
#define	__CBT828_H__

#define BUFF_SIZE		255

class CCodec;
class CVideoInlay;
class CMotionDetection;
class CMDPoints;
class CIPCActivityMask;
class CIoMemory;

class CBT878
{
public:
	BOOL RestartEncoder();
	BOOL EncoderStop();
	BOOL EncoderStart();
	BOOL EnableAGC(BOOL bEnable);
	CBT878(CCodec* const pCodec, DWORD dwIOBase, const CString &sAppIniName);

	void InitPLL();

	// Initialisiert den PLL
	virtual ~CBT878();

	BOOL IsValid() const;
	// Liefert TRUE, wenn das Objekt korrekt angelegt werden konnte

	BOOL Init(const CString &sFileName, const CString &sSection);
	// Initialisiert das Videofrontend
	// sFilename = Name der INI-Datei
	// sSection  = Name des Abschnitts in dem die Initialisierungswerte des BT878 stehen
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetVideoSource(SOURCE_SELECT_STRUCT &SrcSel);
	// Wählt eine der 4 Videoeingänge aus

	BOOL SetCameraToAnalogOut(WORD wSource, WORD wPort);
	// Schaltet die Kamera 'wSource' auf den Videoausgang 'wPort'

	BOOL GetVideoSource(WORD& wSource);
	// Liefert die Kamera auf die der Mux steht.

	BOOL SetVideoFormat(WORD wVideoFormat);
	// Setzt das Videoformat des BT878
	// wVideoFormat = VFMT_NTSC_CCIR, VFMT_PAL_CCIR
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetBrightness(WORD wBrightness);
	// Setzt die Helligkeit
	// wBrightness	 = Helligkeitseinstellung (MIN_BRIGHTNESS...MAX_BRIGHTNESS)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetContrast(WORD wContrast);
	// Setzt die Kontrasteinstellung
	// wContrast = Kontrasteinstellung (MIN_CONTRAST... MAX_CONTRAST)
	// Rückgabe: Im Fehlerfall FALSE
	
	BOOL SetSaturation(WORD wSaturation);
	// Setzt die Farbeinstellung
	// wSaturation = Farbeinstellung (MIN_SATURATION... MAX_SATURATION)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetHue(WORD wHue);
	// Setzt die HUE-einstellung
	// wHue = HUE-einstellung (MIN_HUE... MAX_HUE)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetFilter(WORD wFilter);
	// Aktiviert bzw. deaktiviert die beiden Filter des BT878
	// wFilter = LNOTCH_FILTER, LDEC_FILTER
	// Rückgabe: Im Fehlerfall FALSE

	WORD GetVideoFormat() const;
	// Liefert das Videoformat (VFMT_NTSC_CCIR, VFMT_PAL_CCIR) 

	WORD GetSourceType() const;
	// Liefert den SourceType (SRCTYPE_FBAS, SRCTYPE_SVHS)
	
	WORD GetBrightness() const;
	// Liefert die Helligkeitseinstellung
	// Der Wert liegt innerhalb der Grenzen (MIN_BRIGHTNESS...MAX_BRIGHTNESS)
	
	WORD GetContrast() const;
	// Liefert die Kontrasteinstellung
	// Der Wert liegt innerhalb der Grenzen (MIN_CONTRAST...MAX_CONTRAST)

	WORD GetSaturation() const;
	// Liefert die Farbsättigung
	// Der Wert liegt innerhalb der Grenzen (MIN_SATURATION...MAX_SATURATION)
	
	WORD GetHue() const;
	// Liefert den 'Hue'
	// Der Wert liegt innerhalb der Grenzen (MIN_HUE...MAX_HUE)
	
	WORD GetFilter()const;
	// Liefert die augenblickliche Filtereinstellung des BT878
	
	BOOL IsVideoPresent(BOOL& bPresent);
	// bPresent =  TRUE, wenn der BT878 das Videosignal für gültig hält
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetHScaleE(WORD wHScale);
	// Setzt die horizontale Skalierung im BT878 für das Even field
	// wHScale = horizontaler Skalierungswert (MIN_HSCALE...MAX_HSCALE)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetHScaleO(WORD wHScale);
	// Setzt die horizontale Skalierung im BT878 für das Odd field
	// wHScale = horizontaler Skalierungswert (MIN_HSCALE...MAX_HSCALE)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetVScaleE(WORD wVScale);
	// Setzt die vertikale Skalierung im BT878 für das Even field
	// wVScale = horizontaler Skalierungswert (MIN_VSCALE...MAX_VSCALE)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetVScaleO(WORD wVScale);
	// Setzt die vertikale Skalierung im BT878 für das Odd field
	// wVScale = horizontaler Skalierungswert (MIN_VSCALE...MAX_VSCALE)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL SetHActiveE(WORD wHActive);
	// Setzt die horizontale Active Area für das Even field
	
	BOOL SetHActiveO(WORD wHActive);
	// Setzt die horizontale Active Area für das Odd field
	
	BOOL SetHDelayE(WORD wHDelay);
	// Setzt die horizontalen Startpunkt der Active Area für das Even field

	BOOL SetHDelayO(WORD wHDelay);
	// Setzt die horizontalen Startpunkt der Active Area für das Even field

	BOOL GetHScaleO(WORD& wHScale) const;
	// Liefert die aktuelle horizontale Skalierung des BT878 (Odd field)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL GetHScaleE(WORD& wHScale) const;
	// Liefert die aktuelle horizontale Skalierung des BT878 (Even field)
	// Rückgabe: Im Fehlerfall FALSE
	
	BOOL GetVScaleE(WORD& wVScale) const;
	// Liefert die aktuelle vertikale Skalierung des BT878 (Even field)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL GetVScaleO(WORD& wVScale) const;
	// Liefert die aktuelle vertikale Skalierung des BT878 (Odd field)
	// Rückgabe: Im Fehlerfall FALSE

	BOOL GetHActiveE(WORD& wHActive);
	// Liefert die horizontale Active Area für das Even field
	
	BOOL GetHActiveO(WORD& wHActive);
	// Liefert die horizontale Active Area für das Odd field
	
	BOOL GetHDelayE(WORD& wHDelay);
	// Liefert die horizontalen Startpunkt der Active Area für das Even field

	BOOL GetHDelayO(WORD& wHDelay);
	// Liefert die horizontalen Startpunkt der Active Area für das Even field

	BOOL DetectVideoFormat(WORD& wFormat);
	// Ermittelt anhand der Zeilenzahl die Viodeonorm
	// wFormat = VFMT_PAL_CCIR oder VFMT_NTSC_CCIR
	// Rückgabe: Im Fehlerfall FALSE

	BOOL Diagnostic();
	// Ausführlicherer Test des Videofrontend gegenüber 'CheckIt()'. Wird
	// vom SaVic-Testprogramm ausgewertet.
	
	BOOL BT878Out(WORD wAddress, DWORD dwData);
	// Schreibt ein Byte in ein bestimmtes Register des BT878

	BOOL BT878In(WORD wAddress, DWORD& dwData) const;
	// Liest ein Byte aus einem bestimmten Register des BT878
	
	BOOL BT878In(WORD wAddress, WORD& wData) const;
	// Liest ein Word aus einem bestimmten Register des BT878
	
	BOOL BT878In(WORD wAddress, BYTE& byData) const;
	// Liest ein DWord aus einem bestimmten Register des BT878

	BOOL SetGPIOPin(WORD wBit);
	// Setzt das entsprechende Bit 

	BOOL ClrGPIOPin(WORD wBit);
	// Löscht das entsprechende Bit

	BOOL ReadGPIOPin(WORD wBit);
	// Liest ein Bit des GPIO-Registers

	virtual BOOL VideoInlayInit();
	// Initialisiert das VideoInlay.
	// Note: Die ISA-Karte besitzt kein VideoInlay

	virtual BOOL VideoInlaySetOutputWindow(const CRect &rc) const;
	// Setzt das Begrenzungsrechteck beim Videoinlay
	// Note: Die ISA-Karte besitzt kein VideoInlay

	virtual BOOL VideoInlayActivate() const;
	// Schaltet das Videoinlay ein
  	// Note: Die ISA-Karte besitzt kein VideoInlay

	virtual BOOL VideoInlayDeactivate() const;
	// Schaltet das Videoinlay aus
	// Note: Die ISA-Karte besitzt kein VideoInlay

	virtual BOOL MotionDetectionInit();
	// Initialisiert das MotionDetection.

	virtual BOOL MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points) const;
	// Liefert TRUE, wenn eine Bewegung im Kamerabild detectiert wurde

	virtual BOOL MotionDetectionConfiguration(WORD wSource);
	// Öffnet einen Dialog zur konfiguration des Motiondetection
	// wSource = Camera, für die die Maske verändert werden soll

	virtual BOOL EnableDIBIndication(WORD wSource);
	// Aktiviert die DIB-Indication. D.h. immer wenn bei der Motiondetection ein
	// Frame 'gegrapt' wird, wird dieses als DIB über die virtuelle Funktion 
	// 'OnReceivedMotionDIB' an verwschickt.

	virtual BOOL DisableDIBIndication(WORD wSource);
	// Schaltet die DIB-Indication wieder aus.

 	virtual BOOL ReLoadMDConfiguration();
	// Lädt alle MD-Parameter erneut aus der Registry ein

	virtual BOOL ActivateMotionDetection(WORD wSource);
	// Aktiviert für die Kaera wSource die MotionDetection

	virtual BOOL DeactivateMotionDetection(WORD wSource);
	// Deaktiviert für die Kaera wSource die MotionDetection

	virtual BOOL SetMDMaskSensitivity(WORD wSource, const CString &sLevel);
	// Setzt die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll
	// sLevel  = High, Normal, Low

	virtual BOOL SetMDAlarmSensitivity(WORD wSource, const CString &sLevel);
	// Setzt die Empfindlichkeit der Motiondetection in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll
	// sLevel  = High, Normal, Low

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

protected:
	virtual void CBT878Reset();

	virtual void OnError(WORD wErr, WORD wParam1, WORD wParam2) const;

//	void InitPLL();
	// Initialisiert den PLL

private:
	BOOL CheckIt();
	// Führt einen Schreib/Lesetest auf die Register des BT878 durch

	
	CCriticalSection m_csBT878;					// Synchronisationsobjekt

	DWORD	m_dwBT878Reg[BT878_MAX_REG+1][2];	// Initialisierungswerte des BT878 für die 2 Videoformate		

	WORD	m_wVideoFormat;						// Aktuelles Videoformat
	CCodec* m_pCodec;							// Zum Aufruf von CCodec::OnReceivedRunTimeError()

	CIoMemory* m_pCIo;							// Zugriffsobjekt

	CVideoInlay			*m_pVideoInlay;			// Pointer auf VideoInlay-Objekt
	CMotionDetection	*m_pMotionDetection;	// Pointer auf MotionDetection-Objekt
	CString				m_sAppIniName;
	WORD				m_wFeVideoFormat;		// Videoformat PAL oder Ntsc
};

#endif // __CBT828_H__
