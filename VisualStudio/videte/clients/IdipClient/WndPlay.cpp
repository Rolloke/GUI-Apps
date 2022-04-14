// WndPlay.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "WndPlay.h"

#include "Mainfrm.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"

#include "ViewArchive.h"
#include "CIPCDatabaseIdipClient.h"
#include "DlgComment.h"
#include "CIPCAudioIdipClient.h"
#include "Server.h"
#include "DlgNavigation.h"
#include "DlgField.h"

#include "wndhtml.h"
#include ".\wndplay.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWndPlay, CWndImageRecherche)
/////////////////////////////////////////////////////////////////////////////
CWndPlay::CWndPlay(CServer* pServer, const CIPCSequenceRecord& s)
	: CWndImageRecherche(pServer) , m_SequenceRecord(s)
{
	ASSERT(pServer != NULL);
	Init(pServer);
	m_sArchivName = pServer->GetArchiveName(m_SequenceRecord.GetArchiveNr());
	m_dwCurrentRecord = 0;
	m_dwRequestedRecord = 0;
	m_nType = WST_PLAY;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::Init(CServer* pServer)
{
	m_bIsInRectQuery = FALSE;
	m_iRedrawRectWithText = 0;
	m_pQueryRectTracker = NULL;
	m_pServer = pServer;
	m_dwCurrentRecord = 0;
	m_dwRequestedRecord = 0;
	m_pViewIdipClient = NULL;
	m_pPictureRecord = NULL;
	m_pAudioRecord = NULL;
	m_bWasSomeTimesActive = FALSE;
	m_sError.Empty();
	m_bCorrectRequestRecordNr = FALSE;
	m_bNavigateLast  = FALSE;
	m_bUseOneWindow  = FALSE;
	m_bSetSyncPoint  = FALSE;
	m_bGetPicture    = FALSE;
	m_bContainsAudio = FALSE;
	m_bContainsVideo = FALSE;
	m_bAudioBusy     = FALSE;
	m_bReinitDecoder = FALSE;

	m_PlayStatus     = PS_STOP;
	m_RectPlayStatus = PS_RECT_STOP;

	m_iPictureRequests = 0;

	// m_DibDatas
	m_nDibData = 1;
	m_pDibDataTooltip = NULL;

	// for rectangle search

	//init for rectangle search
	m_bCanRectPlayForward = TRUE;
	m_bCanRectPlayBack	  = TRUE;
	m_wMD_X = 0;
	m_wMD_Y = 0;
	if(m_pServer->CanRectangleQuery())
	{
		m_pQueryRectTracker	= new CQueryRectTracker(this);
	}
	else
	{
		m_pQueryRectTracker = NULL;
	}

	// zoomcounter, max. 3
	m_iCountZooms = 0;

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
			m_pMpeg4Decoder = new CMPEG4Decoder;
			WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
		}
	}
	else
	{
		m_pMpeg4Decoder = new CMPEG4Decoder;
		WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
	}

	m_pJpeg			= new CJpeg;
	m_pH263			= new CH263;
	m_pPTDecoder	= new CPTDecoder(TRUE);

	m_rcZoom = CRect(0,0,0,0); // Zoom erstmal aus!
	if (theApp.GetCPUClockFrequency() >= 1000)
	{
		m_pJpeg->EnableNoiseReduction();
	}
	else
	{
		m_pJpeg->DisableNoiseReduction();
	}

	//Inits for realtime playback
	m_pPlayRealTime	= new CPlayRealTime();
}
/////////////////////////////////////////////////////////////////////////////
CWndPlay::~CWndPlay()
{
	CServer *pServer = GetSafeServer();
	if (pServer)
	{
		CIPCAudioIdipClient* pAudio = pServer->GetDocument()->GetLocalAudio();
		if (pAudio)
		{
			if (pAudio->GetWndPlayActive() == this)
			{
				pAudio->DoRequestStopMediaDecoding();
				pAudio->SetWndPlayActive(NULL, SECID_NO_ID);
			}
		}
	}
	m_DibDatas.DeleteAll();

	{ // critical section scope
		CAutoCritSec acs(&m_csPictureRecord);
		WK_DELETE(m_pPictureRecord);
		WK_DELETE(m_pAudioRecord);
		WK_DELETE(m_pPlayRealTime);
	}

	if(m_pQueryRectTracker)
	{
		CRect rc(0,0,0,0);
		theApp.GetQueryParameter().SetRectToQuery(rc);
		WK_DELETE(m_pQueryRectTracker)
	}
}
/////////////////////////////////////////////////////////////////////////////
CSecID	CWndPlay::GetID()
{
	return CSecID(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr());
}
/////////////////////////////////////////////////////////////////////////////
int	CWndPlay::GetNrDibs()
{
	return m_nDibData;
}
/////////////////////////////////////////////////////////////////////////////
CString	CWndPlay::GetName()
{
	CString s = m_SequenceRecord.GetName();
	if (s.IsEmpty())
	{
		s = m_SequenceRecord.GetTime().GetDateTime();
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CSize CWndPlay::GetPictureSize()
{
	CSize s;
	s.cx = s.cy = 0;

	CAutoCritSec cs(&m_csPictureRecord);
	if (m_pPictureRecord)
	{
		switch (m_pPictureRecord->GetCompressionType())
		{
		case COMPRESSION_PRESENCE:
			if (GetPTFrameSize(m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength(), s))
			{
				break;
			}
			break;
		case COMPRESSION_JPEG:
			if (m_pJpeg->IsValid())
			{
				s = m_pJpeg->GetImageDims();
			}
			else if (GetJpegSize((LPCSTR)m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength(), s))
			{
				if (m_pPictureRecord->GetResolution() == RESOLUTION_QCIF)
				{
					s.cy /= 2;
				}
			}
			break;
		case COMPRESSION_MPEG4:
			s = m_pMpeg4Decoder->GetImageDims();
			break;
		case COMPRESSION_H263: 
			s = m_pH263->GetImageDims();
			break;
		default:
			break;
		}
	}

	return s;
}
///////////////////////////////////////////////////////////////////
void CWndPlay::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1 ? true : false;
	m_csPictureRecord.Lock();
	if (   m_pPictureRecord 
		&& m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
	{
		m_pH263->SetCaps(H263_FIT,!m_b1to1);
	}
	m_csPictureRecord.Unlock();
	RedrawWindow();
}

BEGIN_MESSAGE_MAP(CWndPlay, CWndImageRecherche)
	//{{AFX_MSG_MAP(CWndPlay)
	ON_WM_CREATE()
	ON_MESSAGE(WM_CHANGE_VIDEO, OnChangeVideo)
	ON_MESSAGE(WM_CHANGE_AUDIO, OnChangeAudio)
	ON_MESSAGE(WM_REDRAW_VIDEO, OnRedrawVideo)
	ON_MESSAGE(WM_UPDATE_DIALOGS, OnUpdateDialogs)
	ON_WM_DESTROY()
	ON_COMMAND(ID_NAVIGATE_POS1, OnNavigatePos1)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_POS1, OnUpdateNavigatePos1)
	ON_COMMAND(ID_NAVIGATE_END, OnNavigateEnd)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_END, OnUpdateNavigateEnd)
	ON_COMMAND(ID_NAVIGATE_NEXT, OnNavigateNext)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_NEXT, OnUpdateNavigateNext)
	ON_COMMAND(ID_NAVIGATE_PREV, OnNavigatePrev)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_PREV, OnUpdateNavigatePrev)
	ON_COMMAND(ID_NAVIGATE_FORWARD, OnNavigateForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FORWARD, OnUpdateNavigateForward)
	ON_COMMAND(ID_NAVIGATE_BACK, OnNavigateReview)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_BACK, OnUpdateNavigateReview)
	ON_COMMAND(ID_NAVIGATE_STOP, OnNavigateStop)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_STOP, OnUpdateNavigateStop)
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORIGINAL, OnUpdateViewOriginal)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_DIB, OnFileDib)
	ON_COMMAND(ID_FILE_HTML, OnFileHtml)
	ON_COMMAND(ID_FILE_JPG, OnFileJpg)
	ON_COMMAND(ID_DISPLAY_1, OnDisplay1)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_1, OnUpdateDisplay1)
	ON_COMMAND(ID_DISPLAY_4, OnDisplay4)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_4, OnUpdateDisplay4)
	ON_COMMAND(ID_DISPLAY_9, OnDisplay9)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_9, OnUpdateDisplay9)
	ON_COMMAND(ID_EDIT_COMMENT, OnEditComment)
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_FILE_DIB, OnUpdateFileDib)
	ON_UPDATE_COMMAND_UI(ID_FILE_HTML, OnUpdateFileHtml)
	ON_UPDATE_COMMAND_UI(ID_FILE_JPG, OnUpdateFileJpg)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_ZOOM_OUT, OnDisplayZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateDisplayZoomOut)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_NAVIGATE_SKIP_FORWARD, OnNavigateSkipForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_SKIP_FORWARD, OnUpdateNavigateSkipForward)
	ON_COMMAND(ID_NAVIGATE_SKIP_BACK, OnNavigateSkipBack)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_SKIP_BACK, OnUpdateNavigateSkipBack)
	ON_COMMAND(ID_NAVIGATE_FAST_BACK, OnNavigateFastBack)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FAST_BACK, OnUpdateNavigateFastBack)
	ON_COMMAND(ID_NAVIGATE_FAST_FORWARD, OnNavigateFastForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FAST_FORWARD, OnUpdateNavigateFastForward)
	ON_COMMAND(ID_SMALL_CONTEXT, OnSmallContext)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CONTEXT, OnUpdateSmallContext)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER,OnUser)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
	ON_MESSAGE(WM_DECODE_VIDEO, OnDecodeVideo)
	ON_MESSAGE(WM_PLAY_REALTIME, OnPlayRealTime)
	ON_WM_RBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COMMENT, OnUpdateEditComment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndPlay message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::Create(const RECT& rect, CViewIdipClient* pParentWnd)
{
	m_pViewIdipClient = pParentWnd;
	m_b1to1 = m_pViewIdipClient->m_b1to1 ? true : false;

	return CWnd::Create(SMALL_WINDOW_CLASS, NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						GetID().GetID());

}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::PreTranslateMessage(MSG* pMsg)
{
	//This function was checked against Recherche by TKR, 14.04.2004
	if (::IsWindow(m_ToolTip.m_hWnd) && pMsg->hwnd == m_hWnd)
	{
		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:	
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:	
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:	
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			m_ToolTip.RelayEvent(pMsg);
			break;
		}
	}
	return CWndSmall::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;
	CSize size;
	CRect rect;

	GetClientRect(rect);

	size = pDC->GetOutputTextExtent(m_sArchivName);

	if (size.cx < (rect.Width()-m_wndToolBar.GetSize().cx))
	{
		sTitle = m_sArchivName + _T(" ") + GetServerName();
	}

	size = pDC->GetOutputTextExtent(sTitle);
	if (size.cx < (rect.Width()-m_wndToolBar.GetSize().cx))
	{
		CString s;
		s = GetName();
		if (s.IsEmpty())
		{
			s.LoadString(IDS_WAIT_FOR_IMAGE);
		}
		sTitle += _T(": ") + s;
	}

	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::PrintDIBs(CDC* pDC,CRect rect,int iSpace)
{
	CRect r,ir;
	int w,h,ph;
	CDibData* pDibData;
	int i,c,iS;
	CFont* pFontOld;

	c = m_DibDatas.GetSize();
	w = rect.Width() / m_nDibData + 1;
	h = rect.Height() / m_nDibData + 1;
	ph = (w * 3) / 4;
	iS = iSpace / m_nDibData;

	CFont font;
	font.CreatePointFont(DW_INFO_FONT_SIZE/m_nDibData, GetVarFontFaceName(),pDC);
	pFontOld = pDC->SelectObject(&font);

	for (i=0;i<m_nDibData*m_nDibData;i++)
	{
		r.left = rect.left + (i-(i/m_nDibData)*m_nDibData) * w;
		r.top = rect.top + (i/m_nDibData-(i/(m_nDibData*m_nDibData))*m_nDibData) * h;
		r.right = r.left + w - iS;
		r.bottom = r.top + ph;
		ir = r;
		ir.top = r.bottom + iS;
		ir.bottom = r.top + h - iS;
		if (i<c)
		{
			CString sp,s,f;
			pDibData = m_DibDatas.GetAtFast(i);
			pDibData->GetDib()->OnDraw(pDC,r);
			f.LoadString(IDS_PICTURE_NR);
			s.Format(_T("%s:\t%d\n"),f,pDibData->GetPicNr());
			sp = s + pDibData->GetPicture(); 
			PrintInfoRectPortrat(pDC,ir,&font,iS,
				sp,pDibData->GetData(),pDibData->GetComment());
		}
	}
	pDC->SelectObject(pFontOld);
	font.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
int CWndPlay::PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim /*= FALSE*/)
{
	int ret = 0;
	if (m_pPictureRecord)
	{
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}
			//TRACE(_T(" ### jpeg pDC: 0x%x  Rect: t: %d  b: %d  l: %d  r: %d  zt: %d  zb: %d  zl: %d  zr: %d\n"),
			//	pDC, rect.top, rect.bottom, rect.left, rect.right,
			//	m_rcZoom.top, m_rcZoom.bottom, m_rcZoom.left, m_rcZoom.right);
			m_pJpeg->OnDraw(pDC, rect, m_rcZoom);
			ret = rect.Height();
		}
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}

			m_pMpeg4Decoder->OnDraw(pDC, rect, m_rcZoom);

			ret = rect.Height();
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 11;
			}
			TRACE(_T("Print H.263 Rect=%d,%d,%d,%d Zoom=%d,%d,%d,%d\n"),
				rect.left,rect.top,rect.right,rect.bottom,
				m_rcZoom.left,m_rcZoom.top,m_rcZoom.right,m_rcZoom.bottom);
			m_pH263->OnDraw(pDC,rect, m_rcZoom);
			ret = rect.Height();
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}
			CDib dib(m_pPTDecoder->GetBitmapHandle());
			dib.OnDraw(pDC,rect, m_rcZoom);
			ret = rect.Height();
		}
