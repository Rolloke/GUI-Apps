// CIPCAudioIdipClient.cpp: implementation of the CIPCAudioIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCAudioIdipClient.h"

#include "MainFrm.h"
#include "IdipClient.h"

#include "IdipClientDoc.h"
#include "ViewAudio.h"
#include "ViewIdipClient.h"
#include "Server.h"
#include "WndPlay.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CIPCAudioIdipClient, CObject)

CIPCAudioIdipClient::CIPCAudioIdipClient(LPCTSTR shmName, CServer*pServer, CSecID id, WORD wCodePage)
	: CIPCMediaClient(shmName, FALSE)
{
	InitClassMembers();
	ASSERT(pServer != NULL);
	m_wServerID = pServer->GetID();
	m_pDocument = pServer->GetDocument();
	ASSERT(m_pDocument != NULL);
	m_SecID   = id;
	m_wCodePage = wCodePage;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////
CIPCAudioIdipClient::CIPCAudioIdipClient(LPCTSTR shmName, CIdipClientDoc*pDoc, BOOL bAsMaster/* = FALSE*/)
	: CIPCMediaClient(shmName, bAsMaster)
{
	InitClassMembers();
	m_pDocument = pDoc;
	ASSERT(m_pDocument != NULL);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::InitClassMembers()
{
	m_nOutputVolume     = -1;
	m_nInputChannel     = -1;
	m_nStereoChannel    = -1;
	m_nInputSensitivity = -1;
	m_nThreshold        = -1;
	m_nSentSamples      = 0;

	m_psaInputChannels  = NULL;
	m_pWndNotify        = NULL;
	m_wServerID         = SECID_NO_SUBID;
	m_pDocument         = NULL;
	
	m_pDestinations = new CIPCAudioIdipClientListCS();
	m_pOwnerOfMe = NULL;

	m_pWndPlayActive = NULL;	// Recherche
}

//////////////////////////////////////////////////////////////////////
CIPCAudioIdipClient::~CIPCAudioIdipClient()
{
	if (m_pWndNotify)
	{
		m_pWndNotify->SendMessage(m_wNotifyMessage, MEDIA_DISCONNECT, 0);
	}
	WK_DELETE(m_psaInputChannels);

	if (m_pOwnerOfMe)
	{
		m_pOwnerOfMe->RemoveDestination(this);
	}
	RemoveDestinations();
	WK_DELETE(m_pDestinations);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM)
{
	CAutoCritSec cs(&m_csAudio);
	if (dwRequestID == MEDIA_IDR_INPUT_PINS)
	{
		WK_DELETE(m_psaInputChannels);
		if (pEM)
		{
			m_psaInputChannels = pEM->GetStrings();
			m_saNamedInputs.RemoveAll();
			m_dwaNamedInputs.RemoveAll();
			int i, nSize = m_psaInputChannels->GetSize();
			for (i=0; i<nSize; i++)
			{
				DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_INPUT_PIN_NAME, MEDIA_IDR_INPUT_PINS, MAKELONG(i, 0xffff));
			}
		}
	}
	else if (dwRequestID == 0)
	{
		if (GetIsMaster())
		{
			CIPCMediaRecord::MediaRecordFlags Flags;
			CIPCMediaRecord* pRec;
			int numRecords = 0;
			CIPCMediaRecordArray array;
			Flags.Set(dwFlags);
			if (Flags.IsOutput)					// is an audio output there
			{
				pRec = new CIPCMediaRecord();
				Flags.IsInput = 0;
				Flags.IsEnabled = true;
				Flags.IsDefault = true;
				Flags.IsOkay    = true;
				pRec->Set(_T("LocalOutput"), CSecID(SECID_GROUP_MEDIA, (WORD)numRecords), Flags);
				numRecords++;
				array.Add(pRec);
			}
			Flags.Set(dwFlags);
			if (Flags.IsInput)					// is an audio input there
			{
				pRec = new CIPCMediaRecord();
				Flags.IsOutput = 0;
				Flags.IsEnabled = true;
				Flags.IsDefault = true;
				Flags.IsOkay    = true;
				pRec->Set(_T("LocalOutput"), CSecID(SECID_GROUP_MEDIA, (WORD)numRecords), Flags);
				numRecords++;
				array.Add(pRec);
			}
			OnConfirmInfoMedia(0, 0, numRecords, array);
			array.DeleteAll();
		}
		else
		{
			UpdateFlags(secID, dwFlags);
			if (!theApp.IsResetting())	// do not request while resetting,
			{							// this will cause only an error message
				DoRequestInfoMedia(SECID_NO_GROUPID);
			}
			// OnConfirmInfoMedia is sent after Reset anyway
		}
		if (!theApp.IsResetting())
		{
			DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_INPUT_PIN_NAME, MEDIA_IDR_INPUT_PINS, 0xffffffff);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnConfirmConnection()
{
	if (m_wCodePage != CODEPAGE_UNICODE)			// request only, if unicode is not used
	{
		WORD wCodePage = m_wCodePage;
		m_wCodePage = CODEPAGE_UNICODE;				// request is made in unicode
		DoRequestGetValue(CSecID(), CRF_CODEPAGE);	// Do you speak Unicode?
		m_wCodePage = wCodePage;
	}
	if (GetIsMaster())
	{
		DoRequestSetGroupID(SECID_GROUP_MEDIA);
	}
	else
	{
		CIPCMediaClient::OnConfirmConnection();
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnRequestDisconnect()
{
	if (m_pWndNotify)
	{
		m_pWndNotify->PostMessage(m_wNotifyMessage, MEDIA_DISCONNECT, 0);
		m_pWndNotify = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnConfirmSetGroupID(WORD wGroupID)
{
	DoRequestHardware(CSecID(wGroupID, 0), 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::RequestConfig(CSecID secID)
{
//	WORD wSubID = secID.GetSubID();
	CIPCMediaRecord*pRec;
	pRec = GetMediaRecordPtrFromSecID(secID);
	if (pRec)
	{
		if (pRec->IsCaptureInitialized())
		{
			DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS);
			DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_THRESHOLD      , MEDIA_IDR_INPUT_PINS);
			DoRequestValues(secID, CB_GETCURSEL                         , MEDIA_IDR_INPUT_PINS);
			DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_STEREO_CHANNEL , MEDIA_IDR_INPUT_PINS);
			DoRequestHardware(secID, MEDIA_IDR_INPUT_PINS);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	CAutoCritSec cs(&m_csAudio);
	BOOL bError = (dwCmd & MEDIA_ERROR_OCCURRED) ? TRUE : FALSE;

	if (bError && MEDIA_COMMAND_VALUE(dwCmd) != MEDIA_INPUT_PIN_NAME)
	{
		dwValue = (DWORD)(-1L);
	}
	if (dwCmd & (MEDIA_SET_VALUE|MEDIA_GET_VALUE)) // Request and confirm
	{
		if (MEDIA_COMMAND_VALUE(dwCmd) == CIPC_MEDIA_REQUEST_DECODING)
		{
			m_nSentSamples--;
			if (!DoNotSendSamples() && m_pWndPlayActive)
			{
				m_pWndPlayActive->PostMessage(WM_USER, DW_CHECK_PLAY_STATUS);
			}
		}
		else if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_STATE)
		{
			UpdateFlags(mediaID, dwValue);
		}
		else if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			switch (MEDIA_COMMAND_VALUE(dwCmd))
			{
				case MEDIA_RECORDING_LEVEL:
					m_nInputSensitivity = dwValue; 
					break;
				case MEDIA_THRESHOLD:     
					m_nThreshold        = dwValue; 
					break;
				case MEDIA_STEREO_CHANNEL:  
					m_nStereoChannel    = dwValue; 
					break;
				case MEDIA_INPUT_PIN_NAME:
				if (!bError)
				{
					m_saNamedInputs.Add(pData->GetString());
					m_dwaNamedInputs.Add(dwValue);
				}
				if (m_psaInputChannels)
				{
					WK_TRACE(_T("MEDIA_INPUT_PIN_NAME:%x\n"), dwValue);
					
					if (LOWORD(dwValue) == (WORD)(m_psaInputChannels->GetSize()-1))
					{
						dwID = 0;
					}
				}
				break;
			}
		}
		else if (dwID == MEDIA_IDR_AUDIO_RENDERERS)
		{
			switch (MEDIA_COMMAND_VALUE(dwCmd))
			{
				case MEDIA_VOLUME:
					m_nOutputVolume = dwValue;
					break;
			}
		}
	}
	else if (dwCmd == CB_SELECTSTRING)	// Confirm Selection
	{
		if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			m_nInputChannel = dwValue;
		}
	}
	else if (dwCmd == CB_GETCURSEL)		// Confirm Request
	{
		if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			m_nInputChannel = dwValue;
		}
	}
	if (m_pWndNotify)
	{
		m_pWndNotify->PostMessage(m_wNotifyMessage, dwCmd, dwID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::SetOutputVolume(CSecID secID, int nValue)
{
	ASSERT(IsBetween(nValue, 0, 1000));
	DWORD dwFlagAndCMD = MEDIA_SET_VALUE|MEDIA_VOLUME;
#ifdef _DTS
	 dwFlagAndCMD |= MEDIA_SAVE_IN_REGISTRY;
#endif
	DoRequestValues(secID, dwFlagAndCMD, MEDIA_IDR_AUDIO_RENDERERS, nValue);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::SetInputSensitivity(CSecID secID, int nValue)
{
	ASSERT(IsBetween(nValue, -1, 1000)); // -1 : Automatic Gain
	DWORD dwFlagAndCMD = MEDIA_SET_VALUE|MEDIA_RECORDING_LEVEL;
	DoRequestValues(secID, dwFlagAndCMD, MEDIA_IDR_INPUT_PINS, nValue);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::SetInputChannel(CSecID secID, int nChannel)
{
	if (m_psaInputChannels)
	{
		ASSERT(IsBetween(nChannel, 0, m_psaInputChannels->GetSize()-1));
	}
	DoRequestValues(secID, CB_SELECTSTRING, MEDIA_IDR_INPUT_PINS, nChannel, NULL);
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::SetStereoChannel(CSecID secID, int nChannel)
{
	DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_STEREO_CHANNEL, MEDIA_IDR_INPUT_PINS, nChannel, NULL);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::SetThreshold(CSecID secID, int nValue)
{
	ASSERT(IsBetween(nValue, 0, 100));
	DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_THRESHOLD, MEDIA_IDR_INPUT_PINS, nValue);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::StartRecord(CSecID secID, bool bLocal, DWORD dwUserData)
{
	DWORD dwFlags = MEDIA_ENC_NEW_SEGMENT|MEDIA_ENC_CLIENT_REQUEST|MEDIA_ENC_DELIVER_ALWAYS;

	m_RecordSecID = secID;
	DoRequestStartMediaEncoding(secID, dwFlags, dwUserData);
	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(secID);
	if (pRec)
	{
		pRec->SetCapturing(true);
		OnUpdateRecord(*pRec);
	}
	
	if (bLocal)
	{
		pRec = GetMediaRecordPtrFromSecID(GetDefaultOutputID());
		if (pRec)
		{
			if (!pRec->IsRendering())
			{
				DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_START_LOCAL_REPLAY, MEDIA_IDR_INPUT_PINS, TRUE);
				pRec->SetRendering(true);
				OnUpdateRecord(*pRec);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::StopRecord(CSecID secID)
{
	if (secID == SECID_NO_ID)
	{
		secID = m_RecordSecID;
	}
	DoRequestStopMediaEncoding(secID, 0);								// stop recording
	
	CIPCAudioIdipClient *pDest;
	m_pDestinations->Lock(_T(__FUNCTION__));
	while (m_pDestinations->GetSize())									// if there are Destinations
	{
		pDest = m_pDestinations->GetAtFast(0);
		m_pDestinations->Remove(pDest);									// remove them
		pDest->DoRequestStopMediaDecoding(pDest->GetSecID());		// stop them
		ASSERT(pDest->m_pOwnerOfMe == this);
		pDest->m_pOwnerOfMe = NULL;										// remove owner
	}
	m_pDestinations->Unlock();
	m_RecordSecID = SECID_NO_ID;

	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(secID);
	if (pRec)
	{
		pRec->SetCapturing(false);
		OnUpdateRecord(*pRec);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::StopPlay(CSecID secID)
{
	if (m_pOwnerOfMe)															// if I am owned
	{
		m_pOwnerOfMe->RemoveDestination(this);							// remove me
	}

	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(secID);
	if (pRec)
	{
		pRec->SetRendering(false);
		OnUpdateRecord(*pRec);
	}

	DoRequestStopMediaDecoding(secID);
}
//////////////////////////////////////////////////////////////////////
CString CIPCAudioIdipClient::GetNamedInput(int nIndex, DWORD &dwChannel)  const
{
	int nSize = m_dwaNamedInputs.GetSize();
	if (IsBetween(nIndex, 0, nSize-1))
	{
		dwChannel =  m_dwaNamedInputs.GetAt(nIndex);
		return m_saNamedInputs.GetAt(nIndex);
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnConfirmInfoMedia(WORD wGroupID, int iNumGroups, int iNumRecords, const CIPCMediaRecordArray& records)
{
	theApp.EndResetting();
	CAutoCritSec cs(&m_csAudio);
	CIPCMediaClient::OnConfirmInfoMedia(wGroupID, iNumGroups, iNumRecords, records);
	m_idDefaultInput  = SECID_NO_ID;
	m_idDefaultOutput = SECID_NO_ID;
	int nFirstOut = -1, nFirstIn = -1, i, nCount = GetNumberOfMedia();
	for (i=0; i<nCount; i++)
	{
		const CIPCMediaRecord &mr = GetMediaRecordFromIndex(i);
		if (mr.IsOutput())
		{
			if (nFirstOut == -1)
			{
				nFirstOut = i;
			}
			if (mr.IsDefault())
			{
				m_idDefaultOutput = mr.GetID();
			}
		}
		else if (mr.IsInput())
		{
			if (nFirstIn == -1)
			{
				nFirstIn = i;
			}
			if (mr.IsDefault())
			{
				m_idDefaultInput = mr.GetID();
			}
		}
	}

	if (m_idDefaultOutput == SECID_NO_ID && nFirstOut != -1)
	{
		m_idDefaultOutput = GetMediaRecordFromIndex(nFirstOut).GetID();
	}

	if (m_idDefaultInput == SECID_NO_ID && nFirstIn != -1)
	{
		m_idDefaultInput = GetMediaRecordFromIndex(nFirstIn).GetID();
	}

	if (m_wServerID != SECID_NO_SUBID)
	{
		CView* pOV = (CView*)theApp.GetMainFrame()->GetViewAudio();
		if (pOV)
		{	// CViewAudio::InsertAudios() !
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_AUDIO_CONNECTED, m_wServerID));
		}
	}

//	DoRequestValues(m_idDefaultInput, MEDIA_GET_VALUE|MEDIA_INPUT_PIN_NAME, MEDIA_IDR_INPUT_PINS, 0xffffffff);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnAddRecord(const CIPCMediaRecord& record)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnUpdateRecord(const CIPCMediaRecord& record)
{
	CViewAudio* pOV = theApp.GetMainFrame()->GetViewAudio();
	if (pOV)
	{	// CViewAudio::ActualizeAudio() ! Update TreeItem
		pOV->PostMessage(WM_USER, MAKELONG(WPARAM_ACTUALIZE, m_wServerID), record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnDeleteRecord(const CIPCMediaRecord& record)
{
	if (m_wServerID != SECID_NO_SUBID)
	{
		CViewAudio * pOV = theApp.GetMainFrame()->GetViewAudio();
		if (pOV)
		{
			// Delete Update TreeItem
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_DELETE, m_wServerID), record.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID, DWORD dwUserData)
{
	m_pDestinations->Lock(_T(__FUNCTION__));
	int i, nCount = m_pDestinations->GetSize();
	for (i=0; i<nCount; i++)
	{
		m_pDestinations->GetAtFast(i)->DoRequestMediaDecoding(rec);
	}
	m_pDestinations->Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::AddDestination(CIPCAudioIdipClient *pDest, CSecID secID)
{
	if (pDest->m_pOwnerOfMe == NULL)	// is it in any List?
	{
		pDest->m_pOwnerOfMe = this;
		pDest->m_SecID      = secID;
		m_pDestinations->Lock(_T(__FUNCTION__));
		m_pDestinations->Add(pDest);
		m_pDestinations->Unlock();
		CIPCMediaRecord*pRec = pDest->GetMediaRecordPtrFromSecID(secID);
		if (pRec)
		{
			pRec->SetRendering(true);
			pDest->OnUpdateRecord(*pRec);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
const CIPCAudioIdipClient* CIPCAudioIdipClient::GetDestination(int nDest) const
{
	if (IsInArray(nDest, m_pDestinations->GetSize()))
	{
		return m_pDestinations->GetAtFast(nDest);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::RemoveDestinations()
{
	m_pDestinations->Lock(_T(__FUNCTION__));
	int i, nCount = m_pDestinations->GetSize();
	CIPCAudioIdipClient *pDest;
	for (i=0; i<nCount; i++)
	{
		pDest = m_pDestinations->GetAtFast(i);
		pDest->m_pOwnerOfMe = NULL;
		pDest->m_SecID = SECID_NO_ID;
	}
	m_pDestinations->RemoveAll();
	m_pDestinations->Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::RemoveDestination(CIPCAudioIdipClient*pFindDest)
{
	m_pDestinations->Lock(_T(__FUNCTION__));
	int i, nCount = m_pDestinations->GetSize();
	CIPCAudioIdipClient *pDest;
	for (i=0; i<nCount; i++)
	{
		pDest =m_pDestinations->GetAtFast(i);
		if (pFindDest == pDest)
		{
			ASSERT(pFindDest->m_pOwnerOfMe == this);
			pFindDest->m_pOwnerOfMe = NULL;
			pFindDest->m_SecID = SECID_NO_ID;
			m_pDestinations->Remove(pFindDest);
			break;
		}
	}
	
	if (m_pDestinations->GetSize() == 0)
	{
		StopRecord(m_RecordSecID);
	}
	m_pDestinations->Unlock();
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioIdipClient::HasDestinations()
{
	 return m_pDestinations->GetSize();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::DoRequestStopMediaDecoding(CSecID id)
{
	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(id);
	if (pRec)
	{
		pRec->SetRendering(false);
		OnUpdateRecord(*pRec);
	}
	
	m_nSentSamples = 0;
	CIPCMediaClient::DoRequestStopMediaDecoding(id, 0);
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::DoRequestStopMediaDecoding()
{
	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(GetDefaultOutputID());
	if (pRec && pRec->IsRendering())
	{
		CIPCMediaClient::DoRequestStopMediaDecoding(GetDefaultOutputID(), 0);
		pRec->SetRendering(false);
		m_nSentSamples = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::UpdateFlags(CSecID mediaID, DWORD dwValue)
{
	CIPCMediaRecord*pRec;
	CIPCMediaRecord::MediaRecordFlags Flags;
	Flags.Set(dwValue);
	for (WORD i=0; i<2; i++)
	{
		mediaID.SetSubID(i);
		pRec = GetMediaRecordPtrFromSecID(mediaID);
		if (pRec)
		{
			if (pRec->IsInput())
			{
				pRec->SetCaptureInitialized(Flags.IsCaptureInitialized);
				OnUpdateRecord(*pRec);
			}
			if (pRec->IsOutput())
			{
				if (!pRec->IsRendering())
				{
					pRec->SetEnabled(!Flags.IsRendering);
				}
				m_pDocument->UpdateAllViewsAsync(VDH_SET_AUDIO_INPUT_STATE, new CAudioUpdateHint(this, Flags));

				pRec->SetRenderInitialized(Flags.IsRenderInitialized);
				OnUpdateRecord(*pRec);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	if (sKey == CSD_REJECT_ANNOUNCEMENT)
	{
		StopPlay(id);
		AfxGetMainWnd()->PostMessage(WM_USER, REJECT_ANNOUNCEMENT);
	}
	else if (sKey == CSD_RESET && sValue == CSD_ON)
	{
		theApp.SetResetting();
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::DoRequestMediaDecoding(const CIPCMediaSampleRecord& media)
{
	DWORD dwFlags = 0;
	if (theApp.m_bWarnIfAudioIsBlockedAtNo)
	{
		dwFlags = MEDIA_DEC_CONFIRM_RECEIVING;
	}

	if (media.ContainsData())
	{
		if (m_SecID == m_idDefaultOutput)
		{
			CIPCMediaRecord*pRec;
			pRec = GetMediaRecordPtrFromSecID(GetDefaultOutputID());
			if (pRec)
			{
				pRec->SetRendering(true);
			}
		}
		CIPCMediaClient::DoRequestMediaDecoding(media, m_SecID, dwFlags);
		m_nSentSamples++;
		if (theApp.m_bWarnIfAudioIsBlockedAtNo)
		{
			if (m_nSentSamples == theApp.m_bWarnIfAudioIsBlockedAtNo)
			{
				AfxGetMainWnd()->PostMessage(WM_USER, MAKELONG(NO_MEDIA_RECEIVE_CONFIRM, m_wServerID), m_SecID.GetID());
			}
		}
	}
	else
	{
		CIPCMediaClient::DoRequestMediaDecoding(media, GetDefaultOutputID(), MEDIA_DEC_CONFIRM_RECEIVING);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioIdipClient::SetWndPlayActive(CWndPlay*pActive, CSecID idDestination)
{
#ifdef _DEBUG
	CString sName = _T("NULL");
	if (pActive)
	{
		sName = pActive->GetArchiveName();
	}
	TRACE(_T("### RKE Test: SetWndPlayActive(%s)"), sName);
#endif
	m_pWndPlayActive = pActive;
	m_SecID = idDestination;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioIdipClient::DoNotSendSamples()
{
	return (m_nSentSamples >= theApp.m_nMaxSentSamples) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioIdipClient::CanDecode()
{
	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(GetDefaultOutputID());
	if (pRec)
	{
		return !pRec->IsRendering() && pRec->IsEnabled() && pRec->IsOkay();
	}
	else
	{
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
CString CIPCAudioIdipClient::GetCompressionType(CIPCMediaSampleRecord*pR)
{
	CString s = _T("Audio");
	if (pR->IsLongHeader())
	{
		if (theApp.m_ComDSDll.GetModuleHandle())
		{
			void (WINAPIV*pGetGUIDString)(CString&, GUID*, BOOL) = (void (WINAPIV*)(CString&, GUID*, BOOL)) GetProcAddress(theApp.m_ComDSDll.GetModuleHandle(), GET_GUID_STRING);
			if (pGetGUIDString)
			{
				pR->InitFromBubble();
				GUID gFormat = pR->GetSubType();
				pGetGUIDString(s, &gFormat, TRUE);
			}
		}
	}
	return s;
}

//////////////////////////////////////////////////////////////////////////
// Class CAudioUpdateHint
IMPLEMENT_DYNAMIC(CAudioUpdateHint, CObject)

CAudioUpdateHint::CAudioUpdateHint(CIPCAudioIdipClient*pAudio, DWORD dwFlags)
{
	m_pAudio  = pAudio;
	m_dwFlags = dwFlags;
}
//////////////////////////////////////////////////////////////////////////
CAudioUpdateHint::CAudioUpdateHint()
{
	m_pAudio  = NULL;
	m_dwFlags = 0;
}
