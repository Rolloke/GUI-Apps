/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndMpeg4.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndMpeg4.cpp $
// CHECKIN:		$Date: 3.07.06 16:26 $
// VERSION:		$Revision: 142 $
// LAST CHANGE:	$Modtime: 3.07.06 16:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$

#include "stdafx.h"
#include "IdipClient.h"

#include "WndMpeg4.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewControllItems.h"

#include "Server.h"
#include "CIPCOutputIdipClient.h"
#include "WndReference.h"
#include "MainFrm.h"

#include <WKClasses\wk_utilities.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define PL_ACTIVE	0
#define PL_MOTION	1
#define PL_NOMOTION	2

const int CWndMpeg4::gm_nQualities[MAX_PERFORMANCE_LEVEL+1][3] = 
{
	{25, 25, 25},	// 0	// obsolete
	{25, 25, 5},	// 1	// obsolete
	{25, 5, 5},		// 2
	{ 5, 5, 5},		// 3
	{ 5, 5, 1},		// 4
	{ 5, 1, 1},		// 5
	{ 1, 1, 1}		// 6
};

#define TIMER_EVENT		TRUE
#define STOP_GOP_THREAD	0xffffffff

#define TRACE_TIMER		0x00000001
#define TRACE_PICTURES	0x00000010


#define NO_PICTURE			-1
#define SINGLE_PICTURE		-2

CString NameOfCurrentPictureState(int nCurrentPict, BOOL bReturnNumber)
{
	switch (nCurrentPict)
	{
		NAME_OF_ENUM(NO_PICTURE);
		NAME_OF_ENUM(SINGLE_PICTURE);
		default:
		if (bReturnNumber)
		{
			CString s;
			s.Format(_T("Current Picture:%d\n"), nCurrentPict);
			return s;
		}break;
	}
	return _T("");
}

UINT CWndMpeg4::gm_wTimerRes = 0;


// Mico Resolutions
// RESOLUTION_QCIF 384 x 288, 12:9
// RESOLUTION_CIF 768 x 288, 24:9
// RESOLUTION_2CIF 768 x 576, 12:9
IMPLEMENT_DYNAMIC(CWndMpeg4, CWndLive)

