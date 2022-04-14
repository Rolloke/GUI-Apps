#pragma once

#include "DSCamera.h"

class CIPCPictureRecord;
class CIpCamInetSession;
class CIpCamRTCEvents;

// CDsIpCamera
LPCTSTR NameOfHTTPStatus(DWORD dwStatus);

#define CONTEXTID_STREAM	100+GetCameraSubID()

class CDsIpCamera : public CDsCamera
{
	DECLARE_DYNAMIC(CDsIpCamera)

protected:
	CDsIpCamera(const CString&sName, DWORD dwFlags);           // protected constructor used by dynamic creation
	virtual ~CDsIpCamera();

	// Attribute
public:
	const  CString &GetURL() const { return m_sURL; };
	int	   GetPort() const { return m_nPort; };
	CInternetSession*GetSession();
	CHttpConnection *GetHTTPConnection();

protected:

	// Implementation
public:
	void SetURLandPort(const CString &sUrl, int nPort);
	void SetProxy(CString&sProxyName, CString&sProxyBypass);
	void CloseSession();

protected:
	CString HTTPRequest(const CString &sRequest, DWORD &dwResult, BOOL bPost=FALSE, DWORD dwContext=1);
	void	CloseHTTPServer();
	void	CloseStream();
	UINT	ReadStream(void*pBuff, UINT nCount);
	void	ReadJPG();
	HRESULT	StartReceiveTimer(BOOL bStart=TRUE);

public:
	//{{AFX_VIRTUAL(CDsIpCamera)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual HRESULT InitFilters();
	virtual CString GetLocation();
	virtual CString GetType();
	virtual HRESULT OnEvent(LONG lCode, LONG lParam1, LONG lParam2);
	virtual HRESULT OnRTCEvent(RTC_EVENT enEvent, IDispatch *pDisp);
	virtual void OnInternetEvent(DWORD dwContext, DWORD dwInternetStatus,
		void* lpvStatusInformation, DWORD dwStatusInformationLength);
	virtual void ReleaseFilters();
//	virtual DWORD GetVideoStatus();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDsIpCamera)
	afx_msg void OnTimer(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(CDsIpCamera)
private:
	CString				m_sProxyName;
	CString				m_sProxyBypass;
	CIpCamInetSession*	m_pInternetSession;
	CHttpConnection*	m_pHttpServer;

protected:
	CString				m_sURL;
	int					m_nPort;
	CIPCPictureRecord*	m_pPiture;
	long				m_lCurrentPosition;
	CStringA			m_sLengthID;
	CStringA			m_sStartID;
	CHttpFile*			m_pStream;
	IRTCClient*			m_pClient;
	CIpCamRTCEvents*	m_pEvents;
	IRTCSession*		m_pRTCSession;
	//}}AFX_DATA
};


