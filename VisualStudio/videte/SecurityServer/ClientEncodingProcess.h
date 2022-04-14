// ClientEncodingProcess.h: interface for the CClientEncodingProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTENCODINGPROCESS_H__D8D52264_1135_11D4_A0EA_004005A19028__INCLUDED_)
#define AFX_CLIENTENCODINGPROCESS_H__D8D52264_1135_11D4_A0EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientProcess.h"
#include "TranscoderThread.h"

class CClientEncodingProcess : public CClientProcess  
{
public:
	CClientEncodingProcess(CProcessScheduler* pScheduler,
							CSecID idClient,
							CSecID camID,
							Resolution res, 
							Compression comp,
							CompressionType ct,
							DWORD dwClientTimeSlice,
							int iNumPics=1);
	virtual ~CClientEncodingProcess();

	// attributes
public:
	inline virtual BOOL IsClientEncodingProcess() const;
	inline int GetClientBitrate() const;
	inline int GetFrameCounter() const;

	// overrides
public:
	virtual void PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY, DWORD dwUserID);
	virtual void OnModified(BOOL bNewOutputID);
	virtual CString Format();

	// operations
public:
	void SendPicture(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY);
	BOOL IsPictureTimeout();
	void ResetCounter();

protected:

private:
	volatile int	   m_iFrameCounter;
	CCriticalSection   m_csFrameCounter;
	int				   m_iClientBitrate;
	DWORD			   m_iPictsSendForCamera;
	BOOL			   m_bUseH263;
	BOOL			   m_bUseMpeg4;
	BOOL			   m_bIsStreaming;
	BOOL			   m_bIsMultiCamera;
	BOOL			   m_bCanGOP;
	BOOL			   m_bDropped;
	Resolution		   m_ClientResolution;
	Resolution		   m_UnitResolution;
	CTranscoderThread* m_pTranscoder;

	CCriticalSection   m_csTimeStamps;
	CSystemTime		   m_stLastPictureGot;
	CSystemTime		   m_stLastPictureSent;
	DWORD			   m_dwLastPictureSent;
};
///////////////////////////////////////////////////////////////////////////
inline BOOL CClientEncodingProcess::IsClientEncodingProcess() const
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
inline int CClientEncodingProcess::GetClientBitrate() const
{
	return m_iClientBitrate;
}
///////////////////////////////////////////////////////////////////////////
inline int CClientEncodingProcess::GetFrameCounter() const
{
	return m_iFrameCounter;
}

#endif // !defined(AFX_CLIENTENCODINGPROCESS_H__D8D52264_1135_11D4_A0EA_004005A19028__INCLUDED_)
