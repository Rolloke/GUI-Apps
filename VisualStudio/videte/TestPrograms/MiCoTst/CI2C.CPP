/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CI2C.CPP $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CI2C.CPP $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CIo.h"
#include "ci2c.h"

CCriticalSection	CI2C::m_csI2C;
DWORD				CI2C::m_dwCyclePerMs = 0;
int					CI2C::m_nInstCnt = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CI2C::CI2C
CI2C::CI2C(WORD wIOBase, BYTE bySlaveAddress)
{
	m_csI2C.Lock();
	m_wIOBase 			= wIOBase;
	m_bySlaveAddress 	= bySlaveAddress;
	m_bOk				= FALSE;
		
	// Der I2C-Bus Controller soll nur bei der ersten Instanz initialisiert werden.
	m_nInstCnt++;
	if (m_nInstCnt > 1)
	{
		m_bOk = TRUE;
		m_csI2C.Unlock();
		return;
	}
	
	// Anzahl der Schleifendurchläufe pro ms bestimmen.
	DWORD dwStartTime = GetTickCount();
	for (DWORD dwI = 0; dwI <= 2500000; dwI++)
	{
		dwI += 1;	// Damit der Compiler die Schleife nicht wegoptimiert
		dwI -= 1;	// dito
	}
	DWORD dwTime = GetTickCount() - dwStartTime;

	if (dwTime != 0){
		m_dwCyclePerMs = 2500000 / dwTime;
	}
	else{
		m_dwCyclePerMs = 3000;
	}

//	WK_TRACE(TRUE, "CI2C::CI2C\tCycles per ms=%d\n", m_dwCyclePerMs);

	// I2C-Bus Controller initialisieren		
	if (PCD8584Init(0, I2C_SERIAL_CLOCK_90KHZ) == FALSE)
	{
		m_csI2C.Unlock();
		return;
	}		

	m_bOk = TRUE;
	m_csI2C.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CI2C::~CI2C
CI2C::~CI2C()
{
	m_csI2C.Lock();
	m_nInstCnt --;
	m_csI2C.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CI2C::IsValid
BOOL CI2C::IsValid()
{
	return m_bOk;//OOPS inline ??
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CI2C::WriteToI2C
BOOL CI2C::WriteToI2C(BYTE bySubAddress, BYTE byData)
{
	m_csI2C.Lock();
	PCD8584WaitUntilBusReady();

	PCD8584Out8(I2C_DATA_REG, m_bySlaveAddress); /* Write the slave address in register S0 */
	PCD8584Wait(100); // !
		        
	PCD8584Out8(I2C_CONTROL_REG, I2C_SERIAL_BUS_ON | I2C_START_CMD | I2C_ACK_BIT); /* Generate a start in S1 */
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();
	
	PCD8584Out8(I2C_DATA_REG, bySubAddress);      /* Write the slave address in register S0 */
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();
		
	PCD8584Out8(I2C_DATA_REG, byData);            /* write next value in S0 */
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();
		
	PCD8584Out8(I2C_CONTROL_REG, I2C_PIN_BIT | I2C_SERIAL_BUS_ON |	I2C_STOP_CMD | I2C_ACK_BIT); /* send stop condition */
	PCD8584Wait(100);

	m_csI2C.Unlock();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CI2C::ReadFromI2C
BYTE CI2C::ReadFromI2C(BYTE bySubAddress)
{
	m_csI2C.Lock();
	char byData;
	
	PCD8584WaitUntilBusReady();

	PCD8584Out8(I2C_DATA_REG, m_bySlaveAddress);        /* Write the slave address in register S0 */
	PCD8584Wait(100); // !
		        
	PCD8584Out8(I2C_CONTROL_REG, I2C_SERIAL_BUS_ON | I2C_START_CMD | I2C_ACK_BIT); /* Generate a start in S1 */
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();

	PCD8584Out8(I2C_DATA_REG, bySubAddress);          /* Write the subadress in register S0 */
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();

	PCD8584Out8(I2C_CONTROL_REG, I2C_PIN_BIT | I2C_SERIAL_BUS_ON |	I2C_STOP_CMD | I2C_ACK_BIT); /* send stop condition */
	PCD8584Wait(100);

	PCD8584Out8(I2C_DATA_REG, m_bySlaveAddress+1);       /* Write the slave address */
	PCD8584Wait(100); // !

    PCD8584Out8(I2C_CONTROL_REG, I2C_SERIAL_BUS_ON | I2C_START_CMD | I2C_ACK_BIT); /* Generate a start in S1 */
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();

	byData = PCD8584In8(I2C_DATA_REG);
	PCD8584WaitUntilPinFree();
	PCD8584TestAck();

	PCD8584Out8(I2C_CONTROL_REG, I2C_SERIAL_BUS_ON); /* Disable ACK + ES2=0 */
	byData = PCD8584In8(I2C_DATA_REG);
	PCD8584WaitUntilPinFree();

	PCD8584Out8(I2C_CONTROL_REG, I2C_PIN_BIT | I2C_SERIAL_BUS_ON |	I2C_STOP_CMD | I2C_ACK_BIT); /* send stop condition */
	PCD8584Wait(100);

	m_csI2C.Unlock();

	return byData;
}

//////////////////////////////////////////////////////////////////////////////////////
void CI2C::PCD8584Reset()
{
 	m_csI2C.Lock();

	BYTE byReset;

	// I2C-Bus Controller reseten...
	byReset = IoAccess.ReadFromLatch(m_wIOBase | RESET_OFFSET);

	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_PCF8584_BIT);

	IoAccess.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);
	
	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_PCF8584_BIT);

	IoAccess.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);

	m_csI2C.Unlock();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CI2C::PCD8584Init(BYTE byOwnAddress, BYTE bySerialClock)
{
	m_csI2C.Lock();

	BYTE byErr;

	// I2C-Bus Controller resetten
	PCD8584Reset();

	PCD8584Out8(I2C_CONTROL_REG, 0x80);			// this	is the "fake" write needed by the 8584 after reset.
	PCD8584Wait(100);

	PCD8584Out8(I2C_OWN_ADDRESS_REG, 0x55);		// program the 8584 slave address
	PCD8584Wait(100);

	PCD8584Out8(I2C_CONTROL_REG, I2C_ES1_BIT);	// setup for indirect write to clock control register S2
	PCD8584Wait(100);

	PCD8584Out8(I2C_DATA_REG, (BYTE)(bySerialClock | I2C_CLOCK_6MHZ));// write our clock rates to the	S0 register(base)
	PCD8584Wait(100);
	
	byErr = PCD8584In8(I2C_DATA_REG) & 0x1F;  // read back value of I2C clock, 5 bits 
	PCD8584Wait(100);

	if( byErr != (BYTE)(bySerialClock | I2C_CLOCK_6MHZ))
	{
		WK_TRACE(TRUE, "\tPCD8584 \tReadback test failed (0x%x)\n", (WORD)byErr);
		m_csI2C.Unlock();
		return FALSE;
	}
	else
		WK_TRACE(TRUE, "\tPCD8584 \tReadback test passed\n");
			
	PCD8584Out8(I2C_CONTROL_REG,I2C_PIN_BIT |  I2C_SERIAL_BUS_ON | I2C_ACK_BIT);	   /*Enable I2C	serial interface w/ byte acknowledge*/
	PCD8584Wait(100);

	m_csI2C.Unlock();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CI2C::PCD8584In8(BYTE byRegister )
{
	m_csI2C.Lock();

	BYTE byRet;

	//	WK_TRACE(TRUE, "CI2C::PCD8584In8\t0x%x", (WORD)byRegister);
	byRet = IoAccess.Input(m_wIOBase | PCF8584_OFFSET | byRegister);

	m_csI2C.Unlock();

	return byRet;

}
                         
/////////////////////////////////////////////////////////////////////////////
void CI2C::PCD8584Out8(BYTE byRegister, BYTE byData)
{
	m_csI2C.Lock();
//	WK_TRACE(TRUE, "CI2C::PCD8584Out8\t0x%x\t0x%x", (WORD)byRegister, (WORD)byData);
	IoAccess.Output(m_wIOBase | PCF8584_OFFSET | byRegister, byData);
	m_csI2C.Unlock();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CI2C::PCD8584WaitUntilPinFree() 		/* Test the PIN bit of the PCD8584 */
{
	m_csI2C.Lock();

	DWORD dwTC = GetTickCount();

	while((PCD8584In8(I2C_CONTROL_REG) & I2C_PIN_BIT) != 0)  /* Test if the PIN bit is free */
	{
		if ((GetTickCount() - dwTC)	>= 100)
		{
//			WK_TRACE(TRUE, "CI2C::PCD8584WaitUntilPinFree\tTimeOut (ControlReg=0x%x)\n", PCD8584In8(I2C_CONTROL_REG));
			m_csI2C.Unlock();
			return FALSE;
		}
		Sleep(0);
	}
	m_csI2C.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CI2C::PCD8584WaitUntilBusReady()   /* Test if PCD8584 has the bus busy */
{
	m_csI2C.Lock();

	DWORD dwTC = GetTickCount();

	while((PCD8584In8(I2C_CONTROL_REG) & I2C_BB_N) == 0) /* Test if the chip is free */
	{
		if ((GetTickCount() - dwTC)	>= 100)
		{
//			WK_TRACE(TRUE, "CI2C::PCD8584WaitUntilBusReady\tTimeOut (ControlReg=0x%x)\n", PCD8584In8(I2C_CONTROL_REG));
			m_csI2C.Unlock();
			return FALSE;
		}
		Sleep(0);
	}
	m_csI2C.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CI2C::PCD8584TestAck()
{
	m_csI2C.Lock();

	if (PCD8584In8(I2C_CONTROL_REG) & I2C_LRB_BIT)
	{
//		WK_TRACE(TRUE, "CI2C::PCD8584TestAck\tNO SLAVE ACK (ControlReg=0x%x)\n", PCD8584In8(I2C_CONTROL_REG));
		m_csI2C.Unlock();
		return FALSE;	
	}

	m_csI2C.Unlock();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// nCount = Zeit im Micro Sekunden
void CI2C::PCD8584Wait(int nCount)
{           
	m_csI2C.Lock();

	DWORD dwEnd = m_dwCyclePerMs * (DWORD)nCount / 1000; // pro 100 micro 
	for (DWORD dwI = 0; dwI < dwEnd; dwI++)
	{
		dwI += 1;	// Damit der Compiler die Schleife nicht wegoptimiert
		dwI -= 1;	// dito
	}

	m_csI2C.Unlock();
}

