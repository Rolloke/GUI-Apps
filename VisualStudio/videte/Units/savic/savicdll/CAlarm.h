/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Calarm.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Calarm.h $
// CHECKIN:		$Date: 15.01.03 11:53 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 15.01.03 10:13 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CALARMSAVIC_H__ 
#define __CALARMSAVIC_H__

class CIoMemory;

class CAlarm
{
public:
	CAlarm(DWORD dwIOBase, DWORD dwBoardVariation);
	virtual ~CAlarm();
	virtual BOOL IsValid() const;

	virtual WORD GetState();
	// Liefert den aktuellen Status der 8/16 Alarmeingänge (Rückgabe als 8/16Bit Maske) 
	
	virtual void SetEdge(WORD wState);
	// Programmiert die 16 Alarmeingänge dahingehend, ob diese mit steigender,
	// oder mit fallender Flanke einen Alarm registrieren.

	virtual WORD GetEdge() const;
	// Liefert den mit 'SetEdge()' gesetzten Wert zurück.

	void ClearLatch();
	// Setzt das Latch zurück


protected:
	CIoMemory*  m_pCIo;
	WORD		m_wEdge;
	DWORD		m_dwBoardVariation;
};
		
#endif // __CALARMSAVIC_H__


