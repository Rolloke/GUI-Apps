/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CI2C.H $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CI2C.H $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CI2C_H__ 
#define __CI2C_H__

#include "CIo.h"
 
class CI2C
{
	protected:	 				
		CI2C(WORD wIOBase, BYTE bySlaveAddress);
		~CI2C();
		BOOL IsValid();
		BOOL WriteToI2C(BYTE bySubAddress, BYTE bValue);
		BYTE ReadFromI2C(BYTE bySubAddress);

	private:
		void PCD8584Reset();
		BOOL PCD8584Init(BYTE byOwnAddress, BYTE bySerialClock);
		BYTE PCD8584In8(BYTE byRegister);		
		void PCD8584Out8(BYTE byRegister, BYTE byData);
		BOOL PCD8584WaitUntilPinFree();
		BOOL PCD8584WaitUntilBusReady();
		BOOL PCD8584TestAck();		 		
		void PCD8584Wait(int nCount);		 		
				
		WORD 		 m_wIOBase;
		BYTE 		 m_bySlaveAddress;
		
		static int 	 m_nInstCnt;
		static DWORD m_dwCyclePerMs;
		static CCriticalSection	m_csI2C;

		BOOL		 m_bOk;
		CIo  		 IoAccess;
};

		
#endif // __CI2C_H__


