/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaDA
// FILE:		$Workfile: Cpci.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDA/Cpci.cpp $
// CHECKIN:		$Date: 26.03.04 12:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.03.04 10:57 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define WK_DELETE(ptr)		{if(ptr){delete ptr; ptr=NULL;}}


//#pragma optimize( "", off )
 
///////////////////////////////////////////////////////////////////////////////////////
CPCIBus::CPCIBus(void)
{
	m_wHardmech			= 0;
	m_wInterfaceLevel	= 0;
	m_byLastNumber		= 1;
	m_bPciPresent		= FALSE;
	m_byBusNumber		= 0;
	m_pDeviceList		= new CDeviceList;

	CheckPCI();
}
		 
///////////////////////////////////////////////////////////////////////////////////////
CPCIBus::~CPCIBus(void)
{
	// Alle Einträge aus dem Listenobjekt in die Listbox kopieren
	for (int nI = 0; nI < m_pDeviceList->GetCount(); nI++)
	{
		CPCIDevice *pDevice = (CPCIDevice*)m_pDeviceList->GetAt(m_pDeviceList->FindIndex(nI));
		if (pDevice)
			WK_DELETE(pDevice);
	}

	if (m_pDeviceList)
		m_pDeviceList->RemoveAll();

	WK_DELETE(m_pDeviceList);
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CPCIBus::IsValid()	const
{
	return (m_pDeviceList && m_bPciPresent);
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CPCIBus::CheckPCI(void)
{
	m_wHardmech			= 0;
	m_wInterfaceLevel	= 0;
	m_byLastNumber		= 3; // Scanne 4 Busse
	m_bPciPresent		= TRUE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
CDeviceList* CPCIBus::ScanBus()
{
	BYTE	byBus			= 0;
	WORD	wDevAndFunction	= 0;

	CPCIDevice	*tempDevice		= NULL;
	CPCIDevice	*foundDevice	= NULL;

	if (!IsValid())
	{
		return NULL;
	}

	tempDevice = new CPCIDevice(0, 0);

	m_pDeviceList->RemoveAll();

	for (byBus = 0; byBus <= m_byLastNumber; byBus++)
	{
		for (wDevAndFunction = 0; wDevAndFunction <= 255; wDevAndFunction+=1)
		{
			tempDevice->m_byBusNumber			= byBus;
			tempDevice->m_byDeviceAndFunction	= (BYTE)wDevAndFunction;	
			if ((tempDevice->GetVendorID() == 0xffff) && (tempDevice->GetDeviceID() == 0xffff)) 
 				continue;

			foundDevice = new CPCIDevice(byBus, wDevAndFunction);
 			if (!m_pDeviceList->AddDevice(foundDevice))
				WK_DELETE(foundDevice);
		}
	}

	WK_DELETE(tempDevice);

	return m_pDeviceList;
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice::CPCIDevice(BYTE byBusNumber, BYTE byDeviceAndFunction) 
{
	m_byBusNumber			= byBusNumber;
	m_byDeviceAndFunction	= byDeviceAndFunction;
	m_pIoAddress			= new CIoPort(PCI_CONFIG_ADDRESS);
	m_pIoData				= new CIoPort(PCI_CONFIG_DATA);
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice::~CPCIDevice(void)
{
	WK_DELETE(m_pIoData);
	WK_DELETE(m_pIoAddress);
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CPCIDevice::GetBusDevAndFunction(BYTE  &byBusNumber, BYTE  &byDevAndFunc) const
{
	byBusNumber		= m_byBusNumber;
	byDevAndFunc	= m_byDeviceAndFunction;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
BYTE CPCIDevice::ReadByte(WORD wAt)	const
{
	DWORD dwAddr = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)wAt;
	m_pIoAddress->Outputdw(0, dwAddr);
	return (BYTE)(m_pIoData->Inputdw(0) >> (8* (wAt % sizeof(DWORD))));
}

///////////////////////////////////////////////////////////////////////////////////////
WORD CPCIDevice::ReadWord(WORD wAt)	const
{
	DWORD dwAddr = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)wAt;
	m_pIoAddress->Outputdw(0, dwAddr);
	return (WORD)(m_pIoData->Inputdw(0) >> (8* (wAt % sizeof(DWORD))));
}

///////////////////////////////////////////////////////////////////////////////////////
DWORD CPCIDevice::ReadDWord(WORD wAt) const
{
	DWORD dwAddr = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)wAt;
	m_pIoAddress->Outputdw(0, dwAddr);
	return m_pIoData->Inputdw(0);
}

///////////////////////////////////////////////////////////////////////////////////////
void CPCIDevice::WriteByte(WORD wAt, BYTE byData) const
{
	DWORD dwAddr = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)wAt;
	m_pIoAddress->Outputdw(0, dwAddr);
	m_pIoData->Outputdw(0, (DWORD)byData);
}

///////////////////////////////////////////////////////////////////////////////////////
void CPCIDevice::WriteWord(WORD wAt, WORD wData) const
{
	DWORD dwAddr = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)wAt;
	m_pIoAddress->Outputdw(0, dwAddr);
	m_pIoData->Outputdw(0, (DWORD)wData);
}

///////////////////////////////////////////////////////////////////////////////////////
void CPCIDevice::WriteDWord(WORD wAt, DWORD dwData) const
{
	DWORD dwAddr = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)wAt;
	m_pIoAddress->Outputdw(0, dwAddr);
	m_pIoData->Outputdw(0, dwData);
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CDeviceList::AddDevice(CPCIDevice* pDevice)
{
	if (!pDevice)
		return FALSE;

	// Nur Devices in die Liste aufnehmen, die noch nicht drin stehen.
    for (POSITION pos = GetHeadPosition(); pos != NULL; )
    {
		CPCIDevice *pTempDevice = (CPCIDevice*)GetNext(pos);

		// Wurde das Device schon in die Liste eingetragen?
		if ((pDevice->GetVendorID()			 == pTempDevice->GetVendorID())		&&
			(pDevice->GetDeviceID()			 == pTempDevice->GetDeviceID())		&&
			(pDevice->GetSubsystemID()		 == pTempDevice->GetSubsystemID())	&&
			(pDevice->GetSubsystemVendorID() == pTempDevice->GetSubsystemVendorID()))
			return FALSE;
	}

	AddTail(pDevice);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice*	CDeviceList::GetDevice(WORD wVendorID, WORD wDeviceID) const
{
	CPCIDevice* pDevice			= NULL;
	POSITION	pos				= NULL;
	BOOL		bFoundDevice	= FALSE;

    for(pos = GetHeadPosition(); pos != NULL;)
    {
		pDevice = (CPCIDevice*)GetNext(pos);
		if (pDevice)
		{
			if ((pDevice->GetVendorID() == wVendorID) &&
				(pDevice->GetDeviceID() == wDeviceID))
			{
				bFoundDevice = TRUE;
				break;
			}
		}
	}

	if (bFoundDevice)
		return pDevice;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice*	CDeviceList::GetDevice(WORD wVendorID, WORD wDeviceID, WORD wSubSystemID) const
{
	CPCIDevice* pDevice			= NULL;
	POSITION	pos				= NULL;
	BOOL		bFoundDevice	= FALSE;

    for(pos = GetHeadPosition(); pos != NULL;)
    {
		pDevice = (CPCIDevice*)GetNext(pos);
		if (pDevice)
		{
			if ((pDevice->GetVendorID()		== wVendorID) &&
				(pDevice->GetDeviceID()		== wDeviceID) &&
				(pDevice->GetSubsystemID()	== wSubSystemID))
			{
				bFoundDevice = TRUE;
				break;
			}
		}
	}

	if (bFoundDevice)
		return pDevice;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice*	CDeviceList::GetDevice(BYTE byBaseClass) const
{
	CPCIDevice* pDevice			= NULL;
	POSITION	pos				= NULL;
	BOOL		bFoundDevice	= FALSE;

    for(pos = GetHeadPosition(); pos != NULL;)
    {
		pDevice = (CPCIDevice*)GetNext(pos);
		if (pDevice)
		{
			if (pDevice->GetClass() == byBaseClass)
			{
				bFoundDevice = TRUE;
				break;
			}
		}
	}

	if (bFoundDevice)
		return pDevice;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
void CDeviceList::Trace() const
{
	CPCIDevice* pDevice			= NULL;
	POSITION	pos				= NULL;
	BYTE		byBusNumber		= 0;
	BYTE		byDevAndFunc	= 0;

	TRACE("List of PCI Devices:\n");
    for(pos = GetHeadPosition(); pos != NULL;)
    {
		pDevice = (CPCIDevice*)GetNext(pos);
		if (pDevice)
		{
			pDevice->GetBusDevAndFunction(byBusNumber, byDevAndFunc);
			TRACE("     VendorID=0x%04x DeviceID=0x%04x SubSystemID=0x%04x Bus=%u Function=%u\n",
				pDevice->GetVendorID(),
				pDevice->GetDeviceID(),
				pDevice->GetSubsystemID(),
				(WORD)byBusNumber, byDevAndFunc);
			
		}
	}

}
//#pragma optimize( "", on) 