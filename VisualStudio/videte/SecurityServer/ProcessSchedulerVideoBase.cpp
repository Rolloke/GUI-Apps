#include "stdafx.h"
#include "sec3.h"
#include "processschedulervideobase.h"
#include "VideooutProcess.h"
#include "ClientEncodingProcess.h"
#include "clientrelayprocess.h"
#include "relayprocess.h"
#include "recordingprocess.h"
#include "cipcoutputserver.h"
#include "ProcessListView.h"
#include "RequestCollector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CProcessSchedulerVideoBase::CProcessSchedulerVideoBase(CIPCOutputServer* pCIPCOutputServer)
{
	m_pCIPCOutputServer = pCIPCOutputServer;
	m_sParentShm = pCIPCOutputServer->GetShmName();
	m_dwLastPicture = 0;
	m_iNumLongPSequences=0;
	m_iNumRelayProcesses = 0;
	m_pLocalVideoProcess=NULL;
}

CProcessSchedulerVideoBase::~CProcessSchedulerVideoBase()
{
}
/////////////////////////////////////////////////////////////////////////
BOOL CProcessSchedulerVideoBase::IsImageTimeout()
{
	BOOL bRet = FALSE;

	m_csLastPicture.Lock();
	if (   m_dwLastPicture != 0
		&& m_processes.GetSize()>0
		&& m_iNumStoringProcesses>0)
	{
		// todo check if there are any image processes
		bRet = WK_GetTimeSpan(m_dwLastPicture) > (60 * 1000);
	}
	m_csLastPicture.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////
int CProcessSchedulerVideoBase::GetMaxFps()
{
	return 0;
}
/////////////////////////////////////////////////////////////
int CProcessSchedulerVideoBase::GetMaxNumCameraSwitches()
{
	return 0;
}
/////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::DoClientVideoOut(CSecID id)
{
	Lock(_T(__FUNCTION__));

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (pProcess->IsVideoOutProcess())
		{
			CVideoOutProcess* pVideoOutProcess = (CVideoOutProcess*)pProcess;
			pVideoOutProcess->DoClientVideoOut(id);
		}
	}

	Unlock();
}
/////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::DoActivityVideoOut(CSecID id)
{
	Lock(_T(__FUNCTION__));

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (pProcess->IsVideoOutProcess())
		{
			CVideoOutProcess* pVideoOutProcess = (CVideoOutProcess*)pProcess;
			pVideoOutProcess->DoActivityVideoOut(id);
		}
	}

	Unlock();
}
/////////////////////////////////////////////////////////////
BOOL CProcessSchedulerVideoBase::StartNewProcess(CInput *pInput, 
												CActivation &activation,
												BOOL & bRefThereWasAProcess,
												CProcess*& pProcess,
												int iNumAlreadyRecorded /*= -1*/)
{
	BOOL bHandled = FALSE;
	bRefThereWasAProcess = FALSE;

	if (!CanStartNewProcess()) 
	{
		return TRUE;
	}

	pProcess = NULL;

	Lock(_T(__FUNCTION__));
	switch (activation.GetMacro().GetType())
	{
	case PMT_RELAY:
		// a non-client relay process
		// OOPS redundant code see below
		if (activation.GetInput() && activation.GetActiveProcess()) 
		{
			// OOPS is that all that is needed ?
			activation.GetActiveProcess()->SetState(SPS_INITIALIZED,WK_GetTickCount());
		} 
		else 
		{
			// create...
			BOOL bTimerOkay = CheckProcessForTimer(activation);
			if (bTimerOkay) 
			{
				pProcess = new CRelayProcess(this,&activation);	// non-client relay process
				bRefThereWasAProcess=TRUE;
				// add to 'container' and the according priority array
				AddProcess(pProcess);
				m_iNumRelayProcesses++;
				m_pCIPCOutputServer->DoRequestSetRelay(pProcess->GetCamID(),TRUE);
			} 
		}
		bHandled = TRUE;
		break;
	case PMT_VIDEO_OUT:
		if (CheckProcessForTimer(activation))
		{
			pProcess = new CVideoOutProcess(this,&activation);
			bRefThereWasAProcess=TRUE;
			AddProcess(pProcess);
		} 
		bHandled = TRUE;
		break;
	case PMT_PTZ_PRESET:
		if (CheckProcessForTimer(activation)) 
		{
			CameraControlCmd ccc = activation.GetCameraControlCmd();
			CSecID camID = activation.GetOutputID();

			if (ccc != CCC_INVALID)
			{
				CIPCInputServer* pInputGroup = theApp.GetInputGroups().GetGroupByShmName(SM_COMMUNIT_INPUT);
				if (   pInputGroup
					&& pInputGroup->GetSize()>0) 
				{
					TRACE(_T("set camera %08lx to position %s\n"),camID.GetID(),NameOfEnum(ccc));
					pInputGroup->DoRequestCameraControl(pInputGroup->GetInputAt(0)->GetID(),camID, ccc, 0);
				}
				else
				{
					WK_TRACE(_T("no PTZ Preset communit not connected\n"));
				}
			}
			else
			{
				WK_TRACE(_T("no PTZ Preset illegal PTZ position\n"));
			}
		} 
		bHandled = TRUE;
		break;
	case PMT_ALARM_LIST:
		if (CheckProcessForTimer(activation)) 
		{
			CSecID idOutput = activation.GetOutputID();
			CSecID idArchive;
			if (idOutput.IsOutputID())
			{
				COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(idOutput);
				if (pOutput)
				{
					idArchive = pOutput->GetCurrentArchiveID();
				}
			}
			SaveAlarm(pInput,activation.GetArchiveID().GetSubID(), idOutput,idArchive);
		}
		bHandled = TRUE;
		break;
	}
	Unlock();
	return bHandled;
}
/////////////////////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::AnswerRequest(const CIPCPictureRecord& pict, 
											   DWORD dwMDX,DWORD dwMDY,DWORD pid,
											   CRequestCollector* pRequest)
{
	/*
	TRACE(_T("AnswerRequest %08lx %d %s,%03d\n"),pict.GetCamID().GetID(),pict.GetBlockNr(),
		pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds());
		*/
	CProcess *pProcess = m_processes.GetByID(pid);

	if (pProcess) 
	{
		pProcess->Lock(_T(__FUNCTION__));
		if (   pProcess->GetState() != SPS_TERMINATED) 
		{
			pProcess->PictureData(pict,dwMDX,dwMDY,pRequest->GetID());
		}
		pProcess->Unlock();
	} 
	else 
	{
		// process identified by pid not found
		// might already have terminated
		if (m_bTracePictDataWithoutProcess)
		{
			WK_STAT_PEAK(_T("Process"),1,_T("PictDataWithoutProcess"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*@MHEAD{util:}*/
/*@MD NYD */
void CProcessSchedulerVideoBase::DoClientEncoding(CSecID idClient, 
											CProcessPtr &pProcess, // CAVEAT by ref, is set in call
											CSecID camID,
											Resolution res, 
											Compression comp,
											CompressionType ct,
											int iNumPictures,
											DWORD dwTimeSlice,
											DWORD dwBitrate)
{
	// CAVEAT the kill longSequence has to be before the 
	if (pProcess==NULL) 
	{
		if (iNumPictures) 
		{
			// create new process
			pProcess = new CClientEncodingProcess(this,idClient,camID, res,comp,ct, dwTimeSlice, iNumPictures);
			m_iNumClientProcesses++;
			// pClient->GetConnection(); NOT YET priority
			AddProcess(pProcess);
		} 
		else 
		{
			TRACE(_T("stop on non existing process\n"));
		}
	} 
	else 
	{
		if (iNumPictures) 
		{
			// modify process parameter
			pProcess->Modify(camID,res,comp,iNumPictures,dwTimeSlice, dwBitrate);
		} 
		else 
		{
			// stop process
			//			WK_TRACE(_T("terminating client process by client unrequest %08lx\n"),camID.GetID());
			// deletes pProcess
			TerminateProcess(pProcess,FALSE);
			pProcess=NULL;
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::ClientRelayProcess(CSecID idClient,
										   CProcessPtr &pRefEncodeProcess, // CAVEAT by ref, is set in call
										   CSecID relayID,
										   BOOL bRequestedState)
{
	const COutput *pRelay = m_pCIPCOutputServer->GetOutputAt(relayID.GetSubID());
	if (pRelay==NULL) 
	{
		WK_TRACE_ERROR(_T("%s:Relay %x not found\n"),m_pCIPCOutputServer->GetShmName(),relayID.GetID());
		return;	// <<< EXIT >>>
	}

	if (bRequestedState==TRUE) 
	{
		// "close relay"
		// start new client process

		m_pCIPCOutputServer->DoRequestSetRelay(relayID,TRUE);
		// create new process
		CProcess *pProcess = NULL;
		pProcess = new CClientRelayProcess(this,idClient,*pRelay);
		m_iNumClientProcesses++;	// OOPS what exactly are client processes
		m_iNumRelayProcesses++;

		AddProcess(pProcess);
		pRefEncodeProcess = pProcess;
	} 
	else 
	{
		// "open relay"
		// important is the ClientControlType  and the ClientCanKill
		// terminate client process, if it exists
		// possible kill of other closing-processes

		if (pRelay->ClientMayKillRelayProcess()) 
		{
			Lock(_T(__FUNCTION__));

			m_pCIPCOutputServer->DoRequestSetRelay(relayID,FALSE);

			for (int i=0;i<m_processes.GetSize();i++) 
			{
				CProcess *pProcess = m_processes[i];
				if (pProcess->GetMacro().GetType()==PMT_RELAY
					&& pProcess->GetCamID()==relayID
					) 
				{
					int iOldNum = m_processes.GetSize();
					TerminateProcess(pProcess);
					if (iOldNum!= m_processes.GetSize() ) 
					{
						i--;	// adjust loop
					}
				}
			}
			Unlock();
		} 
		else 
		{
			// no kill so all edge processes will keep on
			// but a client state process has to be killed
			RelayControlType controlType = pRelay->GetRelayControlType();
			if (controlType==RCT_EDGE) 
			{
				// that one is simple here, it is still controlled
				// by the closing time
			} 
			else if (controlType==RCT_STATE) 
			{
				// so there COULD be a client job for this relay
				Lock(_T(__FUNCTION__));

				m_pCIPCOutputServer->DoRequestSetRelay(relayID,FALSE);

				for (int i=0;i<m_processes.GetSize();i++) 
				{
					CProcess *pProcess = m_processes[i];
					if (   pProcess->GetClientID()==idClient
						&& pProcess->GetMacro().GetType()==PMT_RELAY
						&& pProcess->GetCamID()==relayID
						) 
					{
						int iOldNum = m_processes.GetSize();
						TerminateProcess(pProcess);
						if (iOldNum!= m_processes.GetSize() ) 
						{
							i--;	// adjust loop
						}
					}
				}
				Unlock();
			} 
			else 
			{
				// NOT YET invalid control type
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
/*@MD NYD */
void CProcessSchedulerVideoBase::HandleLocalVideo(CSecID idClient,
											  CSecID camID, 
											  Resolution res,
											  const CRect &rect,
											  BOOL bActive)
{
	Lock(_T(__FUNCTION__));

	if (m_pLocalVideoProcess==NULL) 
	{
		if (bActive) {
			// create new process
			m_pLocalVideoProcess = new CClientOverlayProcess(this,idClient,camID, rect);
			UpdateProcessListView(TRUE,m_pLocalVideoProcess);
		} else {
			// stop on non extinging process !?
		}
	} else {
		if (bActive) {
			// modify process parameter
			m_pLocalVideoProcess->Modify(camID,rect);
		} else {
			// stop process
			// NOT YET check view !?
			WK_DELETE(m_pLocalVideoProcess);
		}
	}
	// no other processes--> direct switch
	if (bActive && m_processes.GetSize()==0) {
		m_pCIPCOutputServer->DoRequestLocalVideo(camID,res,rect,bActive);
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CProcessSchedulerVideoBase::TerminateRelayProcess(CProcess *pProcess, BOOL bShutDown/*=FALSE*/)
{
	BOOL bTerminated = TRUE;
	CSecID relayID = pProcess->GetCamID();
	const COutput* pRelay = m_pCIPCOutputServer->GetOutputAt(pProcess->GetCamID().GetSubID());
	// And what about hold time at edge control? This processes must not be terminated!
	// So increase again below!
	m_iNumRelayProcesses--;
	if (m_iNumRelayProcesses<0) 
	{
		WK_TRACE_ERROR(_T("internal error, m_iNumRelayProcesses\n"));
		m_iNumRelayProcesses=0;
	}

	// the complicated case, there are more than one
	// more than one for this specific relay ?
	// if it's the last one for this relay same as simple case
	// else there will be the final one for this relay but now now
	int iNumProcessesForThisRelay = 0;

	for (int i=0;i<m_processes.GetSize();i++) 
	{
		CProcess *pPivot = m_processes[i];
		if (pPivot->IsRelayProcess()
			&& (pPivot->GetCamID()==relayID)
			) 
		{
			iNumProcessesForThisRelay++;
		}
	}	// end of loop over processes

	if (iNumProcessesForThisRelay <= 1) 
	{
		BOOL bClose = TRUE;
		CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(pProcess->GetClientID());
		if (   pClient
			&& pClient->GetIPCState() == CIPC_STATE_DISCONNECTED)
		{
			// a disconnecting client
			TRACE(_T("no more process for relay %s\n"),pRelay->GetName());
			if (	pRelay 
				&&	(pRelay->KeepClosedAfterDisconnect() && (pRelay->GetRelayControlType() == RCT_STATE)))
			{
				WK_TRACE(_T("don't close relay at disconnect %s\n"),pRelay->GetName());
				bClose = FALSE;
			}
		}
		// And what about hold time at edge control?
		if (bClose)
		{
			if (iNumProcessesForThisRelay <= 1) 
			{
				if (    (   (pProcess->GetMacro().GetID() == SECID_PROCESS_RELAY_EDGE_CONTROL)
						 || (pProcess->IsClientProcess() && (pRelay->GetRelayControlType() == RCT_EDGE))
						)
					&&	(pProcess->GetState() != SPS_TERMINATED)
					&&	!bShutDown		// If not shutdown, hold it. Otherwise a server reset should have higher priority
					)
				{
					TRACE(_T("keep relay state for hold time because of edge control %s\n"),pRelay->GetName());
					bTerminated = FALSE; // this process have to remain in the process list!
					m_iNumRelayProcesses++;
					bClose = FALSE;
				}
			}
		}
		if (bClose)
		{
			m_pCIPCOutputServer->DoRequestSetRelay(pProcess->GetCamID(),!bClose);
		}
	} 
	else 
	{
		// nothing todo
	}
	return bTerminated;
}
////////////////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::TerminateClientProcesses(CSecID idClient)
{
	if (idClient != SECID_NO_ID)
	{
		Lock(_T(__FUNCTION__));
		for (int i=m_processes.GetSize()-1;i>=0;i--) 
		{
			CProcess *pProcess = m_processes[i];
			if (pProcess->GetClientID() == idClient) 
			{
				if (pProcess->GetMacro().GetType()==PMT_RELAY) 
				{
					const COutput *pRelay = m_pCIPCOutputServer->GetOutputAt(pProcess->GetCamID().GetSubID());
					if (pRelay) 
					{
						// Flanke ist Flanke, da macht das Disconnect nix
						if (   pRelay->GetRelayControlType() == RCT_STATE
							|| !pRelay->KeepClosedAfterDisconnect()) 
						{
							TerminateProcess(pProcess);
						} 
						else 
						{
							// EDGE or NoKeepClosed
							// this process stays
							// change Client Process to Relay Process
							// OOPS possible invalid client pointer ? NOT YET
							m_iNumClientProcesses--;

							CActivation* pActivation;
							const CProcessMacro* pNewMakro = theApp.GetProcessMacros().GetMacroByID(SECID_PROCESS_RELAY_EDGE_CONTROL);
							pActivation = new CActivation(-1);
							pActivation->SetMacro(pNewMakro);
							pActivation->SetOutputID(pProcess->GetActivation()->GetOutputID());

							DWORD dwTickInit = pProcess->GetTickInit();
							pProcess->Terminate();
							WK_DELETE(pProcess);

							pProcess = new CRelayProcess(this,pActivation);
							pProcess->CalcMode(dwTickInit);
							m_processes.SetAt(i,pProcess);
							UpdateProcessListView(TRUE,pProcess);
						}
					} 
					else 
					{
						// NOT YET
					}

				} 
				else 
				{
					TerminateProcess(pProcess);	// no StopProcess, since client is gone
				}
			}
		}
		if (   m_pLocalVideoProcess 
			&& m_pLocalVideoProcess->GetClientID()==idClient) 
		{
			WK_DELETE(m_pLocalVideoProcess);
		}
		Unlock();
	}
	else
	{
		WK_TRACE_ERROR(_T("CProcessSchedulerVideo::StopClientProcesses client null\n"));
	}
}
////////////////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::OnUnitDisconnect()
{
	Lock(_T(__FUNCTION__));

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (pProcess)
		{
			pProcess->OnUnitDisconnected();
		}
	}

	Unlock();
}
////////////////////////////////////////////////////////////////////////
void CProcessSchedulerVideoBase::OnVideoSignal(CSecID id, int iErrorCode)
{
}
