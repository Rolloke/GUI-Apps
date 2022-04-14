// CJpeg.cpp: implementation of the CJpeg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJpeg.h"
#include "CImageProcessor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CJpeg::CJpeg()
{										  
    m_pDibImageData		= NULL;
	m_bScaleToFit		= TRUE;
	m_bCenterOnPage		= FALSE;
	m_bValidImage		= FALSE;
	m_nChannels			= 3;
	m_colText			= RGB(255,255,255);
	m_colBgrnd			= RGB(128,128,128);
	
	m_Font1.CreatePointFont(10*10, "Courier New");
	m_Font2.CreatePointFont(10*10, "Wingdings");
	
	ZeroMemory(&m_bmi, sizeof(BITMAPINFO));
	ZeroMemory(&m_JpegCoreProp, sizeof(JPEG_CORE_PROPERTIES));
	m_pDibImageData = new BYTE[768*576*m_nChannels];

	m_pPerf1 = new CWK_PerfMon("Decoding...");
	m_pPerf2 = new CWK_PerfMon("Blitting...");
	m_pPerf3 = new CWK_PerfMon("IJL_JBUFF_READWHOLEIMAGE...");

	m_nOSDText1X = -1;
	m_nOSDText1Y = -1;
	m_sOSDText1	 = "";
	
	m_nOSDText2X = -1;
	m_nOSDText2Y = -1;
	m_sOSDText2	 = "";

	m_hDrawDib = DrawDibOpen(); 

	m_blDibLine	= new BYTE[768*m_nChannels]; // Zeilenbuffer

	WK_TRACE("Using VfW\n");

	m_pIP = new CImageProcessor(&m_bmi, m_pDibImageData);
	m_bDoHistogrammCorrection	= FALSE;
	m_bDoLuminanceCorrection	= FALSE;
}

//////////////////////////////////////////////////////////////////////
CJpeg::~CJpeg()
{
	DrawDibClose(m_hDrawDib);

	WK_DELETE(m_pIP);
	WK_DELETE(m_blDibLine);
	WK_DELETE(m_pPerf1);
	WK_DELETE(m_pPerf2);
	WK_DELETE(m_pPerf3);
	WK_DELETE(m_pDibImageData);
}

