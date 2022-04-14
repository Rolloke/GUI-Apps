/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Ci2c.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Ci2c.h $
// CHECKIN:		$Date: 27.06.02 15:03 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 27.06.02 8:00 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef  __CI2C_H__
#define __CI2C_H__

class CWK_PerfMon;
class CI2C  
{
public:
	CI2C(DWORD dwIOBase, BYTE bySlaveAdress);
	// Legt ein I2C Objekt an
	// dwIOBase = Basisadresse des SaVicboards
	// bySlaveAdress = I2C-Bus Adresse des anzusprechenden Bausteins

	virtual ~CI2C();
	
	virtual BOOL IsValid() const;
	// Liefert TRUE, wenn das I2C Objekt korrekt angelegt wurde.

	virtual BOOL ReadFromI2C(BYTE bySubAddress, BYTE& byValue);
	// Liest ein Byte über den I2C-Bus
	// bySubAddress = Adresse an der gelesen werden soll
	// byValue = Referenz auf ein Byte, daß nach dem Aufruf den gelesen
	//			 Wert enthält.
	// Rückgabe: FALSE -> Es ist ein Fehler aufgetreten
	
	virtual BOOL WriteToI2C(BYTE bySubAddress, BYTE byValue);
	// Schreibt ein Byte über den I2C-Bus
	// bySubAddress = Adresse an der gelesen werden soll
	// byValue = Date diegeschrieben werden soll.
	// Rückgabe: FALSE -> Es ist ein Fehler aufgetreten

private:
	static  CCriticalSection	m_csI2C;// Synchronisations-Objekt.

	BYTE		m_bySlaveAddress;		// Slaveadresse
	DWORD		m_dwIOBase;				// Basisadresse
};

#endif // __CI2C_H__
