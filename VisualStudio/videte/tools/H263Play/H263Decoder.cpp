// H263Decoder.cpp: implementation of the CH263Decoder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "H263Play.h"
#include "H263Decoder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TABLES
#include "tables.h"

#define __USE_DD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CH263Decoder::CH263Decoder()
{
	trace = 0;
    m_bQuiet = TRUE;
	fault = 0;

	m_bFit    = FALSE;
	m_iZoom   = 1;
	m_clp     = NULL;
	m_iHeight = 0;
	m_iWidth  = 0;
	
#if 1
	m_bGDI = FALSE;
	m_bDD  = TRUE;
#else
	m_bGDI = TRUE;
	m_bDD  = FALSE;
#endif

	int cc;
	for (cc=0; cc<3; cc++) 
	{
		m_refframe[cc] = NULL;
		m_oldrefframe[cc] = NULL;
		m_bframe[cc] = NULL;
		m_edgeframeorig[cc] = NULL;
	}

	m_hWnd = NULL;
	m_hDrawDib = NULL;
	memset(&m_biHeader,0,sizeof(BITMAPINFOHEADER));
	m_biHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bImageIsReady = FALSE;
	m_pBufRGB = NULL;
	m_pSrc[0] = NULL;
	m_pSrc[1] = NULL;
	m_pSrc[2] = NULL;

	m_bRun = TRUE;
	m_pDecodeThread = AfxBeginThread(DecodeThreadProc,(LPVOID)this);
	m_pDecodeThread->m_bAutoDelete = FALSE;
}
//////////////////////////////////////////////////////////////////////
CH263Decoder::~CH263Decoder()
{
	m_bRun = FALSE;

	if (m_pDecodeThread)
	{
		WaitForSingleObject(m_pDecodeThread->m_hThread,5000);
		delete m_pDecodeThread;
	}

	if (m_clp)
	{
		m_clp -= 384;
		free(m_clp);
	}
	int cc;
	for (cc=0; cc<3; cc++) 
	{
		if (m_refframe[cc])
		{
			free(m_refframe[cc]);
		}
		if (m_oldrefframe[cc])
		{
			free(m_oldrefframe[cc]);
		}
		if (m_bframe[cc])
		{
			free(m_bframe[cc]);
		}
		if (m_edgeframeorig[cc])
		{
			free(m_edgeframeorig[cc]);
		}
	}
}
//////////////////////////////////////////////////////////////////////
int	CH263Decoder::GetHeight()
{
	return m_iHeight;
}
//////////////////////////////////////////////////////////////////////
int	CH263Decoder::GetWidth()
{
	return m_iWidth;
}
//////////////////////////////////////////////////////////////////////
BOOL CH263Decoder::GetFit()
{
	return m_bFit;
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::SetFit(BOOL bFit)
{
	m_bFit = bFit;
	if (m_bDD)
	{
		m_DD.SetFit(bFit);
	}
}
//////////////////////////////////////////////////////////////////////
int	CH263Decoder::GetZoom()
{
	return m_iZoom;
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::SetZoom(int iZoom)
{
	if ((iZoom==1) || (iZoom==2))
	{
		m_iZoom = iZoom;
		if (m_bDD)
		{
			m_DD.SetZoom(iZoom);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::initdecoder()
{
	int i, cc, size;
	
	/* clip table */
	if (!(m_clp=(unsigned char *)malloc(1024)))
		error("malloc failed\n");
	
	m_clp += 384;
	
	for (i=-384; i<640; i++)
		m_clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

	
	switch (source_format) 
	{
		case (SF_SQCIF):
			horizontal_size = 128;
			vertical_size = 96;
			break;
		case (SF_QCIF):
			horizontal_size = 176;
			vertical_size = 144;
			break;
		case (SF_CIF):
			horizontal_size = 352;
			vertical_size = 288;
			break;
		case (SF_4CIF):
			horizontal_size = 704;
			vertical_size = 576;
			break;
		case (SF_16CIF):
			horizontal_size = 1408;
			vertical_size = 1152;
			break;
		default:
			TRACE("ERROR: Illegal input format\n");
			exit(-1);
			break;
	}
	
	
	mb_width = horizontal_size/16;
	mb_height = vertical_size/16;
	coded_picture_width = horizontal_size;
	coded_picture_height = vertical_size;
	chrom_width =  coded_picture_width>>1;
	chrom_height = coded_picture_height>>1;
	blk_cnt = 6;
	
	for (cc=0; cc<3; cc++) {
		if (cc==0)
			size = coded_picture_width*coded_picture_height;
		else
			size = chrom_width*chrom_height;
		
		if (!(m_refframe[cc] = (unsigned char *)malloc(size)))
			error("malloc failed\n");
		
		if (!(m_oldrefframe[cc] = (unsigned char *)malloc(size)))
			error("malloc failed\n");
		
		if (!(m_bframe[cc] = (unsigned char *)malloc(size)))
			error("malloc failed\n");
	}
	
	for (cc=0; cc<3; cc++) {
		if (cc==0) {
			size = (coded_picture_width+64)*(coded_picture_height+64);
			if (!(m_edgeframeorig[cc] = (unsigned char *)malloc(size)))
				error("malloc failed\n");
			m_edgeframe[cc] = m_edgeframeorig[cc] + (coded_picture_width+64) * 32 + 32;
		}
		else {
			size = (chrom_width+32)*(chrom_height+32);
			if (!(m_edgeframeorig[cc] = (unsigned char *)malloc(size)))
				error("malloc failed\n");
			m_edgeframe[cc] = m_edgeframeorig[cc] + (chrom_width+32) * 16 + 16;
		}
	}
	
	/* IDCT */
#ifdef _REFIDCT
	init_idctref();
#else
	init_idct();
#endif
	
	initDisplay(coded_picture_width, coded_picture_height);
}
//////////////////////////////////////////////////////////////////////
int CH263Decoder::initDisplay (int pels, int lines)
{
	int errFlag = 0;
	
	m_iWidth = pels;
	m_iHeight = lines;
	m_biHeader.biWidth = m_iWidth;
	m_biHeader.biHeight = m_iHeight;
	m_biHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_biHeader.biCompression = BI_RGB;
	m_biHeader.biPlanes = 1;
	m_biHeader.biBitCount = 24;
	m_biHeader.biSizeImage = 3 * m_iWidth * m_iHeight;
	m_bImageIsReady = FALSE;
	
	/* allocate the memory needed to hold the RGB and visualization information */
	m_pBufRGB = (BYTE*)malloc (3 * m_iWidth * m_iHeight);
	m_iZoom = 1;

	HDC hDC = GetDC (m_hWnd);
	m_hDrawDib = DrawDibOpen();
	DrawDibBegin (
		m_hDrawDib, 
		hDC, 
		2*m_iWidth, 
		2*m_iHeight, 
		&m_biHeader, 
		m_iWidth, 
		m_iHeight, 
		0
		);
	ReleaseDC(m_hWnd, hDC);

	return errFlag;
}
//////////////////////////////////////////////////////////////////////
int CH263Decoder::closeDisplay()
{
	int errFlag = 0;
	
	m_bImageIsReady = TRUE;
	HDC hDC = GetDC (m_hWnd);
	DrawDibEnd (m_hDrawDib);
	DrawDibClose (m_hDrawDib);
	ReleaseDC (m_hWnd, hDC);
	
	free(m_pBufRGB);
	
	return errFlag;
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::Init(HWND hWnd)
{
	m_hWnd = hWnd;
	if (m_bDD)
	{
		m_DD.Init(hWnd);
	}
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::Exit()
{
	closeDisplay();
	if (m_bDD)
	{
		m_DD.Exit();
	}
}
//////////////////////////////////////////////////////////////////////
UINT DecodeThreadProc(LPVOID lpParam)
{
	CH263Decoder* pDec = (CH263Decoder*)lpParam;
	DWORD dwWaitResult;
	int first, framenum;
	DWORD dwT = 0;
	CString sFPS;

	first = 1;
	framenum = 0;

	while (pDec->m_bRun)
	{
		dwWaitResult = WaitForSingleObject(pDec->m_EventPictureArrived,100);
		if (dwWaitResult!=WAIT_TIMEOUT)
		{
			pDec->temp_ref = 0;
			pDec->prev_temp_ref -1; 

			dwT = GetTickCount();
			framenum = 0;

			while (pDec->m_bRun && pDec->getheader()) 
			{
				if (first) 
				{
					pDec->initdecoder();
					first = 0;
				}
				pDec->getpicture(&framenum);
				framenum++;
				sFPS.Format("FPS = %f\n",((double)framenum*1000)/(double(GetTickCount()-dwT)));
				pDec->m_sFPS = sFPS;
			}
		}
	}


	return 0x0DEC;
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::Decode(const CString& sFilename)
{
	if (!m_Bits.Load(sFilename)) 
	{
		TRACE("Input file %s not found\n",sFilename);
	}

	m_EventPictureArrived.SetEvent();
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::error(char *text)
{
	TRACE(text);
	exit(1);
}
//////////////////////////////////////////////////////////////////////
/* trace output */
void CH263Decoder::printbits(int code,int bits,int len)
{
	int i;
	for (i=0; i<len; i++)
		TRACE("%d",(code>>(bits-1-i))&1);
}
//////////////////////////////////////////////////////////////////////
int CH263Decoder::displayImage (unsigned char *lum, unsigned char *Cr, unsigned char *Cb)
{
	m_pSrc[0] = lum;
	m_pSrc[1] = Cb;
	m_pSrc[2] = Cr;

	m_bImageIsReady = TRUE;
	
	if (m_bGDI)
	{
		m_YUVToRGB.ConvertYUVtoRGB(m_pSrc[0],m_pSrc[1],m_pSrc[2],m_pBufRGB,m_iWidth,m_iHeight);
	}
	else
	{
		m_DD.DrawFrameYUV(m_pSrc[0],m_pSrc[1],m_pSrc[2],m_iWidth,m_iHeight);
	}
	PostMessage(m_hWnd,WM_COMMAND,ID_DRAWFRAME,0);

	while (m_bRun)
	{
		if (WAIT_OBJECT_0==WaitForSingleObject(m_EventPictureBlitted,1000))
		{
			break;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
void CH263Decoder::OnDraw(HDC hDC)
{
	if (m_bGDI)
	{
#ifndef _DEBUG
		if (m_bImageIsReady)
		{
			RECT rect;

			GetClientRect(m_hWnd,&rect);
			DrawDibDraw(m_hDrawDib, 
						hDC, 
						0, 
						0, 
						m_bFit ? rect.right-rect.left : m_iZoom * m_iWidth, 
						m_bFit ? rect.bottom-rect.top : m_iZoom * m_iHeight, 
						&m_biHeader, 
						m_pBufRGB, 
						0, 
						0, 
						m_iWidth, 
						m_iHeight, 
						m_bFit ? DDF_FULLSCREEN : DDF_SAME_DRAW
						); 
		}
	#else
		SetDIBitsToDevice(hDC,
						  0,0,
						  m_iZoom * m_iWidth, 
						  m_iZoom * m_iHeight, 
						  0,0,0,m_iHeight,
						  m_pBufRGB,
						  (BITMAPINFO*)&m_biHeader,
						  DIB_RGB_COLORS);
	#endif
	}

	TextOut(hDC,m_iWidth*m_iZoom+10,10,(const char*)m_sFPS,m_sFPS.GetLength()-1);
	m_EventPictureBlitted.SetEvent();
}
//////////////////////////////////////////////////////////////////////
int CH263Decoder::DrawDIB()
{											      
	HDC hDC = GetDC (m_hWnd);
#ifndef _DEBUG
	if (m_bFit)
	{
		RECT rect;
		GetClientRect(m_hWnd,&rect);

		DrawDibDraw (
			m_hDrawDib, 
			hDC, 
			0, 
			0, 
			rect.right-rect.left,
			rect.bottom-rect.top,
			&m_biHeader, 
			m_pBufRGB, 
			0, 
			0, 
			m_iWidth, 
			m_iHeight, 
			/*DDF_SAME_DRAW|*/DDF_FULLSCREEN
		);
	}
	else
	{
		DrawDibDraw (
			m_hDrawDib, 
			hDC, 
			0, 
			0, 
			m_iZoom * m_iWidth, 
			m_iZoom * m_iHeight, 
			&m_biHeader, 
			m_pBufRGB, 
			0, 
			0, 
			m_iWidth, 
			m_iHeight, 
			DDF_SAME_DRAW
		);
	}
	
#else	
	SetDIBitsToDevice(hDC,
					  0,0,
					  m_iZoom * m_iWidth, 
					  m_iZoom * m_iHeight, 
					  0,0,0,m_iHeight,
					  m_pBufRGB,
					  (BITMAPINFO*)&m_biHeader,
					  DIB_RGB_COLORS);
#endif
	ReleaseDC(m_hWnd, hDC);
	return 0;
}
