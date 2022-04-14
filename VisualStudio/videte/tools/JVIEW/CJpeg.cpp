// CJpeg.cpp: implementation of the CJpeg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJpeg.h"
#include "math.h"

#ifndef _DEBUG
#undef OutputDebugString
#define OutputDebugString(x)
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
	
	ZeroMemory(&m_bmi, sizeof(BITMAPINFO));
	ZeroMemory(&m_JpegCoreProp, sizeof(JPEG_CORE_PROPERTIES));
	m_pDibImageData = new BYTE[768*576*m_nChannels];

	m_nOSDText1X = -1;
	m_nOSDText1Y = -1;
	strcpy(m_szOSDText1,"");

	m_hDrawDib = DrawDibOpen(); 

	m_blDibLine	= new BYTE[768*m_nChannels]; // Zeilenbuffer
}

//////////////////////////////////////////////////////////////////////
CJpeg::~CJpeg()
{
	DrawDibClose(m_hDrawDib);

	delete(m_blDibLine);
	delete(m_pDibImageData);
}
//////////////////////////////////////////////////////////////////////
BOOL CJpeg::DecodeJpegFromMemory(BYTE* pBufferData , DWORD dwBufferLen, BOOL /*bAllowDoubleFieldDecoding=TRUE*/, BOOL /*bInterpolate=TRUE*/)
{
	BYTE* pJpegData1		= pBufferData;
	DWORD dwJpegLen1		= dwBufferLen;

	BYTE* pDibImageData1	= m_pDibImageData;

    if (ijlInit(&m_JpegCoreProp) == IJL_OK)
    {
        m_JpegCoreProp.JPGBytes		= pJpegData1;
        m_JpegCoreProp.JPGSizeBytes	= dwJpegLen1;
        
		if (ijlRead(&m_JpegCoreProp, IJL_JBUFF_READPARAMS) == IJL_OK)
        {
			m_JpegCoreProp.DIBWidth    = m_JpegCoreProp.JPGWidth;
			m_JpegCoreProp.DIBHeight   = -(int)m_JpegCoreProp.JPGHeight; //TopDown
			m_JpegCoreProp.DIBChannels = m_nChannels;
			m_JpegCoreProp.DIBBytes    = pDibImageData1;
										 
			if (ijlRead(&m_JpegCoreProp, IJL_JBUFF_READWHOLEIMAGE) == IJL_OK)
			{
			}
			else
			{
				OutputDebugString("no buffer");
			}
		}
		else
		{
            OutputDebugString("no read");
        }

		ijlFree(&m_JpegCoreProp);
	}
	else
	{
		OutputDebugString("no ijl init\n");
	}

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


	m_bValidImage		= TRUE;

	return TRUE;
}							    

//////////////////////////////////////////////////////////////////////
SIZE CJpeg::GetImageDims()	const
{
	SIZE size;
	size.cx = m_JpegCorePropDims.cx;
	size.cy = m_JpegCorePropDims.cy;

	return size;
}

