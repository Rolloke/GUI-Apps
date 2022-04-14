// IPCAudioServerClient.cpp: implementation of the CIPCAudioServerClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "IPCAudioServerClient.h"
#include "ProcessSchedulerAudio.h"
#include "MediaSampleRecord.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CIPCAudioServerClient::gm_bTraceDoClientEncoding = FALSE;
BOOL CIPCAudioServerClient::gm_bTraceOnRequestStartMediaEncoding = FALSE;
BOOL CIPCAudioServerClient::gm_bTraceOnRequestStopMediaEncoding = FALSE;
BOOL CIPCAudioServerClient::gm_bTraceRequestMediaDecoding = FALSE;
BOOL CIPCAudioServerClient::gm_bTraceOnRequestStopMediaDecoding = FALSE;
BOOL CIPCAudioServerClient::gm_bTraceOnRequestValues = FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCAudioServerClient::CIPCAudioServerClient(const CConnectionRecord &c,
											CIPCServerControlServerSide *pControl,
											LPCTSTR shmName,
											WORD wNr)
	: CIPCMedia(shmName, TRUE) , m_ConnectionRecord(c)
{
	m_dwPriority = 3;
	m_pControl = pControl;
	m_id.Set(SECID_AUDIO_CLIENT,wNr);
	m_dwOptions = 0;
	m_bInfoConfirmed = FALSE;
//	WK_TRACE(_T("CIPCAudioServerClient(%s)\n"), shmName);
	CPermission *pPermission = theApp.GetPermissions().GetPermission(c.GetPermission());
	if (pPermission) 
	{
		m_dwPriority = pPermission->GetPriority()-1;	// OOPS 1..5
		m_idPermission = pPermission->GetID();
	} 
}
//////////////////////////////////////////////////////////////////////
CIPCAudioClientsArray::CIPCAudioClientsArray()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
//////////////////////////////////////////////////////////////////////
CIPCAudioServerClient::~CIPCAudioServerClient()
{
//	WK_TRACE(_T("~CIPCAudioServerClient(%s)\n"), GetShmName());
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestInfoMedia(WORD wGroupID)
{
	CHECK_ACCESS(CIPC_MEDIA_REQUEST_INFO);

	CPermission *pPermission = theApp.GetPermissions().GetPermission(m_ConnectionRecord.GetPermission());

	int iNumGroups = theApp.GetAudioGroups().GetSize();
	CIPCAudioServer* pAudioGroup;
	CIPCMediaRecordArray records;
	records.SetAutoDelete(true);
	CIPCMediaRecord::MediaRecordFlags Flags;
	CIPCMediaRecord::MediaRecordFlags HardwareState;

	for (int g=0;g<iNumGroups;g++) 
	{
		pAudioGroup = theApp.GetAudioGroups().GetAtFast(g);
		if (   pAudioGroup
			&& (wGroupID==SECID_NO_GROUPID || wGroupID == pAudioGroup->GetGroupID())
			)
		{ 
			HardwareState.Set(pAudioGroup->GetHardwareStateFlags());
			for (int i=0;i<pAudioGroup->GetSize();i++)
			{ 
				Flags.Reset();
				const CMedia* pMedia = pAudioGroup->GetMediaAt(i);

				if (pPermission && pPermission->IsOutputAllowed(pMedia->GetID()) && pMedia->IsEnabled())
				{
					if (pMedia->GetType() == SEC_MEDIA_TYPE_INPUT)
					{
#ifdef _DEBUG			// TODO! RKE: test USB devices later
						if (!HardwareState.IsInput)
						{
							continue;
						}
#endif
						Flags.IsInput = true;
					}
					else if (pMedia->GetType() == SEC_MEDIA_TYPE_OUTPUT)
					{
#ifdef _DEBUG			// TODO! RKE: test USB devices later
						if (!HardwareState.IsOutput)
						{
							continue;
						}
#endif
						Flags.IsOutput = true;
					}
					CIPCMediaRecord* pMediaRecord = new CIPCMediaRecord;
					if (pAudioGroup->GetHardwareState()==0)
					{
						Flags.IsOkay    = true;
						Flags.IsEnabled = true;
						Flags.IsAudio   = true;
					}
					Flags.IsDefault = pMedia->IsDefault();

					pMediaRecord->Set(pMedia->GetName(),pMedia->GetID(), Flags);
					records.Add(pMediaRecord);
				}
				else
				{ 
					// no permission
				}
			}
		}
	}
	DoConfirmInfoMedia(wGroupID,iNumGroups,records.GetSize(),records);
	m_bInfoConfirmed = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData)
{
	if (gm_bTraceOnRequestStartMediaEncoding)
	{
		WK_TRACE(_T("CIPCAudioServerClient::OnRequestStartMediaEncoding\n"));
	}
	CHECK_ACCESS(CIPC_MEDIA_REQUESTSTART_ENCODING);

	// NOT YET AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
	if (pCIPCAudioServer)
	{
		DoClientEncoding(pCIPCAudioServer, mediaID, dwFlags, TRUE);
	}
	else
	{
		WK_TRACE_ERROR(_T("no audio group for id %08lx\n"),mediaID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::DoClientEncoding(CIPCAudioServer* pCIPCAudioServer,CSecID mediaID, DWORD dwFlags, BOOL bStart)
{
	if (gm_bTraceDoClientEncoding)
	{
		WK_TRACE(_T("CIPCAudioServerClient::DoClientEncoding\n"));
	}
	CProcess* pProcess = NULL;
	BOOL bFound = FALSE;
	m_Processes.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Processes.GetSize();i++)
	{
		pProcess = m_Processes.GetAtFast(i);
		if (   pProcess
			&& pProcess->IsClientProcess()
			&& (pProcess->GetActivation()->GetOutputID() == mediaID)
			)
		{
			// a process already exist modify
			bFound = TRUE;
			break;
		}
		else
		{
			pProcess = NULL;
		}
	}
	if (   bFound
		&& bStart == FALSE)
	{
		m_Processes.Remove(pProcess);
	}

	pCIPCAudioServer->GetProcessScheduler()->DoClientEncoding(GetID(),pProcess,mediaID,dwFlags,bStart);

	if (   !bFound
		&& pProcess)
	{
		m_Processes.Add(pProcess);
	}
	m_Processes.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags)
{
	if (gm_bTraceOnRequestStopMediaEncoding)
	{
		WK_TRACE(_T("CIPCAudioServerClient::OnRequestStopMediaEncoding\n"));
	}
	CHECK_ACCESS(CIPC_MEDIA_REQUESTSTOP_ENCODING);

	// AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
	if (pCIPCAudioServer)
	{
		DoClientEncoding(pCIPCAudioServer,mediaID,0,FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestMediaDecoding(const CIPCMediaSampleRecord& data, CSecID mediaID, DWORD dwFlags)
{
	CHECK_ACCESS(CIPC_MEDIA_REQUEST_DECODING);
	if (gm_bTraceRequestMediaDecoding)
	{
		WK_TRACE(_T("CIPCAudioServerClient::OnRequestMediaDecoding, No: %d, Size: %d\n"), data.GetPackageNumber(), data.GetActualLength());
	}

	// NOT YET AUDIO decoder raussuchen!
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
		
	if (pCIPCAudioServer)
	{
		pCIPCAudioServer->DoRequestMediaDecoding(this, data, mediaID, dwFlags);
	}
	else
	{
		pCIPCAudioServer = theApp.GetAudioGroups().GetDecoder();
		if (pCIPCAudioServer)
		{
			pCIPCAudioServer->DoRequestMediaDecoding(this, data, pCIPCAudioServer->GetDecoderID(), dwFlags);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	if (gm_bTraceOnRequestStopMediaDecoding)
	{
		WK_TRACE(_T("CIPCAudioServerClient::OnRequestStopMediaDecoding()"));
	}
	CHECK_ACCESS(CIPC_MEDIA_REQUESTSTOP_DECODING);
	
	
	// NOT YET AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
	if (pCIPCAudioServer)
	{
		mediaID.SetSubID(m_id.GetSubID());
		pCIPCAudioServer->DoRequestStopMediaDecoding(mediaID, dwFlags);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestHardware(CSecID secID, DWORD dwIDRequest)
{
	CHECK_ACCESS(CIPC_MEDIA_REQUESTHARDWARE);

	// NOT YET AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(secID);
	if (pCIPCAudioServer)
	{
		secID.SetSubID(m_id.GetSubID());
		pCIPCAudioServer->DoRequestHardware(secID, dwIDRequest);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestValues(CSecID secID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	CHECK_ACCESS(CIPC_MEDIA_REQUEST_VALUES);

	if (gm_bTraceOnRequestValues)
	{
		WK_TRACE(_T("CIPCAudioServerClient::OnRequestValues(%x)=> SecID:%08x, ID:%d, F:%x\n"), dwCmd, secID.GetID(), dwID, dwValue);
	}
	// NOT YET AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(secID);
	if (pCIPCAudioServer)
	{
		secID.SetSubID(m_id.GetSubID());
		pCIPCAudioServer->DoRequestValues(secID, dwCmd, dwID, dwValue, pData);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServerClient::OnRequestDisconnect()
{
	if (m_bInfoConfirmed)
	{
		theApp.StopClientProcesses(this);
	}
	m_pControl->RemoveMe(this);
}