//////////////////////////////////////////////////////////////////////
BOOL CJpeg::DecodeJpegFromFile( LPCTSTR lpszPathName )
{
	CString s = lpszPathName;

	m_csJpeg.Lock();
	TRY
		if (ijlInit(&m_JpegCoreProp) != IJL_OK)
		{
			WK_TRACE_ERROR( "Cannot initialize Intel JPEG library\n");
			AfxThrowUserException();
		}

		m_JpegCoreProp.JPGFile = const_cast<char*>(lpszPathName);
		if (ijlRead(&m_JpegCoreProp, IJL_JFILE_READPARAMS) != IJL_OK)
		{
			WK_TRACE_ERROR( "Cannot read JPEG file header from %s file\n", m_JpegCoreProp.JPGFile );
			AfxThrowUserException();
		}

		m_JpegCoreProp.DIBWidth    = m_JpegCoreProp.JPGWidth;
		m_JpegCoreProp.DIBHeight   = -(int)m_JpegCoreProp.JPGHeight; //TopDown
		m_JpegCoreProp.DIBChannels = m_nChannels;
		m_JpegCoreProp.DIBBytes    = m_pDibImageData;

		if (ijlRead(&m_JpegCoreProp, IJL_JFILE_READWHOLEIMAGE) != IJL_OK)
		{
			WK_TRACE_ERROR("Cannot read image data from %s file\n", m_JpegCoreProp.JPGFile);
			AfxThrowUserException();
		}

		if (ijlFree(&m_JpegCoreProp) != IJL_OK)
			WK_TRACE_ERROR("Cannot free Intel(R) JPEG library\n");

	CATCH_ALL(e)

		ijlFree(&m_JpegCoreProp);
		WK_TRACE_ERROR("Error opening JPEG file\n");
		m_csJpeg.Unlock();
		return FALSE;

	END_CATCH_ALL

	// initializing incapsulated image with correct values
	m_JpegCorePropDims.cx	= m_JpegCoreProp.JPGWidth;
	m_JpegCorePropDims.cy	= m_JpegCoreProp.JPGHeight;

	// now we have m_pDibImageData containing image and
	// m_JpegCorePropDims with image dimensions

	BITMAPINFOHEADER& bih = m_bmi.bmiHeader;
	ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biWidth			= m_JpegCorePropDims.cx;
	bih.biHeight		= m_JpegCorePropDims.cy;
	bih.biCompression	= BI_RGB;
	bih.biPlanes		= 1;
	bih.biBitCount		= 24;
	bih.biSizeImage		= m_JpegCorePropDims.cx*m_JpegCorePropDims.cy*bih.biBitCount/8;

	if (m_bDoLuminanceCorrection)
		m_pIP->LuminanceCorrection();
	if (m_bDoHistogrammCorrection)
		m_pIP->HistogrammCorrection();

	m_bValidImage		= TRUE;
	
	m_csJpeg.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CJpeg::DecodeJpegFromMemory(BYTE* pData , DWORD dwLen)
{
	m_csJpeg.Lock();
	m_pPerf1->Start();

    TRY
        if (ijlInit(&m_JpegCoreProp) != IJL_OK)
        {
            WK_TRACE_ERROR("Cannot initialize Intel JPEG library\n");
            AfxThrowUserException();
        }

        m_JpegCoreProp.JPGBytes		= pData;
        m_JpegCoreProp.JPGSizeBytes	= dwLen;
        
		if (ijlRead(&m_JpegCoreProp, IJL_JBUFF_READPARAMS) != IJL_OK)
        {
            WK_TRACE_ERROR("Cannot read JPEG file header from %s file\n", m_JpegCoreProp.JPGFile);
            AfxThrowUserException();
        }

        m_JpegCoreProp.DIBWidth    = m_JpegCoreProp.JPGWidth;
        m_JpegCoreProp.DIBHeight   = -(int)m_JpegCoreProp.JPGHeight; //TopDown
        m_JpegCoreProp.DIBChannels = m_nChannels;
        m_JpegCoreProp.DIBBytes    = m_pDibImageData;
									 
		if (ijlRead(&m_JpegCoreProp, IJL_JBUFF_READWHOLEIMAGE) != IJL_OK)
        {
            WK_TRACE_ERROR( "Cannot read image data from %s file\n", m_JpegCoreProp.JPGFile);
            AfxThrowUserException();
        }

        if (ijlFree(&m_JpegCoreProp) != IJL_OK)
            WK_TRACE_ERROR("Cannot free Intel(R) JPEG library\n");

    CATCH_ALL(e)

        ijlFree(&m_JpegCoreProp);
		WK_TRACE_ERROR("Error opening JPEG file\n");
		m_pPerf1->Stop();
		m_csJpeg.Unlock();
        return FALSE;

    END_CATCH_ALL

    // initializing incapsulated image with correct values
    m_JpegCorePropDims.cx	= m_JpegCoreProp.JPGWidth;
    m_JpegCorePropDims.cy	= m_JpegCoreProp.JPGHeight;

    // now we have m_pDibImageData containing image and
    // m_JpegCorePropDims with image dimensions
 
    BITMAPINFOHEADER& bih = m_bmi.bmiHeader;
    ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
    bih.biSize			= sizeof(BITMAPINFOHEADER);
    bih.biWidth			= m_JpegCorePropDims.cx;
    bih.biHeight		= m_JpegCorePropDims.cy;
    bih.biCompression	= BI_RGB;
    bih.biPlanes		= 1;
    bih.biBitCount		= 24;
	bih.biSizeImage		= m_JpegCorePropDims.cx*m_JpegCorePropDims.cy*bih.biBitCount/8;

	if (m_bDoLuminanceCorrection)
		m_pIP->LuminanceCorrection();
	if (m_bDoHistogrammCorrection)
		m_pIP->HistogrammCorrection();

	m_bValidImage		= TRUE;

	m_pPerf1->Stop();

	m_csJpeg.Unlock();		 
	return TRUE;
}							    

// ijlWrite zerstört den this-Pointer, wenn die Diskette voll ist, daher wird
// nun erst in einen Buffer encodiert und anschließend der Buffer gespeichert.
// Dieser Bug tritt in der ijl 1.0 und 1jl 1.1 auf!
#if (1)	
//////////////////////////////////////////////////////////////////////
BOOL CJpeg::EncodeJpegToFile(HANDLE hDIB, LPCTSTR lpszPathName)
{
	BOOL	bResult = FALSE;
	DWORD	dwLen	= 200000;

	BYTE* pByte = (BYTE*)new BYTE [dwLen];
	if (pByte && EncodeJpegToMemory(hDIB, pByte, dwLen))
	{
		TRY
		{
			CFile file(lpszPathName, CFile::modeCreate | CFile::modeWrite);
			file.Write(pByte, dwLen);
			bResult = TRUE;
		}
		CATCH_ALL(e)
		{
			bResult = FALSE;
		}
		END_CATCH_ALL

		delete pByte;
		pByte = NULL;
	}

    return bResult;
}
#else
//////////////////////////////////////////////////////////////////////
BOOL CJpeg::EncodeJpegToFile(HANDLE hDIB, LPCTSTR lpszPathName)
{
	LPBITMAPINFO	lpBMI	= NULL;
	
	JPEG_CORE_PROPERTIES JpegCoreProp;
    ZeroMemory(&JpegCoreProp, sizeof(JPEG_CORE_PROPERTIES));

	if (!hDIB)
		return FALSE;

	lpBMI = (LPBITMAPINFO)GlobalLock(hDIB);
	if (!lpBMI)
		return FALSE;

	m_csJpeg.Lock();

    TRY
        if (ijlInit( &JpegCoreProp) != IJL_OK)
        {						   
            WK_TRACE_ERROR("Cannot initialize Intel JPEG library\n");
            AfxThrowUserException();
        }

		JpegCoreProp.DIBWidth		= lpBMI->bmiHeader.biWidth;
		JpegCoreProp.DIBHeight		= -lpBMI->bmiHeader.biHeight; //TopDown
		JpegCoreProp.DIBBytes		= (BYTE*)&lpBMI->bmiColors[0];
		JpegCoreProp.DIBChannels	= lpBMI->bmiHeader.biBitCount/8;
		JpegCoreProp.DIBColor		= IJL_BGR;
		JpegCoreProp.JPGFile		= (char*) lpszPathName;
		JpegCoreProp.JPGWidth		= lpBMI->bmiHeader.biWidth;
		JpegCoreProp.JPGHeight		= lpBMI->bmiHeader.biHeight;
		JpegCoreProp.jquality		= 75;
		
		if (ijlWrite(&JpegCoreProp, IJL_JFILE_WRITEWHOLEIMAGE) != IJL_OK)
		{
            WK_TRACE_ERROR("Cannot write image data from %s file\n", JpegCoreProp.JPGFile);
            AfxThrowUserException();
		}
			
        if (ijlFree(&JpegCoreProp) != IJL_OK)
            WK_TRACE_ERROR("Cannot free Intel(R) JPEG library");

	CATCH_ALL(e)

        ijlFree(&JpegCoreProp);
		m_csJpeg.Unlock();
        return FALSE;

    END_CATCH_ALL
	
	m_csJpeg.Unlock();

	return TRUE;
}
#endif
									 
//////////////////////////////////////////////////////////////////////
BOOL CJpeg::EncodeJpegToMemory(HANDLE hDIB, BYTE* &pJpegData, DWORD &dwJpegLen)
{
	LPBITMAPINFO	lpBMI = NULL;;
	JPEG_CORE_PROPERTIES JpegCoreProp;
    ZeroMemory(&JpegCoreProp, sizeof(JPEG_CORE_PROPERTIES));

	if (!pJpegData || (dwJpegLen == 0) || hDIB == NULL)
		return FALSE;

	lpBMI = (LPBITMAPINFO)GlobalLock(hDIB);
	if (!lpBMI)
		return FALSE;
		   
	m_csJpeg.Lock();
    TRY
        if (ijlInit(&JpegCoreProp) != IJL_OK)
        {
            WK_TRACE_ERROR("Cannot initialize Intel JPEG library\n");
            AfxThrowUserException();
        }

		JpegCoreProp.DIBWidth		= lpBMI->bmiHeader.biWidth;
		JpegCoreProp.DIBHeight		= -lpBMI->bmiHeader.biHeight; //TopDown
		JpegCoreProp.DIBBytes		= (BYTE*)&lpBMI->bmiColors[0];;
		JpegCoreProp.DIBChannels	= lpBMI->bmiHeader.biBitCount/8;
		JpegCoreProp.DIBColor		= IJL_BGR;
		JpegCoreProp.JPGWidth		= lpBMI->bmiHeader.biWidth;
		JpegCoreProp.JPGHeight		= lpBMI->bmiHeader.biHeight;
		JpegCoreProp.jquality		= 75;
        JpegCoreProp.JPGBytes		= pJpegData;
		JpegCoreProp.JPGSizeBytes	= dwJpegLen;

		if (ijlWrite(&JpegCoreProp, IJL_JBUFF_WRITEWHOLEIMAGE) != IJL_OK)
		{
            WK_TRACE_ERROR("Cannot write image data to memory\n");
            AfxThrowUserException();
		}
        dwJpegLen = JpegCoreProp.JPGSizeBytes;

        if (ijlFree(&JpegCoreProp) != IJL_OK)
            WK_TRACE_ERROR("Cannot free Intel(R) JPEG library");
			
    CATCH_ALL(e)

        ijlFree(&JpegCoreProp);
		m_csJpeg.Unlock();
        return FALSE;

    END_CATCH_ALL
	
	m_csJpeg.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
SIZE CJpeg::GetImageDims()	const
{
	SIZE size;
	int	 nYScale = 1;

	if ((float)m_JpegCorePropDims.cx / (float)m_JpegCorePropDims.cy < 2.0)
		nYScale = 1;
	else
		nYScale = 2;								 

	size.cx = m_JpegCorePropDims.cx;
	size.cy = m_JpegCorePropDims.cy * nYScale;

	return size;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::OnDraw( CDC* pDC, const CRect &rectDest, const CRect &rectSrc/*=CRect(0,0,0,0)*/)
{
	int nYScale = 1;

	// Wurde bereits ein Bild erfolgreich dekodiert?
	if (!m_bValidImage)
		return;

    if (m_pDibImageData == NULL)
		return;

	m_csJpeg.Lock();
	CRect rectTo, rectFrom;			 

	// Ist das Bild vertikal um die hälfte gestaucht? (Ein Halbbild)
	if ((float)m_JpegCorePropDims.cx / (float)m_JpegCorePropDims.cy < 2.0)
		nYScale = 1;
	else
		nYScale = 2;								 

	// Ist ein Quellrechteck vorgegeben?
	if (rectSrc.IsRectEmpty())
	{
		// Nein, dann nimm die Originalgröße des Jpegbildes.
		rectFrom.left   = 0;
		rectFrom.top	= 0;
		rectFrom.right	= m_JpegCorePropDims.cx;
		rectFrom.bottom	= m_JpegCorePropDims.cy;
	}
	else
	{
		// Ja, dann nimm dieses, aber berücksichtige eine evtl. vertikale Stauchung.
		int nX1			= rectSrc.left;
		int nY1			= rectSrc.top / nYScale;
		int nX2			= rectSrc.right;
		int nY2			= rectSrc.bottom / nYScale;
		rectFrom		= CRect(nX1, nY1, nX2, nY2);
	
	}

	// ist ein Zielrechteck vorgegeben?
	if (rectDest.IsRectEmpty())
	{
		// Nein, dann nimm die Originalgröße des Jpegs, aber berücksichtige eine evt.
		// vertikale Stauchung.
		rectTo.left		= 0;
		rectTo.top		= 0;
		rectTo.right	= m_JpegCorePropDims.cx;
		rectTo.bottom	= m_JpegCorePropDims.cy *  nYScale;
	}
	else
	{
		// Ja, dann nimm das vorgegebene Zielrechteck.
		rectTo	 = rectDest;
	}

	// Parameter ok?
 	if ((rectTo.IsRectEmpty())||
		(rectFrom.Width() > m_JpegCorePropDims.cx) ||(rectFrom.Height() > m_JpegCorePropDims.cy))
	{
		m_csJpeg.Unlock();
		return;
	}				  

	if (!m_sOSDText1.IsEmpty())
	{	
		CFont *pOldFont = pDC->SelectObject(&m_Font1);
		CSize Textsize = pDC->GetTextExtent(m_sOSDText1);
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(m_colBgrnd);	 
		pDC->SetTextColor(m_colText);
		pDC->TextOut(m_nOSDText1X,m_nOSDText1Y, m_sOSDText1);
		pDC->ExcludeClipRect(CRect(m_nOSDText1X, m_nOSDText1Y, m_nOSDText1X+Textsize.cx,m_nOSDText1Y+Textsize.cy));
		pDC->SelectObject(pOldFont);
	}
	if (!m_sOSDText2.IsEmpty())
	{	
		CFont *pOldFont = pDC->SelectObject(&m_Font2);
		CSize Textsize = pDC->GetTextExtent(m_sOSDText2);
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(m_colBgrnd);	 
		pDC->SetTextColor(m_colText);
		pDC->TextOut(m_nOSDText2X,m_nOSDText2Y, m_sOSDText2);
		pDC->ExcludeClipRect(CRect(m_nOSDText2X, m_nOSDText2Y, m_nOSDText2X+Textsize.cx,m_nOSDText2Y+Textsize.cy));
		pDC->SelectObject(pOldFont);
	}

    if (pDC->IsPrinting())
    {
		// Ursprung bei 'StretchDIBits liegt UNTEN links!!
		BITMAPINFOHEADER bmiHeader = m_bmi.bmiHeader;
		bmiHeader.biHeight *= -1;

		pDC->SetStretchBltMode(COLORONCOLOR);
		StretchDIBits(pDC->m_hDC,				// handle to device context
					  rectTo.left,				// x-coordinate of upper-left corner of dest. rect.
					  rectTo.top,               // y-coordinate of upper-left corner of dest. rect.
					  rectTo.Width(),			// width of destination rectangle
					  rectTo.Height(),          // height of destination rectangle
					  rectFrom.left,			// x-coordinate of upper-left corner of source rect.
					  rectFrom.bottom, 	        // y-coordinate of upper-left corner of source rect.
					  rectFrom.Width(), 		// width of source rectangle
					  -rectFrom.Height(),		// height of source rectangle
					  m_pDibImageData,			// address of bitmap bits
					  (BITMAPINFO*)&bmiHeader,	// address of bitmap data
					  DIB_RGB_COLORS,           // usage
					  SRCCOPY			        // raster operation code
					  );		
	}
	else
	{
		m_pPerf2->Start();
		DrawDibDraw(m_hDrawDib,
					pDC->m_hDC,
					rectTo.left, rectTo.top, rectTo.Width(), rectTo.Height(),
					&m_bmi.bmiHeader, m_pDibImageData,
					rectFrom.left,rectFrom.top,rectFrom.Width(),rectFrom.Height(),
					0);
		m_pPerf2->Stop();
	}
	
	m_csJpeg.Unlock();
}

//////////////////////////////////////////////////////////////////////
HBITMAP CJpeg::CreateBitmap()
{
	m_csJpeg.Lock();

	// Stimmt das Seitenverhältnis, oder muß vertikal gedoppelt werden?
	if ((float)m_bmi.bmiHeader.biWidth / (float)m_bmi.bmiHeader.biHeight > 2.0)
		DoubleVSize();

	HBITMAP hBitmap = CreateDIBitmap(GetDC(NULL), &m_bmi.bmiHeader,
						CBM_INIT, m_pDibImageData, &m_bmi, DIB_RGB_COLORS);

	m_csJpeg.Unlock();

	return hBitmap;
}

//////////////////////////////////////////////////////////////////////
HANDLE CJpeg::CreateDIB()
{
	HANDLE			hDIB	= NULL;
	LPBITMAPINFO	lpBMI	= NULL;
	BYTE*			lpBits	= NULL;

	// Stimmt das Seitenverhältnis, oder muß vertikal gedoppelt werden?
	if ((float)m_bmi.bmiHeader.biWidth / (float)m_bmi.bmiHeader.biHeight > 2.0)
		DoubleVSize();

	hDIB = GlobalAlloc(GHND, m_bmi.bmiHeader.biSize + m_bmi.bmiHeader.biSizeImage);
	if (!hDIB)
		return NULL;

	lpBMI	= (LPBITMAPINFO) GlobalLock(hDIB);
	*lpBMI	= m_bmi;
	lpBits	= (BYTE*)&lpBMI->bmiColors[0];
	memcpy(lpBits, m_pDibImageData, m_bmi.bmiHeader.biSizeImage);
	GlobalUnlock(hDIB);

	return hDIB;
}

//////////////////////////////////////////////////////////////////////
BOOL CJpeg::DoubleVSize()
{
	BOOL	bRet = TRUE;
	int		nW = m_JpegCorePropDims.cx;
	int		nH = m_JpegCorePropDims.cy;
	int		nVScale = 0;

	int nDstIndex = 0;
	int nSrcIndex = 0;
	
	DWORD dwDIBSize		= nW*2*nH*m_nChannels;
	BYTE* pDblDibDest	= new BYTE[dwDIBSize];
	BYTE* pDblDibDest1	= NULL;
	BYTE* pDblDibDest2	= NULL;
	BYTE* pDibSrc		= m_pDibImageData;

	m_bmi.bmiHeader.biHeight	= 2*m_bmi.bmiHeader.biHeight;
	m_bmi.bmiHeader.biSizeImage = dwDIBSize;
	m_JpegCorePropDims.cy		= m_bmi.bmiHeader.biHeight;
	
	// Bild vertikal verdoppeln.
	for (int nY = 0; nY < nH; nY++)
	{
		pDblDibDest1	= &pDblDibDest[(2*nY+0)*nW*m_nChannels];
		pDblDibDest2	= &pDblDibDest[(2*nY+1)*nW*m_nChannels];
		for (int nX = 0; nX < nW; nX++)
		{
			*pDblDibDest1++ = *pDibSrc;		// Blau	
			*pDblDibDest2++ = *pDibSrc++;	
			*pDblDibDest1++ = *pDibSrc;		// Grün	
			*pDblDibDest2++ = *pDibSrc++;	
			*pDblDibDest1++ = *pDibSrc;		// Rot	
			*pDblDibDest2++ = *pDibSrc++;	
		}
	}
	memcpy(m_pDibImageData, pDblDibDest, dwDIBSize);
	WK_DELETE(pDblDibDest);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::SetOSDText1(int nX, int nY, const CString &sText)
{
	m_nOSDText1X = nX;
	m_nOSDText1Y = nY;
	m_sOSDText1	 = sText;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::SetOSDText2(int nX, int nY, const CString &sText)
{
	m_nOSDText2X = nX;
	m_nOSDText2Y = nY;
	m_sOSDText2	 = sText;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::SetTextColor(COLORREF colText)
{
	m_colText = colText;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::SetBackgroundColor(COLORREF colBgrnd)
{
	m_colBgrnd = colBgrnd;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CJpeg::SetFont1(CFont &font)
{
	BOOL bResult = FALSE;
	LOGFONT LogFont;
	ZeroMemory(&LogFont, sizeof(LOGFONT));

	if (font.GetLogFont(&LogFont))
	{
		m_Font1.DeleteObject();
		bResult = m_Font1.CreateFontIndirect(&LogFont);
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CJpeg::SetFont2(CFont &font)
{
	BOOL bResult = FALSE;
	LOGFONT LogFont;
	ZeroMemory(&LogFont, sizeof(LOGFONT));

	if (font.GetLogFont(&LogFont))
	{
		m_Font2.DeleteObject();
		bResult = m_Font2.CreateFontIndirect(&LogFont);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CJpeg::VFlip()
{
	int		nW = m_bmi.bmiHeader.biWidth;
	int		nH = m_bmi.bmiHeader.biHeight;

	int nDstIndex = 0;
	int nSrcIndex = 0;
	
	DWORD dwDIBSize	= nW*nH*m_nChannels;

	m_bmi.bmiHeader.biHeight	= -m_bmi.bmiHeader.biHeight;
	m_bmi.bmiHeader.biSizeImage = dwDIBSize;
	
	// Bild vertikal spiegeln
	for (int nY = 0; nY < nH/2; nY++)
	{
		nDstIndex = (nH-nY-1)*nW*m_nChannels;
		nSrcIndex = nY*nW*m_nChannels;
		memcpy(&m_blDibLine[0], &m_pDibImageData[nDstIndex], nW*m_nChannels); 
		memcpy(&m_pDibImageData[nDstIndex], &m_pDibImageData[nSrcIndex], nW*m_nChannels); 
		memcpy(&m_pDibImageData[nSrcIndex], &m_blDibLine[0], nW*m_nChannels); 
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::SetSelectionRect(const CRect &rcSelection)
{
	if (m_pIP)
		m_pIP->SetSelection(rcSelection);
}

//////////////////////////////////////////////////////////////////////
COLORREF CJpeg::GetPixel(int nX, int nY)
{
	COLORREF col = RGB(0,0,0);

	if (m_pDibImageData)
	{
		int nW = m_bmi.bmiHeader.biWidth;
		int	nH = abs(m_bmi.bmiHeader.biHeight);

		if ((nX >= 0) && (nY >= 0)  && (nX < nW) && (nY < nH))
		{
			int	nOffset = (nX + nY * nW) * m_bmi.bmiHeader.biBitCount/8;
			col = (COLORREF)RGB(m_pDibImageData[nOffset+2],
								m_pDibImageData[nOffset+1],
								m_pDibImageData[nOffset+0]);
		}
	}
	
	return col;
}
