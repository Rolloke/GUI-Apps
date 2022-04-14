/* GlobalReplace: CSkinsDlg --> CPanel */
// MainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "MainFrame.h"

#include "Server.h"
#include "IPCInputDVClient.h"
#include "IPCOutputDVClient.h"
#include "IPCDatabaseDVClient.h"

#include "LiveWindow.h"
#include "PlayWindow.h"
#include "BackupWindow.h"
#include "DebugWindow.h"

#include "CPanel.h"
#include "CMinimizedDlg.h"


#define RC_INVOKED 1
//#include <Devguid.h>
//#include <wdm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_XxX	(6)
#define MAX_1pX	(15)

#define TIMER_FLASH				1
#define TIMER_DEVNODE_CHANGE	2

#define ALL_WINDOWS DISPLAYWINDOW_BASE

/////////////////////////////////////////////////////////////////////////////
CDisplayWindowThread::CDisplayWindowThread(const CString sName, LPVOID lpContext): 
	CWK_Thread(sName, lpContext)
{
	m_dwThreadID = 0;
	m_avgQueueLen.SetNoOfAvgValues(50);
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindowThread::~CDisplayWindowThread()
{
	m_Pictures.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindowThread::Run(LPVOID lpContext)
{
	if (m_dwThreadID == 0)
	{
		m_dwThreadID = GetCurrentThreadId();
	}

	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, TRUE)) 
	{
		switch(msg.message)
		{
			case WM_PICTUREDATA:
				OnPictureData(msg.wParam, msg.lParam);
				break;
			case WM_QUIT:
				m_Pictures.SafeDeleteAll();
				m_dwThreadID = 0;
				return FALSE;
		}
	};

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindowThread::PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_dwThreadID)
	{
		return ::PostThreadMessage(m_dwThreadID, nMsg, wParam, lParam);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindowThread::LivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, CSecID idCam)
{
	CPictureData *pData = new CPictureData(pict, dwX, dwY, idCam);
	m_Pictures.SafeAddTail(pData);
	PostThreadMessage(WM_PICTUREDATA, (WPARAM)pData, 0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDisplayWindowThread::OnPictureData(WPARAM wParam, LPARAM lParam)
{
	int nSize = m_Pictures.GetSize();
	if (nSize)
	{
		CPictureData *pData = m_Pictures.SafeGetAndRemoveHead();
		CMainFrame*   pMF   = (CMainFrame*)m_lpContext;
		CLiveWindow*  pLW   = pMF->GetLiveWindow(pData->m_idCam.GetSubID());
		m_avgQueueLen.AddValue(nSize-1);
		if (pLW)
		{	// set MD values invalid, the are sent through input connection
			ASSERT(pData->m_pPict->GetNumberOfPictures()<=1);
			pLW->PictureData(*pData->m_pPict, pData->m_dwX, pData->m_dwY);
		}
		else
		{
			WK_TRACE_ERROR(_T("no live window for image cam id = %08lx\n"), pData->m_idCam.GetID());
		}
		if (pMF->m_wndStatusBar.m_hWnd)
		{
			pMF->PostMessage(WM_USER, MAKELONG(WPARAM_PANE_INT, 1), nSize);
			pMF->PostMessage(WM_USER, MAKELONG(WPARAM_PANE_INT, 3), pMF->GetLiveWindowQueueLength());
		}
		WK_DELETE(pData);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int  CDisplayWindowThread::GetQueueLength()
{
	return (int)(m_avgQueueLen.GetAverage()+0.5);
}

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_EXT,		// 1
	ID_INDICATOR_CAPS,		// 2
	ID_INDICATOR_NUM,		// 3
	ID_INDICATOR_SCRL,		// 4
	ID_INDICATOR_OVR,		// 5
	ID_INDICATOR_REC,		// 6
	ID_INDICATOR_KANA,		// 7
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
/////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	m_pBigOne = NULL;
	m_wCamera = (WORD)-1;
	m_iXWin = 1;
	m_bAutoViewMode = FALSE;
	m_eViewMode = View1pX;
	m_bDestroying = FALSE;
	m_bLogoValid = FALSE;
	m_sOemName	 = _T("");
	m_sShortOemName	 = _T("");
	m_bBackupStatus = FALSE;
	m_bMustAutoViewMode = FALSE;
	m_uFlashTimer = 0;
	m_colBackGround = RGB(0,0,0);
	m_bWindowsCreationComplete = FALSE;
//	m_hDevNotify = NULL;
	
	CString sComputerName = theApp.GetComputerName();

	// 2 der Messerechner sollten keine Beta meldung ausgeben!
//	m_bDrawBetaWarning = ((sComputerName != _T("210620-3")) && (sComputerName != _T("210620-2")));
	m_bDrawBetaWarning = FALSE;

	// OEM Spezifische Einstellungen
	if (DoesFileExist(theApp.GetHomeDir()+_T("\\")+_T("oem.jpg")))
	{
		m_bLogoValid = m_JpegLogo.DecodeJpegFromFile(theApp.GetHomeDir()+_T("\\")+_T("oem.jpg"));
		UpdateLogoFont();		
		m_colBackGround = m_JpegLogo.GetPixel(0,0);		
		m_JpegLogo.SetBackgroundColor(m_colBackGround);
		m_JpegLogo.SetTextColor(RGB(128,128,128));
	}
	m_rcLogo.SetRectEmpty();

	m_uDevKnodeChangedTimer = 0;
	m_pLiveWindowThread = NULL;
	m_nUnitFps = 0;
#ifdef _DEBUG
//	m_Displays.m_nCurrentThread = 0;	// RKE: enables deadlock detection
#endif

//TODO TRK für Sync Abspielen
/*
	m_pSyncOldestPlayWnd = NULL;
*/
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	WK_DELETE(m_pLiveWindowThread);
	m_Displays.SafeDeleteAll();
//	TRACE(_T("CMainFrame delete\n"));
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_ASKCBFORMATNAME()
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_POWEROFF_PRESSED, OnPowerOffButtonPressed)
	ON_MESSAGE(WM_RESET_PRESSED, OnResetButtonPressed)
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_USER,OnUser)
	ON_WM_QUERYENDSESSION()
	ON_MESSAGE(WM_CHECK_BACKUP_STATUS, OnBackupCheckStatus)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_EXT, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CAPS, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_NUM, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_REC, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_KANA, OnUpdateDummy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	// Messages ans Panel weiterleiten
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
		pPanel->PostMessage(WM_SELFCHECK, wParam, lParam);
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnQueryEndSession() 
{
	WK_TRACE(_T("OnQueryEndSession\n"));

	if (!CFrameWnd::OnQueryEndSession())
		return FALSE; 
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnPowerOffButtonPressed(WPARAM wParam, LPARAM lParam)
{
	// Messages ans Panel weiterleiten
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
		pPanel->PostMessage(WM_POWEROFF_PRESSED, wParam, lParam);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnResetButtonPressed(WPARAM wParam, LPARAM lParam)
{
	// Messages ans Panel weiterleiten
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
		pPanel->PostMessage(WM_RESET_PRESSED, wParam, lParam);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::DoRequest()
{
	if (!m_bDestroying)
	{
		PostMessage(WM_USER,MAKELONG(WPARAM_REQUEST,0), RM_NORMAL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Request()
{
	if (!m_bDestroying)
	{
		int c = 0;
		int nResult = FALSE;

		m_Displays.Lock(_T(__FUNCTION__));
		// first only Playwindows
		for (int i=0;i<m_Displays.GetSize();i++)
		{
			CDisplayWindow* pDW = m_Displays.GetAtFast(i);
			switch (pDW->GetType())
			{
				case DISPLAYWINDOW_PLAY:
				case DISPLAYWINDOW_BACKUP:	// why not handle here
					nResult = pDW->Request();
					break;
				default: 
					nResult = FALSE;
					break;
			}
			if (nResult == TRUE)
			{
				c++;
			}
			else if (nResult < 0)
			{
				c -= nResult;
			}
		}
	/*
		// then the live windows
		// Uwi, don`t forget the backup windows !!!
		for (int i=0;i<m_Displays.GetSize();i++)
		{
			CDisplayWindow* pDW = m_Displays.GetAtFast(i);
			if (pDW)
			{
				if (pDW->GetType() == DISPLAYWINDOW_BACKUP)
				{
					nResult = pDW->Request();
					if (nResult == TRUE)
					{
						c++;
					}
					else if (nResult < 0)
					{
						c -= nResult;
					}
				}
			}
		}
	*/
		m_Displays.Unlock();

		if (m_bMustAutoViewMode)
		{
			m_bMustAutoViewMode = FALSE;
			AutoViewMode();
		}

		Sleep(10);	// this is enough
	}
}
void CMainFrame::RequestLive(int nMode)
{
	if (!m_bDestroying)
	{
		int c = 0, nWndRequested = 0;
		int nFps = 0, nActive = 0;
		int nResult;
		int nRequest = -2;	// indicator for live window requested from RequestThread

		m_Displays.Lock(_T(__FUNCTION__));
		if (nMode == RM_PERFORMANCE) // performance management
		{
			nRequest = -1;	// directly called from performance management
			int nMaxPossibleFps = m_nUnitFps > 0 ? m_nUnitFps : 100;
			int nMaxFps = 0;
			int nCount = 0;
			for (int i=0; i<m_Displays.GetSize(); i++)
			{
				CDisplayWindow* pDW = m_Displays.GetAtFast(i);
				if (pDW)
				{
					if (pDW->GetType() == DISPLAYWINDOW_LIVE)
					{
						CLiveWindow *pLW = (CLiveWindow*)pDW;
						if (pLW->IsYUV())
						{
							if (nMaxFps == 0)
							{
								nMaxFps = MulDiv(nMaxPossibleFps, CLiveWindow::GetPerformancePercent(), 100);
							}
						}
						else
						{
							break;
						}
						if (pLW->IsWindowVisible())
						{
							nCount++;
						}
					}
				}
			}
			if (nMaxFps)
			{
				if (nCount > 1)
				{
					nFps = (nMaxFps - 25) / (nCount-1);
				}
				if (nFps < 5)
				{
					if(nCount > 0)
					{
						nFps = nMaxFps / nCount;
					}
					else
					{
						TRACE(_T("nCount\n"));
					}
					
					if (nFps == 0)
					{
						nActive = nFps = 1;
					}
					else
					{
						nActive = nMaxFps - nFps*(nCount-1);
					}
				}
				else
				{
					nFps = min(nFps, 25);
					nActive = 25;
				}
				TRACE(_T("Requested(%d, %d)\n"), nFps, nActive);
			}
		}
		for (int i=0;i<m_Displays.GetSize();i++)
		{
			CDisplayWindow* pDW = m_Displays.GetAtFast(i);
			if (pDW)
			{
				if (pDW->GetType() == DISPLAYWINDOW_LIVE)
				{
					CLiveWindow *pLW = (CLiveWindow*)pDW;
					if (nFps)
					{
						pLW->m_nCurrentRequestedFPS = pLW->IsActive() ? nActive : nFps;
					}
					nResult = pDW->Request(nRequest);
					if (nResult == TRUE)
					{
						c++;
						nWndRequested++;
					}
					else if (nResult < 0)
					{
						c -= nResult;
					}
				}
			}
		}
#ifdef _DEBUG
		if (theApp.GetPerformanceLevel() < 0)
		{
			int nMaxRequested = -theApp.GetPerformanceLevel();
			while (nWndRequested < nMaxRequested)
			{
				theApp.IncreasePerformanceLevel();
				nMaxRequested = -theApp.GetPerformanceLevel();
			}
		}
#endif
		m_Displays.Unlock();

		Sleep(10);	// this is enough
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = WK_APP_NAME_DVCLIENT;
	
	return CFrameWnd::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowSize()
{
//	TRACE(_T("##CMainFrame::SetWindowSize (Viewmode=%d)\n"), m_eViewMode);
    if (m_Displays.GetSize()>0)
	{
		switch (m_eViewMode)
		{
		case ViewFull:
			SetWindowSizeFull();
			break;
		case ViewXxX:
			SetWindowSizeXxX();
			break;
		case View1pX:
			SetWindowSize1pX();
			break;
		default:
			WK_TRACE_ERROR(_T("invalid view mode %d\n"),m_eViewMode);
			break;
		}
	}
	else
	{
		Invalidate();
	}
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		pPanel->UpdateButtons();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowSize1pX()
{
	int c = GetNrOfWindows(0, SHOWABLE);

	if (c == 1)
	{
		SetWindowSizeFull();
		return;
	}

	CAutoCritSec acs(&m_Displays.m_cs);
	CDisplayWindow* pPlay = GetPlayWindow(m_wCamera);	// kann auch NULL sein
	CDisplayWindow* pLive = GetLiveWindow(m_wCamera);	// kann auch NULL sein
	CDisplayWindow* pBackup = GetBackupWindow();		// kann auch NULL sein
	CDisplayWindow* pDebug  = GetDebugWindow();			// kann auch NULL sein
//	TRACE(_T("##CMainFrame::SetWindowSize1pX\n"));
//TODO TKR für Sync Abspielen
/*
	CDisplayWindow* pLastBigOne = m_pBigOne;
*/
	m_pBigOne = NULL;
	if (pDebug)
	{
        if (pDebug->IsShowable() || pDebug->IsWindowVisible())
		{
			if (pDebug->IsActive())
			{
				m_pBigOne = pDebug;
			}
		}
	}

	if (pBackup)
	{
        if (pBackup->IsShowable() || pBackup->IsWindowVisible())
		{
			if (pBackup->IsActive())
			{
				m_pBigOne = pBackup;
			}
		}
	}
	
	if (m_pBigOne == NULL)
	{
		if (pPlay != NULL)
		{
			if (pPlay->IsShowable() || pPlay->IsWindowVisible())
			{
				if (pPlay->IsActive())
				{
					m_pBigOne = pPlay;

//TODO TKR hier das Umschalten eines kleinen abspielenden PlayWindows auf das große Fenster
		//beim syncplay verhindern,wenn syncplay gestoppt 
/*

					if(theApp.GetPanel()->CanSyncPlay())
					{
						CPlayWindow* pOldestPW = theApp.GetMainFrame()->GetOldestPlayWindow();
						
						if(WK_IS_WINDOW(pOldestPW) && WK_IS_WINDOW(pLastBigOne))
						{
							TRACE(_T("--- pOldestPW: %d   pPlay: %d\n"), pOldestPW->GetID().GetSubID(), 
								pPlay->GetID().GetSubID());

							TRACE(_T("---- TimeStamp pLastBigOne: %s - %d\n"), pLastBigOne->GetTimeStamp().GetDateTime(), pLastBigOne->GetTimeStamp().GetMilliseconds());
							TRACE(_T("---- TimeStamp pPlay:       %s - %d\n"), pPlay->GetTimeStamp().GetDateTime(), pPlay->GetTimeStamp().GetMilliseconds());
							TRACE(_T("---- TimeStamp pOldestPW:   %s - %d\n"), pOldestPW->GetTimeStamp().GetDateTime(), pOldestPW->GetTimeStamp().GetMilliseconds());
							
							if(    pOldestPW == pPlay
							    && pOldestPW->GetPlayStatus() == CPlayWindow::PS_STOP)
							{

								m_pBigOne = pOldestPW;

								//ist kleines SyncPlayFenster wieder großschaltbar?
								if(	   pLastBigOne != pOldestPW
									&& !pOldestPW->CanMakeSyncBigOne())
								{
									pOldestPW->SetSyncBigOne(TRUE);
									m_pBigOne = pLastBigOne;
									//theApp.GetPanel()->SetCamera(m_pBigOne->GetID().GetSubID() & (MAX_CAM-1), Play);
									SetDisplayActive(m_pBigOne);
									TRACE(_T("###### last bigone wird groß\n"));
								}
								else
								{
									TRACE(_T("###### oldest wird groß\n"));
								}
							}
						}
						else
						{
							TRACE(_T("########### LastBigOne: 0x%x  pOldestPW: 0x%x\n"), pLastBigOne, pOldestPW);
						}
					}
*/
				}

			}
		}
	}

	if (m_pBigOne == NULL)
	{
		if (pLive != NULL)
		{
			if (pLive->IsShowable() || pLive->IsWindowVisible())
			{
				if (pLive->IsActive())
				{
					m_pBigOne = pLive;
					
				}
			}
		}
	}

	c = GetNrOfWindows(ALL_WINDOWS, SHOWABLE);
	
	// CAVEAT: pLive und damit pBigOne kann immer noch NULL sein,
	// wenn keine Kamera angeschlossen ist!
	// Dann wird in OnEraseBkgnd schwarz gemalt	und das Fenster mit gezählt!
	if (m_pBigOne == NULL  && c == 0)
	{
		c++;
	}
	
	if (c > 2*m_iXWin)
	{
		// zu viele Fenster Modus hoch schalten
        m_iXWin = c/2 + c%2;
	}
	else if (c <= 2*(m_iXWin - 1))
	{
		// zu wenig Fenster
        m_iXWin = c/2 + c%2;
	}
	else
	{
		// c == 2*m_iXWin oder
		// c == 2*m_iXWin -1
		// alles OK
	}
	if (m_iXWin < 3)
	{
		m_iXWin = 3;
	}

	// OEM Spezifische Einstellungen
	// 1+3 sichtbare(!) Fenster wird durch 2x2 ersetzt, falls XxX Darstellung nicht erlaubt
	if (   m_iXWin == 3
		&& GetNrOfWindows(ALL_WINDOWS, SHOWABLE) == 4
		)
	{
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			OEM oem = pPanel->GetOem();
			int iCameras = pPanel->GetNrOfCameras();
			if (   ((oem == OemAlarmCom) || (oem == OemPOne))
				&& iCameras <= 8
				)
			{
				SetWindowSizeXxX();
				return;
			}
		}
	}

	CRect r;
	GetClientRect(&r);
	int x,y,i,j,cx,cy;
	// dimension of the view window itself
	int viewWidth = r.Width();
	int viewHeight = r.Height();
	// dimension for the child windows, one per camera
	int childWidth,childHeight;
	int childResX, childResY;

	childWidth = viewWidth / m_iXWin;
	childHeight = viewHeight / m_iXWin;
	childResX = viewWidth % m_iXWin;
	childResY = viewHeight % m_iXWin;

	x = y = cx = cy = 0;

	// CAVEAT: pLive und damit pBigOne kann immer noch NULL sein,
	// wenn keine Kamera angeschlossen ist!
	// Dann wird in OnEraseBkgnd schwarz gemalt
	if (m_pBigOne == NULL)
	{
		x = r.left;
		y = r.top;
		cx = childWidth * (m_iXWin-1);
		cy = childHeight * (m_iXWin-1);
		CRect rect(x, y, cx, cy);
		InvalidateRect(rect);
	}

	for (i=0,j=0;i<m_Displays.GetSize();i++) 
	{
		if (m_Displays.GetAtFast(i) == m_pBigOne)
		{
			x = r.left;
			y = r.top;
			cx = childWidth * (m_iXWin-1);
			cy = childHeight * (m_iXWin-1);
			m_Displays.GetAtFast(i)->MoveWindow(x, y, cx, cy, FALSE);
			if (   m_Displays.GetAtFast(i)->IsShowable() 
				&& !m_Displays.GetAtFast(i)->IsWindowVisible())
			{
				m_Displays.GetAtFast(i)->ShowWindow(SW_SHOW);
			}
			m_Displays.GetAtFast(i)->Invalidate();
//			m_Displays.GetAtFast(i)->UpdateWindow();

		}
		else 
		{
			if (j<m_iXWin*2-1)
			{
				cx = childWidth;
				cy = childHeight;
				if (j<m_iXWin-1)
				{
					x = childWidth * j;
					y = childHeight * (m_iXWin-1);
					cy += childResY;
				}
				else
				{
					x = childWidth * (m_iXWin-1);
					cx += childResX;
					y = cy * (j+1-m_iXWin);
				}
				if (m_Displays.GetAtFast(i)->IsWindowVisible())
				{
					m_Displays.GetAtFast(i)->MoveWindow(x, y, cx, cy, FALSE);
					m_Displays.GetAtFast(i)->Invalidate();
//					m_Displays.GetAtFast(i)->UpdateWindow();
					j++;
				}
				else
				{
					if (m_Displays.GetAtFast(i)->IsShowable())
					{
						m_Displays.GetAtFast(i)->MoveWindow(x, y, cx, cy, FALSE);
						m_Displays.GetAtFast(i)->Invalidate();
						m_Displays.GetAtFast(i)->ShowWindow(SW_SHOW);
//						m_Displays.GetAtFast(i)->UpdateWindow();
						j++;
					}
				}
			}
			else
			{
				// TODO: passt nicht mehr
				/*
				if (m_Displays.GetAtFast(i)->GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
				{
					CPlayWindow* pPW = (CPlayWindow*)m_Displays.GetAtFast(i);
					pPW->SetPlayStatus(CPlayWindow::PS_EJECT);
				}
				*/
				m_Displays.GetAtFast(i)->ShowWindow(SW_HIDE);
			}
		}
		BOOL bCanRectangleQuery = TRUE;
#ifndef _DEBUG
		bCanRectangleQuery = theApp.GetServer()->CanRectangleQuery() || theApp.IsReadOnly();
#endif
		if(bCanRectangleQuery)
		{
			//recalculate rectangle (if exists) for new client size
			CQueryRectTracker* pTracker = m_Displays.GetAtFast(i)->GetTracker();

			if(     pTracker 
				&& !pTracker->GetRect()->IsRectEmpty())
			{
				m_Displays.GetAtFast(i)->GetTracker()->ResizeRectangle();
				if(theApp.TKR_Trace())
				{
					TRACE(_T("***** Rectangle resized\n"));
				}
			}
		}
	}
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowSizeXxX()
{
//	TRACE(_T("##CMainFrame::SetWindowSizeXxX\n"));
	CAutoCritSec acs(&m_Displays.m_cs);
	int c = GetNrOfWindows(ALL_WINDOWS, SHOWABLE);
	if (c > m_iXWin*m_iXWin)
	{
		// zu viele Fenster Modus hoch schalten
		m_iXWin = (int)sqrt((double)(c-1)) + 1;
	}
	else if (c <= (m_iXWin-1)*(m_iXWin-1))
	{
		// zu wenig Fenster
		m_iXWin = (int)sqrt((double)(c-1)) + 1;
	}
	else
	{
		// alles OK
	}
	if (m_iXWin < 1)
	{
		m_iXWin = 1;
	}

	if (m_iXWin==0 || m_Displays.GetSize() == 0)
	{
		Invalidate();		
		return;
	}
	CRect r;
	GetClientRect(&r);
	int x,y,i,j;
	// dimension of the view window itself
	int viewWidth = r.Width();
	int viewHeight = r.Height();
	// dimension for the child windows, one per camera
	int cx,cy;

	cx = viewWidth / m_iXWin;
	cy = viewHeight / m_iXWin;

	for (i=0,j=0;i<m_Displays.GetSize();i++) 
	{
		if (j<m_iXWin*m_iXWin)
		{
			x = (j-(j/m_iXWin)*m_iXWin) * cx;
			y = (j/m_iXWin-(j/(m_iXWin*m_iXWin))*m_iXWin) * cy;
			if (m_Displays.GetAtFast(i)->IsWindowVisible())
			{
				m_Displays.GetAtFast(i)->MoveWindow(x, y, cx, cy, FALSE);
				m_Displays.GetAtFast(i)->Invalidate();
//				m_Displays.GetAtFast(i)->UpdateWindow();
				j++;
			}
			else

			{
				if (m_Displays.GetAtFast(i)->IsShowable())
				{
					m_Displays.GetAtFast(i)->MoveWindow(x, y, cx, cy, FALSE);
					m_Displays.GetAtFast(i)->Invalidate();
					m_Displays.GetAtFast(i)->ShowWindow(SW_SHOW);
//					m_Displays.GetAtFast(i)->UpdateWindow();
					j++;
				}
			}
		}
		else
		{
//			m_Displays.GetAtFast(i)->ShowWindow(SW_HIDE);
		}
		BOOL bCanRectangleQuery = TRUE;
#ifndef _DEBUG
		bCanRectangleQuery = theApp.GetServer()->CanRectangleQuery() || theApp.IsReadOnly();
#endif
		if(bCanRectangleQuery)
		{
			//recalculate rectangle (if exists) for new client size
			CQueryRectTracker* pTracker = m_Displays.GetAtFast(i)->GetTracker();

			if(     pTracker 
				&& !pTracker->GetRect()->IsRectEmpty())
			{
				m_Displays.GetAtFast(i)->GetTracker()->ResizeRectangle();
				if(theApp.TKR_Trace())
				{
					TRACE(_T("***** Rectangle resized\n"));
				}
			}
		}
	}
	InvalidateRect(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowSizeFull()
{
//	TRACE(_T("##CMainFrame::SetWindowSizeFull\n"));

	CRect rect;
	GetClientRect(&rect);
	TRACE(_T("SetWindowSizeFull\n"));

	CAutoCritSec acs(&m_Displays.m_cs);
	CDisplayWindow* pBackup	= GetBackupWindow();		// kann auch NULL sein
	CDisplayWindow* pPlay	= GetPlayWindow(m_wCamera);
	CDisplayWindow* pLive	= GetLiveWindow(m_wCamera);
	m_pBigOne = NULL;

	if (pBackup)
	{
        if (pBackup->IsShowable() || pBackup->IsWindowVisible())
		{
			if (pBackup->IsActive())
			{
				m_pBigOne = pBackup;
			}
		}
	}
	
	if (m_pBigOne == NULL)
	{
		if (pPlay != NULL)
		{
			if (pPlay->IsShowable() || pPlay->IsWindowVisible())
			{
				if (pPlay->IsActive())
				{
					m_pBigOne = pPlay;
					
				}
			}
		}
	}

	if (m_pBigOne == NULL)
	{
		if (pLive != NULL)
		{
			if (pLive->IsShowable() || pLive->IsWindowVisible())
			{
				if (pLive->IsActive())
				{
					m_pBigOne = pLive;
					
				}
			}
		}
	}
	
	// CAVEAT: pLive und damit pBigOne kann immer noch NULL sein,
	// wenn keine Kamera angeschlossen ist!
	// Dann wird in OnEraseBkgnd schwarz gemalt
	if (m_pBigOne == NULL)
	{
		InvalidateRect(rect);
	}


	for (int i=0;i<m_Displays.GetSize();i++) 
	{
		if (m_Displays.GetAtFast(i) == m_pBigOne)
		{
			// the big one
			m_Displays.GetAtFast(i)->MoveWindow(rect, FALSE);
			m_Displays.GetAtFast(i)->Invalidate();
			m_Displays.GetAtFast(i)->UpdateWindow();
			if (   m_Displays.GetAtFast(i)->IsShowable() 
				&& !m_Displays.GetAtFast(i)->IsWindowVisible())
			{
				m_Displays.GetAtFast(i)->ShowWindow(SW_SHOW);
			}

			BOOL bCanRectangleQuery = TRUE;
#ifndef _DEBUG
			bCanRectangleQuery = theApp.GetServer()->CanRectangleQuery() || theApp.IsReadOnly();
#endif
			if(bCanRectangleQuery)
			{
				//recalculate rectangle (if exists) for new client size
				CQueryRectTracker* pTracker = m_Displays.GetAtFast(i)->GetTracker();

				if(    pTracker 
					&& !pTracker->GetRectPromilleImage()->IsRectEmpty())
				{
					m_Displays.GetAtFast(i)->GetTracker()->ResizeRectangle();
					if(theApp.TKR_Trace())
					{
						TRACE(_T("***** Rectangle resized\n"));
					}
				}
			}
			m_Displays.GetAtFast(i)->Invalidate();
		}
		else 
		{
			m_Displays.GetAtFast(i)->ShowWindow(SW_HIDE);
		}
	}
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SortDisplayWindows()
{
	int iCur, iMin, iNext, nCount;
	CDisplayWindow* swap;

	m_Displays.Lock(_T(__FUNCTION__));
	nCount = m_Displays.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			if (*m_Displays.GetAtFast(iNext) < *m_Displays.GetAtFast(iMin))
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = m_Displays.GetAtFast(iCur);
			m_Displays.SetAt(iCur,m_Displays.GetAtFast(iMin));
			m_Displays.SetAt(iMin,swap);
		}
	}
	m_Displays.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddLiveWindow(CServer* pServer, const CIPCOutputRecord & rec)
{
	CDisplayWindow* pDW = GetDisplayWindow(rec.GetID());
	
	if (   theApp.GetIntValue(_T("RunLiveWindowThread"), 1) 
		&& m_pLiveWindowThread == NULL)
	{
		m_pLiveWindowThread = new CDisplayWindowThread(_T("LiveWindowThread"), (LPVOID)this);
		m_pLiveWindowThread->StartThread();
	}

	if (pDW)
	{
		// already open
	}
	else
	{
		CLiveWindow* pLW = NULL;

		CRect rect;

		GetClientRect(rect);
		if (m_iXWin)
		{
			rect.right /= m_iXWin;
			rect.bottom /= m_iXWin;
		}
		pLW = new CLiveWindow(this, pServer, rec);

		if (pLW)
		{
			if (!pLW->Create(rect, this))
			{
				delete pLW;
				return;
			}
			pLW->ShowWindow(SW_SHOW);
			if (pLW->IsMpeg4())				// for RequestThread timeout
			{
				theApp.SetTimeOutValue(5);	// 50 ms
			}
			else if (pLW->IsJPEG())
			{
				theApp.SetTimeOutValue(20);	// 200 ms
			}
			else
			{
				theApp.SetTimeOutValue(10);	// 100 ms
				if (m_nUnitFps == 0)
				{
					pLW->GetUnitFPS();	// fire request, don´t wait
					m_nUnitFps = -1;	// undetermined, don´t request twice
					// the value is corrected, if the unit replies
				}
			}
			PostMessage(WM_USER,MAKELONG(WPARAM_REQUEST,0),RM_PERFORMANCE);
			AddDisplayWindow(pLW);

			CIPCOutputDVClient* pCIPCOutput = pServer->GetOutput();
			int i = GetNrOfWindows(DISPLAYWINDOW_LIVE, SHOWABLE);
            if (   (i>0) 
				&& (pCIPCOutput->GetNrOfActiveCameras() == i)
				)
			{
				TRACE(_T("last Live Window created %d\n"),i);
				//if debug window with maintenance text is available, 
				//the active window is the debug window
				if(GetDebugWindow() == NULL)
				{
					CheckActivePanelCamera();
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CheckActivePanelCamera()
{
	CAutoCritSec acs(&m_Displays.m_cs);
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		WORD wCam = (WORD)pPanel->GetActiveCamera();
		CLiveWindow* pLW = GetLiveWindow(wCam);
		if (pLW == NULL)
		{
			CDisplayWindow* pDW;
			for (int i = 0;i<m_Displays.GetSize();i++)
			{
				pDW = m_Displays.GetAtFast(i);
				if (pDW->GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
				{
					pLW = (CLiveWindow*)pDW;
					break;
				}
			}
		}

		// Auf Alarmkamera schalten
		if (theApp.GetServer() && theApp.GetServer()->IsAlarm())
			pPanel->SetCamera(theApp.GetServer()->GetAlarmCameraNr(), Live);
		else if (pLW) // Auf Livekamerta schalten
			pPanel->SetCamera(pLW->GetID().GetSubID(), Live);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddPlayWindow(CServer* pServer, const CIPCArchivRecord& rec)
{
	CSecID id(SECID_GROUP_ARCHIVE,rec.GetSubID());
	CDisplayWindow* pDW = GetDisplayWindow(id);
	
	if (pDW)
	{
		// already open
	}
	else
	{
		CRect rect;
		GetClientRect(rect);

		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		CPlayWindow* pPW = new CPlayWindow(this, pServer, rec);

		if (pPW)
		{
			if (!pPW->Create(rect, this))
			{
				delete pPW;
				return;
			}
			AddDisplayWindow(pPW);
			if (   pPW->IsBackup()
				|| theApp.IsReadOnly())
			{
				pPW->SetPlayStatus(CPlayWindow::PS_STOP);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddBackupWindow(CServer* pServer)
{
	m_evBackupWindow.ResetEvent();
	PostMessage(WM_USER, MAKELONG(WPARAM_ADD_BACKUP,0), 0);

	m_evBackupWindow.Lock(2000);
	m_evBackupWindow.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CLiveWindow* CMainFrame::GetLiveWindow(WORD wCameraNr)
{
	if (m_bDestroying)
	{
		return NULL;
	}

	CDisplayWindow* pDW = NULL;

	CAutoCritSec acs(&m_Displays.m_cs);

	for (int i=0;i<m_Displays.GetSize() && !m_bDestroying; i++) 
	{
		pDW = m_Displays.GetAtFast(i);
		if (pDW->GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
		{
			if (pDW->GetID().GetSubID() == wCameraNr)
			{
				return (CLiveWindow*)pDW;
			}
		}
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CPlayWindow* CMainFrame::GetPlayWindow(WORD wCameraNr)
{
	if (m_bDestroying)
	{
		return NULL;
	}

	CDisplayWindow* pDW = NULL;
	
	CAutoCritSec acs(&m_Displays.m_cs);
	for (int i=0;i<m_Displays.GetSize() && !m_bDestroying;i++) 
	{
		pDW = m_Displays.GetAtFast(i);

		if (pDW && pDW->GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			// Die SubID hat im HYBYTE den Typ des Archives
			// kodiert und im LOBYTE die Kamera Nr.
			// also beim Vergleich HYBYTE ausblenden
			if ((wCameraNr & 0x00FF) == wCameraNr)
			{
				if ((pDW->GetID().GetSubID() & 0x00FF) == (wCameraNr & 0x00FF))
				{
					return (CPlayWindow*)pDW;
				}
			}
			else
			{
				if (pDW->GetID().GetSubID() == wCameraNr)
				{
					return (CPlayWindow*)pDW;
				}
			}
		}
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow*	CMainFrame::GetDisplayWindow(CSecID id)
{
	int i,c;
	CDisplayWindow* pDW = NULL;
	
	CAutoCritSec acs(&m_Displays.m_cs);
	c = m_Displays.GetSize();

	if (c>0)
	{
		for (i=0;i<c;i++) 
		{
			pDW = m_Displays.GetAtFast(i);

			if (pDW)
			{
				if (pDW->GetID() == id)
				{
					return pDW;
				}
				else
				{
//						WK_TRACE_ERROR(_T("pSW cam id wrong\n"));
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("pSW is no window %lx\n"),id.GetID());
			}
		}
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AddDisplayWindow(CDisplayWindow* pDW)
{
	m_Displays.Lock(_T(__FUNCTION__));
	m_Displays.Add(pDW);
	SortDisplayWindows();
	m_Displays.Unlock();

	if (pDW->IsWindowVisible())
	{
		if (m_Displays.GetSize() == 1)
		{
			pDW->SetActive(TRUE);
		}
		AutoViewMode();
	}
	else
	{
		Invalidate();
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			pPanel->UpdateButtons();
			pPanel->UpdateMenu();
		}
	}

	pDW->InitialRequests();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetDisplayActive(CDisplayWindow* pDW)
{
//	TRACE(_T("##CMainFrame::SetDisplayActive\n"));
	CAutoCritSec acs(&m_Displays.m_cs);
    for (int i=0;i<m_Displays.GetSize();i++)
	{
        m_Displays.GetAtFast(i)->SetActive(FALSE);
	}
	if (pDW)
	{
		pDW->SetActive(TRUE);

		if(   pDW->GetType() != DISPLAYWINDOW_BACKUP 
		   && pDW->GetType() != DISPLAYWINDOW_DEBUG) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
		{
			//nicht bei Backup Window
			m_wCamera = (WORD)(pDW->GetID().GetSubID() & 0x00ff);
		}
		if (pDW->GetType() == DISPLAYWINDOW_DEBUG)
		{
			SetWindowSize();
		}
	}
//	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::DoSetForeGround()
{
	PostMessage(WM_USER,MAKELONG(WPARAM_SET_FOREGROUND,0),0);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetMustAutoViewMode()
{
	m_bMustAutoViewMode = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::DoAutoViewMode()
{
	PostMessage(WM_USER,MAKELONG(WPARAM_AUTOVIEWMODE,0),0);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AutoViewMode()
{	
//	TRACE(_T("##CMainFrame::AutoViewMode\n"));
    if (m_Displays.GetSize()>0)
	{
		if (!m_bAutoViewMode)
		{
			m_bAutoViewMode = TRUE;

			SetWindowSize();
			
			CPanel* pPanel = theApp.GetPanel();
			if (pPanel)
			{
				pPanel->UpdateButtons();
				pPanel->UpdateMenu();
			}
			m_bAutoViewMode = FALSE;
		}
	}
	else
	{
		Invalidate();
	}
}
/////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	if ((wParam != 0) || (lParam != 0))
	{
		WORD wCause	  = LOWORD(wParam);
		WORD wInfo = HIWORD(wParam);
		CSecID id = (DWORD)(lParam);

		switch (wCause)
		{
		case WPARAM_ADD_INPUT:
			OnAddInput(id);
			break;
		case WPARAM_UPD_INPUT:
			OnUpdateInput(id);
			break;
		case WPARAM_IND_ALARM:
			OnIndicateAlarm((BOOL)wInfo);
			break;
		case WPARAM_DEL_INPUT:
			// TODO: delete input
			break;
		case WPARAM_ADD_CAMERA:
			TRACE(_T(">>>>WPARAM_ADD_CAMERA (%d)\n"), id.GetSubID());
			OnAddCamera(id);
			TRACE(_T("<<<<WPARAM_ADD_CAMERA (%d)\n"), id.GetSubID());
			break;
		case WPARAM_UPD_CAMERA:
			TRACE(_T(">>>>WPARAM_UPD_CAMERA (%d)\n"), id.GetSubID());
			OnUpdateCamera(id);
			TRACE(_T("<<<<WPARAM_UPD_CAMERA (%d)\n"), id.GetSubID());
			break;
		case WPARAM_DEL_CAMERA:
			TRACE(_T(">>>>WPARAM_DEL_CAMERA (%d)\n"), id.GetSubID());
			OnDeleteDisplay(id);
			TRACE(_T("<<<<WPARAM_DEL_CAMERA (%d)\n"), id.GetSubID());
			break;
		case WPARAM_ADD_RELAIS:
			OnAddRelais(id);
			break;
		case WPARAM_UPD_RELAIS:
			// TODO: update relais
			break;
		case WPARAM_DEL_RELAIS:
			// TODO: delete relais
			break;
		case WPARAM_ADD_ARCHIV:
			OnAddArchive(id);
			break;
		case WPARAM_ADD_ARCHIVS:
			OnAddArchives();
			break;
		case WPARAM_UPD_ARCHIV:
			// TODO: update archiv
			break;
		case WPARAM_DEL_ARCHIV:
			OnDeleteDisplay(id);
			break;
		case WPARAM_ADD_BACKUP:
			OnAddBackup();
			break;
		case WPARAM_DEL_BACKUP:
			OnDeleteBackup();
			break;
		case WPARAM_ADD_DEBUG:
		{
			BOOL bUseAsReport = FALSE;
			if(lParam == 1)
			{
				//don`t show debug window, show maintenance text window
				//lParam = 0 --> open debug window
				//lParam = 1 --> open maintenance text window

				bUseAsReport = TRUE;
			}

			CDebugWindow* pDW = NULL;
			pDW = GetDebugWindow();
	
			if (pDW)
			{
				//if maintenance/RTE window still open, check the text
				//if the text has changed (new RTE) update the window
				CString sNewText, sOldText;
				sNewText = pDW->GetTextLine();
				sOldText = GetMaintenanceText();
				if(GetMaintenanceText() != pDW->GetTextLine())
				{
					pDW->SetTextLine();
				}
			}
			else
			{
				pDW = new CDebugWindow(this, NULL, bUseAsReport);
				CRect rect;
				GetClientRect(rect);
				if (pDW->Create(rect, this))
				{
					pDW->ShowWindow(SW_SHOW);
					AddDisplayWindow(pDW);
				}
				else
				{
					delete pDW;
				}
			}
		}break;
		case WPARAM_DEL_DEBUG:
		{
			m_Displays.Lock(_T(__FUNCTION__));
			CDebugWindow* pDW = GetDebugWindow();
			if (pDW)
			{
				m_Displays.SafeRemove(pDW);
				pDW->DestroyWindow();
				WK_DELETE(pDW);
				CPanel* pPanel = theApp.GetPanel();
				if(pPanel)
				{
					pPanel->SetCamera(pPanel->GetActiveCamera(), LivePlay);
				}
			}
			m_Displays.Unlock();
		}break;
		case WPARAM_SET_VIEW:
// Diese Optimierung sorgt beim Connect/Neustart für falsche Bilddarstellung
// bzw. Fenstersortierung bei nur einer angeschlossenen Kamera!
// AutoViewMode muss wenigstens einmal aufgerufen werden, beim Sichtbarmachen!
// Workaround: Abstellen der Optimierung
// Besser: Neue Message USER_CONNECT zum Aufruf von AutoViewMode beim Connect
//			if (   (m_wCamera != wInfo)
//				|| (m_eViewMode != lParam))
			{
				//m_wCamera = wInfo;
				m_eViewMode = (ViewMode)lParam;
//				TRACE(_T("##CMainFrame:WPARAM_SET_VIEW (wCamera=%d VieMode=%d)\n"), m_wCamera, m_eViewMode);
				AutoViewMode();	
			}
			break;
		case WPARAM_PIN_ENTERED:
			{
				CString sHostName,sHostNumber, sPIN;
				sPIN.Format(_T("%04d"),wInfo);
				CPanel *pPanel = theApp.GetPanel();
				if (pPanel)
				{
					sHostNumber = pPanel->GetHostNumber();
					sHostName   = pPanel->GetHostName();
					theApp.Connect(sHostNumber, sHostName, sPIN);
				}
			}
			break;
		case WPARAM_ALARM_CONNECTION:
			OnAlarmConnection();
			break;
		case WPARAM_DISCONNECT:
			OnDisconnect();
			break;
		case WPARAM_AUTOVIEWMODE:
			AutoViewMode();
			break;
		case WPARAM_SET_WINDOW_SIZE:
		{
			if (lParam != 0)
			{
				m_pBigOne = NULL;
				CDisplayWindow* pDW = NULL;
				CAutoCritSec acs(&m_Displays.m_cs);
				for (int i=0;i<m_Displays.GetSize(); i++) 
				{
					pDW = m_Displays.GetAtFast(i);
					if (pDW->IsShowable())
					{
						break;
					}
				}
				CPanel *pPanel = theApp.GetPanel();
				if (pDW && pPanel)
				{
					pPanel->PostMessage(WM_SET_CAMERA, (WORD)(pDW->GetID().GetSubID() & 0x00ff));
				}
			}
			SetWindowSize();
		}	break;
		case WPARAM_SET_FOREGROUND:
			SetForegroundWindow();
			break;
		case WPARAM_REQUEST:
			Request();
			if (lParam == RM_PERFORMANCE)
			{
				RequestLive(lParam);
			}
			break;
		case WPARAM_UPDATE_MENU:
			if (m_uDevKnodeChangedTimer)
			{
				KillTimer(m_uDevKnodeChangedTimer);
				m_uDevKnodeChangedTimer = 0;
				CPanel *pPanel = theApp.GetPanel();
				if (pPanel)
				{
					pPanel->PostMessage(WM_UPDATE_MENU, WPARAM_CONFIG_MENU, 0);
				}
			}
			break;
		case WPARAM_PANE_INT:
		{
			_TCHAR szText[16];
			_itot(lParam, szText, 10);
			SetPanetext(wInfo, szText);
		}break;

		default:
			WK_TRACE(_T("unknown cause in OnUser %d\n"),wCause);
			break;
		}
	}

	return 0L;
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnAlarmConnection()
{
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	theApp.OnAlarmConnection();
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnDisconnect()
{
	if (m_pLiveWindowThread)// the live window thread may contain pictures to display in its
	{						// queue, stop thread, delete queue and then close windows
		m_pLiveWindowThread->PostThreadMessage(WM_QUIT, 0, 0);
		m_pLiveWindowThread->StopThread();
		WK_DELETE(m_pLiveWindowThread);
	}
	// theApp.Disconnect will delete Server Object, but DVUIThread may UpdateMenu meanwhile,
	// where a DisplayWindow may be called, which will call its member m_pServer!
	// So Lock Displays first!
	m_Displays.Lock(_T(__FUNCTION__));
	theApp.Disconnect();
	for (int i=0;i<m_Displays.GetSize();i++)
	{
		m_Displays.GetAtFast(i)->DestroyWindow();
	}
	m_Displays.DeleteAll();
	m_Displays.Unlock();

	m_wCamera = (WORD)-1;
	m_iXWin = 1;
	m_bAutoViewMode = FALSE;
	m_pBigOne = NULL;
	m_sMaintenanceText.Empty();

	SetWindowSize();
	Invalidate();
	if (m_uFlashTimer != 0)
	{
		if (KillTimer(m_uFlashTimer))
		{
			m_uFlashTimer = 0;
			IndicateAlarmToUser(FALSE);
		}
		else
		{
			WK_TRACE_ERROR(_T("CMainframe::OnDisconnect KillTimer FAILED %u\n"),m_uFlashTimer);
		}
	}
}
/////////////////////////////////////////////////////////////
void CMainFrame::DisconnectInput()
{
	// TODO Remove All Input Stuff
}
/////////////////////////////////////////////////////////////
void CMainFrame::DisconnectOutput()
{
	m_Displays.Lock(_T(__FUNCTION__));
	int c = m_Displays.GetSize()-1;
	for (int i=c;i>=0;i--)
	{
		CDisplayWindow* pDW = m_Displays.GetAtFast(i);
		if (pDW->GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
		{
			if (pDW == m_pBigOne)
			{
				m_pBigOne = NULL;
			}
			pDW->DestroyWindow();
			WK_DELETE(pDW);
			m_Displays.RemoveAt(i);
		}
	}
	m_Displays.Unlock();
	m_nUnitFps = 0;

	AutoViewMode();
	Invalidate();
}
/////////////////////////////////////////////////////////////
void CMainFrame::DisconnectDatabase()
{
	m_Displays.Lock(_T(__FUNCTION__));
	int c = m_Displays.GetSize()-1;
	for (int i=c;i>=0;i--)
	{
		CDisplayWindow* pDW = m_Displays.GetAtFast(i);
		if (pDW)
		{
			if (pDW->GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
			{
				if (pDW == m_pBigOne)
				{
					m_pBigOne = NULL;
				}
				pDW->DestroyWindow();
				WK_DELETE(pDW);
				m_Displays.RemoveAt(i);
			}
			else if (pDW->GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
			{
				if (pDW == m_pBigOne)
				{
					m_pBigOne = NULL;
				}
				pDW->DestroyWindow();
				WK_DELETE(pDW);
				m_Displays.RemoveAt(i);
			}
		}
	}
	m_Displays.Unlock();

	AutoViewMode();
	Invalidate();
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnAddInput(CSecID id)
{
	// TODO: add detector display here
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateInput(CSecID id)
{
	// TODO: update detector display here
	CServer* pServer = theApp.GetServer();
	if (   pServer
		&& pServer->IsInputConnected())
	{
		CIPCInputDVClient* pInput = pServer->GetInput();
		if (pInput)
		{
			CLiveWindow* pLW = GetLiveWindow(id.GetSubID());
			if (pLW)
			{
				pLW->UpdateAlarmState();
			}
		}
	}
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnAddCamera(CSecID id)
{
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		CIPCOutputDVClient* pOutput = pServer->GetOutput();
		if (pOutput)
		{
			AddLiveWindow(pServer,pOutput->GetOutputRecordFromSecID(id));
		}
	}
}

/////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateCamera(CSecID id)
{
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		CIPCOutputDVClient* pOutput = pServer->GetOutput();
		if (pOutput)
		{
			CLiveWindow* pLW = GetLiveWindow(id.GetSubID());
			if (pLW)
			{
				pLW->SetOutputRecord(pOutput->GetOutputRecordFromSecID(id));
			}
		}
	}
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnDeleteDisplay(CSecID id)
{
	CDisplayWindow* pDW = NULL;
	
	m_Displays.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Displays.GetSize();i++) 
	{
		pDW = m_Displays.GetAtFast(i);
		if (pDW && (pDW->GetID()==id))
		{
			m_Displays.RemoveAt(i);
			if (id.GetSubID() == m_wCamera && pDW->IsActive())
			{
				// was the active one oops
				m_wCamera = (WORD)-1;
			}
			pDW->DestroyWindow();
			WK_DELETE(pDW);
			break;
		}
	}
	m_Displays.Unlock();

	AutoViewMode();
	SetWindowSize();
	CheckActivePanelCamera();
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnAddRelais(CSecID id)
{
	// TODO: add relais button here
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnAddArchive(CSecID id)
{
	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->IsDatabaseConnected())
		{
			CIPCDatabaseDVClient* pDatabase = pServer->GetDatabase();
			CIPCArchivRecord* pAR = pDatabase->GetArchive(id);
			if (pAR)
			{
				AddPlayWindow(pServer,*pAR);
			}
		}										   
	}
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnAddArchives()
{
	// got all info about archives 
	if (theApp.IsReadOnly())
	{
		// actualize the modes of the
		// panel
		CAutoCritSec acs(&m_Displays.m_cs);
		int iSize = m_Displays.GetSize();
		if (iSize>0)
		{
			WORD wCameraNr = m_Displays.GetAtFast(0)->GetID().GetSubID();
			wCameraNr &= (MAX_CAM-1);

			CPanel* pPanel = theApp.GetPanel();
			if (pPanel)
			{
				pPanel->SetViewMode((iSize==1) ? ViewFull : View1pX,wCameraNr);
				Sleep(50);
			}

			CPlayWindow* pPW = GetPlayWindow(wCameraNr);
			if (pPW)
			{
				SetDisplayActive(pPW);
			}

			SetWindowSize();
		}
	}
}
/////////////////////////////////////////////////////////////
void CMainFrame::DrawNoCameras(CRect rect, CDC* pDC)
{
	CString s;
	s.LoadString(IDS_NO_CAMERA);
	CFont font;
	CFont* pOldFont;				   

	font.CreatePointFont(10*10,GetFixFontFaceName(),pDC);
	pOldFont = pDC->SelectObject(&font);
	int iMode;
	COLORREF oldTextColor;
	iMode = pDC->SetBkMode(TRANSPARENT);
	oldTextColor = pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(s,rect,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
	pDC->SetBkMode(iMode);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(oldTextColor);
}
/////////////////////////////////////////////////////////////
void CMainFrame::DrawBetaWarning(CRect rect, CDC* pDC)
{
	if (!m_bDrawBetaWarning)
		return;

	if (   theApp.GetMajorVersion() == 1 
		&& theApp.GetMinorVersion()<5)

	{
		return;
	}
	
	CString s;
	s.Format(_T("Version %d.%d.%d.%d Final BETA"),
				theApp.GetMajorVersion(),
				theApp.GetMinorVersion(),
				theApp.GetSubVersion(),
				theApp.GetBuildNr());
	CFont font;
	CFont* pOldFont;				   

//	font.CreatePointFont(20*10,_T("Arial"),pDC);
	int iFontSize = rect.Width()/50;
	font.CreateFont(-iFontSize,
					-iFontSize,
					450,
					0,
				    FW_ULTRABOLD,
					0,
					0,
					0,
					OEM_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH,
					GetVarFontFaceName());
//					_T("Arial"));

	pOldFont = pDC->SelectObject(&font);
	int iMode;
	COLORREF oldTextColor;
	iMode = pDC->SetBkMode(TRANSPARENT);
	oldTextColor = pDC->SetTextColor(RGB(255,0,0));
	pDC->DrawText(s,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SetBkMode(iMode);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(oldTextColor);
}
/////////////////////////////////////////////////////////////
BOOL CMainFrame::OnEraseBkgnd(CDC* pDC) 
{
	CBrush b;
	CRect rect;

	// OEM Spezifische Einstellungen
//	SetWindowText(GetOemName());
	SetWindowText(GetShortOemName());

	GetClientRect(rect);   
	
	if (m_Displays.GetSize() == 0)
	{
		b.CreateSolidBrush(m_colBackGround);
		pDC->FillRect(rect,&b);

		if (theApp.GetServer() && theApp.GetServer()->IsOutputConnected())
		{
			// connected, but no cameras
			// say the user that he has to connect one or two
			DrawNoCameras(rect,pDC);
		}
		else
		{
			// not connected, display logo
			if (m_bLogoValid)
			{		
				SIZE  ImageSize = m_JpegLogo.GetImageDims();
				int nXCenter = (rect.Width() - ImageSize.cx)/2;
				int nYCenter = (rect.Height() - ImageSize.cy)/2;
				m_rcLogo.SetRect(nXCenter,
								 nYCenter,
								 nXCenter + ImageSize.cx,
								 nYCenter + ImageSize.cy
								 );

				if (!GetOemName().IsEmpty())
				{
					// Textposition berechnen
					CString sName = m_sOemName;
					if (theApp.IsNT40())
						sName += _T(" - NT");
					if (theApp.IsWinXP())
						sName += _T(" - XP");

					CSize Textsize = pDC->GetTextExtent(sName);
					int nXText = nXCenter + ImageSize.cx - Textsize.cx;
					int nYText = nYCenter + ImageSize.cy;// - Textsize.cy;
					
					// Logo und Text ausgeben
					m_JpegLogo.SetOSDText1(nXText, nYText, sName);
				}
				m_JpegLogo.OnDraw(pDC, m_rcLogo);
			}
			DrawBetaWarning(rect, pDC);
		}
	}
	else
	{
	 	CRect fr;
		int x,y;
		int sx,sy;					  
		int iVisible = GetNrOfWindows(ALL_WINDOWS, SHOWABLE);
		b.CreateSolidBrush(RGB(0,0,0));

		if (m_iXWin>0)
		{
			x = iVisible % m_iXWin;
			y = iVisible / m_iXWin;
			sx = rect.Width() / m_iXWin;
			sy = rect.Height() / m_iXWin;

			if (   m_eViewMode == ViewXxX
				|| (m_eViewMode == View1pX && m_iXWin == 2)	// Sonderfall 2x2 bei 1+X Darstellung
				)
			{
				// XxX mode
				if (x != 0)
				{
					y++;
				}

				if (x != 0)
				{
					fr.left = sx * x;
					fr.right = rect.right;
					fr.top = sy * (y-1);
					fr.bottom = sy * y;
					pDC->FillRect(fr,&b);
				}
				if (y<m_iXWin)
				{
					fr.left = 0;
					fr.right = rect.right;
					fr.top = sy * y;
					fr.bottom = rect.bottom;
					pDC->FillRect(fr,&b);
				}
			}
			else if (m_eViewMode == View1pX)
			{
				// 1pX mode
				if (m_pBigOne == NULL)
				{
					fr.left = rect.left;
					fr.right = sx * (m_iXWin-1);
					fr.top = rect.top;
					fr.bottom = sy * (m_iXWin-1);
					pDC->FillRect(fr,&b);
					DrawNoCameras(rect,pDC);
				}
				if (iVisible<m_iXWin)
				{
					fr.left = sx * (iVisible-1);
					fr.right = rect.right;
					fr.top = sy * (m_iXWin - 1);
					fr.bottom = rect.bottom;
					pDC->FillRect(fr,&b);
				}
				if (iVisible<2*m_iXWin)
				{
					fr.left = sx * (m_iXWin-1);
					fr.right = rect.right;
					fr.top = sy * (iVisible - m_iXWin);
					fr.bottom = rect.bottom;
					pDC->FillRect(fr,&b);
					DrawBetaWarning(fr,pDC);
				}
			}
			else
			{
				// full screen mode
				if (m_pBigOne == NULL)
				{
					pDC->FillRect(rect,&b);
					DrawNoCameras(rect,pDC);
				}
			}
			
			if (iVisible == 0)
			{
				DrawNoCameras(rect,pDC);
			}
		}
		else
		{
			DrawNoCameras(rect,pDC);
		}
	}

	b.DeleteObject();
	
	return TRUE;
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	m_bDestroying = TRUE;
	if (m_pLiveWindowThread)
	{
		m_pLiveWindowThread->PostThreadMessage(WM_QUIT, 0, 0);
	}
	if (m_uFlashTimer)
	{
		if (KillTimer(m_uFlashTimer))
		{
			m_uFlashTimer = 0;
		}
		else
		{
			WK_TRACE_ERROR(_T("CMainframe::OnDestroy() KillTimer FAILED %u\n"),m_uFlashTimer);
		}
	}
/*	if (m_hDevNotify)
	{
		UnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify = NULL;
	}
*/

	theApp.Disconnect();

	CFrameWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// nothing todo
	// all done in erase background
	
}
/////////////////////////////////////////////////////////////
void CMainFrame::OnPINEntered(WORD wPIN)
{
	PostMessage(WM_USER,MAKELONG(WPARAM_PIN_ENTERED,(WORD)wPIN));
}
/////////////////////////////////////////////////////////////
void CMainFrame::SetView(WORD wCameraNr, ViewMode vm)
{
	//TRACE(_T("SetView %04x,%d\n"),wCameraNr,vm);
	PostMessage(WM_USER,MAKELONG(WPARAM_SET_VIEW,wCameraNr),vm);
}
/////////////////////////////////////////////////////////////
int	CMainFrame::GetNrOfWindows(int nType, BOOL bShowable)
{
	int r = 0;
	m_Displays.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Displays.GetSize();i++)
	{
		CDisplayWindow* pDW = m_Displays.GetAtFast(i);
		if (   nType != 0			// if a specific type is requested
			&& nType != pDW->GetType())	// and type is different
		{
			continue;	// don´t count
		}
		if (bShowable==TRUE)
		{
			 if (!pDW->IsShowable())
			 {
				 continue;
			 }
		}
		else if (bShowable==FALSE)
		{
			 if (pDW->IsShowable())
			 {
				 continue;
			 }
		}
		r++;
	}
	m_Displays.Unlock();
	return r;
}
/////////////////////////////////////////////////////////////
CPlayWindow* CMainFrame::GetNextPlayWindow(CPlayWindow* pPlayWndExcept)
{
	CPlayWindow* pNextPlayWnd = NULL;
	CDisplayWindow* pDisplayWnd = NULL;
	CAutoCritSec acs(&m_Displays.m_cs);
	for (int i=0 ; i<m_Displays.GetSize() ; i++)
	{
		pDisplayWnd = m_Displays.GetAtFast(i);
		if (pDisplayWnd->GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			if (pDisplayWnd->IsShowable() || pDisplayWnd->IsWindowVisible())
			{
				if (pDisplayWnd != pPlayWndExcept)
				{
					pNextPlayWnd = (CPlayWindow*)pDisplayWnd;
				}
			}
		}
	}

	return pNextPlayWnd;
}

LPCTSTR NameOfEvent(UINT nEventType)
{
	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL: 
		return _T("DBT_DEVICEARRIVAL");
	case DBT_DEVICEQUERYREMOVE: 
		return _T("DBT_DEVICEQUERYREMOVE");
	case DBT_DEVICEQUERYREMOVEFAILED: 
		return _T("DBT_DEVICEQUERYREMOVEFAILED");
	case DBT_DEVICEREMOVEPENDING: 
		return _T("DBT_DEVICEREMOVEPENDING");
	case DBT_DEVICEREMOVECOMPLETE: 
		return _T("DBT_DEVICEREMOVECOMPLETE");
	case DBT_DEVICETYPESPECIFIC: 
		return _T("DBT_DEVICETYPESPECIFIC");
	case DBT_CONFIGCHANGED:
		return _T("DBT_CONFIGCHANGED");
	case DBT_DEVNODES_CHANGED:
		return _T("DBT_DEVNODES_CHANGED");
	default:
		return _T("unknown");
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	TRACE(_T("WM_DEVICE_CHANGE %s\n"),NameOfEvent(nEventType));
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	BOOL bRet = TRUE;
	
	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			_TCHAR  szDrive[] = _T("A:");
			int   nDrive;
			DWORD dwUnitMask = pDBV->dbcv_unitmask;
			for (nDrive=-1; nDrive<32 && dwUnitMask!=0; nDrive++) dwUnitMask >>= 1;
			szDrive[0] = (_TCHAR)(_T('A') + nDrive);
			UINT nDriveType = GetDriveType(szDrive);
			switch (nEventType)
			{
				case DBT_DEVICEARRIVAL:
				if ((nDriveType == DRIVE_CDROM) && (pDBV->dbcv_flags & DBTF_MEDIA))
				{
					// nur Medium ins Laufwerk eingelegt, kein neues Laufwerk erkannt
					// neu erkannte Laufwerke (CDROM) werde in OnNotifyDrive() erkannt
					WK_TRACE(_T("Device %s (%d) arrival\n"), szDrive, nDriveType);
					// Post the Drive Type with wParam or lParam

					CIPCDataCarrierClient* pDataCarrier = NULL;
					if(theApp.GetServer())
					{
						pDataCarrier = theApp.GetServer()->GetDataCarrier();
					}

					if(theApp.CanBackupWithACDC() && pDataCarrier)
					{
						//lParam == 0 und wParam == 1 -> new medium in drive, check with ACDC
						CPanel *pPanel = theApp.GetPanel();
						if (pPanel)
						{
							pPanel->PostMessage(WM_NOTIFY_FOUND_MEDIUM, 0, 1);
						}
					}
				}
				else
				{
					CPanel *pPanel = theApp.GetPanel();
					if (pPanel)
					{
						pPanel->PostMessage(WM_NOTIFY_DRIVE, MAKELONG(nDriveType, TRUE), pDBV->dbcv_unitmask);
					}
				}
				break;
				case DBT_DEVICEREMOVECOMPLETE:
				if ((nDriveType == DRIVE_CDROM) && (pDBV->dbcv_flags & DBTF_MEDIA))
				{
					WK_TRACE(_T("CD/R remove complete\n"));	
					if (theApp.IsReadOnly())
					{

						// close database server
						HWND hWnd = ::FindWindow(WK_APP_NAME_DB_SERVER,NULL);
						if (hWnd && IsWindow(hWnd))
						{
							::PostMessage(hWnd,WM_COMMAND,ID_APP_EXIT,0L);
						}
						
						//close auduio unit
						HWND hAudio = ::FindWindow(WK_APP_NAME_AUDIOUNIT1, NULL);
						if (hAudio && IsWindow(hAudio))
						{
							::PostMessage(hAudio, WM_COMMAND,ID_APP_EXIT,0L);	
						}
					

						//exit client
						PostMessage(WM_CLOSE, 0, 0);
					}

//TODO TKR test wieder raus
					//wenn in backup (zwischenspeicherng) dann hier einen Fehler melden
					//vorher allerdings den Kopiervorgang der Zwischenspeicherung in der 
					//Datenbank abbrechen
/*
					else if(GetBackupWindow() && GetBackupWindow()->IsBackupRunning())
					{
						TRACE(_T("**** TKR PS_BACKUP_FAILED gesetzt\n"));
						GetBackupWindow()->SetPlayStatus(CBackupWindow::PS_BACKUP_FAILED);
					}
*/
				}
				else // if (nDriveType == DRIVE_NO_ROOT_DIR)				// Drive is now unplugged
				{
					WK_TRACE(_T("Drive %s remove complete\n"), szDrive);
					CPanel *pPanel = theApp.GetPanel();
					if (pPanel)
					{
						pPanel->PostMessage(WM_NOTIFY_DRIVE, MAKELONG(nDriveType, FALSE), pDBV->dbcv_unitmask);
					}
				}break;
				case DBT_DEVICEQUERYREMOVE:       WK_TRACE(_T("Drive %s query remove\n")  , szDrive); break;
				case DBT_DEVICEREMOVEPENDING:     WK_TRACE(_T("Drive %s remove pending\n"), szDrive); break;
				case DBT_DEVICEQUERYREMOVEFAILED: WK_TRACE(_T("Drive %s remove failed\n") , szDrive); break;
				default: break;
			} // End of switch block

			m_Displays.Lock(_T(__FUNCTION__));
			for (int i=0;i<m_Displays.GetSize();i++)
			{
				if (BROADCAST_QUERY_DENY == m_Displays.GetAtFast(i)->OnDeviceChange(nEventType,dwData))
				{
					bRet = BROADCAST_QUERY_DENY;
				}
			}
			m_Displays.Unlock();
		}
		else if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
		{
			CDevIntEx dix((DEV_BROADCAST_DEVICEINTERFACE_EX*)pHdr);
			CString sClass    = dix.GetGuidClass().GetString();
			CString sDevClass = dix.GetGuidDevClass().GetString();
			CString sClassHidUSB, sClassUSB, sDevClassPrinter, sDevClassDiskDrive;
			WK_TRACE(_T("Name: %s from: %s, Service: %s\nClass:%s\nDevClass:%s\nRegKey HKLM:%s\n"), dix.GetDeviceName(), dix.GetManufacturer(), dix.GetService(), sClass, sDevClass, dix.GetRegistryKey());
/*
			sClassHidUSB.Format(      _T("{%s}"), GUID_DEVINTCLASS_HID_USB);// Predefined Classes
			sClassUSB.Format(         _T("{%s}"), GUID_DEVINTCLASS_USB);
			sDevClassPrinter.Format(  _T("{%s}"), GUID_DEVCLASS_PRINTER);
			sDevClassDiskDrive.Format(_T("{%s}"), GUID_DEVCLASS_DISK_DRIVE);
*/			
			if (  (dix.GetGuidClass() == GUID_DEVINTCLASS_USB)
				|| (dix.GetGuidClass() == GUID_DEVINTCLASS_HID_USB))
			{
				CString str;
				if (nEventType == DBT_DEVICEARRIVAL)
				{
					str.LoadString(IDS_FOUND_NEW_DEVICE);
					Beep(440, 200);
					Beep(532, 200);
					Beep(659, 200);
				}
				else if (nEventType == DBT_DEVICEREMOVECOMPLETE)
				{
					str.LoadString(IDS_REMOVED_DEVICE);
					Beep(659, 200);
					Beep(532, 200);
					Beep(440, 200);
				}
				str += dix.GetDeviceName();
				CPanel *pPanel = theApp.GetPanel();
				if (pPanel)
				{
					pPanel->SetTooltipText(str, true);
				}
				WK_TRACE(_T("%s\n"), str);
			}
			// Drucker angeschlossen oder entfernt
			if ( (dix.GetGuidDevClass() == GUID_DEVCLASS_PRINTER)		// Deviceclass Printer
			   ||(dix.GetService().CompareNoCase(_T("usbprint")) == 0))	// Service usbprint
			{
				if (m_uDevKnodeChangedTimer == 0)
				{
					m_uDevKnodeChangedTimer = SetTimer(TIMER_DEVNODE_CHANGE, 4000, NULL);
				}
			}
			else if (dix.GetGuidDevClass() == GUID_DEVCLASS_DISK_DRIVE)// DiskDrive: USBSTORAGE
			{
				if (nEventType == DBT_DEVICEARRIVAL)
				{
					CString sKey = dix.GetRegistryKey();
					CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, sKey, _T(""));
					CString sSection = DBDI_DEV_PARAM;
					sSection += _T("\\");
					sSection += DBDI_CLASSPNP;
					prof.WriteInt(sSection, DBDI_USERREMOVALPOLICY, 3);// disable Drive Cache
				}
			}
/*
			else if (dix.GetGuidDevClass() == GUID_DEVCLASS_VOLUME)// Volume
			{
				// TODO RKE: Namen ausgeben
			}
*/
		}
	}
/*
	WK_TRACE(_T("OnDeviceChange(%x) return 0x%08lx\n"),dwData, bRet);
	if (nEventType == DBT_DEVNODES_CHANGED)
	{
		if (m_uDevKnodeChangedTimer == 0)
		{
			m_uDevKnodeChangedTimer = SetTimer(TIMER_DEVNODE_CHANGE, 5000, NULL);
		}
	}
*/
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::IsInBackup()
{
	BOOL bRet = FALSE;

	CBackupWindow* pBackupWnd = GetBackupWindow();
	if(pBackupWnd)
	{
		bRet = pBackupWnd->IsBackupRunning();
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::IsDeleting()
{
	BOOL bRet = FALSE;
	CAutoCritSec acs(&m_Displays.m_cs);
	for (int i=0;i<m_Displays.GetSize();i++)
	{
		CDisplayWindow* pDW = m_Displays.GetAtFast(i);
		if (pDW->GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			CPlayWindow* pPW = (CPlayWindow*)pDW;
			if (pPW)
			{
				bRet = bRet || pPW->IsDeleting();
			}
		}
	}
	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnBackupCheckStatus(WPARAM wParam, LPARAM lParam)
{
	m_bBackupStatus = FALSE;
	CPlayWindow* pPlayWnd = NULL;
	CBackupWindow* pBackupWnd = NULL;
	CAutoCritSec acs(&m_Displays.m_cs);
	for (int i=0;i<m_Displays.GetSize();i++)
	{
		if(    theApp.GetServer() 
			&& theApp.GetServer()->CanMultiCDExport())
		{
			CDisplayWindow* pDW = m_Displays.GetAtFast(i);
			if (pDW->GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
			{
				CBackupWindow* pBW = (CBackupWindow*)pDW;
				if (pBW && pBW->IsBackupRunning())
				{
					pBackupWnd = pBW;
					m_bBackupStatus = TRUE;
					break;
				}
			}
		}
	}

	if (m_bBackupStatus)
	{
		switch (wParam)
		{
		//*************************************************************************
		//cases on backup is with PacketCD
		case BS_REQUEST_STATUS:
			if (pPlayWnd)
			{
				pPlayWnd->PostMessage(WM_CHECK_BACKUP_STATUS, BS_IN_BACKUP, lParam);
			}
			else
			{
				::PostMessage((HWND)lParam, WM_CHECK_BACKUP_STATUS, BS_NOT_IN_BACKUP, 0);
			}
			break;

		case BS_FORMAT_SUCCESS:
		case BS_FORMAT_FAILED:
		case BS_REMOUNT_SUCCESS:
		case BS_REMOUNT_FAILED:
		case BS_FINALIZE_SUCCESS:
		case BS_FINALIZE_FAILED:
			if (pPlayWnd)
			{
				pPlayWnd->PostMessage(WM_CHECK_BACKUP_STATUS, wParam, lParam);
			}
			else
			{
				// TODO
			}
			break;
		
		//*************************************************************************
		//cases on backup is with DirectCD
		case BS_DCD_NOT_FOUND_EMPTY_CDR:
		case BS_DCD_FOUND_NOT_EMPTY_CDR:
		case BS_DCD_FORMATTING:
		case BS_DCD_START_BACKUP:
		case BS_DCD_COPYING_DATA:
		case BS_DCD_BACKUP_FAILED:
		case BS_DCD_FINALIZE_SUCCESS:
		case BS_DCD_UNEXPECTED_ERROR:
		case BS_DCD_ERROR:

			if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
			{
				if (pBackupWnd)
				{
					pBackupWnd->PostMessage(WM_CHECK_BACKUP_STATUS, wParam, lParam);
				}
				else if(pPlayWnd)
				{
					pPlayWnd->PostMessage(WM_CHECK_BACKUP_STATUS, wParam, lParam);
				}
				break;
			}
			else
			{
				if (pPlayWnd)
				{
					pPlayWnd->PostMessage(WM_CHECK_BACKUP_STATUS, wParam, lParam);
				}
				break;
			}
		default:
			break;
		}
	}
	else
	{
		::PostMessage((HWND)lParam, WM_CHECK_BACKUP_STATUS, BS_NOT_IN_BACKUP, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ClearOEMName()
{
	m_sOemName.Empty();
}
/////////////////////////////////////////////////////////////////////////////
CString CMainFrame::GetOemName()
{
	// OEM Spezifische Einstellungen
	if (m_sOemName.IsEmpty())
	{
		OEM eOem = OemAlarmCom;
		int nNrOfCameras = 8;
		if (theApp.IsReadOnly())
		{
			/* Read only. Einstellungen aus der Autorun.inf lesen  */
			CString sAutorun;
			GetModuleFileName(theApp.m_hInstance,sAutorun.GetBuffer(_MAX_PATH),_MAX_PATH);
			sAutorun.ReleaseBuffer();
			int p = sAutorun.ReverseFind(_T('\\'));
			sAutorun = sAutorun.Left(p+1) + _T("autorun.inf");
			
			eOem = (OEM)GetPrivateProfileInt(REGKEY_SETTINGS,REGKEY_OEM,(int)eOem,sAutorun);
			nNrOfCameras = GetPrivateProfileInt(REGKEY_SETTINGS,_T("NrOfCam"),(int)nNrOfCameras,sAutorun);
		}
		else
		{
			CWK_Profile prof;

 			// Welcher OEM?
			eOem = (OEM)prof.GetInt(REGKEY_DVCLIENT, REGKEY_OEM,  (int)OemDTS);
			nNrOfCameras = prof.GetInt(_T("DV\\Process"), _T("NrOfCameras"), 4);
		}

		CString sVersion = WK_GetFileVersion(theApp.GetHomeDir()+_T("\\DVClient.exe"));

		// OEM Spezifische Einstellungen
		CString sTemp, sVersionText;
		sVersionText.LoadString(IDS_VERSION);
		if (eOem == OemDTS)
		{
			if (theApp.IsReceiver())
			{
				sTemp = _T("videte IT® DTS ") + sVersionText + _T(" %s");
				m_sOemName.Format(sTemp,sVersion);
			}
			else
			{
				sTemp = _T("videte IT® DTS %d00 ") + sVersionText + _T(" %s");
				m_sOemName.Format(sTemp, nNrOfCameras, sVersion);
			}
		}
		else if (eOem == OemAlarmCom)
		{
			CString sName = _T("Siemens VDR");
			if (theApp.IsReceiver())
			{
				sName = _T("Siemens VDR NetView");
			}
			m_sOemName = sName + _T(" ") + sVersionText + _T(" ") + sVersion;
		}
		else if (eOem == OemPOne)
		{
			CString sName = _T("Protection One VDR");
			if (theApp.IsReceiver())
			{
				sName = _T("Protection One VDR NetView");
			}
			m_sOemName = sName + _T(" ") + sVersionText + _T(" ") + sVersion;
		}
		else if (eOem == OemDResearch)
		{
			if (theApp.IsReceiver())
			{
				sTemp = _T("DResearch Vidia 500 ") + sVersionText + _T(" %s");
				m_sOemName.Format(sTemp, sVersion);
			}
			else
			{
				sTemp = _T("DResearch Vidia 500 - %d ") + sVersionText + _T(" %s");
				m_sOemName.Format(sTemp, nNrOfCameras, sVersion);
			}
		}
		else if (eOem == OemSantec)
		{
			if (theApp.IsReceiver())
			{
				sTemp = _T("Santec SDR ") + sVersionText + _T(" %s");
				m_sOemName.Format(sTemp, sVersion);
			}
			else
			{
				sTemp = _T("Santec SDR-%d ") + sVersionText + _T(" %s");
				m_sOemName.Format(sTemp, nNrOfCameras, sVersion);
			}
		}
	}

	return m_sOemName;
}

/////////////////////////////////////////////////////////////////////////////
CString CMainFrame::GetShortOemName()
{
	// OEM Spezifische Einstellungen
	if (m_sShortOemName.IsEmpty())
	{
		OEM eOem = OemAlarmCom;
		if (theApp.IsReadOnly())
		{
			/* Read only. Einstellungen aus der Autorun.inf lesen  */
			CString sAutorun;
			GetModuleFileName(theApp.m_hInstance,sAutorun.GetBuffer(_MAX_PATH),_MAX_PATH);
			sAutorun.ReleaseBuffer();
			int p = sAutorun.ReverseFind(_T('\\'));
			sAutorun = sAutorun.Left(p+1) + _T("autorun.inf");
			
			eOem = (OEM)GetPrivateProfileInt(REGKEY_SETTINGS,REGKEY_OEM,(int)eOem,sAutorun);
		}
		else
		{
			CWK_Profile prof;

 			// Welcher OEM?
			eOem = (OEM)prof.GetInt(REGKEY_DVCLIENT, REGKEY_OEM,  (int)OemDTS);
		}

		// OEM Spezifische Kurzname
		if (eOem == OemDTS)
		{
			m_sShortOemName = _T("DTS");
		}
		else if ((eOem == OemAlarmCom) || (eOem == OemPOne))
		{
			m_sShortOemName = _T("VDR");
		}
		else if (eOem == OemDResearch)
		{
			m_sShortOemName = _T("Vidia");
		}
		else if (eOem == OemSantec)
		{
			m_sShortOemName = _T("SDR");
		}
	}

	return m_sShortOemName;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
		pPanel->ActivatePanel();
	
	CFrameWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		pPanel->ToggleViewMode((WORD)pPanel->GetActiveCamera());

		pPanel->ActivatePanel();
	}	
	CFrameWnd::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
		pPanel->ActivatePanel();
	
	CFrameWnd::OnMButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////
BOOL CMainFrame::GetLogoPosition(CRect& rect) 
{
	BOOL bRet = FALSE;
	if (m_bLogoValid)
	{
		rect.CopyRect(m_rcLogo);
		bRet = TRUE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAskCbFormatName(UINT nMaxCount, LPTSTR lpszString) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnAskCbFormatName(nMaxCount, lpszString);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CFrameWnd::OnSettingChange(uFlags, lpszSection);

	if (theApp.IsReceiver())
	{	
		// Die Taskleiste wurde verschoben.
		if (uFlags == SPI_SETWORKAREA)
		{
			int x, y, w, h;
			RECT rcDesktop;
			SystemParametersInfo(
						  SPI_GETWORKAREA,  // system parameter to retrieve or set
						  0,   // depends on action to be taken
						  (PVOID)&rcDesktop,  // depends on action to be taken
						  0);    // user profile update option

			
			x = rcDesktop.left;
			y = rcDesktop.top;
			w = rcDesktop.right - rcDesktop.left;
			h = rcDesktop.bottom- rcDesktop.top;

#ifdef _DEBUG
 	w = (w*2)/3;
	h = (h*2)/3;
#endif

			SetWindowPos(&CWnd::wndTopMost, x, y, w, h, SWP_SHOWWINDOW);
			DoAutoViewMode();
		}		
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == m_uFlashTimer)
	{
		IndicateAlarmToUser(TRUE);
	}
	else if (nIDEvent == m_uDevKnodeChangedTimer)
	{
		PostMessage(WM_USER, WPARAM_UPDATE_MENU, 1); 
	}
#ifdef _DEBUG
	else if (nIDEvent == TIMER_EVENT_ONE_SECOND)
	{
/* // Test Rolf 		
		CDisplayWindow* pDW = NULL;
		CAutoCritSec acs(&m_Displays.m_cs);
		int nSize = m_Displays.GetSize();
		BOOL bfound = FALSE; 
		for (int i=0;i<nSize; i++) 
		{
			pDW = m_Displays.GetAtFast(i);
			if (bfound)
			{
				break;
			}
			if (pDW->IsActive())
			{
				bfound = TRUE;
			}
		}
		if (pDW == NULL && nSize > 0)
		{
			pDW = m_Displays.GetAtFast(0);
		}
		if (pDW)
		{
			TRACE(_T("Debug Test Autoselect for GOP Thread\n"));
			theApp.GetPanel()->PostMessage(WM_SET_CAMERA, (WORD)(pDW->GetID().GetSubID() & 0x00ff));
		}
*/
	}
#endif
	else
	{
		CFrameWnd::OnTimer(nIDEvent);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CMenu* pMenu = GetSystemMenu(FALSE);
	if (pMenu)
	{
		pMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
#ifdef NDEBUG
		SetWindowPos(&CWnd::wndTopMost, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE);
#endif	
	}

	if (theApp.GetIntValue(_T("ShowStatusBar")))
	{
		if (m_wndStatusBar.Create(this))
		{
			if (m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
			{
				int i, nSize, nCount = m_wndStatusBar.GetCount();
				UINT nID, nStyle;
				for (i=1; i<nCount; i++)
				{
					m_wndStatusBar.GetPaneInfo(i, nID, nStyle, nSize);
					m_wndStatusBar.SetPaneInfo(i, nID, nStyle, 30);
					m_wndStatusBar.SetPaneText(i, _T(""), FALSE);
				}
			}
			else
			{
				m_wndStatusBar.DestroyWindow();
			}
		}
	}
	m_bWindowsCreationComplete = TRUE;

	RegisterDeviceDetect(m_hWnd, NULL);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnIndicateAlarm(BOOL bAlarm)
{
	if (bAlarm)
	{
		if (   (m_uFlashTimer == 0)
			&& (m_Displays.GetSize() > 0)
			)
		{
			m_uFlashTimer = SetTimer(TIMER_FLASH, 500, NULL);
			if (m_uFlashTimer)
			{
				IndicateAlarmToUser(TRUE);
			}
		}
	}
	else
	{
		BOOL bAlarm = FALSE;
		CDisplayWindow* pDW = NULL;
		m_Displays.Lock(_T(__FUNCTION__));
		for (int i=0;i<m_Displays.GetSize();i++)
		{
			pDW = m_Displays.GetAtFast(i);
			if (pDW && pDW->GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
			{
				bAlarm |= ((CLiveWindow*)pDW)->IsAlarmActive();
			}
		}
		m_Displays.Unlock();
		if (bAlarm == FALSE)
		{
			if (m_uFlashTimer != 0)
			{
				if (KillTimer(m_uFlashTimer))
				{
					m_uFlashTimer = 0;
					IndicateAlarmToUser(FALSE);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::IndicateAlarmToUser(BOOL bAlarm)
{
	if (bAlarm)
	{
		// Flash Taskbar always
		FlashWindow(TRUE);
		if(theApp.AudibleAlarm())
		{
//			Beep(250, 250);
			Beep(440, 250);
		}
	}
	else
	{
		FlashWindow(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAddBackup()
{

	//only add BackupWindow if it not yet exits in m_Displays
	CDisplayWindow* pDisplayWnd = NULL;
	m_Displays.Lock(_T(__FUNCTION__));
	for (int i=0 ; i<m_Displays.GetSize() ; i++)
	{
		pDisplayWnd = m_Displays.GetAtFast(i);
		if (pDisplayWnd->GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
		{
			//BackupWindow already exists
			m_Displays.Unlock();
			m_evBackupWindow.SetEvent();
			return;
		}
	}
	m_Displays.Unlock();

	CRect rect;
	GetClientRect(rect);

	rect.right /= m_iXWin;
	rect.bottom /= m_iXWin;

	WK_TRACE(_T("create new backup window \n"));
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		if(pServer->IsDatabaseConnected())
		{
			CBackupWindow* pBW = new CBackupWindow(this, pServer);

			if (pBW)
			{
				if (!pBW->Create(rect, this))
				{
					delete pBW;
					return;
				}
				AddDisplayWindow(pBW);
//				DoAutoViewMode();
//				pBW->Activate();				//BackupWindow wird großes Fenster
				CPanel* pPanel = theApp.GetPanel();
				if(pPanel)
				{
					pPanel->SetCamera(pPanel->GetActiveCamera(), Backup);
				}
				m_evBackupWindow.SetEvent();
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("CMainFrame::OnAddBackup(): no connection to DB\n"));
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("CMainFrame::OnAddBackup(): Couldn`t get Server\n"));
	}
}


/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDeleteBackup()
{
	CDisplayWindow* pDW = NULL;
	m_Displays.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Displays.GetSize();i++) 
	{
		pDW = m_Displays.GetAtFast(i);

		if (pDW && pDW->GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
		{
			CBackupWindow* pBW = (CBackupWindow*)pDW;
			m_Displays.SafeRemove(pDW);
			pBW->DestroyWindow();
			WK_DELETE(pBW);
			CPanel* pPanel = theApp.GetPanel();
			if(pPanel)
			{
				pPanel->SetCamera(pPanel->GetActiveCamera(), LivePlay);
			}

		}
	}
	m_Displays.Unlock();
//	DoAutoViewMode();	
}

/////////////////////////////////////////////////////////////////////////////
CBackupWindow* CMainFrame::GetBackupWindow()
{
	if(theApp.GetServer() && theApp.GetServer()->CanMultiCDExport())
	{
		if (m_bDestroying)
		{
			return NULL;
		}

		CDisplayWindow* pDW = NULL;
		
		CAutoCritSec acs(&m_Displays.m_cs);
		for (int i=0;i<m_Displays.GetSize() && !m_bDestroying;i++) 
		{
			pDW = m_Displays.GetAtFast(i);

			if (pDW && pDW->GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
			{
				return (CBackupWindow*)pDW;
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDebugWindow* CMainFrame::GetDebugWindow()
{
	if (m_bDestroying)
	{
		return NULL;
	}
	CAutoCritSec acs(&m_Displays.m_cs);
	for (int i=0;i<m_Displays.GetSize() && !m_bDestroying;i++) 
	{
		if (m_Displays.GetAtFast(i)->GetType() == DISPLAYWINDOW_DEBUG)
		{
			return (CDebugWindow*)m_Displays.GetAtFast(i);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::DeleteBackupWindow()
{
	PostMessage(WM_USER, MAKELONG(WPARAM_DEL_BACKUP,0), 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	DoAutoViewMode();	
}

/////////////////////////////////////////////////////////////////////////////
// Stellt sicher, daß die Z-Order vom Mainframe nicht geändert wird. Dadurch
// wird verhindert, daß das Panel unter dem Mainframe liegt. Und beide trotzdem
// Topmost sind. (Ist nur im Empfänger wichtig. Und für die Readonly version)
void CMainFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{	
	if (theApp.IsReceiver() || theApp.IsReadOnly())
	{
		if (m_bWindowsCreationComplete)
		{		
			//wenn das Panel nicht mehr da ist, gibt es bei allen 95/98 usw. Systemem einen
			//Absturz von Windows
			CPanel* pPanel = theApp.GetPanel();
			if(pPanel)
			{
				lpwndpos->flags &= ~SWP_NOZORDER;
				lpwndpos->hwndInsertAfter = HWND_NOTOPMOST;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::LivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, CSecID idCam)
{
	if (m_pLiveWindowThread && pict.GetNumberOfPictures() <= 1)
	{
		m_pLiveWindowThread->LivePictureData(pict, dwX, dwY, idCam);
	}
	else
	{
		CLiveWindow* pLW = GetLiveWindow(idCam.GetSubID());
		if (pLW)
		{	// set MD values invalid, the are sent through input connection
			pLW->PictureData(pict, 0xffffffff, 0xffffffff);
		}
		else
		{
			WK_TRACE_ERROR(_T("no live window for image cam id = %08lx\n"),idCam.GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int	CMainFrame::GetLiveWindowQueueLength()
{
	if (m_pLiveWindowThread)
	{
		return m_pLiveWindowThread->GetQueueLength();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetPanetext(int nPane, LPCTSTR sText)
{
	if (m_wndStatusBar.m_hWnd && nPane < m_wndStatusBar.GetCount())
	{
		m_wndStatusBar.SetPaneText(nPane, sText);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateDummy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetMaintenanceText(CString sValue)
{
	m_sMaintenanceText = sValue;
}

/////////////////////////////////////////////////////////////////////////////
CString CMainFrame::GetMaintenanceText()
{
	return m_sMaintenanceText;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateLogoFont()
{
	if (m_bLogoValid)
	{
		CFont Font;
		Font.CreatePointFont(9*10, GetVarFontFaceName());
		m_JpegLogo.SetFont1(Font);
	}
}
//TODO TKR für Sync Abspielen
/*
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow* CMainFrame::FindOldestDisplayWindow(CSystemTime& st, BOOL bForward, WORD& wPlayingCam)
{
	CDisplayWindow* pRet = NULL;
	CDisplayWindow* pDisplayWnd = NULL;
	CSystemTime stOldest, stNewest, stCurr;

	CDisplayWindow* pPlayingCam = NULL;

	stOldest.GetLocalTime();
	m_Displays.Lock(_T(__FUNCTION__));

	for (int i=0 ; i<m_Displays.GetSize() ; i++)
	{
		pDisplayWnd = m_Displays.GetAtFast(i);
		if (pDisplayWnd->GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			if (   pDisplayWnd->IsShowable() 
				|| pDisplayWnd->IsWindowVisible())
			{
//TODO TKR	hier weiter	

				//stoppe die gerade abspielende Kamera
				CPlayWindow::PlayStatus ps = ((CPlayWindow*)pDisplayWnd)->GetPlayStatus();
				if(ps != CPlayWindow::PS_STOP)
				{
					if(pPlayingCam == NULL)
					{
						pPlayingCam = pDisplayWnd;
						wPlayingCam = (WORD)(((CPlayWindow*)pDisplayWnd)->GetID().GetSubID() & (MAX_CAM-1));
					}
					else
					{
						TRACE(_T("#### ERROR: more than one cam is playing\n"));
					}
	
					//((CPlayWindow*)pDisplayWnd)->SetPlayStatus(CPlayWindow::PS_STOP);
					//TRACE(_T("#######----- Kam %d gestoppt\n"), pDisplayWnd->GetID().GetSubID() & (MAX_CAM-1));
				}

				stCurr = pDisplayWnd->GetTimeStamp();
				WORD wCameraNr = pDisplayWnd->GetID().GetSubID();
				wCameraNr &= (MAX_CAM-1);
				CString sTimeStamp;
				sTimeStamp.Format(_T("#### Nr: %d  %s-%d"), wCameraNr, stCurr.GetDateTime(), stCurr.GetMilliseconds());
				TRACE(_T("%s\n"),sTimeStamp);

				if(bForward)
				{
					if (stCurr < stOldest)
					{
						stOldest = stCurr;
						pRet = pDisplayWnd;
					}
				}
				else
				{
					if (stCurr > stNewest)
					{
						stNewest = stCurr;
						pRet = pDisplayWnd;
					}
				}
			}
		}
	}

	m_Displays.Unlock();

	st = stOldest;
	m_pSyncOldestPlayWnd = (CPlayWindow*)pRet;

	return pRet;
}

/////////////////////////////////////////////////////////////////////////////
CPlayWindow* CMainFrame::GetOldestPlayWindow()
{
	return m_pSyncOldestPlayWnd;
}
*/
