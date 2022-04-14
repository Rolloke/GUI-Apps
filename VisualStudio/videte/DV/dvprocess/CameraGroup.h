// CameraGroup.h: interface for the CCameraGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAGROUP_H__DB8EAFA2_8C75_11D3_A870_004005A19028__INCLUDED_)
#define AFX_CAMERAGROUP_H__DB8EAFA2_8C75_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OutputGroup.h"
#include "RequestThread.h"
#include "Request.h"
#include "Camera.h"

class CCameraGroup : public COutputGroup  
{
	// construction / destruction
public:
	CCameraGroup(LPCTSTR shmName, int wGroup, int size);
	virtual ~CCameraGroup();

	// attributes
public:
	virtual BOOL HasCameras() const;
	virtual int  GetNrOfActiveCameras();
	inline	BOOL IsScanning() const;
			BOOL IsImageTimeout();
			BOOL IsRequestThreadRunning();

	// operations
public:
	virtual void ScanCameras();
	virtual void StartRequests();
	virtual void StopRequests();
	virtual void SaveReferenceImage();
			void RescanCameras();
			void Request();
			void ClearActiveCam(WORD wSubID);
			void SetActiveCam(WORD wSubID);
			void SetClientActiveCam(CSecID id);
			HANDLE ScanEvent();
			DWORD GetAverageSwitchtime();
			void StartRequest(const CCamera* pCamera, BOOL bIsActive);
			void OnResolutionChanged(Resolution resolution);

	static	DWORD GetIFrameDefault(int iFPS);

	// overrides
public:
	virtual void OnConfirmReset(WORD wGroupID);
	virtual	void OnRequestDisconnect();
	virtual void OnConfirmGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
									   DWORD dwUserData, 
									   CSecID idArchive);
	virtual void OnIndicateLocalVideoData(CSecID camID, 
										  WORD wXSize, 
										  WORD wYSize, 
										  const CIPCPictureRecord &pict);
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData, 
								 CSecID idArchive);
	virtual	void OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	virtual	void OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
		CIPCError error, int iErrorCode,
		const CString &sErrorMessage);
	// implementation
protected:
	void RequestImage(int iCount, WORD id, WORD next, DWORD dwID);
	void RequestScanCameras();
	void ScanResult(DWORD dwMask);

	void RequestSync();

	int  DoRequestPictures(CRequest& r);
	void DoSync();
	void DoWaitSync(int iNumPicts);

	void StartTashaRequests();
	void StartQRequests();
	void StopTashaRequests();
	void StopQRequests();
	void StopRequest(const CCamera* pCamera);
	void StartTashaRequest(const CCamera* pCamera, BOOL bIsActive);
	void StartQRequest(const CCamera* pCamera, BOOL bIsActive);

private:
	// request stuff
	HANDLE	m_hSyncSemaphore;
	CRequestThread m_RequestThread;
	volatile LONG 	m_lOutStandingSync;
	DWORD	m_tickSyncSend;
	DWORD	m_dwRequestID;
	WORD	m_wFirstRequest;
	CDWordArray m_dwSwitchTimes;
	int			m_iCurrentSwitchTimeIndex;
	CSecID  m_idClientActiveCam;

	// scanning stuff
	DWORD   m_dwStartScanTick;
	BOOL	m_bScanning; 
	BOOL	m_bMustScan; 
	DWORD	m_dwScanMask;	
	CEvent	m_evScanResult;
	int		m_iNrActiveCameras;
	DWORD	m_dwLastPictureConfirmed;
};
/////////////////////////////////////////////////////////////////
inline BOOL CCameraGroup::IsScanning() const
{
	return m_bScanning;
}

#endif // !defined(AFX_CAMERAGROUP_H__DB8EAFA2_8C75_11D3_A870_004005A19028__INCLUDED_)
