// IPCControlAudioUnit.cpp: implementation of the CIPCAudioVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCControlAudioUnit.h"
#include "Vision.h"

#include "VisionDoc.h"
#include "ObjectView.h"
#include "Server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////
CIPCAudioVision::CIPCAudioVision(LPCTSTR shmName, CServer*pServer, CSecID id) : 
	CIPCMediaClient(shmName, FALSE)
{
	InitClassMembers();
	m_pServer   = pServer;
	ASSERT(m_pServer != NULL);
	m_pDocument = pServer->GetDocument();
	ASSERT(m_pDocument != NULL);
	m_SecID   = id;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////
CIPCAudioVision::CIPCAudioVision(LPCTSTR shmName, CVisionDoc*pDoc, BOOL bAsMaster/* = FALSE*/) :
	CIPCMediaClient(shmName, bAsMaster)
{
	InitClassMembers();
	m_pDocument = pDoc;
	ASSERT(m_pDocument != NULL);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::InitClassMembers()
{
	m_nOutputVolume     = -1;
	m_nInputChannel     = -1;
	m_nStereoChannel    = -1;
	m_nInputSensitivity = -1;
	m_nThreshold        = -1;
	m_nSentSamples      = 0;

	m_psaInputChannels  = NULL;
	m_pWndNotify        = NULL;
	m_pServer           = NULL;
	m_pDocument         = NULL;
	m_pMediaRecordArray = NULL;
	
	m_pDestinations = new CIPCAudioVisionList();
	m_pOwnerOfMe = NULL;
}

//////////////////////////////////////////////////////////////////////
CIPCAudioVision::~CIPCAudioVision()
{
	WK_DELETE(m_psaInputChannels);
	WK_DELETE(m_pMediaRecordArray);

	if (m_pOwnerOfMe)
	{
		m_pOwnerOfMe->RemoveDestination(this);
	}
	RemoveDestinations();
	WK_DELETE(m_pDestinations);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM)
{
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
			DoRequestInfoMedia(SECID_NO_GROUPID);
		}
		DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_INPUT_PIN_NAME, MEDIA_IDR_INPUT_PINS, 0xffffffff);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnConfirmConnection()
{
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
void CIPCAudioVision::OnConfirmSetGroupID(WORD wGroupID)
{
	DoRequestHardware(CSecID(wGroupID, 0), 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::RequestConfig(CSecID secID)
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
void CIPCAudioVision::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
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
		m_pWndNotify->PostMessage(m_nNotifyMessage, dwCmd, dwID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::SetOutputVolume(CSecID secID, int nValue)
{
	ASSERT(IsBetween(nValue, 0, 1000));
	DWORD dwFlagAndCMD = MEDIA_SET_VALUE|MEDIA_VOLUME;
#ifdef _DTS
	 dwFlagAndCMD |= MEDIA_SAVE_IN_REGISTRY;
#endif
	DoRequestValues(secID, dwFlagAndCMD, MEDIA_IDR_AUDIO_RENDERERS, nValue);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::SetInputSensitivity(CSecID secID, int nValue)
{
	ASSERT(IsBetween(nValue, -1, 1000)); // -1 : Automatic Gain
	DWORD dwFlagAndCMD = MEDIA_SET_VALUE|MEDIA_RECORDING_LEVEL;
	DoRequestValues(secID, dwFlagAndCMD, MEDIA_IDR_INPUT_PINS, nValue);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::SetInputChannel(CSecID secID, int nChannel)
{
	if (m_psaInputChannels)
	{
		ASSERT(IsBetween(nChannel, 0, m_psaInputChannels->GetSize()-1));
	}
	DoRequestValues(secID, CB_SELECTSTRING, MEDIA_IDR_INPUT_PINS, nChannel, NULL);
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::SetStereoChannel(CSecID secID, int nChannel)
{
	DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_STEREO_CHANNEL, MEDIA_IDR_INPUT_PINS, nChannel, NULL);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::SetThreshold(CSecID secID, int nValue)
{
	ASSERT(IsBetween(nValue, 0, 100));
	DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_THRESHOLD, MEDIA_IDR_INPUT_PINS, nValue);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::StartRecord(CSecID secID, bool bLocal, DWORD dwUserData)
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
void CIPCAudioVision::StopRecord(CSecID secID)
{
	if (secID == SECID_NO_ID)
	{
		secID = m_RecordSecID;
	}
	DoRequestStopMediaEncoding(secID, 0);								// stop recording
	
	CIPCAudioVision *pDest;
	while (m_pDestinations->GetSize())									// if there are Destinations
	{
		pDest = m_pDestinations->GetAt(0);
		m_pDestinations->Remove(pDest);									// remove them
		pDest->DoRequestStopMediaDecoding(pDest->GetSecID());		// stop them
		ASSERT(pDest->m_pOwnerOfMe == this);
		pDest->m_pOwnerOfMe = NULL;										// remove owner
	}
	m_RecordSecID = SECID_NO_ID;

	CIPCMediaRecord*pRec = GetMediaRecordPtrFromSecID(secID);
	if (pRec)
	{
		pRec->SetCapturing(false);
		OnUpdateRecord(*pRec);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::StopPlay(CSecID secID)
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
CString CIPCAudioVision::GetNamedInput(int nIndex, DWORD &dwChannel)  const
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
void CIPCAudioVision::OnConfirmInfoMedia(WORD wGroupID, int iNumGroups, int iNumRecords, const CIPCMediaRecordArray& records)
{
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

	if (m_pServer)
	{
		CObjectView* pOV = m_pDocument->GetObjectView();
		if (WK_IS_WINDOW(pOV))
		{	// CObjectView::InsertAudios() !
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_AUDIO_CONNECTED, m_pServer->GetID()));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnAddRecord(const CIPCMediaRecord& record)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnUpdateRecord(const CIPCMediaRecord& record)
{
	CObjectView* pOV = m_pDocument->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{	// CObjectView::ActualizeAudio() ! Update TreeItem
		if (m_pServer)
		{
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_ACTUALIZE, m_pServer->GetID()), record.GetID().GetID());
		}
		else
		{
			pOV->PostMessage(WM_USER, (DWORD)MAKELONG(WPARAM_ACTUALIZE, SECID_NO_SUBID), (LPARAM)record.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnDeleteRecord(const CIPCMediaRecord& record)
{
	if (m_pServer)
	{
		CObjectView* pOV = m_pDocument->GetObjectView();
		if (WK_IS_WINDOW(pOV))
		{
			// Delete Update TreeItem
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_DELETE, m_pServer->GetID()), record.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID, DWORD dwUserData)
{
	int i, nCount = m_pDestinations->GetSize();
	CIPCAudioVision *pDest;
	for (i=0; i<nCount; i++)
	{
		pDest = m_pDestinations->GetAt(i);
		pDest->DoRequestMediaDecoding(rec);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::AddDestination(CIPCAudioVision *pDest, CSecID secID)
{
	if (pDest->m_pOwnerOfMe == NULL)	// is it in any List?
	{
		pDest->m_pOwnerOfMe = this;
		pDest->m_SecID      = secID;
		m_pDestinations->Add(pDest);
		CIPCMediaRecord*pRec = pDest->GetMediaRecordPtrFromSecID(secID);
		if (pRec)
		{
			pRec->SetRendering(true);
			pDest->OnUpdateRecord(*pRec);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
const CIPCAudioVision* CIPCAudioVision::GetDestination(int nDest) const
{
	if (nDest < m_pDestinations->GetSize())
	{
		return m_pDestinations->GetAt(nDest);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::RemoveDestinations()
{
	int i, nCount = m_pDestinations->GetSize();
	CIPCAudioVision *pDest;
	for (i=0; i<nCount; i++)
	{
		pDest = m_pDestinations->GetAt(i);
		pDest->m_pOwnerOfMe = NULL;
		pDest->m_SecID = SECID_NO_ID;
	}
	m_pDestinations->RemoveAll();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::RemoveDestination(CIPCAudioVision*pFindDest)
{
	int i, nCount = m_pDestinations->GetSize();
	CIPCAudioVision *pDest;
	for (i=0; i<nCount; i++)
	{
		pDest =m_pDestinations->GetAt(i);
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

}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioVision::HasDestinations()
{
	 return m_pDestinations->GetSize();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::DoRequestStopMediaDecoding(CSecID id)
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
void CIPCAudioVision::UpdateFlags(CSecID mediaID, DWORD dwValue)
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
				
				CObjectView* pOV = m_pDocument->GetObjectView();
				if (WK_IS_WINDOW(pOV))
				{	// CObjectView::ActualizeAudio() ! Update TreeItem
					pOV->PostMessage(WM_USER, (DWORD)MAKELONG(WPARAM_AUDIO_SET_INPUT_STATE, Flags.IsRendering), (LPARAM)this);
				}

				pRec->SetRenderInitialized(Flags.IsRenderInitialized);
				OnUpdateRecord(*pRec);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	if (sKey == CSD_REJECT_ANNOUNCEMENT)
	{
		StopPlay(id);
		AfxGetMainWnd()->PostMessage(WM_USER, REJECT_ANNOUNCEMENT);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioVision::DoRequestMediaDecoding(const CIPCMediaSampleRecord& media)
{
	DWORD dwFlags = 0;
	if (theApp.m_bWarnIfAudioIsBlockedAtNo)
	{
		dwFlags = MEDIA_DEC_CONFIRM_RECEIVING;
	}

	CIPCMediaClient::DoRequestMediaDecoding(media, m_SecID, dwFlags);
	if (media.ContainsData())
	{
		if (theApp.m_bWarnIfAudioIsBlockedAtNo)
		{
			m_nSentSamples++;
			if (m_nSentSamples == theApp.m_bWarnIfAudioIsBlockedAtNo)
			{
				WORD wID = 0;
				if (m_pServer)
				{
					wID = m_pServer->GetID();
				}
				AfxGetMainWnd()->PostMessage(WM_USER, MAKELONG(NO_MEDIA_RECEIVE_CONFIRM, wID), m_SecID.GetID());
			}
		}
	}
}
