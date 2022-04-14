#ifndef __CALARM_H__ 
#define __CALARM_H__

#include "CIo.h"
 
class CAlarm
{
	public:
		CAlarm(WORD wIOBase, WORD wExtCard);

		BYTE GetCurrentState();
		// Liefert den aktuellen Status der 8 Alarmeingänge (Rückgabe als 8Bit Maske) 
		
		BYTE GetState();
		// Liefert den gelatchten Status der 8 Alarmeingänge (Rückgabe als 8Bit Maske)

		void SetEdge(BYTE byState);
		// Programmiert die 8 Alarmeingänge dahingehend, ob diese mit steigender,
		// oder mit fallender Flanke einen Alarm registrieren.
 
		BYTE GetEdge();
		// Liefert den mit 'SetEdge()' gesetzten Wert zurück.

		void SetAck(BYTE byAck);
		// Setzt bzw. löscht das Acknoledge-Bit der 8 Alarmeingänge

		BYTE GetAck();
		// Liefert den mit 'SetAck()' gesetzten Wert zurück

		BOOL IsValid();
		// Liefert TRUE, wenn das CAlarm-Objekt korrekt angelegt werden konnte

		BOOL CheckIt();
		// Überprüft die Funktionalität der Alarmeingänge durch das Auslösen eines
		// simmulierten Alarms

	private:
		WORD m_wExtCard;	// Nummer des Boards (0=MiCo, 1..3=CoVi0..CoVi8) 
		WORD m_wIOBase;		// Basisadresse des Boards
		BYTE m_byAck;		// Zuletzt gesetzte 8Bit Maske des Acknolede-Registers
		BYTE m_byEdge;		// Zuletzt gesetzte 8Bit Maske des Acknolede-Registers
		CIo  m_IoAccess;	// CIO-Objekt über die die I/O-Zugriffe erfolgen
};
		
#endif // __CALARM_H__


