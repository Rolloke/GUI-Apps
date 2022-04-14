// LiveWindow.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "LiveWindow.h"
#include "CPanel.h"

#include "IPCOutputDVClient.h"
#include "IPCInputDVClient.h"
#include "Server.h"
#include "Mainframe.h"
#include ".\livewindow.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STOP_GOP_THREAD	0xffffffff

#define NO_PICTURE			-1
#define SINGLE_PICTURE		-2

CString gsEventError = _T("Entering %s in Thread %s: Event already used\n");
CString gsNoConfirmError = _T("%s(%08x) timeout. No confirm\n");

CWaitEvent::Wait(DWORD dwTimeOut, LPCTSTR sFkt, CSecID id)
{
	BOOL bResult = CEvent::Lock(dwTimeOut);
	if (!bResult)
	{
		WK_TRACE_WARNING(gsNoConfirmError, sFkt, id.GetID());
	}
	return bResult;
}

extern int g_iaCameraFPS[CAMERA_FPS_STEPS];
const int g_nLevelArraySize = MAX_PERFORMANCE_LEVEL+1;
#define PL_ACTIVE	0
#define PL_MOTION	1
#define PL_NOMOTION	2
static const int g_nQualities[g_nLevelArraySize][3] = 
{// Active, MD,  no MD,    Level
	{   25,  5,     5}, //  0	// positive value mean frames per second
	{   25,  5,    -5}, //  1	// negative values mean 100 ms timeout value
	{    5,  5,    -5}, //  2	// factors. the negative sign is used as indicator
	{    5, -5,    -5}, //  3	// e.g. -5: 100 * 5 = 500 ms timeout
	{   -5, -5,    -5}, //  4	//  between picture requests
	{   -5, -5,   -10}, //  5
	{   -5,-10,   -10}, //  6
	{  -10,-10,   -10}, //  7
	{  -10,-10,   -20}, //  8
	{  -10,-20,   -20}, //  9
	{  -20,-20,   -20}  // 10
};

static _TCHAR szTimer[] = _T("Timer%02d");


#define TRACE_TIMER		0x00000001
#define TRACE_PICTURES	0x00000010

UINT CLiveWindow::gm_wTimerRes = 0;
BOOL CLiveWindow::gm_bTraceGOPTimer = FALSE;
int CLiveWindow::gm_nHighPerfWnds = 0;


IMPLEMENT_DYNAMIC(CLiveWindow,CDisplayWindow)
/////////////////////////////////////////////////////////////////////////////
// CLiveWindow
CLiveWindow::CLiveWindow(CMainFrame* pParent, CServer* pServer, const CIPCOutputRecord &or)
	:	CDisplayWindow(pParent, pServer)
{
	m_orOutput = or;
//	TRACE(_T("%s"), or.GetName());
	m_stLastAlarmTime.GetLocalTime();
	m_bLastAlarmTime = FALSE;

	m_pStatistics = NULL;
	if (theApp.ShowFPS())
	{
		m_pStatistics = new CStatistics(50);
	}
	m_bShowable = TRUE;
	m_bSetEventUsed = FALSE;
	m_bGetEventUsed = FALSE;

	DWORD dwDiff = REQUEST_TIMEOUT_SLOW;
	dwDiff -= (REQUEST_TIMEOUT_SLOW/8) * or.GetID().GetSubID();
	m_dwLastRequest = GetTickCount() - dwDiff;
	
	m_iBrightness = 50;
	m_iContrast	  = 50;
	m_iSaturation = 50;
	m_nCompression= COMPRESSION_1;
	m_nCameraFPS = g_iaCameraFPS[0];
	m_nCameraPTZType = 0;
	m_dwCameraPTZID = 0;
	m_nCameraPTZComPort = 0;
	m_iMDMaskSensitivity = 1;
	m_iMDAlarmSensitivity = 1;
	
	m_iAlarmCallEvent = ALARMCALL_EVENT_UNKNOWN;
	m_iAlarmCallEventByCardType = CARD_TYPE_UNKNOWN;

	m_bAlarmActive = FALSE;

	if (m_pServer && m_pServer->IsInputConnected())
	{
		CIPCInputDVClient* pCIPCInputDVClient = m_pServer->GetInput();
		m_bAlarmActive = pCIPCInputDVClient->GetAlarmState(GetID().GetSubID());
	}
	else
	{
		TRACE(_T("CLiveWindow input client not yet connected\n"));
	}
	m_bCanRectSearch = FALSE;

	m_ccType					= CCT_UNKNOWN;
	m_dwPTZFunctions			= PTZF_DEFAULT;
	m_dwPTZFunctionsEx			= 0;

	m_LastCameraControlCmd		= CCC_INVALID;
	m_LastCameraControlCmdStop	= CCC_INVALID;
	m_dwPTZSpeed				= 5;
	if (IsPTZ())
	{
		RequestPTZParams();
		RequestPTZ(CCC_FOCUS_AUTO, 0, 0);
		RequestPTZ(CCC_IRIS_AUTO, 0, 0);
	}

	m_uGOPTimer = 0;
	m_AvgStepMS.SetNoOfAvgValues(10);
	m_nCurrentTimeStep = 0;
	m_nCurrentPicture = -1;
	m_hGOPTimerEvent = NULL;
	m_pGOPThread = NULL;
	m_bHighPerfWnd = FALSE;

	if (theApp.m_bUseMMtimer)
	{
		m_hGOPTimerEvent = CreateEvent(NULL, TRUE, FALSE, GetName());
		m_pGOPThread = AfxBeginThread(GOPThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0);
		m_pGOPThread->m_bAutoDelete = FALSE;
	}

	gm_bTraceGOPTimer = theApp.GetIntValue(_T("TraceGOPTimer"), 0);

#ifdef _SWITCHABLE_LIVE_WINDOWS_
	m_pBtnCancel = NULL;
#endif // _SWITCHABLE_LIVE_WINDOWS_
}
/////////////////////////////////////////////////////////////////////////////
CLiveWindow::~CLiveWindow()
{
//	TRACE(_T("CLiveWindow delete\n"));
	if (m_pGOPThread)
	{
		m_hWnd = NULL;
		PulseEvent(m_hGOPTimerEvent);
		WaitForSingleObject(m_pGOPThread->m_hThread, 2000);
		WK_CLOSE_HANDLE(m_hGOPTimerEvent);
	}
	WK_DELETE(m_pGOPThread);
	m_evConfirmSet.SetEvent();
	m_evConfirmGet.SetEvent();

	Sleep(10);

	m_GOPs.SafeDeleteAll();
	WK_DELETE(m_pStatistics);

#ifdef _SWITCHABLE_LIVE_WINDOWS_
	WK_DELETE(m_pBtnCancel);
#endif // _SWITCHABLE_LIVE_WINDOWS_
}
/////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CLiveWindow, CDisplayWindow)
	//{{AFX_MSG_MAP(CLiveWindow)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER, OnUser)
	//}}AFX_MSG_MAP
