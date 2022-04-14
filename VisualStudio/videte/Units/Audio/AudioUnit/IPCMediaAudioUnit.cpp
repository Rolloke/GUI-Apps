/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AudioUnit
// FILE:		$Workfile: IPCMediaAudioUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Audio/AudioUnit/IPCMediaAudioUnit.cpp $
// CHECKIN:		$Date: 3.12.04 15:00 $
// VERSION:		$Revision: 51 $
// LAST CHANGE:	$Modtime: 3.12.04 9:51 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioUnit.h"
#include "AudioUnitDlg.h"
#include "IPCMediaAudioUnit.h"
#include "CipcInputAudioUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CAudioUnitApp theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCMediaAudioUnit::CIPCMediaAudioUnit(LPCTSTR shmName, BOOL asMaster, CAudioUnitDlg*pDlg) : CIPCMedia(shmName, asMaster)
{
	m_pDlg = pDlg;
	m_wGroupID = SECID_NO_GROUPID;
	StartThread();
	m_bReset = false;
}
//////////////////////////////////////////////////////////////////////
CIPCMediaAudioUnit::~CIPCMediaAudioUnit()
{
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestReset(WORD wGroupID)
{
	WK_TRACE(_T("CIPCMediaAudioUnit::OnRequestReset\tGroupID=%x\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}
	
	m_bReset = true;
	m_pDlg->InitResetTimer();
}
//////////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::ConfirmReset()
{
	m_bReset = false;
	DoConfirmReset(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPCMediaAudioUnit
 Function   : OnRequestHardware
 Description: Requests the hardware capabilities of the Unit

 Parameters:   
  dwSecID:  (E): Contains the GroupID of the unit and the Clients SubID (CSecID)
  dwIDRequest: (E): Requested capability  (DWORD)
	- MEDIA_IDR_AUDIOCOMPRESSORS	// Compressors
	- MEDIA_IDR_AUDIOINPUTDEVICES	// Input Devices
	- MEDIA_IDR_AUDIO_RENDERERS	// Render Devices
	- MEDIA_IDR_INPUT_PINS			// Input Channels
	- MEDIA_IDR_TEST_CONFIG			//	Test Selected Compressor
	- 0									// selected Compressor, Input Dev., Render Dev.
	
 Result type:  (void)
 created: September, 10 2003
 <TITLE OnRequestHardware>
*********************************************************************************************/
void CIPCMediaAudioUnit::OnRequestHardware(CSecID secID, DWORD dwIDRequest)
{
	WORD wCIPC = USE_CIPCMEDIA;
	if (this == m_pDlg->m_pCIPCcontrol)
	{
		wCIPC = USE_CIPCCONTROL;
	}
	ASSERT(HIWORD(dwIDRequest) == 0);	// HIWORD ist reserviert !!
	m_pDlg->PostMessage(WM_REQUESTHARDWARE, secID.GetID(), MAKELONG(LOWORD(dwIDRequest), wCIPC));
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData)
{
	if (theApp.m_bTraceOnRequestStartMediaEncoding)
	{
		WK_TRACE(_T("OnRequestStartMediaEncoding(%x, %x, %x)\n"), mediaID.GetID(), dwFlags, dwUserData);
	}
	if (!(MEDIA_ENC_CLIENT_REQUEST & dwFlags))
	{
		m_dwUserData = dwUserData;
		m_EncodingID = mediaID;
	}
	m_pDlg->PostMessage(WM_REQUESTSTARTMEDIAENCODING, dwFlags);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags)
{
	if (theApp.m_bTraceOnRequestStopMediaEncoding)
	{
		WK_TRACE(_T("OnRequestStopMediaEncoding(%x, %x)\n"), mediaID.GetID(), dwFlags);
	}
	m_dwUserData = 0;
	m_EncodingID = mediaID;
	m_pDlg->PostMessage(WM_REQUESTSTOPMEDIAENCODING, dwFlags);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID, DWORD dwUserData)
{
//	ASSERT(mediaID == GetSecID());
	if (theApp.m_bTraceOnIndicateMediaData)
	{
		WK_TRACE(_T("OnIndicateMedia(%x, %x), No: %d, Size: %d\n"), mediaID.GetID(), dwUserData, rec.GetPackageNumber(), rec.GetActualLength());
	}
	CIPCMediaSampleRecord* pRec = (CIPCMediaSampleRecord*)&rec;
	pRec->InitFromBubble();
	TRACE(_T("Ind:"));
	m_pDlg->OnDecodeMediaData(rec, 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestSetGroupID(WORD wGroupID)
{
	WK_TRACE(_T("CIPCMediaAudioUnit::OnRequestSetGroupID(%x)\n"), wGroupID);
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestMediaDecoding(const CIPCMediaSampleRecord&rec, CSecID mediaID, DWORD dwFlags)
{
	m_ReplyID = mediaID;
	if (theApp.m_bTraceOnRequestMediaDecoding) 
	{
		WK_TRACE(_T("OnRequestMediaDecoding(%x, %x), No: %d, Size: %d\n"), mediaID.GetID(), dwFlags, rec.GetPackageNumber(), rec.GetActualLength());
	}
	TRACE(_T("Rqu:")); 
	m_pDlg->OnDecodeMediaData(rec, dwFlags);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	if (theApp.m_bTraceOnRequestStopMediaDecoding) 
	{
		WK_TRACE(_T("OnRequestStopMediaDecoding(%x, %x)\n"), mediaID.GetID(), dwFlags);
	}
	m_ReplyID = mediaID;
	m_pDlg->PostMessage(WM_REQUESTSTOPMEDIADECODING, dwFlags);
}
//////////////////////////////////////////////////////////////////////
CSecID CIPCMediaAudioUnit::GetSecID(WORD wSubID) const
{
	return CSecID(m_wGroupID, wSubID);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	if (theApp.m_bTraceOnRequestSetValue) 
	{
		WK_TRACE(_T("OnRequestSetValue(%x, %s, %s, %x)\n"), id.GetID(), sKey, sValue, dwServerData);
	}
	m_ReplyID = id;
	m_pDlg->OnRequestSetValue(id, sKey, sValue, dwServerData);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	if (theApp.m_bTraceOnRequestValues) 
	{
		WK_TRACE(_T("OnRequestValues(%x, %x, %x, %x, %d)\n"), mediaID.GetID(), dwCmd, dwID, dwValue, pData != NULL);
	}
	m_ReplyID = mediaID;
	m_pDlg->OnRequestValues(this, mediaID, dwCmd, dwID, dwValue, pData);
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCMediaAudioUnit::IsConnected()
{
	return (GetIPCState() == CIPC_STATE_CONNECTED);
}
//////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{

}
//////////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestDisconnect()
{
	m_pDlg->OnRequestDisconnect(this);
}
//////////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::DoIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID)
{
	if (theApp.m_bDoIndicateMediaData)
	{
		WK_TRACE(_T("DoIndicateMediaData(%x), %d, %d\n"), mediaID.GetID, rec.GetPackageNumber(), rec.GetActualLength());
	}
	CIPCMedia::DoIndicateMediaData(rec, mediaID, m_dwUserData);
}
//////////////////////////////////////////////////////////////////////////
void CIPCMediaAudioUnit::OnRequestGetValue(CSecID id, const CString &sKey,DWORD dwServerData)
{
	WK_TRACE(_T("OnRequestGetValue(%x, %s, %x)\n"), id.GetID(), sKey, dwServerData);
}
