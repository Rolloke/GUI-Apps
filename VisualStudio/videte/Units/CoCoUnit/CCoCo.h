/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CCoCo.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CCoCo.h $
// CHECKIN:		$Date: 29.11.02 8:59 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 29.11.02 8:57 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CCOCO_H__
#define __CCOCO_H__

#include "cpydata.h"

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

// CoCo's  (Müssen von 0 an aufsteigend nummeriert sein!!)
#define COCO_COCOID0				0
#define COCO_COCOID1				1
#define COCO_COCOID2				2
#define COCO_COCOID3				3
#define COCO_COCOID_CURRENT			255
#define MAX_COCO_ID					COCO_COCOID3

// VideoQuellen (Müssen von 0 an aufsteigend nummeriert sein!!)
#define	COCO_SOURCE0				0
#define	COCO_SOURCE1				1
#define	COCO_SOURCE2				2
#define	COCO_SOURCE3				3
#define	COCO_SOURCE4				4
#define	COCO_SOURCE5				5
#define	COCO_SOURCE6				6
#define	COCO_SOURCE7				7
#define COCO_SOURCE_CURRENT			255
#define COCO_MAX_SOURCE				COCO_SOURCE7

// Erweiterungskarten  (Müssen von 0 an aufsteigend nummeriert sein!!)
#define COCO_EXTCARD0				0
#define COCO_EXTCARD1				1
#define COCO_EXTCARD2				2
#define COCO_EXTCARD3				3
#define COCO_EXTCARD_MEGRA			4
#define COCO_EXTCARD_CURRENT		255
#define COCO_MAX_EXTCARD			COCO_EXTCARD_MEGRA

#define COCO_MAX_CAM	COCO_MAX_EXTCARD * (COCO_MAX_SOURCE + 1)

// Video Formate
#define COCO_PAL					1
#define COCO_SECAM					2
#define COCO_NTSC					3
#define COCO_PAL_CIF				4
#define COCO_NTSC_CIF				5

// Art der Eingangsquelle
#define COCO_FBAS					1
#define COCO_SVHS					2

// Art des Ausgangssignals
#define COCO_COMPOSITE				1
#define COCO_RGB            	   	2

// VideoFrontEnd-Filter
#define COCO_LNOTCH					1
#define COCO_LDEC					2

// Wertebereich der Helligkeit
#define COCO_MIN_BRIGHTNESS			0
#define COCO_MAX_BRIGHTNESS			255

// Wertebereich des Kontrastes
#define COCO_MIN_CONTRAST			0
#define COCO_MAX_CONTRAST			511

// Wertebereich des Farbsättigung
#define COCO_MIN_SATURATION			0
#define COCO_MAX_SATURATION			511

// Wertebereich des Hue
#define COCO_MIN_HUE				0
#define COCO_MAX_HUE				255

// Wertebereich der horizontalen Skalierung
#define COCO_MIN_HSCALE				0
#define COCO_MAX_HSCALE				65535

// Wertebereich der vertikalen Skalierung
#define COCO_MIN_VSCALE				0
#define COCO_MAX_VSCALE				65535

// Wertebereich der Encoder/Decoderbitrate
#define COCO_ENC_MIN_BITRATE		0
#define COCO_ENC_MAX_BITRATE		2048000

// Wertebereich für MPI
#define COCO_MIN_MPI				1
#define COCO_MAX_MPI				25

// Mögliche Bildformate
#define COCO_ENCODER_QCIF			(WORD)1
#define COCO_ENCODER_CIF			(WORD)2
#define COCO_ENCODER_NO_FORMAT		(WORD)(-1)

// Bytes pro Frame für die 5 in PicDef.h definierten Kompressionsfaktoren (für CIF)
// QCIF hat jeweils ein viertel der Länge
#define COCO_BPF_1					8000
#define COCO_BPF_2					6000
#define COCO_BPF_3					4000
#define COCO_BPF_4					2000
#define COCO_BPF_5					1000

// Legt fest welche Komponenten beim Aufruf von 'Activate(...)' aktiviert werden sollen
#define COCO_ACT_SOURCE				0x0001
#define COCO_ACT_COCOID				0x0002
#define COCO_ACT_BEVIDEOFORMAT 		0x0004
#define COCO_ACT_BEVIDEOTYPE 		0x0008
#define COCO_ACT_FEFILTER 			0x0010
#define COCO_ACT_FEHSCALE 			0x0020
#define COCO_ACT_FEVSCALE			0x0040
#define COCO_ACT_EDGE				0x0080
#define COCO_ACT_ACK				0x0100
#define COCO_ACT_RELAIS				0x0200
#define COCO_ACT_ALL				0xffff

