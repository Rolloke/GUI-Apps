/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmware
// FILE:		$Workfile: Calarm.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/Calarm.h $
// CHECKIN:		$Date: 11.12.01 9:23 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 8:23 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CALARMJACOB_H__ 
#define __CALARMJACOB_H__
 
class CAlarm
{
public:
	CAlarm();
	virtual ~CAlarm();

	virtual DWORD GetState() const;
	// Liefert den aktuellen Status der Alarmeingänge (Rückgabe als 32Bit Maske) 
	
	virtual void SetEdge(DWORD dwState);
	// Programmiert die 16 Alarmeingänge dahingehend, ob diese mit steigender,
	// oder mit fallender Flanke einen Alarm registrieren.

	virtual DWORD GetEdge() const;
	// Liefert den mit 'SetEdge()' gesetzten Wert zurück.

protected:
	DWORD m_dwAlarmStateMask;
	DWORD m_dwEdgeStateMask;

};
		
#endif // __CALARM_H__


