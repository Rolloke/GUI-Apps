// MPEG4Encoder.h: interface for the CMPEG4Encoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPEG4ENCODER_H__4F07EB33_262C_4AB4_825D_DFE174312C3C__INCLUDED_)
#define AFX_MPEG4ENCODER_H__4F07EB33_262C_4AB4_825D_DFE174312C3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STREAM_INFO2_LENGTH	   28	

extern "C"{
#include "exp_SP_MPEG4_Enc.h"
}

class CMPEG4Encoder  
{
// Konstruktor / Destruktor
public:
	CMPEG4Encoder(CString lpszInputFilePathName);
	virtual ~CMPEG4Encoder();

// Attribute
private:
	uint8*                 m_pInputFrameBuffer;
	uint8*                 m_pOutputFrameBuffer;
	tBaseVideoEncoder*     m_pMP4EncHandle;
	tMPEG4VideoEncParam    m_lVideoEncParam;
	CString                m_lpszInputFilePathName;
	BOOL                   m_bThreadIsRunning;
    uint32*                m_pPacketSizes;
	FILE*                  m_pInputFileStream;
	FILE*                  m_pOutputFileStream;
	uint32                 m_lOutBufSize;
	uint32                 m_lInputFrameSize;
	CWinThread*	           m_pEncodeThread;
	BYTE                   m_pStreamInfoHeader[STREAM_INFO2_LENGTH];
	DWORD                  m_dwByteCnt;

// Methoden
public:
	BOOL IsStreamInfoHeader(BYTE *buffer, DWORD length);
	BOOL ReadParamFile();
	BOOL Init(int VOPWidth, int VOPHeight, int fps);
	static UINT EncodeThreadProc(LPVOID pParam);
};

#endif // !defined(AFX_MPEG4ENCODER_H__4F07EB33_262C_4AB4_825D_DFE174312C3C__INCLUDED_)
