// IPCControlAudioUnit.cpp: implementation of the CIPCAudioDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCAudioDVClient.h"
#include "DVClient.h"
#include "PlayWindow.h"
#include "CPanel.h"
#include "MainFrame.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _UNICODE
 #ifdef _DEBUG
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShowU.dll")
 #else
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShowDU.dll")
 #endif
#define  GET_GUID_STRING "?GetGUIDString@@YAXAAV?$CStringT@GV?$StrTraitMFC_DLL@GV?$ChTraitsCRT@G@ATL@@@@@ATL@@PAU_GUID@@H@Z"
#else
 #ifdef _DEBUG
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShow.dll")
 #else
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShowD.dll")
 #endif
#define  GET_GUID_STRING "?GetGUIDString@@YAXAAVCString@@PAU_GUID@@H@Z"
#endif

CDllModule CIPCAudioDVClient::gm_ComDSDll(COMMON_DIRECT_SHOW_MODULE);

extern CDVClientApp theApp;

LPCTSTR CIPCAudioDVClient::gm_szType[] = 
{
	CSD_AUDIO_RECORDING_OFF, 
	CSD_AUDIO_RECORDING_A, 
	CSD_AUDIO_RECORDING_L, 
	CSD_AUDIO_RECORDING_E, 
	CSD_AUDIO_RECORDING_LA,
	CSD_AUDIO_RECORDING_AE,
	CSD_AUDIO_RECORDING_LE,
	CSD_AUDIO_RECORDING_LAE,
	NULL
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCAudioDVClient::CIPCAudioDVClient(LPCTSTR shmName, CServer* pServer, BOOL bAsMaster) 
	: CIPCMediaClient(shmName, bAsMaster)
{
	m_pServer = pServer;
#ifdef _UNICODE
	if (pServer)
	{
		m_wCodePage = pServer->GetAudioCodePage();
	}
	else
	{
		m_wCodePage = CODEPAGE_UNICODE;
	}
#endif
	StartThread();
	m_nSentSamples = 0;
	m_pActiveDisplayWindow = NULL;
	m_bDecoding  = false;
	m_pDefaultOutput    = NULL;
	m_pDefaultInput     = NULL;
	m_nOutputVolume     = ERROR_VALUE;
	m_nInputSensitivity = ERROR_VALUE;
	m_nThreshold        = ERROR_VALUE;

	m_nRecordingMode    = AUDIO_RECORDING_UNKNOWN;
	m_nCameraNumber     = ERROR_VALUE;

	m_bInputOn          = ERROR_VALUE;
	m_bOutputOn         = ERROR_VALUE;

	WK_TRACE(_T("Create Audio connection %s\n"), shmName);
}
//////////////////////////////////////////////////////////////////////
CIPCAudioDVClient::~CIPCAudioDVClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnAddRecord(const CIPCMediaRecord& record)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnUpdateRecord(const CIPCMediaRecord& record)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnDeleteRecord(const CIPCMediaRecord& record)
{
	if (&record == m_pDefaultOutput)
	{
		m_pDefaultOutput = NULL;
	}
	if (&record == m_pDefaultInput)
	{
		m_pDefaultInput = NULL;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnReadChannelFound()
{
	WK_TRACE(_T("CIPCAudioDVClient::OnReadChannelFound()\n"));
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmConnection()
{
	if (GetIsMaster()) //wenn kein Server da ist (Receiver bzw. ReadOnly) muss die
					   //Hardware selbst vom Client ermittelt werden
	{
		DoRequestSetGroupID(SECID_GROUP_MEDIA);
	}
	else
	{
		CIPCMediaClient::OnConfirmConnection();
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmSetGroupID(WORD wGroupID)
{
	DoRequestHardware(CSecID(wGroupID, 0), 0);
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestDisconnect()
{
	WK_TRACE(_T("CIPCAudioDVClient::OnRequestDisconnect\n"));
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmValues(CSecID media, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	BOOL bError = (dwCmd & MEDIA_ERROR_OCCURRED) ? TRUE : FALSE;
	if (dwCmd & MEDIA_SET_VALUE)
	{
		if (MEDIA_COMMAND_VALUE(dwCmd) == CIPC_MEDIA_REQUEST_DECODING)
		{
			m_nSentSamples--;
			if (!DoNotSendSamples() && m_pActiveDisplayWindow)
			{
				theApp.GetMainFrame()->DoRequest();
			}
		}
		if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_STATE)
		{
			UpdateFlags(dwValue);
		}

		if (!bError)
		{
			if (dwID == MEDIA_IDR_INPUT_PINS)
			{
				switch (MEDIA_COMMAND_VALUE(dwCmd))
				{
					case MEDIA_RECORDING_LEVEL:
					case MEDIA_THRESHOLD:
						m_eRequest.SetEvent();
						dwCmd |= MEDIA_GET_VALUE; // to set the members (*)
						break;
				}
			}
			else if (dwID == MEDIA_IDR_AUDIO_RENDERERS)
			{
				switch (MEDIA_COMMAND_VALUE(dwCmd))
				{
					case MEDIA_VOLUME:
						m_eRequest.SetEvent();
						dwCmd |= MEDIA_GET_VALUE; // to set the members (*)
						break;
				}
			}
		}
		CPanel* pPanel = theApp.GetPanel();
		if(pPanel)
		{
			pPanel->PostMessage(WM_UPDATE_MENU,1);
		}
	}
	if (dwCmd & MEDIA_GET_VALUE) // (*)
	{
		if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			switch (MEDIA_COMMAND_VALUE(dwCmd))
			{
				case MEDIA_RECORDING_LEVEL:
					if (bError)
					{
						m_nInputSensitivity = ERROR_VALUE;
					}
					else if (dwValue == -1)
					{
						m_nInputSensitivity = INPUT_SENSITIVITY_AUTO;
					}
					else
					{
						m_nInputSensitivity = dwValue / 10; 
					}
					break;

				case MEDIA_THRESHOLD:
					m_nThreshold = bError ? ERROR_VALUE : dwValue;
					break;
			}
		}
		else if (dwID == MEDIA_IDR_AUDIO_RENDERERS)
		{
			switch (MEDIA_COMMAND_VALUE(dwCmd))
			{
				case MEDIA_VOLUME:
					m_nOutputVolume = bError ? ERROR_VALUE : dwValue / 10;
					break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwData)
{
	OnConfirmGetValue(id, sKey, sValue, dwData);
	m_eRequest.SetEvent();
	CPanel* pPanel = theApp.GetPanel();
	if(pPanel)
	{
		pPanel->PostMessage(WM_UPDATE_MENU,1);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwData)
{
	if (sKey == CSD_AUDIO_REC)
	{
		m_nRecordingMode = AUDIO_RECORDING_UNKNOWN;
		int i = 0;
		while (gm_szType[i])
		{
			if (sValue == gm_szType[i])
			{
				m_nRecordingMode = i;
				break;
			}
			i++;
		}
	}
	else if (sKey == CSD_AUDIO_CAM)
	{
		if (1 != _stscanf(sValue, _T("%d"), &m_nCameraNumber))
		{
			m_nCameraNumber = ERROR_VALUE;
		}
	}
	else if (sKey == CSD_AUDIO_IN)
	{
		if (1 != _stscanf(sValue, _T("%d"), &m_bInputOn))
		{
			m_bInputOn = ERROR_VALUE;
		}
	}
	else if (sKey == CSD_AUDIO_OUT)
	{
		if (1 != _stscanf(sValue, _T("%d"), &m_bOutputOn))
		{
			m_bOutputOn = ERROR_VALUE;
		}
		if (IsOutputOn())
		{
			if (   GetOutputVolume() == ERROR_VALUE     // if it should request
				&& GetDefaultOutputID() != SECID_NO_ID)	// and can request
			{											// do it
				DoRequestValues(GetDefaultOutputID(), MEDIA_GET_VALUE|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM)
{
	if (dwRequestID == 0)
	{
		if (errorCode == 0)
		{
			CIPCMediaRecord::MediaRecordFlags Flags;
			Flags.Set(dwFlags);
			if (GetIsMaster())  //wenn kein Server da ist (Receiver bzw. ReadOnly) muss die
								//Hardware selbst vom Client ermittelt werden
			{
				CIPCMediaRecord* pRec;
				int numRecords = 0;
				CIPCMediaRecordArray array;
				if (Flags.IsInput)						// is there an audio input
				{
					pRec = new CIPCMediaRecord();		// for what ever
					Flags.IsOutput = 0;
					Flags.IsEnabled = true;
					Flags.IsDefault = true;
					pRec->Set(_T("LocalInput"), CSecID(SECID_GROUP_MEDIA, (WORD)numRecords), Flags);
					numRecords++;
					array.Add(pRec);
				}
				Flags.Set(dwFlags);
				if (Flags.IsOutput)						// is there an audio output
				{
					pRec = new CIPCMediaRecord();		// for replay of archived audio data
					Flags.IsInput = 0;
					Flags.IsEnabled = true;
					Flags.IsDefault = true;
					pRec->Set(_T("LocalOutput"), CSecID(SECID_GROUP_MEDIA, (WORD)numRecords), Flags);
					numRecords++;
					array.Add(pRec);
					DoRequestValues(pRec->GetID(), MEDIA_SET_VALUE|MEDIA_VOLUME|MEDIA_SAVE_IN_REGISTRY, MEDIA_IDR_AUDIO_RENDERERS, 1000);
				}
				OnConfirmInfoMedia(0, 0, numRecords, array);
				array.DeleteAll();
			}
			else
			{
				DoRequestInfoMedia(SECID_NO_GROUPID);
			}

			UpdateFlags(dwFlags);
		}
		else
		{
			CIPCMediaRecord::MediaRecordFlags Flags;
			Flags.Reset();
			UpdateFlags(Flags);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::UpdateFlags(DWORD dwFlags)
{
	CIPCMediaRecord::MediaRecordFlags Flags;
	Flags.Set(dwFlags);
	if (m_pDefaultInput)
	{
		if (Flags.IsInput)
		{
			m_pDefaultInput->SetEnabled(true);
			m_pDefaultInput->SetCaptureInitialized(Flags.IsCaptureInitialized);
		}
	}
	
	if (m_pDefaultOutput)
	{
		if (Flags.IsOutput)
		{
			if (!m_pDefaultOutput->IsRendering())
			{
				m_pDefaultOutput->SetEnabled(!Flags.IsRendering);
			}
			m_pDefaultOutput->SetRenderInitialized(Flags.IsRenderInitialized);
			m_pDefaultOutput->SetCaptureInitialized(Flags.IsCaptureInitialized);
		}
		else
		{
			m_pDefaultOutput->SetEnabled(false);
		}
	}

	if (Flags.IsInput)
	{
		if (!Flags.IsCaptureInitialized)
		{
			CSecID id = GetDefaultInputID();
			if (id == SECID_NO_ID)
			{
				id = GetDefaultOutputID();
			}
			if (id != SECID_NO_ID)
			{
				DoRequestStartMediaEncoding(id, MEDIA_ENC_ONLY_INITIALIZE, 0);
			}
		}
		else
		{
			if (GetInputSensitivity() == ERROR_VALUE)
			{
				DoRequestValues(GetDefaultInputID() , MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS);
			}
		}

		if (GetInputThreshold() == ERROR_VALUE)
		{
			DoRequestValues(GetDefaultInputID() , MEDIA_GET_VALUE|MEDIA_THRESHOLD , MEDIA_IDR_INPUT_PINS);
		}

		if (!GetIsMaster())
		{
			if (GetRecordingMode() == AUDIO_RECORDING_UNKNOWN)
			{
				DoRequestGetValue(SECID_NO_ID, CSD_AUDIO_REC);
			}
			if (GetCameraNumber() == ERROR_VALUE)
			{
				DoRequestGetValue(SECID_NO_ID, CSD_AUDIO_CAM);
			}
			if (m_bInputOn == ERROR_VALUE)
			{
				DoRequestGetValue(SECID_NO_ID, CSD_AUDIO_IN);
			}
		}
	}

	if (Flags.IsOutput)
	{
		if (!GetIsMaster())
		{
			if (m_bOutputOn == ERROR_VALUE)
			{
				DoRequestGetValue(SECID_NO_ID, CSD_AUDIO_OUT);
			}
			else if (   IsOutputOn() 
				&& GetDefaultOutputID() != SECID_NO_ID
				&& GetOutputVolume() != ERROR_VALUE)
			{
				SetOutputVolume(GetOutputVolume());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnConfirmInfoMedia(WORD wGroupID, 
								    int iNumGroups, 
								    int iNumRecords, 
								    const CIPCMediaRecordArray& records)
{
	CIPCMediaClient::OnConfirmInfoMedia(wGroupID, iNumGroups, iNumRecords, records);
	m_pDefaultOutput = NULL;
	m_pDefaultInput  = NULL;
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
				m_pDefaultOutput = (CIPCMediaRecord*)&mr;
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
				m_pDefaultInput = (CIPCMediaRecord*)&mr;
			}
		}
	}
	if (m_pDefaultOutput == NULL && nFirstOut != -1)
	{
		m_pDefaultOutput = (CIPCMediaRecord*)&GetMediaRecordFromIndex(nFirstOut);
	}
	if (m_pDefaultInput == NULL && nFirstIn != -1)
	{
		m_pDefaultInput = (CIPCMediaRecord*)&GetMediaRecordFromIndex(nFirstIn);
	}

	if (m_pDefaultOutput)
	{
		DoRequestValues(m_pDefaultOutput->GetID(), MEDIA_GET_VALUE|MEDIA_STATE, 0);
	}
	else if (m_pDefaultInput)
	{
		DoRequestValues(m_pDefaultInput->GetID(), MEDIA_GET_VALUE|MEDIA_STATE, 0);
	}
}
////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::DoRequestMediaDecoding(const CIPCMediaSampleRecord& media)
{
	if (m_pDefaultOutput == NULL      ) return;
	if (!m_pDefaultOutput->IsEnabled()) return;

	if (media.ContainsData() == false)
	{
		CIPCMediaClient::DoRequestMediaDecoding(media, GetDefaultOutputID(), MEDIA_DEC_CONFIRM_RECEIVING);
	}
	else
	{
		if (m_pDefaultOutput)
		{
			m_pDefaultOutput->SetRendering(true);
		}
		CIPCMediaClient::DoRequestMediaDecoding(media, GetDefaultOutputID(), MEDIA_DEC_CONFIRM_RECEIVING);
		m_nSentSamples++;
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::DoRequestStopMediaDecoding()
{
	if (m_pDefaultOutput && m_pDefaultOutput->IsRendering())
	{
		CIPCMediaClient::DoRequestStopMediaDecoding(GetDefaultOutputID(), 0);
		m_pDefaultOutput->SetRendering(false);
	}
}
//////////////////////////////////////////////////////////////////////////
CString CIPCAudioDVClient::GetCompressionType(CIPCMediaSampleRecord*pR)
{
	CString s = _T("Audio");
	if (pR->IsLongHeader())
	{
		if (gm_ComDSDll.GetModuleHandle())
		{
			void (WINAPIV*pGetGUIDString)(CString&, GUID*, BOOL) = (void (WINAPIV*)(CString&, GUID*, BOOL)) GetProcAddress(gm_ComDSDll.GetModuleHandle(), GET_GUID_STRING);
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
void CIPCAudioDVClient::SetActiveDisplayWindow(CPlayWindow *pActive)
{
	m_pActiveDisplayWindow = pActive;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::DoNotSendSamples()
{
	return (m_nSentSamples >= theApp.m_nMaxSentSamples) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
CSecID CIPCAudioDVClient::GetDefaultOutputID()
{
	if (m_pDefaultOutput)
	{
		return m_pDefaultOutput->GetID();
	}
	else
	{
		return CSecID();
	}
}
//////////////////////////////////////////////////////////////////////////
CSecID CIPCAudioDVClient::GetDefaultInputID()
{
	if (m_pDefaultInput)
	{
		return m_pDefaultInput->GetID();
	}
	else
	{
		return CSecID();
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::CanDecode()
{
	if (m_pDefaultOutput)
	{
		return !m_pDefaultOutput->IsRendering() && m_pDefaultOutput->IsEnabled() && m_pDefaultOutput->IsOkay();
	}
	else
	{
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SetInputSensitivity(int nValue)
{
	BOOL bRet = FALSE;

	if (IsInputOn() || GetCameraNumber() > 0)
	{
		if (nValue == INPUT_SENSITIVITY_AUTO)
		{
			nValue = -1;
		}
		else if (nValue == -1)
		{
			return FALSE;
		}
		else
		{
			nValue *= 10;
		}
		ASSERT(IsBetween(nValue, -1, 1000)); // -1 : Automatic Gain

		m_eRequest.ResetEvent();
		DoRequestValues(GetDefaultInputID(), MEDIA_SET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS, nValue);
		if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
		{
			bRet = TRUE;
		}
	}
	else
	{
		m_nInputSensitivity = nValue;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SetInputThreshold(int nValue)
{
	BOOL bRet = FALSE;
	if (IsInputOn() || GetCameraNumber() > 0)
	{
		ASSERT(IsBetween(nValue, 0, 100));

		m_eRequest.ResetEvent();
		DoRequestValues(GetDefaultInputID(), MEDIA_SET_VALUE|MEDIA_SAVE_IN_REGISTRY|MEDIA_THRESHOLD, MEDIA_IDR_INPUT_PINS, nValue);
		if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
		{
			bRet = TRUE;
		}
	}
	else
	{
		m_nThreshold = nValue;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SetOutputVolume(int nValue)
{
	BOOL bRet = FALSE;
	if (IsOutputOn())
	{
		nValue *= 10;
		ASSERT(IsBetween(nValue, 0, 1000));

		m_eRequest.ResetEvent();
		DoRequestValues(GetDefaultOutputID(), MEDIA_SET_VALUE|MEDIA_VOLUME|MEDIA_SAVE_IN_REGISTRY, MEDIA_IDR_AUDIO_RENDERERS, nValue);
		if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
		{
			bRet = TRUE;
		}
	}
	else
	{
		m_nOutputVolume = nValue;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SetRecordingMode(int nMode)
{
	BOOL bRet = FALSE;

	if (IsBetween(nMode, AUDIO_RECORDING_OFF, AUDIO_RECORDING_ALE)) 
	{ // zwischen aus und englischer bier aufnahme
		m_eRequest.ResetEvent();
		DoRequestSetValue(SECID_NO_ID, CSD_AUDIO_REC, gm_szType[nMode]);
		if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
		{
			bRet = TRUE;
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SetCameraNumber(int nCam)
{
	BOOL bRet = FALSE;
	if (IsBetween(nCam, 0, 16)) 
	{ 
		CString sValue;
		if (nCam == 0)
		{
			sValue = CSD_OFF;
		}
		else
		{
			sValue.Format(_T("%d"), nCam);
		}

		m_eRequest.ResetEvent();
		DoRequestSetValue(SECID_NO_ID, CSD_AUDIO_CAM, sValue);
		if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SwitchInput(LPCTSTR str)
{
	BOOL bRet = FALSE;
	m_eRequest.ResetEvent();
	DoRequestSetValue(SECID_NO_ID, CSD_AUDIO_IN, str);
	if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
	{
		bRet = TRUE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::SwitchOutput(LPCTSTR str)
{
	BOOL bRet = FALSE;
	m_eRequest.ResetEvent();
	DoRequestSetValue(SECID_NO_ID, CSD_AUDIO_OUT, str);
	if(WaitForSingleObject(m_eRequest, 2000) == WAIT_OBJECT_0)
	{
		bRet = TRUE;
	}
	return bRet;
}
