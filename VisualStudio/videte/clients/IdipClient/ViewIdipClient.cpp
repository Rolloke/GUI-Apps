// IdipClientView.cpp : implementation of the CViewIdipClient class
//

#include "stdafx.h"
#include "IdipClient.h"
#include ".\viewidipclient.h"

#include "IdipClientDoc.h"
#include "Mainfrm.h"
#include "ViewArchive.h"
#include "ViewCamera.h"
#include "ViewPrintPreview.h"

#include "WndAlarmList.h"
#include "WndCoco.h"
#include "WndDib.h"
#include "WndHtml.h"
#include "WndMico.h"
#include "WndMpeg4.h"
#include "WndPlay.h"
#ifndef _DTS
	#include "WndPT.h"
#endif
#include "WndReference.h"
#include "WndText.h"

#include "CIPCOutputIdipClientDecoder.h"
#include "CIPCAudioIdipClient.h"

#include "DlgInputDeactivation.h"
#include "DlgExternalFrame.h"
#include "DlgHTMLmap.h"
#include "DlgBackupOld.h"
#include "DlgBackupNew.h"
#include "DlgQuery.h"
#include "DlgBarIdipClient.h"

#include "oemgui\DlgLoginNew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define QUEUE_PLAY_WND_PICTURES 0x00000010

/////////////////////////////////////////////////////////////////////////////
// CPictureBufferThread
/////////////////////////////////////////////////////////////////////////////
CPictureBufferThread::CPictureBufferThread(const CString sName, LPVOID lpContext): 
	CWK_Thread(sName, lpContext)
{
	m_dwThreadID = 0;
	m_dwWaitTime = 5;
	m_dwStatTime = 5*60*60*1000; // alle 5 h
}
/////////////////////////////////////////////////////////////////////////////
CPictureBufferThread::~CPictureBufferThread()
{
	m_Pictures.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPictureBufferThread::Run(LPVOID lpContext)
{
	if (m_dwThreadID == 0)
	{
		m_dwThreadID = GetCurrentThreadId();
	}

	MSG msg;
	int nSleep;
	while(PeekMessage(&msg, NULL, 0, 0, TRUE)) 
	{
		nSleep = 10;
		switch(msg.message)
		{
			case WM_PICTUREDATA:
				OnPictureData(msg.wParam, msg.lParam);
				nSleep = 0;
				break;
			case WM_QUIT:
				m_dwThreadID = 0;
				return FALSE;
			default:
				TRACE(_T("Msg:%d, %x\n)"), msg.message, msg.message);
				break;
		}

		int nSpan = GetTimeSpanSigned(theApp.m_dwIdleTickCount);
		if (nSpan > 1000)
		{
			WK_TRACE(_T("No Idle for %d ms\n"), nSpan);
			theApp.GetMainFrame()->PostMessage(WM_USER, WPARAM_CHECK_PERF_LEVEL);
		}
		else if (nSpan > 5000)
		{
			theApp.CalcCPUusage();
			theApp.CheckPerformanceLevel();
		}
//		else
//		{
//			TRACE(_T("Last Idle %d ms ago\n"), dwDiff);
//		}
		Sleep(nSleep);
	};

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPictureBufferThread::PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_dwThreadID)
	{
		return ::PostThreadMessage(m_dwThreadID, nMsg, wParam, lParam);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CPictureBufferThread::LivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, WORD wServer, CSecID idCam, CSecID idArchive)
{
	CPictureData *pData = new CLivePictureData(pict, dwX, dwY, wServer, idCam, idArchive);
	m_Pictures.SafeAddTail(pData);
	PostThreadMessage(WM_PICTUREDATA, (WPARAM)pData, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CPictureBufferThread::PlayPictureData(DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCPictureRecord &pict, const CIPCFields& fields, BOOL  bShowQueryResults, WORD wServer, WORD wArchiveNr, WORD wSequenceNr)
{
	CPictureData *pData = new CPlayPictureData(dwRecordNr, dwNrOfRecords, pict, fields, bShowQueryResults, wServer, wArchiveNr, wSequenceNr);
	m_Pictures.SafeAddTail(pData);
	PostThreadMessage(WM_PICTUREDATA, (WPARAM)pData, 0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPictureBufferThread::OnPictureData(WPARAM wParam, LPARAM lParam)
{
	if (m_Pictures.GetSize())
	{
		CPictureData *pData = m_Pictures.SafeGetAndRemoveHead();
		CViewIdipClient*pVIC = (CViewIdipClient*)m_lpContext;
		if (theApp.GetMainFrame()->HasStatusBar())
		{
			theApp.GetMainFrame()->PostMessage(WM_USER, MAKELONG(WPARAM_PANE_INT, 6), m_Pictures.GetSize());
		}
		pVIC->LockSmallWindows();
		if (pData->m_nType == LIVE_PICTURE)
		{
			CLivePictureData*pLPD = (CLivePictureData*)pData;
			CWndLive * pLW = pVIC->GetWndLive(pLPD->m_wServer, pLPD->m_idCam);
			if (pLW)
			{
				pLW->PictureData(*pLPD->m_pPict, pLPD->m_idArchive, pLPD->m_dwX, pLPD->m_dwY);
			}
			else
			{
				WK_TRACE_ERROR(_T("no live window for image cam id = %08lx on server id %d\n"), pLPD->m_idCam.GetID(), pLPD->m_wServer);
			}
		}
		else if (pData->m_nType == PLAY_PICTURE)
		{
			CPlayPictureData*pPPD = (CPlayPictureData*)pData;
			CWndPlay * pPW = pVIC->GetWndPlay(pPPD->m_wServer, pPPD->m_wArchiveNr, pPPD->m_wSequenceNr);
			if (pPW)
			{
				pPW->PictureData(pPPD->m_dwRecordNr, pPPD->m_dwNrOfRecords, *pPPD->m_pPict, pPPD->m_Fields, pPPD->m_bShowQueryResults);
			}
			else
			{
				WK_TRACE_ERROR(_T("no play window for image (archive %d, sequence %d) on server id %d\n"), pPPD->m_wArchiveNr, pPPD->m_wSequenceNr, pPPD->m_wServer);
			}
		}
		pVIC->UnlockSmallWindows();
		WK_DELETE(pData);
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CIdipClientView

IMPLEMENT_DYNCREATE(CViewIdipClient, CView)

BEGIN_MESSAGE_MAP(CViewIdipClient, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_1, OnView1)
	ON_COMMAND(ID_VIEW_2_2, OnView22)
	ON_COMMAND(ID_VIEW_3_3, OnView33)
	ON_COMMAND(ID_VIEW_4_4, OnView44)
	ON_UPDATE_COMMAND_UI(ID_VIEW_1, OnUpdateView1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_2_2, OnUpdateView22)
	ON_UPDATE_COMMAND_UI(ID_VIEW_3_3, OnUpdateView33)
	ON_UPDATE_COMMAND_UI(ID_VIEW_4_4, OnUpdateView44)
	ON_COMMAND(ID_VIDEO_FEIN, OnVideoFein)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_FEIN, OnUpdateVideoFein)
	ON_COMMAND(ID_VIDEO_GROB, OnVideoGrob)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_GROB, OnUpdateVideoGrob)
	ON_COMMAND(ID_VIDEO_CSB, OnVideoCsb)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_CSB, OnUpdateVideoCsb)
	ON_COMMAND(ID_VIDEO_SAVE, OnVideoSave)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SAVE, OnUpdateVideoSave)
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORIGINAL, OnUpdateViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_SEQUENZE, OnUpdateSequenze)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_1_PLUS, OnView1Plus)
	ON_UPDATE_COMMAND_UI(ID_VIEW_1_PLUS, OnUpdateView1Plus)
	ON_COMMAND(ID_VIEW_MONITOR16, OnViewMonitor)
	ON_COMMAND(ID_VIEW_TITLE_OF_SMALLWINDOWS, OnViewTitleOfSmallwindows)
	ON_COMMAND(ID_VIEW_TITLE_IN_PICTURE, OnViewTitleInPicture)
	// Standard printing commands
	ON_COMMAND(ID_TOGGLE, OnToggle)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_HTML_PAGE, OnEditHtmlPage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HTML_PAGE, OnUpdateEditHtmlPage)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TITLE_OF_SMALLWINDOWS, OnUpdateViewTitleOfSmallwindows)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TITLE_IN_PICTURE, OnUpdateViewTitleInPicture)
	ON_COMMAND(ID_EDIT_CREATE_HTML_CAMERA_PAGE, OnEditCreateHtmlCameraPage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATE_HTML_CAMERA_PAGE, OnUpdateEditCreateHtmlCameraPage)
	ON_COMMAND(ID_EDIT_CREATE_HTML_HOST_PAGE, OnEditCreateHtmlHostPage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATE_HTML_HOST_PAGE, OnUpdateEditCreateHtmlHostPage)
	ON_COMMAND(ID_EDIT_HTML_MAP_PROPERTIES, OnEditHtmlMapProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HTML_MAP_PROPERTIES, OnUpdateEditHtmlMapProperties)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_WND_THIS, OnUpdateSmallClose)
	ON_COMMAND(ID_FILE_OPEN_DISK, OnFileOpenDisk)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_DISK, OnUpdateFileOpenDisk)
	ON_COMMAND(ID_CLOSE_WND_ALL, OnCloseWndSmallAll)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_WND_ALL, OnUpdateCloseWndSmallAll)
	ON_COMMAND(ID_CLOSE_WND_LIVE_ALL, OnCloseWndSmallAllLive)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_WND_LIVE_ALL, OnUpdateCloseWndSmallAllLive)
	ON_COMMAND(ID_CLOSE_WND_PLAY_ALL, OnCloseWndSmallAllPlay)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_WND_PLAY_ALL, OnUpdateCloseWndSmallAllPlay)
	ON_COMMAND(ID_SEQUENCE_OVER, OnSequenceOver)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_OVER, OnUpdateSequenceOver)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI_RANGE(ID_NAVIGATE_EJECT, ID_NAVIGATE_END, OnUpdateNavigateRange)
	ON_COMMAND_RANGE(ID_NAVIGATE_EJECT, ID_NAVIGATE_END, OnNavigateRange)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_USER, OnUser)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewIdipClient construction/destruction

