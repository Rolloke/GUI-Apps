/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Ci2c.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/Ci2c.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CI2C.h"
#include "CBT878.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "..\\SavicDA\\SavicDirectAccess.h"

// Initialisieren der static member
CCriticalSection	CI2C::m_csI2C;

//////////////////////////////////////////////////////////////////////
CI2C::CI2C(DWORD dwIOBase, BYTE bySlaveAddress)
{
	m_bySlaveAddress	= bySlaveAddress;
	m_dwIOBase		= dwIOBase;
}

//////////////////////////////////////////////////////////////////////
CI2C::~CI2C()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CI2C::IsValid()	const
{
	return (m_dwIOBase != 0);
}

// ca. 115 Tackte. -> 1.15 ms pro Registerzugriff (Bei 100KHz)
//////////////////////////////////////////////////////////////////////
BOOL CI2C::WriteToI2C(BYTE bySubAddress, BYTE byVal)
{
	BOOL bResult = FALSE;

	m_csI2C.Lock();

	bResult = DAWriteToI2C(m_dwIOBase, m_bySlaveAddress, bySubAddress, byVal);

	m_csI2C.Unlock();

	if (!bResult)
		ML_TRACE_ERROR(_T("Can't write over I2C-Bus (Slave=0x%02x, Reg=0x%02x, Val=0x%02x)\n"),
						m_bySlaveAddress, bySubAddress, byVal);

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CI2C::ReadFromI2C(BYTE bySubAddress, BYTE& byVal)
{
	BOOL bResult = FALSE;
	m_csI2C.Lock();

	bResult =  DAReadFromI2C(m_dwIOBase, m_bySlaveAddress, bySubAddress, byVal);
	
	m_csI2C.Unlock();

	if (!bResult)
		ML_TRACE_ERROR(_T("Can't read over I2C-Bus (Slave=0x%02x, Reg=0x%02x)\n"), m_bySlaveAddress, bySubAddress);
	
	return bResult;
}