// Mögliche Transferparameter
#define COCO_DECODER_DATA			1
#define COCO_ENCODER_DATA			2
#define COCO_MCI_DATA				3
#define COCO_TEST_DATA				4

// Fehlermeldungen
#define COCO_ERR_NO_ERRROR				 0
#define COCO_ERR_WRONG_COCOID			 1
#define COCO_ERR_WRONG_SOURCE			 2
#define COCO_ERR_WRONG_EXT_CARD			 3
#define COCO_ERR_WRONG_SOURCE_TYPE		 4
#define COCO_ERR_WRONG_FE_VIDEO_FORMAT   5
#define COCO_ERR_WRONG_BE_VIDEO_FORMAT   6
#define COCO_ERR_WRONG_BE_VIDEO_TYPE	 7
#define COCO_ERR_PARM_OUT_OF_RANGE		 8	
#define COCO_ERR_UNKNOWN_PARAMETER  	 9
#define COCO_ERR_WRONG_FILTER_TYPE		 10
#define COCO_ERR_WRONG_ENCODER_FORMAT	 11
#define COCO_ERR_WRONG_ENCODER_FRAMERATE 12
#define COCO_ERR_WRONG_ENCODER_BITRATE	 13
#define COCO_ERR_WRONG_AVP_PROZESS		 14
#define COCO_ERROR						0xffff

// AVP-Prozesse, mit der der VideoInput-Prozeß verbunden werden kann
#define COCO_VIDIN		1
#define COCO_PREP		2
#define COCO_DECODER	3
#define COCO_ENCODER	4

// Compressiontype
#define COCO_H261		1
#define COCO_H263		2

#ifdef __cplusplus
class CDlgColor;
class CWmCopyData;


typedef struct
{
	WORD wFeVideoFormat;
	WORD wFeSourceType;
	WORD wFeBrightness;
	WORD wFeSaturation;
	WORD wFeContrast;
	WORD wFeHue;
	int nAVPortY;
	int nAVPortC;
} SOURCESTRUCT;

typedef struct
{
	SOURCESTRUCT Source[COCO_MAX_SOURCE+1];
	WORD wSource;
	WORD wEdge;
	WORD wAck;
	WORD wRelais;
} EXTCARDSTRUCT;

typedef struct
{
	EXTCARDSTRUCT ExtCard[COCO_MAX_EXTCARD+1];
	WORD wExtCard;
	WORD wFeFilter;
	WORD wBeVideoFormat;
	WORD wBeVideoType;
	WORD wFeHScale;
	WORD wFeVScale;
	WORD wFramerate;
	WORD wFormat;
	DWORD dwDecBitrate;
	DWORD dwEncBitrate;
} COCOISASTRUCT;

/////////////////////////////////////////////////////////////////////////////
// CCoCo
class CCoCo
{
public:
	CCoCo(HWND hWnd, LPCSTR lpIniAppName);
	// hWnd			= Fensterhandle der Applikation. An dieses Fenster verschickt der Treiber
	//		  		  Meldungen, wenn nötig
	// lpIniAppName = Pointer auf den Namen der INI-Datei
	// 
	// Das Anlegen dieses Objektes bewirkt folgendes:
	//		- Initialisieren aller Membervariablen anhand der INI-Datei
	//			- Aktueller Videoeingang (0...7)
	//			- Aktuelle Erweiterungskarte (0...3)
	//			- Name der Serverapplikation
	//			- VideoFormat des BackEnds (PAL, NTSC)
	//			- VideoType des BackEnds (COMPOSITE, RGB)
	//			- Für jede Videoeingang/ExtCard-Kombination werden die folgenden Einstellungen geladen.
	//				- Video FrontEnd Format (PAL, SECAM, NTSC)
	//				- Video FrontEnd SourceType (FBAS, SVHS)
	//				- Helligkeit
	//				- Kontrast
	//				- Farbsättigung
	//				- Hue
	//				- Physikalische Portnummer Y-Signal	(0...7)
	//				- Physikalische Portnummer C-Signal (0...7) (falls SVHS)
	//		- Laden der Serverapplikation, falls noch nicht gestartet. Dies bewirkt wiederum folgendes:
	//			- Laden aller avpXXX-Dll's
	//			- Öffnen des MCI Device-Treibers
	//			- Initialisierung des AVP's, Systemcontroller, Videohardware
	//		- Anmelden der Applikation beim Server

	~CCoCo();
	//	Abmelden der Applikation beim Server
	//	Ist keine Applikation mehr angemeldet, so passiert folgendes:
	//		- Beenden der Serverapplikation	. Dies bewirkt wiederum folgendes:
	//			- Abmelden beim AVP 
	//			- Schließen des MCI-Device-Treibers
	//			- Beenden aller avpXXX-Dll's
			
