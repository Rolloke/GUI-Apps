// GraphicsLib.cpp: implementation of the CGraphicsLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraphicsLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGraphicsLib::CGraphicsLib()
{
	m_hDIB=NULL;
	m_pIPLBitmap = NULL;
	m_hBITMAP = NULL;
}
//////////////////////////////////////////////////////////////////////
CGraphicsLib::CGraphicsLib(HANDLE hDIB)
{
	m_hDIB       = hDIB;
	m_hBITMAP = NULL;
	
	InitData();
}
//////////////////////////////////////////////////////////////////////
CGraphicsLib::~CGraphicsLib()
{
    DeleteObject(m_hBITMAP);
    iplDeallocate(m_pIPLBitmap, IPL_IMAGE_HEADER);
	GlobalFree(m_hDIB);
}
//////////////////////////////////////////////////////////////////////
HBITMAP CGraphicsLib::DIBToHBITMAP(HANDLE hDIB)
{
	
	
	void *pDataPtr = GlobalLock(hDIB);
	LPBITMAPINFO lpBMI	= (LPBITMAPINFO)pDataPtr;
	
	HBITMAP hBitmap = CreateDIBitmap(GetDC(NULL), 
		&(lpBMI->bmiHeader),
		CBM_INIT, 
		(char*)pDataPtr+lpBMI->bmiHeader.biSize, 
		lpBMI, 
		DIB_RGB_COLORS);
	GlobalUnlock(hDIB);
	
	return hBitmap;
}
//////////////////////////////////////////////////////////////////////
HBITMAP CGraphicsLib::CreateHBITMAP(LPBITMAPINFO lpBMI)
{
	
	LPBITMAPINFOHEADER lpBI;
	BYTE  *lpBits;
	
	lpBI = (LPBITMAPINFOHEADER)lpBMI;
	UINT iCol;
	
	if (lpBI->biClrUsed)
		iCol=lpBI->biClrUsed;   // Eintrag in Informationsblock
	else
		if (lpBI->biBitCount < 16)
			iCol=1 << lpBI->biBitCount;  // 2, 16, 256 Farben
		else
			iCol= 0;                      // 0 bei 16, 24, 32 BPP
		
		
		lpBits =  (BYTE  *) &(lpBMI->bmiColors[iCol]);
		
		// Berücksichtige Maskenbytes bei 16/32-Bit-DIBS mit BI_BITFIELDS
		if (lpBI->biCompression == BI_BITFIELDS)
			lpBits += 3 * sizeof(DWORD);
		
		
		HBITMAP hBitmap = CreateDIBitmap(GetDC(NULL), 
			lpBI,
			CBM_INIT, 
			lpBits,
			lpBMI, 
			DIB_RGB_COLORS);
		
		return hBitmap;
		
}
//////////////////////////////////////////////////////////////////////
IplImage* CGraphicsLib::DIBToIPL(HBITMAP hBitmap)
{
	BITMAP bmp;
    if(!::GetObject(hBitmap,sizeof(BITMAP),&bmp))
	{
		TRACE("Error");
	}
	
	
    int    nChannels = 3;//bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8;
	
    int    depth     = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	
    IplImage  *img = iplCreateImageHeader(nChannels,    // number of channels
		0,            // # of alpha channel, 0-absent
		depth,        // bit depth of channel
		"RGB",        // color model
		"BGR",        // channel sequence
		IPL_DATA_ORDER_PIXEL,// data ordering,
		// by pixels or
		// by channels
		IPL_ORIGIN_BL,       // bitmap origin
		// ..._BL - bottom left
		IPL_ALIGN_DWORD,// scan lines alignment
		bmp.bmWidth,    // width in pixels
		bmp.bmHeight,   // height and
		0,              // ROI, 0 - entire image
		0,              // MASK ROI, 0 - entire image
		0,              // image ID - reserved
		0);             // tile info.
	
    img->imageData=img->imageDataOrigin = (char*)bmp.bmBits;
    return img;
	
}
//////////////////////////////////////////////////////////////////////
HBITMAP CGraphicsLib::CreateBitmap(int iwidth, int iheight, WORD wBitsPerPixel)
{
	HDC         hDC = ::CreateCompatibleDC(0);
    BYTE        tmp[sizeof(BITMAPINFO)+255*4];
    BITMAPINFO *bmi = (BITMAPINFO*)tmp;
    HBITMAP     hBmp;
	
	
    memset(bmi,0,sizeof(BITMAPINFO));
    bmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth    = iwidth;
    bmi->bmiHeader.biHeight   = iheight;
    bmi->bmiHeader.biPlanes   = 1;
    bmi->bmiHeader.biBitCount = wBitsPerPixel;
	
    ((long*)&(bmi->bmiColors))[0] = 0;
    ((long*)&(bmi->bmiColors))[1] =-1;
	
    hBmp = ::CreateDIBSection(hDC,bmi,DIB_RGB_COLORS,0,0,0);
    ::DeleteDC(hDC);
	
    return hBmp;
}
//////////////////////////////////////////////////////////////////////
BOOL CGraphicsLib::ReduceImageSize(int iFactor, int iInterpolate)
{  
/*Interpolation:

  IPL_INTER_NN     = Nearest neighbor
  IPL_INTER_LINEAR	= Linear interpolation
  IPL_INTER_CUBIC  = Cubic interpolation
  IPL_INTER_SUPER  = Super sampling
  
    */
	
#ifdef _DEBUG
	int dwTick;
	dwTick = GetTickCount();
#endif
	
	if(iFactor < 1)
		return FALSE;
	
	m_lWidth =m_lWidth/iFactor;
	m_lHeight=m_lHeight/iFactor;
	
	HBITMAP hDestinationBITMAP;
	IplImage *pDestinationIPLBitmap;
	
	hDestinationBITMAP = CreateBitmap(m_lWidth,m_lHeight,m_wBitsPerPixel);
	
	pDestinationIPLBitmap = DIBToIPL(hDestinationBITMAP);
	
	iplDecimate(m_pIPLBitmap,pDestinationIPLBitmap,1,iFactor,1,iFactor,iInterpolate);
	
	iplDeallocate(m_pIPLBitmap, IPL_IMAGE_HEADER);
	m_pIPLBitmap = pDestinationIPLBitmap;
	
	DeleteObject(m_hBITMAP);
	m_hBITMAP = hDestinationBITMAP;
	
	
#ifdef _DEBUG
	TRACE("REDUCE SAMPLING TIME: %i\n",GetTickCount()-dwTick);
#endif
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CGraphicsLib::ResizeImage(int iWidth, int iHeight, int iInterpolate/*=IPL_INTER_NN*/)
{
/*Interpolation:

  IPL_INTER_NN     = Nearest neighbor
  IPL_INTER_LINEAR	= Linear interpolation
  IPL_INTER_CUBIC  = Cubic interpolation
  IPL_INTER_SUPER  = Super sampling
  
    */
	
#ifdef _DEBUG
	int dwTick;
	dwTick = GetTickCount();
#endif
	

	HBITMAP hDestinationBITMAP;
	IplImage *pDestinationIPLBitmap;
	
	hDestinationBITMAP = CreateBitmap(iWidth,iHeight,m_wBitsPerPixel);
	pDestinationIPLBitmap = DIBToIPL(hDestinationBITMAP);
	
	iplResize(m_pIPLBitmap,pDestinationIPLBitmap,iWidth,m_lWidth,iHeight,m_lHeight,iInterpolate);
	
	iplDeallocate(m_pIPLBitmap, IPL_IMAGE_HEADER);
	m_pIPLBitmap = pDestinationIPLBitmap;
	DeleteObject(m_hBITMAP);
	m_hBITMAP = hDestinationBITMAP;
	m_lWidth = iWidth;
	m_lHeight= iHeight;
	
	
#ifdef _DEBUG
	TRACE("RESIZE SAMPLING TIME: %i\n",GetTickCount()-dwTick);
#endif
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CGraphicsLib::InitData()
{
	
	m_hBITMAP    = DIBToHBITMAP(m_hDIB);
	m_pIPLBitmap = DIBToIPL(m_hBITMAP);
	
	void *pOriginalImagePtr = GlobalLock(m_hDIB);
	
	LPBITMAPINFO lpBMI= (LPBITMAPINFO)pOriginalImagePtr;
	
	m_dwSizeOfHeader =lpBMI->bmiHeader.biSize;
	m_dwSizeOfImage  =lpBMI->bmiHeader.biSizeImage;
	m_wBitsPerPixel  =lpBMI->bmiHeader.biBitCount;
	m_lHeight        =lpBMI->bmiHeader.biHeight;
	m_lWidth         =lpBMI->bmiHeader.biWidth;
	m_pImageDIBData  =(char*)pOriginalImagePtr+m_dwSizeOfHeader;
	m_ibiClrUsed     =lpBMI->bmiHeader.biClrUsed;
	m_iCompression   =lpBMI->bmiHeader.biCompression;
	m_iPlanes		 =lpBMI->bmiHeader.biPlanes;
	m_ixPelsPerMeter =lpBMI->bmiHeader.biXPelsPerMeter;
	m_iyPelsPerMeter =lpBMI->bmiHeader.biYPelsPerMeter;
	
	m_pIPLBitmap->imageData = m_pIPLBitmap->imageDataOrigin = m_pImageDIBData;
	GlobalUnlock(m_hDIB);
	
}
//////////////////////////////////////////////////////////////////////
HBITMAP CGraphicsLib::GetHBitmap()
{
	return m_hBITMAP;
}
//////////////////////////////////////////////////////////////////////
void CGraphicsLib::ResetData(HANDLE hDIB)
{
	
    if(m_hBITMAP)
		DeleteObject(m_hBITMAP);
	
	if(m_pIPLBitmap)
		iplDeallocate(m_pIPLBitmap, IPL_IMAGE_HEADER);
	
	if(m_hDIB)
	{
		GlobalFree(m_hDIB);
	}
	m_hDIB       = hDIB;
	m_hBITMAP = NULL;
	
	InitData();
	
}
//////////////////////////////////////////////////////////////////////
void CGraphicsLib::DisplayImage(CWnd *pWnd, CDC *dc, int id, HBITMAP hBmp)
{
	
	if (!hBmp) return;
	RECT     r;
    pWnd->GetWindowRect(&r);
    pWnd->ScreenToClient(&r);
	dc->SetStretchBltMode(COLORONCOLOR);
	
    CDC      tmp;
    tmp.CreateCompatibleDC(0);
    CBitmap *old = tmp.SelectObject(CBitmap::FromHandle(hBmp));
    BITMAP   bmp;
    ::GetObject(hBmp,sizeof(BITMAP),&bmp);
	/////////////////////////////////////
	//	r.top=r.top+150;
	//	r.bottom=r.bottom-40;					//nur zu testzwecken
	////////////////////////////////////
    int      w   = r.right  - r.left;
    int      h   = r.bottom - r.top;
	
    if (h*bmp.bmWidth < w*bmp.bmHeight)
        w = bmp.bmWidth*h/bmp.bmHeight;
    else
        h = bmp.bmHeight*w/bmp.bmWidth;
	
    r.left   = (r.right + r.left - w)/2;
    r.top    = (r.bottom + r.top - h)/2;
	
    dc->StretchBlt(r.left,r.top,w,h,&tmp,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
    tmp.SelectObject(old);
	
}
//////////////////////////////////////////////////////////////////////
BOOL CGraphicsLib::FixedFilter(IplFilter iFilter)
{
/*
IPL_PREWITT_3x3_V  A gradient filter (vertical Prewitt operator).
IPL_PREWITT_3x3_H  A gradient filter (horizontal Prewitt operator).

		IPL_SOBEL_3x3_V	   A gradient filter (vertical Sobel operator).
		IPL_SOBEL_3x3_H	   A gradient filter (horizontal Sobel operator).
		
		  IPL_LAPLACIAN_3x3  A 3x3 Laplacian highpass filter.
		  IPL_LAPLACIAN_5x5  A 5x5 Laplacian highpass filter.
		  
			IPL_GAUSSIAN_3x3   A 3x3 Gaussian lowpass filter.
			IPL_GAUSSIAN_5x5   A 5x5 Gaussian lowpass filter.
			
			  IPL_HIPASS_3x3     A 3x3 highpass filter.
			  IPL_HIPASS_5x5     A 5x5 highpass filter.
			  
				IPL_SHARPEN_3x3	   A 3x3 sharpening filter.
				
				  
	*/
#ifdef _DEBUG
	int dwTick;
	dwTick = GetTickCount();
#endif
	
	HBITMAP hDestinationBITMAP;
	IplImage *pDestinationIPLBitmap;

	hDestinationBITMAP = CreateBitmap(m_lWidth,m_lHeight,m_wBitsPerPixel);
	
	pDestinationIPLBitmap = DIBToIPL(hDestinationBITMAP);
	
	iplFixedFilter(m_pIPLBitmap,pDestinationIPLBitmap,iFilter);
	
	iplDeallocate(m_pIPLBitmap, IPL_IMAGE_HEADER);
	m_pIPLBitmap = pDestinationIPLBitmap;
	
	DeleteObject(m_hBITMAP);
	m_hBITMAP = hDestinationBITMAP;
	
	
#ifdef _DEBUG
	TRACE("FIXED FILTER SAMPLING TIME: %i\n",GetTickCount()-dwTick);
#endif
	
	return TRUE;
	
	
	
}
//////////////////////////////////////////////////////////////////////
HANDLE CGraphicsLib::GetDIB()
{
	
/*
HBITMAP  hDDB,    // Handle der DDB
HPALETTE hPal,    // Handle der Farbpalette der DDB
UINT     uBPP,    // Anzahl der Bit pro Pixel für die DIB
UINT     uStyle)  // Komprimierungsart der DIB
	*/
	LPBITMAPINFOHEADER lpBI;       // Zeiger auf DIB-Eigenschaften
	LPBITMAPINFO       lpBMI;      // Zeiger auf DIB-Informationsblock
	BYTE		         *lpBits;     // Zeiger auf Bilddaten der DIB
	BITMAPINFOHEADER   BI;         // DIB-Eigenschaften
	BITMAP             BM;         // DDB-Eigenschaften
	HPALETTE           hOldPal;    // Handle der vorigen Palette
	HANDLE             hDIB;       // Handle der DIB
	HANDLE             h;          // Handle
	HDC                hDC;        // Handle des Ausgabekontextes
	DWORD              dwSize;     // Größe der DIB
	UINT               uLines;     // Anzahl der übertragenen Zeilen
	
	HBITMAP  hDDB = m_hBITMAP;
	HPALETTE hPal=NULL;//DIB_RGB_COLORS;
	UINT     uBPP=m_wBitsPerPixel;
	UINT     uStyle = m_iCompression;
	
	// Überprüfe Farbpalette
	if (!hPal)
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);  // Lade Standardpalette
	
	// Ermittle DDB-Daten
	GetObject(hDDB, sizeof(BITMAP), (LPSTR) &BM);
	if (uBPP == 0)
		uBPP = BM.bmPlanes * BM.bmBitsPixel;
	
	// BITMAPINFOHEADER auffüllen
	BI.biSize          = sizeof(BITMAPINFOHEADER);
	BI.biWidth         = BM.bmWidth;   // Breite der DIB
	BI.biHeight        = BM.bmHeight;  // Höhe der DIB
	BI.biPlanes        = 1;            // Anzahl der Farbebenen
	BI.biBitCount      = uBPP;         // 1/4/8/16/24/32 BPP
	BI.biCompression   = uStyle;       // Keine Komprimierung
	BI.biSizeImage     = 0l;           // Bitmapgröße auf DWORD
	BI.biXPelsPerMeter = 2953;         // 75 dpi
	BI.biYPelsPerMeter = 2953;         // 75 dpi
	BI.biClrUsed       = 0;            // Löschen für GetColorsInPalette
	BI.biClrUsed       = m_ibiClrUsed;  // Anzahl der Farben
	// in der Farbtabelle
	BI.biClrImportant  = 0;            // Anzahl der wichtigen Farben
	
	// Speicher für DIB anfordern
	dwSize = BI.biSize + BI.biClrUsed * sizeof(RGBQUAD);
	hDIB = GlobalAlloc(GHND, dwSize);
	if (!hDIB)
    {
		return NULL;  // Speichermangel -> NULL
    }
	
	// Zeiger auf DIB bestimmen und Informationsblock übertragen
	lpBMI  = (LPBITMAPINFO)       GlobalLock(hDIB);
	lpBI   = (LPBITMAPINFOHEADER) lpBMI;
	*lpBI  = BI;  // Informationsblock übertragen
	
	// Ausgabekontext anfordern
	hDC = GetDC(NULL);
	
	// Palette aktivieren
	hOldPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);
	
	// Berechnung der Größe der Bitmapdaten mit GetDIBits
	GetDIBits(hDC, hDDB, 0, (UINT) BI.biHeight, NULL, lpBMI,
		DIB_RGB_COLORS);
	BI = *lpBI;  // Informationsblock kopieren
	GlobalUnlock(hDIB);  // DIB wieder verschiebbar machen
	
	// Überprüfe Größe der Bitmapdaten
	if (!BI.biSizeImage)
    {  // Größe wurde von GetDIBits nicht berechnet
		BI.biSizeImage = ((BI.biWidth * uBPP + 31) / 32) * 4 *
			BI.biHeight;
		if (uStyle != BI_RGB)
        {
			if (hOldPal)           // Alte Palette reaktivieren
				SelectPalette(hDC, hOldPal, FALSE);
			ReleaseDC(NULL, hDC);  // Ausgabekontext wieder freigeben
			GlobalFree(hDIB);      // DIB wieder freigeben
			return NULL;
        }
    }
	
	// Mehr Speicher für DIB erneut anfordern
	BI.biClrUsed = m_ibiClrUsed; // Neu berechnen wegen Fehler
	// in GetDIBits bei manchen
	// Grafiktreibern
	dwSize = BI.biSize + BI.biClrUsed * sizeof(RGBQUAD) + BI.biSizeImage;
	h = GlobalReAlloc(hDIB, dwSize, GHND);
	if (!h)
    {
		
		if (hOldPal)           // Alte Palette reaktivieren
			SelectPalette(hDC, hOldPal, FALSE);
		ReleaseDC(NULL, hDC);  // Ausgabekontext wieder freigeben
		GlobalFree(hDIB);      // DIB wieder freigeben
		return NULL;           // Speichermangel -> NULL
    }
	else
		hDIB = h;
	
	// Zeiger auf DIB bestimmen und Informationsblock übertragen
	lpBMI  = (LPBITMAPINFO)       GlobalLock(hDIB);
	lpBI   = (LPBITMAPINFOHEADER) lpBMI;
	*lpBI  = BI;  // Informationsblock übertragen
	lpBits = (BYTE *) &(lpBMI->bmiColors[m_ibiClrUsed]);
	
	// Übertrage Bilddaten in DIB
	uLines = GetDIBits(hDC, hDDB, 0, (UINT) BI.biHeight, lpBits, lpBMI,
		DIB_RGB_COLORS);
	
	// Aufräumarbeiten
	GlobalUnlock(hDIB);    // DIB wieder verschiebbar machen
	if (hOldPal)           // Alte Palette reaktivieren
		SelectPalette(hDC, hOldPal, FALSE);
	ReleaseDC(NULL, hDC);  // Ausgabekontext wieder freigeben
	if (uLines == 0)
    {
		GlobalFree(hDIB);   // DIB wieder freigeben
		return NULL;
    }
	
	return hDIB;  // DIB zurückgeben
}
//////////////////////////////////////////////////////////////////////
void CGraphicsLib::Convolve2D()
{
	
/* CombineMethod

  IPL_SUM Sums the results.
  IPL_SUMSQ Sums the squares of the results.
  IPL_SUMSQROOT Sums the squares of the results and then takes the square root.
  IPL_MAX Takes the maximum of the results.
  IPL_MIN
	*/
	
	
#ifdef _DEBUG
	int dwTick;
	dwTick = GetTickCount();
#endif
	
	HBITMAP hDestinationBITMAP;
	IplImage *pDestinationIPLBitmap;

	hDestinationBITMAP = CreateBitmap(m_lWidth,m_lHeight,m_wBitsPerPixel);
	
	pDestinationIPLBitmap = DIBToIPL(hDestinationBITMAP);
	
	
	int anchorX = 1;
	int anchorY = 1;
	int nShiftR = 5;
	int nCols   = 3;
	int nRows   = 3;
	int combineMethod = IPL_SUM;
	int one[9] = {-1,-1,-1, -1,42,-1, -1,-1,-1};
	
	IplConvKernel* kernel=iplCreateConvKernel(nCols,nRows,
		anchorX,anchorY, one, nShiftR);
	
	
	iplConvolve2D(m_pIPLBitmap,pDestinationIPLBitmap,&kernel,1,combineMethod);
	
	iplDeallocate(m_pIPLBitmap, IPL_IMAGE_HEADER);
	m_pIPLBitmap = pDestinationIPLBitmap;
	
	DeleteObject(m_hBITMAP);
	m_hBITMAP = hDestinationBITMAP;
	
	
#ifdef _DEBUG
	TRACE("FIXED FILTER SAMPLING TIME: %i\n",GetTickCount()-dwTick);
#endif
}
//////////////////////////////////////////////////////////////////////
BOOL CGraphicsLib::RGB2YUV(BYTE* Y, BYTE* U, BYTE* V)
{
	// RGB -> YUV
#if (1)
	TRACE("RGB2YUV %dx%d\n",m_lWidth,m_lHeight);
	BYTE r,g,b;
	BYTE y,u,v;
	BYTE* pRGB = (BYTE*)m_pIPLBitmap->imageData;
	BOOL bEven;

	for (int i=0;i<m_lWidth;i++)
	{
		bEven = TRUE;
		for (int j=0;j<m_lHeight;j++)
		{
			r = *pRGB++;
			g = *pRGB++;
			b = *pRGB++;
			y	=  0.257*r + 0.504*g + 0.098*b + 16;
			*Y = (BYTE)y;
			*Y++;
			if (bEven)
			{
				u	=  0.439*r - 0.368*g - 0.071*b+128;
				v	= -0.148*r - 0.291*g + 0.439*b+128;
				*U = (BYTE)u;
				*V = (BYTE)v;
				*U++;
				*V++;
			}
			bEven = !bEven;
		}
	}
#else
	IplImage* pDestinationIPLBitmap;
	
	pDestinationIPLBitmap = iplCreateImageHeader(3,				// nr of channels
												 0,				// no alpha channel
												 IPL_DEPTH_8U,	// data of byte type
												 "YUV",			// color model
												 "YUV",			// color order
												 IPL_DATA_ORDER_PLANE, // channel arrangement
												 IPL_ORIGIN_TL,	// top left orientation
												 IPL_ALIGN_DWORD, // 4 bytes align
												 m_lWidth,		// image width
												 m_lHeight,		// image height
												 NULL,			// no ROI
												 NULL,			// no mask ROI
												 NULL,			// no image ID
												 NULL			// no title
												);
	iplAllocateImage(pDestinationIPLBitmap,0,0); // don't fill
	iplRGB2YUV(m_pIPLBitmap,pDestinationIPLBitmap);

	// copy the data
	DWORD dwPlaneLength = m_lWidth * m_lHeight;
	BYTE* pData = (BYTE*)pDestinationIPLBitmap->imageData; 
	CopyMemory(Y,pData,dwPlaneLength);
	pData += dwPlaneLength;
	CopyMemory(U,pData,dwPlaneLength/2);
	pData += dwPlaneLength/2;
	CopyMemory(V,pData,dwPlaneLength/2);
	
	iplDeallocate(pDestinationIPLBitmap,IPL_IMAGE_DATA);
#endif
	return TRUE;
}
	/*
	BITMAPINFOHEADER b;

	ZeroMemory(&b,sizeof(b));
	b.biSize = sizeof(b);
	b.biWidth = m_lWidth;
	b.biHeight = m_lHeight;
	b.biPlanes = 1;
    b.biCompression = BI_RGB;
	b.biBitCount = 8;

	CDib dib(&b,(BYTE*)pDestinationIPLBitmap->imageData,pDestinationIPLBitmap->imageSize);
	static int filenamecounter = 0;
	CString s;
	s.Format("c:\\out\\yuv.bmp",filenamecounter++);
	dib.Save(s);

  */
