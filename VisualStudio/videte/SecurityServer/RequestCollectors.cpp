/* GlobalReplace: Optimiziation --> Optimization */
/* GlobalReplace: TraceOptimize --> TraceRequestOptimization */
// RequestCollector.cpp: implementation of the CRequestCollector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "sec3.h"
#include "RequestCollectors.h"
#include "Process.h"
#include "ProcessScheduler.h"	// only for trace option
#include "ProcessSchedulerVideo.h"	// only for trace option
#include "ClientEncodingProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// static members (configuration/debug toggles)
BOOL CRequestCollectors::ms_bDisableRequestOptimization= FALSE;
BOOL CRequestCollectors::ms_bDisableRequestMultiplexing= FALSE;
BOOL CRequestCollectors::ms_bTraceRequestOptimization = FALSE;
BOOL CRequestCollectors::ms_bTraceRequestMultiplexing = FALSE;


/////////////////////////////////////////////////////////////////////////////////
CRequestCollectors::CRequestCollectors(CIPCOutputServer *pParent)
{
	CWK_Profile wkp;
	
	ASSERT(pParent);
	m_pCIPCOutputServer = pParent;
	m_pCIPCAudioServer = NULL;
	m_iNumTotalPicturesRequested = 0;
	m_iNumTotalCameras = 0;

	m_iFps = 0;
	m_iNumCameraSwitches = 0;

	m_csFpsTasha.Lock();
	m_iFpsTasha[0] = 0;
	m_iFpsTasha[1] = 0;
	m_csFpsTasha.Unlock();


	if (pParent->IsTasha())
	{
	}
	else if (pParent->IsJaCob() || pParent->IsSaVic())
	{
		if (m_pCIPCOutputServer)
		{
			m_iFps = m_pCIPCOutputServer->GetProcessSchedulerVideo()->GetMaxFps();
			m_iNumCameraSwitches = m_pCIPCOutputServer->GetProcessSchedulerVideo()->GetMaxNumCameraSwitches();
		}
	}
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
/////////////////////////////////////////////////////////////////////////////////
CRequestCollectors::CRequestCollectors(CIPCAudioServer *pParent)
{
	ASSERT(pParent);
	m_pCIPCOutputServer = NULL;
	m_pCIPCAudioServer = pParent;
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif

	Lock(_T(__FUNCTION__));
	for (int i=0;i<m_pCIPCAudioServer->GetSize();i++)
	{
		CRequestCollector* pRC = new CRequestCollector(this,NULL,0);
		pRC->SetCamID(m_pCIPCAudioServer->GetMediaAt(i)->GetID());
		CSecID id;
		id.Set(GetIOGroup()->GetGroupID(),CRequestCollector::ms_wSubIDCounter++);
		pRC->SetID(id.GetID());
		Add(pRC);
	}
	Unlock();
}
////////////////////////////////////////////////////////
CRequestCollectors::~CRequestCollectors()
{
	Lock(_T(__FUNCTION__));
	if (GetSize())
	{
		SafeDeleteAll();
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////////
int CRequestCollectors::AddNewRequest(CProcess *pProcess, int iNumPicsIn)
{
	int iNumPicsUsed=0;
	// here is the place to do the multiplexing
	// int iNumPics = iNumPicsIn;	// might be modifed, by some split action
	// check if it fits into an existing request
	// returns how many pics are really used
	
	BOOL bDone=FALSE;

	Lock(_T(__FUNCTION__));

	for (int i=0;i<GetSize() && bDone==FALSE && ms_bDisableRequestMultiplexing==FALSE;i++) 
	{
		CRequestCollector *pRequest = GetAtFast(i);
		if (pRequest->IsSimilarRequest(pProcess)) 
		{	
			// same parameter ?
			if (pRequest->GetNumPics() > iNumPicsIn) 
			{
				// split large existing request
				// create a second request
				// and modify the counts
				CRequestCollector *pTmp = new CRequestCollector(*pRequest);
				InsertAt(i+1,pTmp);
				// the new request has the requested pictures
				// the existing request is reduced by the count of the newly adde
				pTmp->SetNumPics(iNumPicsIn);
				pTmp->AddProcessID(pProcess->GetID().GetID());
				pRequest->SetNumPics(pRequest->GetNumPics() - iNumPicsIn);	// the pics are also in pTmp
				bDone=TRUE;
			} 
			else if (pRequest->GetNumPics() < iNumPicsIn) 
			{
				// the existing request is smaller
				// complicated split, insert a new request
				CRequestCollector *pTmp = new CRequestCollector(*pRequest);
				InsertAt(i+1,pTmp);
				// do not count the pictures from the existing request
				pTmp->SetNumPics(iNumPicsIn-pRequest->GetNumPics());
				// it only has the newly created process
				pTmp->RemoveAllProcessIDs();	// OOPS
				pTmp->AddProcessID(pProcess->GetID().GetID());
				// but also add to the existing request
				pRequest->AddProcessID(pProcess->GetID().GetID());
				bDone=TRUE;
			} 
			else 
			{	// equal numPics;
				pRequest->AddProcessID(pProcess->GetID().GetID());
				bDone=TRUE;
			}
		} 
		else 
		{
			// different parameter
		}
	}
	Unlock();

	// if multiplexing is disabled, bDone will be FALSE
	if (bDone==FALSE) 
	{
		// insert a new request
		CRequestCollector *pRequest = new CRequestCollector(this, pProcess,iNumPicsIn);
		iNumPicsUsed = iNumPicsIn;
		SafeAdd(pRequest);
	} 
	else 
	{
		if (ms_bTraceRequestMultiplexing) 
		{
			CString sTmp;
			sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
			WK_STAT_PEAK(_T("Requests"),iNumPicsIn,sTmp);
		}
	}

	if (iNumPicsUsed) 
	{
		m_iNumTotalPicturesRequested += iNumPicsUsed;
		m_iNumTotalCameras++;
	}

	return iNumPicsUsed;
}
/////////////////////////////////////////////////////////////////////////
int CRequestCollectors::AddNewAudioRequest(CProcess *pProcess)
{
	int iRet = 0;

	if (pProcess->IsAudioProcess())
	{
		Lock(_T(__FUNCTION__));
		for (int i=0;i<GetSize();i++) 
		{
			CRequestCollector *pRequest = GetAtFast(i);
			if (pRequest->IsSimilarRequest(pProcess)) 
			{	
				pRequest->AddProcessID(pProcess->GetID().GetID());

				if (CProcessScheduler::m_bTraceEncodeRequests)
				{
					WK_TRACE(_T("DoRequestStartMediaEncoding %08lx,%08lx\n"),
						pRequest->GetCamID().GetID(),pRequest->GetID());
				}
				m_pCIPCAudioServer->DoRequestStartMediaEncoding(pRequest->GetCamID(),
																MEDIA_ENC_NEW_SEGMENT|MEDIA_ENC_DELIVER_ALWAYS,
																pRequest->GetID());
				iRet = pRequest->GetNrOfProcesses();
				if (ms_bTraceRequestMultiplexing)
				{
					CString sTmp;
					sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
					WK_STAT_PEAK(_T("Requests"),iRet,sTmp);
				}
				break;
			}
		}
		Unlock();
	}
	return iRet;
}
/////////////////////////////////////////////////////////////////////////
int CRequestCollectors::AddNewVideoRequest(CProcess *pProcess)
{
	int iRet = 0;
	if (m_pCIPCOutputServer->IsTasha())
	{
		// TODO Tasha Scheduling
		BOOL bFound = FALSE;
		BOOL bDoRequest = FALSE;
		CRequestCollector* pRequest = NULL;

		Lock(_T(__FUNCTION__));

		for (int i=0;i<GetSize() && !bFound;i++) 
		{
			pRequest = GetAtFast(i);
			if (pRequest->GetCamID() == pProcess->GetCamID())
			{
				if (pRequest->GetID() == 0)
				{
					// high quality storage and net
					if (pProcess->IsClientEncodingProcess())
					{
						CClientEncodingProcess* pClientEncodingProcess = (CClientEncodingProcess*)pProcess;
						if (pClientEncodingProcess->GetClientBitrate()>128*1024)
						{
							bFound = TRUE;
						}
					}
					else if (pProcess->IsRecordingProcess())
					{
						bFound = TRUE;
					}
				}
				else if (pRequest->GetID() == 1)
				{
					// low bandwidth ISDN/DSL/IP-Router
					if (pProcess->IsClientEncodingProcess())
					{
						CClientEncodingProcess* pClientEncodingProcess = (CClientEncodingProcess*)pProcess;
						if (pClientEncodingProcess->GetClientBitrate()<=128*1024)
						{
							bFound = TRUE;
						}
					}
				}
			}
		}
		Unlock();

		if (bFound)
		{
			// already running request 
			// simply add the process id
			int iMaxFPS = 25;

			m_csFpsTasha.Lock();
			if (pRequest->GetID()==0)
			{
				if (m_iFpsTasha[1]>0)
				{
					iMaxFPS /=2;
				}
			}
			m_csFpsTasha.Unlock();

//			TRACE(_T("add CRequestCollector ID%d %08lx\n"),pRequest->GetID(),pRequest->GetCamID().GetID());
			pRequest->AddProcessID(pProcess->GetID().GetID());
			// TODO calc new fps,res,comp
			bDoRequest = pRequest->CalcTashaRequests(iMaxFPS);
			if (pRequest->GetNrOfProcesses() == 1)
			{
				bDoRequest = TRUE;
			}
		}
		else
		{
			// new request
			DWORD dwID = 0;
			if (pProcess->IsClientEncodingProcess())
			{
				CClientEncodingProcess* pClientEncodingProcess = (CClientEncodingProcess*)pProcess;
				if (pClientEncodingProcess->GetClientBitrate()<=128*1024)
				{
					dwID = 1;
				}
			}
			pRequest = new CRequestCollector(this,pProcess,pProcess->GetMacro().GetNumPictures(),dwID);
			//TRACE(_T("new CRequestCollector ID%d %08lx\n"),dwID,pRequest->GetCamID().GetID());
			SafeAdd(pRequest);
			
			// check previous requests for decreasing framerate!
			CheckDecreaseFramerate();
			bDoRequest = TRUE;
		}

		if (bDoRequest)
		{
			//TRACE(_T("DoTashaRequest TRUE %d, %s\n"),pRequest->GetID(),NameOfEnum(pProcess->GetMacro().GetType()));
			DoTashaRequest(pRequest,TRUE);
		}

		iRet = pRequest->GetNrOfProcesses();
		if (ms_bTraceRequestMultiplexing)
		{
			CString sTmp;
			sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
			WK_STAT_PEAK(_T("Requests"),iRet,sTmp);
		}
	}
	else if (m_pCIPCOutputServer->IsQ())
	{
		// TODO Q Scheduling
		BOOL bFound = FALSE;
		BOOL bDoRequest = FALSE;
		CRequestCollector* pRequest = NULL;

		Lock(_T(__FUNCTION__));

		for (int i=0;i<GetSize() && !bFound;i++) 
		{
			pRequest = GetAtFast(i);
			bFound = pRequest->GetCamID() == pProcess->GetCamID();
		}
		if (bFound)
		{
			pRequest->AddProcessID(pProcess->GetID().GetID());
			if (pRequest->GetNumPics()<(int)pProcess->GetMacro().GetNumPictures())
			{
				pRequest->SetNumPics(pProcess->GetMacro().GetNumPictures());
				bDoRequest = TRUE;
			}
			if (pRequest->GetNrOfProcesses() == 1)
			{
				bDoRequest = TRUE;
			}
		}
		else
		{
			pRequest = new CRequestCollector(this,pProcess,
											 pProcess->GetMacro().GetNumPictures(),
											 pProcess->GetCamID().GetSubID());
			//TRACE(_T("new Q CRequestCollector ID%d %08lx\n"),pRequest->GetID(),pRequest->GetCamID().GetID());
			SafeAdd(pRequest);
			bDoRequest = TRUE;
		}
		CalcQRequests(TRUE);
		if (bDoRequest)
		{
			//TRACE(_T("DoQRequest TRUE %d, %s\n"),pRequest->GetID(),NameOfEnum(pProcess->GetMacro().GetType()));
			DoQRequest(pRequest,TRUE);
		}

		iRet = pRequest->GetNrOfProcesses();

		Unlock();
	}
	else if (m_pCIPCOutputServer->IsIP())
	{
		// TODO IP Scheduling
		BOOL bFound = FALSE;
		BOOL bDoRequest = FALSE;
		CRequestCollector* pRequest = NULL;

		Lock(_T(__FUNCTION__));

		for (int i=0;i<GetSize() && !bFound;i++) 
		{
			pRequest = GetAtFast(i);
			bFound = pRequest->GetCamID() == pProcess->GetCamID();
		}

		if (bFound)
		{
			pRequest->AddProcessID(pProcess->GetID().GetID());
			if (pRequest->GetNumPics()<(int)pProcess->GetMacro().GetNumPictures())
			{
				pRequest->SetNumPics(pProcess->GetMacro().GetNumPictures());
				bDoRequest = TRUE;
			}
			if (pRequest->GetNrOfProcesses() == 1)
			{
				bDoRequest = TRUE;
			}
		}
		else
		{
			pRequest = new CRequestCollector(this,pProcess,pProcess->GetMacro().GetNumPictures(),pProcess->GetCamID().GetSubID());
			//TRACE(_T("new IP CRequestCollector ID%d %08lx\n"),pRequest->GetID(),pRequest->GetCamID().GetID());
			SafeAdd(pRequest);
			bDoRequest = TRUE;
		}
		if (bDoRequest)
		{
			DoIPRequest(pRequest,TRUE);
		}

		iRet = pRequest->GetNrOfProcesses();

		Unlock();
	}

	return iRet;
}
/////////////////////////////////////////////////////////////////////////
void CRequestCollectors::CalcQRequests(BOOL bDecreaseFPS)
{
	int iFPSAll = 0;
	CRequestCollector* pRequest = NULL;

	Lock(_T(__FUNCTION__));

	for (int i=0;i<GetSize();i++) 
	{
		pRequest = GetAtFast(i);
		if (pRequest->GetResolution() == RESOLUTION_4CIF)
		{
			iFPSAll += pRequest->GetNumPics() * 2;
		}
		else
		{
			iFPSAll += pRequest->GetNumPics();
		}
	}
	if (bDecreaseFPS)
	{
		if (iFPSAll > 100)
		{
			// TODO UF reduce requests look at priorities
			// TRACE(_T("Q Req %d %d fps\n"),GetSize(),iFPSAll);
			for (int i=0;i<GetSize();i++) 
			{
				pRequest = GetAtFast(i);
				if (pRequest->GetNumPics()>1)
				{
					int iOldNumPics = pRequest->GetNumPics();
					int iNewNumPics = MulDiv(iOldNumPics,100,iFPSAll);
					//TRACE(_T("reduce fps %d->%d %08lx ?\n"),iOldNumPics,iNewNumPics,pRequest->GetCamID().GetID());
					if (   iNewNumPics>0
						&& iNewNumPics != iOldNumPics)
					{
						//TRACE(_T("reduce fps %d->%d %08lx\n"),iOldNumPics,iNewNumPics,pRequest->GetCamID().GetID());
						pRequest->SetNumPics(iNewNumPics);
						DoQRequest(pRequest,TRUE);
					}
				}
			}
		}
	}
	else
	{
		if (iFPSAll<100)
		{
			// eventuell wieder hochschalten ?
			for (int i=0;i<GetSize();i++) 
			{
				pRequest = GetAtFast(i);
				for (int j=0;j<pRequest->GetNrOfProcesses();j++)
				{
					DWORD dwPID = pRequest->GetProcessID(j);
					CProcess* pProcess = m_pCIPCOutputServer->GetProcessScheduler()->GetProcess(dwPID);
					if (   pProcess
						&& pProcess->GetActivation())
					{
						int iProcessFPS = pProcess->GetActivation()->GetMacro().GetNumPictures();
						if (iProcessFPS>pRequest->GetNumPics())
						{
							// hoch
							int iNewNumPics = MulDiv(pRequest->GetNumPics(),100,iFPSAll);
							iNewNumPics = min(iProcessFPS,iNewNumPics);
							pRequest->SetNumPics(iNewNumPics);
							DoQRequest(pRequest,TRUE);
						}
					}
				}
			}
		}
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////
int CRequestCollectors::ActualizeRequest(CProcess *pProcess)
{
	int iRet = 0;

	// TODO change params Stream 0, new request Stream 1
	BOOL bFound = FALSE;
	CRequestCollector* pRequest = NULL;

	Lock(_T(__FUNCTION__));

	for (int i=0;i<GetSize() && !bFound;i++) 
	{
		pRequest = GetAtFast(i);
		if (pRequest->GetCamID().GetID() == pProcess->GetCamID().GetID())
		{
			bFound = pRequest->FindProcess(pProcess->GetID().GetID());
		}
	}
	Unlock();

	if (bFound)
	{
		BOOL bRequest = FALSE;
		if (m_pCIPCOutputServer->IsTasha())
		{
			if (pRequest->GetID() == 0)
			{
				bRequest = pRequest->ActualizeTashaRequest(pProcess);
			}
			else if (pRequest->GetID() == 1)
			{
				int iMinCounter = pRequest->GetMinFrameCounter();
				TRACE(_T("iMinCounter = %d\n"),iMinCounter);
				bRequest = pRequest->ActualizeTashaRequest(pProcess) || (iMinCounter>=0);

				if (   bRequest
					&& pRequest->IsTimedOut())
				{
					DoTashaRequest(pRequest,FALSE);
				}
			}

			if (bRequest)
			{
				DoTashaRequest(pRequest,TRUE);
			}
		}
		else if(m_pCIPCOutputServer->IsQ())
		{
			bRequest = pRequest->ActualizeQRequest(pProcess);
			if (bRequest)
			{
				CalcQRequests(TRUE);
				DoQRequest(pRequest,TRUE);
			}
		}
		else if(m_pCIPCOutputServer->IsIP())
		{
			bRequest = pRequest->ActualizeQRequest(pProcess);
			if (bRequest)
			{
				DoIPRequest(pRequest,TRUE);
			}
		}
	}

	return iRet;
}
/////////////////////////////////////////////////////////////////////////
void CRequestCollectors::DoRequest(CRequestCollector* pRequest, BOOL bStart)
{
	if (m_pCIPCOutputServer)
	{
		if (m_pCIPCOutputServer->IsTasha())
		{
			DoTashaRequest(pRequest,bStart);
		}
		else if (m_pCIPCOutputServer->IsQ())
		{
			DoQRequest(pRequest,bStart);
		}
		else if (m_pCIPCOutputServer->IsIP())
		{
			DoIPRequest(pRequest,bStart);
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CRequestCollectors::DoTashaRequest(CRequestCollector* pRequest,BOOL bStart)
{
	if (bStart)
	{
		m_csFpsTasha.Lock();
		if (   (pRequest->GetID() == 1)
			|| (m_iFpsTasha[pRequest->GetID()] != pRequest->GetNumPics())
			)
		{
			if (CProcessScheduler::m_bTraceEncodeRequests)
			{
				WK_TRACE(_T("DoTashaRequest %08lx, %s, %s %d,%d id=%d\n"),
					pRequest->GetCamID().GetID(),
					NameOfEnum(pRequest->GetResolution()),
					NameOfEnum(pRequest->GetCompression()),
					pRequest->GetNumPics(),
					pRequest->GetIFrame(),
					pRequest->GetID());
			}
			m_pCIPCOutputServer->DoRequestStartVideo(pRequest->GetCamID(),
													pRequest->GetResolution(),
													pRequest->GetCompression(),
													pRequest->GetCompressionType(),
													pRequest->GetNumPics(),
													pRequest->GetIFrame(),
													pRequest->GetID());
			m_iFpsTasha[pRequest->GetID()] = pRequest->GetNumPics();
		}
		m_csFpsTasha.Unlock();
		pRequest->ResetTimeStamp();
		
	}
	else
	{
		if (CProcessScheduler::m_bTraceEncodeRequests)
		{
			WK_TRACE(_T("DoTashaRequest Stop %08lx,%08lx\n"),
				pRequest->GetCamID().GetID(),pRequest->GetID());
		}
		m_csFpsTasha.Lock();
		m_pCIPCOutputServer->DoRequestStopVideo(pRequest->GetCamID(),pRequest->GetID());
		m_iFpsTasha[pRequest->GetID()] = 0;
		m_csFpsTasha.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////
void CRequestCollectors::DoQRequest(CRequestCollector* pRequest,BOOL bStart)
{
	if (bStart)
	{
		if (CProcessScheduler::m_bTraceEncodeRequests)
		{
			WK_TRACE(_T("DoQRequest %08lx, %s, %s %d,%d id=%d\n"),
				pRequest->GetCamID().GetID(),
				NameOfEnum(pRequest->GetResolution()),
				NameOfEnum(pRequest->GetCompression()),
				pRequest->GetNumPics(),
				pRequest->GetIFrame(),
				pRequest->GetID());
		}
		m_pCIPCOutputServer->DoRequestStartVideo(pRequest->GetCamID(),
			pRequest->GetResolution(),
			pRequest->GetCompression(),
			pRequest->GetCompressionType(),
			pRequest->GetNumPics(),
			pRequest->GetIFrame(),
			pRequest->GetID());
		pRequest->ResetTimeStamp();
	}
	else
	{
		if (CProcessScheduler::m_bTraceEncodeRequests)
		{
			WK_TRACE(_T("DoQRequest Stop %08lx,%08lx\n"),
				pRequest->GetCamID().GetID(),pRequest->GetID());
		}
		m_pCIPCOutputServer->DoRequestStopVideo(pRequest->GetCamID(),pRequest->GetID());
	}
}
/////////////////////////////////////////////////////////////////////////
void CRequestCollectors::DoIPRequest(CRequestCollector* pRequest, BOOL bStart)
{
	if (bStart)
	{
		if (CProcessScheduler::m_bTraceEncodeRequests)
		{
			WK_TRACE(_T("DoIPRequest %08lx, %s, %s %d,%d id=%d\n"),
				pRequest->GetCamID().GetID(),
				NameOfEnum(pRequest->GetResolution()),
				NameOfEnum(pRequest->GetCompression()),
				pRequest->GetNumPics(),
				pRequest->GetIFrame(),
				pRequest->GetID());
		}
		m_pCIPCOutputServer->DoRequestStartVideo(pRequest->GetCamID(),
			pRequest->GetResolution(),
			pRequest->GetCompression(),
			pRequest->GetCompressionType(),
			pRequest->GetNumPics(),
			pRequest->GetIFrame(),
			pRequest->GetID());
		pRequest->ResetTimeStamp();
	}
	else
	{
		if (CProcessScheduler::m_bTraceEncodeRequests)
		{
			WK_TRACE(_T("DoIPRequest Stop %08lx,%08lx\n"),
				pRequest->GetCamID().GetID(),pRequest->GetID());
		}
		m_pCIPCOutputServer->DoRequestStopVideo(pRequest->GetCamID(),pRequest->GetID());
	}
}
/////////////////////////////////////////////////////////////////////////
int CRequestCollectors::RemoveAudioRequest(CProcess *pProcess)
{
	int iRet = 0;
	if (pProcess->IsAudioProcess())
	{
		Lock(_T(__FUNCTION__));
		for (int i=0;i<GetSize();i++) 
		{
			CRequestCollector *pRequest = GetAtFast(i);

			if (pRequest->IsSimilarRequest(pProcess)) 
			{	
				pRequest->RemoveProcessID(pProcess->GetID().GetID());

				if (pRequest->GetNrOfProcesses() == 0)
				{
					if (CProcessScheduler::m_bTraceEncodeRequests)
					{
						WK_TRACE(_T("DoRequestStopMediaEncoding %08lx,%08lx\n"),
							pRequest->GetCamID().GetID(),pRequest->GetID());
					}
					m_pCIPCAudioServer->DoRequestStopMediaEncoding(pRequest->GetCamID(),0);
				}
				if (ms_bTraceRequestMultiplexing)
				{
					CString sTmp;
					sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
					WK_STAT_PEAK(_T("Requests"),pRequest->GetNrOfProcesses(),sTmp);
				}

				break;
			}
		}
	
		Unlock();
	}
	return iRet;
}
/////////////////////////////////////////////////////////////////////////
int CRequestCollectors::RemoveVideoRequest(CProcess *pProcess)
{
	int iRet = 0;
	if (m_pCIPCOutputServer->IsTasha())
	{
		// TODO Tasha Scheduling
		CRequestCollector* pRequest = NULL;
		BOOL bIncreaseFrameRate = FALSE;

		Lock(_T(__FUNCTION__));

		for (int i=0;i<GetSize();i++)
		{
			pRequest = GetAtFast(i);
			if (pRequest->RemoveProcessID(pProcess->GetID().GetID()))
			{
//				TRACE(_T("rem CRequestCollector ID%d %08lx\n"),pRequest->GetID(),pRequest->GetCamID().GetID());
				if (pRequest->GetNrOfProcesses() == 0)
				{
					if (   pProcess->IsMDProcess()
						|| (   pProcess->IsRecordingProcess()
						    && !pProcess->IsClientProcess()
							&& pProcess->GetAlarmInput()
						    && pProcess->GetAlarmInput()->IsMD()
							)
						)
					{
					}
					else
					{
						//TRACE(_T("DoTashaRequest FALSE %d, %s\n"),pRequest->GetID(),NameOfEnum(pProcess->GetMacro().GetType()));
						DoTashaRequest(pRequest,FALSE);
					}
					bIncreaseFrameRate = pRequest->GetID() == 1;
				}
				if (ms_bTraceRequestMultiplexing)
				{
					CString sTmp;
					sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
					WK_STAT_PEAK(_T("Requests"),pRequest->GetNrOfProcesses(),sTmp);
				}
				break;
			}
		}
		
		Unlock();
		// check previous requests for increase framerate!
		if (bIncreaseFrameRate)
		{
			CheckIncreaseFramerate();
		}
	}
	else if (m_pCIPCOutputServer->IsQ())
	{
		CRequestCollector* pRequest = NULL;
		BOOL bIncreaseFrameRate = FALSE;

		Lock(_T(__FUNCTION__));

		for (int i=0;i<GetSize();i++)
		{
			pRequest = GetAtFast(i);
			if (pRequest->RemoveProcessID(pProcess->GetID().GetID()))
			{
				//	TRACE(_T("rem CRequestCollector ID%d %08lx\n"),pRequest->GetID(),pRequest->GetCamID().GetID());
				bIncreaseFrameRate = TRUE;
				if (pRequest->GetNrOfProcesses() == 0)
				{
					// TRACE(_T("DoQRequest FALSE %d, %s\n"),pRequest->GetID(),NameOfEnum(pProcess->GetMacro().GetType()));
					DoQRequest(pRequest,FALSE);
				}
				if (ms_bTraceRequestMultiplexing)
				{
					CString sTmp;
					sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
					WK_STAT_PEAK(_T("Requests"),pRequest->GetNrOfProcesses(),sTmp);
				}
				break;
			}
		}

		Unlock();
		
		// check previous requests for increase framerate!
		if (bIncreaseFrameRate)
		{
			CalcQRequests(FALSE);
		}
	}
	else if (m_pCIPCOutputServer->IsIP())
	{
		CRequestCollector* pRequest = NULL;

		Lock(_T(__FUNCTION__));

		for (int i=0;i<GetSize();i++)
		{
			pRequest = GetAtFast(i);
			if (pRequest->RemoveProcessID(pProcess->GetID().GetID()))
			{
				//	TRACE(_T("rem CRequestCollector ID%d %08lx\n"),pRequest->GetID(),pRequest->GetCamID().GetID());
				if (pRequest->GetNrOfProcesses() == 0)
				{
					// TRACE(_T("DoQRequest FALSE %d, %s\n"),pRequest->GetID(),NameOfEnum(pProcess->GetMacro().GetType()));
					DoQRequest(pRequest,FALSE);
				}
				if (ms_bTraceRequestMultiplexing)
				{
					CString sTmp;
					sTmp.Format(_T("Multiplexed|%08x"),pProcess->GetCamID().GetID());
					WK_STAT_PEAK(_T("Requests"),pRequest->GetNrOfProcesses(),sTmp);
				}
				break;
			}
		}

		Unlock();
	}

	return iRet;
}
/////////////////////////////////////////////////////////////////////////
BOOL CRequestCollectors::InsertRequest(CRequestCollector *pRequest,int iNumPics, int iPos)
{
	BOOL bOkay = TRUE;
	BOOL bDone=FALSE;

	WK_ASSERT(ms_bDisableRequestOptimization==FALSE);
	
/*
	if (ms_bTraceRequestOptimization) {
		WK_TRACE(_T("Insert %d pics at pos %d (current Switches %d)\n"),iNumPics,iPos,m_iNumTotalCameras);
	}
*/

	if (m_iNumTotalCameras>m_iNumCameraSwitches) {
		if (ms_bTraceRequestOptimization) {
			WK_STAT_PEAK(_T("Requests"),m_iNumTotalCameras,_T("SwitchesOverflow"));
		}
		return FALSE;	// too many switches
	}

	// here we assume a filled array (one slot for each possible picture (fps)
	if (iPos>=0 && iPos<GetSize()) {
		while (bDone==FALSE && iPos<GetSize()) {
			CRequestCollector *pCurrentRequest = GetAtFast(iPos);
			// multiplexing is done before, not here
			if (pCurrentRequest->IsIdleRequest()) {
				delete pCurrentRequest;	// drop the idle request
				CRequestCollector *pTmp = new CRequestCollector(*pRequest);	// make a copy
				SetAt(iPos,pTmp);	// insert the new one
				pTmp->SetNumPics(iNumPics);	// credit the real requests iNumPics

				// update counters
				m_iNumTotalPicturesRequested+= iNumPics;
				CRequestCollector *pPrev = GetPreviousRequest(iPos-1);
				if (pPrev) {
					if (pPrev->IsSimilarRequest(pTmp)) {	// same parameter ?
						// no need for a switch if the jobs are similar
						// OOPS even count these, if there is an idle job left of the current job
						// TODO 031198
/*
						if (0 && m_iNumTotalCameras==1
							&& iPos-1 >=0 
							&& GetAtFast(iPos-1)->IsIdleRequest()) 
						{
							m_iNumTotalCameras++;
						}*/
					} else {
						m_iNumTotalCameras++;
					}
				} else {
					// no previous job
					m_iNumTotalCameras++;
				}
				bDone=TRUE;
			} else {
				// search next free pos
				iPos++;
			}
		} // end of loop over possible slots
	} else {
//		WK_TRACE_ERROR(_T("pos error, %d not within 0..%d\n"),iPos,GetSize());
		bOkay = FALSE;
	}
	if (bDone==FALSE) 
	{
		bOkay = FALSE;
	}

	if (m_iNumTotalCameras>m_iNumCameraSwitches) 
	{
		if (ms_bTraceRequestOptimization) 
		{
			WK_STAT_PEAK(_T("Requests"),m_iNumTotalCameras,_T("SwitchesOverflow"));
		}
		bOkay = FALSE;	// too many switches
	}
	return bOkay;
}

void CRequestCollectors::SendRequestsToUnit(CIPCOutputServer *pUnit)
{
	Lock(_T(__FUNCTION__));
	int i;
	CRequestCollector *pRequest = NULL;
	
	// loop over all requests and send them to the unit
	for (i=0;i<GetSize();i++) 
	{
		pRequest = GetAtFast(i);
		// either an EncodingRequest or an idle job
		// use the ids from the collector, not the processor ids
		if (pRequest->GetNrOfProcesses()) 
		{	// has some prcoesses attached
			
			if (CProcessScheduler::m_bTraceEncodeUnitRequests) 
			{
				CString sMsg;
				sMsg.Format(_T("EncodeUnitRequest|%x"),pRequest->GetCamID().GetID());
				WK_STAT_PEAK(_T("Requests"),pRequest->GetNumPics(),sMsg);
			}
			BYTE byNextCam=255;
			CRequestCollector *pNextRequest = GetNextRequest(i+1);
			if (pNextRequest) 
			{
				byNextCam = (BYTE) pNextRequest->GetCamID().GetSubID();
			} 
			else 
			{
				// no followup, guess first request
				pNextRequest = GetNextRequest(0);
				if (pNextRequest) 
				{
					byNextCam = (BYTE) pNextRequest->GetCamID().GetSubID();
				}
			}

			if (theApp.m_bTraceVeryOld)
			{
				TRACE(_T("CAM %04x%04x REQ %08x NR %d\n"),pRequest->GetCamID().GetGroupID(),
													  pRequest->GetCamID().GetSubID(),
													  pRequest->GetID(),
												      pRequest->GetNumPics());
			}

			m_pCIPCOutputServer->DoRequestUnitVideo(pRequest->GetCamID().GetGroupID(),
										 (BYTE)pRequest->GetCamID().GetSubID(),
										  byNextCam,
										  pRequest->GetResolution(),
										  pRequest->GetCompression(),
										  pRequest->GetCompressionType(),
										  (WORD)pRequest->GetNumPics(),
										   pRequest->GetID(),
												FALSE);	// no sleep for each request
		} 								
		else 
		{
			// an IDLE job
			// there is only one job, so there is no next cam
			// assume, we continue on the same cam
			for (int p=0;p<pRequest->GetNumPics();p++) 
			{
				// NOT YET iNumPics in MiCoUnit, thus send each request
				m_pCIPCOutputServer->DoRequestIdlePictures(1);
			}
		}
		
	} // end of loop over all requests
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollectors::OnIndicateVideo(const CIPCPictureRecord &pict,DWORD dwMDX,DWORD dwMDY,DWORD dwUserID)
{
	Lock(_T(__FUNCTION__));
	CRequestCollector* pRequestCollector = NULL;
	BOOL bUsed = FALSE;
	// search for the collectorsID and distribute to all
	// attached processes of that specific collector
	for (int i=0;i<GetSize() && bUsed==FALSE;i++) 
	{
		pRequestCollector = GetAtFast(i);
		if (pRequestCollector->GetID()==dwUserID) 
		{
			pRequestCollector->OnIndicateVideo(pict,dwMDX,dwMDY);
			bUsed = TRUE;
		}
	}

	Unlock();
	return bUsed;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollectors::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID, DWORD dwUserData)
{
	CRequestCollector* pRequestCollector = NULL;
	Lock(_T(__FUNCTION__));
	BOOL bUsed = FALSE;
	// search for the collectorsID and distribute to all
	// attached processes of that specific collector
	for (int i=0;i<GetSize() ;i++) 
	{
		pRequestCollector = GetAtFast(i);
		if (pRequestCollector->GetID()==dwUserData) 
		{
			pRequestCollector->OnIndicateMediaData(rec,secID);
			bUsed = TRUE;
		}
	}
	Unlock();

	return bUsed;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::DumpIDs(LPCTSTR szName)
{
	CString sTmp;
	CString sMsg;

	Lock(_T(__FUNCTION__));
	// it's enough to trace the first id
	// for (int i=0;i<GetSize();i++) {
	for (int i=0;(i<1) && (i<GetSize());i++) 
	{
		CRequestCollector *pRequest = GetAtFast(i);
		if (pRequest)
		{
			if (sMsg.GetLength()) {
				sMsg += _T(',');
			}
			sTmp.Format(_T("%08x"),pRequest->GetID());
			sMsg += sTmp;
		}
	}
	Unlock();

	WK_TRACE(_T("RequestIDs %s: %s ...\n"),szName,sMsg);
}
//////////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::Trace()
{
	Lock();

	CString sShm;

	if (m_pCIPCOutputServer)
	{
		sShm = m_pCIPCOutputServer->GetShmName();
	}
	else if(m_pCIPCAudioServer)
	{
		m_pCIPCAudioServer->GetShmName();
	}
	WK_TRACE(_T("CRequestCollectors %s FPS=%d\n"),sShm,m_iFps);

	for (int i=0;i<GetSize();i++)
	{
		CRequestCollector* pRC = GetAtFast(i);
		pRC->Trace();
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::OptimizeRequests(int iNumCameras, int iNumSwitches)
{
	if (ms_bDisableRequestOptimization==TRUE)
	{
		return;	// <<< EXIT >>>
	}

	Lock(_T(__FUNCTION__));
	
	if (ms_bTraceRequestOptimization && m_iNumTotalCameras ) 
	{
		WK_TRACE(_T("%s:Optimize for %d cams %d pics\n"),
			m_pCIPCOutputServer->GetShmName(),
			m_iNumTotalCameras,m_iNumTotalPicturesRequested
			);	// OBSOLETE
		DumpRequests(this,"Before Optimization:",m_pCIPCOutputServer->GetFramesPerSecond());
	}

	BOOL bSingleCamera = (iNumCameras == 1);
	BOOL bOptimizeOK = TRUE;

	// NOT YET perfect condition
	// restrict to 'simple' cases
	if (   m_iNumTotalCameras>0 
		&& m_iNumTotalCameras <= m_iNumCameraSwitches
		&& m_iNumTotalPicturesRequested<=m_iFps
		&& GetSize() 
		&& GetSize()<=m_iNumCameraSwitches
		) 
	{
		int i;
		CRequestCollectors *pSlots = NULL;
		// big loop over possible chunks
		// each process is split into chunks
		// single chunk is the original request
		// it never splits into more chunks than the requestedPictures
		BOOL bOkay = TRUE;
		BOOL bOptimized=FALSE;
		int iStart = bSingleCamera ? m_iFps: m_iNumCameraSwitches;


		for (int c=iStart;c>1 && bOptimized==FALSE;c--) 
		{
			// create empty slots first
			// that is an idle request foreach possible picture
			WK_DELETE(pSlots);
			bOkay = TRUE;	// init
			pSlots = new CRequestCollectors(m_pCIPCOutputServer);
			for (i=0;i<m_pCIPCOutputServer->GetFramesPerSecond();i++) 
			{
				// CAVEAT fake parent
				CRequestCollector *pIdleRequest = new CRequestCollector(this,NULL,1);
				pSlots->Add(pIdleRequest);
			}


			// fill requests into the slots
			for (i=0;i<GetSize() && bOkay;i++) 
			{	// loop over all requests
				CRequestCollector *pRequest = GetAtFast(i);
				int iRequestPics = pRequest->GetNumPics();
				// schedule each single pic
				// upper limit is the number of requested pictures
				int iNumChunks = min(c,iRequestPics);
				int pos = 0;
				int iPicsPerChunk = pRequest->GetNumPics()/iNumChunks;
				// WK_TRACE(_T("iNumChunks %d, picsPerChunk %d\n"),iNumChunks,iPicsPerChunk);	// OBSOLETE
				for (int p=0;bOkay && p<iRequestPics;p += iPicsPerChunk) 
				{
					for (int p2=0;p2<iPicsPerChunk && bOkay==TRUE;p2++) {
						int iNumRequested=1;
						if (p==0) {
							// don't forget the remaining pictures
							// add them to the first request
							iNumRequested += iRequestPics-iPicsPerChunk*iNumChunks;
							p += iRequestPics-iPicsPerChunk*iNumChunks;	// OOPS
						}
						// loop over all, to make singlePic Inserts
						for (int pr=0;pr<iNumRequested;pr++) {
							bOkay = pSlots->InsertRequest(pRequest,1,pos+p2+pr);
						}
					}
					pos += m_pCIPCOutputServer->GetFramesPerSecond()/iNumChunks;
				}
			}	// end of loop over all requests

			if (bOkay) 
			{
				DeleteAll();
				// transfer from the tmp slots to the own ones
				for (i=0;i<pSlots->GetSize();i++) 
				{
					CRequestCollector *pTmp = new CRequestCollector(*pSlots->GetAtFast(i));
					pTmp->SetParent(this);	// OOPS
					Add(pTmp);
				}
				WK_DELETE(pSlots);	// CAVEAT has NO autodelete
				bOptimized = TRUE;	// terminate big loop
			} 
			else 
			{
//				WK_TRACE_ERROR(_T("Failed to optimize requests for %d\n"),c);
			}
		}	// end of big loop over chunks
		if (bOkay==FALSE) {
//			WK_TRACE_ERROR(_T("Failed to schedule requests\n"));
			WK_DELETE(pSlots); // CAVEAT has NO autodelete
			// keep the original requests
		}
	} 
	else 
	{
		// 'complicated' case do not optimize
		bOptimizeOK = FALSE;
		if (ms_bTraceRequestOptimization)
		{
			WK_TRACE(_T("no optimization possible\n"));
		}
	}
	if (   ms_bTraceRequestOptimization
		&& bOptimizeOK) 
	{
		DumpRequests(this,"Before PostOptimization:",m_pCIPCOutputServer->GetFramesPerSecond());
	}
	PostOptimize(iNumSwitches);
	
	if (   ms_bTraceRequestOptimization
		&& bOptimizeOK) 
	{
		DumpRequests(this,"After  PostOptimization:",m_pCIPCOutputServer->GetFramesPerSecond());
	}

	Unlock();
}

void CRequestCollectors::SortRequests(int iNumSlots)
{
	int iNumIdleRequests = 0;
	int iNrOfFrames = 0;
	int* pNrOfFrames = new int[m_pCIPCOutputServer->GetSize()];
	ZeroMemory(pNrOfFrames,m_pCIPCOutputServer->GetSize()*sizeof(int));

	CRequestCollectorsCS tempCollectors1,tempCollectors2;
	tempCollectors1.SetAutoDelete(FALSE);
	tempCollectors2.SetAutoDelete(FALSE);

	// anzahl der requests pro kamera ermitteln
	// alle requests in tempCollectors1 rausfiltern
	for (int i=0;i<GetSize();i++)
	{
		CRequestCollector* pRC = GetAtFast(i);
		if (!pRC->IsIdleRequest())
		{
			pNrOfFrames[pRC->GetCamID().GetSubID()] += pRC->GetNumPics();
			iNrOfFrames += pRC->GetNumPics();
			
			tempCollectors1.Add(pRC);
		}
		else
		{
			iNumIdleRequests++;
			WK_DELETE(pRC);
			SetAt(i,NULL);
		}
	}

	while (tempCollectors1.GetSize())
	{
		// kamera mit meisten requests als erstes gleichmaessig verteilen
		int iMax = 0;
		int c = 0;
		for (int i=0;i<m_pCIPCOutputServer->GetSize();i++)
		{
			if (pNrOfFrames[i]>iMax)
			{
				iMax = pNrOfFrames[i];
				c = i;
			}
		}
		// kamera c mit den meisten requests
		int iInsertPosition = 0;
		int iInserts = 0;
		int offset = 0;
		for (int i=0;i<tempCollectors1.GetSize()&&iInserts<iMax;i++)
		{
			CRequestCollector* pRC = tempCollectors1.GetAtFast(i);
			if (pRC->GetCamID().GetSubID() == c)
			{
				tempCollectors1.RemoveAt(i);
				i--;

				// Soll Position
				iInsertPosition = (iInserts*iNumSlots)/iMax+offset;
				// find a free place
				BOOL bFound = FALSE;
				for (int j=iInsertPosition;j<tempCollectors2.GetSize();j++)
				{
					if (tempCollectors2.GetAtFast(j)==NULL)
					{
						tempCollectors2.SetAt(j,pRC);
						iInserts++;
						if (j>iInsertPosition)
						{
							offset = j-iInsertPosition;
							iInsertPosition = j;
						}
						bFound = TRUE;
						break;
					}
				}
				if (!bFound)
				{
					if (iInsertPosition == 0 && tempCollectors2.GetSize()>0)
					{
						iInsertPosition = tempCollectors2.GetSize();
					}
					tempCollectors2.InsertAt(iInsertPosition,pRC);
					iInserts++;
				}
				// CRequestCollectors::DumpRequests(&tempCollectors2,_T("Sort"),iMax);
			}
		}
		pNrOfFrames[c] = 0;
	}

	RemoveAll();

	for (int i=0;i<tempCollectors2.GetSize();i++)
	{
		CRequestCollector* pRC = tempCollectors2.GetAtFast(i);
		if (pRC)
		{
			Add(pRC);
		}
		else
		{
			Add(new CRequestCollector(this));
		}
	}

	tempCollectors1.RemoveAll();
	tempCollectors2.RemoveAll();
	delete [] pNrOfFrames;

}

void CRequestCollectors::PostOptimize(int iNumSlots)
{
	if (!ms_bDisableRequestOptimization) 
	{
		int iRemovedTrailingIdleRequests = 0;
		// remove trailing idle slots
		while (    GetSize() 
			    && GetAtFast(GetSize()-1)->IsIdleRequest()) 
		{
			delete (GetAtFast(GetSize()-1));
			RemoveAt(GetSize()-1);
			iRemovedTrailingIdleRequests++;
		}

		SortRequests(iNumSlots);
		if (CRequestCollectors::ms_bTraceRequestOptimization)
		{
			DumpRequests(this,_T("after  SortRequests    :"),m_pCIPCOutputServer->GetFramesPerSecond());
		}

		// merge continous jobs
		MergeContinousJobs();
		AdjustIdleRequests(iRemovedTrailingIdleRequests);

	}
}
void CRequestCollectors::MergeContinousJobs()
{
	for (int i=0;i<GetSize();i++) 
	{
		CRequestCollector *pRequest = GetAtFast(i);
		CRequestCollector *pFollowUpRequest = NULL;

		if (i+1<GetSize()) 
		{
			pFollowUpRequest = GetAtFast(i+1);
			// do not merge idle slots
			if (   pRequest->IsIdleRequest()==FALSE
				&& pFollowUpRequest->IsIdleRequest()==FALSE	
				&& pFollowUpRequest->IsSimilarRequest(pRequest)
				// precondition, both request have the same attached processes, then the id are checked
				&& pRequest->GetNrOfProcesses()==pFollowUpRequest->GetNrOfProcesses()) {

					BOOL bSameProcesses=TRUE;
					// are the processes the same ? 
					// We already know they have the same size
					for (int c=0;c<pRequest->GetNrOfProcesses() && bSameProcesses;c++) 
					{
						DWORD pid = pRequest->GetProcessID(c);
						// the order might be different, so search through all
						// that should not be too much anyway
						// and search is aborted, once it is found
						BOOL bFoundInFollowUp=FALSE;
						for (int c2=0;c2<pFollowUpRequest->GetNrOfProcesses() && bFoundInFollowUp==FALSE;c2++) 
						{
							if (pid==pFollowUpRequest->GetProcessID(c2)) 
							{
								bFoundInFollowUp=TRUE;
							}
						}
						if (bFoundInFollowUp) {
							// cant't say anything here, keep on searching
						} else {
							bSameProcesses = FALSE;
						}
					}	// end of loop over pRequest processes

					if (bSameProcesses) {
						// expand the first job
						pRequest->SetNumPics(pRequest->GetNumPics() + pFollowUpRequest->GetNumPics());
						delete pFollowUpRequest;
						RemoveAt(i+1);
						i--;	// adjust loop
					}
				}

		}
	}
}
void CRequestCollectors::AdjustIdleRequests(int iRemovedTrailingIdleRequests)
{
	int iNumIdle = 0;
	int iNumJobs = 0;
	int i;
	for (i=0;i<GetSize();i++)
	{
		CRequestCollector *pRequest = GetAtFast(i);
		if (pRequest->IsIdleRequest())
		{
			iNumIdle++;
		}
		else
		{
			iNumJobs++;
		}
	}


	if (iNumJobs>=(m_iNumCameraSwitches*7)/8)
	{
		// alle idle jobs wieder raus
		for (i=0;i<GetSize();i++)
		{
			CRequestCollector *pRequest = GetAtFast(i);
			if (pRequest->IsIdleRequest())
			{
				delete pRequest;
				RemoveAt(i);
				i--;
			}
		}
	}
	else if (iNumJobs>=(m_iNumCameraSwitches*3)/4)
	{
		// idle jobs zusammenfassen
		for (i=0;i<GetSize();i++)
		{
			CRequestCollector *pRequest = GetAtFast(i);
			CRequestCollector *pFollowUpRequest = NULL;
			if (i+1<GetSize()) 
			{
				pFollowUpRequest = GetAtFast(i+1);

				if (   pRequest->IsIdleRequest()
					&& pFollowUpRequest->IsIdleRequest())
				{
					WK_DELETE(pFollowUpRequest);
					RemoveAt(i+1);
					i--;	// adjust loop
				}
			}
		}
	}
	else
	{
		// idle jobs von 16 auf 25 stretchen, also 
		// aus zwei aufeinanderfolgenden 3 machen 2->3
		for (i=0;i<GetSize() && iRemovedTrailingIdleRequests>0;i++)
		{
			CRequestCollector *pRequest = GetAtFast(i);
			CRequestCollector *pFollowUpRequest = NULL;
			if (i+1<GetSize()) 
			{
				pFollowUpRequest = GetAtFast(i+1);

				if (   pRequest->IsIdleRequest()
					&& pFollowUpRequest->IsIdleRequest())
				{
					CRequestCollector* pNewIdleRequest = new CRequestCollector(this,NULL,1);
					InsertAt(i+2,pNewIdleRequest);
					i +=2; // adjust loop
					iRemovedTrailingIdleRequests--;
				}
			}
		}
	}
}

void CRequestCollectors::DumpRequests(const CRequestCollectorsCS* pRCs,const CString& sPrefix,int iFPS)
{
	CString sDump(sPrefix);
	CString sTemp;


	sTemp.Format(_T("%d "),pRCs->GetSize());
	sDump += sTemp;
		
	for (int i=0;i<pRCs->GetSize();i++) 
	{
		CRequestCollector *pRequest = pRCs->GetAtFast(i);
		char cCam = '0';
		int iNumPics = 0;
		int iNumProcesses = 0;
		if (   pRequest
			&& pRequest->GetNrOfProcesses()) 
		{	
			// has some prcoesses attached
			iNumPics = pRequest->GetNumPics();
			iNumProcesses = pRequest->GetNrOfProcesses();
			int iCam = pRequest->GetCamID().GetSubID();
			if (iCam<10)
			{
				cCam = (char)(iCam+'0');
			}
			else
			{
				cCam = (char)(iCam-10+'A');
			}
		} 
		else 
		{
			cCam = '~';
		}
		if (iNumPics<=1 && iNumProcesses<=1)
		{
			sTemp.Format(_T("(%c)"),cCam);
		}
		else
		{
			sTemp.Format(_T("(%c,%d,%d)"),cCam,iNumPics,iNumProcesses);
		}
		sDump += sTemp;
	}
	for (i=pRCs->GetSize();i<iFPS;i++) 
	{
		sDump += '_';
	}
	WK_TRACE(_T("%s\n"),sDump);
}
//////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::CheckForTimeout()
{
	for (int i=0;i<GetSize();i++)
	{
		CRequestCollector* pRequest = GetAtFast(i);
		if (pRequest->IsTimedOut())
		{
			DoRequest(pRequest,FALSE);
			DoRequest(pRequest,TRUE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::CheckProcesses()
{
	for (int i=0;i<GetSize();i++)
	{
		CRequestCollector* pRequest = GetAtFast(i);
		pRequest->CheckProcesses();
	}
}
//////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::CheckIncreaseFramerate()
{
	int iFps = 0;
	int i=0;
	for (i=0;i<GetSize();i++)
	{
		iFps += GetAtFast(i)->GetNumPics();
	}
	if (iFps<=25)
	{
		// use the adjusted frame rates again
		for (i=0;i<GetSize();i++)
		{
			CRequestCollector* pR = GetAtFast(i);
			if (pR->GetID()==0)
			{
				pR->CalcTashaRequests(25);
				DoTashaRequest(pR,TRUE);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void CRequestCollectors::CheckDecreaseFramerate()
{
	int i = 0;
	int iFps = 0;
	for (i=0;i<GetSize();i++)
	{
		iFps += GetAtFast(i)->GetNumPics();
	}
	if (iFps>25)
	{
		// reduce previous requests
		for (i=0;i<GetSize();i++)
		{
			CRequestCollector* pR = GetAtFast(i);
			if (pR->GetID()==0)
			{
				pR->CalcTashaRequests(pR->GetNumPics()/2);
				DoTashaRequest(pR,TRUE);
			}
		}
	}
}