	BOOL IsValid();
	// Liefert TRUE, wenn das CCoCo-Objekt fehlerfrei angelegt wurde, ansonsten FALSE.

	BOOL SaveConfig();
	// Sichert alle Einstellungen

	WORD GetVersion();
	// Liefert eine Versionsnummer des CoCo-Treibers

	WORD GetLastError();
	// Liefert einen der folgenden Fehlerkodes zurück.
	//	 -  COCO_ERR_NO_ERRROR	
	//	 -  COCO_ERR_WRONG_SOURCE
	//	 -  COCO_ERR_WRONG_EXT_CARD
	//	 -  COCO_ERR_WRONG_SOURCE_TYPE
	//	 -  COCO_ERR_WRONG_FE_VIDEO_FORMAT
	//	 -  COCO_ERR_WRONG_BE_VIDEO_FORMAT
	//	 -  COCO_ERR_PARM_OUT_OF_RANGE
	//	 -  COCO_ERR_UNKNOWN_PARAMETER
	// Das Lesen bewirkt das Rücksetzen des Fehlerstatuses.

	BOOL CheckHardware(BYTE byCoCoID);
	// Testet die Hardware
	// byCoCoID = COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT
	// Liefert TRUE, wenn kein Fehler erkannt wurde, ansonsten FALSE
	
	DWORD GetState();
	// Gibt Auskunft darüber welche Karten (CoCo's, ExtCard's) im System eingebunden sind
	// Rückgabe:	Bit 0	   -> CoCo 0
	//				Bit 1...3  -> 	ExtCard 1...3
	//				Bit 4	   -> CoCo 1
	//				Bit 5...7  ->	ExtCard 1...3
	//				Bit 8	   -> CoCo 2
	//				Bit 9...11 ->	ExtCard 1...3
	//				Bit 12	   -> CoCo 3
	//				Bit 13..15 ->	ExtCard 1...3
	// Gesetztes Bit heißt Karte in System.ini eingetragen

	BOOL Activate(WORD wAct);
	// Diese Funktion aktiviert die individuellen Einstellungen des Objektes.
	// wAct = Legt fest welche Komponenten aktiviert werden.
	//			COCO_ACT_SOURCE			- Umschalten auf die aktuelle Videoquelle.
	//										- Setzen der physikalischen Video-Anschlüsse (Y/C)
	//										- Setzen des Fe-SourceType (FBAS, SVHS)
	//										- Setzen des Fe-Videoformates (PAL, NTSC)
	//										- Setzen der aktuellen Helligkeit
	//										- Setzen des aktuellen Kontrastes
	//										- Setzen der aktuellen Farsättigung
	//										- Setzen des aktuellen 'Hue'
	//			COCO_ACT_BEVIDEOFORMAT	- Setzen des aktuellen Be-Videoformates (PAL, SECAM, NTSC)
	//			COCO_ACT_BEVIDEOTYPE	- Setzen des aktuellen Be-SourceTypes (COMPOSITE, RGB)
	//			COCO_ACT_FEFILTER 		- Setzen des aktuellen Videofilter
	//			COCO_ACT_FEHSCALE 		- Setzen der aktuellen horizontalen Skalierung
	//			COCO_ACT_FEVSCALE		- Setzen der aktuellen vertikalen Skalierung
	//			COCO_ACT_EDGE			- Setzen des aktuellen AlarmEdge 
	//			COCO_ACT_ACK			- Setzen des aktuellen AlarmAck
	//			COCO_ACT_RELAIS			- Setzen des aktuellen Relaisausgänge
	//			COCO_ACT_ALL			- Setzen aller Einstellungen

	DWORD SetInputSource(BYTE byCoCoID, WORD wExtCard, WORD wSource);
	// Schaltet auf die gewünschte Videoquelle um.
	// Das Umschalten auf eine andere Eingangsquelle bewirkt das Setzen der ihr
	// zugeordneten Einstellungen (VideoFormat, SourceType, Helligkeit, Kontrast,
	//							   Farbsättigung, Hue)
	// byCoCoID = COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT
	// wExtCard = COCO_EXTCARD0....COCO_EXTCARD4, COCO_EXTCARD_MEGRA, COCO_EXTCARD_CURRENT
	// wSource	= COCO_SOURCE0...COCO_SOURCE7, COCO_SOURCE_CURRENT
	// Rückgabe: LOWWORD->Bisherige Videoquelle
	//			 HIWORD ->Bisherige Erweiterungskarte
	//			 Im Fehlerfall COCO_ERROR

