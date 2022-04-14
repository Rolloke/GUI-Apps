// ProcessSchedulerTasha.cpp: implementation of the CProcessScheduler_Tasha_Q_IP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ProcessSchedulerTasha.h"
#include "ProcessMacro.h"
#include "MDProcess.h"
#include "UploadProcess.h"
#include "ClientEncodingProcess.h"
#include "RequestCollectors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CProcessScheduler_Tasha_Q_IP::CProcessScheduler_Tasha_Q_IP(CIPCOutputServer *pCipc, int iSize)
: CProcessSchedulerVideoBase(pCipc)
{
	m_bParentIsTasha = pCipc->IsTasha();
	m_bParentIsQ = pCipc->IsQ();
	m_bParentIsIP = pCipc->IsIP();

	// create a CRequestCollectors collection for each camera
	m_iSize = iSize;
	if (   ParentIsQ()
		|| ParentIsIP())
	{
		m_iSize = 1;
	}
	m_pRequestCollectors = new CRequestCollectors*[m_iSize];
	for (int i=0;i<m_iSize;i++)
	{
		m_pRequestCollectors[i] = new CRequestCollectors(m_pCIPCOutputServer);
	}

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CProcessScheduler_Tasha_Q_IP::~CProcessScheduler_Tasha_Q_IP()
{
	if (IsRunning())
	{
		if (!StopThread())
		{
			ExitProcess(0xEEEE);
		}
	}
	for (int i=0;i<m_iSize;i++)
	{
		WK_DELETE(m_pRequestCollectors[i]);
	}
	WK_DELETE_ARRAY(m_pRequestCollectors);
}
//////////////////////////////////////////////////////////////////////
CIPC* CProcessScheduler_Tasha_Q_IP::GetParentCIPC()
{
	return m_pCIPCOutputServer;
}
//////////////////////////////////////////////////////////////////////
int CProcessScheduler_Tasha_Q_IP::GetMaxNumCameraSwitches()
{
	if (ParentIsTasha())
	{
		return 200;
	}
	else if (ParentIsQ())
	{
		return 100;
	}
	else if (ParentIsIP())
	{
		return m_pCIPCOutputServer->GetSize() * 25;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
int CProcessScheduler_Tasha_Q_IP::GetMaxFps()
{
	if (ParentIsTasha())
	{
		return 200;
	}
	else if (ParentIsQ())
	{
		return 100;
	}
	else if (ParentIsIP())
	{
		return m_pCIPCOutputServer->GetSize() * 25;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
int CProcessScheduler_Tasha_Q_IP::GetRCIndex(CSecID id)
{
	if (ParentIsTasha())
	{
		return id.GetSubID();
	}
	else if (ParentIsIP() || ParentIsQ())
	{
		return 0;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::TraceRequests()
{
	CProcessScheduler::TraceRequests();
	Lock(_T(__FUNCTION__));
	if (m_pRequestCollectors)
	{
		for (int i=0;i<m_iSize;i++)
		{
			m_pRequestCollectors[i]->Trace();
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler_Tasha_Q_IP::StartNewProcess(CInput *pInput, 
											 CActivation &activation,
											 BOOL & bRefThereWasAProcess,
											 CProcess*& pProcess,
  											 int iNumAlreadyRecorded /*= -1*/)
{
	BOOL bRet = FALSE;
	bRefThereWasAProcess = FALSE;
	
	if (!CanStartNewProcess()) 
	{
		return FALSE;	// EXIT! 
	}

	pProcess = NULL;
	// first call base class for common processes
	bRet = CProcessSchedulerVideoBase::StartNewProcess(pInput,activation,bRefThereWasAProcess,pProcess,iNumAlreadyRecorded);
	if (!bRet)
	{
		const CProcessMacro &processMacro = activation.GetMacro();
		CSecID outputID = activation.GetOutputID();

		Lock(_T(__FUNCTION__));
		switch (processMacro.GetType())
		{
		case PMT_RECORD:
			if (CheckProcessForTimer(activation)) 
			{
				if (processMacro.GetID() == SECID_PROCESS_CAMERA_MD_OK)
				{
					pProcess = new CMDProcess(this,&activation,m_pCIPCOutputServer->GetCompressionType());
				}
				else
				{
					pProcess = new CRecordingProcess(this,&activation,m_pCIPCOutputServer->GetCompressionType(),iNumAlreadyRecorded);
				}
				bRefThereWasAProcess = TRUE;

				AddProcess(pProcess);
				m_iNumStoringProcesses++;
				if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
				{
					m_pRequestCollectors[GetRCIndex(outputID)]->AddNewVideoRequest(pProcess);
				}
			}
			bRet = TRUE;
			break;
		case PMT_UPLOAD:
			if (CheckProcessForTimer(activation)) 
			{
				pProcess = new CUploadProcess(this,&activation,m_pCIPCOutputServer->GetCompressionType(),iNumAlreadyRecorded);
				bRefThereWasAProcess = TRUE;
				AddProcess(pProcess);
				m_iNumStoringProcesses++;
				if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
				{
					m_pRequestCollectors[GetRCIndex(outputID)]->AddNewVideoRequest(pProcess);
				}
			}
			bRet = TRUE;
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

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler_Tasha_Q_IP::TerminateProcess(CProcess *pProcess, BOOL bShutDown/*=FALSE*/)
{
	CSecID outputID = pProcess->GetCamID();
	if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
	{
		m_pRequestCollectors[GetRCIndex(outputID)]->RemoveVideoRequest(pProcess);
	}
	return CProcessScheduler::TerminateProcess(pProcess,bShutDown);
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::TerminateClientProcesses(CSecID idClient)
{
	Lock(_T(__FUNCTION__));

	for (int i=m_processes.GetSize()-1;i>=0;i--)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (pProcess->IsClientProcess())
		{
			CClientProcess* pClientProcess = (CClientProcess*)pProcess;
			if (pClientProcess->GetClientID() == idClient)
			{
				TerminateProcess(pProcess);
			}
		}
	}

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::RemoveRequest(CProcess* pProcess)
{
	m_pRequestCollectors[GetRCIndex(pProcess->GetCamID())]->RemoveVideoRequest(pProcess);
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::AddRequest(CProcess* pProcess)
{
	m_pRequestCollectors[GetRCIndex(pProcess->GetCamID())]->AddNewVideoRequest(pProcess);
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::Run()
{
	if (m_bShutdownInProgress)
	{
		TerminateAllProcesses();
		return;
	}
	if (PreWaitSecondGrid())
	{
		return;
	}

	// loop run over all processes
	CProcessScheduler::Run();

	PostWaitSecondGrid();

	CheckProcessRequests();
	CheckRequests();
	CheckClientTimeout();

	m_dwLastGridTick = m_dwCurrentTime;
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::CheckClientTimeout()
{
	Lock(_T(__FUNCTION__));

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (   pProcess
			&& pProcess->IsClientEncodingProcess())
		{
			CClientEncodingProcess* pCEP = (CClientEncodingProcess*)pProcess;
			if (pCEP->IsPictureTimeout())
			{
				pCEP->ResetCounter();
			}
		}
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::CheckRequests()
{
	Lock(_T(__FUNCTION__));
	for (int i=0;i<m_iSize;i++)
	{
		m_pRequestCollectors[i]->CheckForTimeout();
		m_pRequestCollectors[i]->CheckProcesses();
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::CheckProcessRequests()
{
	Lock(_T(__FUNCTION__));

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (   pProcess
			&& (   pProcess->IsMDProcess()
			    || pProcess->IsClientEncodingProcess()
				)
			)
		{
			CSecID camID = pProcess->GetCamID();
			DWORD dwRequestID = ParentIsTasha() ? 0 : camID.GetSubID();
			CRequestCollectors* pRequestCollectors = m_pRequestCollectors[GetRCIndex(camID)];
			int c = pRequestCollectors->GetSize();

			for (int j=0;j<c;j++)
			{
				CRequestCollector* pRequestCollector = pRequestCollectors->GetAtFast(j);
				if (   pRequestCollector
					&& pRequestCollector->GetID() == dwRequestID)
				{
					if (!pRequestCollector->FindProcess(pProcess->GetID().GetID()))
					{
						WK_TRACE(_T("%s did not find request for process %08lx\n"),
							GetParentCIPC()->GetShmName(),
							camID.GetID());
						pRequestCollectors->AddNewVideoRequest(pProcess);
					}
				}
			}
		}
	}

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::OnIndicateVideo(const CIPCPictureRecord& pict,
											DWORD dwMDX,
											DWORD dwMDY,
											DWORD dwUserData)
{
	m_csLastPicture.Lock();
	m_dwLastPicture = WK_GetTickCount();
	m_csLastPicture.Unlock();

	Lock(_T(__FUNCTION__));

	CSecID outputID = pict.GetCamID();
	if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
	{
		m_pRequestCollectors[GetRCIndex(outputID)]->OnIndicateVideo(pict,dwMDX,dwMDY,dwUserData);
	}

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::AnswerRequest(const CIPCPictureRecord &pict,
											DWORD dwMDX,
											DWORD dwMDY,
											DWORD pid,
											CRequestCollector* pRequest)
{
	//TRACE(_T("AnswerRequest %08lx, %08lx %d %s,%03d %dfps\n"),pid,pict.GetCamID().GetID(),pict.GetBlockNr(),	pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds(),pRequest->GetNumPics());

	CProcess *pProcess = m_processes.GetByID(pid);

	if (pProcess) 
	{
		pProcess->Lock(_T(__FUNCTION__));
		if (pProcess->GetState() != SPS_TERMINATED) 
		{
			int iProcessFPS = pProcess->GetMacro().GetPicsPerSlice();
			BOOL bCallPictureData = FALSE;
			BOOL bCropIFrame = FALSE;
			
			// drop some tasha images if not all requested
			if (iProcessFPS >= pRequest->GetNumPics())
			{
				// the easy case deliver all
				bCallPictureData = TRUE;
			}
			else
			{
				bCropIFrame = TRUE;
				if (   pict.GetPictureType() == SPT_FULL_PICTURE
					|| pict.GetPictureType() == SPT_GOP_PICTURE)
				{
					// it's an I-Frame
					DWORD dwIFramesToDrop = 0;
					if (ParentIsTasha())
					{
						dwIFramesToDrop = (pRequest->GetNumPics()/pRequest->GetIFrame())-iProcessFPS;
					}
					else if (ParentIsQ())
					{
						dwIFramesToDrop = (pRequest->GetNumPics() / iProcessFPS) - 1;
					}

					bCallPictureData = pProcess->m_dwDropCounter == 0;
					if (bCallPictureData)
					{
						pProcess->m_dwDropCounter = dwIFramesToDrop;
					}
					else
					{
						if (pProcess->m_dwDropCounter>0) pProcess->m_dwDropCounter--;
					}
					//TRACE(_T("dwIFramesToDrop=%d m_dwDropCounter=%d\n"),dwIFramesToDrop,pProcess->m_dwDropCounter);
				}
				else
				{
					// always drop P-Frames
				}
			}

			if (bCallPictureData)
			{
				if (   bCropIFrame
					&& pict.GetPictureType() == SPT_GOP_PICTURE)
				{
					// send only the first I-frame
					CIPCPictureRecord pict1(m_pCIPCOutputServer,
											pict.GetData(0),
											pict.GetDataLength(0),
											pict.GetCamID(),
											pict.GetResolution(),
											pict.GetCompression(),
											pict.GetCompressionType(),
											pict.GetTimeStamp(),
											pict.GetJobTime());
					pProcess->PictureData(pict1,dwMDX,dwMDY,pRequest->GetID());
				}
				else
				{
					pProcess->PictureData(pict,dwMDX,dwMDY,pRequest->GetID());
				}
			}
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
		TRACE(_T("PictDataWithoutProcess %s %08lx\n"),m_pCIPCOutputServer->GetShmName(),pid);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::DoClientEncoding(CSecID idClient, 
											  CProcessPtr &pProcess, // CAVEAT by ref, is set in call
											  CSecID outputID,
											  Resolution res, 
											  Compression comp, 
											  CompressionType ct,
											  int iNumPictures,
											  DWORD dwTimeSlice,
											  DWORD dwBitrate)
{
	if (pProcess == NULL)
	{
		if (iNumPictures>0)
		{
			if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
			{
				// create new process
				pProcess = new CClientEncodingProcess(this,idClient,outputID,res,comp,ct,1000,iNumPictures);
				m_iNumClientProcesses++;
				// pClient->GetConnection(); NOT YET priority
				AddProcess(pProcess);
				m_pRequestCollectors[GetRCIndex(outputID)]->AddNewVideoRequest(pProcess);
			}
		}
		else
		{
			TRACE(_T("stop on non existing process\n"));
		}
	}
	else
	{
		if (pProcess->GetCamID() != outputID)
		{
			WK_TRACE_ERROR(_T("DoClientEncoding wrong cam ID %08lx,%s\n"),outputID.GetID(),pProcess->Format());
		}
		if (iNumPictures>0) 
		{
			// modify process parameter ???
			BOOL bModified = pProcess->Modify(outputID,res,comp,iNumPictures,dwTimeSlice,dwBitrate);
			if (   bModified
				|| dwBitrate<=128*1024)
			{
				if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
				{
					m_pRequestCollectors[GetRCIndex(outputID)]->ActualizeRequest(pProcess);
				}
			}
		} 
		else 
		{
			// stop process
			if (outputID.GetSubID()<m_pCIPCOutputServer->GetSize())
			{
				m_pRequestCollectors[GetRCIndex(outputID)]->RemoveVideoRequest(pProcess);
				TerminateProcess(pProcess,FALSE);
				pProcess=NULL;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcessScheduler_Tasha_Q_IP::OnVideoSignal(CSecID id, int iErrorCode)
{
	Lock(_T(__FUNCTION__));

	if (id.GetSubID()<m_pCIPCOutputServer->GetSize())
	{
		for (int i=0;i<m_processes.GetSize();i++)
		{
			CProcess* pProcess = m_processes.GetAtFast(i);
			if (   pProcess
				&& (pProcess->IsClientEncodingProcess() || pProcess->IsRecordingProcess())
				&& pProcess->GetCamID() == id)
			{
				if (iErrorCode == 0)
				{
					// todo camera wech
					pProcess->OnUnitDisconnected();
//					m_pRequestCollectors[GetRCIndex(id)]->RemoveRequest(pProcess);
				}
				else if (iErrorCode == 1)
				{
					// todo camera wieder da
//					m_pRequestCollectors[GetRCIndex(id)]->AddVideoRequest(pProcess);
				}
			}
		}
	}

	Unlock();
}

