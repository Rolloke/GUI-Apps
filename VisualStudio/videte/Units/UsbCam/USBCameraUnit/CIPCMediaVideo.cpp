/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CIPCMediaVideo.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CIPCMediaVideo.cpp $
// CHECKIN:		$Date: 23.02.05 8:42 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 21.02.05 15:09 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCameraUnit.h"
#include "USBCameraUnitDlg.h"
#include "CIPCMediaVideo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUSBCameraUnitApp theApp;
#ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCMediaVideo::CIPCMediaVideo(LPCTSTR shmName, BOOL asMaster, CUSBCameraUnitDlg*pDlg) : CIPCMedia(shmName, asMaster)
{
	m_pDlg = pDlg;
	m_SecID.SetGroupID(SECID_GROUP_MEDIA);
	m_SecID.SetSubID(0);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCMediaVideo::~CIPCMediaVideo()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestReset(WORD wGroupID)
{
	DoConfirmReset(wGroupID);
}

//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestHardware(CSecID secID, DWORD dwIDRequest)
{
	int nCompressors = m_pDlg->m_cEncoders.GetCount(),
		 nInputDevs   = m_pDlg->m_cCameras.GetCount(),
		 iError       = 0;
	
	DWORD dwFlags1 = 0;
	if (nInputDevs > 0) dwFlags1 |= MEDIA_CAN_VIDEO_INPUT;
	if (dwFlags1  == 0) iError    = CIPC_ERROR_NO_HARDWARE;

	CIPCExtraMemory *pEM = NULL;
	DoConfirmHardware(secID, iError, dwFlags1, dwIDRequest, pEM);
	WK_DELETE(pEM);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID, DWORD dwUserData)
{
//	ASSERT(mediaID== m_SecID);
	m_pDlg->PostMessage(WM_DECODE_MEDIA_DATA, (WPARAM)new CIPCMediaSampleRecord(rec), 0);
}
//////////////////////////////////////////////////////////////////////

void CIPCMediaVideo::OnRequestMediaDecoding(const CIPCMediaSampleRecord&media, CSecID mediaID, DWORD dwFlags)
{
	m_pDlg->PostMessage(WM_DECODE_MEDIA_DATA, (WPARAM)new CIPCMediaSampleRecord(media), dwFlags);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{

}
//////////////////////////////////////////////////////////////////////
CSecID CIPCMediaVideo::GetSecID() const
{
	return m_SecID;
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{

}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnRequestValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{

}
//////////////////////////////////////////////////////////////////////
BOOL CIPCMediaVideo::IsConnected()
{
	return (GetIPCState() == CIPC_STATE_CONNECTED);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaVideo::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{

}
#endif	
