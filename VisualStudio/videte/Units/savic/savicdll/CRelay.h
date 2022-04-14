/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Crelay.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Crelay.h $
// CHECKIN:		$Date: 15.01.03 11:53 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 15.01.03 10:12 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CRELAY_H__ 
#define __CRELAY_H__
class CIoMemory;
 
class CRelay
{						  
public:
	CRelay(DWORD dwIOBase);

	virtual ~CRelay();

	virtual BOOL IsValid() const;
	// Liefert TRUE, wenn das Objekt korrekt angelegt werden konnte

	virtual void SetRelay(BOOL bSet) const;
	// Setzt/Löscht das Relay

	virtual BOOL GetRelay() const;
	// Liefert den den augenblicklichen Status des Relays

	virtual void BeeperOn() const;
	// Schalten den Störmelder über das 4. Relay des SaVic-Board ein

	virtual void BeeperOff() const;
	// Schalten den Störmelder über das 4. Relay des SaVic-Board aus

	BOOL Diagnostic(DWORD dwBoardVariations) const;
	// Testet die Relays mit Hilfe des Gehörs

	BOOL Diagnostic2(DWORD dwBoardVariations) const;
	// Testet die Relay mit Hilfe des ersten Alarmeinganges

protected:
	CIoMemory*	m_pCIo;		// I/O-Objekt
	DWORD		m_dwIOBase;	// I/O-Basisadresse

private:
	void ClearLatch() const;
};
		
#endif // __CRELAY_H__
