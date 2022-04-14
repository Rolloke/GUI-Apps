// WndLive.h : header file
//
#ifndef _WndLive_H
#define _WndLive_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WndImage.h"
class CDlgColorSettings;
class CDlgPTZVideo;
class CDlgPTZConfig;


#define WPARAM_CONTRAST			0x01
#define WPARAM_SATURATION		0x02
#define WPARAM_BRIGHTNESS		0x03
#define WPARAM_CCT				0x04
#define WPARAM_CCF				0x05
#define WPARAM_LCLICK			0x06
#define WPARAM_CCF_EX			0x07
#define WPARAM_EVENT_DO_REQUEST 0x08
#define WPARAM_CCFKT_NAME		0x09
#define WPARAM_UPDATE_TITLE		0x0a

#define RANGE	15

#define RQ_DEFAULT		-1
#define RQ_FORCE		-2
#define RQ_FORCE_ONE	-3

class CDlgMDConfig;

/////////////////////////////////////////////////////////////////////////////
// WndLive window
class CWndLive : public CWndImage
{
	friend class CPictureBufferThread;
	DECLARE_DYNAMIC(CWndLive)
// Construction
public:
	CWndLive(CServer* pServer, const CIPCOutputRecord &or);

// Attributes
public:
	virtual		 CSecID		GetID();
	virtual	BOOL CanPrint();

	virtual	BOOL IsRequesting();
	virtual	BOOL IsWaitingForRequest();
	
	virtual	BOOL IsStreaming();
	virtual	int  GetRequestedFPS();

	inline		 CSecID		GetCamID();
	inline const CString	GetName() const;
	inline		 Resolution	GetResolution() const;
	inline		 int		GetQuality() const;
	inline		 BOOL		IsSn() const;
	inline		 BOOL		IsYUV() const;
	inline		 BOOL		IsStoring() const;
	inline		 BOOL		WasSometimesActive() const;
	inline		 CSecID		GetArchiveID();
	inline		 DWORD		GetPTZSpeed();
	inline CameraControlType GetCameraControlType();
	inline		DWORD		GetPTZFunctions();
	inline		DWORD		GetPTZFunctionsEx();

// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, CViewIdipClient* pParentWnd);

	void SetOutputRecord(const CIPCOutputRecord &or);
	inline	void	SetPTZSpeed(DWORD dwPTZSpeed);

	// picture data
	virtual BOOL PictureData(const CIPCPictureRecord &pict, 
							 CSecID idArchive,
							 DWORD dwX,
							 DWORD dwY);
			void RequestSetContrast(int contrast);
			void RequestSetBrightness(int brightness);
			void RequestSetColor(int color);
			void OnToggleResolution();
			void PopupMenu(CPoint pt);

			void ActivateCamera();
			void DeactivateCamera();

	virtual	void ClearWasSomeTimesActive();
			void SetStore(BOOL bOn);
	virtual void Set1to1(BOOL b1to1) PURE_VIRTUAL(;)
	virtual void SetMDValues(WORD wX, WORD wY);
			BOOL CheckVideo(DWORD dwCurrentTime);
	virtual	void OnIndicateError(DWORD dwCmd,CIPCError error,int iErrorCode);
			
			BOOL SetActiveRequested(BOOL bActive=TRUE);

			void SetMode(const CString& sMode);
			void ConfirmGetMask(const CIPCActivityMask& mask);
			void ConfirmSetMask(MaskType type);
	BOOL operator < (CWndLive & second);

	// printing
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	int  PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim = FALSE);
	void PrintInfoRectPortrat(CDC* pDC,CRect rect, 
							  CFont* pFont, int iSpace,
							  const CString& sPicture,
							  const CString& sData,
							  const CString& sComment);
	int PrintInfoRectLandscape(CDC* pDC,CRect rect, CFont* pFont);
	void PrintTitleRectPortrat(CDC* pDC,CRect rect, CFont* pFont);
	virtual BOOL CopyPictureForPrinting();
	void DeletePictureForPrinting();
	virtual CH263* GetH263();
	CString FormatPicture(CIPCPictureRecord* pPictureRecord, BOOL bTabbed = FALSE);
	CString FormatData(CIPCFields &fields, const CString& sD,const CString& sS);
	void	ConfirmMaskSensitivity(int nMask, int nSensitivity);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndLive)
	//}}AFX_VIRTUAL

	// Overrides
public:
	virtual	BOOL	IsWndLive();
	virtual UINT	GetToolBarID();
	virtual void	SetCIPCOutputWindow();
	virtual CSystemTime	GetTimeStamp();
#ifndef _DTS
	virtual BOOL	DoRequestVideoOut();
#endif

protected:
	virtual CString	GetTitleText(CDC* pDC);

	BOOL	DoRequestPictures(int iMax=RQ_DEFAULT);