	DWORD GetInputSource(BYTE byCoCoID);
	// Liefert die aktuelle Videoquelle und Erweiterungskarte
	// byCoCoID = COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT
	// Rückgabe: LOWWORD->Aktuelle Videoquelle
	//			 HIWORD ->Aktuelle Erweiterungskarte
	//			 Im Fehlerfall COCO_ERROR

	BYTE GetCoCoID();
	// Liefert die augenblicklich eingestellte CoCoID. (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	
	WORD SetFeSourceType(WORD wSourceType);
	// Setzt den SourceType der aktuellen Videoquelle
	// wSourceType	= COCO_FBAS, COCO_SVHS
	// Rückgabe: Bisherigen SourceType
	//			 Im Fehlerfall COCO_ERROR

	WORD GetFeSourceType();
	// Liefert den SourceType der aktuell gewählten Videoquelle (COCO_FBAS, COCO_SVHS)
	
	WORD SetFeVideoFormat(WORD wFeVideoFormat);
	// Setzt das FrontEnd-Videoformat der aktuellen Videoquelle
	// wFeVideoFormat	= COCO_PAL, COCO_NTSC
	// Rückgabe: Bisheriges FrontEnd-Videoformat
	//			 Im Fehlerfall COCO_ERROR

	WORD GetFeVideoFormat();
	// Liefert das FrontEnd-Videoformat der aktuell gewählten Videoquelle. (COCO_PAL, COCO_NTSC) 
	
	WORD SetBeVideoFormat(WORD wBeVideoFormat);
	// Setzt das BackEnd-Videoformat
	// wBeVideoFormat = COCO_PAL, COCO_NTSC 
	// Rückgabe: Bisheriges	BackEnd-Videoformat.
	//			 Im Fehlerfall COCO_ERROR

	WORD GetBeVideoFormat();
	// Liefert das aktuell gewählte BackEnd-Videoformat. (COCO_PAL, COCO_NTSC)

	WORD SetBeVideoType(WORD wBeVideoType);
	// Setzt die BackEnd-Videoausgabe Art
	// wBeVideoFormat = COCO_COMPOSITE, COCO_RGB 
	// Rückgabe: Bisherige	BackEnd-Videoausgabeart.
	//			 Im Fehlerfall COCO_ERROR

	WORD GetBeVideoType();
	// Liefert die aktuell gewählte BackEnd-Videoausgabeart. (COCO_COMPOSITE, COCO_RGB)

	BOOL DlgLuminance();
	// Öffnet einen modless Dialog zur Einstellung von Kontrast, Helligkeit, Farbsättigung und Hue

	WORD SetFeBrightness(WORD wBrightness);
	// Setzt die Helligkeit für die aktuelle Quelle
	// wBrightness	 = Helligkeitseinstellung (COCO_MIN_BRIGHTNESS...COCO_MAX_BRIGHTNESS)
	// Rückgabe: Bisheriges Helligkeitseinstellung
	// Notes:	 liegt wBrightness außerhalb der gültigen Grenzen, wird diese begrenzt.

	WORD GetFeBrightness();
	// Liefert die Helligkeitseinstellung der aktuell gewählten Videoquelle
	// Der Wert liegt innerhalb der Grenzen (COCO_MIN_BRIGHTNESS...COCO_MAX_BRIGHTNESS)

	WORD SetFeContrast(WORD wContrast);
	// Setzt den Kontrast für die aktuelle Quelle
	// wContrast	 = Kontrasteinstellung (COCO_MIN_CONTRAST...COCO_MAX_CONTRAST)
	// Rückgabe: Bisherige Kontrasteinstellung
	// Notes:	 liegt wContrast außerhalb der gültigen Grenzen, wird dieser begrenzt.

	WORD GetFeContrast();
	// Liefert die Kontrasteinstellung der aktuell gewählten Videoquelle
	// Der Wert liegt innerhalb der Grenzen (COCO_MIN_CONTRAST...COCO_MAX_CONTRAST)

	WORD SetFeSaturation(WORD wSaturation);
	// Setzt die Farbsättigung für die aktuelle Quelle
	// wSaturation	 = Farbsättigung (COCO_MIN_SATURATION...COCO_MAX_SATURATION)
	// Rückgabe: Bisherige Farbsättigung
	// Notes:	 liegt wSaturation außerhalb der gültigen Grenzen, wird diese begrenzt.

	WORD GetFeSaturation();
	// Liefert die Farbsättigung der aktuell gewählten Videoquelle
	// Der Wert liegt innerhalb der Grenzen (COCO_MIN_SATURATION...COCO_MAX_SATURATION)

