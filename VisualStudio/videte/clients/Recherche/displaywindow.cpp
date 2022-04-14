// DisplayWindow.cpp : implementation file
//

#include "stdafx.h"

#include "Recherche.h"

#include "RechercheDoc.h"
#include "RechercheView.h"
#include "DisplayWindow.h"
#include "CIPCDataBaseRecherche.h"
#include "CommentDialog.h"
#include "Mainfrm.h"
#include "IPCControlAudioUnit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDisplayWindow::CDisplayWindow(CServer* pServer,CIPCArchivFileRecord* pFile) 
	: CImageWindow(pServer)
{
	Init(pServer);
	m_SequenceRecord.SetArchiveNr(pFile->GetArchivNr());
	m_SequenceRecord.SetSequenceNr(pFile->GetDirNr());
	m_sArchivName = GetServer()->GetArchivName(pFile->GetArchivNr());
	m_dwCurrentRecord = pFile->GetRecNo();
	m_dwRequestedRecord = 0;
	m_SequenceRecord.SetNrOfPictures(pFile->GetRecCount());
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow::CDisplayWindow(CServer* pServer, const CIPCSequenceRecord& s)
	: CImageWindow(pServer) , m_SequenceRecord(s)
{
	Init(pServer);
	m_sArchivName = GetServer()->GetArchivName(m_SequenceRecord.GetArchiveNr());
	m_dwCurrentRecord = 0;
	m_dwRequestedRecord = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::Init(CServer* pServer)
{
	m_pServer = pServer;
	m_dwCurrentRecord = 0;
	m_dwRequestedRecord = 0;
	m_pRechercheView = NULL;
	m_pPictureRecord = NULL;
	m_pAudioRecord = NULL;
	m_bWasSomeTimesActive = FALSE;
	m_sError.Empty();
	m_bIFrame = FALSE;
	m_bNavigateLast  = FALSE;
	m_bUseOneWindow  = FALSE;
	m_bSetSyncPoint  = FALSE;
	m_bGetPicture    = FALSE;
	m_bContainsAudio = FALSE;
	m_bContainsVideo = FALSE;
	m_bAudioBusy     = FALSE;

	m_PlayStatus     = PS_STOP;

	m_iPictureRequests = 0;

	// m_DibDatas
	m_nDibData = 1;
	m_pDibDataTooltip = NULL;

	// for rectangle search

	//init for rectangle search
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
	
	// for check if image can be shown in original size
	m_bImageOriginalSize	= FALSE;
	
	// zoomcounter, max. 3
	m_iCountZooms = 0;

	m_rcZoom = CRect(0,0,0,0); // Zoom erstmal aus!
	if (theApp.GetCPUClockFrequency() >= 1000)
	{
		m_Jpeg.EnableNoiseReduction();
	}
	else
	{
		m_Jpeg.DisableNoiseReduction();
	}

	//Inits for realtime playback
	m_pPlayRealTime	= new CPlayRealTime();
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow::~CDisplayWindow()
{
	CIPCAudioRecherche* pAudio = m_pServer->GetDocument()->GetLocalAudio();
	if (pAudio)
	{
		if (pAudio->GetActiveDisplayWindow() == this)
		{
			pAudio->DoRequestStopMediaDecoding();
			pAudio->SetActiveDisplayWindow(NULL);
		}
	}
	m_DibDatas.DeleteAll();
	WK_DELETE(m_pPictureRecord);
	WK_DELETE(m_pAudioRecord);
	WK_DELETE(m_pPlayRealTime);

	if(m_pQueryRectTracker)
	{
		CRect rc(0,0,0,0);
		theApp.GetQueryParameter().SetRectToQuery(rc);
		WK_DELETE(m_pQueryRectTracker)
	}
}
/////////////////////////////////////////////////////////////////////////////
CSecID	CDisplayWindow::GetID()
{
	return CSecID(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr());
}
/////////////////////////////////////////////////////////////////////////////
int	CDisplayWindow::GetNrDibs()
{
	return m_nDibData;
}
/////////////////////////////////////////////////////////////////////////////
CString	CDisplayWindow::GetName()
{
	CString s = m_SequenceRecord.GetName();
	if (s.IsEmpty())
	{
		s = m_SequenceRecord.GetTime().GetDateTime();
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CSize CDisplayWindow::GetPictureSize()
{
	CSize s;
	s.cx = s.cy = 0;

	CAutoCritSec cs(&m_CS);
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
			if (m_Jpeg.IsValid())
			{
				s = m_Jpeg.GetImageDims();
			}
			else if (GetJpegSize((LPCSTR)m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength(), s))
			{
				if (m_pPictureRecord->GetResolution() == RESOLUTION_LOW)
				{
					s.cy /= 2;
				}
			}
			break;
		case COMPRESSION_H263: 
			s = m_H263.GetImageDims();
			break;
		default:
			break;
		}
	}

	return s;
}
///////////////////////////////////////////////////////////////////
void CDisplayWindow::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1;
	if (   m_pPictureRecord 
		&& m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
	{
		m_H263.SetCaps(H263_FIT,!m_b1to1);
	}
	RedrawWindow();
}

BEGIN_MESSAGE_MAP(CDisplayWindow, CImageWindow)
	//{{AFX_MSG_MAP(CDisplayWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_CHANGE_VIDEO, OnChangeVideo)
	ON_COMMAND(ID_CHANGE_AUDIO, OnChangeAudio)
	ON_COMMAND(ID_REDRAW_VIDEO, OnRedrawVideo)
	ON_COMMAND(ID_UPDATE_DIALOGS, OnUpdateDialogs)
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
	ON_COMMAND(ID_NAVIGATE_REVIEW, OnNavigateReview)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_REVIEW, OnUpdateNavigateReview)
	ON_COMMAND(ID_NAVIGATE_STOP, OnNavigateStop)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_STOP, OnUpdateNavigateStop)
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORIGINAL, OnUpdateViewOriginal)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_DIB, OnFileDib)
	ON_COMMAND(ID_FILE_DISK, OnFileDisk)
	ON_COMMAND(ID_FILE_HTML, OnFileHtml)
	ON_COMMAND(ID_FILE_JPG, OnFileJpg)
	ON_COMMAND(ID_FILE_JPX, OnFileJpx)
	ON_COMMAND(ID_DISPLAY_1, OnDisplay1)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_1, OnUpdateDisplay1)
	ON_COMMAND(ID_DISPLAY_4, OnDisplay4)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_4, OnUpdateDisplay4)
	ON_COMMAND(ID_DISPLAY_9, OnDisplay9)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_9, OnUpdateDisplay9)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_COMMENT, OnEditComment)
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_FILE_DIB, OnUpdateFileDib)
	ON_UPDATE_COMMAND_UI(ID_FILE_DISK, OnUpdateFileDisk)
	ON_UPDATE_COMMAND_UI(ID_FILE_HTML, OnUpdateFileHtml)
	ON_UPDATE_COMMAND_UI(ID_FILE_JPG, OnUpdateFileJpg)
	ON_UPDATE_COMMAND_UI(ID_FILE_JPX, OnUpdateFileJpx)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_DISPLAY_ZOOM_OUT, OnDisplayZoomOut)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_ZOOM_OUT, OnUpdateDisplayZoomOut)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_COMMAND(ID_NAVIGATE_SKIP_FORWARD, OnNavigateSkipForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_SKIP_FORWARD, OnUpdateNavigateSkipForward)
	ON_COMMAND(ID_NAVIGATE_SKIP_BACK, OnNavigateSkipBack)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_SKIP_BACK, OnUpdateNavigateSkipBack)
	ON_COMMAND(ID_NAVIGATE_FAST_BACK, OnNavigateFastBack)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FAST_BACK, OnUpdateNavigateFastBack)
	ON_COMMAND(ID_NAVIGATE_FAST_FORWARD, OnNavigateFastForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FAST_FORWARD, OnUpdateNavigateFastForward)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_SMALL_CLOSE, OnSmallClose)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUser)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
	ON_COMMAND(ID_DECODE_VIDEO, OnDecodeVideo)
	ON_MESSAGE(WM_PLAY_REALTIME,OnPlayRealTime)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::Create(const RECT& rect, CRechercheView* pParentWnd)
{
	m_pRechercheView = pParentWnd;
	m_b1to1 = m_pRechercheView->m_b1to1;

	return CWnd::Create(SMALL_WINDOW_CLASS,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						GetID().GetID());

}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;
	CSize size;
	CRect rect;

	GetClientRect(rect);

	size = pDC->GetOutputTextExtent(m_sArchivName);

	if (size.cx < (rect.Width()-m_ToolBarSize.cx))
	{
		sTitle = m_sArchivName + _T(" ") + GetServerName();
	}

	size = pDC->GetOutputTextExtent(sTitle);
	if (size.cx < (rect.Width()-m_ToolBarSize.cx))
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
void CDisplayWindow::PrintDIBs(CDC* pDC,CRect rect,int iSpace)
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
	font.CreatePointFont(DW_INFO_FONT_SIZE/m_nDibData,_T("Arial"),pDC);
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
			pDibData = m_DibDatas.GetAt(i);
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
int CDisplayWindow::PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim /*= FALSE*/)
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
			m_Jpeg.OnDraw(pDC, rect, m_rcZoom);
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
			m_H263.OnDraw(pDC,rect, m_rcZoom);
			ret = rect.Height();
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}
			CDib dib(m_PTDecoder.GetBitmapHandle());
			dib.OnDraw(pDC,rect, m_rcZoom);
			ret = rect.Height();
		}