// Implementation
public:
	virtual ~CWndLive();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndLive)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVideoFein();
	afx_msg void OnVideoGrob();
	afx_msg void OnUpdateVideoFein(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoGrob(CCmdUI* pCmdUI);
	afx_msg void OnVideoSave();
	afx_msg void OnUpdateVideoSave(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVideoCsb();
	afx_msg void OnUpdateVideoCsb(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnVideoSn();
	afx_msg void OnUpdateVideoSn(CCmdUI* pCmdUI);
	afx_msg void OnVideoPTZ_X(UINT);
	afx_msg void OnUpdateVideoPTZ_X(CCmdUI* pCmdUI);
	afx_msg void OnVideoStop();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVideoSnConfig();
	afx_msg void OnUpdateViewOriginal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSaveReference(CCmdUI* pCmdUI);
	afx_msg void OnVideoShowReference();
	afx_msg void OnUpdateVideoShowReference(CCmdUI* pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnSmallOriginal();
	afx_msg void OnUpdateSmallOriginal(CCmdUI* pCmdUI);
	afx_msg void OnVideoMask();
	afx_msg void OnUpdateVideoMask(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateSmallClose(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSmallContext(CCmdUI* pCmdUI);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditComment();
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateEditComment(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);
	void DrawFace(CDC* pDC);
	void DrawError(CDC* pDC);
	void StorePicture(const CIPCPictureRecord &pict);
	void OnCSBChanged();
	void RequestCSBUpdate();
	void InitialRequest();
	void RequestPTZ(CameraControlCmd ccc, DWORD dwValue, DWORD dwFlags, BOOL bOn = FALSE);
public:
	void ResetLastCameraControlCmd();
	void SetLastCameraControlCmd(CameraControlCmd cccC, CameraControlCmd cccStop, DWORD dwValue);
	BOOL CheckRemotePictureTime(CSystemTime &stLocal);
protected:
	void ZoomWndLive(const CPoint &point);
	BOOL ScrollWndLive();
	BOOL CanMultipleRequests();

	CString GetLocalReferenceFileName();
	CString GetRemoteReferenceFileName();
	
	// data member
protected:

	CIPCOutputRecord	m_orOutput;
	CameraControlType	m_ccType;
	DWORD				m_dwPTZFunctions;
	DWORD				m_dwPTZFunctionsEx;
	Resolution			m_Resolution;
	BOOL				m_bStoring;
//	int					m_iX;
//	int					m_iY;
	CSecID				m_idArchive;

	BOOL				m_bWasSomeTimesActive;
	BOOL				m_bIsRequesting;
	BOOL				m_bWaitingForRequest;
	
	int					m_iWaitingForPicture;
	int					m_iWaitingForPictureTreshold;

	int					m_iPictureRequests;
	DWORD				m_dwLastRequest;
	BOOL				m_bIsDecoding;
	
	BOOL				m_bIPicture;
	CameraControlCmd	m_LastCameraControlCmd;
	CameraControlCmd	m_LastCameraControlCmdStop;
	DWORD				m_dwLastPictureConfirmedTime;

	// camera parameters
	BOOL	m_bMarkedForShowColorDialog;
	int		m_iContrast;
	int		m_iBrightness;
	int		m_iSaturation;
	int		m_iQuality;
	CSecID  m_hostID;
	WORD	m_wMD_X;
	WORD	m_wMD_Y;
	BOOL	m_iCountZooms; //3 zooms alowed, then back to normal view
	CString m_sMode;
	DWORD	m_dwPTZSpeed;

	// child dialogs
	CDlgColorSettings*	m_pDlgColorSettings;
	CDlgPTZVideo*		m_pDlgPTZVideo;
	CDlgPTZConfig*		m_pDlgPTZConfig;
	CDlgMDConfig*		m_pDlgMDConfig;


	// buffers for software decoding
	BOOL				m_bTimeValid;

	CCriticalSection	m_csTracker;

	//for printing copy the Live picture record
	CIPCPictureRecord* m_pPrintPictureRecord;
	CString	m_sComment;

private:

	friend class CViewIdipClient;
	friend class CDlgColorSettings;
	friend class CDlgPTZConfig;
	friend class CDlgPTZVideo;
	friend class CDlgMDConfig;
public:
};
/////////////////////////////////////////////////////////////////////////////
inline CSecID CWndLive::GetCamID()
{
	return m_orOutput.GetID();
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CWndLive::GetName() const
{
	return m_orOutput.GetName();
}
/////////////////////////////////////////////////////////////////////////////
inline Resolution CWndLive::GetResolution() const 
{
	return m_Resolution;
}
/////////////////////////////////////////////////////////////////////////////
inline int CWndLive::GetQuality() const
{
	return m_iQuality;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CWndLive::IsSn() const
{
	return !m_orOutput.CameraIsFixed();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CWndLive::IsYUV() const
{
	return m_orOutput.CameraIsYUV();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CWndLive::IsStoring() const
{
	return m_bStoring;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CWndLive::WasSometimesActive() const
{
	return m_bWasSomeTimesActive;
}
 /////////////////////////////////////////////////////////////////////////////
inline CSecID CWndLive::GetArchiveID()
{
	CSecID id = SECID_NO_ID;
	m_csPictureRecord.Lock();
	id = m_idArchive;
	m_csPictureRecord.Unlock();
	return id;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CWndLive::GetPTZSpeed()
{
	return m_dwPTZSpeed;
}
/////////////////////////////////////////////////////////////////////////////
inline void CWndLive::SetPTZSpeed(DWORD dwPTZSpeed)
{
	m_dwPTZSpeed = dwPTZSpeed;
}
/////////////////////////////////////////////////////////////////////////////
inline CameraControlType CWndLive::GetCameraControlType()
{
	return m_ccType;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CWndLive::GetPTZFunctions()
{
	return m_dwPTZFunctions;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CWndLive::GetPTZFunctionsEx()
{
	return m_dwPTZFunctionsEx;
}
////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _WndLive_H