/////////////////////////////////////////////////////////////////////////////
// CWndMpeg4
CWndMpeg4::CWndMpeg4(CServer* pServer, const CIPCOutputRecord &or) 
	: CWndLive(pServer, or)
{
//	m_iX = 4;
//	m_iY = 3;
	m_nType = WST_MPEG4;

	if (pServer->IsLocal())
	{
		m_iQuality = 25;
	}
	else
	{
		m_iQuality = 25;
	}

	m_bDropped = false;
	m_bMD = false;
	m_bForceHighPerformance = false;
	m_bNeedPrintImage = false;
	m_dwLastMD_TC = GetTickCount();
	m_pStatistics = NULL;
	m_nCurrentPicture = NO_PICTURE;
	m_pMpeg4Decoder = NULL;
	
	m_hGOPTimerEvent = NULL;
	m_pGOPThread = NULL;

	if (theApp.m_bUseAvCodec)
	{
		try
		{
			// Versuch den AVCodec zu öffnen. Falls dies fehlschlägt, wird der emuzed decoder verwendet.
			m_pMpeg4Decoder = new CAVCodec();
			WK_TRACE(_T("Using AVCodec to decoded mpeg4\n"));
		}
		catch(...)
		{
			m_pMpeg4Decoder = NULL;
			WK_TRACE(_T("Cannot load AVCodec\n"));
		}
	}

	if (m_pMpeg4Decoder == NULL)
	{
		m_pMpeg4Decoder = new CMPEG4Decoder;
		WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
	}

	if (theApp.m_bUseMMtimer || theApp.m_bUseGOPThreadAlways)
	{
		CString sName = pServer->GetName() + GetName();
		m_hGOPTimerEvent = CreateEvent(NULL, FALSE, FALSE, sName);
		m_pGOPThread = AfxBeginThread(GOPThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0);
		m_pGOPThread->m_bAutoDelete = FALSE;
	}

	if (theApp.m_bShowFPS || theApp.m_bRegardNetworkFrameRate)
	{
		m_pStatistics = new CStatistics(50);
	}

	m_uGOPTimer = 0;
	m_AvgStepMS.SetNoOfAvgValues(5);
	m_nCurrentTimeStep = 0;
	m_nCurrentFPS = 0;
	m_nCurrentRate = 100;
}
/////////////////////////////////////////////////////////////////////////////
CWndMpeg4::~CWndMpeg4()
{
	m_GOPs.SafeDeleteAll();
	WK_DELETE(m_pStatistics);
}
/////////////////////////////////////////////////////////////////////////////
int CWndMpeg4::GetRequestedFPS()
{
	int iRet = m_iQuality;
	if (m_pServer->IsConnectedOutput())
	{
		if (m_pServer->IsLowBandwidth())
		{
			// RKE: sind 12 oder 6 Fps erlaubt ?
			if (IsCmdActive())
			{
				iRet = min(m_iQuality,12);
			}
			else
			{
				if (m_bMD)
				{
					iRet = min(m_iQuality,5);
				}
				else
				{
					iRet = min(m_iQuality,1);
				}
			}
		}
		else
		{
			int nPerformanceLevel = theApp.GetPerformanceLevel();
			if (nPerformanceLevel < INITIAL_PERFORMANCE_LEVEL)
			{
				nPerformanceLevel = INITIAL_PERFORMANCE_LEVEL;
			}
			else
			{
				m_bForceHighPerformance = false;
			}

			if (   theApp.m_bRegardNetworkFrameRate 
				&& !GetServer()->IsLocal())
			{
				if (GetServer()->m_nRequestState == REQUEST_STATE_RECOVER)
				{
					nPerformanceLevel = LOWEST_PERFORMANCE_LEVEL;
					m_Resolution = GetServer()->m_nMaxWith1Fps == 0 ? RESOLUTION_QCIF : RESOLUTION_2CIF;
				}
				else if (GetServer()->m_nRequestState == REQUEST_STATE_LIMITED)
				{
					return m_nCurrentFPS;
				}
			}

			if (IsCmdActive() || m_bForceHighPerformance)
			{
				iRet = min(m_iQuality, gm_nQualities[nPerformanceLevel][PL_ACTIVE]);
			}
			else
			{
				if (m_bMD)
				{
					iRet = min(m_iQuality, gm_nQualities[nPerformanceLevel][PL_MOTION]);
				}
				else
				{
					iRet = min(m_iQuality, gm_nQualities[nPerformanceLevel][PL_NOMOTION]);
				}
			}
		}

		if (m_pStatistics && theApp.m_bRegardNetworkFrameRate)
		{
			if (iRet != m_nCurrentFPS)
			{
				int nNumValues = 4;					// 3 seconds if the rate is 1 fps
				if (iRet > 1) nNumValues = iRet*2;	// 2 seconds to determine frame rate
//				TRACE(_T("Change Fps %d -> %d\n"), m_nCurrentFPS, iRet);
				m_pStatistics->SetNumValues(nNumValues);
			}
		}
		m_nCurrentFPS = iRet;
	}

	return iRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMpeg4::CheckPerformanceLevel(int nPerformanceBias)
{
	BOOL bReturn = TRUE;
	int nMaxHighPerformanceWnds = INITIAL_PERFORMANCE_LEVEL - theApp.GetPerformanceLevel() + 1;

	if (nMaxHighPerformanceWnds)
	{
		CViewIdipClient*pView = theApp.GetMainFrame()->GetViewIdipClient();
		if (pView)
		{
			pView->LockSmallWindows(_T(__FUNCTION__));
			CWndSmallArray *pSws = pView->GetSmallWindows();
			int i, nSize = pSws->GetSize();
			int nHighPerformanceWnds = 0;			// count high performance windows
			CWndSmallArray swPhP;					// probably high performance
			// 1st step: count window types
			for (i=0; i<nSize; i++)
			{
				CWndSmall*pSW = pSws->GetAtFast(i);
				if (   pSW->GetType() == WST_MPEG4 
					&& pSW->GetServer()->m_nRequestState == REQUEST_STATE_NORMAL)
				{
					CWndMpeg4 *pMP4 = (CWndMpeg4*)pSW;
					pMP4->m_bForceHighPerformance = false;
					if (pMP4->IsCmdActive())
					{
						nHighPerformanceWnds++;		// the cmd active is always highest performance
					}
					else
					{
						swPhP.Add(pSW);
					}
				}
			}
			// 2nd step init MD windows
			nSize = swPhP.GetSize();
			for (i=0; i<nSize; i++)
			{
				CWndMpeg4 *pMP4 = (CWndMpeg4*)swPhP.GetAtFast(i);
				if (nHighPerformanceWnds >= nMaxHighPerformanceWnds)
				{
					swPhP.RemoveAll();
					break;
				}
				if (pMP4->m_bMD)
				{
					swPhP.RemoveAt(i);
					i--;
					nSize--;
					pMP4->m_bForceHighPerformance = true;
					nHighPerformanceWnds++;
				}
			}
			// 3rd step init non MD windows
			nSize = swPhP.GetSize();
			for (i=0; i<nSize; i++)
			{
				CWndMpeg4 *pMP4 = (CWndMpeg4*)swPhP.GetAtFast(i);
				pMP4->m_bForceHighPerformance = true;
				nHighPerformanceWnds++;
				if (nHighPerformanceWnds >= nMaxHighPerformanceWnds)
				{
					break;
				}
			}
			pView->UnlockSmallWindows();
			theApp.m_nNoOfHighPerformanceWnd = nHighPerformanceWnds;
		}
	}
	else
	{
		theApp.m_nNoOfHighPerformanceWnd = 0;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
int CWndMpeg4::DetermineNetworkStatistics(WORD wServer, int&nCameras)
{
	CViewIdipClient*pView = theApp.GetMainFrame()->GetViewIdipClient();
	if (pView)
	{
		pView->LockSmallWindows(_T(__FUNCTION__));
		CWndSmallArray *pSws = pView->GetSmallWindows();
		int i, nSize = pSws->GetSize();
		double dArrivedFps	  = 0;			// count arrived fps
#ifdef _DEBUG
		int nRequestedFps = 0;			// count requested fps
		int nAvgSize      = 0;
		LONGLONG nBitrate = 0;
#endif
		nCameras = 0;

		// 1st step: count window types
		for (i=0; i<nSize; i++)
		{
			CWndSmall*pSW = pSws->GetAtFast(i);
			if (   pSW->GetServerID() == wServer 
				&& pSW->GetType() == WST_MPEG4)
			{
				CWndMpeg4 *pMP4 = (CWndMpeg4*)pSW;
				if (pMP4->m_pStatistics)
				{
					CStatistics*pS = pMP4->m_pStatistics;
					dArrivedFps   += pS->GetFPS();
					nCameras++;
#ifdef _DEBUG
					nRequestedFps += pMP4->m_nCurrentFPS;
					nBitrate += pS->GetBitrate();
					nAvgSize      += pS->GetAverageValue();
#endif
				}
			}
		}
		pView->UnlockSmallWindows();
		int nMaxRequested = MulDiv((int)dArrivedFps, 70, 100);
		if (nMaxRequested < 1 && nCameras > 0)
		{
			nMaxRequested = 1;
		}

#ifdef _DEBUG
		if (nCameras)
		{
			nAvgSize /= nCameras;
		}
		TRACE(_T("Requested Fps: %d\n"), nRequestedFps);
		TRACE(_T("Arrived Fps: %.1f; max %d\n"), dArrivedFps, nMaxRequested);
		TRACE(_T("Bitrate: %lu bits/sec, %lu KB/sec\n"), nBitrate, nBitrate/128);
		TRACE(_T("Avg Picture size: %d Bytes\n"), nAvgSize);
#endif
		return nMaxRequested;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::CalculateFps(CServer*pS, int nCameras)
{
	int nC, nMax5fps, nMax25fps, nMaxFps = pS->m_nMaxRequestedFps;
	short &n25fps = pS->m_nMaxWith25Fps;
	short &n5fps  = pS->m_nMaxWith5Fps;
	short &n1fps  = pS->m_nMaxWith1Fps;
	int nMaxHighPerformanceWnds = INITIAL_PERFORMANCE_LEVEL - theApp.GetPerformanceLevel() + 1;

	nMaxHighPerformanceWnds -= theApp.m_nNoOfHighPerformanceWnd;
	nMax25fps = min(nMaxFps / 25, nMaxHighPerformanceWnds);
	nMax5fps = nMaxFps / 5;
	n5fps = n25fps = 0;
	n1fps = (short)nMaxFps;

	if (nMaxFps < nCameras)
	{
		n1fps = (short)(nMaxFps / 2);
		nC = 0;
	}

	if (nMax25fps > 1)
	{
		n25fps = (short)(nMax25fps-2);
		n5fps  = (short)nMax5fps;
	}
	
	for (nC=nMaxFps-4; nC>0; nC-=4)
	{
		if (nC < nCameras)	break;
		if (n5fps == nMax5fps)
		{
			nC -= nCameras;
			if (nC < 4) break;
			if (n25fps >= nMaxHighPerformanceWnds) break;
			n25fps++;
			nC = nMaxFps - n25fps*25;
			if (nC < 0)
			{
				n25fps--;
				break;
			}
			n1fps = (short)nC;
			nMax5fps = nC / 5;
			n5fps = 0;
			nC += 4;
			continue;
		}
		n5fps++;
		n1fps -= 5;
	}

	nC = n1fps + n5fps+n25fps;
	if (nC < nCameras)
	{
		n1fps+=5;
		n5fps--;
	}
	if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
	{
		WK_TRACE(_T("%s limited: %d fps total, %d cams, %d 25fps, %d 5fps, %d 1fps\n"),
			pS->GetName(), nMaxFps, nCameras, n25fps, n5fps, n1fps);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::SetLimitedFps(CServer *pServer)
{
	CViewIdipClient*pView = theApp.GetMainFrame()->GetViewIdipClient();
	if (pView)
	{
		WORD wServer = pServer->GetID();
		pView->LockSmallWindows(_T(__FUNCTION__));
		CWndSmallArray *pSws = pView->GetSmallWindows();
		int i, nSize = pSws->GetSize();
		short n5fps = 0, n25fps = 0, n1fps = 0;
		// 1st step: init cmd active window and collect the server windows
		CWndSmallArray swServer;
		for (i=0; i<nSize; i++)
		{
			CWndSmall*pSW = pSws->GetAtFast(i);
			if (   pSW->GetServerID() == wServer 
				&& pSW->GetType() == WST_MPEG4)
			{
				CWndMpeg4 *pMP4 = (CWndMpeg4*)pSW;
				if (pMP4->IsCmdActive())
				{
					LimitFps(pServer, pMP4, n25fps, n5fps, n1fps);
				}
				else
				{
					swServer.Add(pSW);
				}
			}
		}
		// 2nd step init MD windows
		nSize = swServer.GetSize();
		for (i=0; i<nSize; i++)
		{
			CWndMpeg4 *pMP4 = (CWndMpeg4*)swServer.GetAtFast(i);
			if (pMP4->m_bMD)
			{
				swServer.RemoveAt(i);
				i--;
				nSize--;
				LimitFps(pServer, pMP4, n25fps, n5fps, n1fps);
			}
		}
		// 3rd step init non MD windows
		for (i=0; i<nSize; i++)
		{
			CWndMpeg4 *pMP4 = (CWndMpeg4*)swServer.GetAtFast(i);
			LimitFps(pServer, pMP4, n25fps, n5fps, n1fps);
		}
		pView->UnlockSmallWindows();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::LimitFps(CServer *pServer, CWndMpeg4*pMP4, short&n25fps, short&n5fps, short&n1fps)
{
	pMP4->m_Resolution = RESOLUTION_2CIF;
	if (n25fps < pServer->m_nMaxWith25Fps)
	{
		pMP4->m_nCurrentFPS = 25;
		n25fps++;
	}
	else if (n5fps < pServer->m_nMaxWith5Fps)
	{
		pMP4->m_nCurrentFPS = 5;
		n5fps++;
	}
	else if (n1fps < pServer->m_nMaxWith1Fps)
	{
		pMP4->m_nCurrentFPS = 1;
		n1fps++;
	}
	else
	{
		pMP4->m_nCurrentFPS = 1;
		pMP4->m_Resolution = RESOLUTION_QCIF;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnSetCmdActive(BOOL bActive, CWndSmall* pWndSmall)
{
	if (m_pServer->m_nRequestState == REQUEST_STATE_LIMITED)
	{
		SetLimitedFps(m_pServer);
	}
	CWndSmall::OnSetCmdActive(bActive, pWndSmall);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnSetSmallBig(BOOL bActive, CWndSmall* pWndSmall)
{
	OnSetCmdActive(bActive,pWndSmall);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndMpeg4, CWndLive)
	//{{AFX_MSG_MAP(CWndMpeg4)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DECODE_VIDEO, OnDecodeVideo)
	ON_MESSAGE(WM_CHANGE_VIDEO, OnChangeVideo)
	ON_MESSAGE(WM_USER, OnUser)
	ON_COMMAND_RANGE(ID_FPS_1_1, ID_FPS_25_1, OnFps_X_X)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FPS_1_1, ID_FPS_25_1, OnUpdateFps_X_X)
	ON_COMMAND(ID_VIDEO_SAVE_REFERENCE, OnVideoSaveReference)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CSize CWndMpeg4::GetPictureSize()
{
	return m_pMpeg4Decoder->GetImageDims();
}
////////////////////////////////////////////////////////////////////////////
int CWndMpeg4::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndLive::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pMpeg4Decoder->Init(NULL,0,0,0);

	if (m_pServer->IsMultiCamera())
	{
		if (m_pServer->IsLowBandwidth())
		{
			m_Resolution = RESOLUTION_QCIF;
			if (m_pServer->IsAlarm())
			{
				if (m_pServer->GetAlarmID() == GetID())
				{
					m_Resolution = RESOLUTION_2CIF;
				}
			}
			else
			{
				if (   (theApp.m_bFirstCam==TRUE)
					|| m_pViewIdipClient->GetNrOfLiveWindows(m_pServer->GetID()) == 0)
				{
					m_Resolution = RESOLUTION_2CIF;
				}
			}
		}
		else
		{
			m_Resolution = RESOLUTION_2CIF;
		}
	}
	if (m_pServer->m_nRequestState == REQUEST_STATE_LIMITED)
	{
		int nCameras = m_pViewIdipClient->GetNrOfWindows(m_wServerID, WST_MPEG4);
		CalculateFps(m_pServer, nCameras);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CWndMpeg4::CalculateFrameStep(int nTimeStep)
{
	if (IsBetween(nTimeStep, 10000, 500000))
	{
		m_AvgStepMS.AddValue(nTimeStep);
	}
	else
	{
		return 40;						// presume 25 fps
	}

	if (m_AvgStepMS.GetActualValues() > 1)	// are there more values
	{										// use them
		int nValues = m_AvgStepMS.GetActualValues() * 1000;
		nTimeStep = MulDiv(m_AvgStepMS.GetSum(), 1, nValues);
	}
	else
	{
		nTimeStep = MulDiv(nTimeStep, 1, 1000);
	}
	//               30,     15,       6,
	//				     25,     12,        5 fps
	int nValues[] = {33, 40, 67, 83, 166, 200}; // ms, (abgerundete Werte)
	int i, nSel = 0, nDiff, nBest = 1000;
	for (i=0; i<6; i++)
	{
		nDiff = nTimeStep - nValues[i];
		if (nDiff < 0)					// negative Werte
		{
			nDiff = -nDiff + 1;			// +1 wegen abrunden
			if (nDiff > nBest)			// der nächste Wert
			{
				break;					// wird noch größer sein
			}
		}
		if (nDiff < nBest)				// 
		{
			nBest = nDiff;
			nSel = i;
		}
	}
	return nValues[nSel];
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMpeg4::PictureData(const CIPCPictureRecord &pict, CSecID idArchive, DWORD dwX, DWORD dwY)
{
	if (m_hWnd == NULL)
	{
		return FALSE;
	}
	if (pict.GetCompressionType() == COMPRESSION_MPEG4)
	{
/*		if (GetID().GetSubID()==0)
		{
			TRACE(_T("%08lx %s,%03d,%s,%d\n"), GetID().GetID(), pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds(),SPT2TCHAR(pict.GetPictureType()),pict.GetNumberOfPictures());
		}	*/
		if (   m_bNeedPrintImage
			&& (pict.GetPictureType() == SPT_GOP_PICTURE || pict.GetPictureType() == SPT_FULL_PICTURE))
		{
			m_bNeedPrintImage = false;
			{
				CAutoCritSec acs(&m_csPictureRecord);
				ASSERT(m_pPrintPictureRecord==NULL);
				m_pPrintPictureRecord = new CIPCPictureRecord(pict);
			}
			theApp.GetMainFrame()->GetActiveView()->RedrawWindow();
			TRACE(_T("Requested print image copied.\n"));
		}

		DWORD dwTickCount = GetTickCount();
		if (dwX > 0)
		{
			m_dwLastMD_TC = dwTickCount;
			if (m_pServer->m_nRequestState == REQUEST_STATE_LIMITED && m_bMD == false)
			{
				SetLimitedFps(m_pServer);
			}
			m_bMD = true;
		}
		else if (GetTimeSpanSigned(m_dwLastMD_TC, dwTickCount) > 3000)
		{
			if (m_pServer->m_nRequestState == REQUEST_STATE_LIMITED && m_bMD == true)
			{
				SetLimitedFps(m_pServer);
			}
			m_bMD = false;
			m_wMD_X = 0;
			m_wMD_Y = 0;
		}
		m_dwLastPictureConfirmedTime = dwTickCount;

		if (   pict.GetPictureType() == SPT_GOP_PICTURE
			&& pict.GetNumberOfPictures()>0)
		{
			if (m_nCurrentPicture == SINGLE_PICTURE)// delete single pictures
			{
				m_GOPs.Lock();
				m_GOPs.DeleteAll();					
				m_MDpoints.RemoveAll();
				m_GOPs.Unlock();
				CAutoCritSec acs(&m_csPictureRecord);
				WK_DELETE(m_pPictureRecord);
				m_nCurrentPicture = NO_PICTURE;
			}

			if (m_iPictureRequests>0)
			{
				m_iPictureRequests--;
			}
			m_idArchive = idArchive;

			m_GOPs.Lock();
			m_GOPs.Add(new CIPCPictureRecord(pict));// add to queue
			m_MDpoints.Add(MAKELONG(dwX, dwY));
			m_GOPs.Unlock();

			int nTimeStep = (int)pict.GetFrameTimeStep();
			int nRoundedTimeStep = CalculateFrameStep(nTimeStep);
			if (m_nCurrentTimeStep != 0 && abs(m_nCurrentTimeStep-nRoundedTimeStep) > 2)
			{
				if (theApp.m_bTraceGOPTimer & TRACE_TIMER)
				{
					WK_TRACE(_T("Correcting GOP timer %d -> %d ms\n"), m_nCurrentTimeStep, nRoundedTimeStep);
				}
				KillGOPTimer(FALSE);
			}

			if (m_uGOPTimer == 0)					// start timer
			{
				m_nCurrentPicture = 0;
				SetGOPTimer(nRoundedTimeStep);
				if (theApp.m_bTraceGOPTimer & TRACE_TIMER)
				{
					WK_TRACE(_T("Starting GOP timer %d ms (%d µs)\n"), nRoundedTimeStep, nTimeStep);
				}
			}
		}
		else
		{
			if (theApp.m_bUseGOPThreadAlways)
			{
				KillGOPTimer(TRUE);

				int nTimeStep = MulDiv(1000, 1, m_nCurrentFPS);
				m_nCurrentTimeStep = nTimeStep;

				if (m_iPictureRequests>0)
				{
					m_iPictureRequests--;
				}
				PostMessage(WM_USER, WPARAM_EVENT_DO_REQUEST, RQ_DEFAULT);
				m_idArchive = idArchive;

				m_nCurrentPicture = SINGLE_PICTURE;
				m_GOPs.Lock();
				m_GOPs.Add(new CIPCPictureRecord(pict));// add to queue
				m_MDpoints.Add(MAKELONG(dwX, dwY));
				m_GOPs.Unlock();
				PulseEvent(m_hGOPTimerEvent);
			}
			else
			{
				KillGOPTimer(TRUE);

				CAutoCritSec acs(&m_csPictureRecord);
				if (m_iPictureRequests>0)
				{
					m_iPictureRequests--;
				}
				PostMessage(WM_USER, WPARAM_EVENT_DO_REQUEST, RQ_DEFAULT);
				if (m_stTimeStamp.GetSecond() != pict.GetTimeStamp().GetSecond())
				{
					m_bUpdateTitle = true;
				}
				m_stTimeStamp = pict.GetTimeStamp();
				m_bTimeValid = TRUE;
				m_idArchive = idArchive;
				WK_DELETE(m_pPictureRecord);
				m_pPictureRecord = new CIPCPictureRecord(pict);

				if (  !m_bDropped 
					|| pict.GetPictureType() == SPT_FULL_PICTURE 
					|| pict.GetPictureType() == SPT_GOP_PICTURE
					)
				{
	//				TRACE(_T("Decode %d, CamNr: %d, %s\n"),m_pMpeg4Decoder->GetDecoderQueueLength(), GetID().GetSubID(), pict.GetPictureType() == SPT_FULL_PICTURE ? _T("I"):_T("P"));
					BOOL bDecoded = m_pMpeg4Decoder->DecodeMpeg4FromMemory((BYTE*)pict.GetData(),	pict.GetDataLength());
					acs.Unlock();
					if(bDecoded)
					{
						m_pMpeg4Decoder->SetXY(dwX, dwY);
						ShowVideo();
						if (m_pStatistics)
						{
							m_pStatistics->AddValue(pict.GetDataLength());
							m_bUpdateTitle = true;
						}
					}
					else
					{
						WK_TRACE(_T("Dropped Picture %s(%x), PT:%d\n"), GetName(), GetID().GetSubID(), pict.GetPictureType());
					}
					m_bDropped = !bDecoded;
				}
				else
				{
					TRACE(_T("dropped %08lx\n"),pict.GetCamID().GetID());
					m_bDropped = true;
				}
			}
		}

//		Sleep(0);
		StorePicture(pict);
		m_iWaitingForPicture = 0;

		return TRUE;
	}
	else if (   pict.GetCompressionType() == COMPRESSION_YUV_422
		     || pict.GetCompressionType() == COMPRESSION_RGB_24)
	{
		// TODO! RKE: IMPLEMENT also yuv replay, select type of window
		CAutoCritSec acs(&m_csPictureRecord);
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = new CIPCPictureRecord(pict);
		CJpeg Jpeg;
		if(Jpeg.SetDIBData((LPBITMAPINFO)pict.GetData()))
		{
			CDC *pDC = GetDC();
			CRect rect;
			GetVideoClientRect(&rect);
			Jpeg.OnDraw(pDC, rect, m_rcZoom);
			ReleaseDC(pDC);
		}
		m_stTimeStamp = pict.GetTimeStamp();
		m_bTimeValid = TRUE;
		m_iWaitingForPicture = 0;
		return TRUE;
	}	
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::SetGOPTimer(UINT uElapse)
{
	if (theApp.m_bUseMMtimer)
	{
		TIMECAPS tc;
		MMRESULT mmr = 0;
		if (gm_wTimerRes == 0)
		{
			mmr = ::timeGetDevCaps(&tc, sizeof(TIMECAPS));
			if (mmr == TIMERR_NOERROR) 
			{
				gm_wTimerRes = min(max(tc.wPeriodMin, 5), tc.wPeriodMax);
				mmr = ::timeBeginPeriod(gm_wTimerRes);
			}

			if (mmr != TIMERR_NOERROR)
			{
				WK_TRACE(_T("No MM timer, using Window Timer\n"));
				theApp.m_bUseMMtimer = FALSE;
				theApp.m_bUseGOPThreadAlways = FALSE;
			}
		}
		if (theApp.m_bUseMMtimer)
		{
			if (m_uGOPTimer)
			{
				timeKillEvent(m_uGOPTimer);
			}
			m_uGOPTimer = ::timeSetEvent(uElapse, gm_wTimerRes, (LPTIMECALLBACK) m_hGOPTimerEvent, (DWORD)this, TIME_PERIODIC|TIME_CALLBACK_EVENT_PULSE);
		}
	}

	if (!theApp.m_bUseMMtimer || m_uGOPTimer == 0)
	{
		CSecID id = GetID();
		DWORD dwTimerID = MAKELONG(id.GetSubID(), id.GetGroupID()>>4);
		m_uGOPTimer = CWnd::SetTimer(dwTimerID, uElapse, NULL);
	}
	m_nCurrentTimeStep = uElapse;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::KillGOPTimer(BOOL bDeleteGOPs)
{
	if (m_uGOPTimer)
	{
		UINT uIDtemp = m_uGOPTimer;
		m_uGOPTimer = 0;				// set to zero, to make event id invalid
		if (theApp.m_bUseMMtimer)
		{
			::timeKillEvent(uIDtemp);
		}
		else
		{
			CWnd::KillTimer(uIDtemp);
		}
		if (theApp.m_bTraceGOPTimer & TRACE_TIMER)
		{
			WK_TRACE(_T("Killing GOP timer %d ms at picture %d, %d GOPS\n"), m_nCurrentTimeStep, m_nCurrentPicture, m_GOPs.GetCount());
		}
		if (bDeleteGOPs)
		{
			m_GOPs.Lock();
			m_GOPs.DeleteAll();
			m_MDpoints.RemoveAll();
			m_GOPs.Unlock();
			m_csPictureRecord.Lock();
			m_nCurrentPicture = NO_PICTURE;
			WK_DELETE(m_pPictureRecord);
			m_csPictureRecord.Unlock();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::ReleaseMMTimer()
{
	if (gm_wTimerRes)
	{
		timeEndPeriod(gm_wTimerRes);
		gm_wTimerRes = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
UINT CWndMpeg4::GOPThread(LPVOID lpParam)
{
	CWndMpeg4*pThis = (CWndMpeg4*)lpParam;
	XTIB::SetThreadName(pThis->m_orOutput.GetName());
	pThis->GOPThread();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::GOPThread()
{
	DWORD dwWait = 0;
	do
	{
		DWORD dwTimeOut = 1000;			// timeout for GOPs
		if (m_nCurrentPicture == SINGLE_PICTURE)
		{								// timeout for single pictures
			dwTimeOut = 50;
			int nGops = m_GOPs.GetSize();
			if (nGops > 1)				// too much pictures in queue
			{
				dwTimeOut = 10;
			}
		}

		dwWait = WaitForSingleObject(m_hGOPTimerEvent, dwTimeOut);
		if (m_uGOPTimer != STOP_GOP_THREAD)
		{
			if (dwWait == WAIT_OBJECT_0 && m_uGOPTimer)
			{
				DrawNextGOPPicture();	// GOP pictures are pulsed by MM timer
			}
			else if (m_nCurrentPicture == SINGLE_PICTURE)
			{
				DrawNextPicture();		// single pictures are pulsed at arrival
			}
		}
	}
	while (m_uGOPTimer != STOP_GOP_THREAD);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::DrawNextGOPPicture()
{
	if (theApp.m_bTraceGOPTimer & TRACE_PICTURES)
	{
		CString s = NameOfCurrentPictureState(m_nCurrentPicture, FALSE);
		if (!s.IsEmpty())
		{
			WK_TRACE(_T("%s\n"), s);
		}
	}

	CAutoCritSec acs(&m_csPictureRecord);
	if (m_nCurrentPicture == NO_PICTURE)	// no picture avaliable
	{										// force request
		PostMessage(WM_USER, WPARAM_EVENT_DO_REQUEST, RQ_FORCE_ONE);
		GetNextPictureOfGOP();				// try anyway
		if (m_pPictureRecord == NULL)		// really no picture
		{
			return;							// wait
		}
	}
	else if (m_pPictureRecord == NULL)		// no current picture
	{
		GetNextPictureOfGOP();				// get next current
	}

	int nNumPictures = 0;
	if (m_pPictureRecord)					// current picture
	{
		int nNumGOPS = m_GOPs.GetSize();	// get queue size
		nNumPictures = m_pPictureRecord->GetNumberOfPictures();
		if (nNumGOPS > 1)					// queue too long
		{									// reduce displayed gop pictures
			int nPicsRemaining = nNumPictures - m_nCurrentPicture;
			if (nPicsRemaining > 2)			// GOP is nearly beginning
			{
				nNumPictures -= (nNumGOPS);	
			}
			else							// GOP is nearly finnished
			{
				nNumPictures -= (nNumGOPS-1);
			}
		}
		else
		{
			DWORD dwCPUusage = theApp.GetAvgCPUusage();
			if (dwCPUusage > 90)			// reduce displayed GOP pictures
			{
				nNumPictures--;
				// 
			}
			if (dwCPUusage > 95)
			{
				nNumPictures--;
			}
		}

		if (m_nCurrentPicture >= nNumPictures) // 
		{
			if ((theApp.m_bTraceGOPTimer & TRACE_PICTURES) && nNumPictures < (int)m_pPictureRecord->GetNumberOfPictures())
			{
				WK_TRACE(_T("Reducing no of pics in GOP %d of %d %08lx\n"), 
					nNumPictures, m_pPictureRecord->GetNumberOfPictures(), GetCamID().GetID());
			}
			GetNextPictureOfGOP();
			nNumPictures = -1;
		}
	}

	if (m_pPictureRecord && m_nCurrentPicture > NO_PICTURE)
	{
		if (nNumPictures == -1)
		{
			nNumPictures = m_pPictureRecord->GetNumberOfPictures();
		}
		if (IsBetween(m_nCurrentPicture, 0, nNumPictures-1))
		{
			BOOL bDecoded = m_pMpeg4Decoder->DecodeMpeg4FromMemory((BYTE*)m_pPictureRecord->GetData(m_nCurrentPicture),
				m_pPictureRecord->GetDataLength(m_nCurrentPicture));

			if(bDecoded)
			{
				CPoint ptMD = m_pPictureRecord->GetMDPoint(0);
				m_pMpeg4Decoder->SetXY(m_wMD_X, m_wMD_Y);
				ShowVideo();
				if (m_pStatistics)
				{
					m_pStatistics->AddValue(m_pPictureRecord->GetDataLength(m_nCurrentPicture));
					m_bUpdateTitle = true;
				}
			}
			else
			{
				WK_TRACE(_T("Dropped Picture %d, %s(%x), PT:%d\n"), m_nCurrentPicture, GetName(), GetID().GetSubID(), m_pPictureRecord->GetPictureType());
			}
			m_nCurrentPicture++;
			m_bDropped = !bDecoded;
		}
		if (m_nCurrentPicture >= nNumPictures || m_bDropped)
		{
			GetNextPictureOfGOP();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::DrawNextPicture()
{
	CIPCPictureRecord*pPict = NULL;
	m_GOPs.Lock();
	if (m_GOPs.GetSize())
	{
		pPict = m_GOPs.GetAt(0);
		m_GOPs.Remove(pPict);
		DWORD dwMD = m_MDpoints.GetAt(0);
		m_MDpoints.RemoveAt(0);
		if (dwMD)
		{
			m_wMD_X = LOWORD(dwMD);
			m_wMD_Y = HIWORD(dwMD);
		}
	}
	m_GOPs.Unlock();

	if (pPict)
	{
		CAutoCritSec acs(&m_csPictureRecord);
		if (m_stTimeStamp.GetSecond() != pPict->GetTimeStamp().GetSecond())
		{
			m_bUpdateTitle = true;
		}
		m_stTimeStamp = pPict->GetTimeStamp();
		m_bTimeValid = TRUE;
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = pPict;

		BOOL bDecoded = m_pMpeg4Decoder->DecodeMpeg4FromMemory((BYTE*)pPict->GetData(),
			pPict->GetDataLength());

		if(bDecoded)
		{
			m_pMpeg4Decoder->SetXY(m_wMD_X, m_wMD_Y);
			ShowVideo();
			if (m_pStatistics)
			{
				m_pStatistics->AddValue(pPict->GetDataLength());
//				TRACE(_T("%d(%d), %.1f\n"), m_pStatistics->GetCurrentIndex(),  m_pStatistics->GetCurrentValues(), m_pStatistics->GetFPS());
				m_bUpdateTitle = true;
			}
		}
		else
		{
			WK_TRACE(_T("Dropped Picture %s(%x), PT:%d\n"), GetName(), GetID().GetSubID(), pPict->GetPictureType());
		}
	}
	else
	{
//			TRACE(_T("No Picture\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_uGOPTimer)
	{
		if (m_hGOPTimerEvent)
		{
			SetEvent(m_hGOPTimerEvent);
		}
		else
		{
			DrawNextGOPPicture();
		}
	}

	CWndLive::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::GetNextPictureOfGOP()
{
	m_GOPs.Lock();
	int nNumGops = m_GOPs.GetSize();
	if (m_pPictureRecord)
	{
		int iMax = 5;
		if (nNumGops > 0)
		{
			if (m_pServer->IsLocal())
			{
				iMax = 2;
			}
			else
			{
				iMax = 5;
			}
		}
		PostMessage(WM_USER, WPARAM_EVENT_DO_REQUEST, iMax);
	}

	if (nNumGops)
	{
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = m_GOPs.GetAt(0);
		m_GOPs.RemoveAt(0);
		DWORD dwMD = m_MDpoints.GetAt(0);
		m_MDpoints.RemoveAt(0);
		if (dwMD)
		{
			m_wMD_X = LOWORD(dwMD);
			m_wMD_Y = HIWORD(dwMD);
		}
		const CSystemTime& currentTime =m_pPictureRecord->GetTimeStamp();
		if (m_stTimeStamp.GetSecond() != currentTime.GetSecond())
		{
			m_bUpdateTitle = true;
		}
		m_stTimeStamp = currentTime;
		m_bTimeValid = TRUE;
		m_nCurrentPicture = 0;
	}
	else
	{
		m_nCurrentPicture = NO_PICTURE;
	}
	m_GOPs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndMpeg4::OnChangeVideo(WPARAM, LPARAM)
{
	SetMDValues((WORD)m_pMpeg4Decoder->GetX(),(WORD)m_pMpeg4Decoder->GetY());
	CRect rc;
	GetFrameRect(&rc);
	InvalidateRect(&rc,FALSE);
	m_bWasSomeTimesActive = TRUE;

	// neues bild anfordern
	if (    IsRequesting()
		&& !IsStreaming())
	{
		DoRequestPictures();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::ShowVideo()
{
	m_bWasSomeTimesActive = TRUE;
	CClientDC dc(this);
	SetMDValues((WORD)m_pMpeg4Decoder->GetX(),(WORD)m_pMpeg4Decoder->GetY());
	OnDraw(&dc);
	if (   m_uGOPTimer == 0
		&&  IsRequesting()
		&& !IsStreaming()
		)
	{
		DoRequestPictures();
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndMpeg4::OnDecodeVideo(WPARAM wParam, LPARAM)
{
	// md coordinaten vom decoder holen
	// neu zeichnen
	SetMDValues((WORD)m_pMpeg4Decoder->GetX(),(WORD)m_pMpeg4Decoder->GetY());
	CRect rc;
	GetFrameRect(&rc);
	InvalidateRect(&rc,FALSE);

	m_bWasSomeTimesActive = TRUE;
	
	// neues bild anfordern
	if (    m_uGOPTimer == 0
		&&  IsRequesting()
		&& !IsStreaming()
		)
	{
		DoRequestPictures();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnDraw(CDC* pDC)
{
	if (m_bWasSomeTimesActive)
	{
		CRect rect;
		GetVideoClientRect(rect);
		DrawCinema(pDC, &rect);
		m_csTracker.Lock();

		m_pMpeg4Decoder->OnDraw(pDC, rect, m_rcZoom);
		m_csTracker.Unlock();
	
		if (   (m_wMD_X>0)
			&& (m_wMD_Y>0)
			&& IsRequesting()
			)
		{
			DrawCross(pDC, m_wMD_X, m_wMD_Y, &rect);
		}
	}
	else
	{
		DrawFace(pDC);
	}

	if (   m_bUpdateTitle 
		|| m_wndToolBar.GetSize().cy < 0
		|| theApp.IsInMainThread())
	{
		DrawTitle(pDC);
	}
	DrawError(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
     m_pMpeg4Decoder->OnDraw(pDC,rcDest);
}
/////////////////////////////////////////////////////////////////////////////
// iQuality = 0 high speed, low quality, high compression
// iQuality = 4 low speed, high quality, low compression
void CWndMpeg4::OnFps_X_X(UINT nID) 
{
	switch (nID)
	{
		case ID_FPS_1_1: m_iQuality = 1; break;
		case ID_FPS_2_1: m_iQuality = 2; break;
		case ID_FPS_3_1: m_iQuality = 3; break;
		case ID_FPS_5_1: m_iQuality = 5; break;
		case ID_FPS_12_1: m_iQuality = 12; break;
		case ID_FPS_25_1: m_iQuality = 25; break;
		default: ASSERT(FALSE); return;
	}
	
	m_AvgStepMS.RemoveAll();
	DoRequestPictures(RQ_FORCE_ONE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnUpdateFps_X_X(CCmdUI* pCmdUI) 
{
	int nQuality = -1;
	switch (pCmdUI->m_nID)
	{
		case ID_FPS_1_1: nQuality = 1; break;
		case ID_FPS_2_1: nQuality = 2; break;
		case ID_FPS_3_1: nQuality = 3; break;
		case ID_FPS_5_1: nQuality = 5; break;
		case ID_FPS_12_1: nQuality = 12; break;
		case ID_FPS_25_1: nQuality = 25; break;
		default: ASSERT(FALSE); return;
	}

	pCmdUI->SetCheck(m_iQuality == nQuality);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMpeg4::CanAdjustFPS(int fps)
{
/*
	int r = m_pViewIdipClient->GetRequestedFPS(GetServerID(), WST_MPEG4);
	int c = GetRequestedFPS();
	int n = r - c + fps;
*/

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMpeg4::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::Set1to1(BOOL b1to1)
{
	CWndSmall::Set1to1(b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnVideoSaveReference() 
{
	CJpeg jpeg;
	CString sLocal,sRemote;

	sRemote = GetRemoteReferenceFileName();
	sLocal = GetLocalReferenceFileName();

	m_csPictureRecord.Lock();
	HGLOBAL hDIB = m_pMpeg4Decoder->GetDib()->GetHDIB();
	m_csPictureRecord.Unlock();
	if (hDIB)
	{
		jpeg.EncodeJpegToFile(hDIB, sRemote);
		GlobalFree(hDIB);
	}

	if (!m_pServer->IsLocal())
	{
		CIPCInputIdipClient* pInput = m_pServer->GetInput();
		if (pInput)
		{
			CFile file;
			LPBYTE pBuffer=NULL;
			DWORD dwLen=0;
			CFileException cfe;
			CFileStatus cfs;

			if (file.Open(sRemote,CFile::modeRead|CFile::shareDenyNone,&cfe))
			{
				if (file.GetStatus(cfs))
				{
					if (IsBetween(cfs.m_size, 0, MAX_DWORD))
					{
						dwLen = (DWORD)cfs.m_size;
					/*	if(dwLen > 100*1024*1024)
						{
							WK_TRACE(_T("#### DateLength: %d\n"), dwLen);
						}
					*/
						pBuffer = new BYTE[(UINT)dwLen];

						TRY
						{
							if (dwLen==file.Read(pBuffer,dwLen))
							{
								pInput->DoRequestFileUpdate(RFU_FULLPATH,
									sLocal,pBuffer,dwLen,FALSE);
							}
						}
						WK_CATCH(_T("cannot read temp reference image"));
					}
					else
					{
						WK_TRACE_ERROR(_T("Filesize out of range\n"));
					}
				}
				file.Close();
				Sleep(10);
			}
			WK_DELETE_ARRAY(pBuffer);
		}
	}
	CWndReference* pRW = m_pViewIdipClient->GetWndReference(GetServerID(),m_orOutput.GetName());
	if (pRW)
	{
		pRW->Reload();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// TODO decide create/delete decoder
	CWndLive::OnShowWindow(bShow, nStatus);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndMpeg4::GetTitleText(CDC* pDC)
{
	CString sTitle = CWndLive::GetTitleText(pDC);
	CString sStatistics, sDebug;
	if (m_pStatistics && theApp.m_bShowFPS)
	{
		CString sGops;
		sGops.Format(_T("G:%d "), m_GOPs.GetSize());
		DWORD dwTimeStep = m_nCurrentTimeStep;
		if (m_uGOPTimer)
		{
			int nValues = m_AvgStepMS.GetActualValues() * 1000;
			if (nValues)
			{
				dwTimeStep = MulDiv(m_AvgStepMS.GetSum(), 1, nValues);
			}
		}

		sStatistics.Format(_T("%02.02f(%d) %s"),
			m_pStatistics->GetFPS(), dwTimeStep,
			sGops);
	}

	if (theApp.m_bShowTitleOfSmallWindows & SHOW_DEBUG_INFO)
	{
		if (m_bMD)
		{
			sDebug += _T("MD ");
		}
		if (m_bForceHighPerformance)
		{
			sDebug += _T("HP ");
		}
	}

	sTitle = sStatistics + sDebug + sTitle;
	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMpeg4::CopyPictureForPrinting()
{
	//copy the current picture for printing
	ASSERT(m_pPrintPictureRecord == NULL);
	WK_DELETE(m_pPrintPictureRecord);

	BOOL bRet = TRUE;
	CAutoCritSec acs(&m_csPictureRecord);
	m_GOPs.Lock();
	int nNumGOPS = m_GOPs.GetSize();
	if(   m_pPictureRecord 
	   && (   m_pPictureRecord->GetPictureType() == SPT_FULL_PICTURE 
	       || m_pPictureRecord->GetPictureType() == SPT_GOP_PICTURE) )
	{
		m_pPrintPictureRecord = new CIPCPictureRecord(*m_pPictureRecord);
	}
	else if (nNumGOPS)
	{
		m_pPrintPictureRecord = new CIPCPictureRecord(*m_GOPs.GetAt(0));
	}
	else
	{
		TRACE(_T("No print image copied, requesting...\n"));
		m_bNeedPrintImage = true;
	}
	if(!m_pPrintPictureRecord)
	{
		bRet = FALSE;
	}
	m_GOPs.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMpeg4::OnDestroy()
{
	if (m_uGOPTimer)
	{
		KillGOPTimer(TRUE);
	}
	else if (m_nCurrentPicture == SINGLE_PICTURE)
	{
		CAutoCritSec acs(&m_csPictureRecord);
		m_nCurrentPicture = NO_PICTURE;
		m_GOPs.SafeDeleteAll();
	}

	if (m_pGOPThread)
	{
		m_uGOPTimer = STOP_GOP_THREAD;
		PulseEvent(m_hGOPTimerEvent);
		WaitForSingleObject(m_pGOPThread->m_hThread, 2000);
		WK_CLOSE_HANDLE(m_hGOPTimerEvent);
	}
	WK_DELETE(m_pGOPThread);

	CWndLive::OnDestroy();

	if (GetServer()->m_nRequestState == REQUEST_STATE_LIMITED)
	{
		int nCameras = m_pViewIdipClient->GetNrOfWindows(m_wServerID, WST_MPEG4) - 1;
		CalculateFps(GetServer(), nCameras);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndMpeg4::OnUser(WPARAM wParam, LPARAM lParam)
{
	WORD wEvent = LOWORD(wParam);
	switch (wEvent)
	{
		case WPARAM_UPDATE_TITLE:
		{
			CClientDC dc(this);
			DrawTitle(&dc);
		}break;
		default:
			CWndLive::OnUser(wParam, lParam);
		break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndMpeg4::GetPerformanceLevelString()
{
	CString str;
	int nLevel = theApp.GetPerformanceLevel();
	if (nLevel < INITIAL_PERFORMANCE_LEVEL)
	{
		str.Format(_T("%d*25"), INITIAL_PERFORMANCE_LEVEL - nLevel + 1);
	}
	else if (nLevel <= MAX_PERFORMANCE_LEVEL)
	{
		str.Format(_T("%d:%d:%d"), 
			gm_nQualities[nLevel][PL_ACTIVE],
			gm_nQualities[nLevel][PL_MOTION],
			gm_nQualities[nLevel][PL_NOMOTION]);
	}
	else
	{
		ASSERT(FALSE);
	}
	return str;
}