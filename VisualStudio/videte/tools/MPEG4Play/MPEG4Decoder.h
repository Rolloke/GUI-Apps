// MPEG4Decoder.h: interface for the CMPEG4Decoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPEG4DECODER_H__AC8F3ED2_DD03_4110_A9B0_AE5F001790C1__INCLUDED_)
#define AFX_MPEG4DECODER_H__AC8F3ED2_DD03_4110_A9B0_AE5F001790C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vfw.h"
#include "exp_mp4sp_vd.h"
#include "exp_H263_vd.h"

#define COMPRESSED_FRAME_SIZE  300000     // maximale Groesse eines komprimierten Video-Fames
#define STREAM_INFO_LENGTH     38         // Annahme: Groesse des Streaminfo ist immer 38 Byte
#define STREAM_INFO2_LENGTH	   28	
#define COLOR_CONV_PRECISION   14
#define COLOR_CONV_ROUND       8192
#define SATURATE_RGB(x) (x > 255) ? 255 : (((x) < 0 ) ? 0 : (x))

static uint8 streamInfoBuffer[STREAM_INFO2_LENGTH];
//static int countStreamInfo = 0;
//static tStreamInfo streamInfoDummy;


__declspec(align(32)) static const uint16 Tab01[4] = 
{
	0x0000,  0xF364,  0x4106,  0x0000
};
__declspec(align(32)) static const uint16 Tab02[4] = 
{
	0xF364,  0x4106,  0x0000,  0xF364
};
__declspec(align(32)) static const uint16 Tab03[4] = 
{
	0x4106,  0x0004,  0xF360,  0x4106
};
__declspec(align(32)) static const uint16 Tab04[4] = 
{
	0x247B,  0xED68,  0x0000,  0x247B
};
__declspec(align(32)) static const uint16 Tab05[4] = 
{
	0xED68,  0x0000,  0x247B,  0xED68
};
__declspec(align(32)) static const uint16 Tab06[4] = 
{
	0x0000,  0x247B,  0xED68,  0x0000
};
__declspec(align(32)) static const uint16 Tab07[4] = 
{
	0x0800,  0x0800,  0x0800,  0x0800
};

class CMPEG4Decoder  
{
// Konstruktor / Destruktor
public:
	CMPEG4Decoder(CString lpszInputFilePathName);
	virtual ~CMPEG4Decoder();

// Attribute
private:
	BOOL                   m_bIsH263;
	int                    m_iPrevTimestamp;
	int                    m_iInputFrameLen;
	HDC                    m_hDC;
	BOOL                   m_bThreadIsRunning;
	HDRAWDIB               m_hDrawDib;
	FILE*                  m_pInputFileStream;
	BITMAPINFO             m_bmi;
    BYTE*                  m_pDibImageData;
	uint8*                 m_pInputFrameBuffer;
	tBaseVideoDecoder*     m_pMP4DecHandle;
	tBaseVideoDecoder*     m_pH263DecHandle;
	tBaseVideoFrame*       m_pOutFrameYUV420;
	CString                m_lpszInputFilePathName;
	BYTE*                  m_pMPEG4Data;
	DWORD                  m_dwMPEG4DataLen;
	HBITMAP                m_hDIB;
	CCriticalSection       m_csDecoder;
	CList <tBaseVideoFrame*, tBaseVideoFrame*> m_listYUVFrameBuffer;
	CWinThread*	           m_pDecodeThread;
	CWnd*                  m_pWnd;
	UINT                   m_uMsg;
	WPARAM                 m_wParam;
	LPARAM                 m_lParam;
	int                    m_iFrameNr;
	tStreamInfo            m_streamInfo;
	int32                  m_lPostprocType;


// Methoden
public:
	void gReadVOPH263(uint8 *buffer, int32 *length, FILE *fp);
	tStreamInfo GetStreamInfo();
	tBaseVideoFrame* CreateOutFrameYUV();
	CList<tBaseVideoFrame*, tBaseVideoFrame*> * GetYUVFrameBufferList();
	BOOL Init(CWnd* pWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnDraw(CDC* pDC, const CRect &rectDest = CRect(0,0,0,0), const CRect &rectSrc = CRect(0,0,0,0));
	void ConvertYUV420FrameToDIB();
	void gReadVOP(uint8 *buffer, int32 *length, FILE *fp);
	void gYUV420toRGB24(tBaseVideoFrame *yuvFrame, uint8 *rgbFrame);
	void sYUV420toRGB24Frame(uint8 *Lum1, uint8 *Lum2, uint8 *Cb, uint8 *Cr,
                             uint8 *rgb1, uint8 *rgb2, int32 lWidth,int32 lumWidth,
                             int32 rgbWidth,int32 actHeight);
	static UINT DecodeThreadProc(LPVOID pParam);
};

#endif // !defined(AFX_MPEG4DECODER_H__AC8F3ED2_DD03_4110_A9B0_AE5F001790C1__INCLUDED_)
