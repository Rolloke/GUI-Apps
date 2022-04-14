/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Cpci.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/Cpci.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CPci.h"
#include "CIoPort.h"
#include "..\\SavicDA\\SavicDirectAccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////
CPCIBus::CPCIBus(void)
{
	m_wHardmech			= 0;
	m_wInterfaceLevel	= 0;
	m_byLastNumber		= 1;
	m_bPciPresent		= FALSE;
	m_byBusNumber		= 0;
	m_pDeviceList		= new CPCIDeviceList;

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
CPCIDeviceList* CPCIBus::ScanBus()
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
BYTE CPCIDevice::ReadByte(WORD wOffset)	const
{
	DWORD dwIndex = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)(wOffset & 0xFC);
	m_pIoAddress->Outputdw(0, dwIndex);
	return m_pIoData->Input(wOffset & 0x03);
}

///////////////////////////////////////////////////////////////////////////////////////
WORD CPCIDevice::ReadWord(WORD wOffset)	const
{
	DWORD dwIndex = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)(wOffset & 0xFC);
	m_pIoAddress->Outputdw(0, dwIndex);
	return m_pIoData->Inputw(wOffset & 0x03);
}

///////////////////////////////////////////////////////////////////////////////////////
DWORD CPCIDevice::ReadDWord(WORD wOffset) const
{
	DWORD dwIndex = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)(wOffset & 0xFC);
	m_pIoAddress->Outputdw(0, dwIndex);
	return m_pIoData->Inputdw(wOffset & 0x03);
}

///////////////////////////////////////////////////////////////////////////////////////
void CPCIDevice::WriteByte(WORD wOffset, BYTE byData) const
{
	DWORD dwIndex = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)(wOffset & 0xFC);
	m_pIoAddress->Outputdw(0, dwIndex);
	m_pIoData->Output(wOffset & 0x03, byData);
}				 

///////////////////////////////////////////////////////////////////////////////////////
void CPCIDevice::WriteWord(WORD wOffset, WORD wData) const
{
	DWORD dwIndex = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)(wOffset & 0xFC);
	m_pIoAddress->Outputdw(0, dwIndex);
	m_pIoData->Outputw(wOffset & 0x03, wData);
}

///////////////////////////////////////////////////////////////////////////////////////
void CPCIDevice::WriteDWord(WORD wOffset, DWORD dwData) const
{
	DWORD dwIndex = 0x80000000L | (DWORD)m_byBusNumber << 16 | (DWORD)m_byDeviceAndFunction << 8 | (DWORD)(wOffset & 0xFC);
	m_pIoAddress->Outputdw(0, dwIndex);
	m_pIoData->Outputdw(wOffset & 0x03, dwData);
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CPCIDeviceList::AddDevice(CPCIDevice* pDevice)
{
	if (!pDevice)
		return FALSE;

	// Nur Devices in die Liste aufnehmen, die noch nicht drin stehen.
    for (POSITION pos = GetHeadPosition(); pos != NULL; )
    {
		CPCIDevice *pTempDevice = (CPCIDevice*)GetNext(pos);
		BYTE byBus1 = 0, byBus2 = 0, byDevFunc1 = 0, byDevFunc2 = 0;

		// Die DeviceNumber holen
		pDevice->GetBusDevAndFunction(byBus1, byDevFunc1);
		pTempDevice->GetBusDevAndFunction(byBus2, byDevFunc2);

		// Wurde das Device schon in die Liste eingetragen?
		if ((pDevice->GetVendorID()			 == pTempDevice->GetVendorID())		&&
			(pDevice->GetDeviceID()			 == pTempDevice->GetDeviceID())		&&
			(pDevice->GetSubsystemID()		 == pTempDevice->GetSubsystemID())	&&
			(pDevice->GetSubsystemVendorID() == pTempDevice->GetSubsystemVendorID()) &&
			((byDevFunc1 & 0xf8)			 == (byDevFunc2 & 0xf8)) &&
			((byBus1						 == byBus2)))
			return FALSE;
	}

	AddTail(pDevice);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice*	CPCIDeviceList::GetDevice(WORD wVendorID, WORD wDeviceID) const
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
CPCIDevice*	CPCIDeviceList::GetDevice(WORD wVendorID, WORD wDeviceID, WORD wSubSystemID) const
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
CPCIDevice*	CPCIDeviceList::GetDeviceByIndex(WORD wVendorID, WORD wDeviceID, int nIndex) const
{
	CPCIDevice* pDevice			= NULL;
	POSITION	pos				= NULL;
	BOOL		bFoundDevice	= FALSE;
	int			nCnt			= 0;

    for(pos = GetHeadPosition(); pos != NULL;)
    {
		pDevice = (CPCIDevice*)GetNext(pos);
		if (pDevice)
		{
			if ((pDevice->GetVendorID() == wVendorID) &&
				(pDevice->GetDeviceID() == wDeviceID))
			{						   
				nCnt++;
				if (nCnt == nIndex)
				{
					bFoundDevice = TRUE;
					break;
				}
			}
		}
	}

	if (bFoundDevice)
		return pDevice;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
CPCIDevice*	CPCIDeviceList::GetDevice(BYTE byBaseClass) const
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
void CPCIDeviceList::Trace() const
{
	CPCIDevice* pDevice			= NULL;
	POSITION	pos				= NULL;
	BYTE		byBusNumber		= 0;
	BYTE		byDevAndFunc	= 0;

	ML_TRACE(_T("List of PCI Devices:\n"));
    for(pos = GetHeadPosition(); pos != NULL;)
    {
		pDevice = (CPCIDevice*)GetNext(pos);
		if (pDevice)
		{
			pDevice->GetBusDevAndFunction(byBusNumber, byDevAndFunc);
			int nIRQ = pDevice->GetIRQLine();
			if (nIRQ >= 1 && nIRQ <= 15)
			{
				ML_TRACE(_T("     VendorID=0x%04x DeviceID=0x%04x SubSystemID=0x%04x SubSystemVendorID=0x%04x Bus=%u Dev=%u Fnc=%u IRQ=%d\n"),
					pDevice->GetVendorID(),
					pDevice->GetDeviceID(),
					pDevice->GetSubsystemID(),
					pDevice->GetSubsystemVendorID(),
					(WORD)byBusNumber,
					(WORD)(byDevAndFunc>>3),
					(WORD)(byDevAndFunc & 0x07),
					pDevice->GetIRQLine());		
			}
			else
			{
				ML_TRACE(_T("     VendorID=0x%04x DeviceID=0x%04x SubSystemID=0x%04x SubSystemVendorID=0x%04x Bus=%u Dev=%u Fnc=%u IRQ=No\n"),
					pDevice->GetVendorID(),
					pDevice->GetDeviceID(),
					pDevice->GetSubsystemID(),
					pDevice->GetSubsystemVendorID(),
					(WORD)byBusNumber,
					(WORD)(byDevAndFunc>>3),
					(WORD)(byDevAndFunc & 0x07));
			}
		}
	}

}
