#include "stdafx.h"
#include <math.h>
#include ".\ecbase.h"
#include "cioport.h"

/////////////////////////////////////////////////////////////////////////////
CECBase::CECBase(DWORD dwBaseAddress)
{
	m_bValidObject	= FALSE;
	m_wChipID		= 0;
	m_wVendorID		= 0;
	m_wDeviceID		= 0;
	m_wChipVersion	= 0;
	m_nMaxFans		= 0;
	m_nMaxTemps		= 0;

	m_bVCoreA		= FALSE;
	m_bVCoreB		= FALSE;
	m_b3_3V			= FALSE;
	m_b5V			= FALSE;
	m_b12V			= FALSE;
	m_bM12V			= FALSE;
	m_bM5V			= FALSE;

	m_sIdentifier	= "";
	m_pIo = new CIoPort(dwBaseAddress);
}

/////////////////////////////////////////////////////////////////////////////
CECBase::~CECBase(void)
{
	WK_DELETE(m_pIo);
}