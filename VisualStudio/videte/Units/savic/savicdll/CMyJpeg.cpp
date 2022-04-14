// CMyJpeg.cpp: implementation of the CMyJpeg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMyJpeg.h"

#include "wk_util.h"
#include "math.h"
#include "MemCopy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Intel® JPEG Library V1.51
// Errata 
// Memory Loss and ijlRead 
//
// After each call to ijlRead to read a JPEG header, the IJL 
// loses sizeof(SCAN_COMPONENT) * number_of_color_components 
// bytes of memory. This number is in the range 24 - 96. In 
// particular, for 3 channel images the loss is 72 bytes. 
// There is no workaround for this problem. 
#define WORKAROUND_IJL15BUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMyJpeg::CMyJpeg()
{										  
	m_bValidImage			= FALSE;
	m_nChannels				= 3;
	m_iJpegQuality			= 75;
	m_pPerf1				= NULL;
	m_pPerf2				= NULL;

	ZeroMemory(&m_JpegCoreProp, sizeof(JPEG_CORE_PROPERTIES));

	SetProperties();

	m_pPerf1 = new CWK_PerfMon(_T("Scale down"));
	m_pPerf2 = new CWK_PerfMon(_T("Compression"));

}

//////////////////////////////////////////////////////////////////////
CMyJpeg::~CMyJpeg()
{
	WK_DELETE(m_pPerf1);
	WK_DELETE(m_pPerf2);
}

