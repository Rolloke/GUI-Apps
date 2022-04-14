// MCIDevice.cpp: implementation of the CMCIDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mci.h"
#include "MCIDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMCIDevice::CMCIDevice()
{
	m_DeviceID=0;
	m_pMainWnd = NULL;
	m_bDeviceClosed=TRUE;

}

CMCIDevice::~CMCIDevice()
{
	if(!m_bDeviceClosed)
		CloseCdromDevice();
}

MCIDEVICEID CMCIDevice::GetID()
{

  return m_DeviceID; 


}
//////////////////////////////////////////////////////////////////////////

//Driveletter in der Form:     G:



BOOL CMCIDevice::OpenCdromDevice(CString sDriveletter)
{
	if(!m_bDeviceClosed)
		return FALSE;

	m_DeviceID=0;
	MCI_OPEN_PARMS mciOpenParms;
	DWORD dwReturn;
	m_sDriveletter=sDriveletter;
	   
	mciOpenParms.lpstrDeviceType = "cdaudio";
	mciOpenParms.lpstrElementName = sDriveletter;

	dwReturn=mciSendCommand(NULL,MCI_OPEN, MCI_WAIT|MCI_OPEN_TYPE | MCI_OPEN_ELEMENT|MCI_WAIT, 
								  (DWORD)(LPVOID) &mciOpenParms);

	m_DeviceID=mciOpenParms.wDeviceID;	

	if(!dwReturn)
	{
		m_bDeviceClosed=FALSE;
	}
    
	return FALSE;
}

void CMCIDevice::SetMainWnd(CWnd *pWnd)
{
	m_pMainWnd = pWnd;
}

BOOL CMCIDevice::OpenCDCaddy()
{
	MCI_SET_PARMS mciSetParms;

//	mciSetParms.dwCallback = (DWORD)m_pMainWnd->m_hWnd;

	return !(mciSendCommand(m_DeviceID,MCI_SET, MCI_SET_DOOR_OPEN|MCI_WAIT, (DWORD)&mciSetParms));
}

BOOL CMCIDevice::CloseCDCaddy()
{
	MCI_SET_PARMS mciSetParms;

	mciSetParms.dwCallback = (DWORD)m_pMainWnd->m_hWnd;
	
	return !(mciSendCommand(m_DeviceID,MCI_SET, MCI_SET_DOOR_CLOSED|MCI_WAIT, (DWORD) &mciSetParms));
}

DWORD CMCIDevice::CloseCdromDevice()
{
	DWORD dwReturn;
	MCI_GENERIC_PARMS  mciCloseParms;
	mciCloseParms.dwCallback=(DWORD)m_pMainWnd->m_hWnd;

		dwReturn=mciSendCommand(m_DeviceID, MCI_CLOSE, MCI_WAIT, 
								  (DWORD)(LPVOID) &mciCloseParms);
		if(!dwReturn)
		{
			m_bDeviceClosed=TRUE;
			TRACE("CDROM DEVICE CLOSED\n");
		}

	return dwReturn;
}

BOOL CMCIDevice::MediaInserted()
{

	DWORD dwReturn;
	MCI_STATUS_PARMS   mciStatusParms;
	mciStatusParms.dwItem=(DWORD)MCI_STATUS_MEDIA_PRESENT;
	mciStatusParms.dwCallback=(DWORD)m_pMainWnd->m_hWnd;

	dwReturn=mciSendCommand(m_DeviceID,MCI_STATUS,MCI_WAIT|MCI_STATUS_ITEM, 
								  (DWORD)(LPMCI_STATUS_PARMS) &mciStatusParms);
	
	return mciStatusParms.dwReturn;
}

BOOL CMCIDevice::DeviceIsReady()
{
	DWORD dwReturn;
	MCI_STATUS_PARMS   mciStatusParms;
	mciStatusParms.dwItem=(DWORD)MCI_STATUS_MODE;
	mciStatusParms.dwCallback=(DWORD)m_pMainWnd->m_hWnd;

	dwReturn=mciSendCommand(m_DeviceID,MCI_STATUS, MCI_WAIT|MCI_STATUS_ITEM , 
								  (DWORD)(LPMCI_STATUS_PARMS) &mciStatusParms);
	
	if(mciStatusParms.dwReturn==MCI_MODE_NOT_READY)
		return FALSE;
	if(mciStatusParms.dwReturn==MCI_MODE_OPEN)
		return FALSE;

	return TRUE;
}

void CMCIDevice::iocontrol()
{


 



}