//////////////////////////////////////////////////////////////////////
void CJpeg::OnDraw(HDC hDC, const RECT &rectDest, const RECT &rectSrc, BOOL bPrint)
{
	// Wurde bereits ein Bild erfolgreich dekodiert?
	if (!m_bValidImage)
		return;

    if (m_pDibImageData == NULL)
		return;

	RECT rectTo, rectFrom;			 

	// Ist ein Quellrechteck vorgegeben?
	if (IsRectEmpty(&rectSrc))
	{
		// Nein, dann nimm die Originalgröße des Jpegbildes.
		rectFrom.left   = 0;
		rectFrom.top	= 0;
		rectFrom.right	= m_JpegCorePropDims.cx;
		rectFrom.bottom	= m_JpegCorePropDims.cy;
	}
	else
	{
		// Ja, dann nimm dieses.
		int nX1			= rectSrc.left;
		int nY1			= rectSrc.top;
		int nX2			= rectSrc.right;
		int nY2			= rectSrc.bottom;
		rectFrom.left	= nX1;
		rectFrom.top    = nY1;
		rectFrom.right  = nX2;
		rectFrom.bottom = nY2;
	
	}

	// ist ein Zielrechteck vorgegeben?
	if (IsRectEmpty(&rectDest))
	{
		// Nein, dann nimm die Originalgröße des Jpegs
		rectTo.left		= 0;
		rectTo.top		= 0;
		rectTo.right	= m_JpegCorePropDims.cx;
		rectTo.bottom	= m_JpegCorePropDims.cy;
	}
	else
	{
		// Ja, dann nimm das vorgegebene Zielrechteck.
		rectTo	 = rectDest;
	}

	// Parameter ok?
 	if ((IsRectEmpty(&rectTo))||
		(   rectFrom.right-rectFrom.left > m_JpegCorePropDims.cx) 
		 ||(rectFrom.bottom-rectFrom.top > m_JpegCorePropDims.cy))
	{
		return;
	}				  							    

	// 1. OnScreendisplay
	if (strlen(m_szOSDText1)>0)
	{	
		SIZE Textsize;
		GetTextExtentPoint32(hDC, m_szOSDText1, strlen(m_szOSDText1), &Textsize);
		SetBkMode(hDC,OPAQUE);
		SetBkColor(hDC,m_colBgrnd);	 
		::SetTextColor(hDC, m_colText);
		TextOut(hDC,m_nOSDText1X,m_nOSDText1Y, m_szOSDText1,strlen(m_szOSDText1));
		ExcludeClipRect(hDC,m_nOSDText1X,m_nOSDText1Y,m_nOSDText1X+Textsize.cx,m_nOSDText1Y+Textsize.cy);
	}

    if (bPrint)
    {
		SetStretchBltMode(hDC,COLORONCOLOR);

		// recalculate coordinates
		// to prevent from negative ones
		int h = rectFrom.bottom - rectFrom.top;
		rectFrom.bottom = m_JpegCorePropDims.cy - rectFrom.top;
		rectFrom.top = rectFrom.bottom - h; 

		/*int iRet = */StretchDIBits(hDC,				// handle to device context
					  rectTo.left,				// x-coordinate of upper-left corner of dest. rect.
					  rectTo.top,               // y-coordinate of upper-left corner of dest. rect.
					  rectTo.right-rectTo.left,			// width of destination rectangle
					  rectTo.bottom-rectTo.top,          // height of destination rectangle
					  rectFrom.left,			// x-coordinate of upper-left corner of source rect.
					  rectFrom.top, 	        // y-coordinate of upper-left corner of source rect.
					  rectFrom.right-rectFrom.left, 		// width of source rectangle
					  rectFrom.bottom-rectFrom.top,		// height of source rectangle
					  m_pDibImageData,			// address of bitmap bits
					  (BITMAPINFO*)&m_bmi,	// address of bitmap data
					  DIB_RGB_COLORS,           // usage
					  SRCCOPY			        // raster operation code
					  );		
	}
	else
	{
		DrawDibDraw(m_hDrawDib,
					hDC,
					rectTo.left, 
					rectTo.top, 
					rectTo.right-rectTo.left, 
					rectTo.bottom-rectTo.top,
					&m_bmi.bmiHeader, 
					m_pDibImageData,
					rectFrom.left,
					rectFrom.top,
					rectFrom.right-rectFrom.left,
					rectFrom.bottom-rectFrom.top,
					0);
	}
}

//////////////////////////////////////////////////////////////////////
void CJpeg::SetOSDText1(int nX, int nY, const char* szText)
{
	m_nOSDText1X = nX;
	m_nOSDText1Y = nY;
	strcpy(m_szOSDText1,szText);
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
//////////////////////////////////////////////////////////////////////
void CJpeg::Copy(HWND hWnd)
{
	HANDLE hDIB;
    BITMAPINFOHEADER bih = m_bmi.bmiHeader;
	DWORD dwBitsSize = sizeof(BITMAPINFOHEADER) + bih.biSizeImage;
	LPSTR pDIB;

	/*
	 * Allocate memory for DIB
	 */
	hDIB = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return;
	}
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	CopyMemory(pDIB,&bih,sizeof(BITMAPINFOHEADER));
	pDIB += sizeof(BITMAPINFOHEADER);
	CopyMemory(pDIB,m_pDibImageData,bih.biSizeImage);

	::GlobalUnlock((HGLOBAL) hDIB);

	if (OpenClipboard(hWnd))
	{
		EmptyClipboard();
		if (!SetClipboardData (CF_DIB, (HANDLE) hDIB))
			GlobalFree(hDIB);
		CloseClipboard();
	}
}