	WORD SetFeHue(WORD wHue);
	// Setzt den 'Hue' für die aktuelle Quelle
	// wHue	 = Farbsättigung (COCO_MIN_HUE...COCO_MAX_HUE)
	// Rückgabe: Bisherige Farbsättigung
	// Notes:	 liegt wHue außerhalb der gültigen Grenzen, wird diese begrenzt.

	WORD GetFeHue();
	// Liefert den 'Hue' der aktuell gewählten Videoquelle
	// Der Wert liegt innerhalb der Grenzen (COCO_MIN_HUE...COCO_MAX_HUE)

	WORD SetFeHScale(WORD wHScale);
	// Setzt die horizontale Skalierung im BT819
	// wHScale = horizontaler Skalierungswert (COCO_MIN_HSCALE...COCO_MAX_HSCALE)
	// Rückgabe bisherige Skalierung
	// Notes:	 liegt wHue außerhalb der gültigen Grenzen, wird dieser begrenzt.

	WORD GetFeHScale();
	// Liefert die aktuelle horizontale Skalierung des BT819
		
	WORD SetFeVScale(WORD wVScale);
	// Setzt die vertikale Skalierung im BT819
	// wVScale = horizontaler Skalierungswert (COCO_MIN_VSCALE...COCO_MAX_VSCALE)
	// Rückgabe: Bisherige Skalierung

	WORD GetFeVScale();
	// Liefert die aktuelle vertikale Skalierung des BT819

	WORD SetFeFilter(WORD wFilter);
	// Aktiviert bzw. deaktiviert die beiden Filter des BT819
	// wFilter = COCO_LNOTCH_ON, COCO_LNOTCH_OFF
	//			 COCO_LDEC_ON, COCO_LDEC_OFF
	// Rückgabe: Bisherige Filtereinstellung.
	//			 Im Fehlerfall COCO_ERROR

	WORD GetFeFilter();
	// Liefert die augenblickliche Filtereinstellung des BT819

	WORD GetCurrentAlarmStatus(BYTE byCoCoID, WORD wExtCard);
	// Liefert den aktuellen Inhalt des CurrentAlarmstatus Registers, bzw. im 
	// Fehlerfall COCO_ERROR
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// Rückgabe: CurrentAlarmstatus: xxxxxxxx00000000...xxxxxxxx11111111
	//			 Im Fehlerfall COCO_ERROR

	WORD GetAlarmStatus(BYTE byCoCoID, WORD wExtCard);
	// Liefert den aktuellen Inhalt des Alarmstatus Registers, bzw. im
	// Fehlerfall COCO_ERROR.
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// Rückgabe: Alarmstatus: xxxxxxxx00000000...xxxxxxxx11111111
	//			 Im Fehlerfall COCO_ERROR
	
	WORD SetAlarmEdge(BYTE byCoCoID, WORD wExtCard, WORD wEdge);
	// Legt Bitweise die Alarmflanke fest
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// wEdge = 00000000B...11111111B
	// Rückgabe: Die bisherige Einstellung
	//			 Im Fehlerfall COCO_ERROR

	WORD GetAlarmEdge(BYTE byCoCoID, WORD wExtCard);
	// Liefert die augenblicklich eingestellte AlarmEdge der gewünschten Karte
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// Rückgabe: wEdge = 00000000B...11111111B
	//			 Im Fehlerfall COCO_ERROR

	WORD SetAlarmAck(BYTE byCoCoID, WORD wExtCard, WORD wAck);
	// Sperrt Bitweise die Alarmeingänge
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// wAck		= 00000000B...11111111B
	// Rückgabe: Die bisherige Einstellung
	//			 Im Fehlerfall COCO_ERROR

	WORD GetAlarmAck(BYTE byCoCoID, WORD wExtCard);
	// Liefert den augenblicklich eingestellten AlarmAcknoledge der gewünschten Karte
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// Rückgabe: wAck = 00000000B...11111111B
	//			 Im Fehlerfall COCO_ERROR

	WORD SetRelais(BYTE byCoCoID, WORD wExtCard, WORD wRelais);
	// Setzt die Relaisausgänge der gewünschten Erweiterungskarte
	// wExtCard = COCO_EXTCARD0...COCO_EXTCARD3
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// wRelais	= xxxx0000B...xxxx1111B
	// Rückgabe: Bisherige Relaiseinstellung
	//			 Im Fehlerfall COCO_ERROR
	
	WORD GetRelais(BYTE byCoCoID, WORD wExtCard);
	// byCoCoID = Nummer der CoCo-Karte (0...MAX_COCO_ID, COCO_COCOID_CURRENT)
	// wExtCard = COCO_EXTCARD0... COCO_EXTCARD3, COCO_EXTCARD_CURRENT)
	// Rückgabe: Aktuelle Relaiseinstellung	der gewünschten Karte.
	//			 Im Fehlerfall COCO_ERROR

