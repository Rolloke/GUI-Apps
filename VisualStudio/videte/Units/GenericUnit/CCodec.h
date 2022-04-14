/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericDll
// FILE:		$Workfile: CCodec.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CCodec.h $
// CHECKIN:		$Date: 24.11.00 7:51 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 24.11.00 7:49 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CCODEC_H__
#define __CCODEC_H__

/////////////////////////////////////////////////////////////////////////////
class CGenericUnitDlg;
class CMotionDetection;
class CCodec
{
public:
	CCodec(CGenericUnitDlg* pMainWnd);

	virtual ~CCodec();
	// Zerstört das CCodec-Objekt

	virtual int GetEncoderState() const;
	// Liefert den aktuellen Status des Encoders (ENCODER_ON/ENCODER_OFF)

	virtual WORD SetInputSource(SOURCE_SELECT_STRUCT &CurrSrcSel, SOURCE_SELECT_STRUCT &NextSrcSel);
	// Schaltet auf die gewünschte Videoquelle um.
	// Das Umschalten auf eine andere Eingangsquelle bewirkt das Setzen der ihr
	// zugeordneten Einstellungen (VideoFormat, Brightness, Contrast, Saturation)
	// SrcSel.wSource		= VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT
	// SrcSel.wFormat		= ENCODER_RES_HIGH, ENCODER_RES_LOW
	// SrcSel.dwBPF			= Anzahl der Bytes pro JPEG-File
	// SrcSel.dwProzessID	= Eine eindeutige ID, die jedem Bild einem Prozeß zuordnet
	// NextSrcSel			= Das inaktive Frontend der Generic-Karte wird auf diese Kamera geschaltet.
	//						  Es sind nur die Einträge NextSrcSel.wSource von interesse.
	// Rückgabe: Bisherige Videoquelle			(VIDEO_SOURCE0...VIDEO_SOURCE15)

	virtual WORD GetInputSource();
	// Liefert die aktuell verwendete Videoquelle und Erweiterungskarte
	//	(VIDEO_SOURCE0...VIDEO_SOURCE15)

	virtual BOOL SetSourceType(WORD wSourceType);
	// Setzt den SourceType der aktuellen Videoquelle
	// wSourceType	= SRCTYPE_FBAS, SRCTYPE_SVHS
	// Rückgabe: TRUE->Kein Fehler erkannt

	virtual WORD GetSourceType() const;
	// Liefert den SourceType der aktuell gewählten Videoquelle (SRCTYPE_FBAS, SRCTYPE_SVHS)
	
	virtual BOOL SetVideoFormat(WORD wFeVideoFormat);
	// Setzt das FrontEnd-Videoformat
	// wFeVideoFormat	= VFMT_PAL_CCIR, VFMT_NTSC_CCIR)
	// Rückgabe: TRUE->Kein Fehler erkannt

	virtual WORD GetVideoFormat() const;
	// Liefert das FrontEnd-Videoformat (VFMT_PAL_CCIR, VFMT_NTSC_CCIR) 
	
	virtual BOOL SetBrightness(WORD wSource, WORD wBrightness);
	// Setzt die Helligkeit
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wBrightness	 = Helligkeitseinstellung (MIN_BRIGHTNESS...MAX_BRIGHTNESS)
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 Liegt wBrightness außerhalb der gültigen Grenzen, wird diese begrenzt.

	virtual WORD GetBrightness(WORD wSource);
	// Liefert die Helligkeitseinstellung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_BRIGHTNESS...MAX_BRIGHTNESS)

	virtual BOOL SetContrast(WORD wSource, WORD wContrast);
	// Setzt den Kontrast
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wContrast	 = Kontrasteinstellung (MIN_CONTRAST...MAX_CONTRAST)
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 liegt wContrast außerhalb der gültigen Grenzen, wird dieser begrenzt.

	virtual WORD GetContrast(WORD wSource);
	// Liefert die Kontrasteinstellung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_CONTRAST...MAX_CONTRAST)

	virtual BOOL SetSaturation(WORD wSource, WORD wSaturation);
	// Setzt die Farbsättigung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wSaturation	 = Farbsättigung (MIN_SATURATION...MAX_SATURATION)
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 liegt wSaturation außerhalb der gültigen Grenzen, wird diese begrenzt.

