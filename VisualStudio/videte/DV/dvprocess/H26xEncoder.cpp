// H26xEncoder.cpp: implementation of the CH26xEncoder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "H26xEncoder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CH26xEncoder::CH26xEncoder()
{
	m_iWidth = 0;
	m_iHeight = 0;			
	m_pEncHandle = Enc_Open();
	m_iCurrentFrameType = 0;
	m_iCurrentBits = 0;
	m_pRGB = 0;
	m_pH263 = 0;
	m_dwH263BufferLen = 0;
	m_dwH263CurrentLen = 0;
}
//////////////////////////////////////////////////////////////////////
CH26xEncoder::~CH26xEncoder()
{
	if (m_pEncHandle)
	{
		Enc_Term(m_pEncHandle);
		m_pEncHandle = NULL;
	}
	WK_DELETE(m_pRGB);
	WK_DELETE(m_pH263);
}
//////////////////////////////////////////////////////////////////////
BOOL CH26xEncoder::Init(int w/*=352*/, int h/*=288*/)
{
	int r = Enc_Init(m_pEncHandle,w,h,PutCallBack,this,NULL);

	if (r==ERR_OK)
	{
		m_iWidth = w;
		m_iHeight = h;			
		m_dwH263BufferLen = 32*1024;
		m_pH263 = new BYTE[m_dwH263BufferLen];
		m_pRGB = new BYTE[w*h*3];

		Enc_SetGrabInitCB(m_pEncHandle,GrabInit,this);
		Enc_SetGrabberCB(m_pEncHandle,GrabberCallback,this);
		Enc_SetFrameCB(m_pEncHandle,FrameCallback,this);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void CH26xEncoder::PutCallBack(BYTE c,void *context)
{
	CH26xEncoder* pThis = (CH26xEncoder*)context;

	pThis->m_pH263[pThis->m_dwH263CurrentLen] = c;
	pThis->m_dwH263CurrentLen++;

	if (pThis->m_dwH263CurrentLen > pThis->m_dwH263BufferLen)
	{
		BYTE* pSwap = new BYTE[pThis->m_dwH263BufferLen*2];
		CopyMemory(pSwap,pThis->m_pH263,pThis->m_dwH263BufferLen);
		WK_DELETE(pThis->m_pH263);
		pThis->m_dwH263BufferLen *=2;
		pThis->m_pH263 = pSwap;
	}
}
//////////////////////////////////////////////////////////////////////
void CH26xEncoder::FrameCallback(FRAME *curr_frame, FRAME *reco_frame, int frame_type, int bits, void *context)
{
	CH26xEncoder* pThis = (CH26xEncoder*)context;
	pThis->m_iCurrentFrameType = frame_type;
	pThis->m_iCurrentBits = bits;
}
//////////////////////////////////////////////////////////////////////
FRAME* CH26xEncoder::GrabberCallback(STREAM *s, FRAME *f, int skip_frame, void *context)
{
	TRACE("CH26xEncoder::GrabberCallback \n");
	FRAME* pFrame = NULL;
	CH26xEncoder* pThis = (CH26xEncoder*)context;

	if (pThis->m_pRGB)
	{
		PICT* pict = Hlp_Frame2Pict(f);

		int w,h;
		int r, g, b;
		long y1_1, y2_1, u_1, v_1, u0_1, u1_1, u2_1, v0_1, v1_1, v2_1;
		long y1_2, y2_2, u_2, v_2, u0_2, u1_2, u2_2, v0_2, v1_2, v2_2;
		long u,v;
		const BYTE* pRGB1;
		const BYTE* pRGB2;
		BYTE* pY_1 = pict->Y;
		BYTE* pY_2;
		BYTE* pU = pict->V;
		BYTE* pV = pict->U;

		pRGB1 = pThis->m_pRGB;
		w = pThis->m_iWidth;
		h = pThis->m_iHeight;
		y1_1 = y2_1 = u_1 = v_1 = u0_1 = u1_1 = u2_1 = v0_1 = v1_1 = v2_1 = 0;
		y1_2 = y2_2 = u_2 = v_2 = u0_2 = u1_2 = u2_2 = v0_2 = v1_2 = v2_2 = 0;

		for (int y=0;y<h;y+=2)
		{
			pRGB2 = pRGB1 + w*3;
			pY_2 = pY_1 + pict->line_skip;
			for (int x=0;x<w;x+=2)
			{
				// First Row First pixel gives Y and 0.5 of chroma
				r = *pRGB1++;			
				g = *pRGB1++;			
				b = *pRGB1++;			
				y1_1  =    16829*r +  33039*g +  6416*b + (0xFFFF & y2_1);
				u1_1  =    -4853*r +  -9530*g + 14383*b;
				v1_1  =    14386*r + -12046*g + -2340*b;

				// First Row Second pixel just yields a Y and 0.25 U, 0.25 V
				r = *pRGB1++;			
				g = *pRGB1++;			
				b = *pRGB1++;
				y2_1  =    16829*r +  33039*g +  6416*b + (0xFFFF & y1_1);
				u2_1  =    -2426*r +  -4765*g +  7191*b;
				v2_1  =     7193*r +  -6023*g + -1170*b;

				// Filter the chroma
				u_1 = u0_1 + u1_1 + u2_1 + (0xFFFF & u_1);
				v_1 = v0_1 + v1_1 + v2_1 + (0xFFFF & v_1);
				u0_1 = u2_1;
				v0_1 = v2_1;

				// Second Row First pixel gives Y and 0.5 of chroma
				r = *pRGB2++;			
				g = *pRGB2++;			
				b = *pRGB2++;			
				y1_2  =    16829*r +  33039*g +  6416*b + (0xFFFF & y2_2);
				u1_2  =    -4853*r +  -9530*g + 14383*b;
				v1_2  =    14386*r + -12046*g + -2340*b;

				// Second Row Second pixel just yields a Y and 0.25 U, 0.25 V
				r = *pRGB2++;			
				g = *pRGB2++;			
				b = *pRGB2++;
				y2_2  =    16829*r +  33039*g +  6416*b + (0xFFFF & y1_2);
				u2_2  =    -2426*r +  -4765*g +  7191*b;
				v2_2  =     7193*r +  -6023*g + -1170*b;

				// Filter the chroma
				u_2 = u0_2 + u1_2 + u2_2 + (0xFFFF & u_2);
				v_2 = v0_2 + v1_2 + v2_2 + (0xFFFF & v_2);
				u0_2 = u2_2;
				v0_2 = v2_2;

				// Y erste Zeile
				*pY_1++ = (y1_1>>16) + 16;
				*pY_1++ = (y2_1>>16) + 16;
				
				// Y zweite Zeile
				*pY_2++ = (y1_2>>16) + 16;
				*pY_2++ = (y2_2>>16) + 16;

				u = (u_1 + u_2)/2;
				v = (v_1 + v_2)/2;

				*pU++ = (u>>16) + 128;
				*pV++ = (v>>16) + 128;
			}

			// 2 Zeilen fertig
			pRGB1 += w*3;
			pY_1 += 2 * pict->line_skip - x;
			pU += pict->line_skip - x/2;
			pV += pict->line_skip - x/2;
		}
		pFrame = Hlp_Pict2Frame(pict);
	}
	return pFrame;
}
//////////////////////////////////////////////////////////////////////
void CH26xEncoder::GrabInit(int w, int h, int skip_rate_s, int skip_rate_t, void *context)
{
	TRACE("CH26xEncoder::GrabInit(w=%d,h=%d,skip_rate_s=%d,skip_rate_t=%d\n",w,h,skip_rate_s,skip_rate_t);
}
//////////////////////////////////////////////////////////////////////
void CH26xEncoder::Run()
{
	if (m_pEncHandle)
	{
		Enc_Loop(m_pEncHandle,1);
	}
}
//////////////////////////////////////////////////////////////////////
CIPCPictureRecord* CH26xEncoder::GetEncodedPicture(CIPC* pCIPC, CSecID camID,const CSystemTime& stTimesStamp)
{
	CIPCPictureRecord* pReturn = NULL;
	if (m_pH263)
	{
		CIPCSavePictureType spt;

		if (m_iCurrentFrameType==FRAME_TYPE_I)
		{
			spt = SPT_FULL_PICTURE;
			Enc_SetIntraInterval(m_pEncHandle,100);
		}
		else
		{
			spt = SPT_DIFF_PICTURE;
		}

		pReturn = new CIPCPictureRecord(pCIPC,
										m_pH263,
										m_dwH263CurrentLen,
										camID,
										RESOLUTION_HIGH,
										COMPRESSION_12,
										COMPRESSION_H261,
										stTimesStamp,
										0,
										NULL,
										0, // Bitrate
										spt);
		m_dwH263CurrentLen = 0;
	}

	return pReturn;
}
//////////////////////////////////////////////////////////////////////
BOOL CH26xEncoder::ForceIntraFrame()
{
	if (m_pEncHandle)
	{
		Enc_SetIntraInterval(m_pEncHandle,0);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CH26xEncoder::SetDIBData(const BYTE* pData)
{
	CopyMemory(m_pRGB,pData,m_iWidth*m_iHeight*3);
}







