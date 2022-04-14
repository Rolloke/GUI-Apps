// IPCControlAudioUnit.cpp: implementation of the CIPCAudioRecherche class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "recherche.h"
#include "IPCControlAudioUnit.h"

#include "MainFrm.h"
#include "RechercheView.h"
#include "DisplayWindow.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
 CDllModule CIPCAudioRecherche::gm_ComDSDll(_T("CommonDirectShowD.dll"));
#else
 CDllModule CIPCAudioRecherche::gm_ComDSDll(_T("CommonDirectShow.dll"));
#endif

extern CRechercheApp theApp;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCAudioRecherche::CIPCAudioRecherche(LPCTSTR shmName, CRechercheDoc* pDoc, BOOL bAsMaster/*=FALSE*/) 
	: CIPCMediaClient(shmName, bAsMaster)
{
	StartThread();
	m_nSentSamples = 0;
	m_pRechercheDoc = pDoc;
	m_pActiveDisplayWindow = NULL;
	m_bDecoding  = false;
	m_pDefaultOutput = NULL;
}
//////////////////////////////////////////////////////////////////////
CIPCAudioRecherche::~CIPCAudioRecherche()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnAddRecord(const CIPCMediaRecord& record)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnUpdateRecord(const CIPCMediaRecord& record)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnDeleteRecord(const CIPCMediaRecord& record)
{
	if (&record == m_pDefaultOutput)
	{
		m_pDefaultOutput = NULL;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnReadChannelFound()
{
	TRACE(_T("CIPCAudioRecherche::OnReadChannelFound()\n"));
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnConfirmConnection()
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
void CIPCAudioRecherche::OnRequestDisconnect()
{
	UpdateFlags(0);
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnConfirmSetGroupID(WORD wGroupID)
{
	DoRequestHardware(CSecID(wGroupID, 0), 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnConfirmValues(CSecID media, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	if (dwCmd & MEDIA_SET_VALUE) 
	{
		if (MEDIA_COMMAND_VALUE(dwCmd) == CIPC_MEDIA_REQUEST_DECODING)
		{
			m_nSentSamples--;
			if (!DoNotSendSamples() && m_pActiveDisplayWindow)
			{
				m_pActiveDisplayWindow->PostMessage(WM_USER, DW_CHECK_PLAY_STATUS);
			}
		}
		if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_STATE)
		{
			UpdateFlags(dwValue);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM)
{
	if (dwRequestID == 0)
	{
		if (errorCode == 0)
		{
			m_nSentSamples = 0;
			CIPCMediaRecord::MediaRecordFlags Flags;
			Flags.Set(dwFlags);
			if (GetIsMaster())
			{
				CIPCMediaRecord* pRec;
				int numRecords = 0;
				CIPCMediaRecordArray array;
				if (Flags.IsOutput)					// is an audio output there
				{
					pRec = new CIPCMediaRecord();
					Flags.IsInput = 0;				// only the output is needed
					Flags.IsEnabled = true;
					Flags.IsDefault = true;
					pRec->Set(_T("LocalOutput"), CSecID(SECID_GROUP_MEDIA, (WORD)numRecords), Flags);
					numRecords++;
					array.Add(pRec);
					DoRequestValues(pRec->GetID(), MEDIA_SET_VALUE|MEDIA_VOLUME|MEDIA_SAVE_IN_REGISTRY, MEDIA_IDR_AUDIO_RENDERERS, 1000);
				}
				OnConfirmInfoMedia(SECID_NO_GROUPID, 0, numRecords, array);
				array.DeleteAll();
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
void CIPCAudioRecherche::UpdateFlags(DWORD dwFlags)
{
	if (m_pDefaultOutput)
	{
		CIPCMediaRecord::MediaRecordFlags Flags;
		Flags.Set(dwFlags);
		if (Flags.IsOutput)
		{
			if (!m_pDefaultOutput->IsRendering())
			{
				m_pDefaultOutput->SetEnabled(!Flags.IsRendering);
			}
			m_pDefaultOutput->SetRenderInitialized(Flags.IsRenderInitialized);
		}
		else
		{
			m_pDefaultOutput->SetEnabled(false);
		}
		CWnd *pWnd = theApp.GetMainWnd();
		ASSERT_KINDOF(CMainFrame, pWnd);
		CRechercheView *pView = ((CMainFrame*)pWnd)->GetRechercheView();
		if (pView)
		{
			CSmallWindows*pSWs = pView->GetSmallWindows();
			if (pSWs)
			{
				int i, nSize = pSWs->GetSize();
				for (i=0; i<nSize; i++)
				{
					CSmallWindow *pSW = pSWs->GetAt(i);
					if (pSW->IsDisplayWindow())
					{
						((CDisplayWindow*)pSW)->UpdateAudioState(this);
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::OnConfirmInfoMedia(WORD wGroupID, 
								    int iNumGroups, 
								    int iNumRecords, 
								    const CIPCMediaRecordArray& records)
{
	CIPCMediaClient::OnConfirmInfoMedia(wGroupID, iNumGroups, iNumRecords, records);
	m_pDefaultOutput = NULL;
	int nFirstOut = -1, i, nCount = GetNumberOfMedia();
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
	}
	if (m_pDefaultOutput == NULL && nFirstOut != -1)
	{
		m_pDefaultOutput = (CIPCMediaRecord*)&GetMediaRecordFromIndex(nFirstOut);
	}

	if (m_pDefaultOutput)
	{
		DoRequestValues(m_pDefaultOutput->GetID(), MEDIA_GET_VALUE|MEDIA_STATE, 0);
	}
}
////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::DoRequestMediaDecoding(const CIPCMediaSampleRecord& media)
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
/*
		// TODO! RKE: wenn die Wiedergabe nicht reagiert, könnte man die Unit resetten.
		// Dies ist aber erst sinnvoll, wenn die Wiedergabe optimiert wurde durch
		// sinnvolles Prefetch von Daten, die auch verarbeitet werden können (Netzlast, Systemlast)
		// mit bekanntem Inhalt der Sequenzen !!!
		if (m_nSentSamples == 30)
		{
			WORD wID = 0;
			if (m_pServer)
			{
				wID = m_pServer->GetID();
			}
			AfxGetMainWnd()->PostMessage(WM_USER, MAKELONG(NO_MEDIA_RECEIVE_CONFIRM, wID), m_SecID.GetID());
		}
*/
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioRecherche::DoRequestStopMediaDecoding()
{
	if (m_pDefaultOutput && m_pDefaultOutput->IsRendering())
	{
		CIPCMediaClient::DoRequestStopMediaDecoding(GetDefaultOutputID(), 0);
		m_pDefaultOutput->SetRendering(false);
		m_nSentSamples = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
CString CIPCAudioRecherche::GetCompressionType(CIPCMediaSampleRecord*pR)
{
	CString s = _T("Audio");
	if (pR->IsLongHeader())
	{
		if (gm_ComDSDll.GetModuleHandle())
		{
			void (WINAPIV*pGetGUIDString)(CString&, GUID*, BOOL) = (void (WINAPIV*)(CString&, GUID*, BOOL)) GetProcAddress(gm_ComDSDll.GetModuleHandle(), "?GetGUIDString@@YAXAAVCString@@PAU_GUID@@H@Z");
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
void CIPCAudioRecherche::SetActiveDisplayWindow(CDisplayWindow *pActive)
{
	CString sName = _T("NULL");
	if (pActive)
	{
		sName = pActive->GetArchiveName();
	}
	TRACE(_T("### RKE Test: SetActiveDisplayWindow(%s)"), sName);
	m_pActiveDisplayWindow = pActive;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCAudioRecherche::DoNotSendSamples()
{
	return (m_nSentSamples >= theApp.m_nMaxSentSamples) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
CSecID CIPCAudioRecherche::GetDefaultOutputID()
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
BOOL CIPCAudioRecherche::CanDecode()
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

