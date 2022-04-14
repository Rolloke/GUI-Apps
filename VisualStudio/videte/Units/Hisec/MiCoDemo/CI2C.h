#ifndef __CI2C_H__ 
#define __CI2C_H__

#include "CIo.h"
 
class CI2C
{
	protected:	 				
		CI2C(WORD wIOBase, BYTE bySlaveAddress);
		// Konstruktor:
		//		wIoBase=Basisadresse des Boards
		//		bySlaveAdress=Slaveadresse des I2C-Bus Bausteines

		~CI2C();
		// Destruktor

		BOOL IsValid();
		// Liefert TRUE, wenn das I2C-Objekt korrekt angelegt werden konnte
		
		BOOL WriteToI2C(BYTE bySubAddress, BYTE bValue);
		// Schreibt einen 8Bit Wert in ein Register des I2C-Bus Bausteins

		BYTE ReadFromI2C(BYTE bySubAddress);
		// Liest einen 8Bit Wert aus einem Register des I2C-Bus Bausteins 

	private:
		void PCD8584Reset();
		// Hardwarereset des I2C-Bus Kontrollers
		
		BOOL PCD8584Init(BYTE byOwnAddress, BYTE bySerialClock);
		// Initialisierung des I2C-Bus Bausteines

		BYTE PCD8584In8(BYTE byRegister);		
		// Lesezugriff auf den I2C-Bus Kontroller

		void PCD8584Out8(BYTE byRegister, BYTE byData);
		// Schreibzugriff auf den I2C-Bus Kontroller
		
		BOOL PCD8584WaitUntilPinFree();
		// ? 

		BOOL PCD8584WaitUntilBusReady();
		// Warte bis I2C-Bus frei ist

		BOOL PCD8584TestAck();		 		
		// Liefert TRUE, wenn Acknoledge

		void PCD8584Wait(int nCount);		 		
		// Verzögerung um nCount-Microsekunden
		
		WORD 		 m_wIOBase;			// Basisadresse des Boards
		
		BYTE 		 m_bySlaveAddress;	// Slaveadresse des I2C-Bus bausteines
		
		static int 	 m_nInstCnt;		// Instanzenzähler
		static DWORD m_dwCyclePerMs;	// Anzahl der Schleifendurchläufe pro ms
		static CCriticalSection	m_csI2C;// Synchronisations-Objekt.

		BOOL		 m_bOk;				// TRUE -> Objekt gültig
		CIo  		 IoAccess;			// I/O-Zugriffs-Objekt
};
		
#endif // __CI2C_H__