	virtual WORD GetSaturation(WORD wSource);
	// Liefert die Farbsättigung
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_SATURATION...MAX_SATURATION)

	virtual BOOL SetHue(WORD wSource, WORD wHue);
	// Setzt den 'Hue'
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle deren Einstellung geändert werden soll.
	// wHue	 = Farbsättigung (MIN_HUE...MAX_HUE)
	// Rückgabe: TRUE->Kein Fehler erkannt
	// Notes:	 liegt wHue außerhalb der gültigen Grenzen, wird diese begrenzt.

	virtual WORD GetHue(WORD wSource);
	// Liefert den 'Hue'
	// wSource = Kameranummer (VIDEO_SOURCE0...VIDEO_SOURCE15, VIDEO_SOURCE_CURRENT)
	//				      der Videoquelle, deren Einstellung erfragt werden soll.
	// Der Wert liegt innerhalb der Grenzen (MIN_HUE...MAX_HUE)

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

	virtual BYTE SetRelais(BYTE byRelais);
	// Setzt die Relaisausgänge.
	// wRelais	= xxxx0000B...xxxx1111B
	// Rückgabe: Bisherige Relaiseinstellung
	
	virtual BYTE GetRelais();
	// Rückgabe: Aktuelle Relaiseinstellung.

	virtual BOOL EncoderStart();
	// Startet den Encoder.
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual BOOL EncoderStop();
	// Stopt den Encoder
	// Rückgabe: TRUE -> Kein Fehler ansonsten FALSE.

	virtual WORD EncoderGetFormat() const;
	// Liefert die gewählte Encoderauflösung (ENCODER_RES_LOW, ENCODER_RES_HIGH)

	virtual DWORD EncoderGetBPF() const;
	// Liefert die aktuell eingestelle 'Byte pro Frame'

	virtual void ActivateMotionDetection(WORD wSource);
	// Aktiviert für die Kaera wSource die MotionDetection

	virtual void DeactivateMotionDetection(WORD wSource);
	// Deaktiviert für die Kaera wSource die MotionDetection

	virtual void SetMDMaskSensitivity(WORD wSource, const CString &sLevel);
	// Setzt die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll
	// sLevel = High, Normal, Low

	virtual void SetMDAlarmSensitivity(WORD wSource, const CString &sLevel);
	// Setzt die Empfindlichkeit der Motiondetection in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll
	// wLevel = High, Normal, Low

	virtual CString GetMDMaskSensitivity(WORD wSource);
	// Liefert die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll

	virtual CString GetMDAlarmSensitivity(WORD wSource);
	// Liefert die Empfindlichkeit der Motiondetection in 3 Stufen.
	// wSource = Kamera, deren Empfindlichkeit gesetzt werden soll

	virtual DWORD ScanForCameras(DWORD dwScanMask);
	// Scant die Kameraeingänge durch und prüft, ob ein Videosignal vorhanden ist oder nicht.
	// Sofern detektierte Kameramaske sich von zu einem vorherigen Aufruf verändert hat,
	// wird die Videonorm der ersten detektierten Kamera ermittelt, und das System evtl.
	// uminitialisiert.
	// dwScanMask = Bitmaske derjenigen Kameraeingänge, die gescannt werden sollen
	// Rückgabe: Bitmaske aller Kameraeingänge: 1=aktiv, 0=nicht aktiv

	virtual void ReadData();
	// Liest Bilddaten aus dem Codebuffer und stellt diese in Form einer JPEG-Struktur
	// zur Verfügung. Die Jpegstruktur enthält neben den eigendlichen JPeg-Bilddaten z.B.
	// noch die Kameranummer.

	virtual BOOL PollAlarm();
	// Pollt auf die Alarmeingänge
	// Rückgabe: TRUE, wenn wenigstens ein Alarm detektiert wurde.

	virtual void MotionDetection();

	virtual void MotionDetectionConfiguration();

	JPEG* ReadJpegFile(BOOL bCurrentFrame);

protected:
	virtual WORD EncoderSetFormat(WORD wEncFormat);
	// wEncFormat = ENCODER_RES_LOW, ENCODER_RES_HIGH
	// Rückgabe: Bisherige Einstellung, bzw. DEFAULT_ERROR

	virtual DWORD EncoderSetBPF(DWORD dwBPF);
	// Setzt die Byte pro Frame
	// Der Maximalwert ist JPEG_BUFFER_SIZE
	// Note: Der neue Wert wird erst nach dem Aufruf von 'SetInputSource(...)',
	//		 oder 'StartEncoder(...)' übernommen.

private:
	friend UINT EncoderDateThread(LPVOID pData);
	friend UINT AlarmPollThread(LPVOID pData);

	BOOL			m_bRun;
	WORD			m_wSource;
	WORD			m_wFormat;
	WORD			m_wSourceType;
	WORD			m_wVideoFormat;
	WORD			m_wAlarmState;
	WORD			m_wBrightness[MAX_CAMERAS];
	WORD			m_wContrast[MAX_CAMERAS];
	WORD			m_wSaturation[MAX_CAMERAS];
	WORD			m_wHue[MAX_CAMERAS];
	int				m_nFrameCounter[MAX_CAMERAS];
	DWORD			m_dwLastGrab[MAX_CAMERAS];

	DWORD			m_dwBPF;
	DWORD			m_dwProzessID;
	DWORD			m_dwReqPics;
	CGenericUnitDlg*m_pMainWnd;
	CWinThread*		m_pEncoderDataThread;	// Holt Daten vom Encoder
	CWinThread*		m_pAlarmPollThread;		// Schaut nach, ob ein Alarm anliegt.
	CCriticalSection m_csCodec;
	CMotionDetection* m_pMotionDetection;

	int				m_nEncoderState;
	int				m_nStoreField;
};
#endif // __CCODEC_H__

