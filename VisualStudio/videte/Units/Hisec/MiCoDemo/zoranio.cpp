#include "stdafx.h"
#include "conio.h"
#include "CIo.h"
#include "zoranio.h"
#include "MiCoreg.h"

//////////////////////////////////////////////////////////////////////////
ZoranIo::ZoranIo(WORD wIOBase)
{
	m_wIOBase = wIOBase;
}

//////////////////////////////////////////////////////////////////////////
BYTE ZoranIo::Read050(WORD wAddress)
{
	_asm cli
	m_IO.Outputw(m_wIOBase + ZR_ADDRESS_OFFSET, wAddress);
	BYTE byVal = (BYTE)m_IO.Input(m_wIOBase + ZR_DATA_OFFSET);
	_asm sti

	return byVal;
}

//////////////////////////////////////////////////////////////////////////
void ZoranIo::Write050(WORD wAdr, BYTE byData)
{
	_asm cli
	m_IO.Outputw(m_wIOBase + ZR_ADDRESS_OFFSET, wAdr);
	m_IO.Output(m_wIOBase  + ZR_DATA_OFFSET, byData);
	_asm sti
}

//////////////////////////////////////////////////////////////////////////
BOOL ZoranIo::Check050End()
{
	return (Read050(0x002f) & 0x04);
}