//////////////////////////////////////////////////////////////////////
void CMyJpeg::SetProperties(int iWidth/*=768*/,int iHeight/*=576*/,IJL_COLOR DIBColor/*=IJL_BGR*/)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_DIBColor = DIBColor;

	switch (m_DIBColor)
	{
		case IJL_BGR:
		case IJL_RGB:
			m_nChannels = 3;
			break;
		case IJL_YCBCR:
			m_nChannels = 2;
			break;
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CMyJpeg::EncodeJpegToMemory(int nSrcWidth, int nSrcHeight, int nDstWidth, int nDstHeight, int nSourceType, BYTE* pDIBData, BYTE* &pJpegData, DWORD &dwJpegLen)
{
	LPBITMAPINFO	lpBMI = NULL;;
	JPEG_CORE_PROPERTIES JpegCoreProp;
    ZeroMemory(&JpegCoreProp, sizeof(JPEG_CORE_PROPERTIES));

	if (!pJpegData || (dwJpegLen == 0) || pDIBData == NULL)
		return FALSE;

    TRY
        if (ijlInit(&JpegCoreProp) != IJL_OK)
        {
            ML_TRACE_ERROR(_T("Cannot initialize Intel JPEG library\n"));
            AfxThrowUserException();
        }

		if (nDstWidth <= 0)
			nDstWidth = nSrcWidth;

		if (nDstHeight <= 0)
			nDstHeight = nSrcHeight;

		// Wenn die Zielrechteck größer als das Quellrechteck ist wird nicht skaliert.
		nDstWidth  = min(nDstWidth, nSrcWidth);
		nDstHeight = min(nDstHeight, nSrcHeight);
		
		// Sind Ziel und Quellgröße gleich?
		if ((nDstWidth < nSrcWidth) || (nDstHeight < nSrcHeight))
			ScaleDownYUV422(CSize(nSrcWidth, nSrcHeight), CSize(nDstWidth, nDstHeight), (YUV422*)pDIBData);
		
		if (m_pPerf2)
			m_pPerf2->Start();

		JpegCoreProp.DIBWidth		= nDstWidth;
		JpegCoreProp.DIBHeight		= nDstHeight;
		JpegCoreProp.DIBBytes		= pDIBData;
		JpegCoreProp.DIBChannels	= 3;

		// Unterstützte Quellformate sind derzeit RGB24 und YUV422
		if (nSourceType == BT878_YUV2)
		{
			JpegCoreProp.DIBColor		= IJL_YCBCR;
			JpegCoreProp.DIBSubsampling	= IJL_422;
		}
		else if (nSourceType == BT878_RGB24)
		{
			JpegCoreProp.DIBColor		= IJL_BGR;
			JpegCoreProp.DIBSubsampling	= IJL_NONE;
		}
		else
			ML_TRACE_ERROR(_T("Unbenanntes Quellformat (%d)\n"), nSourceType);

		JpegCoreProp.jquality		= m_iJpegQuality;
		JpegCoreProp.JPGWidth		= nDstWidth;
		JpegCoreProp.JPGHeight		= nDstHeight;
        JpegCoreProp.JPGBytes		= pJpegData;
		JpegCoreProp.JPGSizeBytes	= dwJpegLen;
		JpegCoreProp.JPGColor		= IJL_YCBCR;
		JpegCoreProp.JPGSubsampling = IJL_422;
		JpegCoreProp.JPGChannels	= 3;

		int iRet = ijlWrite(&JpegCoreProp, IJL_JBUFF_WRITEWHOLEIMAGE);
		if (iRet != IJL_OK)
		{
            ML_TRACE_ERROR(_T("Cannot write image data to memory %d\n"),iRet);
            AfxThrowUserException();
		}
        dwJpegLen = JpegCoreProp.JPGSizeBytes;

        if (ijlFree(&JpegCoreProp) != IJL_OK)
            ML_TRACE_ERROR(_T("Cannot free Intel(R) JPEG library"));
		
		if (m_pPerf2)
			m_pPerf2->Stop();

	CATCH_ALL(e)

    if (ijlFree(&JpegCoreProp) != IJL_OK)
        ML_TRACE_ERROR(_T("Cannot free Intel(R) JPEG library"));

        return FALSE;

    END_CATCH_ALL
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CMyJpeg::ScaleDown(const CSize& siSrc, const CSize& siDst, BGR24* pSrc)
{
	if (m_pPerf1)
		m_pPerf1->Start();

	BGR24* pDst = new BGR24[siDst.cx * siDst.cy];

	if (!pSrc || !pDst)
		return FALSE;

	// Scale down: 
	int		nVScale	= siSrc.cx * siSrc.cy / siDst.cy;
	int		nHScale = (int)((1<<16)*((float)siSrc.cx / (float)siDst.cx));
	int		nSrcIndex[768];

	// Indextabelle erzeugen
	for (int nX=0; nX<siDst.cx; nX++)
		nSrcIndex[nX] = (int)(nX * nHScale) / (1<<16);

	for (int nY=0; nY<siDst.cy; nY++)
	{
		BGR24* pSrcBase = pSrc + nY * nVScale;
		BGR24* pDstBase = pDst + nY * siDst.cx;
		for (int nX=0; nX<siDst.cx; nX++)
		{	
			pDstBase[nX].bB = pSrcBase[nSrcIndex[nX]].bB;
			pDstBase[nX].bR = pSrcBase[nSrcIndex[nX]].bR;
			pDstBase[nX].bG = pSrcBase[nSrcIndex[nX]].bG;
		}
	}
//	memcpy(pSrc, pDst, siDst.cx * siDst.cy * sizeof(BGR24));
	CMemCopy cpy;
	cpy.FastCopy(pSrc, pDst, siDst.cx * siDst.cy * sizeof(BGR24));
	WK_DELETE(pDst);

	if (m_pPerf1)
		m_pPerf1->Stop();

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!
BOOL CMyJpeg::ScaleDownYUV422(const CSize& siSrc, const CSize& siDst, YUV422* pSrc)
{
	if (m_pPerf1)
		m_pPerf1->Start();

	YUV422* pDst = (YUV422*)new BYTE[siDst.cx * siDst.cy * 2];

	if (!pSrc || !pDst)
		return FALSE;

	int 	nHScale = (int)((1<<16)*((float)siSrc.cx / (float)siDst.cx));
	int		nVScale	= siSrc.cx * siSrc.cy / siDst.cy;
	int		nSrcIndex[768];

	// Indextabelle erzeugen
	for (int nX=0; nX<siDst.cx; nX++)
		nSrcIndex[nX] = (int)(nX * nHScale) / (1<<16);

	for (int nY=0; nY<siDst.cy; nY++)
	{
		YUV422* pSrcBase = pSrc + nY * nVScale / 2;
		YUV422* pDstBase = pDst + nY * siDst.cx / 2;
		for (int nX=0; nX<siDst.cx/2; nX++)
		{	
			pDstBase[nX].bY1 = pSrcBase[nSrcIndex[nX]].bY1;
			pDstBase[nX].bY2 = pSrcBase[nSrcIndex[nX]].bY2;
			pDstBase[nX].bU  = pSrcBase[nSrcIndex[nX]].bU;
			pDstBase[nX].bV  = pSrcBase[nSrcIndex[nX]].bV;
		}
	}
//	memcpy(pSrc, pDst, siDst.cx * siDst.cy * 2);
	CMemCopy cpy;
	cpy.FastCopy(pSrc, pDst, siDst.cx * siDst.cy * 2);

	WK_DELETE(pDst);

	if (m_pPerf1)
		m_pPerf1->Stop();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMyJpeg::SetQuality(int iJpegQuality)
{
   m_iJpegQuality = iJpegQuality;
}

