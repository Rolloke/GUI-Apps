/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: zoranio.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/zoranio.cpp $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "conio.h"
#include "zoranio.h"

//////////////////////////////////////////////////////////////////////////
ZoranIo::ZoranIo(WORD wIOBase)
{
	m_wIOBase = wIOBase;
}

//////////////////////////////////////////////////////////////////////////
BYTE ZoranIo::Read050(WORD wAddress)
{
	_asm cli
	_outpw(m_wIOBase + 0x0002, wAddress);
    BYTE byVal = (BYTE)_inp(m_wIOBase + 0x0004);
	_asm sti

	return byVal;
}

//////////////////////////////////////////////////////////////////////////
void ZoranIo::Write050(WORD wAdr, BYTE byData)
{
	_asm cli
	_outpw(m_wIOBase + 0x0002, wAdr);
	_outp(m_wIOBase + 0x0004, byData);
	_asm sti
}

//////////////////////////////////////////////////////////////////////////
BOOL ZoranIo::Check050End()
{
	return (Read050(0x002f) & 0x04);
}
