// RequestCollector.cpp: implementation of the CRequestCollector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RequestCollector.h"
#include "RequestCollectors.h"
#include "Process.h"
#include "ProcessSchedulerVideo.h"
#include "ProcessSchedulerAudio.h"
#include "clientencodingprocess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// do get unique request ids use a wrap around counter
WORD CRequestCollector::ms_wSubIDCounter = 1;

/////////////////////////////////////////////////////////////////////////////////
DWORD CRequestCollector::GetIFrameDefault(BOOL bLowBandwidth,int iFPS)
{
	DWORD i = min(iFPS,5);
	if (!bLowBandwidth)
	{
		switch (iFPS)
		{
		case 30:
			i = 6;
			break;
		case 25:
			i = 5;
			break;
		case 12:
			i = 4;
			break;
		case 10:
			i = 5;
			break;
		case 8:
			i = 4;
			break;
		case 6:
			i = 3;
			break;
		case 5:
			i = 5;
			break;
		case 4:
			i = 4;
			break;
		case 3:
			i = 3;
			break;
		case 2:
			i = 2;
			break;
		default:
			i = 1;
			break;
		}
	}
	else
	{
		i = 128;
	}
	return i;
}
/////////////////////////////////////////////////////////////////////////////////
CRequestCollector::CRequestCollector(CRequestCollectors *pParent, 
									 const CProcess *pProcess, 
									 int iNumPics,
									 DWORD dwID)
{
	m_pParent = pParent;

	m_csTickLastPicture.Lock();
	m_dwTickLastPicture = 0;
	m_csTickLastPicture.Unlock();

	if (pProcess) 
	{
		m_iNumPics = iNumPics;
		m_IFrameIntervall = 1;
		if (   pParent->GetCIPCOutputServer()->IsTasha()
			|| pParent->GetCIPCOutputServer()->IsQ()
			|| pParent->GetCIPCOutputServer()->IsIP())
		{
			m_id = dwID;
			if (pParent->GetCIPCOutputServer()->IsTasha())
			{
				m_IFrameIntervall = GetIFrameDefault(GetID()==1,m_iNumPics);
			}
		}
		else
		{
			m_id.Set(pParent->GetIOGroup()->GetGroupID(),ms_wSubIDCounter++);
		}
		m_camID = pProcess->GetCamID();
		m_Resolution =pProcess->GetMacro().GetResolution();
		m_Compression =	pProcess->GetMacro().GetCompression();
		m_attachedProcesses.Add(pProcess->GetID().GetID());
		m_CompressionType = pProcess->GetCompressionType();
		m_dwBitrateScale = 0;
	} 
	else 
	{
		// an IDLE job
		m_camID = SECID_NO_ID;
		m_Resolution = RESOLUTION_NONE;
		m_Compression = COMPRESSION_NONE;
		m_CompressionType = COMPRESSION_UNKNOWN;
		m_dwBitrateScale = 0;
		m_iNumPics = iNumPics;
		m_IFrameIntervall = 0;
	}
}
//////////////////////////////////////////////////////////////
CRequestCollector::CRequestCollector(const CRequestCollector &src)
{
	m_id.Set(src.m_id.GetGroupID(),ms_wSubIDCounter++);
	m_camID = src.m_camID;
	m_Resolution = src.m_Resolution;
	m_Compression = src.m_Compression;
	m_CompressionType = src.m_CompressionType;
	m_dwBitrateScale= src.m_dwBitrateScale;
	m_iNumPics = src.m_iNumPics;
	m_attachedProcesses.Copy(src.m_attachedProcesses);
	m_pParent = src.m_pParent;
	m_IFrameIntervall = src.m_IFrameIntervall;
	m_dwTickLastPicture = src.m_dwTickLastPicture;
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::OnIndicateVideo(const CIPCPictureRecord &pict,DWORD dwMDX,DWORD dwMDY)
{
	ResetTimeStamp();

	for (int i=0;i<m_attachedProcesses.GetSize();i++) 
	{
		// loop over all attached processes
		DWORD pid = m_attachedProcesses[i];
		CIPCOutputServer* pCIPC = m_pParent->GetCIPCOutputServer();
		if (pCIPC)
		{
			CProcessScheduler* pPS = pCIPC->GetProcessScheduler();
			if (pPS)
			{
				pPS->AnswerRequest(pict,dwMDX,dwMDY, pid,this);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID)
{
	for (int i=0;i<m_attachedProcesses.GetSize();i++) 
	{
		// loop over all attached processes
		DWORD pid = m_attachedProcesses[i];
		CProcessSchedulerAudio* pPSA = (CProcessSchedulerAudio*)m_pParent->GetCIPCAudioServer()->GetProcessScheduler();
		pPSA->AnswerRequest(rec,secID,pid);
	}
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::IsSimilarRequest(const CProcess *pProcess) const
{
	BOOL bSimilar = FALSE;
	bSimilar = (  
			   m_camID ==pProcess->GetCamID()
			&& m_Resolution == pProcess->GetMacro().GetResolution()
			&& m_Compression == pProcess->GetMacro().GetCompression()
			);
			// not here int m_iNumPics; caller has to do that

	return bSimilar;	// NOT YET
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::IsSimilarRequest(const CRequestCollector *pRequest) const
{
	BOOL bSimilar = FALSE;
	bSimilar = (   IsIdleRequest() 
		        && pRequest->IsIdleRequest())
				||
		       (   m_camID == pRequest->m_camID
				&& m_Resolution == pRequest->m_Resolution
				&& m_Compression == pRequest->m_Compression
				&& m_CompressionType == pRequest->m_CompressionType
				&& m_dwBitrateScale == pRequest->m_dwBitrateScale
				);
			// not here int m_iNumPics; caller has to do that

	return bSimilar;	// NOT YET
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::SetID(DWORD dwID)
{
	m_id = dwID;
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::SetCamID(CSecID id)
{
	m_camID = id;
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::SetNumPics(int iNumPics)
{
	m_iNumPics = iNumPics;
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::SetParent(CRequestCollectors *pParent)
{
	m_pParent = pParent;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::FindProcess(DWORD dwPID)
{
	BOOL bFound = FALSE;

	for (int i=0;i<m_attachedProcesses.GetSize() && !bFound;i++)
	{
		bFound = m_attachedProcesses[i] == dwPID;
	}

	return bFound;
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::AddProcessID(DWORD dwPID)
{
	m_attachedProcesses.Add(dwPID);
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::RemoveAllProcessIDs()
{
	m_attachedProcesses.RemoveAll();
	m_dwTickLastPicture = 0;
	m_iNumPics = 0;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::RemoveProcessID(DWORD dwPID)
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_attachedProcesses.GetSize();i++)
	{
		if (m_attachedProcesses[i] == dwPID)
		{
			m_attachedProcesses.RemoveAt(i);
			bFound = TRUE;
			break;
		}
	}
	if (m_attachedProcesses.GetSize() == 0)
	{
		m_dwTickLastPicture = 0;
		m_iNumPics = 0;
	}
	return bFound;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::ActualizeQRequest(CProcess *pProcess)
{
	BOOL bChanged = FALSE;

	if ((int)pProcess->GetMacro().GetNumPictures() > m_iNumPics)
	{
		m_iNumPics = pProcess->GetMacro().GetNumPictures();
		bChanged = TRUE;
	}
	else if ((int)pProcess->GetMacro().GetNumPictures() < m_iNumPics)
	{
		if (m_attachedProcesses.GetSize() == 1)
		{
			m_iNumPics = pProcess->GetMacro().GetNumPictures();
			bChanged = TRUE;
		}
	}

	return bChanged;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::ActualizeTashaRequest(CProcess *pProcess)
{
	BOOL bChanged = FALSE;

	bChanged = CalcTashaRequests();

	// immer höher packen
	if (pProcess->GetMacro().GetCompression()>m_Compression)
	{
		m_Compression = pProcess->GetMacro().GetCompression();
		bChanged = TRUE;
	}
	return bChanged;
}

void AddSorted(CDWordArray& array, DWORD dwValue)
{
	int i=0;
	BOOL bInsert = TRUE;
	for (i=0;i<array.GetSize();i++)
	{
		if (array[i]<dwValue)
		{
			break;
		}
		if (array[i]==dwValue)
		{
			bInsert = FALSE;
			break;
		}
	}
	if (bInsert)
	{
		if (i>=0)
		{
			array.InsertAt(i,dwValue);
		}
		else
		{
			array.Add(dwValue);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::CalcTashaRequests(DWORD iMaxFPS)
{
	BOOL bChanged = FALSE;
	BOOL b4CIF = FALSE;
	// 1ter Stream falls 1x 4CIF max 12 fps
	if (GetID() == 0)
	{
		for (int i=0;i<m_attachedProcesses.GetSize();i++)
		{
			CProcess* pProcess = m_pParent->GetCIPCOutputServer()->GetProcessScheduler()->GetProcess(m_attachedProcesses[i]);
			if (   pProcess
				&& pProcess->GetMacro().GetResolution() == RESOLUTION_4CIF)
			{
				b4CIF = TRUE;
				break;
			}
		}
	}
	if (b4CIF)
	{
		iMaxFPS = 12;
	}

	Resolution res = RESOLUTION_CIF;
	Compression comp = COMPRESSION_1;
	DWORD iFPS = (GetID()==0) ? 1 : 12;
	DWORD iIFrame = GetIFrameDefault(GetID()==1,iMaxFPS);

	CDWordArray dwFPSs;

	// calc the IFrame intervall

	for (int i=0;i<m_attachedProcesses.GetSize();i++)
	{
		CProcess* pProcess = m_pParent->GetCIPCOutputServer()->GetProcessScheduler()->GetProcess(m_attachedProcesses[i]);

		if (pProcess)
		{
			if (GetID() == 0)
			{
				//Neuen Request auch losschicken, wenn aktuelle Resulotion größer als alte ist
				//Vergleich war an dieser Stelle ">"
				if (pProcess->GetMacro().GetResolution()>res)
				{
					res = pProcess->GetMacro().GetResolution();
				}
				DWORD iProcessFPS = min(iMaxFPS,pProcess->GetMacro().GetPicsPerSlice());
				if (iProcessFPS>iFPS)
				{
					iFPS = iProcessFPS;
				}
				if (iProcessFPS>1)
				{
					AddSorted(dwFPSs, iProcessFPS);
				}
			}
			else if (GetID() == 1)
			{
				if (pProcess->GetMacro().GetResolution()>res)
				{
					res = pProcess->GetMacro().GetResolution();
				}
				if (pProcess->GetMacro().GetPicsPerSlice()<iFPS)
				{
					iFPS = pProcess->GetMacro().GetPicsPerSlice();
				}
			}

			// immer höher packen
			if (pProcess->GetMacro().GetCompression()>comp)
			{
				comp = pProcess->GetMacro().GetCompression();
			}
		}
	}

	if (dwFPSs.GetSize()>0)
	{
#if (0)
		CString s,one;
		for (i=0;i<dwFPSs.GetSize();i++)
		{
			one.Format(_T("%d,"),dwFPSs[i]);
			s+=one;
		}
		TRACE(_T("FPS Array %s\n"),s);
#endif

		if (dwFPSs.GetSize()==1)
		{
			iIFrame = GetIFrameDefault(GetID()==1,dwFPSs[0]);
		}
		else if (dwFPSs.GetSize()>1)
		{
			// min 2 Anfragen
			// I Frame intervall Quotient der beiden groessten Werte
			iIFrame = dwFPSs[0] / dwFPSs[1];
		}
	}

	if (res != m_Resolution)
	{
		m_Resolution = res;
		bChanged = TRUE;
	}
	if (comp != m_Compression)
	{
		m_Compression = comp;
		bChanged = TRUE;
	}
	if ((int)iFPS != m_iNumPics)
	{
		if (   GetID() == 0
			&& iFPS<25)
		{
			CString s,one;
			for (i=0;i<dwFPSs.GetSize();i++)
			{
				one.Format(_T("%d,"),dwFPSs[i]);
				s+=one;
			}
			WK_TRACE(_T("FPS Array %d,%s Max=%d %08lx\n"),dwFPSs.GetSize(),s,iMaxFPS,GetCamID().GetID());
			for (int i=0;i<m_attachedProcesses.GetSize();i++)
			{
				CProcess* pProcess = m_pParent->GetCIPCOutputServer()->GetProcessScheduler()->GetProcess(m_attachedProcesses[i]);
				if (pProcess)
				{
					WK_TRACE(_T("FPS Array %s\n"),pProcess->GetDescription(TRUE));
				}
			}
		}
		m_iNumPics = iFPS;
		bChanged = TRUE;
	}

	if (iIFrame != m_IFrameIntervall)
	{
		m_IFrameIntervall = iIFrame;
		bChanged = TRUE;
	}

	return bChanged;
}
/////////////////////////////////////////////////////////////////////////////////
int CRequestCollector::GetMinFrameCounter()
{
	int r = 25;

	for (int i=0;i<m_attachedProcesses.GetSize();i++)
	{
		CProcess* pProcess = m_pParent->GetCIPCOutputServer()->GetProcessScheduler()->GetProcess(m_attachedProcesses[i]);

		if (   pProcess
			&& pProcess->IsClientEncodingProcess())
		{
			CClientEncodingProcess* pClientEncodingProcess = (CClientEncodingProcess*)pProcess;
			if (pClientEncodingProcess->GetFrameCounter() < r)
			{
				r = pClientEncodingProcess->GetFrameCounter();
			}
		}
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::IsTimedOut()
{
	BOOL bRet = FALSE;
	
	if (m_attachedProcesses.GetSize())
	{
		m_csTickLastPicture.Lock();
		DWORD dwWKTick = WK_GetTickCount();
		if (m_dwTickLastPicture != 0)
		{
			DWORD dwDiff = GetID()==0 ? 5000 : 15000;
			bRet = WK_GetTimeSpan(m_dwTickLastPicture,dwWKTick) > dwDiff;
		}
		if (bRet)
		{
			WK_TRACE(_T("TASHA/Q request timed out restart %08lx, %d %d, %d\n"),
				GetCamID(),
				GetID(),
				dwWKTick,m_dwTickLastPicture);
		}
		m_csTickLastPicture.Unlock();
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CRequestCollector::CheckProcesses()
{
	BOOL bRet = FALSE;

	if (   m_pParent
		&& m_pParent->GetCIPCOutputServer()
		&& m_pParent->GetCIPCOutputServer()->GetProcessScheduler())
	{
		CProcessScheduler* pScheduler = m_pParent->GetCIPCOutputServer()->GetProcessScheduler();
		for (int i=m_attachedProcesses.GetSize()-1;i>=0;i--)
		{
			CProcess* pProcess = pScheduler->GetProcess(m_attachedProcesses[i]);
			if (   pProcess
				&& pProcess->GetCamID() != GetCamID())
			{
				WK_TRACE(_T("request has process with wrong cam id\n"));
				m_attachedProcesses.RemoveAt(i);
			}

		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::ResetTimeStamp()
{
	m_csTickLastPicture.Lock();
	m_dwTickLastPicture = WK_GetTickCount();
	m_csTickLastPicture.Unlock();
}
/////////////////////////////////////////////////////////////////////////////////
void CRequestCollector::Trace()
{
	CString sProcessIDs,sOne;
	for (int i=0;i<m_attachedProcesses.GetSize();i++)
	{
		sOne.Format(_T("%08lx"),m_attachedProcesses[i]);
		sProcessIDs += _T(",") + sOne;
	}

	WK_TRACE(_T("RC ID=%08lx,Cam=%08lx,FPS=%d PIDs=%s\n"),m_id.GetID(),m_camID.GetID(),m_iNumPics,sProcessIDs);
	for (int i=0;i<m_attachedProcesses.GetSize();i++)
	{
		CProcess* pProcess = m_pParent->GetCIPCOutputServer()->GetProcessScheduler()->GetProcess(m_attachedProcesses[i]);
		WK_TRACE(_T("PID=%08lx,%s\n"),pProcess->GetID().GetID(),pProcess->Format());
	}
	WK_TRACE(_T("\n"));
}

