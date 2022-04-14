// ProcessSchedulerAudio.cpp: implementation of the CProcessSchedulerAudio class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ProcessSchedulerAudio.h"
#include "ProcessMacro.h"
#include "AudioProcess.h"
#include "RequestCollectors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CProcessSchedulerAudio::CProcessSchedulerAudio(CIPCAudioServer *pCipc)
{
	m_pCIPCAudioServer = pCipc;
	m_bParentIsAudio = TRUE;
	m_sParentShm = pCipc->GetShmName();

	m_pRequestCollectors = new CRequestCollectors(m_pCIPCAudioServer);

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CProcessSchedulerAudio::~CProcessSchedulerAudio()
{
	if (IsRunning())
	{
		if (!StopThread())
		{
			ExitProcess(0xEEEE);
		}
	}
	WK_DELETE(m_pRequestCollectors);
}
//////////////////////////////////////////////////////////////////////
CIPC* CProcessSchedulerAudio::GetParentCIPC()
{
	return m_pCIPCAudioServer;
}
//////////////////////////////////////////////////////////////////////
BOOL CProcessSchedulerAudio::StartNewProcess(CInput *pInput, 
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

	// first call base class for common processes
	bHandled = CProcessScheduler::StartNewProcess(pInput,activation,bRefThereWasAProcess,pProcess, iNumAlreadyRecorded);

	if (!bHandled)
	{
		const CProcessMacro &processMacro = activation.GetMacro();
		Lock(_T(__FUNCTION__));
		switch (processMacro.GetType())
		{
		case PMT_AUDIO:
			if (CheckProcessForTimer(activation)) 
			{
				pProcess = new CAudioProcess(this,&activation);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
				m_pRequestCollectors->AddNewAudioRequest(pProcess);
			} 
			bHandled = TRUE;
			break;
		default:
			WK_TRACE_ERROR(_T("Could not start process type %d for %s input %s\n"),processMacro.GetType(),
				processMacro.GetName(),
				pInput->GetName()
				);
			WK_TRACE_ERROR(_T("Invalid type or non matched hardware\n"));
			break;
		}

		Unlock();

	}
	return bHandled;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcessSchedulerAudio::TerminateProcess(CProcess *pProcess, BOOL bShutDown/*=FALSE*/)
{
	m_pRequestCollectors->RemoveAudioRequest(pProcess);
	return CProcessScheduler::TerminateProcess(pProcess,bShutDown);
}
/////////////////////////////////////////////////////////////////////////////
void CProcessSchedulerAudio::TerminateClientProcesses(CSecID idClient)
{
	Lock(_T(__FUNCTION__));

	for (int i=m_processes.GetSize()-1;i>=0;i--)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (pProcess->IsClientProcess())
		{
			CAudioProcess* pAudioProcess = (CAudioProcess*)pProcess;
			if (pAudioProcess->GetClientID() == idClient)
			{
				TerminateProcess(pProcess);
			}
		}
	}

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessSchedulerAudio::Run()
{
	if (m_bShutdownInProgress)
	{
		TerminateAllProcesses();
		return;
	}
	SetRunTick();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessSchedulerAudio::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID, DWORD dwUserData)
{
	Lock(_T(__FUNCTION__));

	m_pRequestCollectors->OnIndicateMediaData(rec,secID,dwUserData);

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessSchedulerAudio::AnswerRequest(const CIPCMediaSampleRecord& rec, CSecID idMedia, DWORD pid)
{
	CProcess *pProcess = m_processes.GetByID(pid);
	
	if (pProcess) 
	{
		pProcess->Lock(_T(__FUNCTION__));
		if (   pProcess->GetState() != SPS_TERMINATED) 
		{
			pProcess->MediaData(rec,idMedia);
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
void CProcessSchedulerAudio::DoClientEncoding(CSecID idClient, 
											  CProcess*& pProcess, // CAVEAT by ref, is set in call
											  CSecID mediaID,
											  DWORD dwFlags,
											  BOOL bStart)
{
	if (pProcess == NULL)
	{
		if (bStart)
		{
			// create new process
			pProcess = new CAudioProcess(this,idClient,mediaID,dwFlags);
			m_iNumClientProcesses++;
			// pClient->GetConnection(); NOT YET priority
			AddProcess(pProcess);
			m_pRequestCollectors->AddNewAudioRequest(pProcess);
		}
		else
		{
			TRACE(_T("stop on non existing process\n"));
		}
	}
	else
	{
		if (bStart) 
		{
			// modify process parameter ???
		} 
		else 
		{
			// stop process
			m_pRequestCollectors->RemoveAudioRequest(pProcess);
			TerminateProcess(pProcess,FALSE);
			pProcess=NULL;
		}
	}
}

