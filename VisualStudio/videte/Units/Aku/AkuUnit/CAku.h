/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CAku.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CAku.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CAKU__
#define __CAKU__

#include "CIo.h"
#include "wk_msg.h"

class CRelay;
class CAlarm;
class CIo;

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

#define AKU_VERSION		0x0001
#define AKU_INI			"AkuUnit"

#define WM_AKU_ALARM	AKU_MSGBASE + 0

// Aku Karten
#define AKU_CARD0		0
#define AKU_CARD1		1
#define AKU_CARD2		2
#define AKU_CARD3		3
#define AKU_MAX_CARD	AKU_CARD3 +1
#define AKU_CARD_NO		255

// Fehlermeldungen
#define AKU_ERR_NO_ERRROR			 0
#define AKU_ERR_WRONG_EXT_CARD		 1
#define AKU_ERR_UNKNOWN_PARAMETER  	 2
#define AKU_ERR_PARM_OUT_OF_RANGE	 3
#define AKU_ERROR					 0xffff

typedef struct
{
	WORD	wIOBase[AKU_MAX_CARD];		// Basisadressen der Aku-Karten
	HWND	hWnd;						// Fensterhandle der AkuUnit			
} CONFIG;

/////////////////////////////////////////////////////////////////////////////
// CAku
class CAku
{
public:
	CAku(HWND hWnd, LPCSTR lpIniAppName);
	// hWnd			= Fensterhandle der Applikation. An dieses Fenster verschickt der Treiber
	//		  		  Meldungen, wenn nötig
	// lpIniAppName = Pointer auf den Namen der INI-Datei

	~CAku();
	//	Abmelden der Applikation beim Server
	//	Ist keine Applikation mehr angemeldet, so passiert folgendes:
	//		- Beenden der Serverapplikation	. Dies bewirkt wiederum folgendes:
	//			- Abmelden beim AVP 
	//			- Schließen des MCI-Device-Treibers
	//			- Beenden aller avpXXX-Dll's
			
	BOOL IsValid();
	// Liefert TRUE, wenn das CAku-Objekt fehlerfrei angelegt wurde, ansonsten FALSE.

	BOOL SaveConfig();
	// Siehert alle Einstellungen

	WORD GetVersion();
	// Liefert eine Versionsnummer des Aku-Treibers

	WORD GetLastError();
	// Liefert einen der folgenden Fehlerkodes zurück.
	// Das Lesen bewirkt das Rücksetzen des Fehlerstatuses.

	BOOL CheckHardware();
	// Testet die Hardware
	// Liefert TRUE, wenn kein Fehler erkannt wurde, ansonsten FALSE
	
	BOOL PollAlarm();
	// Pollt auf den Alarmeingängen

	DWORD GetHardwareState();
	// Gibt Auskunft darüber welche Karten (Aku's, ExtCard's) im System eingebunden sind
	// Rückgabe:	Bit 0...3  -> 	ExtCard 0...3
	// Gesetztes Bit heißt Karte in System.ini eingetragen

	BYTE GetCurrentAlarmState(WORD wExtCard);
	// Liefert den aktuellen Inhalt des CurrentAlarmstatus Registers, bzw. im 
	// Fehlerfall AKU_ERROR
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// Rückgabe: CurrentAlarmstatus: 00000000...11111111
	//			 Im Fehlerfall AKU_ERROR

	BYTE GetAlarmState(WORD wExtCard);
	// Liefert den aktuellen Inhalt des Alarmstatus Registers, bzw. im
	// Fehlerfall AKU_ERROR.
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// Rückgabe: Alarmstatus: 00000000...11111111
	
	BYTE SetAlarmEdge(WORD wExtCard, BYTE byEdge);
	// Legt Bitweise die Alarmflanke fest
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// byEdge = 00000000B...11111111B
	// Rückgabe: Die bisherige Einstellung

	BYTE GetAlarmEdge(WORD wExtCard);
	// Liefert die augenblicklich eingestellte AlarmEdge der gewünschten Karte
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// Rückgabe: wEdge = 00000000B...11111111B

	BYTE SetAlarmAck(WORD wExtCard, BYTE byAck);
	// Sperrt Bitweise die Alarmeingänge
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// byAck	= 00000000B...11111111B
	// Rückgabe: Die bisherige Einstellung

	BYTE GetAlarmAck(WORD wExtCard);
	// Liefert den augenblicklich eingestellten AlarmAcknoledge der gewünschten Karte
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// Rückgabe: wAck = 00000000B...11111111B
	//			 Im Fehlerfall AKU_ERROR

	BYTE SetRelais(WORD wExtCard, BYTE byRelais);
	// Setzt die Relaisausgänge der gewünschten Erweiterungskarte
	// wExtCard = AKU_CARD0...AKU_CARD3
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// wRelais	= 00000000B...11111111B
	// Rückgabe: Bisherige Relaiseinstellung
	
	BYTE GetRelais(WORD wExtCard);
	// wExtCard = AKU_CARD0... AKU_CARD3, AKU_CARD_CURRENT)
	// Rückgabe: Aktuelle Relaiseinstellung	der gewünschten Karte.
	//			 Im Fehlerfall AKU_ERROR

	BOOL  m_bRun;

private:
	// Ein Satz von Member, die die Übergabeparameter überprüfen.
	BOOL CheckExtCard(WORD wExtCard);
	BOOL CheckRange(LPWORD lpwValue, long lMin, long lMax);
	
	BOOL  m_bOK;
	WORD  m_wLastError;							// Status des Objektes. TRUE=OK
	WORD  m_wExtCard;							// Aktueller Erweiterungskarte (AKU_CARD0...AKU_CARD4,
	WORD  m_dwHardwareState;					// Gibt über verfügbare Hardware auskunft.
	BYTE  m_byAlarmState[AKU_MAX_CARD];
	CWinThread*		m_pAlarmPollThread;
	HWND	m_hWndClient;						// Fensterhandle der Anwendung
	char	m_szAppIniName[_MAX_PATH];			// Name der INI-Datei

	CRelay* m_pRelay[AKU_MAX_CARD];				// Pointer auf Relay-Objekte
	CAlarm* m_pAlarm[AKU_MAX_CARD];				// Pointer auf Alarm-Objekte
	CIo		m_Io;								// IO-Access
	CONFIG	m_Config;							// Konfigurationsparameter

	CCriticalSection m_csAku;
};

#endif // __CAKU_H__