	BOOL SendMCIScript(BYTE byCoCoID, LPCSTR lpScriptFileName);
	// Sendet ein MCI-Scriptfile an die Serverapplikation
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// lpScriptFileName = Pointer auf den Scriptdateinamen (+Pfad)
	// Rückgabe: TRUE = Alles OK, ansonsten FALSE

	BOOL WriteData(PARAMSTRUCT &Param);
	// Überträgt Daten an den Treiber
	// Rückgabe:  TRUE->Alles Ok

	BOOL RequestData(BYTE byCoCoID, WORD wData);
	// Fordert Daten vom Encoder an.
	// byCoCoID = Nummer der CoCo-Karte (0...MAX_COCO_ID, COCO_COCOID_CURRENT)
	// wData	= Spezifiziert den Typ der Daten (z.Z. lediglich COCO_ENCODER_DATA)
	// Rückgabe: TRUE->Anforderung bestätigt, ansonsten FALSE
	//			 Im Fehlerfall wird 0L zurückgeliefert.
	// Notes:    Der Aufruf dieser Funktion bewirkt den erhalt der Messsage	'WM_COPY_DATA'
	//			 sofern Daten im Encoderbuffer vorliegen.

	BOOL ReadData(LPARAM lParam, PARAMSTRUCT &Param);
	// Holt Daten vom Treiber
	// lParam   = lParam beim Eintreffen der 'WM_COPYDATA'-Message
	// Rückgabe:	&Param	= Referenz auf eine Struktur mit Zusatzparametern
	//				TRUE	= Kein Fehler aufgetreten.
	// Notes:	  Diese Funktion MUß bei erhalt der Message WM_COPY_DATA aufgerufen
	//		      werden. Der zurückgelieferte Pointer ist temporär, d.h. er darf nicht 
	//			  gespeichert werden, um zu einem späteren Zeitpunkt auf die Daten zuzugreifen.
	//			  Soll auf die Daten später zugegriffen werden, muß eine Kopie der Daten
	//			  angefertigt werden.

	// lpSource = Zeigt nach dem Aufruf auf die empfangenen Daten
	// dwLen	= Enthält nach dem Aufruf die Anzahl der übertragenen Bytes.
	// Rückgabe:  LOWORD(dwRet) = (z.Z. lediglich COCO_ENCODER_DATA)
	//			  HIWORD(dwRet) = CoCoID (COCO_COCOID0...COCO_COCOID3)
	// Notes:	  Diese Funktion MUß bei erhalt der Message WM_COPY_DATA aufgerufen
	//		      werden. Der zurückgelieferte Pointer ist temporär, d.h. er darf nicht 
	//			  gespeichert werden, um zu einem späteren Zeitpunkt auf dei Daten zuzugreifen.
	//			  Soll auf die Daten später zugegriffen werden, muß eine Kopie der Daten
	//			  angefertigt werden.

	BOOL EncoderStart(BYTE byCoCoID);
	// Startet den Encoder
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe:  TRUE->Alles Ok

	BOOL EncoderStop(BYTE byCoCoID);
	// Stopt den Encoder. Alle Daten die sich im Augenblick des Stopkommandos
	// im Encoderbuffer befinden, werden an die CoCoUnit geschickt.
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe:  TRUE->Alles Ok
	
	BOOL EncoderBreak(BYTE byCoCoID);
	// Stopt den Encoder. Alle Daten die sich im Augenblick des Stopkommandos
	// im Encoderbuffer befinden, gehen verloren.
 	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe:  TRUE->Alles Ok
	
	WORD EncoderSetFormat(BYTE byCoCoID, WORD wEncFormat);
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wEncFormat = COCO_ENCODER_CIF, COCO_ENCODER_QCIF
	// Rückgabe: Bisherige Einstellung, bzw. COCO_ERROR

