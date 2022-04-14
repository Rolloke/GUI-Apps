// TranscoderThread.h: interface for the CTranscoderThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSCODERTHREAD_H__F0AAAD64_8E39_437C_AC63_62A473E7948F__INCLUDED_)
#define AFX_TRANSCODERTHREAD_H__F0AAAD64_8E39_437C_AC63_62A473E7948F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCOutputServerClient;


WK_PTR_LIST_SEMA(CIPCPictureRecordListPlain,CIPCPictureRecordPtr,CIPCPictureRecordQueue);

class CTranscoderThread : public  CWK_Thread  
{
public:
	CTranscoderThread(CSecID idClient);
	virtual ~CTranscoderThread();

public:
	virtual	BOOL Run(LPVOID lpContext);
	virtual	BOOL StopThread();
			void OnModified(int iNumPictures, DWORD dwBitrate);
			void AddPictureRecordH263(const CIPCPictureRecord& pict);
			void AddPictureRecordMpeg4(const CIPCPictureRecord& pict);

protected:
			void CreateJPEGDecoder();
			void EmptyDataQueue();
			void TranscodeH263(const CIPCPictureRecord& pict);
			void TranscodeMpeg4(const CIPCPictureRecord& pict);


private:
	CSecID			m_idClient;
	CIPCPictureRecordQueue  m_PictureQueueH263;
	CIPCPictureRecordQueue  m_PictureQueueMpeg4;
	CJpeg*			m_pJpeg;
	CH26xEncoder*	m_pH263Encoder;
	CMPEG4Encoder*  m_pMpeg4Encoder;
	BOOL			m_bStreaming;
	DWORD			m_dwBitrate;
	int				m_iNumPictures;
};

#endif // !defined(AFX_TRANSCODERTHREAD_H__F0AAAD64_8E39_437C_AC63_62A473E7948F__INCLUDED_)