#ifdef _SWITCHABLE_LIVE_WINDOWS_
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDCANCEL, OnCancel)
#endif // _SWITCHABLE_LIVE_WINDOWS_
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLiveWindow message handlers
/////////////////////////////////////////////////////////////////////////////
CSecID CLiveWindow::GetID() const
{
	return m_orOutput.GetID(); 
}
/////////////////////////////////////////////////////////////////////////////
CString CLiveWindow::GetName() const
{		    
	return m_orOutput.GetName();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetName(const CString &sName)
{		    
	BOOL bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_NAME, sName);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
CString CLiveWindow::GetTitle(CDC* pDC)
{		    
	CString sName, sID;
	int nID = GetID().GetSubID()+1;
	sID.Format(_T("%02d"), nID);
	if (sID != GetName())
	{
		sName.Format(_T("%s "), GetName());
	}

	sID.Format(_T("(%02d) "), nID);
	CString sTime = GetTimeString();
	CString sTitle;
	sTitle = sID + sName + sTime;

	// Fenstertitel ggf. anpassen, falls pDC vorhanden
	if (pDC)
	{
		CRect rect;
		GetClientRect(rect);
		// Ist der Titel zu lang
		CSize size = pDC->GetOutputTextExtent(sTitle);
		if (size.cx > rect.Width())
		{
			// Zeit ohne Sekunden anzeigen?
			sTitle = sID + sName + sTime.Left(sTime.GetLength()-3);
			// Ist er immer noch zu lang
			size = pDC->GetOutputTextExtent(sTitle);
			if (size.cx > rect.Width())
			{
				// Namen weglassen (Zuordnung ist über ID gegeben)
				sTitle = sID + sTime;
			}
			else
			{
				// Wenn nur die Sekunden zu viel sind,
				// können sie aber auch abgeschnitten werden
				sTitle = sID + sName + sTime;
			}
		}
	}

	if (m_pStatistics)
	{
		CString s;
		s.Format(_T(" / %02.02f, %d"),m_pStatistics->GetFPS(), m_iRequests);
		sTitle += s;
	}

	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::GetFooter(CByteArray& Array)
{
	// Ist gezoomt?
	if (!m_rcZoom.IsRectEmpty())
	{
		Array.Add(SYMBOL_ZOOM);
	}

	// UVV-Kassen Symbol
	if (m_sMode == CSD_UVV)
	{
		Array.Add(SYMBOL_UVV);
	}

	// Ist Timer Active
	if (m_sIsTimerActive == CSD_OFF)
	{
		Array.Add(SYMBOL_TIMER);
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CLiveWindow::GetDefaultText()
{
	CString s;
	s.LoadString(IDS_WAIT);
	return GetName() + _T("\n") + s;
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CLiveWindow::GetOSDColor()
{
	COLORREF col;

	if (IsActive())
		col = m_bAlarmActive ? RGB(192,0,0) : RGB(0,192,0);
	else
		col = m_bAlarmActive ? RGB(128,0,0) : RGB(0,128,0);
	return col;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IsShowable()
{
	return m_bShowable;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::SetOutputRecord(const CIPCOutputRecord &or)
{
	m_orOutput = or;
	// Achtung wir sind im Main Thread!
	CPanel* pPanel = theApp.GetPanel();
	if(pPanel)
	{
		pPanel->PostMessage(WM_UPDATE_MENU,1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::UpdateAlarmState()
{
	if (m_pServer && m_pServer->IsInputConnected())
	{
		CIPCInputDVClient* pCIPCInputDVClient = m_pServer->GetInput();
		m_bAlarmActive = pCIPCInputDVClient->GetAlarmState(GetID().GetSubID());
		if (!m_bAlarmActive)
		{
			SetMDValues(0,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IndicateAlarmNr(BOOL bAlarmActive, WORD wX, WORD wY)
{
	m_bAlarmActive = bAlarmActive;

	if (m_bAlarmActive)
	{
		SetMDValues(wX,wY);

		// request more pictures
		if (IsMpeg4())
		{
			// the receiver should display the active window with 25 fps
			// additional requests for the other windows reduce the active windows fps
			if (!theApp.IsReceiver())
			{
				Request(3);	// local it works fine
			}
		}
		else
		{
			Request(3);
		}

		if (m_pPictureRecord)
		{
			m_stLastAlarmTime = m_pPictureRecord->GetTimeStamp();
			m_bLastAlarmTime = TRUE;
		}
	}
	else
	{
		SetMDValues(0,0);
	}

//	DoRedraw();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IsPanelActive()
{
	BOOL bPanelActive = FALSE;
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		bPanelActive = pPanel->GetActiveCamera() == m_orOutput.GetID().GetSubID();
	}
	return bPanelActive;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnChangeVideo()
{
	if (   IsWindowVisible()
		&& IsMpeg4()
		&& m_uGOPTimer == 0)
	{
		Request();	// the Mpeg4 are requested here
		// the request thread performs the requests only if there are less than 2 fps
		// it has also a controlling function if the request/receive timeouts are too long.
	}
	CDisplayWindow::OnChangeVideo();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetHighPerf(BOOL bSet)
{
	int nMaxHPW = -theApp.GetPerformanceLevel();
	if (nMaxHPW > 0)
	{
		if (bSet)
		{
			if (gm_nHighPerfWnds < nMaxHPW)
			{
				if (m_bHighPerfWnd == FALSE)
				{
					TRACE(_T("set wnd:%d to high performance\n"), GetID().GetSubID());
					gm_nHighPerfWnds++;
					m_bHighPerfWnd = bSet;
				}
			}
			else if (gm_nHighPerfWnds > nMaxHPW)
			{
				bSet = FALSE;
			}
		}
		if (!bSet)
		{
			if (m_bHighPerfWnd)
			{
				TRACE(_T("set wnd:%d to low performance\n"), GetID().GetSubID());
				m_bHighPerfWnd = FALSE;
				gm_nHighPerfWnds--;
			}
		}
	}
	return m_bHighPerfWnd;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::Request(int iMax/*=-1*/)
{
	BOOL bRet = CDisplayWindow::Request(iMax);	// MotionDetection is resetted by tickcount of 2000 ms here
	// TRACE(_T("CLiveWindow::Request (Id=0x%x)\n"), GetID());

	// ML 11.05.2004
	// Solange noch kein Bild gekommen ist, dürfen neue Bilder angefordert werden.
	// Ansonsten kommt es häufig zum 'Bitte warten...', da 5 vergebliche Requests verschickt wurden.
	// Nach dem Anlegen des Fensters bleiben die ersten Requests irgendwo hängen!?
	if (   !m_pServer 
		|| !m_pServer->IsOutputConnected()
		|| !IsWindowVisible())
	{
		return bRet;
	}

	DWORD dwTimeSpanToLastRequest = GetTimeSpan(m_dwLastRequest);
	BOOL bPanelActive = IsPanelActive();
	BOOL bFast = m_bAlarmActive || (IsActive()) || bPanelActive;

	if (IsJPEG())
	{
		if (   (m_iRequests > 0) 
			&& (GetTimeSpan(m_dwLastPictureReceivedTC) > 10000))
		{
			m_dwLastPictureReceivedTC = GetTickCount();
			WK_TRACE_WARNING(_T("No response to outstanding picture requests for more then 10 seconds...generating new requests soon (Id=0x%x)\n"), GetID());
			m_iRequests = 0;
		}

		if (iMax <= -1)
		{
			iMax = bFast ? 3 : 1;
		}
		if (m_iRequests < iMax)
		{
			DWORD dwTimeOut = bFast ? REQUEST_TIMEOUT_FAST : REQUEST_TIMEOUT_SLOW;
			if (m_pPictureRecord==NULL)
			{
				dwTimeOut = 0;
			}
			m_dwWaitTimeout = dwTimeOut;

			BOOL bRequest = dwTimeSpanToLastRequest > dwTimeOut;
			if (bRequest)
			{
				if (m_pServer && m_pServer->IsOutputConnected())
				{
					while (m_iRequests < iMax)
					{
						CIPCOutputDVClient* pOutput = m_pServer->GetOutput();
						pOutput->DoRequestNewJpegEncoding(GetID(),RESOLUTION_2CIF,COMPRESSION_1, 1,(DWORD)-1); 
						m_iRequests++;
						if (m_bWasSomeTimesActive && !bPanelActive)
						{	// do not request too fast
							break;
						}
					}
					bRet = TRUE;
				}
			}
			else
			{

			}
		}
	}
	else if (IsMpeg4())
	{
		BOOL bRequestThread = FALSE;
		if (iMax == -2)				// -2 means RequestThread is used to control here
		{
			bRequestThread = TRUE;
			// Sind mehr als 10 Sekunden Picture-Requests ausstehend. dann setzte 'm_iRequests' wieder auf '0',
			// damit weitere Requests abgesetzt werden können.
			if (   (m_iRequests > 0) 
				&& (GetTimeSpan(m_dwLastPictureReceivedTC) > 10000))
			{
				m_dwLastPictureReceivedTC = GetTickCount();
				WK_TRACE_WARNING(_T("No response to outstanding picture requests for more then 10 seconds...generating new requests soon (Id=0x%x)\n"), GetID());
				m_iRequests = 0;
			}
			else if (m_dwWaitTimeout)	// timeout value for request of 1 frame per timeout value
			{
				if (dwTimeSpanToLastRequest > m_dwWaitTimeout)
				{
//					WK_TRACE(_T("RequestTimeout:%d of %d ms, %s\n"), dwTimeSpanToLastRequest, m_dwWaitTimeout, GetName());
//					m_iRequests = 0;
				}
				else
				{
					return TRUE;
				}
			}
			else if (dwTimeSpanToLastRequest > 5000)
			{							// safety request after 5 seconds
				WK_TRACE(_T("RequestTimeout:%d ms, %s\n"), dwTimeSpanToLastRequest, GetName());
//				m_iRequests = 0;
			}
			else
			{
				return TRUE;
			}
		}

		int nPerfLevel = theApp.GetPerformanceLevel();
		if (nPerfLevel < 0)
		{
			nPerfLevel = 0;
		}

		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pOutput = m_pServer->GetOutput();
			DWORD dwTimeOut = 0;
			int iFps			= 1;
			int iIFrameInt		= 1;
			int iMaxRequests	= 1;

			if (bPanelActive)				// the active window receives highest fps
			{
				iFps = g_nQualities[nPerfLevel][PL_ACTIVE];
				iMaxRequests = 5;
			}
			else
			{
				if (IsMotionDetected()>0)	// pictures with motion detected less fps
				{
					if (SetHighPerf(TRUE))	// MD may have high performance
					{
						iFps = g_nQualities[nPerfLevel][PL_ACTIVE];
						iMaxRequests = 5;
					}
					else
					{
						iFps = g_nQualities[nPerfLevel][PL_MOTION];
						iMaxRequests = theApp.IsReceiver() ? 1 : 3;
					}
				}
				else						// other windows receive least fps
				{
					SetHighPerf(FALSE);		// no MD, no need for high performance
					iFps = g_nQualities[nPerfLevel][PL_NOMOTION];
					iMaxRequests = 1;
				}
			}

			if (iFps == 25)
			{	
				iIFrameInt = 5;		// 25 fps means I and P-frames
				dwTimeOut  = 0;		// no timeout
			}
			else if (iFps < 0)
			{
				dwTimeOut = 100 * -iFps; //
				iFps = 1;
			}
			else
			{
				dwTimeOut = 0; 
			}

			if (iMax <= -1)			// -1, -2: max request is determined here
			{
				iMax = iMaxRequests;
			}

			if (!bRequestThread)	// not invoked from RequestThread
			{
				if (dwTimeOut > dwTimeSpanToLastRequest)// if Timeout value is determined
				{
					iMax = 0;		// do not request pictures
				}
			}
//			just to DEBUG behaviour
//			if (m_dwWaitTimeout != dwTimeOut && 
//				GetID().GetSubID() == 1)
//			{
//				WK_TRACE(_T("changing WaitTimeOut of %s from %d to %d, MD:%d\n"), GetName(), m_dwWaitTimeout, dwTimeOut, IsMotionDetected());
//			}
			m_dwWaitTimeout = dwTimeOut;
			m_nCurrentFPS = iFps;

			TRACE(_T("Requesting %d MPEG4 %d fps:%d\n"), GetID().GetSubID(), iFps, iMax - m_iRequests);
			while (m_iRequests < iMax)
			{
				pOutput->DoRequestStartVideo(GetID(),
					RESOLUTION_2CIF,
					COMPRESSION_1,
					COMPRESSION_MPEG4,
					iFps,
					iIFrameInt,
					(DWORD)0
					);
				m_iRequests++;
				bRet++;
				if (m_bWasSomeTimesActive && !bPanelActive)
				{	
					break;			// do not request too fast
				}
			}
			if (bRet)
			{
//			just to DEBUG behaviour
//				if (GetID().GetSubID() == 1)
//				{
//					WK_TRACE(_T("RequestTimeout(%s):%d of %d ms, MD:%d, RT:%d, RQ:%d\n"), GetName(), dwTimeSpanToLastRequest, m_dwWaitTimeout, IsMotionDetected(), bRequestThread, bRet);
//				}
				m_dwLastRequest = GetTickCount();
				bRet = TRUE;
			}
		}
	}
	else	// Q
	{
		CIPCOutputDVClient* pOutput = m_pServer->GetOutput();
		int iFps			= 5;
		int iIFrameInt		= 1;
		bRet = 0;
		iMax = 2;
		while (m_iRequests < iMax)
		{
			pOutput->DoRequestStartVideo(GetID(),
				RESOLUTION_4CIF,
				COMPRESSION_NONE,
				COMPRESSION_YUV_422,
				iFps,
				iIFrameInt,
				(DWORD)0
				);
			m_iRequests++;
			bRet++;
			if (m_bWasSomeTimesActive && !bPanelActive)
			{	
				break;			// do not request too fast
			}
		}
		if (bRet)
		{
			m_dwLastRequest = GetTickCount();
			bRet = TRUE;
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::PictureData(const CIPCPictureRecord *pPict, DWORD dwX, DWORD dwY)
{
	// TRACE(_T("Coordinates:%d, %d\n"), dwX, dwY);
	ASSERT(pPict != NULL);
	BOOL bMPEG4    = FALSE;
	BOOL bUseCJpeg = FALSE;
	const CIPCPictureRecord *pDiplayPict = NULL;
	switch (pPict->GetCompressionType())
	{
		case COMPRESSION_MPEG4:
			if (m_pGOPThread == NULL)
			{
				pDiplayPict = pPict;
			}
			bMPEG4 = TRUE;
			CreateMpegDecoder();
			break;
		case COMPRESSION_JPEG: case COMPRESSION_YUV_422: case COMPRESSION_RGB_24:
			pDiplayPict = pPict;
			bUseCJpeg = TRUE;
			break;
	}

	BOOL bRet = CDisplayWindow::PictureData(pDiplayPict, dwX, dwY);
	//TRACE(_T("confirm %s %s\n"),GetName(),pPict->GetTimeStamp().GetTime());
	if (bRet)
	{
		if (bUseCJpeg)
		{
			ShowVideo();
			if (m_pStatistics)
			{
				m_pStatistics->AddValue(pPict->GetDataLength());
			}
		}
		else if(bMPEG4)
		{
			if (   pPict->GetPictureType() == SPT_GOP_PICTURE
				&& pPict->GetNumberOfPictures()>0)
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

				m_GOPs.Lock();
				m_GOPs.Add(new CIPCPictureRecord(*pPict));// add to queue
				m_MDpoints.Add(MAKELONG(dwX, dwY));
				m_GOPs.Unlock();

				int nTimeStep = (int)pPict->GetFrameTimeStep();
				int nRoundedTimeStep = CalculateFrameStep(nTimeStep);
				if (m_nCurrentTimeStep != 0 && abs(m_nCurrentTimeStep-nRoundedTimeStep) > 2)
				{
					KillGOPTimer(FALSE);
				}

				if (m_uGOPTimer == 0)					// start timer
				{
					m_nCurrentPicture = 0;
					SetGOPTimer(nRoundedTimeStep);
				}
			}
			else
			{
				KillGOPTimer(TRUE);

				int nTimeStep = MulDiv(1000, 1, m_nCurrentFPS);
				m_nCurrentTimeStep = nTimeStep;

				PostMessage(WM_USER, ID_REQUEST, -1);

				m_nCurrentPicture = SINGLE_PICTURE;
				m_GOPs.Lock();
				m_GOPs.Add(new CIPCPictureRecord(*pPict));// add to queue
				m_MDpoints.Add(MAKELONG(dwX, dwY));
				m_GOPs.Unlock();
				PulseEvent(m_hGOPTimerEvent);
			}
		}
		Sleep(0);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
int CLiveWindow::CalculateFrameStep(int nTimeStep)
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
void CLiveWindow::SetGOPTimer( UINT uElapse)
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
				gm_wTimerRes = min(max(tc.wPeriodMin, 10), tc.wPeriodMax);
				mmr = ::timeBeginPeriod(gm_wTimerRes);
			}

			if (mmr != TIMERR_NOERROR)
			{
				TRACE(_T("No MM timer, using Window Timer\n"));
				theApp.m_bUseMMtimer = FALSE;
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

	if (!theApp.m_bUseMMtimer)
	{
		m_uGOPTimer = CWnd::SetTimer(110, uElapse, NULL);
	}
	m_nCurrentTimeStep = uElapse;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::KillGOPTimer(BOOL bDeleteGOPs)
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

		WK_TRACE(_T("Killing GOP timer at picture %d and %d GOPs\n"), m_nCurrentPicture, m_GOPs.GetSize());
		if (bDeleteGOPs)
		{
			m_GOPs.Lock();
			m_GOPs.DeleteAll();
			m_MDpoints.RemoveAll();
			m_GOPs.Unlock();
			m_csPictureRecord.Lock();
			m_nCurrentPicture = NO_PICTURE;
//			WK_DELETE(m_pPictureRecord);	// don´t delete: EraseBkGnd
			m_csPictureRecord.Unlock();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::ReleaseMMTimer()
{
	if (gm_wTimerRes)
	{
		timeEndPeriod(gm_wTimerRes);
		gm_wTimerRes = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CALLBACK CLiveWindow::GOPTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{ 
	CLiveWindow*pThis = (CLiveWindow*)dwUser;
	XTIB * pTib = XTIB::GetTIB();
	if (pTib)
	{
		if (pTib->pvArbitrary == NULL)
		{
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			XTIB::SetThreadName(_T("CLiveWindow GOP Timer"));
		}
	}
	if (pThis->m_uGOPTimer)
	{
		pThis->OnTimer(wTimerID);
	}
	else
	{
		LPCTSTR pszName = XTIB::GetThreadName();
		WK_TRACE(_T("GOPTimer event is 0 %s: %d, %s\n"), pszName ? pszName : _T("Unnamed"), wTimerID, pThis->GetName());
	}
}
/////////////////////////////////////////////////////////////////////////////
UINT CLiveWindow::GOPThread(LPVOID lpParam)
{
	CLiveWindow*pThis = (CLiveWindow*)lpParam;
	XTIB::SetThreadName(pThis->m_orOutput.GetName());
	pThis->GOPThread();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::GOPThread()
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
void CLiveWindow::OnTimer(UINT nIDEvent)
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
	CDisplayWindow::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::GetNextPictureOfGOP()
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
		PostMessage(WM_USER, ID_REQUEST, iMax);
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
			SetMDValues(LOWORD(dwMD), HIWORD(dwMD));
		}
		m_nCurrentPicture = 0;
	}
	else
	{
		m_nCurrentPicture = NO_PICTURE;
	}
	m_GOPs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::DrawNextGOPPicture()
{
	CAutoCritSec acs(&m_csPictureRecord);
	if (m_nCurrentPicture == NO_PICTURE)	// no picture avaliable
	{										// force request
		PostMessage(WM_USER, ID_REQUEST, 1);
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
		BOOL bDecoded = FALSE;
		if (IsBetween(m_nCurrentPicture, 0, nNumPictures-1))
		{
			bDecoded = m_pMpeg->DecodeMpeg4FromMemory((BYTE*)m_pPictureRecord->GetData(m_nCurrentPicture),
				m_pPictureRecord->GetDataLength(m_nCurrentPicture));

			if(bDecoded)
			{
				CPoint ptMD = m_pPictureRecord->GetMDPoint(0);
				SetMDValues((WORD)ptMD.x, (WORD)ptMD.y);
				ShowVideo();
				if (m_pStatistics)
				{
					m_pStatistics->AddValue(m_pPictureRecord->GetDataLength(m_nCurrentPicture));
				}
			}
			else
			{
				WK_TRACE(_T("Dropped Picture %d, %s(%x), PT:%d\n"), m_nCurrentPicture, GetName(), GetID().GetSubID(), m_pPictureRecord->GetPictureType());
			}
			m_nCurrentPicture++;
		}
		if (m_nCurrentPicture >= nNumPictures || !bDecoded)
		{
			GetNextPictureOfGOP();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::DrawNextPicture()
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
			SetMDValues(LOWORD(dwMD), HIWORD(dwMD));
		}
	}
	m_GOPs.Unlock();

	if (pPict)
	{
		CAutoCritSec acs(&m_csPictureRecord);
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = pPict;

		BOOL bDecoded = m_pMpeg->DecodeMpeg4FromMemory((BYTE*)pPict->GetData(),
			pPict->GetDataLength());

		if(bDecoded)
		{
			ShowVideo();
			if (m_pStatistics)
			{
				m_pStatistics->AddValue(pPict->GetDataLength());
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
BOOL CLiveWindow::SetBrightness(int iBrightness)
{
	CString sValue;
	BOOL	bResult = FALSE;

	// 0..255
//	sValue.Format(_T("%d"),(256-100) / 2 + iBrightness);
//	sValue.Format(_T("%d"), 256/2 + (256*(iBrightness - 50)) / 100);
// Warum so kompliziert??? Bei der Umkehr +0,5 reicht und ergibt eindeutige Werte
//ML 20.04.2004 sValue.Format(_T("%d"), iBrightness * 255 / 100);
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		sValue.Format(_T("%d"), (int)((double)iBrightness * 255.0 / 100.0 + 0.5));
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_BRIGHTNESS,sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetContrast(int iContrast)
{
	CString sValue;
	BOOL	bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		// 0..512
		//	sValue.Format(_T("%d"), 512/2 + (512*(iContrast - 50)) / 100);
		// Warum so kompliziert??? Bei der Umkehr +0,5 reicht und ergibt eindeutige Werte
		//ML 20.04.2004	sValue.Format(_T("%d"), iContrast * 512 / 100);
		sValue.Format(_T("%d"), (int)((double)iContrast * 512.0 / 100.0 + 0.5));
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_CONTRAST,sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetSaturation(int iSaturation)
{
	CString sValue;
	BOOL	bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		// 0..512
		//	sValue.Format(_T("%d"), 512/2 + (512*(iSaturation - 50)) / 100);
		// Warum so kompliziert??? Bei der Umkehr +0,5 reicht und ergibt eindeutige Werte
		//ML 20.04.2004	sValue.Format(_T("%d"), iSaturation * 512 / 100);
		sValue.Format(_T("%d"), (int)((double)iSaturation * 512.0 / 100.0 + 0.5));

		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_SATURATION,sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}	
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetCameraTermination(BOOL bCameraTermination)
{
	CString sValue;
	BOOL	bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (bCameraTermination)
			sValue = CSD_ON;
		else
			sValue = CSD_OFF;

		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_TERM,sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}	
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetCompression(int nCompression)
{
	BOOL bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				CString sValue;
				sValue.Format(_T("%d"), nCompression);
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_COMPRESSION,sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}	
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetCameraFPS(int nFPS)
{
	BOOL bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				CString sValue;
				sValue.Format(_T("%d"), nFPS);
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_FPS,sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}	
		}	
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetCameraPTZType(int nCameraPTZType)
{
	BOOL bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				CString sValue = NameOfEnum((CameraControlType)nCameraPTZType);
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(), CSD_CAM_PTZ_TYPE, sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
			if (bResult && nCameraPTZType != CCT_UNKNOWN)
			{
				RequestPTZParams();
				RequestPTZ(CCC_FOCUS_AUTO, 0, 0);
				RequestPTZ(CCC_IRIS_AUTO, 0, 0);
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetCameraPTZID(DWORD dwCameraPTZID)
{
	BOOL	bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				CString sValue;
				sValue.Format(_T("%u"), dwCameraPTZID);
				pDVOC->DoRequestSetValue(m_orOutput.GetID(), CSD_CAM_PTZ_ID, sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					RequestPTZParams();
					bResult = TRUE;
				}
			}	
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetCameraPTZComPort(int nCameraPTZComPort)
{
	BOOL	bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				CString sValue;
				sValue.Format(_T("%d"), nCameraPTZComPort);
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(), CSD_CAM_PTZ_COMPORT, sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					RequestPTZParams();
					bResult = TRUE;
				}
			}	
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::ConfirmGetValue(const CString &sKey,const CString &sValue)
{
	TRACE(_T("CLiveWindow::ConfirmGetValue %s,%s\n"),sKey,sValue);
	if (sKey == CSD_SATURATION)
	{
		// 0...512 in Prozent umrechnen
//		double dValue = atof(sValue);
//		double dValue = (double)_ttoi(sValue);			//<-- Liefert falsche Ergebnisse
//		m_iSaturation = (int)(dValue*100.0/512.0+0.5);
		int iValue = _ttoi(sValue);
		m_iSaturation = MulDiv(iValue, 100, 512);

		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_CONTRAST)
	{
		// 0...512 in Prozent umrechnen
//		double dValue = atof(sValue);
//		double dValue = (double)_ttoi(sValue);			//<-- Liefert falsche Ergebnisse
//		m_iContrast = (int)(dValue*100.0/512.0+0.5);
		int iValue = _ttoi(sValue);
		m_iContrast = MulDiv(iValue, 100, 512);

		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_BRIGHTNESS)
	{
		// 0...255 in Prozent umrechnen
//		double dValue = atof(sValue);
//		double dValue = (double)_ttoi(sValue);			//<-- Liefert falsche Ergebnisse
//		m_iBrightness = (int)(dValue*100.0/255.0+0.5);
		int iValue = _ttoi(sValue);
		m_iBrightness = MulDiv(iValue, 100, 255);
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_COMPRESSION)
	{
		if(sValue == NameOfEnum(COMPRESSION_1))
		{
			m_nCompression = COMPRESSION_1;
		}
		else if(sValue == NameOfEnum(COMPRESSION_2))
		{
			m_nCompression = COMPRESSION_2;
		}
		else if(sValue == NameOfEnum(COMPRESSION_3))
		{
			m_nCompression = COMPRESSION_3;
		}
		else
		{
			m_nCompression = COMPRESSION_1;
		}
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_FPS)
	{
		int i = 0;
		CString sFPS;
		m_nCameraFPS = g_iaCameraFPS[0]; //init with 1 fps

		for (i = 0; i < CAMERA_FPS_STEPS; i++)
		{
			sFPS.Format(_T("%d"),g_iaCameraFPS[i]);
			if(sValue == sFPS)
			{
				m_nCameraFPS = g_iaCameraFPS[i];
				break;
			}
		}
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_CAM_PTZ_TYPE)
	{
		m_nCameraPTZType = NameToCameraControlType(sValue);
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_CAM_PTZ_ID)
	{
		m_dwCameraPTZID = _ttoi(sValue);
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_CAM_PTZ_COMPORT)
	{
		m_nCameraPTZComPort = _ttoi(sValue);
		m_evConfirmGet.SetEvent();		
	}
	else if (sKey == CSD_MODE)
	{
		m_sMode = sValue;
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (sValue == CSD_OFF)
			m_iMDMaskSensitivity = MD_SENSITIVITY_OFF;
		else if (sValue == CSD_LOW)
			m_iMDMaskSensitivity = MD_SENSITIVITY_LOW;
		else if (sValue == CSD_NORMAL)
			m_iMDMaskSensitivity = MD_SENSITIVITY_NORMAL;
		else if (sValue == CSD_HIGH)
			m_iMDMaskSensitivity = MD_SENSITIVITY_HIGH;
		else
			WK_TRACE_WARNING(_T("Unknown CSD_MASK_Value <%s>\n"), sValue);
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (sValue == CSD_LOW)
			m_iMDAlarmSensitivity = MD_SENSITIVITY_LOW;
		else if (sValue == CSD_NORMAL)
			m_iMDAlarmSensitivity = MD_SENSITIVITY_NORMAL;
		else if (sValue == CSD_HIGH)
			m_iMDAlarmSensitivity = MD_SENSITIVITY_HIGH;
		else
			WK_TRACE_WARNING(_T("Unknown CSD_ALARM_Value <%s>\n"), sValue);
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_TERM)
	{
		if (sValue == CSD_ON)
			m_bCameraTermination = TRUE;
		else if (sValue == CSD_OFF)
			m_bCameraTermination = FALSE;
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_DWELL)
	{
		m_iSequencerDwellTime = _ttoi(sValue);
		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_IS_TIMER_ACTIVE)
	{
		m_sIsTimerActive = sValue;
		DoRedraw();
	}
	else if (sKey == CSD_ALARMCALL_EVENT)
	{
		if (sValue == CSD_ALARMCALL_EVENT_OFF)
			m_iAlarmCallEvent = ALARMCALL_EVENT_OFF;
		else if (sValue == CSD_ALARMCALL_EVENT_A)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A;
		else if (sValue == CSD_ALARMCALL_EVENT_A1)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A1;
		else if (sValue == CSD_ALARMCALL_EVENT_A2)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A2;
		else if (sValue == CSD_ALARMCALL_EVENT_A12)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A12;
		else if (sValue == CSD_ALARMCALL_EVENT_1)
			m_iAlarmCallEvent = ALARMCALL_EVENT_1;
		else if (sValue == CSD_ALARMCALL_EVENT_2)
			m_iAlarmCallEvent = ALARMCALL_EVENT_2;
		else if (sValue == CSD_ALARMCALL_EVENT_12)
			m_iAlarmCallEvent = ALARMCALL_EVENT_12;
		else
			m_iAlarmCallEvent = ALARMCALL_EVENT_UNKNOWN;	

		m_evConfirmGet.SetEvent();
	}
	else if (sKey == CSD_CARD_TYPE)
	{
		//check current camera to witch card (Jacob, Savic, Tasha, Q) the camera is connected
		if (sValue == CSD_CARD_TYPE_JACOB)
		{
			m_iAlarmCallEventByCardType = CARD_TYPE_JACOB;
		}
		else if (sValue == CSD_CARD_TYPE_SAVIC)
		{
			m_iAlarmCallEventByCardType = CARD_TYPE_SAVIC;
		}
		else if (sValue == CSD_CARD_TYPE_TASHA)
		{
			m_iAlarmCallEventByCardType = CARD_TYPE_TASHA;
		}
		else if (sValue == CSD_CARD_TYPE_Q)
		{
			m_iAlarmCallEventByCardType = CARD_TYPE_Q;
		}
		else
			m_iAlarmCallEventByCardType = CARD_TYPE_UNKNOWN;
			
		m_evConfirmGet.SetEvent();
	}
	else if (0 == _tcsncmp(sKey,szTimer,5))
	{
		int i = -1;
		if (1==_stscanf(sKey,szTimer,&i))
		{
			m_sTimer[i] = sValue;
            m_evConfirmGet.SetEvent();
		}
		else
		{
			WK_TRACE(_T("unknown confirm timer key %s value %s\n"),sKey,sValue);
		}
	}
//	else if (sKey.Find(CSD_CAM_CONTROL_FKT) == 0)
//	{
//		_stscanf(sValue,_T("%08x"),&m_dwPTZFunctions);
//		m_evConfirmGet.SetEvent();
//	}
	else
	{
		TRACE(_T("CLiveWindow::ConfirmGetValue UNKNOWN key %s value %s\n"),sKey,sValue);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::ConfirmSetValue(const CString &sKey,const CString &sValue)
{
//	TRACE(_T("CLiveWindow::ConfirmSetValue %s,%s\n"),sKey,sValue);
	if (sKey == CSD_SATURATION)
	{
		// 0...512 in Prozent umrechnen
//		double dValue = atof(sValue);
		double dValue = (double)_ttoi(sValue);
		m_iSaturation = (int)(dValue*100.0/512.0+0.5);
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_CONTRAST)
	{
		// 0...512 in Prozent umrechnen
//		double dValue = atof(sValue);
		double dValue = (double)_ttoi(sValue);
		m_iContrast = (int)(dValue*100.0/512.0+0.5);
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_BRIGHTNESS)
	{
		// 0...255 in Prozent umrechnen
//		double dValue = atof(sValue);
		double dValue = (double)_ttoi(sValue);
		m_iBrightness = (int)(dValue*100.0/255.0+0.5);
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_COMPRESSION)
	{
		if(sValue == NameOfEnum(COMPRESSION_1))
		{
			m_nCompression = COMPRESSION_1;
		}
		else if(sValue == NameOfEnum(COMPRESSION_2))
		{
			m_nCompression = COMPRESSION_2;
		}
		else if(sValue == NameOfEnum(COMPRESSION_3))
		{
			m_nCompression = COMPRESSION_3;
		}
		else
		{
			m_nCompression = COMPRESSION_1;
		}
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_FPS)
	{
		int i = 0;
		CString sFPS;
		m_nCameraFPS = g_iaCameraFPS[0]; //init with 1 fps

		for (i = 0; i < CAMERA_FPS_STEPS; i++)
		{
			sFPS.Format(_T("%d"),g_iaCameraFPS[i]);
			if(sValue == sFPS)
			{
				m_nCameraFPS = g_iaCameraFPS[i];
				break;
			}
		}
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_CAM_PTZ_TYPE)
	{
		m_nCameraPTZType = NameToCameraControlType(sValue);
		m_evConfirmSet.SetEvent();
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				pDVOC->DoRequestInfoOutputs(GetID().GetGroupID());
			}
		}
	}
	else if (sKey == CSD_CAM_PTZ_ID)
	{
		m_dwCameraPTZID = _ttoi(sValue);
		m_evConfirmSet.SetEvent();
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				pDVOC->DoRequestInfoOutputs(GetID().GetGroupID());
			}
		}
	}
	else if (sKey == CSD_CAM_PTZ_COMPORT)
	{
		m_nCameraPTZComPort = _ttoi(sValue);
		m_evConfirmSet.SetEvent();
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				pDVOC->DoRequestInfoOutputs(GetID().GetGroupID());
			}
		}
	}
	else if (sKey == CSD_MODE)
	{
		m_sMode = sValue;
		m_evConfirmSet.SetEvent();
		SetMDValues(0,0);
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (sValue == CSD_OFF)
			m_iMDMaskSensitivity = MD_SENSITIVITY_OFF;
		else if (sValue == CSD_LOW)
			m_iMDMaskSensitivity = MD_SENSITIVITY_LOW;
		else if (sValue == CSD_NORMAL)
			m_iMDMaskSensitivity = MD_SENSITIVITY_NORMAL;
		else if (sValue == CSD_HIGH)
			m_iMDMaskSensitivity = MD_SENSITIVITY_HIGH;
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (sValue == CSD_LOW)
			m_iMDAlarmSensitivity = MD_SENSITIVITY_LOW;
		else if (sValue == CSD_NORMAL)
			m_iMDAlarmSensitivity = MD_SENSITIVITY_NORMAL;
		else if (sValue == CSD_HIGH)
			m_iMDAlarmSensitivity = MD_SENSITIVITY_HIGH;
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_TERM)
	{
		if (sValue == CSD_ON)
			m_bCameraTermination = TRUE;
		else if (sValue == CSD_OFF)
			m_bCameraTermination = FALSE;
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_DWELL)
	{
		m_iSequencerDwellTime = _ttoi(sValue);
		m_evConfirmSet.SetEvent();
	}
	else if (sKey == CSD_V_OUT)
	{
		// Actual camera was set to video out port
		// No confirm needed
	}
	else if (sKey == CSD_NAME)
	{
		m_evConfirmSet.SetEvent();
		m_orOutput.Set(sValue,m_orOutput.GetID(),m_orOutput.GetFlags());
		DoRedraw();
	}
	else if (sKey == CSD_ALARMCALL_EVENT)
	{
		if (sValue == CSD_ALARMCALL_EVENT_OFF)
			m_iAlarmCallEvent = ALARMCALL_EVENT_OFF;
		else if (sValue == CSD_ALARMCALL_EVENT_A)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A;
		else if (sValue == CSD_ALARMCALL_EVENT_A1)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A1;
		else if (sValue == CSD_ALARMCALL_EVENT_A2)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A2;
		else if (sValue == CSD_ALARMCALL_EVENT_A12)
			m_iAlarmCallEvent = ALARMCALL_EVENT_A12;
		else if (sValue == CSD_ALARMCALL_EVENT_1)
			m_iAlarmCallEvent = ALARMCALL_EVENT_1;
		else if (sValue == CSD_ALARMCALL_EVENT_2)
			m_iAlarmCallEvent = ALARMCALL_EVENT_2;
		else if (sValue == CSD_ALARMCALL_EVENT_12)
			m_iAlarmCallEvent = ALARMCALL_EVENT_12;
		else
			m_iAlarmCallEvent = ALARMCALL_EVENT_UNKNOWN;	

		m_evConfirmSet.SetEvent();
	}

	else if (0 == _tcsncmp(sKey,szTimer,5))
	{
		int i = -1;
		if (1==_stscanf(sKey,szTimer,&i))
		{
			m_sTimer[i] = sValue;
            m_evConfirmSet.SetEvent();
		}
		else
		{
			WK_TRACE(_T("unknown confirm timer key %s value %s\n"),sKey,sValue);
		}
	}
	else
	{
		TRACE(_T("CLiveWindow::ConfirmSetValue UNKNOWN key %s value %s\n"),sKey,sValue);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::InitialRequests()
{
	if (m_pServer && m_pServer->IsOutputConnected())
	{
		CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();

		pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_SATURATION,0);
		pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_CONTRAST,0);
		pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_BRIGHTNESS,0);
		pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_MODE,0);
		CString sKey;
		for (int i=0;i<7;i++)
		{
			sKey.Format(szTimer,i);
			pDVOC->DoRequestGetValue(m_orOutput.GetID(),sKey,0);
		}
		pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_IS_TIMER_ACTIVE,0);
	}
	else
	{
		WK_TRACE_ERROR(_T("no output connection in RequestInitialValues %s\n"),GetName());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetBrightness(int &nBrightness)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_BRIGHTNESS,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nBrightness = m_iBrightness;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetContrast(int &nContrast)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_CONTRAST,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nContrast = m_iContrast;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

    return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetSaturation(int &nSaturation)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_SATURATION,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nSaturation = m_iSaturation;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetCameraTermination(BOOL &bCameraTermination)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_TERM,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bCameraTermination = m_bCameraTermination;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetCompression(int &nCompression)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_COMPRESSION,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nCompression = m_nCompression;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetCameraFPS(int &nCameraFPS)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_FPS,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nCameraFPS = m_nCameraFPS;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetCameraPTZType(int &nCameraPTZType)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_CAM_PTZ_TYPE,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nCameraPTZType = m_nCameraPTZType;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetCameraPTZID(DWORD& dwCameraPTZID)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(), CSD_CAM_PTZ_ID, 0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					dwCameraPTZID = m_dwCameraPTZID;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetCameraPTZComPort(int &nCameraPTZComPort)
{
	BOOL bResult = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_CAM_PTZ_COMPORT,0);
				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					nCameraPTZComPort = m_nCameraPTZComPort;
					bResult = TRUE;
				}
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::EnableMD()
{
	BOOL bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_MODE,CSD_MD);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::EnableUVV()
{
	BOOL bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_MODE,CSD_UVV);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IsMD()
{
	return m_sMode == CSD_MD;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IsPTZ()
{
	return (m_orOutput.CameraIsFixed() == FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IsJPEG()
{
	return m_orOutput.CameraDoesJpeg();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::IsMpeg4()
{
	return m_orOutput.CameraDoesMpeg();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetMDMaskSensitivity(int nLevel)
{
	CString sValue;
	BOOL	bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (nLevel == MD_SENSITIVITY_OFF)
			sValue = CSD_OFF;
		else if (nLevel == MD_SENSITIVITY_LOW)
			sValue = CSD_LOW;
		else if (nLevel == MD_SENSITIVITY_NORMAL)
			sValue = CSD_NORMAL;
		else if (nLevel == MD_SENSITIVITY_HIGH)
			sValue = CSD_HIGH;
		else
			return bResult;

		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_MD_MASK_SENSITIVITY, sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}


	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetMDAlarmSensitivity(int nLevel)
{
	CString sValue;
	BOOL	bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (nLevel == MD_SENSITIVITY_LOW)
			sValue = CSD_LOW;
		else if (nLevel == MD_SENSITIVITY_NORMAL)
			sValue = CSD_NORMAL;
		else if (nLevel == MD_SENSITIVITY_HIGH)
			sValue = CSD_HIGH;

		if (!sValue.IsEmpty() && m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_MD_ALARM_SENSITIVITY, sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetAlarmCallEvent(int nAlarmCallEvent)
{
	CString sValue;
	BOOL	bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (nAlarmCallEvent == ALARMCALL_EVENT_OFF)
			sValue = CSD_ALARMCALL_EVENT_OFF;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_A)
			sValue = CSD_ALARMCALL_EVENT_A;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_A1)
			sValue = CSD_ALARMCALL_EVENT_A1;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_A2)
			sValue = CSD_ALARMCALL_EVENT_A2;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_A12)
			sValue = CSD_ALARMCALL_EVENT_A12;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_1)
			sValue = CSD_ALARMCALL_EVENT_1;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_2)
			sValue = CSD_ALARMCALL_EVENT_2;
		else if (nAlarmCallEvent == ALARMCALL_EVENT_12)
			sValue = CSD_ALARMCALL_EVENT_12;

		if (!sValue.IsEmpty() && m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmSet.ResetEvent();
				pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_ALARMCALL_EVENT, sValue);
				if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					bResult = TRUE;
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}


	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetSequencerDwellTime(int iSeconds)
{
	CString sValue;
	BOOL	bResult = FALSE;
	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
	//	if (   0 <= m_iSequencerDwellTime
	//		&& m_iSequencerDwellTime <= MAX_SEQUENCER_DWELL_TIME)
		if (   0 <= iSeconds
			&& iSeconds <= MAX_SEQUENCER_DWELL_TIME)
		{
			sValue.Format(_T("%i"), iSeconds);

			if (m_pServer && m_pServer->IsOutputConnected())
			{
				CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
				if (pDVOC)
				{
					m_evConfirmSet.ResetEvent();
					pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_DWELL, sValue);
					if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
					{
						if (   0 <= m_iSequencerDwellTime
							&& m_iSequencerDwellTime <= MAX_SEQUENCER_DWELL_TIME)
						{
							bResult = TRUE;
						}
						else
						{
							WK_TRACE_WARNING(_T("SetSequencerDwellTime invalid value %i confirmed\n"),
																m_iSequencerDwellTime);
							m_iSequencerDwellTime = -1;
						}
					}
				}
			}
		}
		else
		{
			WK_TRACE_WARNING(_T("SetSequencerDwellTime invalid value %i\n"), iSeconds);
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}


	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
int CLiveWindow::GetMDMaskSensitivity()
{
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_MD_MASK_SENSITIVITY,0);
				m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID());
			}
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return m_iMDMaskSensitivity;
}

/////////////////////////////////////////////////////////////////////////////
int CLiveWindow::GetMDAlarmSensitivity()
{
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_MD_ALARM_SENSITIVITY,0);
				m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID());
			}	
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return m_iMDAlarmSensitivity;
}

/////////////////////////////////////////////////////////////////////////////
int CLiveWindow::GetAlarmCallEvent()
{
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_ALARMCALL_EVENT,0);
				m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID());
			}	
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return m_iAlarmCallEvent;
}

/////////////////////////////////////////////////////////////////////////////
int CLiveWindow::GetAlarmCallEventByCardType()
{
//	int iAlarmCallEventByCardType = m_iAlarmCallEventByCardType;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{ 
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_CARD_TYPE,0);
				m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID());
			}	
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return m_iAlarmCallEventByCardType;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::GetSequencerDwellTime(int& iSequencerDwellTime)
{
	BOOL bReturn = FALSE;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (m_pServer && m_pServer->IsOutputConnected())
		{
			CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
			if (pDVOC)
			{
				m_evConfirmGet.ResetEvent();
				pDVOC->DoRequestGetValue(m_orOutput.GetID(),CSD_DWELL,0);

				if (m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID()))
				{
					if (   0 <= m_iSequencerDwellTime
						&& m_iSequencerDwellTime <= MAX_SEQUENCER_DWELL_TIME)
					{
						iSequencerDwellTime = m_iSequencerDwellTime;
						bReturn = TRUE;
					}
					else
					{
						iSequencerDwellTime = -1;
						WK_TRACE_WARNING(_T("GetSequencerDwellTime invalid value %i\n"),
															m_iSequencerDwellTime);
					}
				}
				m_evConfirmGet.Unlock();
			}	
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CLiveWindow::GetTimer(int iDay)
{
	CString sReturn;
	if (!m_bGetEventUsed)
	{
		m_bGetEventUsed = TRUE;
		if (iDay>=0 && iDay<7)
		{
			if (m_pServer && m_pServer->IsOutputConnected())
			{
				CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
				if (pDVOC)
				{
					CString sKey;
					sKey.Format(szTimer,iDay);
					m_evConfirmGet.ResetEvent();
					pDVOC->DoRequestGetValue(m_orOutput.GetID(),sKey,0);
					m_evConfirmGet.Wait(2000, _T(__FUNCTION__), GetID());
				}	
			}
			sReturn = m_sTimer[iDay];
		}
		else
		{
			WK_TRACE(_T("timer index out of range %d\n"),iDay);
		}
		m_bGetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetTimer(int iDay, const CString& sTimer)
{
	BOOL bResult = FALSE;

	if (!m_bSetEventUsed)
	{
		m_bSetEventUsed = TRUE;
		if (iDay>=0 && iDay<7)
		{
			if (m_pServer && m_pServer->IsOutputConnected())
			{
				CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
				if (pDVOC)
				{
					CString sKey;
					sKey.Format(szTimer,iDay);
					m_evConfirmSet.ResetEvent();
					pDVOC->DoRequestSetValue(m_orOutput.GetID(),sKey, sTimer);
					if (m_evConfirmSet.Wait(2000, _T(__FUNCTION__), GetID()))
					{
						bResult = TRUE;
					}
				}
			}
		}
		m_bSetEventUsed = FALSE;
	}
	else
	{
		WK_TRACE(gsEventError, _T(__FUNCTION__), XTIB::GetThreadName());
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLiveWindow::SetActiveWindowToAnalogOut()
{
	// Den analogen Videoausgang nicht remote schalten
	if (theApp.IsTransmitter() && m_pServer && m_pServer->IsOutputConnected())
	{
		CIPCOutputDVClient* pDVOC = m_pServer->GetOutput();
		if (pDVOC)
		{
			pDVOC->DoRequestSetValue(m_orOutput.GetID(),CSD_V_OUT, CSD_PORT1);
			// Kein Confirm notwendig
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (point.y > 16 && IsPTZ())
	{
		if (m_LastCameraControlCmd != CCC_INVALID)
		{
			CSkinDialog::TrackMouseEvent(m_hWnd);// Use common controlled TrackMouseEvent function of CSkinDialog
			RequestPTZ(m_LastCameraControlCmd, m_dwPTZSpeed, TRUE);
		}
	}
	else
	{
		CDisplayWindow::OnLButtonDown(nFlags, point);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
/*	CRect rcVideo;
	GetVideoClientRect(rcVideo);

	if (rcVideo.PtInRect(point))
*/
	if (m_LastCameraControlCmdStop != CCC_INVALID)
	{
		RequestPTZ(m_LastCameraControlCmdStop, m_dwPTZSpeed, TRUE);
	}
	CDisplayWindow::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
//	TRACE(_T("CLiveWindow::OnMouseMove %s PTZ=%d\n"), GetName(), IsPTZ());
	if (IsPTZ() && (point.y > 16))
	{
		HCURSOR hC = theApp.m_hArrow;
		CRect rect, rcZoom;
		CRgn  prof;
		int cx = 0, cy = 0;
		GetClientRect(rect);
		if (m_dwPTZFunctions & (PTZF_PAN))
		{
			cx = rect.Width() / 3;
		}
		if (m_dwPTZFunctions & (PTZF_TILT))
		{
			cy = rect.Height() / 3;
		}
		CPoint pts[4];
		DWORD dwPTZSpeed = (DWORD)-1;
		if (nFlags & MK_LBUTTON)
		{
			// TODO! RKE: Kommandowechsel
			dwPTZSpeed = m_dwPTZSpeed;
		}

		// Zoom
		rcZoom = rect;
		rcZoom.DeflateRect(cx, cy);
		if (rcZoom.PtInRect(point))
		{
			// |--------------------------|
			// |        -       +         |
			// |   |------------------|   |
			// |   |  Zoom  |  Zoom   |   |
			// |   |[----------------]|   |
			// |   |[Near |Auto| Far ]|   |
			// |   |------------------|   |
			// |                          |
			// |--------------------------|
			CPoint ptCenter = rect.CenterPoint();
			if (m_dwPTZFunctions & PTZF_FOCUS && point.y > ptCenter.y)
			{
				int nT = 0;
				if (m_dwPTZFunctions & PTZF_FOCUS_AUTO)
				{
					nT = MulDiv(rect.Width(), 5, 100);
				}
				if (point.x >= ptCenter.x+nT)
				{
					hC = theApp.m_hFocusFar;
					SetLastCameraControlCmd(CCC_FOCUS_FAR, CCC_FOCUS_STOP, dwPTZSpeed);
				}
				else if (point.x <= ptCenter.x-nT)
				{
					hC = theApp.m_hFocusNear;
					SetLastCameraControlCmd(CCC_FOCUS_NEAR, CCC_FOCUS_STOP, dwPTZSpeed);
				}
				else
				{
					hC = theApp.m_hFocusAuto;
					SetLastCameraControlCmd(CCC_FOCUS_AUTO, CCC_FOCUS_STOP, dwPTZSpeed);
				}
			}
			else
			{
				if (point.x > ptCenter.x)
				{
					hC = theApp.m_hZoomIn;
					SetLastCameraControlCmd(CCC_ZOOM_IN, CCC_ZOOM_STOP, dwPTZSpeed);
				}
				else
				{
					hC = theApp.m_hZoomOut;
					SetLastCameraControlCmd(CCC_ZOOM_OUT, CCC_ZOOM_STOP, dwPTZSpeed);
				}
			}
		}
		else
		{
			if (m_dwPTZFunctions & PTZF_MOVE_DIAGONAL)
			{
				//|---------------------|
				//| UL | Tilt Up   | UR |
				//|----|-----------|----|
				//| PL |  (Zoom)   | PR |
				//|----|-----------|----|
				//| DL | Tilt Down | DR |
				//|---------------------|
				if (point.y < rcZoom.top)
				{
					if (point.x < rcZoom.left)
					{
						SetLastCameraControlCmd(CCC_MOVE_LEFT_UP, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hLeftTop;
					}
					else if(point.x > rcZoom.right)
					{
						SetLastCameraControlCmd(CCC_MOVE_RIGHT_UP, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hRightTop;
					}
					else
					{
						SetLastCameraControlCmd(CCC_TILT_UP, CCC_TILT_STOP, dwPTZSpeed);
						hC = theApp.m_hTiltUp;
					}
				}
				else if (point.y > rcZoom.bottom)
				{
					if (point.x < rcZoom.left)
					{
						SetLastCameraControlCmd(CCC_MOVE_LEFT_DOWN, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hLeftBottom;
					}
					else if(point.x > rcZoom.right)
					{
						SetLastCameraControlCmd(CCC_MOVE_RIGHT_DOWN, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hRightBottom;
					}
					else
					{
						SetLastCameraControlCmd(CCC_TILT_DOWN, CCC_TILT_STOP, dwPTZSpeed);
						hC = theApp.m_hTiltDown;
					}
				}
				else
				{
					if (point.x < rcZoom.left)
					{
						SetLastCameraControlCmd(CCC_PAN_LEFT, CCC_PAN_STOP, dwPTZSpeed);
						hC = theApp.m_hPanLeft;
					}
					else
					{
						SetLastCameraControlCmd(CCC_PAN_RIGHT, CCC_PAN_STOP, dwPTZSpeed);
						hC = theApp.m_hPanRight;
					}
				}
			}
			else
			{
				// |-----------------|
				// |\   Tilt Up     /|
				// | \|-----------|/ |
				// |PL|  (Zoom)   |PR|
				// | /|-----------|\ |
				// |/   Tilt Down   \|
				// |-----------------|
				if (point.y < rcZoom.top)
				{
					// TiltUp
					pts[0] = rect.TopLeft();
					pts[1].x = rect.right;
					pts[1].y = rect.top;
					pts[2].x = rect.right-cx;
					pts[2].y = rect.top+cy;
					pts[3].x = rect.left+cx;
					pts[3].y = rect.top+cy;

					prof.CreatePolygonRgn(pts,4,WINDING);

					if (prof.PtInRegion(point))
					{
						prof.DeleteObject();
						hC = theApp.m_hTiltUp;
						SetLastCameraControlCmd(CCC_TILT_UP, CCC_TILT_STOP, dwPTZSpeed);
					}
				}
				else if (point.y > rcZoom.bottom)
				{
					// TiltDown
					pts[0].x = rect.left;
					pts[0].y = rect.bottom;
					pts[1].x = rect.left+cx;
					pts[1].y = rect.bottom-cy;
					pts[2].x = rect.right-cx;
					pts[2].y = rect.bottom-cy;
					pts[3] = rect.BottomRight();
					prof.CreatePolygonRgn(pts,4,WINDING);
					if (prof.PtInRegion(point))
					{
						prof.DeleteObject();
						hC = theApp.m_hTiltDown;
						SetLastCameraControlCmd(CCC_TILT_DOWN, CCC_TILT_STOP, dwPTZSpeed);
					}
				}

				if (hC == theApp.m_hArrow)
				{
					prof.DeleteObject();
					if (point.x < rcZoom.left)
					{
						// PanLeft
						pts[0] = rect.TopLeft();
						pts[1].x = rect.left+cx;
						pts[1].y = rect.top+cy;
						pts[2].x = rect.left+cx;
						pts[2].y = rect.bottom-cy;
						pts[3].x = rect.left;
						pts[3].y = rect.bottom;
						prof.CreatePolygonRgn(pts,4,WINDING);
						if (prof.PtInRegion(point))
						{
							prof.DeleteObject();
							hC = theApp.m_hPanLeft;
							SetLastCameraControlCmd(CCC_PAN_LEFT, CCC_PAN_STOP, dwPTZSpeed);
						}
					}
					else
					{
						// PanRight
	//					pts[0].x = rect.right;
	//					pts[0].y = rect.top;
	//					pts[1] = rect.BottomRight();
	//					pts[2].x = rect.right-cx;
	//					pts[2].y = rect.bottom-cy;
	//					pts[3].x = rect.right-cx;
	//					pts[3].y = rect.top+cy;
	//					prof.CreatePolygonRgn(pts,4,WINDING);
	//					if (prof.PtInRegion(point))
						{	// do not test the region, it must be here
							prof.DeleteObject();
							hC = theApp.m_hPanRight;
							SetLastCameraControlCmd(CCC_PAN_RIGHT, CCC_PAN_STOP, dwPTZSpeed);
						}
					}
				}
			}
		}
		SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)hC);

		if (hC == theApp.m_hArrow)
		{
			SetLastCameraControlCmd(CCC_INVALID, CCC_INVALID, (DWORD)-1);
		}
	}
	else
	{
		CDisplayWindow::OnMouseMove(nFlags, point);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLiveWindow::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (m_LastCameraControlCmdStop != CCC_INVALID)
	{
		RequestPTZ(m_LastCameraControlCmdStop, 0, FALSE);
		m_LastCameraControlCmdStop = m_LastCameraControlCmd = CCC_INVALID;
	}
	// call static function in CSkinDialog::MouseLeave class to confirm CSkinDialog::TrackMouseEvent
	CSkinDialog::MouseLeave(m_hWnd); 
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLiveWindow::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case WPARAM_CAMERA_CT:    m_ccType = (CameraControlType)lParam; break;
		case WPARAM_CAMERA_CF:	  m_dwPTZFunctions = lParam;            break;
		case WPARAM_CAMERA_CF_EX: m_dwPTZFunctionsEx = lParam;          break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::RequestPTZ(CameraControlCmd ccc, DWORD dwValue, BOOL bOn)
{
//	if (m_dwPTZFunctions & dwFlags)
	{
		CIPCInputDVClient* pInput = m_pServer->GetInput();
		if (pInput)
		{
//			TRACE(_T("RequestPTZ InputID %08x CamID %08x Cmd %s Value %u\n"),
//							pInput->GetComPortInputID(), GetID(), NameOfEnum(ccc), dwValue);
			pInput->DoRequestCameraControl(pInput->GetComPortInputID(), GetID(), ccc, dwValue);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::RequestPTZParams()
{
	CIPCInputDVClient* pInput = m_pServer->GetInput();
	if (pInput)
	{
		CString sKey;
		CSecID cid = pInput->GetComPortInputID();
		if (cid!=SECID_NO_ID)
		{
			sKey.Format(CSD_CAM_CONTROL_TYPE_FMT, GetID().GetID());
			pInput->DoRequestGetValue(cid, sKey);
			sKey.Format(CSD_CAM_CONTROL_FKT_FMT, GetID().GetID());
			pInput->DoRequestGetValue(cid, sKey);
		}
		else
		{
			TRACE(_T("RequestPTZFunctions SECID_NO_ID\n"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::SetLastCameraControlCmd(CameraControlCmd cccC, CameraControlCmd cccStop, DWORD dwValue)
{
	CameraControlCmd cccOldC = m_LastCameraControlCmd;
	m_LastCameraControlCmd = cccC;
	if (dwValue != -1)
	{
		if (cccOldC != cccC)
		{
			if (cccC == CCC_INVALID)
			{
				RequestPTZ(m_LastCameraControlCmdStop, m_dwPTZSpeed, TRUE);
			}
			else
			{
				RequestPTZ(m_LastCameraControlCmd, m_dwPTZSpeed, TRUE);
			}
		}
	}
	m_LastCameraControlCmdStop = cccStop;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnDestroy()
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
	m_evConfirmSet.SetEvent();
	m_evConfirmGet.SetEvent();

	CDisplayWindow::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::SetShowable() 
{
	m_bShowable = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::ShowVideo()
{
	if (IsMpeg4())
	{
		if (IsWindowVisible())
		{
//				CWindowDC dc(this);
			CClientDC dc(this);
			OnDraw(&dc);
			if (m_uGOPTimer == 0)
			{
				Request();	// the Mpeg4 are requested here
			}
		}
	}
	else
	{
		OnChangeVideo();
	}
}

#ifdef _SWITCHABLE_LIVE_WINDOWS_
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnDraw(CDC* pDC)
{
	if (m_pBtnCancel)
	{
		pDC->SelectClipRgn(&m_rgnBtnCancel);
	}
	CDisplayWindow::OnDraw(pDC);
	if (m_pBtnCancel)
	{
		pDC->SelectClipRgn(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CLiveWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDisplayWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (theApp.GetIntValue(_T("SwitchableLiveWindow")))
	{
		m_pBtnCancel = new CSkinButton();
		CRect rc;
		GetClientRect(&rc);
		CWindowDC dc(this);
		CSize sz = dc.GetTextExtent(_T("X"), 1);

		rc.bottom = rc.top + sz.cy;
		rc.left = rc.right - sz.cy;
		m_pBtnCancel->SetBaseColor(CPanel::m_BaseColorButtons);
		m_pBtnCancel->CreateEx(0, _T("Button"), _T("X"), WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, this, IDCANCEL, NULL);
		SetWindowPos(m_pBtnCancel, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnCancel()
{
	if (m_pBtnCancel)
	{
		m_bShowable = FALSE;
		ShowWindow(SW_HIDE);

		LPARAM lParam = 0;
		if (IsActive())
		{
			lParam = (LPARAM)GetID().GetID();
			SetActive(FALSE);
		}
		GetParent()->PostMessage(WM_USER, WPARAM_SET_WINDOW_SIZE, lParam);
		m_iRequests = 0;
		m_bWasSomeTimesActive = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLiveWindow::OnSize(UINT nType, int cx, int cy)
{
	CDisplayWindow::OnSize(nType, cx, cy);

	if (m_pBtnCancel)
	{
		CRect rc(CPoint(0,0), CSize(cx, cy)), rcBtn;
		CRgn rgnBtn, rgnWnd;

		m_pBtnCancel->GetClientRect(&rcBtn);
		rcBtn.OffsetRect(rc.right - rcBtn.right, 0);
		rgnWnd.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		rgnBtn.CreateRectRgn(rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
		m_rgnBtnCancel.DeleteObject();
		m_rgnBtnCancel.CreateRectRgn(0,0,0,0);
		m_rgnBtnCancel.CombineRgn(&rgnWnd, &rgnBtn, RGN_DIFF);

		m_pBtnCancel->MoveWindow(cx-rcBtn.bottom, 0, rcBtn.bottom, rcBtn.bottom);
		m_pBtnCancel->InvalidateRect(NULL);
	}
}
#endif // _SWITCHABLE_LIVE_WINDOWS_
