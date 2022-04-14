#pragma once

class CUploadProcess;

class CUploadItem
{
public:
	CUploadItem(const CIPCPictureRecord& pict,
				const CString& sServername,
				const CString& sLogin,
				const CString& sPassword,
				const CString& sCameraname,
				const CString& sFilename);
	virtual ~CUploadItem();

protected:

private:
	CIPCPictureRecord* m_pPicture;
	CString m_sServername;
	CString m_sLogin;
	CString m_sPassword;
	CString m_sCameraname;
	CString m_sFilename;

	friend class CUploadThread;
};
//////////////////////////////////////////////////////////////////////
typedef CUploadItem* CUploadItemPtr;
WK_PTR_ARRAY_CS(CUploadItemArray,CUploadItemPtr,CUploadItems);
//////////////////////////////////////////////////////////////////////
class CUploadThread : public CWK_Thread
{
public:
	CUploadThread();
	virtual ~CUploadThread();

	// operations
public:
	void AddPicture(const CIPCPictureRecord& pict,CUploadProcess* pProcess);
	virtual	BOOL Run(LPVOID lpContext);

	// implementation
	BOOL Upload(const CUploadItem* pItem);

private:
	CUploadItems	m_Queue;
	CJpeg			m_JpegDecoder;
	CJpeg			m_JpegEncoder;
	CMPEG4Decoder	m_Mpeg4Decoder;

public:
	static	BOOL m_bTraceFTP;
};