	WORD EncoderGetFormat(BYTE byCoCoID);
	// Liefert die gewählte Encoderauflösung
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)

	WORD EncoderSetFramerate(BYTE byCoID, WORD wFramerate); 
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// wFramerate = 1, 2, 3, 4 ( SFR/1, SFR/2, SFR/3, SFR/4)
	// Rückgabe: Bisherige Einstellung, bzw. COCO_ERROR
	
	WORD EncoderGetFramerate(BYTE byCoID); 
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Liefert die gewählte Framerate
	
	DWORD EncoderSetBitrate(BYTE byCoID, DWORD dwBitrate); 
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// dwBitrate = 64000...384000
	// Rückgabe: Bisherige Einstellung, bzw. COCO_ERROR

	DWORD EncoderGetBitrate(BYTE byCoID); 
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Liefert die gewählte Bitrate

	BOOL EncoderSetMode(BYTE byCoCoID, WORD wFormat, WORD wFramerate, DWORD dwBitrate);
	// Fast die Funktionen 'EncoderSetFormat', 'EncoderSetFramerate', 'EncoderSetBitrate'
	// zu einem Funktionsaufruf zusammen.

	int EncoderGetState(BYTE byCoID); 
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Liefert den aktuellen Encoderstatus

	BOOL DecoderStart(BYTE byCoCoID);
	// Startet den Decoder
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe:  TRUE->Alles Ok

	BOOL DecoderStop(BYTE byCoCoID);	
	// Stopt den Decoder. Alle Daten die sich zum Zeitpunkt des Stopkommandos in der
	// Decoderqueue befinden, werden noch dargestellt.
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe:  TRUE->Alles Ok

	BOOL DecoderBreak(BYTE byCoCoID);	
	// Stopt den Decoder. Der Decoder stoppt sofort. Daten die sich schon in der
	// Decoderqueue befinden, werden nicht mehr dargestellt.
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe:  TRUE->Alles Ok

	DWORD DecoderSetBitrate(BYTE byCoCoID, DWORD dwBitrate);
	// dwBitrate = 64000...384000
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Rückgabe: Bisherige Einstellung, bzw. COCO_ERROR

	DWORD DecoderGetBitrate(BYTE byCoID); 
	// Liefert die gewählte Bitrate
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)

	int DecoderGetState(BYTE byCoID); 
	// byCoCoID = Nummer der CoCo-Karte (COCO_COCOID0...COCO_COCOID3, COCO_COCOID_CURRENT)
	// Liefert den aktuellen Decoderstatus

	BOOL ConnectVideoOutput(BYTE byCoCoID, WORD wConnect);
	// Legt fest welche Unit innerhalb des AVP's mit der VideoOut-Unit
	// verbunden werden soll.
	// byCoCoID = Nummer der CoCo-Karte (0...MAX_COCO_ID, COCO_COCOID_CURRENT)
	// wConnect = COCO_VIDIN, COCO_DECODER, COCO_ENCODER, COCO_PREP, COCO_POST

private:
	HWND LoadServerApp();	
	BOOL Init();

	// Ein Satz von Member, die die Übergabeparameter überprüfen.
	BOOL CheckCoCoID(BYTE byCoCoID);
	BOOL CheckSource(WORD wSource);
	BOOL CheckExtCard(WORD wExtCard);
	BOOL CheckSourceType(WORD wSourceType);
	BOOL CheckFeVideoFormat(WORD wFeVideoFormat);
	BOOL CheckBeVideoFormat(WORD wBeVideoFormat);
	BOOL CheckBeVideoType(WORD wBeVideoType);
	BOOL CheckFeFilter(WORD wFilter);
	BOOL CheckEncoderFormat(WORD wFormat);
	BOOL CheckEncoderFramerate(WORD wFramerate);
	BOOL CheckEncoderBitrate(DWORD dwBitrate);
	BOOL CheckVideoOutput(WORD wConnect);
	BOOL CheckRange(LPWORD lpwValue, long lMin, long lMax);
	
	BOOL m_bOK;
	WORD m_wLastError;								// Status des Objektes. TRUE=OK
	BYTE m_byCoCoID;								// Aktuelle CoCoID (COCO_COCOID0...COCO_COCOID3)
	WORD m_wExtCard;								// Aktueller Erweiterungskarte (COCO_EXTCARD0...COCO_EXTCARD4,
													//								COCO_EXTCARD_MEGRA)
	WORD m_wSource;									// Aktuelle Videoquelle (COCO_SOURCE0...COCO_SOURCE7)
	DWORD m_dwHardwareState;						// Gibt über verfügbare Hardware auskunft.
	COCOISASTRUCT	m_CoCoISA[MAX_COCO_ID+1];
	CDlgColor*		m_pDlgColor;					// Pointer auf Dialog
	HINSTANCE	m_hThisModule;
	WORD	m_wBeVideoFormat[MAX_COCO_ID+1];		// COCO_PAL, COCO_NTSC
	WORD	m_wBeVideoType[MAX_COCO_ID+1];			// COCO_COMPOSITE, COCO_RGB
	WORD	m_wFeHScale[MAX_COCO_ID+1];				// Horizontale Skalierung des BT819
	WORD	m_wFeVScale[MAX_COCO_ID+1];				// Vertikale Skalierung des BT819

	HWND	m_hWndClient;							// Fensterhandle der Anwendung
	HWND	m_hWndServer;							// Fensterhandle der Serverapplikation
	char	m_szAppIniName[256];					// Name der INI-Datei
	CString m_sServerPathName;						// Name und Pfad der Serverapplikation

	
	DWORD	m_dwTimeOut;							// Timeout in ms, bis Serverapplikation bereit ist.
	CWmCopyData* m_pCopyData;						// Objekt zur IPC mittels 'WM_COPYDATA'
	BOOL	m_bStandalone;							// 1=CoCoUnit Standalone-App
	BOOL    m_bIgnoreRelais4;
};

