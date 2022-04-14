#pragma once

#define MAX_HSIZE 768
#define MAX_VSIZE 576

class CAVCodec
{
public:
	CAVCodec(void);
	virtual ~CAVCodec(void);

	BOOL Create(const CString& sShortName);
	BOOL Open(const CString& sFileName);
	void Destroy();	
	int  DecodeNextFrame(BYTE* pData, DWORD dwLen);
	BOOL OnDraw(CDC *pDC, const CRect &rectDest, const CRect &rectSrc=CRect(0,0,0,0));

private:
	void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);
	BOOL FlipVertical();

	HDRAWDIB			m_hDrawDib;
	
	AVInputFormat*		m_pAVInputFormat;
	AVFormatContext*	m_pFormatCtx;
	AVCodecContext*		m_pCodecCtx;
	AVFrame*			m_pFrame; 
    AVFrame*			m_pFrameRGB;
	AVPacket			m_packet;
	BYTE*				m_pBuffer;
	BITMAPINFO			m_bmi;

	int					m_nFrames;
};
