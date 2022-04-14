// EncoderThread.h: interface for the CEncoderThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCODERTHREAD_H__D240D09E_ACC1_496B_B7B3_AB5DB434C069__INCLUDED_)
#define AFX_ENCODERTHREAD_H__D240D09E_ACC1_496B_B7B3_AB5DB434C069__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCamera;

class CEncoderItem
{
	// construction / destruction
public:
	CEncoderItem(CCamera* pCamera,
				 CSecID idArchive,
			     DWORD dwClientID,
				 CompressionType ctDestination,
			     const CIPCPictureRecord& pict,
			     const CIPCFields& fields);
	virtual ~CEncoderItem();

	// attributes
public:
	inline CSecID GetArchiveID() const;
	inline DWORD  GetClientID() const;
	inline const CIPCPictureRecord* GetPictureRecord() const;
	inline CIPCFields& GetFields();
	inline CCamera* GetCamera() const;
	inline CompressionType GetCompressionType() const;
	inline BOOL IsValid() const;
	inline DWORD GetX() const;
	inline DWORD GetY() const;

	// operations
public:
	void Invalidate();

	// data
private:
	CSecID			   m_idArchive;
	DWORD			   m_dwClientID;
	CIPCPictureRecord* m_pPicture;
	CIPCFields		   m_Fields;
	CCamera*		   m_pCamera;
	CompressionType	   m_ctDestination;
	BOOL			   m_bIsValid;
	DWORD			   m_dwX;
	DWORD			   m_dwY;
};
//////////////////////////////////////////////////////////////////////
inline CSecID CEncoderItem::GetArchiveID() const
{
	return m_idArchive;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CEncoderItem::GetClientID() const
{
	return m_dwClientID;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCPictureRecord* CEncoderItem::GetPictureRecord() const
{
	return m_pPicture;
}
//////////////////////////////////////////////////////////////////////
inline CIPCFields& CEncoderItem::GetFields()
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
inline CCamera* CEncoderItem::GetCamera() const
{
	return m_pCamera;
}
//////////////////////////////////////////////////////////////////////
inline CompressionType CEncoderItem::GetCompressionType() const
{
	return m_ctDestination;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CEncoderItem::IsValid() const
{
	return m_bIsValid;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CEncoderItem::GetX() const
{
	return m_dwX;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CEncoderItem::GetY() const
{
	return m_dwY;
}
//////////////////////////////////////////////////////////////////////
typedef CEncoderItem* CEncoderItemPtr;
WK_PTR_LIST_SEMA(CEncoderItemQueuePlain,CEncoderItemPtr,CEncoderItemQueue);
//////////////////////////////////////////////////////////////////////
class CEmuzedMpeg4Encoder : public CMPEG4Encoder
{
	// construction / dectruction
public:
	CEmuzedMpeg4Encoder(CSecID idCam, DWORD dwID)
	{
		m_dwID = dwID;
		m_CamID = idCam;
		m_bMarkForDelete = FALSE;
	}
	virtual ~CEmuzedMpeg4Encoder()
	{
	}

	// attributes
public:
	inline CSecID GetCamID() const;
	inline DWORD  GetID() const;
	inline BOOL   IsValid() const;
	inline BOOL	  IsDelayedDelete() const;

	// operations
public:
	void DelayedDelete()
	{
		m_bMarkForDelete = TRUE;
	}

private:
	CSecID m_CamID;
	DWORD  m_dwID;
	BOOL   m_bMarkForDelete;
};
//////////////////////////////////////////////////////////////////////
inline CSecID CEmuzedMpeg4Encoder::GetCamID() const
{
	return m_CamID;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CEmuzedMpeg4Encoder::GetID() const
{
	return m_dwID;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CEmuzedMpeg4Encoder::IsDelayedDelete() const
{
	return m_bMarkForDelete;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CEmuzedMpeg4Encoder::IsValid() const
{
	return !m_bMarkForDelete && GetWidth()>0 && GetHeight()>0;
}
//////////////////////////////////////////////////////////////////////
typedef CEmuzedMpeg4Encoder* CEmuzedMpeg4EncoderPtr;
WK_PTR_ARRAY_CS(CEmuzedMpeg4EncoderArray,CEmuzedMpeg4EncoderPtr,CEmuzedMpeg4Encoders);
//////////////////////////////////////////////////////////////////////
class CAVEncoder : public CAVCodec
{
	// construction / dectruction
public:
	CAVEncoder(CSecID idCam, DWORD dwID)
	{
		m_dwID = dwID;
		m_CamID = idCam;
		m_bMarkForDelete = FALSE;
	}
	virtual ~CAVEncoder()
	{
	}

	// attributes
public:
	inline CSecID GetCamID() const;
	inline DWORD  GetID() const;
	inline BOOL   IsValid() const;
	inline BOOL	  IsDelayedDelete() const;
	inline CDWordArray& GetClientIDs();

	// operations
public:
	void DelayedDelete()
	{
		m_bMarkForDelete = TRUE;
	}
	void AddClientID(DWORD dwClient)
	{
		m_dwClientIDs.Add(dwClient);
	}
	void RemoveClientID(DWORD dwClient)
	{
		for (int i=0;i<m_dwClientIDs.GetSize();i++)
		{
			if (m_dwClientIDs[i] == dwClient)
			{
				m_dwClientIDs.RemoveAt(i);
				break;
			}
		}
	}

private:
	CSecID m_CamID;
	DWORD  m_dwID;
	BOOL   m_bMarkForDelete;
	CDWordArray m_dwClientIDs;
};
//////////////////////////////////////////////////////////////////////
inline CSecID CAVEncoder::GetCamID() const
{
	return m_CamID;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CAVEncoder::GetID() const
{
	return m_dwID;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CAVEncoder::IsDelayedDelete() const
{
	return m_bMarkForDelete;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CAVEncoder::IsValid() const
{
	return !m_bMarkForDelete && GetWidth()>0 && GetHeight()>0;
}
//////////////////////////////////////////////////////////////////////
inline CDWordArray& CAVEncoder::GetClientIDs()
{
	return m_dwClientIDs;
}
//////////////////////////////////////////////////////////////////////
typedef CAVEncoder* CAVEncoderPtr;
WK_PTR_ARRAY_CS(CAVEncoderArray,CAVEncoderPtr,CAVEncoders);
//////////////////////////////////////////////////////////////////////
class COutputList;
//////////////////////////////////////////////////////////////////////
class CEncoderThread : public CWK_Thread  
{
public:
	CEncoderThread();
	virtual ~CEncoderThread();

	// overrides
public:
	virtual	BOOL StartThread();
	virtual	BOOL StopThread();
	virtual	BOOL Run(LPVOID lpContext);

	// operations
public:
			void CreateEncoders(COutputList* pOutputs, Resolution res, CompressionType ct);
			void AddPictureRecord(CCamera* pCamera,
								  const CIPCPictureRecord& pict, 
								  const CIPCFields& fields, 
								  WORD wArchiveNr, 
								  DWORD dwClientID,
								  CompressionType ctDestination);
			void AddLowBandwidthClient(DWORD dwClientID, CSecID idCam);
			void RemoveLowBandwidthClient(DWORD dwClientID);
			void RemoveLowBandwidthClient(DWORD dwClientID, CSecID idCam);

			void AddHighBandwidthClient(DWORD dwClientID, CSecID idCam);
			void RemoveHighBandwidthClient(DWORD dwClientID);
			void RemoveHighBandwidthClient(DWORD dwClientID, CSecID idCam);

	// implementation
protected:
			void EmptyDataQueue();
			void EncodeIJL(CEncoderItem& item);
			void EncodeAVEmuzed(CEncoderItem& item);
			void CreateJPEGDecoder();

			CEmuzedMpeg4Encoder* CreateEmuzedEncoder(CEmuzedMpeg4Encoder* pEmuzedEncoder,
													 CSecID idArchive, CSecID idCam, 
													 int iWidth, int iHeight,
													 Compression comp,
													 int iBitrate,
													 int iFramerate);
			CAVEncoder* CreateAVEncoder(CAVEncoder* pAVEncoder,
										CSecID idArchive, CSecID idCam, 
										int iWidth, int iHeight,
										CompressionType ct,
										Compression comp,
										int iBitrate,
										int iFramerate);

private:
	CJpeg*					m_pJpeg;
	BYTE*					m_pJpegData;
	DWORD					m_dwJpegLen;

	CEncoderItemQueue		m_Queue;

	CEmuzedMpeg4Encoders	m_EmuzedEncoders;
	CAVEncoders				m_AVEncoders;

	CWK_PerfMon				m_PerfMon;
	BOOL					m_bUseAVCodec;
};

#endif // !defined(AFX_ENCODERTHREAD_H__D240D09E_ACC1_496B_B7B3_AB5DB434C069__INCLUDED_)