#endif // __CCOCO_H__
#endif // __cplusplus
// Der Treiber verschickt an die angemeldeten Applikationen bei bedarf die folgenden
// Windowsmessages:

// WM_COCO_CHANGED_SOURCE
//		Die aktuelle Videoquelle oder Erweiterungskarte wurde gewechselt.
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		LOWORD(lParam) = wNewAVPortYC (Physikalischer Videoanschluß)
//						 LOBYTE(wNewAVPortYC) = u8AVPortY (Portnummer für Y-Signal (0...7))
//						 HIBYTE(wNewAVPortYC) = u8AVPortC (Portnummer für C-Siganl (0...7))
//		HIWORD(lParam) = Nummer der neuen Erweiterungskarte (COCO_EXTCARD0... COCO_EXTCARD3)

// WM_COCO_CHANGED_FE_SOURCE_TYPE
//		Der SourceType (COCO_FBAS, COCO_SVHS) wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neuer Sourcetype

// WM_COCO_CHANGED_FE_VIDEO_FORMAT
//		Das Videoformat (COCO_PAL, COCO_NTSC)) wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neues Videoformat

// WM_COCO_CHANGED_FE_BRIGHTNESS
//		Die Helligkeitseinstellung wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue Helligkeitseinstellung

// WM_COCO_CHANGED_FE_CONTRAST 
//		Die Kontrasteinstellung wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue Kontrasteinstellung

// WM_COCO_CHANGED_FE_SATURATION 
//		Die Farbsättigung wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue Farbsättigung

// WM_COCO_CHANGED_FE_HUE	
//		Die Hueeinstellung wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue Hueeinstellung

// WM_COCO_CHANGED_FE_HSCALE
//		Die horizontale Skalierung im BT819 wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue horizontale Skalierung

// WM_COCO_CHANGED_FE_VSCALE
//		Die vertikale Skalierung im BT819 wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue vertikale Skalierung

// WM_COCO_CHANGED_FE_FILTER	
//		Die Filtereinstellung im BT819 wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue Filtereinstellung	(COCO_LNOTCH_ON/OFF, COCO_LDEC_ON/OFF)

// WM_COCO_CHANGED_BE_VIDEO_FORMAT	
//		Die BE-Videoformateinstellung (COCO_PAL, COCO_NTSC) wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue BE-Videoformateinstellung

// WM_COCO_CHANGED_BE_VIDEO_TYPE
//		Die BE-Videotypeeinstellung (COCO_COMPOSITE, COCO_RGB) wurde geändert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		lParam = Neue BE-Videotypeeinstellung

// WM_COCO_CHANGED_RELAIS
//		Ein Relaisausgang wurde geändert.
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		LOWORD(lParam) = Nummer der Erweiterungskarte (COCO_EXTCARD0... COCO_EXTCARD3)
//		HIWORD(lParam) = Neue Relaisstellung

// WM_COCO_CHANGED_ALARM_ACK	
//		Der Wert des Alarm-Acknoledgeregister hat sich geädert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		LOWORD(lParam) = Nummer der Erweiterungskarte (COCO_EXTCARD0... COCO_EXTCARD3)
//		HIWORD(lParam) = Neuer Wert im Alarm-Acknoledgeregister

// WM_COCO_CHANGED_ALARM_EDGE	
//		Der Wert des Alarm-Edgeregister hat sich geädert
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		LOWORD(lParam) = Nummer der Erweiterungskarte (COCO_EXTCARD0... COCO_EXTCARD3)
//		HIWORD(lParam) = Neuer Wert im Alarm-Edgeregister

// WM_COCO_ALARM		
//		Ein Alarm wurde festgestellt.
//		wParam = CoCoID (COCO_COCOID0...COCO_COCOID3) des CoCos
//		LOWORD(lParam) = Nummer der Erweiterungskarte (COCO_EXTCARD0... COCO_EXTCARD3)
//		HIWORD(lParam) = Wert im AlarmStatus-Register

