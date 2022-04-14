// VisionView.cpp : implementation of the CVisionView class
//

#include "stdafx.h"
#include "Vision.h"

#include "VisionDoc.h"
#include "VisionView.h"
#include "Mainfrm.h"
#include "ObjectView.h"

#include "MicoWindow.h"

	#include "CocoWindow.h"
#ifndef _DTS
	#include "PTWindow.h"
#endif


#include "CIPCOutputVisionDecoder.h"

#include "ExternalFrameDlg.h"
#include "HTMLmapProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CStateCmdUI::Enable(BOOL bOn)
{
	m_bEnabled = bOn;
}

void CStateCmdUI::SetCheck(int nC)
{
	m_nChecked = nC;
}

void CStateCmdUI::SetRadio( BOOL bOn)
{
	m_nChecked = bOn;
}

/////////////////////////////////////////////////////////////////////////////
// CVisionView

IMPLEMENT_DYNCREATE(CVisionView, CView)

BEGIN_MESSAGE_MAP(CVisionView, CView)
	//{{AFX_MSG_MAP(CVisionView)
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
	ON_WM_MOVE()
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
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_TOGGLE, OnToggle)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
#ifdef _HTML_ELEMENTS
	ON_COMMAND(ID_EDIT_HTML_PAGE, OnEditHtmlPage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HTML_PAGE, OnUpdateEditHtmlPage)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TITLE_OF_SMALLWINDOWS, OnUpdateViewTitleOfSmallwindows)
	ON_COMMAND(ID_EDIT_CREATE_HTML_CAMERA_PAGE, OnEditCreateHtmlCameraPage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATE_HTML_CAMERA_PAGE, OnUpdateEditCreateHtmlCameraPage)
	ON_COMMAND(ID_EDIT_CREATE_HTML_HOST_PAGE, OnEditCreateHtmlHostPage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATE_HTML_HOST_PAGE, OnUpdateEditCreateHtmlHostPage)
	ON_COMMAND(ID_EDIT_HTML_MAP_PROPERTIES, OnEditHtmlMapProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HTML_MAP_PROPERTIES, OnUpdateEditHtmlMapProperties)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisionView construction/destruction

CVisionView::CVisionView()
{
	m_crKeyColor = RGB(255,0,255);
	m_iFontHeight = 11;
	m_iXWin = 1; // must never be 0, is use as divisor!
	m_b1to1 = FALSE;
	m_bView1Plus = FALSE;
	m_pHostMapWindow = NULL;
	m_dwIDCounter = 1;
	m_dwTimerCounter = 0;
	m_uTimerID = 0;
	m_pCmdActiveSmallWindow = NULL;
	m_pBigSmallWindow = NULL;
	m_bSizing = FALSE;
}

