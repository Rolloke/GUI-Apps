// TranscoderThread.h: interface for the CTranscoderThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSCODERTHREAD_H__F0AAAD64_8E39_437C_AC63_62A473E7948F__INCLUDED_)
#define AFX_TRANSCODERTHREAD_H__F0AAAD64_8E39_437C_AC63_62A473E7948F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCOutputDVClient;
class CCamera;


WK_PTR_LIST_SEMA(CIPCPictureRecordListPlain,CIPCPictureRecordPtr,CIPCPictureRecordQueue);

class CTranscoderThread : public  CWK_Thread  
{
public:
	CTranscoderThread(CIPCOutputDVClient* pClient);
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
	CIPCOutputDVClient* m_pClient;
	CCamera*			m_pCamera;
	CIPCPictureRecordQueue  m_PictureQueueH263;
	CIPCPictureRecordQueue  m_PictureQueueMpeg4;
	CJpeg*			m_pJpeg;
	CH26xEncoder*	m_pH263Encoder;
	CMPEG4Encoder*  m_pMpeg4Encoder;
	CWK_PerfMon		m_PerfMonMPEG4;
	CSecID			m_idLastCamera;
	DWORD			m_dwBitrate;
	int				m_iNumPictures;
};

#endif // !defined(AFX_TRANSCODERTHREAD_H__F0AAAD64_8E39_437C_AC63_62A473E7948F__INCLUDED_)
