#pragma once

#include "DirectShowThread.h"

#define CAMS_UNKNOWN			0xffffffff
#define CAMS_NO_VIDEO			FALSE
#define CAMS_VIDEO_OK			TRUE

#define CAMTF_MOTION_DETECTION	DSTF_USER_MIN

class CIPCOutputIPcamUnit;
/////////////////////////////////////////////////////////////////////////////
// CDsCamera
class CDsCamera : public CDirectShowThread
{
	DECLARE_DYNAMIC(CDsCamera)

protected:
	CDsCamera(const CString&sName, DWORD dwFlags);           // protected constructor used by dynamic creation
	virtual ~CDsCamera();

	// Attribute
public:
	inline WORD				GetCameraSubID() const;
	inline Resolution		GetResolution() const;
	inline Compression		GetCompression() const;
	inline CompressionType	GetCompressionType() const;
	inline int				GetFPS() const;
	inline int				GetIFrameInterval() const;
	inline DWORD			GetUserData() const;
	BOOL					IsThreadRunning() const;
	
	// Implementation
public:
	inline void	SetCameraSubID(WORD wID);
	inline void	SetUserData(DWORD wUserData);
	void	SetResolution(Resolution);
	void	SetCompression(Compression);
	void	SetCompressionType(CompressionType);
	void	SetFPS(int);
	void	SetIFrameInterval(int);
	void	SetRotation(const CString&sRot);
	void	DrawPicture(const BYTE*pData, DWORD dwLenght, CompressionType ct);
	void	DrawPicture(CIPCPictureRecord*pPict);

protected:
	HRESULT InitDumpFilterReceiveFnc(AM_MEDIA_TYPE*pmt=NULL, int nCount=0, BOOL bEnum=FALSE);
private:
	static HRESULT WINAPI ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam);
	
	//{{AFX_VIRTUAL()
public:
	virtual int Run();
	virtual CString GetLocation();
	virtual CString GetType();
	virtual BOOL OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData);
	virtual BOOL OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	virtual DWORD GetVideoStatus();
protected:
	virtual HRESULT ReceiveMediaSample(IMediaSample *pSample, long nPackageNo);
	virtual void ReleaseFilters();
	//}}AFX_VIRTUAL

public:

protected:
	//{{AFX_MSG()
	afx_msg void OnRequestGetValue(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRequestSetValue(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	//{{AFX_DATA()
	IBaseFilter *m_pDecoder;
	IBaseFilter *m_pDump;
	Resolution	m_Resolution; 
	Compression m_Compression;
	CompressionType m_CompressionType;
	int			m_iFPS;
	int			m_iIFrameInterval;
	CString		m_sRotation;
	WORD		m_wPackageNr;
private:
	WORD		m_wCamSubID;
	DWORD		m_dwUserData;
	CJpeg		m_JpgDecoder;
	//}}AFX_DATA

};

/////////////////////////////////////////////////////////////////////////////
inline void	CDsCamera::SetCameraSubID(WORD wID)
{
	m_wCamSubID = wID;
}
inline void	CDsCamera::SetUserData(DWORD dwUserData)
{
	m_dwUserData = dwUserData;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD	CDsCamera::GetCameraSubID() const
{
	return m_wCamSubID;
}
/////////////////////////////////////////////////////////////////////////////
inline Resolution CDsCamera::GetResolution() const
{
	return m_Resolution;
}
/////////////////////////////////////////////////////////////////////////////
inline Compression CDsCamera::GetCompression() const
{
	return m_Compression;
}
/////////////////////////////////////////////////////////////////////////////
inline CompressionType CDsCamera::GetCompressionType() const
{
	return m_CompressionType;
}
/////////////////////////////////////////////////////////////////////////////
inline int CDsCamera::GetFPS() const
{
	return m_iFPS;
}
/////////////////////////////////////////////////////////////////////////////
inline int CDsCamera::GetIFrameInterval() const
{
	return m_iIFrameInterval;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CDsCamera::GetUserData() const
{
	return m_dwUserData;
}
/////////////////////////////////////////////////////////////////////////////