CViewIdipClient::CViewIdipClient()
{
	m_crKeyColor = SKIN_COLOR_KEY;// RGB(255,0,255);
	m_iFontHeight = 11;
	m_iXWin = 1; // must never be 0, is use as divisor!
	m_b1to1 = FALSE;
	m_bView1Plus = FALSE;
	m_pHostMapWindow = NULL;
	m_dwIDCounter = 1;
	m_dwTimerCounter = 0;
	m_pWndSmallCmdActive = NULL;
	m_pWndSmallBig = NULL;
	m_bSizing = FALSE;

	m_pDlgBackup		  = NULL;
	m_bSequenceOver = TRUE;
	m_bSequenceRealtime = FALSE;
	m_bUncheckNavigateForward = FALSE;
	m_bUncheckNavigateFastForward = FALSE;
	m_bUncheckNavigateBack = FALSE;
	m_bUncheckNavigateFastBack = FALSE;

	m_bPreviewPrinting = FALSE;
	m_bLockSmallWindowResize = FALSE;

	m_pPictureBufferThread = NULL;
	if (theApp.m_dwPictureBufferThreadFlags)
	{
		m_pPictureBufferThread = new CPictureBufferThread(_T("PictureBufferThread"), (LPVOID)this);
		m_pPictureBufferThread->StartThread();
	}

#ifdef _DEBUG
	m_SmallWindows.m_nCurrentThread = 0;	// to see the lock states !!
#endif
}
/////////////////////////////////////////////////////////////////////////////
CViewIdipClient::~CViewIdipClient()
{
	WK_DELETE(m_pPictureBufferThread);

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndSmall* pSW;
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows[i];
		WK_DELETE(pSW);
	}
	m_SmallWindows.RemoveAll();
	acs.Unlock();

	// Do not delete m_pDlgBackup here, is self-deleting
	m_QueryResultsToShow.SafeDeleteAll();
}
void CViewIdipClient::LockSmallWindows(LPCTSTR sFkt)
{
	m_SmallWindows.Lock(sFkt);
}
void CViewIdipClient::UnlockSmallWindows()
{
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnIdle()
{
	CWndSmall* pSW = NULL;
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->IsWndPlay())
		{
			((CWndPlay*)pSW)->OnIdle();
		}
#ifdef _DEBUG
		else if (pSW->GetType() == WST_MICO)
		{
			((CWndMico*)pSW)->OnIdle();
		}
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnTab(BOOL bNext)
{
	int i, nSize = m_SmallWindows.GetSize();
	if (nSize > 1)
	{
		CAutoCritSec acs(&m_SmallWindows.m_cs);
		BOOL bFound = FALSE;
		if (bNext)
		{
			for (i=0; i<nSize; i++)
			{
				CWndSmall *pSW = m_SmallWindows.GetAtFast(i);
				if (bFound)
				{
					SetWndSmallCmdActive(pSW);
					break;
				}
				else if (pSW == m_pWndSmallCmdActive)
				{
					bFound = TRUE;
				}
			}
			if (i==nSize)
			{
				SetWndSmallCmdActive(m_SmallWindows.GetAtFast(0));
			}
		}
		else
		{
			for (i=nSize-1; i>=0; i--)
			{
				CWndSmall *pSW = m_SmallWindows.GetAtFast(i);
				if (bFound)
				{
					SetWndSmallCmdActive(pSW);
					break;
				}
				else if (pSW == m_pWndSmallCmdActive)
				{
					bFound = TRUE;
				}
			}
			if (i<0)
			{
				SetWndSmallCmdActive(m_SmallWindows.GetAtFast(nSize-1));
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::ShowMapWindow()
{
	if (m_pHostMapWindow)
	{
		m_pHostMapWindow->SendMessage(WM_COMMAND,ID_CLOSE_WND_THIS);
	}
	else
	{
		CString sMapURL = theApp.GetMapURL();
		if (!sMapURL.IsEmpty())
		{
			if (DoesFileExist(sMapURL))
			{
				CRect rect;
				GetClientRect(rect);
				m_pHostMapWindow = new CWndHTML(theApp.GetNextHtmlWindowID(), sMapURL);
				m_pHostMapWindow->Create(rect,1,this);
				m_pHostMapWindow->NavigateTo(sMapURL);
				AddWndSmall(m_pHostMapWindow);
			}
		}
	}

	SetWndSmallCmdActive(m_pHostMapWindow);
	if(IsView1plus())
	{
		SetWndSmallBig(m_pHostMapWindow);
	}
	else if(Is1x1())
	{
		//if view 1-mode is active, switch active html into 1-mode
		SetView1();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnInitialUpdate() 
{
	m_Images.Create(IDB_ALARM,_IMAGE_WIDTH,0, RGB(0,255,255));
	CView::OnInitialUpdate();
}
/////////////////////////////////////////////////////////////////////////////
// CViewIdipClient drawing

void CViewIdipClient::OnDraw(CDC* pDC)
{
/*
	// Background is now white
	CIdipClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect,&m_CinemaBrush);
*/
}
void CViewIdipClient::OnFilePrint()
{
	CWndSmall* pSW;
	if (!m_bPreviewPrinting)
	{
		pSW = GetWndSmallCmdActive();
		if(pSW && pSW->IsWndLive())
		{
			//copy the picture for printing
			CWndLive* pLW = (CWndLive*)pSW;
			pLW->CopyPictureForPrinting();
			m_bPreviewPrinting = TRUE;
		}
	}
	CView::OnFilePrint();
	pSW = GetWndSmallCmdActive();
	if(pSW && pSW->IsWndLive())
	{
		//copy the picture for printing
		CWndLive* pLW = (CWndLive*)pSW;
		pLW->DeletePictureForPrinting();
	}
	m_bPreviewPrinting = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnFilePrintPreview()
{
	CPrintPreviewState* pState = new CPrintPreviewState;

	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this,
		RUNTIME_CLASS(CViewPrintPreview), pState))
	{
		// In derived classes, reverse special window handling here for
		// Preview failure case

		TRACE(traceAppMsg, 0, "Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	CWndSmall* pSW = GetWndSmallCmdActive();
	if (pSW)
	{
		if (   pSW->IsKindOf(RUNTIME_CLASS(CWndAlarmList))
			|| pSW->IsKindOf(RUNTIME_CLASS(CWndText))
			|| pSW->IsKindOf(RUNTIME_CLASS(CWndHTML)))
		{
			return pSW->OnPreparePrinting(pInfo);
		}
	}

	return DoPreparePrinting(pInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CWndSmall* pSW = GetWndSmallCmdActive();
	if(pSW)
	{
		GetDocument()->SetTitle(pSW->GetTitleText(pDC));
		if (pSW->IsWndLive() && !m_bPreviewPrinting)
		{
			//copy the picture for printing
			CWndLive* pLW = (CWndLive*)pSW;
			pLW->CopyPictureForPrinting();
		}

	}
	//baseclass does nothing
	//CView::OnBeginPrinting(pDC, pInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CWndSmall* pSW = GetWndSmallCmdActive();
	if (pSW)
	{
		pSW->OnPrint(pDC,pInfo);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	CWndSmall* pSW = GetWndSmallCmdActive();
	if(pSW && pSW->IsWndLive() && !m_bPreviewPrinting)
	{
		//copy the picture for printing
		CWndLive* pLW = (CWndLive*)pSW;
		pLW->DeletePictureForPrinting();
	}
	
	//baseclass does nothing
	//CView::OnEndPrinting(pDC, pInfo);
}
/////////////////////////////////////////////////////////////////////////////
// CViewIdipClient diagnostics
#ifdef _DEBUG
void CViewIdipClient::AssertValid() const
{
	CView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CIdipClientDoc* CViewIdipClient::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIdipClientDoc)));
	return (CIdipClientDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
int CViewIdipClient::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// for Coco Color Key
	if (!m_KeyBrush.CreateSolidBrush(m_crKeyColor))
		return -1;
	
	if (!m_CinemaBrush.CreateSolidBrush(CSkinDialog::GetBaseColor()))
		return -1;
	
	if (!m_TitleFont.CreateFont(-11,0,0,0,400,0,0,0,0,0,0,0,0,_T("Arial")))
		return -1;

	CDC* pDC;
	CFont* pOldFont;
	TEXTMETRIC tm;

	pDC = GetDC();
	pOldFont = pDC->SelectObject(&m_TitleFont);
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);
	m_iFontHeight = tm.tmHeight + tm.tmExternalLeading;
	ReleaseDC(pDC);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CIPCOutputRecord* pRec = NULL;
	CIdipClientDoc* pDoc = GetDocument();
	CServer* pServer = pDoc->GetServer(wServerID);

	switch (cause)
	{
	case NULL:	// called by CView::OnInitialUpdate()
	case VDH_INPUT:
	case VDH_OUTPUT:
		break;
	case VDH_REMOVE_SERVER:
	{
		CloseWndSmallAll(wServerID);
		const CServers& servers = GetDocument()->GetServers();
		BOOL bAnyAlarm = FALSE;
		CAutoCritSec acs((CCriticalSection*)&servers.m_cs);
		for (int i=0; i<servers.GetSize(); i++)
		{
			CServer *pServer = servers.GetAtFast(i);
			if (   wServerID != pServer->GetID()// this server is removed,
				&& pServer->IsAlarm())			// so test all other servers
			{
				bAnyAlarm = TRUE;
				break;
			}
		}
		acs.Unlock();

		if (!bAnyAlarm)
		{
			theApp.m_bAlarmAnimation = FALSE;
			theApp.GetMainFrame()->ChangeFullScreenModus(FALSE|FULLSCREEN_FORCE);
			// TODO! RKE: should the User notice the Alarm even when its over
			// may be in an alarm list?
		}
	}break;
	case VDH_ADD_CAMWINDOW:
		ASSERT_KINDOF(CIPCOutputRecord, pHint);
		pRec = (CIPCOutputRecord*)pHint;
		AddWndLive(pServer,*pRec);
		break;
	case VDH_REMOVE_CAMWINDOW:
		ASSERT_KINDOF(CIPCOutputRecord, pHint);
		pRec = (CIPCOutputRecord*)pHint;
		DeleteWndSmall(wServerID,pRec->GetID());
		break;
	case VDH_ACTIVATE_REQUEST_CAM:
		ASSERT_KINDOF(CIPCOutputRecord, pHint);
		pRec = (CIPCOutputRecord*)pHint;
		SetWndLiveRequested(wServerID,pRec->GetID());
		break;
	case VDH_INFORMATION:
		AddHtmlWindow(pServer,CSecID((DWORD)pHint),_T(""));
		break;
	case VDH_INPUT_CONFIRM:
		DeleteWndSmall(wServerID,CSecID((DWORD)pHint));
		break;
	case VDH_DELETE_CAMWINDOW:
		DeleteWndSmall(wServerID,CSecID((DWORD)pHint));
		break;
	case VDH_INPUT_URL:
		{
			CString sURL = *((CString*)pHint);
			AddHtmlWindow(pServer,SECID_NO_ID,sURL);
		}
		break;
	case VDH_DEACTIVATE_CAMERAS:
		DeactivateCameras(wServerID);
		break;
	case VDH_NOTIFICATION_MESSAGE:
		{
			ASSERT_KINDOF(CStringHint, pHint);
			AddTextWindow(pServer,SECID_NO_ID, ((CStringHint*)pHint)->GetMessage());
		}
		break;
	case VDH_SINGLE_CAM:
		ASSERT_KINDOF(CIPCOutputRecord, pHint);
		pRec = (CIPCOutputRecord*)pHint;
		SingleWndLive(pServer,*pRec);
		break;
	case VDH_CAMERA_MAP:
	if (theApp.AllowHTMLmaps())
	{
		CSecID idHost(SECID_LOCAL_HOST); // Local Host als vorgabe;
		if (pServer)
		{
			idHost = pServer->GetHostID();
		}
		CString sFile;
		sFile.Format(_T("%s\\%08lx\\%s"), CNotificationMessage::GetWWWRoot(), idHost, MAIN_CAMERAMAP_FILE_NAME);
		if (DoesFileExist(sFile))
		{
			AddHtmlWindow(pServer,SECID_NO_ID, MAIN_CAMERAMAP_FILE_NAME);
		}
	} break;
	case VDH_CAM_MAP:
	if (theApp.AllowHTMLmaps())
	{
		CWndHTML *pHW = GetWndHtml(wServerID, MAIN_CAMERAMAP_FILE_NAME);
		if (pHW)
		{
			pHW->OnUpdate(pSender, lHint, pHint);
		}
	}
	break;
	case VDH_HOST_MAP:
	if (theApp.AllowHTMLmaps())
	{
		CWndHTML *pHW = GetWndHtml(0, _T(""));
		if (pHW)
		{
			pHW->OnUpdate(pSender, lHint, pHint);
		}
	}
	break;
	case VDH_AUDIO:
	case VDH_DATABASE:
	case VDH_DEL_SEQUENCE:
		{
			DWORD dw = (DWORD)pHint;
			WORD wArchivNr = LOWORD(dw);
			WORD wSequenceNr = HIWORD(dw);
			DeleteWndSmall(wServerID,CSecID(wArchivNr,wSequenceNr));
		}
		break;
	case VDH_ADD_ARCHIV:
		break;
	case VDH_DEL_ARCHIV:
		CloseWndPlayAllOfArchive(wServerID,(WORD)pHint);
		break;
	case VDH_ADD_SEQUENCE:
		break;
	case VDH_OPEN_SEQUENCE:
		ASSERT_KINDOF(CIPCSequenceRecord, pHint);
		AddWndPlay(pServer,(CIPCSequenceRecord*)pHint);
		break;
	case VDH_BACKUP:
		if (   pHint == NULL			// database disconnect
			&& pServer && m_pDlgBackup	// during backup to local server
			&& wServerID == m_pDlgBackup->GetServerID())
		{
			m_pDlgBackup->Close();
		}
		else
		{
			OnArchivBackup((CServer*)pHint);
		}
		break;
	case VDH_BACKUP_ADD_SEQUENCE:
		if (m_pDlgBackup)
		{
			ASSERT_KINDOF(CIPCSequenceRecord, pHint);
			m_pDlgBackup->AddSequence((CIPCSequenceRecord*)pHint);
		}
		break;
	case VDH_BACKUP_DEL_SEQUENCE:
		if (m_pDlgBackup)
		{
			ASSERT_KINDOF(CIPCSequenceRecord, pHint);
			m_pDlgBackup->DeleteSequence((CIPCSequenceRecord*)pHint);
		}
		break;
	case VDH_BACKUP_ADD_ARCHIVE:
		if (m_pDlgBackup)
		{
			ASSERT_KINDOF(CIPCArchivRecord, pHint);
			m_pDlgBackup->AddArchive((CIPCArchivRecord*)pHint);
		}
		break;
	case VDH_BACKUP_DEL_ARCHIVE:
		if (m_pDlgBackup)
		{
			ASSERT_KINDOF(CIPCArchivRecord, pHint);
			CIPCArchivRecord* pArchivRecord = (CIPCArchivRecord*)pHint;
			m_pDlgBackup->DeleteArchive(pArchivRecord->GetID(),TRUE);
		}
		break;
	case VDH_SYNC_DEL_ARCHIVE:
	case VDH_SELECT_SEQUENCE:
		break;
	case VDH_SET_CMD_ACTIVE_WND:
		{
			ASSERT_KINDOF(CIPCOutputRecord, pHint);
			pRec = (CIPCOutputRecord*)pHint;
			CWndSmall* pSW = GetWndLive(wServerID, pRec->GetID());
			SetWndSmallCmdActive(pSW);
			if(IsView1plus())
			{
				SetWndSmallBig(pSW);
			}
			else if(Is1x1())
			{
				//if view 1-mode is active, switch active html into 1-mode
				SetView1();
			}
		}
		break;
	case VDH_SET_AUDIO_INPUT_STATE:
		{
			ASSERT_KINDOF(CAudioUpdateHint, pHint);
			CIPCAudioIdipClient *pAudio = ((CAudioUpdateHint*)pHint)->GetAudio();
			
			CAutoCritSec acs(&m_SmallWindows.m_cs);
			int i, nSize = m_SmallWindows.GetSize();
			for (i=0; i<nSize; i++)
			{
				CWndSmall *pSW = m_SmallWindows.GetAtFast(i);
				if (pSW->IsWndPlay())
				{
					((CWndPlay*)pSW)->UpdateAudioState(pAudio);
				}
			}
		}
		break;
	default:
		TRACE(_T("unknown cause in CViewIdipClient::OnUpdate %d\n"),cause);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::LivePictureData(WORD wServerID, const CIPCPictureRecord &pict, CSecID id, DWORD dwMDX, DWORD dwMDY, CSecID idArchive)
{
//	TRACE(_T("CViewIdipClient::PictureData %d,image=%08lx,archive=%08lx\n"),wServerID,id.GetID(),idArchive.GetID());
	// Achtung in CIPCThread !
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pWndLive = GetWndLive(wServerID,id);
	if (pWndLive && pWndLive->GetServer()->IsConnectedOutput())
	{
		BOOL bUseLiveWndThread = FALSE;
		if (pict.GetNumberOfPictures() <= 1)
		{
			if (pWndLive->GetType() == WST_MPEG4)
			{
				bUseLiveWndThread = !theApp.m_bUseGOPThreadAlways;
			}
			else
			{
				bUseLiveWndThread = TRUE;
			}
		}
		if (m_pPictureBufferThread && bUseLiveWndThread)
		{
			m_pPictureBufferThread->LivePictureData(pict, dwMDX, dwMDY, wServerID, id, idArchive);
		}
		else
		{
			pWndLive->PictureData(pict, idArchive,dwMDX,dwMDY);
		}
		if (   !GetDocument()->GetSequence()
			|| pWndLive->GetServer()->IsMultiCamera())
		{
			for (int i=0;i<m_SmallWindows.GetSize();i++)
			{
				CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
				if (   pSW->IsWndLive()
					&& (pSW->GetServerID() == pWndLive->GetServerID())
					)
				{
					CWndLive* pWndLive2 = (CWndLive*)pSW;
					if (pWndLive2->IsWaitingForRequest())
					{
						TRACE(_T("set waiting cam as requested %s\n"),pWndLive2->GetName());
						pWndLive2->DoRequestPictures();
						break;
					}
				}
			}
		}
	}
	else
	{
		// bitte nicht, so bekommt der server unnˆtige stops
		//theApp.GetMainFrame()->PostMessage(WM_USER, MAKELONG(WPARAM_UNREQUEST_PICTURES, wServerID), id.GetID());
		TRACE(_T("no window for image %d,%08lx\n"),wServerID, id.GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::PlayPictureData(DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCPictureRecord &pict, CWndPlay*pPW, const CIPCFields& fields, BOOL bShowQueryResults, WORD wServer, WORD wArchiveNr, WORD wSequenceNr)
{
	if (m_pPictureBufferThread && theApp.m_dwPictureBufferThreadFlags & QUEUE_PLAY_WND_PICTURES)
	{
		m_pPictureBufferThread->PlayPictureData(dwRecordNr, dwNrOfRecords, pict, fields, bShowQueryResults, wServer, wArchiveNr, wSequenceNr);
	}
	else
	{
		pPW->PictureData(dwRecordNr, dwNrOfRecords, pict, fields, bShowQueryResults);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::LockSmallWindowResizing(BOOL bLock)
{
	if (IsBetween(bLock, FALSE, TRUE))
	{
		m_bLockSmallWindowResize = bLock;
	}
	else
	{
#ifdef _DEBUG
		if (bLock == 2)
		{
			ShowWindow(SW_HIDE);
			m_bLockSmallWindowResize = TRUE;
		}
		else if (bLock < 0)
		{
			ShowWindow(SW_SHOW);
			m_bLockSmallWindowResize = FALSE;
		}
#endif
		return;
	}
	if (m_bLockSmallWindowResize == FALSE)
	{
		CWndSmall *pSW = NULL;
		CAutoCritSec acs(&m_SmallWindows.m_cs);
		if (m_SmallWindows.GetSize())
		{
			pSW = m_SmallWindows.GetAt(0);
		}
		if (pSW)
		{
			if (m_pWndSmallBig == NULL || m_pWndSmallCmdActive == NULL)
			{
				SetWndSmallCmdActive(pSW);
				SetWndSmallBig(pSW);
			}
			pSW->EnableTitle(theApp.m_bShowTitleOfSmallWindows);
			SortSmallWindowsByID(m_SmallWindows);
		//	m_SmallWindows.Unlock();
			acs.Unlock();

			// Set the appropriate quad view mode
			SetAppropriateQuadViewMode();
			SetWindowSize();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::AddWndSmall(CWndSmall* pSW, BOOL bIsAlarm /*=FALSE*/)
{
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	m_SmallWindows.Add(pSW);
	pSW->EnableTitle(theApp.m_bShowTitleOfSmallWindows);

	if (m_bLockSmallWindowResize)
	{
		return;
	}

	if (   (m_SmallWindows.GetSize() == 1)
		|| bIsAlarm)
	{
		// war das erste Fenster
		SetWndSmallCmdActive(pSW);
		SetWndSmallBig(pSW);
	}
	SortSmallWindowsByID(m_SmallWindows);
	acs.Unlock();

	// Set the appropriate quad view mode
	SetAppropriateQuadViewMode();
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::AddWndPlay(CServer* pServer, CIPCSequenceRecord* pSequence)
{
	CWndSmall* pSWnd = NULL;
	if(pSequence->IsAlarmList())
	{
		pSWnd = GetWndAlarmList(pServer->GetID(),
			pSequence->GetArchiveNr(),
			pSequence->GetSequenceNr());
	}
	else
	{
		pSWnd = GetWndPlay(pServer->GetID(),
			CSecID(pSequence->GetArchiveNr(),
			pSequence->GetSequenceNr()));
	}


	if (pSWnd)
	{
		// already open
	}
	else
	{
		CRect rect;

		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;
		CWndSmall* pSW = NULL;

		if (pSequence->IsAlarmList())
		{
			pSW = new CWndAlarmList(pServer, pSequence);
		}
		else
		{
			pSW = new CWndPlay(pServer, *pSequence);
		}

		if (pSW)
		{
			if (!pSW->Create(rect, this))
			{
				delete pSW;
				return;
			}
			AddWndSmall(pSW);
			SetWndSmallCmdActive(pSW);
//			if(IsView1plus())
			{
				SetWndSmallBig(pSW);
			}
//			else
			if(Is1x1())
			{
				//if view 1-mode is active, switch active html into 1-mode
				SetView1();
			}

			DWORD dwUserData = MAKELONG(pServer->GetID(),
				pSequence->GetSequenceNr()); // low,high
			if(pServer->GetDatabase())
			{
				if (pSequence->IsAlarmList())
				{
					pServer->GetDatabase()->DoRequestRecords(pSequence->GetArchiveNr(),pSequence->GetSequenceNr(),0,0);

				}
				else
				{
					pServer->GetDatabase()->DoRequestCameraNames(pSequence->GetArchiveNr(),
						dwUserData);
				}
				//			pDW->Navigate(1);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CloseWndPlayAll() 
{
// TODO GF check for bugs of merge
	CWndSmall* pSmall = NULL;
	CWndPlay* pPlay = NULL;
	
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=m_SmallWindows.GetSize()-1;i>=0;i--)
	{
		pSmall = m_SmallWindows.GetAtFast(i);
		if (pSmall->IsWndPlay())
		{
			pPlay = (CWndPlay*)pSmall;
			OnDeleteWindow(pPlay);
			pPlay->DestroyWindow();
			WK_DELETE(pPlay);
			m_SmallWindows.RemoveAt(i);
			ShowHideRectangle();
		}
	}
	// All PlayWindows are close prefer LiveWindows
	if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
	{	// no LiveWindow: take any
		FindNewCmdActiveWndSmall();
	}
	// m_SmallWindows.Unlock();
	acs.Unlock();
	SetAppropriateQuadViewMode();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetAppropriateQuadViewMode()
{
	CMainFrame *pFrame = theApp.GetMainFrame();

	if (     pFrame
		 && (pFrame->GetNoOfMonitors()       > 1) 
		 && (pFrame->GetNoOfExternalFrames() > 0))
	{
		CAutoCritSec acs(&m_SmallWindows.m_cs);
		int    i, j, nPossibleSW, nSW = m_SmallWindows.GetSize();

		// Alles schˆn, wenn gleichverteilt, aber...
		// ... Viel Spaﬂ wenn nicht!
		int iMaxMonitors = pFrame->GetNoOfExternalFrames() + 1;
		int iMonitorsPlusRegardless = iMaxMonitors + 1;
//		int iMonitor[iMonitorsPlusRegardless] = {0};
		CDWordArray dwaMonitor;
		dwaMonitor.SetSize(iMonitorsPlusRegardless);
		for (i=0 ; i<dwaMonitor.GetSize() ; i++)
		{
			dwaMonitor[i] = 0;
		}
		DWORD dwMonitorFlag = 0;
		CWndSmall* pSW = NULL;
		DWORD dwUseMonitor = 1;
		BOOL bRegardless = TRUE;
		for (i=0 ; i<nSW ; i++)
		{
			pSW = m_SmallWindows.GetAtFast(i);
			dwMonitorFlag = pSW->GetMonitorFlag();
			dwUseMonitor = 1;
			bRegardless = TRUE;
			for (j=1 ; j<=iMaxMonitors ; j++)
			{
				if (dwMonitorFlag & dwUseMonitor)
				{
					bRegardless = FALSE;
					dwaMonitor[j]++;
					break;
				}
				dwUseMonitor = dwUseMonitor << 1;
			}
			if (bRegardless)
			{
				dwaMonitor[0]++;
			}
		}

		if ((int)dwaMonitor[0] == nSW) // All Regardless
		{
			double dppm  = (double)nSW / ((double)pFrame->GetNoOfExternalFrames()+1);
			double dSqrt = sqrt(dppm);
			int    nIncrement = 0;
			m_iXWin = max ((int)dSqrt, 1);
			for (i=0; i<pFrame->GetNoOfExternalFrames(); i++)
			{
				pFrame->GetExternalFrame(i)->m_iXWin = m_iXWin;
			}
			do 
			{
				if      (nIncrement == 1) m_iXWin++;
				else if (nIncrement > 1)  pFrame->GetExternalFrame(nIncrement-2)->m_iXWin++;
				
				nPossibleSW = m_iXWin * m_iXWin;
				for (i=0; i<pFrame->GetNoOfExternalFrames(); i++)
				{
					nPossibleSW += (pFrame->GetExternalFrame(i)->m_iXWin*pFrame->GetExternalFrame(i)->m_iXWin);
				}
				nIncrement--;
				if (nIncrement <= 0)
				{
					nIncrement = pFrame->GetNoOfExternalFrames()+1;
				}
			} while (nSW > nPossibleSW);
			nPossibleSW = 0;
		}
		else // not all regardless
		{
			CDWordArray dwaXWin;
			dwaXWin.SetSize(iMaxMonitors);
			for (i=0 ; i<dwaXWin.GetSize() ; i++)
			{
				dwaXWin[i] = 0;
			}
			DWORD dwWinAllocated = 0;
			DWORD dwWinPossible = 0;
			double dSqrt = 0;
			for (i=0 ; i<iMaxMonitors ; i++)
			{
				dwWinAllocated += dwaMonitor[i+1];
				dSqrt = sqrt((double)(dwaMonitor[i+1]));
				dwaXWin[i] = max((int)ceil(dSqrt), 1);
				dwWinPossible += dwaXWin[i]*dwaXWin[i];
			}
			while (dwaMonitor[0] > dwWinPossible-dwWinAllocated) // Not all allocated
			{
				int iIndexMin = iMaxMonitors-1;
				for (i=iIndexMin ; i>1 ; i--)
				{
					if (dwaXWin[i-1] < dwaXWin[iIndexMin])
					{
						iIndexMin = i-1;
					}
				}
				if (dwaXWin[0] < (dwaXWin[iIndexMin]))
				{
					iIndexMin = 0;
				}
				dwaXWin[iIndexMin]++;
				dwWinPossible += (2 * dwaXWin[iIndexMin]) - 1;
			}
			m_iXWin = max ((int)dwaXWin[0], 1);
			for (i=0; i<pFrame->GetNoOfExternalFrames() ; i++)
			{
				pFrame->GetExternalFrame(i)->m_iXWin = max(dwaXWin[i+1], 1);
			}
		}
	}
	else // no external frame(s) or only 1 monitor
	{
		// m_iXWin has to be the up-rounded square root of all windows
		double	dSqrt = sqrt((double)(m_SmallWindows.GetSize()));
		int		iSqrt = (int)dSqrt;

		// Round up to next integer and
		// always keep > 0, will be used as divisor
		if (iSqrt != dSqrt)
		{
			iSqrt++;
		}

		m_iXWin = max(iSqrt, 1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SortSmallWindowsByID(CWndSmallArray& sw)
{
	int iCur, iMin, iNext, nCount;
	CWndSmall* swap;

	nCount = sw.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
//		TRACE(_T("HostID %04x\n"), sw.GetAtFast(iCur)->GetHostID());
//		TRACE(_T("    ID %08x\n"), sw.GetAtFast(iCur)->GetID());
		iMin = iCur;
		for (iNext=iCur+1; iNext<nCount; iNext++)
		{
			if (*sw.GetAtFast(iNext) < *sw.GetAtFast(iMin))
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = sw.GetAtFast(iCur);
			sw.SetAt(iCur,sw.GetAtFast(iMin));
			sw.SetAt(iMin,swap);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SortSmallWindowsByIDandTime(CWndSmallArray& sw)
{
	int iCur, iMin, iNext, nCount;
	CWndSmall* swap;
	CWndSmall *pSW1, *pSW2;
	CWndLive *pDW1, *pDW2;

	nCount = sw.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			pSW1 = sw.GetAtFast(iNext);
			pSW2 = sw.GetAtFast(iMin);

			// Only Live Windows can be sorted by time
			if (   pSW1->IsWndLive()
				&& pSW2->IsWndLive()
				)
			{
				pDW1 = (CWndLive*)sw.GetAtFast(iNext);
				pDW2 = (CWndLive*)sw.GetAtFast(iMin);
				if (*pDW1 < *pDW2)
				{
					iMin = iNext;
				}
			}
			// keep other smallwindows in normal order
			else if (*pSW1 < *pSW2)
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = sw.GetAtFast(iCur);
			sw.SetAt(iCur, sw.GetAtFast(iMin));
			sw.SetAt(iMin, swap);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SortSmallWindowsByMonitor(CWndSmallArray &sw, DWORD dwPrefer)
{
	int iCur, iMin, iNext, nCount;
	CWndSmall* swap, *pSW1, *pSW2;
	int nMonitor1, nMonitor2;

	nCount = sw.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur+1; iNext<nCount; iNext++)
		{
			pSW1 = sw.GetAtFast(iNext);
			pSW2 = sw.GetAtFast(iMin);
			nMonitor1 = (int) pSW1->GetMonitorFlag();
			nMonitor2 = (int) pSW2->GetMonitorFlag();
			if (dwPrefer == 0xffffffff)									// Don¥t Care nach hinten
			{
				if (nMonitor1 == 0) nMonitor1 |= 0x40000000;
				else if (nMonitor1 & USE_MONITOR_EXCLUSIVE) nMonitor1 |= 0x80000000;
				if (nMonitor2 == 0) nMonitor2 |= 0x40000000;
				else if (nMonitor2 & USE_MONITOR_EXCLUSIVE) nMonitor2 |= 0x80000000;

			}
			else																	// Normalfall: don¥t care vorne
			{
				if (dwPrefer & USE_MONITOR_1)								// Monitor 1 nach vorne
				{
					if (nMonitor1 == USE_MONITOR_1) nMonitor1 |= 0x80000000;
					if (nMonitor2 == USE_MONITOR_1) nMonitor2 |= 0x80000000;
				}
				if (dwPrefer & USE_MONITOR_2)								// Monitor 2 nach vorne
				{
					if (nMonitor1 == USE_MONITOR_2) nMonitor1 |= 0x80000000;
					if (nMonitor2 == USE_MONITOR_2) nMonitor2 |= 0x80000000;
				}
			}

			if (nMonitor1 < nMonitor2)
			{
				iMin = iNext;
			}
			else if (nMonitor1 == nMonitor2)
			{
				if (*pSW1 < *pSW2)
				{
					iMin = iNext;
				}
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = sw.GetAtFast(iCur);
			sw.SetAt(iCur,sw.GetAtFast(iMin));
			sw.SetAt(iMin,swap);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::AddReferenceWindow(CServer* pServer, CSecID id, 
									 const CString& sCameraName)
{
	CWndReference* pRW = GetWndReference(pServer->GetID(),sCameraName);
	
	if (pRW)
	{
		pRW->Reload();
	}
	else
	{
		CString sPath;
		sPath.Format(_T("%s\\%08lx\\%08lx.jpg"),CNotificationMessage::GetWWWRoot(),
			pServer->GetHostID().GetID(),id.GetID());

		if (DoesFileExist(sPath))
		{
			CRect rect;
			CWndReference* pReferenceWindow = NULL;

			if (!sCameraName.IsEmpty())
			{
				pReferenceWindow = new CWndReference(pServer,id,sCameraName);
			}
			
			if (pReferenceWindow)
			{
				GetClientRect(rect);
				rect.right /= m_iXWin;
				rect.bottom /= m_iXWin;
				pReferenceWindow->Create(rect,m_dwIDCounter++,this);
				AddWndSmall(pReferenceWindow);

				SetWndSmallCmdActive(pReferenceWindow);
				if(IsView1plus())
				{
					SetWndSmallBig(pReferenceWindow);
				}
				else if(Is1x1())
				{
					//if view 1-mode is active, switch active html into 1-mode
					SetView1();
				}
			}
		}
		else
		{
			CString s;
			s.LoadString(IDS_NO_REFERENCE);
			COemGuiApi::MessageBox(s,20);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::AddTextWindow(CServer* pServer,  CSecID id, 
								const CString& sMessage)
{
	CWndText* pTW = NULL;
	if (pServer)
	{
		pTW = GetWndText(pServer->GetID(),sMessage);
	}
	if (pTW)
	{
		pTW->SetNotificationText(sMessage);
	}
	else
	{
		CRect rect;
		CWndText* pTextWindow = NULL;

		if (!sMessage.IsEmpty())
		{
			pTextWindow = new CWndText(pServer, theApp.GetNextHtmlWindowID(), sMessage);
		}
		else if (pServer == NULL && id == SECID_NO_ID)
		{
			pTextWindow = new CWndText(pServer, id, sMessage);
		}
		
		if (pTextWindow)
		{
			GetClientRect(rect);
			rect.right /= m_iXWin;
			rect.bottom /= m_iXWin;
			pTextWindow->Create(rect,m_dwIDCounter++,this);
			AddWndSmall(pTextWindow);

			SetWndSmallCmdActive(pTextWindow);
			if(IsView1plus())
			{
				SetWndSmallBig(pTextWindow);
			}
			else if(Is1x1())
			{
				//if view 1-mode is active, switch active html into 1-mode
				SetView1();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML* CViewIdipClient::AddHtmlWindow(CServer* pServer,  CSecID id, const CString& sURL)
{
	CWndHTML* pHW=NULL;
	WORD wID = SECID_NO_SUBID;
	if (pServer)
	{
		wID = pServer->GetID();
	}
	
	if (id!=SECID_NO_ID)
	{
		pHW = GetWndHtml(wID, id);
	}
	else if (!sURL.IsEmpty())
	{
		pHW = GetWndHtml(wID, sURL);
	}

	if (pHW==NULL)
	{
		CRect rect;
		CWndHTML* pHtmlWindow = NULL;

		if (id.IsInputID()) 
		{
			CIPCInputIdipClient* pInput = pServer->GetInput();
			if (pInput)
			{
				pInput->Lock();
				CIPCInputRecord* pInputRecord = pInput->GetInputRecordPtrFromSecID(id);
				if (pInputRecord)
				{
					pHtmlWindow = new CWndHTML(pServer,id,pInputRecord->GetName());
				}
				pInput->Unlock();
			}
		}
		if (id.IsOutputID()) 
		{
			CIPCOutputIdipClient* pOutput = pServer->GetOutput();
			const CIPCOutputRecord& rec = pOutput->GetOutputRecordFromSecID(id);
			pHtmlWindow = new CWndHTML(pServer,id,rec.GetName());
		}
		else if (!sURL.IsEmpty())
		{
			pHtmlWindow = new CWndHTML(pServer,sURL, theApp.GetNextHtmlWindowID());
		}
		
		if (pHtmlWindow)
		{
			GetClientRect(rect);
			rect.right /= m_iXWin;
			rect.bottom /= m_iXWin;
			pHtmlWindow->Create(rect,m_dwIDCounter++,this);
			AddWndSmall(pHtmlWindow);
			pHW = pHtmlWindow;
		}
	}
	else
	{
		// make the window visible
		pHW->Refresh();
		pHW->ShowWindow(SW_SHOW);
//		pHW->SendMessage(WM_COMMAND,ID_CLOSE_WND_THIS);
	}

	// if an alarm server is supporting a camera id do not overwrite it with the
	// HTML window
	if (pServer && pServer->IsAlarm() && pServer->GetAlarmID() != SECID_NO_ID)
	{
		return pHW;
	}

	SetWndSmallCmdActive(pHW);
	if(IsView1plus())
	{
		SetWndSmallBig(pHW);
	}
	else if(Is1x1())
	{
		//if view 1-mode is active, switch active html into 1-mode
		SetView1();
	}
	return pHW;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::AddWndLive(CServer* pServer, const CIPCOutputRecord & rec)
{
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);

	CWndLive* pDW = GetWndLive(pServer->GetID(),rec.GetID());
	
	if (pDW)
	{
		// already open
		SetWndLiveRequested(pDW);
		SetWndSmallBig(pDW);
		acs.Unlock();
	}
	else
	{
		acs.Unlock();

		CRect rect;
		GetClientRect(rect);

		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		if (rec.CameraHasColor())
		{
			if (rec.CameraIsPresence())
			{
				pDW = new CWndPT(pServer, rec);
			}
			else if (rec.CameraDoesJpeg())
			{
				if (   (pServer->GetConfirmedOutputOptions() & SCO_JPEG_AS_H263)
					&& pServer->GetOutput()->GetBitrate()<=128*1024)
				{
					// Jpeg 2 H.263
					pDW = new CWndCoco(pServer, rec, TRUE);
				}
				else
				{
					pDW = new CWndMico(pServer, rec);
				}
			}
			else if (rec.CameraDoesMpeg())
			{
				// TODO! RKE: IPcam test remove !
/*#ifdef _DEBUG
				pDW = new CWndMico(pServer, rec);
#else*/
				pDW = new CWndMpeg4(pServer,rec);
//#endif
			}
			else if (rec.CameraIsYUV())
			{
				pDW = new CWndMico(pServer,rec);
			}
			else
			{
				BOOL bHardwareCocoAtTransmitter = TRUE;
				CString v = pServer->GetVersion();
				if (pServer->IsDTS())
				{
					// DTS hat niemals echten Coco
					bHardwareCocoAtTransmitter = FALSE;
				}
				else if ((_T("4.3") <= v) && (v<=_T("4.4.0.519")))
				{
					bHardwareCocoAtTransmitter = FALSE;
				}
				else if (v>=_T("4.7"))
				{
					bHardwareCocoAtTransmitter = FALSE;
				}
				pDW = new CWndCoco(pServer, rec, !bHardwareCocoAtTransmitter);
			}
		}
		else
		{
			// no black white anymore
		}

		if (pDW)
		{
			if (pDW->Create(rect, this))
			{
				BOOL bAlarm = (pServer->GetAlarmID() == rec.GetID());
				AddWndSmall(pDW, bAlarm);
				pDW->InitialRequest();
			}
			else
			{
				WK_DELETE(pDW);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnDeleteWindow(CWndSmall* pSW)
{
	if (m_pWndSmallCmdActive == pSW)
	{
		m_pWndSmallCmdActive = NULL;
		theApp.GetMainFrame()->GetDlgBarIdipClient()->SetPTZWindow(NULL);
	}
	if (m_pWndSmallBig == pSW)
	{
		m_pWndSmallBig = NULL;
	}
	if (m_pHostMapWindow==pSW)
	{
		m_pHostMapWindow = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SingleWndLive(CServer* pServer, const CIPCOutputRecord & rec)
{
	BOOL bCallSetWindowSize = FALSE;
	int i,c;
	CWndSmall* pWndSmall = NULL;
	CWndLive* pWndLive = NULL;
	CWndLive* pWndLiveSingle = NULL;
	WORD wServerID = pServer->GetID();
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);

	c = m_SmallWindows.GetSize();
	for (i=c-1 ; i>=0 ; i--)
	{
		pWndSmall = m_SmallWindows.GetAtFast(i);
		if (   pWndSmall
			&& pWndSmall->IsWndLive()
			&& (pWndSmall->GetServerID() == wServerID)
			)
		{
			// window is of same server and is WndLive
			pWndLive = (CWndLive*)pWndSmall;
			if (pWndLive->GetCamID() == rec.GetID())
			{
				pWndLiveSingle = pWndLive;
			}
			else
			{
				if (   pWndLive->CanMultipleRequests()
					|| (GetNrOfLiveWindows(wServerID) == 1)
					)
				{
					pWndLive->DeactivateCamera();
				}
				OnDeleteWindow(pWndLive);
				pWndSmall->DestroyWindow();
				WK_DELETE(pWndSmall);
				m_SmallWindows.RemoveAt(i);
				bCallSetWindowSize = TRUE;
			}
		}
	}
	// Close a live Window: prefer LiveWindows
	if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
	{	// if no LiveWindow: take Play or Dib Window
		if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndImageRecherche)))
		{	// if none: take any
			FindNewCmdActiveWndSmall();
		}
	}


	if (pWndLiveSingle == NULL)
	{
		acs.Unlock();
		AddWndLive(pServer,rec);
	}
	else
	{
		if (!pWndLiveSingle->IsRequesting())
		{
			pWndLiveSingle->SetActiveRequested();
		}
		else if (pWndLiveSingle->IsWaitingForRequest())
		{
			pWndLiveSingle->DoRequestPictures();
		}
		acs.Unlock();
	}

	if (bCallSetWindowSize)
	{
		SetAppropriateQuadViewMode();
		SetWindowSize();
	}
}
/////////////////////////////////////////////////////////////////////////////
int	CViewIdipClient::GetNrOfLiveWindows(WORD wServerID)
{
	int i,c;
	int r=0;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0 ; i<c ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->IsWndLive()
			&& pSW->GetServerID() == wServerID)
		{
			r++;
		}
	}
	return r;
}
/////////////////////////////////////////////////////////////////////////////
int	CViewIdipClient::GetNrOfWindows(WORD wServerID, EnumWndSmallType nType)
{
	int i,c;
	int r=0;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0 ; i<c ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->GetType() == nType
			&& pSW->GetServerID() == wServerID)
		{
			r++;
		}
	}
	return r;
}
/////////////////////////////////////////////////////////////////////////////
int	CViewIdipClient::GetNrOfLiveWindows()
{
	int i,c;
	int r=0;
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		if (m_SmallWindows.GetAtFast(i)->IsWndLive())
		{
			r++;
		}
	}
	return r;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetStore(BOOL bOn)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && pSW->IsWndLive())
		{
			((CWndLive*)pSW)->SetStore(bOn);
		}
	}
//	m_SmallWindows.Unlock();

//  SetStore not longer nessessary in CoolBar, because we don`t use a CoolBar anymore
//	CMainFrame* pMF = (CMainFrame*)theApp.GetMainWnd();
//	pMF->SetStore(bOn);
}
/////////////////////////////////////////////////////////////////////////////
CWndLive* CViewIdipClient::GetWndLiveCmdActive()
{
	if (   m_pWndSmallCmdActive
		&& m_pWndSmallCmdActive->IsWndLive())
	{
		if (!theApp.IsInMainThread())
		{
			WK_TRACE_ERROR(_T("GetWndLiveCmdActive() not called in MainThread"));
			CUnhandledException::TraceCallStack(NULL);
		}
		return (CWndLive*)m_pWndSmallCmdActive;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndSmall* CViewIdipClient::GetWndSmallCmdActive()
{
	if (m_pWndSmallCmdActive)
	{
		if (!theApp.IsInMainThread())
		{
			WK_TRACE_ERROR(_T("GetWndSmallCmdActive() not called in MainThread"));
			CUnhandledException::TraceCallStack(NULL);
		}
		return m_pWndSmallCmdActive;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndSmall* CViewIdipClient::GetWndSmallExclusive()
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	int i, nSize = m_SmallWindows.GetSize();
	if (nSize)
	{
		CWndSmall* pSW;
		for (i=0; i<nSize; i++)
		{
			pSW = m_SmallWindows.GetAtFast(i);
			if (pSW->GetMonitorFlag() & USE_MONITOR_EXCLUSIVE)
			{
				return pSW;
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWndLive*	CViewIdipClient::GetWndLive(WORD wServerID, CSecID camID)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();

	for (i=0 ; i<c ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->IsWndLive())
		{
			if (pSW->GetServerID() == wServerID)
			{
				CWndLive* pDW = (CWndLive*)pSW;
				if (pDW->GetCamID() == camID)
				{
					return pDW;
				}
				else
				{
					if (camID.IsInputID())
					{
						WORD wCameraNr = (WORD)(camID.GetSubID() & 0x001F);
						if (wCameraNr == pDW->GetCamID().GetSubID())
						{
							return pDW;
						}
					}
				}
			}
			else
			{
//					WK_TRACE_ERROR(_T("pSW server id wrong\n"));
			}
		}
		else
		{
//				WK_TRACE_ERROR(_T("pSW is no display window\n"));
		}
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::HaveAllWindowsGotAPicture(WORD wServerID)
{
	BOOL bRet = TRUE;

	CWndSmall* pSW = NULL;
	
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; i<m_SmallWindows.GetSize() && bRet ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);

		if (pSW)
		{
			if (pSW->IsWndLive())
			{
				if (pSW->GetServerID() == wServerID)
				{
					bRet &= ((CWndLive*)pSW)->WasSometimesActive();
				}
			}
		}
	}
//	m_SmallWindows.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::TraceSmallWindows()
{
	int i,c;
	CWndSmall* pSW = NULL;
	
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0 ; i<c ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW)
		{
			WK_TRACE(_T("sw %i s=%d c=%08lx\n"),i,pSW->GetServerID(),pSW->GetID().GetID());
		}
		else
		{
			WK_TRACE(_T("sw %i (null)\n"));
		}
	}
//	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CWndReference* CViewIdipClient::GetWndReference(WORD wServerID, 
												  const CString& sCameraName)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->GetServerID() == wServerID
			&& pSW->GetType()     == WST_REFERENCE)
		{
			ASSERT_KINDOF(CWndReference, pSW);
			CWndReference* pRW = (CWndReference*)pSW;
			if (sCameraName == pRW->GetCameraName())
			{
				return pRW;
			}
		}
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWndText* CViewIdipClient::GetWndText(WORD wServerID, const CString& sText)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->GetServerID() == wServerID
			&& pSW->GetType()     == WST_TEXT)
		{
			ASSERT_KINDOF(CWndText, pSW);
			return (CWndText*)pSW;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML* CViewIdipClient::GetWndHtml(WORD wServerID, const CString& sURL)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSW);
			if (pSW->GetServerID()==wServerID)
			{
				CWndHTML* pHW = (CWndHTML*)pSW;
				if (pHW->IsURL(sURL))
				{
					return pHW;
				}
			}
			else if (pSW->GetServerID() == SECID_NO_SUBID)
			{
				CServer *pServer = GetDocument()->GetLocalServer();
				if (pServer && pServer->GetID() == wServerID)
				{
					pSW->PostMessage(WM_COMMAND, ID_CLOSE_WND_THIS);
				}
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML* CViewIdipClient::GetWndHtml(WORD wServerID, CSecID id)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->GetServerID() == wServerID
			&& pSW->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSW);
			CWndHTML* pHW = (CWndHTML*)pSW;
			if (pHW->GetID()==id)
			{
				return pHW;
			}
		}
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::DeleteWndSmall(WORD wServerID, CSecID id)
{
	BOOL bSetAppropriateQuadViewMode = FALSE;
	int i,c;
	CWndSmall* pSW = NULL;
	BOOL bWasLiveActive = FALSE;
	BOOL bWasPlayActive = FALSE;
	BOOL bAnyWindowDeleted = FALSE;
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF == NULL) return;

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=c-1;i>=0;i--) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (pSW->GetID()==id)
			{
				if (pSW->IsWndLive())
				{
					CWndLive* pDW = (CWndLive*)pSW;
					bWasLiveActive = pDW->IsRequesting();
					if (   pDW->CanMultipleRequests()
						|| GetNrOfLiveWindows(wServerID)==1)
					{
						pDW->DeactivateCamera();
						bSetAppropriateQuadViewMode = TRUE;
					}
				}
				else if(pSW->IsWndPlay())
				{
					bWasPlayActive = TRUE;
					bSetAppropriateQuadViewMode = TRUE;
					
					CWndPlay* pWndPlay = ((CWndPlay*)pSW);
					PlayStatus ps = pWndPlay->GetPlayStatus();
					
					if(ps != PS_STOP)
					{
						pWndPlay->SetPlayStatus(PS_STOP);
						//if on delete wndplay the window still plays pictures, stop it before deleting
						switch(ps)
						{
							case PS_FAST_BACK:
								pWndPlay->SendMessage(WM_COMMAND,ID_NAVIGATE_FAST_BACK);
								break;
							case PS_PLAY_BACK:
								pWndPlay->SendMessage(WM_COMMAND,ID_NAVIGATE_BACK);
								break;
							case PS_PLAY_FORWARD:
								pWndPlay->SendMessage(WM_COMMAND,ID_NAVIGATE_FORWARD);
								break;
							case PS_FAST_FORWARD:
								pWndPlay->SendMessage(WM_COMMAND,ID_NAVIGATE_FAST_FORWARD);
								break;
							default:
								break;
						}
						UpdateWindow();

					}
				}
				else if (pSW->GetType() == WST_HTML)
				{
					ASSERT_KINDOF(CWndHTML, pSW);
					((CWndHTML*)pSW)->OnRequestSave();
					bSetAppropriateQuadViewMode = TRUE;
				}
				else
				{
					//alle anderen Fenster (Text, Alarmlist, Referenz)
					bSetAppropriateQuadViewMode = TRUE;
				}
				for (int j=0; j<pMF->GetNoOfExternalFrames(); j++)
				{
					pMF->GetExternalFrame(j)->RemoveWndSmall(pSW);
				}
				OnDeleteWindow(pSW);
				m_SmallWindows.RemoveAt(i);
				pSW->DestroyWindow();
				WK_DELETE(pSW);
				bAnyWindowDeleted = TRUE;

				// Werden keine Fenster mehr angezeigt?
				// Dann m¸ssen wir aus dem Vollbildmodus raus
				// sonst ist nur noch ein weiﬂer Bildschirm da...
				CheckSmallWindowsFullScreen();
				break;
			}
		}
	}
//	m_SmallWindows.Unlock();
	acs.Unlock();

	//TKR
	//nur in den quadratischen Modus wechseln, wenn das zu schlieﬂende WndSmall
	//auch wirklich nicht mehr angezeigt werden soll, sprich, es geschlossen wird 
	//und dieses zu schlieﬂende Fenster sich in der 1x1 Ansicht befand.
	//Nicht den Modus wechseln, wenn z.B. nur die ‰lteste Sequenz eines Ringarchivs
	//gelˆscht werden soll. Dann wird diese Funkton (DeleteWndSmall()) n‰mlich auch
	//aufgerufen. In diesem Fall soll die 1x1 Ansicht bestehen bleiben, es sei denn,
	//nicht nur eine Sequenz sondern das gesamte Archiv selbst soll gelˆscht 
	//werden. War diese zu lˆschende Archivfenster in der 1x1 Ansicht, dann 
	//auch in den quadratischen Modus wechseln
	if (m_bLockSmallWindowResize)
	{
		return;
	}

	if(bSetAppropriateQuadViewMode)
	{
		SetAppropriateQuadViewMode();
	}

	// Only arrange windows if any window was really deleted
	if (bAnyWindowDeleted)
	{
		SetWindowSize();
	}

	CIdipClientDoc* pDoc = GetDocument();
	pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW,wServerID),(CObject*)id.GetID());
	
	if (bWasLiveActive)
	{
		//LiveWindow was closed
		// prefer LiveWindows
		if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
		{	// If not found: take Play or Dib Windows
			if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndImageRecherche)))
			{	// If not found: take any
				FindNewCmdActiveWndSmall();
			}
		}
	
//		m_SmallWindows.Lock(_T(__FUNCTION__));
		CAutoCritSec acs(&m_SmallWindows.m_cs);
		if (m_pWndSmallCmdActive && m_pWndSmallCmdActive->IsWndLive())
		{
			((CWndLive*)m_pWndSmallCmdActive)->ActivateCamera();
		}
//		m_SmallWindows.Unlock();
	}
	else if(bWasPlayActive)
	{
		//PlayWindow was closed
		// Close any SmallWindow: prefer Play or Dib Window
		if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndImageRecherche)))
		{	// If not found: take LiveWindows
			if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
			{	// If not found: take any
				FindNewCmdActiveWndSmall();
			}
		}
	}
	else
	{   
		//any other window was closed
		// Close any SmallWindow: prefer Play or Dib Window
		if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
		{	// If not found: take LiveWindows
			if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndImageRecherche)))
			{	// If not found: take any
				FindNewCmdActiveWndSmall();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::DeactivateCameras(WORD wServerID)
{
	int i,c;
	CWndSmall* pSW = NULL;
	
//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=c-1;i>=0;i--) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (pSW->IsWndLive())
			{
				CWndLive* pDW = (CWndLive*)pSW;
				if (   pDW->IsCmdActive() 
					|| pDW->IsRequesting())
				{
					pDW->DeactivateCamera();
				}
			}
		}
	}
//	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CloseWndSmallAll(WORD wServerID)
{
	int i,c;
	CWndSmall* pSW = NULL;

//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=c-1 ; i>=0 ; i--) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSW);
			((CWndHTML*)pSW)->OnRequestSave();
		}
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			m_SmallWindows.RemoveAt(i);
			OnDeleteWindow(pSW);
			pSW->DestroyWindow();
			WK_DELETE(pSW);

			ShowHideRectangle();
		}
	}
	// m_SmallWindows.Unlock();
	acs.Unlock();

	if (   m_pDlgBackup 
		&& m_pDlgBackup->GetServerID() == wServerID)
	{
		m_pDlgBackup->DestroyWindow();
	}

	// Close a live Window: prefer LiveWindows
	if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
	{	// if no LiveWindow: take Play or Dib Window
		if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndImageRecherche)))
		{	// if none: take any
			FindNewCmdActiveWndSmall();
		}
	}

	SetAppropriateQuadViewMode();
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::SetWndLiveRequested(CWndLive* pDWRequested)
{
	if (!pDWRequested)
	{
		WK_TRACE_ERROR(_T("CWndSmall* pSWRequested is not a window"));
		return FALSE;
	}
//	TRACE(_T("SetWndLiveRequested %s %s\n"),pDWRequested->GetServer()->GetName(),pDWRequested->GetName());

	// activate request SmallWindow
	// because of manual/Sequencer Request
	CWndSmall* pSW = NULL;
	BOOL bRet = FALSE;
	BOOL bFound = FALSE;
	// De-Request all other windows of this server

	// m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; i<m_SmallWindows.GetSize() && !bFound ; i++)
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW
			&& (pSW->GetServerID() == pDWRequested->GetServerID())
			)
		{
			if (pSW->IsWndLive())
			{
				CWndLive* pDW = (CWndLive*)pSW;
				if (pDW->IsRequesting())
				{
					bFound = TRUE;
					if (pDW == pDWRequested)
					{
						if (pDW->IsWaitingForRequest())
						{
							pDW->DoRequestPictures();
						}
						bRet = TRUE;
					}
					else if (pDW->WasSometimesActive())
					{
						pDW->SetActiveRequested(FALSE);
						pDWRequested->SetActiveRequested();
						bRet = TRUE;
					}
					else
					{
						// wird dann behandelt, wenn das n‰chste Bild kommt
					}
				}
			}
		}
	}

	if (!bFound && !bRet)
	{
		pDWRequested->SetActiveRequested();
		bRet = TRUE;
	}
//	m_SmallWindows.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWndLiveRequested(WORD wServerID, CSecID id)
{
	CWndSmall* pSW = NULL;

//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	int c = m_SmallWindows.GetSize();
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; i<c ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (   (pSW->GetID() == id)
				&& pSW->IsWndLive()
				)
			{
				CWndLive* pDW = (CWndLive*)pSW;
				pDW->ActivateCamera();
				break;
			}
		}
	}
//	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWndSmallCmdActive(CWndSmall* pSW)
{
	if (   pSW
		&& m_pWndSmallCmdActive!=pSW)
	{
		if (   m_pWndSmallCmdActive
			&& m_pWndSmallCmdActive->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, m_pWndSmallCmdActive);
			if (((CWndHTML*)m_pWndSmallCmdActive)->IsEditing())
			{
				return;
			}
		}
		//TKR Bugfix 559:
		//hier muss zun‰chst das m_pWndSmallCmdActive gewechselt werden (m_pWndSmallCmdActive = pSW)
		//und erst dann das vorher aktive Fenster deaktiviert werden (OnSetCmdActive(FALSE),
		//da ansonsten das neue aktive Fenster zwar auf 'nicht aktiv' (OnSetCmdActive(FALSE) 
		//gesetzt wurde, aber im SetActiv() selbst wurde mit UpdateWindow() 
		//das alte aktive Fenster (mit Titel) neu gezeichnet. 

		CWndSmall* pDummySW = NULL;
		pDummySW = m_pWndSmallCmdActive;
		m_pWndSmallCmdActive = pSW;

		if (pDummySW)
		{
			pDummySW->OnSetCmdActive(FALSE,pSW);
		}

		m_pWndSmallCmdActive->OnSetCmdActive(FALSE,pDummySW);
		if (m_pWndSmallCmdActive->IsWndLive())
		{
			ASSERT_KINDOF(CWndLive, pSW);
			CWndLive* pDW = (CWndLive*)pSW;
			SetWndLiveRequested(pDW);
			if (pDW->GetType() == WST_MICO && pDW->IsYUV())
			{
				CalculateRequestedFPS(pDW->GetServer());
			}
#ifndef _DTS
			pDW->DoRequestVideoOut();
#endif
			//wird ein anderes Fenster als das LiveFenster auf aktiv gesetzt,
			//dann auch die PTZ-Buttons behandeln
			theApp.GetMainFrame()->GetDlgBarIdipClient()->SetPTZWindow(pDW);
		}
		else
		{
			theApp.GetMainFrame()->GetDlgBarIdipClient()->SetPTZWindow(NULL);
		}
		

	}
	ShowHideRectangle();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWndSmallBig(CWndSmall* pSW)
{
	if(pSW)
	{
		TRACE(_T("SetWndSmallBig %08lx\n"),pSW->GetID());
	}

	if (m_pWndSmallBig)
	{
		m_pWndSmallBig->OnSetSmallBig(FALSE,pSW);
	}

	CWndSmall* pDummy = m_pWndSmallBig;

	if (m_bView1Plus)
	{
		if (   pSW
			&& m_pWndSmallBig!=pSW)
		{
			m_pWndSmallBig = pSW;
			SetWindowSize();
		}
	}
	else
	{
		// Always set the BigOne, will be used at next manual 1plus switch
		m_pWndSmallBig = pSW;
		CServer* pServer = pSW->GetServer();
		if (   pServer
			&& (pServer->IsAlarm() || pServer->IsControl())
			&& pServer->GetAlarmID() == pSW->GetID()
			&& theApp.m_b1PlusAlarm
			)
		{
			OnView1Plus();
		}
	}
	if (pSW)
	{
		pSW->OnSetSmallBig(TRUE,pDummy);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWndSmallView1(CWndSmall* pSW)
{
// Wird nur durch Doppelklick vom SmallWindow aufgerufen
// Aufruf im Men¸ und Toolbar bei Multi-Monitor disabled, also hier auch!
// Auch hier (bzw. bei SetSmallWindowPositionsXxX) wieder das Problem
// mit dem View-Status, jedenfalls bei Multi-Monitor, SIGH!
	if (IsView1x1Allowed())
	{
		if (pSW->IsWndLive())
		{
			CWndLive* pDW = (CWndLive*)pSW;
			TRACE(_T("SetWndSmallView1 %s\n"),pDW->GetName());
			if (!pDW->IsRequesting())
			{
				SetWndLiveRequested(pDW);
			}
		}
		Cancel1PlusMode();
		SetWndSmallCmdActive(pSW);
// kann auch vorher 1Plus und trotzdem m_iXWin ==1 gewesen sein
// Grrr, deshalb wollte ich einen ViewStatus und nicht getrennte Variablen! gf
		if (   (Is1x1() == FALSE)
			|| IsView1plus()
			)
		{		
			SetXWin(1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (m_bLockSmallWindowResize)
	{
		return;
	}
	if ((cx != 0) && (cy != 0))
	{
		SetWindowSize(TRUE);
	}

	if (WK_IS_WINDOW(theApp.m_pDlgLogin))
	{
		theApp.m_pDlgLogin->SetWindowPosition();
		theApp.GetMainFrame()->SetActiveView(this);
		theApp.m_pDlgLogin->SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetXWin(int iXWin)
{
	TRACE(_T("SetXWin\n"), iXWin);
	if (iXWin == 0)
	{
		WK_TRACE_ERROR(_T("SetXWin iXWin == 0\n"));
		iXWin = 1;
	}
	m_iXWin = iXWin;
	Cancel1PlusMode();

	CMainFrame *pFrame = theApp.GetMainFrame();
	if (iXWin == 1 && pFrame && pFrame->GetNoOfExternalFrames())
	{
		iXWin = 1;
		int nRest = m_SmallWindows.GetSize() - 1;
		if (nRest > 1 && pFrame->GetNoOfExternalFrames() > 1)
		{
			nRest /= pFrame->GetNoOfExternalFrames();
		}
		if (nRest > 1)
		{
			iXWin = (int)ceil(sqrt((double) nRest));
		}

		if (m_pWndSmallCmdActive == NULL)
		{
			return;
		}
		CDlgExternalFrame *pExtFrame;
		CWndSmall*pSW;
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<pFrame->GetNoOfExternalFrames() ; i++)
		{
			pExtFrame = pFrame->GetExternalFrame(i);
			if (pExtFrame)
			{
				pExtFrame->m_iXWin = iXWin;
				pExtFrame->RemoveWndSmall(m_pWndSmallCmdActive);
				
				int i = 0;
				for (i=0 ; i<m_SmallWindows.GetSize() && i<nRest; i++) 
				{
					pSW = m_SmallWindows.GetAtFast(i);
					if (m_pWndSmallCmdActive != pSW)
					{
						pExtFrame->AddWndSmall(pSW);
					}
				}
				CWndSmallArray aSWs;
				CDlgExternalFrame::GetSmallWindows(pExtFrame->m_hWnd, aSWs);
				SetSmallWindowPositionsXxX(aSWs, pExtFrame);
			}
		}
		CWndSmallArray aSWs;
		aSWs.Add(m_pWndSmallCmdActive);
		SetSmallWindowPositionsXxX(aSWs, this);

		return;
	}
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetSmallWindowPositions(CWndSmallArray& sws)
{
	if (   m_bView1Plus
		&& sws.GetSize() > 1)
	{
		SetSmallWindowPositions1pX(sws);
	}
	else
	{
		SetSmallWindowPositionsXxX(sws);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetSmallWindowPositions1pX(CWndSmallArray& sws, CWnd *pExternal)
{
	CWndSmallArray aMain, *psws = &sws;
	int iSmallWindowCount = sws.GetSize();
	int iXWin = 1;

	CMainFrame *pFrame = theApp.GetMainFrame();

	if (   pFrame
		&& pFrame->GetNoOfExternalFrames()
		&& pExternal == NULL)
	{
		CWndSmall* pSmallWindow = NULL;
		CWndSmallArray aExt;
		aExt.Copy(sws);
		// sort the main SW array to make sure,
		// that the exclusive SW is the first after sorting
		SortSmallWindowsByMonitor(aExt);
		DWORD dwMonitor = aExt.GetAtFast(0)->GetMonitorFlag();
		if (dwMonitor & USE_MONITOR_EXCLUSIVE)
		{
			int i;
			CDlgExternalFrame *pEF = pFrame->GetExternalFrame(0);
			CWndSmallArray aSW;
			CWndSmall *pExclusive = aExt.GetAtFast(0);
			aSW.Add(pExclusive);
			if (pExclusive == m_pWndSmallBig)
			{
				m_pWndSmallBig = NULL;
			}
			if (dwMonitor & USE_MONITOR_1)
			{
				pFrame->RemoveSWfromExtFrames(aExt.GetAtFast(0));
				m_iXWin = 1;
				SetSmallWindowPositionsXxX(aSW, this);

				for (i=1; i<iSmallWindowCount; i++)
				{
					if (m_pWndSmallBig == NULL)
					{
						m_pWndSmallBig = aExt.GetAtFast(i);
					}
					pEF->AddWndSmall(aExt.GetAtFast(i));
				}
				CWndSmallArray aSWs;
				CDlgExternalFrame::GetSmallWindows(pEF->m_hWnd, aSWs);
				if (aSWs.GetSize() <= 1)
				{
					SetSmallWindowPositionsXxX(aSWs, pEF);
				}
				else
				{
					SetSmallWindowPositions1pX(aSWs, pEF);
				}
			}
			else if (dwMonitor & USE_MONITOR_2)
			{
				pEF->AddWndSmall(aExt.GetAtFast(0));
				pEF->m_iXWin = 1;
				SetSmallWindowPositionsXxX(aSW, pEF);
				for (i=1; i<iSmallWindowCount; i++)
				{
					if (m_pWndSmallBig == NULL)
					{
						m_pWndSmallBig = aExt.GetAtFast(i);
					}
					pFrame->RemoveSWfromExtFrames(aExt.GetAtFast(i));
				}
				CWndSmallArray aSWs;
				CDlgExternalFrame::GetSmallWindows(m_hWnd, aSWs);
				if (aSWs.GetSize() <= 1)
				{
					SetSmallWindowPositionsXxX(aSWs, this);
				}
				else
				{
					SetSmallWindowPositions1pX(aSWs, this);
				}
			}
			return;
		}

		if (iSmallWindowCount > 1)
		{
			CRect rc;
			CDlgExternalFrame *pEF = pFrame->GetExternalFrame(0);
			ASSERT(pEF!=NULL);
			int i, nMain, nExt = 0;

			nMain = CDlgExternalFrame::GetSmallWindows(m_hWnd, aMain);
			if (!m_bSizing)
			{
				if (m_pWndSmallBig == NULL)
				{
					m_pWndSmallBig = sws.GetAtFast(0);
				}
				aExt.Remove(m_pWndSmallBig);

				if (pFrame->RemoveSWfromExtFrames(m_pWndSmallBig))
				{
					nMain++;
				}
/*
				// bei diesen Schwellwerten wurden im Multimonitorbetrieb SmallWnds vom 2. auf den 1. 
				// Monitor verschoben.
				if (iSmallWindowCount > 25)
				{
					if      (nMain > 10) SortSmallWindowsByMonitor(aExt, USE_MONITOR_2);
					else if (nMain < 10) SortSmallWindowsByMonitor(aExt, USE_MONITOR_1);
					iSmallWindowCount = aExt.GetSize();
					for (i=0; i<iSmallWindowCount; i++)
					{
						pSmallWindow = aExt.GetAtFast(i);
						if (nMain < 10)
						{
							if (pFrame->RemoveSWfromExtFrames(pSmallWindow)) nMain++;
						}
						else if (nMain > 10)
						{
							if (pEF->AddWndSmall(pSmallWindow)) nMain--;
						}
						else break;
					}
					nExt = 0;
				}
				else if (iSmallWindowCount > 17)
				{
					if      (nMain > 8) SortSmallWindowsByMonitor(aExt, USE_MONITOR_2);
					else if (nMain < 8) SortSmallWindowsByMonitor(aExt, USE_MONITOR_1);
					iSmallWindowCount = aExt.GetSize();
					for (i=0; i<iSmallWindowCount; i++)
					{
						pSmallWindow = aExt.GetAtFast(i);
						if (nMain < 8)
						{
							if (pFrame->RemoveSWfromExtFrames(pSmallWindow)) nMain++;
						}
						else if (nMain > 8)
						{
							if (pEF->AddWndSmall(pSmallWindow)) nMain--;
						}
						else break;
					}
					nExt = 0;
				}
				else
*/
				// Ohne Schwellwerte ist 1-Plus so definiert, dass 1 Blid auf dem 1. Monitor ist und
				// alle anderen auf dem 2. Monitor
				{
					iSmallWindowCount = aExt.GetSize();
					for (i=0; i<iSmallWindowCount; i++)
					{
						pSmallWindow = aExt.GetAtFast(i);
						if (pEF->AddWndSmall(pSmallWindow))
						{
							nMain--;
						}
					}
					nExt = aExt.GetSize();
				}
			}
			
			if (nExt == 0)
			{
				nExt  = CDlgExternalFrame::GetSmallWindows(pEF->m_hWnd, aExt);
			}
			int iXWin = max((int)ceil(sqrt((double)(nExt))), 1);
			pEF->m_iXWin = iXWin;
			SetSmallWindowPositionsXxX(aExt, pEF);
			if (nMain>1)
			{
				nMain = CDlgExternalFrame::GetSmallWindows(m_hWnd, aMain);
				psws = &aMain;
				iSmallWindowCount = psws->GetSize();
			}
			else
			{
				GetClientRect(&rc);
				m_pWndSmallBig->MoveWindow(&rc);
				if (!m_pWndSmallBig->IsWindowVisible())
				{
					m_pWndSmallBig->ShowWindow(SW_SHOW);
				}
				return;
			}
		}
	}

	if (iSmallWindowCount > 2*iXWin)
	{
		// zu viele Fenster Modus hoch schalten
        iXWin = iSmallWindowCount/2 + iSmallWindowCount%2;
	}
	else if (iSmallWindowCount <= 2*(iXWin - 1))
	{
		// zu wenig Fenster
        iXWin = iSmallWindowCount/2 + iSmallWindowCount%2;
	}
	else
	{
		// iSmallWindowCount == 2*iXWin oder
		// iSmallWindowCount == 2*iXWin -1
		// alles OK
	}
	if (iXWin < 3)
	{
		iXWin = 3;
	}
	if (pExternal == NULL)
	{
		pExternal = this;
	}

	CRect r;
	pExternal->GetClientRect(&r);
	int x,y,i,j,cx,cy;
	// dimension of the view window itself
	int viewWidth = r.Width();
	int viewHeight = r.Height();
	// dimension for the child windows, one per camera
	int childWidth,childHeight;
	int childResX, childResY;

	childWidth = viewWidth / iXWin;
	childHeight = viewHeight / iXWin;
	childResX = viewWidth % iXWin;
	childResY = viewHeight % iXWin;

	x = y = cx = cy = 0;

	CheckAndSetActiveSmallWindow();

	CWndSmall* pSmallWindow = NULL;
	for (i=0,j=0;i<iSmallWindowCount && i<psws->GetSize();i++) 
	{
		pSmallWindow = psws->GetAtFast(i);
		if (pSmallWindow == m_pWndSmallBig)
		{
			x = r.left;
			y = r.top;
			cx = childWidth * (iXWin-1);
			cy = childHeight * (iXWin-1);
			pSmallWindow->MoveWindow(x, y, cx, cy, TRUE);
			if (!pSmallWindow->IsWindowVisible())
			{
				pSmallWindow->ShowWindow(SW_SHOW);
			}
		}
		else 
		{
			if (j<iXWin*2-1)
			{
				cx = childWidth;
				cy = childHeight;
				if (j<iXWin-1)
				{
					x = childWidth * j;
					y = childHeight * (iXWin-1);
					cy += childResY;
				}
				else
				{
					x = childWidth * (iXWin-1);
					cx += childResX;
					y = cy * (j+1-iXWin);
				}
				pSmallWindow->MoveWindow(x, y, cx, cy, TRUE);

				if (!pSmallWindow->IsWindowVisible())
				{
					pSmallWindow->ShowWindow(SW_SHOW);
				}
				j++;
			}
			else
			{
				// TODO: passt nicht mehr
				/*
				if (pSmallWindow->IsWndPlay())
				{
					CPlayWindow* pPW = (CPlayWindow*)pSmallWindow;
					pPW->SetPlayStatus(CPlayWindow::PS_EJECT);
				}
				*/
				pSmallWindow->ShowWindow(SW_HIDE);
			}
		}
//		pSmallWindow->Invalidate();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetSmallWindowPositionsXxX(CWndSmallArray& sws, CWnd *pExternal)
{
	CMainFrame *pFrame = theApp.GetMainFrame();
	if (    pFrame
		&& (pFrame->GetNoOfExternalFrames() > 0)
		&& (pExternal == NULL) )
	{
		int iSmallWindowCount = sws.GetSize();
		CWndSmall* pSmallWindow = NULL;
		if (iSmallWindowCount > 0)
		{
			// Sort small windows by monitor ...
			CRect rc;
			CDlgExternalFrame *pEF = pFrame->GetExternalFrame(0);
			ASSERT(pEF!=NULL);
			int i=0, nMain = 0, nExt = 0, nExclusive = 0;
			DWORD dwMonitor;

			CWndSmallArray aMain, aExt, &aAll = sws;
			// sort the main SW array to make sure,
			// that the exclusive SW is the first after sorting
			SortSmallWindowsByMonitor(aAll);

			dwMonitor = aAll.GetAtFast(0)->GetMonitorFlag();
			int nPossibleMain = m_iXWin * m_iXWin;
			if (   dwMonitor & USE_MONITOR_EXCLUSIVE	// exclusiv
				|| m_iXWin == 1)						// 1:1 View
			{
				if (dwMonitor & USE_MONITOR_1)
				{
					nExclusive = USE_MONITOR_1;
					nPossibleMain = m_iXWin = 1;
				}
				if (dwMonitor & USE_MONITOR_2)
				{
					nExclusive = USE_MONITOR_2;
					nPossibleMain = iSmallWindowCount-1;
					m_iXWin = max((int)ceil(sqrt((double)nPossibleMain)), 1);
				}
			}
			
			// ... and move those assign to its monitor, count their numbers
			for (i=0; i<iSmallWindowCount; i++)
			{
				pSmallWindow = aAll.GetAtFast(i);
				dwMonitor = pSmallWindow->GetMonitorFlag();
//				TRACE(_T("Window %d, %x\n"), i, dwMonitor);
				if (nExclusive == USE_MONITOR_1 && nMain > 0)
				{
					dwMonitor &= ~USE_MONITOR_1;
				}
				if (nExclusive == USE_MONITOR_2 && nExt > 0)
				{
					dwMonitor &= ~USE_MONITOR_2;
				}

				if (dwMonitor & USE_MONITOR_1)
				{
					pFrame->RemoveSWfromExtFrames(pSmallWindow);
					nMain++;
				}
				if (dwMonitor & USE_MONITOR_2)
				{
					pEF->AddWndSmall(pSmallWindow);
					nExt++;
				}
				if (dwMonitor == 0)
				{
					// all following windows are regardless, see SortSmallWindowsByMonitor()
					break; 
				}
			}

			// Assign all regardless windows to empty places
			if (nExclusive == USE_MONITOR_1)
			{
				nPossibleMain = 1;
			}

			for (i=nMain+nExt ; i<aAll.GetSize() ; i++)
			{
				pSmallWindow = aAll.GetAtFast(i);
				// Check, if main monitor has all possible
				if (nMain < nPossibleMain)
				{
					pFrame->RemoveSWfromExtFrames(pSmallWindow);
					nMain++;
				}
				else if ((nExclusive & USE_MONITOR_2) == 0)
				{
					pEF->AddWndSmall(pSmallWindow);
					nExt++;
				}
			}

			// Now sort the small windows on each monitor
			nMain = CDlgExternalFrame::GetSmallWindows(m_hWnd, aMain);
			SortSmallWindowsByID(aMain);
			SetSmallWindowPositionsXxX(aMain, this);
			
			nExt  = CDlgExternalFrame::GetSmallWindows(pEF->m_hWnd, aExt);
			SortSmallWindowsByID(aExt);
			int iXWin = max((int)ceil(sqrt((double)nExt)), 1);
			pEF->m_iXWin = iXWin;
			SetSmallWindowPositionsXxX(aExt, pEF);
			return;
		}
	}

	int iXWin = m_iXWin;
	CRect r;
	if (pExternal)
	{
		pExternal->GetClientRect(&r);
		if (pExternal != this)
		{
			iXWin = ((CDlgExternalFrame*)pExternal)->m_iXWin;
		}
	}
	else 
	{
		GetClientRect(&r);
	}
	// Always keep iXWin NOT 0, it is used as divisor!
	if (iXWin == 0)
	{
		WK_TRACE_ERROR(_T("SetSmallWindowPositionsXxX iXWin == 0\n"));
		iXWin = 1;
	}

	int x,y,i;
	// dimension of the view window itself
	int viewWidth = r.Width();
	int viewHeight = r.Height();
	// dimension for the child windows, one per camera
	int childWidth,childHeight;

	childWidth = viewWidth / iXWin;
	childHeight = viewHeight / iXWin;

	CWndSmall* pSW = NULL;
	CRect rect;

	for (i=0;i<sws.GetSize();i++)
	{
		pSW = sws.GetAtFast(i);
		x = (i - (i / iXWin) * iXWin) * childWidth;
		y = ((i / iXWin) - (i / (iXWin * iXWin)) * iXWin) * childHeight;

		if (pExternal)	pExternal->ScreenToClient(rect);
		else           ScreenToClient(rect);
		if (   rect.left != x
			|| rect.top != y
			|| rect.Width() != childWidth
			|| rect.Height() != childHeight
			)
		{
			pSW->MoveWindow(x, y, childWidth, childHeight);
		}

		if (!pSW->IsWindowVisible())
		{
			pSW->ShowWindow(SW_SHOW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWindowSize(BOOL bFromOnSize/*=FALSE*/)
{
	int iMaxVisible = 0;

	if (m_bView1Plus)
	{
		iMaxVisible = m_SmallWindows.GetSize();
	}
	else
	{
		CMainFrame *pFrame = theApp.GetMainFrame();
		if (pFrame == NULL) return;

		iMaxVisible = m_iXWin*m_iXWin;
		CDlgExternalFrame *pExtFrame;
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<pFrame->GetNoOfExternalFrames() ; i++)
		{
			pExtFrame = pFrame->GetExternalFrame(i);
			if (pExtFrame)
			{
				int iXWin = pExtFrame->m_iXWin;
				iMaxVisible += (iXWin*iXWin);
			}
		}
	}

//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	m_bSizing = bFromOnSize;

	if (bFromOnSize)
	{
		SetWindowSizeAtResizing();
	}
	else if (m_SmallWindows.GetSize()<=iMaxVisible)
	{
		// alle Fenster werden angezeigt
//		TRACE(_T("SetWindowSize all visible resizing\n"));
		SetSmallWindowPositions(m_SmallWindows);
	}
	else
	{
		SetWindowSizeReplacing(iMaxVisible);
	}

	m_bSizing = FALSE;
//	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWindowSizeAtResizing()
{
//	TRACE(_T("SetWindowSize visible resizing\n"));
	CWndSmallArray justVisible;
	// play all visible windows in oldVisible
	CWndSmall* pSW = NULL;
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; i<m_SmallWindows.GetSize() ; i++) 
	{
		pSW = m_SmallWindows[i];
		if (   pSW
			&& pSW->IsWindowVisible())
		{
			justVisible.Add(pSW);
		}
	}
	SetSmallWindowPositions(justVisible);
	justVisible.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetWindowSizeReplacing(int iMaxVisible)
{
	CWndSmallArray oldVisibleDisplays, oldVisibleNonDisplays, oldVisibles;
	CWndSmallArray newVisibleDisplays, newVisibleNonDisplays, newVisibles, newHiddenDisplays;
	CWndSmall* pSW = NULL;
	int i = 0;

	// play all visible windows in oldVisibleDisplays and oldVisibleNonDisplays
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (i=0 ; i<m_SmallWindows.GetSize() ; i++) 
	{
		pSW = m_SmallWindows[i];
		if (pSW)
		{
			if (pSW->IsWindowVisible())
			{
				if (pSW->IsWndLive())
				{
					oldVisibleDisplays.Add(pSW);
				}
				else
				{
					oldVisibleNonDisplays.Add(pSW);
				}
			}
		}
	}
	oldVisibles.Append(oldVisibleDisplays);
	oldVisibles.Append(oldVisibleNonDisplays);
	SortSmallWindowsByIDandTime(oldVisibles);

	// what window should be visible by requesting or as non-display window
	if (   m_iXWin == 1
		&& m_pWndSmallCmdActive!=NULL)
	{
		newVisibles.Add(m_pWndSmallCmdActive);
	}
	else
	{
		for (i=0;i<m_SmallWindows.GetSize();i++) 
		{
			pSW = m_SmallWindows[i];
			if (pSW->IsWndLive())
			{
				if (pSW->IsRequesting())
				{
					newVisibleDisplays.Add(pSW);
				}
				else
				{
					newHiddenDisplays.Add(pSW);
				}
			}
			else
			{
				newVisibleNonDisplays.Add(pSW);
			}
		}
		newVisibles.Append(newVisibleDisplays);
		newVisibles.Append(newVisibleNonDisplays);
		SortSmallWindowsByIDandTime(newVisibles);
	}

//	TRACE(_T("old=%d new=%d both=%d windows %d Max\n"),oldVisibleDisplay.GetSize(),newVisible.GetSize(),iBoth,iMaxVisible);

	int iNewVisible = newVisibles.GetSize();

	// Sonderfall 1er View, immer das CmdActiveWindow anzeigen, befindet sich in newVisibles
	if (m_iXWin == 1)
	{
		// Hide all oldVisible except if it is the newVisible
		CMainFrame *pFrame = theApp.GetMainFrame();

		if (pFrame && pFrame->GetNoOfExternalFrames() == 0)
		{
			for (i=0 ; i<oldVisibles.GetSize() ; i++)
			{
				pSW = oldVisibles.GetAtFast(i);
				if (   (iNewVisible == 0)
					|| (pSW != newVisibles.GetAtFast(0))
					)
				{
					pSW->ShowWindow(SW_HIDE);
				}
			}
			SetSmallWindowPositions(newVisibles);
		}
		else
		{
			if (newVisibles.GetSize())
			{
				m_pWndSmallBig = newVisibles.GetAtFast(0);
			}
			SetSmallWindowPositions(newVisibles);
		}
	}
	else
	{
		if (   (oldVisibles.GetSize() == iMaxVisible)
			&& (newVisibles.GetSize() == iMaxVisible)
			)
		{
			// es werden nur Fenster auf dem Bildschirm ersetzt
			for (i=0 ; i<newVisibles.GetSize() ; i++)
			{
				// looking for a place on screen
				if (oldVisibles.GetSize())
				{
					CRect rect;
					CWndSmall* pOldSW = oldVisibles.GetAtFast(0);
					CWndSmall* pNewSW = newVisibles.GetAtFast(i);
					// only redraw if necessary
					if (pOldSW != pNewSW)
					{
						pOldSW->GetWindowRect(rect);
						ScreenToClient(rect);
						pNewSW->MoveWindow(rect,FALSE);
						pNewSW->ShowWindow(SW_SHOW);
						pOldSW->ShowWindow(SW_HIDE);
					}
					oldVisibles.RemoveAt(0);
				}
			}
		}
		else if (iNewVisible <= iMaxVisible)
		{
			if (iNewVisible < iMaxVisible)
			{
				SortSmallWindowsByIDandTime(newHiddenDisplays);
				int iToBeHidden = min(newHiddenDisplays.GetSize(), max(0, newHiddenDisplays.GetSize()+iNewVisible-iMaxVisible));
				for (i=newHiddenDisplays.GetSize()-1 ; i>=iToBeHidden ; i--)
				{
					pSW = newHiddenDisplays.GetAtFast(i);
					newHiddenDisplays.RemoveAt(i);
					newVisibles.Add(pSW);
				}
			}
			TRACE(_T("newVisible=%d iMaxVisible=%d\n"), newVisibles.GetSize(), iMaxVisible);
			SortSmallWindowsByID(newVisibles);
			SetSmallWindowPositions(newVisibles);
		}
		else if (iNewVisible > iMaxVisible)
		{
			int iToHide = iNewVisible-iMaxVisible;
			if (iToHide > newVisibles.GetSize())
			{
				iToHide = newVisibles.GetSize();
			}
			for (i=0 ; i<iToHide ; i++)
			{
				pSW = newVisibles.GetAtFast(0);
				newVisibles.RemoveAt(0);
				newHiddenDisplays.Add(pSW);
			}
			SetSmallWindowPositions(newVisibles);
		}
		for (i=0 ; i<newHiddenDisplays.GetSize() ; i++)
		{
			pSW = newHiddenDisplays.GetAtFast(i);
			pSW->ShowWindow(SW_HIDE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CheckAndSetActiveSmallWindow()
{
	if (m_pWndSmallBig == NULL && m_SmallWindows.GetSize() > 0)
	{
		CAutoCritSec acs(&m_SmallWindows.m_cs);
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<m_SmallWindows.GetSize() ; i++) 
		{
			if (m_SmallWindows.GetAtFast(i)->IsWindowVisible())
			{
				m_pWndSmallBig = m_SmallWindows.GetAtFast(i);
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnToggle()
{
	CStateCmdUI state;

	EnumViewMode eViewMode;
	if (m_bView1Plus)
	{
		eViewMode = VIEW_1PLUS;
	}
	else
	{
		if (m_iXWin == 1)
		{
			eViewMode = VIEW_1x1;
		}
		else
		{
			eViewMode = VIEW_XxX;
		}
	}

	if (eViewMode == VIEW_1x1)
	{	// try to toggle to 1plus
		OnUpdateView1Plus(&state);
		if (state.m_bEnabled)
		{
			OnView1Plus();
		}
		else
		{
			eViewMode = VIEW_1PLUS;
		}
	}
	if (eViewMode == VIEW_1PLUS)
	{
		OnUpdateView22(&state);
		if (state.m_bEnabled)
		{
			OnView22();
		}
		else
		{
			OnUpdateView33(&state);
			if (state.m_bEnabled)
			{
				OnView33();
			}
			else
			{
				OnUpdateView44(&state);
				if (state.m_bEnabled)
				{
					OnView44();
				}
				else
				{
					// seems to be only one window
					eViewMode = VIEW_XxX;
				}
			}
		}
	}
	if (eViewMode == VIEW_XxX)
	{	// toggle to 1x1
		OnView1();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateView1(CCmdUI* pCmdUI) 
{
	// View1 independend from valid user to enable switch at alarm connections
	BOOL bEnable = FALSE;
	if (m_SmallWindows.GetSize() > 0)
	{
		bEnable = IsView1x1Allowed();
	}
	pCmdUI->Enable(bEnable);
	BOOL bChecked = m_iXWin==1 && !m_bView1Plus;
	pCmdUI->SetCheck(bChecked);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateView33(CCmdUI* pCmdUI) 
{
	// View independend from valid user to enable switch at alarm connections
	//only button 3x3 is and stays visible, no 2x2, 4x4 ... buttons are needed
	//we only have to check which view the current should be (3x3, 4x4, 5x5..)
	// GF it is the QuadView-Buttun, regardless how many small windows are visible
	BOOL bEnable = (m_SmallWindows.GetSize() > 0);
	pCmdUI->Enable(bEnable);
	BOOL bChecked = m_iXWin!=1 && !m_bView1Plus;
	pCmdUI->SetCheck(bChecked);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateView1Plus(CCmdUI* pCmdUI) 
{
	// 1plus independend from valid user to enable switch at alarm connections
	BOOL bEnable = (m_SmallWindows.GetSize() > 0);
	if (!bEnable)
	{
		bEnable = DoFullScreenOrView1PlusAtAlarm();
	}
	pCmdUI->Enable(bEnable);
	BOOL bChecked = m_bView1Plus;
	pCmdUI->SetCheck(bChecked);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::DoFullScreenOrView1PlusAtAlarm()
{
	if (theApp.m_b1PlusAlarm || theApp.m_bFullscreenAlarm)
	{
		const CServers &servers = GetDocument()->GetServers();
		CAutoCritSec acs((CCriticalSection*)&servers.m_cs);
		int i, nSize = servers.GetSize();
		for (i=0; i<nSize; i++)
		{
			if (servers.GetAtFast(i)->IsAlarm())
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnView1() 
{
	m_bView1Plus = FALSE;
	SetXWin(1);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnView33() 
{
	m_bView1Plus = FALSE;
	if (m_pWndSmallBig != m_pWndSmallCmdActive)
	{
		m_pWndSmallBig = m_pWndSmallCmdActive;
	}
	SetAppropriateQuadViewMode();
	SetXWin(m_iXWin);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnView1Plus() 
{
	// GF Only change TO 1plus, change FROM 1Plus is done by other buttons
	if (!m_bView1Plus)
	{
		m_bView1Plus = TRUE;
		SetAppropriateQuadViewMode(); // Is needed after 1x1 for correct scaling!
		SetWindowSize();
		if (m_pWndSmallBig == NULL)
		{
		//aktives Fenster aus vorheriger Ansicht in 1Plus als groﬂes Fenster anzeigen
			SetWndSmallBig(m_pWndSmallCmdActive);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnView22() 
{
	SetXWin(2);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnView44() 
{
	SetAppropriateQuadViewMode();
	SetXWin(m_iXWin);
}
/////////////////////////////////////////////////////////////////////////////
int CViewIdipClient::GetNrOfRequestingWindows()
{
	 int iRequesting = 0;
	 CAutoCritSec acs(&m_SmallWindows.m_cs);
	 //changes for VisualStudio 2005
	 int i = 0;
	 for (i=0 ; i<m_SmallWindows.GetSize() ; i++)
	 {
		 CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		 if (   pSW
			 && pSW->IsWndLive())
		 {
			 CWndLive* pDW = (CWndLive*)pSW;
			 if (pDW->IsRequesting())
			 {
				 iRequesting++;
			 }
		 }
	 }

	 return iRequesting;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateView22(CCmdUI* pCmdUI) 
{
	BOOL bEnable = OnUpdateViewXX(2);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_iXWin==2 && !m_bView1Plus);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateView44(CCmdUI* pCmdUI) 
{
	BOOL bEnable = OnUpdateViewXX(4);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_iXWin==4 && !m_bView1Plus);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::OnUpdateViewXX(int nX)
{
	int nQm1 = nX-1, nQ = nX, nQp1 = nX+1;
	BOOL bEnable = FALSE;
	nQm1 *= nQm1;

	CMainFrame *pFrame = theApp.GetMainFrame();
	if (pFrame && m_SmallWindows.GetSize() > nQm1)
	{
		int nExtFrames = pFrame->GetNoOfExternalFrames();
		nQ *= nQ;
		DWORD dwMonitor = 0;
		CWndSmall *pSWEx = GetWndSmallExclusive(); 
		if (pSWEx)
		{
			dwMonitor = pSWEx->GetMonitorFlag();
		}
		if (dwMonitor & USE_MONITOR_EXCLUSIVE)
		{
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfLiveWindows() - 1;
			bEnable =  ((iRequesting + iNonDisplay) <= nQ);
		}
		else if (nExtFrames)
		{
			nQp1 *= nQp1;
			int nMin = nQm1 + nExtFrames * nQ;
			int nMax = nQ   + nExtFrames * nQp1;
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfLiveWindows();
			int nWnd = iRequesting + iNonDisplay;
			bEnable  = (nWnd > nMin && nWnd <= nMax);
		}
		else
		{
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfLiveWindows();
			bEnable =  ((iRequesting + iNonDisplay) <= nQ);
		}
	}
	return bEnable;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnVideoFein() 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnVideoFein();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateVideoFein(CCmdUI* pCmdUI) 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnUpdateVideoFein(pCmdUI);
	}
	else
	{
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnVideoGrob() 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnVideoGrob();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateVideoGrob(CCmdUI* pCmdUI) 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnUpdateVideoGrob(pCmdUI);
	}
	else
	{
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnVideoCsb() 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnVideoCsb();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateVideoCsb(CCmdUI* pCmdUI) 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnUpdateVideoCsb(pCmdUI);
	}
	else
	{
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnVideoSave() 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnVideoSave();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateVideoSave(CCmdUI* pCmdUI) 
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndLive* pDW = GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->OnUpdateVideoSave(pCmdUI);
	}
	else
	{
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnViewOriginal() 
{
	m_b1to1 = !m_b1to1;

	int i,c;
	CWndSmall* pSW = NULL;
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	c = m_SmallWindows.GetSize();
	for (i=0 ; i<c ; i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && pSW->IsWndLive())
		{
			CWndLive* pDW = (CWndLive*)pSW;
			pDW->Set1to1(m_b1to1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL bHandled = FALSE;

	if (m_pWndSmallCmdActive)
	{
		if (m_pWndSmallCmdActive->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			bHandled = TRUE;
		}
	}

	if (!bHandled)
	{
		bHandled = CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}
	return bHandled;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateSequenze(CCmdUI* pCmdUI) 
{
	int i,c;

	// TODO! RKE: new Sequencer modus to be implemented
	c = 0;
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (i=0 ; i<m_SmallWindows.GetSize() ; i++)
	{
		CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && pSW->IsWndLive())
		{
			CWndLive* pDW = (CWndLive*)pSW;
			CServer* pServer = pDW->GetServer();
			if (pServer)
			{
				if (!pServer->IsMultiCamera())
				{
					c++;
				}
			}
		}
	}
	acs.Unlock();

	BOOL bEnable = (c > 1);
	pCmdUI->Enable(bEnable);
	if (bEnable == FALSE)
	{
		GetDocument()->SetSequence(FALSE);
	}
	pCmdUI->SetCheck(GetDocument()->GetSequence());
}
/////////////////////////////////////////////////////////////////////////////
CWndLive* CViewIdipClient::GetNextSequenceWndLive(WORD wServerID, CWndLive* pCurrent)
{
	CWndLive* pDWReturn = NULL;
	CWndLive* pDW = NULL;
	CWndSmall* pSW = NULL;
	CWndSmallArray serverwindows;

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	// erst mal alle fenster des Servers raussuchen
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; i<m_SmallWindows.GetSize() ; i++)
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->IsWndLive())
		{
			pDW = (CWndLive*)pSW;
			if (pDW->GetServerID() == wServerID)
			{
				serverwindows.Add(pDW);
			}
		}
	}

	if (serverwindows.GetSize())
	{
		for (i=0;i<serverwindows.GetSize();i++)
		{
			pDW = (CWndLive*)serverwindows.GetAtFast(i);
			if (pDW->GetCamID() == pCurrent->GetCamID())
			{
				// found
				if (i+1<serverwindows.GetSize())
				{
					pDWReturn = (CWndLive*)serverwindows.GetAtFast(i+1);
				}
				else
				{
					pDWReturn = (CWndLive*)serverwindows.GetAtFast(0);
				}
				break;
			}
		}
	}
	else
	{
		pDWReturn = pCurrent;
	}
	return pDWReturn;
}
/////////////////////////////////////////////////////////////////////////////
WORD CViewIdipClient::GetNextSequenceServer(WORD wServerID, int start)
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=start ; i<m_SmallWindows.GetSize() ; i++)
	{
		CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->IsWndLive())
		{
			if (wServerID != m_SmallWindows.GetAtFast(i)->GetServerID())
			{
				return m_SmallWindows.GetAtFast(i)->GetServerID();
			}
		}
	}
	return (WORD)(-1);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::Sequence()
{
	// TODO! RKE: new Sequencer modus to be implemented

/*
	if (theApp.m_bTraceSequence)
	{
		WK_TRACE(_T("START SEQUENCE %d\n"),m_dwTimerCounter);
	}

	BOOL bSwitched = FALSE;

	CAutoCritSec acs(&m_SmallWindows.m_cs);

	if (m_SmallWindows.GetSize()>0)
	{
		WORD wCurrentServer=(WORD)(-1);
		BOOL bCallSetWindowSize = FALSE;
		int i;

		// find out the first server id
		// with real display window
		wCurrentServer = GetNextSequenceServer(wCurrentServer,0);
		
		for (i=0;i<m_SmallWindows.GetSize();i++)
		{
			CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
			
			if (   pSW->IsWndLive()
				&& pSW->GetServerID() == wCurrentServer
				)
			{
				CWndLive* pDW = (CWndLive*)pSW;
				wCurrentServer = pSW->GetServerID();
				if (pDW->IsRequesting())
				{
					WORD wServerID = pDW->GetServerID();
					int iWaits = pDW->IsWaitingForPicture();
					if (iWaits==1)
					{
						if (theApp.m_bTraceSequence)
						{
							WK_TRACE(_T("SEQUENCE waiting once for picture %s\n"),pDW->GetName());
						}
						wCurrentServer = GetNextSequenceServer(wServerID,i);
						bSwitched = TRUE;
					}
					else if (iWaits==2)
					{
						if (theApp.m_bTraceSequence)
						{
							WK_TRACE(_T("SEQUENCE waiting twice for picture %s\n"),pDW->GetName());
						}
						pDW->DoRequestPictures();
						wCurrentServer = GetNextSequenceServer(wServerID,i);
						bSwitched = TRUE;
					}
					else
					{
						CWndLive* pNextDW = GetNextSequenceWndLive(wServerID,pDW);
						if (pNextDW != pDW)
						{
							bSwitched = TRUE;
							
							if (theApp.m_bTraceSequence)
							{
								WK_TRACE(_T("SEQUENCE %s off\n"),pDW->GetName());
							}
							
							if (   pDW->SetActiveRequested(FALSE)
								&& pNextDW
								)
							{
								if (theApp.m_bTraceSequence)
								{
									WK_TRACE(_T("SEQUENCE %s on\n"),pNextDW->GetName());
								}
								pNextDW->SetActiveRequested();
								if (m_iXWin == 1)
								{
									m_pWndSmallCmdActive = pNextDW;
									if (m_pWndSmallCmdActive->IsWndLive())
									{
										theApp.GetMainFrame()->GetDlgBarIdipClient()->SetPTZWindow((CWndLive*)m_pWndSmallCmdActive);
									}
								}

								if (!pNextDW->IsWindowVisible())
								{
									if (theApp.m_bTraceSequence)
									{
										WK_TRACE(_T("next window is not visible call SetWindowSize\n"));
									}
									bCallSetWindowSize = TRUE;
								}
							}
						}
						wCurrentServer = GetNextSequenceServer(wServerID,i);
					}
				}
			}
		}
		if (bCallSetWindowSize)
		{
			if (theApp.m_bTraceSequence)
				WK_TRACE(_T("SEQUENCE SetWindowSize\n"));
			SetWindowSize();
		}
	}
	else
	{
		if (theApp.m_bTraceSequence)
			WK_TRACE(_T("sequence without any window\n"));
	}

	if (theApp.m_bTraceSequence)
	{
		WK_TRACE(_T("END SEQUENCE %d\n"),m_dwTimerCounter);
	}

	acs.Unlock();

	if (!bSwitched)
	{
		GetDocument()->SetSequence(FALSE);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == theApp.GetMainFrame()->GetOneSecTimerID())
	{
		m_dwTimerCounter++;
		if (GetDocument()->GetSequence())
		{
			// TODO! RKE: new Sequencer modus to be implemented
			if (((m_dwTimerCounter-1)%GetDocument()->GetSequenceIntervall()==0))
			{
				Sequence();
			}
		}
		CheckSmallWindows();

		if (m_dwTimerCounter == 60)
		{
			GetDocument()->OnIdle();
			m_dwTimerCounter = 0;
		}
	}
	else
	{
		CView::OnTimer(nIDEvent);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CheckSmallWindows() 
{
	CWndSmall* pSW = NULL;
	CServers*pServers = (CServers*)&GetDocument()->GetServers();
	int nPlayWindows  = 0,
		nPerfControlled  = 0;

	if	(theApp.m_bRegardNetworkFrameRate)
	{
		pServers->ResetPictureTimeOffsets();
	}

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	DWORD dwCurrentTime = GetTickCount();	// determine times after entering cs
	CSystemTime stLocal;
	stLocal.GetLocalTime();
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->IsWndLive())
		{
			CWndLive *pLW = (CWndLive*)pSW;
			pLW->CheckVideo(dwCurrentTime);
			if (    pLW->GetType() == WST_MPEG4
				|| (pLW->GetType() == WST_MICO && pLW->IsYUV()))
			{
				nPerfControlled++;
				if (   theApp.m_bRegardNetworkFrameRate
					&& !pSW->GetServer()->IsLocal())
				{
					pLW->CheckRemotePictureTime(stLocal);
				}
			}
		}
		else if (pSW->IsWndPlay())
		{
			nPlayWindows++;
		}
	}
	acs.Unlock();

	if	(theApp.m_bRegardNetworkFrameRate)			// limit no of requested fps, if needed
	{
		CServer *pServer;
		CAutoCritSec acsS(&pServers->m_cs);
		int i, nSize = pServers->GetSize();
		for (i=0;i<nSize;i++)
		{
			pServer = pServers->GetAtFast(i);
			if (pServer->IsLocal())					// local should not be limited
			{
				continue;
			}
			switch(pServer->m_nRequestState)
			{
				case REQUEST_STATE_NORMAL:			// a server is normally not limited
				if (pServer->m_nMaxPictureTimeOffset >= RQP_TIME_OFFSET_NORMAL)
				{									// if pictures come too late
					int nCameras;					// determine how many fps were arriving
					int nMaxRequested = CWndMpeg4::DetermineNetworkStatistics(pServer->GetID(), nCameras);
					if (nMaxRequested)
					{
						pServer->m_nRequestState = REQUEST_STATE_RECOVER;
						pServer->m_nMaxRequestedFps = (short)nMaxRequested;
						pServer->m_nStateCounter = 0;// reset counter for additional recovery time
						CWndMpeg4::CalculateFps(pServer, nCameras);// distribute fps to cameras
						if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
						{
							WK_TRACE(_T("%s switch to recovery state\n"), pServer->GetName());
						}
					}
				}	break;
				case REQUEST_STATE_RECOVER:			// in recovery state
				if (   pServer->m_nMaxPictureTimeOffset <= 0 // if the pictures arrive in time
					&& pServer->m_nStateCounter++ == RECOVERY_TIME_COUNT)// wait additional time for complete recovery
				{
					if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
					{
						WK_TRACE(_T("%s recovered, switch to limited\n"), pServer->GetName());
					}
					pServer->m_nRequestState = REQUEST_STATE_LIMITED;
					pServer->m_nStateCounter = 0;	// switcht to limited state and reset counter
					CWndMpeg4::SetLimitedFps(pServer);
				}
				else if (pServer->m_nMaxPictureTimeOffset >= RQP_TIME_OFFSET_RECOVER)
				{
					int nCameras = GetNrOfWindows(pServer->GetID(), WST_MPEG4);
					if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
					{
						WK_TRACE(_T("%s reducing fps in recovery state %d -> %d\n"), pServer->GetName(), pServer->m_nMaxRequestedFps, nCameras-1);
					}
					pServer->m_nMaxRequestedFps = (short)(nCameras-1);
					CWndMpeg4::CalculateFps(pServer, nCameras);// distribute fps to cameras
				}	break;
				case REQUEST_STATE_LIMITED:			// in limited state
				if (pServer->m_nMaxPictureTimeOffset >= RQP_TIME_OFFSET_LIMITED)
				{									// it the pictures also come too late
					int nCameras = GetNrOfWindows(pServer->GetID(), WST_MPEG4);
					pServer->m_nStateCounter = 0;
					if (pServer->m_nMaxRequestedFps >= RQP_TIME_OFFSET_LIMITED)
					{
						pServer->m_nMaxRequestedFps--;// reduce fps and recalculate distribution
						if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
						{
							WK_TRACE(_T("%s limited %d, decreasing fps to:\n"), pServer->GetName(), pServer->m_nMaxPictureTimeOffset);
						}
						CWndMpeg4::CalculateFps(pServer, nCameras);
					}
					if (pServer->m_nMaxPictureTimeOffset >= RQP_TIME_OFFSET_LIM_X)
					{								// if the pictures come very late
						pServer->m_nRequestState = REQUEST_STATE_RECOVER;// switch to recovery state
						if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
						{
							WK_TRACE(_T("%s limited [%d, (%d < %d)], switch to recover\n"), pServer->GetName(), pServer->m_nMaxPictureTimeOffset, pServer->m_nMaxRequestedFps, nCameras);
						}
						break;						// no further handling nessesary
					}
					CWndMpeg4::SetLimitedFps(pServer);
				}
				else if (pServer->m_nStateCounter++ > INCREASE_TIME_COUNT)// after a while
				{
					pServer->m_nStateCounter = 0;
					if (pServer->m_nMaxRequestedFps < MAX_REQ_FPS_LIMITED)
					{
						pServer->m_nMaxRequestedFps += 2;// increase fps and recalculate distribution
						int nCameras = GetNrOfWindows(pServer->GetID(), WST_MPEG4);
						if (theApp.m_bRegardNetworkFrameRate&TRACE_NETWORK_RQ_STATE)
						{
							WK_TRACE(_T("%s limited, increasing fps to:\n"), pServer->GetName());
						}
						CWndMpeg4::CalculateFps(pServer, nCameras);
						CWndMpeg4::SetLimitedFps(pServer);
					}
				}	break;
			}
		}
		acsS.Unlock();
	}

	if (nPlayWindows)
	{
		if (theApp.GetPerformanceLevel() < INITIAL_PERFORMANCE_LEVEL)
		{	// set to level 25:5:5
			theApp.SetPerformanceLevel(INITIAL_PERFORMANCE_LEVEL);
		}
	}
	else if (nPerfControlled == 0)
	{	// set to level 25:5:5
		theApp.SetPerformanceLevel(INITIAL_PERFORMANCE_LEVEL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnDestroy() 
{
	if (m_pPictureBufferThread)
	{
		m_pPictureBufferThread->PostThreadMessage(WM_QUIT, 0, 0);
	}
	CView::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
int CViewIdipClient::GetRequestedFPS(WORD wServer, EnumWndSmallType wst/*=WST_ALL*/)
{
	int r = 0;
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		if (pSW && pSW->IsWndLive())
		{
			CWndLive* pDW = (CWndLive*)pSW;
			if (   (wServer == 0   || pDW->GetServerID() == wServer)
				&& (WST_ALL == wst || pDW->GetType()     == wst))
			{
				r += pDW->GetRequestedFPS();
			}
		}
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CalculateRequestedFPS(CServer*pServer)
{
	if (pServer == NULL)
	{
		CServers* pServers = (CServers*)&GetDocument()->GetServers();
		CServer*pServer = pServers->GetLocalServer();
		if (pServer)
		{
			CalculateRequestedFPS(pServer);
		}
		return ;
	}

	WORD wServer = pServer->GetID();
	int  nMaxFps = pServer->GetMaxFPS();
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndSmallArray swaTotal;
	int i, nFps = 0, nActive = 0, nCount;
	for (i=0;i<m_SmallWindows.GetSize();i++)
	{
		CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->GetServerID() == wServer
			&& pSW->GetType() == WST_MICO)
		{
			swaTotal.Add(pSW);
		}
	}
	nCount = swaTotal.GetSize();
	if (nCount > 1)
	{
		nFps = (nMaxFps - 25) / (nCount-1);
	}
	if (   nFps < 5
		&& nCount>0)
	{
		nFps = nMaxFps / nCount;
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
		nActive = 25;
	}

	nFps = min(nFps, 25);
	nActive = min(nActive, 25);	// not more than 25 fps

	//TRACE(_T("Requested(%d, %d)\n"), nFps, nActive);
	for (i=0;i<nCount;i++)
	{
		CWndMico* pDW = (CWndMico*)swaTotal.GetAtFast(i);
		pDW->SetRequestedFPS(pDW->IsCmdActive() ? nActive : nFps);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::Cancel1PlusMode()
{
	m_bView1Plus = FALSE;
	// Do not clear m_pWndSmallBig, it will be used at next manual switch again
//	m_pWndSmallBig = NULL;
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnViewMonitor() 
{
	CRect       rc;
	if (!m_bView1Plus)
	{
		SetAppropriateQuadViewMode();
	}
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	SetSmallWindowPositions(m_SmallWindows);
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnViewTitleOfSmallwindows() 
{
	if (theApp.m_bShowTitleOfSmallWindows & SHOW_TITLE)
	{
		theApp.m_bShowTitleOfSmallWindows &= ~SHOW_TITLE;
	}
	else
	{
		theApp.m_bShowTitleOfSmallWindows |= SHOW_TITLE;
	}
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		pSW->EnableTitle(theApp.m_bShowTitleOfSmallWindows);
	}		
//	SetWindowSize(FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateViewTitleOfSmallwindows(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(theApp.m_bShowTitleOfSmallWindows & SHOW_TITLE ? TRUE : FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnViewTitleInPicture() 
{
	if (theApp.m_bShowTitleOfSmallWindows & SHOW_TITLE_IN_PICTURE)
	{
		theApp.m_bShowTitleOfSmallWindows &= ~SHOW_TITLE_IN_PICTURE;
	}
	else
	{
		theApp.m_bShowTitleOfSmallWindows |= SHOW_TITLE_IN_PICTURE;
	}
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
		pSW->EnableTitle(theApp.m_bShowTitleOfSmallWindows);
	}		
//	SetWindowSize(FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateViewTitleInPicture(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(theApp.m_bShowTitleOfSmallWindows & SHOW_TITLE_IN_PICTURE ? TRUE : FALSE);
}
//////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::IsView1x1Allowed()
{
	BOOL bEnable = FALSE;
	bEnable =  (GetWndSmallCmdActive() != NULL);
	return bEnable;
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnEditHtmlPage() 
{
	if (CanEditHTMLWindow())
	{
		BOOL bEdit = ((CWndHTML*)m_pWndSmallCmdActive)->OnEditHtmlPage();
		if (bEdit)
		{
			SetWndSmallBig(m_pWndSmallCmdActive);
			OnView1();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateEditHtmlPage(CCmdUI* pCmdUI) 
{
	if (CanEditHTMLWindow())
	{
		((CWndHTML*)m_pWndSmallCmdActive)->OnUpdateEditHtmlPage(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnEditClear() 
{
	if (CanEditHTMLWindow())
	{
		((CWndHTML*)m_pWndSmallCmdActive)->OnEditClear();
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	if (CanEditHTMLWindow())
	{
		((CWndHTML*)m_pWndSmallCmdActive)->OnUpdateEditClear(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnEditCreateHtmlCameraPage() 
{
	CDlgHTMLmap dlg;
	dlg.m_sPath.Format(_T("%s\\%08lx\\"),CNotificationMessage::GetWWWRoot(), CSecID(SECID_LOCAL_HOST));
	dlg.m_sTitle.LoadString(IDS_CAMERAS);
	if (dlg.DoModal() == IDOK)
	{
		CString sCameraHTM = MAIN_CAMERAMAP_FILE_NAME;
		dlg.CheckCopyPictureFile();
		dlg.m_sPath += sCameraHTM;
		CWndHTML wnd(theApp.GetNextHtmlWindowID(), dlg.m_sPath, dlg.m_sTitle, dlg.m_sPictureFileName, dlg.m_nZoomValue, HTML_TYPE_CAMERA_MAP);
		wnd.OnFileSaveHtmlText(dlg.m_szPicture);
		CWndHTML*pHW = AddHtmlWindow(GetDocument()->GetLocalServer(), theApp.GetNextHtmlWindowID(), sCameraHTM);
		if (pHW)
		{
			SetWndSmallCmdActive(pHW);
			OnEditHtmlPage();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateEditCreateHtmlCameraPage(CCmdUI* pCmdUI) 
{
	if (   theApp.m_pPermission 
		 && theApp.m_pPermission->IsSuperVisor())
	{
		if (   m_pWndSmallCmdActive 
			&& m_pWndSmallCmdActive->GetType() == WST_HTML
			 && ((CWndHTML*)m_pWndSmallCmdActive)->IsEditing())
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			if (CViewControllItems::GetLocalServerID() != SECID_NO_SUBID)	// Local host should be connected
			{
				CString sFile;
				sFile.Format(_T("%s\\%08lx\\%s"), CNotificationMessage::GetWWWRoot(), SECID_LOCAL_HOST, MAIN_CAMERAMAP_FILE_NAME);
				pCmdUI->Enable(!DoesFileExist(sFile));
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnEditCreateHtmlHostPage() 
{
	CDlgHTMLmap dlg;
	dlg.m_sPath.Format(_T("%s\\"),CNotificationMessage::GetWWWRoot());
	dlg.m_sTitle.LoadString(IDS_HOSTS);
	if (dlg.DoModal() == IDOK)
	{
		dlg.CheckCopyPictureFile();
		dlg.m_sPath += _T("map.htm");
		CWndHTML wnd(theApp.GetNextHtmlWindowID(), dlg.m_sPath, dlg.m_sTitle, dlg.m_sPictureFileName, dlg.m_nZoomValue, HTML_TYPE_HOST_MAP);
		wnd.OnFileSaveHtmlText(dlg.m_szPicture);
		theApp.CheckHostMap();
		ShowMapWindow();
		if (m_pHostMapWindow)
		{
			SetWndSmallCmdActive(m_pHostMapWindow);
		}
		OnEditHtmlPage();
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateEditCreateHtmlHostPage(CCmdUI* pCmdUI) 
{
	if (   theApp.m_pPermission 
		 && theApp.m_pPermission->IsSuperVisor()
		 && !theApp.IsReadOnlyModus())
	{
		if (   m_pWndSmallCmdActive 
			&& m_pWndSmallCmdActive->GetType() == WST_HTML
			 && ((CWndHTML*)m_pWndSmallCmdActive)->IsEditing())
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CStateCmdUI cmdui;
			GetDocument()->OnUpdateConnectMap(&cmdui);
			pCmdUI->Enable(!cmdui.m_bEnabled);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnEditHtmlMapProperties() 
{
	if (CanEditHTMLWindow())
	{
		((CWndHTML*)m_pWndSmallCmdActive)->OnEditHtmlMapProperties();
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateEditHtmlMapProperties(CCmdUI* pCmdUI) 
{
	if (CanEditHTMLWindow())
	{
		((CWndHTML*)m_pWndSmallCmdActive)->OnUpdateEditHtmlMapProperties(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
/*
void CViewIdipClient::OnUpdateEditCreateHtmlText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_pPermission && theApp.m_pPermission->IsSuperVisor());
}
*/
//////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::CanEditHTMLWindow()
{
	if (   theApp.m_pPermission 
		 && theApp.m_pPermission->IsSuperVisor()
		 && m_pWndSmallCmdActive
		 && m_pWndSmallCmdActive->GetType() == WST_HTML)
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OpenFile()
{
	COemFileDialog dlg(this);
	CString sOpen,sDIB,sJPX,sJPG;

	sOpen.LoadString(IDS_OPEN);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(theApp.m_sOpenDirectory);

	sJPG.LoadString(IDS_JPEG);
	dlg.AddFilter(sJPG,_T("jpg"));

	sDIB.LoadString(IDS_DIB);
	dlg.AddFilter(sDIB,_T("bmp"));

	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		CRect rect;
		int p = sFileName.ReverseFind(_T('\\'));
		if (theApp.m_nRemoveableDriveLetter != theApp.m_sOpenDirectory.GetAt(0))
		{
			theApp.m_sOpenDirectory = sFileName.Left(p);
			theApp.SaveOpenDirPath();
		}
		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		CWndDib* pDW = new CWndDib();
		if (pDW)
		{
			if (!pDW->Create(rect, this))
			{
				delete pDW;
				return;
			}
			if (!pDW->LoadFile(sFileName))
			{
				pDW->DestroyWindow();
				delete pDW;
				return;
			}
			AddWndSmall(pDW);
			SetWndSmallCmdActive(pDW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateSmallClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnFileOpenDisk()
{
	COemFileDialog dlg(this);
	CString sOpen,sJPX,sJPEG,sBMP;

	sOpen.LoadString(IDS_OPEN);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(_T("a:"));

	sJPEG.LoadString(IDS_JPEG);
	dlg.AddFilter(sJPEG, _T("jpg"));

	sBMP.LoadString(IDS_DIB);
	dlg.AddFilter(sBMP, _T("bmp"));

	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		CRect rect;
		int p = sFileName.ReverseFind(_T('\\'));
		if (theApp.m_nRemoveableDriveLetter != theApp.m_sOpenDirectory.GetAt(0))
		{
			theApp.m_sOpenDirectory = sFileName.Left(p);
			theApp.SaveOpenDirPath();
		}
		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		CWndDib* pDW = new CWndDib();
		if (pDW)
		{
			if (!pDW->Create(rect, this))
			{
				delete pDW;
				return;
			}
			if (!pDW->LoadFile(sFileName))
			{
				pDW->DestroyWindow();
				delete pDW;
				return;
			}
			AddWndSmall(pDW);
			SetWndSmallCmdActive(pDW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateFileOpenDisk(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pDlgBackup == NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnCloseWndSmallAll()
{
	CloseWndSmallAllOfType(WST_ALL);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateCloseWndSmallAll(CCmdUI *pCmdUI)
{
	BOOL bEnable = (HasWndSmallOfType(WST_ALL) > 0);
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnCloseWndSmallAllLive()
{
	CloseWndSmallAllOfType(WST_LIVE);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateCloseWndSmallAllLive(CCmdUI *pCmdUI)
{
	BOOL bEnable = (HasWndSmallOfType(WST_LIVE) > 0);
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnCloseWndSmallAllPlay()
{
	CloseWndSmallAllOfType(WST_PLAY);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateCloseWndSmallAllPlay(CCmdUI *pCmdUI)
{
	BOOL bEnable = (HasWndSmallOfType(WST_PLAY) > 0);
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CloseWndSmallAllOfType(EnumWndSmallType type)
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	CWndSmall*		pWndSmall = NULL;
	CWndSmallArray arClose;
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0; i<m_SmallWindows.GetSize(); i++)
	{
		pWndSmall = m_SmallWindows.GetAtFast(i);
		if (type != WST_ALL)
		{
			if (  (type == WST_LIVE && pWndSmall->IsWndLive())
				||(type == WST_PLAY && pWndSmall->IsWndPlay())
				||(type == pWndSmall->GetType()))
			{
				arClose.Add(pWndSmall);
			}
		}
		else
		{
			arClose.Add(pWndSmall);
		}
	}

	CIdipClientDoc* pDoc = GetDocument();
	LockSmallWindowResizing(TRUE);
	for (i=0; i<arClose.GetSize(); i++)
	{
		pWndSmall = arClose.GetAtFast(i);
		if (pWndSmall->IsWndLive())
		{
			pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW, pWndSmall->GetServerID()),
				(CObject*)pWndSmall->GetID().GetID());
		}
		DeleteWndSmall(pWndSmall->GetServerID(), pWndSmall->GetID());
	}
	LockSmallWindowResizing(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
int CViewIdipClient::HasWndSmallOfType(EnumWndSmallType type)
{
	int iCount = 0;

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	if (type == WST_ALL)
	{
		iCount = m_SmallWindows.GetSize();
	}
	else
	{
		CWndSmall* pWndSmall = NULL;
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<m_SmallWindows.GetSize() ; i++)
		{
			pWndSmall = m_SmallWindows.GetAtFast(i);
			if (   (type == WST_LIVE && pWndSmall->IsWndLive())
				|| (type == WST_PLAY && pWndSmall->IsWndPlay())
				|| (type == pWndSmall->GetType())
				)
			{
				iCount++;
			}
		}
	}

	return iCount;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnSequenceOver()
{
	m_bSequenceOver = !m_bSequenceOver;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateSequenceOver(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSequenceOver);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnBackupClosed()
{
	GetDocument()->UpdateAllViews(this, VDH_BACKUP, NULL);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::UseSingleWindow()
{
	return m_bSequenceOver;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::CanPlayRealtime()
{
	return m_bSequenceRealtime;
}
/////////////////////////////////////////////////////////////////////////////
// TODO GF CAVEAT really use direct pointer?
/////////////////////////////////////////////////////////////////////////////
CWndSmallArray*	CViewIdipClient::GetSmallWindows()
{
	CWndSmallArray* pSW = &m_SmallWindows;
	return pSW;
}
/////////////////////////////////////////////////////////////////////////////
CWndPlay* CViewIdipClient::GetWndPlay(WORD wServerID, WORD wArchiv, WORD wSequence)
{
	return GetWndPlay(wServerID,CSecID(wArchiv,wSequence));
}
/////////////////////////////////////////////////////////////////////////////
CWndPlay* CViewIdipClient::GetWndPlay(WORD wServerID, CSecID id)
{
	CWndSmall* pSW = NULL;

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->IsWndPlay()
			&& pSW->GetServerID()==wServerID)
		{
			CWndPlay* pDW = (CWndPlay*)pSW;
			if (id.GetSubID()==0)
			{
				if (pDW->GetID().GetGroupID()==id.GetGroupID())
				{
					return pDW;
				}
			}
			else
			{
				if (pDW->GetID()==id)
				{
					return pDW;
				}
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWndAlarmList* CViewIdipClient::GetWndAlarmList(WORD wServerID, WORD wArchiv, WORD wSequence)
{
	CWndSmall* pSW = NULL;
	CSecID id(wArchiv,wSequence);

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (   pSW->GetServerID()== wServerID
			&& pSW->GetType()    == WST_ALARM_LIST)
		{
			ASSERT_KINDOF(CWndAlarmList, pSW);
			CWndAlarmList* pALW = (CWndAlarmList*)pSW;
			if (pALW->GetID()==id)
			{
				return pALW;
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CheckSmallWindowsFullScreen()
{
	if (m_SmallWindows.GetSize() == 0)
	{
		theApp.GetMainFrame()->ChangeFullScreenModus(FALSE|FULLSCREEN_FORCE);
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndPlay* CViewIdipClient::GetWndPlayYoungest(CSystemTime& t)
{
	CWndPlay* pRet = NULL;
	CSystemTime n,d;

//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows.GetAtFast(i)->IsWndPlay())
		{
			CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
			if (pSW->IsWndPlay())
			{
				CWndPlay* pDW = (CWndPlay*)pSW;
				if (pDW->GetTimeStamp(d))
				{
					if (d>n)
					{
						n = d;
						pRet = pDW;
					}
				}
			}
		}
	}
//	m_SmallWindows.Unlock();

	t = n;

	return pRet;
}
/////////////////////////////////////////////////////////////////////////////
CWndPlay* CViewIdipClient::GetWndPlayOldest(CSystemTime& t)
{
	CWndPlay* pRet = NULL;
	CSystemTime n,d;

	n.GetLocalTime();

//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows.GetAtFast(i)->IsWndPlay())
		{
			CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
			if (pSW->IsWndPlay())
			{
				CWndPlay* pDW = (CWndPlay*)pSW;
				if (pDW->GetTimeStamp(d))
				{
					if (d<n)
					{
						n = d;
						pRet = pDW;
					}
				}
			}
		}
	}
//	m_SmallWindows.Unlock();

	t = n;

	return pRet;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateFilePrint(CCmdUI *pCmdUI)
{	
	if (m_pWndSmallCmdActive)
	{
//		pCmdUI->Enable((m_SmallWindows.GetSize()>0));
		pCmdUI->Enable(m_pWndSmallCmdActive->CanPrint());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateFilePrintPreview(CCmdUI *pCmdUI)
{
	OnUpdateFilePrint(pCmdUI);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnArchivBackup(CServer* pServer)
{
	if (!theApp.m_bAllowBackup)
	{
		return;
	}
	if (pServer)
	{
		BeginWaitCursor();
		OnCloseWndSmallAllPlay();

		if (pServer->CanSplittingBackup())
		{
			CDlgBackupNew *pDlgBackupNew = NULL;
			pDlgBackupNew = new CDlgBackupNew(pServer,this);

			if(pDlgBackupNew)
			{
				pDlgBackupNew->RefreshInfo();
				m_pDlgBackup = pDlgBackupNew;
			}
		}
		else
		{
			m_pDlgBackup = new CDlgBackupOld(pServer,this);
			m_pDlgBackup->ShowWindow(SW_SHOW);
		}

		EndWaitCursor();
	}
	else
	{
		if (m_pDlgBackup)
		{
			m_pDlgBackup->DestroyWindow();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CDlgBackup* CViewIdipClient::GetDlgBackup()
{
	return m_pDlgBackup;
}
/////////////////////////////////////////////////////////////////////////////
//if QueryDialog is on Desktop, show or hide Checkbox for Rectangle-Search
void CViewIdipClient::ShowHideRectangle()
{
	//gehe alle offenen SmallWindows durch, ist ein Rechteck vorhanden, dann im
	//QueryDialog entsprechend die Bereichsuche enablen und den Dateibaum entsprechend anzeigen
	CViewArchive* pVA = theApp.GetMainFrame()->GetViewArchive();
	CDlgQuery* pQDlg = pVA->GetDlgQuery();
	CWndSmall* pSW = NULL;
	CAutoCritSec acs(&m_SmallWindows.m_cs);

	int iSize = m_SmallWindows.GetSize();
	BOOL bShowHideRect = FALSE;
	if(iSize)
	{
		CWndPlay* pDW = NULL;
		//changes for VisualStudio 2005
		int i = 0;
		for(i=0; i < iSize; i++)
		{
			pSW = m_SmallWindows.GetAtFast(i);
			if(pSW && pSW->IsWndPlay())
			{
				pDW = (CWndPlay*)pSW;
				if(pDW && !pDW->GetRectToQuery().IsRectEmpty())
				{
					if(pDW->IsCmdActive())
					{
						bShowHideRect = TRUE;
						break;
					}
				}
			}
		}
	}

	if(pQDlg)
	{
		pQDlg->ShowHideRectangle(bShowHideRect);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CloseWndPlayAllOfArchive(WORD wServerID, WORD wArchivID)
{
	int i;
	CWndSmall* pSW = NULL;

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (i=m_SmallWindows.GetSize()-1 ; i>=0 ; i--) 
	{
		pSW = m_SmallWindows.GetAtFast(i);
		if (pSW->GetServerID() == wServerID)
		{
			WORD wWndArchiveID = 0xffff;
			if (pSW->IsWndPlay())
			{
				ASSERT_KINDOF(CWndPlay, pSW);
				wWndArchiveID = ((CWndPlay*)pSW)->GetArchivNr();
			}
			else if (pSW->IsWndLive())
			{	// RKE: leave this code because IsWndLive() is much faster than
				// pSW->IsKindOf(..). Most of WndSmall¥s will be WndLive¥s.
				// the archive Nr. is only used to open an archive according
				// to this WndLive
//				ASSERT_KINDOF(CWndLive, pSW);
//				wWndArchiveID = ((CWndLive*)pSW)->GetArchivNr();
			}
			else if (pSW->GetType() == WST_ALARM_LIST)
			{
				ASSERT_KINDOF(CWndAlarmList, pSW);
				wWndArchiveID = ((CWndAlarmList*)pSW)->GetArchivNr();
			}
			if (wWndArchiveID != 0xffff)
			{
				OnDeleteWindow(pSW);
				pSW->DestroyWindow();
				WK_DELETE(pSW);
				m_SmallWindows.RemoveAt(i);

				ShowHideRectangle();
			}
		}
	}
	// all PlayWindows closed: prefer LiveWindows
	if (!FindNewCmdActiveWndSmall(RUNTIME_CLASS(CWndLive)))
	{	// if not found: take any
		FindNewCmdActiveWndSmall();
	}
	acs.Unlock();

	if (m_SmallWindows.GetSize() <= ((m_iXWin-1)*(m_iXWin-1)))
	{
		if (m_iXWin > 1)
		{
			m_iXWin--;
		}
	}
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateNavigateRange(CCmdUI *pCmdUI)
{
	BOOL bEnable = TRUE;
	int iLive = HasWndSmallOfType(WST_LIVE);
	int iPlay = HasWndSmallOfType(WST_PLAY);
	if (   iLive == 0
		&& iPlay == 0)
	{
		bEnable = FALSE;
	}

	if(iPlay > 0)
	{
		m_bUncheckNavigateForward = TRUE;
		m_bUncheckNavigateFastForward = TRUE;
		m_bUncheckNavigateBack = TRUE;
		m_bUncheckNavigateFastBack = TRUE;
	}

	if(iPlay == 0)
	{
		if(    m_bUncheckNavigateForward == TRUE
			&& pCmdUI->m_nID == ID_NAVIGATE_FORWARD)
		{
			m_bUncheckNavigateForward = !m_bUncheckNavigateForward;
			pCmdUI->SetCheck(m_bUncheckNavigateForward);
		}

		if(    m_bUncheckNavigateFastForward == TRUE
			&& pCmdUI->m_nID == ID_NAVIGATE_FAST_FORWARD)
		{
			m_bUncheckNavigateFastForward = !m_bUncheckNavigateFastForward;
			pCmdUI->SetCheck(m_bUncheckNavigateFastForward);
		}

		if(    m_bUncheckNavigateBack == TRUE
			&& pCmdUI->m_nID == ID_NAVIGATE_BACK)
		{
			m_bUncheckNavigateBack = !m_bUncheckNavigateBack;
			pCmdUI->SetCheck(m_bUncheckNavigateBack);
		}
		if(    m_bUncheckNavigateFastBack == TRUE
			&& pCmdUI->m_nID == ID_NAVIGATE_FAST_BACK)
		{
			m_bUncheckNavigateFastBack = !m_bUncheckNavigateFastBack;
			pCmdUI->SetCheck(m_bUncheckNavigateFastBack);
		}

	}

	if(    bEnable 
		&& m_pWndSmallCmdActive)
	{
		if(    m_pWndSmallCmdActive->GetType() == WST_HTML //html
			||     (pCmdUI->m_nID == ID_NAVIGATE_EJECT
			    && m_pWndSmallCmdActive->IsWndLive()))
		{
			bEnable = FALSE;
		}

		//TKR disable all buttons in panel on syncplay, when livepicture is active
		if(     bEnable
			&&	m_pWndSmallCmdActive->IsWndLive()
			&& ((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
		{
			bEnable = FALSE;
		}

	}
	else
	{
		bEnable = FALSE;
	}
	
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnNavigateRange(UINT uID)
{
	if (m_pWndSmallCmdActive)
	{
		if(m_pWndSmallCmdActive->IsWndLive())
		{
			CWndLive* pWndLive = (CWndLive*)m_pWndSmallCmdActive;
			// switch to play window with current picture
			CServer* pServer = pWndLive->GetServer();
			CSecID idArchive = pWndLive->GetArchiveID();
			if (   pServer
				&& (idArchive != SECID_NO_ID)
				)
			{
				WORD wArchiveID = idArchive.GetSubID();
				CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
				if (pDatabase)
				{
					CIPCFields fields;
					CSystemTime stTimeStamp = pWndLive->GetTimeStamp();
					CString sStartDatum = stTimeStamp.GetDBDate();
					CString sStartTime = stTimeStamp.GetDBTime();

					fields.Add(new CIPCField(_T("DBP_DATE"),
												sStartDatum,
												8,_T('{')));
					fields.Add(new CIPCField(_T("DBP_TIME"),
												sStartTime,
												8,_T('{')));

					pDatabase->DoRequestNewQuery(MAKELONG(QUERY_LIVE_TO_PLAY, LOWORD(uID)), 1, &wArchiveID, fields, 1);
				}
			}
		}
		else if(m_pWndSmallCmdActive->IsWndPlay())
		{
			if(uID == ID_NAVIGATE_EJECT)
			{
				//close current opened PlayWnd
				m_pWndSmallCmdActive->PostMessage(WM_COMMAND,ID_CLOSE_WND_THIS);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::OnEraseBkgnd(CDC* pDC)
{
	return CView::OnEraseBkgnd(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::CloseWndAll() 
{
	// TODO GF check for bugs of merge
	CWndSmall* pSmall = NULL;

	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=m_SmallWindows.GetSize()-1;i>=0;i--)
	{
		pSmall = m_SmallWindows.GetAtFast(i);
		OnDeleteWindow(pSmall);
		pSmall->DestroyWindow();
		WK_DELETE(pSmall);
		m_SmallWindows.RemoveAt(i);
		ShowHideRectangle();
	}
	acs.Unlock();

	SetAppropriateQuadViewMode();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::FindNewCmdActiveWndSmall(CRuntimeClass*pRTC/*=NULL*/)
{
	if (m_pWndSmallCmdActive == NULL)
	{
		CAutoCritSec acs(&m_SmallWindows.m_cs);
		for (int i=0;i<m_SmallWindows.GetSize();i++) 
		{
			CWndSmall *pSW = m_SmallWindows.GetAtFast(i);
			if (pSW)
			{
				if (   pRTC == NULL
					|| pSW->IsKindOf(pRTC))
				{
					SetWndSmallCmdActive(pSW);
					if(IsView1plus())
					{
						SetWndSmallBig(pSW);
					}
					else if(Is1x1())
					{
						//if view 1-mode is active, switch active wnd into 1-mode
						SetView1();
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewIdipClient::OnUser(WPARAM wParam, LPARAM lParam)
{
//	CDocument* pDoc = GetDocument();
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
//	CServer* pServer = GetDocument()->GetServer(wServerID);
//	CSecID id = (DWORD)(lParam);
	switch (cause)
	{
		case WPARAM_CAMERA_MAP:
			OnUpdate(this, MAKELONG(VDH_CAMERA_MAP, wServerID), NULL);
			break;
		case WPARAM_ERROR_MESSAGE:
			theApp.OnErrorMessage();
			break;
		case ID_SEARCH_RESULT:
			OnSearchResult();
			break;
		case WPARAM_UPDATE_SYNCDLGS_IN_WNDPLAY:
			OnUpdateSyncDlgsOnAllWndPlay();
			break;
		case WPARAM_INSERT_DEBUGGER_WINDOW:
			AddTextWindow(NULL, SECID_NO_ID, _T(""));
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::ConfirmQuery(DWORD dwUserID,
                                   WORD wServerNr,
								   WORD wArchivNr,
								   WORD wSequenceNr, 
								   DWORD dwRecordNr,
								   int iNumFields,
								   const CIPCField fields[])
{
	// CAVEAT: In CIPC thread
	// It must be our own Query!
	if ((LOWORD(dwUserID) & 0x0fff) == QUERY_LIVE_TO_PLAY)
	{
		CQueryResult* pResult = new CQueryResult(HIWORD(dwUserID), wServerNr, wArchivNr, wSequenceNr, dwRecordNr, iNumFields, fields);
		if (pResult)
		{
			m_QueryResultsToShow.SafeAddTail(pResult);
			PostMessage(WM_USER, ID_SEARCH_RESULT);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnSearchResult()
{
	CQueryResult* pQueryResult = m_QueryResultsToShow.SafeGetAndRemoveHead();
	if (pQueryResult)
	{
		WORD wServer = pQueryResult->GetServer();
		CServer* pServer = GetDocument()->GetServer(wServer);
		if (pServer)
		{
			WORD wArchive = pQueryResult->GetArchiveNr();
			WORD wSequence = pQueryResult->GetSequenceNr();
			// try to find a play window with this archive and sequence
			CWndPlay* pWndPlay = GetWndPlay(wServer, wArchive, wSequence);
			if (pWndPlay == NULL)
			{
				// try to find a play window with at least this archive
				pWndPlay = GetWndPlay(wServer, wArchive, 0);
			}
			if (pWndPlay)
			{
//				pWndPlay->Navigate(pQueryResult->GetRecordNr());
				UINT uID = pQueryResult->GetUserID();
				SetWndSmallCmdActive(pWndPlay);
				SetWndSmallBig(pWndPlay);
				pWndPlay->SendMessage(WM_COMMAND, uID);
			}
			else
			{
				pServer->OpenSequence(wArchive, wSequence, pQueryResult->GetRecordNr());
				pWndPlay = GetWndPlay(wServer, wArchive, wSequence);
				if (pWndPlay)
				{
					SetWndSmallCmdActive(pWndPlay);
					SetWndSmallBig(pWndPlay);
				}
			}
		}
		else
		{
			TRACE(_T("CViewIdipClient::OnSearchResult() No server\n"));
		}
		WK_DELETE(pQueryResult);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::RedrawSmallWindows()
{
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		m_SmallWindows.GetAtFast(i)->RedrawWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::SetView1()
{
	OnView1();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewIdipClient::Is1x1() const
{
	CMainFrame *pFrame = theApp.GetMainFrame();

	if (pFrame && (pFrame->GetNoOfExternalFrames() > 0))
	{
		if (pFrame->GetExternalFrame(0)->m_iXWin > 1)
		{
			return FALSE;
		}
	}
	return m_iXWin == 1;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIdipClient::OnUpdateSyncDlgsOnAllWndPlay()
{
	CWndPlay* pRet = NULL;

//	m_SmallWindows.Lock(_T(__FUNCTION__));
	CAutoCritSec acs(&m_SmallWindows.m_cs);
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows.GetAtFast(i)->IsWndPlay())
		{
			CWndSmall* pSW = m_SmallWindows.GetAtFast(i);
			if (pSW->IsWndPlay())
			{
				//aktualisiere die Dlg>Fields im WndPlay
				pRet = (CWndPlay*)pSW;
				pRet->EnableDlgFields(TRUE);
			}
		}
	}
//	m_SmallWindows.Unlock();
}
