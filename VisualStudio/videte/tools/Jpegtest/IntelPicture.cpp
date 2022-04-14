// IntelPicture.cpp: implementation of the CIntelPicture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "jpegtest.h"
#include "IntelPicture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIntelPicture::CIntelPicture()
{
	Init();
}
//////////////////////////////////////////////////////////////////////
CIntelPicture::~CIntelPicture()
{
	Deinit();
}
//////////////////////////////////////////////////////////////////////
void CIntelPicture::Init()
{
	ZeroMemory(&m_Image,sizeof(IplImage));
	m_Image.nSize = sizeof(IplImage);
	m_Image.nChannels = 3;
	m_Image.align = IPL_ALIGN_DWORD;
	m_Image.depth = IPL_DEPTH_8U;
	m_Image.dataOrder = IPL_DATA_ORDER_PIXEL;
	m_Image.imageData = NULL;
	m_iColorSpace = 0;
}
//////////////////////////////////////////////////////////////////////
void CIntelPicture::Deinit()
{
	if (m_Image.imageData)
	{
		iplDeallocate(&m_Image,IPL_IMAGE_DATA);
	}
}
//////////////////////////////////////////////////////////////////////
void CIntelPicture::SetData(int iWidth, int iHeight, const BYTE* pBuffer, IJL_COLOR color)
{
	Deinit();
	Init();

	m_Image.height = iHeight;
	m_Image.width = iWidth;
	m_iColorSpace = color;

	switch (color)
	{
	case IJL_RGB:
		strcpy(m_Image.colorModel,"RGB");
		m_Image.nChannels = 3;
		break;
	case IJL_BGR:
		strcpy(m_Image.colorModel,"BGR");
		m_Image.nChannels = 3;
		break;
	case IJL_YCBCR:
		strcpy(m_Image.colorModel,"YUV");
		m_Image.nChannels = 2;
		break;
	default:
		TRACE("unsupported color model\n");
		break;
	}

	iplAllocateImage(&m_Image,0,0);
	if (m_Image.imageData)
	{
		CopyMemory(m_Image.imageData,pBuffer,m_Image.height*m_Image.width*m_Image.nChannels);
		TRACE("image allocated\n");
	}
}
//////////////////////////////////////////////////////////////////////
void CIntelPicture::Resize(int iWidth, int iHeight)
{
	IplImage newImage;
	newImage = m_Image;
	newImage.imageData = NULL;
	newImage.width = iWidth;
	newImage.height = iHeight;
	iplAllocateImage(&newImage,0,0);
	iplResize(&m_Image,&newImage,iWidth,m_Image.width,iHeight,m_Image.height,IPL_INTER_LINEAR);
	Deinit();
	m_Image = newImage;
}