#endif
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::ResetTracker()
{
	//This function was checked against Recherche by TKR, 14.04.2004
	WK_DELETE(m_pQueryRectTracker);
	m_pQueryRectTracker = new CQueryRectTracker(this);
	EnableNavigationSlider(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDraw(CDC* pDC)
{
	if (m_nDibData == 1)
	{
		if (m_bWasSomeTimesActive)
		{
			CRect rect;
			GetVideoClientRect(rect);

			CompressionType ct = COMPRESSION_UNKNOWN;

			m_csPictureRecord.Lock();
			if (m_pPictureRecord)
			{
				ct = m_pPictureRecord->GetCompressionType();
			}
			m_csPictureRecord.Unlock();
			BOOL bPrinting = pDC->m_bPrinting;
			if (m_rcZoom.bottom)
			{	// RKE: beim Drucken wird interpoliert.
				pDC->m_bPrinting = TRUE;
			}
			if (ct==COMPRESSION_JPEG)
			{
				m_pJpeg->OnDraw(pDC, rect, m_rcZoom);
			}
			else if (ct==COMPRESSION_MPEG4)
			{
				m_wMD_X = (WORD)m_pMpeg4Decoder->GetX();
				m_wMD_Y = (WORD)m_pMpeg4Decoder->GetY();
				BOOL bSuccess = m_pMpeg4Decoder->OnDraw(pDC, rect, m_rcZoom);

				if(bSuccess)
				{
					m_bReinitDecoder = FALSE;
				}
				else
				{
					if (m_bReinitDecoder < 3)
					{
						WK_TRACE(_T("#### Reinit MPeg (%d,%d,%d,%d) (%d,%d,%d,%d) (%d,%d): %d\n"),
							rect.left, rect.top, rect.right, rect.bottom,
							m_rcZoom.left, m_rcZoom.top, m_rcZoom.right, m_rcZoom.bottom,
							m_wMD_X, m_wMD_Y, GetLastError());
						m_csPictureRecord.Lock();
						m_pMpeg4Decoder->Decode(*m_pPictureRecord,0,0);
						m_csPictureRecord.Unlock();
						Sleep(100);
						PostMessage(WM_USER, DW_INVALIDATE_RECT);
					}
					m_bReinitDecoder++;
				}
			}
			else if (ct==COMPRESSION_H263)
			{
				m_pH263->OnDraw(pDC,rect, m_rcZoom);
			}
#ifndef _DTS
			else if (ct==COMPRESSION_PRESENCE)
			{
				m_pPTDecoder->OnDraw(pDC->GetSafeHdc(),rect, m_rcZoom);
			}
#endif
			pDC->m_bPrinting = bPrinting;
			if (!m_pDlgField->IsWindowVisible())
			{
				pDC->SaveDC();
				CString str = _T("® >>");
				CFont font;
				font.CreatePointFont(10*10, GetVarFontFaceName(), pDC);
				pDC->SelectObject(&font);
				pDC->SetTextColor(SKIN_COLOR_RED);
				//pDC->SetBkMode(TRANSPARENT);
				pDC->TextOut(rect.left, rect.top, str);
				pDC->RestoreDC(-1);
			}

			if (   m_pServer->CanRectangleQuery()
				&& m_pPictureRecord)
			{
				// Im Wiedergabefenster mehrere Kreuze zeichnen
				DoDrawAllCrosses(pDC);

				if (m_pQueryRectTracker && !m_pQueryRectTracker->GetRect()->IsRectEmpty())
				{
					DrawRectToQuery(pDC);
					//TRACE(_T("#### TKR OnDraw() WithoutText: %d\n"),m_iRedrawRectWithText);
					if (IsInRectQuery())
					{
						if(m_iRedrawRectWithText > 0)
						{
							pDC->SaveDC();
							if(CSkinDialog::UseGlobalFonts())
							{
								pDC->SelectObject(CSkinDialog::GetDialogItemGlobalFont());
							}

							CString sQuery;
							sQuery.LoadString(IDS_RECTSEARCH);
							//changes for VisualStudio 2005
							int i = 0;
							for(i = 1; i <= m_iRedrawRectWithText; i++)
							{
								sQuery = sQuery + ".";
								pDC->DrawText(sQuery,&rect,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
							}
							
							if(m_iRedrawRectWithText == RECTSEARCHENDREACHED)
							{
								CString sEnd;
								sQuery.LoadString(IDS_RECTSEARCH);
								sEnd.LoadString(IDS_RECTSEARCHEND);
								if(!m_pDlgField->IsWindowVisible())
								{
									//zu kleine Ansicht, Anzeige mit Zeilenumbruch
									sQuery = sQuery + _T("...\r\n") + sEnd;
									pDC->DrawText(sQuery,&rect,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
								}
								else
								{
									sQuery = sQuery + _T("... ") + sEnd;
									pDC->DrawText(sQuery,&rect,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
														}
								KillQueryRectTimer();
							}
							pDC->RestoreDC(-1);
						}
					}
				}
			}
			else
			{
				//kein Rechteck vorhanden, nur ein Kreuz zeichnen
				DrawCross(pDC,m_wMD_X,m_wMD_Y, &rect);
			}

			DrawCinema(pDC);
			DrawTitle(pDC);
		}
		else
		{
			DrawFace(pDC);
			DrawTitle(pDC);
		}
	}
	else
	{
		CRect rect;
		
		//ist aktuelle Ansicht _T("4 Bilder") oder _T("9 Bilder") wird ein evtl. vorhandenes Rechteck
		//gelöscht
		ResetTracker();
		
		//War der Suchdialog geöffnet, diesen bezüglich des Rechtecks aktualisieren 
		if(m_pViewIdipClient)
		{
			m_pViewIdipClient->ShowHideRectangle();
		}
						
		//Setzt in CQueryParameter die Rechteckparameter wieder auf 0
		theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());
		
		GetVideoClientRect(rect);
		DrawCinema(pDC, &rect);
//		TRACE(_T("GetVideoClientRect %d,%d,%d,%d"),rect.left,rect.top,rect.Width(),rect.Height());
		OnDrawDIBs(pDC,rect);
		DrawTitle(pDC);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::DrawCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col)
{
	// Den Zoom berücksichtigen.
	if (!(m_rcZoom.IsRectEmpty()))
	{
		int x,y;
		CSize ImageSize(GetPictureSize());
		x = (wX * ImageSize.cx) / 1000 - m_rcZoom.left;
		y = (wY * ImageSize.cy) / 1000 - m_rcZoom.top;

		wX = (WORD)(1000 * x / m_rcZoom.Width());
		wY = (WORD)(1000 * y / m_rcZoom.Height());
	}

	CRect rect;
	GetVideoClientRect(rect);
	int x,y;
	x = (wX * rect.Width()) / 1000 + rect.left;
	y = (wY * rect.Height()) / 1000 + rect.top;
	CPen pen;
	CPen* pOldPen;

	pen.CreatePen(PS_SOLID,1,col);

	pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rect.left,y);
	pDC->LineTo(rect.right,y);

	pDC->MoveTo(x,rect.top);
	pDC->LineTo(x,rect.bottom);

	int r = 4;

	pDC->Arc(x-r,y-r,x+r+1,y+r+1,x-r,y,x-r,y);

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::DrawAllCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col,BOOL bLines /* = FALSE */)
{
	// Ist das Fadenkreuz eingeschaltet UND sind gültige Koordinaten vorhanden?
	if ((wX > 0)	&& (wY > 0))
	{
		// Den Zoom berücksichtigen.
		if (!(m_rcZoom.IsRectEmpty()))
		{
			int x,y;
			CSize ImageSize = (GetPictureSize());
			x = (wX * ImageSize.cx) / 1000 - m_rcZoom.left;
			y = (wY * ImageSize.cy) / 1000 - m_rcZoom.top;
			
			wX = (WORD)(1000 * x / m_rcZoom.Width());
			wY = (WORD)(1000 * y / m_rcZoom.Height());
		}
		
		CRect rect;
		GetVideoClientRect(rect);
		int x,y;
		x = (wX * rect.Width()) / 1000 + rect.left;
		y = (wY * rect.Height()) / 1000 + rect.top;
		CPen pen;
		CPen* pOldPen;
		
		pen.CreatePen(PS_SOLID,1,col);
		pOldPen = pDC->SelectObject(&pen);
		int r = 4;
		pDC->Arc(x-r,y-r,x+r+1,y+r+1,x-r,y,x-r,y);
		
		if(bLines)
		{
			pDC->MoveTo(rect.left,y);
			pDC->LineTo(rect.right,y);
		}
		else
		{	
			pDC->MoveTo(x-r,y-r);
			pDC->LineTo(x+r+1,y+r+1);
		}
		
		if(bLines)
		{
			pDC->MoveTo(x,rect.top);
			pDC->LineTo(x,rect.bottom);
		}
		else
		{
			pDC->MoveTo(x+r,y-r);
			pDC->LineTo(x-r,y+r);
		}
		
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CWndPlay::DoDrawAllCrosses(CDC* pDC)
{
	CAutoCritSec acs(&m_csPictureRecord);
	int iSize = m_waAllMD_X.GetSize();
	if(iSize > 0)
	{
		if(m_pQueryRectTracker && !m_pQueryRectTracker->GetRect()->IsRectEmpty())
		{
			CRect rc = m_pQueryRectTracker->GetRectPromilleImage();
			rc.NormalizeRect();
			
			if(AllMDsOutsideRect(iSize, rc))
			{
				//alle Kreuze außerhalb des Rechtecks, nur das Hauptkreuz weiss zeichen
				DrawAllCrosses(pDC,m_waAllMD_X.GetAt(0),m_waAllMD_Y.GetAt(0), RGB(255, 255, 255), TRUE);
			}
			else
			{
				BOOL bMaxMDInRect = TRUE;
				//changes for VisualStudio 2005
				int i = 0;
				for(i=0; i<iSize; i++)
				{
					CPoint point((int)m_waAllMD_X.GetAt(i), (int)m_waAllMD_Y.GetAt(i)); 
					if ((point.x > 0) && (point.y > 0))
					{
						//bei Rechteck alle Kreuze farbig abgestuft anzeigen und nur das der
						//maximalen Bewegung innerhalb des Rechtecks als Fadenkreuz zeichnen

						if(rc.PtInRect(point))
						{
							//zeichne nur das Kreuz der stärksten Bewegung als Fadenkreuz 
							if(bMaxMDInRect)
							{
								DrawAllCrosses(pDC, (WORD)point.x, (WORD)point.y, RGB(255, 255, 255), TRUE);
								bMaxMDInRect = FALSE;
							}
							else
							{
								DrawAllCrosses(pDC, (WORD)point.x, (WORD)point.y, RGB(255, (i+1)*40, (i+1)*40));
							}
						}
						else
						{
							DrawAllCrosses(pDC, (WORD)point.x, (WORD)point.y, RGB(255, (i+1)*40, (i+1)*40));
						}
					}
				}
			}
		}
		else
		{
			// kein Rechteck da, nur das Hauptkreuz weiss zeichen
			if (theApp.m_bTargetDisplay
				&& m_waAllMD_X.GetSize()>0
				&& m_waAllMD_Y.GetSize()>0)
			{
				CPoint point((int)m_waAllMD_X.GetAt(0), (int)m_waAllMD_Y.GetAt(0)); 
				if ((point.x > 0) && (point.y > 0))
				{
					DrawAllCrosses(pDC, (WORD)point.x, (WORD)point.y, RGB(255, 255, 255), TRUE);
				}
			}
		}
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDrawDIBs(CDC* pDC, CRect rect)
{
	CRect r;
	int w,h;
	CDibData* pDibData;
	int i,c;

	c = m_DibDatas.GetSize();
	w = rect.Width() / m_nDibData;
	h = rect.Height() / m_nDibData;


	for (i=0;i<m_nDibData*m_nDibData;i++)
	{
		r.left = rect.left + (i-(i/m_nDibData)*m_nDibData) * w;
		r.top = rect.top + (i/m_nDibData-(i/(m_nDibData*m_nDibData))*m_nDibData) * h;
		r.right = r.left + w;
		r.bottom = r.top + h;
		if (i<c)
		{
			pDibData = m_DibDatas.GetAtFast(i);
			pDibData->GetDib()->OnDraw(pDC,r);
			CString sn;
			sn.Format(_T("%d"),pDibData->GetPicNr());
			if (m_DibDatas.GetCurrentPicNr()==pDibData->GetPicNr())
			{
				//if a records number is twice on screen
				//only change color of the correct picture in line 
				if(m_DibDatas.GetCurrentSeqNr()==pDibData->GetSeqNr())
				{
					CBrush b;
					b.CreateSolidBrush(RGB(255,0,0));
					pDC->FrameRect(r,&b);
					b.DeleteObject();
					COLORREF old = pDC->SetBkColor(RGB(255,0,0));
					pDC->TextOut(r.left,r.top,sn);
					pDC->SetBkColor(old);
				}
				else
				{
					pDC->FrameRect(r,&m_pViewIdipClient->m_CinemaBrush);

					pDC->TextOut(r.left,r.top,sn);
				}
			}
			else
			{
				pDC->FrameRect(r,&m_pViewIdipClient->m_CinemaBrush);
				pDC->TextOut(r.left,r.top,sn);
			}
		}
		else
		{
			pDC->FillRect(r,&m_pViewIdipClient->m_CinemaBrush);
		}
	}
	{
		CRect fr(rect);
		fr.top = rect.top + m_nDibData*h;
		pDC->FillRect(fr,&m_pViewIdipClient->m_CinemaBrush);
		fr = rect;
		fr.left = rect.left + m_nDibData*w;
		pDC->FillRect(fr,&m_pViewIdipClient->m_CinemaBrush);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::DrawFace(CDC* pDC)
{
	CRect rect;
	GetFrameRect(rect);
	pDC->FillRect(rect,&m_pViewIdipClient->m_CinemaBrush);

	if (!m_sError.IsEmpty())
	{
		CFont font;
		CFont* pOldFont;

		font.CreatePointFont(10*10, GetVarFontFaceName(),pDC);

		pOldFont = pDC->SelectObject(&font);
		int iMode;
		COLORREF oldTextColor;
		iMode = pDC->SetBkMode(TRANSPARENT);
		oldTextColor = pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(m_sError,rect,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
		pDC->SetBkMode(iMode);
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(oldTextColor);
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnIdle()
{
	CheckPlayStatus();
}
////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CanNavigate() const
{
	BOOL bDeliver = TRUE;


	if(    m_PlayStatus == PS_PLAY_BACK
		|| m_PlayStatus == PS_FAST_BACK
		|| m_PlayStatus == PS_FAST_FORWARD
		|| theApp.CanPlayRealtime())
	{
		//playback && fastforward && fastback && PlayRealtime no prefetch
		bDeliver =     (m_iPictureRequests < 1) 
			&& (m_pMpeg4Decoder->GetDecoderQueueLength()<5);		
	}
	//check realtime status on video AND on audio  
	else if (   m_bContainsAudio && !m_bAudioBusy
		|| !theApp.CanPlayRealtime())
	{
		bDeliver =     (m_iPictureRequests < theApp.m_nMaxRequestedRecords) 
					&& (m_pMpeg4Decoder->GetDecoderQueueLength()<5);
	}
	else
	{
		bDeliver =     (m_iPictureRequests < 5) 
					&& (m_pMpeg4Decoder->GetDecoderQueueLength()<5);
	}

	CAutoCritSec acs(&((CWndPlay*)this)->m_csPictureRecord);
	if (m_pAudioRecord)
	{
		CIPCAudioIdipClient* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && pAudio->DoNotSendSamples())
		{
			bDeliver = FALSE;
			TRACE(_T("Rejecting Sample c. o. Overflow\n"));
		}
	}

	return bDeliver;
}
////////////////////////////////////////////////////////////////////////////
void CWndPlay::CheckPlayStatus()
{
	if (m_PlayStatus != PS_PLAY_FORWARD)
	{
		CIPCAudioIdipClient* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && pAudio->GetWndPlayActive() == this)
		{
			pAudio->DoRequestStopMediaDecoding();
			pAudio->SetWndPlayActive(NULL, SECID_NO_ID);
			m_bAudioBusy = FALSE;
		}
	}

	if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
	{
		return;
	}

	if (theApp.GetPerformanceLevel() > INITIAL_PERFORMANCE_LEVEL)
	{
		DWORD dwWait = (theApp.GetPerformanceLevel()-INITIAL_PERFORMANCE_LEVEL)*10;
		Sleep(dwWait);
	}

	CAutoCritSec cs(&m_csPictureRecord);

	switch(m_PlayStatus)
	{
	case PS_PLAY_FORWARD:
		{
			if (IsQueryRectEmpty())
			{
				while (   m_PlayStatus == PS_PLAY_FORWARD		// State may be changed in Navigate(..)
					&& CanNavigate())
				{
					Navigate(m_dwRequestedRecord,1);
				}
			}
			else
			{
				if (CanNavigate())
				{
					Navigate(m_dwRequestedRecord,1);
				}
			}
		}
		break;
	case PS_PLAY_BACK:
		if (CanNavigate())
		{
			Navigate(m_dwRequestedRecord,-1);
		}
		break;
	case PS_FAST_FORWARD:
		if (CanNavigate())
		{
			//TKR nur im 10-Schritt vorwärts springen, wenn es die
			//Gesamtanzahl der Records erlaubt, ansonsten kürzen und das letzte
			//Bild der Sequenz anspringen
			DWORD dwNrOfRecords = m_SequenceRecord.GetNrOfPictures();
			DWORD dwStep = 10;
			if(	   (dwNrOfRecords > m_dwRequestedRecord)
				&& ((dwNrOfRecords - m_dwRequestedRecord) <= dwStep))
			{
				dwStep = dwNrOfRecords - m_dwRequestedRecord;
			}
			Navigate(m_dwRequestedRecord,dwStep);
		}
		break;
	case PS_FAST_BACK:
		if (CanNavigate())
		{
			Navigate(m_dwRequestedRecord,-10);
		}
		break;
	case PS_STOP:
		if (m_iPictureRequests==0)
		{
			if (!m_pViewIdipClient->UseSingleWindow())
			{
				if ((m_dwRequestedRecord==m_SequenceRecord.GetNrOfPictures()) ||
					(m_dwRequestedRecord<2))
				{
					TRACE(_T("stop by last or first record %d\n"),m_dwRequestedRecord);
					SetPlayStatus(PS_STOP);
					m_iPictureRequests = 0;
				}
			}
		}
		break;
	default:
		break;
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndPlay::NoPictureFoundForCamera(CSecID id)
{
	DecreasePictureRequests();

	if (m_pViewIdipClient->UseSingleWindow())
	{
		if (   m_PlayStatus ==	PS_FAST_BACK
			|| m_PlayStatus == PS_SKIP_BACK
			|| m_PlayStatus == PS_PLAY_BACK)
		{
			Navigate(0,-1);
		}
		else if (   m_PlayStatus ==	PS_FAST_FORWARD
				 || m_PlayStatus == PS_SKIP_FORWARD
				 || m_PlayStatus == PS_PLAY_FORWARD)
		{
			Navigate(m_SequenceRecord.GetNrOfPictures()+1,1);
		}
	}
	else
	{
		SetPlayStatus(PS_STOP);
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndPlay::RecordNrOutOfRange(CSecID id)
{
	if (m_PlayStatus !=	PS_STOP)
	{		
		SetPlayStatus(PS_STOP);
	}
	DecreasePictureRequests();
}
////////////////////////////////////////////////////////////////////////////
void CWndPlay::CannotReadPictureForCamera()
{
	if (m_PlayStatus !=	PS_STOP)
	{
		SetPlayStatus(PS_STOP);
	}
	m_bWasSomeTimesActive = FALSE;
	m_sError.LoadString(IDS_READ_ERROR);
	DecreasePictureRequests();
	PostMessage(WM_REDRAW_VIDEO);
}
////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::PictureData(DWORD dwRecordNr,
					 DWORD dwNrOfRecords,
					 const CIPCPictureRecord &pict,
					 const CIPCFields& fields,
					 BOOL  bShowQueryResults)
{
/*	if (pict.GetPictureType()==SPT_FULL_PICTURE)
	{
		TRACE(_T("PictureData %d/%d i frame %08lx\n"),
			dwRecordNr,dwNrOfRecords,pict.GetCamID().GetID());
	}
	else
	{
		TRACE(_T("PictureData %d/%d p frame %08lx\n"),
			dwRecordNr,dwNrOfRecords,pict.GetCamID().GetID());
	}*/

	// attention in database thread
	{	// Critical Section scope
		CAutoCritSec acs(&m_csPictureRecord);
		m_bContainsVideo = TRUE;
	/*
		if (m_PlayStatus == PS_PLAY_FORWARD)
		{
			if (m_pPictureRecord)
			{
				TRACE(_T("%s-%s\n"),pict.GetTimeStamp().GetTime(),m_pPictureRecord->GetTimeStamp().GetTime());
				CSystemTime diff = pict.GetTimeStamp() - m_pPictureRecord->GetTimeStamp();
				DWORD dwDiff = diff.GetMilliseconds()+
							   diff.GetSecond()*1000+
							   diff.GetMilliseconds()*1000*60+
							   diff.GetHour()*1000*60*60;
				TRACE(_T("dwDiff = %d ms\n"),dwDiff);

			}
		}
	*/

		WK_DELETE(m_pPictureRecord);
		WK_DELETE(m_pAudioRecord);

		//bShowQueryResults wird von außen in CIPCDatabaseIdipClient::OnConfirmRecordNr()
		//gesetzt. Ist zur Korrektur (setzen von m_dwRequestedRecord)
		//des gerade angezeigten Bildes nötig
		if (bShowQueryResults)
		{
//			TRACE(_T("bShowQueryResults=%d\n"),bShowQueryResults);
		}

		if (m_bGetPicture || bShowQueryResults)
		{
			m_dwRequestedRecord = dwRecordNr;
		}

		if (m_SequenceRecord.GetNrOfPictures() != dwNrOfRecords)
		{
			m_SequenceRecord.SetNrOfPictures(dwNrOfRecords);
			CIPCSequenceRecord* pSeq = m_pServer->GetCIPCSequenceRecord(m_SequenceRecord.GetArchiveNr(),
											 m_SequenceRecord.GetSequenceNr());
			if (pSeq)
			{
				pSeq->SetNrOfPictures(dwNrOfRecords);
			}

		}
		

		m_dwCurrentRecord = dwRecordNr;
		if (   m_pDlgField
			&& m_pDlgField->GetSelectedCamID() != SECID_NO_ID)
		{
			if (m_dwCurrentRecord>m_dwRequestedRecord)
			{
				m_dwRequestedRecord = m_dwCurrentRecord;
			}
		}
		if (m_bCorrectRequestRecordNr)
		{
			m_dwRequestedRecord = m_dwCurrentRecord;
			m_bCorrectRequestRecordNr = FALSE;
		}
		m_pPictureRecord = new CIPCPictureRecord(pict);
		m_Fields = fields;

#ifdef _DEBUG
		CString sSize;
		sSize.Format(_T("%d Bytes"),m_pPictureRecord->GetDataLength());
		m_Fields.SafeAdd(new CIPCField(_T("Größe"),sSize,_T('C')));
#endif
	} // end of critical section
	DecreasePictureRequests();


	CString sDBFieldX;
	CString sDBFieldY;
	CIPCField *pFX,*pFY;
	DWORD dwX,dwY;
	dwX = dwY = 0;
	{ // Critical section scope
		CAutoCritSec acs(&m_csPictureRecord);
		if (m_pServer->CanRectangleQuery())
		{
			//sichere alle Fadenkreuze des Bildes
			pFX = m_Fields.GetCIPCField(_T("DBD_MD_X"));
			if (pFX == NULL)
			{
				pFX = m_Fields.GetCIPCField(_T("DVD_MD_X"));
				sDBFieldX = _T("DVD_MD_X");
			}
			else
			{
				sDBFieldX = _T("DBD_MD_X");
			}

			pFY = m_Fields.GetCIPCField(_T("DBD_MD_Y"));
			if (pFY == NULL)
			{
				pFY = m_Fields.GetCIPCField(_T("DVD_MD_Y"));
				sDBFieldY = _T("DVD_MD_Y");
			}
			else
			{
				sDBFieldY = _T("DBD_MD_Y");
			}
			
			if (pFX && pFY)
			{
				//setze Fadenkreuze
				CIPCField *pFMoreX,*pFMoreY;
				RemoveAllMDValues();
				//changes for VisualStudio 2005
				int i = 0;
				for(i=0; i<=4; i++)
				{
					CString s, sX, sY;
					sX = sDBFieldX;
					sY = sDBFieldY;
					if(i>0)
					{			
						s.Format(_T("%i"), i+1);
						sX = sDBFieldX + s;
						sY = sDBFieldY + s;
					}
			
					dwX = dwY = 0;

					pFMoreX = m_Fields.GetCIPCField(sX);
					pFMoreY = m_Fields.GetCIPCField(sY);

					if (  pFMoreX
						&& pFMoreY)
					{
						_stscanf(pFMoreX->GetValue(),_T("%04x"),&dwX);
						_stscanf(pFMoreY->GetValue(),_T("%04x"),&dwY);
					}
					if ((dwX>0) && (dwY>0))
					{
						SetAllMDValues((WORD)dwX, (WORD)dwY);
					}
					else
					{
						SetAllMDValues(0, 0);
					}
				}
			}

		}
		else
		{
			//keine Rechtecksuche möglich, da ältere Version
			pFX = m_Fields.GetCIPCField(_T("DBD_MD_X"));
			if (pFX == NULL)
			{
				pFX = m_Fields.GetCIPCField(_T("DVD_MD_X"));
			}
			pFY = m_Fields.GetCIPCField(_T("DBD_MD_Y"));
			if (pFY == NULL)
			{
				pFY = m_Fields.GetCIPCField(_T("DVD_MD_Y"));
			}
			if (pFX && pFY)
			{
				_stscanf(pFX->GetValue(),_T("%04x"),&dwX);
				_stscanf(pFY->GetValue(),_T("%04x"),&dwY);
			}
		}

		if (m_pPictureRecord)
		{
			if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
			{
				m_pH263->Decode(*m_pPictureRecord);
				SetMDValues((WORD)dwX, (WORD)dwY);
			}
			else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4)
			{
				BOOL bDecode = m_pMpeg4Decoder->DecodeMpeg4FromMemory((BYTE*)m_pPictureRecord->GetData(),
					m_pPictureRecord->GetDataLength());

				if(bDecode)
				{
					m_pMpeg4Decoder->SetXY(dwX, dwY);
				}
				PostMessage(WM_DECODE_VIDEO);
			}
			else
			{
				SetMDValues((WORD)dwX, (WORD)dwY);
				PostMessage(WM_CHANGE_VIDEO);
			}
		}
	} // end of critical section

	Sleep(0);
	
	//wenn das mit Rechtecksuche gefundene Bild angeizeigt wird, dann den RectPlayStatus
	//zurücksetzen. Der RectPlayStatus betrifft nur die Einzelschritte vorwärts/rückwärts
	//da die gänzlich ohne PlayStatus bejandelt werden.
	m_RectPlayStatus = PS_RECT_STOP;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::AudioData(DWORD dwRecordNr,
							   DWORD dwNrOfRecords,
							   const CIPCMediaSampleRecord &media,
							   int iNumFields,
							   const CIPCField fields[])
{
	{	// Critical Section Scope
		CAutoCritSec cs(&m_csPictureRecord);

		m_bContainsAudio = TRUE;
		if (m_PlayStatus != PS_PLAY_FORWARD)
		{
			m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		}
		WK_DELETE(m_pAudioRecord);
		//do not delete Picture record, because audio has no pictures to show
	//	WK_DELETE(m_pPictureRecord);
		m_SequenceRecord.SetNrOfPictures(dwNrOfRecords);
		m_dwCurrentRecord = dwRecordNr;
		m_pAudioRecord = new CIPCMediaSampleRecord(media);
		m_Fields.FromArray(iNumFields, fields);
		TRACE(_T("confirm    A=%d, S=%d, CR=%d, BNr=%d\n"),m_SequenceRecord.GetArchiveNr(),
			m_SequenceRecord.GetSequenceNr(),m_dwCurrentRecord,media.GetPackageNumber());
		DecreasePictureRequests();

#ifdef _DEBUG
		CString sSize;
		sSize.Format(_T("%d Bytes"),m_pAudioRecord->GetActualLength());
		m_Fields.SafeAdd(new CIPCField(_T("Größe"),sSize,_T('C')));
#endif

		CIPCAudioIdipClient* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && !m_bAudioBusy)
		{
			if (m_PlayStatus == PS_PLAY_FORWARD)
			{
				if (m_bSetSyncPoint)
				{
					pAudio->ResetSentSamples();
					m_pAudioRecord->SetSyncPoint(true);
					m_pAudioRecord->SetContainsData(true);
					m_pAudioRecord->UpdateFlagsToBubble();
					m_bSetSyncPoint = FALSE;
				}
				CWndPlay *pDW = pAudio->GetWndPlayActive();
				if (pDW == NULL)
				{
					pAudio->SetWndPlayActive(this, pAudio->GetDefaultOutputID());
				}
				else if (pDW != this)
				{
					m_bAudioBusy = TRUE;
					PostMessage(WM_UPDATE_DIALOGS);
					return FALSE;
				}

				pAudio->DoRequestMediaDecoding(*m_pAudioRecord);
				TRACE(_T("decode     A=%d, S=%d, CR=%d, BNr=%d\n"),m_SequenceRecord.GetArchiveNr(),
					m_SequenceRecord.GetSequenceNr(),m_dwCurrentRecord,m_pAudioRecord->GetPackageNumber());
	//			TRACE(_T("SentSamples.%d\n"), pAudio->GetSentSamples());
			}
			else if (m_PlayStatus == PS_STOP && m_pAudioRecord->IsLongHeader())
			{
				m_pAudioRecord->SetContainsData(false);
				m_pAudioRecord->UpdateFlagsToBubble();
				pAudio->DoRequestMediaDecoding(*m_pAudioRecord);
				TRACE(_T("decode     A=%d, S=%d, CR=%d, BNr=%d\n"),m_SequenceRecord.GetArchiveNr(),
					m_SequenceRecord.GetSequenceNr(),m_dwCurrentRecord,m_pAudioRecord->GetPackageNumber());
			}
		}
	}	// end of Critical Section Scope

	PostMessage(WM_CHANGE_AUDIO);
 	Sleep(0);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::DecreasePictureRequests()
{
	if (m_iPictureRequests>0)
	{
		m_iPictureRequests--;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (pMF->IsSyncMode())
	{
		CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
		if(pSCB)
		{
			pSCB->PostMessage(WM_USER,SCB_CONFIRM_PICTURE);
		}

	}
//	TRACE(_T("PictureData(): m_iPictureRequests = %i\n"), m_iPictureRequests);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::CameraNames(int iNumFields, const CIPCField fields[])
{
	if (WK_IS_WINDOW(m_pDlgField))
	{
		m_pDlgField->CameraNames(iNumFields,fields);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT  CWndPlay::OnChangeVideo(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
//	TRACE(_T("OnChangeVideo\n"));
	BOOL bRedrawNow = FALSE;
	BOOL bUpdateDialogs = FALSE;

	{ // Critical section scope
		CAutoCritSec acs(&m_csPictureRecord);

		if (m_pPictureRecord==NULL)
		{
			return 0;
		}

		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			if (m_pJpeg->DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(),
												m_pPictureRecord->GetDataLength()))
			{	
	//			m_rcZoom = CRect(0,0,0,0); // Zoom ausschalten
				if (m_nDibData>1)
				{
					HBITMAP hBitmap = m_pJpeg->CreateBitmap();
					if (hBitmap)
					{
						CDib* pDib = new CDib(hBitmap);
						if (pDib)
						{
							WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
	 						CDibData* pDibData = new CDibData(pDib,GetCurrentRecordNr(),
								FormatPicture(TRUE),
								FormatData(_T("\n"),_T(":\t")),
								m_sComment,
								wSeqNr);
							m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
						}
						DeleteObject(hBitmap);
					}
				}

				bRedrawNow = TRUE;
				bUpdateDialogs = TRUE;
				m_bWasSomeTimesActive = TRUE;
				m_sError.Empty();
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4)
		{
			bRedrawNow = TRUE;
			bUpdateDialogs = TRUE;
			m_bWasSomeTimesActive = TRUE;
			if (m_nDibData>1)
			{
				WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
				CSize s = GetPictureSize();
				CDib* pDib = NULL;
				CDibData* pDibData = NULL;
				pDib = m_pMpeg4Decoder->GetDib();
				if (pDib)
				{
					pDibData = new CDibData(pDib,GetCurrentRecordNr(),
						FormatPicture(TRUE),
						FormatData(_T("\n"),_T(":\t")),
						m_sComment,
						wSeqNr);
					m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
				}
			}
		}
	#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			m_pPTDecoder->Decode((BYTE*)m_pPictureRecord->GetData(), 
									m_pPictureRecord->GetDataLength(), 
									m_pPictureRecord->GetPictureType()==SPT_FULL_PICTURE, 
									FALSE);
		
			if (m_nDibData>1)
			{
				WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
				CSize s = GetPictureSize();
				CDibData* pDibData = new CDibData(new CDib(m_pPTDecoder->GetBitmapHandle()),
					GetCurrentRecordNr(),
					FormatPicture(TRUE),
					FormatData(_T("\n"),_T(":\t")),
					m_sComment,
					wSeqNr);
				m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
			}

			bRedrawNow = TRUE;
			bUpdateDialogs = TRUE;
			m_bWasSomeTimesActive = TRUE;
			m_sError.Empty();
		}
#endif
	} // end of critical section

	if (bUpdateDialogs)
	{
		UpdateDialogs();
	}
	
	if (bRedrawNow)
	{
		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndPlay::OnChangeAudio(WPARAM, LPARAM)
{
	CAutoCritSec acs(&m_csPictureRecord);
	if (m_pAudioRecord==NULL)
	{
		return 0;
	}
	m_csPictureRecord.Lock();
	UpdateDialogs();
	m_csPictureRecord.Unlock();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CWndPlay::OnUpdateDialogs(WPARAM, LPARAM)
{
	m_csPictureRecord.Lock();
	UpdateDialogs();
	m_csPictureRecord.Unlock();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnSetWindowSize()
{
/*
	if (m_pViewIdipClient->m_iXWin > 1 && m_nDibData>1)
	{
		OnDisplay1();
	}
*/
//	UpdateDialogs();

}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndPlay::OnRedrawVideo(WPARAM, LPARAM)
{
	CRect rc;
	GetFrameRect(&rc);
	InvalidateRect(&rc,FALSE);
//	Invalidate();
//	UpdateWindow();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndPlay::OnDecodeVideo(WPARAM, LPARAM)
{
	CRect rect;

	if (m_nDibData>1)
	{
		WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
		CSize s = GetPictureSize();
		CDib* pDib = NULL;
		CDibData* pDibData = NULL;

		if (m_pPictureRecord->GetCompressionType() == COMPRESSION_H263)
		{
			pDib = m_pH263->GetDib();
		}
		else if (m_pPictureRecord->GetCompressionType() == COMPRESSION_MPEG4)
		{
			pDib = m_pMpeg4Decoder->GetDib();
		}
		if (pDib)
		{
			pDibData = new CDibData(pDib,GetCurrentRecordNr(),
									FormatPicture(TRUE),
									FormatData(_T("\n"),_T(":\t")),
									m_sComment,
									wSeqNr);
			m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
		}
	}

	m_csPictureRecord.Lock();
	UpdateDialogs();
	m_csPictureRecord.Unlock();

	CRect rc;
	GetFrameRect(&rc);
	InvalidateRect(&rc,FALSE);
	Sleep(0);
	m_bWasSomeTimesActive = TRUE;
	m_sError.Empty();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
UINT CWndPlay::GetToolBarID()
{
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (pMF->IsSyncMode())
	{
		return IDR_DIB;
	}
	else
	{
		//TKR immer die DTS ToolBar anzeigen
		return IDR_DIB;
/*
		if (m_pServer->IsDTS())
		{
			return IDR_PLAY_DTS;
		}
		else
		{
			return IDR_PLAY;
		}
*/
	}
}
/////////////////////////////////////////////////////////////////////////////
int CWndPlay::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndImageRecherche::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableToolTips(TRUE);
	if (m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX) && m_ToolTip.AddTool(this))
	{
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);

		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (Maximale Zeit=SHRT_MAX)
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 100);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);
		if (CSkinDialog::UseGlobalFonts())
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont());
		}
	}

	m_pH263->SetCaps(H263_FIT,!m_b1to1);
	m_pH263->Init(this,WM_DECODE_VIDEO,0, 0);

	m_pMpeg4Decoder->Init(this,0,0,0);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::PopupMenu(CPoint pt)
{
	CMenu menu;
	CSkinMenu* pMenu;

	menu.LoadMenu(IDR_MENU_ARCHIVE);
	pMenu = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_PLAY_WINDOW);// Database Pictures
	ASSERT(pMenu != NULL);

#ifdef _DTS
		CMenu* pPopupSaveAs = NULL;
		for (DWORD i=0;i<pMenu->GetMenuItemCount();i++)
		{
			pPopupSaveAs = pMenu->GetSubMenu(i);
			if (pPopupSaveAs)
			{
				break;
			}
		}
#endif
	if (pMenu)
	{
		COemGuiApi::DoUpdatePopupMenu(this,pMenu);
		pMenu->ConvertMenu(TRUE);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());
		pMenu->ConvertMenu(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndPlay::OnUser(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == DW_CHECK_PLAY_STATUS)
	{
		CheckPlayStatus();
	}
	else if (wParam == DW_INVALIDATE_RECT)
	{
		InvalidateRect(LPRECT(lParam));
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDestroy() 
{
	if (m_pPlayRealTime)
	{
		BOOL bRealTimePlay = m_pPlayRealTime->IsRealTimePictureDateTime();
		m_pPlayRealTime->StopThread();
		if (bRealTimePlay)
		{
			WK_TRACE(_T("Destroying CWndPlay playing realtime\n"));
			MSG msg;
			while (PeekMessage(&msg, m_hWnd, WM_PLAY_REALTIME, WM_PLAY_REALTIME, PM_REMOVE))
			{
				WK_TRACE(_T("removing WM_PLAY_REALTIME\n"));
			}
			Sleep(500);
		}
	}

	CServer *pServer = GetSafeServer();
	if (pServer && pServer->IsConnectedPlay())
	{
		CIPCDatabaseIdipClient*pDatabase = pServer->GetDatabase();
		if (   pDatabase
			&& !m_pServer->IsDTS())
		{
			pDatabase->DoRequestCloseSequence(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr());
		}
	}
	CWndImageRecherche::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::PreDestroyWindow(CViewIdipClient* pViewIdipClient)
{
	CServer *pServer = GetSafeServer();
	if (pServer && pServer->IsConnectedPlay())
	{
		CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
		if (   WK_IS_WINDOW(pMF)
			&& WK_IS_WINDOW(pViewIdipClient)
			&& pViewIdipClient->GetDocument()
			&& pMF->IsSyncMode())
		{
			CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
			pSCB->DeleteArchive(m_wServerID,m_SequenceRecord.GetArchiveNr(),FALSE);
			CIdipClientDoc* pDoc = pViewIdipClient->GetDocument();
			// TODO! RKE: derive a CHint Class from CObject for type verification
			// Recherche calls UpdateMyViews!
			pDoc->UpdateAllViews(pViewIdipClient, VDH_SYNC_DEL_ARCHIVE, (CObject*)MAKELONG(m_wServerID,m_SequenceRecord.GetArchiveNr()));
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigatePos1() 
{
	SetPlayStatus(PS_STOP);
	if (m_pViewIdipClient->UseSingleWindow() && GetServer())
	{
		m_bGetPicture = FALSE; //muss FALSE sein, da ansonsten bei gemischten Archiven teilweise
							   //nicht zum ersten Datensatz (Audio oder Video) gesprungen wird.
		CIPCSequenceRecord* pSequence = GetServer()->GetFirstSequenceRecord(m_SequenceRecord.GetArchiveNr());
		if (   pSequence
			&& pSequence->GetSequenceNr() != m_SequenceRecord.GetSequenceNr())
		{
			Navigate(*pSequence,1);
			CViewArchive* pViewArchive = ((CMainFrame*)theApp.m_pMainWnd)->GetViewArchive();
			pViewArchive->PostMessage(WM_USER,
				MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
				MAKELONG(pSequence->GetArchiveNr(), pSequence->GetSequenceNr()));
		}
		else
		{
			Navigate(1,0);
		}
	}
	else
	{
		Navigate(1,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::IsQueryRectEmpty() const
{
	return m_pQueryRectTracker && m_pQueryRectTracker->GetRect()->IsRectEmpty();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigatePos1(CCmdUI* pCmdUI) 
{
	BOOL bRectIsEmpty = IsQueryRectEmpty();
	if (m_pViewIdipClient->UseSingleWindow() && GetServer())
	{
		CIPCSequenceRecord* pSequence = GetServer()->GetFirstSequenceRecord(m_SequenceRecord.GetArchiveNr());
		if (   pSequence
			&& pSequence->GetSequenceNr() != m_SequenceRecord.GetSequenceNr())
		{
			pCmdUI->Enable(    !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
							&& bRectIsEmpty);
		}
		else
		{
			pCmdUI->Enable(   m_dwRequestedRecord>1
						   && bRectIsEmpty
						   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
		}
	}
	else
	{
		pCmdUI->Enable(   m_dwRequestedRecord>1
					   && bRectIsEmpty
					   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::NavigateEnd()
{
	OnNavigateEnd();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateEnd() 
{
	SetPlayStatus(PS_STOP);
	m_bNavigateLast = TRUE;
	m_bGetPicture   = FALSE;
	if (m_pViewIdipClient->UseSingleWindow() && GetServer())
	{
		CIPCSequenceRecord* pSequence = GetServer()->GetLastSequenceRecord(m_SequenceRecord.GetArchiveNr());
		if (   pSequence
			&& pSequence->GetSequenceNr() != m_SequenceRecord.GetSequenceNr())
		{
			Navigate(*pSequence,pSequence->GetNrOfPictures());
			CViewArchive* pViewArchive = ((CMainFrame*)theApp.m_pMainWnd)->GetViewArchive();
			pViewArchive->PostMessage(WM_USER,
				MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
				MAKELONG(pSequence->GetArchiveNr(), pSequence->GetSequenceNr()));
		}
		else
		{
			Navigate(m_SequenceRecord.GetNrOfPictures(),0);
		}
	}
	else
	{
		Navigate(m_SequenceRecord.GetNrOfPictures(),0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateEnd(CCmdUI* pCmdUI) 
{
	BOOL bRequRecordWithinNrOfRecords = m_dwRequestedRecord < m_SequenceRecord.GetNrOfPictures();
	BOOL bRectIsEmpty = IsQueryRectEmpty();
	if(bRequRecordWithinNrOfRecords == FALSE)
	{
		//TKR wenn es eine nächste Sequenz gibt, dann den Button nicht disablen
		if(IsNextSequence())
		{
			bRequRecordWithinNrOfRecords = TRUE;
		}
	}

	pCmdUI->Enable(   CanPlayForward()
				   && bRectIsEmpty
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateNext() 
{
	m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
	if (CanNavigate())
	{
		if(!IsQueryRectEmpty())
		{
			m_RectPlayStatus = PS_RECT_NAVIGATE_NEXT;
			SetCanRectPlayForward(TRUE);
		}
		Navigate(m_dwRequestedRecord,1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateNext(CCmdUI* pCmdUI) 
{
	BOOL bCanRectNavigateForward = TRUE;
/*
	// TODO! RKE: activate in 5.1
	// verhindert das Navigieren über die Pfeiltasten und disabled die Buttons
	// Achtung: beeinflusst aber auch die Rechtecksuche!
	if (m_pDlgNavigation && !m_pDlgNavigation->m_ctrlSlider.IsWindowEnabled())
	{
		bCanRectNavigateForward = FALSE;
	}
	else 
*/
	if(   !IsQueryRectEmpty()
	   && !CanRectPlayForward())
	{
		bCanRectNavigateForward = FALSE;
	}
	pCmdUI->Enable(   bCanRectNavigateForward
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				   && CanPlayForward());
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigatePrev() 
{
	m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
	if (CanNavigate())
	{
		if(!IsQueryRectEmpty())
		{
			m_RectPlayStatus = PS_RECT_NAVIGATE_PREV;
			SetCanRectPlayBack(TRUE);
		}
		Navigate(m_dwRequestedRecord,-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigatePrev(CCmdUI* pCmdUI) 
{
	BOOL bCanRectNavigateBack = TRUE;
/*
	// TODO! RKE: activate in 5.1
	// verhindert das Navigieren über die Pfeiltasten und disabled die Buttons
	// Achtung: beeinflusst aber auch die Rechtecksuche!
	if (m_pDlgNavigation && !m_pDlgNavigation->m_ctrlSlider.IsWindowEnabled())
	{
		bCanRectNavigateBack = FALSE;
	}
	else 
*/
	if(   !IsQueryRectEmpty()
		    && !CanRectPlayBack())
	{
		bCanRectNavigateBack = FALSE;
	}
	pCmdUI->Enable(   bCanRectNavigateBack   
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				   && CanPlayBack());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CanPlayForward()
{
	BOOL bRequRecordWithinNrOfRecords = m_dwRequestedRecord < m_SequenceRecord.GetNrOfPictures();
	if(bRequRecordWithinNrOfRecords == FALSE)
	{
		//TKR wenn es eine nächste Sequenz gibt, dann ist die Vorwärtsrichtung möglich
		if(IsNextSequence())
		{
			bRequRecordWithinNrOfRecords = TRUE;
		}
	}

	if (m_pViewIdipClient->UseSingleWindow() && GetServer())
	{
		return	   bRequRecordWithinNrOfRecords
			    || (m_dwRequestedRecord==1 && m_SequenceRecord.GetNrOfPictures()==1)
			    || GetServer()->GetNextCIPCSequenceRecord(m_SequenceRecord);
	}
	else
	{
		return (m_dwRequestedRecord < m_SequenceRecord.GetNrOfPictures());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CanRectPlayForward()
{
	return m_bCanRectPlayForward;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::SetCanRectPlayForward(BOOL bValue)
{
	m_bCanRectPlayForward = bValue;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CanPlayBack()
{
	if (m_pViewIdipClient->UseSingleWindow() && GetServer())
	{
		return    m_dwRequestedRecord>1 
			   || GetServer()->GetPrevCIPCSequenceRecord(m_SequenceRecord);
	}
	else
	{
		return m_dwRequestedRecord>1;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CanRectPlayBack()
{
	return m_bCanRectPlayBack;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::SetCanRectPlayBack(BOOL bValue)
{	
	m_bCanRectPlayBack = bValue;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateForward() 
{
	if (m_PlayStatus != PS_PLAY_FORWARD)
	{
		SetPlayStatus(PS_PLAY_FORWARD);
		m_bSetSyncPoint = TRUE;
		m_bGetPicture   = FALSE;
		m_bAudioBusy    = FALSE;
		CIPCAudioIdipClient* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && !pAudio->CanDecode())
		{
			m_bAudioBusy    = TRUE;
			PostMessage(WM_UPDATE_DIALOGS);
			if (m_bContainsAudio && !m_bContainsVideo)
			{
				PostMessage(WM_COMMAND, ID_NAVIGATE_STOP);
			}
		}
		CheckPlayStatus();
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateForward(CCmdUI* pCmdUI) 
{
/*  TODO TKR warum springt der Button Play nach dem ersten Einrasten kurz wieder raus
	um dann wieder einzurasten und eigerastet zu bleiben?
	BOOL bEnable = CanPlayForward()&& !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode();
	if(!bEnable)
	{
		TRACE(_T("### TKR OnUpdateNavigateForward(): bEnable == FALSE\n"));
	}
*/
	BOOL bCanRectNavigateForward = TRUE;
	if(    !IsQueryRectEmpty()
		&& !CanRectPlayForward())
	{
		bCanRectNavigateForward = FALSE;
	}
	pCmdUI->Enable(   CanPlayForward()
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				   && bCanRectNavigateForward);
	pCmdUI->SetCheck(m_PlayStatus == PS_PLAY_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateReview() 
{
	if (m_PlayStatus != PS_PLAY_BACK)
	{
		SetPlayStatus(PS_PLAY_BACK);
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		CheckPlayStatus();
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateReview(CCmdUI* pCmdUI) 
{
	BOOL bCanRectNavigateBack = TRUE;
	if(    !IsQueryRectEmpty()
		&& !CanRectPlayBack())
	{
		bCanRectNavigateBack = FALSE;
	}
	pCmdUI->Enable(   CanPlayBack() 
				   && (m_pPictureRecord && m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				   && bCanRectNavigateBack);
	pCmdUI->SetCheck(m_PlayStatus == PS_PLAY_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateStop() 
{
	//if realtime play was active, stop it
	if(	   theApp.CanPlayRealtime()
		&& m_pPlayRealTime
		&& m_pPlayRealTime->IsRealTimePictureDateTime())
	{
		TRACE(_T("STOP playing realtime\n"));
		m_pPlayRealTime->SetRealTimePictureDateTime(FALSE);
	}

	//wenn eine Rechtecksuche aktiv war, bei der längere Zeit nichts gefunden wurde
	//(in diesem Fall steht der Text "Rechtecksuche..." im Fenster)
	//soll diese Suche beendet und der angezeigte Text gelöscht werden
	if(IsInRectQuery())
	{
		if(m_pServer)
		{
			CIPCDatabaseIdipClient* pDatabase = m_pServer->GetDatabase();
			if(pDatabase)
			{
				//TRACE(_T("+++++++++++ OnNavigateStop() in Rechtecksuche\n"));

				//cancel query in database, the database will not answer after this
				//stop signal, so ConfirmQuery() will not be called
				pDatabase->DoResponseQueryResult(0); //cancel query
				SetIsInRectQuery(FALSE);
				KillQueryRectTimer();
				RedrawWindow();
			}
		}
	}

//	TRACE(_T("play status stop by user\n"));
	SetPlayStatus(PS_STOP);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateStop(CCmdUI* pCmdUI) 
{
	BOOL bEnable =    (m_PlayStatus != PS_STOP)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode();
	pCmdUI->Enable(bEnable);
	bEnable =   (m_PlayStatus != PS_PLAY_FORWARD)
		     && (m_PlayStatus != PS_PLAY_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateSkipForward() 
{
	if (m_PlayStatus != PS_SKIP_FORWARD)
	{
		SetPlayStatus(PS_SKIP_FORWARD);
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		m_bCorrectRequestRecordNr = TRUE;
		Navigate(m_dwRequestedRecord, 1);
// RKE: OnNavigateNext ruft ebenso Navigate(m_dwRequestedRecord,1);
// mit gleichen Bedingungen auf. Außer das die CamID auf 0xfffffffe gesetzt wird.
// Fehler liegt also in DBS.
//		int nSkip = m_SequenceRecord.GetNrOfPictures() - m_dwRequestedRecord + 1;
//		Navigate(m_dwRequestedRecord, nSkip);
		SetPlayStatus(PS_STOP);
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateSkipForward(CCmdUI* pCmdUI) 
{
	BOOL bRectIsEmpty = IsQueryRectEmpty();
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayForward()
//				   && IsNextSequence()
				   && bRectIsEmpty
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateSkipBack() 
{
	if (m_PlayStatus != PS_SKIP_FORWARD)
	{
		SetPlayStatus(PS_SKIP_FORWARD);
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		m_bCorrectRequestRecordNr = TRUE;
		Navigate(m_dwRequestedRecord, -1);
//		int nSkip = - (int)(m_dwRequestedRecord + 1);
//		Navigate(m_dwRequestedRecord, nSkip);
		SetPlayStatus(PS_STOP);
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateSkipBack(CCmdUI* pCmdUI) 
{
	BOOL bRectIsEmpty = IsQueryRectEmpty();
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayBack()
//                   && GetServer()->GetPrevCIPCSequenceRecord(m_SequenceRecord)
				   && bRectIsEmpty
				   && (m_pPictureRecord && m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateFastBack() 
{
	if (m_PlayStatus != PS_FAST_BACK)
	{
		SetPlayStatus(PS_FAST_BACK);
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		Navigate(m_dwRequestedRecord,-10);
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateFastBack(CCmdUI* pCmdUI) 
{
	BOOL bRectIsEmpty = IsQueryRectEmpty();
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayBack() 
				   && bRectIsEmpty
				   && (m_pPictureRecord && m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	pCmdUI->SetCheck(m_PlayStatus == PS_FAST_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnNavigateFastForward() 
{
	if (m_PlayStatus != PS_FAST_FORWARD)
	{
		SetPlayStatus(PS_FAST_FORWARD);
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		Navigate(m_dwRequestedRecord,10);
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateNavigateFastForward(CCmdUI* pCmdUI) 
{
	BOOL bRectIsEmpty = IsQueryRectEmpty();
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayForward() 
				   && bRectIsEmpty
				   && (  (   m_pPictureRecord 
				          && (m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
						  )
					   || m_pServer->IsDTS()
					   )
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				   );
	pCmdUI->SetCheck(m_PlayStatus == PS_FAST_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::Skip(BOOL bPlayOverSequenceBorder, int i/*=1*/)
{
	m_bUseOneWindow = TRUE;
	return Navigate(m_dwRequestedRecord,i);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::SetNewSequence(CIPCSequenceRecord* pSR)
{
	if (pSR)
	{
		m_bContainsAudio = FALSE;	// the new Sequence may contain anything
		m_bContainsVideo = FALSE;
		m_SequenceRecord = *pSR;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::Navigate(const CIPCSequenceRecord& s,
							  int nRecNr)
{
	m_bUseOneWindow = TRUE;
	if (m_PlayStatus != PS_STOP)
	{
		OnNavigateStop();
	}
	CIPCSequenceRecord swap = m_SequenceRecord;
	m_SequenceRecord = s;
	if (Navigate(nRecNr))
	{
		return TRUE;
	}
	m_SequenceRecord = swap;
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::Navigate(int nRecNr, int nRecCount /*=0*/)
{
	ASSERT(GetServer() != NULL);
	BOOL bRet = FALSE;
	DWORD dwCamID = m_pDlgField->GetSelectedCamID();
	if (IsQueryRectEmpty())
	{
		if  (  (m_PlayStatus == PS_SKIP_FORWARD)
			|| (m_PlayStatus == PS_SKIP_BACK)
			)
		{
			dwCamID = (DWORD)(-1-NAVIGATION_TRACK);
		}

		if (m_bGetPicture)
		{
			CSecID sID(dwCamID);
			dwCamID = CSecID(SECID_GROUP_OUTPUT, sID.GetSubID()).GetID();
		}
		
		if (GetServer())
		{
			bRet = CheckForSequenceBorder(nRecCount, nRecNr, dwCamID);
		}
	}
	else if (    !IsInRectQuery()
			  && !IsRectQueryWithQueryDlg()
			  && m_iPictureRequests == 0)
	{
	
		//Weiter unter wird ein DoRequestNewQuery() aufgerufen und damit das jeweils nächste
		//Bild in der DB gesucht, welchen den Rechtecksuchkriterien entspricht. Dieses Bild
		//wird dann gefunden und angezeigt.
		//Das darf aber nur dann passieren, wenn die Rechtecksuche NICHT aus dem SuchDlg heraus
		//geschieht. Im Falle der Rechtecksuche mittels SuchDlg darf hier dann nicht erneut
		//nach dem jeweils nächstem Bild gesucht werden, da das ja schon in der Rechtecksuche im
		//SuchDlg geschieht.	
		SetIsInRectQuery(TRUE);
		m_iRedrawRectWithText = 0;
		SetTimer(1, 1000, NULL);
		//TRACE(_T("TKR ********* SetTimer  CWndPlay::Navigate() m_bIsInRectQuery = TRUE\n"));

		// build up the query parameters 
		// archiv nr,
		// date, time, ms
		// rectangle coordinates
		CIPCFields fields;
		CSystemTime st;

		m_csPictureRecord.Lock();
		if (m_pPictureRecord)
		{
			st = m_pPictureRecord->GetTimeStamp();
		}
		m_csPictureRecord.Unlock();

		//abhängig von der Suchrichtung wird die Datenbank
		//vorwärts bzw. rückwärts durchsucht
		char op = _T('{');

		// TODO what happens if nRecCount
		if (nRecCount>0)
		{
			st.IncrementTime(0,0,0,0,1);
			op = _T('}');
		}
		else if (nRecCount<0)
		{
			op = _T('{');
			st.DecrementTime(0,0,0,0,1);
		}
		CString sDate(st.GetDBDate());
		CString sTime(st.GetDBTime());
		WORD wMilliSec = st.GetMilliseconds();
		CString sMilliSec;
		sMilliSec.Format(_T("%03i"), wMilliSec);
		if (GetServer()->IsDTS())
		{
			fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,op));
			fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,op));
			fields.SafeAdd(new CIPCField(_T("DVR_MS"),sMilliSec,op));
		}
		else
		{
			fields.SafeAdd(new CIPCField(_T("DBP_DATE"),sDate,op));
			fields.SafeAdd(new CIPCField(_T("DBP_TIME"),sTime,op));
			fields.SafeAdd(new CIPCField(_T("DBP_MS"),sMilliSec,op));
		}

		AddRectToDatabaseSearch(fields);

		WORD w[1];
		w[0] = m_SequenceRecord.GetArchiveNr();


		//On RectanglePlay (Client PlayButtons PlayForward and PlayBack and Rectangle)
		//the query in the database will be stoppt automatically after 
		//dwMaxQueryResults (1 in this case) is reached.
		//Normally we have only one query and after delivering the queryresult the query is over
		//and the querythread will be destroyed (all in the database).
		//In case of rectangle play we have one query after the other and the querythread should
		//be alive until nothing more result were found or the query will be stoppt
		//from outside (Client). Because of this we need to tell the database this special
		//case of searching with the HIWORD of the dwMaxQueryResults
		DWORD dwFlags = 1;
		if(    m_PlayStatus == PS_PLAY_FORWARD
			|| m_PlayStatus == PS_PLAY_BACK
			|| m_RectPlayStatus == PS_RECT_NAVIGATE_NEXT
			|| m_RectPlayStatus == PS_RECT_NAVIGATE_PREV)
		{
			dwFlags = MAKELONG(1, 1); //LO=1: MaxQueryResults, HI=1: SpecialQueryCancel
		}

		//GetServer()->GetDatabase()->DoRequestNewQuery(GetID().GetID(),1,w,fields,1);
		CIPCDatabaseIdipClient* pDB = GetServer()->GetDatabase();
		if(pDB)
		{
			pDB->DoRequestNewQuery(GetID().GetID(),1,w,fields,dwFlags);
		}

		TRACE(_T("CWndPlay::Navigate() RectSearch per Button ID: 0x%x\n"), GetID().GetID());
		// result will be in ConfirmRectQuery
		bRet = TRUE;
	}
//	TRACE(_T("Navigate(): m_iPictureRequests = %i\n"), m_iPictureRequests);
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CheckForSequenceBorder(int nRecCount, int nRecNr, DWORD dwCamID)
{
	ASSERT(GetServer() != NULL);
	BOOL bRet = FALSE;
	if (nRecCount>0)
	{
		if (   (nRecNr+nRecCount<=(int)m_SequenceRecord.GetNrOfPictures()) 
			|| ((nRecNr==1) && (m_SequenceRecord.GetNrOfPictures()==1))
			)
		{
			m_dwRequestedRecord = nRecNr+nRecCount;
			GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
				m_SequenceRecord.GetSequenceNr(),
				nRecNr,   // current
				m_dwRequestedRecord, // new
				dwCamID);
			m_iPictureRequests++;
			bRet = TRUE;
		}
		else
		{
			// next sequence
			if (   m_bUseOneWindow
				|| m_pViewIdipClient->UseSingleWindow()
				|| ((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				)

			{
				TRACE(_T("try to skip sequence border forward %s\n"),m_sArchivName);
				CIPCSequenceRecord* pNextSequence = GetServer()->GetNextCIPCSequenceRecord(m_SequenceRecord);
				if (pNextSequence)
				{
					TRACE(_T("skipping sequence border forward %s\n"),m_sArchivName);

					SetNewSequence(pNextSequence);
					m_dwCurrentRecord = 0;
					m_dwRequestedRecord = 1;

					GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
						m_SequenceRecord.GetSequenceNr(),
						0, // current
						m_dwRequestedRecord, // new
						dwCamID);
					m_iPictureRequests++;

					CViewArchive* pViewArchive = ((CMainFrame*)theApp.m_pMainWnd)->GetViewArchive();
					pViewArchive->PostMessage(WM_USER,
						MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
						MAKELONG(m_SequenceRecord.GetArchiveNr(), m_SequenceRecord.GetSequenceNr()));
					bRet = TRUE;
				}
				else
				{
					SetPlayStatus(PS_STOP);
				}
			}
			else
			{
				SetPlayStatus(PS_STOP);
			}
		}
	}
	else if (nRecCount<0)
	{
		if (nRecNr+nRecCount>0)
		{
			m_dwRequestedRecord = nRecNr+nRecCount;
			m_bCorrectRequestRecordNr = TRUE;
			GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
				m_SequenceRecord.GetSequenceNr(),
				nRecNr,  // current
				m_dwRequestedRecord, // new
				dwCamID);
			//TRACE(_T("SequNr: %d  nRecNr: %d  nRecCount: %d\n"),
			//	m_SequenceRecord.GetSequenceNr(), nRecNr, nRecCount);
			m_iPictureRequests++;
			bRet = TRUE;
		}
		else
		{
			// prev sequence
			if (   m_bUseOneWindow
				|| m_pViewIdipClient->UseSingleWindow()
				|| ((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				)
			{
				TRACE(_T("try to skip sequence border back %s\n"),m_sArchivName);
				CIPCSequenceRecord* pPrevSequence = GetServer()->GetPrevCIPCSequenceRecord(m_SequenceRecord);
				if (pPrevSequence)
				{
					TRACE(_T("skipping sequence border back %s\n"),m_sArchivName);

					SetNewSequence(pPrevSequence);
					DWORD dwNavigate = (DWORD)(-1-NAVIGATION_LAST);
					m_bCorrectRequestRecordNr = TRUE;
					if (dwCamID != SECID_NO_ID)
					{
						dwNavigate = dwCamID;
					}

					m_dwRequestedRecord = m_SequenceRecord.GetNrOfPictures();
					GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
						m_SequenceRecord.GetSequenceNr(),
						0, // current
						m_dwRequestedRecord, // new
						dwNavigate);

					m_iPictureRequests++;

					CViewArchive* pViewArchive = ((CMainFrame*)theApp.m_pMainWnd)->GetViewArchive();
					pViewArchive->PostMessage(WM_USER,
						MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
						MAKELONG(m_SequenceRecord.GetArchiveNr(), m_SequenceRecord.GetSequenceNr()));
					bRet = TRUE;
				}
				else
				{
					SetPlayStatus(PS_STOP);
				}
			}
			else
			{
				SetPlayStatus(PS_STOP);
			}
		}
	}
	else
	{
		if (m_bNavigateLast)
		{
			dwCamID = (DWORD)(-1-NAVIGATION_LAST);
			m_bNavigateLast = FALSE;
		}
		m_dwRequestedRecord = nRecNr;
		GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
			m_SequenceRecord.GetSequenceNr(),
			0,
			m_dwRequestedRecord,
			dwCamID);
		m_iPictureRequests++;
		bRet = TRUE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::ConfirmRectQuery(DWORD dwUserID,
								WORD wArchivNr,
								WORD wSequenceNr, 
								DWORD dwRecordNr,
								int iNumFields,
								const CIPCField fields[])
{
	ASSERT(GetServer() != NULL);
	CIPCSequenceRecord* pNextSequence = NULL;

	SetIsInRectQuery(FALSE);

	//TRACE(_T("TKR ********* CWndPlay::ConfirmRectQuery() m_bIsInRectQuery = FALSE\n"));
	//check if next sequence is reached
	//forward
	WORD wCurrentSequenceNr = m_SequenceRecord.GetSequenceNr();
	if(wCurrentSequenceNr != wSequenceNr)
	{
		pNextSequence = GetServer()->GetCIPCSequenceRecord(wArchivNr, wSequenceNr);
		if(pNextSequence)
		{
			m_SequenceRecord = *pNextSequence;

			//Update Archive Treeview
			CViewArchive* pViewArchive = ((CMainFrame*)theApp.m_pMainWnd)->GetViewArchive();
			pViewArchive->PostMessage(WM_USER,
				MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
				MAKELONG(m_SequenceRecord.GetArchiveNr(), m_SequenceRecord.GetSequenceNr()));
		}
	}

	TRACE(_T("rectangle query result %d,%d,%d\n"),wArchivNr,wSequenceNr,dwRecordNr);
	GetServer()->RequestRecord(wArchivNr,
								wSequenceNr,
								0,
								dwRecordNr,
								SECID_NO_ID);
	m_iRedrawRectWithText = 0;
	KillQueryRectTimer();
	
	m_dwRequestedRecord = dwRecordNr;
	m_iPictureRequests++;
	SetCanRectPlayBack(TRUE);
	SetCanRectPlayForward(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::ConfirmQuery()
{
	//wenn bei einer Rechtecksuche nichts mehr gefunden wurde (Ende erreicht)
	//dann hier nochmal das Bild neu zeichnen, weil im Suchrechteck evtl. eingeblendeter 
	//Text wieder gelöscht werden soll.
	if(    m_pQueryRectTracker
		&& !m_pQueryRectTracker->GetRect()->IsRectEmpty())
	{
		if(m_iRedrawRectWithText > 0) //text was shown on screen because nothing was found
									  //in rectangle query
		{
			m_iRedrawRectWithText = RECTSEARCHENDREACHED;
		}
		else
		{
			KillQueryRectTimer();
		}
		RedrawWindow();
	}
	SetIsInRectQuery(FALSE);

	TRACE(_T("TKR ********* CWndPlay::ConfirmQuery() m_bIsInRectQuery = FALSE\n"));
	if (m_iPictureRequests==0)
	{
		SetPlayStatus(PS_STOP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::AddRectToDatabaseSearch(CIPCFields &fields)
{
	/*			Y-Achse

	^
	sY1__|_ _ _ _ _______________
	|		|				|
	|		|				|
	|		|	       _|_..|.... Fadenkreuz auf Bild
	|		|			|	|	  mit Koordinaten
	|		|				|	  DBD_MD_X und DBD_MD_Y
	sY2__|_ _ _ _|_______________|
	|		|				|
	|
	|_______|_______________|__ > X-Achse

	|				|
	sX1			   sX2
	*/	

	//search only within the rectangle
	if (m_pQueryRectTracker && !m_pQueryRectTracker->GetRectPromilleImage()->IsRectEmpty())
	{
		CString sX1, sY1, sX2, sY2;
		//hole Promille Werte des Rechtecks bez. auf das Image
		CRect rcQuery = m_pQueryRectTracker->GetRectPromilleImage();
		CString sTyp;

		if (GetServer()->IsDTS())
		{
			sTyp = _T("DVR_TYP");
		}
		else
		{
			sTyp = _T("DBP_TYP");
		}

		sX1.Format(_T("%04x"), rcQuery.left);
		fields.Add(new CIPCField(_T("DBD_MD_X"),
			sX1,
			4, _T('}')));
		sY1.Format(_T("%04x"), rcQuery.top);
		fields.Add(new CIPCField(_T("DBD_MD_Y"),
			sY1,
			4, _T('}')));
		sX2.Format(_T("%04x"), rcQuery.right);
		fields.Add(new CIPCField(_T("DBD_MD_X"),
			sX2,
			4, _T('{')));
		sY2.Format(_T("%04x"), rcQuery.bottom);
		fields.Add(new CIPCField(_T("DBD_MD_Y"),
			sY2,
			4, _T('{')));

		/*			TRACE(_T("*** übergebene Rechteck Werte*****\n"));
		TRACE(_T("*** left: %i  top: %i  right: %i  bottom: %i \n"),
		rcQuery.left, rcQuery.top, rcQuery.right, rcQuery.bottom);
		*/
		//bei Tasha und Rechtecksuche nur nach IFRames suchen


		m_csPictureRecord.Lock();
		if (   m_pPictureRecord
			&& m_pPictureRecord->GetCompressionType() == COMPRESSION_MPEG4)
		{	
			CString sIFrameOnly = _T("2"); //1 = Differenzbild
			fields.Add(new CIPCField(sTyp,
				sIFrameOnly,
				1, _T('=')));
		}
		m_csPictureRecord.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnViewOriginal() 
{
	m_b1to1 = !m_b1to1;
	if (   m_pPictureRecord 
		&& m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
	{
		m_pH263->SetCaps(H263_FIT,!m_b1to1);
	}
	if (m_b1to1)
	{
		OnDisplayZoomOut();
	}
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);

	if (m_pServer->CanRectangleQuery())
	{
		pCmdUI->Enable(CanShowImageInOriginalSize());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnFilePrint() 
{
	m_pViewIdipClient->OnFilePrint();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDisplay1() 
{
	m_nDibData = 1;
	m_b1to1 = FALSE;
	EnsureDibQueueNotEmpty();
	m_ToolTip.Activate(FALSE);
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateDisplay1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pViewIdipClient && m_pPictureRecord);
	pCmdUI->SetCheck(m_nDibData == 1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDisplay4() 
{
	m_nDibData = 2;
	m_b1to1 = FALSE;
	EnsureDibQueueNotEmpty();
	m_ToolTip.Activate(TRUE);
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateDisplay4(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord!=NULL);
	pCmdUI->SetCheck(m_nDibData == 2);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDisplay9() 
{
	m_nDibData = 3;
	m_b1to1 = FALSE;
	EnsureDibQueueNotEmpty();
	m_ToolTip.Activate(TRUE);
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateDisplay9(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord!=NULL);
	pCmdUI->SetCheck(m_nDibData == 3);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::EnsureDibQueueNotEmpty()
{
	if (m_pPictureRecord && m_DibDatas.GetSize()==0)
	{
		CSize s = GetPictureSize();
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			HBITMAP hBitmap = m_pJpeg->CreateBitmap();
			if (hBitmap)
			{
				CDib* pDib = new CDib(hBitmap);
				if (pDib)
				{
					WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
					CDibData* pDibData = new CDibData(pDib,GetCurrentRecordNr(),
						FormatPicture(TRUE),
						FormatData(_T("\n"),_T(":\t")),
						m_sComment,
						wSeqNr);
					m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
				}
				DeleteObject(hBitmap);
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4)
		{
			WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
			CDibData* pDibData = new CDibData(m_pMpeg4Decoder->GetDib(),GetCurrentRecordNr(),
				FormatPicture(TRUE),
				FormatData(_T("\n"),_T(":\t")),
				m_sComment,
				wSeqNr);
			m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
			CDibData* pDibData = new CDibData(m_pH263->GetDib(),GetCurrentRecordNr(),
				FormatPicture(TRUE),
				FormatData(_T("\n"),_T(":\t")),
				m_sComment,
				wSeqNr);
			m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
			CDibData* pDibData = new CDibData(new CDib(m_pPTDecoder->GetBitmapHandle()),
				GetCurrentRecordNr(),
				FormatPicture(TRUE),
				FormatData(_T("\n"),_T(":\t")),
				m_sComment,
				wSeqNr);
			m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
		}
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnEditComment() 
{
	CDlgComment dlg(this);

	dlg.m_sComment = m_sComment;
	if (IDOK==dlg.DoModal())
	{
		m_sComment = dlg.m_sComment;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateEditComment(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	BOOL bHandledNotify = FALSE;

	CPoint pt;
	VERIFY(::GetCursorPos(&pt));
	ScreenToClient(&pt);

	m_pDibDataTooltip = HittestDibDataToolTip(pt);

	if (m_pDibDataTooltip)
	{
		pTTT->lpszText = (LPTSTR)m_pDibDataTooltip->GetTooltipText();
		bHandledNotify = TRUE;
	}
	else
	{
		pTTT->szText[0] = 0;
		bHandledNotify = FALSE;
	}
	return bHandledNotify;
}
/////////////////////////////////////////////////////////////////////////////
CDibData* CWndPlay::HittestDibDataToolTip(CPoint pt)
{
	CRect rect;
	int cx,cy,x,y,i;

	GetVideoClientRect(rect);

	if (rect.PtInRect(pt))
	{
		x = pt.x - rect.left;
		y = pt.y - rect.top;
		cx = rect.Width() / m_nDibData;
		cy = rect.Height() / m_nDibData;

		i = x/cx + (y/cy)*m_nDibData;

		if (i<m_DibDatas.GetSize())
		{
			return m_DibDatas.GetAtFast(i);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (::IsWindow(m_ToolTip.m_hWnd))
	{
		CDibData* pHit = HittestDibDataToolTip(point);

		if (!pHit || pHit != m_pDibDataTooltip || m_nDibData == 1)
		{
			// Use Activate() to hide the tooltip.
			m_ToolTip.Activate(FALSE);		
		}

		if (pHit)
		{
			m_ToolTip.Activate(TRUE);
			m_pDibDataTooltip = pHit;
		}
	}

 	if (   m_pServer->CanRectangleQuery()
		&& m_pPictureRecord
		&& m_pQueryRectTracker)
	{
		CRect rcRect, rcTitle;
		GetVideoClientRect(rcRect);
		GetTitleRect(rcTitle);

		if(rcRect.PtInRect(point) && !rcTitle.PtInRect(point))
		{
			//mouse in over picture
			if(m_pQueryRectTracker->HitTest(point) < 0)
			{	
				if(m_nDibData == 1)
				{
					SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_GLASS));
					SetCursor(theApp.LoadCursor(IDC_GLASS));
				}
				else
				{
					SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_ARROW));
					SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
				}
			}
			else
			{
				theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());
			}
		}
		else
		{
			//mouse in over title
			SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_ARROW));
			SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
		}

	}
	else
	{
		if (   m_nDibData==1
			&& m_pPictureRecord)
		{
			CRect rcRect, rcTitle, rcFrame;
			GetVideoClientRect(rcRect);
			GetTitleRect(rcTitle);
			
			if(rcRect.PtInRect(point) && !rcTitle.PtInRect(point))
			{
				SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_GLASS));
				SetCursor(theApp.LoadCursor(IDC_GLASS));
			}
			else
			{
				SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_ARROW));
				SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
			}
		}
		else
		{
			SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_ARROW));
			SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
		}
	}

	CWndImageRecherche::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateFileDib(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   && theApp.m_pPermission 
				   && (theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateFileDisk(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateFileHtml(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateFileJpg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_nDibData>1)
	{
		m_DibDatas.DeleteAllExceptCurrent();
		m_nDibData = 1;
		m_b1to1 = FALSE;
		m_ToolTip.Activate(FALSE);
		RedrawWindow();
	}
	else
	{
		CWndImageRecherche::OnLButtonDblClk(nFlags, point);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnDisplayZoomOut() 
{
	if (m_pPictureRecord)
	{
		// switch off zoom
		m_rcZoom = CRect(0, 0, 0, 0);
		TRACE(_T("TKR OnDisplayZoomOut() ############################# m_rcZoom: top: %i  left: %i Button: %i right: %i\n"),
									m_rcZoom.top, m_rcZoom.left, m_rcZoom.bottom, m_rcZoom.right);
		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
//		InvalidateRect(NULL);
		m_iCountZooms = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateDisplayZoomOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord && ScrollWndPlay());
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (   m_pPictureRecord == NULL
		&& m_pAudioRecord == NULL)
	{
		return;
	}
	CRect rcVideoClient;
	GetVideoClientRect(rcVideoClient);

	//rectangle only in picture records, not in audiorecords
	if(m_pPictureRecord)
	{
		if (m_pServer->CanRectangleQuery())
		{
			// Liegt der Punkt im Videorect?
			if (rcVideoClient.PtInRect(point))
			{
				//Liegt Punkt innerhalb des Suchrechteckes?
				if (   m_pQueryRectTracker 
					&& !m_pQueryRectTracker->GetRect()->IsRectNull()
					&& m_pQueryRectTracker->HitTest(point) >= 0)
				{
					//das unveränderte Rectangle sichern
					m_pQueryRectTracker->SaveLastRect();
					
					m_pQueryRectTracker->Track(this, point, TRUE);

					m_pQueryRectTracker->Draw(this->GetDC());
					
					//hat sich die Lage des QueryRectangle geändert
					//berechne relative Promillewerte des Lage neu

					if(m_pQueryRectTracker->m_rect != m_pQueryRectTracker->GetLastRect())
					{
						m_pQueryRectTracker->SetRectPromille(m_rcZoom);
					}
					RedrawWindow();

				}
				else
				{	
					if (!ScrollWndPlay(point))
					{
						if(m_iCountZooms < 3)
						{
							ZoomWndPlay(point);
						}
						else
						{
							OnDisplayZoomOut();
						}
					}
				}
				SetCanRectPlayForward(TRUE);
				SetCanRectPlayBack(TRUE);
			}
		}
		else 
		{
			// Liegt der Punkt im Videorect?
			if (rcVideoClient.PtInRect(point))
			{
				if (!ScrollWndPlay())
				{
					if(m_iCountZooms < 3)
					{
						ZoomWndPlay(point);
					}
					else
					{
						OnDisplayZoomOut();
					}
				}
			}
		}
	}
	


	CWndImageRecherche::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CWndPlay::ZoomWndPlay(const CPoint &point)
{
	if (m_pPictureRecord)
	{
		m_iCountZooms++;
#if 0
		CRect rcVideo,rcTrack,rcZoom;
		int		nX1, nY1, nX2, nY2;
		CSize s = GetPictureSize();

		GetVideoClientRect(rcVideo);

		if (m_rcZoom.IsRectEmpty())
		{
			m_rcZoom = CRect(0,0, s.cx, s.cy);
		}

		rcTrack.left	= point.x	- rcVideo.Width()  / 4;
		rcTrack.top		= point.y	- rcVideo.Height() / 4;
		rcTrack.right	= point.x	+ rcVideo.Width()  / 4;
		rcTrack.bottom	= point.y	+ rcVideo.Height() / 4;

		rcZoom.left		= rcTrack.left	- rcVideo.left;
		rcZoom.right	= rcTrack.right	- rcVideo.left;
		rcZoom.top		= rcTrack.top	- rcVideo.top;
		rcZoom.bottom	= rcTrack.bottom- rcVideo.top;
		rcZoom.NormalizeRect();
		rcZoom.IntersectRect(rcZoom, CRect(0,0, rcVideo.Width(), rcVideo.Height()));
		
		nX1 = m_rcZoom.left + rcZoom.left	* m_rcZoom.Width()  / rcVideo.Width();
		nY1	= m_rcZoom.top	+ rcZoom.top	* m_rcZoom.Height() / rcVideo.Height(); 
		nX2	= m_rcZoom.left + rcZoom.right	* m_rcZoom.Width()  / rcVideo.Width();
		nY2	= m_rcZoom.top	+ rcZoom.bottom	* m_rcZoom.Height() / rcVideo.Height(); 
		m_rcZoom = CRect(nX1, nY1, nX2, nY2);
#else		
		// m_rcZoom is the current zoom rect based on the original picture size
		CSize s = GetPictureSize();
		if (m_rcZoom.IsRectEmpty())
		{
			m_rcZoom = CRect(0,0, s.cx, s.cy);
		}

		// rcVideo ist the video view size of the client window
		CRect rcVideo;
		GetVideoClientRect(rcVideo);

		// point is relativ to client window
		// normalise point to video view of client window
		CPoint pointNormalised;
		pointNormalised.x = point.x - rcVideo.left;
		pointNormalised.y = point.y - rcVideo.top;

		// normalise point to original picture
		pointNormalised.x = m_rcZoom.left + ( ( pointNormalised.x * m_rcZoom.Width()  )  / rcVideo.Width()  );
		pointNormalised.y = m_rcZoom.top  + ( ( pointNormalised.y * m_rcZoom.Height() )  / rcVideo.Height() ); 

		// new zoom rect based on normalised point
		CRect rcZoom;
		rcZoom.left		= pointNormalised.x - m_rcZoom.Width() / 4;
		rcZoom.right	= pointNormalised.x + m_rcZoom.Width() / 4;
		rcZoom.top		= pointNormalised.y - m_rcZoom.Height() / 4;
		rcZoom.bottom	= pointNormalised.y + m_rcZoom.Height() / 4;

		// Check, zoom rect is inside picture size
		// if necessary, move inside
		if (rcZoom.left < 0)
		{
			rcZoom.OffsetRect(-rcZoom.left, 0);
		}
		if (rcZoom.right > s.cx)
		{
			rcZoom.OffsetRect(-(rcZoom.right - s.cx), 0);
		}
		if (rcZoom.top < 0)
		{
			rcZoom.OffsetRect(0, -rcZoom.top);
		}
		if (rcZoom.bottom > s.cy)
		{
			rcZoom.OffsetRect(0, -(rcZoom.bottom - s.cy));
		}
		m_rcZoom = rcZoom;
#endif
		RedrawWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::ScrollWndPlay(CPoint point /* = (0,0) */)
{
	CPoint	StartPoint(0,0);					 
	CPoint	CurrentPoint(0,0);					 
	CRect	rcSavedZoom;
	rcSavedZoom = m_rcZoom;

	SIZE size = GetPictureSize(); 
	CRect rcImage = CRect(0,0, size.cx, size.cy);

	ShowCursor(FALSE);
	GetCursorPos(&StartPoint);

	if (m_pServer->CanRectangleQuery() && !IsAlarm())
	{
		while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) 
		{
			GetCursorPos(&CurrentPoint);

			m_rcZoom = rcSavedZoom;
			m_rcZoom.left	+= StartPoint.x - CurrentPoint.x;
			m_rcZoom.right	+= StartPoint.x - CurrentPoint.x;
			m_rcZoom.top	+= StartPoint.y - CurrentPoint.y;
			m_rcZoom.bottom	+= StartPoint.y - CurrentPoint.y;
			
			if (m_rcZoom.left<0)
			{
				m_rcZoom.right += abs(m_rcZoom.left);
				m_rcZoom.left	= 0;
			}
			if (m_rcZoom.top<0)
			{
				m_rcZoom.bottom += abs(m_rcZoom.top);
				m_rcZoom.top = 0;
			}
			if (m_rcZoom.right>rcImage.right)
			{
				m_rcZoom.left -= (m_rcZoom.right-rcImage.right);
				m_rcZoom.right = rcImage.right;
			}
			if (m_rcZoom.bottom>rcImage.bottom)
			{
				m_rcZoom.top -= (m_rcZoom.bottom-rcImage.bottom);
				m_rcZoom.bottom  = rcImage.bottom;
			}

			//nicht gezoomter Zustand
			if(m_rcZoom.IsRectEmpty())
			{
				if(StartPoint != CurrentPoint)
				{
					
					TRACE(_T("** StartPoint != CurrentPoint\n"));

					//Rechteck aufziehen nur möglich bei Ansicht "1 Bild", nicht bei Ansicht
					//_T("4 Bilder") oder "9 Bilder" (m_nDibData > 1)
					if(m_pQueryRectTracker && m_nDibData == 1)
					{
						
						CSize pic = GetPictureSize();
						CRect rcPicture(0,0,pic.cx, pic.cy);
						CRect rcClient, rcVideoClient;
						GetClientRect(rcClient);
						GetVideoClientRect(rcVideoClient);

						SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.LoadCursor(IDC_RECT));
						SetCursor(theApp.LoadCursor(IDC_RECT));
						
						//save picture size
						m_pQueryRectTracker->SetPictureRect(rcPicture);
						
						//save current client and video client rectangle
						m_pQueryRectTracker->SaveLastClientRect(rcClient);
						m_pQueryRectTracker->SaveLastVideoClientRect(rcVideoClient);
			
/*						TRACE(_T("** tkr ScrollWndPlay: rcClient        left: %i  top: %i  right: %i  bottom: %i\n"),
						rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						TRACE(_T("** tkr ScrollWndPlay: rcVideoClient   left: %i  top: %i  right: %i  bottom: %i\n"),
						rcVideoClient.left, rcVideoClient.top, rcVideoClient.right, rcVideoClient.bottom);
*/
						
						//noch kein rectangle vorhanden
						if(!m_pQueryRectTracker->GetRect()->IsRectNull())
						{
							m_pQueryRectTracker->SetRectEmpty();
							CRect rc;
							GetFrameRect(&rc);
							InvalidateRect(&rc,FALSE);
//							InvalidateRect(NULL);
							UpdateWindow();
						}
						ShowCursor(TRUE);
						m_pQueryRectTracker->DrawNewRect(point);
						m_pQueryRectTracker->SetRectPromille(m_rcZoom);
						ShowCursor(FALSE);

						//Setzt in CQueryParameter die Rechteckparameter
						theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());

						//Sind mehrere SmallWindows geöffnet und befindet sich auf einem anderen 
						//SmallWindow als das gerade aktuelle (neues Rechteck) ebenfalls ein Rechteck
						//dieses "alte" Rechteck löschen
						
						CSecID sCurrentID = GetID();
/*				
						CWndSmall* pSW = NULL;
						int iSize = m_pViewIdipClient->GetSmallWindows()->GetSize();
							
						if(iSize > 1)
						{
							CWndPlay* pDW = NULL;
							//changes for VisualStudio 2005
							int i = 0;
							for(i=0; i < iSize; i++)
							{
								pSW = m_pViewIdipClient->GetSmallWindows()->GetAtFast(i);
								if(pSW && pSW->IsWndPlay())
								{
									pDW = (CWndPlay*)pSW;
									if(pDW && !pDW->GetRectToQuery().IsRectEmpty())
									{
										if(pDW->GetID() != sCurrentID)
										{
											pDW->ResetTracker();
											pDW->InvalidateRect(NULL);
											pDW->UpdateWindow();		
										}
									}
								}
							}
						}
						*/

						//War der Suchdialog geöffnet, diesen bezüglich des Rechtecks aktualisieren 
						if(m_pViewIdipClient)
						{
							m_pViewIdipClient->ShowHideRectangle();
						}
						EnableNavigationSlider(FALSE);
					}
				}
			}
			else
			{
				if(m_pQueryRectTracker && !m_pQueryRectTracker->GetRect()->IsRectEmpty())
				{
					//zeichnet Rechteck neu, wenn im Bild gescrollt wird
					TRACE(_T("TKR ------ ScrollWndPlay() gezoomt: Recalc\n"));
					m_pQueryRectTracker->RecalcNewRectPositionFromPromille(m_rcZoom);
				}
			}

			CRect rc;
			GetFrameRect(&rc);
			InvalidateRect(&rc,FALSE);
//			InvalidateRect(NULL);
			UpdateWindow();
		}	
	}
	else
	{
		while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) 
		{
			GetCursorPos(&CurrentPoint);

			m_rcZoom = rcSavedZoom;
			m_rcZoom.left	+= StartPoint.x - CurrentPoint.x;
			m_rcZoom.right	+= StartPoint.x - CurrentPoint.x;
			m_rcZoom.top	+= StartPoint.y - CurrentPoint.y;
			m_rcZoom.bottom	+= StartPoint.y - CurrentPoint.y;
			
			if (m_rcZoom.left<0)
			{
				m_rcZoom.right += abs(m_rcZoom.left);
				m_rcZoom.left	= 0;
			}
			if (m_rcZoom.top<0)
			{
				m_rcZoom.bottom += abs(m_rcZoom.top);
				m_rcZoom.top = 0;
			}
			if (m_rcZoom.right>rcImage.right)
			{
				m_rcZoom.left -= (m_rcZoom.right-rcImage.right);
				m_rcZoom.right = rcImage.right;
			}
			if (m_rcZoom.bottom>rcImage.bottom)
			{
				m_rcZoom.top -= (m_rcZoom.bottom-rcImage.bottom);
				m_rcZoom.bottom  = rcImage.bottom;
			}
			
			CRect rc;
			GetFrameRect(&rc);
			InvalidateRect(&rc,FALSE);
//			InvalidateRect(NULL);
			UpdateWindow();
		}
	}
	
	ShowCursor(TRUE);

	return (StartPoint != CurrentPoint);

}

/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_pQueryRectTracker)
	{
		if (pWnd == this && m_pQueryRectTracker->SetCursor(this, nHitTest))
		{
			return TRUE;
		}
	}
	return CWndImageRecherche::OnSetCursor(pWnd, nHitTest, message);
}
/////////////////////////////////////////////////////////////////////////////
 void CWndPlay::DrawRectToQuery(CDC* pDC)
{
 	CRect rcAbsolute, rcClient, rcVideoClient, rcPromille;

 	GetClientRect(rcClient);
 	GetVideoClientRect(rcVideoClient);
 	rcClient.NormalizeRect();
	rcVideoClient.NormalizeRect();

	//QueryRectangle mit Promille-Daten nur dann neu berechnen, 
	//wenn sich die Fenstergrösse des WndPlays (des DB-Bildes)
	//geändert hat
	if(rcClient != m_pQueryRectTracker->GetLastRect())
	{
		m_pQueryRectTracker->SaveLastClientRect(rcClient);
		m_pQueryRectTracker->SaveLastVideoClientRect(rcVideoClient);
 		m_pQueryRectTracker->RecalcNewRectPositionFromPromille(m_rcZoom);
//		TRACE(_T("TKR DrawRectToQuery() ------------- Recalc:  m_rcZoom: top: %i  left: %i Button: %i right: %i\n"),
//									m_rcZoom.top, m_rcZoom.left, m_rcZoom.bottom, m_rcZoom.right);
	}

	m_pQueryRectTracker->Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::GetTimeStamp(CSystemTime& t) const
{
	CAutoCritSec acs(&((CWndPlay*)this)->m_csPictureRecord);

	if (m_pPictureRecord)
	{
		t = m_pPictureRecord->GetTimeStamp();
		return TRUE;
	}

	if(m_pAudioRecord)
	{
		CSystemTime st;
		m_pAudioRecord->GetMediaTime(st);
		t = st;
		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::SetMDValues(WORD wX, WORD wY)
{
	m_wMD_X = wX;
	m_wMD_Y = wY;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//wird nur bei REchtecksuche verwendet
BOOL CWndPlay::SetAllMDValues(WORD wX, WORD wY)
{
	m_waAllMD_X.Add(wX);
	m_waAllMD_Y.Add(wY);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//wird nur bei REchtecksuche verwendet
BOOL CWndPlay::RemoveAllMDValues()
{
	m_waAllMD_X.RemoveAll();
	m_waAllMD_Y.RemoveAll();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//liegen alle Kreuze außerhalb des Rechtecks ?
BOOL CWndPlay::AllMDsOutsideRect(int iAllMDs, CRect rcRectangle)
{
	BOOL bRet = TRUE;
	//changes for VisualStudio 2005
	int i = 0;
	for(i=0; i<iAllMDs; i++)
	{
		CPoint point((int)m_waAllMD_X.GetAt(i), (int)m_waAllMD_Y.GetAt(i)); 
		if ((point.x > 0) && (point.y > 0))
		{
			if(rcRectangle.PtInRect(point))
			{
				return FALSE;
			}
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
//ist archiv ein alarmarchiv ?
BOOL CWndPlay::IsAlarm()
{
	BOOL bRet = FALSE;

	BYTE ArchivTyp = m_SequenceRecord.GetFlags();

	if (   ArchivTyp == CAR_IS_SAFE_RING
		|| ArchivTyp == CAR_IS_ALARM)
	{
		bRet = TRUE;
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
//return the current play status
PlayStatus CWndPlay::GetPlayStatus()
{
	return m_PlayStatus;
}
/////////////////////////////////////////////////////////////////////////////
//return the current rectplay status
RectPlayStatus CWndPlay::GetRectPlayStatus()
{
	return m_RectPlayStatus;
}
/////////////////////////////////////////////////////////////////////////////
CPlayRealTime* CWndPlay::GetPlayRealTime()
{
	return m_pPlayRealTime;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndPlay::OnPlayRealTime(WPARAM wParam, LPARAM lParam)
{

	WORD wArchivNr		= 0;
	WORD wSequenceNr	= 0;
	DWORD dwRecordNr	= 0;
	DWORD dwNrOfRecords	= 0;
	CIPCPictureRecord* ppict = NULL;
	int iNumFields		= 0; 
	CIPCFields	fields;
	
	m_pPlayRealTime->GetRealTimePicture(wArchivNr, wSequenceNr, dwRecordNr, dwNrOfRecords,
									    ppict, iNumFields, fields);
	
	
/*
	// fieldarray dynamisch an größe anpassen
	// sample C Array code
	CIPCField* pFields = NULL;
	
	if(iNumFields > 0)
	{
		pFields = new CIPCField[iNumFields];

		//changes for VisualStudio 2005
		int i = 0;
		for(i = 0; i < iNumFields; i++)
		{
			pFields[i] = *pfields->GetAtFast(i);
		}
	}
	//	WK_DELETE_ARRAY(pFields);
	*/
	m_pViewIdipClient->PlayPictureData(dwRecordNr, dwNrOfRecords, *ppict, this, fields, FALSE, m_pServer->GetID(), wArchivNr, wSequenceNr);
//	PictureData(dwRecordNr,dwNrOfRecords, *ppict, *pfields,FALSE);
	WK_DELETE(ppict);

	return 0;

}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::UpdateAudioState(CIPCAudioIdipClient*pAudio)
{
	CIPCAudioIdipClient*pLocalAudio = m_pServer->GetDocument()->GetLocalAudio();
	if (   (pLocalAudio == NULL && pAudio != NULL)
		|| m_PlayStatus != PS_PLAY_FORWARD && pAudio == pLocalAudio)
	{
		m_bAudioBusy = !pAudio->CanDecode();
		PostMessage(WM_UPDATE_DIALOGS);
	}
// TRACE(_T("### RKE Test: UpdateAudioState:%s: %d, %d, %s\n"), NameOfEnum(m_PlayStatus), pLocalAudio, pAudio, m_sArchivName);
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{		
		m_iRedrawRectWithText++;
		if(m_iRedrawRectWithText > 3)
		{
			m_iRedrawRectWithText = 1;
		}
		RedrawWindow();
	}
	CWndImageRecherche::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::SetSyncPlayStatus(PlayStatus ps)
{
	CIPCAudioIdipClient* pAudio = m_pServer->GetDocument()->GetLocalAudio();
	if (ps == PS_PLAY_FORWARD)
	{
		m_bSetSyncPoint = TRUE;
		if (pAudio)
		{
			if (   pAudio->GetWndPlayActive() != NULL
				&& pAudio->GetWndPlayActive() != this)
			{
				m_bAudioBusy = TRUE;
			}
			PostMessage(WM_UPDATE_DIALOGS);
		}
	}

//	TRACE(_T("### RKE Test: SetSyncPlayStatus:%s, %d, %d, %s\n"), NameOfEnum(ps), this, pAudio->GetWndPlayActive(), m_sArchivName);
	SetPlayStatus(ps);
//	m_PlayStatus = ps;
}
//////////////////////////////////////////////////////////////////////////
// Save.cpp stuff is now here, where it belongs to
void CWndPlay::OnFileDib() 
{
	CString sFileName = GetSaveFilename(IDS_DIB, _T("bmp"));
	if (!sFileName.IsEmpty())
	{
		if (sFileName.MakeLower().Right(4) != _T(".bmp")) 
			sFileName += _T(".bmp");

		BeginWaitCursor();
		BOOL bOK = SaveDIB(sFileName);
		EndWaitCursor();
		ConfirmSaving(bOK, sFileName);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnFileHtml() 
{
	CString sFileName = GetSaveFilename(IDS_HTML, _T("htm"));
	if (!sFileName.IsEmpty())
	{
		if ((sFileName.MakeLower().Right(4) != _T(".htm")) && (sFileName.MakeLower().Right(5) != _T(".html"))) 
			sFileName += _T(".htm");

		BeginWaitCursor();
		BOOL bOK = SaveHTML(sFileName);
		EndWaitCursor();
		ConfirmSaving(bOK, sFileName);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndPlay::OnFileJpg() 
{
	CString sFileName = GetSaveFilename(IDS_JPEG, _T("jpg"));
	if (!sFileName.IsEmpty())
	{
		if (sFileName.MakeLower().Right(4) != _T(".jpg")) 
			sFileName += _T(".jpg");

		BeginWaitCursor();
		BOOL bOK = SaveJPG(sFileName);
		EndWaitCursor();
		ConfirmSaving(bOK, sFileName);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::SaveJPG(const CString& sFileName)
{
	BOOL bRet = TRUE;
	
	if (m_pPictureRecord)
	{
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			// todo uncomment for stretching jpg files
			if (m_pPictureRecord->GetResolution() == RESOLUTION_2CIF)
			{
				HANDLE hDIB = m_pJpeg->CreateDIB();
				if (hDIB)
				{
					bRet = m_pJpeg->EncodeJpegToFile(hDIB, sFileName);
					GlobalFree(hDIB);		
				}
			}
			else
			{
				bRet = SaveRAW(sFileName);
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4)
		{
			CJpeg jpeg;
			CDib* pDib = m_pMpeg4Decoder->GetDib();

			HGLOBAL hDIB = pDib->GetHDIB();
			if (hDIB)
			{
				bRet = jpeg.EncodeJpegToFile(hDIB, sFileName);
				GlobalFree(hDIB);
			}
			WK_DELETE(pDib);
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			CJpeg jpeg;
			CDib* pDib = m_pH263->GetDib();

			HGLOBAL hDIB = pDib->GetHDIB();
			if (hDIB)
			{
				bRet = jpeg.EncodeJpegToFile(hDIB, sFileName);
				GlobalFree(hDIB);
			}
			WK_DELETE(pDib);
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			CDib dib(m_pPTDecoder->GetBitmapHandle());
			dib.IncreaseTo24Bit();
			CJpeg jpeg;

			HGLOBAL hDIB = dib.GetHDIB();
			if (hDIB)
			{
				bRet = jpeg.EncodeJpegToFile(hDIB, sFileName);
				GlobalFree(hDIB);
			}
		}
#endif
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::BuildExtraData()
{
	// InfoString1@InfoString2@Kommentar@GA oder SDI Daten
	CString s;

	// Check des Kommentarfeldes: Archivname und Kamera Nr ergänzen,
	// so noch nicht vorhanden
	CString sTemp = m_sComment;
	
	if (m_sComment.Find(_T("Archiv:")) == -1)
	{
		sTemp.Format(_T("Archiv: %s\r\nKamera Nr: %d\r\n%s"), 
							(LPCTSTR)m_sArchivName, 
							1+(int)m_pPictureRecord->GetCamID().GetSubID(), 
							(LPCTSTR)m_sComment);
	}

	s = FormatPicture();
	s += _T('@');
	s += FormatData(_T(", "),_T(":"));
	s += _T('@');
	s += sTemp;
	s += _T('@');
	if (m_pPictureRecord)
	{
		s += m_pPictureRecord->GetInfoString();
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
int	CWndPlay::GetRecNo()
{
	return m_dwCurrentRecord;
}
/////////////////////////////////////////////////////////////////////////////
int	CWndPlay::GetRecCount()
{
	return m_SequenceRecord.GetNrOfPictures();
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::FormatComment()
{
	return m_sComment;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::FormatCommentHtml()
{
	CString sReturn = m_sComment;
	sReturn.Replace(_T("&"), _T("&amp;"));
	sReturn.Replace(_T("<"), _T("&lt;"));
	sReturn.Replace(_T(">"), _T("&gt;"));
	sReturn.Replace(_T("\""), _T("&quot;"));
	sReturn.Replace(_T("\r\n"), _T("<br>")); // check first to avoid two line breaks 
	sReturn.Replace(_T("\n\r"), _T("<br>")); // check first to avoid two line breaks 
	sReturn.Replace(_T("\n"), _T("<br>"));
	sReturn.Replace(_T("  "), _T("&nbsp;&nbsp;"));
	sReturn.Replace(_T("\t"), _T("&nbsp;&nbsp;&nbsp;&nbsp;"));
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::GetDate()
{
	CString s;
	CAutoCritSec acs(&m_csPictureRecord);

	if (m_pAudioRecord)
	{	
		CSystemTime t;
		m_pAudioRecord->GetMediaTime(t);
		s = t.GetDate();
	}
	else if (m_pPictureRecord)
	{	
		s = m_pPictureRecord->GetTimeStamp().GetDate();
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::GetTime()
{
	CString s;
	CAutoCritSec acs(&m_csPictureRecord);

	if (m_pAudioRecord)
	{	
		CSystemTime t;
		m_pAudioRecord->GetMediaTime(t);
		s.Format(_T("%s,%03d"),t.GetTime(),t.GetMilliseconds());
	}
	else if (m_pPictureRecord)
	{	
		s.Format(_T("%s,%03d"),m_pPictureRecord->GetTimeStamp().GetTime(),m_pPictureRecord->GetTimeStamp().GetMilliseconds());
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::GetCompression()
{
	CString s;

	CAutoCritSec acs(&m_csPictureRecord);
	if (m_pAudioRecord)
	{
		if (m_bAudioBusy)
		{
			s.LoadString(IDS_AUDIO_BUSY);
			if (m_pPictureRecord == NULL)
			{
				OnNavigateStop();
			}
		}
		else
		{
			if (m_pAudioRecord->IsLongHeader())
			{
				s = CIPCAudioIdipClient::GetCompressionType(m_pAudioRecord);
				m_sLastCompressionType = s;
			}
			else
			{
				s = m_sLastCompressionType;
			}
		}
#ifdef _DEBUG
		if (m_pAudioRecord->IsLongHeader())
		{
			s += _T(" LH");
		}
		if (m_pAudioRecord->GetIsSyncPoint())
		{
			s += _T(" SP");
		}
#endif
	}
	else if (m_pPictureRecord)
	{	
		switch (m_pPictureRecord->GetCompressionType())
		{
		case COMPRESSION_H263:
			s = _T("H.263");
#ifdef _DEBUG
			if (m_pPictureRecord->GetPictureType() == SPT_FULL_PICTURE)
			{
				s += _T(" I");
			}
			else if (m_pPictureRecord->GetPictureType() == SPT_DIFF_PICTURE)
			{
				s += _T(" P");
			}
#endif
			break;
		case COMPRESSION_JPEG:
			s = _T("JPEG");
			break;
		case COMPRESSION_RGB_24:
			s = _T("Bitmap");
			break;
		case COMPRESSION_PRESENCE:
			s = _T("PRESENCE");
			break;
		case COMPRESSION_MPEG4:
			s = _T("MPEG4");
			break;
		}
	}
	
	return s;
}
/////////////////////////////////////////////////////////////////////////////
int CWndPlay::GetCameraNr()
{
	CAutoCritSec acs(&m_csPictureRecord);
	if (m_pPictureRecord)
	{
		return 1+(int)m_pPictureRecord->GetCamID().GetSubID();
	}
	else if (m_pAudioRecord)
	{
		return 1+(int)m_pAudioRecord->GetID().GetSubID();
	}
	return -1;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndPlay::FormatPicture(BOOL bTabbed /*= FALSE*/)
{
	CString s;

	if (m_pPictureRecord)
	{	
		CString sFormat,sDate,sTime;
		sDate = m_pPictureRecord->GetTimeStamp().GetDate();
		sTime = m_pPictureRecord->GetTimeStamp().GetTime();
		sFormat.LoadString(bTabbed ? IDS_PICTURE_TABBED : IDS_PICTURE);
		
		CSize size = GetPictureSize();
		s.Format(sFormat, 1+(int)m_pPictureRecord->GetCamID().GetSubID(),
						  sDate,sTime,size.cx,size.cy);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::SaveDIB(const CString& sFileName)
{
	BOOL bRet = TRUE;
	if (m_pPictureRecord)
	{
		if (m_pPictureRecord && m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{					 
			CSize s = GetPictureSize();

			HBITMAP hBitmap = m_pJpeg->CreateBitmap();
			CDib dib(hBitmap);
			bRet = dib.Save(sFileName);
			DeleteObject(hBitmap);
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4)
		{
			CDib* pDib;
			pDib = m_pMpeg4Decoder->GetDib();
			if (pDib)
			{
				bRet = pDib->Save(sFileName);
				WK_DELETE(pDib);
			}
		}					   
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			CDib* pDib;
			pDib = m_pH263->GetDib();
			if (pDib)
			{
				bRet = pDib->Save(sFileName);
				WK_DELETE(pDib);
			}
		}					   
#ifndef _DTS
		else if (m_pPictureRecord && m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			CDib dib(m_pPTDecoder->GetBitmapHandle());
			dib.IncreaseTo24Bit();
			bRet = dib.Save(sFileName);
		}
#endif
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::SaveRAW(const CString& sFileName)
{
	CFile file;
	BOOL bRet = TRUE;

	TRY
	{
		if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite))
		{
			file.Write((BYTE*)m_pPictureRecord->GetData(),
					   m_pPictureRecord->GetDataLength()
						);
			file.Close();

		}
	}
	CATCH (CFileException, e)
	{
		bRet = FALSE;
	}
	END_CATCH

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::SaveHTML(const CString& sPathName)
{
	CString sName,sJpg;
	int p;
	BOOL bRet = TRUE;

	p = sPathName.ReverseFind(_T('.'));
	sJpg = sPathName.Left(p+1) + _T("jpg");
	sName = sPathName.Left(p);
	p = sName.ReverseFind(_T('\\'));
	sName = sName.Mid(p+1);

	bRet = SaveJPG(sJpg);
	if (bRet)
	{
		TRY
		{
			CStdioFileU html(sPathName, CFile::modeCreate |CFile::modeReadWrite | CFile::shareExclusive);
			CString sTemp,sT,sL;
			CString str;
			CString sMetaTag = CWndHTML::GetHTMLCodePageMetaTag() + HTML_NEWLINE;
			CSize size;

			html.SetFileMCBS((WORD)CWK_String::GetCodePage());	// do not save in unicode

			str = theApp.GetStationName();
			if (str.IsEmpty())
			{
				str = COemGuiApi::GetProducerName();
			}
			size = GetPictureSize();
			html.WriteString(HTML_BEGIN_TAG HTML_NEWLINE);
			html.WriteString(HTML_HEAD_TAG HTML_NEWLINE);
			html.WriteString(sMetaTag);
			sMetaTag.Format(HTML_GENERATOR_METATAG HTML_NEWLINE, theApp.m_pszAppName);
			html.WriteString(sMetaTag);

			sTemp.Format(HTML_TITLE_TAG HTML_NEWLINE,str);
			html.WriteString(sTemp);
			html.WriteString(HTML_HEADEND_TAG HTML_NEWLINE);
			
			html.WriteString(HTML_BODY_TAG HTML_NEWLINE);

			html.WriteString(_T("<table border=\"1\" cellpadding=\"1\">\n"));

			html.WriteString(_T("<tr><td><h3 align=\"center\">\n"));
			html.WriteString(str);
			html.WriteString(_T("</h3></td></tr>\n"));

			html.WriteString(_T("<tr><td>\n"));
		//	sTemp.Format(_T("<img src=\")%s.jpg\_T(" width=\")%d\_T(" height=\")%d\_T(">\n"),
		//				 sName,size.cx,size.cy);
			sTemp.Format(_T("<img src=\"%s.jpg\">\n"),sName);
			html.WriteString(sTemp);
			html.WriteString(_T("</tr></td>\n"));

			html.WriteString(_T("<tr><td>\n"));
			html.WriteString(_T("<table border=\"1\" cellpadding=\"1\" width=\"100%\">\n"));

			CString sData,name, value;

			// PC - Date
			// PC - Time
			// Resolution
			name.LoadString(IDS_REC_DATE);
			value = m_pPictureRecord->GetTimeStamp().GetDate();
			sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name,value);
			html.WriteString(sData);

			name.LoadString(IDS_RESOLUTION);
			sData.Format(_T("<tr><td>%s</td><td>%dx%d</td></tr>\n"),name,size.cx,size.cy);
			html.WriteString(sData);

			name.LoadString(IDS_REC_TIME);
			value = m_pPictureRecord->GetTimeStamp().GetTime();
			sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name,value);
			html.WriteString(sData);

			for (int i=0;i<m_Fields.GetSize();i++)
			{
				CIPCField* pField = m_Fields.GetAtFast(i);
				const CString&sFieldName = pField->GetName();
				value = pField->ToString();
				TRACE(_T("%s: %s\n"), sFieldName, value);
				if (!value.IsEmpty())
				{
					name = theApp.GetMappedString(sFieldName);
					if (name.IsEmpty() && GetServer())
					{
						name = GetServer()->GetFieldName(sFieldName);
					}
					if (!name.IsEmpty())
					{
						sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name,value);
						html.WriteString(sData);
					}
				}
			}

			// Comment
			value = FormatCommentHtml();
			if (!value.IsEmpty())
			{
				name.LoadString(IDS_COMMENT);
				sData.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),name, value);
				html.WriteString(sData);
			}

			html.WriteString(_T("</table>\n"));
			html.WriteString(_T("</tr></td>\n"));

			html.WriteString(_T("</table>\n"));

			html.WriteString(_T("</body>\n"));
			html.WriteString(_T("</html>\n"));

			html.Close();
		}
		CATCH_ALL(e)
		{
			bRet = FALSE;
		}
		END_CATCH_ALL
	}
	return bRet;
}
// end of Save.cpp stuff
//////////////////////////////////////////////////////////////////////////
void CWndPlay::OnRButtonDown(UINT nFlags, CPoint point)
{
	//This function was checked against Recherche by TKR, 13.04.2004
	CWndImageRecherche::OnRButtonDown(nFlags, point);

	//ist der Mauszeiger über dem Rechteck (falls eines da ist), dann nicht das Menü anzeigen
	//sondern das Rechteck löschen
	if(    m_pQueryRectTracker
		&& !m_pQueryRectTracker->GetRect()->IsRectNull()
		&& (m_pQueryRectTracker->HitTest(point) >= 0))
	{
		ResetTracker();
		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
//		InvalidateRect(NULL);

		//Setzt in CQueryParameter die Rechteckparameter wieder auf 0
		theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());

		//War der Suchdialog geöffnet, diesen bezüglich des Rechtecks aktualisieren 
		if(m_pViewIdipClient)
		{
			m_pViewIdipClient->ShowHideRectangle();
		}

		UpdateWindow();
	}
	else if (!theApp.m_bDisableRightClick || !theApp.GetMainFrame()->IsFullScreen())
	{
		ClientToScreen(&point);
		PopupMenu(point);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::OnUpdateSmallContext(CCmdUI* pCmdUI)
{
	if (theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(!theApp.GetMainFrame()->IsFullScreen());
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::IsNextSequence()
{	
	//TKR check, if there is a next sequence available after current sequence
	BOOL bRet = FALSE;

	CServer* pServer = GetServer();
	if(pServer)
	{
		CIPCSequenceRecord* pNextSequence = pServer->GetNextCIPCSequenceRecord(m_SequenceRecord);
		if (pNextSequence)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::SetPlayStatus(PlayStatus psNew)
{
	if(    psNew != m_PlayStatus
		|| psNew == PS_STOP)
	{
		//bei jedem Wechsel des PlayStatus m_iPictureRequests auf 0 setzen
		//da ansonsten teilweise (wenn m_iPictureRequests > 0) das 
		//PlayBack und FastBack nach einem Bild stehen bleibt.
		if(m_iPictureRequests > 0)
		{
			m_iPictureRequests = 0;
		}
	}

	m_PlayStatus = psNew;
	switch (m_PlayStatus)
	{
		case PS_STOP:
			EnableNavigationSlider(TRUE);
			break;
		case PS_PLAY_FORWARD:
		case PS_PLAY_BACK:
		case PS_FAST_BACK:
		case PS_FAST_FORWARD:
			EnableNavigationSlider(FALSE);
			break;
	}
	m_RectPlayStatus = PS_RECT_STOP; 
}
//////////////////////////////////////////////////////////////////////////
CString CWndPlay::GetSaveFilename(UINT nIDFilterText, LPCTSTR sExt)
{
	COemFileDialog dlg(this);
	CString sSave,sFilter;

	sSave.LoadString(IDS_SAVE);
	sFilter.LoadString(nIDFilterText);
	dlg.SetProperties(FALSE, sSave, sSave);
	dlg.SetInitialDirectory(theApp.m_sOpenDirectory);
	dlg.AddFilter(sFilter, sExt);
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
//		if (DoesFileExist(sFileName))
//		{
//			CString s;
//			s.Format(IDP_FILE_OVERWRITE,sFileName);
//			if (IDYES == AfxMessageBox(s,MB_YESNO))
//			{
//				return sFileName;
//			}
//		}
//		else
		{
			return sFileName;
		}
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::ConfirmSaving(BOOL bOK, const CString &sFileName)
{
	CString s;
	if (bOK)
	{
		s.Format(IDP_DISK_WRITTEN,sFileName);
	}
	else
	{
		s.Format(IDS_WRITE_FAILED,sFileName);
	}
	COemGuiApi::MessageBox(s,20);
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::KillQueryRectTimer()
{
	m_iRedrawRectWithText = 0;
	KillTimer(1);
}
//////////////////////////////////////////////////////////////////////////
void CWndPlay::SetIsInRectQuery(BOOL bIsInRectQuery)
{
	m_bIsInRectQuery = bIsInRectQuery;
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::IsRectQueryWithQueryDlg()
{
	BOOL bRet = FALSE;
    
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if(pMF)
	{
		CViewArchive* pViewArchive = pMF->GetViewArchive();
		if(pViewArchive)
		{
			CDlgSearchResult* pSearchResult = pViewArchive->GetDlgSearchResult();
			if(pSearchResult)
			{
				bRet = TRUE;
			}
		}
	}
	
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndPlay::CanPrint()
{
	return (m_pPictureRecord != NULL) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
//void CWndPlay::OnFileDisk() 
//{
//	DWORD dwDrives = GetLogicalDrives();
//	int i,j;
//	char ch = _T('a');
//	CString sRoot,sFloppies;
//
//	// browse drives for floppies
//	for (i=0;i<32;i++)
//	{
//		if ( (1<<i) & dwDrives ) 
//		{
//			sRoot.Format(_T("%c:\\"),ch);
//			if (GetDriveType((LPCTSTR)sRoot)==DRIVE_REMOVABLE)
//			{
//				sFloppies += ch;
//			}
//		}
//		ch++;
//	}
//
//	if (sFloppies.IsEmpty())
//	{
//		return;
//	}
//
//    DWORD dwSectorsPerCluster;
//    DWORD dwBytesPerSector;
//    DWORD dwNumberOfFreeClusters;
//    DWORD dwTotalNumberOfClusters;
//	DWORD dwTotalBytes,dwFreeBytes;
//	
//	dwTotalBytes = 0;
//	dwFreeBytes = 0;
//
//	for (i=0;i<sFloppies.GetLength();i++)
//	{
//		sRoot.Format(_T("%c:\\"),sFloppies[i]);
//		if (GetDiskFreeSpace((LPCTSTR)sRoot,
//							&dwSectorsPerCluster,
//							&dwBytesPerSector,
//							&dwNumberOfFreeClusters,
//							&dwTotalNumberOfClusters))
//		{
//			dwTotalBytes = dwTotalNumberOfClusters * dwSectorsPerCluster * dwBytesPerSector;
//			dwFreeBytes = dwNumberOfFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
//			break;
//		}
//	}
//	if (dwTotalBytes==0)
//	{
//		CString sLS,sMes;
//
//		sLS.LoadString(IDP_NO_DISK);
//		sMes.Format(sLS,(LPCTSTR)sFloppies);
//		COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONSTOP);
//		return;
//	}
//
//	if (dwFreeBytes<70*1024)
//	{
//		CString sLS,sMes;
//
//		WK_TRACE_ERROR(_T("cannot store on drive %s , not enough space left"),(LPCTSTR)sRoot);
//
//		sLS.LoadString(IDP_NO_DISK_SPACE);
//		sMes.Format(sLS,(LPCTSTR)sRoot.Left(1));
//		COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONSTOP);
//		return;
//	}
//
//	CStringArray sFiles;
//	CString sExt;
//#ifdef _DTS
//	sExt = _T("htm");
//#else
//	sExt = _T("jpx");
//#endif
//	WK_SearchFiles(sFiles,sRoot,_T("*.") + sExt);
//
//	CString sFileName;
//	BOOL bFree = TRUE;
//
//	for (i=0;i<32767;i++)
//	{
//		sFileName.Format(_T("%08d.%s"),i,sExt);
//		bFree = TRUE;
//		for (j=0;j<sFiles.GetSize();j++)
//		{
//			if (sFileName == sFiles[j])
//			{
//				bFree = FALSE;
//				break;
//			}
//		}
//		if (bFree)
//		{
//			break;
//		}
//	}
//	
//	if (bFree)
//	{
//		sFileName = sRoot + sFileName;
//		CString sLS,sMes;
//
//#ifdef _DTS
//		if (SaveHTML(sFileName))
//#else
//		if (SaveJPX(sFileName))
//#endif
//		{
//			WK_TRACE(_T("wrote file %s"),(LPCTSTR)sFileName);
//
//			sLS.LoadString(IDP_DISK_WRITTEN);
//			sMes.Format(sLS,(LPCTSTR)sFileName);
//			COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONINFORMATION);
//		}
//		else
//		{
//			WK_TRACE_ERROR(_T("Datei %s nicht geschrieben, Diskette defekt oder read only"),(LPCTSTR)sFileName);
//			sMes.LoadString(IDP_DISK_RO);
//			COemGuiApi::MessageBox(sMes,20,MB_OK|MB_ICONSTOP);
//		}
//	}
//	else
//	{
//		// should never be reached
//		WK_TRACE_ERROR(_T("kein freier Dateiname mehr  auf %s"),(LPCTSTR)sRoot);
//	}
//}
/////////////////////////////////////////////////////////////////////////////
//BOOL CWndPlay::SaveJPX(const CString& sFileName)
//{
//	CString sTemp = WK_GetTempFile(_T("REC"));
//
//	if (m_pPictureRecord->GetCompression()==COMPRESSION_JPEG)
//	{
//		if (!SaveRAW(sTemp))
//		{
//			DeleteFile(sTemp);
//			return FALSE;
//		}
//	}
//	else
//	{
//		if (!SaveJPG(sTemp))
//		{
//			DeleteFile(sTemp);
//			return FALSE;
//		}
//	}
//
//	BOOL bRet = TRUE; // GF Interessanter Default-Wert ...
//	CFile file;
//	BYTE  b;
//	WORD  w;
//	DWORD dw;
//	CString sData;
//	CFile temp;
//
//	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite))
//	{
//		TRY
//		{
//			// DateiTyp schreiben
//			b = 1;
//			file.Write(&b, 1);
//
//			// Anzahl Bilder schreiben
//			w = 1;
//			file.Write(&w, 2);
//			
//			// Compression schreiben
//			b = (BYTE)COMPRESSION_JPEG;
//			file.Write(&b, 1);
//
//			// BildResolution schreiben
//			if (m_pPictureRecord->GetCompression()==COMPRESSION_JPEG)
//			{
//				b = (BYTE)m_pPictureRecord->GetResolution();
//			}
//			else
//			{
//				b = (BYTE)RESOLUTION_LOW;
//			}
//			file.Write(&b, 1);
//
//			// Laenge der ExtraData schreiben
//			sData = BuildExtraData();
//			dw = sData.GetLength();
//			file.Write(&dw, 4);
//
//			// ExtraData schreiben
//			file.Write((LPCTSTR)sData, (UINT)sData.GetLength());
//
//			if (temp.Open(sTemp,CFile::modeRead))
//			{
//				DWORD dwLen;
//				BYTE* pBuffer = NULL;
//				CFileStatus cfs;
//
//				TRY
//				{
//					if (temp.GetStatus(cfs))
//					{
//						if (!IsBetween(cfs.m_size, 0, MAX_DWORD))
//						{
//							AfxThrowFileException(CFileException::generic, 0, _T(""));
//						}
//						dwLen = (DWORD)cfs.m_size;
//						pBuffer = new BYTE[dwLen];
//						if (dwLen!=temp.Read(pBuffer,dwLen))
//						{
//							WK_DELETE_ARRAY(pBuffer);
//						}
//					}
//					temp.Close();
//				}
//				CATCH (CFileException,e)
//				{
//					// GF Schön ... dass sich hierbei jemand was gedacht hat .. aber was?
//					// Kein Fehler? Alles ok?
//					// Vielleicht doch lieber FALSE zurück liefern?
//					// Ist doch schließlich das Bild, was hier gelesen worden sein muss - oder?
//					bRet = FALSE;
//				}
//				END_CATCH
//
//				if (pBuffer)
//				{
//					// Laenge der PictData schreiben
//					file.Write(&dwLen,4);
//					
//					// PictData schreiben
//					file.Write(pBuffer, (UINT)dwLen);
//				}
//				WK_DELETE_ARRAY(pBuffer);
//			}
//			else
//			{
//				// GF Schön ... dass sich hierbei jemand NICHTS gedacht hat ...
//				// Kein Fehler? Alles ok?
//				// Vielleicht doch lieber FALSE zurück liefern?
//				// Ist doch schließlich das Bild, was hier gelesen worden sein muss - oder?
//				bRet = FALSE;
//			}
//		}
//		CATCH (CFileException,e)
//		{
//			bRet = FALSE;
//		}
//		END_CATCH
//	}
//	else
//	{
//		// GF Schön ... dass sich hierbei jemand NICHTS gedacht hat ...
//		// Kein Fehler? Alles ok?
//		// Vielleicht doch lieber FALSE zurück liefern?
//		// Ist doch schließlich das Bild, was hier hinein geschrieben worden sein sollte ...
//		bRet = FALSE;
//	}
//	
//	return bRet;
//}
