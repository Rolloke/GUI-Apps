// DisplayWindow.cpp : implementation file
//

#include "stdafx.h"
#include "winspool.h"
#include "dvclient.h"
#include "DisplayWindow.h"
#include "mainframe.h"
#include "CPanel.h"
#include "Server.h"

#include "LiveWindow.h"
#include "PlayWindow.h"
#include ".\displaywindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CDisplayWindow, CWnd)

#define MOTION_DETECTION_TIMEOUT 3500	// timeout value for MD indication in ms

int  CDisplayWindow::m_bShowTargetCross = TRUE;

/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow
CDisplayWindow::CDisplayWindow(CMainFrame* pParent, CServer* pServer)
{
	m_pMainFrame = pParent;
	m_pServer = pServer;
	m_pPictureRecord = NULL;
	m_iRequests = 0;
	m_dwLastRequest = GetTickCount() - REQUEST_TIMEOUT_SLOW;
	m_dwWaitTimeout = 0;
	m_bIsActive = FALSE;
	m_pJpeg = NULL;
	m_pMpeg = NULL;
	m_wMD_X	= 0;
	m_wMD_Y	= 0;
	m_bMD	= FALSE;
	m_dwLastMD_TC = GetTickCount();
	m_pTracker = NULL;

	m_rcZoom = CRect(0,0,0,0);
	m_bCanRectSearch = TRUE;
	m_bWasSomeTimesActive = FALSE;

	m_dwLastPictureReceivedTC = GetTickCount();

#ifndef _DEBUG
	if (pServer)
	{
		m_bCanRectSearch = pServer->CanRectangleQuery() || theApp.IsReadOnly();
		if (m_bCanRectSearch)
		{
			m_bCanRectSearch = theApp.CanRectangleQuery();

		}
	}
#endif

	CPanel *pPanel = theApp.GetPanel();
	if (pPanel)
	{
		pPanel->SetCanRectSearch(m_bCanRectSearch);
	}


	//init for rectangle search
	if(m_bCanRectSearch)
	{
		m_pTracker	= new CQueryRectTracker(this);
	}
	else
	{
		m_pTracker = NULL;
	}


}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow::~CDisplayWindow()
{
	DeletePicture(FALSE);
	WK_DELETE(m_pTracker);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDestroy()
{
	DestroyJpegDecoder();
	DestroyMpegDecoder();
	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDisplayWindow, CWnd)
	//{{AFX_MSG_MAP(CDisplayWindow)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER, OnUser)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CSecID CDisplayWindow::GetID() const
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
	return SECID_NO_ID;
}
/////////////////////////////////////////////////////////////////////////////
CString	CDisplayWindow::GetName() const
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetTitle(CDC* pDC)
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::GetFooter(CByteArray& Array)
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDisplayWindow::GetDefaultText()
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CDisplayWindow::GetOSDColor()
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
	return RGB(0,0,0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::IsShowable()
{
	WK_TRACE(_T("%s not overridden\n"),_T(__FUNCTION__));
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::Request(int iMax/*=-1*/)
{
	if (iMax == -2)
	{
		if ((GetTickCount() - m_dwLastMD_TC) > MOTION_DETECTION_TIMEOUT)
		{
			m_bMD = FALSE;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetActive(BOOL bActive)
{
	m_bIsActive = bActive;
	if (m_bIsActive)
	{
		SetActiveWindowToAnalogOut();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::Create(const RECT& rect, CWnd* pParentWnd)
{
	m_ilSymbols.Create(IDB_SYMBOLS, 16, 0, SKIN_COLOR_KEY);

	return CWnd::CreateEx(0, _T("DVDisplay"), GetName(),	WS_CHILD |
														WS_CLIPCHILDREN |
														WS_CLIPSIBLINGS |
														CS_DBLCLKS,
														rect,
														pParentWnd,
														(int)GetID().GetID());

}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::CreateJpegDecoder()
{
	if (m_pJpeg == NULL)
	{
		m_pJpeg = new CJpeg();

/*		
		CSize size = m_pJpeg->GetImageDims();
		m_pJpeg->SetSelectionRect(CRect(0,0, 360, 288));			    
		m_pJpeg->EnableLuminanceCorrection();

		m_pJpeg->EnableHistogrammCorrection();
		MATRIX3X3 Matrix;
		Matrix.nKoeff[0] = -1;
		Matrix.nKoeff[1] = -1;
		Matrix.nKoeff[2] = -1;
		Matrix.nKoeff[3] = -1;
		Matrix.nKoeff[4] = 9;
		Matrix.nKoeff[5] = -1;
		Matrix.nKoeff[6] = -1;
		Matrix.nKoeff[7] = -1;
		Matrix.nKoeff[8] = -1;
		m_pJpeg->SetFilterMatrix(Matrix, 1);
		m_pJpeg->EnableFilter();
		m_pJpeg->EnableNoiseReduction();
*/		
		CSize size = m_pJpeg->GetImageDims();
		m_pJpeg->SetSelectionRect(CRect(0,0, 720, 560));			    


		if(!theApp.IsReadOnly())
		{
			CWK_Profile prof;
			// LuminanceCorrectionfilter aktivieren?
			BOOL bLumCorrection = prof.GetInt(REGKEY_DVCLIENT, _T("LuminanceCorrection"), 0);
			prof.WriteInt(REGKEY_DVCLIENT, _T("LuminanceCorrection"),bLumCorrection);
			if (bLumCorrection)
				m_pJpeg->EnableLuminanceCorrection();
			else
				m_pJpeg->DisableLuminanceCorrection();
		}

#if (0)
		// NoiseReductionfilter aktivieren?
		BOOL bNoiseReduction = prof.GetInt(REGKEY_DVCLIENT, _T("NoiseReduction"), 0);
		prof.WriteInt(REGKEY_DVCLIENT, _T("NoiseReduction"),bNoiseReduction);
		if (bNoiseReduction)
			m_pJpeg->EnableNoiseReduction();
		else
			m_pJpeg->DisableNoiseReduction();
#else
		if (GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			// Mindestens 1GHz
			if (theApp.GetCPUClockFrequency() >= 1000)
			{
				m_pJpeg->EnableNoiseReduction();
				WK_TRACE(_T("Enable Noise reduction (CPUClock is %dMHz)\n"), theApp.GetCPUClockFrequency());
			}
			else
			{
				m_pJpeg->DisableNoiseReduction();
			}
		}
#endif

									    
		CByteArray Array;
		Array.Add(0);
	//	Array.Add(1);										  
	//	Array.Add(2);
		Array.Add(3);
		m_pJpeg->SetImageList(50, 50, &m_ilSymbols, Array);
		CFont font1;
//		font1.CreatePointFont(10*10, _T("Arial"));
		font1.CreatePointFont(10*10, GetVarFontFaceName());
		m_pJpeg->SetFont1(font1);
		CFont font2;
		font2.CreatePointFont(12*10, _T("Wingdings"));
		m_pJpeg->SetFont2(font2);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DestroyJpegDecoder()
{
	WK_DELETE(m_pJpeg);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::CreateMpegDecoder()
{
	if (m_pMpeg == NULL)
	{
//		m_pMpeg = new CMPEG4Decoder();
		if (theApp.GetIntValue(_T("UseAVCodec"), 0))
		{
			try
			{
				// Versuch den AVCodec zu ˆffnen. Falls dies fehlschl‰gt, wird der emuzed decoder verwendet.
				m_pMpeg = new CAVCodec();
				WK_TRACE(_T("Using AVCodec to decoded mpeg4\n"));
			}
			catch(...)
			{
				m_pMpeg = new CMPEG4Decoder;
				WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
			}
		}
		else
		{
			m_pMpeg = new CMPEG4Decoder;
			WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
		}

		m_pMpeg->Init(NULL,0,0,0);
								    
		CByteArray Array;
		Array.Add(0);
	//	Array.Add(1);										  
	//	Array.Add(2);
		Array.Add(3);
		
		m_pMpeg->SetImageList(50, 50, &m_ilSymbols, Array);
		CFont font1;
		font1.CreatePointFont(10*10, GetVarFontFaceName());
		m_pMpeg->SetFont1(font1);
		CFont font2;
		font2.CreatePointFont(12*10, _T("Wingdings"));
		m_pMpeg->SetFont2(font2);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DestroyMpegDecoder()
{
	WK_DELETE(m_pMpeg);
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DeletePicture(BOOL bRedraw)
{
	m_csPictureRecord.Lock();
	WK_DELETE(m_pPictureRecord);
	DestroyJpegDecoder();
	DestroyMpegDecoder();
	m_csPictureRecord.Unlock();
	if (bRedraw)
	{
		DoRedraw();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::PictureData(const CIPCPictureRecord *pPict,DWORD dwX, DWORD dwY)
{
	if (m_hWnd == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	m_dwLastPictureReceivedTC = GetTickCount();
	// ML 10.06.2004 Zeitpunkt des letzten Bildes merken, um ggf. neue Requests zu generieren, falls
	// die vorherigen Requests keine Bilder liefern sollten. Es werden z.Z. max 5 Requests
	// abgesetzt. Der DVClient wartet dann bis in alle Ewigkeit auf diese Bilder. Deshalb wird
	// nun mittels 'm_dwLastPictureReceivedTC' der Zeitpunkt des letzten g¸ltigen Bildes vermerkt, um
	// dann ggf. (nach  z.B. 1 Minute) wieder neue Requests zu generieren.

	/*TRACE(_T("picture %d %08lx %s,%d %s\n"),
		pict.GetBlockNr(),GetID().GetID(),
		pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().wMilliseconds,
		(pict.GetPictureType()==SPT_FULL_PICTURE)?_T("I"):_T("P"));*/
	// Achtung in CIPCThread !
	BOOL bRet = TRUE;

	if (pPict)
	{
		CAutoCritSec acs(&m_csPictureRecord);
		if (m_pPictureRecord)
		{
			if (  (pPict->GetBlockNr() == 0) 
				||(pPict->GetBlockNr() != m_pPictureRecord->GetBlockNr())
				)
			{
				// 18.03.2005 ML: m_pPictureRecord darf nicht NULL werden, da
				// ansonsten in 'OnEraseBkgnd' das letzte Bild mit schwarz ¸bermalt wird.
				// Dies war die Ursache des flackerns.
				CIPCPictureRecord* picTemp = m_pPictureRecord;
				m_pPictureRecord = new CIPCPictureRecord(*pPict);
				WK_DELETE(picTemp);
			}
			else
			{
				WK_TRACE(_T("SAME picture %d %08lx %s,%d %s,%d %s\n"),
					pPict->GetBlockNr(),GetID().GetID(),
					pPict->GetTimeStamp().GetTime(),pPict->GetTimeStamp().wMilliseconds,
					m_pPictureRecord->GetTimeStamp().GetTime(),
					m_pPictureRecord->GetTimeStamp().wMilliseconds,
					(pPict->GetPictureType()==SPT_FULL_PICTURE)?_T("I"):_T("P"));
				bRet = FALSE; 
			}
		}
		else
		{
			m_pPictureRecord = new CIPCPictureRecord(*pPict);
		}
		acs.Unlock();

		if ((int)dwX>0 && (int)dwY>0)
		{
			SetMDValues((WORD)dwX, (WORD)dwY);
		}
		else
		{	// the values are only set to zero if the MD timeout is over
			SetMDValues(0, 0);
		}
		switch (pPict->GetCompressionType())
		{
			case COMPRESSION_JPEG: case COMPRESSION_YUV_422: case COMPRESSION_RGB_24:
				CreateJpegDecoder();
				break;
			case COMPRESSION_MPEG4:
				CreateMpegDecoder();
				break;
		}
	}


	if (m_iRequests > 0)
	{
		m_iRequests--;
	}
	else
	{
		TRACE(_T("image without request %d %d\n"),GetID().GetSubID(),m_iRequests);
	}


	if (GetID().GetSubID() == 0)
	{
		//TRACE(_T("%d Request=%d CONF\n"), GetID().GetSubID(),m_iRequests);
	}


	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::operator < (const CDisplayWindow& s)
{
	if (GetType() == DISPLAYWINDOW_LIVE)	// IsKindOf(RUNTIME_CLASS(CLiveWindow)))
	{
		if (s.GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
		{
			return GetID().GetID() < s.GetID().GetID();
		}
		else if (s.GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			return TRUE;
		}
	}
	else if (GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
	{
		if (s.GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
		{
			return FALSE;
		}
		else if (s.GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			return (GetID().GetID() & 0x00FF) < (s.GetID().GetID() & 0x00FF);
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SetMDValues(WORD wX, WORD wY)
{
	m_csPictureRecord.Lock();

	DWORD dwTickCount = GetTickCount();
	if (wX > 0 || wY > 0)
	{
		m_wMD_X = wX;
		m_wMD_Y = wY;
		m_dwLastMD_TC = dwTickCount;
		if (m_bMD == FALSE)
		{
			m_bMD = TRUE;
			Request();
		}
	}
	else if ((dwTickCount - m_dwLastMD_TC) > MOTION_DETECTION_TIMEOUT)
	{
		m_bMD = FALSE;
		m_wMD_X = 0;
		m_wMD_Y = 0;
	}

	m_csPictureRecord.Unlock();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnChangeVideo() 
{
	m_csPictureRecord.Lock();
	BOOL bRedraw = TRUE;
	if (m_pPictureRecord)
	{
		// decode the image
		switch (m_pPictureRecord->GetCompressionType())
		{
			case COMPRESSION_JPEG:
			if (m_pJpeg)
			{
				m_pJpeg->DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(),
													m_pPictureRecord->GetDataLength());
			}
			break;
			case COMPRESSION_RGB_24:
			case COMPRESSION_YUV_422:
			if (m_pJpeg)
			{
				m_pJpeg->SetDIBData((LPBITMAPINFO)m_pPictureRecord->GetData());
			}
			break;
			case COMPRESSION_MPEG4:
				bRedraw = TRUE;
			break;
		}
	}
	m_csPictureRecord.Unlock();

	// redraw the window
	if (bRedraw)
	{
		OnRedrawVideo();
	}

	if (m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
	{
		InitializeTracker();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DoRedraw()
{
	PostMessage(WM_USER, ID_REDRAW_VIDEO);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnRedrawVideo() 
{
	m_bWasSomeTimesActive = TRUE;
	if (GetType() == DISPLAYWINDOW_LIVE)
	{
		CWindowDC dc(this);
		OnDraw(&dc);
	}
	else
	{
		// redraw the window
		Invalidate();
		UpdateWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	OnDraw(&dc);		  
}
/////////////////////////////////////////////////////////////////////////////
// ML: Die beiden Klassen CJpeg und CMPEG4Decoder sollten von einer Gemeinsamen
//     Basisklasse z.B. CDecoder abgeleitet werden, die alle gemeinsamkeiten enth‰lt,
//     dadurch w‰re viel doppelter Code vermeidbar.
#if (1)
void CDisplayWindow::OnDraw(CDC* pDC)
{
	if (m_pPictureRecord)
	{
		CompressionType cT = m_pPictureRecord->GetCompressionType();
		if ((cT == COMPRESSION_JPEG) ||	(cT == COMPRESSION_RGB_24) || (cT == COMPRESSION_YUV_422))
		{
			DrawJpeg(pDC);
		}
		else if (cT == COMPRESSION_MPEG4)
		{
			DrawMpeg4(pDC);
		}
		else
		{
			WK_TRACE_WARNING(_T("CDisplayWindow::OnDraw: Unknown CompressionType (%d)"), cT);
		}
	}
	else
	{
		DrawDefaultText(pDC);
	}
}
#else
void CDisplayWindow::OnDraw(CDC* pDC)
{
	CRect		rcWnd(0,0,0,0);
	CBitmap*	pOldBitmap = NULL;
	CBitmap		Bitmap;
	CDC			memDC;

	// Offscreensurface anlegen
	memDC.CreateCompatibleDC(pDC);
	GetClientRect(rcWnd);

	// Kompatible Bitmap mit dem Offscreensurface verbinden
	Bitmap.CreateCompatibleBitmap(pDC, rcWnd.Width(), rcWnd.Height());
	pOldBitmap = memDC.SelectObject(&Bitmap);

	if (m_pPictureRecord)
	{
		CompressionType cT = m_pPictureRecord->GetCompressionType();
		if ((cT == COMPRESSION_JPEG) ||	(cT == COMPRESSION_RGB_24) || (cT == COMPRESSION_YUV_422))
		{
			DrawJpeg(&memDC);
		}
		else if (cT == COMPRESSION_MPEG4)
		{
			DrawMpeg4(&memDC);
		}
		else
			WK_TRACE_WARNING(_T("CDisplayWindow::OnDraw: Unknown CompressionType (%d)"), cT);
	}
	else
	{
		DrawDefaultText(&memDC);
	}
 
	pDC->BitBlt(rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	Bitmap.DeleteObject();
	memDC.DeleteDC();
}
#endif

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawJpeg(CDC* pDC)
{
	if (m_pPictureRecord && m_pJpeg && m_pJpeg->IsValid())
	{
		CRect rect;
		GetClientRect(rect);
		m_pJpeg->SetBackgroundColor(GetOSDColor());
		m_pJpeg->SetOSDText1(0,0,GetTitle(pDC));
		
		CByteArray Array;
		GetFooter(Array);
		m_pJpeg->SetImageList(0, rect.bottom-16, &m_ilSymbols, Array);

		// Aktives Fenster hervorheben.
		if (IsActive())
			m_pJpeg->SetTextColor(RGB(255,255,255));
		else
			m_pJpeg->SetTextColor(RGB(200,200,200));

		BOOL bPrinting = pDC->m_bPrinting;
		if (   m_rcZoom.bottom
			&& GetType() == DISPLAYWINDOW_PLAY)
		{	// RKE: Beim Drucken wird StretchBlit(HALFTONE) verwendet.
			pDC->m_bPrinting = TRUE;
		}

		m_pJpeg->OnDraw(pDC, rect, m_rcZoom);

		pDC->m_bPrinting = bPrinting;

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			//im LiveWindow nur ein einziges Kreuz zeichnen
			if(GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
			{
				DrawCross(pDC,m_wMD_X,m_wMD_Y);
			}
			else
			{
				// Im Wiedergabefenster mehrere Kreuze zeichnen
				DoDrawAllCrosses(pDC);
			}

			if(m_pTracker)
			{
				if(!m_pTracker->GetRect()->IsRectEmpty())
				{
					m_pTracker->Draw(pDC);
				}
			}
		}
		else
		{
			DrawCross(pDC,m_wMD_X,m_wMD_Y);
		}
	}
	else
	{
		DrawDefaultText(pDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawMpeg4(CDC* pDC)
{
	if (m_pPictureRecord && m_pMpeg)
	{
		CRect rect;
		GetClientRect(rect);
		m_pMpeg->SetBackgroundColor(GetOSDColor());
		m_pMpeg->SetOSDText1(0,0,GetTitle(pDC));
		
		CByteArray Array;
		GetFooter(Array);
		 m_pMpeg->SetImageList(0, rect.bottom-16, &m_ilSymbols, Array);

		// Aktives Fenster hervorheben.
		if (IsActive())
			m_pMpeg->SetTextColor(RGB(255,255,255));
		else
			m_pMpeg->SetTextColor(RGB(200,200,200));

		WORD x,y;
		x = m_wMD_X;
		y = m_wMD_Y;
		BOOL bPrinting = pDC->m_bPrinting;
		if (   m_rcZoom.bottom
			&& GetType() == DISPLAYWINDOW_PLAY)
		{	// RKE: Beim Drucken wird StretchBlit(HALFTONE) verwendet.
			pDC->m_bPrinting = TRUE;
		}
		
		m_pMpeg->OnDraw(pDC, rect, m_rcZoom);

		pDC->m_bPrinting = bPrinting;

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			//im LiveWindow nur ein einziges Kreuz zeichnen
			if(GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
			{
				DrawCross(pDC,x,y);
			}
			else
			{
				// Im Wiedergabefenster mehrere Kreuze zeichnen
				DoDrawAllCrosses(pDC);
			}

			if(m_pTracker)
			{
				if(!m_pTracker->GetRect()->IsRectEmpty())
				{
					m_pTracker->Draw(pDC);
				}
			}
		}
		else
		{
			DrawCross(pDC,x,y);
		}
	}
	else
	{
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawDefaultText(CDC* pDC)
{
	CFont font;
	font.CreatePointFont(10*10, GetFixFontFaceName(), pDC);

	pDC->SaveDC();
	pDC->SelectObject(&font);
	CRect rect;
	GetClientRect(rect);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(GetDefaultText(),rect,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
	pDC->RestoreDC(-1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::OnEraseBkgnd(CDC* pDC) 
{
	if (m_pPictureRecord == NULL)
	{
		CBrush b;
		CRect rect;

		b.CreateSolidBrush(RGB(0,0,0));
		GetClientRect(rect);
		pDC->FillRect(rect,&b);
		b.DeleteObject();
	}
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawCross(CDC* pDC, WORD wX, WORD wY)
{
	// Ist das Fadenkreuz eingeschaltet UND sind g¸ltige Koordinaten vorhanden?
//	TRACE(_T("Coordinates:%d, %d\n"), wX, wY);
	if (m_bShowTargetCross	&& (wX > 0)	&& (wY > 0))
	{
		// Den Zoom ber¸cksichtigen.
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
		GetClientRect(rect);
		int x,y;
		x = (wX * rect.Width()) / 1000 + rect.left;
		y = (wY * rect.Height()) / 1000 + rect.top;
		
		
		CPen pen;
		CPen* pOldPen;
		
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
void CDisplayWindow::DrawAllCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col,
									BOOL bLines /* = FALSE */)
{
	// Ist das Fadenkreuz eingeschaltet UND sind g¸ltige Koordinaten vorhanden?
	if (m_bShowTargetCross	&& (wX > 0)	&& (wY > 0))
	{
		// Den Zoom ber¸cksichtigen.
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
		GetClientRect(rect);
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
CSize CDisplayWindow::GetPictureSize()
{
	CSize s(0,0);
	if (m_pPictureRecord)
	{
		CompressionType cT = m_pPictureRecord->GetCompressionType();
		if ((cT == COMPRESSION_JPEG) ||	(cT == COMPRESSION_RGB_24) || (cT == COMPRESSION_YUV_422))
		{
			if (m_pJpeg && m_pJpeg->IsValid())
				s = m_pJpeg->GetImageDims();
		}
		else if (cT == COMPRESSION_MPEG4)
		{
			if (m_pMpeg)
				s = m_pMpeg->GetImageDims();
		}
		else
			WK_TRACE_WARNING(_T("CDisplayWindow::GetPictureSize unknown CompressionType\n"));
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CIPCPictureRecord* CDisplayWindow::GetPictureCopy()
{
	CIPCPictureRecord* pRet = NULL;

	if (m_pPictureRecord)
	{
		m_csPictureRecord.Lock();
		pRet = new CIPCPictureRecord(*m_pPictureRecord);
		m_csPictureRecord.Unlock();
	}
	return pRet;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetTimeString()
{	
	CString sRet;
	
	m_csPictureRecord.Lock();
	if (m_pPictureRecord)
	{
		sRet = m_pPictureRecord->GetTimeStamp().GetTime();
		if (theApp.ShowMilliseconds())
		{
			CString ms;
			ms.Format(_T(",%03d"),m_pPictureRecord->GetTimeStamp().GetMilliseconds());
			sRet += ms;
		}
	}
	m_csPictureRecord.Unlock();
	return sRet;
}

/////////////////////////////////////////////////////////////////////////////
CSystemTime CDisplayWindow::GetTimeStamp()
{	
	CSystemTime t;
	m_csPictureRecord.Lock();
	if (m_pPictureRecord)
	{
		t = m_pPictureRecord->GetTimeStamp();
	}
	else
	{
		t.GetLocalTime();
	}
	m_csPictureRecord.Unlock();
	return t; 
}

/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetDateString()
{		    
	CString s;
	
	m_csPictureRecord.Lock();

	if (m_pPictureRecord)
	{
		s = m_pPictureRecord->GetTimeStamp().GetDate();
	}
	m_csPictureRecord.Unlock();
	return s;
}

/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetShortDateString()
{
	CString s;
	
	m_csPictureRecord.Lock();

	if (m_pPictureRecord)
	{
		s.Format(_T("%02d.%02d"),m_pPictureRecord->GetTimeStamp().GetDay(),
			m_pPictureRecord->GetTimeStamp().GetMonth());
	}
	m_csPictureRecord.Unlock();
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetPrintTitle()
{	
	CString s;
	s = GetName();
	s += _T(" / ") + GetDateString();
	s += _T(" / ") + GetTimeString();

	return s;
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CPanel* pPanel = theApp.GetPanel();
	// Focus wieder auf das Panel
	if (pPanel)
	{
		pPanel->ActivatePanel();
	}
	
	CWnd::OnMButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPanel* pPanel = theApp.GetPanel();
	CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
	BOOL bBackupWnd = FALSE;

	if(pBW)
	{
		bBackupWnd = GetName() == pBW->GetName();
	}

	//Rechtecksuche und Zoom nicht im Backup Window mˆglich
	if (point.y > 16 && !bBackupWnd)
	{
		if (!ScrollDisplayWindow(point))
		{
			ZoomDisplayWindow(point, TRUE);
		}
	}
	else
	{
		// Kamera aktivieren
		if (pPanel)
		{
			WORD wCam = GetID().GetSubID();
			if (GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
				pPanel->SetCamera(wCam, Play);
			else if (GetType() == DISPLAYWINDOW_LIVE) // IsKindOf(RUNTIME_CLASS(CLiveWindow)))
				pPanel->SetCamera(wCam, Live);
			else if (GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
				pPanel->SetCamera(wCam, Backup);
		}
	}

	// Focus wieder auf das Panel
	if (pPanel)
		pPanel->ActivatePanel();

	CWnd::OnLButtonDown(nFlags, point);

}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ZoomDisplayWindow(const CPoint &point, BOOL bIn)
{
	if (m_pPictureRecord)
	{
		CRect rcVideo,rcTrack,rcZoom;
		int		nX1, nY1, nX2, nY2;
		CSize s = GetPictureSize();
		if (s.cx <= 0 && s.cy <= 0)
		{
			return;
		}

		GetClientRect(rcVideo);
		if (m_rcZoom.IsRectEmpty())
		{
			m_rcZoom = CRect(0,0, s.cx, s.cy);
		}

		if (bIn)
		{
			TRACE(_T("********** ZOOM IN *********************\n"));
			rcTrack.left	= point.x - rcVideo.Width()  / 4;
			rcTrack.top		= point.y - rcVideo.Height() / 4;
			rcTrack.right	= point.x + rcVideo.Width()  / 4;
			rcTrack.bottom	= point.y + rcVideo.Height() / 4;

			int nXOff = 0;
			int nYOff = 0;

			// Liegt das Rechteck auﬂerhalb des sichtbaren Bereiches?
			if (rcTrack.left < rcVideo.left)
				nXOff = rcVideo.left - rcTrack.left;
			else
				nXOff = 0;

			if (rcTrack.top < rcVideo.top)
				nYOff = rcVideo.top - rcTrack.top;
			else
				nYOff = 0;
			rcTrack.OffsetRect(nXOff, nYOff);		
		
			if (rcTrack.right > rcVideo.right)
				nXOff = rcTrack.right - rcVideo.right;
			else
				nXOff = 0;

			if (rcTrack.bottom > rcVideo.bottom)
				nYOff = rcTrack.bottom - rcVideo.bottom;
			else
				nYOff = 0;
			rcTrack.OffsetRect(-nXOff, -nYOff);			

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

			// Den maximalen Zoomfaktor begrenzen
			if (s.cx / m_rcZoom.Width() < 5)
			{		
				m_rcZoom = CRect(nX1, nY1, nX2, nY2);
			}
			else
			{
				theApp.MessageBeep(DEFAULT_BEEP);
			}
		}
		else
		{
			TRACE(_T("********** ZOOM OUT *********************\n"));
			m_rcZoom=CRect(0,0,0,0);
		}
		
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			//Neue Rechteckwerte f¸r den Zoom aus PromilleDaten berechnen
			m_pTracker->RecalcNewRectPositionFromPromille(m_rcZoom);
			InvalidateRect(NULL);
			UpdateWindow();
		}
		RedrawWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::ScrollDisplayWindow(CPoint point)
{
	CPoint StartPoint;					 
	CPoint CurrentPoint;					 
	CRect  rcSavedZoom;
 	rcSavedZoom = m_rcZoom;
	BOOL bAlarmArchiv = FALSE;
	BOOL bCanMilliSeconds = TRUE;

	SIZE size = GetPictureSize(); 
	CRect rcImage = CRect(0,0, size.cx, size.cy);

	ShowCursor(FALSE);
	GetCursorPos(&StartPoint);
	CurrentPoint = StartPoint;

	//Rechtecksuche nicht im AlarmArchiv Window mˆglich
	//Abfrage ob Alarm Archiv nur im nicht gezoomten Zustand notwendig
	CPlayWindow* pPlayWnd = NULL;
	if (m_rcZoom.IsRectEmpty())
	{
		if(GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
		{
			CPlayWindow* pThis = (CPlayWindow*)this;
			bAlarmArchiv = pThis->IsAlarm();
			bCanMilliSeconds = pThis->CanMilliSeconds();
		}
	}


	if (   m_bCanRectSearch					//abh‰ngig von der Server-Version, erst ab 1.4.1 mˆglich
		&& !bAlarmArchiv					//Rechtecksuche nicht in Alarmarchiven
		&& !theApp.GetMainFrame()->IsInBackup()//Rechtecksuche nur, wenn nicht gerade im Backup
		&& bCanMilliSeconds)				//Rechtecksuche nur bei Archiven, die den Eintrag DVR_MS
	{
		//rectangle already on client window and cursor is on rectangle
		if(    m_pTracker 
			&& !m_pTracker->GetRect()->IsRectNull()
			&& (m_pTracker->HitTest(point) >= 0))
		{
			TRACE(_T("** vor while\n"));
			//save the original rectangle
			m_pTracker->SaveLastRect();

			TRACE(_T("** vor  ->Track\n"));
			m_pTracker->Track(this, point);
			TRACE(_T("** nach ->Track\n"));
			InvalidateRect(NULL);  
			UpdateWindow();
			ShowCursor(TRUE);
			//if the rectangles position changed, recalculate promille position
			if(m_pTracker->m_rect != *m_pTracker->GetLastRect())
			{
				//falls Buttons disabled waren, diese wieder enablen
				if(GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
				{
					pPlayWnd = theApp.GetMainFrame()->GetPlayWindow(GetID().GetSubID());
					if(pPlayWnd)
					{
						if(!pPlayWnd->GetNextQueryRectResult())
						{
							pPlayWnd->SetNextQueryRectResult(TRUE, TRUE);
							CPanel* pPanel = theApp.GetPanel();
							if(pPanel)
							{
								::PostMessage(pPanel->m_hWnd, WM_UPDATE_BUTTONS,1,0);
							}
						}

					}
				}
				m_pTracker->SetRectPromille(m_rcZoom);
				RedrawWindow();
			}

			return TRUE;
		}
		else
		{	while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) 
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
						if(m_pTracker)
						{
							
							CSize pic = GetPictureSize();
							CRect rcPicture(0,0,pic.cx, pic.cy);
							CRect rc;
							GetClientRect(rc);

							SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.m_hRect);
							SetCursor(theApp.m_hRect);
							
							//save picture size
							m_pTracker->SetPictureRect(rcPicture);
							
							//save current client rectangle
							m_pTracker->SaveLastClientRect(rc);
							
							//noch kein rectangle vorhanden
							if(!m_pTracker->GetRect()->IsRectNull())
							{
								m_pTracker->SetRectEmpty();
								InvalidateRect(NULL);
								UpdateWindow();
							}
							ShowCursor(TRUE);
							m_pTracker->DrawNewRect(point);

							m_pTracker->SetRectPromille(m_rcZoom);
							ShowCursor(FALSE);

							//disable buttons fastforward&back, skipforward&back
							theApp.GetPanel()->UpdateButtons();
						}
					}
				}
				else
				{
					//zeichnet Rechteck neu, wenn im Bild gescrollt wird
					m_pTracker->RecalcNewRectPositionFromPromille(m_rcZoom);
				}
				InvalidateRect(NULL);
				UpdateWindow();
			}
			ShowCursor(TRUE);
			TRACE(_T("**** ShowCursor - TRUE\n"));
			return (StartPoint != CurrentPoint);
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


		ShowCursor(TRUE);
		return (StartPoint != CurrentPoint);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	TRACE(_T("##CDisplayWindow::OnRButtonDown\n"));
	
	CPanel* pPanel = theApp.GetPanel();

	CPlayWindow*	pPlayWnd;	
	CLiveWindow*	pLiveWnd;
	CBackupWindow*	pBackupWnd;
	WORD wCam = GetID().GetSubID();

	pPlayWnd	= theApp.GetMainFrame()->GetPlayWindow(wCam);
 	pLiveWnd	= theApp.GetMainFrame()->GetLiveWindow(wCam);
	pBackupWnd	= theApp.GetMainFrame()->GetBackupWindow();

	if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
	{
		//if mouse is over rectangle, delete rectangle
		if(    m_pTracker
			&& !m_pTracker->GetRect()->IsRectNull()
			&& (m_pTracker->HitTest(point) >= 0))
		{
			ResetTracker();

			if(pPlayWnd && GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
			{
				ResetRectMembersInPlayWnd();
				pPlayWnd->SetPlayStatus(CPlayWindow::PS_STOP);
				pPlayWnd->SetRectPlayStatus(CPlayWindow::PS_RECT_NO_RECT);
			}
			InvalidateRect(NULL);
			UpdateWindow();
		}
		else
		{
			if (!(m_rcZoom.IsRectEmpty()))
			{
				ZoomDisplayWindow(point,FALSE);
			}
			else
			{
				if (pPanel)
				{	
					m_pMainFrame->SetDisplayActive(this);
					// Kamera aktivieren
					if (pPanel)
					{
						WORD wCam = (WORD)(GetID().GetSubID() & 0x00ff);
						if (GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
							wCam = (WORD)pPanel->GetActiveCamera();
						pPanel->ToggleViewMode(wCam);
					}
				}
			}
		}
	}
	else
	{
		if (!(m_rcZoom.IsRectEmpty()))
		{
			ZoomDisplayWindow(point,FALSE);
		}
		else
		{
			if (pPanel)
			{
				m_pMainFrame->SetDisplayActive(this);

				//bei Backup Window die alte angezeigte Kamera im panel lassen
				if (GetType() == DISPLAYWINDOW_BACKUP) // IsKindOf(RUNTIME_CLASS(CBackupWindow)))
					wCam = (WORD)pPanel->GetActiveCamera();
				pPanel->ToggleViewMode(wCam);
			}
		}
	}

	// Focus wieder auf das Panel
	if (pPanel)
	{
		pPanel->ActivatePanel();
	}

	CWnd::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
//	TRACE(_T("CDisplayWindow::OnMouseMove %s\n"), GetName());
	if (point.y > 16)
	{
		if (m_pPictureRecord)
		{
			if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
			{
				if(    !m_pTracker 
					|| m_pTracker->GetRect()->IsRectNull()
					|| (m_pTracker->HitTest(point) == -1))
				{
//					SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.m_hGlass);
					SetCursor(theApp.m_hGlass);
				}
			}
			else
			{
//				SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.m_hGlass);
				SetCursor(theApp.m_hGlass);
			}
		}
		else
		{
//		    SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.m_hNoCursor);
			SetCursor(theApp.m_hNoCursor);
		}
	}
	else
	{
		SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)theApp.m_hArrow);
		SetCursor(theApp.m_hArrow);
	}

/*
	if (m_pJpeg)
	{
		CRect rcROI(0,0,0,0);
		CRect rcClient(0,0,0,0);

		GetClientRect(rcClient);
		CSize siImage = m_pJpeg->GetImageDims();

		point.x		= (int)((double)point.x * (double)siImage.cx / (double)rcClient.Width());
		point.y		= (int)((double)point.y * (double)siImage.cy / (double)rcClient.Height());

		rcROI.left		= max(point.x-100, rcClient.left);
		rcROI.right		= rcROI.left+200;
		rcROI.top		= max(point.y-100, rcClient.top);
		rcROI.bottom	= rcROI.top+200;
	
		rcROI.OffsetRect(CPoint(min(siImage.cx-rcROI.right,0), min(siImage.cy-rcROI.bottom,0)));
		m_pJpeg->SetSelectionRect(rcROI);
	}
*/

	CWnd::OnMouseMove(nFlags, point);


}
#include <vimage\dib.h>
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::ExportImage(const CString& sExportPath)
{
	BOOL bResult = FALSE;
	CString sFileName = _T("");
	int	nI = 0;

	//nur Export starten, wenn Bild angezeigt
	//wenn Archive vorhanden, aber keine Kamera angeschlossen, ist Bildschirm schwarz
	//dann kein Export ermˆglichen
	if (m_pPictureRecord)
	{
		m_csPictureRecord.Lock();
		CIPCPictureRecord pict(*m_pPictureRecord);
		m_csPictureRecord.Unlock();
		do
		{
				sFileName.Format(_T("%s%s_%s_%s_%03d.jpg"),    sExportPath,
															 GetName(),
															 pict.GetTimeStamp().GetDBDate(),
															 pict.GetTimeStamp().GetDBTime(),
															 nI++);
		}
		while (DoesFileExist(sFileName));


		HANDLE hDIB = NULL;
		CJpeg jpeg;

		jpeg.EnableNoiseReduction();
		if (pict.GetCompressionType() == COMPRESSION_JPEG)
		{
			jpeg.DecodeJpegFromMemory((BYTE*)pict.GetData(),pict.GetDataLength());
		}
		else if (   pict.GetCompressionType() == COMPRESSION_RGB_24
			     || pict.GetCompressionType() == COMPRESSION_YUV_422)
		{
			jpeg.SetDIBData((LPBITMAPINFO)pict.GetData());
		}
		else if (pict.GetCompressionType() == COMPRESSION_MPEG4)
		{
			if (m_pMpeg)
			{
				m_csPictureRecord.Lock();
				hDIB = m_pMpeg->GetDib()->GetHDIB();
				m_csPictureRecord.Unlock();
			}
		}
		TRY
		{
			//nach jedem Speichern einer Datei auf Diskette ein
			//ConfirmSelfCheck an den Starter schicken, da des Kopieren
			//aller Dateien insgesamt teilweise l‰nger als das Timeout f¸r die
			//SelfCheck-Message im Starter dauern kann
			CPanel* pPanel = theApp.GetPanel();
			if (hDIB == NULL)
				hDIB = jpeg.CreateDIB();

			if (hDIB)
			{
				if (jpeg.EncodeJpegToFile(hDIB, sFileName))
				{
					if (SaveHtmlTag(sFileName))
					{
						if (theApp.CopyFile(theApp.GetHomeDir()+_T("\\jview.exe"), sExportPath+_T("jview.exe")))
						{
							if(pPanel)
							{
								pPanel->ForceConfirmSelfcheck();
							}

							if (theApp.CopyFile(theApp.GetHomeDir()+_T("\\ijl15.dll"), sExportPath+_T("ijl15.dll")))
								bResult = TRUE;
							else
								WK_TRACE_ERROR(_T("ExportImage: Can't copy ijl15.dll\n"));
								
							if(pPanel)
							{
								pPanel->ForceConfirmSelfcheck();
							}
						}
						else
							WK_TRACE_ERROR(_T("ExportImage: Can't copy jview.exe\n"));
					}
					else
						WK_TRACE_ERROR(_T("ExportImage: Can't save HTMLTag to file <%s>\n"), sFileName);
				}
				else
					WK_TRACE_ERROR(_T("ExportImage: Can't encode Jpeg to file <%s>\n"), sFileName);
			}
		}
		CATCH(CFileException, e)
		{
			WK_TRACE_ERROR(_T("Can't save file <%s>, %s\n"), sFileName,GetLastErrorString(e->m_lOsError));
			bResult = FALSE;
		}
		END_CATCH

		if (hDIB)
			GlobalFree(hDIB);
	}

		

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SaveHtmlTag(const CString &sJpegDriveFileName)
{
	CString sTemp;
	CString sCam;
	CString sDate;
	CString sTime;
	CString sHtmlFileName;
	CString sJpegFileName;
	CStdioFile html;
	CFileException fe;
    CSize ImageSize;
	
	if(m_pJpeg)
	{
		ImageSize = m_pJpeg->GetImageDims();
	}
	else if(m_pMpeg)
	{
		ImageSize = m_pMpeg->GetImageDims();
	}

	// .jpg durch .html ersetzen
	int nI = sJpegDriveFileName.Find(_T(".jpg"), 0);
	if (nI == -1)
		return -1;
	sHtmlFileName = sJpegDriveFileName.Left(nI) + _T(".html");
	
	// Laufwerksnamen entfernen.
	nI = sJpegDriveFileName.Find(_T(":\\"), 0);
	if (nI != -1)
		sJpegFileName = sJpegDriveFileName.Mid(nI+2);		

	if (!html.Open(sHtmlFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		TCHAR szError[1024];
		fe.GetErrorMessage(szError, 1024);
		WK_TRACE_ERROR(szError);
		
		return FALSE;
	}

	html.WriteString(_T("<html>\n"));

	html.WriteString(_T("<head>\n"));
	sTemp.Format(_T("<title>%s</title>\n"), GetPrintTitle());
	html.WriteString(sTemp);
	html.WriteString(_T("</head>\n"));

	html.WriteString(_T("<body>\n"));

	// Jpeg importiren
	sTemp.Format(_T("<img src=\"%s\">\n"),sJpegFileName);
	html.WriteString(sTemp);

	// Tabelle
	sTemp.Format(_T("<table border=\"1\" width=%d\">\n"), ImageSize.cx);
	html.WriteString(sTemp);

	// Kamera: Kameraname
	html.WriteString(_T("<tr>\n"));
	sCam.LoadString(IDS_CAMERA);
	sTemp.Format(_T("<td width=\"50%%\"><strong>%s</strong></td>\n"), sCam);
	html.WriteString(sTemp);
	sTemp.Format(_T("<td width=\"50%%\"><strong>%s</strong></td>\n"), GetName());
	html.WriteString(sTemp);
	html.WriteString(_T("</tr>\n"));

	// Datum: dd:mm:yy
	html.WriteString(_T("<tr>\n"));
	sDate.LoadString(IDS_DATE);
	sTemp.Format(_T("<td width=\"50%%\"><strong>%s</strong></td>\n"), sDate);
	html.WriteString(sTemp);
	sTemp.Format(_T("<td width=\"50%%\"><strong>%s</strong></td>\n"),GetDateString());
	html.WriteString(sTemp);
	html.WriteString(_T("</tr>\n"));

	// Uhrzeit: hh.mm.ss
	html.WriteString(_T("<tr>\n"));
	sTime.LoadString(IDS_TIME);
	sTemp.Format(_T("<td width=\"50%%\"><strong>%s</strong></td>\n"), sTime);
	html.WriteString(sTemp);
	sTemp.Format(_T("<td width=\"50%%\"><strong>%s</strong></td>\n"),GetTimeString());
	html.WriteString(sTemp);
	html.WriteString(_T("</tr>\n"));

	html.WriteString(_T("</table>\n"));
	html.WriteString(_T("</body>\n"));
	html.WriteString(_T("</html>\n"));

	html.Close();

	return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::PrintImage()
{
	BOOL	bResult = FALSE;

	// Compressiontype ermitteln.
	m_csPictureRecord.Lock();
	CIPCPictureRecord pict(*m_pPictureRecord);
	m_csPictureRecord.Unlock();
	CompressionType comType = pict.GetCompressionType();

	CPrintDialog dlg(FALSE);
	dlg.m_pd.Flags &= ~PD_RETURNDC;
	if (dlg.GetDefaults())
	{
		DEVMODE *pDM = dlg.GetDevMode();
		pDM->dmOrientation  = DMORIENT_LANDSCAPE;
		pDM->dmPrintQuality = DMRES_HIGH;
		::GlobalUnlock(dlg.m_pd.hDevMode);

		HDC hDCPrint = dlg.CreatePrinterDC();
		if (hDCPrint)
		{
			WK_TRACE(_T("printing on %s\n"),dlg.GetDeviceName());
			CDC dcPrint;
			dcPrint.Attach(hDCPrint);
			dcPrint.m_bPrinting = TRUE;
			dcPrint.SaveDC();
			
			DOCINFO di;
			CString s = GetTitle(NULL);
			ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = s;
			
			if (SP_ERROR != dcPrint.StartDoc(&di))
			{
				if (0<dcPrint.StartPage())
				{
					CSize  szPrint(dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
					CPoint ptOffset(dcPrint.GetDeviceCaps(PHYSICALOFFSETX), dcPrint.GetDeviceCaps(PHYSICALOFFSETY)); 
					CRect  rect(ptOffset, szPrint);
					dcPrint.DPtoLP(&rect);
					
					CSize TextSize = dcPrint.GetOutputTextExtent(GetPrintTitle());
					int nOff = TextSize.cy;
					
//						dcPrint.TextOut(rect.left+nOff,	rect.top + nOff, GetPrintTitle());
					// zentrieren
					dcPrint.TextOut(rect.left + ((rect.Width() - TextSize.cx) / 2),	rect.top + nOff, GetPrintTitle());
					
					CRect printRect;
					printRect.left = rect.left + nOff;
					printRect.top = rect.top + 3 * nOff;
					
					CSize ImageSize = GetPictureSize();
					
					int iHalfPicture = 1; //if Mpeg (halfpicture), calculate correct printrect-size
										  //JpegSize: 720x560, MPEGSize: 704x288
					if (comType == COMPRESSION_MPEG4)
					{
						iHalfPicture = 2;
					}
					
					if (rect.Height() > rect.Width())
					{
						// Portrait
						printRect.right = rect.right - nOff;
						printRect.bottom = printRect.top + printRect.Width() * (ImageSize.cy * iHalfPicture) / ImageSize.cx;
					}
					else
					{
						// Landscape
						printRect.bottom = rect.bottom - nOff;
						printRect.right = printRect.left + printRect.Height() * ImageSize.cx / (ImageSize.cy * iHalfPicture);
						// zentrieren
						nOff = (rect.Width() - printRect.Width()) / 2;
						printRect.left  += nOff;
						printRect.right += nOff;
					}
					
					if (comType == COMPRESSION_MPEG4)
					{
						m_pMpeg->SetOSDText1(0,0,_T(""));
						/*TRACE(_T(" ### mpeg pDC: 0x%x  Rect: t: %d  b: %d  l: %d  r: %d  zt: %d  zb: %d  zl: %d  zr: %d\n"),
							dcPrint, printRect.top, printRect.bottom, printRect.left, printRect.right,
							m_rcZoom.top, m_rcZoom.bottom, m_rcZoom.left, m_rcZoom.right);
							*/
						m_pMpeg->OnDraw(&dcPrint, printRect, m_rcZoom);
					}
					else
					{
						m_pJpeg->SetOSDText1(0,0,_T(""));
						/*TRACE(_T(" ### jpeg pDC: 0x%x  Rect: t: %d  b: %d  l: %d  r: %d  zt: %d  zb: %d  zl: %d  zr: %d\n"),
							dcPrint, printRect.top, printRect.bottom, printRect.left, printRect.right,
							m_rcZoom.top, m_rcZoom.bottom, m_rcZoom.left, m_rcZoom.right);
							*/
						m_pJpeg->OnDraw(&dcPrint, printRect, m_rcZoom);
					}
					dcPrint.EndPage();
				}
				else
				{
					WK_TRACE(_T("cannot start page\n"));
				}
				
				dcPrint.EndDoc();
			}
			else
			{
				WK_TRACE(_T("cannot start doc\n"));
			}
			dcPrint.RestoreDC(-1);
			dcPrint.Detach();
			DeleteDC(hDCPrint);
			
			bResult = TRUE;
		}
		else
		{
			WK_TRACE(_T("cannot get printer dc\n"));
		}
	}
	else
	{
		WK_TRACE(_T("cannot get printer defaults\n"));
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::InitialRequests()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SetActiveWindowToAnalogOut()
{
	// Must be overridden in CLiveWindow only
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetShowTargetCross(BOOL bShow)
{
	m_bShowTargetCross = bShow;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::GetShowTargetCross()
{
	return m_bShowTargetCross;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
	{
		if(m_pTracker && !m_pTracker->GetRect()->IsRectNull())
		{
			m_pTracker->m_nStyle = CRectTracker::hatchedBorder;
			m_pTracker->m_nStyle ^= CRectTracker::resizeOutside;
			if (pWnd == this && m_pTracker->SetCursor(this, nHitTest))
			{
				return TRUE;
			}
			
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
CRect CDisplayWindow::GetZoom()
{
	return m_rcZoom;
}

/////////////////////////////////////////////////////////////////////////////
//inits the rectangle in PlayWindow with the rectangle from LiveWindow
void CDisplayWindow::SetTrackerPromille(CRect rcNew)
{
	//SetTracker is only called when a rectangle is on LiveWindow
	//this is only when PlayWindow is not yet open
	//after PlayWindow is open, the rectangle in LiveWindow is deleted
	//if a PlayWindow is open, a rectangle cannot be created on LiveWindow
	m_pTracker->GetRectPromilleImage()->left = rcNew.left;
	m_pTracker->GetRectPromilleImage()->top = rcNew.top;
	m_pTracker->GetRectPromilleImage()->right = rcNew.right;
	m_pTracker->GetRectPromilleImage()->bottom = rcNew.bottom;
}

/////////////////////////////////////////////////////////////////////////////
//Sets PlayWindow variables for rectangle search
//is called only when rectangle was build in LiveWindow
BOOL CDisplayWindow::SetTracker()
{
	BOOL bRet = FALSE;
	if(	   m_pTracker 
		&& !m_pTracker->GetRectPromilleImage()->IsRectEmpty())
	{
		if(m_pTracker->GetRect()->IsRectEmpty())
		{
			//save picture size
			CSize pic = GetPictureSize();

			if(pic.cx != 0 && pic.cy != 0)
			{
				CRect rcPicture(0,0,pic.cx, pic.cy);
				m_pTracker->SetPictureRect(rcPicture);

				//save current client rectangle
				CRect rc;
				GetClientRect(rc);
				m_pTracker->SaveLastClientRect(rc);

				//calculates rectangle from rectangles promille
				m_pTracker->RecalcNewRectPositionFromPromille(m_rcZoom);
				
				TRACE(_T("***** SetTracker DisplayWindow rect vollst‰ndig initialisiert\n"));
				bRet = TRUE;
//				m_pTracker->SetInitFromLiveWindow(FALSE);
			}
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
//destroys the old tracker object and creates a new 
void CDisplayWindow::ResetTracker()
{
	WK_DELETE(m_pTracker);

	m_pTracker = new CQueryRectTracker(this);
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DoDrawAllCrosses(CDC *pDC)
{
	CAutoCritSec acs(&m_csPictureRecord);

	int iSize = m_waAllMD_X.GetSize();
	if(iSize > 0)
	{
		if(m_pTracker && !m_pTracker->GetRect()->IsRectEmpty())
		{
			CRect rc		= m_pTracker->GetRectPromilleImage();
			rc.NormalizeRect();
			
			if(AllMDsOutsideRect(iSize, rc))
			{
				//alle Kreuze auﬂerhalb des Rechtecks, nur das Hauptkreuz weiss zeichen
				DrawCross(pDC,m_waAllMD_X.GetAt(0),m_waAllMD_Y.GetAt(0));
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
			//kein Rechteck da, nur das Hauptkreuz weiss zeichen
			if((m_waAllMD_X.GetAt(0) > 0) && (m_waAllMD_Y.GetAt(0) > 0))
			{
				DrawAllCrosses(pDC,m_waAllMD_X.GetAt(0),m_waAllMD_Y.GetAt(0), RGB(255, 255, 255), TRUE);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ResetRectMembersInPlayWnd()
{
	WORD wCam = GetID().GetSubID();
	CPlayWindow* pPlayWnd;
	pPlayWnd = m_pMainFrame->GetPlayWindow(wCam);
	if(pPlayWnd)
	{
		pPlayWnd->InitRectQueryMembers();
		CPanel *pPanel = theApp.GetPanel();
		if(pPanel)
		{
			pPanel->UpdateMenu();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::AllMDsOutsideRect(int iAllMDs, CRect rcRectangle)
{
	BOOL bRet = TRUE;

	//liegen alle Kreuze auﬂerhalb des Rechtecks ?
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
void CDisplayWindow::InitializeTracker()
{
	if (GetType() == DISPLAYWINDOW_PLAY) // IsKindOf(RUNTIME_CLASS(CPlayWindow)))
	{
		if (SetTracker())
		{
			CRect rect = m_pTracker->GetRect();
			SetCapture();
			SetCursorPos(rect.CenterPoint().x,rect.CenterPoint().y);
			Invalidate();
			ReleaseCapture();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDisplayWindow::OnUser(WPARAM wParam, LPARAM lParam)
{
	WORD wEvent = LOWORD(wParam);
	switch (wEvent)
	{
		case ID_CHANGE_VIDEO: OnChangeVideo(); break;
		case ID_REDRAW_VIDEO: OnRedrawVideo(); break;
		case ID_REQUEST: Request(lParam); break; 
	}
	return 0;
}

void CDisplayWindow::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	if (bShow && GetType() == DISPLAYWINDOW_LIVE)
	{
		PostMessage(WM_USER, ID_REQUEST, -1);
	}
//	CString sRTC(GetRuntimeClass()->m_lpszClassName);
//	TRACE(_T("%s::OnShowWindow(%d, %d)\n"), sRTC, bShow, nStatus);
}
