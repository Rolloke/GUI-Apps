// IPCOutputDVClient.h: interface for the CIPCOutputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCOUTPUTDVCLIENT_H__43C9E49F_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_IPCOUTPUTDVCLIENT_H__43C9E49F_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COutputClient;

#include "TranscoderThread.h"
#include "EncoderThread.h"
#include "GOPSendThread.h"

class CIPCOutputDVClient : public CIPCOutput  
{
	// construction/destruction
public:
	CIPCOutputDVClient(COutputClient* pClient, LPCTSTR shmName);
	virtual ~CIPCOutputDVClient();

	// attributes
public:
	BOOL IsTimedOut();
	inline COutputClient* GetClient();

	// overrides
public:
	virtual void OnRequestInfoOutputs(WORD wGroupID);
	virtual	void OnRequestNewJpegEncoding(CSecID camID, 
										  Resolution res, 
										  Compression comp,
										  int iNumPictures,
										  DWORD	dwUserID);
	virtual	void OnRequestStartVideo(CSecID id,			// camera nr.
									Resolution res,		// resolution
									Compression comp,		// image size or bitrate
									CompressionType ct,	// JPEG, YUV, MPEG4 ...
									int iFPS,
									int iIFrameInterval, 
									DWORD dwUserData		// unique server id
									);
	virtual void OnRequestStopVideo(CSecID camID,
									DWORD dwUserData);
	virtual void OnRequestStartH263Encoding(CSecID camID, 
											Resolution res, 
											Compression comp,
											DWORD dwBitrate,
											DWORD dwRecordTime);
	virtual void OnRequestStopH263Encoding(CSecID camID);
	virtual void OnRequestSync(DWORD dwTickSend, DWORD dwType,DWORD dwUserID);
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);
	// values:
	virtual void OnRequestGetValue(CSecID id,const CString &sKey,DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,const CString &sKey,const CString &sValue,DWORD dwServerData);
	// disconnection
	virtual void	OnRequestDisconnect();

	virtual void OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);

	// implementation
public:
	void ConfirmPictureSlow(CCamera* pCamera,const CIPCPictureRecord& pict, DWORD dwX, DWORD dwY);
	void SendPicture(CCamera* pCamera, const CIPCPictureRecord& pict, DWORD dwX, DWORD dwY, BOOL bFromEncoderThread);
//	void SendPicture(const CIPCPictureRecord& pict, CSecID id, CSecID idArchive);
	void PictureSent(const CIPCPictureRecord& pict, int x);

	void ResetClientCounter();
	void CheckNagScreen(); //checks if maintenance and notifable RTEs should be shown on screen

	// implementation
protected:
	void CreateTranscoder(int iNumPictures,DWORD dwCalculatedBitrate);
	void DestroyTranscoder();
	CString CheckMaintenance();
	CString CheckNotifiableErrors();

	//get correct maintenance language
	CString GetMaintenanceText();
	CString GetPathCurrentLanguageDll(); 
	void LoadLanguageDll();
	BOOL UnloadLanguageDll();

private:
	COutputClient* m_pClient;

	CCriticalSection m_csConfirms;
	int				 m_iRequest[MAX_CAM];
	int				 m_iFPS[MAX_CAM];
	WORD			 m_wBlockNr[MAX_CAM];
	CSystemTime		 m_stConfirm[MAX_CAM];

	int	m_iFastConfirms;
	int	m_iSlowConfirms;

	CTranscoderThread* m_pTranscoder;
	CCriticalSection   m_csTranscoder;
	DWORD			   m_dwTime;
	CSecID			   m_idActiceVOUTCam;
	CEncoderThread	   m_Encoder;
	CGOPSendThread	   m_GOPSender;
	HINSTANCE		   m_hLanguageDll; //to get current dll for correct maintenance text
};
inline COutputClient* CIPCOutputDVClient::GetClient()
{
	return m_pClient;
}

#endif // !defined(AFX_IPCOUTPUTDVCLIENT_H__43C9E49F_8B84_11D3_99EB_004005A19028__INCLUDED_)