CVisionView::~CVisionView()
{
	m_SmallWindows.Lock();
	CSmallWindow* pSW;
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows[i];
		WK_DELETE(pSW);
	}
	m_SmallWindows.RemoveAll();
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::ShowMapWindow()
{
	if (WK_IS_WINDOW(m_pHostMapWindow))
	{
		m_pHostMapWindow->BringWindowToTop();
	}
	else
	{
		CRect rect;
		GetClientRect(rect);

		CString sMapUrl = theApp.GetMapURL();
		m_pHostMapWindow = new CHtmlWindow(theApp.GetNextHtmlWindowID(), sMapUrl);
		m_pHostMapWindow->Create(rect,1,this);
		m_pHostMapWindow->NavigateTo(sMapUrl);
		AddSmallWindow(m_pHostMapWindow);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnInitialUpdate() 
{
	m_Images.Create(IDB_ALARM,_IMAGE_WIDTH,0, RGB(0,255,255));
	CView::OnInitialUpdate();
	
//	EnableToolTips();

	CString sMapURL;
	sMapURL = theApp.GetMapURL();
	if (!sMapURL.IsEmpty())
	{
		if (DoesFileExist(sMapURL))
		{
			ShowMapWindow();
		}
	}

	m_uTimerID = SetTimer(3, 1000, NULL);
}
/////////////////////////////////////////////////////////////////////////////
// CVisionView drawing

void CVisionView::OnDraw(CDC* pDC)
{
/*	CVisionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect,&m_CinemaBrush);
*/
}

/////////////////////////////////////////////////////////////////////////////
// CVisionView printing

BOOL CVisionView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVisionView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CVisionView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CVisionView diagnostics

#ifdef _DEBUG
void CVisionView::AssertValid() const
{
	CView::AssertValid();
}

void CVisionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVisionDoc* CVisionView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVisionDoc)));
	return (CVisionDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisionView message handlers


int CVisionView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// for Coco Color Key
	if (!m_KeyBrush.CreateSolidBrush(m_crKeyColor))
		return -1;
	
	if (!m_CinemaBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE)))//RGB(192,192,192)))
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
void CVisionView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CIPCOutputRecord* pRec = NULL;
	CVisionDoc* pDoc = GetDocument();
	CServer* pServer = pDoc->GetServer(wServerID);

	switch (cause)
	{
	case NULL:	// called by CView::OnInitialUpdate()
	case VDH_INPUT:
	case VDH_OUTPUT:
		break;
	case VDH_ADD_CAMWINDOW:
		pRec = (CIPCOutputRecord*)pHint;
		AddDisplayWindow(pServer,*pRec);
		break;
	case VDH_REMOVE_CAMWINDOW:
		pRec = (CIPCOutputRecord*)pHint;
		DeleteSmallWindow(wServerID,pRec->GetID());
		break;
	case VDH_ACTIVATE_REQUEST_CAM:
		pRec = (CIPCOutputRecord*)pHint;
		SetRequestedDisplayWindow(wServerID,pRec->GetID());
		break;
	case VDH_DELETE_CAMWINDOW:
		DeleteSmallWindow(wServerID,CSecID((DWORD)pHint));
		break;
	case VDH_REMOVE_SERVER:
		DeleteSmallWindows((WORD)pHint);
		break;
	case VDH_INFORMATION:
		AddHtmlWindow(pServer,CSecID((DWORD)pHint),_T(""));
		break;
	case VDH_INPUT_CONFIRM:
		DeleteSmallWindow(wServerID,CSecID((DWORD)pHint));
		break;
	case VDH_INPUT_URL:
		{
			CString sURL = *((CString*)pHint);
			AddHtmlWindow(pServer,SECID_NO_ID,sURL);
		}
		break;
	case VDH_NOTIFICATION_MESSAGE:
		{
			CString sMessage = *((CString*)pHint);
			AddTextWindow(pServer,SECID_NO_ID,sMessage);
		}
		break;
	case VDH_DEACTIVATE_CAMERAS:
		DeactivateCameras((WORD)pHint);
		break;
	case VDH_SINGLE_CAM:
		pRec = (CIPCOutputRecord*)pHint;
		SingleDisplayWindow(pServer,*pRec);
		break;
	case VDH_CAMERA_MAP:
	{
		CSecID idHost(SECID_LOCAL_HOST); // Local Host als vorgabe;
		if (pServer)
		{
			idHost = pServer->GetHostID();
		}
		CString sFile;
		sFile.Format(_T("%s\\%08lx\\camera.htm"), CNotificationMessage::GetWWWRoot(), idHost);
		if (DoesFileExist(sFile))
		{
			AddHtmlWindow(pServer,SECID_NO_ID,_T("camera.htm"));
		}
	}
	break;
#ifdef _HTML_ELEMENTS
	case VDH_CAM_MAP:
	{
		CHtmlWindow *pHW = GetHtmlWindow(wServerID, _T("camera.htm"));
		if (pHW)
		{
			pHW->OnUpdate(pSender, lHint, pHint);
		}
	}break;
	case VDH_HOST_MAP:
	{
		CHtmlWindow *pHW = GetHtmlWindow(0, _T(""));
		if (pHW)
		{
			pHW->OnUpdate(pSender, lHint, pHint);
		}
	}
#endif
	default:
		TRACE(_T("unknown cause in CVisionView::OnUpdate %d\n"),cause);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::PictureData(WORD wServerID, const CIPCPictureRecord &pict, CSecID id)
{
//	TRACE(_T("CVisionView::PictureData %d,%08lx\n"),wServerID,id);
	// Achtung in CIPCThread !
	m_SmallWindows.Lock();
	CDisplayWindow* pDW = GetDisplayWindow(wServerID,id);
	if (WK_IS_WINDOW(pDW))
	{
		pDW->PictureData(pict);
#ifndef _DTS
		// ML 20.7.99 Alle CoCoWindows deaktivieren, mit Ausnahme des Fensters,
		// an welches die Bilddaten gerichtet sind.
		if (pDW->IsCoco())
		{
			ClearCocoWasSomeTimesActive(pict.GetCamID());
		}
#endif
		if (   !GetDocument()->GetSequence()
			|| pDW->GetServer()->IsMultiCamera())
		{
			for (int i=0;i<m_SmallWindows.GetSize();i++)
			{
				CSmallWindow* pSW = m_SmallWindows.GetAt(i);
				if (   pSW->IsDisplayWindow()
					&& pSW->GetServerID() == pDW->GetServerID())
				{
					CDisplayWindow* pDW2 = (CDisplayWindow*)pSW;
					if (pDW2->IsWaitingForRequest())
					{
						TRACE(_T("set waiting cam as requested %s\n"),pDW2->GetName());
						pDW2->DoRequestPictures();
						break;
					}
				}
			}
		}
	}
	else
	{
		TRACE(_T("no window for image %d,%08lx\n"),wServerID,id.GetID());
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::AddSmallWindow(CSmallWindow* pSW, BOOL bIsAlarm /*=FALSE*/)
{
	m_SmallWindows.SafeAdd(pSW);
	
	if (   (m_SmallWindows.GetSize() == 1)
		|| bIsAlarm)
	{
		// war das erste Fenster
		SetCmdActiveSmallWindow(pSW);
		SetBigSmallWindow(pSW);
	}
	pSW->EnableTitle(theApp.m_bShowTitleOfSmallWindows);
	SortSmallWindowsByID(m_SmallWindows);

	// Set the appropriate quad view mode
	SetAppropriateQuadViewMode();
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetAppropriateQuadViewMode()
{
	CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
	ASSERT(pFrame != NULL);

	if (pFrame
		 && (pFrame->GetNoOfMonitors()       > 1) 
		 && (pFrame->GetNoOfExternalFrames() > 0))
	{
		int    i, j, nPossibleSW, nSW = m_SmallWindows.GetSize();

		// Alles schön, wenn gleichverteilt, aber...
		// ... Viel Spaß wenn nicht!
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
		CSmallWindow* pSW = NULL;
		DWORD dwUseMonitor = 1;
		BOOL bRegardless = TRUE;
		for (i=0 ; i<nSW ; i++)
		{
			pSW = m_SmallWindows.GetAt(i);
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
void CVisionView::SortSmallWindowsByID(CSmallWindows& sw)
{
	int iCur, iMin, iNext, nCount;
	CSmallWindow* swap;

	sw.Lock();
	nCount = sw.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
//		TRACE(_T("HostID %04x\n"), sw.GetAt(iCur)->GetHostID());
//		TRACE(_T("    ID %08x\n"), sw.GetAt(iCur)->GetID());
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			if (*sw.GetAt(iNext) < *sw.GetAt(iMin))
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = sw.GetAt(iCur);
			sw.SetAt(iCur,sw.GetAt(iMin));
			sw.SetAt(iMin,swap);
		}
	}
	sw.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SortSmallWindowsByIDandTime(CSmallWindows& sw)
{
	int iCur, iMin, iNext, nCount;
	CSmallWindow* swap;
	CSmallWindow *pSW1, *pSW2;
	CDisplayWindow *pDW1, *pDW2;

	sw.Lock();
	nCount = sw.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			pSW1 = sw.GetAt(iNext);
			pSW2 = sw.GetAt(iMin);

			// Only DisplayWindows can be sorted by time
			if (   pSW1->IsDisplayWindow()
				&& pSW2->IsDisplayWindow()
				)
			{
				pDW1 = (CDisplayWindow*)sw.GetAt(iNext);
				pDW2 = (CDisplayWindow*)sw.GetAt(iMin);
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
			swap = sw.GetAt(iCur);
			sw.SetAt(iCur, sw.GetAt(iMin));
			sw.SetAt(iMin, swap);
		}
	}

	sw.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SortSmallWindowsByMonitor(CSmallWindows &sw, DWORD dwPrefer)
{
	int iCur, iMin, iNext, nCount;
	CSmallWindow* swap, *pSW1, *pSW2;
	int nMonitor1, nMonitor2;

	sw.Lock();
	nCount = sw.GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur+1; iNext<nCount; iNext++)
		{
			pSW1 = sw.GetAt(iNext);
			pSW2 = sw.GetAt(iMin);
			nMonitor1 = (int) pSW1->GetMonitorFlag();
			nMonitor2 = (int) pSW2->GetMonitorFlag();
			if (dwPrefer == 0xffffffff)									// Don´t Care nach hinten
			{
				if (nMonitor1 == 0) nMonitor1 |= 0x40000000;
				else if (nMonitor1 & USE_MONITOR_EXCLUSIVE) nMonitor1 |= 0x80000000;
				if (nMonitor2 == 0) nMonitor2 |= 0x40000000;
				else if (nMonitor2 & USE_MONITOR_EXCLUSIVE) nMonitor2 |= 0x80000000;

			}
			else																	// Normalfall: don´t care vorne
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
			swap = sw.GetAt(iCur);
			sw.SetAt(iCur,sw.GetAt(iMin));
			sw.SetAt(iMin,swap);
		}
	}

	sw.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::AddReferenceWindow(CServer* pServer, CSecID id, 
									 const CString& sCameraName)
{
	CReferenceWindow* pRW = GetReferenceWindow(pServer->GetID(),sCameraName);
	
	if (WK_IS_WINDOW(pRW))
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
			CReferenceWindow* pReferenceWindow = NULL;

			if (!sCameraName.IsEmpty())
			{
				pReferenceWindow = new CReferenceWindow(pServer,id,sCameraName);
			}
			
			if (pReferenceWindow)
			{
				GetClientRect(rect);
				rect.right /= m_iXWin;
				rect.bottom /= m_iXWin;
				pReferenceWindow->Create(rect,m_dwIDCounter++,this);
				AddSmallWindow(pReferenceWindow);
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
void CVisionView::AddTextWindow(CServer* pServer,  CSecID id, 
								const CString& sMessage)
{
	CTextWindow* pTW = GetTextWindow(pServer->GetID(),sMessage);

	if (WK_IS_WINDOW(pTW))
	{
		pTW->SetNotificationText(sMessage);
	}
	else
	{
		CRect rect;
		CTextWindow* pTextWindow = NULL;

		if (!sMessage.IsEmpty())
		{
			pTextWindow = new CTextWindow(pServer, theApp.GetNextHtmlWindowID(), sMessage);
		}
		
		if (pTextWindow)
		{
			GetClientRect(rect);
			rect.right /= m_iXWin;
			rect.bottom /= m_iXWin;
			pTextWindow->Create(rect,m_dwIDCounter++,this);
			AddSmallWindow(pTextWindow);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow* CVisionView::AddHtmlWindow(CServer* pServer,  CSecID id, const CString& sURL)
{
	CHtmlWindow* pHW=NULL;
	WORD wID = SECID_NO_SUBID;
	if (pServer)
	{
		wID = pServer->GetID();
	}
	
	if (id!=SECID_NO_ID)
	{
		pHW = GetHtmlWindow(wID, id);
	}
	else if (!sURL.IsEmpty())
	{
		pHW = GetHtmlWindow(wID, sURL);
	}

	if (pHW==NULL)
	{
		CRect rect;
		CHtmlWindow* pHtmlWindow = NULL;

		if (id.IsInputID()) 
		{
			CIPCInputVision* pInput = pServer->GetInput();
			if (pInput)
			{
				pInput->Lock();
				CIPCInputRecord* pInputRecord = pInput->GetInputRecordPtrFromSecID(id);
				if (pInputRecord)
				{
					pHtmlWindow = new CHtmlWindow(pServer,id,pInputRecord->GetName());
				}
				pInput->Unlock();
			}
		}
		if (id.IsOutputID()) 
		{
			CIPCOutputVision* pOutput = pServer->GetOutput();
			const CIPCOutputRecord& rec = pOutput->GetOutputRecordFromSecID(id);
			pHtmlWindow = new CHtmlWindow(pServer,id,rec.GetName());
		}
		else if (!sURL.IsEmpty())
		{
			pHtmlWindow = new CHtmlWindow(pServer,sURL, theApp.GetNextHtmlWindowID());
		}
		
		if (pHtmlWindow)
		{
			GetClientRect(rect);
			rect.right /= m_iXWin;
			rect.bottom /= m_iXWin;
			pHtmlWindow->Create(rect,m_dwIDCounter++,this);
			AddSmallWindow(pHtmlWindow);
			pHW = pHtmlWindow;
		}
	}
	else
	{
		// make the window visible
		pHW->Refresh();
		pHW->ShowWindow(SW_SHOW);
	}
	return pHW;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::AddDisplayWindow(CServer* pServer, const CIPCOutputRecord & rec)
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetDisplayWindow(pServer->GetID(),rec.GetID());
	
	if (pDW)
	{
		// already open
		SetRequestedDisplayWindow(pDW);
		SetBigSmallWindow(pDW);
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		// if we have a view that has no 'free' space left
		// we have to increase m_iXWin first to calculate the correct size
		// gf why? will be calculated at SetWindowSize() anyway...
//DEL		while (m_SmallWindows.GetSize() >= (m_iXWin*m_iXWin))
//DEL		{
//DEL			m_iXWin++;
//DEL		}

		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

#ifdef _DTS
		if (rec.CameraDoesJpeg() && rec.CameraHasColor())
		{
			pDW = new CMicoWindow(pServer, rec);
		}
		else if (!rec.CameraDoesJpeg() && rec.CameraHasColor())
		{
			TRACE(_T("creating new CocoWindow for DTS %08lx\n"),rec.GetID().GetID());
			pDW = new CCocoWindow(pServer, rec, TRUE);
		}
#else
		if (rec.CameraIsPresence() && rec.CameraHasColor())
		{
			pDW = new CPTWindow(pServer, rec);
		}
		else if (!rec.CameraDoesJpeg() && rec.CameraHasColor())
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
			pDW = new CCocoWindow(pServer, rec, !bHardwareCocoAtTransmitter);
		}
		else if (rec.CameraDoesJpeg() && rec.CameraHasColor())
		{
			if (   (pServer->GetConfirmedOutputOptions() & SCO_JPEG_AS_H263)
				&& pServer->GetOutput()->GetBitrate()<=128*1024)
			{
				pDW = new CCocoWindow(pServer, rec, TRUE);
			}
			else
			{
				pDW = new CMicoWindow(pServer, rec);
			}
		}
#endif

		if (pDW)
		{
			if (pDW->Create(rect, this))
			{
				BOOL bAlarm = (pServer->GetAlarmID() == rec.GetID());
				AddSmallWindow(pDW, bAlarm);
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
void CVisionView::OnDeleteWindow(CSmallWindow* pSW)
{
	if (m_pCmdActiveSmallWindow == pSW)
	{
		m_pCmdActiveSmallWindow = NULL;
	}
	if (m_pBigSmallWindow == pSW)
	{
		m_pBigSmallWindow = NULL;
	}
	if (m_pHostMapWindow==pSW)
	{
		m_pHostMapWindow = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SingleDisplayWindow(CServer* pServer, const CIPCOutputRecord & rec)
{
	BOOL bCallSetWindowSize = FALSE;
	int i,c;
	CSmallWindow* pSW = NULL;
	CDisplayWindow* pSingleDW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=c-1;i>=0;i--)
	{
		pSW = m_SmallWindows.GetAt(i);
		if (   WK_IS_WINDOW(pSW)
			&& pSW->IsDisplayWindow()
			&& pSW->GetServerID() == pServer->GetID())
		{
			// window is of same server and is displaywindow
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			if (pDW->GetCamID() == rec.GetID())
			{
				pSingleDW = pDW;
			}
			else
			{
				if (pSW->IsDisplayWindow())
				{
					CDisplayWindow* pDW = (CDisplayWindow*)pSW;
					if (   pDW->CanMultipleRequests()
						|| GetNrOfDisplayWindows(pServer->GetID())==1)
					{
						pDW->DeactivateCamera();
					}
				}
				OnDeleteWindow(pDW);
				pSW->DestroyWindow();
				WK_DELETE(pSW);
				m_SmallWindows.RemoveAt(i);
				bCallSetWindowSize = TRUE;
			}
		}
	}

	m_SmallWindows.Unlock();

	if (pSingleDW == NULL)
	{
		AddDisplayWindow(pServer,rec);
	}
	else
	{
		if (!pSingleDW->IsRequesting())
		{
			pSingleDW->SetActiveRequested();
		}
		else if (pSingleDW->IsWaitingForRequest())
		{
			pSingleDW->DoRequestPictures();
		}
	}

	if (bCallSetWindowSize)
	{
		SetAppropriateQuadViewMode();
		SetWindowSize();
	}
}
/////////////////////////////////////////////////////////////////////////////
int	CVisionView::GetNrOfDisplayWindows(WORD wServerID)
{
	int i,c;
	int r=0;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (   pSW 
			&& pSW->IsDisplayWindow()
			&& pSW->GetServerID() == wServerID)
		{
			r++;
		}
	}
	m_SmallWindows.Unlock();
	return r;
}
/////////////////////////////////////////////////////////////////////////////
int	CVisionView::GetNrOfDisplayWindows()
{
	int i,c;
	int r=0;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			r++;
		}
	}
	m_SmallWindows.Unlock();
	return r;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetStore(BOOL bOn)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			pDW->SetStore(bOn);
		}
	}
	m_SmallWindows.Unlock();

	CMainFrame* pMF = (CMainFrame*)theApp.GetMainWnd();
	pMF->SetStore(bOn);
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow*	CVisionView::GetCmdActiveDisplayWindow()
{
	if (   WK_IS_WINDOW(m_pCmdActiveSmallWindow)
		&& m_pCmdActiveSmallWindow->IsDisplayWindow())
	{
		return (CDisplayWindow*)m_pCmdActiveSmallWindow;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CSmallWindow*	CVisionView::GetCmdActiveSmallWindow()
{
	if (WK_IS_WINDOW(m_pCmdActiveSmallWindow))
	{
		return m_pCmdActiveSmallWindow;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CSmallWindow*	CVisionView::GetExclusiveSmallWindow()
{
	CSmallWindow* pSW = m_SmallWindows.GetAt(0);
	if (WK_IS_WINDOW(pSW) && pSW->GetMonitorFlag() & USE_MONITOR_EXCLUSIVE)
	{
		return pSW;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow*	CVisionView::GetDisplayWindow(WORD wServerID, CSecID camID)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();

	if (c>0)
	{
		for (i=0;i<c;i++) 
		{
			pSW = m_SmallWindows.GetAt(i);

			if (WK_IS_WINDOW(pSW))
			{
				if (pSW->IsDisplayWindow())
				{
					if (pSW->GetServerID()==wServerID)
					{
						CDisplayWindow* pDW = (CDisplayWindow*)pSW;
						if (pDW->GetCamID()==camID)
						{
							m_SmallWindows.Unlock();
							return pDW;
						}
						else
						{
							if (camID.IsInputID())
							{
								WORD wCameraNr = (WORD)(camID.GetSubID() & 0x001F);
								if (wCameraNr==pDW->GetCamID().GetSubID())
								{
									m_SmallWindows.Unlock();
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
			else
			{
//				WK_TRACE_ERROR(_T("pSW is no window %d %lx\n"),wServerID,camID.GetID());
			}
		}
	}

	m_SmallWindows.Unlock();

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionView::HaveAllWindowsGotAPicture(WORD wServerID)
{
	BOOL bRet = TRUE;

	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();

	for (int i=0;i<m_SmallWindows.GetSize() && bRet;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);

		if (WK_IS_WINDOW(pSW))
		{
			if (pSW->IsDisplayWindow())
			{
				if (pSW->GetServerID()==wServerID)
				{
					CDisplayWindow* pDW = (CDisplayWindow*)pSW;
					bRet &= pDW->WasSometimesActive();
				}
			}
		}
	}
	m_SmallWindows.Unlock();
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::TraceSmallWindows()
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW)
		{
			WK_TRACE(_T("sw %i s=%d c=%08lx\n"),i,pSW->GetServerID(),pSW->GetID().GetID());
		}
		else
		{
			WK_TRACE(_T("sw %i (null)\n"));
		}
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CReferenceWindow* CVisionView::GetReferenceWindow(WORD wServerID, 
												  const CString& sCameraName)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && 
			(pSW->IsReferenceWindow()) &&
			(pSW->GetServerID()==wServerID))
		{
			CReferenceWindow* pRW = (CReferenceWindow*)pSW;
			if (sCameraName == pRW->GetCameraName())
			{
				m_SmallWindows.Unlock();
				return pRW;
			}
		}
	}
	m_SmallWindows.Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CTextWindow* CVisionView::GetTextWindow(WORD wServerID, const CString& sText)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && 
			(pSW->IsTextWindow()) &&
			(pSW->GetServerID()==wServerID))
		{
			CTextWindow* pTW = (CTextWindow*)pSW;
			if (sText == pTW->GetText())
			{
				m_SmallWindows.Unlock();
				return pTW;
			}
		}
	}
	m_SmallWindows.Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow* CVisionView::GetHtmlWindow(WORD wServerID, const CString& sURL)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsHtmlWindow())
		{
			if (pSW->GetServerID()==wServerID)
			{
				CHtmlWindow* pHW = (CHtmlWindow*)pSW;
				if (pHW->IsURL(sURL))
				{
					m_SmallWindows.Unlock();
					return pHW;
				}
			}
			else if (pSW->GetServerID() == SECID_NO_SUBID)
			{
				CServer *pServer = GetDocument()->GetLocalServer();
				if (pServer && pServer->GetID() == wServerID)
				{
					pSW->PostMessage(WM_COMMAND, ID_SMALL_CLOSE);
				}
			}
		}
	}
	m_SmallWindows.Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow* CVisionView::GetHtmlWindow(WORD wServerID, CSecID id)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && 
			(pSW->IsHtmlWindow()) &&
			(pSW->GetServerID()==wServerID))
		{
			CHtmlWindow* pHW = (CHtmlWindow*)pSW;
			if (pHW->GetID()==id)
			{
				m_SmallWindows.Unlock();
				return pHW;
			}
		}
	}
	m_SmallWindows.Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
//DEL void CVisionView::DeleteSmallWindow(CSmallWindow* pSWtoDelete)
//DEL {
//DEL 	int i;
//DEL 	CSmallWindow* pSW = NULL;
//DEL 	BOOL bWasActive = FALSE;
//DEL 	WORD wServerID = pSWtoDelete->GetServerID();
//DEL 	CSecID id = pSWtoDelete->GetID();
//DEL 	
//DEL 	if (pSWtoDelete->IsDisplayWindow())
//DEL 	{
//DEL 		CDisplayWindow* pDW = (CDisplayWindow*)pSWtoDelete;
//DEL 		if (   pDW->CanMultipleRequests()
//DEL 			|| GetNrOfDisplayWindows(wServerID)==1)
//DEL 		{
//DEL 			pDW->DeactivateCamera();
//DEL 		}
//DEL 		CVisionDoc* pDoc = GetDocument();
//DEL 		pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW,wServerID),(CObject*)id.GetID());
//DEL 	}
//DEL 
//DEL 
//DEL 	m_SmallWindows.Lock();
//DEL 	for (i=0;i<m_SmallWindows.GetSize();i++) 
//DEL 	{
//DEL 		pSW = m_SmallWindows.GetAt(i);
//DEL 		if (pSW == pSWtoDelete)
//DEL 		{
//DEL 			OnDeleteWindow(pSW);
//DEL 			m_SmallWindows.RemoveAt(i);
//DEL 			pSW->DestroyWindow();
//DEL 			WK_DELETE(pSW);
//DEL 			break;
//DEL 		}
//DEL 	}
//DEL 	m_SmallWindows.Unlock();
//DEL 
//DEL 	if (m_SmallWindows.GetSize()<=(m_iXWin-1)*(m_iXWin-1))
//DEL 	{
//DEL 		if (m_iXWin>1)
//DEL 		{
//DEL 			m_iXWin--;
//DEL 		}
//DEL 	}
//DEL 	SetWindowSize();
//DEL 	if (bWasActive)
//DEL 	{
//DEL 		for (i=0;i<m_SmallWindows.GetSize();i++) 
//DEL 		{
//DEL 			pSW = m_SmallWindows.GetAt(i);
//DEL 			if (   pSW 
//DEL 				&& (pSW->GetServerID()==wServerID))
//DEL 			{
//DEL 				if (pSW->IsDisplayWindow())
//DEL 				{
//DEL 					CDisplayWindow* pDW = (CDisplayWindow*)pSW;
//DEL 					pDW->ActivateCamera();
//DEL 				}
//DEL 			}
//DEL 		}
//DEL 	}
//DEL }
/////////////////////////////////////////////////////////////////////////////
void CVisionView::DeleteSmallWindow(WORD wServerID, CSecID id)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	BOOL bWasActive = FALSE;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (pSW->GetID()==id)
			{
				CMainFrame* pMF = (CMainFrame*)GetParentFrame();
				ASSERT(pMF != NULL);
				if (pSW->IsDisplayWindow())
				{
					CDisplayWindow* pDW = (CDisplayWindow*)pSW;
					bWasActive = pDW->IsRequesting();
					if (   pDW->CanMultipleRequests()
						|| GetNrOfDisplayWindows(wServerID)==1)
					{
						pDW->DeactivateCamera();
					}
				}
#ifdef _HTML_ELEMENTS
				else if (pSW->IsHtmlWindow())
				{
					((CHtmlWindow*)pSW)->OnRequestSave();
				}
#endif
				for (int j=0; j<pMF->GetNoOfExternalFrames(); j++)
				{
					pMF->GetExternalFrame(j)->RemoveSmallWindow(pSW);
				}
				OnDeleteWindow(pSW);
				m_SmallWindows.RemoveAt(i);
				pSW->DestroyWindow();
				WK_DELETE(pSW);
				// Werden keine Fenster mehr angezeigt?
				// Dann müssen wir aus dem Vollbildmodus raus
				// sonst ist nur noch ein weißer Bildschirm da...
				if (m_SmallWindows.GetSize() == 0)
				{
					if (pMF && pMF->IsFullScreen())
					{
						pMF->ChangeFullScreenModus();
					}
				}
				break;
			}
		}
	}
	m_SmallWindows.Unlock();

	SetAppropriateQuadViewMode();
	SetWindowSize();

	CVisionDoc* pDoc = GetDocument();
	pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW,wServerID),(CObject*)id.GetID());
	
	if (bWasActive)
	{
		// GF todo m_SmallWindows.Lock;
		c = m_SmallWindows.GetSize();
		for (i=0;i<c;i++) 
		{
			pSW = m_SmallWindows.GetAt(i);
			if (pSW && (pSW->GetServerID()==wServerID))
			{
				if (pSW->IsDisplayWindow())
				{
					CDisplayWindow* pDW = (CDisplayWindow*)pSW;
					pDW->ActivateCamera();
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::DeactivateCameras(WORD wServerID)
{
	int i,c;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=c-1;i>=0;i--) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (pSW->IsDisplayWindow())
			{
				CDisplayWindow* pDW = (CDisplayWindow*)pSW;
				if (   pDW->IsActive() 
					|| pDW->IsRequesting())
				{
					pDW->DeactivateCamera();
				}
			}
		}
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::DeleteSmallWindows(WORD wServerID)
{
	int i,c;
	CSmallWindow* pSW = NULL;

	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=c-1;i>=0;i--) 
	{
		pSW = m_SmallWindows.GetAt(i);
#ifdef _HTML_ELEMENTS
		if (pSW->IsHtmlWindow())
		{
			((CHtmlWindow*)pSW)->OnRequestSave();
		}
#endif
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			m_SmallWindows.RemoveAt(i);
			OnDeleteWindow(pSW);
			pSW->DestroyWindow();
			WK_DELETE(pSW);
		}
	}
	m_SmallWindows.Unlock();

	SetAppropriateQuadViewMode();
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionView::SetRequestedDisplayWindow(CDisplayWindow* pDWRequested)
{
	if (!WK_IS_WINDOW(pDWRequested))
	{
		WK_TRACE_ERROR(_T("CSmallWindow* pSWRequested is not a window"));
		return FALSE;
	}
//	TRACE(_T("SetRequestedDisplayWindow %s %s\n"),pDWRequested->GetServer()->GetName(),pDWRequested->GetName());

	// activate request SmallWindow
	// because of manual/Sequencer Request
	CSmallWindow* pSW = NULL;
	BOOL bRet = FALSE;
	BOOL bFound = FALSE;
	// De-Request all other windows of this server
	m_SmallWindows.Lock();

	for (int i=0;i<m_SmallWindows.GetSize() && !bFound; i++)
	{
		pSW = m_SmallWindows.GetAt(i);
		if (   WK_IS_WINDOW(pSW)
			&& pSW->GetServerID() == pDWRequested->GetServerID())
		{
			if (pSW->IsDisplayWindow())
			{
				CDisplayWindow* pDW = (CDisplayWindow*)pSW;
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
						// wird dann behandelt, wenn das nächste Bild kommt
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
	m_SmallWindows.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetRequestedDisplayWindow(WORD wServerID, CSecID id)
{
	CSmallWindow* pSW = NULL;
	m_SmallWindows.Lock();
	int c = m_SmallWindows.GetSize();
	for (int i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (   pSW->GetID()==id
				&& pSW->IsDisplayWindow())
			{
				CDisplayWindow* pDW = (CDisplayWindow*)pSW;
				pDW->ActivateCamera();
				break;
			}
		}
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetCmdActiveSmallWindow(CSmallWindow* pSW)
{
	if (   WK_IS_WINDOW(pSW)
		&& m_pCmdActiveSmallWindow!=pSW)
	{
		if (WK_IS_WINDOW(m_pCmdActiveSmallWindow))
		{
			m_pCmdActiveSmallWindow->SetActive(FALSE);
		}
		m_pCmdActiveSmallWindow = pSW;
		m_pCmdActiveSmallWindow->SetActive();
		if (m_pCmdActiveSmallWindow->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			SetRequestedDisplayWindow(pDW);
#ifndef _DTS
			pDW->DoRequestVideoOut();
#endif
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetBigSmallWindow(CSmallWindow* pSW)
{
	TRACE(_T("SetBigSmallWindow %08lx\n"),pSW->GetID());
	if (m_bView1Plus)
	{
		if (   WK_IS_WINDOW(pSW)
			&& m_pBigSmallWindow!=pSW)
		{
			m_pBigSmallWindow = pSW;
			SetWindowSize();
		}
	}
	else
	{
		// Always set the BigOne, will be used at next manual 1plus switch
		m_pBigSmallWindow = pSW;
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
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetSmallWindowView1(CSmallWindow* pSW)
{
// Wird nur durch Doppelklick vom SmallWindow aufgerufen
// Aufruf im Menü und Toolbar bei Multi-Monitor disabled, also hier auch!
// Auch hier (bzw. bei SetSmallWindowPositionsXxX) wieder das Problem
// mit dem View-Status, jedenfalls bei Multi-Monitor, SIGH!
	if (IsView1x1Allowed())
	{
		if (pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			TRACE(_T("SetSmallWindowView1 %s\n"),pDW->GetName());
			if (!pDW->IsRequesting())
			{
				SetRequestedDisplayWindow(pDW);
			}
		}
		Cancel1PlusMode();
		SetCmdActiveSmallWindow(pSW);
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
void CVisionView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if ((cx != 0) && (cy != 0))
	{
		SetWindowSize(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetXWin(int iXWin)
{
	TRACE(_T("SetXWin\n"), iXWin);
	if (iXWin == 0)
	{
		WK_TRACE_ERROR(_T("SetXWin iXWin == 0\n"));
		iXWin = 1;
	}
	m_iXWin = iXWin;
	Cancel1PlusMode();
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetSmallWindowPositions(CSmallWindows& sws)
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
void CVisionView::SetSmallWindowPositions1pX(CSmallWindows& sws)
{
	CSmallWindows aMain, *psws = &sws;
	int iSmallWindowCount = sws.GetSize();
	int iXWin = 1;

	CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
	ASSERT(pFrame != NULL);
	if (pFrame->GetNoOfExternalFrames())
	{
		CSmallWindow* pSmallWindow = NULL;
		CSmallWindows aExt;
		aExt.Copy(sws);
		if (iSmallWindowCount > 1)
		{
			CRect rc;
			CExternalFrameDlg *pEF = pFrame->GetExternalFrame(0);
			ASSERT(pEF!=NULL);
			int i, nMain, nExt = 0;

			nMain = CExternalFrameDlg::GetSmallWindows(m_hWnd, aMain);
			if (!m_bSizing)
			{
				if (m_pBigSmallWindow == NULL)
				{
					m_pBigSmallWindow = sws.GetAt(0);
				}
				aExt.Remove(m_pBigSmallWindow);

				if (pFrame->RemoveSWfromExtFrames(m_pBigSmallWindow))
				{
					nMain++;
				}
				if (iSmallWindowCount > 25)
				{
					if      (nMain > 10) SortSmallWindowsByMonitor(aExt, USE_MONITOR_2);
					else if (nMain < 10) SortSmallWindowsByMonitor(aExt, USE_MONITOR_1);
					iSmallWindowCount = aExt.GetSize();
					for (i=0; i<iSmallWindowCount; i++)
					{
						pSmallWindow = aExt.GetAt(i);
						if (nMain < 10)
						{
							if (pFrame->RemoveSWfromExtFrames(pSmallWindow)) nMain++;
						}
						else if (nMain > 10)
						{
							if (pEF->AddSmallWindow(pSmallWindow)) nMain--;
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
						pSmallWindow = aExt.GetAt(i);
						if (nMain < 8)
						{
							if (pFrame->RemoveSWfromExtFrames(pSmallWindow)) nMain++;
						}
						else if (nMain > 8)
						{
							if (pEF->AddSmallWindow(pSmallWindow)) nMain--;
						}
						else break;
					}
					nExt = 0;
				}
				else
				{
					iSmallWindowCount = aExt.GetSize();
					for (i=0; i<iSmallWindowCount; i++)
					{
						pSmallWindow = aExt.GetAt(i);
						if (pEF->AddSmallWindow(pSmallWindow))
						{
							nMain--;
						}
					}
					nExt = aExt.GetSize();
				}
			}
			
			if (nExt == 0)
			{
				nExt  = CExternalFrameDlg::GetSmallWindows(pEF->m_hWnd, aExt);
			}
			int iXWin = max((int)ceil(sqrt((double)(nExt))), 1);
			pEF->m_iXWin = iXWin;
			SetSmallWindowPositionsXxX(aExt, pEF);
			if (nMain>1)
			{
				nMain = CExternalFrameDlg::GetSmallWindows(m_hWnd, aMain);
				psws = &aMain;
				iSmallWindowCount = psws->GetSize();
			}
			else
			{
				GetClientRect(&rc);
				m_pBigSmallWindow->MoveWindow(&rc);
				if (!m_pBigSmallWindow->IsWindowVisible())
				{
					m_pBigSmallWindow->ShowWindow(SW_SHOW);
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

	CRect r;
	GetClientRect(&r);
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

	CSmallWindow* pSmallWindow = NULL;
	for (i=0,j=0;i<iSmallWindowCount && i<psws->GetSize();i++) 
	{
		pSmallWindow = psws->GetAt(i);
		if (pSmallWindow == m_pBigSmallWindow)
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
				if (pSmallWindow->IsKindOf(RUNTIME_CLASS(CPlayWindow)))
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
void CVisionView::SetSmallWindowPositionsXxX(CSmallWindows& sws, CWnd *pExternal)
{
	CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
	ASSERT(pFrame != NULL);
	if (   (pFrame->GetNoOfExternalFrames() > 0)
		 && (pExternal == NULL) )
	{
		int iSmallWindowCount = sws.GetSize();
		CSmallWindow* pSmallWindow = NULL;
		if (iSmallWindowCount > 0)
		{
			// Sort small windows by monitor ...
			CRect rc;
			CExternalFrameDlg *pEF = pFrame->GetExternalFrame(0);
			ASSERT(pEF!=NULL);
			int i=0, nMain = 0, nExt = 0, nExclusive = 0;
			DWORD dwMonitor;
//			ASSERT(&sws == &m_SmallWindows);
			CSmallWindows aMain, aExt, &aAll = sws;// sort the main SW array
//			aAll.Copy(sws);								// to make sure, that the exclusive SW is the first
			SortSmallWindowsByMonitor(aAll);       // after sorting

			dwMonitor = aAll.GetAt(0)->GetMonitorFlag();
			int nPossibleMain = m_iXWin * m_iXWin;
			if (dwMonitor & USE_MONITOR_EXCLUSIVE)
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
				pSmallWindow = aAll.GetAt(i);
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
					pEF->AddSmallWindow(pSmallWindow);
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
				pSmallWindow = aAll.GetAt(i);
				// Check, if main monitor has all possible
				if (nMain < nPossibleMain)
				{
					pFrame->RemoveSWfromExtFrames(pSmallWindow);
					nMain++;
				}
				else if ((nExclusive & USE_MONITOR_2) == 0)
				{
					pEF->AddSmallWindow(pSmallWindow);
					nExt++;
				}
			}

			// Now sort the small windows on each monitor
			nMain = CExternalFrameDlg::GetSmallWindows(m_hWnd, aMain);
			SortSmallWindowsByID(aMain);
			SetSmallWindowPositionsXxX(aMain, this);
			
			nExt  = CExternalFrameDlg::GetSmallWindows(pEF->m_hWnd, aExt);
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
			iXWin = ((CExternalFrameDlg*)pExternal)->m_iXWin;
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

	CSmallWindow* pSW = NULL;
	CRect rect;

	for (i=0;i<sws.GetSize();i++)
	{
		pSW = sws.GetAt(i);
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
void CVisionView::SetWindowSize(BOOL bFromOnSize/*=FALSE*/)
{
	int iMaxVisible = 0;

	if (m_bView1Plus)
	{
		iMaxVisible = m_SmallWindows.GetSize();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
		ASSERT(pFrame != NULL);
		iMaxVisible = m_iXWin*m_iXWin;
		CExternalFrameDlg *pExtFrame;
		for (int i=0 ; i<pFrame->GetNoOfExternalFrames() ; i++)
		{
			pExtFrame = pFrame->GetExternalFrame(i);
			if (pExtFrame)
			{
				int iXWin = pExtFrame->m_iXWin;
				iMaxVisible += (iXWin*iXWin);
			}
		}
	}

	m_SmallWindows.Lock();
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

#ifndef _DTS
	SetDisplayOutputWindow(SDOW_DISPLAY|SDOW_CLEARSTA);
#endif

	CheckAndSetActiveSmallWindow();
	m_bSizing = FALSE;

	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetWindowSizeAtResizing()
{
	TRACE(_T("SetWindowSize visible resizing\n"));
	CSmallWindows justVisible;
	// play all visible windows in oldVisible
	CSmallWindow* pSW = NULL;
	for (int i=0 ; i<m_SmallWindows.GetSize() ; i++) 
	{
		pSW = m_SmallWindows[i];
		if (   WK_IS_WINDOW(pSW)
			&& pSW->IsWindowVisible())
		{
			justVisible.Add(pSW);
		}
	}
	SetSmallWindowPositions(justVisible);
	justVisible.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetWindowSizeReplacing(int iMaxVisible)
{
	TRACE(_T("SetWindowSize replacing windows\n"));
	// es ist nicht genug Platz für alle Fenster
	// es wird jetzt ermittelt, welches Fenster weichen muss
	// Ziel ist es alte Fenster auszublenden und durch neue zu ersetzen
	CSmallWindows oldVisibleDisplays, oldVisibleNonDisplays, oldVisibles;
	CSmallWindows newVisibleDisplays, newVisibleNonDisplays, newVisibles, newHiddenDisplays;
	CSmallWindow* pSW = NULL;
	int i = 0;

	// play all visible windows in oldVisibleDisplays and oldVisibleNonDisplays
	for (i=0 ; i<m_SmallWindows.GetSize() ; i++) 
	{
		pSW = m_SmallWindows[i];
		if (WK_IS_WINDOW(pSW))
		{
			if (pSW->IsWindowVisible())
			{
				if (pSW->IsDisplayWindow())
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
		&& m_pCmdActiveSmallWindow!=NULL)
	{
		newVisibles.Add(m_pCmdActiveSmallWindow);
	}
	else
	{
		for (i=0;i<m_SmallWindows.GetSize();i++) 
		{
			pSW = m_SmallWindows[i];
			if (pSW->IsDisplayWindow())
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
		CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
		ASSERT(pFrame != NULL);
		if (pFrame->GetNoOfExternalFrames() == 0)
		{
			for (i=0 ; i<oldVisibles.GetSize() ; i++)
			{
				pSW = oldVisibles.GetAt(i);
				if (   (iNewVisible == 0)
					|| (pSW != newVisibles.GetAt(0))
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
				m_pBigSmallWindow = newVisibles.GetAt(0);
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
					CSmallWindow* pOldSW = oldVisibles.GetAt(0);
					CSmallWindow* pNewSW = newVisibles.GetAt(i);
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
					pSW = newHiddenDisplays.GetAt(i);
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
				pSW = newVisibles.GetAt(0);
				newVisibles.RemoveAt(0);
				newHiddenDisplays.Add(pSW);
			}
			SetSmallWindowPositions(newVisibles);
		}
		for (i=0 ; i<newHiddenDisplays.GetSize() ; i++)
		{
			pSW = newHiddenDisplays.GetAt(i);
			pSW->ShowWindow(SW_HIDE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::CheckAndSetActiveSmallWindow()
{
	if (   m_pBigSmallWindow == NULL
		&& m_SmallWindows.GetSize()>0)
	{
		for (int i=0;i<m_SmallWindows.GetSize();i++) 
		{
			if (m_SmallWindows.GetAt(i)->IsWindowVisible())
			{
				m_pBigSmallWindow = m_SmallWindows.GetAt(i);
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnToggle()
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
void CVisionView::OnView1Plus() 
{
	TRACE(_T("CVisionView::OnView1Plus\n"));
	m_bView1Plus = !m_bView1Plus;
	SetAppropriateQuadViewMode(); // Is needed after 1x1 for correct scaling!
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnView1() 
{
	SetXWin(1);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnView22() 
{
	SetXWin(2);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnView33() 
{
	SetXWin(3);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnView44() 
{
	SetAppropriateQuadViewMode();
	SetXWin(m_iXWin);
}
/////////////////////////////////////////////////////////////////////////////
int CVisionView::GetNrOfRequestingWindows()
{
	 int iRequesting = 0;

	 for (int i=0;i<m_SmallWindows.GetSize();i++)
	 {
		 CSmallWindow* pSW = m_SmallWindows.GetAt(i);
		 if (   pSW
			 && pSW->IsDisplayWindow())
		 {
			 CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			 if (pDW->IsRequesting())
			 {
				 iRequesting++;
			 }
		 }
	 }

	 return iRequesting;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateView1Plus(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bView1Plus);
	pCmdUI->Enable(m_SmallWindows.GetSize()>1);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateView1(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if (m_SmallWindows.GetSize()>0)
	{
		bEnable = IsView1x1Allowed();
	}
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_iXWin==1 && !m_bView1Plus);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateView22(CCmdUI* pCmdUI) 
{
	BOOL bEnable = OnUpdateViewXX(2);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_iXWin==2 && !m_bView1Plus);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateView33(CCmdUI* pCmdUI) 
{
	BOOL bEnable = OnUpdateViewXX(3);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_iXWin==3 && !m_bView1Plus);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateView44(CCmdUI* pCmdUI) 
{
	BOOL bEnable = OnUpdateViewXX(4);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_iXWin==4 && !m_bView1Plus);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionView::OnUpdateViewXX(int nX)
{
	int nQm1 = nX-1, nQ = nX, nQp1 = nX+1;
	BOOL bEnable = FALSE;
	nQm1 *= nQm1;

	if (m_SmallWindows.GetSize() > nQm1)
	{
		CMainFrame *pFrame = ((CMainFrame*)GetParentFrame());
		if (!WK_IS_WINDOW(pFrame)) return bEnable;
		int nExtFrames = pFrame->GetNoOfExternalFrames();
		nQ *= nQ;
		DWORD dwMonitor = m_SmallWindows.GetAt(0)->GetMonitorFlag();
		if (dwMonitor & USE_MONITOR_EXCLUSIVE)
		{
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfDisplayWindows() - 1;
			bEnable =  ((iRequesting + iNonDisplay) <= nQ);
		}
		else if (nExtFrames)
		{
			nQp1 *= nQp1;
			int nMin = nQm1 + nExtFrames * nQ;
			int nMax = nQ   + nExtFrames * nQp1;
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfDisplayWindows();
			int nWnd = iRequesting + iNonDisplay;
			bEnable  = (nWnd > nMin && nWnd <= nMax);
		}
		else
		{
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfDisplayWindows();
			bEnable =  ((iRequesting + iNonDisplay) <= nQ);
		}
	}
	return bEnable;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnVideoFein() 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();
	if (WK_IS_WINDOW(pDW))
	{
		pDW->OnVideoFein();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateVideoFein(CCmdUI* pCmdUI) 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();

	if (WK_IS_WINDOW(pDW))
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
void CVisionView::OnVideoGrob() 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();
	if (WK_IS_WINDOW(pDW))
	{
		pDW->OnVideoGrob();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateVideoGrob(CCmdUI* pCmdUI) 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();

	if (WK_IS_WINDOW(pDW))
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
void CVisionView::OnVideoCsb() 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();

	if (WK_IS_WINDOW(pDW))
	{
		pDW->OnVideoCsb();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateVideoCsb(CCmdUI* pCmdUI) 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();

	if (WK_IS_WINDOW(pDW))
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
#ifndef _DTS
void CVisionView::ClearDisplayOutputWindow()
{
	CVisionDoc*  pDoc  = GetDocument();
	CIPCOutputVisionDecoder* pDecoder = pDoc->GetCIPCOutputVisionDecoder();
	if (pDecoder)
	{
		pDecoder->DoRequestSetDisplayWindow(SECID_NO_GROUPID,CRect(0,0,0,0));
	}
	ClearCocoWasSomeTimesActive(SECID_NO_ID);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::ClearCocoWasSomeTimesActive(CSecID camID)
{
	int i,c;
	CDisplayWindow* pDW;
	CSmallWindow* pSW;
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++)
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW->IsDisplayWindow())
		{
			pDW = (CDisplayWindow*)pSW;
			if (pDW && pDW->IsCoco() && pDW->GetCamID()!=camID)
			{
				pDW->ClearWasSomeTimesActive();
			}
		}
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::SetDisplayOutputWindow(DWORD dwOptions)
{
	CMainFrame* pMF = (CMainFrame*)GetParentFrame();
	ASSERT(pMF != NULL);

	if (pMF->IsActive())
	{
		CVisionDoc*  pDoc  = GetDocument();
		CIPCOutputVisionDecoder* pDecoder = pDoc->GetCIPCOutputVisionDecoder();
		if (pDecoder)
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = GetCmdActiveDisplayWindow();
			if (WK_IS_WINDOW(pDW) && pDW->IsCoco())
			{
				CCocoWindow* pCW = (CCocoWindow*)pDW;
				CRect rect;
				BOOL bToLarge = FALSE;

				GetWindowRect(rect);

				rect.left = max(rect.left,0);
				rect.top  = max(rect.top,0);
				rect.right = min(rect.right,GetSystemMetrics(SM_CXSCREEN));
				rect.bottom = min(rect.bottom,GetSystemMetrics(SM_CYSCREEN));

				if (dwOptions & SDOW_REACTIVATE)
				{
					// gf todo muss ggf. angepasst werden, wenn ge-switched wurde
					pCW->ActivateCamera();
				}

				if (( rect.Width() > theApp.m_MaxMegraRect.Width() ) ||
					( rect.Height() > theApp.m_MaxMegraRect.Height()) )
				{
					pCW->GetVideoClientRect(rect);
					pCW->ClientToScreen(rect);
					bToLarge = TRUE;
				}

				pDecoder->DoRequestSetDisplayWindow(SECID_NO_GROUPID,rect);
				if (dwOptions & SDOW_DISPLAY)
				{
					if (bToLarge)
					{
						rect.bottom = rect.Height();
						rect.right = rect.Width();
						rect.top = rect.left = 0;
						pCW->SetCIPCOutputWindow(rect);

						// clear active flag of other coco windows
						ClearCocoWasSomeTimesActive(pCW->GetCamID());
					}
					else
					{
						pCW->SetCIPCOutputWindow();
						if (dwOptions & SDOW_CLEARSTA)
						{
							ClearCocoWasSomeTimesActive(pCW->GetCamID());
						}
					}
				}
			}
			else
			{
				ClearDisplayOutputWindow();
			}
		}
	}
	else
	{
//		TRACE(_T("CVisionView::SetCIPCDisplayAndOutputWindow() not active\n"));
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnMove(int x, int y) 
{
	CView::OnMove(x, y);
#ifndef _DTS
	SetDisplayOutputWindow(0);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnVideoSave() 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();

	if (WK_IS_WINDOW(pDW))
	{
		pDW->OnVideoSave();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateVideoSave(CCmdUI* pCmdUI) 
{
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = GetCmdActiveDisplayWindow();

	if (WK_IS_WINDOW(pDW))
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
void CVisionView::OnViewOriginal() 
{
	m_b1to1 = !m_b1to1;

	int i,c;
	CSmallWindow* pSW = NULL;
	m_SmallWindows.Lock();
	c = m_SmallWindows.GetSize();
	for (i=0;i<c;i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			pDW->Set1to1(m_b1to1);
		}
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL bHandled = FALSE;

	if (WK_IS_WINDOW(m_pCmdActiveSmallWindow))
	{
		if (m_pCmdActiveSmallWindow->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
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
void CVisionView::OnUpdateSequenze(CCmdUI* pCmdUI) 
{
	int i,c;

	c = 0;
	// GF todo m_SmallWindows.Lock;
	for (i=0;i<m_SmallWindows.GetSize();i++)
	{
		CSmallWindow* pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
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
	BOOL bEnable = (c > 1);
	pCmdUI->Enable(bEnable);
	if (bEnable == FALSE)
	{
		GetDocument()->SetSequence(FALSE);
	}
	pCmdUI->SetCheck(GetDocument()->GetSequence());
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow* CVisionView::GetNextSequenceDisplayWindow(WORD wServerID, CDisplayWindow* pCurrent)
{
	CDisplayWindow* pDWReturn = NULL;
	CDisplayWindow* pDW = NULL;
	CSmallWindow* pSW = NULL;
	CSmallWindows serverwindows;

	// erst mal alle fenster des Servers raussuchen
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW->IsDisplayWindow())
		{
			pDW = (CDisplayWindow*)pSW;
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
			pDW = (CDisplayWindow*)serverwindows.GetAt(i);
			if (pDW->GetCamID() == pCurrent->GetCamID())
			{
				// found
				if (i+1<serverwindows.GetSize())
				{
					pDWReturn = (CDisplayWindow*)serverwindows.GetAt(i+1);
				}
				else
				{
					pDWReturn = (CDisplayWindow*)serverwindows.GetAt(0);
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
WORD CVisionView::GetNextSequenceServer(WORD wServerID, int start)
{
	for (int i=start;i<m_SmallWindows.GetSize();i++)
	{
		CSmallWindow* pSW = m_SmallWindows.GetAt(i);
		if (pSW->IsDisplayWindow())
		{
			if (wServerID != m_SmallWindows.GetAt(i)->GetServerID())
			{
				return m_SmallWindows.GetAt(i)->GetServerID();
			}
		}
	}
	return (WORD)(-1);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::Sequence()
{
	if (theApp.m_bTraceSequence)
	{
		WK_TRACE(_T("START SEQUENCE %d\n"),m_dwTimerCounter);
	}

	BOOL bSwitched = FALSE;

	m_SmallWindows.Lock();

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
			CSmallWindow* pSW = m_SmallWindows.GetAt(i);
			
			if (   pSW->IsDisplayWindow()
				&& pSW->GetServerID() == wCurrentServer
				)
			{
				CDisplayWindow* pDW = (CDisplayWindow*)pSW;
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
						CDisplayWindow* pNextDW = GetNextSequenceDisplayWindow(wServerID,pDW);
						if (pNextDW != pDW)
						{
							bSwitched = TRUE;
							
							if (theApp.m_bTraceSequence)
							{
								WK_TRACE(_T("SEQUENCE %s off\n"),pDW->GetName());
							}
							
							if (   pDW->SetActiveRequested(FALSE)
								&& WK_IS_WINDOW(pNextDW)
								)
							{
								if (theApp.m_bTraceSequence)
								{
									WK_TRACE(_T("SEQUENCE %s on\n"),pNextDW->GetName());
								}
								pNextDW->SetActiveRequested();
								if (m_iXWin == 1)
								{
									m_pCmdActiveSmallWindow = pNextDW;
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
		WK_TRACE(_T("END SEQUENCE %d\n"),m_dwTimerCounter);
	m_SmallWindows.Unlock();

	if (!bSwitched)
	{
		GetDocument()->SetSequence(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_uTimerID)
	{
		m_dwTimerCounter++;
		if (GetDocument()->GetSequence())
		{
			if (((m_dwTimerCounter-1)%GetDocument()->GetSequenceIntervall()==0))
			{
				Sequence();
			}
		}
		else 
		{
			if ((m_dwTimerCounter%10==0))
			{
				CheckSmallWindows();
			}
		}

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
void CVisionView::CheckSmallWindows() 
{
	CSmallWindow* pSW = NULL;
	DWORD dwCurrentTime = GetTickCount();
	// called from timer in Mainthread
	// Smallwindows will be only created or
	// deleted in main thread so don't lock here
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			((CDisplayWindow*)pSW)->CheckVideo(dwCurrentTime);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::OnDestroy() 
{
	if (m_uTimerID)
	{
		KillTimer(m_uTimerID);
	}

	CView::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
int CVisionView::GetRequestedFPS(WORD wServer)
{
	int r = 0;
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		CSmallWindow* pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			if (   pDW->GetServerID() == wServer
				|| wServer == 0)
			{
				r += pDW->GetRequestedFPS();
			}
		}
	}
	return r;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionView::Cancel1PlusMode()
{
	m_bView1Plus = FALSE;
	m_pBigSmallWindow = NULL;
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnViewMonitor() 
{
	CRect       rc;
	if (!m_bView1Plus)
	{
		SetAppropriateQuadViewMode();
	}
	SetSmallWindowPositions(m_SmallWindows);
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnViewTitleOfSmallwindows() 
{
	theApp.m_bShowTitleOfSmallWindows = !theApp.m_bShowTitleOfSmallWindows;
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		CSmallWindow* pSW = m_SmallWindows.GetAt(i);
		pSW->EnableTitle(theApp.m_bShowTitleOfSmallWindows);
	}		
//	SetWindowSize(FALSE);
}
//////////////////////////////////////////////////////////////////////////

void CVisionView::OnUpdateViewTitleOfSmallwindows(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(theApp.m_bShowTitleOfSmallWindows);
}
//////////////////////////////////////////////////////////////////////////
BOOL CVisionView::IsView1x1Allowed()
{
	BOOL bEnable = FALSE;
	CMainFrame *pFrame = ((CMainFrame*)GetParentFrame());
	if (WK_IS_WINDOW(pFrame))
	{
		int nExtFrames = pFrame->GetNoOfExternalFrames();
		if (nExtFrames)
		{
			int iRequesting = GetNrOfRequestingWindows();
			int iNonDisplay = m_SmallWindows.GetSize() - GetNrOfDisplayWindows();
			int nWnd = (iRequesting + iNonDisplay) / (nExtFrames+1);
			bEnable  = (nWnd <= 2);
		}
		else
		{
//			bEnable =  (GetNrOfRequestingWindows()<=1) ? TRUE : FALSE;
			bEnable =  (GetCmdActiveSmallWindow() != NULL);
		}
	}
	return bEnable;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _HTML_ELEMENTS
void CVisionView::OnEditHtmlPage() 
{
	if (CanEditHTMLWindow())
	{
		BOOL bEdit = ((CHtmlWindow*)m_pCmdActiveSmallWindow)->OnEditHtmlPage();
		if (bEdit)
		{
			SetBigSmallWindow(m_pCmdActiveSmallWindow);
			OnView1();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateEditHtmlPage(CCmdUI* pCmdUI) 
{
	if (CanEditHTMLWindow())
	{
		((CHtmlWindow*)m_pCmdActiveSmallWindow)->OnUpdateEditHtmlPage(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnEditClear() 
{
	if (CanEditHTMLWindow())
	{
		((CHtmlWindow*)m_pCmdActiveSmallWindow)->OnEditClear();
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	if (CanEditHTMLWindow())
	{
		((CHtmlWindow*)m_pCmdActiveSmallWindow)->OnUpdateEditClear(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnEditCreateHtmlCameraPage() 
{
	CObjectView*pOV = GetDocument()->GetObjectView();
	if (pOV)
	{
		CHTMLmapProperties dlg;
		dlg.m_sPath.Format(_T("%s\\%08lx\\"),CNotificationMessage::GetWWWRoot(), CSecID(SECID_LOCAL_HOST));
		dlg.m_sTitle.LoadString(IDS_CAMERAS);
		if (dlg.DoModal() == IDOK)
		{
			CString sCameraHTM = _T("camera.htm");
			dlg.CheckCopyPictureFile();
			dlg.m_sPath += sCameraHTM;
			CHtmlWindow wnd(theApp.GetNextHtmlWindowID(), dlg.m_sPath, dlg.m_sTitle, dlg.m_sPictureFileName, dlg.m_nZoomValue, HTML_TYPE_CAMERA_MAP);
			wnd.OnFileSaveHtmlText(dlg.m_szPicture);
			CHtmlWindow*pHW = AddHtmlWindow(GetDocument()->GetLocalServer(), theApp.GetNextHtmlWindowID(), sCameraHTM);
			if (pHW)
			{
				SetCmdActiveSmallWindow(pHW);
				OnEditHtmlPage();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateEditCreateHtmlCameraPage(CCmdUI* pCmdUI) 
{
	if (   theApp.m_pPermission 
		 && theApp.m_pPermission->IsSuperVisor())
	{
		if (   m_pCmdActiveSmallWindow 
			 && m_pCmdActiveSmallWindow->IsHtmlWindow()
			 && ((CHtmlWindow*)m_pCmdActiveSmallWindow)->IsEditing())
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CObjectView*pOV = GetDocument()->GetObjectView();
			if (pOV)
			{
				if (   pOV->GetLocalServerID() != SECID_NO_SUBID
					 && pOV->GetLocalServerID() == pOV->GetSelectedServer())
				{
					CString sFile;
					sFile.Format(_T("%s\\8000fffe\\camera.htm"),CNotificationMessage::GetWWWRoot());
					pCmdUI->Enable(!DoesFileExist(sFile));
				}
				else
				{
					pCmdUI->Enable(FALSE);
				}
			}
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnEditCreateHtmlHostPage() 
{
	CObjectView*pOV = GetDocument()->GetObjectView();
	if (pOV)
	{
		CHTMLmapProperties dlg;
		dlg.m_sPath.Format(_T("%s\\"),CNotificationMessage::GetWWWRoot());
		dlg.m_sTitle.LoadString(IDS_HOSTS);
		if (dlg.DoModal() == IDOK)
		{
			dlg.CheckCopyPictureFile();
			dlg.m_sPath += _T("map.htm");
			CHtmlWindow wnd(theApp.GetNextHtmlWindowID(), dlg.m_sPath, dlg.m_sTitle, dlg.m_sPictureFileName, dlg.m_nZoomValue, HTML_TYPE_HOST_MAP);
			wnd.OnFileSaveHtmlText(dlg.m_szPicture);
			theApp.CheckHostMap();
			CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
			pFrame->UpdateToolbar();
			ShowMapWindow();
			if (m_pHostMapWindow)
			{
				SetCmdActiveSmallWindow(m_pHostMapWindow);
			}
			OnEditHtmlPage();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateEditCreateHtmlHostPage(CCmdUI* pCmdUI) 
{
	if (   theApp.m_pPermission 
		 && theApp.m_pPermission->IsSuperVisor())
	{
		if (   m_pCmdActiveSmallWindow 
			 && m_pCmdActiveSmallWindow->IsHtmlWindow()
			 && ((CHtmlWindow*)m_pCmdActiveSmallWindow)->IsEditing())
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
void CVisionView::OnEditHtmlMapProperties() 
{
	if (CanEditHTMLWindow())
	{
		((CHtmlWindow*)m_pCmdActiveSmallWindow)->OnEditHtmlMapProperties();
	}
}
//////////////////////////////////////////////////////////////////////////
void CVisionView::OnUpdateEditHtmlMapProperties(CCmdUI* pCmdUI) 
{
	if (CanEditHTMLWindow())
	{
		((CHtmlWindow*)m_pCmdActiveSmallWindow)->OnUpdateEditHtmlMapProperties(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
/*
void CVisionView::OnUpdateEditCreateHtmlText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_pPermission && theApp.m_pPermission->IsSuperVisor());
}
*/
//////////////////////////////////////////////////////////////////////////
BOOL CVisionView::CanEditHTMLWindow()
{
	if (   theApp.m_pPermission 
		 && theApp.m_pPermission->IsSuperVisor()
		 && m_pCmdActiveSmallWindow
		 && m_pCmdActiveSmallWindow->IsHtmlWindow())
	{
		return TRUE;
	}
	return FALSE;
}
// CAVEAT: end of #ifdef _HTML_ELEMENTS
#endif
//////////////////////////////////////////////////////////////////////////
