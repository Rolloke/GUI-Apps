// H26xEncoder.h: interface for the CH26xEncoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H26XENCODER_H__BCD37AE3_0325_11D5_99A6_004005A19028__INCLUDED_)
#define AFX_H26XENCODER_H__BCD37AE3_0325_11D5_99A6_004005A19028__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CH26xEncoder  
{
	// construction/destruction
public:
		CH26xEncoder();
	virtual ~CH26xEncoder();

	// attributes
public:

	// operations
public:
	BOOL Init(int w=352, int h=288);
	BOOL ForceIntraFrame();
	void Run();
	CIPCPictureRecord* GetEncodedPicture(CIPC* pCIPC, CSecID camID, const CSystemTime& stTimesStamp);
	void SetDIBData(const BYTE* pData);

	// implementation
protected:
	static void   PutCallBack(BYTE c,void *context);
	static void   FrameCallback(FRAME *curr_frame, FRAME *reco_frame, int frame_type, int bits, void *context);
	static FRAME* GrabberCallback(STREAM *s, FRAME *f, int skip_frame, void *context);
	static void   GrabInit(int w, int h, int skip_rate_s, int skip_rate_t, void *context);


private:
	int					m_iWidth;
	int					m_iHeight;			
	ENC_STATUS*			m_pEncHandle;
	int					m_iCurrentFrameType;
	int					m_iCurrentBits;
	BYTE*				m_pRGB;

	BYTE*				m_pH263;
	DWORD				m_dwH263BufferLen;
	DWORD				m_dwH263CurrentLen;
};

#endif // !defined(AFX_H26XENCODER_H__BCD37AE3_0325_11D5_99A6_004005A19028__INCLUDED_)