#endif
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDraw(CDC* pDC)
{
	if (m_nDibData == 1)
	{
		if (m_bWasSomeTimesActive)
		{
			CRect rect;

			if (m_pPictureRecord)
			{
				if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
				{
					GetVideoClientRect(rect);
					m_Jpeg.OnDraw(pDC, rect, m_rcZoom);
				}
				else if (m_pPictureRecord && m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
				{
					//GetFrameRect(rect);
					GetVideoClientRect(rect);
					m_H263.OnDraw(pDC,rect, m_rcZoom);
				}
#ifndef _DTS
				else if (m_pPictureRecord && m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
				{
					GetVideoClientRect(rect);
					m_PTDecoder.OnDraw(pDC->GetSafeHdc(),rect, m_rcZoom);
				}
#endif

			}

			if (   m_pServer->CanRectangleQuery()
				&& m_pPictureRecord)
			{
				// Im Wiedergabefenster mehrere Kreuze zeichnen
				DoDrawAllCrosses(pDC);

				if(m_pQueryRectTracker && !m_pQueryRectTracker->GetRect()->IsRectEmpty())
				{
					DrawRectToQuery(pDC);
				}
			}
			else
			{
				//kein Rechteck vorhanden, nur ein Kreuz zeichnen
				DrawCross(pDC,m_wMD_X,m_wMD_Y);
			}


			DrawTitle(pDC);
			DrawCinema(pDC);
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
		//gelˆscht
		ResetTracker();
		
		//War der Suchdialog geˆffnet, diesen bez¸glich des Rechtecks aktualisieren 
		if(m_pRechercheView)
		{
			m_pRechercheView->ShowHideRectangle();
		}
						
		//Setzt in CQueryParameter die Rechteckparameter wieder auf 0
		theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());
		
		
		GetVideoClientRect(rect);
		OnDrawDIBs(pDC,rect);
		DrawTitle(pDC);
		DrawCinema(pDC);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawCross(CDC* pDC, WORD wX, WORD wY)
{
	if (theApp.m_bTargetDisplay)
	{
		// Den Zoom ber¸cksichtigen.
		if (!(m_rcZoom.IsRectEmpty()))
		{
			int x,y;
			CSize ImageSize(m_Jpeg.GetImageDims());
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
		
		if (m_pServer->CanRectangleQuery())
			pen.CreatePen(PS_SOLID,1,RGB(255,255,0));
		else
			pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
		
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
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col)
{
	// Den Zoom ber¸cksichtigen.
	if (!(m_rcZoom.IsRectEmpty()))
	{
		int x,y;
		CSize ImageSize(m_Jpeg.GetImageDims());
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
void CDisplayWindow::DrawAllCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col,BOOL bLines /* = FALSE */)
{
	// Ist das Fadenkreuz eingeschaltet UND sind g¸ltige Koordinaten vorhanden?
	if ((wX > 0)	&& (wY > 0))
	{
		// Den Zoom ber¸cksichtigen.
		if (!(m_rcZoom.IsRectEmpty()))
		{
			int x,y;
			CSize ImageSize = (m_Jpeg.GetImageDims());
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
		{	pDC->MoveTo(x-r,y-r);
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
void CDisplayWindow::DoDrawAllCrosses(CDC* pDC)
{
	int iSize = m_waAllMD_X.GetSize();
	if(iSize > 0)
	{
		if(m_pQueryRectTracker && !m_pQueryRectTracker->GetRect()->IsRectEmpty())
		{
			CRect rc = m_pQueryRectTracker->GetRectPromilleImage();
			rc.NormalizeRect();
			
			if(AllMDsOutsideRect(iSize, rc))
			{
				//alle Kreuze auﬂerhalb des Rechtecks, nur das Hauptkreuz weiss zeichen
				DrawAllCrosses(pDC,m_waAllMD_X.GetAt(0),m_waAllMD_Y.GetAt(0), RGB(255, 255, 255), TRUE);
			}
			else
			{
				BOOL bMaxMDInRect = TRUE;
				for(int i=0; i<iSize; i++)
				{
					if ((m_waAllMD_X.GetAt(i) > 0) && (m_waAllMD_Y.GetAt(i) > 0))
					{
						//bei Rechteck alle Kreuze farbig abgestuft anzeigen und nur das der
						//maximalen Bewegung innerhalb des Rechtecks als Fadenkreuz zeichnen

						CPoint point((int)m_waAllMD_X.GetAt(i), (int)m_waAllMD_Y.GetAt(i)); 


						if(rc.PtInRect(point))
						{
							//zeichne nur das Kreuz der st‰rksten Bewegung als Fadenkreuz 
							if(bMaxMDInRect)
							{
								DrawAllCrosses(pDC,m_waAllMD_X.GetAt(i),m_waAllMD_Y.GetAt(i), RGB(255, 255, 255), TRUE);
								bMaxMDInRect = FALSE;
							}
							else
							{
								DrawAllCrosses(pDC,m_waAllMD_X.GetAt(i),m_waAllMD_Y.GetAt(i), RGB(255, (i+1)*40, (i+1)*40));
							}
						}
						else
						{
							DrawAllCrosses(pDC,m_waAllMD_X.GetAt(i),m_waAllMD_Y.GetAt(i), RGB(255, (i+1)*40, (i+1)*40));

						}
					}
				}
			}
		}
		else
		{
			// kein Rechteck da, nur das Hauptkreuz weiss zeichen
			if (theApp.m_bTargetDisplay)
			{
				if ((m_waAllMD_X.GetAt(0) > 0) && (m_waAllMD_Y.GetAt(0) > 0))
				{
					DrawAllCrosses(pDC,m_waAllMD_X.GetAt(0),m_waAllMD_Y.GetAt(0), RGB(255, 255, 255), TRUE);
				}
			}
		}
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDrawDIBs(CDC* pDC, CRect rect)
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
			pDibData = m_DibDatas.GetAt(i);
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
					CBrush b;
					b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
					pDC->FrameRect(r,&b);
					b.DeleteObject();
					pDC->TextOut(r.left,r.top,sn);
				}
			}
			else
			{
				CBrush b;
				b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
				pDC->FrameRect(r,&b);
				b.DeleteObject();
				pDC->TextOut(r.left,r.top,sn);
			}
		}
		else
		{
			CBrush b;
			b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			pDC->FillRect(r,&b);
			b.DeleteObject();
		}
	}
	{
		CRect fr(rect);
		fr.top = rect.top + m_nDibData*h;
		CBrush b;
		b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		pDC->FillRect(fr,&b);
		fr = rect;
		fr.left = rect.left + m_nDibData*w;
		pDC->FillRect(fr,&b);

		b.DeleteObject();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawFace(CDC* pDC)
{
	CRect rect;
	GetFrameRect(rect);
	CBrush b;
	b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(rect,&b);
	b.DeleteObject();

	if (!m_sError.IsEmpty())
	{
		CFont font;
		CFont* pOldFont;

		font.CreatePointFont(10*10,_T("Arial"),pDC);

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
void CDisplayWindow::OnIdle()
{
	CheckPlayStatus();
}
////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::CanNavigate() const
{
	BOOL bDeliver = TRUE;

	//check realtime status on video AND on audio  
	if (   m_bContainsAudio && !m_bAudioBusy
		|| !theApp.CanPlayRealtime())
	{
		bDeliver = (m_iPictureRequests < theApp.m_nMaxRequestedRecords);
	}
	else
	{
		bDeliver = (m_iPictureRequests == 0);
	}

	if (m_pAudioRecord)
	{
		CIPCAudioRecherche* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && pAudio->DoNotSendSamples())
		{
			bDeliver = FALSE;
			TRACE(_T("Rejecting Sample c. o. Overflow\n"));
		}
	}

	return bDeliver;
}
////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::CheckPlayStatus()
{
	if (m_PlayStatus != PS_PLAY_FORWARD)
	{
		CIPCAudioRecherche* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && pAudio->GetActiveDisplayWindow() == this)
		{
			pAudio->DoRequestStopMediaDecoding();
			pAudio->SetActiveDisplayWindow(NULL);
		}
	}

	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (pMF && pMF->IsSyncMode())
	{
		return;
	}

	CAutoCritSec cs(&m_CS);

	switch(m_PlayStatus)
	{
	case PS_PLAY_FORWARD:
		{
			while (   m_PlayStatus == PS_PLAY_FORWARD		// State may be changed in Navigate(..)
				    && CanNavigate())
			{
				Navigate(GetRequestBaseRecord(),1);
			}
		}break;
	case PS_PLAY_BACK:
		if (CanNavigate())
		{
			Navigate(GetRequestBaseRecord(),-1);
		}
		break;
	case PS_FAST_FORWARD:
		if (CanNavigate())
		{
			Navigate(GetRequestBaseRecord(),10);
		}
		break;
	case PS_FAST_BACK:
		if (CanNavigate())
		{
			Navigate(GetRequestBaseRecord(),-10);
		}
		break;
	case PS_STOP:
		if (m_iPictureRequests==0)
		{
			if (!m_pRechercheView->UseSingleWindow())
			{
				if ((m_dwRequestedRecord==m_SequenceRecord.GetNrOfPictures()) ||
					(m_dwRequestedRecord<2))
				{
					TRACE(_T("stop by last or first record %d\n"),m_dwRequestedRecord);
					m_PlayStatus = PS_STOP;
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
void CDisplayWindow::NoPictureFoundForCamera(CSecID id)
{
	DecreasePictureRequests();

	if (m_pRechercheView->UseSingleWindow())
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
		m_PlayStatus = PS_STOP;
	}
}
////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::RecordNrOutOfRange(CSecID id)
{
	if (m_PlayStatus !=	PS_STOP)
	{
		m_PlayStatus = PS_STOP;
	}
	DecreasePictureRequests();
}
////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::CannotReadPictureForCamera()
{
	if (m_PlayStatus !=	PS_STOP)
	{
		m_PlayStatus = PS_STOP;
	}
	m_bWasSomeTimesActive = FALSE;
	m_sError.LoadString(IDS_READ_ERROR);
	DecreasePictureRequests();
	PostMessage(WM_COMMAND,ID_REDRAW_VIDEO);
}
////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::PictureData(const CIPCArchivFileRecord &data, 
								 const CIPCPictureRecord &pict)
{
	// attention in database thread
	{	// Critical Section
		CAutoCritSec cs(&m_CS);

		WK_DELETE(m_pPictureRecord);
		WK_DELETE(m_pAudioRecord);
		if (m_bGetPicture)
		{
			m_dwRequestedRecord = data.GetRecNo();
		}

		m_SequenceRecord.SetNrOfPictures(data.GetRecCount());
		m_dwCurrentRecord = data.GetRecNo();
		CSystemTime t;
		t.FromTime(data.GetFirstTime());
		m_pPictureRecord = new CIPCPictureRecord(pict.GetCIPC(),
												 pict.GetData(),
												 pict.GetDataLength(),
												 pict.GetCamID(),
												 pict.GetResolution(),
												 pict.GetCompression(),
												 pict.GetCompressionType(),
												 t,
												 pict.GetJobTime(),
												 pict.GetInfoString(),
												 pict.GetBitrate(),
												 pict.GetPictureType(),
												 pict.GetBlockNr());

		//check for original image size is showable
		CanShowImageInOriginalSize();

		if (m_SequenceRecord.GetName().IsEmpty())
		{
			m_SequenceRecord.SetName(m_pPictureRecord->GetTimeStamp().GetDateTime());
		}
		m_Fields.SafeDeleteAll();
		TRACE(_T("infostring <%s>\n"),pict.GetInfoString());
		m_Fields.FromString(pict.GetInfoString());
		DecreasePictureRequests();
	}

	if (m_pPictureRecord)
	{
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			m_H263.Decode(*m_pPictureRecord);
		}
		else
		{
			PostMessage(WM_COMMAND,ID_CHANGE_VIDEO);
		}
	}

	Sleep(10);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::PictureData(DWORD dwRecordNr,
					 DWORD dwNrOfRecords,
					 const CIPCPictureRecord &pict,
					 int iNumFields,
					 const CIPCField fields[])
{
	if (pict.GetPictureType()==SPT_FULL_PICTURE)
	{
		TRACE(_T("PictureData %d/%d i frame %08lx\n"),
			dwRecordNr,dwNrOfRecords,pict.GetCamID().GetID());
	}
	else
	{
		TRACE(_T("PictureData %d/%d p frame %08lx\n"),
			dwRecordNr,dwNrOfRecords,pict.GetCamID().GetID());
	}

	// attention in database thread
	{	// Critical Section
		CAutoCritSec cs(&m_CS);
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
		if (m_bGetPicture)
		{
			m_dwRequestedRecord = dwRecordNr;
		}

		m_SequenceRecord.SetNrOfPictures(dwNrOfRecords);
		m_dwCurrentRecord = dwRecordNr;
		m_pPictureRecord = new CIPCPictureRecord(pict);

		m_Fields.FromArray(iNumFields,fields);

	#ifdef _DEBUG
		CString sSize;
		sSize.Format(_T("%d Bytes"),m_pPictureRecord->GetDataLength());
		m_Fields.SafeAdd(new CIPCField(_T("Grˆﬂe"),sSize,_T('C')));
	#endif

		//check for original image size is showable
		CanShowImageInOriginalSize();

		DecreasePictureRequests();
	}


	CString sDBFieldX;
	CString sDBFieldY;
	CIPCField *pFX,*pFY;

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
			for(int i=0; i<=4; i++)
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
		
				DWORD dwX,dwY;
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
		//keine Rechtecksuche mˆglich, da ‰ltere Version
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
			WORD dwX,dwY;
			dwX = dwY = 0;
			_stscanf(pFX->GetValue(),_T("%04x"),&dwX);
			_stscanf(pFY->GetValue(),_T("%04x"),&dwY);
			if ((dwX>0) && (dwY>0))
			{
				SetMDValues((WORD)dwX, (WORD)dwY);
			}
			else
			{
				SetMDValues(0,0);
			}
		}
	}

	if (m_pPictureRecord)
	{
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			m_H263.Decode(*m_pPictureRecord);
		}
		else
		{
			PostMessage(WM_COMMAND,ID_CHANGE_VIDEO);
		}
	}

	Sleep(0);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::AudioData(DWORD dwRecordNr,
							   DWORD dwNrOfRecords,
							   const CIPCMediaSampleRecord &media,
							   int iNumFields,
							   const CIPCField fields[])
{
	{	// Critical Section Scope
		CAutoCritSec cs(&m_CS);

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
		m_Fields.SafeAdd(new CIPCField(_T("Grˆﬂe"),sSize,_T('C')));
	#endif

		CIPCAudioRecherche* pAudio = m_pServer->GetDocument()->GetLocalAudio();
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
				CDisplayWindow *pDW = pAudio->GetActiveDisplayWindow();
				if (pDW == NULL)
				{
					pAudio->SetActiveDisplayWindow(this);
				}
				else if (pDW != this)
				{
					m_bAudioBusy = TRUE;
					PostMessage(WM_COMMAND, ID_UPDATE_DIALOGS);
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

	PostMessage(WM_COMMAND, ID_CHANGE_AUDIO);
 	Sleep(0);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DecreasePictureRequests()
{
	if (m_iPictureRequests>0)
	{
		m_iPictureRequests--;
		CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
		if (pMF->IsSyncMode())
		{
			pMF->GetSyncCoolBar()->ConfirmPicture();
		}
	}
	TRACE(_T("PictureData(): m_iPictureRequests = %i\n"), m_iPictureRequests);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::CameraNames(int iNumFields,
								 const CIPCField fields[])
{
	if (WK_IS_WINDOW(m_pFieldDialog))
	{
		m_pFieldDialog->CameraNames(iNumFields,fields);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnChangeVideo() 
{
	BOOL bRedrawNow = FALSE;
	BOOL bUpdateDialogs = FALSE;

	if (m_pPictureRecord==NULL)
	{
		return;
	}

	m_CS.Lock();

	if (theApp.m_bAllowMiCo && 
		(m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG))
	{
		if (m_Jpeg.DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(),
											   m_pPictureRecord->GetDataLength()))
		{	
//			m_rcZoom = CRect(0,0,0,0); // Zoom ausschalten
			if (m_nDibData>1)
			{
				HBITMAP hBitmap = m_Jpeg.CreateBitmap();
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
#ifndef _DTS
	else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
	{
		m_PTDecoder.Decode((BYTE*)m_pPictureRecord->GetData(), 
								m_pPictureRecord->GetDataLength(), 
								m_pPictureRecord->GetPictureType()==SPT_FULL_PICTURE, 
								FALSE);
	
		if (m_nDibData>1)
		{
			WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
			CSize s = GetPictureSize();
			CDibData* pDibData = new CDibData(new CDib(m_PTDecoder.GetBitmapHandle()),
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

	m_CS.Unlock();

	if (bUpdateDialogs)
	{
		UpdateDialogs();
	}
	
	if (bRedrawNow)
	{
		InvalidateRect(NULL,FALSE);
		UpdateWindow(); //ML
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnChangeAudio()
{
	if (m_pAudioRecord==NULL)
	{
		return;
	}
	m_CS.Lock();
	UpdateDialogs();
	m_CS.Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateDialogs()
{
	m_CS.Lock();
	UpdateDialogs();
	m_CS.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnSetWindowSize()
{
/*
	if (m_pRechercheView->m_iXWin > 1 && m_nDibData>1)
	{
		OnDisplay1();
	}
*/
//	UpdateDialogs();

}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnRedrawVideo() 
{
	Invalidate();
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDecodeVideo() 
{
	CRect rect;

	if (m_nDibData>1)
	{
		WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
		CSize s = GetPictureSize();
		CDibData* pDibData = new CDibData(m_H263.GetDib(),GetCurrentRecordNr(),
			FormatPicture(TRUE),
			FormatData(_T("\n"),_T(":\t")),
			m_sComment,
			wSeqNr);
		m_DibDatas.Add(pDibData,m_nDibData*m_nDibData);
	}

	GetClientRect(rect);
	InvalidateRect(rect,FALSE);
	Sleep(0);
	m_bWasSomeTimesActive = TRUE;
	m_sError.Empty();

	m_CS.Lock();
	UpdateDialogs();
	m_CS.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
UINT CDisplayWindow::GetToolBarID()
{
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (pMF->IsSyncMode())
	{
		return IDR_DIB;
	}
	else
	{
		if (m_pServer->IsDV())
		{
			return IDR_DISPLAY_DTS;
		}
		else
		{
			return IDR_DISPLAY;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CDisplayWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CImageWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_H263.SetCaps(H263_FIT,!m_b1to1);
	m_H263.Init(this,WM_COMMAND,ID_DECODE_VIDEO,0);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::PopupMenu(CPoint pt)
{
	CMenu menu;
	CMenu* pMenu;

	menu.LoadMenu(IDR_CONTEXT);
	pMenu = menu.GetSubMenu(3);

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
		if (pPopupSaveAs)
		{
			pPopupSaveAs->DeleteMenu(ID_FILE_DISK,MF_BYCOMMAND);
			pPopupSaveAs->DeleteMenu(ID_FILE_JPX,MF_BYCOMMAND);
		}
#endif
		
	COemGuiApi::DoUpdatePopupMenu(this,pMenu);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());

}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDisplayWindow::OnUser(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == DW_CHECK_PLAY_STATUS)
	{
		CheckPlayStatus();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDestroy() 
{
	if (   GetServer()
		&& GetServer()->IsConnected())
	{
		CIPCDatabaseRecherche* pDataBase;
		pDataBase = GetServer()->GetDatabase();
		if (pDataBase->GetVersion()<3)
		{
			CIPCArchivFileRecord* pFile = GetServer()->GetCIPCArchivFileRecord(m_SequenceRecord.GetArchiveNr(),
				m_SequenceRecord.GetSequenceNr());
			if (pFile)
			{
				pDataBase->DoRequestFileClosed(*pFile);
			}
		}
		else
		{
			pDataBase->DoRequestCloseSequence(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr());
		}
	}
	CImageWindow::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::PreDestroyWindow(CRechercheView* pRechercheView)
{
	if (   GetServer()
		&& GetServer()->IsConnected())
	{
		CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
		if (   WK_IS_WINDOW(pMF)
			&& WK_IS_WINDOW(pRechercheView)
			&& pRechercheView->GetDocument()
			&& pMF->IsSyncMode())
		{
			CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
			pSCB->DeleteArchive(m_wServerID,m_SequenceRecord.GetArchiveNr(),FALSE);
			CRechercheDoc* pDoc = pRechercheView->GetDocument();
			pDoc->UpdateMyViews(pRechercheView,
							    VDH_SYNC_DEL_ARCHIVE,
							    (CObject*)MAKELONG(m_wServerID,m_SequenceRecord.GetArchiveNr()));
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDisplayWindow::GetRequestBaseRecord() const
{
	DWORD dwReturn = m_dwRequestedRecord;

	if (   WK_IS_WINDOW(m_pFieldDialog)
		&& m_pFieldDialog->GetSelectedCamID() != SECID_NO_ID)
	{
		dwReturn = m_dwCurrentRecord;
	}

	return dwReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigatePos1() 
{
	m_PlayStatus = PS_STOP;
	if (m_pRechercheView->UseSingleWindow())
	{
		CIPCSequenceRecord* pSequence = GetServer()->GetFirstSequenceRecord(m_SequenceRecord.GetArchiveNr());
		if (   pSequence
			&& pSequence->GetSequenceNr() != m_SequenceRecord.GetSequenceNr())
		{
			Navigate(*pSequence,1);
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
void CDisplayWindow::OnUpdateNavigatePos1(CCmdUI* pCmdUI) 
{
	if (m_pRechercheView->UseSingleWindow())
	{
		CIPCSequenceRecord* pSequence = GetServer()->GetFirstSequenceRecord(m_SequenceRecord.GetArchiveNr());
		if (   pSequence
			&& pSequence->GetSequenceNr() != m_SequenceRecord.GetSequenceNr())
		{
			pCmdUI->Enable(!((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
		}
		else
		{
			pCmdUI->Enable(m_dwRequestedRecord>1
				&& !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
		}
	}
	else
	{
		pCmdUI->Enable(m_dwRequestedRecord>1
			&& !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateEnd() 
{
	m_PlayStatus = PS_STOP;	
	m_bNavigateLast = TRUE;
	m_bGetPicture   = FALSE;
	if (m_pRechercheView->UseSingleWindow())
	{
		CIPCSequenceRecord* pSequence = GetServer()->GetLastSequenceRecord(m_SequenceRecord.GetArchiveNr());
		if (   pSequence
			&& pSequence->GetSequenceNr() != m_SequenceRecord.GetSequenceNr())
		{
			Navigate(*pSequence,pSequence->GetNrOfPictures());
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
void CDisplayWindow::OnUpdateNavigateEnd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwRequestedRecord<m_SequenceRecord.GetNrOfPictures()
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateNext() 
{
	m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
	if (CanNavigate())
	{
		Navigate(GetRequestBaseRecord(),1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   CanPlayForward()
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigatePrev() 
{
	m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
	if (CanNavigate())
	{
		Navigate(GetRequestBaseRecord(),-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::CanPlayForward()
{
	if (m_pRechercheView->UseSingleWindow())
	{
		return    m_dwRequestedRecord<m_SequenceRecord.GetNrOfPictures() 
			    || (m_dwRequestedRecord==1 && m_SequenceRecord.GetNrOfPictures()==1)
			    || GetServer()->GetNextCIPCSequenceRecord(m_SequenceRecord);
	}
	else
	{
		return (m_dwRequestedRecord < m_SequenceRecord.GetNrOfPictures());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::CanPlayBack()
{
	if (m_pRechercheView->UseSingleWindow())
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
void CDisplayWindow::OnUpdateNavigatePrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   CanPlayBack()
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateForward() 
{
	if (m_PlayStatus != PS_PLAY_FORWARD)
	{
		m_PlayStatus = PS_PLAY_FORWARD;
		m_bSetSyncPoint = TRUE;
		m_bGetPicture   = FALSE;
		m_bAudioBusy    = FALSE;
		CIPCAudioRecherche* pAudio = m_pServer->GetDocument()->GetLocalAudio();
		if (pAudio && !pAudio->CanDecode())
		{
			m_bAudioBusy    = TRUE;
			PostMessage(WM_COMMAND, ID_UPDATE_DIALOGS);
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
void CDisplayWindow::OnUpdateNavigateForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_dwRequestedRecord<m_SequenceRecord.GetNrOfPictures()
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	pCmdUI->SetCheck(m_PlayStatus == PS_PLAY_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateReview() 
{
	if (m_PlayStatus != PS_PLAY_BACK)
	{
		m_PlayStatus = PS_PLAY_BACK;	
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		CheckPlayStatus();
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateReview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   CanPlayBack() 
				   && (m_pPictureRecord && m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	pCmdUI->SetCheck(m_PlayStatus == PS_PLAY_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateStop() 
{
	//if realtime play was active, stop it
	if(	   theApp.CanPlayRealtime()
		&& m_pPlayRealTime
		&& m_pPlayRealTime->IsRealTimePictureDateTime())
	{
		TRACE(_T("STOP playing realtime\n"));
		m_pPlayRealTime->SetRealTimePictureDateTime(FALSE);
	}

	TRACE(_T("play status stop by user\n"));
	m_PlayStatus = PS_STOP;	
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateStop(CCmdUI* pCmdUI) 
{
	BOOL bEnable =    (m_PlayStatus != PS_STOP)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode();
	pCmdUI->Enable(bEnable);
	bEnable =   (m_PlayStatus != PS_PLAY_FORWARD)
		     && (m_PlayStatus != PS_PLAY_BACK);
	m_pNavigationDialog->EnableSlider(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateSkipForward() 
{
	if (m_PlayStatus != PS_SKIP_FORWARD)
	{
		m_PlayStatus = PS_SKIP_FORWARD;	
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		Navigate(GetRequestBaseRecord(),1);
		m_PlayStatus = PS_STOP;
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateSkipForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayForward()
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateSkipBack() 
{
	if (m_PlayStatus != PS_SKIP_FORWARD)
	{
		m_PlayStatus = PS_SKIP_FORWARD;	
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		Navigate(GetRequestBaseRecord(),-1);
		m_PlayStatus = PS_STOP;
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateSkipBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayBack() 
				   && (m_pPictureRecord && m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateFastBack() 
{
	if (m_PlayStatus != PS_FAST_BACK)
	{
		m_PlayStatus  = PS_FAST_BACK;	
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		Navigate(GetRequestBaseRecord(),-10);
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateFastBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayBack() 
				   && (m_pPictureRecord && m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	pCmdUI->SetCheck(m_PlayStatus == PS_FAST_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnNavigateFastForward() 
{
	if (m_PlayStatus != PS_FAST_FORWARD)
	{
		m_PlayStatus = PS_FAST_FORWARD;	
		m_bGetPicture = m_bContainsAudio && m_bContainsVideo;
		Navigate(GetRequestBaseRecord(),10);
	}
	else
	{
		OnNavigateStop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateNavigateFastForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_nDibData == 1 
				   && CanPlayForward() 
				   && (  (   m_pPictureRecord 
				          && (m_pPictureRecord->GetCompressionType()!=COMPRESSION_H263)
						  )
					   || m_pServer->IsDV()
					   )
				   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode()
				   );
	pCmdUI->SetCheck(m_PlayStatus == PS_FAST_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::Skip(BOOL bPlayOverSequenceBorder, int i/*=1*/)
{
	m_bUseOneWindow = TRUE;
	return Navigate(GetRequestBaseRecord(),i);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::Navigate(const CIPCSequenceRecord& s,
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
BOOL CDisplayWindow::Navigate(int nRecNr,
							  int nRecCount /*=0*/)
{
	BOOL bRet = FALSE;
	DWORD dwCamID = m_pFieldDialog->GetSelectedCamID();
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
		if (nRecCount>0)
		{
			if (   (nRecNr<(int)m_SequenceRecord.GetNrOfPictures()) 
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
					|| m_pRechercheView->UseSingleWindow()
					|| ((CMainFrame*)m_pRechercheView->GetParentFrame())->IsSyncMode()
					)

				{
					TRACE(_T("try to skip sequence border forward %s\n"),m_sArchivName);
					CIPCSequenceRecord* pNextSequence = GetServer()->GetNextCIPCSequenceRecord(m_SequenceRecord);
					if (pNextSequence)
					{
						TRACE(_T("skipping sequence border forward %s\n"),m_sArchivName);

						m_bContainsAudio = FALSE;	// the new Sequence may contain anything
						m_bContainsVideo = FALSE;

						m_SequenceRecord = *pNextSequence;
						m_dwCurrentRecord = 0;
						m_dwRequestedRecord = 1;
						GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
							m_SequenceRecord.GetSequenceNr(),
							0, // current
							m_dwRequestedRecord, // new
							dwCamID);
						m_iPictureRequests++;

						CObjectView* pObjectView = ((CMainFrame*)theApp.m_pMainWnd)->GetObjectView();
						pObjectView->PostMessage(WM_USER,
							MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
							MAKELONG(m_SequenceRecord.GetArchiveNr(), m_SequenceRecord.GetSequenceNr()));
						bRet = TRUE;
					}
					else
					{
						m_PlayStatus = PS_STOP;
					}
				}
			}
		}
		else if (nRecCount<0)
		{
			if (nRecNr+nRecCount>0)
			{
				m_dwRequestedRecord = nRecNr+nRecCount;
				GetServer()->RequestRecord(m_SequenceRecord.GetArchiveNr(),
										   m_SequenceRecord.GetSequenceNr(),
										   nRecNr,  // current
										   m_dwRequestedRecord, // new
										   dwCamID);
				bRet = TRUE;
				m_iPictureRequests++;
			}
			else
			{
				// prev sequence
				if (   m_bUseOneWindow
					|| m_pRechercheView->UseSingleWindow()
					|| ((CMainFrame*)m_pRechercheView->GetParentFrame())->IsSyncMode()
					)
				{
					TRACE(_T("try to skip sequence border back %s\n"),m_sArchivName);
					CIPCSequenceRecord* pPrevSequence = GetServer()->GetPrevCIPCSequenceRecord(m_SequenceRecord);
					if (pPrevSequence)
					{
						TRACE(_T("skipping sequence border back %s\n"),m_sArchivName);

						m_bContainsAudio = FALSE;	// the new Sequence may contain anything
						m_bContainsVideo = FALSE;

						m_SequenceRecord = *pPrevSequence;
						DWORD dwNavigate = (DWORD)(-1-NAVIGATION_LAST);
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
						
						CObjectView* pObjectView = ((CMainFrame*)theApp.m_pMainWnd)->GetObjectView();
						pObjectView->PostMessage(WM_USER,
							MAKELONG(VDH_SELECT_SEQUENCE, GetServer()->GetID()),
							MAKELONG(m_SequenceRecord.GetArchiveNr(), m_SequenceRecord.GetSequenceNr()));

						m_iPictureRequests++;
						bRet = TRUE;
					}
					else
					{
						m_PlayStatus = PS_STOP;
					}
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
	}
	TRACE(_T("Navigate(): m_iPictureRequests = %i\n"), m_iPictureRequests);
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnViewOriginal() 
{
	m_b1to1 = !m_b1to1;
	if (   m_pPictureRecord 
		&& m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
	{
		m_H263.SetCaps(H263_FIT,!m_b1to1);
	}
	if (m_b1to1)
	{
		OnDisplayZoomOut();
	}
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);

	if (m_pServer->CanRectangleQuery())
	{
		pCmdUI->Enable(m_bImageOriginalSize);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord != NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnFilePrint() 
{
	m_pRechercheView->OnFilePrint();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDisplay1() 
{
	m_nDibData = 1;
	m_b1to1 = FALSE;
	EnsureDibQueueNotEmpty();
	m_ToolTip.Activate(FALSE);
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateDisplay1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pRechercheView && m_pRechercheView->m_iXWin<3 && m_pPictureRecord);
	pCmdUI->SetCheck(m_nDibData == 1);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDisplay4() 
{
	m_nDibData = 2;
	m_b1to1 = FALSE;
	EnsureDibQueueNotEmpty();
	m_ToolTip.Activate(TRUE);
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateDisplay4(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord!=NULL);
	pCmdUI->SetCheck(m_nDibData == 2);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDisplay9() 
{
	m_nDibData = 3;
	m_b1to1 = FALSE;
	EnsureDibQueueNotEmpty();
	m_ToolTip.Activate(TRUE);
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateDisplay9(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord!=NULL);
	pCmdUI->SetCheck(m_nDibData == 3);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::EnsureDibQueueNotEmpty()
{
	if (m_pPictureRecord && m_DibDatas.GetSize()==0)
	{
		CSize s = GetPictureSize();
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			HBITMAP hBitmap = m_Jpeg.CreateBitmap();
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
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			WORD wSeqNr = m_SequenceRecord.GetSequenceNr();
			CDibData* pDibData = new CDibData(m_H263.GetDib(),GetCurrentRecordNr(),
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
			CDibData* pDibData = new CDibData(new CDib(m_PTDecoder.GetBitmapHandle()),
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
void CDisplayWindow::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord != NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnEditCopy() 
{
	if (m_pPictureRecord) 
	{
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			CSize s = GetPictureSize();
			HBITMAP hBitmap = m_Jpeg.CreateBitmap();
			if (hBitmap)
			{
				if (OpenClipboard())
				{
					EmptyClipboard();
					if (!SetClipboardData (CF_BITMAP, (HANDLE) hBitmap))
						DeleteObject(hBitmap);
					CloseClipboard();
				}
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			CDib* pDib;
			pDib = m_H263.GetDib();
			if (pDib)
			{
				pDib->CopyToClipBoard();
				WK_DELETE(pDib);
			}
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			CDib dib(m_PTDecoder.GetBitmapHandle());
			dib.IncreaseTo24Bit();
			dib.CopyToClipBoard();
		}
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnEditComment() 
{
	CCommentDialog dlg(this);

	dlg.m_sComment = m_sComment;
	if (IDOK==dlg.DoModal())
	{
		m_sComment = dlg.m_sComment;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
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
CDibData* CDisplayWindow::HittestDibDataToolTip(CPoint pt)
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
			return m_DibDatas.GetAt(i);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnMouseMove(UINT nFlags, CPoint point) 
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

	CImageWindow::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateFileDib(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   && theApp.m_pPermission 
				   && (theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateFileDisk(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateFileHtml(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateFileJpg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateFileJpx(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPictureRecord
				   &&theApp.m_pPermission && 
		(theApp.m_pPermission->GetFlags() & WK_ALLOW_DATA_EXPORT));
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnLButtonDblClk(UINT nFlags, CPoint point) 
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
		CImageWindow::OnLButtonDblClk(nFlags, point);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDisplayZoomOut() 
{
	if (m_pPictureRecord)
	{
		// switch off zoom
		m_rcZoom = CRect(0, 0, 0, 0);
		TRACE(_T("TKR OnDisplayZoomOut() ############################# m_rcZoom: top: %i  left: %i Button: %i right: %i\n"),
									m_rcZoom.top, m_rcZoom.left, m_rcZoom.bottom, m_rcZoom.right);
		InvalidateRect(NULL);
		m_iCountZooms = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateDisplayZoomOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord && ScrollDisplayWindow());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnLButtonDown(UINT nFlags, CPoint point) 
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
					//das unver‰nderte Rectangle sichern
					m_pQueryRectTracker->SaveLastRect();
					
					m_pQueryRectTracker->Track(this, point, TRUE);

					m_pQueryRectTracker->Draw(this->GetDC());
					
					//hat sich die Lage des QueryRectangle ge‰ndert
					//berechne relative Promillewerte des Lage neu

					if(m_pQueryRectTracker->m_rect != m_pQueryRectTracker->GetLastRect())
					{
						m_pQueryRectTracker->SetRectPromille(m_rcZoom);
					}
					RedrawWindow();
				}
				else
				{	
					if (!ScrollDisplayWindow(point))
					{
						if(m_iCountZooms < 3)
						{
							ZoomDisplayWindow(point);
						}
						else
						{
							OnDisplayZoomOut();
						}
					}
				}
			}
		}
		else 
		{
			// Liegt der Punkt im Videorect?
			if (rcVideoClient.PtInRect(point))
			{
				if (!ScrollDisplayWindow())
				{
					if(m_iCountZooms < 3)
					{
						ZoomDisplayWindow(point);
					}
					else
					{
						OnDisplayZoomOut();
					}
				}
			}
		}
	}
	


	CImageWindow::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ZoomDisplayWindow(const CPoint &point)
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
BOOL CDisplayWindow::ScrollDisplayWindow(CPoint point /* = (0,0) */)
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

					//Rechteck aufziehen nur mˆglich bei Ansicht "1 Bild", nicht bei Ansicht
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
			
/*						TRACE(_T("** tkr ScrollDisplayWindow: rcClient        left: %i  top: %i  right: %i  bottom: %i\n"),
						rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						TRACE(_T("** tkr ScrollDisplayWindow: rcVideoClient   left: %i  top: %i  right: %i  bottom: %i\n"),
						rcVideoClient.left, rcVideoClient.top, rcVideoClient.right, rcVideoClient.bottom);
*/
						
						//noch kein rectangle vorhanden
						if(!m_pQueryRectTracker->GetRect()->IsRectNull())
						{
							m_pQueryRectTracker->SetRectEmpty();
							InvalidateRect(NULL);
							UpdateWindow();
						}
						ShowCursor(TRUE);
						m_pQueryRectTracker->DrawNewRect(point);
						m_pQueryRectTracker->SetRectPromille(m_rcZoom);
						ShowCursor(FALSE);

						//Setzt in CQueryParameter die Rechteckparameter
						theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());

						//Sind mehrere SmallWindows geˆffnet und befindet sich auf einem anderen 
						//SmallWindow als das gerade aktuelle (neues Rechteck) ebenfalls ein Rechteck
						//dieses "alte" Rechteck lˆschen
						CSmallWindow* pSW = NULL;
						CSecID sCurrentID = GetID();
						int iSize = m_pRechercheView->GetSmallWindows()->GetSize();
						
						if(iSize > 1)
						{
							CDisplayWindow* pDW = NULL;
							for(int i=0; i < iSize; i++)
							{
								pSW = m_pRechercheView->GetSmallWindows()->GetAt(i);
								if(pSW)
								{
									pDW = (CDisplayWindow*)pSW;
									if(pDW && !pDW->GetRectToQuery().IsRectEmpty())
									{
										if(pDW->GetID() != sCurrentID)
										{
											pSW->ResetTracker();
											pSW->InvalidateRect(NULL);
											pSW->UpdateWindow();		
										}
									}
								}
							}
						}

						//War der Suchdialog geˆffnet, diesen bez¸glich des Rechtecks aktualisieren 
						if(m_pRechercheView)
						{
							m_pRechercheView->ShowHideRectangle();
						}
					}
				}
			}
			else
			{
				if(m_pQueryRectTracker && !m_pQueryRectTracker->GetRect()->IsRectEmpty())
				{
					//zeichnet Rechteck neu, wenn im Bild gescrollt wird
					TRACE(_T("TKR ------ ScrollDisplayWindow() gezoomt: Recalc\n"));
					m_pQueryRectTracker->RecalcNewRectPositionFromPromille(m_rcZoom);
				}
			}

			InvalidateRect(NULL);
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
			
			InvalidateRect(NULL);
			UpdateWindow();
		}
	}
	
	ShowCursor(TRUE);

	return (StartPoint != CurrentPoint);

}

/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_pQueryRectTracker)
	{
		if (pWnd == this && m_pQueryRectTracker->SetCursor(this, nHitTest))
		{
			return TRUE;
		}
	}
	return CImageWindow::OnSetCursor(pWnd, nHitTest, message);
}
/////////////////////////////////////////////////////////////////////////////
 void CDisplayWindow::DrawRectToQuery(CDC* pDC)
{
 	CRect rcAbsolute, rcClient, rcVideoClient, rcPromille;

 	GetClientRect(rcClient);
 	GetVideoClientRect(rcVideoClient);
 	rcClient.NormalizeRect();
	rcVideoClient.NormalizeRect();

	//QueryRectangle mit Promille-Daten nur dann neu berechnen, 
	//wenn sich die Fenstergrˆsse des DisplayWindows (des DB-Bildes)
	//ge‰ndert hat
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
//set m_bImageOriginalSize to true or false
BOOL CDisplayWindow::CanShowImageInOriginalSize()
{
	//kann das Originalbild in voller Grˆﬂe angezeigt werden?
	BOOL bRet = FALSE;

	CRect rcFrame;
	GetFrameRect(rcFrame);
	m_bImageOriginalSize = FALSE;

	if(GetPictureSize().cx > 0 && GetPictureSize().cy > 0)
	{
		if(rcFrame.Width() >= GetPictureSize().cx && rcFrame.Height() >= GetPictureSize().cy)
		{
			m_bImageOriginalSize = TRUE;
			bRet = m_bImageOriginalSize;
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnSize(UINT nType, int cx, int cy) 
{
	CImageWindow::OnSize(nType, cx, cy);

	CanShowImageInOriginalSize();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::GetTimeStamp(CSystemTime& t) const
{
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
BOOL CDisplayWindow::SetMDValues(WORD wX, WORD wY)
{
	m_wMD_X = wX;
	m_wMD_Y = wY;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//wird nur bei REchtecksuche verwendet
BOOL CDisplayWindow::SetAllMDValues(WORD wX, WORD wY)
{
	m_waAllMD_X.Add(wX);
	m_waAllMD_Y.Add(wY);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//wird nur bei REchtecksuche verwendet
BOOL CDisplayWindow::RemoveAllMDValues()
{
	m_waAllMD_X.RemoveAll();
	m_waAllMD_Y.RemoveAll();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//liegen alle Kreuze auﬂerhalb des Rechtecks ?
BOOL CDisplayWindow::AllMDsOutsideRect(int iAllMDs, CRect rcRectangle)
{
	BOOL bRet = TRUE;
	
	for(int i=0; i<iAllMDs; i++)
	{
		if ((m_waAllMD_X.GetAt(i) > 0) && (m_waAllMD_Y.GetAt(i) > 0))
		{
			CPoint point((int)m_waAllMD_X.GetAt(i), (int)m_waAllMD_Y.GetAt(i)); 
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
BOOL CDisplayWindow::IsAlarm()
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
PlayStatus CDisplayWindow::GetPlayStatus()
{
	return m_PlayStatus;
}

/////////////////////////////////////////////////////////////////////////////
CPlayRealTime* CDisplayWindow::GetPlayRealTime()
{
	return m_pPlayRealTime;
}
/////////////////////////////////////////////////////////////////////////////
long CDisplayWindow::OnPlayRealTime(WPARAM wParam, LPARAM lParam)
{

	WORD wArchivNr		= 0;
	WORD wSequenceNr	= 0;
	DWORD dwRecordNr	= 0;
	DWORD dwNrOfRecords	= 0;
	const CIPCPictureRecord* ppict = NULL;
	int iNumFields		= 0; 
	const CIPCFields* pfields = NULL;
	
	m_pPlayRealTime->GetRealTimePicture(wArchivNr, wSequenceNr, dwRecordNr, dwNrOfRecords,
									    ppict, iNumFields, pfields);
	
	//TODO TKR fieldarray dynamisch an grˆﬂe anpassen
	CIPCField fields[100];

	if(iNumFields > 0)
	{
		fields[1] = *pfields->GetAt(0); 

		for(int i = 2; i < iNumFields; i++)
		{
			fields[i] = *pfields->GetAt(i-1);
		}
	}

	PictureData(dwRecordNr,dwNrOfRecords, *ppict, iNumFields,fields);
//	RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,*ppict,iNumFields,fields);

	return 0;

}
//////////////////////////////////////////////////////////////////////////
void CDisplayWindow::UpdateAudioState(CIPCAudioRecherche*pAudio)
{
	CIPCAudioRecherche*pLocalAudio = m_pServer->GetDocument()->GetLocalAudio();
	if (   (pLocalAudio == NULL && pAudio != NULL)
		|| m_PlayStatus != PS_PLAY_FORWARD && pAudio == pLocalAudio)
	{
		m_bAudioBusy = !pAudio->CanDecode();
		PostMessage(WM_COMMAND, ID_UPDATE_DIALOGS);
	}
// TRACE(_T("### RKE Test: UpdateAudioState:%s: %d, %d, %s\n"), NameOfEnum(m_PlayStatus), pLocalAudio, pAudio, m_sArchivName);
}
//////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnTimer(UINT nIDEvent) 
{
	CImageWindow::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetSyncPlayStatus(PlayStatus ps)
{
	CIPCAudioRecherche* pAudio = m_pServer->GetDocument()->GetLocalAudio();
	if (ps == PS_PLAY_FORWARD)
	{
		m_bSetSyncPoint = TRUE;
		if (pAudio)
		{
			if (   pAudio->GetActiveDisplayWindow() != NULL
				&& pAudio->GetActiveDisplayWindow() != this)
			{
				m_bAudioBusy = TRUE;
			}
			PostMessage(WM_COMMAND, ID_UPDATE_DIALOGS);
		}
	}

//	TRACE(_T("### RKE Test: SetSyncPlayStatus:%s, %d, %d, %s\n"), NameOfEnum(ps), this, pAudio->GetActiveDisplayWindow(), m_sArchivName);
	m_PlayStatus = ps;
}
