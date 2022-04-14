/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cwatchdog.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/cwatchdog.h $
// CHECKIN:		$Date: 15.10.02 13:04 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 15.10.02 11:02 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CWATCHDOG_H__
#define __CWATCHDOG_H__

class CPCF8563;
class CIoMemory;

class CWatchDog
{
public:
	CWatchDog(DWORD dwIOBase);
	virtual ~CWatchDog();

public:
	virtual BOOL TriggerWatchDog(WORD wTi) const;
	// Triggert den Watchdog.
	// wTi = Zeit in Sekunden, bis der Watchdogcounter 0 erreicht.
	
	virtual BOOL EnableWatchDog() const;
	// Aktiviert den Watchdog

	virtual BOOL DisableWatchDog() const;
	// Deaktiviert den Watchdog
	
	virtual BOOL Diagnostic() const;
	// Testet den Watchdog.
	// Liefert im Fehlerfall FALSE zurück


protected:
	virtual BOOL GetWatchDogState() const;
	// Liefert den Status des Watchdogrelays zurück. TRUE -> Relay angezogen

	CPCF8563*	m_pPCF8563;
	CIoMemory*	m_pCIo;			// Zugriffsobjekt

};

#endif // __CWATCHDOG_H__
