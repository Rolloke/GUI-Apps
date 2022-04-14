#include "stdafx.h"
#include "MiCoReg.h"
#include "MiCoDefs.h"
#include "CRelay.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CRelay::CRelay(WORD wIOBase, WORD wExtCard)
{
	m_wIOBase		= wIOBase;			// Basisadresse des Boards
	m_wExtCard		= wExtCard;			// Nummer des Boards (0=MiCo, 1...3=CoVi1...CoVi3)			
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRelay::SetRelay(BYTE byRelais)
{
	// MiCo	oder CoVi?
	if (m_wExtCard == MICO_EXTCARD0)
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
		byReg &= 0xf0;
		byReg |= (byRelais & 0x0f);
		m_IoAccess.Output(m_wIOBase + RELAY_OFFSET, byReg);
	}
	else
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_COVI_OFFSET);
		byReg &= 0xf0;
		byReg |= (byRelais & 0x0f);
		m_IoAccess.Output(m_wIOBase + RELAY_COVI_OFFSET, byReg);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CRelay::GetRelay()
{
	// MiCo oder CoVi ?
	if (m_wExtCard == MICO_EXTCARD0)
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
		return (byReg & 0x0f);
	}
	else
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_COVI_OFFSET);
		return (byReg & 0x0f);
	}
}
